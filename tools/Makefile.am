if USE_GTK3
INDICATOR_LIB = -lindicator3
else
INDICATOR_LIB = -lindicator
endif

libexec_PROGRAMS = \
	indicator-loader

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

xsessiondir = $(sysconfdir)/X11/Xsession.d

xsession_DATA = 80indicator-debugging

EXTRA_DIST = $(xsession_DATA)
