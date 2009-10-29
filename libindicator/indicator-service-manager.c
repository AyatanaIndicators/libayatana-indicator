#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator-service-manager.h"

typedef struct _IndicatorServiceManagerPrivate IndicatorServiceManagerPrivate;
struct _IndicatorServiceManagerPrivate {
	int dummy;
};

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
	                             "name", dbus_name,
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
