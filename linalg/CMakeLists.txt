cmake_minimum_required(VERSION 3.7)

set(LINALG_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/linalg.h
        ${CMAKE_CURRENT_SOURCE_DIR}/linalg_vec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/linalg_mat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/linalg_vecmat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/linalg_util.c
        ${CMAKE_CURRENT_SOURCE_DIR}/linalg_util.h
        )
add_library(linalg ${LINALG_SRC})
target_link_libraries(linalg m)


