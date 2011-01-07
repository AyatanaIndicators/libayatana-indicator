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
	DBusGProxy * service_proxy;
	gboolean connected;
	guint this_service_version;
	guint restart_count;
	gint restart_source;
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

/* GObject Stuff */
#define INDICATOR_SERVICE_MANAGER_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATOR_SERVICE_MANAGER_TYPE, IndicatorServiceManagerPrivate))

static void indicator_service_manager_class_init (IndicatorServiceManagerClass *klass);
static void indicator_service_manager_init       (IndicatorServiceManager *self);
static void indicator_service_manager_dispose    (GObject *object);
static void indicator_service_manager_finalize   (GObject *object);

/* Prototypes */
static void set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void service_proxy_destroyed (DBusGProxy * proxy, gpointer user_data);
static void start_service (IndicatorServiceManager * service);
static void start_service_again (IndicatorServiceManager * manager);

G_DEFINE_TYPE (IndicatorServiceManager, indicator_service_manager, G_TYPE_OBJECT);

/* Build all of our signals and proxies and tie everything
   all together.  Lovely. */
static void
indicator_service_manager_class_init (IndicatorServiceManagerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorServiceManagerPrivate));

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

	return;
}

/* This inits all the variable and sets up the proxy
   to dbus.  It doesn't look for the service as at this
   point we don't know it's name. */
static void
indicator_service_manager_init (IndicatorServiceManager *self)
{
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(self);

	/* Get the private variables in a decent state */
	priv->name = NULL;
	priv->service_proxy = NULL;
	priv->connected = FALSE;
	priv->this_service_version = 0;
	priv->restart_count = 0;
	priv->restart_source = 0;

	return;
}

/* If we're connected this provides all the signals to say
   that we're about to not be.  Then it takes down the proxies
   and tells the service that we're not interested in being
   its friend anymore either. */
