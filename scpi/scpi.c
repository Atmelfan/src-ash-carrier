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
    SCPI_NEXT,
    
    SCPI_HEADER,
    SCPI_ARGS,
    SCPI_NEXT
};

scpi_error_e scpitok_header(char* s, char** token, char** end) {
    if(s)
        *end = s;
    
    /* Empty string, do not continue */
    if(**end == NULL || *end == NULL)
        return SCPI_NOERROR;
    
    char* str = *end;
    *token = str;
    
    for(; *str; str++){
        /* Start of args */
        if(isspace(*str)){
            *str = '\0';
            return _SCPI_ARGS;
        }
        /* Start of next command */
        else if(*str == ';'){
            *str = '\0';
            return _SCPI_NEXT;
        }
        /* Header continues */
        else if(*str == ':'){
            *str = '\0';
            return _SCPI_HEADER;
        }
        /* Check for unallowed characters */
        if(!isalnum(*str))
            return SCPI_SYNTAX_ERROR;
    }
    
    /* End of string */
    return _SCPI_HEADER;
}

scpi_error_e scpitok_arg(char** token, char** end) {
    /* Empty string, do not continue */
    if(**end == NULL || *end == NULL)
        return SCPI_NOERROR;
    
    char* str = *end;
    *token = str;
    
    bool quoted = false;
    
    for(; *str; str++){
        /* Check for a string argument */
        if(*str == '"' || *str == '\''){
            //TODO, check for escape char
            //TODO, check previous quote char
            quoted = !quoted;
        }        
        /* Check for arg delimiter */
        else if(*str == ',' && !quoted){
            *str = '\0';
            return _SCPI_ARGS;
        }
        /* Start of next command */
        else if(*str == ';' && !quoted){
            *str = '\0';
            return _SCPI_NEXT;
        }
        /* Check for invalid (non-ascii) characters */
        if(iscntrl(*str) || *str > 127)
            return SCPI_INVALID_CHAR;
    }
    
    /* End of string */
    return _SCPI_ARGS;
}

scpi_error_e scpitok_arg_bool(char** token, char** end, bool* arg){
    /* Parse argument */
    scpi_error_e r = scpitok_arg(token, end);
    if(r != _SCPI_ARGS)
        return r;
    
    /* Parse boolean */
    if(!scpi_cmp(*token, "ON") || !strcmp(*token, "1")){
        *arg = true;    
    }else if(!scpi_cmp(*token, "OFF") || !strcmp(*token, "0")){
        *arg = false;
    }else{
        return SCPI_SYNTAX_ERROR;
    }
    return r;
}

scpi_error_e scpitok_arg_numeric(char** token, char** end, int32_t* arg){
     /* Parse argument */
    scpi_error_e r = scpitok_arg(token, end);
    if(r != _SCPI_ARGS)
        return r;
    
    //TODO, make custom numeric parser
    scpi_error_e i = scpi_atoi(*token, arg);
    
    return i ? i : r;
}

scpi_error_e scpitok_arg_enumeric(char** token, char** end, int32_t* arg, int32_t max, int32_t min, int32_t def){
     /* Parse argument */
    scpi_error_e r = scpitok_arg(token, end);
    if(r != _SCPI_ARGS)
        return r;
    
    if(!scpi_cmp(*token, "DEFault")){
        *arg = def;
    }else if(!scpi_cmp(*token, "UP")){
        if(*arg < max)
            *arg++;
    }else if(!scpi_cmp(*token, "DOWN")){
        if(*arg > min)
            *arg--;
    }else if(scpi_cmp(*token, "MINimum")){
        *arg = min;
    }else if(scpi_cmp(*token, "MAXimum")){
        *arg = max;
    }else{
        scpi_error_e i = scpi_atoi(*token, arg);
        return i ? i : r;
    }
    return r;
}


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
