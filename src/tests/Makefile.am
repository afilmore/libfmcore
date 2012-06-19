##include $(top_srcdir)/Makefile.decl

NULL=
TEST_PROGS =

AM_CPPFLAGS = \
	-I../ \
	-I../base \
	-I../fm-vala \
	-I../gtk \
	-I../gtk-dlg \
	-I../jobs \
	-I../jobs/FmJobs \
	-I../utils \
	$(GTK_CFLAGS) \
	$(GIO_CFLAGS) \
	$(PANGO_CFLAGS) \
	$(MENU_CACHE_CFLAGS) \
	$(DBUS_CFLAGS) \
	-Werror-implicit-function-declaration \
	$(NULL)

noinst_PROGRAMS = $(TEST_PROGS)

TEST_PROGS += main
main_SOURCES = main.c
main_LDADD= \
	$(GTK_LIBS) \
	$(GIO_LIBS) \
	$(PANGO_LIBS) \
	$(MENU_CACHE_LIBS) \
	$(DBUS_LIBS) \
	$(INTLLIBS) \
	../libfmcore-@LIBFMCORE_API_VERSION@.la \
	$(NULL)
