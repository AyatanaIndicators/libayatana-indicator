/*
 * Test that demonstrates the GtkCssProvider memory leak in
 * indicator_ng_set_label (before fix) and validates the fix.
 *
 * Before the fix, every call to indicator_ng_set_label() created a new
 * GtkCssProvider and added it to the label's GtkStyleContext without
 * removing the old one. Since the datetime indicator updates the label
 * once per second, this leaked ~2 KB/sec (~170 MB/day).
 *
 * This test calls set_label in a loop and measures heap growth via
 * /proc/self/statm to show the leak is real and the fix works.
 */

#include <gtk/gtk.h>
#include <stdio.h>

static long
get_rss_kb (void)
{
    long rss = 0;
    FILE *f = fopen ("/proc/self/statm", "r");
    if (f) {
        long size, resident;
        if (fscanf (f, "%ld %ld", &size, &resident) == 2)
            rss = resident * (sysconf (_SC_PAGESIZE) / 1024);
        fclose (f);
    }
    return rss;
}

/*
 * Simulate what indicator_ng_set_label does using the UNFIXED code:
 * creates and adds a new GtkCssProvider every call.
 */
static void
set_label_leaky (GtkLabel *label, guint nPadding)
{
    GtkCssProvider *pCssProvider = gtk_css_provider_new ();
    GtkStyleContext *pStyleContext = gtk_widget_get_style_context (GTK_WIDGET (label));
    gtk_style_context_add_provider (pStyleContext, GTK_STYLE_PROVIDER (pCssProvider),
                                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gchar *sCss = g_strdup_printf ("label{padding-left: %ipx;}", nPadding);
    gtk_css_provider_load_from_data (pCssProvider, sCss, -1, NULL);
    g_free (sCss);
    g_object_unref (pCssProvider);
}

/*
 * Simulate what indicator_ng_set_label does AFTER the fix:
 * reuse a single provider, only reload its CSS data.
 */
static void
set_label_fixed (GtkLabel *label, guint nPadding, GtkCssProvider **cached)
{
    if (*cached == NULL) {
        *cached = gtk_css_provider_new ();
        GtkStyleContext *pStyleContext = gtk_widget_get_style_context (GTK_WIDGET (label));
        gtk_style_context_add_provider (pStyleContext, GTK_STYLE_PROVIDER (*cached),
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
    gchar *sCss = g_strdup_printf ("label{padding-left: %ipx;}", nPadding);
    gtk_css_provider_load_from_data (*cached, sCss, -1, NULL);
    g_free (sCss);
}

#define ITERATIONS 100000

static void
test_leaky_set_label (void)
{
    GtkWidget *label = gtk_label_new ("12:00:00");
    g_object_ref_sink (label);

    long rss_before = get_rss_kb ();

    for (int i = 0; i < ITERATIONS; i++) {
        set_label_leaky (GTK_LABEL (label), 6);
    }

    long rss_after = get_rss_kb ();
    long growth_kb = rss_after - rss_before;

    g_test_message ("leaky: RSS before=%ld KB, after=%ld KB, growth=%ld KB over %d iterations",
                    rss_before, rss_after, growth_kb, ITERATIONS);

    /* 100k leaked providers should use at least 50 MB.
       If we see significant growth, the leak is confirmed. */
    g_assert_cmpint (growth_kb, >, 50000);

    g_object_unref (label);
}

static void
test_fixed_set_label (void)
{
    GtkWidget *label = gtk_label_new ("12:00:00");
    g_object_ref_sink (label);

    long rss_before = get_rss_kb ();

    GtkCssProvider *cached = NULL;
    for (int i = 0; i < ITERATIONS; i++) {
        set_label_fixed (GTK_LABEL (label), 6, &cached);
    }

    long rss_after = get_rss_kb ();
    long growth_kb = rss_after - rss_before;

    g_test_message ("fixed: RSS before=%ld KB, after=%ld KB, growth=%ld KB over %d iterations",
                    rss_before, rss_after, growth_kb, ITERATIONS);

    /* With the fix, memory growth should be negligible (< 5 MB). */
    g_assert_cmpint (growth_kb, <, 5000);

    g_object_unref (cached);
    g_object_unref (label);
}

int
main (int argc, char **argv)
{
    gtk_test_init (&argc, &argv);

    g_test_add_func ("/indicator-ng/css-provider-leak/leaky", test_leaky_set_label);
    g_test_add_func ("/indicator-ng/css-provider-leak/fixed", test_fixed_set_label);

    return g_test_run ();
}
