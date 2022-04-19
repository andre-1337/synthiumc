#include "../include/typecheck.h"

WaitingRequest typecheck_create_waiting_request(WaitingType kind, Ty *to_fill, Module *to_fill_mod, Ty *waiting_for_ty, Module *waiting_for_mod, int32_t field_idx) {
    WaitingRequest request = {
        .kind = kind,
        .field_idx = field_idx,
        .to_fill = to_fill,
        .to_fill_mod = to_fill_mod,
        .waiting_for_ty = waiting_for_ty,
        .waiting_for_mod = waiting_for_mod
    };

    return request;
}

Ident *typecheck_req_waiting_for_ident(WaitingRequest *r) {
    if (r->kind == WAITING_STRUCT) {
        return &ty_as_struct(r->waiting_for_ty)->name;
    }

    return NULL;
}

WaitingRequestMap typecheck_create_waiting_request_map() {
    WaitingRequestMap map = {
        .init_flag = 1,
        .requests = vec_create(sizeof(Vec)),
        .request_map = map_create()
    };

    return map;
}

void typecheck_add_request(WaitingRequestMap *wrm, SpanInterner *si, WaitingRequest request) {
    Vec *requests = NULL;
    Ident *ident = typecheck_req_waiting_for_ident(&request);

    const char *s = ident_to_string(ident, si);
    printf("debug: adding request for '%s'\n", s);
    free((void *) s);

    Key key = map_key_from_ident(si, ident);
    void *request_idx = map_get(&wrm->request_map, key);
    int32_t idx = -1;

    if (ptr2int(request_idx) == 0) {
        idx = wrm->requests.len;

        Vec v = vec_create(sizeof(WaitingRequest));
        vec_push(&wrm->requests, (void *) &v);

        request_idx = int2ptr(idx);
        map_insert(&wrm->request_map, key, request_idx + 1);
    } else {
        idx = ptr2int(request_idx) - 1;
    }

    requests = (Vec *) vec_get_ptr(&wrm->requests, idx);
    vec_push(requests, (void *) &requests);
}

Vec *typecheck_get_waiting(WaitingRequestMap *wrm, SpanInterner *si, Ident *ident) {
    Key key = map_key_from_ident(si, ident);
    void *request_idx = map_get(&wrm->request_map, key);

    if (request_idx == NULL) {
        return NULL;
    }

    int32_t idx = ptr2int(request_idx) - 1;

    return (Vec *) vec_get_ptr(&wrm->requests, idx);
}

void typecheck_free_wait_map(WaitingRequestMap *wrm) {
    int32_t i = 0;
    while (i < wrm->requests.len) {
        Vec *v = (Vec *) vec_get_ptr(&wrm->requests, i);
        vec_free(v);

        i++;
    }

    vec_free(&wrm->requests);
    map_free(&wrm->request_map);
}

TypeError typecheck_err(TypeChecker *tc, const char *text, Span span) {
    TypeError error = {
        .text = text,
        .span = span
    };

    return error;
}

void typecheck_free_err(TypeError *err) {
    free((void *) err->text);
}

Ctx typecheck_empty_ctx() {
    Ctx ctx = {
        .mod = NULL,
        .imports = map_create(),
        .scopes = scope_empty_stack()
    };

    return ctx;
}

Ctx typecheck_create_ctx(Module *mod, SpanInterner *si, Scope *global) {
    ScopeStack scopes = scope_create_stack(si);

    scope_push(&scopes, global);
    scope_push(&scopes, &mod->ty->scope);
    scope_open(&scopes);

    Ctx ctx = {
        .mod = mod,
        .imports = map_create(),
        .scopes = scopes
    };

    return ctx;
}

void typecheck_free_ctx(Ctx *ctx) {
    scope_free_stack(&ctx->scopes);
    map_free(&ctx->imports);
}

