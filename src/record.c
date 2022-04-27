#include "../include/record.h"

Field record_field_empty() {
    Field field = {
        .ident = ident_empty(),
        .ty = type_empty()
    };

    return field;
}

Field record_field_create(Ident ident, Type ty) {
    Field field = {
        .ident = ident,
        .ty = ty
    };

    return field;
}

void record_fields_free(Fields *fs) {
    map_free(&fs->field_map);
    vec_free(&fs->fields);
}

Fields record_fields_create(SpanInterner *si, Vec fields) {
    Map field_map = map_with_cap(fields.len);
    int32_t i = 0;

    while (i < fields.len) {
        Field field = record_field_empty();
        vec_get(&fields, i, (void *) &field);

        const char *s = type_to_string(&field.ty, si);
        free((void *) s);

        Key key = map_key_from_ident(si, &field.ident);
        void *idx = int2ptr(i) + 1;

        map_insert(&field_map, key, idx);
        i++;
    }

    Fields f = {
        .field_map = field_map,
        .fields = fields
    };

    return f;
}

bool record_fields_get_field_at(Fields *fs, int32_t i, Field *dest) {
    return vec_get(&fs->fields, i, (void *) dest);
}

const char *record_fields_to_string(Fields *fs, SpanInterner *si) {
    char *fields = "";
    int32_t i = 0;

    while (i < fs->fields.len) {
        Field f = record_field_empty();
        if (!record_fields_get_field_at(fs, i, &f)) {
            free((void *) fields);
            return NULL;
        }

        const char *n = ident_to_string(&f.ident, si);
        const char *t = type_to_string(&f.ty, si);
        int32_t extra_len = 2;

        if (i > 0) {
            extra_len = 4;
        }

        int32_t new_len = strlen(fields) + strlen(n) + strlen(t) + 1 + extra_len;
        char *new_fields = (char *) malloc(new_len);
        char *old_fields = fields;

        fields = strcpy(new_fields, fields);
        free((void *) old_fields);

        if (extra_len > 2) {
            fields = strcat(fields, ", ");
        }

        fields = strcat(fields, n);
        fields = strcat(fields, ": ");
        fields = strcat(fields, t);
        fields = strcat(fields, "\0");

        free((void *) n);
        free((void *) t);

        i++;
    }

    return fields;
}

StructDecl record_struct_create(SpanInterner *si, Ident name, Vec fields) {
    Fields f = record_fields_create(si, fields);
    StructDecl s = {
        .name = name,
        .fields = f
    };

    return s;
}

void record_struct_free(StructDecl *s) {
    record_fields_free(&s->fields);
}

int32_t record_num_fields(StructDecl *s) {
    return s->fields.fields.len;
}

bool record_field_at(StructDecl *s, int32_t i, Field *dest) {
    return record_fields_get_field_at(&s->fields, i, dest);
}

bool record_field_by_ident(StructDecl *s, Ident name, SpanInterner *si, Field *dest) {
    void *ptr = map_get(&s->fields.field_map, map_key_from_ident(si, &name));
    if (ptr == NULL) return false;

    int32_t idx = ptr2int(ptr) - 1;
    return record_field_at(s, idx, dest);
}

const char *record_to_string(StructDecl *s, SpanInterner *si) {
    const char *name = ident_to_string(&s->name, si);
    const char *fields = record_fields_to_string(&s->fields, si);
    const char *str = fmt_str("type %s struct { %s }", name, fields);

    free((void *) name);
    free((void *) fields);

    return str;
}
