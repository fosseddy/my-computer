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

static int lexer_peek(struct Lexer *lex)
{
    int c = fgetc(lex->file);
    ungetc(c, lex->file);

    return c;
}

static void lexer_next(struct Lexer *lex)
{
    lex->ch = fgetc(lex->file);
}

static void lexer_skip_whitespace(struct Lexer *lex)
{
    while (lex->ch != EOF && isspace(lex->ch)) {
        lexer_next(lex);
    }
}

static void lexer_skip_comment(struct Lexer *lex)
{
    // line comment
    if (lex->ch == '/') {
        while (lex->ch != EOF && lex->ch != '\n') {
            lexer_next(lex);
        }
    // doc or multiline comment
    } else {
        // add extra * if doc comment
        if (lexer_peek(lex) == '*') {
            lexer_next(lex);
        }

        while (lex->ch != EOF) {
            lexer_next(lex);
            if (lex->ch == '*' && lexer_peek(lex) == '/') {
                lexer_next(lex);
                break;
            }
        }
    }
}

static void lexer_lex_identifier(struct Lexer *lex)
{
    size_t i = 0;

    while (lex->ch != EOF) {
        lex->token->value[i] = lex->ch;
        i++;
        assert(i < TOKEN_VALUE_SIZE);

        char next = lexer_peek(lex);
        if (isspace(next) || is_symbol(next)) break;

        lexer_next(lex);
    }

    lex->token->value[i] = '\0';

    if (is_keyword(lex->token->value)) {
        lex->token->kind = TOKEN_KIND_KEYWORD;
    } else {
        lex->token->kind = TOKEN_KIND_IDENT;
    }
}

static void lexer_lex_int_constant(struct Lexer *lex)
{
    lex->token->kind = TOKEN_KIND_INT_CONST;

    size_t i = 0;

    while (lex->ch != EOF) {
        lex->token->value[i] = lex->ch;
        i++;
        assert(i < TOKEN_VALUE_SIZE);

        if (!isdigit(lexer_peek(lex))) break;

        lexer_next(lex);
    }

    lex->token->value[i] = '\0';
}

static void lexer_lex_str_constant(struct Lexer *lex)
{
    // skip first "
    lexer_next(lex);

    lex->token->kind = TOKEN_KIND_STR_CONST;

    size_t i = 0;

    while (lex->ch != EOF && lex->ch != '"') {
        lex->token->value[i] = lex->ch;
        i++;
        assert(i < TOKEN_VALUE_SIZE);

        lexer_next(lex);
    }

    lex->token->value[i] = '\0';
}

struct Lexer *make_lexer(const char *file_name)
{
    FILE *f = fopen(file_name, "r");
    assert(f != NULL);

    struct Lexer *lex = malloc(sizeof(struct Lexer));
    assert(lex != NULL);

    lex->ch = '\0';
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

LEX_AGAIN:
    lexer_next(lex);

    lexer_skip_whitespace(lex);

    if (lex->ch == '_' || isalpha(lex->ch)) {
        lexer_lex_identifier(lex);
    } else if (isdigit(lex->ch)) {
        lexer_lex_int_constant(lex);
    } else if (lex->ch == '"') {
        lexer_lex_str_constant(lex);
    } else if (is_symbol(lex->ch)) {
        if (lex->ch == '/') {
            char next = lexer_peek(lex);
            if (next == '/' || next == '*') {
                lexer_next(lex);
                lexer_skip_comment(lex);
                goto LEX_AGAIN;
            }
        }

        lex->token->kind = TOKEN_KIND_SYMBOL;
        lex->token->value[0] = lex->ch;
        lex->token->value[1] = '\0';
    } else {
        lex->token->kind = TOKEN_KIND_UNINIT;
        lex->token->value[0] = '\0';
    }

    lex->has_tokens = !feof(lex->file);
}
