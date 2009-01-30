
#include <glib.h>
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
