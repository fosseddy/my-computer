#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "symbol-table.h"
#include "translator.h"

#define VARIABLES_RAM 16

static char outfile[256];
static struct symtable symtable;

void populate_symtable(char *infile)
{
    struct parser p = make_parser(infile);
    size_t line_num = 0;
    while (parser_peek_line(&p)) {
        struct instruction inst = parser_parse_instruction(&p);
        if (inst.kind == INSTRUCTION_KIND_L) {
            symtable_put(&symtable, inst.label, line_num);
        } else {
            line_num++;
        }
    }
}

void generate_and_write_machine_code(char *infile)
{
    struct parser p = make_parser(infile);
    FILE *fout = fopen(outfile, "w");
    assert(fout != NULL);

    size_t var_addr = VARIABLES_RAM;
    while (parser_peek_line(&p)) {
        struct instruction inst = parser_parse_instruction(&p);

        switch (inst.kind) {
            case INSTRUCTION_KIND_A: {
                int val = atoi(inst.label);
                if (val == 0) {
                    if (symtable_has(&symtable, inst.label)) {
                        val = symtable_get(&symtable, inst.label);
                    } else {
                        symtable_put(&symtable, inst.label, var_addr);
                        val = var_addr;
                        var_addr++;
                    }
                }

                char code[BINARY_LENGTH];
                translator_translate_inst_a(val, code);
                fprintf(fout, "%s\n", code);
            } break;

            case INSTRUCTION_KIND_C: {
                char code[BINARY_LENGTH];
                translator_translate_inst_c(&inst, code);
                fprintf(fout, "%s\n", code);
            } break;

            case INSTRUCTION_KIND_L: break;
        }
    }

    fclose(fout);
}

void create_outfile(char *infile)
{
    char *ext = ".hack";
    size_t maxlen = sizeof(outfile) - 1,
           extlen = strlen(ext),
           i = 0;

    while (infile[i] != '\0') {
        if (infile[i] == '.' || i > maxlen) break;
        outfile[i] = infile[i];
        i++;
    }

    // truncate
    if (i > maxlen) {
        i -= i - maxlen + extlen;
    }

    outfile[i] = '\0';
    strcat(outfile, ext);
}

int main(int argc, char *argv[])
{
    // @Todo(art): user friendly error handling
    assert(argc > 1);

    char *infile = argv[1];

    create_outfile(infile);
    symtable_init(&symtable);
    populate_symtable(infile);
    generate_and_write_machine_code(infile);

    return 0;
}

