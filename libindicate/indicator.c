
#include "glib.h"
#include "glib/gmessages.h"
#include "indicator.h"

/* Signals */
enum {
	HIDE,
	SHOW,
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
	g_debug("Indicator Class Initialized.");

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
	signals[HIDE] = g_signal_new(INDICATE_INDICATOR_SIGNAL_HIDE,
	                                     G_TYPE_FROM_CLASS(class),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET(IndicateIndicatorClass, hide),
	                                     NULL, NULL,
	                                     g_cclosure_marshal_VOID__VOID,
	                                     G_TYPE_NONE, 0);
	signals[SHOW] = g_signal_new(INDICATE_INDICATOR_SIGNAL_SHOW,
	                                     G_TYPE_FROM_CLASS(class),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET(IndicateIndicatorClass, show),
	                                     NULL, NULL,
	                                     g_cclosure_marshal_VOID__VOID,
	                                     G_TYPE_NONE, 0);

	return;
}

static void
indicate_indicator_init (IndicateIndicator * indicator)
{
	g_debug("Indicator Object Initialized.");

	indicator->is_visible = FALSE;
	indicator->server = indicate_server_ref_default();
	indicator->id = indicate_server_get_next_id(indicator->server);
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

void
indicate_indicator_show (IndicateIndicator * indicator)
{
	if (indicator->is_visible) {
		return;
	}

	if (indicator->server) {
		indicate_server_show(indicator->server);
	}

	indicator->is_visible = TRUE;
	g_signal_emit(indicator, signals[SHOW], 0, TRUE);

	return;
}

void
indicate_indicator_hide (IndicateIndicator * indicator)
{
	if (!indicator->is_visible) {
		return;
	}

	indicator->is_visible = FALSE;
	g_signal_emit(indicator, signals[HIDE], 0, TRUE);

	return;
}

gboolean
indicate_indicator_is_visible (IndicateIndicator * indicator)
{
	return indicator->is_visible;
}

guint
indicate_indicator_get_id (IndicateIndicator * indicator)
{
	g_return_val_if_fail(INDICATE_IS_INDICATOR(indicator), 0);
	return indicator->id;
}

const gchar *
indicate_indicator_get_indicator_type (IndicateIndicator * indicator)
{
	g_return_val_if_fail(INDICATE_IS_INDICATOR(indicator), NULL);
	IndicateIndicatorClass * class = INDICATE_INDICATOR_GET_CLASS(indicator);

	if (class->get_type != NULL) {
		return INDICATE_INDICATOR_GET_CLASS(indicator)->get_type(indicator);
	}

	return NULL;
}


