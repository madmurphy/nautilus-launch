dnl  Process this file with autoconf to produce a configure script.


dnl  **************************************************************************
dnl     A U T O C O N F   E N V I R O N M E N T
dnl  **************************************************************************


AC_PREREQ([2.69])

AC_INIT([Nautilus Launch],
	[1.0.5],
	[madmurphy333@gmail.com],
	[nautilus-launch],
	[https://gitlab.gnome.org/madmurphy/nautilus-launch])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_SRCDIR([src/nautilus-launch.c])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile
src/Makefile
po/Makefile.in])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC

AC_PROG_GREP

PKG_CHECK_MODULES([NAUTILUS_LAUNCH], [glib-2.0 libnautilus-extension])

AC_CHECK_PROG([HAVE_PKGCONFIG], [pkg-config], [yes], [no])

AS_IF([test "x${HAVE_PKGCONFIG}" = xno],
	[AC_MSG_ERROR([you need to have pkgconfig installed!])])


dnl  **************************************************************************
dnl     I N T E R N A T I O N A L I Z A T I O N
dnl  **************************************************************************

AM_GNU_GETTEXT([external])

AM_GNU_GETTEXT_VERSION([0.19])

AM_CONDITIONAL([NLS_ENABLED], [test "x${USE_NLS}" != xno])

AM_COND_IF([NLS_ENABLED], [
	AC_SUBST([GETTEXT_PACKAGE], AC_PACKAGE_TARNAME)
	AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],
		["${GETTEXT_PACKAGE}"], [GETTEXT package name])
])


dnl  **************************************************************************
dnl     F I N A L I Z A T I O N
dnl  **************************************************************************

AC_ARG_WITH([nautilus-libdir],
	[AS_HELP_STRING([--without-nautilus-libdir],
		[ignore the <libdir> variable used by libnautilus-extension and
		and use the current <libdir> instead @<:@default=no@:>@])],
	[:],
	[AS_VAR_SET([with_nautilus_libdir], ['yes'])])

AS_IF([test "x${with_nautilus_libdir}" = xno],
	[AS_VAR_SET([NAUTILUS_EXTENSION_DIR], m4_normalize(["$(
		"${PKG_CONFIG}" --define-variable='libdir=${libdir}'
			--variable=extensiondir libnautilus-extension
	)"]))],
	[AS_VAR_SET([NAUTILUS_EXTENSION_DIR], m4_normalize(["$(
		"${PKG_CONFIG}" --variable=extensiondir libnautilus-extension
	)"]))])

AC_SUBST([NAUTILUS_EXTENSION_DIR])


LT_INIT([disable-static])

AC_OUTPUT


dnl  EOF

