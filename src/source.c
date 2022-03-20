#include "../include/source.h"

SourceFile source_empty() {
    SourceFile source = {
        .file = file_empty()
    };

    return source;
}

void source_free_sf(SourceFile *sf) {
    free((void *) sf->code);
    file_free(&sf->file);
}

const char *source_file_name_dup(SourceFile *sf) {
    return file_name_dup(&sf->file);
}

const char *source_code(SourceFile *sf) {
    return sf->code;
}

int32_t source_read(PathBuf pb, SourceFile *sf) {
    File file = file_create(pb);
    const char *s = NULL;
    int32_t res = file_read_to_string(&file, &s);

    if (res != 0) return res;

    SourceFile f = {
        .file = file,
        .code = s
    };

    *sf = f;

    return 0;
}

uint32_t source_find_line(BigSpan *span, SourceFile *file, uint32_t *last_nl_pos) {
    uint32_t lines = 1;
    uint32_t last_nl = span->start;
    const char *ptr = file->code + span->start;

    while (ptr != file->code) {
        if (*ptr == '\n') {
            lines++;

            if (last_nl == span->start) {
                last_nl = ptr - file->code;
            }
        }

        ptr--;
    }

    if (last_nl_pos != NULL) {
        *last_nl_pos = last_nl;
    }

    return lines;
}