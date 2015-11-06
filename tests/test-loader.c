/*
Test for libindicator

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

#include <gtk/gtk.h>
#include "libayatana-indicator/indicator-object.h"

#include "dummy-indicator-entry-func.h"

void
entry_func_swap (IndicatorObject * io)
{
	static void (*saved_func) (IndicatorObject * io, IndicatorObjectEntry * entry, guint windowid, guint timestamp) = NULL;
	IndicatorObjectClass * klass = INDICATOR_OBJECT_GET_CLASS(io);

	if (saved_func == NULL) {
		saved_func = klass->entry_activate_window;
	}

	if (klass->entry_activate_window == NULL) {
		klass->entry_activate_window = saved_func;
	} else {
		klass->entry_activate_window = NULL;
	}

	return;
}

void
test_loader_entry_func_window (void)
{
	IndicatorObject * object = indicator_object_new_from_file(BUILD_DIR "/.libs/libdummy-indicator-entry-func.so");
	g_assert(object != NULL);

	DummyIndicatorEntryFunc * entryfunc = (DummyIndicatorEntryFunc *)(object);

	entryfunc->entry_activate_called = FALSE;
	entryfunc->entry_activate_window_called = FALSE;
	entryfunc->entry_close_called = FALSE;

	entry_func_swap(object);
	indicator_object_entry_activate_window(object, NULL, 0, 0);
	g_assert(entryfunc->entry_activate_called);

	entry_func_swap(object);
	indicator_object_entry_activate_window(object, NULL, 0, 0);
	g_assert(entryfunc->entry_activate_window_called);

	g_object_unref(object);

	return;
}

void
test_loader_entry_funcs (void)
{
	IndicatorObject * object = indicator_object_new_from_file(BUILD_DIR "/.libs/libdummy-indicator-entry-func.so");
	g_assert(object != NULL);

	DummyIndicatorEntryFunc * entryfunc = (DummyIndicatorEntryFunc *)(object);

	entryfunc->entry_activate_called = FALSE;
	entryfunc->entry_activate_window_called = FALSE;
	entryfunc->entry_close_called = FALSE;

	indicator_object_entry_activate(object, NULL, 0);
	g_assert(entryfunc->entry_activate_called);

	indicator_object_entry_activate_window(object, NULL, 0, 0);
	g_assert(entryfunc->entry_activate_window_called);

	indicator_object_entry_close(object, NULL, 0);
	g_assert(entryfunc->entry_close_called);

	g_object_unref(object);

	return;
}

void destroy_cb (gpointer data, GObject * object);

void
entry_change_cb (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer data)
{
	IndicatorObjectEntry *other_entry = data;
	other_entry->name_hint = entry->name_hint;
	other_entry->parent_object = entry->parent_object;
	return;
}

void
entry_move_cb (IndicatorObject * io, IndicatorObjectEntry * entry, gint old, gint new, gpointer data)
{
	return entry_change_cb(io, entry, data);
}

void
test_loader_filename_dummy_signaler (void)
{
	IndicatorObject * object = indicator_object_new_from_file(BUILD_DIR "/.libs/libdummy-indicator-signaler.so");
	g_assert(object != NULL);

	IndicatorObjectEntry *added_entry, *moved_entry, *removed_entry;
	IndicatorObjectEntry entries[3];

	added_entry = &entries[0];
	moved_entry = &entries[1];
	removed_entry = &entries[2];

	g_signal_connect_after(G_OBJECT(object), INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED,   G_CALLBACK(entry_change_cb), added_entry);
	g_signal_connect_after(G_OBJECT(object), INDICATOR_OBJECT_SIGNAL_ENTRY_MOVED,   G_CALLBACK(entry_move_cb), moved_entry);
	g_signal_connect_after(G_OBJECT(object), INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED, G_CALLBACK(entry_change_cb), removed_entry);

	GList * list = indicator_object_get_entries(object);
	g_assert(list != NULL);
	g_list_free(list);

	while (g_main_context_pending(NULL)) {
		g_main_context_iteration(NULL, TRUE);
	}

	g_assert(g_strcmp0(added_entry->name_hint, "added") == 0);
	g_assert(g_strcmp0(removed_entry->name_hint, "removed") == 0);
	g_assert(g_strcmp0(moved_entry->name_hint, "moved") == 0);

	g_assert(added_entry->parent_object == object);
	g_assert(removed_entry->parent_object == NULL);

	g_object_unref(object);

	return;
}

/***
****
***/

static void
visible_entry_added (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer box)
{
	GtkWidget * child = GTK_WIDGET (entry->label);
	g_assert (child != NULL);

	if (g_object_get_data (G_OBJECT(child), "frame-parent") == NULL)
	{
		GtkWidget * frame = gtk_frame_new (NULL);
		gtk_container_add (GTK_CONTAINER(frame), child);
		gtk_box_pack_start (GTK_BOX(box), frame, FALSE, FALSE, 0);
		g_object_set_data (G_OBJECT(child), "frame-parent", frame);
	}
}

