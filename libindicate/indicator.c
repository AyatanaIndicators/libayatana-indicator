
#include "glib.h"
#include "indicator.h"

/* Signals */
enum {
	USER_DISPLAY,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (IndicateIndicator, indicate_indicator, G_TYPE_OBJECT);

static void indicate_indicator_finalize (GObject * object);


/* Functions */
static void
indicate_indicator_class_init (IndicateIndicatorClass * class)
{
	GObjectClass * gobj;
	gobj = G_OBJECT_CLASS(class);

	gobj->finalize = indicate_indicator_finalize;

	signals[USER_DISPLAY] = g_signal_new(INDICATE_INDICATOR_SIGNAL_DISPLAY,
	                                     G_TYPE_FROM_CLASS(class),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET(IndicateIndicatorClass, user_display),
	                                     NULL, NULL,
	                                     g_cclosure_marshal_VOID__VOID,
	                                     G_TYPE_NONE, 0);

	return;
}

static void
indicate_indicator_init (IndicateIndicator * indicator)
{
	indicator->id = 0;

	indicator->server = indicate_server_ref_default();
	indicate_server_add_indicator(indicator->server, indicator);

	return;
}

static void
indicate_indicator_finalize (GObject * obj)
{
	IndicateIndicator * indicator = INDICATE_INDICATOR(obj);

	indicate_server_remove_indicator(indicator->server, indicator);
	g_object_unref(indicator->server);
	indicator->server = NULL;

	return;
}

IndicateIndicator *
indicate_indicator_new (void)
{
	IndicateIndicator * indicator = g_object_new(INDICATE_TYPE_INDICATOR, NULL);
	return indicator;
}

