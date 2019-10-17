//
// Created by atmelfan on 2018-03-12.
//

#include "dtb_parser.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/**
 * For whatever reason FDT are encoded using big endian format
 * @param t
 * @return
 */
uint32_t fdt_read_u32(void* t){
    return (((uint8_t*)t)[0] << 24) |
           (((uint8_t*)t)[1] << 16) |
           (((uint8_t*)t)[2] <<  8) |
           (((uint8_t*)t)[3] <<  0);
}


fdt_reserve_entry* fdt_get_reserved(fdt_header_t* fdt){
    return (fdt_reserve_entry*)((void*)fdt + fdt_read_u32(&fdt->off_mem_rsvmap));
}

/**
 *
 * @param fdt
 * @return
 */
fdt_token* fdt_get_tokens(fdt_header_t* fdt){
    return (fdt_token*)((void*)fdt + fdt_read_u32(&fdt->off_dt_struct));
}

/**
 *
 * @param fdt
 * @return
 */
char* fdt_get_strings(fdt_header_t* fdt){
    return (char*)((void*)fdt + fdt_read_u32(&fdt->off_dt_strings));
}

char* fdt_prop_name(fdt_header_t* fdt, fdt_token* prop){
    if(fdt_read_u32(&prop->token) != FDT_PROP)
        return NULL;

    return fdt_get_strings(fdt) + fdt_read_u32(&prop->nameoff);
}

uint32_t fdt_prop_len(fdt_header_t* fdt, fdt_token* prop){
    (void)fdt;
    if(fdt_read_u32(&prop->token) != FDT_PROP)
        return 0;
    return fdt_read_u32(&prop->len);
}

fdt_token* fdt_token_next(fdt_header_t* fdt, fdt_token* token){
    /*Return root if token is null*/
    if(token == NULL){
        //token = fdt_get_tokens(fdt);
        return NULL;
    }

    /*Skip ahead by length of current token*/
    uint32_t len = 0;
    switch(fdt_read_u32(&token->token)){
        case FDT_PROP: /*Property contains 2 uint32 + len bytes*/
            len = 2*sizeof(uint32_t) + 4*((fdt_read_u32(&token->len) + 3)/4);
            break;

        case FDT_BEGIN_NODE:
            len = strlen(token->name) == 0 ? sizeof(uint32_t) : 4*((strlen(token->name) + 4)/4);
            break;

        case FDT_END_NODE:
            break;

        case FDT_NOP:
            break;

        case FDT_END:
            return NULL;

        default:
            printf("#FDT ERROR: Found unknown token %04X", (unsigned int)fdt_read_u32(&token->token));
            return NULL;
    }
    return (fdt_token*)((void*)token + sizeof(uint32_t)  + len);
}

fdt_token* fdt_node_end(fdt_header_t* fdt, fdt_token* prop){
    int level = 0;
    while(prop){
        prop = fdt_token_next(fdt, prop);
        switch(fdt_read_u32(&prop->token)){
            case FDT_BEGIN_NODE:
                level++;
                //printf("%s,%d\n", prop->name, level);
                break;

            case FDT_END_NODE:
                if(level == 0)
                    return prop;
                level--;
                break;

            case FDT_END:
                return NULL;

            default:
                break;
        }
    }

    return prop;
}

fdt_token* fdt_find_phandle(fdt_header_t* fdt, uint32_t p){

    /* phandle 0 is no*/
    if(p == 0)
        return NULL;

    fdt_token* t = fdt_get_tokens(fdt);
    fdt_token* node = NULL;
    while(t){
        switch(fdt_token_get_type(t)){
            case FDT_BEGIN_NODE:
                node = t;
                break;

            case FDT_PROP:

                if(!strcmp(fdt_prop_name(fdt, t), "phandle")){
                    //printf("Found phandle: %d\n", fdt_read_u32(&t->cells[0]));
                    if(fdt_read_u32(&t->cells[0]) == p){
                        return node;
                    }
                }
                break;
            default:
                break;
        }
        t = fdt_token_next(fdt, t);
    }
    return NULL;
}

