//
// Created by atmelfan on 4/1/18.
//

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "scpi.h"

void scpi_init(scpi_context_t *context, const scpi_command_t *commands, void (*write)(char)) {
    context->push_out = write;
}

void scpi_puts(const scpi_context_t *context, const char *s) {
    assert(context->push_out);

    for (; *s; s++) {
        context->push_out(*s);
    }
}



enum scpi_cmd_typ {
    SCPI_NONE = 0,
    SCPI_SET,
    SCPI_GET,
    SCPI_NEXT
};

/**
 * Compare a string against a SCPI identifier
 * @param s
 * @param ident
 * @return
 */
enum scpi_cmd_typ scpi_strcmp(char* s, char* ident, char** next){
    *next = NULL;

    if(!s)
        return SCPI_NONE;

    for (; *ident; ++s, ++ident) {
        //printf("%c %c\n", *s, *ident);
        if((*s == '\0' || *s == ' ' || *s == '?' || *s == ':') && islower(*ident)){
            break;
        }else if(tolower(*s) != tolower(*ident)){
            return SCPI_NONE;
        }
    }

    switch(*s){
        case '?':
            return SCPI_GET;
        case ':':
            *next = s + 1;
            return SCPI_NEXT;
        default:
            break;
    }
    return SCPI_SET;
}

scpi_status_t scpi_execute_cmd(scpi_context_t* context, scpi_command_t* t, char* s){
    while(*s && isspace(*s)){
        s++;
    }

    for (int i = 0; t->sub[i].name; ++i) {
        char* next;
        enum scpi_cmd_typ x = scpi_strcmp(s, t->sub[i].name, &next);
        if(x == SCPI_SET){
            if(t->sub[i].set)
                return t->sub[i].set(context, s);
        }else if(x == SCPI_GET){
            if(t->sub[i].get)
                return t->sub[i].get(context, s);
        }else if(x == SCPI_NEXT){
            context->last = &t->sub[i];
            return scpi_execute_cmd(context, &t->sub[i], next);
        }
    }
    return SCPI_NOCOMMAND;
}

void scpi_execute(scpi_context_t* context, char* s, char* buf){
    char* end;
    char* token = strtok_r(s, ";", &end);
    context->last = &context->root;
    while(token){
        if(*token == ':'){
            context->last = &context->root;
            token++;
        }

        if(*token == '*'){
            scpi_execute_cmd(context, &context->root, token);
        }else{
            scpi_execute_cmd(context, context->last, token);
        }

        token = strtok_r(NULL, ";", &end);
    }

}

uint8_t scpi_get_stb(scpi_context_t *context) {
    uint8_t tmp = (context->_stb & context->sre) & (uint8_t)~(1 << 6);
    tmp |= (tmp ? (1 << 6) : 0);
    tmp |= (context->esr & context->ese) ? (1 << 5) : 0;
    return (uint8_t) (tmp | (tmp ? 0x40 : 0x00));
}

bool scpi_push_error(scpi_context_t *context, scpi_error_e err) {

    /* Advance end pointer and wrap around if necessary */
    uint8_t index = context->end;

    /*  */
    if(index != context->begin){
        context->errors[index] = err;
        context->end++;
        return true;
    }
    return false;
}
