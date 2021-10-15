#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

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
            strtok(line, "//");
        }

        trim_right(line);

        if (strlen(line) == 0) continue;

        printf("%s\n", line);
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
