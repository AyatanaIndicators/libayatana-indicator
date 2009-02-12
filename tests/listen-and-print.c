/*
A test for libindicate to ensure its quality.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This program is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License version 3, as published 
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <glib.h>
#include "libindicate/listener.h"

static void
show_property_cb (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * property, gchar * propertydata, gpointer data)
{
	g_debug("Indicator Property:       %s %d %s %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), INDICATE_LISTENER_INDICATOR_ID(indicator), property, propertydata);
	return;
}

static void
show_property_time_cb (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * property, GTimeVal * propertydata, gpointer data)
{
	time_t timet;
	struct tm * structtm;

	timet = propertydata->tv_sec;
	structtm = localtime(&timet);

	gchar timestring[80];
	strftime(timestring, 80, "%I:%M", structtm);

	g_debug("Indicator Property:       %s %d %s %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), INDICATE_LISTENER_INDICATOR_ID(indicator), property, timestring);
	return;
}

static void 
show_property_icon_cb (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * property, GdkPixbuf * propertydata, gpointer data)
{
	g_debug("Indicator Property:       %s %d %s %dx%d", INDICATE_LISTENER_SERVER_DBUS_NAME(server), INDICATE_LISTENER_INDICATOR_ID(indicator), property, gdk_pixbuf_get_width(propertydata), gdk_pixbuf_get_height(propertydata));
	g_object_unref(G_OBJECT(propertydata));
	return;
}

static void
show_property (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * property)
{
	if (!strcmp(property, "icon")) {
		indicate_listener_get_property_icon(listener, server, indicator, property, show_property_icon_cb, NULL);
	} else if (!strcmp(property, "time")) {
		indicate_listener_get_property_time(listener, server, indicator, property, show_property_time_cb, NULL);
	} else {
		indicate_listener_get_property(listener, server, indicator, property, show_property_cb, NULL);
	}

	return;
}

static void
get_properties (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator)
{
	//TODO: Not in API yet.
}

static void
indicator_added (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type, gpointer data)
{
	g_debug("Indicator Added:          %s %d %s", INDICATE_LISTENER_SERVER_DBUS_NAME(server), INDICATE_LISTENER_INDICATOR_ID(indicator), type);
	get_properties(listener, server, indicator);
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
	show_property(listener, server, indicator, property);
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

	IndicateListener * listener = indicate_listener_ref_default();

	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_INDICATOR_ADDED, G_CALLBACK(indicator_added), NULL);
	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_INDICATOR_REMOVED, G_CALLBACK(indicator_removed), NULL);
	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_INDICATOR_MODIFIED, G_CALLBACK(indicator_modified), NULL);
	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_SERVER_ADDED, G_CALLBACK(server_added), NULL);
	g_signal_connect(listener, INDICATE_LISTENER_SIGNAL_SERVER_REMOVED, G_CALLBACK(server_removed), NULL);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
