#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator-desktop-shortcuts.h"

typedef struct _IndicatorDesktopShortcutsPrivate IndicatorDesktopShortcutsPrivate;
struct _IndicatorDesktopShortcutsPrivate {
	GKeyFile * keyfile;
};

#define INDICATOR_DESKTOP_SHORTCUTS_GET_PRIVATE(o) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATOR_TYPE_DESKTOP_SHORTCUTS, IndicatorDesktopShortcutsPrivate))

static void indicator_desktop_shortcuts_class_init (IndicatorDesktopShortcutsClass *klass);
static void indicator_desktop_shortcuts_init       (IndicatorDesktopShortcuts *self);
static void indicator_desktop_shortcuts_dispose    (GObject *object);
static void indicator_desktop_shortcuts_finalize   (GObject *object);

G_DEFINE_TYPE (IndicatorDesktopShortcuts, indicator_desktop_shortcuts, G_TYPE_OBJECT);

static void
indicator_desktop_shortcuts_class_init (IndicatorDesktopShortcutsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorDesktopShortcutsPrivate));

	object_class->dispose = indicator_desktop_shortcuts_dispose;
	object_class->finalize = indicator_desktop_shortcuts_finalize;

	return;
}

static void
indicator_desktop_shortcuts_init (IndicatorDesktopShortcuts *self)
{
	IndicatorDesktopShortcutsPrivate * priv = INDICATOR_DESKTOP_SHORTCUTS_GET_PRIVATE(self);

	priv->keyfile = NULL;

	return;
}

static void
indicator_desktop_shortcuts_dispose (GObject *object)
{

	G_OBJECT_CLASS (indicator_desktop_shortcuts_parent_class)->dispose (object);
	return;
}

static void
indicator_desktop_shortcuts_finalize (GObject *object)
{

	G_OBJECT_CLASS (indicator_desktop_shortcuts_parent_class)->finalize (object);
	return;
}

