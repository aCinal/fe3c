AC_PREREQ([2.68])
AC_INIT([Fe3C], [1.0])

# Keep the root directory clean and put automake's scripts in a subdirectory
AC_CONFIG_AUX_DIR([build-aux])

# Use C as the language of choice
AC_LANG([C])

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

# Set common flags
AM_CPPFLAGS="$AM_CPPFLAGS"
AM_CFLAGS="$AM_CFLAGS -Wall -Werror"
AM_LDFLAGS="$AM_LDFLAGS"

# Use libtool
LT_INIT([])

# Export the .so version to the Makefiles
AC_SUBST([FE3C_VERSION_LIBSO], [1:0:0])

# Check if -fvisibility=hidden is supported to reduce .so size
VISIBILITY_CFLAGS="-fvisibility=hidden"
AC_CACHE_CHECK([whether -fvisibility=hidden is supported],
               [fe3c_cv_visibility_hidden_supported],
               [OLD_CFLAGS="$CFLAGS"
                CFLAGS="$CFLAGS $VISIBILITY_CFLAGS"
                AC_LINK_IFELSE([AC_LANG_PROGRAM()],
                               [fe3c_cv_visibility_hidden_supported=yes],
                               [fe3c_cv_visibility_hidden_supported=no])
                CFLAGS="$OLD_CFLAGS"])
# If failed to link with -fvisibility=hidden, reset VISIBILITY_CFLAGS
AS_IF([test "x$fe3c_cv_visibility_hidden_supported" != "xyes"],
      [VISIBILITY_CFLAGS=""])
AM_CFLAGS="$AM_CFLAGS $VISIBILITY_CFLAGS"



# -------------------------------------------------------------------------------------------
# -------------------------------- Supported elliptic curves --------------------------------
# -------------------------------------------------------------------------------------------
ed25519="no"
AC_ARG_ENABLE([ed25519],
              [AS_HELP_STRING([--enable-ed25519], [Enable support for the Ed25519 curve])],
              [AS_IF([test "x$enableval" = "xyes"],
                     [ed25519="yes"],

                     [test "x$enableval" != "xno"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-ed25519])])],
              [])
AM_CONDITIONAL([FE3C_SUPPORT_CURVE_ED25519], [test "x$ed25519" = "xyes"])
AS_IF([test "x$ed25519" = "xyes"], AC_DEFINE([FE3C_SUPPORT_CURVE_ED25519]), [])

# -------------------------------------------------------------------------------------------

ed448="no"
AC_ARG_ENABLE([ed448],
              [AS_HELP_STRING([--enable-ed448], [Enable support for the Ed448 curve])],
              [AS_IF([test "x$enableval" = "xyes"],
                     [ed448="yes"],

                     [test "x$enableval" != "xno"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-ed448])])],
              [])
AM_CONDITIONAL([FE3C_SUPPORT_CURVE_ED448], [test "x$ed448" = "xyes"])
AS_IF([test "x$ed448" = "xyes"], AC_DEFINE([FE3C_SUPPORT_CURVE_ED448]), [])
# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------



# -------------------------------------------------------------------------------------------
# ----------------------------- Optional features/optimizations -----------------------------
# -------------------------------------------------------------------------------------------
ed448_isogeny="no"
AC_ARG_ENABLE([ed448-isogeny],
              [AS_HELP_STRING([--enable-ed448-isogeny],
                              [Enable performing the Ed448 computations on a 4-isogenous curve, which
                               may increase performance, but amounts to using cofactored verification,
                               which may lead to consensus issues where different implementations
                               disagree on the validity of maliciously constructed signatures.])],
              [AS_IF([test "x$enableval" = "xyes"],
                     [ed448_isogeny="yes"],

                     [test "x$enableval" != "xno"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-ed448-isogeny])])],
              [])
AM_CONDITIONAL([FE3C_ED448_ISOGENY], [test "x$ed448_isogeny" = "xyes"])
AS_IF([test "x$ed448_isogeny" = "xyes"], AC_DEFINE([FE3C_ED448_ISOGENY]), [])

# -------------------------------------------------------------------------------------------

small_precomputation="no"
AC_ARG_ENABLE([small-precomputation],
              [AS_HELP_STRING([--enable-small-precomputation],
                              [Minimize the size of the precomputation tables when using the comb method.
                               This may result in a smaller library size at the cost of decreased
                               performance.])],
              [AS_IF([test "x$enableval" = "xyes"],
                     [small_precomputation="yes"],

                     [test "x$enableval" != "xno"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-small-precomputation])])],
              [])
