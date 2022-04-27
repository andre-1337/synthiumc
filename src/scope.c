#include "../include/ty.h"
#include "../include/scope.h"

Scope scope_create() {
    Scope scope = {
        .bindings = map_create()
    };

    return scope;
}

void scope_free(Scope *s) {
    map_free(&s->bindings);
}

bool scope_bind_in(Scope *s, SpanInterner *si, Ident *ident, Ty *value) {
    return map_insert(&s->bindings, map_key_from_ident(si, ident), (void *) value);
}

bool scope_s_bind_in(Scope *s, int32_t ident_len, const char *ident, Ty *value) {
    return map_insert(&s->bindings, map_create_key(ident_len, ident), (void *) value);
}

Ty *scope_get_in(Scope *s, SpanInterner *si, Ident *ident) {
    return (Ty *) map_get(&s->bindings, map_key_from_ident(si, ident));
}

Ty *scope_s_get_in(Scope *s, int32_t key_len, const char *key) {
    return (Ty *) map_get(&s->bindings, map_create_key(key_len, key));
}

ScopeStack scope_empty_stack() {
    ScopeStack stack = {
        .scopes = vec_create(0),
        .si = NULL
    };

    return stack;
}

ScopeStack scope_create_stack(SpanInterner *si) {
    ScopeStack stack = {
        .scopes = vec_create(sizeof(Scope)),
        .si = si
    };

    return stack;
}

void scope_free_stack(ScopeStack *s) {
    int32_t i = 2;
    while (i < scope_num_scopes(s)) {
        scope_free(scope_at(s, i));
        i++;
    }

    vec_free(&s->scopes);
}

void scope_open(ScopeStack *s) {
    Scope scope = scope_create();
    vec_push(&s->scopes, (void *) &scope);
}

void scope_push(ScopeStack *s, Scope *scope) {
    vec_push(&s->scopes, (void *) scope);
}

void scope_close(ScopeStack *s) {
    scope_free(scope_top(s));
    s->scopes.len--;
}

Scope *scope_at(ScopeStack *s, int32_t i) {
    return (Scope *) vec_get_ptr(&s->scopes, i);
}

Scope *scope_top(ScopeStack *s) {
    return scope_at(s, s->scopes.len - 1);
}

bool scope_bind(ScopeStack *s, Ident *ident, struct Ty *value) {
    return scope_bind_in(scope_top(s), s->si, ident, value);
}

int32_t scope_num_scopes(ScopeStack *s) {
    return s->scopes.len;
}

Ty *scope_lookup(ScopeStack *s, Ident *ident) {
    int32_t i = scope_num_scopes(s) - 1;
    while (i >= 0) {
        Scope *scope = scope_at(s, i);
        Ty *ty = NULL;

        if ((ty = scope_get_in(scope, s->si, ident)) != NULL) {
            return ty;
        }

        i--;
    }

    return NULL;
}
