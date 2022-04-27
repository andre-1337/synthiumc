#ifndef SYNTHIUMC_MAP_H
#define SYNTHIUMC_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "span.h"
#include "ident.h"
#include "utils.h"

#define HASH_NUM 65599
#define LOAD_FACTOR 0.75

typedef struct Key {
    int32_t len;
    const char *value;
} Key;

typedef struct Item {
    Key key;
    void *value;
} Item;

typedef struct Bucket {
    Item item;
    struct Bucket *next;
} Bucket;

typedef struct Map {
    Bucket *buckets;
    int32_t len;
    int32_t cap;
} Map;

int32_t map_hash(const char *key, int32_t len);
int32_t map_get_idx(int32_t cap, const char *key, int32_t key_len);
Key map_key_from_ident(SpanInterner *si, Ident *ident);
Key map_create_key(uint16_t len, const char *value);
Map map_create();
Map map_with_cap(int32_t cap);
void map_free(Map *m);
void map_free_buckets(Bucket *buckets);
bool map_insert(Map *map, Key key, void *value);
void *map_get(Map *map, Key key);
void map_resize_if_needed(Map *map);
bool map_key_eq(Key *first, Key *second);
bool map_bucket_is_empty(Bucket *b);
void map_insert_all(Map *old_map, Map *new_map);

#endif
