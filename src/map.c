#include "../include/map.h"
#include "../include/span.h"
#include "../include/ident.h"

int32_t map_hash(const char *key, int32_t len) {
    int32_t h = 0;

    for (int32_t i = 0; i < len; i++) {
        h = h * HASH_NUM + key[i];
    }

    return h;
}

int32_t map_get_idx(int32_t cap, const char *key, int32_t key_len) {
    int32_t h = map_hash(key, key_len);
    return h & (cap - 1);
}

Key map_key_from_ident(SpanInterner *si, Ident *ident) {
    uint16_t len = span_get(si, ident->ident_span).len;
    return map_create_key(len, ident->ident);
}

Key map_create_key(uint16_t len, const char *value) {
    Key key = {
        .len = len,
        .value = value
    };

    return key;
}

Map map_create() {
    Map map = {
        .buckets = NULL,
        .len = 0,
        .cap = 0
    };

    return map;
}

Map map_with_cap(int32_t cap) {
    if ((cap % 2) != 0) cap = next_pow_of_2(cap);

    Map map = {
        .buckets = (Bucket *) calloc(cap, sizeof(Bucket)),
        .len = 0,
        .cap = cap
    };

    return map;
}

void map_free(Map *m) {
    int32_t i = 0;
    while (i < m->cap) {
        map_free_buckets(m->buckets + i);
        i++;
    }

    if (m->buckets != NULL) {
        free((void *) m->buckets);
    }
}

void map_free_buckets(Bucket *buckets) {
    Bucket *last = buckets->next;

    while (last != NULL) {
        Bucket *t = last->next;
        free((void *) last);
        last = t;
    }
}

bool map_insert(Map *map, Key key, void *value) {
    map_resize_if_needed(map);

    Bucket *b = map->buckets + map_get_idx(map->cap, key.value, key.len);
    Item item = {
        .key = key,
        .value = value
    };

    if (map_bucket_is_empty(b)) {
        b->item = item;
        return false;
    }

    while (b != NULL) {
        if (map_bucket_is_empty(b)) {
            Item itm = {
                .key = key,
                .value = value
            };

            b->item = itm;
            return false;
        }

        if (map_key_eq(&b->item.key, &key)) {
            b->item.value = value;
            return true;
        }

        if (b->next == NULL) {
            Bucket *bucket = (Bucket *) malloc(sizeof(Bucket));
            bucket->item = item;
            bucket->next = NULL;

            b->next = bucket;
            return false;
        }

        b = b->next;
    }

    abort();
    return false;
}

void *map_get(Map *map, Key key) {
    if (map->buckets == NULL) return NULL;

    Bucket *b = map->buckets + map_get_idx(map->cap, key.value, key.len);

    while (b != NULL) {
        if (map_bucket_is_empty(b)) return NULL;
        if (map_key_eq(&b->item.key, &key)) return b->item.value;

        b = b->next;
    }

    return NULL;
}

void map_resize_if_needed(Map *map) {
    int32_t cap = map->cap;
    if (cap == 0) {
        map->cap = cap = 16;
        map->buckets = (Bucket *) calloc(cap, sizeof(Bucket));
    }
    
    if (int2flt(map->len) > LOAD_FACTOR * int2flt(cap)) {
        cap *= 2;

        Bucket *new_buckets = (Bucket *) calloc(map->cap, sizeof(Bucket));
        Map new_map = {
            .buckets = new_buckets,
            .len = map->len,
            .cap = cap
        };
        map_insert_all(map, &new_map);
        *map = new_map;
    }
}

bool map_key_eq(Key *first, Key *second) {
    if (first->len != second->len) return false;
    return strncmp(first->value, second->value, first->len) == 0;
}

bool map_bucket_is_empty(Bucket *b) {
    return b->item.key.value == NULL;
}

void map_insert_all(Map *old_map, Map *new_map) {
    int32_t i = 0;
    while (i < old_map->cap) {
        Bucket *b = old_map->buckets + i;

        while (b != NULL) {
            if (map_bucket_is_empty(b)) {
                map_insert(new_map, b->item.key, b->item.value);
            }

            b = b->next;
        }

        i++;
    }
}