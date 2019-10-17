//
// Created by atmelfan on 4/1/18.
//

#ifndef SRC_CARRIER_SCPI_H
#define SRC_CARRIER_SCPI_H


#include <stdint.h>

#define SCPI_IDN_STR "GPA Robotics, GPA-AIX-323b, 1.0.0"

typedef struct scpi_command_t scpi_command_t;
typedef struct scpi_context_t scpi_context_t;

typedef enum {
    SCPI_SUCCESS = 0,
    SCPI_NOCOMMAND
} scpi_status_t;


struct scpi_command_t{

    /*Name of command*/
    char* name;

    /**
     *
     * @param context
     * @param args
     * @return
     */
    scpi_status_t (*set)(const scpi_context_t* context, char* args);

    /**
     *
     * @param context
     * @param args
     * @return
     */
    scpi_status_t (*get)(const scpi_context_t* context, char* args);

    uint16_t num_sub;
    scpi_command_t* sub;
};

struct scpi_context_t {
    /* Root command */
    uint16_t num_commands;
    scpi_command_t root;
    /* Last command */
    scpi_command_t* last;
};

void scpi_init(scpi_context_t* context);

void scpi_register(scpi_context_t* context, scpi_command_t* t);

void scpi_execute(scpi_context_t* context, char* s, char* buf);

#endif //SRC_CARRIER_SCPI_H
