#include "../include/tuple.h"

void tuple_swap(Tuple *first, Tuple *second) {
    Tuple temp = *first;
    *first = *second;
    *second = temp;
}

void tuple_inner_sort(Tuple *arr, int32_t len) {
    if (len <= 1) return;

    Tuple *pivot = arr + len - 1;
    int32_t j = len - 1;
    int32_t i = -1;

    while (true) {
        while (arr[++i].value < pivot->value);
        while (arr[--j].value > pivot->value && j > 0);

        if (i >= j) break;

        tuple_swap(arr + i, arr + j);
    }
}

int32_t *tuple_isort(const int32_t *import_nums, int32_t len) {
    Tuple *tuples = malloc(len * sizeof(Tuple));

    for (int32_t i = 0; i < len; i++) {
        tuples[i] = (Tuple) {
            .orig_idx = i,
            .value = import_nums[i]
        };
    }

    tuple_inner_sort(tuples, len);

    int32_t *result = malloc(len * sizeof(int32_t));
    for (int32_t i = 0; i < len; i++) {
        result[i] = tuples[i].orig_idx;
    }

    free(tuples);

    return result;
}
