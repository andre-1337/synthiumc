#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
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
} ErrorCode;

const char *error_err2str(ErrorCode err_code, ...);