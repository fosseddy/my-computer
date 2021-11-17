#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define KEYWORDS_LEN 21
static const char * const keywords[KEYWORDS_LEN] = {
    "class",   "constructor", "function",
    "method",  "field",       "static",
    "var",     "int",         "char",
    "boolean", "void",        "true",
    "false",   "null",        "this",
    "let",     "do",          "if",
    "else",    "while",       "return"
};

#define SYMBOLS_LEN 19
static const char symbols[SYMBOLS_LEN] = {
    '{', '}',
    '(', ')',
    '[', ']',
    '.', ',', ';',
    '+', '-', '*', '/',
    '&', '|', '<', '>', '=', '~'
};

enum Token_Kind {
    TOKEN_KIND_UNINIT,
    TOKEN_KIND_KEYWORD,
    TOKEN_KIND_SYMBOL,
    TOKEN_KIND_INT_CONST,
    TOKEN_KIND_STR_CONST,
    TOKEN_KIND_IDENTIFIER,
};

struct Token {
    enum Token_Kind kind;
    char *value;
};

struct Lexer {
    FILE *file;
    bool has_tokens;
    struct Token *token;
};

void lexer_advance(struct Lexer *lex);

struct Lexer *make_lexer(const char *file_name)
{
    FILE *f = fopen(file_name, "r");
    assert(f != NULL);

    struct Lexer *lex = malloc(sizeof(struct Lexer));
    assert(lex != NULL);

    lex->file = f;
    lex->has_tokens = true;
    lex->token = NULL;

    return lex;
}

void free_lexer(struct Lexer *lex)
{
    fclose(lex->file);
    free(lex);
}

#define MAX_VALUE_LEN 80
void lexer_advance(struct Lexer *lex)
{
    while (lex->has_tokens) {
        char value[MAX_VALUE_LEN] = {0};
        size_t i = 0;
        char c = '\0';

        for (;;) {
            c = fgetc(lex->file);

            if (feof(lex->file) || isspace(c)) break;

            if (c == '/') {
                char next = fgetc(lex->file);
                // line comment
                if (next == '/') {
                    while (c != '\n' && !feof(lex->file)) {
                        c = fgetc(lex->file);
                    }
                    continue;
                // doc or multiline comment
                } else if (next == '*') {
                    bool found_closing = false;
                    while (!feof(lex->file)) {
                        c = fgetc(lex->file);
                        if (c == '*' || c == '/') {
                            value[i] = c;
                            i++;
                        }

                        if (strcmp(value, "*/") == 0 ||
                                strcmp(value, "**/") == 0) {
                            found_closing = true;
                            break;
                        }
                    }
                    assert(found_closing);
                    memset(value, 0, MAX_VALUE_LEN);
                    i = 0;
                    continue;
                // it is going to be symbol
                } else {
                    ungetc(next, lex->file);
                }
            }

            // check for symbols
            bool symbol_found = false;
            for (size_t i = 0; i < SYMBOLS_LEN; ++i) {
                if (c == symbols[i]) {
                    symbol_found = true;
                    break;
                }
            }

            if (symbol_found) {
                // word does not have space before symbol
                if (strlen(value) > 0) {
                    ungetc(c, lex->file);
                } else {
                    // symbol symbol
                    value[i] = c;
                }
                break;
            }

            // check for string
            if (c == '"') {
                bool found_closing = false;
                while (!feof(lex->file)) {
                    c = fgetc(lex->file);

                    if (c == '"') {
                        found_closing = true;
                        break;
                    }

                    value[i] = c;
                    i++;
                }

                assert(found_closing);
                break;
            }

            value[i] = c;
            i++;
        }

        // last value should be 0 so it is null terminated
        assert(i < MAX_VALUE_LEN);

        lex->has_tokens = !feof(lex->file);

        if (strlen(value) == 0) continue;

        printf("%s\n", value);
        break;
    }
}

int main(int argc, char **argv)
{
    (void)keywords;

    assert(argc > 1);
    const char *in_file = argv[1];

    struct Lexer *lex = make_lexer(in_file);

    while (lex->has_tokens) {
        lexer_advance(lex);
    }

    free_lexer(lex);

    return 0;
}
