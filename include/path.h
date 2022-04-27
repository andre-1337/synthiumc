#ifndef SYNTHIUMC_PATH_H
#define SYNTHIUMC_PATH_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "error.h"
#include "utils.h"

#define SYSTEM_SEPARATOR '/'

typedef struct Path {
    int32_t len;
    const char *inner;
} Path;

typedef struct PathBuf {
    Path inner;
} PathBuf;

Path path_empty();
Path path_create(const char *value, int32_t len);
bool path_ends_with(Path *p, const char *s);
ErrorCode path_from_str(const char *s, Path *p);
PathBuf path_buf_from(Path path);
Path path_parent(Path *p);
int32_t path_canonicalize(Path *p, PathBuf *dest);
bool path_is_abs(Path *p);
bool path_is_root(Path *p);
const char *path_to_string(Path *p);
PathBuf path_get_cwd();
PathBuf path_new_pathbuf(const char *path);
PathBuf path_create_pathbuf(const char *path);
PathBuf path_from_path(Path path);
int32_t path_merge_abs_rel(Path *base, Path *child, PathBuf *dest);
int32_t path_merge_abs_rel_suffix(Path *base, Path *child, const char *suffix, PathBuf *dest);
void path_free(PathBuf *pb);

#endif
