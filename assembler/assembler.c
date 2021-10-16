#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define MAX_INST_SIZE 4
#define MAX_LABEL_SIZE 101
#define MAX_CELL_STR_VAL 8

typedef union {
    int as_int;
    char as_str[MAX_CELL_STR_VAL];
} Cell_Value;

typedef struct {
    char key[MAX_LABEL_SIZE];
    Cell_Value value;
} Cell;

typedef struct {
    size_t size;
    Cell **cells;
} Table;

typedef enum {
    INST_A = 0,
    INST_C,
    INST_L
} Inst_Type;

typedef struct {
    Inst_Type type;
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
void table_insert(Table *t, const char *k, Cell_Value v);
int table_contains(Table *t, const char *k);
Cell_Value table_get(Table *t, const char *k);

int main(int argc, char **argv) {
    // @TODO: proper error handle
    assert(argc > 1);

    // @TODO: validate file
    char *file_name = argv[1];
    FILE *f = fopen(file_name, "r");

    // @TODO: proper error handle
    assert(f != NULL);

    Table *sym_table = table_new();

    table_insert(sym_table, "SP", (Cell_Value){ .as_int = 0 });
    table_insert(sym_table, "LCL", (Cell_Value){ .as_int = 1 });
    table_insert(sym_table, "ARG", (Cell_Value){ .as_int = 2 });
    table_insert(sym_table, "THIS", (Cell_Value){ .as_int = 3 });
    table_insert(sym_table, "THAT", (Cell_Value){ .as_int = 4 });

    table_insert(sym_table, "SCREEN", (Cell_Value){ .as_int = 16384 });
    table_insert(sym_table, "KBD", (Cell_Value){ .as_int = 24576 });

    table_insert(sym_table, "R0", (Cell_Value){ .as_int = 0 });
    table_insert(sym_table, "R1", (Cell_Value){ .as_int = 1 });
    table_insert(sym_table, "R2", (Cell_Value){ .as_int = 2 });
    table_insert(sym_table, "R3", (Cell_Value){ .as_int = 3 });
    table_insert(sym_table, "R4", (Cell_Value){ .as_int = 4 });
    table_insert(sym_table, "R5", (Cell_Value){ .as_int = 5 });
    table_insert(sym_table, "R6", (Cell_Value){ .as_int = 6 });
    table_insert(sym_table, "R7", (Cell_Value){ .as_int = 7 });
    table_insert(sym_table, "R8", (Cell_Value){ .as_int = 8 });
    table_insert(sym_table, "R9", (Cell_Value){ .as_int = 9 });
    table_insert(sym_table, "R10", (Cell_Value){ .as_int = 10 });
    table_insert(sym_table, "R11", (Cell_Value){ .as_int = 11 });
    table_insert(sym_table, "R12", (Cell_Value){ .as_int = 12 });
    table_insert(sym_table, "R13", (Cell_Value){ .as_int = 13 });
    table_insert(sym_table, "R14", (Cell_Value){ .as_int = 14 });
    table_insert(sym_table, "R15", (Cell_Value){ .as_int = 15 });

    char *line = NULL;
    size_t line_size = 0;

    size_t line_num = 0;
    while (getline(&line, &line_size, f) != -1) {
        parse_line(line);
        if (strlen(line) == 0) continue;

        Inst inst = {
            .type = -1,
            .label = "",
            .dest = "",
            .comp = "",
            .jump = ""
        };

        parse_inst(line, &inst);
        assert(inst.type != -1);

        if (inst.type == INST_L) {
            table_insert(sym_table, inst.label, (Cell_Value){ .as_int = line_num });
        } else {
            line_num++;
        }
    }

    fclose(f);
    free(line);

    // @TODO: validate file
    f = fopen(file_name, "r");
    // @TODO: proper error handle
    assert(f != NULL);

    Table *dest_table = table_new();

    table_insert(dest_table, "M", (Cell_Value){ .as_str = "001" });
    table_insert(dest_table, "D", (Cell_Value){ .as_str = "010" });
    table_insert(dest_table, "MD", (Cell_Value){ .as_str = "011" });
    table_insert(dest_table, "A", (Cell_Value){ .as_str = "100" });
    table_insert(dest_table, "AM", (Cell_Value){ .as_str = "101" });
    table_insert(dest_table, "AD", (Cell_Value){ .as_str = "110" });
    table_insert(dest_table, "AMD", (Cell_Value){ .as_str = "111" });

    Table *jump_table = table_new();

    table_insert(jump_table, "JGT", (Cell_Value){ .as_str = "001" });
    table_insert(jump_table, "JEQ", (Cell_Value){ .as_str = "010" });
    table_insert(jump_table, "JGE", (Cell_Value){ .as_str = "011" });
    table_insert(jump_table, "JLT", (Cell_Value){ .as_str = "100" });
    table_insert(jump_table, "JNE", (Cell_Value){ .as_str = "101" });
    table_insert(jump_table, "JLE", (Cell_Value){ .as_str = "110" });
    table_insert(jump_table, "JMP", (Cell_Value){ .as_str = "111" });

    Table *comp_table = table_new();

    table_insert(comp_table, "0", (Cell_Value){ .as_str = "0101010" });
    table_insert(comp_table, "1", (Cell_Value){ .as_str = "0111111" });
    table_insert(comp_table, "-1", (Cell_Value){ .as_str = "0111010" });
    table_insert(comp_table, "D", (Cell_Value){ .as_str = "0001100" });
    table_insert(comp_table, "A", (Cell_Value){ .as_str = "0110000" });
    table_insert(comp_table, "M", (Cell_Value){ .as_str = "1110000" });

    table_insert(comp_table, "!D", (Cell_Value){ .as_str = "0001101" });
    table_insert(comp_table, "!A", (Cell_Value){ .as_str = "0110001" });
    table_insert(comp_table, "!M", (Cell_Value){ .as_str = "1110001" });

    table_insert(comp_table, "-D", (Cell_Value){ .as_str = "0001111" });
    table_insert(comp_table, "-A", (Cell_Value){ .as_str = "0110011" });
    table_insert(comp_table, "-M", (Cell_Value){ .as_str = "1110011" });

    table_insert(comp_table, "D+1", (Cell_Value){ .as_str = "0011111" });
    table_insert(comp_table, "A+1", (Cell_Value){ .as_str = "0110111" });
    table_insert(comp_table, "M+1", (Cell_Value){ .as_str = "1110111" });

    table_insert(comp_table, "D-1", (Cell_Value){ .as_str = "0001110" });
    table_insert(comp_table, "A-1", (Cell_Value){ .as_str = "0110010" });
    table_insert(comp_table, "M-1", (Cell_Value){ .as_str = "1110010" });

    table_insert(comp_table, "D+A", (Cell_Value){ .as_str = "0000010" });
    table_insert(comp_table, "D+M", (Cell_Value){ .as_str = "1000010" });
    table_insert(comp_table, "D-A", (Cell_Value){ .as_str = "0010011" });
    table_insert(comp_table, "D-M", (Cell_Value){ .as_str = "1010011" });

    table_insert(comp_table, "A-D", (Cell_Value){ .as_str = "0000111" });
    table_insert(comp_table, "M-D", (Cell_Value){ .as_str = "1000111" });

    table_insert(comp_table, "D&A", (Cell_Value){ .as_str = "0000000" });
    table_insert(comp_table, "D&M", (Cell_Value){ .as_str = "1000000" });

    table_insert(comp_table, "D|A", (Cell_Value){ .as_str = "0010101" });
    table_insert(comp_table, "D|M", (Cell_Value){ .as_str = "1010101" });

    line = NULL;
    line_size = 0;
    size_t var_addr = 16;
    while (getline(&line, &line_size, f) != -1) {
        parse_line(line);
        if (strlen(line) == 0) continue;

        Inst inst = {
            .type = -1,
            .label = "",
            .dest = "",
            .comp = "",
            .jump = ""
        };

        parse_inst(line, &inst);
        assert(inst.type != -1);

        switch (inst.type) {
            case INST_A: {
                int val = atoi(inst.label);
                if (val == 0) {
                    if (table_contains(sym_table, inst.label)) {
                        val = table_get(sym_table, inst.label).as_int;
                    } else {
                        table_insert(sym_table, inst.label, (Cell_Value){ .as_int = var_addr });
                        val = var_addr;
                        var_addr++;
                    }
                }

                char binary[17] = "";
                int i = 0;

                do {
                    sprintf(&binary[i], "%d", val % 2);
                    val = val / 2;
                    i++;
                } while (val != 0);

                for (int j = 0; j < (i / 2); ++j) {
                    char tmp = binary[j];
                    binary[j] = binary[i - 1 - j];
                    binary[i - 1 - j] = tmp;
                }

                char code[17] = "";
                for (int j = 0; j < 17 - i - 1; ++j) {
                    strcpy(&code[j], "0");
                }
                code[17] = '\0';

                strcat(code, binary);

                printf("%16s\n", code);
            } break;

            case INST_C: {
                char dest[MAX_CELL_STR_VAL] = "";
                char jump[MAX_CELL_STR_VAL] = "";
                char comp[MAX_CELL_STR_VAL] = "";

                if (strlen(inst.dest) == 0) {
                    dest[0] = '0';
                    dest[1] = '0';
                    dest[2] = '0';
                    dest[3] = '\0';
                } else {
                    Cell_Value cv = table_get(dest_table, inst.dest);
                    strcpy(dest, cv.as_str);
                }

                if (strlen(inst.jump) == 0) {
                    jump[0] = '0';
                    jump[1] = '0';
                    jump[2] = '0';
                    jump[3] = '\0';
                } else {
                    Cell_Value cv = table_get(jump_table, inst.jump);
                    strcpy(jump, cv.as_str);
                }

                Cell_Value cv = table_get(comp_table, inst.comp);
                strcpy(comp, cv.as_str);

                char code[17] = "111";
                strcat(code, comp);
                strcat(code, dest);
                strcat(code, jump);

                printf("%16s\n", code);
            } break;

            default:
        }
    }

    fclose(f);
    free(line);
    table_delete(sym_table);
    table_delete(dest_table);
    table_delete(jump_table);
    table_delete(comp_table);

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

void table_insert(Table *t, const char *k, Cell_Value v) {
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

Cell_Value table_get(Table *t, const char *k) {
    for (int i = 0; i < t->size; ++i) {
        if (strcmp(t->cells[i]->key, k) == 0) {
            return t->cells[i]->value;
        }
    }

    Cell_Value cv = { .as_int = -1 };
    return cv;
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
