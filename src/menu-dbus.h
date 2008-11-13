
#ifndef MENU_DBUS_H__SEEN__
#define MENU_DBUS_H__SEEN__ 1

#include <gtk/gtk.h>
#include <dbus/dbus-glib.h>

#define DBUS_MENU_ROOT_ID  (0)

GtkWidget * dbus_menu_make_menuitem (DBusGConnection * bus,
                                     const char * name,
                                     const char * object,
                                     guint menu_id);



#endif /* MENU_DBUS_H__SEEN__ */
