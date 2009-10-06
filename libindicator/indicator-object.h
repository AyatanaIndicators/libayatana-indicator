#ifndef __INDICATOR_OBJECT_H__
#define __INDICATOR_OBJECT_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define INDICATOR_OBJECT_TYPE            (indicator_object_get_type ())
#define INDICATOR_OBJECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_OBJECT_TYPE, IndicatorObject))
#define INDICATOR_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_OBJECT_TYPE, IndicatorObjectClass))
#define IS_INDICATOR_OBJECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_OBJECT_TYPE))
#define IS_INDICATOR_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_OBJECT_TYPE, IndicatorObjectClass))

typedef struct _IndicatorObject      IndicatorObject;
typedef struct _IndicatorObjectClass IndicatorObjectClass;

struct _IndicatorObjectClass {
	GObjectClass parent_class;

};

struct _IndicatorObject {
	GObject parent;

};

GType indicator_object_get_type (void);

G_END_DECLS

#endif
