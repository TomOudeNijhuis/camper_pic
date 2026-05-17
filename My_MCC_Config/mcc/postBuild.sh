mic# - File: postBuild.sh
# - Description: Shell script that can be executed in the MPLAB X post build
# -               step to build the application binary image.
# - 
# - Requirements: pyfwimagebuilder, python, hexmate
# - Arguments:
# -     $1 - the IS_DEBUG argument is passed by the MPLAB post build step to identify if the application is being built to a hex file (for production) or an elf file (for debugging).
# -     $2 - the PROJECT_NAME argument is passed by the MPLAB build step to identify the project name of the application.
# -     $3 - the INPUT_IMAGE_DIR argument is passed by the MPLAB post build step and it holds the output image directory. (.hex or .elf normally).
# -     $4 - the INPUT_IMAGE_TYPE argument is passed by the MPLAB post build to identify the image type used.
# -     $5 - the IS_COMBINED argument is passed by the MPLAB post build to verify that a combined configuration is to be created.
# ----------------------------------------------------------------------------

IS_DEBUG=$1
PROJECT_NAME=$2
INPUT_IMAGE_DIR=$3
INPUT_IMAGE_TYPE=$4
INPUT_IMAGE_PATH=$INPUT_IMAGE_DIR/$PROJECT_NAME.hex
UNIFIED_HEX_FILE=$INPUT_IMAGE_DIR/$PROJECT_NAME.unified.hex
IS_COMBINED=$([ "$5" = "COMBINED" ] && echo true || echo false)

if [ "$IS_DEBUG" = false ]; then
    # - Fill Unimplemented Flash Space with 0x3FFF
    /opt/microchip/xc8/v2.46/pic/bin/hexmate r0-FFFFFFFF,$INPUT_IMAGE_PATH -O$INPUT_IMAGE_PATH -FILL=0x3FFF@0x2000:0x7FFF

    # - Calculate CRC-16 and store at footer
    /opt/microchip/xc8/v2.46/pic/bin/hexmate $INPUT_IMAGE_PATH -O$INPUT_IMAGE_PATH +-CK=0x2000-0x7FFB@0x7FFC+FFFFg5w-2p1021t3F.1
    
    # - Build the application binary
    /opt/microchip/pymdfu/pyfwimagebuilder-bin build -e -i $INPUT_IMAGE_PATH -c ./bootloader_configuration_copy.toml -o camper_pic_application.img

    # - Create a combined conifguration
    if [ "$IS_COMBINED" = "true" ]; then
        /opt/microchip/xc8/v2.46/pic/bin/hexmate r0x2000-0x7FFF,$INPUT_IMAGE_PATH r0-0x1FFF,$UNIFIED_HEX_FILE -o$UNIFIED_HEX_FILE
    fi
fi