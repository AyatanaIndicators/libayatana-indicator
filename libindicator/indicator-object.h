/*
An interface for indicators to link to for creation.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 3.0 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License version 3.0 for more details.

You should have received a copy of the GNU General Public
License along with this library. If not, see
<http://www.gnu.org/licenses/>.
*/

#ifndef __INDICATOR_OBJECT_H__
#define __INDICATOR_OBJECT_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define INDICATOR_OBJECT_TYPE            (indicator_object_get_type ())
#define INDICATOR_OBJECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_OBJECT_TYPE, IndicatorObject))
#define INDICATOR_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_OBJECT_TYPE, IndicatorObjectClass))
#define INDICATOR_IS_OBJECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_OBJECT_TYPE))
#define INDICATOR_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_OBJECT_TYPE))
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
IndicatorObject * indicator_object_new_from_file (const gchar * file);

GtkLabel * indicator_object_get_label (IndicatorObject * io);
GtkImage * indicator_object_get_icon (IndicatorObject * io);
GtkMenu * indicator_object_get_menu (IndicatorObject * io);

G_END_DECLS

#endif
