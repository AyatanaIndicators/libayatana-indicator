
#include "indicator-ng.h"

#include <string.h>

struct _IndicatorNg
{
  IndicatorObject parent;

  gchar *service_file;
  gchar *name;
  gchar *object_path;
  gchar *profile;
  gchar *header_action;

  guint name_watch_id;

  GActionGroup *actions;
  GMenuModel *menu;

  IndicatorObjectEntry entry;
  gchar *accessible_desc;
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
indicator_ng_set_icon_from_string (IndicatorNg *self,
                                   const gchar *str)
{
  GIcon *icon;
  GError *error = NULL;

  if (str == NULL || *str == '\0')
    {
      gtk_image_clear (self->entry.image);
      gtk_widget_hide (GTK_WIDGET (self->entry.image));
      return;
    }

  gtk_widget_show (GTK_WIDGET (self->entry.image));

  icon = g_icon_new_for_string (str, &error);
  if (icon)
    {
      gtk_image_set_from_gicon (self->entry.image, icon, GTK_ICON_SIZE_LARGE_TOOLBAR);
      g_object_unref (icon);
    }
  else
    {
      g_warning ("invalid icon string '%s': %s", str, error->message);
      gtk_image_set_from_stock (self->entry.image, GTK_STOCK_MISSING_IMAGE, GTK_ICON_SIZE_LARGE_TOOLBAR);
      g_error_free (error);
    }
}

static void
indicator_ng_update_entry (IndicatorNg *self)
{
  GVariant *state;

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
      gchar *label;
      gchar *iconstr;
      gchar *accessible_desc;
      gboolean visible;

      g_variant_get (state, "(sssb)", &label, &iconstr, &accessible_desc, &visible);

      gtk_label_set_label (GTK_LABEL (self->entry.label), label);
      indicator_ng_set_icon_from_string (self, iconstr);
      indicator_ng_set_accessible_desc (self, accessible_desc);
      indicator_object_set_visible (INDICATOR_OBJECT (self), visible);

      g_free (label);
      g_free (iconstr);
      g_free (accessible_desc);
    }
  else
    g_warning ("the action of the indicator menu item must have state with type (sssb)");

  if (state)
    g_variant_unref (state);
}

static void
indicator_ng_menu_changed (GMenuModel *menu,
                           gint        position,
                           gint        removed,
                           gint        added,
                           gpointer    user_data)
{
  IndicatorNg *self = user_data;

  g_return_if_fail (position == 0);
  g_return_if_fail (added < 2 && removed < 2 && added ^ removed);

  if (removed)
    indicator_object_set_visible (INDICATOR_OBJECT (self), FALSE);

  if (added)
    {
      GMenuModel *popup;
      gchar *action;

      g_clear_pointer (&self->header_action, g_free);
      g_menu_model_get_item_attribute (self->menu, 0, G_MENU_ATTRIBUTE_ACTION, "s", &action);
      if (action && g_str_has_prefix (action, "indicator."))
        self->header_action = g_strdup (action + 10);

      popup = g_menu_model_get_item_link (self->menu, 0, G_MENU_LINK_SUBMENU);
      if (popup)
        {
          gtk_menu_shell_bind_model (GTK_MENU_SHELL (self->entry.menu), popup, NULL, TRUE);
          g_object_unref (popup);
        }

      indicator_ng_update_entry (self);

      g_free (action);
    }
}

static void
indicator_ng_service_appeared (GDBusConnection *connection,
                               const gchar     *name,
                               const gchar     *name_owner,
                               gpointer         user_data)
{
  IndicatorNg *self = user_data;
  gchar *menu_object_path;

  g_assert (!self->actions);
  g_assert (!self->menu);

  self->actions = G_ACTION_GROUP (g_dbus_action_group_get (connection, name_owner, self->object_path));
  gtk_widget_insert_action_group (GTK_WIDGET (self->entry.menu), "indicator", self->actions);
  g_signal_connect_swapped (self->actions, "action-added", G_CALLBACK (indicator_ng_update_entry), self);
  g_signal_connect_swapped (self->actions, "action-removed", G_CALLBACK (indicator_ng_update_entry), self);
  g_signal_connect_swapped (self->actions, "action-state-changed", G_CALLBACK (indicator_ng_update_entry), self);

  menu_object_path = g_strconcat (self->object_path, "/", self->profile, NULL);
  self->menu = G_MENU_MODEL (g_dbus_menu_model_get (connection, name_owner, menu_object_path));
  g_signal_connect (self->menu, "items-changed", G_CALLBACK (indicator_ng_menu_changed), self);
  if (g_menu_model_get_n_items (self->menu))
    indicator_ng_menu_changed (self->menu, 0, 0, 1, self);

  indicator_ng_update_entry (self);

  g_free (menu_object_path);
}

static void
indicator_ng_service_vanished (GDBusConnection *connection,
                               const gchar     *name,
                               gpointer         user_data)
{
  IndicatorNg *self = user_data;

  indicator_ng_free_actions_and_menu (self);

  indicator_object_set_visible (INDICATOR_OBJECT (self), FALSE);
}

static gboolean
indicator_ng_initable_init (GInitable     *initable,
                            GCancellable  *cancellable,
                            GError       **error)
{
  IndicatorNg *self = INDICATOR_NG (initable);
  GKeyFile *keyfile;
  gchar *bus_name = NULL;

  keyfile = g_key_file_new ();
  if (!g_key_file_load_from_file (keyfile,
                                  self->service_file,
                                  G_KEY_FILE_NONE,
                                  error))
    {
      g_key_file_free (keyfile);
      return FALSE;
    }

  if ((self->name = g_key_file_get_string (keyfile, "Indicator Service", "Name", error)) &&
      (bus_name = g_key_file_get_string (keyfile, "Indicator Service", "BusName", error)) &&
      (self->object_path = g_key_file_get_string (keyfile, "Indicator Service", "ObjectPath", error)))
    {
      self->entry.name_hint = self->name;

      self->name_watch_id = g_bus_watch_name (G_BUS_TYPE_SESSION,
                                              bus_name,
                                              G_BUS_NAME_WATCHER_FLAGS_AUTO_START,
                                              indicator_ng_service_appeared,
                                              indicator_ng_service_vanished,
                                              self, NULL);
    }

  g_free (bus_name);
  g_key_file_free (keyfile);

  return self->name_watch_id > 0;
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
  self->entry.label = g_object_ref_sink (gtk_label_new (NULL));
  gtk_widget_show (GTK_WIDGET (self->entry.label));

  self->entry.image = g_object_ref_sink (gtk_image_new ());
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
