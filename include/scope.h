#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "vec.h"
#include "map.h"
#include "span.h"
#include "ident.h"

struct Ty;

typedef struct Scope {
    Map bindings;
} Scope;

typedef struct ScopeStack {
    Vec scopes;
    SpanInterner *si;
} ScopeStack;

Scope scope_create();
void scope_free(Scope *s);
bool scope_bind_in(Scope *s, SpanInterner *si, Ident ident, Ty value);
bool scope_s_bind_in(Scope *s, int32_t ident_len, char *const ident, Ty value);
Ty *scope_get_in(Scope *s, SpanInterner *si, Ident *ident);
Ty *scope_s_get_in(Scope *s, int32_t key_len, char *const key);

ScopeStack scope_empty_stack();
ScopeStack scope_create_stack(SpanInterner *si);
void scope_free_stack(ScopeStack *s);
void scope_open(ScopeStack *s);
void scope_push(ScopeStack *s, Scope *scope);
void scope_close(ScopeStack *s);
Scope *scope_at(ScopeStack *s, int32_t i);
Scope *scope_top(ScopeStack *s);
bool scope_bind(ScopeStack *s, Ident *ident, Ty *value);
int32_t scope_num_scopes(ScopeStack *s);
Ty *scope_lookup(ScopeStack *s, Ident *ident);