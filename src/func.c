#include "../include/func.h"

ParamList func_pl_from_vec(Vec params) {
    ParamList param_list = {
        .params = params
    };

    return param_list;
}

void func_pl_free(ParamList *pl) {
    vec_free(&pl->params);
}

void func_add_param(ParamList *pl, Param *p) {
    vec_push(&pl->params, (void *) p);
}

FuncDef func_create(Token ident, ParamList params, Type ret_ty, bool is_extern) {
    FuncDef func_def = {
        .is_extern = is_extern,
        .name = ident_create(ident),
        .ret_ty = ret_ty,
        .params = params
    };

    return func_def;
}

int32_t func_num_params(FuncDef *f) {
    return f->params.params.len;
}

void func_free(FuncDef *f) {
    func_pl_free(&f->params);
}