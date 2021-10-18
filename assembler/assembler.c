#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "symbol-table.h"
#include "translator.h"

int main(int argc, char **argv) {
    // @TODO: proper error handle
    assert(argc > 1);

    // @TODO: validate file
    const char *file_name = argv[1];

    // first pass
    Symbol_Table sym_table = make_symbol_table();
    Parser p = make_parser(file_name);
    size_t line_num = 0;
    while (parser_peek_line(&p)) {
        Instruction inst = parser_parse_instruction(&p);
        if (inst.type == INSTRUCTION_TYPE_L) {
            symbol_table_insert(&sym_table, inst.label, line_num);
        } else {
            line_num++;
        }
    }

    // second pass
    p = make_parser(file_name);
    char out_file_name[strlen(file_name) + 2];
    int i = 0;
    for (i = 0; i < strlen(file_name); ++i) {
        out_file_name[i] = file_name[i];
        if (file_name[i] == '.') {
            break;
        }
    }
    out_file_name[i+1] = 'h';
    out_file_name[i+2] = 'a';
    out_file_name[i+3] = 'c';
    out_file_name[i+4] = 'k';
    out_file_name[i+5] = '\0';

    FILE *out = fopen(out_file_name, "w");
    // @TODO: proper error handle
    assert(out != NULL);

    size_t var_addr = 16;
    while (parser_peek_line(&p)) {
        Instruction inst = parser_parse_instruction(&p);

        switch (inst.type) {
            case INSTRUCTION_TYPE_A: {
                int val = atoi(inst.label);
                if (val == 0) {
                    if (symbol_table_contains(&sym_table, inst.label)) {
                        val = symbol_table_get(&sym_table, inst.label);
                    } else {
                        symbol_table_insert(&sym_table, inst.label, var_addr);
                        val = var_addr;
                        var_addr++;
                    }
                }

                char code[BINARY_LENGTH] = "";
                translator_translate_inst_a(val, code);
                fprintf(out, "%s\n", code);

            } break;

            case INSTRUCTION_TYPE_C: {
                char code[BINARY_LENGTH] = "";
                translator_translate_inst_c(&inst, code);
                fprintf(out, "%s\n", code);
            } break;

            default:
        }
    }

    fclose(out);
    free_symbol_table(&sym_table);

    return 0;
}

