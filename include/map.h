#pragma once

#include <stdint.h>

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