static void
indicator_service_manager_dispose (GObject *object)
{
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(object);

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

	/* If we have a proxy, tell it we're shutting down.  Just
	   to be polite about it. */
	if (priv->service_proxy != NULL) {
		dbus_g_proxy_call_no_reply(priv->service_proxy, "UnWatch", G_TYPE_INVALID);
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
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(object);

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

	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(self);
	g_return_if_fail(priv != NULL);

	switch (prop_id) {
	/* *********************** */
	case PROP_NAME:
		if (priv->name != NULL) {
			g_error("Name can not be set twice!");
			return;
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

	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(self);
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

/* A callback from telling a service that we want to watch
   it.  It gives us the service API version and the version
   of the other APIs it supports.  We check both of those.
   If they don't match then we unwatch it.  Otherwise, we
   signal a connection change to tell the rest of the world
   that we have a service now.  */
static void
watch_cb (DBusGProxy * proxy, guint service_api_version, guint this_service_version, GError * error, gpointer user_data)
{
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(user_data);

	if (error != NULL) {
		g_warning("Unable to set watch on '%s': '%s'", priv->name, error->message);
		g_error_free(error);
		start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
		return;
	}

	/* We've done it, now let's stop counting. */
	/* Note: we're not checking versions.  Because, the hope is that
	   the guy holding the name we want with the wrong version will
	   drop and we can start another service quickly. */
	priv->restart_count = 0;

	if (service_api_version != INDICATOR_SERVICE_VERSION) {
		g_warning("Service is using a different version of the service interface.  Expecting %d and got %d.", INDICATOR_SERVICE_VERSION, service_api_version);
		dbus_g_proxy_call_no_reply(priv->service_proxy, "UnWatch", G_TYPE_INVALID);

		/* Let's make us wait a little while, then try again */
		priv->restart_count = TIMEOUT_A_LITTLE_WHILE;
		start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
		return;
	}

	if (this_service_version != priv->this_service_version) {
		g_warning("Service is using a different API version than the manager.  Expecting %d and got %d.", priv->this_service_version, this_service_version);
		dbus_g_proxy_call_no_reply(priv->service_proxy, "UnWatch", G_TYPE_INVALID);

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

/* The callback after asking the dbus-daemon to start a
   service for us.  It can return success or failure, on
   failure we can't do much.  But, with sucess, we start
   to build a proxy and tell the service that we're watching. */
static void
start_service_cb (DBusGProxy * proxy, guint status, GError * error, gpointer user_data)
{
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(user_data);

	if (error != NULL) {
		g_warning("Unable to start service '%s': %s", priv->name, error->message);
		start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
		return;
	}

	if (status != DBUS_START_REPLY_SUCCESS && status != DBUS_START_REPLY_ALREADY_RUNNING) {
		g_warning("Status of starting the process '%s' was an error: %d", priv->name, status);
		start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
		return;
	}

	/* Woot! it's running.  Let's do it some more. */
	priv->service_proxy = dbus_g_proxy_new_for_name_owner(priv->bus,
	                                                  priv->name,
	                                                  INDICATOR_SERVICE_OBJECT,
	                                                  INDICATOR_SERVICE_INTERFACE,
	                                                  &error);

	if (error != NULL || priv->service_proxy == NULL) {
		g_warning("Unable to create service proxy on '%s': %s", priv->name, error == NULL ? "(null)" : error->message);
		priv->service_proxy = NULL; /* Should be already, but we want to be *really* sure. */
		g_error_free(error);
		start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
		return;
	}

	g_object_add_weak_pointer(G_OBJECT(priv->service_proxy), (gpointer *)&(priv->service_proxy));
	g_signal_connect(G_OBJECT(priv->service_proxy), "destroy", G_CALLBACK(service_proxy_destroyed), user_data);

	org_ayatana_indicator_service_watch_async(priv->service_proxy,
	                                          watch_cb,
	                                          user_data);

	return;
}

/* The function that handles getting us connected to the service.
   In many cases it will start the service, but if the service
   is already there it just allocates the service proxy and acts
   like it was no big deal. */
static void
start_service (IndicatorServiceManager * service)
{
	GError * error = NULL;
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(service);

	g_return_if_fail(priv->name != NULL);

	if (priv->service_proxy != NULL) {
		g_object_unref(priv->service_proxy);
		priv->service_proxy = NULL;
	}

	/* Check to see if we can get a proxy to it first. */
	priv->service_proxy = dbus_g_proxy_new_for_name_owner(priv->bus,
	                                                      priv->name,
	                                                      INDICATOR_SERVICE_OBJECT,
	                                                      INDICATOR_SERVICE_INTERFACE,
	                                                      &error);

	if (error != NULL || priv->service_proxy == NULL) {
		/* We don't care about the error, just start the service anyway. */
		g_error_free(error);
		org_freedesktop_DBus_start_service_by_name_async (priv->dbus_proxy,
		                                                  priv->name,
		                                                  0,
		                                                  start_service_cb,
		                                                  service);
	} else {
		g_object_add_weak_pointer(G_OBJECT(priv->service_proxy), (gpointer *)&(priv->service_proxy));
		g_signal_connect(G_OBJECT(priv->service_proxy), "destroy", G_CALLBACK(service_proxy_destroyed), service);

		/* If we got a proxy just because we're good people then
		   we need to call watch on it just like 'start_service_cb'
		   does. */
		org_ayatana_indicator_service_watch_async(priv->service_proxy,
		                                          watch_cb,
		                                          service);
	}

	return;
}

/* Responds to the destory event of the proxy and starts
   setting up to restart the service. */
static void
service_proxy_destroyed (DBusGProxy * proxy, gpointer user_data)
{
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(user_data);
	if (priv->connected) {
		priv->connected = FALSE;
		g_signal_emit(G_OBJECT(user_data), signals[CONNECTION_CHANGE], 0, FALSE, TRUE);
	}
	return start_service_again(INDICATOR_SERVICE_MANAGER(user_data));
}

/* The callback that starts the service for real after
   the timeout as determined in 'start_service_again'.
   This could be in the idle or a timer. */
static gboolean
start_service_again_cb (gpointer data)
{
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(data);
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
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(manager);

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
indicator_service_manager_new (gchar * dbus_name)
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
indicator_service_manager_new_version (gchar * dbus_name, guint version)
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
	IndicatorServiceManagerPrivate * priv = INDICATOR_SERVICE_MANAGER_GET_PRIVATE(sm);
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
indicator_service_manager_set_refresh (IndicatorServiceManager * sm, guint time_in_ms)
{

	return;
}
