# for use in libtool -version-info
ABI_VERSION=2:0:0

NULL=

# current dir needs to be built before tests
SUBDIRS= \
	. \
	tests \
	$(NULL)

AM_CPPFLAGS = \
	-include $(CONFIG_HEADER) \
	-I$(srcdir) \
	-I$(srcdir)/jobs \
	-I$(srcdir)/base \
	-I$(srcdir)/gtk \
	-I$(srcdir)/gtk/exo \
	-I$(srcdir)/gtk-dlg \
	-I$(srcdir)/utils \
	-DPACKAGE_LIB_DIR=\""$(libdir)/libfmcore-$(LIBFMCORE_API_VERSION)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(NULL)

VALAFLAGS = \
	--vapidir=$(srcdir)/../vapi \
	--pkg vconfig \
	--pkg $(GTK_PKG) \
    --pkg libfmcore \
    --pkg debug \
    --pkg posix \
    --header fm-vala.h \
	$(NULL)


vala_SOURCES = \
	base/fm-config.vala \
	jobs/Job.vala \
	jobs/FileJob.vala \
	jobs/CopyJob.vala \
	jobs/DeleteJob.vala \
	jobs/TrashJob.vala \
	jobs/UI/FileJobUI.vala \
	jobs/UI/GtkFileJobUI.vala \
	jobs/UI/GtkFileJobUIXML.vala \
	jobs/UI/GtkUI.vala \
	jobs/UI/RenameDialog.vala \
	jobs/UI/RenameDialogXML.vala \
	jobs/UI/UI.vala \
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
	gtk/fm-cell-renderer-pixbuf.c \
	gtk/fm-cell-renderer-text.c \
	gtk/fm-dir-tree-item.c \
	gtk/fm-dir-tree-model.c \
	gtk/fm-dir-tree-view.c \
	gtk/fm-dnd-auto-scroll.c \
	gtk/fm-dnd-dest.c \
	gtk/fm-dnd-src.c \
	gtk/fm-folder-model.c \
	gtk/fm-folder-view.c \
	gtk/fm-gtk-marshal.c \
	gtk/fm-icon-pixbuf.c \
	gtk/fm-path-entry.c \
	gtk/fm-thumbnail.c \
	gtk-dlg/fm-app-chooser-combo-box.c \
	gtk-dlg/fm-app-chooser-dlg.c \
	gtk-dlg/fm-app-chooser-xml.c \
	gtk-dlg/fm-app-menu-view.c \
	gtk-dlg/fm-gtk-launcher.c \
	gtk-dlg/fm-gtk-launcher-xml.c \
	gtk-dlg/fm-file-properties.c \
	gtk-dlg/fm-file-properties-xml.c \
	gtk-dlg/fm-select-folder-dlg.c \
	gtk-dlg/fm-user-input-dlg.c \
	utils/fm-app-info.c \
	utils/fm-archiver.c \
	utils/fm-clipboard.c \
	utils/fm-file-menu.c \
	utils/fm-file-ops.c \
	utils/fm-mount.c \
	utils/fm-msgbox.c \
	utils/fm-trash.c \
	$(NULL)


exo_SOURCES = \
	gtk/exo/exo-icon-view.c \
	gtk/exo/exo-private.c \
	gtk/exo/exo-tree-view.c \
	gtk/exo/exo-lxde.c \
	gtk/exo/exo-marshal.c \
	$(NULL)


jobs_SOURCES = \
	jobs/fm-dir-list-job.c \
	jobs/fm-deep-count-job.c  \
	jobs/fm-file-info-job.c \
	$(NULL)


libfmcore_SOURCES = \
	$(vala_SOURCES) \
	fm.c \
	$(base_SOURCES) \
	$(jobs_SOURCES) \
	$(exo_SOURCES) \
	$(NULL)


libfmcoreincludedir = $(includedir)/libfmcore-$(LIBFMCORE_API_VERSION)
libfmcoreinclude_HEADERS = \
	fm.h \
	fm-vala.h \
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
	jobs/fm-deep-count-job.h \
	jobs/fm-dir-list-job.h \
	jobs/fm-file-info-job.h \
	gtk/exo/exo-icon-view.h \
	gtk/exo/exo-tree-view.h \
	gtk/fm-dnd-auto-scroll.h \
	gtk/fm-cell-renderer-pixbuf.h \
	gtk/fm-cell-renderer-text.h \
	gtk/fm-dir-tree-item.h \
	gtk/fm-dir-tree-model.h \
	gtk/fm-dir-tree-view.h \
	gtk/fm-dnd-dest.h \
	gtk/fm-dnd-src.h \
	gtk/fm-folder-model.h \
	gtk/fm-folder-view.h \
	gtk/fm-gtk-marshal.h \
	gtk/fm-icon-pixbuf.h \
	gtk/fm-path-entry.h \
	gtk/fm-thumbnail.h \
	gtk-dlg/fm-app-chooser-combo-box.h \
	gtk-dlg/fm-app-chooser-dlg.h \
	gtk-dlg/fm-app-menu-view.h \
	gtk-dlg/fm-file-properties.h \
	gtk-dlg/fm-gtk-launcher.h \
	gtk-dlg/fm-select-folder-dlg.h \
	gtk-dlg/fm-user-input-dlg.h \
	utils/fm-app-info.h \
	utils/fm-archiver.h \
	utils/fm-clipboard.h \
	utils/fm-file-menu.h \
	utils/fm-file-ops.h \
	utils/fm-mount.h \
	utils/fm-msgbox.h \
	utils/fm-trash.h \
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
	$(patsubst %.vala,%.c,$(filter %.vala, $(vala_SOURCES))) \
	base/fm-marshal.h \
	base/fm-marshal.c \
	gtk/fm-gtk-marshal.h \
	gtk/fm-gtk-marshal.c \
	libfmcore_@LIBFMCORE_API_VERSION@_la_vala.stamp \
	$(NULL)


# A lib to be loaede with LD_PRELOADS to override GtkFileChooserWidget
#lib_LTLIBRARIES=libfm-chooser-hack.la

#libfm_chooser_hack_la_SOURCES=libfm-chooser-hack.c
#libfm_chooser_hack_la_CFLAGS=$(GTK_CFLAGS)
#libfm_chooser_hack_la_LIBADD=$(GMODULE_LIBS)
#libfm_chooser_hack_la_LDFLAGS= \
#        -no-undefined \
#        -export-symbols-regex gtk



