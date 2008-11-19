
#include <gtk/gtk.h>
#include "indicator-audio.h"

GtkWidget *
create_output_menu_item (void)
{
	GtkWidget * menuitem = gtk_menu_item_new();

	GtkWidget * label_hbox = gtk_hbox_new(FALSE, 12);

	GtkWidget * label = gtk_label_new("Volume:");
	gtk_box_pack_end(GTK_BOX(label_hbox), label, TRUE, FALSE, 0);
	gtk_widget_show(label);


	gtk_container_add(GTK_CONTAINER(menuitem), label_hbox);
	gtk_widget_show(label_hbox);

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

