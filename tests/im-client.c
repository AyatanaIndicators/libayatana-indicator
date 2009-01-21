
#include <glib.h>
#include "libindicate/indicator-message.h"

int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateIndicatorMessage * indicator;

	indicator = indicate_indicator_message_new();
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "subtype", "im");
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "sender", "Ted Gould");
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "time", "11:11");
	indicate_indicator_show(indicator);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
