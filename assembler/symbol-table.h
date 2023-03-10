#define LABEL_CAPACITY 101

struct cell {
    char key[LABEL_CAPACITY];
    int value;
};

struct symbol_table {
    size_t size;
    struct cell *cells;
};

struct symbol_table make_symbol_table();
void free_symbol_table(struct symbol_table *t);
void symbol_table_insert(struct symbol_table *t, const char *k, int v);
int symbol_table_contains(struct symbol_table *t, const char *k);
int symbol_table_get(struct symbol_table *t, const char *k);
