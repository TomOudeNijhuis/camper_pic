# The following variables contains the files used by the different stages of the build process.
set(camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemble)
set_source_files_properties(${camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemblePreprocess)
set_source_files_properties(${camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemblePreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemblePreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/drivers/src/adc_sampler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/drivers/src/commands.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/drivers/src/crc16.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/drivers/src/errors.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/drivers/src/neopixels.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/drivers/src/power.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/drivers/src/protocol.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/drivers/src/telemetry.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/footer_certificate.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/mcc_generated_files/adc/src/adc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/mcc_generated_files/fvr/src/fvr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/mcc_generated_files/system/src/clock.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/mcc_generated_files/system/src/config_bits.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/mcc_generated_files/system/src/interrupt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/mcc_generated_files/system/src/pins.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/mcc_generated_files/system/src/system.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/mcc_generated_files/timer/src/tmr0.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../My_MCC_Config/mcc/mcc_generated_files/uart/src/eusart1.c")
set_source_files_properties(${camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_link)
set(camper_pic_Bootloaded_image_name "Bootloaded.elf")
set(camper_pic_Bootloaded_image_base_name "Bootloaded")

# The output directory of the final image.
set(camper_pic_Bootloaded_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/camper_pic")

# The full path to the final image.
set(camper_pic_Bootloaded_full_path_to_image ${camper_pic_Bootloaded_output_dir}/${camper_pic_Bootloaded_image_name})

# Potential output file extensions
set(output_extensions
    .hex
    .hxl
    .mum
    .o
    .sdb
    .sym
    .cmf)
list(TRANSFORM output_extensions PREPEND "${camper_pic_Bootloaded_output_dir}/${camper_pic_Bootloaded_image_base_name}")
