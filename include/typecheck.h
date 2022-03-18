#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ast.h"
#include "map.h"
#include "mod.h"
#include "vec.h"
#include "path.h"
#include "span.h"
#include "scope.h"
#include "ident.h"
#include "ptrvec.h"
#include "record.h"
#include "ty_types.h"

typedef enum WaitingType {
    WAITING_STRUCT = 1
} WaitingType;

typedef struct WaitingRequest {
    WaitingType kind;
    int32_t field_idx;
    Ty *to_fill;
    Module *to_fill_mod;
    Ty *waiting_for_ty;
    Module *waiting_for_mod;
} WaitingRequest;

typedef struct WaitingRequestMap {
    int32_t init_flag;
    Vec requests;
    Map request_map;
} WaitingRequestMap;

typedef struct TypeError {
    char *const text;
    Span span;
} TypeError;

typedef struct Ctx {
    Module *mod;
    Map imports;
    ScopeStack scopes;
} Ctx;

typedef struct TypeChecker {
    SpanInterner *si;
    Map *mods;
    int32_t *sorted_mods;
    Ptrvec temp_types;
    Ctx ctx;
    Scope globals;
    Vec errors;
    Vec requests;
} TypeChecker;

WaitingRequest typecheck_create_waiting_request(WaitingType kind, Ty *to_fill, Module *to_fill_mod, Ty *waiting_for_ty, Module *waiting_for_mod, int32_t field_idx);
Ident *typecheck_req_waiting_for_ident(WaitingRequest *r);

WaitingRequestMap typecheck_create_waiting_request_map();
void typecheck_add_request(WaitingRequestMap *wrm, SpanInterner *si, WaitingRequest request);
Vec *typecheck_get_waiting(WaitingRequestMap *wrm, SpanInterner *si, Ident *ident);
void typecheck_free_wait_map(WaitingRequestMap *wrm);

TypeError typecheck_err(TypeChecker *tc, char *const text, Span span);
void typecheck_free_err(TypeError *err);

Ctx typecheck_empty_ctx();
Ctx typecheck_create_ctx(Module *mod, SpanInterner *si, Scope *global);
void typecheck_free_ctx(Ctx *ctx);
Module *typecheck_get_mod_by_alias(Ctx *ctx, int32_t alias_len, char *const alias);

TypeChecker typecheck_create(SpanInterner *si, ModuleMap *mods);
void typecheck_wait_for(TypeChecker *tc, WaitingRequest req);
WaitingRequestMap *typecheck_get_waiting_map(TypeChecker *tc, Module *mod);
Scope typecheck_create_global_scope(TypeChecker *tc);
int32_t typecheck_num_errs(TypeChecker *tc);
void typecheck_push_err(TypeChecker *tc, TypeError error);
void typecheck_push_mk_error(TypeChecker *tc, char *const string, Span span);
TypeError *typecheck_get_err(TypeChecker *tc, int32_t i);
void typecheck_add_import_alias(TypeChecker *tc, Ident *ident, Module *mod);
int32_t *typecheck_sorted_mods(ModuleMap *mods, SpanInterner *si);
Mod *typecheck_make_mod_type(TypeChecker *tc, Module *mod, SpanInterner *si);
Ty *typecheck_lookup_ident(TypeChecker *tc, Ident *ident);
Ty *typecheck_lookup_ident_mod(TypeChecker *tc, Ident *ident, Module **out_mod);
void typecheck_check(TypeChecker *tc);
Mod *typecheck_check_mod(TypeChecker *tc, Module *mod);
Ty *typecheck_push_tmp_ty(TypeChecker *tc, Ty *ty);
void typecheck_bind(TypeChecker *tc, Ident *ident, Ty *ty);
void typecheck_fill_struct_fields(TypeChecker *tc, StructDecl *s_decl, Struct *s_ty);
void typecheck_update_waiting(TypeChecker *tc, Module *mod, Ty *resolved_ty, Ident *ident);
Ident *typecheck_get_import_alias(TypeChecker *tc, ImportStmt *imp);
Stmt *typecheck_check_stmt(TypeChecker *tc, Stmt *s);
Expr *typecheck_check_expr(TypeChecker *tc, Expr *e);
void typecheck_free_tc(TypeChecker *tc);
