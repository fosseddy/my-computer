#include <stdio.h>
#include <assert.h>

#include "lexer.h"

int main(int argc, char **argv)
{
    assert(argc > 1);
    const char *in_file = argv[1];

    struct Lexer *lex = make_lexer(in_file);

    printf("tokens:\n");
    while (lex->has_tokens) {
        lexer_advance(lex);

        switch (lex->token->kind) {
            case TOKEN_KIND_KEYWORD:
                printf("    keyword:    %s\n", lex->token->value);
                break;

            case TOKEN_KIND_IDENT:
                printf("    ident:      %s\n", lex->token->value);
                break;

            case TOKEN_KIND_SYMBOL:
                printf("    symbol:     %s\n", lex->token->value);
                break;

            case TOKEN_KIND_INT_CONST:
                printf("    integer:    %s\n", lex->token->value);
                break;

            case TOKEN_KIND_STR_CONST:
                printf("    string:     %s\n", lex->token->value);
                break;

            case TOKEN_KIND_UNINIT:
            default:
                break;
        }
    }

    free_lexer(lex);

    return 0;
}
