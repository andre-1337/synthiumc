#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ty.h"
#include "ast.h"

#define SYNTHIUM_EXTENSION ".syn"

typedef struct Module {
    Ptrvec statements;
    Path path;
    Vec imports;
    Vec functions;
    Vec structs;
    Mod *ty;
    int32_t idx;
} Module;

typedef struct ModuleMap {
    Ptrvec mods;
    Map mod_paths;
} ModuleMap;

Module *mod_create(Path p);
Ty *mod_s_lookup(Module *m, int32_t key_len, const char *key);
Stmt *mod_get_stmt_at(Module *m, int32_t i);
void mod_set_stmt_at(Module *m, int32_t i, Stmt *s);
int32_t mod_num_stmts(Module *m);
Stmt *mod_get_special_stmt_at(Module *m, Vec *indices, int32_t i);
ImportStmt *mod_get_import_at(Module *m, int32_t i);
int32_t mod_num_imports(Module *m);
FuncDeclStmt *mod_get_function_at(Module *m, int32_t i);
int32_t mod_num_functions(Module *m);
StructDeclStmt *mod_get_struct_at(Module *m, int32_t i);
int32_t mod_num_structs(Module *m);
void mod_push_stmt(Module *m, Stmt *stmt);
Stmt *mod_get_stmt(Module *m, int32_t i);
void mod_free(Module *m);
ModuleMap mod_map_with_cap(int32_t size);
int32_t mod_num_mods(ModuleMap *mm);
void mod_add_mod(ModuleMap *mm, Module *m);
Module *mod_get_mod(ModuleMap *mm, int32_t i);
int32_t mod_get_mod_idx(ModuleMap *mm, Path *abs_path);
Module *mod_get_mod_by_path(ModuleMap *mm, Path *abs_path);
void mod_free_map(ModuleMap *mm);
int32_t mod_get_abs_import_path(Module *m, ImportStmt *imp, SpanInterner *si, PathBuf *dest);
Module *mod_try_get_mod_from_import(ModuleMap *mm, Module *m, SpanInterner *si, ImportStmt *imp, char **errdest);
