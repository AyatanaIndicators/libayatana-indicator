#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator-message.h"

typedef struct _IndicateIndicatorMessagePrivate IndicateIndicatorMessagePrivate;

struct _IndicateIndicatorMessagePrivate
{
};

#define INDICATE_INDICATOR_MESSAGE_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATE_INDICATOR_MESSAGE_TYPE, IndicateIndicatorMessagePrivate))

static void     indicate_indicator_message_class_init (IndicateIndicatorMessageClass *klass);
static void     indicate_indicator_message_init       (IndicateIndicatorMessage *self);
static void     indicate_indicator_message_dispose    (GObject *object);
static void     indicate_indicator_message_finalize   (GObject *object);
static const gchar *  get_indicator_type                    (IndicateIndicator * indicator);

G_DEFINE_TYPE (IndicateIndicatorMessage, indicate_indicator_message, INDICATE_TYPE_INDICATOR);

static void
indicate_indicator_message_class_init (IndicateIndicatorMessageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicateIndicatorMessagePrivate));

	object_class->dispose = indicate_indicator_message_dispose;
	object_class->finalize = indicate_indicator_message_finalize;

	IndicateIndicatorClass * indicator_class = INDICATE_INDICATOR_CLASS(klass);

	indicator_class->get_type = get_indicator_type;

	return;
}

static void
indicate_indicator_message_init (IndicateIndicatorMessage *self)
{
}

static void
indicate_indicator_message_dispose (GObject *object)
{
G_OBJECT_CLASS (indicate_indicator_message_parent_class)->dispose (object);
}

static void
indicate_indicator_message_finalize (GObject *object)
{
G_OBJECT_CLASS (indicate_indicator_message_parent_class)->finalize (object);
}

static const gchar *
get_indicator_type (IndicateIndicator * indicator)
{
	return "message";
}

IndicateIndicatorMessage *
indicate_indicator_message_new (void)
{
	return g_object_new(INDICATE_TYPE_INDICATOR_MESSAGE, NULL);
}
