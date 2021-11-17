#include <stdio.h>
#include <string.h>

enum Token_Kind {
    TOKEN_KIND_KEYWORD,
    TOKEN_KIND_SYMBOL,
    TOKEN_KIND_INT_CONST,
    TOKEN_KIND_STR_CONST,
    TOKEN_KIND_IDENTIFIER,
};

//struct Lexer {};

#define KEYWORDS_LEN 21

static const char * const keywords[KEYWORDS_LEN] = {
    "class",
    "constructor",
    "function",
    "method",
    "field",
    "static",
    "var",
    "int",
    "char",
    "boolean",
    "void",
    "true",
    "false",
    "null",
    "this",
    "let",
    "do",
    "if",
    "else",
    "while",
    "return"
};

int main(void)
{
    for (size_t i = 0; i < KEYWORDS_LEN; ++i) {
        printf("%s\n", keywords[i]);
    }

    return 0;
}
