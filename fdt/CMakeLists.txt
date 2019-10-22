cmake_minimum_required(VERSION 3.7)

# Create library
set(FDT_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/fdt_parser.c
        ${CMAKE_CURRENT_SOURCE_DIR}/fdt_parser.h
		${CMAKE_CURRENT_SOURCE_DIR}/fdt_utils.h
        )
add_library(fdt ${FDT_SOURCE})

# Add to include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})