fdt_token* fdt_find_subnode(fdt_header_t* fdt, fdt_token* token, char* name){
    int level = 0;
    token = fdt_token_next(fdt, token);
    while(token){
        switch(fdt_token_get_type(token)){
            case FDT_BEGIN_NODE:
                if(!strcmp(name, token->name) && level == 0)
                    return token;
                level++;
                break;
            case FDT_END_NODE:
                if(level == 0){
                    return NULL;
                }
                level--;
                break;

            default:
                break;
        }
        token = fdt_token_next(fdt, token);
    }
    return NULL;
}


fdt_token* fdt_find_node(fdt_header_t* fdt, char* name){
    fdt_token* t = fdt_get_tokens(fdt);
    while(t){
        switch(fdt_token_get_type(t)){
            case FDT_BEGIN_NODE:
                if(!strcmp(name, t->name))
                    return t;
                break;

            default:
                break;
        }
        t = fdt_token_next(fdt, t);
    }
    return NULL;
}

fdt_token* fdt_node_get_prop(fdt_header_t* fdt, fdt_token* t, char* name, bool recursive){
    int level = 0;
    t = fdt_token_next(fdt, t);
    while(t){
        switch(fdt_token_get_type(t)){
            case FDT_PROP:
                //printf(" -> %d, %s = %s\n", level, fdt_prop_name(fdt, t), name);
                if(!strcmp(name, fdt_prop_name(fdt, t)) && (level == 0 || recursive))
                    return t;
                break;

            case FDT_BEGIN_NODE:
                level++;
                break;

            case FDT_END_NODE:
                if(level > 0)
                    level--;
                else
                    return NULL;
                break;

            default:
                break;
        }
        t = fdt_token_next(fdt, t);
    }
    return NULL;
}

uint32_t fdt_node_get_u32(fdt_header_t* fdt, fdt_token* t, char* name, uint32_t def){
    fdt_token* f = fdt_node_get_prop(fdt, t, name, false);
    if(f && fdt_read_u32(&f->len))
        return fdt_read_u32(&f->cells[0]);
    //else
    //    printf("Failed to find %s\n", name);
    return def;
}



char* fdt_node_get_str(fdt_header_t* fdt, fdt_token* t, char* name, char* def){
    fdt_token* f = fdt_node_get_prop(fdt, t, name, false);
    if(f && fdt_read_u32(&f->len))
        return f->prop_str;
    else
        return def;
}

uint32_t fdt_token_get_type(fdt_token* prop){
    return fdt_read_u32(&prop->token);
}

char* fdt_trace(fdt_header_t* fdt, fdt_token* t, char* buf){
    fdt_token* n = fdt_get_tokens(fdt);
    fdt_token* trace_stack[16];
    int i = 0;

    while(n != t){
        switch(fdt_token_get_type(n)){
            case FDT_BEGIN_NODE:
                trace_stack[i] = n;
                i++;
                break;
            case FDT_END_NODE:
                i--;
                break;
            case FDT_END:
                //printf("END\n");
                return NULL;

            default:
                break;
        }

        if(i >= 16)
            return NULL;

        n = fdt_token_next(fdt, n);
        //printf("t=%p,n=%p,i=%d\n", t,n,i);
    }
    char* s = buf;
    for(int j = 0; j < i; ++j){
        //printf("//%s", trace_stack[j]->name);
        char* nodename = trace_stack[j]->name;
        while(*nodename){
            *buf = *nodename;
            buf++;
            nodename++;
        }
        *buf = '/';
        buf++;
    }


    char* nodename = fdt_token_get_type(n) == FDT_PROP ? fdt_prop_name(fdt, n) : n->name;
    while(*nodename){
        *buf = *nodename;
        buf++;
        nodename++;
    }
    *buf = '\0';
    return s;
}


