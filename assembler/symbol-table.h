#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

#include "parser.h"

typedef struct {
    char key[LABEL_CAPACITY];
    int value;
} Cell;

typedef struct {
    size_t size;
    Cell *cells;
} Symbol_Table;

Symbol_Table make_symbol_table();
void free_symbol_table(Symbol_Table *t);
void symbol_table_insert(Symbol_Table *t, const char *k, int v);
bool symbol_table_contains(Symbol_Table *t, const char *k);
int symbol_table_get(Symbol_Table *t, const char *k);

#endif
