# Fe3C

*Fast Efficient Edwards Elliptic Curves signatures* - an efficient C implementation of EdDSA for embedded systems

## Build

To build and install a static library for the host machine run:

```bash
autoreconf --install
mkdir build && cd build
../configure [--enable-ed25519] [--enable-ed448]
make && make install
```

Optionally, you can pass `--enable-32bit` to `configure` to use elliptic curve arithmetic optimized for 32-bit architectures. Note that the default requires `stdint.h` to define 128-bits-wide integers (or support for the *tetra-integer* (TI) mode)

Link against the installed library by passing the flag `-lcementite` to the linker.

## Supported curves

To get a list of supported elliptic (Edwards) curves run:

```bash
./configure --help
```
