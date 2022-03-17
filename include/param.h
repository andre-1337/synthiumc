#pragma once

#include <stdbool.h>

#include "tyid.h"
#include "ident.h"

typedef struct Param {
    Ident name;
    Type ty;
} Param;

Param param_create(Ident name, Type ty);
bool param_is_varargs(Param *p);