/*
A little helper to make a themed image with fallbacks that
is only constrained in the vertical dimention.

Copyright 2010 Canonical Ltd.

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

#ifndef __INDICATOR_IMAGE_HELPER_H__
#define __INDICATOR_IMAGE_HELPER_H__

#include <gtk/gtk.h>

GtkImage *   indicator_image_helper                     (const gchar * name);
void         indicator_image_helper_update              (GtkImage * image,
                                                         const gchar * name);
void         indicator_image_helper_update_from_gicon   (GtkImage * image,
                                                         GIcon * icon);

#endif /* __INDICATOR_IMAGE_HELPER_H__ */
