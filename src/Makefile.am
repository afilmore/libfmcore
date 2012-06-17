# for use in libtool -version-info
ABI_VERSION=2:0:0

NULL=

# current dir needs to be built before tests
SUBDIRS= \
	fm-vala \
	jobs \
	. \
	$(NULL)

AM_CPPFLAGS = \
	-I$(srcdir) \
	-I$(srcdir)/fm-vala \
	-I$(srcdir)/jobs \
	-I$(srcdir)/jobs/FmJobs \
	-I$(srcdir)/base \
	-I$(srcdir)/gtk \
	-I$(srcdir)/gtk-dlg \
	-I$(srcdir)/utils \
	-DPACKAGE_LIB_DIR=\""$(libdir)/libfmcore-$(LIBFMCORE_API_VERSION)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(NULL)

base_SOURCES = \
	base/fm-dummy-monitor.c \
	base/fm-file-info.c \
	base/fm-file-info-list.c \
	base/fm-folder.c \
	base/fm-icon.c \
	base/fm-list.c \
	base/fm-marshal.c \
	base/fm-mime-type.c \
	base/fm-monitor.c \
	base/fm-path.c \
	base/fm-path-list.c \
	base/fm-utils.c \
	gtk/fm-clipboard.c \
	gtk/fm-dnd-auto-scroll.c \
	gtk/fm-dnd-dest.c \
	gtk/fm-dnd-src.c \
	gtk/fm-file-menu.c \
	gtk/fm-folder-model.c \
	gtk/fm-gtk-marshal.c \
	gtk/fm-icon-pixbuf.c \
	gtk/fm-thumbnail.c \
	gtk-dlg/fm-app-chooser-combo-box.c \
	gtk-dlg/fm-app-chooser-dlg.c \
	gtk-dlg/fm-app-menu-view.c \
	gtk-dlg/fm-gtk-launcher.c \
	gtk-dlg/fm-file-properties.c \
	utils/fm-app-info.c \
	utils/fm-archiver.c \
	utils/fm-file-ops.c \
	utils/fm-mount.c \
	utils/fm-msgbox.c \
	utils/fm-select-folder-dlg.c \
	utils/fm-trash.c \
	utils/fm-user-input-dlg.c \
	$(NULL)

	
jobs_SOURCES = \
	jobs/FmJobs/fm-dir-list-job.c \
	jobs/FmJobs/fm-deep-count-job.c  \
	jobs/FmJobs/fm-file-info-job.c \
	$(NULL)


libfmcore_SOURCES = \
	fm.c \
	$(base_SOURCES) \
	$(jobs_SOURCES) \
	$(NULL)


libfmcoreincludedir = $(includedir)/libfmcore-$(LIBFMCORE_API_VERSION)
libfmcoreinclude_HEADERS = \
	fm.h \
	fm-vala/fm-vala.h \
	base/fm-debug.h \
	base/fm-dummy-monitor.h \
	base/fm-file-info.h \
	base/fm-file-info-list.h \
	base/fm-folder.h \
	base/fm-icon.h \
	base/fm-list.h \
	base/fm-marshal.h \
	base/fm-mime-type.h \
	base/fm-monitor.h \
	base/fm-path.h \
	base/fm-path-list.h \
	base/fm-utils.h \
	jobs/fm-jobs.h \
	jobs/FmJobs/fm-deep-count-job.h \
	jobs/FmJobs/fm-dir-list-job.h \
	jobs/FmJobs/fm-file-info-job.h \
	gtk/fm-clipboard.h \
	gtk/fm-dnd-auto-scroll.h \
	gtk/fm-dnd-dest.h \
	gtk/fm-dnd-src.h \
	gtk/fm-file-menu.h \
	gtk/fm-folder-model.h \
	gtk/fm-gtk-marshal.h \
	gtk/fm-icon-pixbuf.h \
	gtk/fm-thumbnail.h \
	gtk-dlg/fm-app-chooser-combo-box.h \
	gtk-dlg/fm-app-chooser-dlg.h \
	gtk-dlg/fm-app-menu-view.h \
	gtk-dlg/fm-file-properties.h \
	gtk-dlg/fm-gtk-launcher.h \
	utils/fm-app-info.h \
	utils/fm-archiver.h \
	utils/fm-file-ops.h \
	utils/fm-mount.h \
	utils/fm-msgbox.h \
	utils/fm-select-folder-dlg.h \
	utils/fm-trash.h \
	utils/fm-user-input-dlg.h \
	$(NULL)


pkginclude_HEADERS = \
	$(LIBFMCORE_INCLUDES) \
	$(NULL)


lib_LTLIBRARIES = libfmcore-@LIBFMCORE_API_VERSION@.la

libfmcore_@LIBFMCORE_API_VERSION@_la_SOURCES = \
	$(libfmcore_SOURCES) \
	$(NULL)

libfmcore_@LIBFMCORE_API_VERSION@_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(GIO_CFLAGS) \
	$(PANGO_CFLAGS) \
	$(MENU_CACHE_CFLAGS) \
	$(DBUS_CFLAGS) \
	-Werror-implicit-function-declaration \
	$(NULL)

libfmcore_@LIBFMCORE_API_VERSION@_la_LIBADD = \
	$(GTK_LIBS) \
	$(GIO_LIBS) \
	$(PANGO_LIBS) \
	$(MENU_CACHE_LIBS) \
	$(DBUS_LIBS) \
	$(INTLLIBS) \
	$(srcdir)/fm-vala/libfmvala.la \
	$(srcdir)/jobs/libfmjobs.la \
	$(NULL)

libfmcore_@LIBFMCORE_API_VERSION@_la_LDFLAGS = \
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

