
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
	GtkWidget * hbox = gtk_hbox_new(FALSE, 3);

	GtkWidget * label = gtk_label_new("Test");
	gtk_box_pack_end(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_widget_show(label);

	gtk_container_add(GTK_CONTAINER(applet), hbox);
	gtk_widget_show(hbox);

	gtk_widget_show(GTK_WIDGET(applet));
	return TRUE;
}
