//
// Created by atmelfan on 3/29/18.
//

#include <stdio.h>
#include <stdarg.h>
#include "log.h"

log_t default_log = LOG_INIT;

void log_push(log_t *log, const char *level) {

    if(log->sp >= MAX_LEVELS)
        return;
    /*Add string to stack*/
    log->stack[log->sp] = level;
    log->sp++;
}

void log_pop(log_t *log) {
    if(log->sp == 0)
        return;

    log->sp--;
}

void log_printf(log_t *log, log_level lvl, char *msg, ...) {
    va_list args;
    va_start(args, msg);

    /*Print level*/
    switch(lvl){
        case LOG_DEBUG:
            printf("[DBG] ");
            break;
        case LOG_INFO:
            printf("[INF] ");
            break;
        case LOG_WARNING:
            printf("[WRN] ");
            break;
        case LOG_ERROR:
            printf("[ERR] ");
            break;
        case LOG_CRITICAL:
            printf("[CRT] ");
            break;
        default:
            printf("[---] ");
            break;
    }

    /*Print hierarchy*/
    for (int i = 0; i < log->sp; ++i) {
        if(i == log->sp-1)
            printf("%s ~ ", log->stack[i]);
        else
            printf("%s:", log->stack[i]);

    }

    /*Print message*/
    vprintf(msg, args);

    va_end(args);
}
