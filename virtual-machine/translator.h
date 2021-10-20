#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>
#include "parser.h"

typedef struct {
    FILE *f;
    const char file_name[256];
    size_t unique_counter;
} Translator;

void translator_translate_inst(Translator *t, Instruction *inst);
Translator make_translator(const char *file_path);
void free_translator(Translator *t);

#endif
