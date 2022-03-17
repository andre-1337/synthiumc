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

Path empty_path();
Path create_path(char *const value, int32_t len);
bool ends_with(Path *p, char *const s);
int32_t from_str(char *const s, Path *p);
Path parent(Path *p);
int32_t canonicalize(Path *p, PathBuf *dest);
bool is_abs(Path *p);
bool is_root(Path *p);
char *const to_string(Path *p);
PathBuf get_cwd();
PathBuf new_path_buf(char *const path);
PathBuf create_path_buf(char *const path);
PathBuf buf_from(Path path);
int32_t merge_abs_rel(Path *base, Path *child, PathBuf *dest);
int32_t merge_abs_rel_suffix(Path *base, Path *child, char *const suffix, PathBuf *dest);
void free_pb(PathBuf *pb);