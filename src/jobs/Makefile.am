NULL=

AM_CPPFLAGS = \
	-include $(CONFIG_HEADER) \
	-I$(srcdir)/../ \
	-I$(srcdir)/../base \
	-I$(srcdir)/../fm-vala \
	-I$(srcdir)/../gtk \
	-I$(srcdir)/../gtk-dlg \
	-I$(srcdir)/../jobs \
	-I$(srcdir)/../utils \
	-DPACKAGE_LIB_DIR=\""$(libdir)/libfmcore-$(LIBFMCORE_API_VERSION)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(NULL)

noinst_LTLIBRARIES = libfmjobs.la

libfmjobs_la_SOURCES = \
	job.vala \
	file-job.vala \
	copy-job.vala \
	delete-job.vala \
	trash-job.vala \
	ui/ui.vala \
	ui/gtk-ui.vala \
	ui/rename-dlg.vala \
	$(NULL)

libfmjobs_la_CFLAGS = \
	$(GIO_CFLAGS) \
	$(GTK_CFLAGS) \
	$(MENU_CACHE_CFLAGS) \
	-Werror-implicit-function-declaration \
	$(NULL)

VALAFLAGS = \
	--vapidir=$(srcdir)/../../vapi \
	--pkg vconfig \
	--pkg $(GTK_PKG) \
    --pkg libfmcore \
    --pkg posix \
    --header fm-jobs.h \
	$(NULL)

libfmjobsincludedir = $(includedir)/libfmcore-$(LIBFMCORE_API_VERSION)
libfmjobsinclude_HEADERS = \
	fm-jobs.h \
	$(NULL)

libfmjobs_la_LDFLAGS = \
	$(NULL)

libfmjobs_la_LIBADD = \
	$(INTLLIBS) \
	$(NULL)

CLEANFILES = $(patsubst %.vala,%.c,$(filter %.vala, $(SOURCES))) fm-jobs.h libfmjobs_la_vala.stamp


