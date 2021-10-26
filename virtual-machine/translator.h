#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>
#include "parser.h"

struct Translator {
    FILE *f;
    const char file_name[256];
    size_t unique_counter;
};

void translator_translate_inst(struct Translator *t, struct Instruction *inst);
struct Translator make_translator(const char *file_path);
void free_translator(struct Translator *t);

#endif
