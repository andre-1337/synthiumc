#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "error.h"

#define SYSTEM_SEPARATOR '/'

typedef struct Path {
    int32_t len;
    char *const inner;
} Path;

typedef struct PathBuf {
    char *const inner;
} PathBuf;

Path path_empty();
Path path_create(char *const value, int32_t len);
bool path_ends_with(Path *p, char *const s);
int32_t path_from_str(char *const s, Path *p);
Path path_parent(Path *p);
int32_t path_canonicalize(Path *p, PathBuf *dest);
bool path_is_abs(Path *p);
bool path_is_root(Path *p);
char *const path_to_string(Path *p);
PathBuf path_get_cwd();
PathBuf path_new_pathbuf(char *const path);
PathBuf path_create_pathbuf(char *const path);
PathBuf path_from_path(Path path);
int32_t path_merge_abs_rel(Path *base, Path *child, PathBuf *dest);
int32_t path_merge_abs_rel_suffix(Path *base, Path *child, char *const suffix, PathBuf *dest);
void pathbuf_free(PathBuf *pb);