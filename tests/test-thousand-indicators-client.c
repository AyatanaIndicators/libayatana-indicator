
#include <glib.h>
#include "libindicate/indicator.h"

static gboolean passed = TRUE;
static GMainLoop * mainloop = NULL;

static gboolean
done_timeout_cb (gpointer data)
{
	g_debug("All done.");
	g_main_loop_quit(mainloop);
	return FALSE;
}

int
main (int argc, char * argv)
{
	g_type_init();

	int i;
	for (i = 0; i < 1000; i++) {
		/* Memory leak :) */
		IndicateIndicator * indicator = indicate_indicator_new();
		indicate_indicator_show(indicator);
	}

	g_timeout_add_seconds(2, done_timeout_cb, NULL);

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);

	return !passed;
}
