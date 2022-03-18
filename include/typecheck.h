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

