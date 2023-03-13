struct symtable {
    size_t size;
    size_t cap;
    struct slot *slots; // defined in symtable.c
};

void symtable_init(struct symtable *st);
void symtable_free(struct symtable *st);
void symtable_put(struct symtable *st, char *key, int value);
int symtable_get(struct symtable *st, char *key);
int symtable_has(struct symtable *st, char *key);
