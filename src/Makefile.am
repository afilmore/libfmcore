# for use in libtool -version-info
ABI_VERSION=2:0:0

NULL=

# current dir needs to be built before tests
SUBDIRS= \
	. \
	$(NULL)

AM_CPPFLAGS = \
	-I$(srcdir) \
	-I$(srcdir)/base \
	-I$(srcdir)/job \
	-I$(srcdir)/gtk \
	-DPACKAGE_LIB_DIR=\""$(libdir)/libfmcore"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(NULL)

base_SOURCES = \
	base/fm-list.c \
	base/fm-path.c \
	base/fm-folder.c \
	base/fm-file-info.c \
	base/fm-mime-type.c \
	base/fm-utils.c \
	base/fm-file-launcher.c \
	base/fm-icon.c \
	base/fm-monitor.c \
	base/fm-dummy-monitor.c \
	base/fm-app-info.c \
	base/fm-marshal.c \
	gtk/fm-app-menu-view.c \
	gtk/fm-app-chooser-dlg.c \
	gtk/fm-app-chooser-combo-box.c \
	gtk/fm-cell-renderer-pixbuf.c \
	gtk/fm-icon-pixbuf.c \
	gtk/fm-folder-model.c \
	gtk/fm-thumbnail.c \
	gtk/fm-gtk-utils.c \
	gtk/fm-gtk-file-launcher.c \
	gtk/fm-dnd-src.c \
	gtk/fm-dnd-dest.c \
	gtk/fm-dnd-auto-scroll.c \
	$(NULL)

job_SOURCES = \
	job/fm-job.c \
	job/fm-simple-job.c \
	job/fm-dir-list-job.c \
	job/fm-deep-count-job.c  \
	job/fm-file-ops-job.c \
	job/fm-file-info-job.c \
	job/fm-file-ops-job-xfer.c \
	job/fm-file-ops-job-delete.c \
	job/fm-file-ops-job-change-attr.c \
	$(NULL)

libfmcore_SOURCES = \
	fmcore.c \
	$(base_SOURCES) \
	$(job_SOURCES) \
	$(NULL)

libfmcoreincludedir = $(includedir)/libfmcore
libfmcoreinclude_HEADERS = \
	fmcore.h \
	base/fm-app-info.h \
	base/fm-dummy-monitor.h \
	base/fm-file-info.h \
	base/fm-file-launcher.h \
	base/fm-folder.h \
	base/fm-icon.h \
	base/fm-list.h \
	base/fm-marshal.h \
	base/fm-mime-type.h \
	base/fm-monitor.h \
	base/fm-path.h \
	base/fm-utils.h \
	job/fm-deep-count-job.h \
	job/fm-dir-list-job.h \
	job/fm-file-info-job.h \
	job/fm-file-ops-job.h \
	job/fm-file-ops-job-change-attr.h \
	job/fm-file-ops-job-delete.h \
	job/fm-file-ops-job-xfer.h \
	job/fm-job.h \
	job/fm-simple-job.h \
	gtk/fm-app-chooser-combo-box.h \
	gtk/fm-app-chooser-dlg.h \
	gtk/fm-app-menu-view.h \
	gtk/fm-cell-renderer-pixbuf.h \
	gtk/fm-file-menu.h \
	gtk/fm-folder-model.h \
	gtk/fm-gtk-file-launcher.h \
	gtk/fm-gtk-marshal.h \
	gtk/fm-gtk-utils.h \
	gtk/fm-icon-pixbuf.h \
	gtk/fm-thumbnail.h \
	gtk/fm-dnd-src.h \
	gtk/fm-dnd-dest.h \
	gtk/fm-dnd-auto-scroll.h \
	$(NULL)

pkginclude_HEADERS = \
	$(LIBFMCORE_INCLUDES) \
	$(NULL)

lib_LTLIBRARIES = libfmcore.la
libfmcore_la_SOURCES = \
	$(libfmcore_SOURCES) \
	$(NULL)

libfmcore_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(GIO_CFLAGS) \
	$(PANGO_CFLAGS) \
	$(MENU_CACHE_CFLAGS) \
	$(DBUS_CFLAGS) \
	-Werror-implicit-function-declaration \
	$(NULL)

libfmcore_la_LIBADD = \
	$(GTK_LIBS) \
	$(GIO_LIBS) \
	$(PANGO_LIBS) \
	$(MENU_CACHE_LIBS) \
	$(DBUS_LIBS) \
	$(INTLLIBS) \
	$(NULL)

libfmcore_la_LDFLAGS = \
	-no-undefined \
	-export-symbols-regex ^fm \
	-version-info $(ABI_VERSION) \
	$(NULL)

# GObject marshallers
base/fm-marshal.h: base/fm-marshal.list
	glib-genmarshal --header --prefix=fm_marshal $^ > $@.tmp && mv $@.tmp $@

base/fm-marshal.c: base/fm-marshal.list
	glib-genmarshal --body --prefix=fm_marshal $^ > $@.tmp && mv $@.tmp $@

gtk/fm-gtk-marshal.h: gtk/fm-gtk-marshal.list
	glib-genmarshal --header --prefix=fm_marshal $^ > $@.tmp && mv $@.tmp $@

gtk/fm-gtk-marshal.c: gtk/fm-gtk-marshal.list
	glib-genmarshal --body --prefix=fm_marshal $^ > $@.tmp && mv $@.tmp $@

# Workarounds to force the build
BUILT_SOURCES= \
	base/fm-marshal.h \
	base/fm-marshal.c \
	gtk/fm-gtk-marshal.h \
	gtk/fm-gtk-marshal.c \
	$(NULL)

EXTRA_DIST = \
	base/fm-marshal.list \
	gtk/fm-gtk-marshal.list \
	$(NULL)

CLEANFILES = \
	base/fm-marshal.h \
	base/fm-marshal.c \
	gtk/fm-gtk-marshal.h \
	gtk/fm-gtk-marshal.c \
	$(NULL)

# A lib to be loaede with LD_PRELOADS to override GtkFileChooserWidget
#lib_LTLIBRARIES=libfm-chooser-hack.la

#libfm_chooser_hack_la_SOURCES=libfm-chooser-hack.c
#libfm_chooser_hack_la_CFLAGS=$(GTK_CFLAGS)
#libfm_chooser_hack_la_LIBADD=$(GMODULE_LIBS)
#libfm_chooser_hack_la_LDFLAGS= \
#        -no-undefined \
#        -export-symbols-regex gtk

# Little program to optimize size of xml files
noinst_PROGRAMS=xml-purge
xml_purge_SOURCES=xml-purge.c
xml_purge_CFLAGS=$(GIO_CFLAGS)
xml_purge_LDADD=$(GIO_LIBS)
