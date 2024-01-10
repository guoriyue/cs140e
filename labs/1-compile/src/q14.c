#include <sys/types.h>
void PUT16(void *addr, unsigned val){
    int16_t* x = addr;
    *x = val;
}