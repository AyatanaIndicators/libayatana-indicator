
#include <gtk/gtk.h>
#include "indicator-audio.h"

GtkWidget *
create_output_menu_item (void)
{
	GtkWidget * menuitem = gtk_menu_item_new();

	GtkWidget * label_hbox = gtk_hbox_new(FALSE, 12);

	GtkWidget * label = gtk_label_new("Volume:");
	gtk_box_pack_start(GTK_BOX(label_hbox), label, TRUE, FALSE, 0);
	gtk_widget_show(label);

	GtkWidget * slider_hbox = gtk_hbox_new(FALSE, 3);

	GtkWidget * mute_button = gtk_button_new();
	GtkWidget * mute_icon = gtk_image_new_from_icon_name("audio-volume-low", GTK_ICON_SIZE_MENU);
	gtk_container_add(GTK_CONTAINER(mute_button), mute_icon);
	gtk_widget_show(mute_icon);
	gtk_box_pack_start(GTK_BOX(slider_hbox), mute_button, FALSE, FALSE, 3);
	gtk_widget_show(mute_button);

	GtkWidget * scale = gtk_hscale_new_with_range(0.0, 1.0, 0.1);
	gtk_scale_set_digits(GTK_SCALE(scale), 2);
	gtk_scale_set_draw_value(GTK_SCALE(scale), FALSE);
	gtk_box_pack_start(GTK_BOX(slider_hbox), scale, TRUE, TRUE, 3);
	gtk_widget_show(scale);

	GtkWidget * max_button = gtk_button_new();
	GtkWidget * max_icon = gtk_image_new_from_icon_name("audio-volume-high", GTK_ICON_SIZE_MENU);
	gtk_container_add(GTK_CONTAINER(max_button), max_icon);
	gtk_widget_show(max_icon);
	gtk_box_pack_start(GTK_BOX(slider_hbox), max_button, FALSE, FALSE, 3);
	gtk_widget_show(max_button);

	gtk_box_pack_start(GTK_BOX(label_hbox), slider_hbox, TRUE, TRUE, 0);
	gtk_widget_show(slider_hbox);

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

