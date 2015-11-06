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

#ifndef __LIBINDICATOR_INDICATOR_H_SEEN__
#define __LIBINDICATOR_INDICATOR_H_SEEN__ 1

#include <gtk/gtk.h>

#define INDICATOR_GET_VERSION_S "get_version"
typedef gchar * (*get_version_t) (void);
gchar * get_version (void);

#define INDICATOR_VERSION "0.3.0"
#define INDICATOR_SET_VERSION  gchar * get_version(void) { return INDICATOR_VERSION; }
#define INDICATOR_VERSION_CHECK(x)  (!g_strcmp0(x, INDICATOR_VERSION))

#define INDICATOR_GET_TYPE_S "get_type"
typedef GType (*get_type_t) (void);
#define INDICATOR_SET_TYPE(x)  GType get_type (void) { return x; }

#endif /* __LIBINDICATOR_INDICATOR_H_SEEN__ */

