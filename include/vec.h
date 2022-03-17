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

Vec create_vec(int32_t elem_size);
Vec vec_with_cap(int32_t elem_size, int32_t cap);
void push(Vec *v, void *elem);
bool get(Vec *v, int32_t i, void *ptr);
void *get_ptr(Vec *v, int32_t i);
void resize(Vec *v);
void init_zero(Vec *v);
void free_v(Vec *v);