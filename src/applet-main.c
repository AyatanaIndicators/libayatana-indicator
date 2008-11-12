
#include <panel-applet.h>

#include "indicator-audio.h"
#include "indicator-messages.h"
#include "indicator-network.h"
#include "indicator-power.h"
#include "indicator-system.h"

static gboolean     applet_fill_cb (PanelApplet * applet, const gchar * iid, gpointer data);


/*************
 * main
 * ***********/

PANEL_APPLET_BONOBO_FACTORY ("OAFIID:GNOME_IndicatorApplet_Factory",
               PANEL_TYPE_APPLET,
               "indicator-applet", "0",
               applet_fill_cb, NULL);

typedef GtkWidget * (*menuitem_func) (void);

menuitem_func indicators[] = {
	indicator_audio_menuitem,
	indicator_messages_menuitem,
	indicator_network_menuitem,
	indicator_power_menuitem,
	indicator_system_menuitem,
	NULL
};

/*************
 * init function
 * ***********/

static gboolean
applet_fill_cb (PanelApplet * applet, const gchar * iid, gpointer data)
{
	int i;
	GtkWidget * menubar = gtk_menu_bar_new();
	gtk_widget_set_name (menubar, "indicator-applet-menubar");
	gtk_container_add(GTK_CONTAINER(applet), menubar);
	gtk_widget_show(menubar);

	GtkWidget * item = gtk_menu_item_new_with_label("Test");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), item);
	gtk_widget_show(item);

	for (i = 0; indicators[i] != NULL; i++) {
		GtkWidget * item = indicators[i]();
		if (item == NULL) continue;
		gtk_menu_shell_append(GTK_MENU_SHELL(menubar), item);
		gtk_widget_show(item);
	}

	gtk_widget_show(GTK_WIDGET(applet));
	return TRUE;
}
