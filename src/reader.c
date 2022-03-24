#include "../include/reader.h"

FileMap reader_create() {
    FileMap filemap = {
        .home_dir = getenv("HOME"),
        .files = vec_create(sizeof(SourceFile))
    };

    return filemap;
}

int32_t reader_num_files(FileMap *fm) {
    return fm->files.len;
}

SourceFile reader_get_by_idx(FileMap *fm, int32_t idx) {
    SourceFile s = source_empty();
    vec_get(&fm->files, idx, (void *) &s);
    return s;
}

SourceFile *reader_get_ptr_by_idx(FileMap *fm, int32_t idx) {
    return (SourceFile *) vec_get_ptr(&fm->files, idx);
}

void reader_free_fm(FileMap *fm) {
    int32_t i = 0;
    while (i < fm->files.len) {
        SourceFile s = source_empty();
        if (vec_get(&fm->files, i, (void *) &s)) {
            source_free_sf(&s);
        }

        i++;
    }

    vec_free(&fm->files);
}

FileAddResult reader_add_std_lib(FileMap *fm, Path *bin_path) {
    int32_t len = num_stdlib_files();
    const char **file_names = get_stdlib_files();
    const char *std_dir = fmt_str("%s/.synthium/stdlib", fm->home_dir);
    int32_t i = 0;

    while (i < len) {
        const char *file_name = file_names[i];
        const char *full_file_name = fmt_str("%s/%s", std_dir, file_name);
        int32_t res = 0;

        if ((res = reader_add_file(fm, bin_path, full_file_name)) != 0) {
            free((void *) std_dir);
            FileAddResult result = {
                .file_name = file_name,
                .err_code = res
            };

            return result;
        }

        int32_t idx = reader_num_files(fm) - 1;
        SourceFile *sptr = reader_get_ptr_by_idx(fm, idx);

        if (sptr != NULL) {
            PathBuf new_path = path_new_pathbuf(file_name);
            file_free(&sptr->file);
            sptr->file.path = new_path;
        }

        free((void *) full_file_name);
        i++;
    }

    free((void *) std_dir);
    FileAddResult result = {
        .file_name = NULL,
        .err_code = 0
    };

    return result;
}

FileAddResult reader_add_all(FileMap *fm, Path *bin_path, int32_t len, const char **file_names) {
    int32_t i = 0;
    while (i < len) {
        const char *file_name = file_names[i];
        int32_t res = 0;

        if ((res = reader_add_file(fm, bin_path, file_name)) != 0) {
            FileAddResult result = {
                .file_name = file_name,
                .err_code = res
            };

            return result;
        }

        i++;
    }

    FileAddResult result = {
        .file_name = NULL,
        .err_code = 0
    };

    return result;
}

int32_t reader_add_file(FileMap *fm, Path *bin_path, const char *name) {
    Path path = path_empty();
    int32_t res = 0;

    if ((res = path_from_str(name, &path)) != 0) {
        return res;
    }

    PathBuf abs_path = path_buf_from(path_empty());
    int32_t error = path_merge_abs_rel(bin_path, &path, &abs_path);
    
    if (error != 0) {
        return error;
    }

    SourceFile sf = source_empty();
    if ((res = source_read(abs_path, &sf)) != 0) {
        path_free(&abs_path);
        return res;
    }

    vec_push(&fm->files, (void *) &sf);
    return 0;
}