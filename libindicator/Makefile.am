BUILT_SOURCES = 
CLEANFILES = 
DISTCLEANFILES = 
EXTRA_DIST = \
	indicator.pc.in

include $(top_srcdir)/Makefile.am.marshal

libindicatorincludedir=$(includedir)/libindicator-0.3/libindicator

indicator_headers = \
	indicator.h \
	indicator-object.h \
	indicator-service.h \
	indicator-service-manager.h

libindicatorinclude_HEADERS = \
	$(indicator_headers)

lib_LTLIBRARIES = \
	libindicator.la

libindicator_la_SOURCES = \
	$(indicator_headers) \
	dbus-shared.h \
	indicator-object.c \
	indicator-object-marshal.h \
	indicator-object-marshal.c \
	indicator-service.c \
	indicator-service-manager.c

libindicator_la_CFLAGS = \
	$(LIBINDICATOR_CFLAGS) \
	-DG_LOG_DOMAIN=\"libindicator\" \
	-Wall -Werror

libindicator_la_LIBADD = \
	$(LIBINDICATOR_LIBS)

pkgconfig_DATA = indicator.pc
pkgconfigdir = $(libdir)/pkgconfig

glib_marshal_list = indicator-object-marshal.list
glib_marshal_prefix = _indicator_object_marshal

##################################
# DBus Specs
##################################

DBUS_SPECS = \
	indicator-service.xml

%-client.h: %.xml
	dbus-binding-tool \
		--prefix=_$(subst -,_,$(basename $(notdir $<)))_client \
		--mode=glib-client \
		--output=$@ \
		$<

%-server.h: %.xml
	dbus-binding-tool \
		--prefix=_$(subst -,_,$(basename $(notdir $<)))_server \
		--mode=glib-server \
		--output=$@ \
		$<

BUILT_SOURCES += \
	$(DBUS_SPECS:.xml=-client.h) \
	$(DBUS_SPECS:.xml=-server.h)

CLEANFILES += $(BUILT_SOURCES)

EXTRA_DIST += $(DBUS_SPECS)
