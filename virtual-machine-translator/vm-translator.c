#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "parser.h"

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
        printf("%d | ", inst.op_type);
        printf("%d | ", inst.mem_seg_type);
        printf("%ld\n", inst.mem_addr);

        switch (inst.op_type) {
            case OP_TYPE_PUSH: {
                fprintf(f_out, "// PUSH OP\n");
                fprintf(f_out, "@%li\n", inst.mem_addr);
                fprintf(f_out, "D=A\n");
                fprintf(f_out, "@SP\n");
                fprintf(f_out, "A=M\n");
                fprintf(f_out, "M=D\n");
                fprintf(f_out, "@SP\n");
                fprintf(f_out, "M=M+1\n");
            } break;

            case OP_TYPE_ADD: {
                fprintf(f_out, "// ADD OP\n");
            } break;
        }
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
