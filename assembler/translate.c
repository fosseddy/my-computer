#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "symtable.h"
#include "translate.h"

struct translation {
    char *key;
    char *value;
};

static struct translation dest_table[] = {
    { .key = "",    .value = "000" },
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
    { .key = "",    .value = "000" },
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

static int variable_address = 16;

static char *table_get(struct translation *table, char *key)
{
    int i;

    for (i = 0; table[i].key != NULL; ++i) {
        if (strcmp(table[i].key, key) == 0) {
            return table[i].value;
        }
    }

    return NULL;
}

static int is_number(char *value)
{
    int i;

    for (i = 0; value[i] != '\0'; ++i) {
        if (isdigit(value[i]) == 0) {
            return 0;
        }
    }

    return 1;
}

static int get_symbol_addr(struct command *cmd, struct symtable *st)
{
    int val;

    if (is_number(cmd->symbols) == 1) {
        val = atoi(cmd->symbols);
    } else {
        if (symtable_has(st, cmd->symbols) == 1) {
            val = symtable_get(st, cmd->symbols);
        } else {
            val = variable_address;
            variable_address++;
            symtable_put(st, cmd->symbols, val);
        }
    }

    return val;
}

static void to_binary(int val, char *buf, int bufsize)
{
    int i = bufsize - 2;

    memset(buf, '0', bufsize - 1);

    do {
        buf[i] = val % 2 + '0';
        val = val / 2;
        i--;
    } while (val != 0 && i >= 0);
}

void translate_a(struct command *cmd, char *buf, int bufsize,
                 struct symtable *st)
{
    int addr;

    addr = get_symbol_addr(cmd, st);
    to_binary(addr, buf, bufsize);
}

void translate_c(struct command *cmd, char *buf)
{
    char *dest, *comp, *jump;

    dest = table_get(dest_table, cmd->dest);
    comp = table_get(comp_table, cmd->comp);
    jump = table_get(jump_table, cmd->jump);

    sprintf(buf, "111%s%s%s", comp, dest, jump);
}
