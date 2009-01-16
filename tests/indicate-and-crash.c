
#include <glib.h>
#include "libindicate/indicator.h"

gboolean crashfunc (gpointer data) { *(int *)data = 5; return FALSE;}

int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateIndicator * indicator = indicate_indicator_new();
	indicate_indicator_show(indicator);

	g_timeout_add_seconds(15, crashfunc, NULL);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
