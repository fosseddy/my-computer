#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "parser.h"

static void trim_left(char *s);
static void trim_right(char *s);

//static void dump_str(char *s);

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

    printf("%s\n", p->line);

    char *op= strtok(p->line, " ");
    assert(op != NULL);

    if (strcmp(op, "push") == 0) {
        inst.op_type = OP_TYPE_PUSH;
    } else if (strcmp(op, "add") == 0) {
        inst.op_type = OP_TYPE_ADD;
    } else {
        assert(0);
    }

    char *mem_seg = strtok(NULL, " ");
    if (mem_seg == NULL) return inst;

    if (strcmp(mem_seg, "constant") == 0) {
        inst.mem_seg_type = MEM_SEG_TYPE_CONST;
    } else {
        assert(0);
    }

    char *mem_addr = strtok(NULL, " ");
    if (mem_addr == NULL) return inst;
    inst.mem_addr = atoi(mem_addr);

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

//static void dump_str(char *s)
//{
//    for (size_t i = 0; i <= strlen(s); ++i) {
//        if (s[i] == '\n') {
//            printf("\\n");
//        } else if(isspace(s[i])) {
//            printf(".");
//        } else if(s[i] == '\0') {
//            printf("\\0");
//        } else {
//            printf("%c", s[i]);
//        }
//    }
//    printf("\n");
//}
