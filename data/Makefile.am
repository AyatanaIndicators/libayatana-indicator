# This file will be processed with automake to create Makefile.in

##############################
# Bonobo .server
##############################

serverdir =					\
	$(libdir)/bonobo/servers
server_in_files =				\
	GNOME_IndicatorApplet.server.in \
	GNOME_IndicatorAppletSUS.server.in
server_DATA =					\
	$(server_in_files:.server.in=.server)

%.server.in: %.server.in.in
	sed -e "s|\@LIBEXECDIR\@|$(libexecdir)|" -e "s|\@VERSION\@|$(VERSION)|" $< > $@

@INTLTOOL_SERVER_RULE@


##############################
# GConf Schema
##############################

schemadir =					\
	$(GCONF_SCHEMA_FILE_DIR)

schema_in_files =				\
	indicator-applet.schemas.in

schema_DATA =					\
	$(schema_in_files:.schemas.in=.schemas)

@INTLTOOL_SCHEMAS_RULE@

##############################
# Icons
##############################

iconsdir = $(datadir)/icons/hicolor/scalable/apps

icons_DATA = indicator-applet.svg

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi


##############################
# Autojunk
##############################

CLEANFILES =					\
	$(server_in_files)			\
	$(server_DATA)				\
	$(schema_DATA)				\
	*.bak

EXTRA_DIST =					\
	$(icons_DATA) \
	GNOME_IndicatorApplet.server.in.in	\
	GNOME_IndicatorAppletSUS.server.in.in	\
	$(schema_in_files)

MAINTAINERCLEANFILES =				\
	Makefile.in

if GCONF_SCHEMAS_INSTALL
install-data-local:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
		gconftool-2 --makefile-install-rule $(schema_DATA)
else
install-data-local:
endif



