#include <string.h>
#include <assert.h>
#include "translator.h"
#include "path.h"

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

static void translate_push_pop_for(char *mem_seg, Instruction *inst, FILE *f)
{
    fprintf(f, "@%s\n", mem_seg);
    fprintf(f, "D=M\n");
    fprintf(f, "@%li\n", inst->mem_offset);
    if (inst->op_kind == OP_KIND_PUSH) {
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
}

Translator make_translator(const char *file_path)
{
    FILE *file = fopen(file_path, "w");
    assert (file != NULL);

    Translator t = {
        .f = file,
        .unique_counter = 0
    };

    char file_name[256];
    path_get_file_name(file_name, file_path);
    strcpy((char *) t.file_name, file_name);

    return t;
}

void free_translator(Translator *t)
{
    fclose(t->f);
}

void translator_translate_inst(Translator *t, Instruction *inst)
{
    switch (inst->op_kind) {
        case OP_KIND_PUSH:
        case OP_KIND_POP: {
            switch (inst->mem_seg_kind) {
                case MEM_SEG_KIND_LCL: {
                    translate_push_pop_for("LCL", inst, t->f);
                } break;

                case MEM_SEG_KIND_ARG: {
                    translate_push_pop_for("ARG", inst, t->f);
                } break;

                case MEM_SEG_KIND_THIS: {
                    translate_push_pop_for("THIS", inst, t->f);
                } break;

                case MEM_SEG_KIND_THAT: {
                    translate_push_pop_for("THAT", inst, t->f);
                } break;

                case MEM_SEG_KIND_CONST: {
                    if (inst->op_kind == OP_KIND_PUSH) {
                        fprintf(t->f, "@%li\n", inst->mem_offset);
                        fprintf(t->f, "D=A\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        assert(0);
                    }
                } break;

                case MEM_SEG_KIND_STATIC: {
                    if (inst->op_kind == OP_KIND_PUSH) {
                        // @TODO: change for file name
                        fprintf(t->f, "@%s.%li\n", t->file_name, inst->mem_offset);
                        fprintf(t->f, "D=M\n");

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "M=D\n");

                        inc_stack_pointer(t->f);
                    } else {
                        dec_stack_pointer(t->f);

                        jump_to_stack_pointer(t->f);
                        fprintf(t->f, "D=M\n");

                        // @TODO: change for file name
                        fprintf(t->f, "@%s.%li\n", t->file_name, inst->mem_offset);
                        fprintf(t->f, "M=D\n");
                    }
                } break;

                case MEM_SEG_KIND_TEMP: {
                    size_t mem_addr = inst->mem_offset + TEMP_BASE_ADDR;
                    assert(mem_addr <= TEMP_MAX_ADDR);

                    fprintf(t->f, "@%li\n", mem_addr);
                    if (inst->op_kind == OP_KIND_PUSH) {
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

                case MEM_SEG_KIND_POINTER: {
                    assert(inst->mem_offset == 0 || inst->mem_offset == 1);

                    char *mem_addr = NULL;
                    if (inst->mem_offset == 1) {
                        mem_addr = "THAT";
                    } else {
                        mem_addr = "THIS";
                    }

                    fprintf(t->f, "@%s\n", mem_addr);
                    if (inst->op_kind == OP_KIND_PUSH) {
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
            }
        } break;

        case OP_KIND_LABEL:
        case OP_KIND_GOTO:
        case OP_KIND_IF:
        case OP_KIND_FUNC:
        case OP_KIND_CALL:
        case OP_KIND_RET:
            break;

        case OP_KIND_ADD: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");
            dec_stack_pointer(t->f);
            jump_to_top_value(t->f);
            fprintf(t->f, "M=D+M\n");
        } break;

        case OP_KIND_SUB: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");
            dec_stack_pointer(t->f);
            jump_to_top_value(t->f);
            fprintf(t->f, "M=M-D\n");
        } break;

        case OP_KIND_EQ: {
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

        case OP_KIND_LT: {
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

        case OP_KIND_GT: {
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

        case OP_KIND_NEG: {
            jump_to_top_value(t->f);
            fprintf(t->f, "M=-M\n");
        } break;

        case OP_KIND_AND: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");

            dec_stack_pointer(t->f);

            jump_to_top_value(t->f);
            fprintf(t->f, "M=D&M\n");
        } break;

        case OP_KIND_OR: {
            jump_to_top_value(t->f);
            fprintf(t->f, "D=M\n");

            dec_stack_pointer(t->f);

            jump_to_top_value(t->f);
            fprintf(t->f, "M=D|M\n");
        } break;

        case OP_KIND_NOT: {
            jump_to_top_value(t->f);
            fprintf(t->f, "M=!M\n");
        } break;
    }
}
