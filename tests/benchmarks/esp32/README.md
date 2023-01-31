# ESP32 benchmark

Clone the `https://github.com/espressif/esp-idf` repository and set up the ESP-IDF environment according to the instructions. To customize the `fe3c` build (e.g. enable/disable optimizations or support for different curves) run:

```bash
idf.py menuconfig
```

and find the `cementite (fe3c)` options. Build the benchmark using the command:

```bash
idf.py build
```

To then flash the board and monitor the runtime run:

```bash
idf.py flash monitor
```

The benchmark prints its current status as well as the results via UART-USB bridge and to the host computer for inspection.

Note: it may be necessary to disable the Task Watchdog Timer or set its timeout period to a large enough value, since the benchmark does not yield until completion. To do this run:

```bash
idf.py menuconfig
```

and navigate to `Component config -> ESP System Settings -> Enable Task Watchdog Timer` (as of ESP-IDF v5.1).
