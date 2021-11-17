#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define KEYWORDS_SIZE 21
static const char * const keywords[KEYWORDS_SIZE] = {
    "class",   "constructor", "function",
    "method",  "field",       "static",
    "var",     "int",         "char",
    "boolean", "void",        "true",
    "false",   "null",        "this",
    "let",     "do",          "if",
    "else",    "while",       "return"
};

#define SYMBOLS_SIZE 19
static const char symbols[SYMBOLS_SIZE] = {
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

#define VALUE_SIZE 80
struct Token {
    enum Token_Kind kind;
    char value[VALUE_SIZE];
};

struct Lexer {
    FILE *file;
    bool has_tokens;
};

struct Lexer *make_lexer(const char *file_name)
{
    FILE *f = fopen(file_name, "r");
    assert(f != NULL);

    struct Lexer *lex = malloc(sizeof(struct Lexer));
    assert(lex != NULL);

    lex->file = f;
    lex->has_tokens = true;

    return lex;
}

void free_lexer(struct Lexer *lex)
{
    fclose(lex->file);
    free(lex);
}

static bool is_symbol(const char c)
{
    for (size_t i = 0; i < SYMBOLS_SIZE; ++i) {
        if (c == symbols[i]) {
            return true;
        }
    }

    return false;
}

static bool is_keyword(const char *s)
{
    for (size_t i = 0; i < KEYWORDS_SIZE; ++i) {
        if (strcmp(keywords[i], s) == 0) {
            return true;
        }
    }

    return false;
}

static bool is_integer(const char *s)
{
    for (size_t i = 0; i < strlen(s); ++i) {
        if (!isdigit(s[i])) {
            return false;
        }
    }

    return true;
}

void lexer_advance(struct Lexer *lex, struct Token *token)
{
    char value[VALUE_SIZE] = {0};
    size_t value_index = 0;
    char c = '\0';

    for (;;) {
        c = fgetc(lex->file);
        if (c == EOF) break;

        if (isspace(c)) {
            if (strlen(value) > 0) {
                if (is_keyword(value)) {
                    token->kind = TOKEN_KIND_KEYWORD;
                } else if (is_integer(value)) {
                    token->kind = TOKEN_KIND_INT_CONST;
                } else {
                    token->kind = TOKEN_KIND_IDENTIFIER;
                }

                strcpy(token->value, value);
                break;
            }
            continue;
        }

        // remove comments
        if (c == '*') {
            char next = fgetc(lex->file);
            if (next == '/') {
                assert(0 && "Closing comment tag found, but no opening");
            } else {
                ungetc(next, lex->file);
            }
        }

        if (c == '/') {
            char next = fgetc(lex->file);
            // line comment
            if (next == '/') {
                while (c != EOF && c != '\n') {
                    c = fgetc(lex->file);
                }
                continue;
            // doc or multiline comment
            } else if (next == '*') {
                // add extra '*' if it is doc comment
                next = fgetc(lex->file);
                if (next != '*') {
                    ungetc(next, lex->file);
                }

                char tmp[3] = {0};
                size_t i = 0;
                while (c != EOF && strcmp(tmp, "*/") != 0) {
                    c = fgetc(lex->file);

                    if (c == '*' || c == '/') {
                        tmp[i] = c;
                        i++;
                        // overflow of tmp buffer, forgot to close comment tag
                        assert(i < 3);
                    }
                }
                continue;
            } else {
                ungetc(next, lex->file);
            }
        }

        if (is_symbol(c)) {
            if (strlen(value) > 0) {
                ungetc(c, lex->file);

                if (is_keyword(value)) {
                    token->kind = TOKEN_KIND_KEYWORD;
                } else if (is_integer(value)) {
                    token->kind = TOKEN_KIND_INT_CONST;
                } else {
                    token->kind = TOKEN_KIND_IDENTIFIER;
                }

                strcpy(token->value, value);
            } else {
                value[value_index] = c;
                token->kind = TOKEN_KIND_SYMBOL;
                strcpy(token->value, value);
            }
            break;
        }

        if (c == '"') {
            while (c != EOF) {
                c = fgetc(lex->file);
                if (c == '"') break;

                value[value_index] = c;
                value_index++;

                assert(value_index < VALUE_SIZE);
            }
            token->kind = TOKEN_KIND_STR_CONST;
            strcpy(token->value, value);
            break;
        }

        value[value_index] = c;
        value_index++;

        assert(value_index < VALUE_SIZE);
    }

    if (strlen(value) == 0) {
        token->kind = TOKEN_KIND_UNINIT;
    }

    lex->has_tokens = !feof(lex->file);
}

int main(int argc, char **argv)
{
    assert(argc > 1);
    const char *in_file = argv[1];

    struct Lexer *lex = make_lexer(in_file);
    struct Token *token = malloc(sizeof(struct Token));

    printf("<tokens>\n");
    while (lex->has_tokens) {
        lexer_advance(lex, token);

        switch (token->kind) {
            case TOKEN_KIND_KEYWORD:
                printf("<keyword> %s </keyword>\n", token->value);
                break;

            case TOKEN_KIND_IDENTIFIER:
                printf("<identifier> %s </identifier>\n", token->value);
                break;

            case TOKEN_KIND_SYMBOL:
                printf("<symbol> %s </symbol>\n", token->value);
                break;

            case TOKEN_KIND_INT_CONST:
                printf("<integerConstant> %s </integerConstant>\n", token->value);
                break;

            case TOKEN_KIND_STR_CONST:
                printf("<stringConstant> %s </stringConstant>\n", token->value);
                break;

            case TOKEN_KIND_UNINIT: break;
            default: assert(0 && "Uncreachable");
        }
    }
    printf("</tokens>\n");

    free(token);
    free_lexer(lex);

    return 0;
}
