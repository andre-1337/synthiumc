#include "../include/ty.h"

Ty *ty_new_i32() {
    I32 *i32 = (I32 *) malloc(sizeof(I32));
    i32->t = ty_create_type(TY_I32);

    Ty *t = (Ty *) i32;

    ty_fill_width_align(t);

    return t;
}

bool ty_is_i32(Ty *t) {
    return t->kind == TY_I32;
}

I32 *ty_as_i32(Ty *t) {
    return (I32 *) t;
}

Ty *ty_new_ptr(int32_t count, Ty *inner) {
    Ptr *ptr = (Ptr *) malloc(sizeof(Ptr));
    ptr->t = ty_create_type(TY_PTR);
    ptr->count = count;
    ptr->inner = inner;

    Ty *t = (Ty *) ptr;

    ty_fill_width_align(t);

    return t;
}

bool ty_is_ptr(Ty *t) {
    return t->kind == TY_PTR;
}

Ptr *ty_as_ptr(Ty *t) {
    return (Ptr *) t;
}

Ty *ty_new_string() {
    String *str = (String *) malloc(sizeof(String));
    str->t = ty_create_type(TY_STRING);

    Ty *t = (Ty *) str;

    ty_fill_width_align(t);

    return t;
}

bool ty_is_string(Ty *t) {
    return t->kind == TY_STRING;
}

String *ty_as_string(Ty *t) {
    return (String *) t;
}

Ty *ty_new_struct(Ident name, Vec fields) {
    Struct *struc = (Struct *) malloc(sizeof(Struct));
    struc->t = ty_create_type(TY_STRUCT);
    struc->name = name;
    struc->fields = fields;

    Ty *t = (Ty *) struc;

    flag_set(&t->flags, FLAG_SCOPED);
    ty_fill_width_align(t);

    return t;
}

void ty_init_struct(Ty *t, Ident name) {
    Struct *struc = (Struct *) malloc(sizeof(Struct));
    struc->t = *t;
    struc->name = name;
    struc->fields = vec_create(sizeof(StructField));

    flag_set(&struc->t.flags, FLAG_SCOPED);

    memcpy((void *) t, (void *) struc, sizeof(Struct));
}

bool ty_is_struct(Ty *t) {
    return t->kind == TY_STRUCT;
}

Struct *ty_as_struct(Ty *t) {
    return (Struct *) t;
}

void ty_push_field(Struct *t, Ident name, Ty *ty) {
    StructField field = {
        .name = name,
        .ty = ty
    };
    
    vec_push(&t->fields, (void *) &field);
}

StructField *ty_field_at(Struct *t, int32_t i) {
    return (StructField *) vec_get_ptr(&t->fields, i);
}

int32_t ty_num_fields(Struct *t) {
    return t->fields.len;
}

Ty *ty_new_func(Ty *ret, Ptrvec params, Ident name) {
    TypeList parameters = ty_create_type_list(params);
    Func *func = (Func *) malloc(sizeof(Func));
    func->t = ty_create_type(TY_FUNC);
    func->ret = ret;
    func->params = parameters;
    func->name = name;

    Ty *t = (Ty *) func;

    ty_fill_width_align(t);

    return t;
}

bool ty_is_func(Ty *t) {
    return t->kind == TY_FUNC;
}

Func *ty_as_func(Ty *t) {
    return (Func *) t;
}

Ty *ty_new_mod() {
    Mod *mod = (Mod *) malloc(sizeof(Mod));
    mod->t = ty_create_type(TY_MOD);
    mod->scope = scope_create();

    Ty *t = (Ty *) mod;

    flag_set(&t->flags, FLAG_SCOPED);
    flag_set(&t->flags, FLAG_UNSIZED);

    return t;
}

bool ty_is_mod(Ty *t) {
    return t->kind == TY_MOD;
}

Mod *ty_as_mod(Ty *t) {
    return (Mod *) t;
}

Ty *ty_new_placeholder_type(TyTypes kind, int32_t size) {
    void *mem = malloc(size);
    Ty t = ty_create_type(kind);

    flag_set(&t.flags, FLAG_PLACEHOLDER);
    memcpy(mem, (void *) &t, sizeof(Ty));

    return (Ty *) mem;
}

Ty ty_create_type(TyTypes ty) {
    Ty t = {
        .kind = ty,
        .width = WIDTH_UNKNOWN,
        .align = 0,
        .flags = 0
    };

    return t;
}

bool ty_is_initialized(Ty *t) {
    return (t->width != WIDTH_UNKNOWN) || (flag_get(&t->flags, FLAG_UNSIZED));
}

bool ty_is_scoped(Ty *t) {
    if (t == NULL) {
        return false;
    }

    return flag_get(&t->flags, FLAG_SCOPED);
}

// TODO : finish porting