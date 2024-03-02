#include "rpi.h"
#include "procmap.h"

#define MOD 10005

void notmain(void) { 
    int mat_a[1024 * 128];
    int mat_b[128 * 16];
    int mat_c[1024 * 16];
    for (int i = 0; i < 1024 * 128; i++) {
        mat_a[i] = i % MOD;
    }
    for (int i = 0; i < 128 * 16; i++) {
        mat_b[i] = i % MOD;
    }
    // memset(mat_c, 0, sizeof(mat_c));
    for (int i = 0; i < 1024 * 16; i++) {
        mat_c[i] = 0;
    }
    // baseline of matrix multiplication
    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < 16; j++) {
            for (int k = 0; k < 128; k++) {
                mat_c[i * 16 + j] += mat_a[i * 128 + k] * mat_b[k * 16 + j];
            }
        }
    }
    printk("SUCCESS!\n");
}