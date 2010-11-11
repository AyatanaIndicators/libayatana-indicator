if USE_GTK3
VER=3
lib_LTLIBRARIES = libindicator3.la
else
VER=
lib_LTLIBRARIES = libindicator.la
endif

BUILT_SOURCES = indicator-object-enum-types.h indicator-object-enum-types.c
CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST = \
	indicator$(VER).pc.in

include $(top_srcdir)/Makefile.am.marshal

libindicatorincludedir=$(includedir)/libindicator$(VER)-0.3/libindicator

indicator_headers = \
	indicator.h \
	indicator-desktop-shortcuts.h \
	indicator-image-helper.h \
	indicator-object.h \
	indicator-service.h \
	indicator-service-manager.h

libindicatorinclude_HEADERS = \
	$(indicator_headers)

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

libindicator_la_CFLAGS = \
	$(LIBINDICATOR_CFLAGS) \
	-DG_LOG_DOMAIN=\"libindicator\" \
	-Wall -Werror

libindicator_la_LIBADD = \
	$(LIBINDICATOR_LIBS)

libindicator_la_LDFLAGS = \
	-version-info 1:0:0 \
	-no-undefined \
	-export-symbols-regex "^[^_].*"

# We duplicate these here because Automake won't let us use $(VER) on the left hand side.
# Since we carefully use $(VER) in the right hand side above, we can assign the same values.
# Only one version of the library is every compiled at the same time, so it is safe to reuse
# the right hand sides like this.
libindicator3includedir = $(libindicatorincludedir)
libindicator3include_HEADERS = $(indicator_headers)
libindicator3_la_SOURCES = $(libindicator_la_SOURCES)
libindicator3_la_CFLAGS = $(libindicator_la_CFLAGS)
libindicator3_la_LIBADD = $(libindicator_la_LIBADD)
libindicator3_la_LDFLAGS = $(libindicator_la_LDFLAGS)

pkgconfig_DATA = indicator$(VER).pc
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
