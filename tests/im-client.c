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

static void
display (IndicateIndicator * indicator, gpointer data)
{
	g_debug("Ah, I've been displayed");
}

int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateServer * server = indicate_server_ref_default();
	GValue value = {0};
	g_value_init(&value, G_TYPE_STRING);
	g_value_set_static_string(&value, "message.im");
	g_object_set_property(G_OBJECT(server), "type", &value);

	IndicateIndicatorMessage * indicator;

	indicator = indicate_indicator_message_new();
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "subtype", "im");
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "sender", "Ted Gould");
	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "time", "11:11");
	indicate_indicator_show(INDICATE_INDICATOR(indicator));

	indicate_indicator_set_property(INDICATE_INDICATOR(indicator), "time", "11:12");

	g_signal_connect(G_OBJECT(indicator), INDICATE_INDICATOR_SIGNAL_DISPLAY, G_CALLBACK(display), NULL);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
