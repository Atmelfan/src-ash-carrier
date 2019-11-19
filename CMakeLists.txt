cmake_minimum_required(VERSION 3.7)
project(src-ash-carrier C ASM)
include(ExternalProject)


set(CMAKE_C_STANDARD 11)

#Parameters
set(OPT -Os)
set(ASM_OPTIONS "-x assembler-with-cpp")
set(ENABLE_SEMIHOSTING OFF)

### Compiler ###
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")

### LIBOPENCM3 ###
set(LIBOCM3 ${CMAKE_CURRENT_SOURCE_DIR}/libopencm3)
set(DEVICE stm32f415rgt6)
include(libopencm3.cmake)

#flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OCM3_ARCH_FLAGS} ${OPT} -Wall -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MMD -MP -Wno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OCM3_DEFS}")
SET(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} ${ASM_OPTIONS}" )

#linker
set(LIBS c m nosys)

if (ENABLE_SEMIHOSTING)
    message(WARNING " Semihosting is ENABLED! Will not run without debugger!")
    set(LDFLAGS "${OCM3_DEFS} -specs=nano.specs -specs=rdimon.specs  -nostartfiles -T ${CMAKE_BINARY_DIR}/generated.ld -Wl,-Map=symbols.map,--cref -Wl,--gc-sections -Wl,--print-memory-usage")
    add_definitions(-DENABLE_SEMIHOSTING)
else()
    set(LDFLAGS "${OCM3_DEFS} -specs=nano.specs -specs=nosys.specs  -nostartfiles -T ${CMAKE_BINARY_DIR}/generated.ld -Wl,-Map=symbols.map,--cref -Wl,--gc-sections -Wl,--print-memory-usage")
endif ()


#add linker flags
set( CMAKE_CXX_FLAGS  "${LDFLAGS}" )
set( CMAKE_EXE_LINKER_FLAGS  "${LDFLAGS}" )

add_definitions(-DF_CPU=16000000UL)

### Libs ###
# FDT
add_subdirectory(fdt)
include(fdt/fdt.cmake)
add_fdt_target(fdt_blobs octapod.dts)

# Linalg
add_subdirectory(linalg)

# Gait
add_subdirectory(gait)

# SCPI
add_subdirectory(scpi)

### Project ###
set(SOURCE_FILES main.c leg.c leg.h scpi/scpi.c scpi/scpi.h fdt_mathutil.c fdt_mathutil.h body.c body.h)
add_executable(src-ash-carrier ${SOURCE_FILES} ${FPGA_SOURCE} ${RADIO_SOURCE})
add_dependencies(src-ash-carrier libopencm3)
target_link_libraries(src-ash-carrier ${LIBS} ${OCM3_LIBS})
target_link_libraries(src-ash-carrier fdt fdt_blobs)
target_link_libraries(src-ash-carrier linalg)
target_link_libraries(src-ash-carrier gait)
target_link_libraries(src-ash-carrier scpi-parser)
