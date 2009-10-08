
noinst_PROGRAMS = \
	test-loader

test_loader_SOURCES = \
	test-loader.c

test_loader_CFLAGS = \
	-Wall -Werror \
	$(UPANEL_CFLAGS) -I$(top_srcdir)

test_loader_LDADD = \
	$(UPANEL_LIBS) $(top_builddir)/libindicator/.libs/libindicator.a

XML_REPORT  = loader-check-results.xml
HTML_REPORT = loader-check-results.html

loader-tester: test-loader
	@gtester -o=$(XML_REPORT) ./test-loader

check-local: loader-tester

DISTCLEANFILES = $(XML_REPORT) $(HTML_REPORT)

