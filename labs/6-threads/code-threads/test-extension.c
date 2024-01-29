// run N threads that yield and explicitly call exit.
#include "test-header.h"

void delay_thread(void* arg) {
    int n = (int)arg;
    // delay_ms(500);
    rpi_sleep(5000000); 
    // usecs
    trace("thread %d exiting\n", rpi_cur_thread()->tid);
    rpi_exit(0);
}


void ping_threads(void* arg) {
    int n = 15;
    while (n--) {
        delay_ms(10);
        trace("ping\n");
        rpi_yield();
    }
    trace("thread %d exiting\n", rpi_cur_thread()->tid);
    rpi_exit(0);
}
void notmain(void) {
    
    test_init();
    int n = 10;
    for(int i = 0; i < n; i++)
        rpi_fork(delay_thread, (void*)i);
    rpi_fork(ping_threads, NULL);
    rpi_thread_start();
    trace("SUCCESS\n");
}
