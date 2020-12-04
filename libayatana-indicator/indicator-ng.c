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

#include "indicator-ng.h"
#include "indicator-image-helper.h"
#include <libayatana-ido/ayatanamenuitemfactory.h>
#include <string.h>

#define MENU_SECTIONS 20

struct _IndicatorNg
{
  IndicatorObject parent;

  gchar *service_file;
  gchar *name;
  gchar *object_path;
  gchar *menu_object_path;
  gchar *bus_name;
  gchar *profile;
  gchar *header_action;
  gchar *scroll_action;
  gchar *secondary_action;
  gchar *submenu_action;
  gint position;

  guint name_watch_id;

  GDBusConnection *session_bus;
  GActionGroup *actions;
  GMenuModel *menu;

  IndicatorObjectEntry entry;
  gchar *accessible_desc;

  gint64 last_service_restart;
    GMenuModel *lMenuSections[MENU_SECTIONS];
};

static void indicator_ng_initable_iface_init (GInitableIface *initable);
G_DEFINE_TYPE_WITH_CODE (IndicatorNg, indicator_ng, INDICATOR_OBJECT_TYPE,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE, indicator_ng_initable_iface_init))

enum
{
  PROP_0,
  PROP_SERVICE_FILE,
  PROP_PROFILE,
  N_PROPERTIES
};

static GQuark m_pActionMuxer = 0;
static GParamSpec *properties[N_PROPERTIES];

