/*
* Copyright 2013 Canonical Ltd.
*
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License version 3, as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranties of
* MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Authors:
*     Lars Uebernickel <lars.uebernickel@canonical.com>
*/

#ifndef __AYATANA_MENU_ITEM_FACTORY_H__
#define __AYATANA_MENU_ITEM_FACTORY_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AYATANA_TYPE_MENU_ITEM_FACTORY         (ayatana_menu_item_factory_get_type ())
#define AYATANA_MENU_ITEM_FACTORY(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), AYATANA_TYPE_MENU_ITEM_FACTORY, AyatanaMenuItemFactory))
#define AYATANA_IS_MENU_ITEM_FACTORY(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), AYATANA_TYPE_MENU_ITEM_FACTORY))
#define AYATANA_MENU_ITEM_FACTORY_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), AYATANA_TYPE_MENU_ITEM_FACTORY, AyatanaMenuItemFactoryInterface))

#define AYATANA_MENU_ITEM_FACTORY_EXTENSION_POINT_NAME "ayatana-menu-item-factory"

typedef struct _AyatanaMenuItemFactoryInterface AyatanaMenuItemFactoryInterface;
typedef struct _AyatanaMenuItemFactory          AyatanaMenuItemFactory;

struct _AyatanaMenuItemFactoryInterface
{
  GTypeInterface iface;

  GtkMenuItem * (*create_menu_item)  (AyatanaMenuItemFactory *factory,
                                      const gchar           *type,
                                      GMenuItem             *menuitem,
                                      GActionGroup          *actions);
};

GDK_AVAILABLE_IN_3_10
GList *                 ayatana_menu_item_factory_get_all                (void);

GDK_AVAILABLE_IN_3_10
GType                   ayatana_menu_item_factory_get_type               (void);

GDK_AVAILABLE_IN_3_10
GtkMenuItem *           ayatana_menu_item_factory_create_menu_item       (AyatanaMenuItemFactory *factory,
                                                                         const gchar           *type,
                                                                         GMenuItem             *menuitem,
                                                                         GActionGroup          *actions);

G_END_DECLS

#endif
