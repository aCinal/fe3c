AC_PREREQ([2.68])
AC_INIT([Fe3C], [1.0])

# Keep the root directory clean and put automake's scripts in a subdirectory
AC_CONFIG_AUX_DIR([build-aux])

# Use automake
# subdir-objects  --> put objects into subdirectories named after the source files
# foreign         --> not require all the GNU Coding Style files such as NEWS, README, AUTHORS, etc.
# -Wall -Werror   --> turn all warnings into errors
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Werror])
# Sanity-check correct srcdir
AC_CONFIG_SRCDIR([include/fe3c/eddsa.h])
# Do not build in verbose mode by default
AM_SILENT_RULES([yes])

# Check for the C compiler
AC_PROG_CC
# Check for the archiver
AM_PROG_AR
# Check for ranlib
AC_PROG_RANLIB

# Check for header files
AC_CHECK_HEADERS([stdint.h stddef.h])

# Check for typedefs and compiler characteristics
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_C_BIGENDIAN(
  AC_DEFINE(FE3C_BIGENDIAN_TARGET),
  AC_DEFINE(FE3C_LILENDIAN_TARGET),
  AC_MSG_WARN([Cannot determine endianness])
)

AM_CFLAGS="$AM_CFLAGS -Wall -Werror"
AC_SUBST([AM_CFLAGS])
AM_CPPFLAGS="$AM_CPPFLAGS"
AC_SUBST([AM_CPPFLAGS])

# -------------------------------- Supported elliptic curves --------------------------------
ed25519="no"
AC_ARG_ENABLE([ed25519],
              [AS_HELP_STRING([--enable-ed25519], [Enable support for the ED25519 curve])],
              [AS_IF([test "x$enableval" = "xyes"],
                     [ed25519="yes"],

                     [test "x$enableval" != "xno"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-ed25519])])],
              [])
AM_CONDITIONAL([FE3C_SUPPORT_CURVE_ED25519], [test "x$ed25519" = "xyes"])
AS_IF([test "x$ed25519" = "xyes"], AC_DEFINE([FE3C_SUPPORT_CURVE_ED25519]), [])

ed448="no"
AC_ARG_ENABLE([ed448],
              [AS_HELP_STRING([--enable-ed448], [Enable support for the ED448 curve])],
              [AS_IF([test "x$enableval" = "xyes"],
                     [ed448="yes"],

                     [test "x$enableval" != "xno"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-ed448])])],
              [])
AM_CONDITIONAL([FE3C_SUPPORT_CURVE_ED448], [test "x$ed448" = "xyes"])
AS_IF([test "x$ed448" = "xyes"], AC_DEFINE([FE3C_SUPPORT_CURVE_ED448]), [])
# -------------------------------------------------------------------------------------------

optimize32="no"
AC_ARG_ENABLE([32bit],
              [AS_HELP_STRING([--enable-32bit],
                              [Optimize elliptic curve arithmetic for 32-bit architecture(s)])],
              [AS_IF([test "x$enableval" = "xyes"],
                     [optimize32="yes"],

                     [test "x$enableval" != "xno"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-32bit])])],
              [])
AM_CONDITIONAL([FE3C_32BIT], [test "x$optimize32" = "xyes"])
AS_IF([test "x$optimize32" = "xyes"], AC_DEFINE([FE3C_32BIT]), AC_DEFINE([FE3C_64BIT]))

sanity_checks="no"
AC_ARG_ENABLE([sanity-checks],
              [AS_HELP_STRING([--enable-sanity-checks],
                              [Enable sanity-check assertions in the code])],
              [AS_IF([test "x$enableval" = "xyes"],
                     [sanity_checks="yes"],

                     [test "x$enableval" != "xno"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-sanity-checks])])]
              [])
AS_IF([test "x$sanity_checks" = "xyes"], AC_DEFINE([FE3C_ENABLE_SANITY_CHECKS]), [])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
AC_MSG_RESULT([
    $PACKAGE $VERSION

    Curve Ed25519 supported:             ${ed25519}
    Curve Ed448 supported:               ${ed448}

    Optimized for 32-bit architectures:  ${optimize32}
    Sanity checks enabled:               ${sanity_checks}
])
