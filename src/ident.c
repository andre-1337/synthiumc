#include "../include/ident.h"

Ident ident_create(Token t) {
    Ident ident = {
        .ident = t.lexeme,
        .ident_span = t.span
    };

    return ident;
}

Ident ident_from_str(Span span, const char *str) {
    Ident ident = {
        .ident = str,
        .ident_span = span
    };

    return ident;
}

Ident ident_empty() {
    return ident_create(lexer_empty_token());
}

int32_t ident_len(Ident *id, SpanInterner *si) {
    return span_get(si, id->ident_span).len;
}

char ident_char_at(Ident *id, int32_t pos) {
    return id->ident[pos];
}

int32_t ident_index_of(Ident *id, int32_t ident_len, char c) {
    int32_t i = 0;
    while (i < ident_len) {
        if (ident_char_at(id, i) == '.') {
            return i;
        }

        i++;
    }

    return -1;
}

const char *ident_to_string(Ident *id, SpanInterner *si) {
    uint16_t len = span_get(si, id->ident_span).len;
    return strndup(id->ident, len);
}
