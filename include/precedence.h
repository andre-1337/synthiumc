#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "lexer.h"

typedef enum Precedence : int32_t {
    PRECEDENCE_NONE,
    PRECEDENCE_ASSIGN,
    PRECEDENCE_AND,
    PRECEDENCE_EQUALITY,
    PRECEDENCE_COMPARISON,
    PRECEDENCE_SUM,
    PRECEDENCE_PRODUCT,
    PRECEDENCE_UNARY,
    PRECEDENCE_CALL,
    PRECEDENCE_ACCESS
} Precedence;

int32_t precedence_get(TokenType ty);