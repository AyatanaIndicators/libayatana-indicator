#include <gtk/gtk.h>
#include "libindicator/indicator-desktop-shortcuts.h"

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

void
test_desktop_shortcuts_suite (void)
{
	g_test_add_func ("/libindicator/desktopshortcuts/creation",    test_desktop_shortcuts_creation);
	g_test_add_func ("/libindicator/desktopshortcuts/globalnosho", test_desktop_shortcuts_globalnoshow);

	return;
}

int
main (int argc, char ** argv)
{
	g_type_init (); 
	g_test_init (&argc, &argv, NULL);
	gtk_init(&argc, &argv);

	test_desktop_shortcuts_suite();

	g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);

	return g_test_run();
}
