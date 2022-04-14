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