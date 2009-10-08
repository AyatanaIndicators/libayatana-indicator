
check_PROGRAMS = \
	test-loader

lib_LTLIBRARIES = \
	libdummy-indicator-null.la

#############################
# Test Loader
#############################

test_loader_SOURCES = \
	test-loader.c

test_loader_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir) \
	-DBUILD_DIR="\"$(builddir)\""

test_loader_LDADD = \
	$(LIBINDICATOR_LIBS) $(top_builddir)/libindicator/.libs/libindicator.a

#############################
# Dummy Indicator
#############################

libdummy_indicator_null_la_SOURCES = \
	dummy-indicator-null.c

libdummy_indicator_null_la_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_null_la_LIBADD = \
	$(LIBINDICATOR_LIBS)

libdummy_indicator_null_la_LDFLAGS = \
	-module \
	-avoid-version

#############################
# Test stuff
#############################

XML_REPORT  = loader-check-results.xml
HTML_REPORT = loader-check-results.html

loader-tester: test-loader libdummy-indicator-null.la
	@gtester -k --verbose -o=$(XML_REPORT) ./test-loader

check-local: loader-tester

DISTCLEANFILES = $(XML_REPORT) $(HTML_REPORT)

