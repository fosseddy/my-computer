#include <string.h>
#include <assert.h>

#include "parser.h"

static int has_src(struct parser *p)
{
    return p->cur < p->len;
}

static int eol(struct parser *p)
{
    return p->src[p->cur] == '\r' || p->src[p->cur] == '\n';
}

static void parse_line(struct parser *p, char *line)
{
    int has_line = 0,
        i = 0;

    while (has_line == 0 && has_src(p) == 1) {
        switch (p->src[p->cur]) {
        case '\t':
        case ' ':
            p->cur++;
            break;

        case '\r':
            p->cur += 2;
            has_line = 1;
            break;

        case '\n':
            p->cur++;
            has_line = 1;
            break;

        case '/':
            while (eol(p) == 0 && has_src(p) == 1) p->cur++;
            break;

        default:
            assert(i < 128);
            line[i++] = p->src[p->cur++];
            break;
        }
    }
}

int parse_command(struct parser *p, struct command *cmd)
{
    char line[128];
    size_t i, j;

parse_again:
    memset(line, 0, sizeof(line));
    parse_line(p, line);

    switch (line[0]) {
    case '\0':
        if (has_src(p) == 0) {
            return 0;
        }
        goto parse_again;

    case '(':
        cmd->kind = COMMAND_L;
        assert(strlen(line) - 2 < 128);
        memcpy(cmd->symbols, line + 1, strlen(line) - 2);
        break;

    case '@':
        cmd->kind = COMMAND_A;
        assert(strlen(line) - 1 < 128);
        memcpy(cmd->symbols, line + 1, strlen(line) - 1);
        break;

    default:
        cmd->kind = COMMAND_C;

        i = 0;
        j = 0;
        while (line[i] != '\0') {
            if (line[i] == '=') {
                assert(j < 4);
                memcpy(cmd->dest, cmd->comp, j);
                memset(cmd->comp, 0, sizeof(cmd->comp));
                i++;
                j = 0;
                continue;
            }

            if (line[i] == ';') {
                i++;
                j = 0;
                while (line[i] != '\0') {
                    assert(j < 4);
                    cmd->jump[j++] = line[i++];
                }
                break;
            }

            assert(j < 4);
            cmd->comp[j++] = line[i++];
        }

        break;
    }

    return 1;
}

void parser_reset(struct parser *p)
{
    p->cur = 0;
}
