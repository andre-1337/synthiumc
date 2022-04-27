#include "../include/span.h"

Span span_empty() {
    Span span = {
        .start = 0,
        .len_or_tag = 0,
        .ctx_or_idx = 0
    };

    return span;
}

Span span_merge(SpanInterner *si, Span first, Span second) {
    BigSpan one = span_get(si, first);
    BigSpan two = span_get(si, second);

    if (one.ctx != two.ctx) {
        return span_empty();
    }

    return span_create(si, one.start, two.start + two.len, one.ctx);
}

Span span_create(SpanInterner *si, uint32_t start, uint32_t end, uint16_t ctx) {
    if (end < start) {
        uint32_t t = start;
        start = end;
        end = t;
    }

    uint32_t len = end - start;
    if (len < MAX_LEN && ctx < MAX_CTX) {
        Span span = {
            .start = start,
            .len_or_tag = len,
            .ctx_or_idx = ctx
        };

        return span;
    } else {
        BigSpan big = {
            .start = start,
            .len = len,
            .ctx = ctx
        };

        uint32_t i = span_intern(si, &big);

        Span span = {
            .start = start,
            .len_or_tag = TAG_INTERNED,
            .ctx_or_idx = i
        };

        return span;
    }

    // unreachable
    Span span = {
        .start = 0,
        .len_or_tag = 0,
        .ctx_or_idx = 0
    };

    return span;
}

bool span_is_interned(Span *span) {
    return span->len_or_tag == TAG_INTERNED;
}

SpanInterner span_create_interner() {
    SpanInterner interner = {
        .spans = vec_create(sizeof(BigSpan))
    };

    return interner;
}

uint32_t span_intern(SpanInterner *si, BigSpan *span) {
    uint32_t idx = si->spans.len;
    vec_push(&si->spans, (void *) span);

    return idx;
}

BigSpan span_get(SpanInterner *si, Span span) {
    if (!span_is_interned(&span)) {
        BigSpan big = {
            .start = span.start,
            .len = span.len_or_tag,
            .ctx = span.ctx_or_idx
        };

        return big;
    }

    return span_get_idx(si, span.ctx_or_idx);
}

BigSpan span_get_idx(SpanInterner *si, uint16_t idx) {
    BigSpan big = {
        .start = 0,
        .len = 0,
        .ctx = 0
    };

    vec_get(&si->spans, idx, (void *) &big);

    return big;
}

void span_free_interner(SpanInterner *si) {
    vec_free(&si->spans);
}
