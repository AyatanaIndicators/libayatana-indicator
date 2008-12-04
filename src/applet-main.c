
#include <panel-applet.h>

#define SYMBOL_NAME  "get_menu_item"
#define ICONS_DIR  (DATADIR G_DIR_SEPARATOR_S "indicator-applet" G_DIR_SEPARATOR_S "icons")

static gboolean     applet_fill_cb (PanelApplet * applet, const gchar * iid, gpointer data);


/*************
 * main
 * ***********/

PANEL_APPLET_BONOBO_FACTORY ("OAFIID:GNOME_IndicatorApplet_Factory",
               PANEL_TYPE_APPLET,
               "indicator-applet", "0",
               applet_fill_cb, NULL);

/*************
 * init function
 * ***********/
static gboolean
load_module (const gchar * name)
{

	return FALSE;
}

static gboolean
applet_fill_cb (PanelApplet * applet, const gchar * iid, gpointer data)
{
	gtk_icon_theme_append_search_path(gtk_icon_theme_get_default(),
	                                  ICONS_DIR);

	int i;
	GtkWidget * menubar = gtk_menu_bar_new();
	gtk_widget_set_name (menubar, "indicator-applet-menubar");
	gtk_container_add(GTK_CONTAINER(applet), menubar);
	gtk_widget_show(menubar);

	int indicators_loaded = 0;

	/* load 'em */
	if (g_file_test(INDICATOR_DIR, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))) {
		GDir * dir = g_dir_open(INDICATOR_DIR, 0, NULL);

		const gchar * name;
		while ((name = g_dir_read_name(dir)) != NULL) {
			if (load_module(name))
				indicators_loaded++;
		}
	}

	if (indicators_loaded == 0) {
		GtkWidget * item = gtk_menu_item_new_with_label("No Indicators");
		gtk_widget_set_sensitive(item, FALSE);
		gtk_menu_shell_append(GTK_MENU_SHELL(menubar), item);
		gtk_widget_show(item);
	}

	gtk_widget_show(GTK_WIDGET(applet));
	return TRUE;
}
