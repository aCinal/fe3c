LIB = $(top_builddir)/lib

VPATH = $(srcdir) $(builddir)

lib_LTLIBRARIES = $(LIB)/libcementite.la

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/src

AM_LDFLAGS += -version-number '$(FE3C_VERSION_LIBSO)'

cementiteincludedir = $(includedir)/fe3c
cementiteinclude_HEADERS = \
    $(top_srcdir)/include/fe3c/eddsa.h

__LIB__libcementite_la_SOURCES = \
    eddsa/eddsa.c \
    curves/curves.c

if FE3C_SUPPORT_CURVE_ED448
__LIB__libcementite_la_SOURCES += \
    curves/curve_ed448.c \
    hash_functions/hash_function_shake256.c \
    points/points_ed448.c

if FE3C_32BIT
__LIB__libcementite_la_SOURCES += \
    scalars/scalars_ed448_32.c \
    field_elements/field_elements_ed448_32.c
else
__LIB__libcementite_la_SOURCES += \
    scalars/scalars_ed448_64.c \
    field_elements/field_elements_ed448_64.c
endif

endif

if FE3C_SUPPORT_CURVE_ED25519
__LIB__libcementite_la_SOURCES += \
    curves/curve_ed25519.c \
    hash_functions/hash_function_sha512.c \
    points/points_ed25519.c

if FE3C_32BIT
__LIB__libcementite_la_SOURCES += \
    scalars/scalars_ed25519_32.c \
    field_elements/field_elements_ed25519_32.c
else
__LIB__libcementite_la_SOURCES += \
    scalars/scalars_ed25519_64.c \
    field_elements/field_elements_ed25519_64.c
endif

endif
