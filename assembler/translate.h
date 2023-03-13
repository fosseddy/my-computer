struct command;  // parser.h
struct symtable; // symtable.h

void translate_a(struct command *cmd, char *buf, int bufsize, struct symtable *st);
void translate_c(struct command *cmd, char *buf);
