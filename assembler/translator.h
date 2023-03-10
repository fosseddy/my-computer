#define BINARY_LENGTH 17

struct instruction;

struct translation_table {
    char *key;
    char *value;
};

void translator_translate_inst_a(int val, char *out);
void translator_translate_inst_c(struct instruction *inst, char *out);
