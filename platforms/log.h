//
// Created by atmelfan on 3/29/18.
//

#ifndef SRC_CARRIER_LOG_H
#define SRC_CARRIER_LOG_H

#include <stdint.h>

#define MAX_LEVELS 8

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL
} log_level;

typedef struct {
    const char* stack[MAX_LEVELS];
    uint8_t sp;
} log_t;

#define LOG_INIT {.sp = 0}

extern log_t default_log;

/**
 * Prints a message to log
 * @param log
 * @param lvl
 * @param msg
 * @param ...
 */
void log_printf(log_t* log, log_level lvl, char* msg, ...);
#define logd_printf(lvl, msg, ...) log_printf(&default_log, lvl, msg, __VA_ARGS__)
#define logd_printfs(lvl, msg) log_printf(&default_log, lvl, msg)

void log_push(log_t* log, const char* level);
#define logd_push(l) log_push(&default_log, l)

void log_pop(log_t* log);
#define logd_pop() log_pop(&default_log)

#endif //SRC_CARRIER_LOG_H
