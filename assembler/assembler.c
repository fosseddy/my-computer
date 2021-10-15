#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define MAX_INST_SIZE 4
#define MAX_LABEL_SIZE 101

typedef enum {
    INST_A = 0,
    INST_C,
    INST_L
} Inst_type;

typedef struct {
    Inst_type type;
    char dest[MAX_INST_SIZE];
    char comp[MAX_INST_SIZE];
    char jump[MAX_INST_SIZE];
    char label[MAX_LABEL_SIZE];
} Inst;

void trim_left(char *s);
void trim_right(char *s);

int main(int argc, char **argv) {
    // @TODO: proper error handle
    assert(argc > 1);

    // @TODO: validate file
    char *file_name = argv[1];
    FILE *f = fopen(file_name, "r");

    // @TODO: proper error handle
    assert(f != NULL);

    char *line = NULL;
    size_t line_size = 0;
    while (getline(&line, &line_size, f) != -1) {
        trim_left(line);

        // line includes comment
        if (strstr(line, "//") != NULL) {
            // whole line is comment
            if (strncmp(line, "//", 2) == 0) continue;
            // line has inline comment
            char line_copy[strlen(line)];
            strcpy(line_copy, line);
            char *line_with_no_comment = strtok(line_copy, "//");
            strcpy(line, line_with_no_comment);
        }

        trim_right(line);

        if (strlen(line) == 0) continue;

        Inst inst = {
            .type = -1,
            .label = "",
            .dest = "",
            .comp = "",
            .jump = ""
        };

        printf("%15s | ", line);

        // instruction A starts with @
        if (strncmp(line, "@", 1) == 0) {
            inst.type = INST_A;

            assert(strlen(line) - 1 < MAX_LABEL_SIZE);
            for (int i = 1; i <= strlen(line); ++i) {
                inst.label[i - 1] = line[i];
            }
        // instruction L starts with '(' and ends with ')'
        } else if (strncmp(line, "(", 1) == 0) {
            inst.type = INST_L;

            assert(strlen(line) - 2 < MAX_LABEL_SIZE);
            // remove '(' and ')' from label: (LABEL_NAME) -> LABEL_NAME
            for (int i = 1; i < strlen(line) - 1; ++i) {
                inst.label[i - 1] = line[i];
            }
            inst.label[strlen(line) - 2] = '\0';
        } else {
            inst.type = INST_C;

            // if instruction C includes '=', then extract dest
            if (strchr(line, '=') != NULL) {
                char line_copy[strlen(line)];
                strcpy(line_copy, line);

                char *dest = strtok(line_copy, "=");

                assert(strlen(dest) < MAX_INST_SIZE);
                strcpy(inst.dest, dest);

                char *next_tok = strtok(NULL, "=");
                strcpy(line, next_tok);
            }

            // if instruction C includes ';', then extract jump
            if (strchr(line, ';') != NULL) {
                char line_copy[strlen(line)];
                strcpy(line_copy, line);

                char *comp = strtok(line_copy, ";");
                strcpy(line, comp);

                char *jump = strtok(NULL, ";");

                assert(strlen(jump) < MAX_INST_SIZE);
                strcpy(inst.jump, jump);
            }

            assert(strlen(line) < MAX_INST_SIZE);
            strcpy(inst.comp, line);
        }

        assert(inst.type != -1);
        printf("type: %d label: %15s dest: %3s comp: %3s jmp: %3s\n",
                inst.type, inst.label, inst.dest, inst.comp, inst.jump);
    }

    free(line);
    fclose(f);

    return 0;
}

void trim_left(char *s) {
    int pad = 0;
    int len = strlen(s);

    for (int i = 0; i < len; ++i) {
        if (isspace(s[i])) {
            pad++;
        } else {
            break;
        }
    }

    if (pad > 0) {
        for (int i = pad; i <= len; ++i) {
            s[i - pad] = s[i];
        }
    }
}

void trim_right(char *s) {
    int len = strlen(s);

    for (int i = len - 1; i >= 0; --i) {
        if (!isspace(s[i])) {
            s[i + 1] = '\0';
            break;
        }
    }
}
