#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>

#define INSTRUCTION_CAPACITY 4
#define LABEL_CAPACITY 101

typedef struct {
    FILE *file;
    char *line;
    size_t line_size;
} Parser;

typedef enum {
    INSTRUCTION_TYPE_A = 0,
    INSTRUCTION_TYPE_C,
    INSTRUCTION_TYPE_L
} Instruction_Type;

typedef struct {
    Instruction_Type type;
    char dest[INSTRUCTION_CAPACITY];
    char comp[INSTRUCTION_CAPACITY];
    char jump[INSTRUCTION_CAPACITY];
    char label[LABEL_CAPACITY];
} Instruction;

Parser make_parser(const char *file_path);
bool parser_peek_line(Parser *p);
Instruction parser_parse_instruction(Parser *p);

#endif
