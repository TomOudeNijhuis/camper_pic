# camper_pic

Firmware for the PIC16F18056 on the customer board from the
[camper_interface_pca](https://github.com/) project.

## Build configurations

The project has two build configurations:

- **default** — standalone firmware that can be programmed directly to the MCU
  (developed using a [MPLAB Snap](https://www.microchip.com/en-us/development-tool/pg164100)
  programmer).
- **Bootloaded** — firmware to be used together with the
  [camper_pic_bootloader](https://github.com/) project. The resulting image can
  be uploaded over the bootloader using
  [pymdfu](https://pypi.org/project/pymdfu/).

## Toolchain

Built with [VS Code](https://code.visualstudio.com/) and the
[MPLAB extensions](https://marketplace.visualstudio.com/search?term=MPLAB&target=VSCode).
Peripheral configuration is managed via MCC (see `My_MCC_Config/`).

## Serial console

To open the serial console on Linux:

```sh
picocom -b 115200 /dev/ttyS0
```

## Update using bootloader

```sh
# Query the connected bootloader client
pymdfu client-info --tool serial --port /dev/ttyS0 --baudrate 115200

# Push a new application image
pymdfu update --tool serial --image camper_pic_application.img --port /dev/ttyS0 --baudrate 115200
```

```sh
sudo systemctl stop camper_api.service
```