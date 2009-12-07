
#include <glib.h>
#include "libindicator/indicator-service.h"
#include "service-version-values.h"

static GMainLoop * mainloop = NULL;
static gboolean passed = FALSE;

gboolean
timeout (gpointer data)
{
	passed = FALSE;
	g_debug("Timeout with no shutdown.");
	g_main_loop_quit(mainloop);
	return FALSE;
}

void
shutdown (void)
{
	g_debug("Shutdown");
	passed = TRUE;
	g_main_loop_quit(mainloop);
	return;
}

int
main (int argc, char ** argv)
{
	g_type_init();

	IndicatorService * is = indicator_service_new_version("org.ayatana.version.good", SERVICE_VERSION_GOOD);
	g_signal_connect(G_OBJECT(is), INDICATOR_SERVICE_SIGNAL_SHUTDOWN, shutdown, NULL);

	g_timeout_add_seconds(1, timeout, NULL);

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);

	g_debug("Quiting");
	if (passed) {
		g_debug("Passed");
		return 0;
	}
	g_debug("Failed");
	return 1;
}
