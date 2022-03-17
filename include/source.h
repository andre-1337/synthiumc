#pragma once

#include <stdlib.h>
#include <stdint.h>

#include "span.h"
#include "file.h"
#include "path.h"

typedef struct SourceFile {
    File file;
    char *const code;
} SourceFile;

SourceFile empty_sourcefile();
char *const file_name_dup(SourceFile *sf);
char *const code(SourceFile *sf);
int32_t read(PathBuf pb, SourceFile *sf);
int32_t find_line(BigSpan *span, SourceFile *file, int32_t *last_nl_pos);