#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#define BINARY_LENGTH 17

typedef struct {
    char *key;
    char *value;
} Translation_Table;

void translator_translate_inst_a(int val, char *out);
void translator_translate_inst_c(Instruction *inst, char *out);

#endif
