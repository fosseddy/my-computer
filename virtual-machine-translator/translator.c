#include <assert.h>
#include "translator.h"

static size_t unique_count = 0;

static void jump_to_top_value(FILE *f)
{
    fprintf(f, "@SP\n");
    fprintf(f, "A=M-1\n");
}

static void dec_stack_pointer(FILE *f)
{
    fprintf(f, "@SP\n");
    fprintf(f, "M=M-1\n");
}

void translator_translate_inst(Instruction *inst, FILE *f)
{
    switch (inst->op_type) {
        case OP_TYPE_PUSH: {
            fprintf(f, "// PUSH BEGIN\n");
            fprintf(f, "@%li\n", inst->mem_addr);
            fprintf(f, "D=A\n");
            fprintf(f, "@SP\n");
            fprintf(f, "A=M\n");
            fprintf(f, "M=D\n");
            fprintf(f, "@SP\n");
            fprintf(f, "M=M+1\n");
            fprintf(f, "// PUSH END\n");
            fprintf(f, "\n");
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

            // M = 0 (a not equal to b)
            fprintf(f, "M=0\n");

            // if (D == 0) M=1
            fprintf(f, "@eq_%li\n", unique_count);
            fprintf(f, "D;JNE\n");

            // M = 1 (a equals to b)
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

            // M = 0 (a not less then b)
            fprintf(f, "M=0\n");

            // if (D < 0) M=1
            fprintf(f, "@lt_%li\n", unique_count);
            fprintf(f, "D;JGE\n");

            // M = 1 (a less then b)
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

            // M = 0 (a not greater then b)
            fprintf(f, "M=0\n");

            // if (D < 0) M=1
            fprintf(f, "@gt_%li\n", unique_count);
            fprintf(f, "D;JLE\n");

            // M = 1 (a greater then b)
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

        case OP_TYPE_NOT:
            fprintf(f, "// NOT BEGIN\n");

            jump_to_top_value(f);
            fprintf(f, "M=!M\n");

            fprintf(f, "// NOT END\n");
            fprintf(f, "\n");

        case OP_TYPE_POP:
            break;

        case OP_TYPE_UNINIT:
            assert(0);
            break;
    }
}
