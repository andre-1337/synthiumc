#pragma once

#include <stdlib.h>

#include "path.h"
#include "utils.h"

typedef struct File {
    PathBuf path;
} File;

File file_create(PathBuf path);
int32_t file_read_to_string(File *s, char *const dest);
File file_empty();
char *const file_name_dup(File *f);
void file_free(File *f);