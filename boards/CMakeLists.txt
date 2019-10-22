cmake_minimum_required(VERSION 3.7)

set(BOARD_SELECT "gpa_ash_carrier")
if (NOT BOARD_SELECT)
    message(WARNING "No board selected. Defaulting to ${BOARD_DEFAULT}!")
    set(BOARD_SELECT ${BOARD_DEFAULT})
endif ()

add_subdirectory(${BOARD_SELECT})

# Create library
set(BOARD_SOUR
        ${CMAKE_CURRENT_SOURCE_DIR}/board.h
        ${BOARD_SOURCE}
        )

add_library(boards ${BOARD_SOUR})

# Add to include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})