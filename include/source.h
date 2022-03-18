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
    const char *code;
} SourceFile;

SourceFile source_empty();
const char *source_file_name_dup(SourceFile *sf);
const char *source_code(SourceFile *sf);
int32_t source_read(PathBuf pb, SourceFile *sf);
uint32_t source_find_line(BigSpan *span, SourceFile *file, uint32_t *last_nl_pos);
void source_free_sf(SourceFile *sf);