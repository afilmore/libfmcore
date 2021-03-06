#***********************************************************************************************************************
#   
#   Configuration File for LibFm Core
#
#   A Lightweight FileManager Library.
#
#   Sample Configuration Commands :
#
#   make clean;make distclean;./autogen.sh && ./configure --enable-debug=yes --prefix=/usr && make
#
#
#***********************************************************************************************************************

AC_PREREQ([2.63])
AC_INIT([libfmcore], [0.2.0], [https://github.com/afilmore/libfmcore])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_LIBTOOL


#*****************************************************************************************
#   Check Vala Compiler...
#
#
#*****************************************************************************************
AM_PROG_VALAC([0.16.0])


#*****************************************************************************************
#   ...
#
#
#*****************************************************************************************
AC_HEADER_STDC
AC_FUNC_MMAP
AC_CHECK_LIB(m, pow)


#*****************************************************************************************
#   Large file support...
#
#
#*****************************************************************************************
AC_ARG_ENABLE([largefile],
    AS_HELP_STRING([--enable-largefile],
        [enable Large file support (default: yes)]),
    largefile=$enableval, largefile="yes")

if test x"$largefile" = x"yes"; then
  CPPFLAGS="$CPPFLAGS -D_LARGEFILE_SOURCE -D_REENTRANT -D_FILE_OFFSET_BITS=64"
  AC_DEFINE(_LARGEFILE64_SOURCE, 1, [Whether to enable large file support])
  AC_DEFINE(_FILE_OFFSET_BITS, 64, [File offset bits])
fi


#*****************************************************************************************
#   IntlTool...
#
#
#*****************************************************************************************
#~ IT_PROG_INTLTOOL([0.40.0])


#*****************************************************************************************
#   Check Libraries...
#
#
#*****************************************************************************************
pkg_modules="gtk+-3.0 >= 3.0.8"
GTK_PKG="gtk+-3.0"
CPPFLAGS="$CPPFLAGS -DENABLE_GTK3"
AC_DEFINE(ENABLE_GTK3, 1, [Enable GTK3])
AC_SUBST([LIBFMCORE_API_VERSION], [3.0])
AC_SUBST([LIBFMCORE_SO_VERSION], [3:0:0])
AC_SUBST(GTK_PKG)

PKG_CHECK_MODULES(GTK, [$pkg_modules])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)


glib_modules="glib-2.0 >= 2.24.0 gobject-2.0 >= 2.26.0"
PKG_CHECK_MODULES(GLIB, [$glib_modules])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

gio_modules="gthread-2.0 gio-unix-2.0 >= 2.26.0 glib-2.0 >= 2.26.0 gobject-2.0"
PKG_CHECK_MODULES(GIO, [$gio_modules])
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

pango_modules="pango >= 1.16.0"
PKG_CHECK_MODULES(PANGO, [$pango_modules])
AC_SUBST(PANGO_CFLAGS)
AC_SUBST(PANGO_LIBS)

gmodule_modules="gmodule-2.0"
PKG_CHECK_MODULES(GMODULE, [$gmodule_modules])
AC_SUBST(GMODULE_CFLAGS)
AC_SUBST(GMODULE_LIBS)

menu_modules="libmenu-cache >= 0.3.2"
PKG_CHECK_MODULES(MENU_CACHE, [$menu_modules])
AC_SUBST(MENU_CACHE_CFLAGS)
AC_SUBST(MENU_CACHE_LIBS)


#*****************************************************************************************
#   ...
#
#   special checks for glib/gio 2.27 since it contains backward imcompatible changes.
#   glib 2.26 uses G_DESKTOP_APP_INFO_LOOKUP_EXTENSION_POINT_NAME extension point while
#   glib 2.27 uses x-scheme-handler/* mime-type to register handlers.
#
#*****************************************************************************************
PKG_CHECK_MODULES(GLIB2_27, [glib-2.0 >= 2.27.0],
    [have_scheme_handler="yes"],
    [have_scheme_handler="no"])
AM_CONDITIONAL(HAVE_SCHEME_HANDLER, test "x$have_scheme_handler" = "xyes")
if test "x$have_scheme_handler" = "xyes"; then
    AC_DEFINE_UNQUOTED(HAVE_SCHEME_HANDLER, [1], [Have x-scheme/uri handler support])
fi


#*****************************************************************************************
#   UDisk Support...
#
#
#*****************************************************************************************
AC_ARG_ENABLE(udisks,
    [AC_HELP_STRING([--enable-udisks],
        [build libfmcore with udisks support (Linux only) @<:@default=yes@:>@])],
    [enable_udisks="${enableval}"],
    [enable_udisks=no]
)
AM_CONDITIONAL(ENABLE_UDISKS, test x"$enable_udisks" = x"yes")
if test x"$enable_udisks" = x"yes"; then
    # turn on udisks support
    AC_DEFINE_UNQUOTED(USE_UDISKS, [1], [Enable UDisks support])

    # test for availability of dbus
    dbus_modules="dbus-glib-1"
    PKG_CHECK_MODULES(DBUS, [$dbus_modules])
    AC_SUBST(DBUS_CFLAGS)
    AC_SUBST(DBUS_LIBS)
fi


CPPFLAGS=
#*****************************************************************************************
#   Enable Debug...
#
#
#*****************************************************************************************
AC_ARG_ENABLE(debug,
              [AC_HELP_STRING([--enable-debug],
              [Debug Support @<:@default=no@:>@])],
              [enable_debug="${enableval}"],
              [enable_debug=no])

AC_ARG_ENABLE(job-debug,
              [AC_HELP_STRING([--enable-job-debug],
              [Debug The Job System (default: no)])],
              enable-job-debug=$enableval,
              enable-job-debug="no")

AC_ARG_ENABLE(treeview-debug,
              [AC_HELP_STRING([--enable-treeview-debug],
              [Debug The Tree View (default: no)])],
              enable_treeview_debug=$enableval,
              enable_treeview_debug="no")


if test "$enable_job_debug" = "yes"; then
    CPPFLAGS="$CPPFLAGS -DENABLE_JOB_DEBUG"
    enable_debug="yes"
fi

if test "$enable_treeview_debug" = "yes"; then
    CPPFLAGS+="-DENABLE_TREEVIEW_DEBUG"
    enable_debug="yes"
fi

if test "$enable_debug" = "yes"; then
    # turn on debug and disable optimization
    CPPFLAGS="-O0 -g $CPPFLAGS -DENABLE_DEBUG -DG_ENABLE_DEBUG"
else
    # turn off glib debug checks
    CPPFLAGS="$CPPFLAGS -DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
fi


#*****************************************************************************************
#   GIO extension module...
#
#
#*****************************************************************************************
giomodulesdir=$libdir/gio/modules
AC_SUBST(giomodulesdir)


#*****************************************************************************************
#   GetText Support...
#
#
#*****************************************************************************************
GETTEXT_PACKAGE=libfmcore
AC_SUBST(GETTEXT_PACKAGE)


#*****************************************************************************************
#   Generate po/LINGUAS on the fly...
#
#~ # Generate po/LINGUAS on the fly rather than relying on translators
#~ # to maintain it manually. This also overcome the problem that Transifex
#~ # cannot add a language to po/LINGUAS if a new po file is submitted.
#
#*****************************************************************************************
rm -f $srcdir/po/LINGUAS
for po_file in `ls $srcdir/po/*.po | sort`;
do
    lang=`echo "$po_file" | sed "s|.*/po/\(.*\)\.po|\1|g"`
    echo $lang >> $srcdir/po/LINGUAS
done

AM_GLIB_GNU_GETTEXT
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])


#*****************************************************************************************
#   Output Files...
#
#
#*****************************************************************************************

AC_CONFIG_FILES([
    Makefile
    data/Makefile
    src/Makefile
    src/tests/Makefile
    vapi/Makefile
    libfmcore-${LIBFMCORE_API_VERSION}.pc
])

AC_OUTPUT


#*****************************************************************************************
#   Configuration Summary...
#
#
#*****************************************************************************************
echo "--------------------------------------------------------------------------------"
echo "                    - LibFm Core Configuration Summary -"
echo "--------------------------------------------------------------------------------"
echo "      Version:                                                $VERSION"
echo "      prefix:                                                 $prefix"
echo "      CPPFLAGS: $CPPFLAGS"
echo "      Debugging:                                              $enable_debug"
echo "      Jobs Debug:                                             $enable_job_debug"
echo "      Tree View Debug:                                        $enable_treeview_debug"
echo "      UDisks Support:                                         $enable_udisks"
echo "      Large file support:                                     $largefile"
echo
echo "--------------------------------------------------------------------------------"




