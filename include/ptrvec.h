#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Ptrvec {
    int32_t len;
    int32_t cap;
    void **elements;
} Ptrvec;

Ptrvec ptrvec_create();
Ptrvec ptrvec_with_cap(int32_t cap);
bool ptrvec_is_empty(Ptrvec *v);
void *ptrvec_pop(Ptrvec *v);
void ptrvec_append(Ptrvec *v, Ptrvec *other);
void ptrvec_push_ptr(Ptrvec *v, void *elem);
void *ptrvec_get(Ptrvec *v, int32_t i);
void ptrvec_set(Ptrvec *v, int32_t, void *elem);
void ptrvec_resize_s(Ptrvec *v, int32_t size);
void ptrvec_resize(Ptrvec *v);
void ptrvec_free(Ptrvec *v);