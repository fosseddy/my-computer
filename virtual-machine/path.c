#include <string.h>

#define PATH_SEP '/'

void path_get_file_name(char *out, const char *s)
{
    int dot_pos = strlen(s) - 1;
    for (; s[dot_pos] != '.'; --dot_pos);

    int len = 0;
    for (int i = dot_pos - 1; i >= 0 && s[i] != PATH_SEP; --i, ++len) {
        out[len] = s[i];
    }
    out[len] = '\0';

    for (int i = 0; i < (len / 2); ++i) {
        int tmp = out[i];
        out[i] = out[len - 1 - i];
        out[len - 1 - i] = tmp;
    }
}

void path_concat_ext(char *s, const char *ext)
{
    strcat(s, ext);
}
