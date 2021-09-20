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
#include "service-version-values.h"

static GMainLoop * mainloop = NULL;
static gboolean con_good = FALSE;
static gboolean con_bad = FALSE;

gboolean
timeout (gpointer data)
{
    g_debug("Timeout.");
    g_main_loop_quit(mainloop);
    return FALSE;
}

void
connection_bad (IndicatorServiceManager * sm, gboolean connected, gpointer user_data)
{
    if (!connected) return;
    g_debug("Connection From Bad!");
    con_bad = TRUE;
    return;
}

void
connection_good (IndicatorServiceManager * sm, gboolean connected, gpointer user_data)
{
    if (!connected) return;
    g_debug("Connection From Good.");
    con_good = TRUE;
    return;
}

int
main (int argc, char ** argv)
{
    g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);
    g_print("Manager: DBUS_SESSION_BUS_ADDRESS = %s\n", g_getenv("DBUS_SESSION_BUS_ADDRESS"));

    IndicatorServiceManager * goodis = indicator_service_manager_new_version("org.ayatana.version.good", SERVICE_VERSION_GOOD);
    g_signal_connect(G_OBJECT(goodis), INDICATOR_SERVICE_MANAGER_SIGNAL_CONNECTION_CHANGE, G_CALLBACK(connection_good), NULL);

    IndicatorServiceManager * badis = indicator_service_manager_new_version("org.ayatana.version.bad", SERVICE_VERSION_GOOD);
    g_signal_connect(G_OBJECT(badis), INDICATOR_SERVICE_MANAGER_SIGNAL_CONNECTION_CHANGE, G_CALLBACK(connection_bad), NULL);

    g_timeout_add_seconds(1, timeout, NULL);

    mainloop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainloop);

    g_object_unref(goodis);
    g_object_unref(badis);

    g_debug("Quiting");
    if (con_good && !con_bad) {
        g_debug("Passed");
        return 0;
    }
    g_debug("Failed");
    return 1;
}
