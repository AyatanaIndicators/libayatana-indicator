
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
load_module (const gchar * name, GtkWidget * menu)
{
	g_debug("Looking at Module: %s", name);
	g_return_val_if_fail(name != NULL, FALSE);

	guint suffix_len = strlen(G_MODULE_SUFFIX);
	guint name_len = strlen(name);

	g_return_val_if_fail(name_len > suffix_len, FALSE);

	int i;
	for (i = 0; i < suffix_len; i++) {
		if (name[(name_len - suffix_len) + i] != (G_MODULE_SUFFIX)[i])
			return FALSE;
	}
	g_debug("Loading Module: %s", name);

	gchar * fullpath = g_build_filename(INDICATOR_DIR, name, NULL);
	GModule * module = g_module_open(fullpath,
                                     G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
	g_free(fullpath);
	g_return_val_if_fail(module != NULL, FALSE);

	GtkWidget * (*make_item)(void);
	g_return_val_if_fail(g_module_symbol(module, SYMBOL_NAME, (gpointer *)(&make_item)), FALSE);
	g_return_val_if_fail(make_item != NULL, FALSE);

	GtkWidget * menuitem = make_item();
	g_return_val_if_fail(GTK_MENU_ITEM(menuitem), FALSE);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

	return TRUE;
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
			if (load_module(name, menubar))
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
