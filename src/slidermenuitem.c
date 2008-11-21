
#include "slidermenuitem.h"

/* *** Enums *** */

enum {
  VALUE_CHANGED,
  LAST_SIGNAL
};

enum {
  PROP_0,
};

/* *** Prototypes *** */



/* *** globals *** */

static guint slider_menu_item_signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE (SliderMenuItem, slider_menu_item, GTK_TYPE_MENU_ITEM)

void
slider_menu_item_init (SliderMenuItem * smi)
{

	return;
}

static void
slider_menu_item_class_init (SliderMenuItemClass * klass)
{

	return;
}

GtkWidget *
slider_menu_item_new (const gchar * label, gdouble min, gdouble max, gdouble step)
{
	GtkWidget * smi;

	smi = g_object_new(TYPE_SLIDER_MENU_ITEM, NULL);

	GtkWidget * label_hbox = gtk_hbox_new(FALSE, 12);

	/* Note: look into gtk_accel_label */
	GtkWidget * wlabel = gtk_label_new(label);
	gtk_box_pack_start(GTK_BOX(label_hbox), wlabel, TRUE, FALSE, 0);
	gtk_widget_show(wlabel);

	GtkWidget * slider_hbox = gtk_hbox_new(FALSE, 3);

	GtkWidget * mute_button = gtk_button_new();
	GtkWidget * mute_icon = gtk_image_new_from_icon_name("audio-volume-low", GTK_ICON_SIZE_MENU);
	gtk_container_add(GTK_CONTAINER(mute_button), mute_icon);
	gtk_widget_show(mute_icon);
	gtk_box_pack_start(GTK_BOX(slider_hbox), mute_button, FALSE, FALSE, 3);
	gtk_widget_show(mute_button);

	GtkWidget * scale = gtk_hscale_new_with_range(min, max, step);
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

	gtk_container_add(GTK_CONTAINER(smi), label_hbox);
	gtk_widget_show(label_hbox);

	return smi;
}
