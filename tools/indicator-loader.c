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

static GHashTable * entry_to_menuitem = NULL;

#define ENTRY_DATA_NAME "indicator-custom-entry-data"

static void
activate_entry (GtkWidget * widget, gpointer user_data)
{
	g_return_if_fail(INDICATOR_IS_OBJECT(user_data));
	gpointer entry = g_object_get_data(G_OBJECT(widget), ENTRY_DATA_NAME);
	if (entry == NULL) {
		g_debug("Activation on: (null)");
	}

	indicator_object_entry_activate(INDICATOR_OBJECT(user_data), (IndicatorObjectEntry *)entry, gtk_get_current_event_time());
	return;
}

static GtkWidget*
create_menu_item (IndicatorObjectEntry * entry)
{
	GtkWidget * hbox;
	GtkWidget * menuitem;

	menuitem = gtk_menu_item_new();

#if GTK_CHECK_VERSION(3,0,0)
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
#else
	hbox = gtk_hbox_new(FALSE, 3);
#endif

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

	return menuitem;
}

static void
entry_added (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer user_data)
{
	GtkWidget * menuitem;

	g_debug("Signal: Entry Added");

	if (entry->parent_object == NULL) {
		g_warning("Entry '%p' does not have a parent object", entry);
	}

	menuitem = g_hash_table_lookup (entry_to_menuitem, entry);
	if (menuitem == NULL) {
		g_debug ("This is the first time this entry's been added -- creating a new menuitem for it");
		menuitem = create_menu_item (entry);
		g_hash_table_insert (entry_to_menuitem, entry, menuitem);

		g_object_set_data(G_OBJECT(menuitem), ENTRY_DATA_NAME, entry);
		g_signal_connect (G_OBJECT(menuitem), "activate", G_CALLBACK(activate_entry), io);

		gtk_menu_shell_append (GTK_MENU_SHELL(user_data), menuitem);

	}
	gtk_widget_show (menuitem);

	return;
}

static void 
entry_removed (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer user_data)
{
	g_debug("Signal: Entry Removed");

	GtkWidget * menuitem = g_hash_table_lookup (entry_to_menuitem, entry);
	if (menuitem != NULL)
		gtk_widget_hide (menuitem);

	return;
}

static void
menu_show (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp, gpointer user_data)
{
	if (entry != NULL) {
		g_debug("Show Menu: %s", entry->label != NULL ? gtk_label_get_text(entry->label) : "No Label");
	} else {
		g_debug("Show Menu: (null)");
	}
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
	g_signal_connect(G_OBJECT(io), INDICATOR_OBJECT_SIGNAL_MENU_SHOW,     G_CALLBACK(menu_show),      NULL);

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
	/* Make sure we don't proxy to ourselves */
	g_unsetenv("UBUNTU_MENUPROXY");

	gtk_init(&argc, &argv);

	entry_to_menuitem = g_hash_table_new (g_direct_hash, g_direct_equal);

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

	g_hash_table_destroy (entry_to_menuitem);
	return 0;
}
