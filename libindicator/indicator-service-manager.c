#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator-service-manager.h"

/* Private Stuff */
typedef struct _IndicatorServiceManagerPrivate IndicatorServiceManagerPrivate;
struct _IndicatorServiceManagerPrivate {
	gchar * name;
};

/* Signals Stuff */
enum {
	CONNECTION_CHANGE,
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
#define INDICATOR_SERVICE_MANAGER_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATOR_SERVICE_MANAGER_TYPE, IndicatorServiceManagerPrivate))

static void indicator_service_manager_class_init (IndicatorServiceManagerClass *klass);
static void indicator_service_manager_init       (IndicatorServiceManager *self);
static void indicator_service_manager_dispose    (GObject *object);
static void indicator_service_manager_finalize   (GObject *object);

G_DEFINE_TYPE (IndicatorServiceManager, indicator_service_manager, G_TYPE_OBJECT);

static void
indicator_service_manager_class_init (IndicatorServiceManagerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorServiceManagerPrivate));

	object_class->dispose = indicator_service_manager_dispose;
	object_class->finalize = indicator_service_manager_finalize;

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

	return;
}

static void
indicator_service_manager_init (IndicatorServiceManager *self)
{

	return;
}

static void
indicator_service_manager_dispose (GObject *object)
{

	G_OBJECT_CLASS (indicator_service_manager_parent_class)->dispose (object);
	return;
}

static void
indicator_service_manager_finalize (GObject *object)
{

	G_OBJECT_CLASS (indicator_service_manager_parent_class)->finalize (object);
	return;
}

/* API */
IndicatorServiceManager *
indicator_service_manager_new (gchar * dbus_name)
{
	GObject * obj = g_object_new(INDICATOR_SERVICE_MANAGER_TYPE,
	                             PROP_NAME_S, dbus_name,
	                             NULL);

	return INDICATOR_SERVICE_MANAGER(obj);
}

gboolean
indicator_service_manager_connected (IndicatorServiceManager * sm)
{

	return FALSE;
}

void
indicator_service_manager_set_refresh (IndicatorServiceManager * sm, guint time_in_ms)
{

	return;
}
