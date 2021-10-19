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
    OP_TYPE_PUSH = 0,
    OP_TYPE_ADD,
} Op_Type;

typedef enum {
    MEM_SEG_TYPE_CONST = 0
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
