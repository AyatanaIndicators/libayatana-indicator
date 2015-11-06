/*
 * A small test loader for loading indicators in test suites
 * and during development of them.
 *
 * Copyright 2009 Canonical Ltd.
 *
 * Authors:
 *   Ted Gould <ted@canonical.com>
 *   Lars Uebernickel <lars.uebernickel@canonical.com>
 *   Charles Kerr <charles.kerr@canonical.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 3.0 for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <libayatana-ido/libayatana-ido.h>
#include <libayatana-indicator/indicator-object.h>
#if GTK_CHECK_VERSION (3,0,0)
 #include <libayatana-indicator/indicator-ng.h>
#endif

static GHashTable * entry_to_menu_item = NULL;

G_DEFINE_QUARK (indicator_loader, entry_data)

static void
activate_entry (GtkWidget * widget, gpointer user_data)
{
  gpointer entry;

  g_return_if_fail (INDICATOR_IS_OBJECT(user_data));

  entry = g_object_get_qdata (G_OBJECT(widget), entry_data_quark());

  if (entry == NULL)
    {
      g_debug("Activation on: (null)");
    }
  else
    {
      indicator_object_entry_activate (INDICATOR_OBJECT(user_data),
                                       entry,
                                       gtk_get_current_event_time());
    }
}

static void
scroll_entry (GtkWidget *widget, GdkEventScroll* event, gpointer user_data)
{
  gpointer entry;

  g_return_if_fail (INDICATOR_IS_OBJECT(user_data));

  entry = g_object_get_qdata (G_OBJECT(widget), entry_data_quark());
  IndicatorScrollDirection direction = G_MAXINT;

  switch (event->direction)
    {
      case GDK_SCROLL_UP:
        direction = INDICATOR_OBJECT_SCROLL_UP;
        break;
      case GDK_SCROLL_DOWN:
        direction = INDICATOR_OBJECT_SCROLL_DOWN;
        break;
      case GDK_SCROLL_LEFT:
        direction = INDICATOR_OBJECT_SCROLL_LEFT;
        break;
      case GDK_SCROLL_RIGHT:
        direction = INDICATOR_OBJECT_SCROLL_RIGHT;
        break;
      default:
        break;
    }

  if (entry == NULL)
    {
      g_debug("Scroll on: (null)");
    }
  else if (direction == G_MAXINT)
    {
      g_debug("Scroll direction not supported");
    }
  else
    {
      g_signal_emit_by_name(INDICATOR_OBJECT(user_data),
                            INDICATOR_OBJECT_SIGNAL_ENTRY_SCROLLED,
                            entry, 1, direction);
    }
}

static GtkWidget*
create_menu_item (IndicatorObjectEntry * entry)
{
  GtkWidget * menu_item;
  GtkWidget * hbox;
  gpointer w;

  menu_item = gtk_menu_item_new();

#if GTK_CHECK_VERSION (3,0,0)
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
#else
  hbox = gtk_hbox_new (FALSE, 3);
#endif

  if ((w = entry->image))
    gtk_box_pack_start (GTK_BOX (hbox), GTK_WIDGET(w), FALSE, FALSE, 0);
  if ((w = entry->label))
    gtk_box_pack_start (GTK_BOX (hbox), GTK_WIDGET(w), FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER(menu_item), hbox);
  gtk_widget_show (hbox);

  if ((w = entry->menu))
    gtk_menu_item_set_submenu (GTK_MENU_ITEM(menu_item), GTK_WIDGET(w));

  return menu_item;
}

static void
entry_added (IndicatorObject      * io,
             IndicatorObjectEntry * entry,
             gpointer               user_data)
{
  GtkWidget * menu_item;

  g_debug ("Signal: Entry Added");

  g_warn_if_fail (entry->parent_object != NULL);

  menu_item = g_hash_table_lookup (entry_to_menu_item, entry);

  if (menu_item == NULL)
    {
      g_debug ("creating a menuitem for new entry %p", entry);
      menu_item = create_menu_item (entry);
      g_hash_table_insert (entry_to_menu_item, entry, menu_item);

      g_object_set_qdata (G_OBJECT(menu_item), entry_data_quark(), entry);
      g_signal_connect (menu_item, "activate", G_CALLBACK(activate_entry), io);

      gtk_widget_set_events (menu_item, gtk_widget_get_events (menu_item) | GDK_SCROLL_MASK);
      g_signal_connect (menu_item, "scroll-event", G_CALLBACK(scroll_entry), io);

      gtk_menu_shell_append (GTK_MENU_SHELL(user_data), menu_item);
    }

  gtk_widget_show (menu_item);
}

static void 
entry_removed (IndicatorObject      * io,
               IndicatorObjectEntry * entry,
               gpointer               user_data)
{
  GtkWidget * w;

  g_debug ("Signal: Entry Removed");

  if ((w = g_hash_table_lookup (entry_to_menu_item, entry)))
    gtk_widget_hide (w);
}

static void
menu_show (IndicatorObject      * io,
           IndicatorObjectEntry * entry,
           guint                  timestamp,
           gpointer               user_data)
{
  const char * text;

  if (entry == NULL)
    text = "(null)";
  else if (entry->label == NULL)
    text = "(no label)";
  else
    text = gtk_label_get_text (entry->label);

  g_debug ("Show Menu: %s", text);
}

/***
****
***/

