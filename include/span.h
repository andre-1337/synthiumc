#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "vec.h"

#define TAG_INTERNED 32768
#define MAX_LEN 32767
#define MAX_CTX 65535

typedef struct Span {
    int32_t start;
    int32_t len_or_tag;
    int32_t ctx_or_idx;
} Span;

typedef struct BigSpan {
    int32_t start;
    int32_t len;
    int32_t ctx;
} BigSpan;

typedef struct SpanInterner {
    Vec spans;
} SpanInterner;

Span empty_span();
Span merge(SpanInterner *si, Span first, Span second);
Span create(SpanInterner *si, int32_t start, int32_t end, int32_t ctx);
bool is_interned(Span *span);
SpanInterner create_interner();
int32_t intern(SpanInterner *si, BigSpan *span);
BigSpan get(SpanInterner *si, Span span);
BigSpan get_idx(SpanInterner *si, int32_t idx);
void free_interner(SpanInterner *si);