Module *typecheck_get_mod_by_alias(Ctx *ctx, int32_t alias_len, const char *alias) {
    return (Module *) map_get(&ctx->imports, map_create_key(alias_len, alias));
}

TypeChecker typecheck_create(SpanInterner *si, ModuleMap *mods) {
    TypeChecker typechecker = {
        .si = si,
        .mods = mods,
        .sorted_mods = typecheck_sorted_mods(mods, si),
        .temp_types = ptrvec_with_cap(256),
        .ctx = typecheck_empty_ctx(),
        .globals = scope_create(),
        .errors = vec_create(sizeof(TypeError)),
        .requests = vec_with_cap(sizeof(WaitingRequestMap), mod_num_mods(mods))
    };

    vec_init_zero(&typechecker.requests);
    typechecker.globals = typecheck_create_global_scope(&typechecker);

    return typechecker;
}

void typecheck_wait_for(TypeChecker *tc, WaitingRequest req) {
    int32_t idx = req.waiting_for_mod->idx;
    WaitingRequestMap *map = (WaitingRequestMap *) vec_get_ptr(&tc->requests, idx);

    if (map->init_flag == 0) {
        *map = typecheck_create_waiting_request_map();
    }

    typecheck_add_request(map, tc->si, req);
}

WaitingRequestMap *typecheck_get_waiting_map(TypeChecker *tc, Module *mod) {
    return (WaitingRequestMap *) vec_get_ptr(&tc->requests, mod->idx);
}

Scope typecheck_create_global_scope(TypeChecker *tc) {
    Scope scope = scope_create();
    scope_s_bind_in(&scope, 3, "i32", typecheck_push_tmp_ty(tc, ty_new_i32()));

    return scope;
}

int32_t typecheck_num_errs(TypeChecker *tc) {
    return tc->errors.len;
}

void typecheck_push_err(TypeChecker *tc, TypeError error) {
    vec_push(&tc->errors, (void *) &error);
}

void typecheck_push_mk_error(TypeChecker *tc, const char *string, Span span) {
    TypeError error = typecheck_err(tc, string, span);
    vec_push(&tc->errors, (void *) &error);
}

TypeError *typecheck_get_err(TypeChecker *tc, int32_t i) {
    return (TypeError *) vec_get_ptr(&tc->errors, i);
}

void typecheck_add_import_alias(TypeChecker *tc, Ident *ident, Module *mod) {
    map_insert(&tc->ctx.imports, map_key_from_ident(tc->si, ident), (void *) mod);
}

int32_t *typecheck_sorted_mods(ModuleMap *mods, SpanInterner *si) {
    int32_t num_mods = mod_num_mods(mods);
    int32_t *import_nums = (int32_t *) malloc(num_mods * sizeof(int32_t));
    int32_t i = 0;

    while (i < num_mods) {
        Module *m = mod_get_mod(mods, i);
        import_nums[i] = mod_num_imports(m);

        i++;
    }

    int32_t *sorted_nums = tuple_isort(import_nums, num_mods);
    free((void *) import_nums);

    return sorted_nums;
}

Mod *typecheck_make_mod_type(TypeChecker *tc, Module *mod, SpanInterner *si) {
    Mod *mod_ty = (Mod *) ty_new_mod();
    int32_t i = 0;
    int32_t num_structs = mod_num_structs(mod);

    while (i < num_structs) {
        StructDecl *s = &mod_get_struct_at(mod, i)->decl;
        Ty *s_ty = ty_new_placeholder_type(TY_STRUCT, sizeof(Struct));

        ty_init_struct(s_ty, s->name);
        typecheck_push_tmp_ty(tc, s_ty);

        const char *name = ident_to_string(&s->name, si);
        printf("debug: creating placeholder for '%s'\n", name);
        free((void *) name);

        scope_bind_in(&mod_ty->scope, si, &s->name, s_ty);
        i++;
    }

    return mod_ty;
}

