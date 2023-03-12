#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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

struct instruction {
    void *ptr;
};

int parse_instruction(struct instruction *inst)
{
    (void) inst;
    return 0;
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

    for (;;) {
        if (parse_instruction(&inst) == 0) {
            break;
        }
    }

    free(src);
    return 0;
}
