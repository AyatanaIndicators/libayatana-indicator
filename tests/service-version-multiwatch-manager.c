
#include <glib.h>
#include "libindicator/indicator-service-manager.h"
#include "service-version-values.h"

static GMainLoop * mainloop = NULL;
static gboolean passed = FALSE;
static IndicatorServiceManager * goodis = NULL;

gboolean
timeout (gpointer data)
{
	g_debug("Timeout.");
	passed = FALSE;
	g_main_loop_quit(mainloop);
	return FALSE;
}

void
connection_good (IndicatorServiceManager * sm, gboolean connected, gpointer user_data)
{
	if (!connected) return;
	g_debug("Connection From Service.");
	passed = TRUE;
	g_main_loop_quit(mainloop);
	return;
}

gboolean
delay_start (gpointer data)
{
	goodis = indicator_service_manager_new_version("org.ayatana.version.good", SERVICE_VERSION_GOOD);
	g_signal_connect(G_OBJECT(goodis), INDICATOR_SERVICE_MANAGER_SIGNAL_CONNECTION_CHANGE, G_CALLBACK(connection_good), NULL);

	g_timeout_add_seconds(1, timeout, NULL);

	return FALSE;
}

int
main (int argc, char ** argv)
{
	g_type_init();
	g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);
	g_print("Manager: DBUS_SESSION_BUS_ADDRESS = %s\n", g_getenv("DBUS_SESSION_BUS_ADDRESS"));

	g_timeout_add(500, delay_start, NULL);

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);

	g_object_unref(goodis);

	g_debug("Quiting");
	if (passed) {
		g_debug("Passed");
		return 0;
	}
	g_debug("Failed");
	return 1;
}
