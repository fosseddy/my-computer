#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>
#include "parser.h"

void translator_translate_inst(Instruction *inst, FILE *f);

#endif
