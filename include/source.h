#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "span.h"
#include "file.h"
#include "path.h"

typedef struct SourceFile {
    File file;
    char *const code;
} SourceFile;

SourceFile source_empty();
char *const source_file_name_dup(SourceFile *sf);
char *const source_code(SourceFile *sf);
int32_t source_read(PathBuf pb, SourceFile *sf);
int32_t source_find_line(BigSpan *span, SourceFile *file, int32_t *last_nl_pos);