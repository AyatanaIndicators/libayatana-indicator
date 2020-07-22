if USE_IDO
if USE_GTK3
INDICATOR_LIB = -layatana-indicator3
libexec_PROGRAMS = ayatana-indicator-loader3
VER=3
endif
endif


#############################
# Indicator Loader
#############################

ayatana_indicator_loader_SOURCES = \
	indicator-loader.c

ayatana_indicator_loader_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) \
	$(LIBINDICATOR_IDO_CFLAGS) -I$(top_srcdir) \
	-DBUILD_DIR="\"$(builddir)\""

ayatana_indicator_loader_LDADD = \
	$(LIBINDICATOR_LIBS) \
	$(LIBINDICATOR_IDO_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

# We duplicate these here because Automake won't let us use $(VER) on the left hand side.
# Since we carefully use $(VER) in the right hand side above, we can assign the same values.
# Only one version of the library is every compiled at the same time, so it is safe to reuse
# the right hand sides like this.
ayatana_indicator_loader3_SOURCES = $(ayatana_indicator_loader_SOURCES)
ayatana_indicator_loader3_CFLAGS = $(ayatana_indicator_loader_CFLAGS)
ayatana_indicator_loader3_LDADD = $(ayatana_indicator_loader_LDADD)

xsessiondir = $(pkgdatadir)

xsession_DATA = 80indicator-debugging

EXTRA_DIST = $(xsession_DATA)

DISTCLEANFILES = Makefile.in