static void
indicator_ng_get_property (GObject    *object,
                           guint       property_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  IndicatorNg *self = INDICATOR_NG (object);

  switch (property_id)
    {
    case PROP_SERVICE_FILE:
      g_value_set_string (value, self->service_file);
      break;

    case PROP_PROFILE:
      g_value_set_string (value, self->profile);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
indicator_ng_set_property (GObject      *object,
                           guint         property_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  IndicatorNg *self = INDICATOR_NG (object);

  switch (property_id)
    {
    case PROP_SERVICE_FILE: /* construct-only */
      self->service_file = g_strdup (g_value_get_string (value));
      break;

    case PROP_PROFILE: /* construct-only */
      self->profile = g_strdup (g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
indicator_ng_free_actions_and_menu (IndicatorNg *self)
{
  if (self->actions)
    {
      gtk_widget_insert_action_group (GTK_WIDGET (self->entry.menu), "indicator", NULL);
      g_signal_handlers_disconnect_by_data (self->actions, self);
      g_clear_object (&self->actions);
    }

  if (self->menu)
    {
        for (guint nMenuSection = 0; nMenuSection < MENU_SECTIONS; nMenuSection++)
        {
            if (self->lMenuSections[nMenuSection])
            {
                g_object_unref(self->lMenuSections[nMenuSection]);
                self->lMenuSections[nMenuSection] = NULL;
            }
        }

      g_signal_handlers_disconnect_by_data (self->menu, self);
      g_clear_object (&self->menu);
    }
}

static void
indicator_ng_dispose (GObject *object)
{
  IndicatorNg *self = INDICATOR_NG (object);

  if (self->name_watch_id)
    {
      g_bus_unwatch_name (self->name_watch_id);
      self->name_watch_id = 0;
    }

  g_clear_object (&self->session_bus);

  indicator_ng_free_actions_and_menu (self);

  g_clear_object (&self->entry.label);
  g_clear_object (&self->entry.image);
  g_clear_object (&self->entry.menu);

  G_OBJECT_CLASS (indicator_ng_parent_class)->dispose (object);
}

static void
indicator_ng_finalize (GObject *object)
{
  IndicatorNg *self = INDICATOR_NG (object);

  g_free (self->service_file);
  g_free (self->name);
  g_free (self->object_path);
  g_free (self->menu_object_path);
  g_free (self->bus_name);
  g_free (self->accessible_desc);
  g_free (self->header_action);
  g_free (self->scroll_action);
  g_free (self->secondary_action);
  g_free (self->submenu_action);

  G_OBJECT_CLASS (indicator_ng_parent_class)->finalize (object);
}

static GList *
indicator_ng_get_entries (IndicatorObject *io)
{
  IndicatorNg *self = INDICATOR_NG (io);

  return g_list_append (NULL, &self->entry);
}

static gint
indicator_ng_get_position (IndicatorObject *io)
{
  IndicatorNg *self = INDICATOR_NG (io);

  return self->position;
}

static void
indicator_ng_entry_scrolled (IndicatorObject          *io,
                             IndicatorObjectEntry     *entry,
                             gint                      delta,
                             IndicatorScrollDirection  direction)
{
  IndicatorNg *self = INDICATOR_NG (io);

  if (self->actions && self->scroll_action)
    {
      if (direction == INDICATOR_OBJECT_SCROLL_DOWN ||
          direction == INDICATOR_OBJECT_SCROLL_LEFT)
        {
          delta *= -1;
        }

      g_action_group_activate_action (self->actions, self->scroll_action,
                                      g_variant_new_int32 (delta));
    }
}

void
indicator_ng_secondary_activate (IndicatorObject      *io,
                                 IndicatorObjectEntry *entry,
                                 guint                 timestamp,
                                 gpointer              user_data)
{
  IndicatorNg *self = INDICATOR_NG (io);

  if (self->actions && self->secondary_action)
    {
      g_action_group_activate_action (self->actions, self->secondary_action, NULL);
    }
}

static gboolean indicator_ng_menu_insert_idos(IndicatorNg *self, GMenuModel *pSection, guint nModelItem, guint nMenuItem, gboolean bNamespace, gchar *sNamespace)
{
    gboolean bChanged = FALSE;
    gchar *sType;
    gboolean bHasType = g_menu_model_get_item_attribute(pSection, nModelItem, "x-ayatana-type", "s", &sType);

    if (bHasType)
    {
        GList *lMenuItems = gtk_container_get_children(GTK_CONTAINER(self->entry.menu));
        GtkWidget *pMenuItemOld = GTK_WIDGET(g_list_nth_data(lMenuItems, nMenuItem));
        const gchar *sName = gtk_widget_get_name(pMenuItemOld);

        if (!g_str_equal(sName, sType))
        {
            GActionGroup *pActionGroup = (GActionGroup*)g_object_get_qdata(G_OBJECT(self->entry.menu), m_pActionMuxer);
            GMenuItem *pMenuModelItem = g_menu_item_new_from_model(pSection, nModelItem);
            GtkMenuItem* pMenuItemNew = NULL;
            gchar *sAction;

            if (bNamespace && g_menu_item_get_attribute(pMenuModelItem, G_MENU_ATTRIBUTE_ACTION, "s", &sAction))
            {
                gchar *sNamespacedAction = g_strconcat(sNamespace, ".", sAction, NULL);
                g_menu_item_set_attribute(pMenuModelItem, G_MENU_ATTRIBUTE_ACTION, "s", sNamespacedAction);
                g_free (sNamespacedAction);
                g_free (sAction);
            }

            for (GList *pFactory = ayatana_menu_item_factory_get_all(); pFactory != NULL && pMenuItemNew == NULL; pFactory = pFactory->next)
            {
                pMenuItemNew = ayatana_menu_item_factory_create_menu_item(pFactory->data, sType, pMenuModelItem, pActionGroup);
                bChanged = TRUE;
            }

            if (pMenuItemNew == NULL)
            {
                pMenuItemNew = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Failed to create IDO object"));
            }

            gtk_widget_set_name(GTK_WIDGET(pMenuItemNew), sType);
            gtk_widget_show(GTK_WIDGET(pMenuItemNew));
            gtk_container_remove(GTK_CONTAINER(self->entry.menu), pMenuItemOld);
            gtk_menu_shell_insert(GTK_MENU_SHELL(self->entry.menu), GTK_WIDGET(pMenuItemNew), nMenuItem);
            g_object_unref(pMenuModelItem);
        }

        g_list_free(lMenuItems);
        g_free(sType);
    }

    return bChanged;
}

static void indicator_ng_menu_size_allocate(GtkWidget *pWidget, GtkAllocation *pAllocation, gpointer pUserData)
{
    IndicatorNg *self = pUserData;
    GList *pMenuItem = gtk_container_get_children(GTK_CONTAINER(self->entry.menu));
    guint nWidth = 0;
    guint nHeight = 0;
    GdkWindow *pWindowBin = NULL;

    while (pMenuItem)
    {
        if (!pWindowBin)
        {
            pWindowBin = gtk_widget_get_parent_window(pMenuItem->data);
        }

        gint nWidthNat;
        gint nHeightNat;
        gtk_widget_get_preferred_width(pMenuItem->data, NULL, &nWidthNat);
        gtk_widget_get_preferred_height(pMenuItem->data, NULL, &nHeightNat);
        nWidth = MAX(nWidth, nWidthNat);
        nHeight += nHeightNat;
        GtkBorder cPadding;
        GtkStyleContext *pContext = gtk_widget_get_style_context(GTK_WIDGET(pMenuItem->data));
        gtk_style_context_get_padding(pContext, gtk_style_context_get_state(pContext), &cPadding);
        nWidth += cPadding.left + cPadding.right;
        pMenuItem = g_list_next(pMenuItem);
    }

    g_list_free(pMenuItem);
    GtkBorder cPadding;
    GtkStyleContext *pContext = gtk_widget_get_style_context(GTK_WIDGET(self->entry.menu));
    gtk_style_context_get_padding(pContext, gtk_style_context_get_state(pContext), &cPadding);
    gint nBorderWidth = gtk_container_get_border_width(GTK_CONTAINER(self->entry.menu));
    gint nIconWidth;
    gtk_icon_size_lookup(GTK_ICON_SIZE_MENU, &nIconWidth, NULL);
    nWidth += (2 * nBorderWidth) + cPadding.left + cPadding.right + (nIconWidth * 3) / 2;
    nHeight += (2 * nBorderWidth) + cPadding.top + cPadding.bottom + (nIconWidth * 3) / 4;

    GdkRectangle cRectangle = {0};
    GdkDisplay *pDisplay = gdk_display_get_default();
    GdkMonitor *pMonitor = gdk_display_get_primary_monitor(pDisplay);
    gdk_monitor_get_workarea(pMonitor, &cRectangle);

    if (nHeight <= cRectangle.height)
    {
        gdk_window_move_resize(pWindowBin, 0, 0, nWidth, nHeight);
    }

    nHeight = MIN(nHeight, cRectangle.height);

    GdkWindow *pWindow = gtk_widget_get_parent_window(GTK_WIDGET(self->entry.menu));
    gdk_window_resize(pWindow, nWidth, nHeight);

    gtk_menu_reposition(self->entry.menu);
}

static void indicator_ng_menu_section_changed(GMenuModel *pMenuSection, gint nPosition, gint nRemoved, gint nAdded, gpointer pUserData)
{
    IndicatorNg *self = pUserData;
    GMenuModel *pModel = g_menu_model_get_item_link(self->menu, 0, G_MENU_LINK_SUBMENU);
    guint nMenuItem = 0;
    gboolean bChanged = FALSE;

    if (pModel)
    {
        guint nSections = g_menu_model_get_n_items(pModel);

        for (guint nSection = 0; nSection < nSections; nSection++)
        {
            GMenuModel *pSection = g_menu_model_get_item_link(pModel, nSection, G_MENU_LINK_SECTION);
            guint nSubsections = 0;

            if (pSection)
            {
                gchar *sNamespace;
                gboolean bNamespace = g_menu_model_get_item_attribute(pModel, nSection, G_MENU_ATTRIBUTE_ACTION_NAMESPACE, "s", &sNamespace);
                nSubsections = g_menu_model_get_n_items(pSection);

                for (guint nSubsection = 0; nSubsection < nSubsections; nSubsection++)
                {
                    GMenuModel *pSubsection = g_menu_model_get_item_link(pSection, nSubsection, G_MENU_LINK_SECTION);

                    if (pSubsection)
                    {
                        guint nItems = g_menu_model_get_n_items(pSubsection);

                        // Skip the subsection separator (if there is one)
                        GList *lMenuItems = gtk_container_get_children(GTK_CONTAINER(self->entry.menu));
                        GtkWidget *pMenuItem = GTK_WIDGET(g_list_nth_data(lMenuItems, nMenuItem));

                        if (GTK_IS_SEPARATOR_MENU_ITEM(pMenuItem))
                        {
                            nMenuItem++;
                        }

                        g_list_free(lMenuItems);

                        for (guint nItem = 0; nItem < nItems; nItem++)
                        {
                            bChanged = indicator_ng_menu_insert_idos(self, pSubsection, nItem, nMenuItem, bNamespace, sNamespace) || bChanged;
                            nMenuItem++;
                        }
                    }

                    g_object_unref(pSubsection);

                    bChanged = indicator_ng_menu_insert_idos(self, pSection, nSubsection, nMenuItem, bNamespace, sNamespace) || bChanged;

                    if (!g_str_equal(self->name, "ayatana-indicator-messages"))
                    {
                        nMenuItem++;
                    }
                }

                if (bNamespace)
                {
                    g_free(sNamespace);
                }

                g_object_unref(pSection);
            }

            if (pSection && nSubsections)
            {
                nMenuItem++;
            }
        }

        g_object_unref(pModel);
    }

    if (bChanged)
    {
        indicator_ng_menu_size_allocate(NULL, NULL, self);
    }
}

static void indicator_ng_menu_shown(GtkWidget *pWidget, gpointer pUserData)
{
    IndicatorNg *self = pUserData;
    guint nSectionCount = 0;

    if (!self->lMenuSections[0])
    {
        self->lMenuSections[0] = g_menu_model_get_item_link(self->menu, 0, G_MENU_LINK_SUBMENU);

        if (self->lMenuSections[0])
        {
            guint nSections = g_menu_model_get_n_items(self->lMenuSections[0]);

            for (gint nSection = 0; nSection < nSections; nSection++)
            {
                self->lMenuSections[++nSectionCount] = g_menu_model_get_item_link(self->lMenuSections[0], nSection, G_MENU_LINK_SECTION);

                if (self->lMenuSections[nSectionCount])
                {
                    g_signal_connect(self->lMenuSections[nSectionCount], "items-changed", G_CALLBACK(indicator_ng_menu_section_changed), self);
                    guint nSubsections = g_menu_model_get_n_items(self->lMenuSections[nSectionCount]);
                    guint nParent = nSectionCount;

                    for (guint nSubsection = 0; nSubsection < nSubsections; nSubsection++)
                    {
                        self->lMenuSections[++nSectionCount] = g_menu_model_get_item_link(self->lMenuSections[nParent], nSubsection, G_MENU_LINK_SECTION);

                        if (self->lMenuSections[nSectionCount])
                        {
                            g_signal_connect(self->lMenuSections[nSectionCount], "items-changed", G_CALLBACK(indicator_ng_menu_section_changed), self);
                        }
                    }
                }
            }

            g_signal_connect(self->lMenuSections[0], "items-changed", G_CALLBACK(indicator_ng_menu_section_changed), self);
            indicator_ng_menu_section_changed(self->lMenuSections[0], 0, 0, 1, self);
        }
    }

    if (self->submenu_action)
    {
        g_action_group_change_action_state(self->actions, self->submenu_action, g_variant_new_boolean(TRUE));
    }
}

static void
indicator_ng_menu_hidden (GtkWidget *widget,
                          gpointer   user_data)
{
  IndicatorNg *self = user_data;

  if (self->submenu_action)
    g_action_group_change_action_state (self->actions, self->submenu_action,
                                        g_variant_new_boolean (FALSE));
}

static void
indicator_ng_set_accessible_desc (IndicatorNg *self,
                                  const gchar *accessible_desc)
{
  g_free (self->accessible_desc);
  self->accessible_desc = g_strdup (accessible_desc);

  self->entry.accessible_desc = self->accessible_desc;
  g_signal_emit_by_name (self, INDICATOR_OBJECT_SIGNAL_ACCESSIBLE_DESC_UPDATE, &self->entry);
}

static void
indicator_ng_set_icon_from_variant (IndicatorNg *self,
                                    GVariant    *variant)
{
  GIcon *icon;

  if (variant == NULL)
    {
      if (self->entry.image)
        {
          gtk_image_clear (self->entry.image);
          gtk_widget_hide (GTK_WIDGET (self->entry.image));
        }
      return;
    }

  gtk_widget_show (GTK_WIDGET (self->entry.image));

  icon = g_icon_deserialize (variant);
  if (icon)
    {
      indicator_image_helper_update_from_gicon (self->entry.image, icon);
      g_object_unref (icon);
    }
  else
    {
      gchar *text = g_variant_print (variant, TRUE);
      g_warning ("invalid icon variant '%s'", text);
      gtk_image_set_from_icon_name (self->entry.image, "image-missing", GTK_ICON_SIZE_LARGE_TOOLBAR);
      g_free (text);
    }
}

static void indicator_ng_set_label(IndicatorNg *self, const gchar *label)
{
    if (!self->entry.label)
    {
        return;
    }

    const gchar *sLabel = label;
    guint nSpacing = 3;
    guint nPadding = 6;

    if (label == NULL || *label == '\0' || !self->entry.image || !gtk_widget_get_visible(GTK_WIDGET(self->entry.image)))
    {
        nSpacing = 0;
        nPadding = 0;
    }

    GtkWidget *pParent = gtk_widget_get_parent(GTK_WIDGET(self->entry.label));
    GtkCssProvider *pCssProvider = gtk_css_provider_new();
    GtkStyleContext *pStyleContext = gtk_widget_get_style_context(GTK_WIDGET(self->entry.label));
    gtk_style_context_add_provider(pStyleContext, GTK_STYLE_PROVIDER(pCssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gchar *sCss = g_strdup_printf("label{padding-left: %ipx;}", nPadding);
    gtk_css_provider_load_from_data(pCssProvider, sCss, -1, NULL);
    g_free(sCss);
    g_object_unref(pCssProvider);
    if (GTK_IS_BOX(pParent))
    {
        gtk_box_set_spacing(GTK_BOX(pParent), nSpacing);
    }
    gtk_label_set_label(GTK_LABEL (self->entry.label), sLabel);

    if (label)
    {
        gtk_widget_show(GTK_WIDGET (self->entry.label));
    }
}

static void
indicator_ng_update_entry (IndicatorNg *self)
{
  GVariant *state;
  const gchar *label = NULL;
  GVariant *icon = NULL;
  const gchar *accessible_desc = NULL;
  gboolean visible = TRUE;

  g_return_if_fail (self->menu != NULL);
  g_return_if_fail (self->actions != NULL);

  if (!self->header_action ||
      !g_action_group_has_action (self->actions, self->header_action))
    {
      indicator_object_set_visible (INDICATOR_OBJECT (self), FALSE);
      return;
    }

  state = g_action_group_get_action_state (self->actions, self->header_action);
  if (state && g_variant_is_of_type (state, G_VARIANT_TYPE ("(sssb)")))
    {
      const gchar *iconstr = NULL;

      g_variant_get (state, "(&s&s&sb)", &label, &iconstr, &accessible_desc, &visible);

      if (iconstr)
        icon = g_variant_ref_sink (g_variant_new_string (iconstr));
    }
  else if (state && g_variant_is_of_type (state, G_VARIANT_TYPE ("a{sv}")))
    {
      g_variant_lookup (state, "label", "&s", &label);
      g_variant_lookup (state, "icon", "*", &icon);
      g_variant_lookup (state, "accessible-desc", "&s", &accessible_desc);
      g_variant_lookup (state, "visible", "b", &visible);
    }
  else
    g_warning ("the action of the indicator menu item must have state with type (sssb) or a{sv}");

  indicator_ng_set_label (self, label);
  indicator_ng_set_icon_from_variant (self, icon);
  indicator_ng_set_accessible_desc (self, accessible_desc);
  indicator_object_set_visible (INDICATOR_OBJECT (self), visible);

  if (icon)
    g_variant_unref (icon);
  if (state)
    g_variant_unref (state);
}

static gboolean
indicator_ng_menu_item_is_of_type (GMenuModel  *menu,
                                   gint         index,
                                   const gchar *expected_type)
{
  gchar *type;
  gboolean has_type = FALSE;

  if (g_menu_model_get_item_attribute (menu, index, "x-ayatana-type", "s", &type))
    {
      has_type = g_str_equal (type, expected_type);
      g_free (type);
    }

  return has_type;
}

static void
indicator_ng_menu_changed (GMenuModel *menu,
                           gint        position,
                           gint        removed,
                           gint        added,
                           gpointer    user_data)
{
  IndicatorNg *self = user_data;

  /* The menu may only contain one item (the indicator title menu).
   * Thus, the position is always 0, and there is either exactly one
   * item added or exactly one item removed.
   */
  g_return_if_fail (position == 0);
  g_return_if_fail (added < 2 && removed < 2 && added ^ removed);

  if (removed)
    indicator_object_set_visible (INDICATOR_OBJECT (self), FALSE);

  if (added)
    {
      g_clear_pointer (&self->header_action, g_free);
      g_clear_pointer (&self->scroll_action, g_free);
      g_clear_pointer (&self->secondary_action, g_free);

      if (indicator_ng_menu_item_is_of_type (self->menu, 0, "org.ayatana.indicator.root"))
        {
          GMenuModel *popup;
          gchar *action;

          if (g_menu_model_get_item_attribute (self->menu, 0, G_MENU_ATTRIBUTE_ACTION, "s", &action))
            {
              if (g_str_has_prefix (action, "indicator."))
                self->header_action = g_strdup (action + strlen ("indicator."));
              g_free (action);
            }

          if (g_menu_model_get_item_attribute (self->menu, 0, "x-ayatana-scroll-action", "s", &action))
            {
              if (g_str_has_prefix (action, "indicator."))
                self->scroll_action = g_strdup (action + strlen ("indicator."));
              g_free (action);
            }

          if (g_menu_model_get_item_attribute (self->menu, 0, "x-ayatana-secondary-action", "s", &action))
            {
              if (g_str_has_prefix (action, "indicator."))
                self->secondary_action = g_strdup (action + strlen ("indicator."));
              g_free (action);
            }

          if (g_menu_model_get_item_attribute (self->menu, 0, "submenu-action", "s", &action))
            {
              if (g_str_has_prefix (action, "indicator."))
                self->submenu_action = g_strdup (action + strlen ("indicator."));
              g_free (action);
            }

            for (guint nMenuSection = 0; nMenuSection < MENU_SECTIONS; nMenuSection++)
            {
                if (self->lMenuSections[nMenuSection])
                {
                    g_object_unref(self->lMenuSections[nMenuSection]);
                }
            }

          popup = g_menu_model_get_item_link (self->menu, 0, G_MENU_LINK_SUBMENU);
          if (popup)
            {
              gtk_menu_shell_bind_model (GTK_MENU_SHELL (self->entry.menu), popup, NULL, TRUE);
              g_object_unref (popup);
            }

          indicator_ng_update_entry (self);
        }
      else
        g_warning ("indicator menu item must be of type 'org.ayatana.indicator.root'");
    }
}

static void
indicator_ng_service_appeared (GDBusConnection *connection,
                               const gchar     *name,
                               const gchar     *name_owner,
                               gpointer         user_data)
{
  IndicatorNg *self = user_data;

  g_assert (!self->actions);
  g_assert (!self->menu);

  /* watch is not established when menu_object_path == NULL */
  g_assert (self->menu_object_path);

  self->session_bus = g_object_ref (connection);

  self->actions = G_ACTION_GROUP (g_dbus_action_group_get (connection, name_owner, self->object_path));
  gtk_widget_insert_action_group (GTK_WIDGET (self->entry.menu), "indicator", self->actions);
  g_signal_connect_swapped (self->actions, "action-added", G_CALLBACK (indicator_ng_update_entry), self);
  g_signal_connect_swapped (self->actions, "action-removed", G_CALLBACK (indicator_ng_update_entry), self);
  g_signal_connect_swapped (self->actions, "action-state-changed", G_CALLBACK (indicator_ng_update_entry), self);

  self->menu = G_MENU_MODEL (g_dbus_menu_model_get (connection, name_owner, self->menu_object_path));
  g_signal_connect (self->menu, "items-changed", G_CALLBACK (indicator_ng_menu_changed), self);
  if (g_menu_model_get_n_items (self->menu))
    indicator_ng_menu_changed (self->menu, 0, 0, 1, self);

  indicator_ng_update_entry (self);
}

static void
indicator_ng_service_started (GObject      *source_object,
                              GAsyncResult *res,
                              gpointer      user_data)
{
  IndicatorNg *self = user_data;
  GError *error = NULL;
  GVariant *result;
  guint32 start_service_reply;

  result = g_dbus_connection_call_finish (G_DBUS_CONNECTION (source_object), res, &error);
  if (!result)
    {
      g_warning ("Could not activate service '%s': %s", self->name, error->message);
      indicator_object_set_visible (INDICATOR_OBJECT (self), FALSE);
      g_error_free (error);
      return;
    }

  start_service_reply = 0;
  g_variant_get (result, "(u)", &start_service_reply);

  switch (start_service_reply)
    {
    case 1: /* DBUS_START_REPLY_SUCCESS */
      break;

    case 2: /* DBUS_START_REPLY_ALREADY_RUNNING */
      g_warning ("could not start service '%s': it is already running", self->name);
      break;

    default:
      g_assert_not_reached ();
    }

  g_variant_unref (result);
}

static void
indicator_ng_service_vanished (GDBusConnection *connection,
                               const gchar     *name,
                               gpointer         user_data)
{
  IndicatorNg *self = user_data;

  indicator_ng_free_actions_and_menu (self);

  /* Names may vanish because the service decided it doesn't need to
   * show its indicator anymore, or because it crashed.  Let's assume it
   * crashes and restart it unless it explicitly hid its indicator. */

  if (indicator_object_entry_is_visible (INDICATOR_OBJECT (self), &self->entry))
    {
      gint64 now;

      /* take care not to start it if it repeatedly crashes */
      now = g_get_monotonic_time ();
      if (now - self->last_service_restart < 1 * G_USEC_PER_SEC)
        {
          g_warning ("The indicator '%s' vanished too quickly after appearing. It won't "
                     "be respawned anymore, as it could be crashing repeatedly.", self->name);
          return;
        }

      self->last_service_restart = now;

      g_dbus_connection_call (self->session_bus,
                              "org.freedesktop.DBus",
                              "/",
                              "org.freedesktop.DBus",
                              "StartServiceByName",
                              g_variant_new ("(su)", self->bus_name, 0),
                              G_VARIANT_TYPE ("(u)"),
                              G_DBUS_CALL_FLAGS_NONE,
                              -1,
                              NULL,
                              indicator_ng_service_started,
                              self);
    }
}

/* Get an integer from a keyfile. Returns @default_value if the key
 * doesn't exist exists or is not an integer */
static gint
g_key_file_maybe_get_integer (GKeyFile    *keyfile,
                              const gchar *group,
                              const gchar *key,
                              gint         default_value)
{
  GError *error = NULL;
  gint i;

  i = g_key_file_get_integer (keyfile, group, key, &error);
  if (error)
    {
      g_error_free (error);
      return default_value;
    }

  return i;
}

static gboolean
indicator_ng_load_from_keyfile (IndicatorNg  *self,
                                GKeyFile     *keyfile,
                                GError      **error)
{
  g_assert (self->name == NULL);
  g_assert (self->object_path == NULL);
  g_assert (self->menu_object_path == NULL);

  self->name = g_key_file_get_string (keyfile, "Indicator Service", "Name", error);
  if (self->name == NULL)
    return FALSE;

  self->object_path = g_key_file_get_string (keyfile, "Indicator Service", "ObjectPath", error);
  if (self->object_path == NULL)
    return FALSE;

  self->position = g_key_file_maybe_get_integer (keyfile, "Indicator Service", "Position", -1);

  /*
   * Don't throw an error when the profile doesn't exist. Non-existant
   * profiles are silently ignored by not showing an indicator at all.
   */
  if (g_key_file_has_group (keyfile, self->profile))
    {
      /* however, if the profile exists, it must have "ObjectPath" */
      self->menu_object_path = g_key_file_get_string (keyfile, self->profile, "ObjectPath", error);
      if (self->menu_object_path == NULL)
        return FALSE;

      /* a position in the profile overrides the global one */
      self->position = g_key_file_maybe_get_integer (keyfile, self->profile, "Position", self->position);
    }

  return TRUE;
}

static gboolean
indicator_ng_initable_init (GInitable     *initable,
                            GCancellable  *cancellable,
                            GError       **error)
{
  IndicatorNg *self = INDICATOR_NG (initable);
  GKeyFile *keyfile;
  gboolean success = FALSE;

  self->bus_name = g_path_get_basename (self->service_file);

  keyfile = g_key_file_new ();
  if (g_key_file_load_from_file (keyfile, self->service_file, G_KEY_FILE_NONE, error) &&
      indicator_ng_load_from_keyfile (self, keyfile, error))
    {
      self->entry.name_hint = self->name;

      /* only watch the service when it supports the proile we're interested in */
      if (self->menu_object_path)
        {
          self->name_watch_id = g_bus_watch_name (G_BUS_TYPE_SESSION,
                                                  self->bus_name,
                                                  G_BUS_NAME_WATCHER_FLAGS_AUTO_START,
                                                  indicator_ng_service_appeared,
                                                  indicator_ng_service_vanished,
                                                  self, NULL);
        }

      success = TRUE;
    }

  g_key_file_free (keyfile);
  return success;
}

static void
indicator_ng_class_init (IndicatorNgClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  IndicatorObjectClass *io_class = INDICATOR_OBJECT_CLASS (class);

  object_class->get_property = indicator_ng_get_property;
  object_class->set_property = indicator_ng_set_property;
  object_class->dispose = indicator_ng_dispose;
  object_class->finalize = indicator_ng_finalize;

  io_class->get_entries = indicator_ng_get_entries;
  io_class->get_position = indicator_ng_get_position;
  io_class->entry_scrolled = indicator_ng_entry_scrolled;
  io_class->secondary_activate = indicator_ng_secondary_activate;

  properties[PROP_SERVICE_FILE] = g_param_spec_string ("service-file",
                                                       "Service file",
                                                       "Path of the service file",
                                                       NULL,
                                                       G_PARAM_READWRITE |
                                                       G_PARAM_CONSTRUCT_ONLY |
                                                       G_PARAM_STATIC_STRINGS);

  properties[PROP_PROFILE] = g_param_spec_string ("profile",
                                                  "Profile",
                                                  "Indicator profile",
                                                  "desktop",
                                                  G_PARAM_READWRITE |
                                                  G_PARAM_CONSTRUCT_ONLY |
                                                  G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
}

static void
indicator_ng_initable_iface_init (GInitableIface *initable)
{
  initable->init = indicator_ng_initable_init;
}

static void
indicator_ng_init (IndicatorNg *self)
{
    m_pActionMuxer = g_quark_from_static_string ("gtk-widget-action-muxer");

    for (guint nMenuSection = 0; nMenuSection < MENU_SECTIONS; nMenuSection++)
    {
        self->lMenuSections[nMenuSection] = NULL;
    }

  self->entry.label = (GtkLabel*)g_object_ref_sink (gtk_label_new (NULL));
  self->entry.image = (GtkImage*)g_object_ref_sink (gtk_image_new ());

  self->entry.menu = (GtkMenu*)g_object_ref_sink (gtk_menu_new ());

  g_signal_connect (self->entry.menu, "show", G_CALLBACK (indicator_ng_menu_shown), self);
  g_signal_connect (self->entry.menu, "hide", G_CALLBACK (indicator_ng_menu_hidden), self);
  g_signal_connect (self->entry.menu, "size-allocate", G_CALLBACK (indicator_ng_menu_size_allocate), self);

    GtkCssProvider *pCssProvider = gtk_css_provider_new();
    GtkStyleContext *pStyleContext = gtk_widget_get_style_context(GTK_WIDGET(self->entry.menu));
    gtk_style_context_add_provider(pStyleContext, GTK_STYLE_PROVIDER(pCssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_data(pCssProvider, "menu > arrow{min-height: 0; padding: 0; margin: 0;}", -1, NULL);

    GtkWidget *pWindow = gtk_widget_get_parent(GTK_WIDGET(self->entry.menu));
    pStyleContext = gtk_widget_get_style_context(pWindow);
    gtk_style_context_add_provider(pStyleContext, GTK_STYLE_PROVIDER(pCssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_data(pCssProvider, "window > decoration {box-shadow: 0 1px 2px rgba(0,0,0,0.2), 0 0 0 1px rgba(0,0,0,0.13);}", -1, NULL);

    g_object_unref(pCssProvider);

  /* work around IndicatorObject's warning that the accessible
   * description is missing. We never set it on construction, but when
   * the menu model has arrived on the bus.
   */
  self->accessible_desc = g_strdup ("");
  self->entry.accessible_desc = self->accessible_desc;

  self->position = -1;

  indicator_object_set_visible (INDICATOR_OBJECT (self), FALSE);
}

IndicatorNg *
indicator_ng_new (const gchar  *service_file,
                  GError      **error)
{
  return g_initable_new (INDICATOR_TYPE_NG, NULL, error,
                         "service-file", service_file,
                         NULL);
}

IndicatorNg *
indicator_ng_new_for_profile (const gchar  *service_file,
                              const gchar  *profile,
                              GError      **error)
{
  return g_initable_new (INDICATOR_TYPE_NG, NULL, error,
                         "service-file", service_file,
                         "profile", profile,
                         NULL);
}

const gchar *
indicator_ng_get_service_file (IndicatorNg *self)
{
  g_return_val_if_fail (INDICATOR_IS_NG (self), NULL);

  return self->service_file;
}

const gchar *
indicator_ng_get_profile (IndicatorNg *self)
{
  g_return_val_if_fail (INDICATOR_IS_NG (self), NULL);

  return self->profile;
}
