
#include <gtk/gtk.h>
#include "slidermenuitem.h"
#include "indicator-audio.h"

void
block_prelight (GtkWidget * widget, GtkStateType prev, gpointer data)
{
	if (GTK_WIDGET_STATE(widget) == GTK_STATE_PRELIGHT) {
		gtk_widget_set_state(widget, prev);
	}
	return;
}

GtkWidget *
create_output_menu_item (void)
{
	GtkWidget * menuitem = slider_menu_item_new("Volume:", 0.0, 1.0, 0.02);

	return menuitem;
}

GtkWidget *
indicator_audio_menuitem (void)
{
	GtkWidget * mainmenu = gtk_menu_item_new();

	GtkWidget * icon = gtk_image_new_from_icon_name("audio-volume-muted",
	                                                GTK_ICON_SIZE_MENU);

	gtk_container_add(GTK_CONTAINER(mainmenu), icon);
	gtk_widget_show(icon);

	GtkWidget * menu = gtk_menu_new();

	GtkWidget * item = create_output_menu_item();
	gtk_menu_append(GTK_MENU(menu), item);
	gtk_widget_show(item);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainmenu), menu);
	gtk_widget_show(menu);

	return mainmenu;
}

