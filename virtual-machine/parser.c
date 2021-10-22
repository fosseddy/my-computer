#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "parser.h"

static void trim_left(char *s);
static void trim_right(char *s);

Parser make_parser(const char *file_path)
{
    FILE *f = fopen(file_path, "r");
    assert(f != NULL);

    return (Parser) {
        .file = f,
        .line_size = 0,
        .line = NULL
    };
}

bool parser_peek_line(Parser *p)
{
    for (;;) {
        ssize_t read = getline(&p->line, &p->line_size, p->file);

        if (read == -1) {
            fclose(p->file);
            free(p->line);
            return false;
        }

        trim_left(p->line);

        if (strstr(p->line, "//") != NULL) {
            // whole line is comment
            if (strncmp(p->line, "//", 2) == 0) continue;

            // line has inline comment
            char line_copy[strlen(p->line)];
            strcpy(line_copy, p->line);
            char *line_without_comment = strtok(line_copy, "//");
            strcpy(p->line, line_without_comment);
        }

        trim_right(p->line);

        if (strlen(p->line) == 0) continue;

        return true;
    }
}

Instruction parser_parse_instruction(Parser *p)
{
    Instruction inst = {0};

    char *op= strtok(p->line, " ");
    assert(op != NULL);

    if (strcmp(op, "push") == 0) {
        inst.op_kind = OP_KIND_PUSH;
    } else if (strcmp(op, "pop") == 0) {
        inst.op_kind = OP_KIND_POP;
    } else if (strcmp(op, "add") == 0) {
        inst.op_kind = OP_KIND_ADD;
    } else if (strcmp(op, "sub") == 0) {
        inst.op_kind = OP_KIND_SUB;
    } else if (strcmp(op, "eq") == 0) {
        inst.op_kind = OP_KIND_EQ;
    } else if (strcmp(op, "lt") == 0) {
        inst.op_kind = OP_KIND_LT;
    } else if (strcmp(op, "gt") == 0) {
        inst.op_kind = OP_KIND_GT;
    } else if (strcmp(op, "and") == 0) {
        inst.op_kind = OP_KIND_AND;
    } else if (strcmp(op, "or") == 0) {
        inst.op_kind = OP_KIND_OR;
    } else if (strcmp(op, "not") == 0) {
        inst.op_kind = OP_KIND_NOT;
    } else if (strcmp(op, "neg") == 0) {
        inst.op_kind = OP_KIND_NEG;
    } else if (strcmp(op, "label") == 0) {
        inst.op_kind = OP_KIND_LABEL;
    } else if (strcmp(op, "goto") == 0) {
        inst.op_kind = OP_KIND_GOTO;
    } else if (strcmp(op, "if-goto") == 0) {
        inst.op_kind = OP_KIND_IF;
    } else if (strcmp(op, "function") == 0) {
        inst.op_kind = OP_KIND_FUNC;
    } else if (strcmp(op, "call") == 0) {
        inst.op_kind = OP_KIND_CALL;
    } else if (strcmp(op, "return") == 0) {
        inst.op_kind = OP_KIND_RET;
    } else {
        assert(0 && "Unreachable");
    }

    char *arg_1 = strtok(NULL, " ");
    if (arg_1 == NULL) return inst;

    char *arg_2 = strtok(NULL, " ");

    switch (inst.op_kind) {
        case OP_KIND_PUSH:
        case OP_KIND_POP: {
            if (strcmp(arg_1, "local") == 0) {
                inst.mem_seg_kind = MEM_SEG_KIND_LCL;
            } else if (strcmp(arg_1, "argument") == 0) {
                inst.mem_seg_kind = MEM_SEG_KIND_ARG;
            } else if (strcmp(arg_1, "this") == 0) {
                inst.mem_seg_kind = MEM_SEG_KIND_THIS;
            } else if (strcmp(arg_1, "that") == 0) {
                inst.mem_seg_kind = MEM_SEG_KIND_THAT;
            } else if (strcmp(arg_1, "constant") == 0) {
                inst.mem_seg_kind = MEM_SEG_KIND_CONST;
            } else if (strcmp(arg_1, "static") == 0) {
                inst.mem_seg_kind = MEM_SEG_KIND_STATIC;
            } else if (strcmp(arg_1, "pointer") == 0) {
                inst.mem_seg_kind = MEM_SEG_KIND_POINTER;
            } else if (strcmp(arg_1, "temp") == 0) {
                inst.mem_seg_kind = MEM_SEG_KIND_TEMP;
            } else {
                assert(0 && "Unreachable");
            }

            assert(arg_2 != NULL);
            inst.mem_offset = atoi(arg_2);
        } break;

        case OP_KIND_FUNC:
        case OP_KIND_CALL: {
            inst.func_name = arg_1;
            assert(arg_2 != NULL);
            inst.func_args_num = atoi(arg_2);
        } break;

        case OP_KIND_LABEL:
        case OP_KIND_GOTO:
        case OP_KIND_IF: {
            inst.label = arg_1;
        } break;

        default: break;
    }

    return inst;
}

static void trim_left(char *s)
{
    size_t pad = 0;
    while (isspace(s[pad])) pad++;

    if (pad > 0) {
        for (size_t i = pad; i <= strlen(s); ++i) {
            s[i - pad] = s[i];
        }
    }
}

static void trim_right(char *s)
{
    if (*s == '\0') return;

    size_t pad = strlen(s) - 1;
    while (isspace(s[pad])) pad--;
    s[pad + 1] = '\0';
}
