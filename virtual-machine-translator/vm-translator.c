#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "parser.h"
#include "translator.h"

#define OUTPUT_EXT ".asm"

void create_out_file_from_in_file(const char *in_file, char *out_file);

int main (int argc, char **argv)
{
    assert(argc > 1);

    const char *in_file = argv[1];
    char out_file[strlen(in_file) + 2];
    create_out_file_from_in_file(in_file, out_file);

    Parser p = make_parser(in_file);

    FILE *f_out = fopen(out_file, "w");
    while (parser_peek_line(&p)) {
        Instruction inst = parser_parse_instruction(&p);
        translator_translate_inst(&inst, f_out);
    }

    fclose(f_out);

    return 0;
}

void create_out_file_from_in_file(const char *in_file, char *out_file)
{
    size_t i = 0;
    for (; in_file[i] != '.'; ++i) out_file[i] = in_file[i];
    out_file[i] = '\0';
    strcat(out_file, OUTPUT_EXT);
}
