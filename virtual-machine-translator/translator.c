#include <assert.h>
#include "translator.h"

#define STATIC_BASE_ADDR 16
#define STATIC_MAX_ADDR 255

#define TEMP_BASE_ADDR 5
#define TEMP_MAX_ADDR 12

static size_t unique_count = 0;

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

void translator_translate_inst(Instruction *inst, FILE *f)
{
    switch (inst->op_type) {
        case OP_TYPE_PUSH:
        case OP_TYPE_POP: {
            switch (inst->mem_seg_type) {
                case MEM_SEG_TYPE_LCL: {
                    fprintf(f, "@LCL\n");
                    fprintf(f, "D=M\n");
                    fprintf(f, "@%li\n", inst->mem_addr);

                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(f, "A=D+A\n");
                        fprintf(f, "D=M\n");

                        jump_to_stack_pointer(f);
                        fprintf(f, "M=D\n");
                        inc_stack_pointer(f);
                    } else {
                        fprintf(f, "D=D+A\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "M=D\n");

                        dec_stack_pointer(f);
                        jump_to_stack_pointer(f);

                        fprintf(f, "D=M\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "A=M\n");

                        fprintf(f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_ARG: {
                    fprintf(f, "@ARG\n");
                    fprintf(f, "D=M\n");
                    fprintf(f, "@%li\n", inst->mem_addr);

                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(f, "A=D+A\n");
                        fprintf(f, "D=M\n");

                        jump_to_stack_pointer(f);
                        fprintf(f, "M=D\n");
                        inc_stack_pointer(f);
                    } else {
                        fprintf(f, "D=D+A\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "M=D\n");

                        dec_stack_pointer(f);
                        jump_to_stack_pointer(f);

                        fprintf(f, "D=M\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "A=M\n");

                        fprintf(f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_THIS: {
                    fprintf(f, "@THIS\n");
                    fprintf(f, "D=M\n");
                    fprintf(f, "@%li\n", inst->mem_addr);

                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(f, "A=D+A\n");
                        fprintf(f, "D=M\n");

                        jump_to_stack_pointer(f);
                        fprintf(f, "M=D\n");
                        inc_stack_pointer(f);
                    } else {
                        fprintf(f, "D=D+A\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "M=D\n");

                        dec_stack_pointer(f);
                        jump_to_stack_pointer(f);

                        fprintf(f, "D=M\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "A=M\n");

                        fprintf(f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_THAT: {
                    fprintf(f, "@THAT\n");
                    fprintf(f, "D=M\n");
                    fprintf(f, "@%li\n", inst->mem_addr);

                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(f, "A=D+A\n");
                        fprintf(f, "D=M\n");

                        jump_to_stack_pointer(f);
                        fprintf(f, "M=D\n");
                        inc_stack_pointer(f);
                    } else {
                        fprintf(f, "D=D+A\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "M=D\n");

                        dec_stack_pointer(f);
                        jump_to_stack_pointer(f);

                        fprintf(f, "D=M\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "A=M\n");

                        fprintf(f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_CONST: {
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(f, "@%li\n", inst->mem_addr);
                        fprintf(f, "D=A\n");

                        jump_to_stack_pointer(f);
                        fprintf(f, "M=D\n");
                        inc_stack_pointer(f);
                    }
                } break;

                case MEM_SEG_TYPE_STATIC: {
                    if (inst->op_type == OP_TYPE_PUSH) {
                        // @TODO: change for file name
                        fprintf(f, "@Foo.%li\n", inst->mem_addr);
                        fprintf(f, "D=M\n");

                        jump_to_stack_pointer(f);
                        fprintf(f, "M=D\n");
                        inc_stack_pointer(f);
                    } else {
                        dec_stack_pointer(f);
                        jump_to_stack_pointer(f);

                        fprintf(f, "D=M\n");
                        // @TODO: change for file name
                        fprintf(f, "@Foo.%li\n", inst->mem_addr);
                        fprintf(f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_TEMP: {
                    size_t mem_addr = inst->mem_addr + TEMP_BASE_ADDR;
                    assert(mem_addr <= TEMP_MAX_ADDR);

                    fprintf(f, "@%li\n", mem_addr);
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(f, "D=M\n");

                        jump_to_stack_pointer(f);
                        fprintf(f, "M=D\n");
                        inc_stack_pointer(f);
                    } else {
                        fprintf(f, "D=A\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "M=D\n");

                        dec_stack_pointer(f);
                        jump_to_stack_pointer(f);

                        fprintf(f, "D=M\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "A=M\n");

                        fprintf(f, "M=D\n");
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

                    fprintf(f, "@%s\n", mem_addr);
                    if (inst->op_type == OP_TYPE_PUSH) {
                        fprintf(f, "D=M\n");

                        jump_to_stack_pointer(f);
                        fprintf(f, "M=D\n");
                        inc_stack_pointer(f);
                    } else {
                        fprintf(f, "D=A\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "M=D\n");

                        dec_stack_pointer(f);
                        jump_to_stack_pointer(f);

                        fprintf(f, "D=M\n");
                        fprintf(f, "@R15\n");
                        fprintf(f, "A=M\n");

                        fprintf(f, "M=D\n");
                    }
                } break;

                case MEM_SEG_TYPE_UNINIT: assert(0);
            }
        } break;

        case OP_TYPE_ADD: {
            fprintf(f, "// ADD BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "D=M\n");
            dec_stack_pointer(f);
            jump_to_top_value(f);
            fprintf(f, "M=D+M\n");

            fprintf(f, "// ADD END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_SUB: {
            fprintf(f, "// SUB BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "D=M\n");
            dec_stack_pointer(f);
            jump_to_top_value(f);
            fprintf(f, "M=M-D\n");

            fprintf(f, "// SUB END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_EQ: {
            fprintf(f, "// EQ BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "D=M\n");

            dec_stack_pointer(f);

            jump_to_top_value(f);
            fprintf(f, "D=M-D\n");

            fprintf(f, "M=0\n");
            fprintf(f, "@eq_%li\n", unique_count);
            fprintf(f, "D;JNE\n");

            jump_to_top_value(f);
            fprintf(f, "M=-1\n");
            fprintf(f, "(eq_%li)\n", unique_count);

            fprintf(f, "// EQ END\n");
            fprintf(f, "\n");

            unique_count++;
        } break;

        case OP_TYPE_LT: {
            fprintf(f, "// LT BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "D=M\n");

            dec_stack_pointer(f);

            jump_to_top_value(f);
            fprintf(f, "D=M-D\n");

            fprintf(f, "M=0\n");
            fprintf(f, "@lt_%li\n", unique_count);
            fprintf(f, "D;JGE\n");

            jump_to_top_value(f);
            fprintf(f, "M=-1\n");
            fprintf(f, "(lt_%li)\n", unique_count);

            fprintf(f, "// LT END\n");
            fprintf(f, "\n");

            unique_count++;
        } break;

        case OP_TYPE_GT: {
            fprintf(f, "// GT BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "D=M\n");

            dec_stack_pointer(f);

            jump_to_top_value(f);
            fprintf(f, "D=M-D\n");

            fprintf(f, "M=0\n");
            fprintf(f, "@gt_%li\n", unique_count);
            fprintf(f, "D;JLE\n");

            jump_to_top_value(f);
            fprintf(f, "M=-1\n");
            fprintf(f, "(gt_%li)\n", unique_count);

            fprintf(f, "// GT END\n");
            fprintf(f, "\n");

            unique_count++;
        } break;

        case OP_TYPE_NEG: {
            fprintf(f, "// NEG BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "M=-M\n");

            fprintf(f, "// NEG END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_AND: {
            fprintf(f, "// AND BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "D=M\n");

            dec_stack_pointer(f);

            jump_to_top_value(f);
            fprintf(f, "M=D&M\n");

            fprintf(f, "// AND END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_OR: {
            fprintf(f, "// OR BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "D=M\n");

            dec_stack_pointer(f);

            jump_to_top_value(f);
            fprintf(f, "M=D|M\n");

            fprintf(f, "// OR END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_NOT: {
            fprintf(f, "// NOT BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "M=!M\n");

            fprintf(f, "// NOT END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_UNINIT: assert(0);
    }
}
