# Fe3C tests

## Build and run tests

To build and run the tests use

```bash
./run_tests
```

The `run_tests` script will call the CMake build system, fetch the CppUTest harness, build each functional test into a separate executable and then iterate over the generated binaries and execute them.
