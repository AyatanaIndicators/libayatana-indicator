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
#include "libindicator/indicator-object.h"

void destroy_cb (gpointer data, GObject * object);

void
entry_change_cb (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer data)
{
	gpointer * valuestore = (gpointer *)data;
	*valuestore = entry;
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

	gpointer added_value = NULL, removed_value = NULL, moved_value = NULL;

	g_signal_connect(G_OBJECT(object), INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED,   G_CALLBACK(entry_change_cb), &added_value);
	g_signal_connect(G_OBJECT(object), INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED, G_CALLBACK(entry_change_cb), &removed_value);
	g_signal_connect(G_OBJECT(object), INDICATOR_OBJECT_SIGNAL_ENTRY_MOVED,   G_CALLBACK(entry_move_cb),   &moved_value);

	GList * list = indicator_object_get_entries(object);
	g_assert(list != NULL);
	g_list_free(list);

	while (g_main_context_pending(NULL)) {
		g_main_context_iteration(NULL, TRUE);
	}

	g_assert(GPOINTER_TO_UINT(added_value) == 5);
	g_assert(GPOINTER_TO_UINT(removed_value) == 5);
	g_assert(GPOINTER_TO_UINT(moved_value) == 5);

	g_object_unref(object);

	return;
}

/***
****
***/

static void
visible_entry_added (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer box)
{
	// make a frame for the entry, and add the frame to the box
	GtkWidget * frame = gtk_frame_new (NULL);
	GtkWidget * child = GTK_WIDGET(entry->label);
	g_assert (child != NULL);
	gtk_container_add (GTK_CONTAINER(frame), child);
	gtk_box_pack_start (GTK_BOX(box), frame, FALSE, FALSE, 0);
	g_object_set_data (G_OBJECT(child), "frame-parent", frame);
}

static void
visible_entry_removed (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer box)
{
	// destroy this entry's frame
	gpointer parent = g_object_steal_data (G_OBJECT(entry->label), "frame-parent");
	if (GTK_IS_WIDGET(parent))
		gtk_widget_destroy(GTK_WIDGET(parent));
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
	g_assert(g_list_length(list) == 0);
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

	g_assert(indicator_object_get_location(object, (IndicatorObjectEntry *)entries->data) == 0);
	g_assert(indicator_object_get_location(object, NULL) == 0);

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
	g_test_add_func ("/libindicator/loader/dummy/visible",  test_loader_filename_dummy_visible);

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
