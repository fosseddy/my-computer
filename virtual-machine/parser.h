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
    OP_KIND_PUSH,
    OP_KIND_POP,

    OP_KIND_LABEL,
    OP_KIND_GOTO,
    OP_KIND_IF,

    OP_KIND_FUNC,
    OP_KIND_CALL,
    OP_KIND_RET,

    OP_KIND_ADD,
    OP_KIND_SUB,
    OP_KIND_NEG,
    OP_KIND_EQ,
    OP_KIND_GT,
    OP_KIND_LT,
    OP_KIND_AND,
    OP_KIND_OR,
    OP_KIND_NOT
} Op_Kind;

typedef enum {
    MEM_SEG_KIND_LCL,
    MEM_SEG_KIND_ARG,
    MEM_SEG_KIND_THIS,
    MEM_SEG_KIND_THAT,

    MEM_SEG_KIND_CONST,
    MEM_SEG_KIND_STATIC,
    MEM_SEG_KIND_TEMP,
    MEM_SEG_KIND_POINTER
} Mem_Seg_Kind;

typedef struct {
    Op_Kind op_kind;

    Mem_Seg_Kind mem_seg_kind;
    size_t mem_offset;

    char *label;

    char *func_name;
    size_t func_args_num;
} Instruction;


Parser make_parser(const char *file_path);
bool parser_peek_line(Parser *p);
Instruction parser_parse_instruction(Parser *p);

#endif
