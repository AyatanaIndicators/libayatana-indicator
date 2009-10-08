
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator.h"
#include "indicator-object.h"

typedef struct _IndicatorObjectPrivate IndicatorObjectPrivate;
struct _IndicatorObjectPrivate {
	GtkLabel * label;
	GtkImage * icon;
	GtkMenu * menu;
};

#define INDICATOR_OBJECT_GET_PRIVATE(o) \
			(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATOR_OBJECT_TYPE, IndicatorObjectPrivate))

static void indicator_object_class_init (IndicatorObjectClass *klass);
static void indicator_object_init       (IndicatorObject *self);
static void indicator_object_dispose    (GObject *object);
static void indicator_object_finalize   (GObject *object);

G_DEFINE_TYPE (IndicatorObject, indicator_object, G_TYPE_OBJECT);

static void
indicator_object_class_init (IndicatorObjectClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorObjectPrivate));

	object_class->dispose = indicator_object_dispose;
	object_class->finalize = indicator_object_finalize;

	return;
}

static void
indicator_object_init (IndicatorObject *self)
{
	IndicatorObjectPrivate * priv = INDICATOR_OBJECT_GET_PRIVATE(self);

	priv->label = NULL;
	priv->icon = NULL;
	priv->menu = NULL;

	return;
}

static void
indicator_object_dispose (GObject *object)
{
	IndicatorObjectPrivate * priv = INDICATOR_OBJECT_GET_PRIVATE(object);
	
	if (priv->label != NULL) {
		g_object_unref(priv->label);
		priv->label = NULL;
	}

	if (priv->icon != NULL) {
		g_object_unref(priv->icon);
		priv->icon = NULL;
	}

	if (priv->menu != NULL) {
		g_object_unref(priv->menu);
		priv->menu = NULL;
	}

	G_OBJECT_CLASS (indicator_object_parent_class)->dispose (object);
	return;
}

static void
indicator_object_finalize (GObject *object)
{

	G_OBJECT_CLASS (indicator_object_parent_class)->finalize (object);
	return;
}

IndicatorObject *
indicator_object_new_from_file (const gchar * file)
{
	if (file == NULL) {
		g_warning("Invalid filename.");
		return NULL;
	}

	GModule * module = g_module_open(file,
                                     G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
	if(module != NULL) {
		g_warning("Unable to load module: %s", file);
		return NULL;
	}

	get_version_t lget_version = NULL;
	if (!g_module_symbol(module, INDICATOR_GET_VERSION_S, (gpointer *)(&lget_version))) {
		g_warning("Unable to get the symbol for getting the version.");
		return NULL;
	}

	if (!INDICATOR_VERSION_CHECK(lget_version())) {
		g_warning("Indicator using API version '%s' we're expecting '%s'", lget_version(), INDICATOR_VERSION);
		return NULL;
	}

	GObject * object = g_object_new(INDICATOR_OBJECT_TYPE, NULL);
	IndicatorObjectPrivate * priv = INDICATOR_OBJECT_GET_PRIVATE(object);

	/* The function for grabbing a label from the module
	   execute it, and make sure everything is a-okay */
	get_label_t lget_label = NULL;
	if (!g_module_symbol(module, INDICATOR_GET_LABEL_S, (gpointer *)(&lget_label))) {
		g_warning("Unable to get '" INDICATOR_GET_LABEL_S "' symbol from module: %s", file);
		goto unrefandout;
	}
	if (lget_label == NULL) {
		g_warning("Symbol '" INDICATOR_GET_LABEL_S "' is (null) in module: %s", file);
		goto unrefandout;
	}
	priv->label = lget_label();

	/* The function for grabbing an icon from the module
	   execute it, and make sure everything is a-okay */
	get_icon_t lget_icon = NULL;
	if (!g_module_symbol(module, INDICATOR_GET_ICON_S, (gpointer *)(&lget_icon))) {
		g_warning("Unable to get '" INDICATOR_GET_ICON_S "' symbol from module: %s", file);
		goto unrefandout;
	}
	if (lget_icon == NULL) {
		g_warning("Symbol '" INDICATOR_GET_ICON_S "' is (null) in module: %s", file);
		goto unrefandout;
	}
	priv->icon = lget_icon();

	/* The function for grabbing a menu from the module
	   execute it, and make sure everything is a-okay */
	get_menu_t lget_menu = NULL;
	if (!g_module_symbol(module, INDICATOR_GET_MENU_S, (gpointer *)(&lget_menu))) {
		g_warning("Unable to get '" INDICATOR_GET_MENU_S "' symbol from module: %s", file);
		goto unrefandout;
	}
	if (lget_menu == NULL) {
		g_warning("Symbol '" INDICATOR_GET_MENU_S "' is (null) in module: %s", file);
		goto unrefandout;
	}
	priv->menu = lget_menu();

	if (priv->label == NULL && priv->icon == NULL) {
		/* This is the case where there is nothing to display,
		   kinda odd that we'd have a module with nothing. */
		g_warning("No label or icon.  Odd.");
		g_object_unref(object);
		return NULL;
	}

	return INDICATOR_OBJECT(object);

unrefandout:
	g_object_unref(object);
	return NULL;
}
