#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum TyTypes : int32_t {
    TY_UNKNOWN,
    TY_PTR,
    TY_I32,
    TY_STRING,
    TY_FUNC,
    TY_MOD,
    TY_STRUCT
} TyTypes;