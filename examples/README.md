# Fe3C example application

## Build

To build the example application make sure you have built and installed the `libcementite` library first, and then run:

```bash
make
```

## Run

Try deriving a public key from a secret key (for example using the curve Ed448). Note that EdDSA secret key is just a string of bits, so we take it from any file, in this case from the source file:
```bash
./example --keygen --ed448 --secret_key=example.c > example.pub
```

To sign a given file (e.g. the Makefile) run:

```bash
./example --sign --ed448 --message=Makefile --secret_key=example.c > example.sig
```

To verify the signature run:

```bash
./example --verify --ed448 --message=Makefile --public_key=example.pub --signature=example.sig
```

No output means the verification was successful. You can also check the process exit code:

```bash
echo $?
```
