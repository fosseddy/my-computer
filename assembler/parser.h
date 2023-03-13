enum command_kind {
    COMMAND_A,
    COMMAND_C,
    COMMAND_L,
};

struct command {
    enum command_kind kind;
    char dest[4];
    char comp[4];
    char jump[4];
    char symbols[128];
};

struct parser {
    char *src;
    size_t cur;
    size_t len;
};

int parse_command(struct parser *p, struct command *cmd);
void parser_init(struct parser *p, char *src);
