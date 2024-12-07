LIB = $(top_builddir)/lib

VPATH = $(srcdir) $(builddir)

lib_LTLIBRARIES = $(LIB)/libcementite.la

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/src

AM_LDFLAGS += -version-number '$(FE3C_VERSION_LIBSO)'

cementiteincludedir = $(includedir)/fe3c
cementiteinclude_HEADERS = \
    $(top_srcdir)/include/fe3c/eddsa.h

# Common sources
__LIB__libcementite_la_SOURCES = \
    eddsa/eddsa.c \
    curves/curves.c

# ------------------------- Ed25519 sources -------------------------
if FE3C_SUPPORT_CURVE_ED25519
__LIB__libcementite_la_SOURCES += \
    curves/curve_ed25519.c \
    hash_functions/hash_function_sha512.c \
    points/points_ed25519.c \
    scalars/scalars_ed25519_$(archbits).c \
    field_elements/field_elements_ed25519_$(archbits).c

# --------------------------- Comb method ---------------------------
if FE3C_ED25519_COMB_METHOD

if FE3C_ED25519_SMALL_PRECOMPUTATION
ed25519_small_precomp_tag = _small
else
ed25519_small_precomp_tag =
endif

__LIB__libcementite_la_SOURCES += \
    points/comb/comb_ed25519$(ed25519_small_precomp_tag).c \
    points/comb/precomputation/precomputation_ed25519$(ed25519_small_precomp_tag)_$(archbits).c

endif
# -------------------------------------------------------------------

# --------------------- Lattice basis reduction ---------------------
if FE3C_ED25519_LATTICE_BASIS_REDUCTION
__LIB__libcementite_la_SOURCES += \
    points/lattice_basis_reduction/lattices_ed25519.c
endif
# -------------------------------------------------------------------

endif
# -------------------------------------------------------------------

# -------------------------- Ed448 sources --------------------------
if FE3C_SUPPORT_CURVE_ED448
__LIB__libcementite_la_SOURCES += \
    curves/curve_ed448.c \
    hash_functions/hash_function_shake256.c \
    scalars/scalars_ed448_$(archbits).c \
    field_elements/field_elements_ed448_$(archbits).c \
    points/points_ed448.c

# --------------------------- Comb method ---------------------------
if FE3C_ED448_COMB_METHOD

if FE3C_ED448_SMALL_PRECOMPUTATION
ed448_small_precomp_tag = _small
else
ed448_small_precomp_tag =
endif

__LIB__libcementite_la_SOURCES += \
    points/comb/comb_ed448$(ed448_small_precomp_tag).c \
    points/comb/precomputation/precomputation_ed448$(ed448_small_precomp_tag)_$(archbits).c

endif
# -------------------------------------------------------------------

# --------------------- Lattice basis reduction ---------------------
if FE3C_ED448_LATTICE_BASIS_REDUCTION
__LIB__libcementite_la_SOURCES += \
    points/lattice_basis_reduction/lattices_ed448.c
endif
# -------------------------------------------------------------------

endif
# -------------------------------------------------------------------
