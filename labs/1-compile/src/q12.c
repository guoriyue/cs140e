#include <sys/types.h>
int8_t GET8(void *addr){
    int8_t* x = addr;
    int8_t b = *x;
    return b;
}
