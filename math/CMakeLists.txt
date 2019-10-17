set(SOURCE_FILES
            ${SOURCE_FILES}
            ${CMAKE_CURRENT_SOURCE_DIR}/linalg.h
            ${CMAKE_CURRENT_SOURCE_DIR}/linalg_vec.c
            ${CMAKE_CURRENT_SOURCE_DIR}/linalg_mat.c
            ${CMAKE_CURRENT_SOURCE_DIR}/linalg_vecmat.c
            ${CMAKE_CURRENT_SOURCE_DIR}/linalg_util.c
            ${CMAKE_CURRENT_SOURCE_DIR}/linalg_util.h
            PARENT_SCOPE
        )

set(LINALG_SRC
        linalg.h
        linalg_vec.c
        linalg_mat.c
        linalg_vecmat.c
        linalg_util.c
        linalg_util.h
        )
add_library(linalg ${LINALG_SRC})


