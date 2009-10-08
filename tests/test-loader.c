#include <glib.h>
#include "libindicator/indicator-object.h"

void
test_loader_refunref (void)
{

	return;
}

void
test_loader_creation_deletion_suite (void)
{
	g_test_add_func ("/libindicator/loader/ref_and_unref", test_loader_refunref);

	return;
}


int
main (int argc, char ** argv)
{
	g_type_init (); 
	g_test_init (&argc, &argv, NULL);

	test_loader_creation_deletion_suite();

	return g_test_run();
}
