#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator-service.h"

typedef struct _IndicatorServicePrivate IndicatorServicePrivate;

struct _IndicatorServicePrivate {
	int dummy;
};

#define INDICATOR_SERVICE_GET_PRIVATE(o) \
			(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATOR_SERVICE_TYPE, IndicatorServicePrivate))

static void indicator_service_class_init (IndicatorServiceClass *klass);
static void indicator_service_init       (IndicatorService *self);
static void indicator_service_dispose    (GObject *object);
static void indicator_service_finalize   (GObject *object);

G_DEFINE_TYPE (IndicatorService, indicator_service, G_TYPE_OBJECT);

static void
indicator_service_class_init (IndicatorServiceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorServicePrivate));

	object_class->dispose = indicator_service_dispose;
	object_class->finalize = indicator_service_finalize;


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

IndicatorService *
indicator_service_new (gchar * name)
{
	GObject * obj = g_object_new(INDICATOR_SERVICE_TYPE, NULL);

	return INDICATOR_SERVICE(obj);
}
