#pragma once

#include <stdlib.h>

#include "path.h"
#include "utils.h"

typedef struct File {
    PathBuf path;
} File;

File create(PathBuf path);
int32_t read_to_string(File *s, char *const dest);
File empty_file();
char *const name_dup(File *f);
void free_f(File *f);