BUILT_SOURCES = indicator-object-enum-types.h indicator-object-enum-types.c
CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST = \
	indicator.pc.in \
	indicator3.pc.in

include $(top_srcdir)/Makefile.am.marshal

libindicatorincludedir=$(includedir)/libindicator-0.3/libindicator
libindicator3includedir=$(includedir)/libindicator-3.3/libindicator

indicator_headers = \
	indicator.h \
	indicator-desktop-shortcuts.h \
	indicator-image-helper.h \
	indicator-object.h \
	indicator-service.h \
	indicator-service-manager.h


libindicatorinclude_HEADERS = \
	$(indicator_headers)
libindicator3include_HEADERS = \
	$(indicator3_headers)

lib_LTLIBRARIES = \
	libindicator.la \
	libindicator3.la

libindicator_la_SOURCES = \
	$(indicator_headers) \
	dbus-shared.h \
	indicator-object.c \
	indicator-object-enum-types.c \
	indicator-desktop-shortcuts.c \
	indicator-image-helper.c \
	indicator-object-marshal.h \
	indicator-object-marshal.c \
	indicator-service.c \
	indicator-service-manager.c
libindicator3_la_SOURCES = $(libindicator_la_SOURCES)

libindicator_la_CFLAGS = \
	$(LIBINDICATOR_CFLAGS) \
	-DG_LOG_DOMAIN=\"libindicator\" \
	-Wall -Werror
libindicator3_la_CFLAGS = \
	$(LIBINDICATOR3_CFLAGS) \
	-DG_LOG_DOMAIN=\"libindicator\" \
	-Wall -Werror

libindicator_la_LIBADD = \
	$(LIBINDICATOR_LIBS)
libindicator3_la_LIBADD = \
	$(LIBINDICATOR3_LIBS)

pkgconfig_DATA = indicator.pc indicator3.pc
pkgconfigdir = $(libdir)/pkgconfig

glib_marshal_list = indicator-object-marshal.list
glib_marshal_prefix = _indicator_object_marshal

indicator-object-enum-types.h: s-enum-types-h
	@true
s-enum-types-h: $(indicator_headers)
	( cd $(srcdir) && $(GLIB_MKENUMS) --template $(srcdir)/indicator-object-enum-types.h.template								\
		$(indicator_headers) ) >> tmp-indicator-object-enum-types.h											\
	&& (cmp -s tmp-indicator-object-enum-types.h indicator-object-enum-types.h || cp tmp-indicator-object-enum-types.h indicator-object-enum-types.h )	\
	&& rm -f tmp-indicator-object-enum-types.h && echo timestamp > $(@F)

indicator-object-enum-types.c: s-enum-types-c
	@true
s-enum-types-c: $(indicator_headers)
	( cd $(srcdir) && $(GLIB_MKENUMS) --template $(srcdir)/indicator-object-enum-types.c.template								\
		$(indicator_headers) ) > tmp-indicator-object-enum-types.c											\
	&& (cmp -s tmp-indicator-object-enum-types.c indicator-object-enum-types.c || cp tmp-indicator-object-enum-types.c indicator-object-enum-types.c )	\
	&& rm -f tmp-indicator-object-enum-types.c

EXTRA_DIST += indicator-object-enum-types.h.template indicator-object-enum-types.c.template
CLEANFILES +=						\
	indicator-object-enum-types.h			\
	indicator-object-enum-types.c			\
	s-enum-types-h					\
	s-enum-types-c

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
