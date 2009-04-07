/* From LP: #351537 */

#include <glib.h>
#include "libindicate/server.h"
#include "libindicate/indicator-message.h"

gboolean hidden = TRUE;

static gboolean
timeout_cb (gpointer data)
{
    IndicateServer * server = INDICATE_SERVER(data);

	if (hidden) {
        printf("showing... ");
        indicate_server_show(server);
        printf("ok\n");
        hidden = FALSE;
	} else {
        printf("hiding... ");
        indicate_server_hide(server);
        printf("ok\n");
        hidden = TRUE;
	}

	return TRUE;
}


int
main (int argc, char ** argv)
{
	g_type_init();

	IndicateServer * server = indicate_server_ref_default();
	indicate_server_set_type(server, "message.im");
	indicate_server_set_desktop_file(server, "/usr/share/applications/empathy.desktop");
	g_timeout_add_seconds(1, timeout_cb, server);

	g_main_loop_run(g_main_loop_new(NULL, FALSE));

	return 0;
}

