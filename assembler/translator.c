#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "translator.h"
#include "parser.h"

#define DEST_TABLE_LENGTH 7
#define JUMP_TABLE_LENGTH 7
#define COMP_TABLE_LENGTH 28

static char *translation_table_get(struct translation_table *t, size_t t_len, char *k);

static struct translation_table dest_table[DEST_TABLE_LENGTH] = {
    { .key = "M", .value = "001" },
    { .key = "D", .value = "010" },
    { .key = "MD", .value = "011" },
    { .key = "A", .value = "100" },
    { .key = "AM", .value = "101" },
    { .key = "AD", .value = "110" },
    { .key = "AMD", .value = "111" }
};

static struct translation_table jump_table[JUMP_TABLE_LENGTH] = {
    { .key = "JGT", .value = "001" },
    { .key = "JEQ", .value = "010" },
    { .key = "JGE", .value = "011" },
    { .key = "JLT", .value = "100" },
    { .key = "JNE", .value = "101" },
    { .key = "JLE", .value = "110" },
    { .key = "JMP", .value = "111" }
};

static struct translation_table comp_table[COMP_TABLE_LENGTH] = {
    { .key = "0", .value = "0101010" },
    { .key = "1", .value = "0111111" },
    { .key = "-1", .value = "0111010" },
    { .key = "D", .value = "0001100" },
    { .key = "A", .value = "0110000" },
    { .key = "M", .value = "1110000" },

    { .key = "!D", .value = "0001101" },
    { .key = "!A", .value = "0110001" },
    { .key = "!M", .value = "1110001" },

    { .key = "-D", .value = "0001111" },
    { .key = "-A", .value = "0110011" },
    { .key = "-M", .value = "1110011" },

    { .key = "D+1", .value = "0011111" },
    { .key = "A+1", .value = "0110111" },
    { .key = "M+1", .value = "1110111" },

    { .key = "D-1", .value = "0001110" },
    { .key = "A-1", .value = "0110010" },
    { .key = "M-1", .value = "1110010" },

    { .key = "D+A", .value = "0000010" },
    { .key = "D+M", .value = "1000010" },
    { .key = "D-A", .value = "0010011" },
    { .key = "D-M", .value = "1010011" },

    { .key = "A-D", .value = "0000111" },
    { .key = "M-D", .value = "1000111" },

    { .key = "D&A", .value = "0000000" },
    { .key = "D&M", .value = "1000000" },

    { .key = "D|A", .value = "0010101" },
    { .key = "D|M", .value = "1010101" }
};

void translator_translate_inst_a(int val, char *out)
{
    char binary[BINARY_LENGTH] = "";
    size_t i = 0;

    do {
        sprintf(&binary[i], "%d", val % 2);
        val = val / 2;
        i++;
    } while (val != 0);

    for (size_t j = 0; j < (i / 2); ++j) {
        char tmp = binary[j];
        binary[j] = binary[i - 1 - j];
        binary[i - 1 - j] = tmp;
    }

    for (size_t j = 0; j < BINARY_LENGTH - i - 1; ++j) {
        strcpy(&out[j], "0");
    }
    out[BINARY_LENGTH] = '\0';

    strcat(out, binary);
}

void translator_translate_inst_c(struct instruction *inst, char *out)
{
    char dest[4] = "";
    char jump[4] = "";
    char comp[8] = "";

    if (strlen(inst->dest) == 0) {
        strcpy(dest, "000");
    } else {
        char *v = translation_table_get(dest_table, DEST_TABLE_LENGTH, inst->dest);
        assert(v != NULL);
        strcpy(dest, v);
    }

    if (strlen(inst->jump) == 0) {
        strcpy(jump, "000");
    } else {
        char *v = translation_table_get(jump_table, JUMP_TABLE_LENGTH, inst->jump);
        assert(v != NULL);
        strcpy(jump, v);
    }

    char *v = translation_table_get(comp_table, COMP_TABLE_LENGTH, inst->comp);
    assert(v != NULL);
    strcpy(comp, v);

    strcpy(out,"111");
    strcat(out, comp);
    strcat(out, dest);
    strcat(out, jump);
}

static char *translation_table_get(struct translation_table *t, size_t t_len, char *k)
{
    for (size_t i = 0; i < t_len; ++i) {
        if (strcmp(t[i].key, k) == 0) {
            return t[i].value;
        }
    }

    return NULL;
}
