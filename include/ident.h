#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "span.h"
#include "lexer.h"

typedef struct Ident {
    char *const ident;
    Span ident_span;
} Ident;

Ident create_ident(Token t);
Ident empty_ident();
int32_t len(Ident *i, SpanInterner *si);
char char_at(Ident *i, int32_t pos);
int32_t index_of(Ident *i, int32_t ident_len, char c);
char *const to_string(Ident *i, SpanInterner *si);
Ident from_str(Span span, char *const str);