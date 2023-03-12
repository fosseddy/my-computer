#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

void print_usage(FILE *stream, char *msg)
{
    if (msg != NULL) {
        fprintf(stream, "%s\n\n", msg);
    }

    fprintf(stream, "Usage: asm FILE\n"
                    "Compiles hack assembly program into "
                    "hack machine language\n"
                    "\n"
                    "FILE:\n"
                    "   path to assembly file\n");
}

char *read_file(char *filepath)
{
    FILE *f;
    char *src;
    struct stat statbuf;

    f = fopen(filepath, "r");
    if (f == NULL) return NULL;

    if (stat(filepath, &statbuf) < 0) return NULL;

    src = malloc(statbuf.st_size);
    if (src == NULL) return NULL;

    fread(src, 1, statbuf.st_size, f);
    return src;
}

struct instruction {
    void *ptr;
};

int parse_instruction(struct instruction *inst)
{
    (void) inst;
    return 1;
}

int main(int argc, char **argv)
{
    char *src;
    struct instruction inst;

    --argc;
    ++argv;

    if (argc < 1) {
        print_usage(stderr, "missing file to compile");
        return 1;
    }

    src = read_file(*argv);
    if (src == NULL) {
        perror("failed to read file");
        return 1;
    }

    for (;;) {
        if (parse_instruction(&inst) == 0) break;
    }

    return 0;
}
