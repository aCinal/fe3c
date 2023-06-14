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

## Issues of the cofactor

**Note**: This is an advanced section, which can be skipped by most users.

Fe3C uses strictly cofactorless signature verification by default. This is in contrast to, e.g., OpenSSL (as of June 2023), which employs Hamburg's 4-isogeny on Ed448 (see: [paper](https://ia.cr/2014/027)), effectively clearing the small-order components of points (points in the subgroup of order equal to the cofactor), much like in cofactored verification. Below is an example Ed448 vector (in Python syntax) which will be accepted by OpenSSL, but rejected by, e.g., WolfSSL, as well as by Fe3C in the default configuration:

```python
message = ""
signature = \
    "83084a86c7092bdbca6c56ce17c32d37915288970a97c5c6f9be2ab73" + \
    "7bae207b6620e6be3ac481bf4f02d2a005a39bf90d44edeea35c6da00" + \
    "bf8d6deecf827632ecade6fe93e9c067ac0fbdf05504b0fb23aad184f" + \
    "47f23fcac37132af6f95c87592d067dfbf9865477abb9d597a4fa3a00"
public_key = \
    "a028bb64a64b9e02d31878139e952b95e25ecbdb7a58f1e075f158a27" + \
    "e169887120edb8964b938f9e42987c20e1af0932e05e5415017da9e00"
```

The vector was generated by taking the first Ed448 test vector from RFC 8032, adding the point `(0, -1)` to the public key, and recomputing the signature under this new public key.

Support for the 4-isogeny can be enabled by building with option `--enable-ed448-isogeny`. This may yield some performance benefits, but broadens the set of verifiable signatures. Use at your own discretion!

Note that under cofactorless verification, it is still possible that mixed-order public key (point in the subgroup of order `cL`, where `L` is the order of the basepoint and `c` is the cofactor) gets accepted during signature verification, if the Schnorr challenge (hash over the commitment, the public key, and the message) happens to be a multiple of the cofactor (or just the order of the public key divided by `L`).
