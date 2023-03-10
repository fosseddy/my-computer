#define INSTRUCTION_CAPACITY 4
#define LABEL_CAPACITY 101

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
    char dest[INSTRUCTION_CAPACITY];
    char comp[INSTRUCTION_CAPACITY];
    char jump[INSTRUCTION_CAPACITY];
    char label[LABEL_CAPACITY];
};

struct parser make_parser(const char *file_path);
int parser_peek_line(struct parser *p);
struct instruction parser_parse_instruction(struct parser *p);