static IndicatorObject *
load_module (const gchar * file_name)
{
  IndicatorObject * io = NULL;

  if (file_name && g_str_has_suffix (file_name, G_MODULE_SUFFIX))
    {
      io = indicator_object_new_from_file (file_name);

      if (io == NULL)
        g_warning ("could not load indicator from '%s'", file_name);
    }

  return io;
}

static IndicatorObject *
load_profile (const char * file_name, const char * profile)
{
  IndicatorObject * io = NULL;

#if GTK_CHECK_VERSION (3,0,0)

  GError * error = NULL;

  io = INDICATOR_OBJECT (indicator_ng_new_for_profile (file_name,
                                                       profile,
                                                       &error));
  if (error != NULL)
    {
      g_warning ("couldn't load profile '%s' from '%s': %s",
                 profile, file_name, error->message);
      g_error_free (error);
    }

#endif

  return io;
}

/***
****
***/

static void
add_indicator_to_menu (GtkMenuShell * menu_shell, IndicatorObject * io)
{
  GList * entries;
  GList * entry;

  g_return_if_fail (INDICATOR_IS_OBJECT (io));

  /* connect to its signals */
  g_signal_connect (io, INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED,
                    G_CALLBACK(entry_added), menu_shell);
  g_signal_connect (io, INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED,
                    G_CALLBACK(entry_removed),  menu_shell);
  g_signal_connect (io, INDICATOR_OBJECT_SIGNAL_MENU_SHOW,
                    G_CALLBACK(menu_show), NULL);

  /* process the entries */
  entries = indicator_object_get_entries(io);
  for (entry=entries; entry!=NULL; entry=entry->next)
    entry_added (io, entry->data, menu_shell);
  g_list_free (entries);
}

static void
add_menu_to_grid (GtkGrid    * grid,
                  int          top,
                  const char * text_,
                  GtkWidget  * menu)
{
  gchar * text;
  GtkWidget * label;

  text = g_strdup_printf ("%s:", text_);
  label = gtk_label_new (text);
  g_free (text);

  gtk_grid_attach (GTK_GRID(grid), label, 0, top, 1, 1);
  gtk_grid_attach (GTK_GRID(grid), menu,  1, top, 1, 1);

  g_object_set (label, "halign", GTK_ALIGN_START,
                       "hexpand", FALSE,
                       "margin-right", 6,
                       "valign", GTK_ALIGN_CENTER,
                       NULL);

  g_object_set (menu, "halign", GTK_ALIGN_START,
                      "hexpand", TRUE,
                      NULL);
}

/***
****
***/

int
main (int argc, char ** argv)
{
  int menu_count = 0;
  const gchar * file_name;
  gchar * base_name;
  GtkWidget * grid;

  if (argc != 2)
    {
      base_name = g_path_get_basename (argv[0]);
      g_warning ("Use: %s filename", base_name);
      g_free (base_name);
      return 0;
    }

  /* make sure we don't proxy to ourselves */
  g_setenv ("UBUNTU_MENUPROXY", "0", TRUE);

  gtk_init (&argc, &argv);
  ido_init ();

  entry_to_menu_item = g_hash_table_new (g_direct_hash, g_direct_equal);

  file_name = argv[1];

  grid = g_object_new (GTK_TYPE_GRID, "margin", 4,
                                      "column-spacing", 6,
                                      "row-spacing", 12,
                                      NULL);

  /* if it's an old-style indicator... */
  if (g_str_has_suffix (file_name, G_MODULE_SUFFIX))
    {
      IndicatorObject * io = load_module (file_name);
      GtkWidget * menu = gtk_menu_bar_new ();
      add_indicator_to_menu (GTK_MENU_SHELL(menu), io);
      base_name = g_path_get_basename (file_name);
      add_menu_to_grid (GTK_GRID(grid), menu_count++, base_name, menu);
      g_free (base_name);
    }
  else /* treat it as a GMenu indicator's keyfile */
    {
      GError * error;
      GKeyFile * key_file;

      key_file = g_key_file_new ();
      error = NULL;
      g_key_file_load_from_file (key_file, file_name, G_KEY_FILE_NONE, &error);
      if (error != NULL)
        {
          g_warning ("loading '%s' failed: %s", file_name, error->message);
          g_error_free (error);
        }
      else
        {
          gchar ** groups;
          int i;

          groups = g_key_file_get_groups (key_file, NULL);
          for (i=0; groups && groups[i]; i++)
            {
              const gchar * const profile = groups[i];
              IndicatorObject * io;

              if (!g_strcmp0 (profile, "Indicator Service"))
                continue;

              if ((io = load_profile (file_name, profile)))
                {
                  GtkWidget * menu = gtk_menu_bar_new ();
                  add_indicator_to_menu (GTK_MENU_SHELL(menu), io);
                  add_menu_to_grid (GTK_GRID(grid), menu_count++, profile, menu);
                }
            }

          g_strfreev (groups);
        }

      g_key_file_free (key_file);
    }

  if (menu_count > 0)
    {
      GtkWidget * window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      base_name = g_path_get_basename (file_name);
      gtk_window_set_title (GTK_WINDOW(window), base_name);
      g_free (base_name);
      g_signal_connect (window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
      gtk_container_add (GTK_CONTAINER(window), grid);
      gtk_widget_show_all (window);
      gtk_main ();
    }

  /* cleanup */
  g_hash_table_destroy (entry_to_menu_item);
  return 0;
}
