#pragma once

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

static char const *const standard_library_files[] = {
    "io.syn"
};

size_t const len_stdlib_files = sizeof(standard_library_files) / sizeof(char *);

int32_t get_errno();
char const *const cwd(char *const buf, int32_t buf_size);
int32_t vmft_str(char **dest, char *const fmt, va_list args);
char const *const fmt_str(char *const fmt, ...);
int32_t num_stdlib_files();
char const *const *get_stdlib_files();
bool is_file(char *const path);
int32_t read_file(char *const path, char *const *content);
uint64_t next_pow_of_2(uint64_t num);
float int2flt(int64_t i);
int32_t read_char(char *const string, int32_t s_len, int32_t *ch);
int32_t chr2int(char c);
int32_t ptr2int(void *ptr);
void *int2ptr(int32_t i);
void flag_set(uint32_t *flags, uint32_t flag);
bool flag_get(uint32_t *flags, uint32_t flag);
void flag_unset(uint32_t *flags, uint32_t flag);