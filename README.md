# Fe3C

*Fast Efficient Edwards Elliptic Curves signatures* - an efficient C implementation of EdDSA for embedded systems

## Build

Build system for the host machine is based on GNU Autotools. To build a native library run:

```bash
autoreconf --install
mkdir build && cd build
../configure [--enable-ed25519] [--enable-ed448]
make && make install
```

For all available build options, see:

```bash
../configure --help
```

## Usage

All public APIs are declared in the `fe3c/eddsa.h` header and the application must be linked against the Fe3C library by passing the flag `-lcementite` to the linker. Below is an example of signature generation, public key derivation, and signature verification in the Ed25519 scheme. Switching to Ed448 amounts to initializing the `curve_id` variable with `EDDSA_CURVE_ED448` instead.

```c
#include <fe3c/eddsa.h>

...
{
    eddsa_curve curve_id = EDDSA_CURVE_ED25519;

    u8 message[MESSAGE_LENGTH];
    u8 secret_key[eddsa_get_secret_key_length(curve_id)];

    ...

    u8 signature[eddsa_get_signature_length(curve_id)];
    /* Sign message with secret_key */
    eddsa_sign_request sreq = {
        .signature = signature,
        .message = message,
        .message_length = MESSAGE_LENGTH,
        .secret_key = secret_key,
        .curve_id = curve_id
    };
    eddsa_sign(&sreq);

    ...

    u8 public_key[eddsa_get_public_key_length(curve_id)];
    /* Derive the public key */
    eddsa_derive_public_key(public_key, secret_key, curve_id);

    /* Verify the signature */
    eddsa_verify_request vreq = {
        .signature = signature,
        .message = message,
        .message_length = MESSAGE_LENGTH,
        .public_key = public_key,
        .curve_id = curve_id
    };
    if (eddsa_verify(&vreq)) {

        /* Signature ok */

    } else {

        /* Signature invalid */
    }
}
```

If the public key is precomputed, it can be added to `eddsa_sign_request` to speed up signature generation. In this case the application is responsible for consistency between the secret and public keys. For more advanced signature scheme variants, such as Ed25519ctx or Ed25519ph, refer to the inline documentation (Doxygen) in `fe3c/eddsa.h` or build the documentation by running:

```bash
doxygen
```

in the root of the repository. The documentation shall be put in the `doc` subdirectory.

**Warning**: The library does not perform any input length validation! APIs are exposed for the application to verify that the (possibly maliciously constructed) inputs will not cause out-of-bounds reads. These include:

```c
int eddsa_get_signature_length(eddsa_curve curve_id);
int eddsa_get_public_key_length(eddsa_curve curve_id);
int eddsa_get_secret_key_length(eddsa_curve curve_id);
/* Check at runtime if the library has been compiled with support for a given curve */
int eddsa_is_curve_supported(eddsa_curve curve_id);
```

Extra safety guards and assertions can be enabled by building with option `--enable-sanity-checks`. This option likely should not be used in production systems as it heavily impacts performance, but can be used by application developers to resolve doubts about validity of arguments.

## Cofactors and compliance

Fe3C uses *cofactored* verification. This means that the Schnorr verification equation `[s]B = R + [Hash(R, A, M)]A` (where `(R, s)` is the signature on the message `M` under the public key `A`) gets multiplied by the underlying curve's *cofactor* (8, in the case of Ed25519, or 4, in the case of Ed448), making it more permissive (points that differ by a small-order point are now considered equivalent). Both [RFC 8032](https://datatracker.ietf.org/doc/html/rfc8032) and [FIPS 186-5 (Digital Signature Standard)](https://csrc.nist.gov/pubs/fips/186-5/final) say that the more permissive verification equation with multiplication by the curve's cofactor determines whether the signature is valid or not, and that it is *sufficient, but not required*, to instead check the cofactorless equation. In fact, the [initial public draft of FIPS 186-5](https://csrc.nist.gov/pubs/fips/186-5/ipd) required always using cofactored verification. This was later relaxed after comments received about the draft from STMicroelectronics: "we would prefer that FIPS 186-5 will more clearly allow, as RFC 8032 does, the usage of cofactorless verification." I argue that neither RFC 8032 nor FIPS 186-5 actually "allows" cofactorless verification. Indeed, it is *sufficient, but not required*, that the cofactorless equation hold. If it holds, then the signature is definitely valid (happy path), but if not, then the "full" equation must be consulted, that is, the "cofactor components" must be cleared. In other words, the cofactorless equation not holding *does not* imply that the signature is invalid.

## Directory tree

Below is the directory tree of the repository:

    .
    ├── esp32                       # ESP32-specific sources and build files
    │   └── components              # Directory to be set in esp-idf as EXTRA_COMPONENT_DIRS
    │       └── cementite
    │           ├── port            # Optimized ESP32-specific implementations of selected components
    │           └── test            # ESP32-specific test cases
    ├── examples                    # Example applications
    │   ├── esp32
    │   └── host
    ├── include                     # Public interface headers
    ├── src                         # Common source files
    │   ├── curves                  # Edwards curves objects
    │   ├── eddsa                   # Signature and verification algorithms
    │   ├── field_elements          # Finite field arithmetic (mod p)
    │   ├── hash_functions          # SHA-512 and SHAKE-256 implementations
    │   ├── points                  # Group arithmetic
    │   ├── scalars                 # Finite field arithmetic (mod L)
    │   └── utils
    └── tests
        ├── benchmarks              # Performance benchmarks
        │   ├── esp32
        │   └── host
        └── functional              # Functional (unit) tests
            ├── esp32               # Test suite build files for ESP32
            ├── host                # Test suite build files for the host machine
            └── src                 # Common unit test sources
                ├── eddsa
                ├── field_elements
                ├── hash_functions
                ├── points
                └── scalars
