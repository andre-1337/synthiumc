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

TypeList ty_new_empty_list() {
    TypeList list = {
        .types = ptrvec_create()
    };

    return list;
}

TypeList ty_create_type_list(Ptrvec types) {
    TypeList list = {
        .types = types
    };

    return list;
}

Ty *ty_type_at(TypeList *tl, int32_t i) {
    return (Ty *) ptrvec_get(&tl->types, i);
}

void ty_type_list_free(TypeList *tl) {
    int32_t i = 0;
    while (i < tl->types.len) {
        ty_type_free(ty_type_at(tl, i));
        i++;
    }

    ptrvec_free(&tl->types);
}

void ty_type_free(Ty *t) {
    if (t == NULL) {
        return;
    }

    if (ty_is_func(t)) {
        Func *f_ty = ty_as_func(t);
        ty_type_free(f_ty->ret);
        ty_type_list_free(&f_ty->params);
    }

    if (ty_is_struct(t)) {
        Struct *s_ty = ty_as_struct(t);
        vec_free(&s_ty->fields);
    }

    if (ty_is_mod(t)) {
        Mod *m_ty = ty_as_mod(t);
        scope_free(&m_ty->scope);
    }

    free((void *) t);
}

bool ty_width_was_calculated(Ty *t) {
    return t->align > 0;
}

bool ty_fill_width_align(Ty *t) {
    if (ty_fill_width_align(t)) {
        return true;
    }

    int32_t align = 0;
    int32_t width = WIDTH_UNKNOWN;
    bool has_placeholders = false;

    if (ty_is_ptr(t)) {
        width = 8;
    } else if (ty_is_i32(t)) {
        width = 4;
    } else if (ty_is_string(t)) {
        width = 8;
    } else if (ty_is_struct(t)) {
        Struct *s_ty = ty_as_struct(t);
        int32_t max_align = align;
        int32_t width_sum = 0;
        int32_t i = 0;

        while (i < s_ty->fields.len) {
            StructField *f = ty_field_at(s_ty, i);
            bool is_placeholder = flag_get(&f->ty->flags, FLAG_PLACEHOLDER);

            if (is_placeholder) {
                has_placeholders = true;
            }

            if (f != NULL && !is_placeholder) {
                int32_t a = f->ty->align;
                if (a > max_align) {
                    max_align = a;
                }

                int32_t w = f->ty->width;
                width_sum += w;
            }

            i++;
        }

        width = width_sum;
        align = max_align;
    }

    if (has_placeholders) {
        return false;
    }

    if (width != WIDTH_UNKNOWN) {
        t->width = width;

        if (align == 0) {
            align = width;
        }
        
        t->align = align;
        flag_unset(&t->flags, FLAG_PLACEHOLDER);

        return true;
    }

    return false;
}

const char *ty_to_string(Ty *t, SpanInterner *si) {
    if (t == NULL) {
        return NULL;
    }

    if (ty_is_i32(t)) {
        return strdup("i32");
    }

    if (ty_is_string(t)) {
        return strdup("string");
    }

    if (ty_is_func(t)) {
        char *name = "name";
        const char *s = fmt_str("fn %s(...): ...", name);
        return s;
    }

    if (ty_is_mod(t)) {
        return strdup("mod");
    }

    return NULL;
}

Ty *ty_clone(Ty *t) {
    if (t == NULL) {
        return NULL;
    }

    void *nt = malloc(sizeof(Ty));
    memcpy(nt, (void *) t, sizeof(Ty));

    return (Ty *) nt;
}