ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = \
	data \
	libayatana-indicator \
	tools

tools: libayatana-indicator

if WANT_TESTS
SUBDIRS += \
	tests

tests: libayatana-indicator
endif

DISTCLEANFILES = \
	libayatana-indicator-*.tar.gz

include $(top_srcdir)/Makefile.am.coverage