AM_CONDITIONAL([FE3C_SMALL_PRECOMPUTATION], [test "x$small_precomputation" = "xyes"])
AS_IF([test "x$small_precomputation" = "xyes"], AC_DEFINE([FE3C_SMALL_PRECOMPUTATION]), [])

# -------------------------------------------------------------------------------------------

fast_squaring="yes"
AC_ARG_ENABLE([fast-squaring],
              [AS_HELP_STRING([--disable-fast-squaring],
                              [Disable optimization of squaring in the field and reuse multiplication routine,
                               which should reduce code footprint but may decrease performance])],
              [AS_IF([test "x$enableval" = "xno"],
                     [fast_squaring="no"],

                     [test "x$enableval" != "xyes"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-fast-squaring])])],
              [])
AM_CONDITIONAL([FE3C_FAST_SQUARING], [test "x$fast_squaring" = "xyes"])
AS_IF([test "x$fast_squaring" = "xyes"], AC_DEFINE([FE3C_FAST_SQUARING]), [])

# -------------------------------------------------------------------------------------------

comb_method="yes"
AC_ARG_ENABLE([comb-method],
              [AS_HELP_STRING([--disable-comb-method],
                              [Disable comb method of scalar multiplication, which may reduce data footprint,
                               but will definitely decrease performance])],
              [AS_IF([test "x$enableval" = "xno"],
                     [comb_method="no"],

                     [test "x$enableval" != "xyes"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-comb-method])])],
              [])
AM_CONDITIONAL([FE3C_COMB_METHOD], [test "x$comb_method" = "xyes"])
AS_IF([test "x$comb_method" = "xyes"], AC_DEFINE([FE3C_COMB_METHOD]), [])

# -------------------------------------------------------------------------------------------

skip_zeroization="no"
AC_ARG_ENABLE([zeroization],
              [AS_HELP_STRING([--disable-zeroization],
                              [Disable zeroization of intermediate results on stack, which may marginally increase
                               performance and reduce code footprint, but decreases security in systems where stack
                               is shared with unsafe code, which may read data left behind by crypto operations])],
              [AS_IF([test "x$enableval" = "xno"],
                     [skip_zeroization="yes"],

                     [test "x$enableval" != "xyes"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-zeroization])])],
              [])
AM_CONDITIONAL([FE3C_SKIP_ZEROIZATION], [test "x$skip_zeroization" = "xyes"])
AS_IF([test "x$skip_zeroization" = "xyes"], AC_DEFINE([FE3C_SKIP_ZEROIZATION]), [])
# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------



# -------------------------------------------------------------------------------------------
# --------------------------------- Architecture intrinsics ---------------------------------
# -------------------------------------------------------------------------------------------
archbits="64"
AC_ARG_ENABLE([32bit],
              [AS_HELP_STRING([--enable-32bit],
                              [Optimize elliptic curve arithmetic for 32-bit architecture(s)])],
              [AS_IF([test "x$enableval" = "xyes"],
                     [archbits="32"],

                     [test "x$enableval" != "xno"],
                     [AC_MSG_ERROR([bad value ${enableval} for --enable-32bit])])],
              [])
AC_SUBST([archbits])
AS_IF([test "x$archbits" = "x32"], AC_DEFINE([FE3C_32BIT]), AC_DEFINE([FE3C_64BIT]))
# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------



# -------------------------------------------------------------------------------------------
# -------------------------------------- Debug options --------------------------------------
# -------------------------------------------------------------------------------------------
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
# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------

AC_CONFIG_FILES([Makefile src/Makefile])

# Export the changed variables to the Makefiles
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_LDFLAGS])

# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------

AC_OUTPUT
AC_MSG_RESULT([
    $PACKAGE $VERSION

    Curve Ed25519 supported:                ${ed25519}
    Curve Ed448 supported:                  ${ed448}

    Architecture:                           ${archbits}-bit
    Sanity checks enabled:                  ${sanity_checks}

    Using 4-isogenous curve of Ed448:       ${ed448_isogeny}
    Optimized field element squaring:       ${fast_squaring}
    Comb method for scalar multiplication:  ${comb_method}
    Small precomputation tables:            ${small_precomputation}
    Skip secret zeroization on stack:       ${skip_zeroization}
])
