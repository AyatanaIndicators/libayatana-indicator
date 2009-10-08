#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator-object.h"

typedef struct _IndicatorObjectPrivate IndicatorObjectPrivate;
struct _IndicatorObjectPrivate {
	guint data;
};

#define INDICATOR_OBJECT_GET_PRIVATE(o) \
			(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATOR_OBJECT_TYPE, IndicatorObjectPrivate))

static void indicator_object_class_init (IndicatorObjectClass *klass);
static void indicator_object_init       (IndicatorObject *self);
static void indicator_object_dispose    (GObject *object);
static void indicator_object_finalize   (GObject *object);

G_DEFINE_TYPE (IndicatorObject, indicator_object, G_TYPE_OBJECT);

static void
indicator_object_class_init (IndicatorObjectClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorObjectPrivate));

	object_class->dispose = indicator_object_dispose;
	object_class->finalize = indicator_object_finalize;

	return;
}

static void
indicator_object_init (IndicatorObject *self)
{

	return;
}

static void
indicator_object_dispose (GObject *object)
{

	G_OBJECT_CLASS (indicator_object_parent_class)->dispose (object);
	return;
}

static void
indicator_object_finalize (GObject *object)
{

	G_OBJECT_CLASS (indicator_object_parent_class)->finalize (object);
	return;
}

IndicatorObject *
indicator_object_new_from_file (const gchar * file)
{
	return NULL;
}
