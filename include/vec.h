#ifndef SYNTHIUMC_VEC_H
#define SYNTHIUMC_VEC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct Vec {
    int64_t len;
    int64_t cap;
    int64_t elem_size;
    void *elements;
} Vec;

Vec vec_create(int64_t elem_size);
Vec vec_with_cap(int64_t elem_size, int64_t cap);
void vec_push(Vec *v, void *elem);
bool vec_get(Vec *v, int64_t i, void *ptr);
void *vec_get_ptr(Vec *v, int64_t i);
void vec_resize(Vec *v);
void vec_init_zero(Vec *v);
void vec_free(Vec *v);

#endif
