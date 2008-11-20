
libexec_PROGRAMS = \
	indicator-applet

indicator_applet_CFLAGS = \
	-DG_LOG_DOMAIN=\""Indicator-Applet"\" \
	$(APPLET_CFLAGS)

indicator_applet_SOURCES = 			\
	applet-main.c					\
	indicator-audio.c				\
	indicator-audio.h				\
	indicator-messages.c			\
	indicator-messages.h			\
	indicator-network.c				\
	indicator-network.h				\
	indicator-power.c				\
	indicator-power.h				\
	indicator-system.c				\
	indicator-system.h				\
	menu-dbus.c						\
	menu-dbus.h						\
	slidermenuitem.c				\
	slidermenuitem.h

indicator_applet_LDADD = \
	$(APPLET_LIBS)
