#include "../include/path.h"

#define SYSTEM_SEPARATOR '/'

Path path_empty() {
    Path path = {
        .len = 0,
        .inner = NULL
    };

    return path;
}

Path path_create(const char *value, int32_t len) {
    Path path = {
        .len = len,
        .inner = value
    };

    return path;
}

ErrorCode path_from_str(const char *s, Path *p) {
    int32_t len = strlen(s);
    if (len <= 0) {
        return ERROR_PATH_EMPTY;
    }

    Path path = {
        .len = len,
        .inner = s
    };

    *p = path;

    return OK;
}

bool path_ends_with(Path *p, const char *s) {
    int32_t len = strlen(s);
    if (len > p->len) {
        return false;
    }

    return strncmp(p->inner + (p->len - len), s, len) == 0;
}

Path path_parent(Path *p) {
    if (path_is_root(p)) {
        return *p;
    }

    const char *ptr = p->inner + p->len - 1;
    while (ptr > p->inner) {
        ptr--;

        if (*ptr == SYSTEM_SEPARATOR) {
            return path_create(p->inner, ptr - p->inner);
        }
    }

    return *p;
}

int32_t path_canonicalize(Path *p, PathBuf *dest) {
    char *buf = strndup(p->inner, p->len);
    char *real = realpath(buf, NULL);
    
    free((void *) buf);

    if (real == NULL) {
        return get_errno();
    }

    *dest = path_create_pathbuf(real);

    return 0;
}

bool path_is_abs(Path *p) {
    return *p->inner == SYSTEM_SEPARATOR || *p->inner == '$';
}

bool path_is_root(Path *p) {
    return path_is_abs(p) && strlen(p->inner) == 1;
}

const char *path_to_string(Path *p) {
    return strndup(p->inner, p->len);
}

PathBuf path_get_cwd() {
    int32_t buf_size = 255;
    const char *buf = (const char *) malloc(buf_size * sizeof(char));

    if (getcwd(buf, buf_size) == NULL) {
        free((void *) buf);
        return path_buf_from(path_empty());
    }

    return path_create_pathbuf(buf);
}

PathBuf path_new_pathbuf(const char *path) {
    return path_create_pathbuf(strdup(path));
}

PathBuf path_create_pathbuf(const char *path) {
    Path p = path_empty();
    path_from_str(path, &p);

    PathBuf buf = {
        .inner = p
    };

    return buf;
}

PathBuf path_buf_from(Path path) {
    PathBuf buf = {
        .inner = path
    };

    return buf;
}

int32_t path_merge_abs_rel(Path *base, Path *child, PathBuf *dest) {
    return path_merge_abs_rel_suffix(base, child, NULL, dest);
}

int32_t path_merge_abs_rel_suffix(Path *base, Path *child, const char *suffix, PathBuf *dest) {
    if (base->len == 0 || path_is_abs(child)) {
        *dest = path_create_pathbuf(child->inner);
        int32_t error = path_canonicalize(&dest->inner, dest);

        if (error != 0) {
            path_free(dest);
            return error;
        }

        return 0;
    }

    const char *combined = NULL;

    if (suffix != NULL) {
        combined = fmt_str("%.*s%c%.*s%s", base->len, base->inner, SYSTEM_SEPARATOR, child->len, child->inner, suffix);
    } else {
        combined = fmt_str("%.*s%c%.*s", base->len, base->inner, SYSTEM_SEPARATOR, child->len, child->inner);
    }

    PathBuf pb = path_create_pathbuf(combined);
    int32_t error = path_canonicalize(&pb.inner, &pb);

    if (error != 0) {
        path_free(&pb);
        return error;
    }

    free((void *) combined);

    *dest = pb;

    return 0;
}

void path_free(PathBuf *pb) {
    if (pb->inner.inner != NULL) free((void *) pb->inner.inner);
}