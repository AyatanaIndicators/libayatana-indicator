EXTRA_DIST = \
	indicator.pc.in

libindicatorincludedir=$(includedir)/libindicator-0.1/libindicator

indicator_headers = \
	indicator.h \
	indicator-object.h

libindicatorinclude_HEADERS = \
	$(indicator_headers)

lib_LTLIBRARIES = \
	libindicator.la

libindicator_la_SOURCES = \
	$(indicator_headers) \
	indicator-object.c

pkgconfig_DATA = indicator.pc
pkgconfigdir = $(libdir)/pkgconfig

