#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "span.h"
#include "ident.h"

typedef struct Type {
    int32_t pointer_count;
    Ident ident;
} Type;

Type type_empty();
bool type_is_empty(Type *t);
Type type_create(Ident ident);
Span type_span(Type *t);
Type type_create_ptr(Ident ident, int32_t pointer_count);
bool type_is_ptr(Type *t);
char *const type_to_string(Type *t, SpanInterner *si);