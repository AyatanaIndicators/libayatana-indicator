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

#include <string.h>

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

  guint name_watch_id;

  GDBusConnection *session_bus;
  GActionGroup *actions;
  GMenuModel *menu;

  IndicatorObjectEntry entry;
  gchar *accessible_desc;

  gint64 last_service_restart;
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

  G_OBJECT_CLASS (indicator_ng_parent_class)->finalize (object);
}

static GList *
indicator_ng_get_entries (IndicatorObject *io)
{
  IndicatorNg *self = INDICATOR_NG (io);

  return g_list_append (NULL, &self->entry);
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

  if (!self->entry.image)
    self->entry.image = g_object_ref_sink (gtk_image_new ());

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
      gtk_image_set_from_stock (self->entry.image, GTK_STOCK_MISSING_IMAGE, GTK_ICON_SIZE_LARGE_TOOLBAR);
      g_free (text);
    }
}

static void
indicator_ng_set_label (IndicatorNg *self,
                        const gchar *label)
{
  if (label == NULL || *label == '\0')
    {
      if (self->entry.label)
        gtk_widget_hide (GTK_WIDGET (self->entry.label));
      return;
    }

  if (!self->entry.label)
    self->entry.label = g_object_ref_sink (gtk_label_new (NULL));

  gtk_label_set_label (GTK_LABEL (self->entry.label), label);
  gtk_widget_show (GTK_WIDGET (self->entry.label));
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

  if (g_menu_model_get_item_attribute (menu, index, "x-canonical-type", "s", &type))
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

      if (indicator_ng_menu_item_is_of_type (self->menu, 0, "com.canonical.indicator.root"))
        {
          GMenuModel *popup;
          gchar *action;

          if (g_menu_model_get_item_attribute (self->menu, 0, G_MENU_ATTRIBUTE_ACTION, "s", &action) &&
              g_str_has_prefix (action, "indicator."))
            {
              self->header_action = g_strdup (action + strlen ("indicator."));
            }

          popup = g_menu_model_get_item_link (self->menu, 0, G_MENU_LINK_SUBMENU);
          if (popup)
            {
              gtk_menu_shell_bind_model (GTK_MENU_SHELL (self->entry.menu), popup, NULL, TRUE);
              g_object_unref (popup);
            }

          indicator_ng_update_entry (self);

          g_free (action);
        }
      else
        g_warning ("indicator menu item must be of type 'com.canonical.indicator.root'");
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
                              GAsyncResult *result,
                              gpointer      user_data)
{
  IndicatorNg *self = user_data;
  GError *error = NULL;
  GVariant *reply;

  reply = g_dbus_connection_call_finish (G_DBUS_CONNECTION (source_object), result, &error);
  if (!reply)
    {
      g_warning ("Could not activate service '%s': %s", self->name, error->message);
      indicator_object_set_visible (INDICATOR_OBJECT (self), FALSE);
      g_error_free (error);
      return;
    }

  switch (g_variant_get_uint32 (reply))
    {
    case 1: /* DBUS_START_REPLY_SUCCESS */
      break;

    case 2: /* DBUS_START_REPLY_ALREADY_RUNNING */
      g_warning ("could not start service '%s': it is already running", self->name);
      break;

    default:
      g_assert_not_reached ();
    }

  g_variant_unref (reply);
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
        return;

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
  self->entry.menu = g_object_ref_sink (gtk_menu_new ());

  /* work around IndicatorObject's warning that the accessible
   * description is missing. We never set it on construction, but when
   * the menu model has arrived on the bus.
   */
  self->accessible_desc = g_strdup ("");
  self->entry.accessible_desc = self->accessible_desc;

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