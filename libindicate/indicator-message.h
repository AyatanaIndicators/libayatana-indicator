/*
A library to allow applictions to provide simple indications of
information to be displayed to users of the application through the
interface shell.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This program is free software: you can redistribute it and/or modify it 
under the terms of either or both of the following licenses:

1) the GNU Lesser General Public License version 3, as published by the 
Free Software Foundation; and/or
2) the GNU Lesser General Public License version 2.1, as published by 
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the applicable version of the GNU Lesser General Public 
License for more details.

You should have received a copy of both the GNU Lesser General Public 
License version 3 and version 2.1 along with this program.  If not, see 
<http://www.gnu.org/licenses/>
*/

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
