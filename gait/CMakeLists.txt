cmake_minimum_required(VERSION 3.7)

# Create library
set(GAIT_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/gait.c
        ${CMAKE_CURRENT_SOURCE_DIR}/gait.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gait_dts_const.h
        )
set(GAIT_SOURCE ${GAIT_SOURCE} PARENT_SCOPE)
add_library(gait ${GAIT_SOURCE})

