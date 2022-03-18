#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Tuple {
    int32_t orig_idx;
    int32_t value;
} Tuple;

void tuple_swap(Tuple *first, Tuple *second);
void tuple_inner_sort(Tuple *arr, int32_t len);
int32_t *tuple_isort(const int32_t *import_nums, int32_t len);