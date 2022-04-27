#include "../include/mod.h"

Module *mod_create(Path p) {
    Module *module = (Module *) malloc(sizeof(Module));

    module->statements = ptrvec_create();
    module->path = p;
    module->imports = vec_create(sizeof(int32_t));
    module->functions = vec_create(sizeof(int32_t));
    module->structs = vec_create(sizeof(int32_t));
    module->ty = NULL;
    module->idx = -1;

    return module;
}

Ty *mod_s_lookup(Module *m, int32_t key_len, const char *key) {
    return scope_s_get_in(&m->ty->scope, key_len, key);
}

Stmt *mod_get_stmt_at(Module *m, int32_t i) {
    return (Stmt *) ptrvec_get(&m->statements, i);
}

void mod_set_stmt_at(Module *m, int32_t i, Stmt *s) {
    ptrvec_set(&m->statements, i, (void *) s);
}

int32_t mod_num_stmts(Module *m) {
    return m->statements.len;
}

Stmt *mod_get_special_stmt_at(Module *m, Vec *indices, int32_t i) {
    int32_t idx = 0;
    if (!vec_get(indices, i, (void *) &idx)) {
        return NULL;
    }

    Stmt *s = mod_get_stmt(m, idx);
    if (s == NULL) {
        return NULL;
    }

    return s;
}

ImportStmt *mod_get_import_at(Module *m, int32_t i) {
    return ast_as_import_stmt(mod_get_special_stmt_at(m, &m->imports, i));
}

int32_t mod_num_imports(Module *m) {
    return m->imports.len;
}

FuncDeclStmt *mod_get_function_at(Module *m, int32_t i) {
    return ast_as_func_decl_stmt(mod_get_special_stmt_at(m, &m->functions, i));
}

int32_t mod_num_functions(Module *m) {
    return m->functions.len;
}

StructDeclStmt *mod_get_struct_at(Module *m, int32_t i) {
    return ast_as_struct_decl_stmt(mod_get_special_stmt_at(m, &m->structs, i));
}

int32_t mod_num_structs(Module *m) {
    return m->structs.len;
}

void mod_push_stmt(Module *m, Stmt *stmt) {
    if (ast_is_import_stmt(stmt)) {
        int32_t idx = m->statements.len;
        vec_push(&m->imports, (void *) &idx);
    } else if (ast_is_func_decl_stmt(stmt)) {
        int32_t idx = m->statements.len;
        vec_push(&m->functions, (void *) &idx);
    } else if (ast_is_struct_decl_stmt(stmt)) {
        int32_t idx = m->statements.len;
        vec_push(&m->structs, (void *) &idx);
    }

    ptrvec_push_ptr(&m->statements, (void *) stmt);
}

Stmt *mod_get_stmt(Module *m, int32_t i) {
    return (Stmt *) ptrvec_get(&m->statements, i);
}

void mod_free(Module *m) {
    int32_t i = 0;
    while (i < m->statements.len) {
        ast_stmt_free(mod_get_stmt(m, i));
        i++;
    }

    ptrvec_free(&m->statements);
    vec_free(&m->imports);
    vec_free(&m->functions);
    vec_free(&m->structs);
    ty_type_free((Ty *) m->ty);
}

ModuleMap mod_map_with_cap(int32_t size) {
    ModuleMap modmap = {
        .mods = ptrvec_with_cap(size),
        .mod_paths = map_with_cap(size)
    };

    return modmap;
}

int32_t mod_num_mods(ModuleMap *mm) {
    return mm->mods.len;
}

void mod_add_mod(ModuleMap *mm, Module *m) {
    int32_t i = mm->mods.len;
    ptrvec_push_ptr(&mm->mods, (void *) m);
    m->idx = i;

    void *idx = int2ptr(i + 1);
    Key key = map_create_key(m->path.len, m->path.inner);
    map_insert(&mm->mod_paths, key, idx);
}

Module *mod_get_mod(ModuleMap *mm, int32_t i) {
    return (Module *) ptrvec_get(&mm->mods, i);
}

int32_t mod_get_mod_idx(ModuleMap *mm, Path *abs_path) {
    Key key = map_create_key(abs_path->len, abs_path->inner);
    void *ptr = map_get(&mm->mod_paths, key);

    if (ptr == NULL) {
        return -1;
    }

    return ptr2int(ptr) - 1;
}

Module *mod_get_mod_by_path(ModuleMap *mm, Path *abs_path) {
    int32_t idx = mod_get_mod_idx(mm, abs_path);
    if (idx < 0) {
        return NULL;
    }

    return mod_get_mod(mm, idx);
}

void mod_free_map(ModuleMap *mm) {
    int32_t i = 0;
    while (i < mod_num_mods(mm)) {
        Module *mod = mod_get_mod(mm, i);
        mod_free(mod);
        free((void *) mod);

        i++;
    }

    ptrvec_free(&mm->mods);
    map_free(&mm->mod_paths);
}

int32_t mod_get_abs_import_path(Module *m, ImportStmt *imp, SpanInterner *si, PathBuf *dest) {
    int32_t len = span_get(si, imp->mod.ident_span).len;
    Path imp_path = path_create(imp->mod.ident, len);
    Path dir = path_parent(&m->path);

    return path_merge_abs_rel_suffix(&dir, &imp_path, SYNTHIUM_EXTENSION, dest);
}

Module *mod_try_get_mod_from_import(ModuleMap *mm, Module *m, SpanInterner *si, ImportStmt *imp, char **errdest) {
    int32_t plen = ident_len(&imp->mod, si);
    const char *path_str = fmt_str("%.*s%s", plen, imp->mod.ident, SYNTHIUM_EXTENSION);
    Path p = path_create(path_str, plen + strlen(SYNTHIUM_EXTENSION));
    Module *imported_mod = mod_get_mod_by_path(mm, &p);

    free((void *) path_str);

    if (imported_mod != NULL) {
        return imported_mod;
    }

    PathBuf abs = path_buf_from(path_empty());
    int32_t error = mod_get_abs_import_path(m, imp, si, &abs);

    if (error != 0) {
        Path parent = path_parent(&m->path);
        const char *mod_s = path_to_string(&parent);
        const char *imp_s = ident_to_string(&imp->mod, si);

        if (errdest != NULL) {
            char *err = (char *) fmt_str("%s: %s/%s%s", strerror(error), mod_s, imp_s, SYNTHIUM_EXTENSION);
            *errdest = err;
        }

        free((void *) imp_s);
        free((void *) mod_s);

        return NULL;
    }

    imported_mod = mod_get_mod_by_path(mm, &abs.inner);

    if (imported_mod != NULL) {
        if (errdest != NULL) {
            const char *s = path_to_string(&abs.inner);
            *errdest = (char *) fmt_str("%s is not being compiled", s);

            free((void *) s);
        }

        path_free(&abs);
        return NULL;
    }

    path_free(&abs);
    return imported_mod;
}
