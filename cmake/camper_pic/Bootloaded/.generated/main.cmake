include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(camper_pic_Bootloaded_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group Bootloaded_toolchain
if(camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemble)
add_library(camper_pic_Bootloaded_Bootloaded_toolchain_assemble OBJECT ${camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemble})
    camper_pic_Bootloaded_Bootloaded_toolchain_assemble_rule(camper_pic_Bootloaded_Bootloaded_toolchain_assemble)
    list(APPEND camper_pic_Bootloaded_library_list "$<TARGET_OBJECTS:camper_pic_Bootloaded_Bootloaded_toolchain_assemble>")

endif()

# Handle files with suffix S, for group Bootloaded_toolchain
if(camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemblePreprocess)
add_library(camper_pic_Bootloaded_Bootloaded_toolchain_assemblePreprocess OBJECT ${camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_assemblePreprocess})
    camper_pic_Bootloaded_Bootloaded_toolchain_assemblePreprocess_rule(camper_pic_Bootloaded_Bootloaded_toolchain_assemblePreprocess)
    list(APPEND camper_pic_Bootloaded_library_list "$<TARGET_OBJECTS:camper_pic_Bootloaded_Bootloaded_toolchain_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group Bootloaded_toolchain
if(camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_compile)
add_library(camper_pic_Bootloaded_Bootloaded_toolchain_compile OBJECT ${camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_compile})
    camper_pic_Bootloaded_Bootloaded_toolchain_compile_rule(camper_pic_Bootloaded_Bootloaded_toolchain_compile)
    list(APPEND camper_pic_Bootloaded_library_list "$<TARGET_OBJECTS:camper_pic_Bootloaded_Bootloaded_toolchain_compile>")

endif()


# Main target for this project
add_executable(camper_pic_Bootloaded_image_45lDJ2bn ${camper_pic_Bootloaded_library_list})

set_target_properties(camper_pic_Bootloaded_image_45lDJ2bn PROPERTIES
    OUTPUT_NAME "Bootloaded"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${camper_pic_Bootloaded_output_dir}")
target_link_libraries(camper_pic_Bootloaded_image_45lDJ2bn PRIVATE ${camper_pic_Bootloaded_Bootloaded_toolchain_FILE_TYPE_link})

# Add the link options from the rule file.
camper_pic_Bootloaded_link_rule( camper_pic_Bootloaded_image_45lDJ2bn)


# The following step will be performed after each build if final image is rebuilt
add_custom_command(TARGET camper_pic_Bootloaded_image_45lDJ2bn POST_BUILD
    COMMAND cd My_MCC_Config/mcc && ./postBuild.sh false Bootloaded ../../out/camper_pic production
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../../..)
