#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "vec.h"
#include "map.h"
#include "tyid.h"
#include "span.h"
#include "ident.h"

typedef struct Field {
    Ident ident;
    Type ty;
} Field;

typedef struct Fields {
    Map field_map;
    Vec fields;
} Fields;

typedef struct StructDecl {
    Ident name;
    Fields fields;
} StructDecl;

Field record_field_empty();
Field record_field_create(Ident ident, Type ty);
Fields record_fields_create(SpanInterner *si, Vec fields);
bool record_fields_get_field_at(Fields *fs, int32_t i, Field *dest);
char *const record_fields_to_string(Fields *fs, SpanInterner *si);
void record_fields_free(Fields *fs);
StructDecl record_struct_create(SpanInterner *si, Ident name, Vec fields);
int32_t record_num_fields(StructDecl *s);
bool record_field_at(StructDecl *s, int32_t i, Field *dest);
bool record_field_by_ident(StructDecl *s, Ident name, SpanInterner *si, Field *dest);
char *const record_to_string(StructDecl *s, SpanInterner *si);
void record_struct_free(StructDecl *s);