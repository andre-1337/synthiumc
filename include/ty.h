#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "vec.h"
#include "span.h"
#include "ident.h"
#include "scope.h"
#include "ptrvec.h"
#include "ty_types.h"

#define WIDTH_UNKNOWN -1
#define FLAG_SCOPED 1
#define FLAG_UNSIZED 2
#define FLAG_PLACEHOLDER 4

struct Mod;
struct Scope;

typedef struct Ty {
    TyTypes kind;
    int32_t width;
    int32_t align;
    uint32_t flags;
} Ty;

typedef struct I32 {
    Ty t;
} I32;

typedef struct Ptr {
    Ty t;
    int32_t count;
    Ty *inner;
} Ptr;

typedef struct String {
    Ty t;
} String;

typedef struct StructField {
    Ident name;
    Ty *ty;
} StructField;

typedef struct Struct {
    Ty t;
    Ident name;
    Vec fields;
} Struct;

typedef struct TypeList {
    Ptrvec types;
} TypeList;

typedef struct Func {
    Ty t;
    Ty *ret;
    TypeList params;
    Ident name;
} Func;

typedef struct Mod {
    Ty t;
    Scope scope;
} Mod;

Ty *ty_new_i32();
bool ty_is_i32(Ty *t);
I32 *ty_as_i32(Ty *t);

Ty *ty_new_ptr(int32_t count, Ty *inner);
bool ty_is_ptr(Ty *t);
Ptr *ty_as_ptr(Ty *t);

Ty *ty_new_string();
bool ty_is_string(Ty *t);
String *ty_as_string(Ty *t);

Ty *ty_new_struct(Ident name, Vec fields);
void ty_init_struct(Ty *t, Ident name);
bool ty_is_struct(Ty *t);

Struct *ty_as_struct(Ty *t);
void ty_push_field(Struct *t, Ident name, Ty *ty);
StructField *ty_field_at(Struct *t, int32_t i);
int32_t ty_num_fields(Struct *t);

Ty *ty_new_func(Ty *ret, Ptrvec params, Ident name);
bool ty_is_func(Ty *t);
Func *ty_as_func(Ty *t);

Ty *ty_new_mod();
bool ty_is_mod(Ty *t);
Mod *ty_as_mod(Ty *t);

Ty *ty_new_placeholder_type(TyTypes kind, int32_t size);
Ty ty_create_type(TyTypes ty);
bool ty_is_initialized(Ty *t);
bool ty_is_scoped(Ty *t);

TypeList ty_new_empty_list();
TypeList ty_create_type_list(Ptrvec types);
Ty *ty_type_at(TypeList *tl, int32_t i);
void ty_type_list_free(TypeList *tl);
void ty_type_free(Ty *t);

bool ty_width_was_calculated(Ty *t);
bool ty_fill_width_align(Ty *t);
const char *ty_to_string(Ty *t, SpanInterner *si);
Ty *ty_clone(Ty *t);