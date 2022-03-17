#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Vec {
    int32_t len;
    int32_t cap;
    int32_t elem_size;
    void *elements;
} Vec;

Vec vec_create(int32_t elem_size);
Vec vec_with_cap(int32_t elem_size, int32_t cap);
void vec_push(Vec *v, void *elem);
bool vec_get(Vec *v, int32_t i, void *ptr);
void *vec_get_ptr(Vec *v, int32_t i);
void vec_resize(Vec *v);
void vec_init_zero(Vec *v);
void vec_free(Vec *v);