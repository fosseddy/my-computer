#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "symbol-table.h"

Symbol_Table make_symbol_table()
{
    Symbol_Table t = {
        .size = 0,
        .cells = NULL
    };

    symbol_table_insert(&t, "SP", 0);
    symbol_table_insert(&t, "LCL", 1);
    symbol_table_insert(&t, "ARG", 2);
    symbol_table_insert(&t, "THIS", 3);
    symbol_table_insert(&t, "THAT", 4);

    symbol_table_insert(&t, "SCREEN", 16384);
    symbol_table_insert(&t, "KBD", 24576);

    symbol_table_insert(&t, "R0", 0);
    symbol_table_insert(&t, "R1", 1);
    symbol_table_insert(&t, "R2", 2);
    symbol_table_insert(&t, "R3", 3);
    symbol_table_insert(&t, "R4", 4);
    symbol_table_insert(&t, "R5", 5);
    symbol_table_insert(&t, "R6", 6);
    symbol_table_insert(&t, "R7", 7);
    symbol_table_insert(&t, "R8", 8);
    symbol_table_insert(&t, "R9", 9);
    symbol_table_insert(&t, "R10", 10);
    symbol_table_insert(&t, "R11", 11);
    symbol_table_insert(&t, "R12", 12);
    symbol_table_insert(&t, "R13", 13);
    symbol_table_insert(&t, "R14", 14);
    symbol_table_insert(&t, "R15", 15);

    return t;
}

void symbol_table_insert(Symbol_Table *t, const char *k, int v)
{
    t->size++;

    t->cells = realloc(t->cells, t->size * sizeof(Cell));
    assert(t->cells != NULL);

    t->cells[t->size - 1] = (Cell) { .key = (char *) k, .value = v };
}

bool symbol_table_contains(Symbol_Table *t, const char *k)
{
    for (size_t i = 0; i < t->size; ++i) {
        if (strcmp(t->cells[i].key, k) == 0) {
            return true;
        }
    }

    return false;
}

int symbol_table_get(Symbol_Table *t, const char *k)
{
    for (size_t i = 0; i < t->size; ++i) {
        if (strcmp(t->cells[i].key, k) == 0) {
            return t->cells[i].value;
        }
    }

    return -1;
}

void free_symbol_table(Symbol_Table *t)
{
    free(t->cells);
}
