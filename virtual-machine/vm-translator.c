#include <assert.h>
#include "parser.h"
#include "translator.h"
#include "path.h"

#define OUTPUT_EXT ".asm"

int main (int argc, char **argv)
{
    assert(argc > 1);

    const char *in_file = argv[1];
    char out_file[256];
    path_get_file_name(out_file, in_file);
    path_concat_ext(out_file, OUTPUT_EXT);

    Parser p = make_parser(in_file);
    Translator t = make_translator(out_file);

    while (parser_peek_line(&p)) {
        Instruction inst = parser_parse_instruction(&p);

        printf("----------------------------------------\n");
        printf("op_kind: %i\n", inst.op_kind);
        printf("mem_seg_kind: %i\n", inst.mem_seg_kind);
        printf("mem_offset: %li\n", inst.mem_offset);
        printf("label: %s\n", inst.label);
        printf("func_name: %s\n", inst.func_name);
        printf("func_args_num: %li\n", inst.func_args_num);
        printf("----------------------------------------\n");

        translator_translate_inst(&t, &inst);
    }

    free_translator(&t);

    return 0;
}
