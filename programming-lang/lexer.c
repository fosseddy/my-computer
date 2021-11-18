#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"

#include "lexer.h"

#define KEYWORDS_SIZE 21
static const char *const keywords[KEYWORDS_SIZE] = {
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

static bool is_keyword(const char *s)
{
    for (size_t i = 0; i < KEYWORDS_SIZE; ++i) {
        if (strcmp(s, keywords[i]) == 0) {
            return true;
        }
    }

    return false;
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

struct Lexer *make_lexer(const char *file_name)
{
    FILE *f = fopen(file_name, "r");
    assert(f != NULL);

    struct Lexer *lex = malloc(sizeof(struct Lexer));
    assert(lex != NULL);

    lex->file = f;
    lex->has_tokens = true;

    lex->token = malloc(sizeof(struct Token));
    assert(lex->token != NULL);

    return lex;
}

void free_lexer(struct Lexer *lex)
{
    fclose(lex->file);
    free(lex->token);
    free(lex);
}

void lexer_advance(struct Lexer *lex)
{
    if (!lex->has_tokens) return;
    char c = '\0';

LEX_AGAIN:
    c = fgetc(lex->file);

    // skip whitespace
    while (c != EOF && isspace(c)) {
        c = fgetc(lex->file);
    }

    // remove comments
    if (c == '/') {
        char next = fgetc(lex->file);
        // line comment
        if (next == '/') {
            while (c != EOF && c != '\n') {
                c = fgetc(lex->file);
            }
            goto LEX_AGAIN;
        // doc or multiline comment
        } else if (next == '*') {
            // add extra * if doc comment
            next = fgetc(lex->file);
            if (next != '*') {
                ungetc(next, lex->file);
            }

            char tmp[3] = {0};
            while (c != EOF && strcmp(tmp, "*/") != 0) {
                c = fgetc(lex->file);
                if (strlen(tmp) == 0 && c == '*') {
                    tmp[0] = c;
                }

                if (strlen(tmp) == 1 && c == '/') {
                    tmp[1] = c;
                }
            }
            // forgot to close comment tag
            assert(strcmp(tmp, "*/") == 0);
            goto LEX_AGAIN;
        // this is not comment
        } else {
            ungetc(next, lex->file);
        }
    }

    // identifier or keyword
    if (c == '_' || isalpha(c)) {
        size_t i = 0;
        while (c != EOF && !isspace(c) && !is_symbol(c)) {
            lex->token->value[i] = c;
            i++;
            assert(i < TOKEN_VALUE_SIZE);

            c = fgetc(lex->file);
        }
        lex->token->value[i] = '\0';

        if (is_symbol(c)) {
            ungetc(c, lex->file);
        }

        if (is_keyword(lex->token->value)) {
            lex->token->kind = TOKEN_KIND_KEYWORD;
        } else {
            lex->token->kind = TOKEN_KIND_IDENT;
        }
    // integer constant
    } else if (isdigit(c)) {
        lex->token->kind = TOKEN_KIND_INT_CONST;
        size_t i = 0;
        while (c != EOF && isdigit(c)) {
            lex->token->value[i] = c;
            i++;
            assert(i < TOKEN_VALUE_SIZE);

            c = fgetc(lex->file);
        }
        lex->token->value[i] = '\0';
    // string constant
    } else if (c == '"') {
        lex->token->kind = TOKEN_KIND_STR_CONST;
        size_t i = 0;
        // skip first "
        c = fgetc(lex->file);
        while (c != EOF && c != '"') {
            lex->token->value[i] = c;
            i++;
            assert(i < TOKEN_VALUE_SIZE);

            c = fgetc(lex->file);
        }
        lex->token->value[i] = '\0';
    // symbol
    } else if (is_symbol(c)) {
        lex->token->kind = TOKEN_KIND_SYMBOL;
        lex->token->value[0] = c;
        lex->token->value[1] = '\0';
    } else {
        lex->token->kind = TOKEN_KIND_UNINIT;
        lex->token->value[0] = '\0';
    }

    lex->has_tokens = !feof(lex->file);
}
