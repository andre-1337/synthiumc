#include "../include/utils.h"

static char const *standard_library_files[] = {
    "io.syn"
};

size_t const len_stdlib_files = sizeof(standard_library_files) / sizeof(char *);

int32_t get_errno() {
    return errno;
}

int32_t vfmt_str(char **dest, const char *fmt, va_list args) {
    int32_t size = 0;
    va_list tmp_args;
    
    va_copy(tmp_args, args);
    size = vsnprintf(NULL, 0, fmt, tmp_args);
    va_end(tmp_args);

    if (size < 0) return size;

    char *str = malloc(size + 1);
    if (str == NULL) return -1;

    size = vsprintf(str, fmt, args);
    
    *dest = str;

    return size;
}

char const *fmt_str(const char *fmt, ...) {
    char *s = NULL;
    va_list args;
    va_start(args, fmt);
    int32_t size = vfmt_str(&s, fmt, args);
    va_end(args);

    if (size < 0) return NULL;

    return s;
}

int32_t num_stdlib_files() {
    return len_stdlib_files;
}

char const **get_stdlib_files() {
    return standard_library_files;
}

bool is_file(const char *path) {
    struct stat s;

    if (stat(path, &s) == 0) {
        return s.st_mode & S_IFREG;
    }

    return false;
}

ErrorCode read_file(const char *path, const char **content) {
    if (!is_file(path)) return ERROR_COULD_NOT_OPEN_FILE;

    FILE *file = fopen(path, "rb");
    if (file == NULL) return ERROR_COULD_NOT_OPEN_FILE;

    fseek(file, 0L, SEEK_END);
    size_t file_size = (size_t) ftell(file);
    rewind(file);

    char *buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        return ERROR_COULD_NOT_ALLOCATE_BUFFER;
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        fclose(file);
        free(buffer);

        return ERROR_COULD_NOT_READ_FILE;
    }

    buffer[bytes_read] = '\0';

    fclose(file);
    *content = buffer;

    return OK;
}

uint64_t next_pow_of_2(uint64_t num) {
    uint64_t n = num - 1;

    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n += 1 + (n == 0);

    return n;
}

float int2flt(int64_t i) {
    return (float) i;
}

int32_t read_char(const char *string, int32_t s_len, int32_t *ch) {
    int32_t c_len = 1;

    if ((*string & 0xf8) == 0xf0) {
        c_len = 4;
    } else if ((*string & 0xf0) == 0xe0) {
        c_len = 3;
    } else if ((*string & 0xe0) == 0xc0) {
        c_len = 2;
    }

    if (c_len > s_len) {
        return s_len;
    }

    if (ch != NULL) {
        char buffer[4] = { 0 };
        memcpy(buffer, string, c_len);

        *ch = *(int32_t *) buffer;
    }
    
    return c_len;
}

int32_t chr2int(char c) {
    return (int32_t) c;
}

int32_t ptr2int(void *ptr) {
    return (int32_t)((size_t) ptr);
}

void *int2ptr(int32_t i) {
    size_t s = i;
    return *((void **) &s);
}

void flag_set(uint32_t *flags, uint32_t flag) {
    *flags |= flag;
}

bool flag_get(uint32_t *flags, uint32_t flag) {
    return *flags & flag;
}

void flag_unset(uint32_t *flags, uint32_t flag) {
    if (flag_get(flags, flag)) {
        *flags ^= flag;
    }
}
