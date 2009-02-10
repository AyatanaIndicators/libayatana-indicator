
#include <glib.h>
#include "libindicate/listener.h"

static void
indicator_added (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type, gpointer data)
{
	g_debug("Indicator Added:          %s %d %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), INDICATE_LISTENER_INDICATOR_ID(indicator), type);
}

static void
indicator_removed (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type, gpointer data)
{
	g_debug("Indicator Removed:        %s %d %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), INDICATE_LISTENER_INDICATOR_ID(indicator), type);
}

static void
indicator_modified (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type, gchar * property, gpointer data)
{
	g_debug("Indicator Modified:       %s %d %s %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), INDICATE_LISTENER_INDICATOR_ID(indicator), type, property);
}

static void
server_added (IndicateListener * listener, IndicateListenerServer * server, gchar * type, gpointer data)
{
	g_debug("Indicator Server Added:   %s %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), type);
}

static void
server_removed (IndicateListener * listener, IndicateListenerServer * server, gchar * type, gpointer data)
{
	g_debug("Indicator Server Removed: %s %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), type);
}

int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateListener * listener = indicate_listener_new();

	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_INDICATOR_ADDED, G_CALLBACK(indicator_added), NULL);
	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_INDICATOR_REMOVED, G_CALLBACK(indicator_removed), NULL);
	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_INDICATOR_MODIFIED, G_CALLBACK(indicator_modified), NULL);
	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_SERVER_ADDED, G_CALLBACK(server_added), NULL);
	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_SERVER_REMOVED, G_CALLBACK(server_removed), NULL);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
