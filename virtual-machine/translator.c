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

static void translate_predefined_push_pop(char *mem_seg,
                                          struct Instruction *inst,
                                          FILE *f)
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

static void translate_const_push_pop(struct Instruction *inst, FILE *f)
{
    if (inst->op_kind == OP_KIND_PUSH) {
        fprintf(f, "@%li\n", inst->mem_offset);
        fprintf(f, "D=A\n");

        jump_to_stack_pointer(f);
        fprintf(f, "M=D\n");

        inc_stack_pointer(f);
    } else {
        assert(0 && "Can not use pop on constant segment");
    }
}

static void translate_static_push_pop(struct Instruction *inst,
                                      struct Translator *t)
{
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
}

static void translate_temp_push_pop(struct Instruction *inst, FILE *f)
{
    size_t mem_addr = inst->mem_offset + TEMP_BASE_ADDR;
    assert(mem_addr <= TEMP_MAX_ADDR);

    fprintf(f, "@%li\n", mem_addr);
    if (inst->op_kind == OP_KIND_PUSH) {
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
}

static void translate_pointer_push_pop(struct Instruction *inst, FILE *f)
{
    assert(inst->mem_offset == 0 || inst->mem_offset == 1);

    char *mem_addr = NULL;
    if (inst->mem_offset == 1) {
        mem_addr = "THAT";
    } else {
        mem_addr = "THIS";
    }

    fprintf(f, "@%s\n", mem_addr);
    if (inst->op_kind == OP_KIND_PUSH) {
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
}

static void translate_label(char *label, FILE *f)
{
    fprintf(f, "(%s)\n", label);
}

static void translate_goto(char *label, FILE *f)
{
    fprintf(f, "@%s\n", label);
    fprintf(f, "0;JMP\n");
}

static void translate_if(char *label, FILE *f)
{

    jump_to_top_value(f);
    fprintf(f, "D=M\n");

    dec_stack_pointer(f);

    fprintf(f, "@%s\n", label);
    fprintf(f, "D;JNE\n");
}

//static void translate_call(struct Instruction *t, FILE *f)
//{
//    fprintf(f, "@%li\n", t->func_args_num);
//    fprintf(f, "D=A\n");
//
//    jump_to_stack_pointer(f);
//    // Save caller's stack frame
//
//    // set ARG
//    fprintf(f, "D=A-D\n");
//    fprintf(f, "@ARG\n");
//    fprintf(f, "M=D\n");
//
//    // Jumps to function lable
//}

struct Translator make_translator(const char *file_path)
{
    FILE *file = fopen(file_path, "w");
    assert (file != NULL);

    struct Translator t = {
        .f = file,
        .unique_counter = 0
    };

    char file_name[256];
    path_get_file_name(file_name, file_path);
    strcpy((char *) t.file_name, file_name);

    return t;
}

void free_translator(struct Translator *t)
{
    fclose(t->f);
}

void translator_translate_inst(struct Translator *t, struct Instruction *inst)
{
    switch (inst->op_kind) {
        case OP_KIND_PUSH:
        case OP_KIND_POP: {
            switch (inst->mem_seg_kind) {
                case MEM_SEG_KIND_LCL:
                    translate_predefined_push_pop("LCL", inst, t->f);
                    break;

                case MEM_SEG_KIND_ARG:
                    translate_predefined_push_pop("ARG", inst, t->f);
                    break;

                case MEM_SEG_KIND_THIS:
                    translate_predefined_push_pop("THIS", inst, t->f);
                    break;

                case MEM_SEG_KIND_THAT:
                    translate_predefined_push_pop("THAT", inst, t->f);
                    break;

                case MEM_SEG_KIND_CONST:
                    translate_const_push_pop(inst, t->f);
                    break;

                case MEM_SEG_KIND_STATIC:
                    translate_static_push_pop(inst, t);
                    break;

                case MEM_SEG_KIND_TEMP:
                    translate_temp_push_pop(inst, t->f);
                    break;

                case MEM_SEG_KIND_POINTER:
                    translate_pointer_push_pop(inst, t->f);
                    break;

                default:
                    assert(0 && "Unreachable");
            }
        } break;

        case OP_KIND_LABEL:
            translate_label(inst->label, t->f);
            break;

        case OP_KIND_GOTO:
            translate_goto(inst->label, t->f);
            break;

        case OP_KIND_IF:
            translate_if(inst->label, t->f);
            break;

        case OP_KIND_FUNC:
            break;

        case OP_KIND_CALL:
//            translate_call();
            break;

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
