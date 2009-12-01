#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <dbus/dbus-glib-bindings.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "indicator-service.h"

/* DBus Prototypes */
static gboolean _indicator_service_server_watch (IndicatorService * service, DBusGMethodInvocation * method);

#include "indicator-service-server.h"
#include "dbus-shared.h"

/* Private Stuff */
typedef struct _IndicatorServicePrivate IndicatorServicePrivate;

struct _IndicatorServicePrivate {
	gchar * name;
	DBusGProxy * dbus_proxy;
	guint timeout;
	GList * watchers;
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
};

/* The strings so that they can be slowly looked up. */
#define PROP_NAME_S                    "name"

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

static void
indicator_service_init (IndicatorService *self)
{
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(self);

	/* Get the private variables in a decent state */
	priv->name = NULL;
	priv->dbus_proxy = NULL;
	priv->timeout = 0;
	priv->watchers = NULL;

	/* Start talkin' dbus */
	GError * error = NULL;
	DBusGConnection * bus = dbus_g_bus_get(DBUS_BUS_STARTER, &error);
	if (error != NULL) {
		g_error("Unable to get starter bus: %s", error->message);
		g_error_free(error);

		/* Okay, fine let's try the session bus then. */
		/* I think this should automatically, but I can't find confirmation
		   of that, so we're putting the extra little code in here. */
		error = NULL;
		bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
		if (error != NULL) {
			g_error("Unable to get session bus: %s", error->message);
			g_error_free(error);
			return;
		}
	}

	priv->dbus_proxy = dbus_g_proxy_new_for_name_owner(bus,
	                                                   DBUS_SERVICE_DBUS,
	                                                   DBUS_PATH_DBUS,
	                                                   DBUS_INTERFACE_DBUS,
	                                                   &error);
	if (error != NULL) {
		g_error("Unable to get the proxy to DBus: %s", error->message);
		g_error_free(error);
		return;
	}

	dbus_g_connection_register_g_object(bus,
	                                    INDICATOR_SERVICE_OBJECT,
	                                    G_OBJECT(self));

	return;
}

static void
indicator_service_dispose (GObject *object)
{
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(object);

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

static void
indicator_service_finalize (GObject *object)
{
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(object);

	if (priv->name != NULL) {
		g_free(priv->name);
	}

	if (priv->watchers != NULL) {
		g_list_foreach(priv->watchers, (GFunc)g_free, NULL);
		g_list_free(priv->watchers);
		priv->watchers = NULL;
	}

	G_OBJECT_CLASS (indicator_service_parent_class)->finalize (object);
	return;
}

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
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}

	return;
}

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
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}

	return;
}

static gboolean
timeout_no_watchers (gpointer data)
{
	g_signal_emit(G_OBJECT(data), signals[SHUTDOWN], 0, TRUE);
	return FALSE;
}

static void
try_and_get_name_cb (DBusGProxy * proxy, guint status, GError * error, gpointer data)
{
	IndicatorService * service = INDICATOR_SERVICE(data);
	g_return_if_fail(service != NULL);

	if (status != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER && status != DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER) {
		/* The already owner seems like it shouldn't ever
		   happen, but I have a hard time throwing an error
		   on it as we did achieve our goals. */
		g_signal_emit(G_OBJECT(data), signals[SHUTDOWN], 0, TRUE);
		return;
	}

	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(service);
	priv->timeout = g_timeout_add(500, timeout_no_watchers, service);

	return;
}

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

static gboolean
_indicator_service_server_watch (IndicatorService * service, DBusGMethodInvocation * method)
{
	g_return_val_if_fail(INDICATOR_IS_SERVICE(service), FALSE);
	IndicatorServicePrivate * priv = INDICATOR_SERVICE_GET_PRIVATE(service);

	priv->watchers = g_list_append(priv->watchers,
	                               g_strdup(dbus_g_method_get_sender(method)));

	if (priv->timeout != 0) {
		g_source_remove(priv->timeout);
		priv->timeout = 0;
	}

	dbus_g_method_return(method, INDICATOR_SERVICE_VERSION, 0);
	return TRUE;
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
