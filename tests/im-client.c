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
#include "libindicate/server.h"
#include "libindicate/indicator-message.h"

gchar * patha = "/usr/share/icons/hicolor/16x16/apps/empathy.png";
gchar * pathb = "/usr/share/icons/hicolor/22x22/apps/empathy.png";
gchar * lastpath = NULL;

static gboolean
timeout_cb (gpointer data)
{
	g_debug("Modifying properties");

	IndicateIndicator * indicator = INDICATE_INDICATOR(data);

	GTimeVal time;
	g_get_current_time(&time);
	indicate_indicator_set_property_time(INDICATE_INDICATOR(indicator), "time", &time);

	if (lastpath == patha) {
		lastpath = pathb;
	} else {
		lastpath = patha;
	}
	
	GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(lastpath, NULL);
	g_return_val_if_fail(pixbuf != NULL, TRUE);

	indicate_indicator_set_property_icon(INDICATE_INDICATOR(indicator), "icon", pixbuf);
	g_object_unref(G_OBJECT(pixbuf));

	return TRUE;
}

static void
display (IndicateIndicator * indicator, gpointer data)
{
	g_debug("Ah, my indicator has been displayed");
}

static void
server_display (IndicateServer * server, gpointer data)
{
	g_debug("Ah, my server has been displayed");
}

static void
interest_added (IndicateServer * server, IndicateInterests interest)
{
	g_debug("Oh, someone is interested in my for: %d", interest);
}

void
interest_removed (IndicateServer * server, IndicateInterests interest)
{
	g_debug("Someone is no longer interested in my for: %d", interest);
}

int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateServer * server = indicate_server_ref_default();
	indicate_server_set_type(server, "message.im");
	indicate_server_set_desktop_file(server, "/usr/share/applications/empathy.desktop");
	g_signal_connect(G_OBJECT(server), INDICATE_SERVER_SIGNAL_SERVER_DISPLAY, G_CALLBACK(server_display), NULL);
	g_signal_connect(G_OBJECT(server), INDICATE_SERVER_SIGNAL_INTEREST_ADDED, G_CALLBACK(interest_added), NULL);
	g_signal_connect(G_OBJECT(server), INDICATE_SERVER_SIGNAL_INTEREST_REMOVED, G_CALLBACK(interest_removed), NULL);

	IndicateIndicatorMessage * indicator;

	indicator = indicate_indicator_message_new();
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "subtype", "im");
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "sender", "IM Client Test");
	GTimeVal time; g_get_current_time(&time);
	indicate_indicator_set_property_time(INDICATE_INDICATOR(indicator), "time", &time);
	indicate_indicator_show(INDICATE_INDICATOR(indicator));

	g_get_current_time(&time);
	indicate_indicator_set_property_time(INDICATE_INDICATOR(indicator), "time", &time);

	g_signal_connect(G_OBJECT(indicator), INDICATE_INDICATOR_SIGNAL_DISPLAY, G_CALLBACK(display), NULL);

	g_timeout_add_seconds(180, timeout_cb, indicator);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
