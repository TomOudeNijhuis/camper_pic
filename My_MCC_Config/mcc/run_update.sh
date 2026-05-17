#!/bin/sh
# - File: run_pymdfu.sh
# - Description: Shell script that demonstrates the CLI call to initiate a
# -               DFU using the chosen pymdfu host tool.
# -
# - Requirements: pymdfu, python
# ----------------------------------------------------------------------------

# - Serial Command (UART)
pymdfu update -v debug --tool serial --image ./camper_pic_application.img --baudrate 115200 --port /dev/ttyS0


