#include <stdlib.h>
#include <assert.h>
#include "translator.h"

#define STATIC_BASE_ADDR 16
#define STATIC_MAX_ADDR 255

#define TEMP_BASE_ADDR 5
#define TEMP_MAX_ADDR 12

static void jump_to_top_value(FILE *f)
{
    fprintf(f, "@SP\n");
    fprintf(f, "A=M-1\n");
}

static void jump_to_stack_pointer(FILE *f)
{
    fprintf(f, "@SP\n");
    fprintf(f, "A=M\n");
}

static void dec_stack_pointer(FILE *f)
{
    fprintf(f, "@SP\n");
    fprintf(f, "M=M-1\n");
}

static void inc_stack_pointer(FILE *f)
{
    fprintf(f, "@SP\n");
    fprintf(f, "M=M+1\n");
}

Translator make_translator(const char *file_path)
{
    FILE *file = fopen(file_path, "w");
    assert (file != NULL);

    return (Translator) {
        .f = file,
        .file_name = file_path,
        .unique_counter = 0
    };
}

void free_translator(Translator *t)
{
    free(t->f);
}

void translator_translate_inst(Translator *t, Instruction *inst)
{
    printf(t->file_name);
    printf("\n");
    switch (inst->op_type) {
        case OP_TYPE_PUSH:
        case OP_TYPE_POP: {
            switch (inst->mem_seg_type) {
                case MEM_SEG_TYPE_LCL: {
                    fprintf(t->f, "@LCL\n");
                    fprintf(t->f, "D=M\n");
                    fprintf(t->f, "@%li\n", inst->mem_addr);
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(t->f, "A=D+A\n");
                        fprintf(t->f, "D=M\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        fprintf(t->f, "D=D+A\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "M=D\n");

                        dec_stack_pointer(t->f);

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "D=M\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "A=M\n");
                        fprintf(t->f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_ARG: {
                    fprintf(t->f, "@ARG\n");
                    fprintf(t->f, "D=M\n");
                    fprintf(t->f, "@%li\n", inst->mem_addr);
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(t->f, "A=D+A\n");
                        fprintf(t->f, "D=M\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        fprintf(t->f, "D=D+A\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "M=D\n");

                        dec_stack_pointer(t->f);

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "D=M\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "A=M\n");
                        fprintf(t->f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_THIS: {
                    fprintf(t->f, "@THIS\n");
                    fprintf(t->f, "D=M\n");
                    fprintf(t->f, "@%li\n", inst->mem_addr);
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(t->f, "A=D+A\n");
                        fprintf(t->f, "D=M\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        fprintf(t->f, "D=D+A\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "M=D\n");

                        dec_stack_pointer(t->f);

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "D=M\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "A=M\n");
                        fprintf(t->f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_THAT: {
                    fprintf(t->f, "@THAT\n");
                    fprintf(t->f, "D=M\n");
                    fprintf(t->f, "@%li\n", inst->mem_addr);
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(t->f, "A=D+A\n");
                        fprintf(t->f, "D=M\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        fprintf(t->f, "D=D+A\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "M=D\n");

                        dec_stack_pointer(t->f);

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "D=M\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "A=M\n");
                        fprintf(t->f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_CONST: {
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(t->f, "@%li\n", inst->mem_addr);
                        fprintf(t->f, "D=A\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        assert(0);
                    }
                } break;

                case MEM_SEG_TYPE_STATIC: {
                    if (inst->op_type == OP_TYPE_PUSH) {
                        // @TODO: change for file name
                        fprintf(t->f, "@%s.%li\n", t->file_name, inst->mem_addr);
                        fprintf(t->f, "D=M\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        dec_stack_pointer(t->f);

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "D=M\n");

                        // @TODO: change for file name
                        fprintf(t->f, "@%s.%li\n", t->file_name, inst->mem_addr);
                        fprintf(t->f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_TEMP: {
                    size_t mem_addr = inst->mem_addr + TEMP_BASE_ADDR;
                    assert(mem_addr <= TEMP_MAX_ADDR);

                    fprintf(t->f, "@%li\n", mem_addr);
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(t->f, "D=M\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        fprintf(t->f, "D=A\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "M=D\n");

                        dec_stack_pointer(t->f);

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "D=M\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "A=M\n");
                        fprintf(t->f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_POINTER: {
                    assert(inst->mem_addr == 0 || inst->mem_addr == 1);

                    char *mem_addr = NULL;
                    if (inst->mem_addr == 1) {
                        mem_addr = "THAT";
                    } else {
                        mem_addr = "THIS";
                    }

                    fprintf(t->f, "@%s\n", mem_addr);
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(t->f, "D=M\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        fprintf(t->f, "D=A\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "M=D\n");

                        dec_stack_pointer(t->f);

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "D=M\n");

                        fprintf(t->f, "@R15\n");
                        fprintf(t->f, "A=M\n");
                        fprintf(t->f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_UNINIT: assert(0);
            }
        } break;

        case OP_TYPE_ADD: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");
            dec_stack_pointer(t->f);
            jump_to_top_value(t->f);
            fprintf(t->f, "M=D+M\n");
        } break;

        case OP_TYPE_SUB: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");
            dec_stack_pointer(t->f);
            jump_to_top_value(t->f);
            fprintf(t->f, "M=M-D\n");
        } break;

        case OP_TYPE_EQ: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");

            dec_stack_pointer(t->f);

            jump_to_top_value(t->f);
            fprintf(t->f, "D=M-D\n");

            fprintf(t->f, "M=0\n");
            fprintf(t->f, "@eq_%li\n", t->unique_counter);
            fprintf(t->f, "D;JNE\n");

            jump_to_top_value(t->f);
            fprintf(t->f, "M=-1\n");
            fprintf(t->f, "(eq_%li)\n", t->unique_counter);

            t->unique_counter++;
        } break;

        case OP_TYPE_LT: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");

            dec_stack_pointer(t->f);

            jump_to_top_value(t->f);
            fprintf(t->f, "D=M-D\n");

            fprintf(t->f, "M=0\n");
            fprintf(t->f, "@lt_%li\n", t->unique_counter);
            fprintf(t->f, "D;JGE\n");

            jump_to_top_value(t->f);
            fprintf(t->f, "M=-1\n");
            fprintf(t->f, "(lt_%li)\n", t->unique_counter);

            t->unique_counter++;
        } break;

        case OP_TYPE_GT: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");

            dec_stack_pointer(t->f);

            jump_to_top_value(t->f);
            fprintf(t->f, "D=M-D\n");

            fprintf(t->f, "M=0\n");
            fprintf(t->f, "@gt_%li\n", t->unique_counter);
            fprintf(t->f, "D;JLE\n");

            jump_to_top_value(t->f);
            fprintf(t->f, "M=-1\n");
            fprintf(t->f, "(gt_%li)\n", t->unique_counter);

            t->unique_counter++;
        } break;

        case OP_TYPE_NEG: {
            jump_to_top_value(t->f);
            fprintf(t->f, "M=-M\n");
        } break;

        case OP_TYPE_AND: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");

            dec_stack_pointer(t->f);

            jump_to_top_value(t->f);
            fprintf(t->f, "M=D&M\n");
        } break;

        case OP_TYPE_OR: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");

            dec_stack_pointer(t->f);

            jump_to_top_value(t->f);
            fprintf(t->f, "M=D|M\n");
        } break;

        case OP_TYPE_NOT: {
            jump_to_top_value(t->f);
            fprintf(t->f, "M=!M\n");
        } break;

        case OP_TYPE_UNINIT: assert(0);
    }
}
