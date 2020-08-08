/*
Test for libayatana-indicator

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

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
#include "libayatana-indicator/indicator-service.h"
#include "service-version-values.h"

static GMainLoop * mainloop = NULL;
static gboolean passed = FALSE;

gboolean
timeout (gpointer data)
{
    passed = FALSE;
    g_debug("Timeout with no shutdown.");
    g_main_loop_quit(mainloop);
    return FALSE;
}

void
shutdown (void)
{
    g_debug("Shutdown");
    passed = TRUE;
    g_main_loop_quit(mainloop);
    return;
}

int
main (int argc, char ** argv)
{
    IndicatorService * is = indicator_service_new_version("org.ayatana.version.bad", SERVICE_VERSION_BAD);
    g_signal_connect(G_OBJECT(is), INDICATOR_SERVICE_SIGNAL_SHUTDOWN, shutdown, NULL);

    g_timeout_add_seconds(1, timeout, NULL);

    mainloop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainloop);

    g_debug("Quiting");
    if (passed) {
        g_debug("Passed");
        return 0;
    }
    g_debug("Failed");
    return 1;
}
