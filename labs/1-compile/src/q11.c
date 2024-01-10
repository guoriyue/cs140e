#include <sys/types.h>
int16_t GET16(void *addr){
    int16_t* x = addr;
    int16_t b = *x;
    return b;
}
