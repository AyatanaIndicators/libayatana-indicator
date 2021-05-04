/*
An object used to manage services.  Either start them or
just connect to them.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 3.0 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License version 3.0 for more details.

You should have received a copy of the GNU General Public
License along with this library. If not, see
<http://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <gio/gio.h>

#include "indicator-service-manager.h"
#include "gen-indicator-service.xml.h"
#include "dbus-shared.h"

/* Private Stuff */
/**
	IndicatorServiceManagerPrivate:
	@name: The well known dbus name the service should be on.
	@service_proxy: The proxy to the service itself.
	@connected: Whether we're connected to the service or not.
	@this_service_version: The version of the service that we're looking for.
	@restart_count: The number of times we've restarted this service.
*/
typedef struct _IndicatorServiceManagerPrivate IndicatorServiceManagerPrivate;
struct _IndicatorServiceManagerPrivate {
	gchar * name;
	GDBusProxy * service_proxy;
	GCancellable * service_proxy_cancel;
	guint name_watcher;
	gboolean connected;
	guint this_service_version;
	guint restart_count;
	gint restart_source;
	GCancellable * watch_cancel;
};

/* Signals Stuff */
enum {
	CONNECTION_CHANGE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

/* If this env variable is set, we don't restart */
#define TIMEOUT_ENV_NAME   "INDICATOR_SERVICE_RESTART_DISABLE"
#define TIMEOUT_MULTIPLIER 100 /* In ms */
/* What to reset the restart_count to if we know that we're
   in a recoverable error condition, but waiting a little bit
   will probably make things better.  5 ~= 3 sec. */
#define TIMEOUT_A_LITTLE_WHILE  5

/* Properties */
/* Enum for the properties so that they can be quickly
   found and looked up. */
enum {
	PROP_0,
	PROP_NAME,
	PROP_VERSION
};

/* The strings so that they can be slowly looked up. */
#define PROP_NAME_S                    "name"
#define PROP_VERSION_S                 "version"

/* GDBus Stuff */
static GDBusNodeInfo *            node_info = NULL;
static GDBusInterfaceInfo *       interface_info = NULL;

static void indicator_service_manager_class_init (IndicatorServiceManagerClass *klass);
static void indicator_service_manager_init       (IndicatorServiceManager *self);
static void indicator_service_manager_dispose    (GObject *object);
static void indicator_service_manager_finalize   (GObject *object);

/* Prototypes */
static void set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void start_service (IndicatorServiceManager * service);
static void start_service_again (IndicatorServiceManager * manager);
static void unwatch (GDBusProxy * proxy);
static void service_proxy_cb (GObject * object, GAsyncResult * res, gpointer user_data);
static void service_proxy_name_changed (GDBusConnection * connection, const gchar * sender_name, const gchar * object_path, const gchar * interface_name, const gchar * signal_name, GVariant * parameters, gpointer user_data);

G_DEFINE_TYPE_WITH_PRIVATE (IndicatorServiceManager, indicator_service_manager, G_TYPE_OBJECT);

/* Build all of our signals and proxies and tie everything
   all together.  Lovely. */
static void
indicator_service_manager_class_init (IndicatorServiceManagerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = indicator_service_manager_dispose;
	object_class->finalize = indicator_service_manager_finalize;

	/* Property funcs */
	object_class->set_property = set_property;
	object_class->get_property = get_property;

	/**
		IndicatorServiceManager::connecton-change:
		@arg0: The #IndicatorServiceManager object
		@arg1: The state of the connection, TRUE is connected.
		
		Signaled when the service is connected or disconnected
		depending on it's previous state.
	*/
	signals[CONNECTION_CHANGE] = g_signal_new (INDICATOR_SERVICE_MANAGER_SIGNAL_CONNECTION_CHANGE,
	                                  G_TYPE_FROM_CLASS(klass),
	                                  G_SIGNAL_RUN_LAST,
	                                  G_STRUCT_OFFSET (IndicatorServiceManagerClass, connection_change),
	                                  NULL, NULL,
	                                  g_cclosure_marshal_VOID__BOOLEAN,
	                                  G_TYPE_NONE, 1, G_TYPE_BOOLEAN, G_TYPE_NONE);

	/* Properties */
	g_object_class_install_property(object_class, PROP_NAME,
	                                g_param_spec_string(PROP_NAME_S,
	                                                    "The DBus name for the service to monitor",
	                                                    "This is the name that should be used to start a service.",
	                                                    NULL,
	                                                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property(object_class, PROP_VERSION,
	                                g_param_spec_uint(PROP_VERSION_S,
	                                                  "The version of the service that we're expecting.",
	                                                  "A number to check and reject a service if it gives us the wrong number.  This should match across the manager and the service",
	                                                  0, G_MAXUINT, 0,
	                                                  G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/* Setting up the DBus interfaces */
	if (node_info == NULL) {
		GError * error = NULL;

		node_info = g_dbus_node_info_new_for_xml(_indicator_service, &error);
		if (error != NULL) {
			g_error("Unable to parse Indicator Service Interface description: %s", error->message);
			g_error_free(error);
		}
	}

	if (interface_info == NULL) {
		interface_info = g_dbus_node_info_lookup_interface(node_info, INDICATOR_SERVICE_INTERFACE);

		if (interface_info == NULL) {
			g_error("Unable to find interface '" INDICATOR_SERVICE_INTERFACE "'");
		}
	}

	return;
}

/* This inits all the variable and sets up the proxy
   to dbus.  It doesn't look for the service as at this
   point we don't know it's name. */
static void
indicator_service_manager_init (IndicatorServiceManager *self)
{
	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(self);

	/* Get the private variables in a decent state */
	priv->name = NULL;
	priv->service_proxy = NULL;
	priv->service_proxy_cancel = NULL;
	priv->name_watcher = 0;
	priv->connected = FALSE;
	priv->this_service_version = 0;
	priv->restart_count = 0;
	priv->restart_source = 0;
	priv->watch_cancel = NULL;

	return;
}

/* If we're connected this provides all the signals to say
   that we're about to not be.  Then it takes down the proxies
   and tells the service that we're not interested in being
   its friend anymore either. */
static void
indicator_service_manager_dispose (GObject *object)
{
	IndicatorServiceManager * sm = INDICATOR_SERVICE_MANAGER(object);
	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(sm);

	/* Removing the idle task to restart if it exists. */
	if (priv->restart_source != 0) {
		g_source_remove(priv->restart_source);
	}
	/* Block any restart calls */
	priv->restart_source = -1;

	/* If we were connected we need to make sure to
	   tell people that it's no longer the case. */
	if (priv->connected) {
		priv->connected = FALSE;
		g_signal_emit(object, signals[CONNECTION_CHANGE], 0, FALSE, TRUE);
	}

	if (priv->name_watcher != 0) {
		g_dbus_connection_signal_unsubscribe(g_dbus_proxy_get_connection(priv->service_proxy),
		                                     priv->name_watcher);
		priv->name_watcher = 0;
	}

	/* If we're still getting the proxy, stop looking so we
	   can then clean up some more. */
	if (priv->service_proxy_cancel != NULL) {
		g_cancellable_cancel(priv->service_proxy_cancel);
		g_object_unref(priv->service_proxy_cancel);
		priv->service_proxy_cancel = NULL;
	}

	/* If we've sent a watch, cancel looking for the reply before
	   sending the unwatch */
	if (priv->watch_cancel != NULL) {
		g_cancellable_cancel(priv->watch_cancel);
		g_object_unref(priv->watch_cancel);
		priv->watch_cancel = NULL;
	}

	/* If we have a proxy, tell it we're shutting down.  Just
	   to be polite about it. */
	if (priv->service_proxy != NULL) {
		unwatch(priv->service_proxy);
	}

	/* Destory our service proxy, we won't need it. */
	if (priv->service_proxy != NULL) {
		g_object_unref(G_OBJECT(priv->service_proxy));
		priv->service_proxy = NULL;
	}

	/* Let's see if our parents want to do anything. */
	G_OBJECT_CLASS (indicator_service_manager_parent_class)->dispose (object);
	return;
}

/* Ironically, we don't allocate a lot of memory ourselves. */
static void
indicator_service_manager_finalize (GObject *object)
{
	IndicatorServiceManager * sm = INDICATOR_SERVICE_MANAGER(object);
	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(sm);

	if (priv->name != NULL) {
		g_free(priv->name);
		priv->name = NULL;
	}

	G_OBJECT_CLASS (indicator_service_manager_parent_class)->finalize (object);
	return;
}

/* Either copies the name into the private variable or
   sets the version.  Do it wrong and it'll get upset. */
static void
set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
	IndicatorServiceManager * self = INDICATOR_SERVICE_MANAGER(object);
	g_return_if_fail(self != NULL);

	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(self);
	g_return_if_fail(priv != NULL);

	switch (prop_id) {
	/* *********************** */
	case PROP_NAME:
		if (priv->name != NULL) {
			g_error("Name can not be set twice!");
		}
		priv->name = g_value_dup_string(value);
		start_service(self);
		break;
	/* *********************** */
	case PROP_VERSION:
		priv->this_service_version = g_value_get_uint(value);
		break;
	/* *********************** */
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}

	return;
}

/* Grabs the values from the private variables and
   puts them into the value. */
static void
get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
	IndicatorServiceManager * self = INDICATOR_SERVICE_MANAGER(object);
	g_return_if_fail(self != NULL);

	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(self);
	g_return_if_fail(priv != NULL);

	switch (prop_id) {
	/* *********************** */
	case PROP_NAME:
		g_value_set_string(value, priv->name);
		break;
	/* *********************** */
	case PROP_VERSION:
		g_value_set_uint(value, priv->this_service_version);
		break;
	/* *********************** */
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}

	return;
}

/* Small little function to make a long function call a little
   bit cleaner. */
static void
unwatch (GDBusProxy * proxy)
{
	g_dbus_proxy_call(proxy,
	                  "UnWatch",
	                  NULL,   /* parameters */
	                  G_DBUS_CALL_FLAGS_NONE,
	                  -1,     /* timeout */
	                  NULL,   /* cancelable */
	                  NULL,   /* callback */
	                  NULL);  /* user data */
	return;
}

/* A callback from telling a service that we want to watch
   it.  It gives us the service API version and the version
   of the other APIs it supports.  We check both of those.
   If they don't match then we unwatch it.  Otherwise, we
   signal a connection change to tell the rest of the world
   that we have a service now.  */
static void
watch_cb (GObject * object, GAsyncResult * res, gpointer user_data)
{
	GError * error = NULL;
	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(user_data);

	GVariant * params = g_dbus_proxy_call_finish(G_DBUS_PROXY(object), res, &error);

	if (error != NULL) {
		g_warning("Unable to set watch on '%s': '%s'", priv->name, error->message);
		g_error_free(error);
		start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
		return;
	}

	guint service_api_version;
	guint this_service_version;

	g_variant_get(params, "(uu)", &service_api_version, &this_service_version);
	g_variant_unref(params);

	/* We've done it, now let's stop counting. */
	/* Note: we're not checking versions.  Because, the hope is that
	   the guy holding the name we want with the wrong version will
	   drop and we can start another service quickly. */
	priv->restart_count = 0;

	if (service_api_version != INDICATOR_SERVICE_VERSION) {
		g_warning("Service is using a different version of the service interface.  Expecting %d and got %d.", INDICATOR_SERVICE_VERSION, service_api_version);
		unwatch(priv->service_proxy);

		/* Let's make us wait a little while, then try again */
		priv->restart_count = TIMEOUT_A_LITTLE_WHILE;
		start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
		return;
	}

	if (this_service_version != priv->this_service_version) {
		g_warning("Service is using a different API version than the manager.  Expecting %d and got %d.", priv->this_service_version, this_service_version);
		unwatch(priv->service_proxy);

		/* Let's make us wait a little while, then try again */
		priv->restart_count = TIMEOUT_A_LITTLE_WHILE;
		start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
		return;
	}

	if (!priv->connected) {
		priv->connected = TRUE;
		g_signal_emit(G_OBJECT(user_data), signals[CONNECTION_CHANGE], 0, TRUE, TRUE);
	}

	return;
}

/* The function that handles getting us connected to the service.
   In many cases it will start the service, but if the service
   is already there it just allocates the service proxy and acts
   like it was no big deal. */
static void
start_service (IndicatorServiceManager * service)
{
	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(service);

	g_return_if_fail(priv->name != NULL);

	if (priv->service_proxy_cancel != NULL) {
		/* A service proxy is being gotten currently */
		return;
	}

	if (priv->service_proxy != NULL) {
		g_object_unref(priv->service_proxy);
		priv->service_proxy = NULL;
	}

	priv->service_proxy_cancel = g_cancellable_new();

	g_dbus_proxy_new_for_bus(G_BUS_TYPE_SESSION,
	                         G_DBUS_PROXY_FLAGS_NONE,
	                         interface_info,
	                         priv->name,
	                         INDICATOR_SERVICE_OBJECT,
	                         INDICATOR_SERVICE_INTERFACE,
	                         priv->service_proxy_cancel,
	                         service_proxy_cb,
	                         service);

	return;
}

/* Callback from trying to create the proxy for the service, this
   could include starting the service.  Sometime it'll fail and
   we'll try to start that dang service again! */
static void
service_proxy_cb (__attribute__((unused)) GObject * object, GAsyncResult * res, gpointer user_data)
{
	GError * error = NULL;

	IndicatorServiceManager * service = INDICATOR_SERVICE_MANAGER(user_data);
	g_return_if_fail(service != NULL);

	GDBusProxy * proxy = g_dbus_proxy_new_for_bus_finish(res, &error);

	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(user_data);

	if (priv->service_proxy_cancel != NULL) {
		g_object_unref(priv->service_proxy_cancel);
		priv->service_proxy_cancel = NULL;
	}

	if (error != NULL) {
		/* Unable to create the proxy, eh, let's try again
		   in a bit */
		g_error_free(error);
		start_service_again(service);
		return;
	}

	gchar * name = g_dbus_proxy_get_name_owner(proxy);
	if (name == NULL) {
		/* Hmm, since creating the proxy should start it, it seems very
		   odd that it wouldn't have an owner at this point.  But, all
		   we can do is try again. */
		g_object_unref(proxy);
		start_service_again(service);
		return;
	}
	g_free(name);

	/* Okay, we're good to grab the proxy at this point, we're
	   sure that it's ours. */
	priv->service_proxy = proxy;

	/* Signal for drop */
	priv->name_watcher = g_dbus_connection_signal_subscribe(
	                                   g_dbus_proxy_get_connection(proxy),
	                                   "org.freedesktop.DBus",
	                                   "org.freedesktop.DBus",
	                                   "NameOwnerChanged",
	                                   "/org/freedesktop/DBus",
	                                   g_dbus_proxy_get_name(proxy),
	                                   G_DBUS_SIGNAL_FLAGS_NONE,
	                                   service_proxy_name_changed,
	                                   user_data,
	                                   NULL);

	/* Build cancelable if we need it */
	if (priv->watch_cancel == NULL) {
		priv->watch_cancel = g_cancellable_new();
	}

	/* Send watch */
	g_dbus_proxy_call(priv->service_proxy,
	                  "Watch",
	                  NULL, /* params */
	                  G_DBUS_CALL_FLAGS_NONE,
	                  -1,
	                  priv->watch_cancel,
	                  watch_cb,
	                  user_data);

	return;
}

/* Responds to the name owner changing of the proxy, this
   usually means the service died.  We're dropping the proxy
   and recreating it so that it'll restart the service. */
static void
service_proxy_name_changed (__attribute__((unused)) GDBusConnection * connection,
                            __attribute__((unused)) const gchar * sender_name,
                            __attribute__((unused)) const gchar * object_path,
                            __attribute__((unused)) const gchar * interface_name,
                            __attribute__((unused)) const gchar * signal_name,
                            GVariant * parameters,
                            gpointer user_data)

{
	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(user_data);

	const gchar * new_name = NULL;
	const gchar * prev_name = NULL;
	g_variant_get(parameters, "(&s&s&s)", NULL, &prev_name, &new_name);

	if (new_name == NULL || new_name[0] == 0) {
		if (priv->connected) {
			priv->connected = FALSE;
			g_signal_emit(G_OBJECT(user_data), signals[CONNECTION_CHANGE], 0, FALSE, TRUE);
		}

		start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
	} else {
		/* If we weren't connected before, we are now.  Let's tell the
		   world! */
		if (!priv->connected) {
			priv->connected = TRUE;
			g_signal_emit(G_OBJECT(user_data), signals[CONNECTION_CHANGE], 0, TRUE, TRUE);
		}

		/* If the names are both valid, and they're not the same, it means that
		   we've actually changed.  So we need to tell the new guy that we're
		   watching them */
		if (new_name != NULL && prev_name != NULL && new_name[0] != 0 && prev_name != 0 && g_strcmp0(prev_name, new_name) != 0) {
			/* Send watch */
			g_dbus_proxy_call(priv->service_proxy,
			                  "Watch",
			                  NULL, /* params */
			                  G_DBUS_CALL_FLAGS_NONE,
			                  -1,
			                  priv->watch_cancel,
			                  watch_cb,
			                  user_data);
		}
	}

	return;
}

/* The callback that starts the service for real after
   the timeout as determined in 'start_service_again'.
   This could be in the idle or a timer. */
static gboolean
start_service_again_cb (gpointer data)
{
	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(data);
	priv->restart_count++;
	g_debug("Restarting service '%s' count %d", priv->name, priv->restart_count);
	start_service(INDICATOR_SERVICE_MANAGER(data));
	priv->restart_source = 0;
	return FALSE;
}

/* This function tries to start a new service, perhaps
   after a timeout that it determines.  The real issue
   here is that it throttles restarting if we're not
   being successful. */
static void
start_service_again (IndicatorServiceManager * manager)
{
	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(manager);

	/* If we've already got a restart source running then
	   let's not do this again. */
	if (priv->restart_source != 0) {
		return;
	}

	/* Allow the restarting to be disabled */
	if (g_getenv(TIMEOUT_ENV_NAME)) {
		return;
	}

	if (priv->restart_count == 0) {
		/* First time, do it in idle */
		g_idle_add(start_service_again_cb, manager);
	} else {
		/* Not our first time 'round the block.  Let's slow this down. */
		if (priv->restart_count > 16)
			priv->restart_count = 16; /* Not more than 1024x */
		priv->restart_source = g_timeout_add((1 << priv->restart_count) * TIMEOUT_MULTIPLIER, start_service_again_cb, manager);
	}

	return;
}

/* API */

/**
	indicator_service_manager_new:
	@dbus_name: The well known name of the service on DBus

	This creates a new service manager object.  If the service
	is not running it will start it.  No matter what, it will
	give a IndicatorServiceManager::connection-changed event
	signal when it gets connected.

	Return value: A brand new lovely #IndicatorServiceManager
		object.
*/
IndicatorServiceManager *
indicator_service_manager_new (const gchar * dbus_name)
{
	GObject * obj = g_object_new(INDICATOR_SERVICE_MANAGER_TYPE,
	                             PROP_NAME_S, dbus_name,
	                             NULL);

	return INDICATOR_SERVICE_MANAGER(obj);
}

/**
	inicator_service_manager_new_version:
	@dbus_name: The well known name of the service on DBus
	@version: Version of the service we expect

	This creates a new service manager object.  It also sets
	the version of the service that we're expecting to see.
	In general, it behaves similarly to #indicator_service_manager_new()
	except that it checks @version against the version returned
	by the service.

	Return value: A brand new lovely #IndicatorServiceManager
		object.
*/
IndicatorServiceManager *
indicator_service_manager_new_version (const gchar * dbus_name, guint version)
{
	GObject * obj = g_object_new(INDICATOR_SERVICE_MANAGER_TYPE,
	                             PROP_NAME_S, dbus_name,
	                             PROP_VERSION_S, version,
	                             NULL);

	return INDICATOR_SERVICE_MANAGER(obj);
}

/**
	indicator_service_manager_connected:
	@sm: #IndicatorServiceManager object to check

	Checks to see if the service manager is connected to a
	service.

	Return value: #TRUE if there is a service connceted.
*/
gboolean
indicator_service_manager_connected (IndicatorServiceManager * sm)
{
	g_return_val_if_fail(INDICATOR_IS_SERVICE_MANAGER(sm), FALSE);
	IndicatorServiceManagerPrivate * priv = indicator_service_manager_get_instance_private(sm);
	return priv->connected;
}

/**
	indicator_service_manager_set_refresh:
	@sm: #IndicatorServiceManager object to configure
	@time_in_ms: The refresh time in milliseconds

	Use this function to set the amount of time between restarting
	services that may crash or shutdown.  This is mostly useful
	for testing and development.

	NOTE: Not yet implemented.
*/
void
indicator_service_manager_set_refresh (__attribute__((unused)) IndicatorServiceManager * sm, __attribute__((unused)) guint time_in_ms)
{

	return;
}
