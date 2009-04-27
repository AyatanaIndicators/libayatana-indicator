
#include <glib.h>
#include "libindicate/listener.h"

static gboolean passed = TRUE;
static GMainLoop * mainloop = NULL;

static void
server_added (IndicateListener * listener, IndicateListenerServer * server, gchar * type, gpointer data)
{
	g_debug("Indicator Server Added:   %s %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), type);
	#define INTEREST 2
	g_debug("Setting Interest: %d", INTEREST);
	indicate_listener_server_show_interest(listener, server, INTEREST);
	return;
}

static gboolean
failed_cb (gpointer data)
{
	g_debug("Done indicatating interest");
	g_main_loop_quit(mainloop);
	return FALSE;
}

int
main (int argc, char * argv)
{
	g_type_init();

	IndicateListener * listener = indicate_listener_ref_default();

	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_SERVER_ADDED, G_CALLBACK(server_added), NULL);

	g_timeout_add_seconds(2, failed_cb, NULL);

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);

	return !passed;
}