static void
visible_entry_removed (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer box)
{
	GtkWidget * child = GTK_WIDGET (entry->label);
	g_assert (child != NULL);
	g_assert (g_object_get_data (G_OBJECT(child), "frame-parent") != NULL);
}

void
test_loader_filename_dummy_visible (void)
{
	const GQuark is_hidden_quark = g_quark_from_static_string ("is-hidden");
	IndicatorObject * object = indicator_object_new_from_file(BUILD_DIR "/.libs/libdummy-indicator-visible.so");
	g_assert(object != NULL);

	// create our local parent widgetry
#if GTK_CHECK_VERSION(3,0,0)
	GtkWidget * box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
#else
	GtkWidget * box = gtk_hbox_new (TRUE, 0);
#endif
	g_signal_connect(object, INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED,
	                 G_CALLBACK(visible_entry_added), box);
	g_signal_connect(object, INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED,
	                 G_CALLBACK(visible_entry_removed), box);

	// on startup, DummyVisible has one entry and it has a label
	GList * list = indicator_object_get_entries(object);
	g_assert(g_list_length(list) == 1);
	IndicatorObjectEntry * entry = list->data;
	g_assert(entry != NULL);
	g_list_free(list);
	g_assert(GTK_IS_LABEL(entry->label));
	g_assert(entry->parent_object == object);
	g_assert(INDICATOR_IS_OBJECT(entry->parent_object));
	GtkWidget * label = GTK_WIDGET(entry->label);
        g_assert(g_object_get_qdata(G_OBJECT(label), is_hidden_quark) == NULL);

	// add the inital entry to our local parent widgetry
	visible_entry_added (object, entry, box);
	entry = NULL;
	list = gtk_container_get_children (GTK_CONTAINER(box));
	g_assert(g_list_length(list) == 1);
	g_list_free(list);
	
	// hide the entries and confirm that the label survived
	indicator_object_set_visible (object, FALSE);
	while (g_main_context_pending(NULL))
		g_main_context_iteration(NULL, TRUE);
	g_assert(GTK_IS_LABEL(label));
        g_assert(g_object_get_qdata(G_OBJECT(label), is_hidden_quark) != NULL);
	list = gtk_container_get_children (GTK_CONTAINER(box));
	g_assert(g_list_length(list) == 1);
	g_list_free(list);

	// restore the entries and confirm that the label survived
	indicator_object_set_visible (object, TRUE);
	while (g_main_context_pending(NULL))
		g_main_context_iteration(NULL, TRUE);
	g_assert(GTK_IS_LABEL(label));
        g_assert(g_object_get_qdata(G_OBJECT(label), is_hidden_quark) == NULL);
	list = gtk_container_get_children (GTK_CONTAINER(box));
	g_assert(g_list_length(list) == 1);
	g_list_free(list);

	// cleanup
	g_object_unref(object);
	gtk_widget_destroy(box);
}

/***
****
***/

void
test_loader_filename_dummy_simple_location (void)
{
	IndicatorObject * object = indicator_object_new_from_file(BUILD_DIR "/.libs/libdummy-indicator-simple.so");
	g_assert(object != NULL);

	GList * entries = indicator_object_get_entries(object);
	g_assert(entries != NULL);
	g_assert(g_list_length(entries) == 1);

	IndicatorObjectEntry *entry = entries->data;

	g_assert(indicator_object_get_location(object, entry) == 0);
	g_assert(indicator_object_get_location(object, NULL) == 0);
	g_assert(entry->parent_object == object);

	g_object_unref(object);

	return;
}

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
	g_assert(object != NULL);
	g_assert(indicator_object_get_entries(object) == NULL);
	g_object_unref(G_OBJECT(object));
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
	g_test_add_func ("/libindicator/loader/dummy/simple_location", test_loader_filename_dummy_simple_location);
	g_test_add_func ("/libindicator/loader/dummy/signaler",  test_loader_filename_dummy_signaler);
	g_test_add_func ("/libindicator/loader/dummy/entry_funcs",  test_loader_entry_funcs);
	g_test_add_func ("/libindicator/loader/dummy/entry_func_window",  test_loader_entry_func_window);
	g_test_add_func ("/libindicator/loader/dummy/visible",  test_loader_filename_dummy_visible);

	return;
}


int
main (int argc, char ** argv)
{
	g_test_init (&argc, &argv, NULL);
	gtk_init(&argc, &argv);

	test_loader_creation_deletion_suite();

	g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);

	return g_test_run();
}
