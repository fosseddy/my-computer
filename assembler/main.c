#include <stdio.h>
#include <sys/stat.h>

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

int main(int argc, char **argv)
{
    FILE *infile;
    struct stat statbuf;

    --argc;
    ++argv;

    if (argc < 1) {
        print_usage(stderr, "missing file to compile");
        return 1;
    }

    infile = fopen(*argv, "r");
    if (infile == NULL) {
        perror("failed to open file");
        return 1;
    }

    if (stat(*argv, &statbuf) < 0) {
        perror("failed to get info about file");
        return 1;
    }

    printf("file size is %d\n", statbuf.st_size);

    return 0;
}
