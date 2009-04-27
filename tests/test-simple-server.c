
#include <glib.h>
#include "libindicate/listener.h"

static gboolean passed = TRUE;
static GMainLoop * mainloop = NULL;

static void
server_added (IndicateListener * listener, IndicateListenerServer * server, gchar * type, gpointer data)
{
	g_debug("Indicator Server Added:   %s %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), type);
	g_main_loop_quit(mainloop);
}

static gboolean
failed_cb (gpointer data)
{
	g_debug("Failed to get a server in 5 seconds.");
	passed = FALSE;
	g_main_loop_quit(mainloop);
	return FALSE;
}

int
main (int argc, char * argv)
{
	g_type_init();

	IndicateListener * listener = indicate_listener_ref_default();

	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_SERVER_ADDED, G_CALLBACK(server_added), NULL);

	g_timeout_add_seconds(5, failed_cb, NULL);

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);

	return !passed;
}
