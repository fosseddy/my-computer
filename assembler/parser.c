#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>

#include "parser.h"

static void trim_left(char *s);
static void trim_right(char *s);

struct parser make_parser(const char *file_path)
{
    FILE *f = fopen(file_path, "r");
    assert(f != NULL);

    return (struct parser) {
        .file = f,
        .line_size = 0,
        .line = NULL
    };
}

int parser_peek_line(struct parser *p)
{
    for (;;) {
        ssize_t read = getline(&p->line, &p->line_size, p->file);

        if (read == -1) {
            fclose(p->file);
            free(p->line);
            return 0;
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

        return 1;
    }
}

struct instruction parser_parse_instruction(struct parser *p)
{
    assert(strlen(p->line) > 0);

    struct instruction inst = {0};

    // instruction A starts with @
    if (strncmp(p->line, "@", 1) == 0) {
        assert(strlen(p->line) - 1 < LABEL_CAPACITY);

        inst.kind = INSTRUCTION_KIND_A;
        // remove '@' from inst: @R1 -> R1
        for (size_t i = 1; i <= strlen(p->line); ++i) {
            inst.label[i - 1] = p->line[i];
        }
    // instruction L starts with '(' and ends with ')'
    } else if (strncmp(p->line, "(", 1) == 0) {
        assert(strlen(p->line) - 2 < LABEL_CAPACITY);

        inst.kind = INSTRUCTION_KIND_L;
        // remove '(' and ')' from label: (LABEL_NAME) -> LABEL_NAME
        size_t i = 1;
        for (; i < strlen(p->line) - 1; ++i) {
            inst.label[i - 1] = p->line[i];
        }
        inst.label[i - 1] = '\0';
    } else {
        inst.kind = INSTRUCTION_KIND_C;

        // if instruction C includes '=', then extract dest
        if (strchr(p->line, '=') != NULL) {
            char line_copy[strlen(p->line)];
            strcpy(line_copy, p->line);

            char *dest = strtok(line_copy, "=");

            assert(strlen(dest) < INSTRUCTION_CAPACITY);
            strcpy(inst.dest, dest);

            char *next_tok = strtok(NULL, "=");
            strcpy(p->line, next_tok);
        }

        // if instruction C includes ';', then extract jump
        if (strchr(p->line, ';') != NULL) {
            char line_copy[strlen(p->line)];
            strcpy(line_copy, p->line);

            char *comp = strtok(line_copy, ";");
            strcpy(p->line, comp);

            char *jump = strtok(NULL, ";");

            assert(strlen(jump) < INSTRUCTION_CAPACITY);
            strcpy(inst.jump, jump);
        }

        assert(strlen(p->line) < INSTRUCTION_CAPACITY);
        strcpy(inst.comp, p->line);
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
