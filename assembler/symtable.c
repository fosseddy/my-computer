#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "symtable.h"

struct slot {
    char key[128];
    int value;
};

void symtable_init(struct symtable *st)
{
    st->size = 0;
    st->cap = 32;
    st->slots = malloc(st->cap * sizeof(struct slot));
    assert(st->slots != NULL);

    symtable_put(st, "SP", 0);
    symtable_put(st, "LCL", 1);
    symtable_put(st, "ARG", 2);
    symtable_put(st, "THIS", 3);
    symtable_put(st, "THAT", 4);

    symtable_put(st, "SCREEN", 16384);
    symtable_put(st, "KBD", 24576);

    symtable_put(st, "R0", 0);
    symtable_put(st, "R1", 1);
    symtable_put(st, "R2", 2);
    symtable_put(st, "R3", 3);
    symtable_put(st, "R4", 4);
    symtable_put(st, "R5", 5);
    symtable_put(st, "R6", 6);
    symtable_put(st, "R7", 7);
    symtable_put(st, "R8", 8);
    symtable_put(st, "R9", 9);
    symtable_put(st, "R10", 10);
    symtable_put(st, "R11", 11);
    symtable_put(st, "R12", 12);
    symtable_put(st, "R13", 13);
    symtable_put(st, "R14", 14);
    symtable_put(st, "R15", 15);
}

void symtable_free(struct symtable *st)
{
    free(st->slots);
}

void symtable_put(struct symtable *st, char *key, int val)
{
    struct slot *newslot;

    if (st->size == st->cap) {
        st->cap *= 2;
        st->slots = realloc(st->slots, st->cap * sizeof(struct slot));
        assert(st->slots != NULL);
    }

    newslot = st->slots + st->size;
    newslot->value = val;

    assert(strlen(key) < sizeof(newslot->key));
    memset(newslot->key, 0, strlen(key) + 1);
    memcpy(newslot->key, key, strlen(key));

    st->size++;
}

int symtable_get(struct symtable *st, char *key)
{
    size_t i;

    for (i = 0; i < st->size; ++i) {
        if (strcmp(st->slots[i].key, key) == 0) {
            return st->slots[i].value;
        }
    }

    return -1;
}
