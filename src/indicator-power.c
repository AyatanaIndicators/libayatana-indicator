
#include <gtk/gtk.h>
#include "indicator-power.h"

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

	return mainmenu;
}

