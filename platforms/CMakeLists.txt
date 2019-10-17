
set(PLATFORM_DEFAULT "stm32f4")
if (NOT PLATFORM_SELECT)
    message(WARNING "No platform selected. Defaulting to ${PLATFORM_DEFAULT}!")
    set(PLATFORM_SELECT ${PLATFORM_DEFAULT})
endif ()

set(BOARD_DEFAULT "gpa_tx2_carrier")
if (NOT BOARD_SELECT)
    message(WARNING "No board selected. Defaulting to ${BOARD_DEFAULT}!")
    set(BOARD_SELECT ${BOARD_DEFAULT})
endif ()
message(INFO " *** Platform: ${PLATFORM_SELECT} Board: ${BOARD_SELECT} ***")

add_subdirectory(${PLATFORM_SELECT})
add_subdirectory(drivers)

include(../fdt/fdt.cmake)

add_fdt_target(fdt_board ${PLATFORM_SELECT}/${BOARD_SELECT}/board.dts)


set(PLATFORM_SUPPORT_SRC dev.c dev.h gpio.h i2c.h serial.h spi.h board.c board.h ${PLATFORM_SRC} ${DRIVER_SOURCE} log.c log.h i2c.c pwm.c pwm.h gpio.c systick.h)

add_library(platforms ${PLATFORM_SUPPORT_SRC} )
target_link_libraries(platforms opencm3_stm32f4)
target_link_libraries(platforms fdt)
target_link_libraries(platforms fdt_board)