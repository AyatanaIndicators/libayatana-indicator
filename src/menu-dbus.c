
#include "menu-dbus.h"

GtkWidget *
_dbus_menu_make_menuitem_val (DBusGProxy * proxy, guint id)
{



}

GtkWidget *
_dbus_menu_make_menuitem_recurse (DBusGProxy * proxy, guint id)
{
	GError * error = NULL;
	GArray * items = NULL;
	gboolean proxyret;

	/* TODO: switch to begin_call */
	proxyret = dbus_g_proxy_call (proxy, "GetSubmenuItems", &error,
	                              G_TYPE_UINT, id,
	                              G_TYPE_INVALID,
	                              DBUS_TYPE_G_UINT_ARRAY, &items,
	                              G_TYPE_INVALID);

	if (!proxyret) {
		/* TODO: clear error */
		return NULL;
	}

	GtkWidget * menu = gtk_menu_new();
	gint i;
	for (i = 0; i < items->len; i++) {
		guint id = g_array_index(items, guint, i);
		GtkWidget * item = _dbus_menu_make_menuitem_val(proxy, id);
		if (item != NULL) {
			gtk_menu_append(menu, item);
			gtk_widget_show(item);
		}
	}

	return menu;
}

GtkWidget *
dbus_menu_make_menuitem (DBusGConnection * bus, const char * name, const char * object, guint menu_id)
{
	DBusGProxy * iface = dbus_g_proxy_new_for_name_owner (bus, name, object, "com.canonical.menu", NULL);
	if (iface == NULL) {
		return NULL;
	}

	return _dbus_menu_make_menuitem_recurse(iface, menu_id);
}


