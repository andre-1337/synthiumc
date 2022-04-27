#include "../include/vec.h"

Vec vec_create(int64_t elem_size) {
    Vec vec = { 
        .len = 0, 
        .cap = 0, 
        .elem_size = elem_size, 
        .elements = NULL
    };

    return vec;
}

Vec vec_with_cap(int64_t elem_size, int64_t cap) {
    Vec vec = {
        .len = 0,
        .cap = cap,
        .elem_size = elem_size,
        .elements = malloc(cap * elem_size)
    };

    return vec;
}

void vec_push(Vec *v, void *elem) {
    if (v->len + 1 > v->cap) vec_resize(v);
    memcpy(v->elements + v->len * v->elem_size, elem, v->elem_size);

    v->len++;
}

void vec_init_zero(Vec *v) {
    v->len = v->cap;
    memset(v->elements, 0, v->elem_size * v->cap);
}

bool vec_get(Vec *v, int64_t i, void *ptr) {
    if (i >= 0 && i < v->len) {
        memcpy(ptr, v->elements + i * v->elem_size, v->elem_size);
        return true;
    }

    return false;
}

void *vec_get_ptr(Vec *v, int64_t i) {
    if (i >= 0 && i < v->len) {
        return v->elements + i * v->elem_size;
    }

    return NULL;
}

void vec_resize(Vec *v) {
    if (v->elements == NULL) {
        v->cap = 2;
        v->elements = malloc(v->cap * v->elem_size);
    } else {
        v->cap *= 2;
        v->elements = realloc(v->elements, v->cap * v->elem_size);
    }
}

void vec_free(Vec *v) {
    free(v->elements);
}
