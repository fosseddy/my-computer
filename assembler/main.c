#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "parser.h"
#include "symtable.h"
#include "translate.h"

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

char *read_file(char *pathname)
{
    int fd;
    struct stat statbuf;
    char *src;

    fd = open(pathname, O_RDONLY);
    if (fd < 0) {
        perror("failed to open file");
        exit(1);
    }

    if (fstat(fd, &statbuf) < 0) {
        perror("failed to get file info");
        exit(1);
    }

    src = malloc(statbuf.st_size + 1);
    if (src == NULL) {
        perror("failed to allocate memory for source");
        exit(1);
    }

    if (read(fd, src, statbuf.st_size) < 0) {
        perror("failed read file content into source");
        exit(1);
    }

    src[statbuf.st_size] = '\0';

    close(fd);
    return src;
}

void populate_symtable(struct symtable *st, char *src)
{
    struct parser p;
    struct command cmd;
    int line_count = 0;

    parser_init(&p, src);

    for (;;) {
        memset(&cmd, 0, sizeof(cmd));
        if (parse_command(&p, &cmd) == 0) {
            break;
        }

        if (cmd.kind == COMMAND_L) {
            symtable_put(st, cmd.symbols, line_count);
        } else {
            line_count++;
        }
    }
}

void translate(char *src, struct symtable *st)
{
    FILE *out;
    struct parser p;
    struct command cmd;
    char binary[17];

    out = fopen("out.hack", "w");
    if (out == NULL) {
        perror("failed to create output file");
        exit(1);
    }

    parser_init(&p, src);

    for (;;) {
        memset(&binary, 0, sizeof(binary));
        memset(&cmd, 0, sizeof(cmd));

        if (parse_command(&p, &cmd) == 0) {
            break;
        }

        switch (cmd.kind) {
        case COMMAND_A:
            translate_a(&cmd, binary, sizeof(binary), st);
            fprintf(out, "%s\n", binary);
            break;

        case COMMAND_C:
            translate_c(&cmd, binary);
            fprintf(out, "%s\n", binary);
            break;

        case COMMAND_L: break;
        }
    }

    fclose(out);
}

int main(int argc, char **argv)
{
    struct symtable st;
    char *src;

    argc--;
    argv++;

    if (argc < 1) {
        print_usage(stderr, "missing file to compile");
        return 1;
    }

    src = read_file(*argv);
    symtable_init(&st);

    populate_symtable(&st, src);
    translate(src, &st);

    symtable_free(&st);
    free(src);
    return 0;
}