Ty *typecheck_lookup_ident(TypeChecker *tc, Ident *ident) {
    return typecheck_lookup_ident_mod(tc, ident, NULL);
}

Ty *typecheck_lookup_ident_mod(TypeChecker *tc, Ident *ident, Module **out_mod) {
    int32_t len = ident_len(ident, tc->si);
    int32_t dot_idx = ident_index_of(ident, len, '.');

    if (dot_idx < 0) {
        return scope_lookup(&tc->ctx.scopes, ident);
    }

    Module *mod = typecheck_get_mod_by_alias(&tc->ctx, dot_idx, ident->ident);
    if (mod == NULL) {
        printf("[error] debug: module '%.*s' not found\n", len, ident->ident);
        return NULL;
    }

    if (out_mod != NULL) {
        *out_mod = mod;
    }

    Ty *ty = mod_s_lookup(mod, len - dot_idx - 1, ident->ident + dot_idx + 1);
    if (ty == NULL) {
        printf("[error] debug: type '%.*s' not found\n", len, ident->ident);
        return NULL;
    }

    return ty;
}

void typecheck_check(TypeChecker *tc) {
    int32_t i = 0;
    while (i < mod_num_mods(tc->mods)) {
        int32_t idx = tc->sorted_mods[i];
        Module *m = mod_get_mod(tc->mods, idx);

        typecheck_check_mod(tc, m);

        i++;
    }
}

Mod *typecheck_check_mod(TypeChecker *tc, Module *mod) {
    if (mod->ty != NULL) {
        return mod->ty;
    }

    mod->ty = typecheck_make_mod_type(tc, mod, tc->si);

    typecheck_free_ctx(&tc->ctx);
    tc->ctx = typecheck_create_ctx(mod, tc->si, &tc->globals);

    printf("checking '%.*s'\n", mod->path.len, mod->path.inner);

    int32_t i = 0;
    while (i < mod_num_stmts(mod)) {
        Stmt *result = typecheck_check_stmt(tc, mod_get_stmt_at(mod, i));
        if (result != NULL) {
            mod_set_stmt_at(mod, i, result);
        }

        i++;
    }

    return mod->ty;
}

Ty *typecheck_push_tmp_ty(TypeChecker *tc, Ty *ty) {
    ptrvec_push_ptr(&tc->temp_types, (void *) ty);
    return ty;
}

void typecheck_bind(TypeChecker *tc, Ident *ident, Ty *ty) {
    scope_bind(&tc->ctx.scopes, ident, ty);
}

