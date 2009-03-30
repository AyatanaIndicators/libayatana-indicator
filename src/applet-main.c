/*
A small wrapper utility to load indicators and put them as menu items
into the gnome-panel using it's applet interface.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This program is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License version 3, as published 
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <config.h>
#include <panel-applet.h>
#include <libgnomeui/gnome-ui-init.h>

#define SYMBOL_NAME  "get_menu_item"
#define ICONS_DIR  (DATADIR G_DIR_SEPARATOR_S "indicator-applet" G_DIR_SEPARATOR_S "icons")

static gboolean     applet_fill_cb (PanelApplet * applet, const gchar * iid, gpointer data);


static void cw_panel_background_changed (PanelApplet               *applet,
                                         PanelAppletBackgroundType  type,
                        				         GdkColor                  *colour,
                        				         GdkPixmap                 *pixmap,
                                         GtkWidget                 *menubar);

/* ****************** *
 *  Global Variables  *
 * ****************** */

static GnomeProgram *program = NULL;


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
menubar_press (GtkWidget * widget,
                    GdkEventButton *event,
                    gpointer data)
{
	if (event->button != 1) {
		g_signal_stop_emission_by_name(widget, "button-press-event");
	}

	return FALSE;
}

static gboolean
menubar_on_expose (GtkWidget * widget,
                    GdkEventExpose *event,
                    GtkWidget * menubar)
{
	if (GTK_WIDGET_HAS_FOCUS(menubar))
		gtk_paint_focus(widget->style, widget->window, GTK_WIDGET_STATE(menubar),
		                NULL, widget, "menubar-applet", 0, 0, -1, -1);

	return FALSE;
}

static void
about_cb (BonoboUIComponent *ui_container,
	  gpointer           data,
	  const gchar       *cname)
{
	static const gchar *authors[] = {
		"Ted Gould <ted@canonical.com>",
		NULL
	};

	static gchar *license[] = {
        N_("This program is free software: you can redistribute it and/or modify it "
           "under the terms of the GNU General Public License version 3, as published "
           "by the Free Software Foundation."),
        N_("This program is distributed in the hope that it will be useful, but "
           "WITHOUT ANY WARRANTY; without even the implied warranties of "
           "MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR "
           "PURPOSE.  See the GNU General Public License for more details."),
        N_("You should have received a copy of the GNU General Public License along "
           "with this program.  If not, see <http://www.gnu.org/licenses/>."),
		NULL
	};
	gchar *license_i18n;

	license_i18n = g_strconcat (_(license[0]), "\n\n", _(license[1]), "\n\n", _(license[2]), NULL);

	gtk_show_about_dialog(NULL,
		"version", "0.1",
		"copyright", "Copyright \xc2\xa9 2009 Canonical, Ltd.",
		"comments", _("An applet to hold all of the system indicators."),
		"authors", authors,
		"license", license_i18n,
		"wrap-license", TRUE,
		"translator-credits", _("translator-credits"),
		"logo-icon-name", "indicator-applet",
		"website", "http://launchpad.net/indicator-applet",
		"website-label", _("Indicator Applet Website"),
		NULL
	);

	g_free (license_i18n);

	return;
}

#ifdef N_
#undef N_
#endif
#define N_(x) x

static gboolean
applet_fill_cb (PanelApplet * applet, const gchar * iid, gpointer data)
{
	static const BonoboUIVerb menu_verbs[] = {
		BONOBO_UI_VERB ("IndicatorAppletAbout", about_cb),
		BONOBO_UI_VERB_END
	};
	static const gchar * menu_xml = 
		"<popup name=\"button3\">"
			"<menuitem name=\"About Item\" verb=\"IndicatorAppletAbout\" _label=\"" N_("_About") "\" pixtype=\"stock\" pixname=\"gtk-about\"/>"
		"</popup>";

	GtkWidget *menubar;
	gint i;
	gint indicators_loaded = 0;

	/* check if we are running stracciatella session */
	if (g_strcmp0(g_getenv("GDMSESSION"), "gnome-stracciatella") == 0) {
		g_debug("Running stracciatella GNOME session, disabling myself");
		return TRUE;
	}
  
	static gboolean first_time = FALSE;

	if (!first_time)
	{
        gint argc = 1;
        gchar *argv[2] = { "indicator-applet", NULL};
	    
		first_time = TRUE;
		program = gnome_program_init ("indicator-applet", "0.1",
				    LIBGNOMEUI_MODULE, argc, argv,
				    GNOME_PROGRAM_STANDARD_PROPERTIES,
				    NULL);
	}

	/* Set panel options */
	gtk_container_set_border_width(GTK_CONTAINER (applet), 0);
	panel_applet_set_flags(applet, PANEL_APPLET_EXPAND_MINOR);
	panel_applet_setup_menu(applet, menu_xml, menu_verbs, NULL);
    atk_object_set_name (gtk_widget_get_accessible (GTK_WIDGET (applet)),
                         "indicator-applet");
  
	/* Init some theme/icon stuff */
	gtk_icon_theme_append_search_path(gtk_icon_theme_get_default(),
	                                  ICONS_DIR);
	/* g_debug("Icons directory: %s", ICONS_DIR); */
	gtk_rc_parse_string (
	    "style \"indicator-applet-style\"\n"
        "{\n"
        "    GtkMenuBar::shadow-type = none\n"
        "    GtkMenuBar::internal-padding = 0\n"
        "    GtkWidget::focus-line-width = 0\n"
        "    GtkWidget::focus-padding = 0\n"
        "}\n"
        "widget \"*.fast-user-switch-applet\" style \"indicator-applet-style\"");
	//gtk_widget_set_name(GTK_WIDGET (applet), "indicator-applet-menubar");
	gtk_widget_set_name(GTK_WIDGET (applet), "fast-user-switch-applet");

	/* Build menubar */
	menubar = gtk_menu_bar_new();
	GTK_WIDGET_SET_FLAGS (menubar, GTK_WIDGET_FLAGS(menubar) | GTK_CAN_FOCUS);
	g_signal_connect(menubar, "button-press-event", G_CALLBACK(menubar_press), NULL);
	g_signal_connect_after(menubar, "expose-event", G_CALLBACK(menubar_on_expose), menubar);
	gtk_container_set_border_width(GTK_CONTAINER(menubar), 0);

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
		/* A label to allow for click through */
		GtkWidget * item = gtk_label_new(_("No Indicators"));
		gtk_container_add(GTK_CONTAINER(applet), item);
		gtk_widget_show(item);
	} else {
		gtk_container_add(GTK_CONTAINER(applet), menubar);
		panel_applet_set_background_widget(applet, menubar);
		gtk_widget_show(menubar);
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

