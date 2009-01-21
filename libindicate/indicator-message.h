#ifndef __INDICATE_INDICATOR_MESSAGE_H__
#define __INDICATE_INDICATOR_MESSAGE_H__

#include <glib.h>
#include <glib-object.h>

#include "indicator.h"

G_BEGIN_DECLS

#define INDICATE_TYPE_INDICATOR_MESSAGE            (indicate_indicator_message_get_type ())
#define INDICATE_INDICATOR_MESSAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATE_TYPE_INDICATOR_MESSAGE, IndicateIndicatorMessage))
#define INDICATE_INDICATOR_MESSAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATE_TYPE_INDICATOR_MESSAGE, IndicateIndicatorMessageClass))
#define INDICATE_IS_INDICATOR_MESSAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATE_TYPE_INDICATOR_MESSAGE))
#define INDICATE_IS_INDICATOR_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATE_TYPE_INDICATOR_MESSAGE))
#define INDICATE_INDICATOR_MESSAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATE_TYPE_INDICATOR_MESSAGE, IndicateIndicatorMessageClass))

typedef struct _IndicateIndicatorMessage      IndicateIndicatorMessage;
typedef struct _IndicateIndicatorMessageClass IndicateIndicatorMessageClass;

struct _IndicateIndicatorMessageClass
{
IndicateIndicatorClass parent_class;
};

struct _IndicateIndicatorMessage
{
IndicateIndicator parent;
};

GType indicate_indicator_message_get_type (void);
IndicateIndicatorMessage * indicate_indicator_message_new (void);

G_END_DECLS

#endif
