if USE_GTK3
INDICATOR_LIB = -lindicator3
libexec_PROGRAMS = indicator-loader3
VER=3
else
VER=
INDICATOR_LIB = -lindicator
libexec_PROGRAMS = indicator-loader
endif


#############################
# Indicator Loader
#############################

indicator_loader_SOURCES = \
	indicator-loader.c

indicator_loader_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir) \
	-DBUILD_DIR="\"$(builddir)\""

indicator_loader_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libindicator/.libs \
	$(INDICATOR_LIB)

# We duplicate these here because Automake won't let us use $(VER) on the left hand side.
# Since we carefully use $(VER) in the right hand side above, we can assign the same values.
# Only one version of the library is every compiled at the same time, so it is safe to reuse
# the right hand sides like this.
indicator_loader3_SOURCES = $(indicator_loader_SOURCES)
indicator_loader3_CFLAGS = $(indicator_loader_CFLAGS)
indicator_loader3_LDADD = $(indicator_loader_LDADD)

xsessiondir = $(pkgdatadir)

xsession_DATA = 80indicator-debugging

EXTRA_DIST = $(xsession_DATA)
