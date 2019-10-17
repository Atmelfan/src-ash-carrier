set(DTC_COMPILER dtc)
set(DTC_FLAGS -I dts -O dtb )
set(DTC_PREPROCESSOR ${CMAKE_C_COMPILER})
set(DTC_PREPROCESS_FLAGS -E -nostdinc -undef -D__DTS__  -I${CMAKE_CURRENT_SOURCE_DIR} -x assembler-with-cpp)
function(add_fdt_target target)
    set(sources)
    set(psources)
    set(txt ".global FDT_NAME"
            ".section .rodata"
            "FDT_NAME:"
            ".incbin FDT_FILE"
            "1:"
            )
    string (REPLACE ";" "\\\\n" txt "${txt}")
    foreach(f ${ARGN})
        get_filename_component(_basename ${f} NAME_WE)
        get_filename_component(_subdir ${f} DIRECTORY)
        add_custom_command(
                OUTPUT dtbs/${f}.S
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${f}
                COMMAND ${CMAKE_COMMAND} -E make_directory dtbs/${_subdir}
                COMMAND "${DTC_PREPROCESSOR}" ${DTC_PREPROCESS_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR} -o- ${CMAKE_CURRENT_SOURCE_DIR}/${f} | "${DTC_COMPILER}" ${DTC_FLAGS} -o dtbs/${f}.dtb
                COMMAND echo ${txt} | ${CMAKE_C_COMPILER} -E -o "dtbs/${f}.S" -DFDT_FILE=\\"${CMAKE_CURRENT_BINARY_DIR}/dtbs/${f}.dtb\\" -DFDT_NAME=${_basename} -
        )

        list(APPEND sources ${CMAKE_CURRENT_BINARY_DIR}/dtbs/${f}.S)
        list(APPEND psources ${f}.S)
    endforeach()

    #add_custom_target(${target} ALL
    #        DEPENDS ${sources})
    add_library(${target} ${sources})
    #get_filename_component(psources ${sources} NAME_WE)
    message(INFO " DTS @ ${CMAKE_CURRENT_SOURCE_DIR} ")
    set(FDT_BLOB_SOURCE ${sources} PARENT_SCOPE)
endfunction()