void typecheck_fill_struct_fields(TypeChecker *tc, StructDecl *s_decl, Struct *s_ty) {
    if (ty_is_initialized((Ty *) s_ty)) {
        return;
    }

    const char *s = ident_to_string(&s_decl->name, tc->si);
    printf("-----\ndebug: resolving '%s'\n", s);
    free((void *) s);

    bool error = false;
    bool waiting = false;
    int32_t nf = record_num_fields(s_decl);
    int32_t i = 0;

    while (i < nf) {
        Field f = record_field_empty();

        if (!record_field_at(s_decl, i, &f)) {
            printf("[error] debug: todo\n");
            return;
        }

        const char *s = type_to_string(&f.ty, tc->si);
        printf("debug: field type = '%s'\n", s);
        free((void *) s);

        Module *mod = NULL;
        Ty *field_ty = typecheck_lookup_ident_mod(tc, &f.ty.ident, &mod);

        if (field_ty != NULL) {
            bool is_ptr = type_is_ptr(&f.ty);

            if (!ty_is_initialized(field_ty) && !is_ptr) {
                if (ty_is_struct(field_ty)) {
                    if (mod == NULL) {
                        mod = tc->ctx.mod;
                    }

                    printf("debug: field type is a struct, creating a placeholder...\n");

                    Ty *placeholder = ty_new_placeholder_type(TY_STRUCT, sizeof(Struct));
                    ty_init_struct(placeholder, f.ty.ident);
                    typecheck_push_tmp_ty(tc, placeholder);
                    ty_push_field(s_ty, f.ident, placeholder);

                    WaitingRequest request = typecheck_create_waiting_request(WAITING_STRUCT, (Ty *) s_ty, tc->ctx.mod, field_ty, mod, i);
                    typecheck_wait_for(tc, request);
                    waiting = true;
                } else {
                    printf("\ndebug: unreachable\n\n");
                }
            } else {
                if (is_ptr) {
                    field_ty = ty_new_ptr(f.ty.pointer_count, field_ty);
                    typecheck_push_tmp_ty(tc, field_ty);
                }

                ty_push_field(s_ty, f.ident, field_ty);
            }
        } else {
            error = true;

            const char *s = ident_to_string(&f.ty.ident, tc->si);
            printf("[error] debug: '%s' is null\n", s);
            free((void *) s);
        }

        i++;
    }

    s = ident_to_string(&s_decl->name, tc->si);

    if (!error && !waiting) {
        Ty *ty = (Ty *) s_ty;
        ty_fill_width_align(ty);

        printf("debug: '%s' has a width of '%d' and an alignment of '%d'\n", s, ty->width, ty->align);
        free((void *) s);

        typecheck_update_waiting(tc, tc->ctx.mod, ty, &s_ty->name);

        return;
    }

    if (error) {
        printf("[error] debug: error while filling '%s'\n", s);
    }

    free((void *) s);
}

void typecheck_update_waiting(TypeChecker *tc, Module *mod, Ty *resolved_ty, Ident *ident) {
    WaitingRequestMap *waiting_map = typecheck_get_waiting_map(tc, mod);
    Vec *waiting = typecheck_get_waiting(waiting_map, tc->si, ident);

    if (waiting == NULL) {
        const char *name = ident_to_string(ident, tc->si);
        printf("debug: no other types are waiting for '%s' (with width of '%d' and an alignment of '%d')\n", name, resolved_ty->width, resolved_ty->align);
        free((void *) name);

        return;
    }

    const char *name = ident_to_string(ident, tc->si);
    printf("debug: there are '%lld' types waiitng for '%s'\n", waiting->len, name);
    free((void *) name);

    int32_t i = 0;
    while (i < waiting->len) {
        WaitingRequest *req = (WaitingRequest *) vec_get_ptr(waiting, i);
        Ty *waiting_ty = req->to_fill;
        Module *waiting_mod = req->to_fill_mod;
        Struct *waiting_s_ty = (Struct *) waiting_ty;
        Ident *waiting_ident = &waiting_s_ty->name;

        StructField *placeholder = ty_field_at(waiting_s_ty, req->field_idx);
        placeholder->ty = resolved_ty;

        bool no_placeholders = ty_fill_width_align(waiting_ty);
        if (no_placeholders) {
            const char *name = ident_to_string(waiting_ident, tc->si);
            printf("debug: '%s' has no placeholder left\n", name);
            free((void *) name);

            typecheck_update_waiting(tc, waiting_mod, waiting_ty, waiting_ident);
        }

        i++;
    }

    const char *s = ident_to_string(ident, tc->si);
    printf("debug: ----- '%s' with width of '%d' and an alignment of '%d'\n", s, resolved_ty->width, resolved_ty->align);
    free((void *) s);
}

Ident *typecheck_get_import_alias(TypeChecker *tc, ImportStmt *imp) {
    return &imp->mod;
}

