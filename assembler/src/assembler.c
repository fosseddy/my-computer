#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "symbol-table.h"
#include "translator.h"

#define VARIABLES_RAM 16
#define OUTPUT_EXT ".hack"

void create_out_file_from_in_file(const char *in_file, char *out_file);
void populate_sym_table(Symbol_Table *sym_table, const char *in_file);
void generate_and_write_machine_code(Symbol_Table *sym_table,
        const char *in_file, const char *out_file);

int main(int argc, char **argv)
{
    assert(argc > 1);

    const char *in_file = argv[1];
    char out_file[strlen(in_file) + 2];
    create_out_file_from_in_file(in_file, out_file);

    Symbol_Table sym_table = make_symbol_table();

    populate_sym_table(&sym_table, in_file);
    generate_and_write_machine_code(&sym_table, in_file, out_file);

    free_symbol_table(&sym_table);

    return 0;
}

void populate_sym_table(Symbol_Table *sym_table, const char *in_file)
{
    Parser p = make_parser(in_file);
    size_t line_num = 0;
    while (parser_peek_line(&p)) {
        Instruction inst = parser_parse_instruction(&p);
        if (inst.type == INSTRUCTION_TYPE_L) {
            symbol_table_insert(sym_table, inst.label, line_num);
        } else {
            line_num++;
        }
    }
}

void generate_and_write_machine_code(Symbol_Table *sym_table,
        const char *in_file, const char *out_file)
{
    Parser p = make_parser(in_file);
    FILE *f_out = fopen(out_file, "w");
    assert(f_out != NULL);

    size_t var_addr = VARIABLES_RAM;
    while (parser_peek_line(&p)) {
        Instruction inst = parser_parse_instruction(&p);

        switch (inst.type) {
            case INSTRUCTION_TYPE_A: {
                int val = atoi(inst.label);
                if (val == 0) {
                    if (symbol_table_contains(sym_table, inst.label)) {
                        val = symbol_table_get(sym_table, inst.label);
                    } else {
                        symbol_table_insert(sym_table, inst.label, var_addr);
                        val = var_addr;
                        var_addr++;
                    }
                }

                char code[BINARY_LENGTH];
                translator_translate_inst_a(val, code);
                fprintf(f_out, "%s\n", code);

            } break;

            case INSTRUCTION_TYPE_C: {
                char code[BINARY_LENGTH];
                translator_translate_inst_c(&inst, code);
                fprintf(f_out, "%s\n", code);
            } break;

            case INSTRUCTION_TYPE_L: break;
        }
    }

    fclose(f_out);
}

void create_out_file_from_in_file(const char *in_file, char *out_file)
{
    size_t i = 0;
    for (; in_file[i] != '.'; ++i) out_file[i] = in_file[i];
    out_file[i] = '\0';
    strcat(out_file, OUTPUT_EXT);
}
