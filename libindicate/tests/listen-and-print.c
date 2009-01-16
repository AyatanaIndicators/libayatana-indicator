
#include <glib.h>
#include "libindicate/listener.h"

static void
indicator_added (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type, gpointer data)
{
	g_debug("Indicator Added:          %s %d %s", (gchar *)server, (guint)indicator, type);
}

static void
indicator_removed (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type, gpointer data)
{
	g_debug("Indicator Removed:        %s %d %s", (gchar *)server, (guint)indicator, type);
}

static void
indicator_modified (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type, gchar * property, gpointer data)
{
	g_debug("Indicator Modified:       %s %d %s %s", (gchar *)server, (guint)indicator, type, property);
}

static void
server_added (IndicateListener * listener, IndicateListenerServer * server, gpointer data)
{
	g_debug("Indicator Server Added:   %s", (gchar *)server);
}

static void
server_removed (IndicateListener * listener, IndicateListenerServer * server, gpointer data)
{
	g_debug("Indicator Server Removed: %s", (gchar *)server);
}

int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateListener * listener = indicate_listener_new();

	g_signal_connect(listener, "indicator-added", G_CALLBACK(indicator_added), NULL);
	g_signal_connect(listener, "indicator-removed", G_CALLBACK(indicator_removed), NULL);
	g_signal_connect(listener, "indicator-modified", G_CALLBACK(indicator_modified), NULL);
	g_signal_connect(listener, "server-added", G_CALLBACK(server_added), NULL);
	g_signal_connect(listener, "server-removed", G_CALLBACK(server_removed), NULL);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
