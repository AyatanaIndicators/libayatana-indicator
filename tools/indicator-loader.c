/*
A small test loader for loading indicators in test suites
and during development of them.

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
#include <libindicator/indicator-object.h>

#define ENTRY_DATA_NAME "indicator-custom-entry-data"

static void
entry_added (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer user_data)
{
	g_debug("Signal: Entry Added");

	GtkWidget * menuitem = gtk_menu_item_new();
	GtkWidget * hbox = gtk_hbox_new(FALSE, 3);

	if (entry->image != NULL) {
		gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(entry->image), FALSE, FALSE, 0);
	}
	if (entry->label != NULL) {
		gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(entry->label), FALSE, FALSE, 0);
	}
	gtk_container_add(GTK_CONTAINER(menuitem), hbox);
	gtk_widget_show(hbox);

	if (entry->menu != NULL) {
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), GTK_WIDGET(entry->menu));
	}

	gtk_menu_shell_append(GTK_MENU_SHELL(user_data), menuitem);
	gtk_widget_show(menuitem);

	g_object_set_data(G_OBJECT(menuitem), ENTRY_DATA_NAME, entry);

	return;
}

static void
entry_removed_cb (GtkWidget * widget, gpointer userdata)
{
	gpointer data = g_object_get_data(G_OBJECT(widget), ENTRY_DATA_NAME);

	if (data != userdata) {
		return;
	}

	gtk_widget_destroy(widget);
	return;
}

static void 
entry_removed (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer user_data)
{
	g_debug("Signal: Entry Removed");

	gtk_container_foreach(GTK_CONTAINER(user_data), entry_removed_cb, entry);

	return;
}

static gboolean
load_module (const gchar * name, GtkWidget * menu)
{
	g_debug("Looking at Module: %s", name);
	g_return_val_if_fail(name != NULL, FALSE);

	if (!g_str_has_suffix(name, G_MODULE_SUFFIX)) {
		return FALSE;
	}

	g_debug("Loading Module: %s", name);

	/* Build the object for the module */
	IndicatorObject * io = indicator_object_new_from_file(name);

	/* Connect to it's signals */
	g_signal_connect(G_OBJECT(io), INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED,   G_CALLBACK(entry_added),    menu);
	g_signal_connect(G_OBJECT(io), INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED, G_CALLBACK(entry_removed),  menu);

	/* Work on the entries */
	GList * entries = indicator_object_get_entries(io);
	GList * entry = NULL;

	for (entry = entries; entry != NULL; entry = g_list_next(entry)) {
		IndicatorObjectEntry * entrydata = (IndicatorObjectEntry *)entry->data;
		entry_added(io, entrydata, menu);
	}

	g_list_free(entries);

	return TRUE;
}

static void
destroy (gpointer data)
{
	gtk_main_quit();
	return;
}

int
main (int argc, char ** argv)
{
	gtk_init(&argc, &argv);

	if (argc != 2) {
		g_error("Need filename");
		return 1;
	}

	GtkWidget * menubar = gtk_menu_bar_new();
	if (!load_module(argv[1], menubar)) {
		g_error("Unable to load module");
		return 1;
	}

	GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);

	gtk_container_add(GTK_CONTAINER(window), menubar);

	gtk_widget_show(menubar);
	gtk_widget_show(window);

	gtk_main();

	return 0;
}
