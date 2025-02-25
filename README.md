# Robo-ARM
A simple robot arm, running on nRF52840 and Zephyr.

## Getting Started

Before getting started, make sure you have a proper Zephyr development
environment. Follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

### Initialization

The first step is to initialize the workspace folder (``workspace``).

Run the following
command:

```shell
# initialize workspace for Robo-ARM (main branch)
west init -m https://github.com/seankyer/Robo-ARM --mr main workspace
# update Zephyr modules
cd workspace
west update
```

### nRF Toolchain Installation

Ensure you have Nordic commandline tools installed so you can make use of the ``west flash`` command later.

Install:
1. [nRF command line tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools/download#infotabs)

2. [JLink](https://www.segger.com/downloads/jlink)

### Building and running

To build the application, run the following command:

```shell
cd example-application
west build -b nrf52840dk/nrf52840 app
```

Once you have built the application, run the following command to flash it:

```shell
west flash
```

### Testing

To execute Twister integration tests, run the following command:

```shell
west twister -T tests --integration
```
