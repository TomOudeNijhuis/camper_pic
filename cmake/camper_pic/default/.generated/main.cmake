include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(camper_pic_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(camper_pic_default_default_XC8_FILE_TYPE_assemble)
add_library(camper_pic_default_default_XC8_assemble OBJECT ${camper_pic_default_default_XC8_FILE_TYPE_assemble})
    camper_pic_default_default_XC8_assemble_rule(camper_pic_default_default_XC8_assemble)
    list(APPEND camper_pic_default_library_list "$<TARGET_OBJECTS:camper_pic_default_default_XC8_assemble>")

endif()

# Handle files with suffix S, for group default-XC8
if(camper_pic_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(camper_pic_default_default_XC8_assemblePreprocess OBJECT ${camper_pic_default_default_XC8_FILE_TYPE_assemblePreprocess})
    camper_pic_default_default_XC8_assemblePreprocess_rule(camper_pic_default_default_XC8_assemblePreprocess)
    list(APPEND camper_pic_default_library_list "$<TARGET_OBJECTS:camper_pic_default_default_XC8_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC8
if(camper_pic_default_default_XC8_FILE_TYPE_compile)
add_library(camper_pic_default_default_XC8_compile OBJECT ${camper_pic_default_default_XC8_FILE_TYPE_compile})
    camper_pic_default_default_XC8_compile_rule(camper_pic_default_default_XC8_compile)
    list(APPEND camper_pic_default_library_list "$<TARGET_OBJECTS:camper_pic_default_default_XC8_compile>")

endif()


# Main target for this project
add_executable(camper_pic_default_image_1P8H2elx ${camper_pic_default_library_list})

set_target_properties(camper_pic_default_image_1P8H2elx PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${camper_pic_default_output_dir}")
target_link_libraries(camper_pic_default_image_1P8H2elx PRIVATE ${camper_pic_default_default_XC8_FILE_TYPE_link})

# Add the link options from the rule file.
camper_pic_default_link_rule( camper_pic_default_image_1P8H2elx)


