
#include <glib.h>
#include "libindicate/listener.h"


int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateListener * listener = indicate_listener_new();

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}
