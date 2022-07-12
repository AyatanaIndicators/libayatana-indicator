/*
An object used to provide a simple interface for a service
to query version and manage whether it's running.

Copyright 2009 Canonical Ltd.
Copyright 2022 Robert tari

Authors:
    Ted Gould <ted@canonical.com>
    Robert Tari <robert@tari.in>

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

#include <gio/gio.h>

#include "indicator-service.h"
#include "gen-indicator-service.xml.h"
#include "dbus-shared.h"

static void unwatch_core (IndicatorService * service, const gchar * name);
static void watchers_remove (gpointer value);
static void bus_get_cb (GObject * object, GAsyncResult * res, gpointer user_data);
static GVariant * bus_watch (IndicatorService * service, const gchar * sender);

/* Private Stuff */
/**
    IndicatorSevicePrivate:
    @name: The DBus well known name for the service.
    @timeout: The source ID for the timeout event.
    @watcher: A list of processes on dbus that are watching us.
    @this_service_version: The version to hand out that we're
        implementing.  May not be set, so we'll send zero (default).
    @dbus_registration: The handle for this object being registered
        on dbus.
*/
typedef struct _IndicatorServicePrivate IndicatorServicePrivate;
struct _IndicatorServicePrivate {
    gchar * name;
    GDBusConnection * bus;
    GCancellable * bus_cancel;
    guint timeout;
    guint timeout_length;
    GHashTable * watchers;
    guint this_service_version;
    guint dbus_registration;
    gboolean replace_mode;
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
static void bus_method_call (GDBusConnection * connection, const gchar * sender, const gchar * path, const gchar * interface, const gchar * method, GVariant * params, GDBusMethodInvocation * invocation, gpointer user_data);

/* GDBus Stuff */
static GDBusNodeInfo *            node_info = NULL;
static GDBusInterfaceInfo *       interface_info = NULL;
static GDBusInterfaceVTable       interface_table = {
    .method_call  = bus_method_call,
    .get_property = NULL, /* No properties */
    .set_property = NULL  /* No properties */
};

/* THE define */
G_DEFINE_TYPE_WITH_PRIVATE (IndicatorService, indicator_service, G_TYPE_OBJECT);

static void
indicator_service_class_init (IndicatorServiceClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

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

/* This function builds the variables, sets up the dbus
   proxy and registers the object on dbus.  Importantly,
   it does not request a name as we don't know what name
   we have yet. */
static void
indicator_service_init (IndicatorService *self)
{
    IndicatorServicePrivate * priv = indicator_service_get_instance_private(self);

    /* Get the private variables in a decent state */
    priv->name = NULL;
    priv->timeout = 0;
    priv->watchers = NULL;
    priv->bus = NULL;
    priv->bus_cancel = NULL;
    priv->this_service_version = 0;
    priv->timeout_length = 500;
    priv->dbus_registration = 0;
    priv->replace_mode = FALSE;

    const gchar * timeoutenv = g_getenv("INDICATOR_SERVICE_SHUTDOWN_TIMEOUT");
    if (timeoutenv != NULL) {
        gdouble newtimeout = g_strtod(timeoutenv, NULL);
        if (newtimeout >= 1.0f) {
            priv->timeout_length = newtimeout;
            g_debug("Setting shutdown timeout to: %u", priv->timeout_length);
        }
    }

    const gchar * replaceenv = g_getenv("INDICATOR_SERVICE_REPLACE_MODE");
    if (replaceenv != NULL) {
        priv->replace_mode = TRUE;
        g_debug("Putting into replace mode");
    }

    /* NOTE: We're using g_free here because that's what needs to
       happen and we're watchers_remove as well to clean up the dbus
       watches we've setup. */
    priv->watchers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, watchers_remove);

    priv->bus_cancel = g_cancellable_new();
    g_bus_get(G_BUS_TYPE_SESSION,
              priv->bus_cancel,
              bus_get_cb,
              self);

    return;
}

/* Unrefcounting the proxies and making sure that our
   timeout doesn't come to haunt us. */
static void
indicator_service_dispose (GObject *object)
{
    IndicatorService * service = INDICATOR_SERVICE(object);
    IndicatorServicePrivate * priv = indicator_service_get_instance_private(service);

    g_clear_pointer (&priv->watchers, g_hash_table_destroy);

    if (priv->timeout != 0) {
        g_source_remove(priv->timeout);
        priv->timeout = 0;
    }

    if (priv->dbus_registration != 0) {
        g_dbus_connection_unregister_object(priv->bus, priv->dbus_registration);
        /* Don't care if it fails, there's nothing we can do */
        priv->dbus_registration = 0;
    }

    g_clear_object (&priv->bus);

    if (priv->bus_cancel != NULL) {
        g_cancellable_cancel(priv->bus_cancel);
        g_object_unref(priv->bus_cancel);
        priv->bus_cancel = NULL;
    }

    G_OBJECT_CLASS (indicator_service_parent_class)->dispose (object);
    return;
}

/* Freeing the name we're looking for and all of the
   information on the watchers we're tracking. */
static void
indicator_service_finalize (GObject *object)
{
    IndicatorService * service = INDICATOR_SERVICE(object);
    IndicatorServicePrivate * priv = indicator_service_get_instance_private(service);

    g_free (priv->name);
    g_clear_pointer (&priv->watchers, g_hash_table_destroy);

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

    IndicatorServicePrivate * priv = indicator_service_get_instance_private(self);
    g_return_if_fail(priv != NULL);

    switch (prop_id) {
    /* *********************** */
    case PROP_NAME:
        if (G_VALUE_HOLDS_STRING(value)) {
            if (priv->name != NULL) {
                g_error("Name can not be set twice!");
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

    IndicatorServicePrivate * priv = indicator_service_get_instance_private(self);
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

/* Callback for getting our connection to DBus */
static void
bus_get_cb (__attribute__((unused)) GObject * object, GAsyncResult * res, gpointer user_data)
{
    GError * error = NULL;
    GDBusConnection * connection = g_bus_get_finish(res, &error);

    if (error != NULL)
    {
        g_error("Unable to get a connection to the session DBus: %s", error->message);
    }

    IndicatorServicePrivate * priv = indicator_service_get_instance_private(user_data);

    g_warn_if_fail(priv->bus == NULL);
    priv->bus = connection;

    if (priv->bus_cancel != NULL) {
        g_object_unref(priv->bus_cancel);
        priv->bus_cancel = NULL;
    }

    /* Now register our object on our new connection */
    priv->dbus_registration = g_dbus_connection_register_object(priv->bus,
                                                                INDICATOR_SERVICE_OBJECT,
                                                                interface_info,
                                                                &interface_table,
                                                                user_data,
                                                                NULL,
                                                                &error);
    if (error != NULL) {
        g_error("Unable to register the object to DBus: %s", error->message);
    }

    return;
}

/* A method has been called from our dbus inteface.  Figure out what it
   is and dispatch it. */
static void
bus_method_call (__attribute__((unused)) GDBusConnection * connection, const gchar * sender, __attribute__((unused)) const gchar * path, __attribute__((unused)) const gchar * interface, const gchar * method, __attribute__((unused)) GVariant * params, GDBusMethodInvocation * invocation, gpointer user_data)
{
    IndicatorService * service = INDICATOR_SERVICE(user_data);
    GVariant * retval = NULL;

    if (g_strcmp0(method, "Watch") == 0) {
        retval = bus_watch(service, sender);
    } else if (g_strcmp0(method, "UnWatch") == 0) {
        unwatch_core(service, sender);
    } else if (g_strcmp0(method, "Shutdown") == 0) {
        g_signal_emit(G_OBJECT(service), signals[SHUTDOWN], 0, TRUE);
    } else {
        g_warning("Calling method '%s' on the indicator service and it's unknown", method);
    }

    g_dbus_method_invocation_return_value(invocation, retval);
    return;
}

/* A function to remove the signals on a proxy before we destroy
   it because in this case we've stopped caring. */
static void
watchers_remove (gpointer value)
{
    g_bus_unwatch_name(GPOINTER_TO_UINT(value));
    return;
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

/* Callback saying that the name we were looking for has been
   found and we've got it.  Now start the timer to see if anyone
   cares about us. */
static void
try_and_get_name_acquired_cb (GDBusConnection * connection, __attribute__((unused)) const gchar * name, gpointer user_data)
{
    g_return_if_fail(connection != NULL);
    g_return_if_fail(INDICATOR_IS_SERVICE(user_data));

    IndicatorServicePrivate * priv = indicator_service_get_instance_private(user_data);

    /* Check to see if we already had a timer, if so we want to
       extend it a bit. */
    if (priv->timeout != 0) {
        g_source_remove(priv->timeout);
        priv->timeout = 0;
    }

    /* Allow some extra time at start up as things can be in high
       contention then. */
    priv->timeout = g_timeout_add(priv->timeout_length * 2, timeout_no_watchers, user_data);

    return;
}

/* Callback saying that we didn't get the name, so we need to
   shutdown this service. */
static void
try_and_get_name_lost_cb (GDBusConnection * connection, const gchar * name, gpointer user_data)
{
    g_return_if_fail(connection != NULL);
    g_return_if_fail(INDICATOR_IS_SERVICE(user_data));

    IndicatorServicePrivate * priv = indicator_service_get_instance_private(user_data);

    if (!priv->replace_mode) {
        g_warning("Name request failed.");
        g_signal_emit(G_OBJECT(user_data), signals[SHUTDOWN], 0, TRUE);
    } else {
        /* If we're in replace mode we can be a little more trickey
           here.  We're going to tell the other guy to shutdown and hope
           that we get the name. */
        GDBusMessage * message = NULL;
        message = g_dbus_message_new_method_call(name,
                                                 INDICATOR_SERVICE_OBJECT,
                                                 INDICATOR_SERVICE_INTERFACE,
                                                 "Shutdown");

        g_dbus_connection_send_message(connection, message, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
        g_object_unref(message);

        /* Check to see if we need to clean up a timeout */
        if (priv->timeout != 0) {
            g_source_remove(priv->timeout);
            priv->timeout = 0;
        }

        /* Set a timeout for no watchers if we can't get the name */
        priv->timeout = g_timeout_add(priv->timeout_length * 4, timeout_no_watchers, user_data);
    }

    return;
}

/* This function sets up the request for the name on dbus. */
static void
try_and_get_name (IndicatorService * service)
{
    IndicatorServicePrivate * priv = indicator_service_get_instance_private(service);
    g_return_if_fail(priv->name != NULL);

    g_bus_own_name(G_BUS_TYPE_SESSION,
                   priv->name,
                   G_BUS_NAME_OWNER_FLAGS_NONE,
                   NULL, /* bus acquired */
                   try_and_get_name_acquired_cb, /* name acquired */
                   try_and_get_name_lost_cb, /* name lost */
                   service,
                   NULL); /* user data destroy */

    return;
}

/* When the watcher vanishes we don't really care about it
   anymore. */
static void
watcher_vanished_cb (__attribute__((unused)) GDBusConnection * connection, const gchar * name, gpointer user_data)
{
    g_return_if_fail(INDICATOR_IS_SERVICE(user_data));
    IndicatorServicePrivate * priv = indicator_service_get_instance_private(user_data);

    gpointer finddata = g_hash_table_lookup(priv->watchers, name);
    if (finddata != NULL) {
        unwatch_core(INDICATOR_SERVICE(user_data), name);
    } else {
        g_warning("Odd, we were watching for '%s' and it disappeard, but then it wasn't in the hashtable.", name);
    }

    return;
}

/* Here is the function that gets called by the dbus
   interface "Watch" function.  It is an async function so
   that we can get the sender and store that information.  We
   put them in a list and reset the timeout. */
static GVariant *
bus_watch (IndicatorService * service, const gchar * sender)
{
    g_return_val_if_fail(INDICATOR_IS_SERVICE(service), NULL);
    IndicatorServicePrivate * priv = indicator_service_get_instance_private(service);

    if (GPOINTER_TO_UINT(g_hash_table_lookup(priv->watchers, sender)) == 0) {
        guint watch = g_bus_watch_name_on_connection(priv->bus,
                                                     sender,
                                                     G_BUS_NAME_WATCHER_FLAGS_NONE,
                                                     NULL, /* appeared, we dont' care, should have already happened. */
                                                     watcher_vanished_cb,
                                                     service,
                                                     NULL);

        if (watch != 0) {
            g_hash_table_insert(priv->watchers, g_strdup(sender), GUINT_TO_POINTER(watch));
        } else {
            g_warning("Unable watch for '%s'", sender);
        }
    }

    if (priv->timeout != 0) {
        g_source_remove(priv->timeout);
        priv->timeout = 0;
    }

    return g_variant_new("(uu)", INDICATOR_SERVICE_VERSION, priv->this_service_version);
}

/* Performs the core of loosing a watcher; it removes them
   from the list of watchers.  If there are none left, it then
   starts the timer for the shutdown signal. */
static void
unwatch_core (IndicatorService * service, const gchar * name)
{
    g_return_if_fail(name != NULL);
    g_return_if_fail(INDICATOR_IS_SERVICE(service));

    IndicatorServicePrivate * priv = indicator_service_get_instance_private(service);

    /* Remove us from the watcher list here */
    gpointer watcher_item = g_hash_table_lookup(priv->watchers, name);
    if (watcher_item != NULL) {
        gchar * safe_name = g_strdup(name);
        g_hash_table_remove(priv->watchers, safe_name);
        g_free(safe_name);
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
        priv->timeout = g_timeout_add(priv->timeout_length, timeout_no_watchers, service);
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
