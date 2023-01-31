# Fe3C example ESP32 application

A simple HTTP server implementing a signing oracle.

## Build

Clone the `https://github.com/espressif/esp-idf` repository and set up the ESP-IDF environment according to the instructions. To customize the `fe3c` build (e.g. enable/disable optimizations or support for different curves) run:

```bash
idf.py menuconfig
```

and find the `cementite (fe3c)` options. Build the example application using the command:

```bash
idf.py build
```

## Run

Flash the board (automatically find the port) and monitor its startup:

```bash
idf.py flash monitor
```

If startup is successful and you see something like `I (1485) example: Starting the signing oracle server on port 80...` you can start issuing HTTP requests (press `Ctrl+]` to exit the monitor). A convenience Python script `client.py` is provided for this purpose. Run:

```bash
python client.py --help
```

to learn more.

Tested with ESP-IDF v5.1.
