#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "./vec.h"
#include "./utils.h"
#include "./source.h"

typedef struct FileAddResult {
    const char *file_name;
    int32_t err_code;
} FileAddResult;

typedef struct FileMap {
    const char *home_dir;
    Vec files;
} FileMap;

FileMap reader_create();
int32_t reader_num_files(FileMap *fm);
SourceFile reader_get_by_idx(FileMap *fm, int32_t idx);
SourceFile *reader_get_ptr_by_idx(FileMap *fm, int32_t idx);
void reader_free_fm(FileMap *fm);
FileAddResult reader_add_std_lib(FileMap *fm, Path *bin_path);
FileAddResult reader_add_all(FileMap *fm, Path *bin_path, int32_t len, const char **file_names);
int32_t reader_add_file(FileMap *fm, Path *bin_path, const char *name);