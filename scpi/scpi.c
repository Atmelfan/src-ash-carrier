//
// Created by atmelfan on 4/1/18.
//

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "scpi.h"

void scpi_init(scpi_context_t* context){
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

    for (int i = 0; i < t->num_sub; ++i) {
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
