/*
An object used to provide a simple interface for a service
to query version and manage whether it's running.

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
#include <dbus/dbus-glib-bindings.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "indicator-service.h"

static void unwatch_core (IndicatorService * service, const gchar * name);
static void proxy_destroyed (GObject * proxy, gpointer user_data);
static gboolean watchers_remove (gpointer key, gpointer value, gpointer user_data);
/* DBus Prototypes */
static gboolean _indicator_service_server_watch (IndicatorService * service, DBusGMethodInvocation * method);
static gboolean _indicator_service_server_un_watch (IndicatorService * service, DBusGMethodInvocation * method);

#include "indicator-service-server.h"
#include "dbus-shared.h"

/* Private Stuff */
/**
	IndicatorSevicePrivate:
	@name: The DBus well known name for the service.
	@dbus_proxy: A proxy for talking to the dbus bus manager.
	@timeout: The source ID for the timeout event.
	@watcher: A list of processes on dbus that are watching us.
	@this_service_version: The version to hand out that we're
		implementing.  May not be set, so we'll send zero (default).
*/
typedef struct _IndicatorServicePrivate IndicatorServicePrivate;
struct _IndicatorServicePrivate {
	gchar * name;
	DBusGProxy * dbus_proxy;
	DBusGConnection * bus;
	guint timeout;
	GHashTable * watchers;
	guint this_service_version;
};

/* Signals Stuff */
enum {
	SHUTDOWN,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

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
#define INDICATOR_SERVICE_GET_PRIVATE(o) \
			(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATOR_SERVICE_TYPE, IndicatorServicePrivate))

static void indicator_service_class_init (IndicatorServiceClass *klass);
static void indicator_service_init       (IndicatorService *self);
static void indicator_service_dispose    (GObject *object);
static void indicator_service_finalize   (GObject *object);

/* Other prototypes */
static void set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static void try_and_get_name (IndicatorService * service);

G_DEFINE_TYPE (IndicatorService, indicator_service, G_TYPE_OBJECT);

