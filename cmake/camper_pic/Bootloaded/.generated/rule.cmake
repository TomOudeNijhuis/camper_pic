# The following functions contains all the flags passed to the different build stages.

set(PACK_REPO_PATH "/home/oudenijhuis/.mchp_packs" CACHE PATH "Path to the root of a pack repository.")

function(camper_pic_Bootloaded_Bootloaded_toolchain_assemble_rule target)
    set(options
        "-c"
        "${MP_EXTRA_AS_PRE}"
        "-mcpu=16F18056"
        "-mdfp=${PACK_REPO_PATH}/Microchip/PIC16F1xxxx_DFP/1.31.465/xc8"
        "-fno-short-double"
        "-fno-short-float"
        "-mrom=default,-000-FFF"
        "-O2"
        "-fasmfile"
        "-maddrqual=require"
        "-mwarn=-3"
        "-msummary=-psect,-class,+mem,-hex,-file"
        "-mcodeoffset=0x1000"
        "-ginhx32"
        "-Wl,--data-init"
        "-mkeep-startup"
        "-mno-osccal"
        "-mno-resetbits"
        "-mno-save-resetbits"
        "-mno-download"
        "-mno-stackcall"
        "-mno-default-config-bits"
        "-std=c99"
        "-gdwarf-3"
        "-mstack=compiled:auto:auto")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__16F18056__"
        PRIVATE "XPRJ_Bootloaded=Bootloaded")
endfunction()
function(camper_pic_Bootloaded_Bootloaded_toolchain_assemblePreprocess_rule target)
    set(options
        "-c"
        "${MP_EXTRA_AS_PRE}"
        "-mcpu=16F18056"
        "-x"
        "assembler-with-cpp"
        "-mdfp=${PACK_REPO_PATH}/Microchip/PIC16F1xxxx_DFP/1.31.465/xc8"
        "-fno-short-double"
        "-fno-short-float"
        "-mrom=default,-000-FFF"
        "-O2"
        "-fasmfile"
        "-maddrqual=require"
        "-mwarn=-3"
        "-msummary=-psect,-class,+mem,-hex,-file"
        "-mcodeoffset=0x1000"
        "-ginhx32"
        "-Wl,--data-init"
        "-mkeep-startup"
        "-mno-osccal"
        "-mno-resetbits"
        "-mno-save-resetbits"
        "-mno-download"
        "-mno-stackcall"
        "-mno-default-config-bits"
        "-std=c99"
        "-gdwarf-3"
        "-mstack=compiled:auto:auto")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__16F18056__"
        PRIVATE "XPRJ_Bootloaded=Bootloaded")
endfunction()
function(camper_pic_Bootloaded_Bootloaded_toolchain_compile_rule target)
    set(options
        "-c"
        "${MP_EXTRA_CC_PRE}"
        "-mcpu=16F18056"
        "-mdfp=${PACK_REPO_PATH}/Microchip/PIC16F1xxxx_DFP/1.31.465/xc8"
        "-fno-short-double"
        "-fno-short-float"
        "-mrom=default,-000-FFF"
        "-O2"
        "-fasmfile"
        "-maddrqual=require"
        "-mwarn=-3"
        "-msummary=-psect,-class,+mem,-hex,-file"
        "-mcodeoffset=0x1000"
        "-ginhx32"
        "-Wl,--data-init"
        "-mkeep-startup"
        "-mno-osccal"
        "-mno-resetbits"
        "-mno-save-resetbits"
        "-mno-download"
        "-mno-stackcall"
        "-mno-default-config-bits"
        "-std=c99"
        "-gdwarf-3"
        "-mstack=compiled:auto:auto")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__16F18056__"
        PRIVATE "XPRJ_Bootloaded=Bootloaded")
endfunction()
function(camper_pic_Bootloaded_link_rule target)
    set(options
        "-Wl,-Map=mem.map"
        "${MP_EXTRA_LD_PRE}"
        "-mcpu=16F18056"
        "-Wl,--defsym=__MPLAB_BUILD=1"
        "-mdfp=${PACK_REPO_PATH}/Microchip/PIC16F1xxxx_DFP/1.31.465/xc8"
        "-fno-short-double"
        "-fno-short-float"
        "-mrom=default,-000-FFF"
        "-O2"
        "-fasmfile"
        "-maddrqual=require"
        "-mwarn=-3"
        "-msummary=-psect,-class,+mem,-hex,-file"
        "-mcodeoffset=0x1000"
        "-ginhx32"
        "-Wl,--data-init"
        "-mkeep-startup"
        "-mno-osccal"
        "-mno-resetbits"
        "-mno-save-resetbits"
        "-mno-download"
        "-mno-stackcall"
        "-mno-default-config-bits"
        "-std=c99"
        "-gdwarf-3"
        "-mstack=compiled:auto:auto"
        "-Wl,--memorysummary,memoryfile.xml")
    list(REMOVE_ITEM options "")
    target_link_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target} PRIVATE "XPRJ_Bootloaded=Bootloaded")
endfunction()
