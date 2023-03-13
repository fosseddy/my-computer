#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "parser.h"
#include "symtable.h"

struct symtable symtable;
struct parser parser;

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

void populate_symtable()
{
    struct command cmd;
    size_t line_count = 0;

    for (;;) {
        memset(&cmd, 0, sizeof(cmd));
        if (parse_command(&parser, &cmd) == 0) {
            break;
        }

        if (cmd.kind == COMMAND_L) {
            symtable_put(&symtable, cmd.symbols, line_count);
        } else {
            line_count++;
        }
    }

    parser_reset(&parser);
}

void compile()
{
    struct command cmd;
    //size_t var_addr_start = 16;

    for (;;) {
        memset(&cmd, 0, sizeof(cmd));
        if (parse_command(&parser, &cmd) == 0) {
            break;
        }
    }

    parser_reset(&parser);
}

int main(int argc, char **argv)
{
    --argc;
    ++argv;

    if (argc < 1) {
        print_usage(stderr, "missing file to compile");
        return 1;
    }

    parser.src = read_file(*argv);
    parser.len = strlen(parser.src);
    parser.cur = 0;

    symtable_init(&symtable);
    populate_symtable();
    compile();

    free(parser.src);
    return 0;
}
