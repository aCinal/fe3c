
AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/src

lib_LIBRARIES = libcementite.a
cementiteincludedir = $(includedir)/fe3c
cementiteinclude_HEADERS = \
    $(top_srcdir)/include/fe3c/eddsa.h

libcementite_a_SOURCES = \
    eddsa/eddsa.c \
    curves/curves.c

if FE3C_SUPPORT_CURVE_ED448
libcementite_a_SOURCES += \
    curves/curve_ed448.c \
    hash_functions/hash_function_shake256.c \
    points/points_ed448.c

if FE3C_32BIT
libcementite_a_SOURCES += \
    scalars/scalars_ed448_32.c \
    field_elements/field_elements_ed448_32.c
else
libcementite_a_SOURCES += \
    scalars/scalars_ed448_64.c \
    field_elements/field_elements_ed448_64.c
endif

endif

if FE3C_SUPPORT_CURVE_ED25519
libcementite_a_SOURCES += \
    curves/curve_ed25519.c \
    hash_functions/hash_function_sha512.c \
    points/points_ed25519.c

if FE3C_32BIT
libcementite_a_SOURCES += \
    scalars/scalars_ed25519_32.c \
    field_elements/field_elements_ed25519_32.c
else
libcementite_a_SOURCES += \
    scalars/scalars_ed25519_64.c \
    field_elements/field_elements_ed25519_64.c
endif

endif
