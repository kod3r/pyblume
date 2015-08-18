#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>

#include "blume.h"

#define N (1024 * 1024)

int main(int argc, char *argv[])
{
    struct blume *bf;
    int done;
    uint64_t i;
    blume_error err;

    mtrace();

    err = blume_create(&bf, 1024 * 1024 * 500, 0.000001, "/tmp/test.bf");

    for(i = 0; i < N; i++) {
        if ((i % 10) == 0) {
            err = blume_add(bf, &i, sizeof(i));
            if (err != BLUME_ERROR_SUCCESS) {
                fprintf(stderr, "WTF!\n");
            }
        }
    }

    uint64_t count_fps = 0;
    uint64_t count_fns = 0;
    uint64_t count_total = 0;
    for(i = 0; i < N; i++) {
        count_total += 1;
        int hit = blume_check(bf, &i, sizeof(i));
        if (hit && ((i % 10) != 0)) {
            count_fps += 1;
        } else if (!hit && ((i % 10) == 0)) {
            count_fns += 1;
        }
    }


    printf("total: %lld\nfps: %lld\nfns: %lld\n", count_total, count_fps, count_fns);
    printf("P(fp): %0.6f\n", ((double)count_fps / count_total));

    blume_close(&bf);


    err = blume_open(&bf, "/tmp/test.bf", 0);
    if (err != BLUME_ERROR_SUCCESS) {
        fprintf(stderr, "WTF!\n");
    }

    count_fps = 0;
    count_fns = 0;
    count_total = 0;
    for(i = 0; i < N; i++) {
        count_total += 1;
        int hit = blume_check(bf, &i, sizeof(i));
        if (hit && ((i % 10) != 0)) {
            count_fps += 1;
        } else if (!hit && ((i % 10) == 0)) {
            count_fns += 1;
        }
    }


    printf("total: %lld\nfps: %lld\nfns: %lld\n", count_total, count_fps, count_fns);
    printf("P(fp): %0.6f\n", ((double)count_fps / count_total));

    blume_close(&bf);


    err = blume_create(&bf, 1024 * 1024 * 100, 0.000001, "/tmp/scale.bf");

    i = 0;
    done = 0;
    while(!done) {
        i += 1;
        err = blume_add(bf, &i, sizeof(i));
        if (err != BLUME_ERROR_SUCCESS) {
            fprintf(stderr, "Failed at %lld entries.\n", i);
            done = 1;
        }
    }

    blume_close(&bf);


    muntrace();

    return 0;
}