Stmt *typecheck_check_stmt(TypeChecker *tc, Stmt *s) {
    if (ast_is_import_stmt(s)) {
        ImportStmt *i_s = ast_as_import_stmt(s);
        char *error = NULL;
        Module *imported_mod = mod_try_get_mod_from_import(tc->mods, tc->ctx.mod, tc->si, i_s, &error);

        if (imported_mod == NULL) {
            typecheck_push_mk_error(tc, error, i_s->mod.ident_span);
            return NULL;
        }

        Ident *alias = typecheck_get_import_alias(tc, i_s);
        Mod *mod_ty = imported_mod->ty;

        if (mod_ty == NULL) {
            Ctx ctx = tc->ctx;
            tc->ctx = typecheck_empty_ctx();

            mod_ty = typecheck_check_mod(tc, imported_mod);

            typecheck_free_ctx(&tc->ctx);
            tc->ctx = ctx;
        }

        typecheck_bind(tc, &i_s->mod, (Ty *) mod_ty);

        return s;
    }

    if (ast_is_struct_decl_stmt(s)) {
        StructDecl *s_d = &ast_as_struct_decl_stmt(s)->decl;
        Ty *definition = typecheck_lookup_ident(tc, &s_d->name);

        if (definition == NULL) {
            const char *name = ident_to_string(&s_d->name, tc->si);
            printf("[debug] error: '%s' is not defined\n", name);
            free((void *) name);
        }

        ty_init_struct(definition, s_d->name);
        typecheck_fill_struct_fields(tc, s_d, (Struct *) definition);

        return s;
    }

    if (ast_is_let_stmt(s)) {
        LetStmt *l_s = ast_as_let_stmt(s);
        Expr *value = typecheck_check_expr(tc, l_s->value);

        if (value == NULL) {
            return NULL;
        }

        l_s->value = value;
        typecheck_bind(tc, &l_s->ident, l_s->value->ty);

        return s;
    }

    if (ast_is_expr_stmt(s)) {
        ExprStmt *e_s = ast_as_expr_stmt(s);
        Expr *expr = typecheck_check_expr(tc, e_s->expr);
        
        if (expr == NULL) {
            return NULL;
        }

        e_s->expr = expr;

        return s;
    }

    return NULL;
}

Expr *typecheck_check_expr(TypeChecker *tc, Expr *e) {
    if (ast_is_int_expr(e)) {
        e->ty = ty_new_i32();
        typecheck_push_tmp_ty(tc, e->ty);

        return e;
    }

    if (ast_is_string_expr(e)) {
        e->ty = ty_new_string();
        typecheck_push_tmp_ty(tc, e->ty);

        return e;
    }

    if (ast_is_ident_expr(e)) {
        e->ty = typecheck_lookup_ident(tc, &ast_as_ident_expr(e)->ident);

        if (e->ty == NULL) {
            return NULL;
        }

        return e;
    }

    if (ast_is_access_expr(e)) {
        AccessExpr *a_e = ast_as_access_expr(e);
        Expr *left = typecheck_check_expr(tc, a_e->left);

        if (left == NULL) {
            return NULL;
        }

        a_e->left = left;

        if (!ty_is_scoped(a_e->left->ty)) {
            const char *ty_s = ty_to_string(a_e->left->ty, tc->si);
            const char *error = fmt_str("'%s' cannot be accessed with the dot operator", ty_s);
            free((void *) ty_s);

            typecheck_push_mk_error(tc, error, a_e->left->span);

            return NULL;
        }
    }

    return e;
}

void typecheck_free_tc(TypeChecker *tc) {
    typecheck_free_ctx(&tc->ctx);

    int32_t i = 0;
    while (i < tc->temp_types.len) {
        ty_type_free((Ty *) ptrvec_get(&tc->temp_types, i));
        i++;
    }

    ptrvec_free(&tc->temp_types);
    free((void *) tc->sorted_mods);

    i = 0;
    while  (i < tc->errors.len) {
        typecheck_free_err(typecheck_get_err(tc, i));
        i++;
    }

    vec_free(&tc->errors);
    scope_free(&tc->globals);

    i = 0;
    while (i < tc->requests.len) {
        typecheck_free_wait_map((WaitingRequestMap *) vec_get_ptr(&tc->requests, i));
        i++;
    }

    vec_free(&tc->requests);
}