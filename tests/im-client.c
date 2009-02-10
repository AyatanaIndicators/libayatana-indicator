
#include <glib.h>
#include "libindicate/server.h"
#include "libindicate/indicator-message.h"

static void
display (IndicateIndicator * indicator, gpointer data)
{
	g_debug("Ah, I've been displayed");
}

int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateServer * server = indicate_server_ref_default();
	GValue value = {0};
	g_value_init(&value, G_TYPE_STRING);
	g_value_set_static_string(&value, "message.im");
	g_object_set_property(G_OBJECT(server), "type", &value);

	IndicateIndicatorMessage * indicator;

	indicator = indicate_indicator_message_new();
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "subtype", "im");
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "sender", "Ted Gould");
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "time", "11:11");
	indicate_indicator_show(INDICATE_INDICATOR(indicator));

	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "time", "11:12");

	g_signal_connect(G_OBJECT(indicator), INDICATE_INDICATOR_SIGNAL_DISPLAY, G_CALLBACK(display), NULL);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
