
#include <gtk/gtk.h>
#include "slidermenuitem.h"
#include "indicator-audio.h"


GtkWidget *
indicator_audio_menuitem (void)
{
	GtkWidget * mainmenu = gtk_menu_item_new();

	GtkWidget * icon = gtk_image_new_from_icon_name("audio-volume-muted",
	                                                GTK_ICON_SIZE_MENU);

	gtk_container_add(GTK_CONTAINER(mainmenu), icon);
	gtk_widget_show(icon);

	GtkWidget * menu = gtk_menu_new();

	GtkWidget * item = gtk_menu_item_new_with_label("Mute All");
	gtk_menu_append(GTK_MENU(menu), item);
	gtk_widget_show(item);

	item = gtk_menu_item_new_with_label("Sound Settings...");
	gtk_menu_append(GTK_MENU(menu), item);
	gtk_widget_show(item);

	GtkWidget * sep = gtk_separator_menu_item_new();
	gtk_menu_append(GTK_MENU(menu), sep);
	gtk_widget_show(sep);

	item = slider_menu_item_new("Speakers:", 0.0, 1.0, 0.02);
	gtk_menu_append(GTK_MENU(menu), item);
	gtk_widget_show(item);

	item = slider_menu_item_new("Headphones:", 0.0, 1.0, 0.02);
	gtk_menu_append(GTK_MENU(menu), item);
	gtk_widget_show(item);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainmenu), menu);
	gtk_widget_show(menu);

	return mainmenu;
}

