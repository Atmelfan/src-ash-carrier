cmake_minimum_required(VERSION 3.7)

# Compile scpi-parser
set(LIBSCPI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scpi-parser/libscpi)
set(SCPI_PARSER_SOURCES
        ${LIBSCPI_DIR}/src/error.c
        ${LIBSCPI_DIR}/src/fifo.c
        ${LIBSCPI_DIR}/src/ieee488.c
        ${LIBSCPI_DIR}/src/minimal.c
        ${LIBSCPI_DIR}/src/parser.c
        ${LIBSCPI_DIR}/src/units.c
        ${LIBSCPI_DIR}/src/utils.c
        ${LIBSCPI_DIR}/src/lexer.c
        ${LIBSCPI_DIR}/src/expression.c
        )
set(SCPI_PARSER_HEADERS
        ${LIBSCPI_DIR}/src/lexer_private.h
        ${LIBSCPI_DIR}/src/utils_private.h
        ${LIBSCPI_DIR}/src/fifo_private.h
        ${LIBSCPI_DIR}/src/parser_private.h
        ${LIBSCPI_DIR}/inc/scpi/scpi.h
        ${LIBSCPI_DIR}/inc/scpi/constants.h
        ${LIBSCPI_DIR}/inc/scpi/error.h
        ${LIBSCPI_DIR}/inc/scpi/ieee488.h
        ${LIBSCPI_DIR}/inc/scpi/minimal.h
        ${LIBSCPI_DIR}/inc/scpi/parser.h
        ${LIBSCPI_DIR}/inc/scpi/types.h
        ${LIBSCPI_DIR}/inc/scpi/units.h
        ${LIBSCPI_DIR}/inc/scpi/expression.h
        )

add_library(scpi-parser ${SCPI_PARSER_SOURCES} ${SCPI_PARSER_HEADERS})
target_include_directories(scpi-parser PUBLIC ${LIBSCPI_DIR}/inc)