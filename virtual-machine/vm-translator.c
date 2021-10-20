#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "translator.h"

#define OUTPUT_EXT ".asm"
#define PATH_SEP '/'

void create_out_file_from_in_file(const char *in_file, char *out_file);

int main (int argc, char **argv)
{
    assert(argc > 1);

    const char *in_file = argv[1];
    char out_file[256];
    create_out_file_from_in_file(in_file, out_file);

    Parser p = make_parser(in_file);
    Translator t = make_translator(out_file);

    while (parser_peek_line(&p)) {
        Instruction inst = parser_parse_instruction(&p);
        translator_translate_inst(&t, &inst);
    }

    free_translator(&t);

    return 0;
}

void create_out_file_from_in_file(const char *in_file, char *out_file)
{
    int dot_pos = strlen(in_file) - 3;
    int len = 0;
    for (int i = dot_pos - 1; i >= 0 && in_file[i] != PATH_SEP; --i, ++len) {
        out_file[len] = in_file[i];
    }
    out_file[len] = '\0';

    for (int i = 0; i < (len / 2); ++i) {
        int tmp = out_file[i];
        out_file[i] = out_file[len - 1 - i];
        out_file[len - 1 - i] = tmp;
    }

    strcat(out_file, OUTPUT_EXT);
}
