#ifndef SYNTHIUMC_FUNC_H
#define SYNTHIUMC_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "vec.h"
#include "tyid.h"
#include "span.h"
#include "ident.h"
#include "lexer.h"
#include "param.h"

typedef struct ParamList {
    Vec params;
} ParamList;

typedef struct FuncDef {
    bool is_extern;
    Ident name;
    Type ret_ty;
    ParamList params;
} FuncDef;

ParamList func_pl_from_vec(Vec params);
void func_add_param(ParamList *pl, Param *p);
void func_pl_free(ParamList *pl);
FuncDef func_create(Token ident, ParamList params, Type ret_ty, bool is_extern);
int32_t func_num_params(FuncDef *f);
void func_free(FuncDef *f);

#endif
