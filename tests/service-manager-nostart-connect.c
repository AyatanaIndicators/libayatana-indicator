/*
Test for libayatana-indicator

Copyright 2009 Canonical Ltd.
Copyright 2021 Robert Tari

Authors:
    Ted Gould <ted@canonical.com>
    Robert Tari <robert@tari.in>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 3.0 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License version 3.0 for more details.

You should have received a copy of the GNU General Public
License along with this library. If not, see
<http://www.gnu.org/licenses/>.
*/


#include <glib.h>
#include "indicator-service-manager.h"

static GMainLoop * mainloop = NULL;
static gboolean passed = FALSE;

gboolean
timeout (gpointer data)
{
    passed = FALSE;
    g_error("Timeout with no connection.");
    g_main_loop_quit(mainloop);
    return FALSE;
}

void
connection (IndicatorServiceManager * sm, gboolean connected, gpointer user_data)
{
    static gboolean has_connected = FALSE;

    if (has_connected && connected) {
        g_warning("We got two connected signals.  FAIL.");
        passed = FALSE;
        return;
    }

    if (!connected) {
        g_debug("Not connected");
        return;
    }

    has_connected = TRUE;
    g_debug("Connection");
    passed = TRUE;
    g_main_loop_quit(mainloop);
    return;
}

int
main (int argc, char ** argv)
{
    g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);

    g_usleep(150000);

    IndicatorServiceManager * is = indicator_service_manager_new("org.ayatana.test");
    g_signal_connect(G_OBJECT(is), INDICATOR_SERVICE_MANAGER_SIGNAL_CONNECTION_CHANGE, G_CALLBACK(connection), NULL);

    g_timeout_add_seconds(1, timeout, NULL);

    mainloop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainloop);

    g_object_unref(is);

    g_debug("Quiting");
    if (passed) {
        g_debug("Passed");
        return 0;
    }
    g_debug("Failed");
    return 1;
}
