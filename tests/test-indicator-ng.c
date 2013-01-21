
#include <libindicator/indicator-ng.h>

static void
test_non_existing (void)
{
  IndicatorNg *indicator;
  GError *error = NULL;

  indicator = indicator_ng_new (SRCDIR "/com.canonical.does.not.exist.indicator", &error);
  g_assert (indicator == NULL);
  g_assert_error (error, G_FILE_ERROR, G_FILE_ERROR_NOENT);

  g_clear_error (&error);
}

static void
test_instantiation (void)
{
  IndicatorNg *indicator;
  GError *error = NULL;

  indicator = indicator_ng_new (SRCDIR "/com.canonical.test.indicator", &error);
  g_assert (indicator);
  g_assert (error == NULL);

  g_assert_cmpstr (indicator_ng_get_service_file (indicator), ==, SRCDIR "/com.canonical.test.indicator");
  g_assert_cmpstr (indicator_ng_get_profile (indicator), ==, "desktop");

  /* no service running, so there shouldn't be any indicators */
  g_assert (indicator_object_get_entries (INDICATOR_OBJECT (indicator)) == NULL);

  g_object_unref (indicator);
}

static void
test_instantiation_with_profile (void)
{
  IndicatorNg *indicator;
  GError *error = NULL;

  indicator = indicator_ng_new_for_profile (SRCDIR "/com.canonical.test.indicator", "greeter", &error);
  g_assert (indicator);
  g_assert (error == NULL);

  g_assert_cmpstr (indicator_ng_get_profile (indicator), ==, "greeter");

  g_object_unref (indicator);
}

int
main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);
  gtk_init (&argc, &argv);

  g_test_add_func ("/indicator-ng/non-existing", test_non_existing);
  g_test_add_func ("/indicator-ng/instantiation", test_instantiation);
  g_test_add_func ("/indicator-ng/instantiation-with-profile", test_instantiation_with_profile);

  return g_test_run ();
}
