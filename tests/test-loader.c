#include <gtk/gtk.h>
#include "libindicator/indicator-object.h"

void destroy_cb (gpointer data, GObject * object);

void
test_loader_filename_dummy_simple_accessors (void)
{
	IndicatorObject * object = indicator_object_new_from_file(BUILD_DIR "/.libs/libdummy-indicator-simple.so");
	g_assert(object != NULL);

	g_assert(indicator_object_get_entries(object) != NULL);

	g_object_unref(object);

	return;
}

void
test_loader_filename_dummy_simple (void)
{
	IndicatorObject * object = indicator_object_new_from_file(BUILD_DIR "/.libs/libdummy-indicator-simple.so");
	g_assert(object != NULL);

	gboolean unreffed = FALSE;
	g_object_weak_ref(G_OBJECT(object), destroy_cb, &unreffed);

	g_object_unref(object);
	g_assert(unreffed == TRUE);

	return;
}

void
test_loader_filename_dummy_blank (void)
{
	IndicatorObject * object = indicator_object_new_from_file(BUILD_DIR "/.libs/libdummy-indicator-blank.so");
	g_assert(object == NULL);
	return;
}

void
test_loader_filename_dummy_null (void)
{
	IndicatorObject * object = indicator_object_new_from_file(BUILD_DIR "/.libs/libdummy-indicator-null.so");
	g_assert(object == NULL);
	return;
}

void
test_loader_filename_bad (void)
{
	IndicatorObject * object = indicator_object_new_from_file("/this/file/should/not/exist.so");
	g_assert(object == NULL);
	return;
}

void
destroy_cb (gpointer data, GObject * object)
{
	gboolean * bob = (gboolean *)data;
	*bob = TRUE;
	return;
}

void
test_loader_refunref (void)
{
	GObject * object = g_object_new(INDICATOR_OBJECT_TYPE, NULL);

	gboolean unreffed = FALSE;
	g_object_weak_ref(object, destroy_cb, &unreffed);

	g_object_unref(object);

	g_assert(unreffed == TRUE);

	return;
}

void
test_loader_creation_deletion_suite (void)
{
	g_test_add_func ("/libindicator/loader/ref_and_unref", test_loader_refunref);
	g_test_add_func ("/libindicator/loader/filename_bad",  test_loader_filename_bad);
	g_test_add_func ("/libindicator/loader/dummy/null_load",  test_loader_filename_dummy_null);
	g_test_add_func ("/libindicator/loader/dummy/blank_load",  test_loader_filename_dummy_null);
	g_test_add_func ("/libindicator/loader/dummy/simple_load",  test_loader_filename_dummy_simple);
	g_test_add_func ("/libindicator/loader/dummy/simple_accessors", test_loader_filename_dummy_simple_accessors);

	return;
}


int
main (int argc, char ** argv)
{
	g_type_init (); 
	g_test_init (&argc, &argv, NULL);
	gtk_init(&argc, &argv);

	test_loader_creation_deletion_suite();

	g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);

	return g_test_run();
}
