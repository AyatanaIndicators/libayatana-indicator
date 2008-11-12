
#include <panel-applet.h>


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
applet_fill_cb (PanelApplet * applet, const gchar * iid, gpointer data)
{
	GtkWidget * menubar = gtk_menu_bar_new();
	gtk_widget_set_name (menubar, "indicator-applet-menubar");
	gtk_container_add(GTK_CONTAINER(applet), menubar);
	gtk_widget_show(menubar);

	GtkWidget * item = gtk_menu_item_new_with_label("Test");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), item);
	gtk_widget_show(item);

	gtk_widget_show(GTK_WIDGET(applet));
	return TRUE;
}
