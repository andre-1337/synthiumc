#ifndef SYNTHIUMC_IDENT_H
#define SYNTHIUMC_IDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "span.h"
#include "lexer.h"

typedef struct Ident {
    const char *ident;
    Span ident_span;
} Ident;

Ident ident_create(Token t);
Ident ident_empty();
int32_t ident_len(Ident *id, SpanInterner *si);
char ident_char_at(Ident *id, int32_t pos);
int32_t ident_index_of(Ident *id, int32_t ident_len, char c);
const char *ident_to_string(Ident *id, SpanInterner *si);
Ident ident_from_str(Span span, const char *str);

#endif
