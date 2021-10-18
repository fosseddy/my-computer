#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_INST_SIZE 4
#define MAX_LABEL_SIZE 101

typedef struct {
    FILE *file;
    char *line;
    size_t line_size;
} Parser;

typedef enum {
    INST_TYPE_A = 0,
    INST_TYPE_C,
    INST_TYPE_L
} Inst_Type;

typedef struct {
    Inst_Type type;
    char dest[MAX_INST_SIZE];
    char comp[MAX_INST_SIZE];
    char jump[MAX_INST_SIZE];
    char label[MAX_LABEL_SIZE];
} Inst;

Parser make_parser(const char *file_path);
bool parser_peek_line(Parser *p);
Inst parser_parse_inst(Parser *p);

#endif
