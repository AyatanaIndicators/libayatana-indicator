/*
A test for libindicate to ensure its quality.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This program is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License version 3, as published 
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <glib.h>
#include "libindicate/indicator.h"

#define ALOT  30


int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateIndicator * indicators[ALOT];
	int i;

	for (i = 0; i < ALOT; i++) {
		indicators[i] = indicate_indicator_new();
		indicate_indicator_show(indicators[i]);
	}

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
