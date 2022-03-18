#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "path.h"
#include "utils.h"

typedef struct File {
    PathBuf path;
} File;

File file_create(PathBuf path);
int32_t file_read_to_string(File *s, const char *dest);
File file_empty();
const char *file_name_dup(File *f);
void file_free(File *f);