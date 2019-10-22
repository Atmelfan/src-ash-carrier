//
// Created by atmelfan on 2018-03-12.
//

#ifndef SRC_CARRIER_DTB_PARSER_H
#define SRC_CARRIER_DTB_PARSER_H


#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    FDT_BEGIN_NODE = 1,
    FDT_END_NODE = 2,
    FDT_PROP = 3,
    FDT_NOP = 4,
    FDT_END = 9
} fdt_token_type;

typedef struct __attribute__((__packed__)){
    uint64_t address;
    uint64_t size;
} fdt_reserve_entry;

typedef struct __attribute__((__packed__)){
    uint32_t token;
    union {
        /* FDT PROP */
        struct __attribute__((__packed__)){
            uint32_t len;
            uint32_t nameoff;
            union {
                char prop_str[0];
                uint32_t cells[0];
                uint8_t bytestr[0];
            };
        };

        /* FDT NODE */
        char name[0];
    };

} fdt_token;

typedef struct __attribute__((__packed__)) {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
} fdt_header_t;

/**
 * Converts a FDT's big endian u32 to little endian
 * @param t, pointer to FDT u32
 * @return converted u32
 */
uint32_t fdt_read_u32(void* t);

/**
 *
 * @param fdt
 * @return
 */
fdt_reserve_entry* fdt_get_reserved(fdt_header_t* fdt);

/**
 * Get pointer to token block
 * @param fdt, pointer to fdt header
 * @return start of tokens
 */
fdt_token* fdt_get_tokens(fdt_header_t* fdt);

/**
 * Get pointer to string block
 * @param fdt, pointer to fdt header
 * @return start of strings
 */
char* fdt_get_strings(fdt_header_t* fdt);

/**
 * Get name of property
 * @param fdt, pointer to fdt header
 * @param prop, property token
 * @return name of property or NULL if token is not a property
 */
char* fdt_prop_name(fdt_header_t* fdt, fdt_token* prop);

/**
 * Get length of property data
 * @param fdt, pointer to fdt header
 * @param prop, property token
 * @return length of property or 0 if token is not a property
 */
uint32_t fdt_prop_len(fdt_header_t* fdt, fdt_token* prop);

/**
 * Get token after this token
 * @param fdt, pointer to fdt header
 * @param token, token to check
 * @return pointer to next token or NULL if EOT
 */
fdt_token* fdt_token_next(fdt_header_t* fdt, fdt_token* token);

/**
 *
 * @param fdt
 * @param prop
 * @return
 */
fdt_token* fdt_node_end(fdt_header_t* fdt, fdt_token* prop);

/**
 * Get type of token
 * @param token, token to check
 * @return type of token
 */
uint32_t fdt_token_get_type(fdt_token* token);

/**
 *
 * @param fdt
 * @param t
 * @param name
 * @param recursive
 * @return
 */
fdt_token* fdt_node_get_prop(fdt_header_t* fdt, fdt_token* t, char* name, bool recursive);

/**
 * Find a node by its phandle
 * @param fdt, pointer to fdt header
 * @param t, token to search
 * @param p, phandle to find
 * @return pointer to node or NULL if not found
 */

fdt_token* fdt_find_phandle(fdt_header_t* fdt, uint32_t p);

/**
 * Find a node by its name inside current node
 * @param fdt, pointer to fdt header
 * @param token, token to search
 * @param name, name to find
 * @return pointer to node
 */
fdt_token* fdt_find_subnode(fdt_header_t* fdt, fdt_token* token, char* name);

/**
 * Find a node by its name
 * @param fdt, pointer to fdt header
 * @param name, name to find
 * @return pointer to node
 */
fdt_token* fdt_find_node(fdt_header_t* fdt, char* name);

/**
 * Find a u32 property and return it or default
 * @param fdt, pointer to fdt header
 * @param t, token to search within
 * @param name, name of property
 * @param def, default return value if node is not found
 * @return value of first cell or defaul if not found
 */
uint32_t fdt_node_get_u32(fdt_header_t* fdt, fdt_token* t, char* name, uint32_t def);

/**
 *
 * @param fdt
 * @param t
 * @param name
 * @param def
 * @return
 */
char* fdt_node_get_str(fdt_header_t* fdt, fdt_token* t, char* name, char* def);

char* fdt_trace(fdt_header_t* fdt, fdt_token* t, char* buf);

#endif //SRC_CARRIER_DTB_PARSER_H
