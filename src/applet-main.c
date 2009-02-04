
#include <panel-applet.h>

#define SYMBOL_NAME  "get_menu_item"
#define ICONS_DIR  (DATADIR G_DIR_SEPARATOR_S "indicator-applet" G_DIR_SEPARATOR_S "icons")

static gboolean     applet_fill_cb (PanelApplet * applet, const gchar * iid, gpointer data);


static void cw_panel_background_changed (PanelApplet               *applet,
                                         PanelAppletBackgroundType  type,
                        				         GdkColor                  *colour,
                        				         GdkPixmap                 *pixmap,
                                         GtkWidget                 *menubar);

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
	GtkWidget *menubar;
	gint i;
	gint indicators_loaded = 0;
  
	/* Set panel options */
	gtk_container_set_border_width(GTK_CONTAINER (applet), 0);
	panel_applet_set_flags(applet, PANEL_APPLET_EXPAND_MINOR);	
  
	/* Init some theme/icon stuff */
	gtk_icon_theme_append_search_path(gtk_icon_theme_get_default(),
	                                  ICONS_DIR);
	gtk_rc_parse_string (
		"style \"panel-menubar-style\"\n"
		"{\n"
		" GtkMenuBar::shadow-type = none\n"
		" GtkMenuBar::internal-padding = 0\n"
		"}\n"
		"widget \"*indicator-applet-menubar*\" style : highest \"panel-menubar-style\""); 	
  
	/* Build menubar */
	menubar = gtk_menu_bar_new();
	gtk_widget_set_name(GTK_WIDGET (applet), "indicator-applet-menubar");//leave
	gtk_container_add(GTK_CONTAINER(applet), menubar);
	gtk_widget_show(menubar);

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
  
	/* Background of applet */
	g_signal_connect(applet, "change-background",
			  G_CALLBACK(cw_panel_background_changed), menubar);
  
	gtk_widget_show(GTK_WIDGET(applet));

	return TRUE;
}

static void 
cw_panel_background_changed (PanelApplet               *applet,
                             PanelAppletBackgroundType  type,
                             GdkColor                  *colour,
                             GdkPixmap                 *pixmap,
                             GtkWidget                 *menubar)
{
	GtkRcStyle *rc_style;
	GtkStyle *style;

	/* reset style */
	gtk_widget_set_style(GTK_WIDGET (applet), NULL);
 	gtk_widget_set_style(menubar, NULL);
	rc_style = gtk_rc_style_new ();
	gtk_widget_modify_style(GTK_WIDGET (applet), rc_style);
	gtk_widget_modify_style(menubar, rc_style);
	gtk_rc_style_unref(rc_style);

	switch (type) 
	{
		case PANEL_NO_BACKGROUND:
			break;
		case PANEL_COLOR_BACKGROUND:
			gtk_widget_modify_bg(GTK_WIDGET (applet), GTK_STATE_NORMAL, colour);
			gtk_widget_modify_bg(menubar, GTK_STATE_NORMAL, colour);
			break;
    
		case PANEL_PIXMAP_BACKGROUND:
			style = gtk_style_copy(GTK_WIDGET (applet)->style);
			if (style->bg_pixmap[GTK_STATE_NORMAL])
				g_object_unref(style->bg_pixmap[GTK_STATE_NORMAL]);
			style->bg_pixmap[GTK_STATE_NORMAL] = g_object_ref (pixmap);
			gtk_widget_set_style(GTK_WIDGET (applet), style);
			gtk_widget_set_style(GTK_WIDGET (menubar), style);
			g_object_unref(style);
			break;
  }
}

