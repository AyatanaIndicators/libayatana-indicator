
#include <gtk/gtk.h>
#include "indicator-power.h"

typedef struct  {
	char * label;
	char * icon;
	char * time;
} menuitem_t;

#define MENU_ITEM_CNT 3
menuitem_t menuitems[MENU_ITEM_CNT] = {
	{"Main", "audio-volume-low", "(1:34 left)"},
	{"Mouse", "audio-volume-med", "(0:40 left)"},
	{"Phone", "audio-volume-high", "(10:40 left)"}
};

GtkWidget *
power_menu (void)
{
	GtkMenu * menu = GTK_MENU(gtk_menu_new());

	GtkSizeGroup * icons  = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	GtkSizeGroup * labels = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	GtkSizeGroup * times  = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

	int i;
	for (i = 0 ; i < MENU_ITEM_CNT; i++) {
		GtkWidget * item = gtk_menu_item_new ();

		GtkWidget * hbox = gtk_hbox_new (FALSE, 3);

		GtkWidget * icon = gtk_image_new_from_icon_name(menuitems[i].icon, GTK_ICON_SIZE_MENU);
		gtk_size_group_add_widget(icons, icon);
		gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
		gtk_widget_show(icon);

		GtkWidget * label = gtk_label_new(menuitems[i].label);
		gtk_size_group_add_widget(labels, label);
		gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
		gtk_widget_show(label);

		GtkWidget * time = gtk_label_new(menuitems[i].time);
		gtk_size_group_add_widget(times, time);
		gtk_box_pack_start(GTK_BOX(hbox), time, FALSE, FALSE, 0);
		gtk_widget_show(time);

		gtk_container_add(GTK_CONTAINER(item), hbox);
		gtk_widget_show(hbox);

		gtk_menu_append(menu, item);
		gtk_widget_show(item);
	}

	return GTK_WIDGET(menu);
}

GtkWidget *
indicator_power_menuitem (void)
{
	GtkWidget * mainmenu = gtk_menu_item_new();

	GtkWidget * hbox = gtk_hbox_new(FALSE, 0);

	GtkWidget * icon = gtk_image_new_from_icon_name("audio-volume-high",
	                                                GTK_ICON_SIZE_MENU);
	gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
	gtk_widget_show(icon);

	GtkWidget * label = gtk_label_new("2:30");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	gtk_container_add(GTK_CONTAINER(mainmenu), hbox);
	gtk_widget_show(hbox);

	GtkWidget * submenu = power_menu();
	if (submenu != NULL) {
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainmenu), submenu);
		gtk_widget_show(submenu);
	}

	return mainmenu;
}

