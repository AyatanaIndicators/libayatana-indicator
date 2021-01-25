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

DISTCHECK_CONFIGURE_FLAGS = --disable-deprecations --disable-silent-rules --with-gtk=3

DISTCLEANFILES = \
	libayatana-indicator-*.tar.gz

include $(top_srcdir)/Makefile.am.coverage
