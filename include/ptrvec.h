#ifndef SYNTHIUMC_PTRVEC_H
#define SYNTHIUMC_PTRVEC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct Ptrvec {
    int64_t len;
    int64_t cap;
    void **elements;
} Ptrvec;

Ptrvec ptrvec_create();
Ptrvec ptrvec_with_cap(int64_t cap);
bool ptrvec_is_empty(Ptrvec *v);
void *ptrvec_pop(Ptrvec *v);
void ptrvec_append(Ptrvec *v, Ptrvec *other);
void ptrvec_push_ptr(Ptrvec *v, void *elem);
void *ptrvec_get(Ptrvec *v, int64_t i);
void ptrvec_set(Ptrvec *v, int64_t i, void *elem);
void ptrvec_resize_s(Ptrvec *v, int64_t size);
void ptrvec_resize(Ptrvec *v);
void ptrvec_free(Ptrvec *v);

#endif
