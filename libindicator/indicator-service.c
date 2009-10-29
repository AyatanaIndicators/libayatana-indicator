#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator-service.h"

/* Private Stuff */
typedef struct _IndicatorServicePrivate IndicatorServicePrivate;

struct _IndicatorServicePrivate {
	int dummy;
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

	return;
}

static void
indicator_service_init (IndicatorService *self)
{

	return;
}

static void
indicator_service_dispose (GObject *object)
{

	G_OBJECT_CLASS (indicator_service_parent_class)->dispose (object);
	return;
}

static void
indicator_service_finalize (GObject *object)
{

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
		break;
	/* *********************** */
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}

	return;
}

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
