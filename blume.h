#pragma once

#include <stdint.h>

struct blume;

typedef enum {
    BLUME_ERROR_SUCCESS = 0,
    BLUME_ERROR_ERRNO,
    BLUME_ERROR_OUT_OF_MEMORY,
    BLUME_ERROR_OUT_OF_FILTERS,
    BLUME_ERROR_FULL,
    BLUME_ERROR_LOCKED,
    BLUME_ERROR_CORRUPT,
} blume_error;

blume_error   blume_create(struct blume **bf, off_t max_file_size, double p, const char *path);
blume_error   blume_open(struct blume **bf, const char *path, int for_write);
blume_error   blume_close(struct blume **bf);

blume_error   blume_add(struct blume *bf, const void *key, size_t len);
int           blume_check(struct blume *bf, const void *key, size_t len);
uint64_t      blume_length(struct blume *bf);
blume_error   blume_validate(struct blume *bf);