static void
indicator_service_class_init (IndicatorServiceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorServicePrivate));

	object_class->dispose = indicator_service_dispose;
	object_class->finalize = indicator_service_finalize;

	/* Property funcs */
	object_class->set_property = set_property;
	object_class->get_property = get_property;

	/* Properties */
	g_object_class_install_property(object_class, PROP_NAME,
	                                g_param_spec_string(PROP_NAME_S,
	                                                    "The DBus name for this service",
	                                                    "This is the name that should be used on DBus for this service.",
	                                                    NULL,
	                                                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property(object_class, PROP_VERSION,
	                                g_param_spec_uint(PROP_VERSION_S,
	                                                  "The version of the service that we're implementing.",
	                                                  "A number to represent the version of the other APIs the service provides.  This should match across the manager and the service",
	                                                  0, G_MAXUINT, 0,
	                                                  G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/* Signals */

	/**
		IndicatorService::shutdown:
		@arg0: The #IndicatorService object
		
		Signaled when the service should shutdown as no one
		is listening anymore.
	*/
	signals[SHUTDOWN] = g_signal_new (INDICATOR_SERVICE_SIGNAL_SHUTDOWN,
	                                  G_TYPE_FROM_CLASS(klass),
	                                  G_SIGNAL_RUN_LAST,
	                                  G_STRUCT_OFFSET (IndicatorServiceClass, shutdown),
	                                  NULL, NULL,
	                                  g_cclosure_marshal_VOID__VOID,
	                                  G_TYPE_NONE, 0, G_TYPE_NONE);

	/* Initialize the object as a DBus type */
	dbus_g_object_type_install_info(INDICATOR_SERVICE_TYPE,
	                                &dbus_glib__indicator_service_server_object_info);

	return;
}

/* This function builds the variables, sets up the dbus
   proxy and registers the object on dbus.  Importantly,
   it does not request a name as we don't know what name
   we have yet. */
static void
indicator_service_init (IndicatorService *self)
{
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(self);

	/* Get the private variables in a decent state */
	priv->name = NULL;
	priv->dbus_proxy = NULL;
	priv->timeout = 0;
	priv->watchers = NULL;
	priv->bus = NULL;
	priv->this_service_version = 0;

	priv->watchers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref);

	/* Start talkin' dbus */
	GError * error = NULL;
	priv->bus = dbus_g_bus_get(DBUS_BUS_STARTER, &error);
	if (error != NULL) {
		g_error("Unable to get starter bus: %s", error->message);
		g_error_free(error);

		/* Okay, fine let's try the session bus then. */
		/* I think this should automatically, but I can't find confirmation
		   of that, so we're putting the extra little code in here. */
		error = NULL;
		priv->bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
		if (error != NULL) {
			g_error("Unable to get session bus: %s", error->message);
			g_error_free(error);
			return;
		}
	}

	priv->dbus_proxy = dbus_g_proxy_new_for_name_owner(priv->bus,
	                                                   DBUS_SERVICE_DBUS,
	                                                   DBUS_PATH_DBUS,
	                                                   DBUS_INTERFACE_DBUS,
	                                                   &error);
	if (error != NULL) {
		g_error("Unable to get the proxy to DBus: %s", error->message);
		g_error_free(error);
		return;
	}

	dbus_g_connection_register_g_object(priv->bus,
	                                    INDICATOR_SERVICE_OBJECT,
	                                    G_OBJECT(self));

	return;
}

/* Unrefcounting the proxies and making sure that our
   timeout doesn't come to haunt us. */
static void
indicator_service_dispose (GObject *object)
{
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(object);

	if (priv->watchers != NULL) {
		g_hash_table_foreach_remove(priv->watchers, watchers_remove, object);
	}

	if (priv->dbus_proxy != NULL) {
		g_object_unref(G_OBJECT(priv->dbus_proxy));
		priv->dbus_proxy = NULL;
	}

	if (priv->timeout != 0) {
		g_source_remove(priv->timeout);
		priv->timeout = 0;
	}

	G_OBJECT_CLASS (indicator_service_parent_class)->dispose (object);
	return;
}

/* Freeing the name we're looking for and all of the
   information on the watchers we're tracking. */
static void
indicator_service_finalize (GObject *object)
{
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(object);

	if (priv->name != NULL) {
		g_free(priv->name);
	}

	if (priv->watchers != NULL) {
		g_hash_table_destroy(priv->watchers);
		priv->watchers = NULL;
	}

	G_OBJECT_CLASS (indicator_service_parent_class)->finalize (object);
	return;
}

/* Either copies a string for the name or it just grabs
   the value of the version. */
static void
set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
	IndicatorService * self = INDICATOR_SERVICE(object);
	g_return_if_fail(self != NULL);

	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(self);
	g_return_if_fail(priv != NULL);

	switch (prop_id) {
	/* *********************** */
	case PROP_NAME:
		if (G_VALUE_HOLDS_STRING(value)) {
			if (priv->name != NULL) {
				g_error("Name can not be set twice!");
				return;
			}
			priv->name = g_value_dup_string(value);
			try_and_get_name(self);
		} else {
			g_warning("Name property requires a string value.");
		}
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

/* Copies out the name into a value or the version number.
   Probably this is the least useful code in this file. */
static void
get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
	IndicatorService * self = INDICATOR_SERVICE(object);
	g_return_if_fail(self != NULL);

	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(self);
	g_return_if_fail(priv != NULL);

	switch (prop_id) {
	/* *********************** */
	case PROP_NAME:
		if (G_VALUE_HOLDS_STRING(value)) {
			g_value_set_string(value, priv->name);
		} else {
			g_warning("Name property requires a string value.");
		}
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

/* A function to remove the signals on a proxy before we destroy
   it because in this case we've stopped caring. */
static gboolean
watchers_remove (gpointer key, gpointer value, gpointer user_data)
{
	g_signal_handlers_disconnect_by_func(G_OBJECT(value), G_CALLBACK(proxy_destroyed), user_data);
	return TRUE;
}

/* This is the function that gets executed if we timeout
   because there are no watchers.  We sent the shutdown
   signal and hope someone does something sane with it. */
static gboolean
timeout_no_watchers (gpointer data)
{
	g_warning("No watchers, service timing out.");
	if (g_getenv("INDICATOR_ALLOW_NO_WATCHERS") == NULL) {
		g_signal_emit(G_OBJECT(data), signals[SHUTDOWN], 0, TRUE);
	} else {
		g_warning("\tblocked by environment variable.");
	}
	return FALSE;
}

/* The callback from our request to get a well known name
   on dbus.  If we can't get it we send the shutdown signal.
   Else we start the timer to see if anyone cares about us. */
static void
try_and_get_name_cb (DBusGProxy * proxy, guint status, GError * error, gpointer data)
{
	IndicatorService * service = INDICATOR_SERVICE(data);
	g_return_if_fail(service != NULL);

	if (error != NULL) {
		g_warning("Unable to send message to request name: %s", error->message);
		g_signal_emit(G_OBJECT(data), signals[SHUTDOWN], 0, TRUE);
		return;
	}

	if (status != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER && status != DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER) {
		/* The already owner seems like it shouldn't ever
		   happen, but I have a hard time throwing an error
		   on it as we did achieve our goals. */
		g_warning("Name request failed.  Status returned: %d", status);
		g_signal_emit(G_OBJECT(data), signals[SHUTDOWN], 0, TRUE);
		return;
	}

	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(service);
	priv->timeout = g_timeout_add_seconds(1, timeout_no_watchers, service);

	return;
}

/* This function sets up the request for the name on dbus. */
static void
try_and_get_name (IndicatorService * service)
{
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(service);
	g_return_if_fail(priv->dbus_proxy != NULL);
	g_return_if_fail(priv->name != NULL);

	org_freedesktop_DBus_request_name_async(priv->dbus_proxy,
	                                        priv->name,
	                                        DBUS_NAME_FLAG_DO_NOT_QUEUE,
	                                        try_and_get_name_cb,
	                                        service);

	return;
}

/* If the proxy gets destroyed that's the same as getting an
   unwatch signal.  Make it so. */
static void
proxy_destroyed (GObject * proxy, gpointer user_data)
{
	g_return_if_fail(INDICATOR_IS_SERVICE(user_data));

	const gchar * name = dbus_g_proxy_get_bus_name(DBUS_G_PROXY(proxy));
	unwatch_core(INDICATOR_SERVICE(user_data), name);

	return;
}

/* Here is the function that gets called by the dbus
   interface "Watch" function.  It is an async function so
   that we can get the sender and store that information.  We
   put them in a list and reset the timeout. */
static gboolean
_indicator_service_server_watch (IndicatorService * service, DBusGMethodInvocation * method)
{
	g_return_val_if_fail(INDICATOR_IS_SERVICE(service), FALSE);
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(service);

	const gchar * sender = dbus_g_method_get_sender(method);
	if (g_hash_table_lookup(priv->watchers, sender) == NULL) {
		GError * error = NULL;
		DBusGProxy * senderproxy = dbus_g_proxy_new_for_name_owner(priv->bus,
		                                                           sender,
		                                                           "/",
		                                                           DBUS_INTERFACE_INTROSPECTABLE,
		                                                           &error);

		g_signal_connect(G_OBJECT(senderproxy), "destroy", G_CALLBACK(proxy_destroyed), service);

		if (error == NULL) {
			g_hash_table_insert(priv->watchers, g_strdup(sender), senderproxy);
		} else {
			g_warning("Unable to create proxy for watcher '%s': %s", sender, error->message);
			g_error_free(error);
		}
	}

	if (priv->timeout != 0) {
		g_source_remove(priv->timeout);
		priv->timeout = 0;
	}

	dbus_g_method_return(method, INDICATOR_SERVICE_VERSION, priv->this_service_version);
	return TRUE;
}

/* A function connecting into the dbus interface for the
   "UnWatch" function.  It is also an async function to get
   the sender and passes everything to unwatch_core to remove it. */
static gboolean
_indicator_service_server_un_watch (IndicatorService * service, DBusGMethodInvocation * method)
{
	g_return_val_if_fail(INDICATOR_IS_SERVICE(service), FALSE);

	unwatch_core(service, dbus_g_method_get_sender(method));

	dbus_g_method_return(method);
	return TRUE;
}

/* Performs the core of loosing a watcher; it removes them
   from the list of watchers.  If there are none left, it then
   starts the timer for the shutdown signal. */
static void
unwatch_core (IndicatorService * service, const gchar * name)
{
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(service);

	/* Remove us from the watcher list here */
	gpointer watcher_item = g_hash_table_lookup(priv->watchers, name);
	if (watcher_item != NULL) {
		/* Free the watcher */
		watchers_remove((gpointer)name, watcher_item, service);
		g_hash_table_remove(priv->watchers, name);
	} else {
		/* Odd that we couldn't find the person, but, eh */
		g_warning("Unable to find watcher who is unwatching: %s", name);
	}

	/* If we're out of watchers set the timeout for shutdown */
	if (g_hash_table_size(priv->watchers) == 0) {
		if (priv->timeout != 0) {
			/* This should never really happen, but let's ensure that
			   bad things don't happen if it does. */
			g_warning("No watchers timeout set twice.  Resolving, but odd.");
			g_source_remove(priv->timeout);
			priv->timeout = 0;
		}
		/* If we don't get a new watcher quickly, we'll shutdown. */
		priv->timeout = g_timeout_add(500, timeout_no_watchers, service);
	}

	return;
}

/* API */

/**
	indicator_service_new:
	@name: The name for the service on dbus

	This function creates the service on DBus and tries to
	get a well-known name specified in @name.  If the name
	can't be estabilished then the #IndicatorService::shutdown
	signal will be sent.

	Return value: A brand new #IndicatorService object or #NULL
		if there is an error.
*/
IndicatorService *
indicator_service_new (gchar * name)
{
	GObject * obj = g_object_new(INDICATOR_SERVICE_TYPE,
	                             PROP_NAME_S, name,
	                             NULL);

	return INDICATOR_SERVICE(obj);
}

/**
	indicator_service_new_version:
	@name: The name for the service on dbus
	@version: The version of the other interfaces provide
		by the service.

	This function creates the service on DBus and tries to
	get a well-known name specified in @name.  If the name
	can't be estabilished then the #IndicatorService::shutdown
	signal will be sent.

	Return value: A brand new #IndicatorService object or #NULL
		if there is an error.
*/
IndicatorService *
indicator_service_new_version (gchar * name, guint version)
{
	GObject * obj = g_object_new(INDICATOR_SERVICE_TYPE,
	                             PROP_NAME_S, name,
	                             PROP_VERSION_S, version,
	                             NULL);

	return INDICATOR_SERVICE(obj);
}
