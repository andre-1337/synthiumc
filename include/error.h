#pragma once

#include <stdlib.h>
#include <stdint.h>

typedef enum ErrorCodes {
    OK,
    ERROR_PATH_EMPTY,
    ERROR_COULD_NOT_OPEN_FILE,
    ERROR_COULD_NOT_ALLOCATE_BUFFER,
    ERROR_COULD_NOT_READ_FILE,
    ERROR_UNEXPECTED_EOF,
    ERROR_EXPECTED_BUT_GOT,
    ERROR_UNKNOWN_SYMBOL,
    ERROR_CHAR_LIT_LEN,
    ERROR_COULD_NOT_PARSE_STMT,
    ERROR_INVALID_TYPE_IDENT
} ErrorCodes;

char const *const error_texts[] = {
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

char const *const error_err2str(int32_t err_code, ...);