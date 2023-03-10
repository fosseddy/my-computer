#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "translator.h"
#include "parser.h"

#define BINARY_LEN 17

struct translation {
    char *key;
    char *value;
};

static struct translation dest_table[] = {
    { .key = "M",   .value = "001" },
    { .key = "D",   .value = "010" },
    { .key = "MD",  .value = "011" },
    { .key = "A",   .value = "100" },
    { .key = "AM",  .value = "101" },
    { .key = "AD",  .value = "110" },
    { .key = "AMD", .value = "111" },
    { .key = NULL }
};

static struct translation jump_table[] = {
    { .key = "JGT", .value = "001" },
    { .key = "JEQ", .value = "010" },
    { .key = "JGE", .value = "011" },
    { .key = "JLT", .value = "100" },
    { .key = "JNE", .value = "101" },
    { .key = "JLE", .value = "110" },
    { .key = "JMP", .value = "111" },
    { .key = NULL }
};

static struct translation comp_table[] = {
    { .key = "0",   .value = "0101010" },
    { .key = "1",   .value = "0111111" },
    { .key = "-1",  .value = "0111010" },
    { .key = "D",   .value = "0001100" },
    { .key = "A",   .value = "0110000" },
    { .key = "M",   .value = "1110000" },
    { .key = "!D",  .value = "0001101" },
    { .key = "!A",  .value = "0110001" },
    { .key = "!M",  .value = "1110001" },
    { .key = "-D",  .value = "0001111" },
    { .key = "-A",  .value = "0110011" },
    { .key = "-M",  .value = "1110011" },
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
    { .key = "D|M", .value = "1010101" },
    { .key = NULL }
};

static char *table_get(struct translation *table, char *k)
{
    size_t i;

    for (i = 0; table[i].key != NULL; i++) {
        if (strcmp(table[i].key, k) == 0) {
            return table[i].value;
        }
    }
    return NULL;
}

void translate_inst_a(int val, char *out)
{
    char binary[BINARY_LEN] = {0};
    size_t i, j;

    i = 0;
    do {
        sprintf(binary + i, "%d", val % 2);
        val = val / 2;
        i++;
    } while (val != 0);

    for (j = 0; j < (i / 2); j++) {
        char tmp;

        tmp = binary[j];
        binary[j] = binary[i - 1 - j];
        binary[i - 1 - j] = tmp;
    }

    for (size_t j = 0; j < BINARY_LEN - i - 1; ++j) {
        strcpy(&out[j], "0");
    }

    strcat(out, binary);
}

// @Todo(art): proper error handling
void translate_inst_c(struct instruction *inst, char *out)
{
    char dest[4] = {0};
    char jump[4] = {0};
    char comp[8] = {0};

    if (strlen(inst->dest) == 0) {
        strcpy(dest, "000");
    } else {
        char *v = table_get(dest_table, inst->dest);
        assert(v != NULL);
        strcpy(dest, v);
    }

    if (strlen(inst->jump) == 0) {
        strcpy(jump, "000");
    } else {
        char *v = table_get(jump_table, inst->jump);
        assert(v != NULL);
        strcpy(jump, v);
    }

    char *v = table_get(comp_table, inst->comp);
    assert(v != NULL);
    strcpy(comp, v);

    strcpy(out,"111");
    strcat(out, comp);
    strcat(out, dest);
    strcat(out, jump);
}
