#pragma once

#include <stdlib.h>
#include <stdint.h>

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

char const *const err2str(int32_t err_code, ...);