
libexec_PROGRAMS = \
	indicator-applet-sus

indicator_applet_sus_CFLAGS = \
	-DG_LOG_DOMAIN=\""Indicator-Applet"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DINDICATOR_DIR=\""$(libdir)/indicators/2"\" \
	-I$(srcdir)/.. \
	$(APPLET_CFLAGS)

indicator_applet_sus_SOURCES = 			\
	applet-main.c

indicator_applet_sus_LDADD = \
	$(APPLET_LIBS)
