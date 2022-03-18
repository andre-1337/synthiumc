#include "../include/file.h"

File file_empty() {
    File file = {
        .path = path_buf_from(path_empty())
    };

    return file;
}

File file_create(PathBuf path) {
    File file = {
        .path = path
    };

    return file;
}

const char *file_name_dup(File *f) {
    return path_to_string(&f->path.inner);
}

int32_t file_read_to_string(File *f, const char **dest) {
    const char *path = path_to_string(&f->path.inner);
    int32_t content = read_file(path, dest);

    free((void *) path);

    return content;
}

void file_free(File *f) {
    path_free(&f->path);
}