#include <assert.h>
#include "translator.h"

static size_t unique_count = 0;

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
            // a + b
            fprintf(f, "// ADD BEGIN\n");
            // D = b
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            fprintf(f, "D=M\n");
            // SP--
            fprintf(f, "@SP\n");
            fprintf(f, "M=M-1\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // M = b + a
            fprintf(f, "M=D+M\n");
            fprintf(f, "// ADD END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_SUB: {
            // a - b
            fprintf(f, "// SUB BEGIN\n");
            // D = b
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            fprintf(f, "D=M\n");
            // SP--
            fprintf(f, "@SP\n");
            fprintf(f, "M=M-1\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // M = a - b
            fprintf(f, "M=M-D\n");
            fprintf(f, "// SUB END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_EQ: {
            // a == b
            fprintf(f, "// EQ BEGIN\n");
            // D = b
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            fprintf(f, "D=M\n");
            // SP--
            fprintf(f, "@SP\n");
            fprintf(f, "M=M-1\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // D = a - b
            fprintf(f, "D=M-D\n");
            // M = 0 (a not equal to b)
            fprintf(f, "M=0\n");
            // if (D == 0) M=1
            fprintf(f, "@eq_%li\n", unique_count);
            fprintf(f, "D;JNE\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // M = 1 (a equals to b)
            fprintf(f, "M=-1\n");
            fprintf(f, "(eq_%li)\n", unique_count);
            fprintf(f, "// EQ END\n");
            fprintf(f, "\n");

            unique_count++;
        } break;

        case OP_TYPE_LT: {
            // a < b
            fprintf(f, "// LT BEGIN\n");
            // D = b
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            fprintf(f, "D=M\n");
            // SP--
            fprintf(f, "@SP\n");
            fprintf(f, "M=M-1\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // D = a - b
            fprintf(f, "D=M-D\n");
            // M = 0 (a not less then b)
            fprintf(f, "M=0\n");
            // if (D < 0) M=1
            fprintf(f, "@lt_%li\n", unique_count);
            fprintf(f, "D;JGE\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // M = 1 (a less then b)
            fprintf(f, "M=-1\n");
            fprintf(f, "(lt_%li)\n", unique_count);
            fprintf(f, "// LT END\n");
            fprintf(f, "\n");

            unique_count++;
        } break;

        case OP_TYPE_GT: {
            // a > b
            fprintf(f, "// GT BEGIN\n");
            // D = b
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            fprintf(f, "D=M\n");
            // SP--
            fprintf(f, "@SP\n");
            fprintf(f, "M=M-1\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // D = a - b
            fprintf(f, "D=M-D\n");
            // M = 0 (a not greater then b)
            fprintf(f, "M=0\n");
            // if (D < 0) M=1
            fprintf(f, "@gt_%li\n", unique_count);
            fprintf(f, "D;JLE\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // M = 1 (a greater then b)
            fprintf(f, "M=-1\n");
            fprintf(f, "(gt_%li)\n", unique_count);
            fprintf(f, "// GT END\n");
            fprintf(f, "\n");

            unique_count++;
        } break;

        case OP_TYPE_NEG: {
            // a = -a
            fprintf(f, "// NEG BEGIN\n");
            // M = -a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            fprintf(f, "M=-M\n");
            fprintf(f, "// NEG END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_AND: {
            // a & b
            fprintf(f, "// AND BEGIN\n");
            // D = b
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            fprintf(f, "D=M\n");
            // SP--
            fprintf(f, "@SP\n");
            fprintf(f, "M=M-1\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // M = b & a
            fprintf(f, "M=D&M\n");
            fprintf(f, "// AND END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_OR: {
            // a | b
            fprintf(f, "// OR BEGIN\n");
            // D = b
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            fprintf(f, "D=M\n");
            // SP--
            fprintf(f, "@SP\n");
            fprintf(f, "M=M-1\n");
            // M = a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
            // M = b & a
            fprintf(f, "M=D|M\n");
            fprintf(f, "// OR END\n");
            fprintf(f, "\n");
        } break;

        case OP_TYPE_NOT:
            // !a
            fprintf(f, "// NOT BEGIN\n");
            // M = !a
            fprintf(f, "@SP\n");
            fprintf(f, "A=M-1\n");
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
