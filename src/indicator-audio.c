
#include <gtk/gtk.h>
#include "indicator-audio.h"

GtkWidget *
indicator_audio_menuitem (void)
{
	GtkWidget * mainmenu = gtk_menu_item_new();

	GtkWidget * icon = gtk_image_new_from_icon_name("audio-volume-muted",
	                                                GTK_ICON_SIZE_MENU);

	gtk_container_add(GTK_CONTAINER(mainmenu), icon);
	gtk_widget_show(icon);

	return mainmenu;
}

