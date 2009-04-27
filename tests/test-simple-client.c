
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

	IndicateIndicator * indicator = indicate_indicator_new();
	indicate_indicator_show(indicator);

	g_timeout_add_seconds(2, done_timeout_cb, indicator);

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);

	return !passed;
}
