#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    FILE *file;
    char *line;
    size_t line_size;
} Parser;

typedef enum {
    OP_TYPE_UNINIT = 0,

    OP_TYPE_PUSH,
    OP_TYPE_POP,

    OP_TYPE_ADD,
    OP_TYPE_SUB,
    OP_TYPE_NEG,
    OP_TYPE_EQ,
    OP_TYPE_GT,
    OP_TYPE_LT,
    OP_TYPE_AND,
    OP_TYPE_OR,
    OP_TYPE_NOT
} Op_Type;

typedef enum {
    MEM_SEG_TYPE_UNINIT = 0,

    MEM_SEG_TYPE_LCL,
    MEM_SEG_TYPE_ARG,
    MEM_SEG_TYPE_THIS,
    MEM_SEG_TYPE_THAT,

    MEM_SEG_TYPE_CONST,
    MEM_SEG_TYPE_STATIC,
    MEM_SEG_TYPE_TEMP,
    MEM_SEG_TYPE_POINTER
} Mem_Seg_Type;

typedef struct {
    Op_Type op_type;
    Mem_Seg_Type mem_seg_type;
    size_t mem_addr;
} Instruction;

Parser make_parser(const char *file_path);
bool parser_peek_line(Parser *p);
Instruction parser_parse_instruction(Parser *p);

#endif
