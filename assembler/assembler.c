#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define MAX_INST_SIZE 4
#define MAX_LABEL_SIZE 101

typedef struct {
    char key[MAX_LABEL_SIZE];
    int value;
} Cell;

typedef struct {
    unsigned int size;
    Cell **cells;
} Table;

typedef enum {
    INST_A = 0,
    INST_C,
    INST_L
} Inst_type;

typedef struct {
    Inst_type type;
    char dest[MAX_INST_SIZE];
    char comp[MAX_INST_SIZE];
    char jump[MAX_INST_SIZE];
    char label[MAX_LABEL_SIZE];
} Inst;

void trim_left(char *s);
void trim_right(char *s);

void parse_line(char *line);
void parse_inst(char *line, Inst *inst);

Table *table_new();
void table_delete(Table *t);
void table_insert(Table *t, const char *k, int v);
int table_contains(Table *t, const char *k);
int table_get(Table *t, const char *k);

int main(int argc, char **argv) {
    // @TODO: proper error handle
    assert(argc > 1);

    // @TODO: validate file
    char *file_name = argv[1];
    FILE *f = fopen(file_name, "r");

    // @TODO: proper error handle
    assert(f != NULL);

    Table *sym_table = table_new();

    table_insert(sym_table, "SP", 0);
    table_insert(sym_table, "LCL", 1);
    table_insert(sym_table, "ARG", 2);
    table_insert(sym_table, "THIS", 3);
    table_insert(sym_table, "THAT", 4);

    table_insert(sym_table, "SCREEN", 16384);
    table_insert(sym_table, "KBD", 24576);

    table_insert(sym_table, "R0", 0);
    table_insert(sym_table, "R1", 1);
    table_insert(sym_table, "R2", 2);
    table_insert(sym_table, "R3", 3);
    table_insert(sym_table, "R4", 4);
    table_insert(sym_table, "R5", 5);
    table_insert(sym_table, "R6", 6);
    table_insert(sym_table, "R7", 7);
    table_insert(sym_table, "R8", 8);
    table_insert(sym_table, "R9", 9);
    table_insert(sym_table, "R10", 10);
    table_insert(sym_table, "R11", 11);
    table_insert(sym_table, "R12", 12);
    table_insert(sym_table, "R13", 13);
    table_insert(sym_table, "R14", 14);
    table_insert(sym_table, "R15", 15);

    char *line = NULL;
    size_t line_size = 0;
    while (getline(&line, &line_size, f) != -1) {
        parse_line(line);
        if (strlen(line) == 0) continue;
        printf("%15s | ", line);

        Inst inst = {
            .type = -1,
            .label = "",
            .dest = "",
            .comp = "",
            .jump = ""
        };

        parse_inst(line, &inst);

        assert(inst.type != -1);
        printf("type: %d label: %15s dest: %3s comp: %3s jmp: %3s\n",
                inst.type, inst.label, inst.dest, inst.comp, inst.jump);
    }

    free(line);
    table_delete(sym_table);
    fclose(f);

    return 0;
}

Table *table_new() {
    Table *t = malloc(sizeof(Table));
    t->size = 0;
    t->cells = NULL;

    return t;
}

void table_delete(Table *t) {
    for (int i = 0; i < t->size; ++i) {
        free(t->cells[i]);
    }

    free(t->cells);
    free(t);
}

void table_insert(Table *t, const char *k, int v) {
    t->size++;

    t->cells = realloc(t->cells, t->size * sizeof(Cell));
    assert(t->cells != NULL);

    Cell *c = malloc(sizeof(Cell));
    assert(c != NULL);

    strcpy(c->key, k);
    c->value = v;
    t->cells[t->size - 1] = c;
}

int table_contains(Table *t, const char *k) {
    for (int i = 0; i < t->size; ++i) {
        if (strcmp(t->cells[i]->key, k) == 0) {
            return 1;
        }
    }

    return 0;
}

int table_get(Table *t, const char *k) {
    for (int i = 0; i < t->size; ++i) {
        if (strcmp(t->cells[i]->key, k) == 0) {
            return t->cells[i]->value;
        }
    }

    return -1;
}

void trim_left(char *s) {
    int pad = 0;
    int len = strlen(s);

    for (int i = 0; i < len; ++i) {
        if (isspace(s[i])) {
            pad++;
        } else {
            break;
        }
    }

    if (pad > 0) {
        for (int i = pad; i <= len; ++i) {
            s[i - pad] = s[i];
        }
    }
}

void trim_right(char *s) {
    int len = strlen(s);

    for (int i = len - 1; i >= 0; --i) {
        if (!isspace(s[i])) {
            s[i + 1] = '\0';
            break;
        }
    }
}

void parse_line(char *line) {
    trim_left(line);

    // line includes comment
    if (strstr(line, "//") != NULL) {
        // whole line is comment
        if (strncmp(line, "//", 2) == 0) {
            strcpy(line, "");
        } else {
            // line has inline comment
            char line_copy[strlen(line)];
            strcpy(line_copy, line);
            char *line_with_no_comment = strtok(line_copy, "//");
            strcpy(line, line_with_no_comment);
        }
    }

    trim_right(line);
}

void parse_inst(char *line, Inst *inst) {
    // instruction A starts with @
    if (strncmp(line, "@", 1) == 0) {
        inst->type = INST_A;

        assert(strlen(line) - 1 < MAX_LABEL_SIZE);
        // remove '@' from inst: @R1 -> R1
        for (int i = 1; i <= strlen(line); ++i) {
            inst->label[i - 1] = line[i];
        }
    // instruction L starts with '(' and ends with ')'
    } else if (strncmp(line, "(", 1) == 0) {
        inst->type = INST_L;

        assert(strlen(line) - 2 < MAX_LABEL_SIZE);
        // remove '(' and ')' from label: (LABEL_NAME) -> LABEL_NAME
        for (int i = 1; i < strlen(line) - 1; ++i) {
            inst->label[i - 1] = line[i];
        }
        inst->label[strlen(line) - 2] = '\0';
    } else {
        inst->type = INST_C;

        // if instruction C includes '=', then extract dest
        if (strchr(line, '=') != NULL) {
            char line_copy[strlen(line)];
            strcpy(line_copy, line);

            char *dest = strtok(line_copy, "=");

            assert(strlen(dest) < MAX_INST_SIZE);
            strcpy(inst->dest, dest);

            char *next_tok = strtok(NULL, "=");
            strcpy(line, next_tok);
        }

        // if instruction C includes ';', then extract jump
        if (strchr(line, ';') != NULL) {
            char line_copy[strlen(line)];
            strcpy(line_copy, line);

            char *comp = strtok(line_copy, ";");
            strcpy(line, comp);

            char *jump = strtok(NULL, ";");

            assert(strlen(jump) < MAX_INST_SIZE);
            strcpy(inst->jump, jump);
        }

        assert(strlen(line) < MAX_INST_SIZE);
        strcpy(inst->comp, line);
    }
}
