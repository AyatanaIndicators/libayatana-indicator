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

#include <gtk/gtk.h>
#include "indicator-desktop-shortcuts.h"

/* Basic object creation and destruction.  Stop big
   f*** ups here. */
void
test_desktop_shortcuts_creation (void)
{

    IndicatorDesktopShortcuts * ids = indicator_desktop_shortcuts_new(SRCDIR "/test-well-formed.desktop", "France");
    g_assert(ids != NULL);

    g_object_add_weak_pointer(G_OBJECT(ids), (gpointer *)&ids);
    g_object_unref(G_OBJECT(ids));

    g_assert(ids == NULL);
    return;
}

/* Tests that the NotShowIn the desktop group is watched
   for */
void
test_desktop_shortcuts_globalnoshow (void)
{

    IndicatorDesktopShortcuts * ids = indicator_desktop_shortcuts_new(SRCDIR "/test-well-formed.desktop", "Germany");
    g_assert(ids != NULL);

    const gchar ** nicks = indicator_desktop_shortcuts_get_nicks(ids);
    g_assert(nicks[0] == NULL);

    g_object_unref(ids);

    return;
}

gboolean
nicks_contains (const gchar ** nicks, const gchar * search)
{
    if (nicks[0] == NULL)
        return FALSE;
    if (g_strcmp0(nicks[0], search) == 0)
        return TRUE;
    return nicks_contains(&nicks[1], search);
}

/* Checking that the local show OnlyIn works. */
void
test_desktop_shortcuts_localfilter (void)
{
    IndicatorDesktopShortcuts * ids = indicator_desktop_shortcuts_new(SRCDIR "/test-well-formed.desktop", "France");
    g_assert(ids != NULL);

    const gchar ** nicks = indicator_desktop_shortcuts_get_nicks(ids);

    g_assert(nicks_contains(nicks, "bob"));
    g_assert(nicks_contains(nicks, "alvin"));
    g_assert(!nicks_contains(nicks, "jim"));

    g_object_unref(ids);

    return;
}

/* Nick names -- checks to see they all have names */
void
test_desktop_shortcuts_nicknames (void)
{
    IndicatorDesktopShortcuts * ids = indicator_desktop_shortcuts_new(SRCDIR "/test-well-formed.desktop", "France");
    g_assert(ids != NULL);

    const gchar ** nicks = indicator_desktop_shortcuts_get_nicks(ids);
    gint i = 0;
    while (nicks[i] != NULL) {
        gchar * expectedstr = g_strdup_printf("%s's shortcut", nicks[i]);
        gchar * name = indicator_desktop_shortcuts_nick_get_name(ids, nicks[i]);
        g_assert(name != NULL);

        gboolean same = (g_strcmp0(expectedstr, name) == 0);

        g_free(name);
        g_free(expectedstr);

        g_assert(same);

        i++;
    }


    g_object_unref(ids);

    return;
}

/* Try executing a shortcut which will touch a file */
void
test_desktop_shortcuts_launch (void)
{
    return;
    IndicatorDesktopShortcuts * ids = indicator_desktop_shortcuts_new(SRCDIR "/test-well-formed.desktop", "TouchTest");
    g_assert(ids != NULL);

    const gchar ** nicks = indicator_desktop_shortcuts_get_nicks(ids);
    g_assert(nicks_contains(nicks, "touch"));

    g_assert(indicator_desktop_shortcuts_nick_exec_with_context(ids, "touch", NULL));
    g_usleep(100000);
    g_assert(g_file_test(BUILD_DIR "/test-desktop-shortcuts-touch-test", G_FILE_TEST_EXISTS));

    g_object_unref(ids);

    return;
}

/* Build our test suite */
void
test_desktop_shortcuts_suite (void)
{
    g_test_add_func ("/libayatana-indicator/desktopshortcuts/creation",    test_desktop_shortcuts_creation);
    g_test_add_func ("/libayatana-indicator/desktopshortcuts/globalnosho", test_desktop_shortcuts_globalnoshow);
    g_test_add_func ("/libayatana-indicator/desktopshortcuts/nicknames",   test_desktop_shortcuts_nicknames);
    g_test_add_func ("/libayatana-indicator/desktopshortcuts/launch",      test_desktop_shortcuts_launch);

    return;
}

int
main (int argc, char ** argv)
{
    g_test_init (&argc, &argv, NULL);
    gtk_init(&argc, &argv);

    test_desktop_shortcuts_suite();

    g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);

    return g_test_run();
}
