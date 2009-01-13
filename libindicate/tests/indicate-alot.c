
#include <glib.h>
#include "libindicate/indicator.h"

#define ALOT  30


int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateIndicator * indicators[ALOT];
	int i;

	for (i = 0; i < ALOT; i++) {
		indicators[i] = indicate_indicator_new();
		indicate_indicator_show(indicators[i]);
	}

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
