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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator-message.h"

typedef struct _IndicateIndicatorMessagePrivate IndicateIndicatorMessagePrivate;

struct _IndicateIndicatorMessagePrivate
{
	gchar * subtype;
};

#define INDICATE_INDICATOR_MESSAGE_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATE_INDICATOR_MESSAGE_TYPE, IndicateIndicatorMessagePrivate))

static void     indicate_indicator_message_class_init (IndicateIndicatorMessageClass *klass);
static void     indicate_indicator_message_init       (IndicateIndicatorMessage *self);
static void     indicate_indicator_message_dispose    (GObject *object);
static void     indicate_indicator_message_finalize   (GObject *object);
static const gchar *  get_indicator_type                    (IndicateIndicator * indicator);

G_DEFINE_TYPE (IndicateIndicatorMessage, indicate_indicator_message, INDICATE_TYPE_INDICATOR);

static void
indicate_indicator_message_class_init (IndicateIndicatorMessageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicateIndicatorMessagePrivate));

	object_class->dispose = indicate_indicator_message_dispose;
	object_class->finalize = indicate_indicator_message_finalize;

	IndicateIndicatorClass * indicator_class = INDICATE_INDICATOR_CLASS(klass);

	indicator_class->get_type = get_indicator_type;

	return;
}

static void
indicate_indicator_message_init (IndicateIndicatorMessage *self)
{
}

static void
indicate_indicator_message_dispose (GObject *object)
{
G_OBJECT_CLASS (indicate_indicator_message_parent_class)->dispose (object);
}

static void
indicate_indicator_message_finalize (GObject *object)
{
G_OBJECT_CLASS (indicate_indicator_message_parent_class)->finalize (object);
}

/**
	indicate_indicator_message_new:

	Builds a new indicator message object using #g_object_new.

	Return value: A pointer to a new #IndicateIndicatorMessage object.
*/
static const gchar *
get_indicator_type (IndicateIndicator * indicator)
{
	return "message";
}

IndicateIndicatorMessage *
indicate_indicator_message_new (void)
{
	return g_object_new(INDICATE_TYPE_INDICATOR_MESSAGE, NULL);
}
