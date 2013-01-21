
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

  GtkWidget *label;
  GtkWidget *image;
  GtkWidget *gtkmenu;
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
      gtk_widget_insert_action_group (self->gtkmenu, self->name, self->actions);
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

  g_clear_object (&self->label);
  g_clear_object (&self->image);
  g_clear_object (&self->gtkmenu);

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

static GtkLabel *
indicator_ng_get_label (IndicatorObject *io)
{
  IndicatorNg *self = INDICATOR_NG (io);

  gtk_widget_show (self->label);

  return GTK_LABEL (self->label);
}

static GtkImage *
indicator_ng_get_image (IndicatorObject *io)
{
  IndicatorNg *self = INDICATOR_NG (io);

  gtk_widget_show (self->image);

  return GTK_IMAGE (self->image);
}

static GtkMenu *
indicator_ng_get_menu (IndicatorObject *io)
{
  IndicatorNg *self = INDICATOR_NG (io);

  return GTK_MENU (self->gtkmenu);
}

static const gchar *
indicator_ng_get_accessible_desc (IndicatorObject *io)
{
  IndicatorNg *self = INDICATOR_NG (io);

  return self->accessible_desc;
}

static const gchar *
indicator_ng_get_name_hint (IndicatorObject *io)
{
  IndicatorNg *self = INDICATOR_NG (io);

  return self->name;
}

static void
indicator_ng_set_accessible_desc (IndicatorNg *self,
                                  const gchar *accessible_desc)
{
  GList *entries;

  g_free (self->accessible_desc);

  self->accessible_desc = g_strdup (accessible_desc);

  entries = indicator_object_get_entries (INDICATOR_OBJECT (self));
  g_return_if_fail (entries != NULL);

  g_signal_emit_by_name (self, INDICATOR_OBJECT_SIGNAL_ACCESSIBLE_DESC_UPDATE, entries->data);

  g_list_free (entries);
}

static gboolean
gtk_image_set_from_gicon_string (GtkImage    *img,
                                 const gchar *str)
{
  GIcon *icon;
  GError *error = NULL;

  icon = str ? g_icon_new_for_string (str, &error) : NULL;
  if (icon)
    {
      gtk_image_set_from_gicon (img, icon, GTK_ICON_SIZE_LARGE_TOOLBAR);
      g_object_unref (icon);
      return TRUE;
    }
  else
    {
      if (error)
        {
          g_warning ("invalid icon string '%s': %s", str, error->message);
          g_error_free (error);
        }
      return FALSE;
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

      gtk_label_set_label (GTK_LABEL (self->label), label);
      if (!gtk_image_set_from_gicon_string (GTK_IMAGE (self->image), iconstr))
        gtk_widget_hide (self->image);
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
          gtk_menu_shell_bind_model (GTK_MENU_SHELL (self->gtkmenu), popup, NULL, TRUE);
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
  gtk_widget_insert_action_group (self->gtkmenu, "indicator", self->actions);
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

      self->name_watch_id = g_bus_watch_name (G_BUS_TYPE_SESSION,
                                              bus_name,
                                              G_BUS_NAME_WATCHER_FLAGS_NONE,
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

  io_class->get_label = indicator_ng_get_label;
  io_class->get_image = indicator_ng_get_image;
  io_class->get_menu = indicator_ng_get_menu;
  io_class->get_accessible_desc = indicator_ng_get_accessible_desc;
  io_class->get_name_hint = indicator_ng_get_name_hint;

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
  self->label = g_object_ref_sink (gtk_label_new (NULL));
  self->image = g_object_ref_sink (gtk_image_new ());
  self->gtkmenu = g_object_ref_sink (gtk_menu_new ());

  /* work around IndicatorObject's warning that the accessible
   * description is missing. We never set it on construction, but when
   * the menu model has arrived on the bus.
   */
  self->accessible_desc = g_strdup ("");

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
