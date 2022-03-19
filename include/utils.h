#pragma once

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "error.h"

static char const *standard_library_files[] = {
    "io.syn"
};

size_t const len_stdlib_files = sizeof(standard_library_files) / sizeof(char *);

int32_t vfmt_str(char **dest, const char *fmt, va_list args);
char const *fmt_str(const char *fmt, ...);
int32_t num_stdlib_files();
char const **get_stdlib_files();
bool is_file(const char *path);
ErrorCode read_file(const char *path, const char **content);
uint64_t next_pow_of_2(uint64_t num);
float int2flt(int64_t i);
int32_t read_char(const char *string, int32_t s_len, int32_t *ch);
int32_t chr2int(char c);
int32_t ptr2int(void *ptr);
void *int2ptr(int32_t i);
void flag_set(uint32_t *flags, uint32_t flag);
bool flag_get(uint32_t *flags, uint32_t flag);
void flag_unset(uint32_t *flags, uint32_t flag);