struct parser {
    FILE *file;
    char *line;
    size_t line_size;
};

enum instruction_kind {
    INSTRUCTION_KIND_A = 0,
    INSTRUCTION_KIND_C,
    INSTRUCTION_KIND_L
};

struct instruction {
    enum instruction_kind kind;
    char dest[4];
    char comp[4];
    char jump[4];
    char label[128];
};

void parser_init(struct parser *p, char *filepath);
int peek_line(struct parser *p);
void parse_instruction(struct parser *p, struct instruction *inst);
