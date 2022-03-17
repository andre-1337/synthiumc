#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "span.h"
#include "ident.h"
#include "utils.h"

#define LOAD_FACTOR 0.75

typedef struct Key {
    int32_t len;
    char *const value;
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

int32_t hash(char *const key);
int32_t get_map_idx(int32_t cap, char *const key, int32_t key_len);
Key key_from_ident(SpanInterner *si, Ident *ident);
Key create_key(int32_t len, char *const value);
Map create_map();
Map map_with_cap(int32_t cap);
void free_m(Map *m);
void free_buckets(Bucket *buckets);
bool insert(Map *map, Key key, void *value);
void *get(Map *map, Key key);
void resize_if_needed(Map *map);
bool key_eq(Key *first, Key *second);
bool bucket_is_empty(Bucket *b);
void insert_all(Map *old_map, Map *new_map);