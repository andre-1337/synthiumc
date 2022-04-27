#include "../include/ptrvec.h"

Ptrvec ptrvec_create() {
    Ptrvec ptrvec = {
        .len = 0,
        .cap = 0,
        .elements = NULL
    };

    return ptrvec;
}

Ptrvec ptrvec_with_cap(int64_t cap) {
    Ptrvec ptrvec = {
        .len = 0,
        .cap = cap,
        .elements = (void **) calloc(cap, sizeof(void *))
    };

    return ptrvec;
}

bool ptrvec_is_empty(Ptrvec *v) {
    return v->len <= 0;
}

void *ptrvec_pop(Ptrvec *v) {
    if (!ptrvec_is_empty(v)) {
        void *elem = ptrvec_get(v, 0);
        v->len--;

        memmove((void *) v->elements, (void *)(v->elements + 1), v->len * sizeof(void **));

        return elem;
    }

    return NULL;
}

void ptrvec_append(Ptrvec *v, Ptrvec *other) {
    if (ptrvec_is_empty(v)) return;

    int64_t new_len = v->len + other->len;
    ptrvec_resize_s(v, new_len);

    memcpy((void *)(v->elements + v->len), (void *) other->elements, other->len * sizeof(void *));

    v->len = new_len;
}

void ptrvec_push_ptr(Ptrvec *v, void *elem) {
    if (v->len + 1 > v->cap) ptrvec_resize(v);

    v->elements[v->len] = elem;
    v->len++;
}

void *ptrvec_get(Ptrvec *v, int64_t i) {
    if (i < v->len) return v->elements[i];
    return NULL;
}

void ptrvec_set(Ptrvec *v, int64_t i, void *elem) {
    if (i < v->len) v->elements[i] = elem;
}

void ptrvec_resize_s(Ptrvec *v, int64_t size) {
    size = (size * sizeof(void **));

    if (size <= v->cap) return;
    v->cap = size;

    if (v->elements == NULL) v->elements = (void **) malloc(v->cap * sizeof(void **));
    else v->elements = (void **) realloc((void *) v->elements, v->cap * sizeof(void **));
}

void ptrvec_resize(Ptrvec *v) {
    if (v->elements == NULL) ptrvec_resize_s(v, 2);
    else ptrvec_resize_s(v, v->cap * 2);
}

void ptrvec_free(Ptrvec *v) {
    int64_t i = 0;
    for (i = 0; i < v->len; i++) {
        free((void *) v->elements[i]);
    }
}
