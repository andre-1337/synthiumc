#include "../include/param.h"

Param param_create(Ident name, Type ty) {
    Param param = {
        .name = name,
        .ty = ty
    };

    return param;
}

bool param_is_varargs(Param *p) {
    return memcmp((void *) &p->name.ident, (void *) "...", 3) == 0;
}