
libexec_PROGRAMS = \
	indicator-applet

indicator_applet_CFLAGS = \
	-DG_LOG_DOMAIN=\""Indicator-Applet"\" \
	-DDATADIR=\""$(datadir)"\" \
	$(APPLET_CFLAGS)

indicator_applet_SOURCES = 			\
	applet-main.c					\
	indicator-messages.c			\
	indicator-messages.h

indicator_applet_LDADD = \
	$(APPLET_LIBS)
