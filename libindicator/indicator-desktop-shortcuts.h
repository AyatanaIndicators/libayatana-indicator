#ifndef __INDICATOR_DESKTOP_SHORTCUTS_H__
#define __INDICATOR_DESKTOP_SHORTCUTS_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define INDICATOR_TYPE_DESKTOP_SHORTCUTS            (indicator_desktop_shortcuts_get_type ())
#define INDICATOR_DESKTOP_SHORTCUTS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_TYPE_DESKTOP_SHORTCUTS, IndicatorDesktopShortcuts))
#define INDICATOR_DESKTOP_SHORTCUTS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_TYPE_DESKTOP_SHORTCUTS, IndicatorDesktopShortcutsClass))
#define IS_INDICATOR_DESKTOP_SHORTCUTS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_TYPE_DESKTOP_SHORTCUTS))
#define IS_INDICATOR_DESKTOP_SHORTCUTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_TYPE_DESKTOP_SHORTCUTS))
#define INDICATOR_DESKTOP_SHORTCUTS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_TYPE_DESKTOP_SHORTCUTS, IndicatorDesktopShortcutsClass))

typedef struct _IndicatorDesktopShortcuts      IndicatorDesktopShortcuts;
typedef struct _IndicatorDesktopShortcutsClass IndicatorDesktopShortcutsClass;

struct _IndicatorDesktopShortcutsClass {
	GObjectClass parent_class;
};

struct _IndicatorDesktopShortcuts {
	GObject parent;
};

GType indicator_desktop_shortcuts_get_type (void);

G_END_DECLS

#endif
