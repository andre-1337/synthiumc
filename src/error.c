#include "../include/error.h"
#include "../include/utils.h"

const char *error_texts[] = {
    NULL,
    "path was empty",
    "could not open file '%s'",
    "could not allocate buffer",
    "could not read file '%s'",
    "unexpected eof",
    "expected '%s'; got '%s' instead",
    "unknown symbol '%.*s'",
    "character literal with invalid length of %1$d: '%.*s'",
    "could not parse statement\n%.*s",
    "illegal type identifier '%.*s': '%s'"
};

size_t const len_error_strings = sizeof(error_texts) / sizeof(char *);

const char *error_err2str(ErrorCode err_code, ...) {
    if (err_code < 0 || err_code > len_error_strings) {
        return error_texts[0];
    }

    char *s = NULL;
    va_list args;
    va_start(args, err_code);
    int32_t size = vfmt_str(&s, error_texts[err_code], args);
    va_end(args);

    if (size < 0) return error_texts[0];

    return s;
}
