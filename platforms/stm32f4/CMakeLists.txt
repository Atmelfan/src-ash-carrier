
set(PLATFORM_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/systick.c
        ${CMAKE_CURRENT_SOURCE_DIR}/gpio.c
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c.c
        ${CMAKE_CURRENT_SOURCE_DIR}/serial.c
        ${CMAKE_CURRENT_SOURCE_DIR}/spi.c
        ${BOARD_SRC}
        PARENT_SCOPE)