# Created by Gustav Palmqvist, gpa-robotics.com, 2018

message(INFO " * * * Device information * * *")
message(INFO " Device:\t${DEVICE}")


# Extract device information from name
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${LIBOCM3}/scripts/genlink.py ${LIBOCM3}/ld/devices.data ${DEVICE} FAMILY
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE OCM3_FAMILY
        )
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${LIBOCM3}/scripts/genlink.py ${LIBOCM3}/ld/devices.data ${DEVICE} SUBFAMILY
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE OCM3_SUBFAMILY
        )
message(INFO "\tfamily:\t${OCM3_FAMILY} (${OCM3_SUBFAMILY})")

execute_process(COMMAND ${PYTHON_EXECUTABLE} ${LIBOCM3}/scripts/genlink.py ${LIBOCM3}/ld/devices.data ${DEVICE} CPU
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE OCM3_CPU
        )
message(INFO "\tcpu:\t${OCM3_CPU}")

execute_process(COMMAND ${PYTHON_EXECUTABLE} ${LIBOCM3}/scripts/genlink.py ${LIBOCM3}/ld/devices.data ${DEVICE} FPU
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE OCM3_FPU
        )
message(INFO "\tfpu:\t${OCM3_FPU}")

execute_process(COMMAND ${LIBOCM3}/scripts/genlink.py ${LIBOCM3}/ld/devices.data ${DEVICE} CPPFLAGS
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE OCM3_CPPFLAGS
        )
message(INFO "\tflags:\t${OCM3_CPPFLAGS}")

execute_process(COMMAND ${PYTHON_EXECUTABLE} ${LIBOCM3}/scripts/genlink.py ${LIBOCM3}/ld/devices.data ${DEVICE} DEFS
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE OCM3_DEFS
        )
message(INFO "\tdefs:\t${OCM3_DEFS}")
#

# Create arch flags
set(OCM3_ARCH_FLAGS "-mcpu=${OCM3_CPU}")

if (OCM3_CPU STREQUAL "cortex-m0"
        OR OCM3_CPU STREQUAL "cortex-m0plus"
        OR OCM3_CPU STREQUAL "cortex-m3"
        OR OCM3_CPU STREQUAL "cortex-m4"
        OR OCM3_CPU STREQUAL "cortex-m7")
    set(OCM3_ARCH_FLAGS "${OCM3_ARCH_FLAGS} -mthumb")
endif ()

if (OCM3_FPU STREQUAL "soft")
    set(OCM3_ARCH_FLAGS "${OCM3_ARCH_FLAGS} -msoft-float")
elseif (OCM3_FPU STREQUAL "hard-fpv4-sp-d16")
    set(OCM3_ARCH_FLAGS "${OCM3_ARCH_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16")
elseif (OCM3_FPU STREQUAL "hard-fpv5-sp-d16")
    set(OCM3_ARCH_FLAGS "${OCM3_ARCH_FLAGS} -mfloat-abi=hard -mfpu=fpv5-sp-d16")
endif ()

# Hack because CMake is a piece of shit
set(args "${OCM3_ARCH_FLAGS} ${OCM3_DEFS} -P -E ${LIBOCM3}/ld/linker.ld.S > ${CMAKE_BINARY_DIR}/generated.ld")
separate_arguments(args)

# Generate a linker script
add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/generated.ld
        COMMAND "${CMAKE_C_COMPILER}" ${args}
        MAIN_DEPENDENCY ${LIBOCM3}/ld/linker.ld.S
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating linker script"
        VERBATIM
)

# Custom target for linker script
add_custom_target(libopencm3
        DEPENDS ${CMAKE_BINARY_DIR}/generated.ld
)

# Add all libraries
set(OCM3_LIBS)
if (EXISTS ${LIBOCM3}/lib/libopencm3_${OCM3_FAMILY}.a )
    set(OCM3_LIBS ${OCM3_LIBS} opencm3_${OCM3_FAMILY})
endif ()
if (EXISTS ${LIBOCM3}/lib/libopencm3_${OCM3_SUBFAMILY}.a )
    set(OCM3_LIBS ${OCM3_LIBS} opencm3_${OCM3_SUBFAMILY})
endif ()

message(INFO " libopencm3 libs: ${OCM3_LIBS}")

# Add lib directory
link_directories(${LIBOCM3}/lib)

# Add include directory
include_directories(${LIBOCM3}/include)






