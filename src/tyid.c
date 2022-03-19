#include "../include/tyid.h"

Type type_empty() {
    return type_create(ident_empty());
}

bool type_is_empty(Type *t) {
    Type empty = type_empty();
    memset((void *) &empty, 0, sizeof(Ident));
    return memcmp((void *) t, (void *) &empty, sizeof(Type)) == 0;
}

Type type_create(Ident ident) {
    Type type = {
        .pointer_count = 0,
        .ident = ident
    };

    return type;
}

Span type_span(Type *t) {
    return t->ident.ident_span;
}

Type type_create_ptr(Ident ident, int32_t pointer_count) {
    Type ptr = {
        .pointer_count = pointer_count,
        .ident = ident
    };

    return ptr;
}

bool type_is_ptr(Type *t) {
    return t->pointer_count > 0;
}

const char *type_to_string(Type *t, SpanInterner *si) {
    char *ident = (char *) ident_to_string(&t->ident, si);
    int32_t ident_len = strlen(ident);
    int32_t len = t->pointer_count + ident_len + 1;
    void *buffer = malloc(len);

    memset(buffer, chr2int('*'), t->pointer_count);
    memcpy(buffer + t->pointer_count, (void *) ident, ident_len);
    free((void *) ident);

    char *buf = buffer;
    buf[len - 1] = '\0';

    return (const char *) buf;
}