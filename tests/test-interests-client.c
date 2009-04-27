
#include <glib.h>
#include "libindicate/indicator.h"
#include "libindicate/server.h"
#include "libindicate/interests.h"

static gboolean passed = TRUE;
static GMainLoop * mainloop = NULL;
static gboolean interests[INDICATE_INTEREST_LAST] = {0};

static gboolean
check_interests (void)
{
	guint i;
	for (i = INDICATE_INTEREST_NONE + 1; i < INDICATE_INTEREST_LAST; i++) {
		if (!interests[i]) {
			return FALSE;
		}
	}

	return TRUE;
}

static void
interest_added (IndicateServer * server, IndicateInterests interest)
{
	g_debug("Oh, someone is interested in my for: %d", interest);
	interests[interest] = TRUE;

	if (check_interests()) {
		g_main_loop_quit(mainloop);
	}

	return;
}

static gboolean
done_timeout_cb (gpointer data)
{
	g_debug("All interests not set");
	passed = FALSE;
	g_main_loop_quit(mainloop);
	return FALSE;
}

int
main (int argc, char * argv)
{
	g_type_init();

	IndicateIndicator * indicator = indicate_indicator_new();
	indicate_indicator_show(indicator);

	IndicateServer * server = indicate_server_ref_default();
	g_signal_connect(G_OBJECT(server), INDICATE_SERVER_SIGNAL_INTEREST_ADDED, G_CALLBACK(interest_added), NULL);

	g_timeout_add_seconds(2, done_timeout_cb, indicator);

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);

	return !passed;
}
