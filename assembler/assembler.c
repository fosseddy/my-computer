#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv) {
    // @TODO: proper error handle
    assert(argc > 1);

    // @TODO: validate file
    char *file_name = argv[1];
    FILE *f = fopen(file_name, "r");

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        // trim left
        while (isspace(*line)) {
            line++;
        }

        // line includes comment
        if (strstr(line, "//") != NULL) {
            // whole line is comment
            if (strncmp(line, "//", 2) == 0) continue;
            // line has inline comment
            strtok(line, "//");
        }

        // trim right
        char *end = line + strlen(line) - 1;
        while (isspace(*end)) {
            end--;
        }
        end[1] = '\0';

        if (strlen(line) == 0) continue;

        printf("%s\n", line);
    }

    fclose(f);

    return 0;
}
