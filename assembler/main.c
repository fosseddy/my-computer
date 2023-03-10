#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "symbol-table.h"
#include "translator.h"

enum {
    VARS_ADDR_START = 16,
    BINARY_LEN = 17
};

static char outfile[256];
static struct symtable symtable;

void populate_symtable(char *infile)
{
    struct parser p;
    size_t line_num = 0;

    parser_init(&p, infile);
    while (peek_line(&p)) {
        struct instruction inst;

        parse_instruction(&p, &inst);
        if (inst.kind == INSTRUCTION_KIND_L) {
            symtable_put(&symtable, inst.label, line_num);
        } else {
            line_num++;
        }
    }
}

void generate_and_write_machine_code(char *infile)
{
    struct parser p;
    FILE *fout;
    size_t var_addr = VARS_ADDR_START;

    parser_init(&p, infile);
    fout = fopen(outfile, "w");
    assert(fout != NULL);

    while (peek_line(&p)) {
        struct instruction inst;
        char code[BINARY_LEN];

        parse_instruction(&p, &inst);
        switch (inst.kind) {
            case INSTRUCTION_KIND_A: {
                int val;

                val = atoi(inst.label);
                if (val == 0) {
                    if (symtable_has(&symtable, inst.label)) {
                        val = symtable_get(&symtable, inst.label);
                    } else {
                        symtable_put(&symtable, inst.label, var_addr);
                        val = var_addr;
                        var_addr++;
                    }
                }

                translate_inst_a(val, code);
                fprintf(fout, "%s\n", code);
            } break;

            case INSTRUCTION_KIND_C: {
                translate_inst_c(&inst, code);
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
    size_t i, maxlen, extlen;

    maxlen = sizeof(outfile) - 1;
    extlen = strlen(ext);

    i = 0;
    while (infile[i] != '\0') {
        if (infile[i] == '.') break;
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
    // @Todo(art): proper error handling
    assert(argc > 1);

    char *infile = argv[1];

    create_outfile(infile);
    symtable_init(&symtable);
    populate_symtable(infile);
    generate_and_write_machine_code(infile);

    return 0;
}

