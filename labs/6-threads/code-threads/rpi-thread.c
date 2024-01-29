// engler, cs140e: starter code for trivial threads package.
#include "rpi.h"
#include "rpi-thread.h"
#include "redzone.h"

// tracing code.  set <trace_p>=0 to stop tracing
enum { trace_p = 1};
#define th_trace(args...) do {                          \
    if(trace_p) {                                       \
        trace(args);                                   \
    }                                                   \
} while(0)

/***********************************************************************
 * datastructures used by the thread code.
 *
 * you don't have to modify this.
 */

#define E rpi_thread_t
#include "libc/Q.h"

// currently only have a single run queue and a free queue.
// the run queue is FIFO.
static Q_t runq, freeq;
static rpi_thread_t *cur_thread;        // current running thread.
static rpi_thread_t *scheduler_thread;  // first scheduler thread.
unsigned sleep_until[128];
// monotonically increasing thread id: won't wrap before reboot :)
static unsigned tid = 1;

/***********************************************************************
 * simplistic pool of thread blocks: used to make alloc/free faster (plus,
 * our kmalloc doesn't have free (other than reboot).
 *
 * you don't have to modify this.
 */

// total number of thread blocks we have allocated.
static unsigned nalloced = 0;

// keep a cache of freed thread blocks.  call kmalloc if run out.
static rpi_thread_t *th_alloc(void) {
    redzone_check(0);
    rpi_thread_t *t = Q_pop(&freeq);

    if(!t) {
        t = kmalloc_aligned(sizeof *t, 8);
        nalloced++;
    }
#   define is_aligned(_p,_n) (((unsigned)(_p))%(_n) == 0)
    demand(is_aligned(&t->stack[0],8), stack must be 8-byte aligned!);
    t->tid = tid++;
    return t;
}

static void th_free(rpi_thread_t *th) {
    redzone_check(0);
    // push on the front in case helps with caching.
    Q_push(&freeq, th);
}



static int Q_in(Q_t *q, E *e) {
    for(E *x = q->head; x; x = x->next) {
        printk("x->tid=%d, e->tid=%d\n", x->tid, e->tid);
        if(x->tid == e->tid)
            return 1;
    }
    return 0;
}

static void Q_delete(Q_t *q, E *e) {
    printk("delete thread %d\n", e->tid);
    if(!q->head) {
        assert(!q->tail);
        return;
    }
    if(q->head == e) {
        q->head = e->next;
        if(!q->head)
            q->tail = 0;
        return;
    }
    for(E *x = q->head; x; x = x->next) {
        if(x->next == e) {
            x->next = e->next;
            if(!x->next)
                q->tail = x;
            return;
        }
    }
    panic("shouldn't get here\n");
}

/***********************************************************************
 * implement the code below.
 */

// stack offsets we expect.
// enum {
//     R4_OFFSET = 0,
//     R5_OFFSET,
//     R6_OFFSET,
//     R7_OFFSET,
//     R8_OFFSET,
//     R9_OFFSET,
//     R10_OFFSET,
//     R11_OFFSET,
//     R14_OFFSET = 8,
//     LR_OFFSET = 8
// };
enum {
    R4_OFFSET = 0,
    R5_OFFSET = 1,
    R6_OFFSET = 2,
    R7_OFFSET = 3,
    R8_OFFSET = 4,
    R9_OFFSET = 5,
    R10_OFFSET = 6,
    R11_OFFSET = 7,
    // R14_OFFSET = 8,
    LR_OFFSET = 8
};

// return pointer to the current thread.  
rpi_thread_t *rpi_cur_thread(void) {
    return cur_thread;
}

// create a new thread.
rpi_thread_t *rpi_fork(void (*code)(void *arg), void *arg) {
    redzone_check(0);
    rpi_thread_t *t = th_alloc();

    // write this so that it calls code,arg.
    void rpi_init_trampoline(void);
    // void rpi_init_trampoline(void (*code)(void *arg), void *arg, void *func_addr);

    /*
     * must do the "brain surgery" (k.thompson) to set up the stack
     * so that when we context switch into it, the code will be
     * able to call code(arg).
     *
     *  1. write the stack pointer with the right value.
     *  2. store arg and code into two of the saved registers.
     *  3. store the address of rpi_init_trampoline into the lr
     *     position so context switching will jump there.
     */
    // todo("initialize thread stack");
    
    t->saved_sp = (uint32_t*) t->stack;
    t->saved_sp += THREAD_MAXSTACK;
    // total offset 36
    t->saved_sp -= 9;
    t->saved_sp[R4_OFFSET] = (uint32_t) arg;
    t->saved_sp[R5_OFFSET] = (uint32_t) code;
    t->saved_sp[LR_OFFSET] = (uint32_t) rpi_init_trampoline;


    th_trace("rpi_fork: tid=%d, code=[%p], arg=[%x], saved_sp=[%p]\n",
            t->tid, code, arg, t->saved_sp);

    Q_append(&runq, t);
    return t;
}


// exit current thread.
//   - if no more threads, switch to the scheduler.
//   - otherwise context switch to the new thread.
//     make sure to set cur_thread correctly!
void rpi_exit(int exitcode) {
    redzone_check(0);

    // when you switch back to the scheduler thread:
    //      th_trace("done running threads, back to scheduler\n");
    // todo("implement rpi_exit");


    rpi_thread_t* free_thread = cur_thread;
    Q_append(&freeq, cur_thread);

    cur_thread = Q_pop(&runq);
    if (cur_thread == NULL) {
        th_trace("done running threads, back to scheduler\n");
        rpi_cswitch(&free_thread->saved_sp, scheduler_thread->saved_sp);
    }
    else {
        // rpi_yield();
        rpi_cswitch(&free_thread->saved_sp, cur_thread->saved_sp);
        // while (1) {
        //     unsigned clk = timer_get_usec();
        //     printk("clk=%d, sleep_until=%d\n", clk, sleep_until[cur_thread->tid]);
        //     if (sleep_until[cur_thread->tid]) {
        //         if (clk >= sleep_until[cur_thread->tid]) {
        //             sleep_until[cur_thread->tid] = 0;
        //             // printk("wake up thread %d\n", cur_thread->tid);
        //             rpi_cswitch(&free_thread->saved_sp, cur_thread->saved_sp);
        //             break;
        //         }
        //         else {
        //             // printk("thread %d still sleeping\n", cur_thread->tid);
        //             Q_append(&runq, cur_thread);
        //             cur_thread = Q_pop(&runq);
        //             // Q_append(&runq, cur_thread);
        //             // rpi_yield();
        //             // jump over all sleeping threads
        //             // otherwise might switch from 9 to 10 instead of 2 to 10

        //         }
        //     }
        //     else {
        //         // printk("thread %d never sleeping\n", cur_thread->tid);
        //         // Q_append(&runq, cur_thread);
        //         // cur_thread = Q_pop(&runq);
        //         // th_trace("switching from tid=%d to tid=%d\n", old->tid, cur_thread->tid);
        //         rpi_cswitch(&free_thread->saved_sp, cur_thread->saved_sp);
        //         // rpi_cswitch(&old->saved_sp, cur_thread->saved_sp);
        //         // break;
        //     }
        // }
    }
    // should never return.
    not_reached();
}

// yield the current thread.
//   - if the runq is empty, return.
//   - otherwise: 
//      * add the current thread to the back 
//        of the runq (Q_append)
//      * context switch to the new thread.
//        make sure to set cur_thread correctly!
void rpi_yield(void) {
    redzone_check(0);
    // if you switch, print the statement:
    //     th_trace("switching from tid=%d to tid=%d\n", old->tid, t->tid);

    // todo("implement the rest of rpi_yield");
    Q_append(&runq, cur_thread);
    rpi_thread_t* old = cur_thread;
    cur_thread = Q_pop(&runq);
    // printk("trying to switch from tid=%d to tid=%d\n", old->tid, cur_thread->tid);
    if (cur_thread == old) {
        return;
    }
    // else if (sleep_until[cur_thread->tid]) {
    //     while (1) {
    //         unsigned clk = timer_get_usec();
    //         // printk("clk=%d, sleep_until=%d\n", clk, sleep_until[cur_thread->tid]);
    //         if (sleep_until[cur_thread->tid]) {
    //             if (clk >= sleep_until[cur_thread->tid]) {
    //                 sleep_until[cur_thread->tid] = 0;
    //                 // printk("wake up thread %d\n", cur_thread->tid);
    //                 rpi_cswitch(&old->saved_sp, cur_thread->saved_sp);
    //                 break;
    //             }
    //             else {
    //                 // printk("thread %d still sleeping\n", cur_thread->tid);
    //                 Q_append(&runq, cur_thread);
    //                 cur_thread = Q_pop(&runq);
    //                 // Q_append(&runq, cur_thread);
    //                 // rpi_yield();
    //                 // jump over all sleeping threads
    //                 // otherwise might switch from 9 to 10 instead of 2 to 10

    //             }
    //         }
    //         else {
    //             // printk("thread %d never sleeping\n", cur_thread->tid);
    //             // Q_append(&runq, cur_thread);
    //             // cur_thread = Q_pop(&runq);
    //             // th_trace("switching from tid=%d to tid=%d\n", old->tid, cur_thread->tid);
    //             if (cur_thread == old) {
    //                 return;
    //             } else {
    //                 rpi_cswitch(&old->saved_sp, cur_thread->saved_sp);
    //                 break;
    //             }
    //             // rpi_cswitch(&old->saved_sp, cur_thread->saved_sp);
    //             // break;
    //         }
    //     }
    // }
    else {
        th_trace("switching from tid=%d to tid=%d\n", old->tid, cur_thread->tid);
        rpi_cswitch(&old->saved_sp, cur_thread->saved_sp);
    }
}

/*
 * starts the thread system.  
 * note: our caller is not a thread!  so you have to 
 * create a fake thread (assign it to scheduler_thread)
 * so that context switching works correctly.   your code
 * should work even if the runq is empty.
 */
void rpi_thread_start(void) {
    redzone_init();
    th_trace("starting threads!\n");

    // no other runnable thread: return.
    if(Q_empty(&runq))
        goto end;

    // setup scheduler thread block.
    if(!scheduler_thread)
        scheduler_thread = th_alloc();

    // todo("implement the rest of rpi_thread_start");
    cur_thread = Q_pop(&runq);
    if(cur_thread) {
        rpi_cswitch(&scheduler_thread->saved_sp, cur_thread->saved_sp);
    }

end:
    redzone_check(0);
    // if not more threads should print:
    th_trace("done with all threads, returning\n");
}

// helper routine: can call from assembly with r0=sp and it
// will print the stack out.  it then exits.
// call this if you can't figure out what is going on in your
// assembly.
void rpi_print_regs(uint32_t *sp) {
    // use this to check that your offsets are correct.
    printk("cur-thread=%d\n", cur_thread->tid);
    printk("sp=%p\n", sp);
    
    // stack pointer better be between these.
    printk("stack=%p\n", &cur_thread->stack[THREAD_MAXSTACK]);
    assert(sp < &cur_thread->stack[THREAD_MAXSTACK]);
    assert(sp >= &cur_thread->stack[0]);
    for(unsigned i = 0; i < 9; i++) {
        unsigned r = i == 8 ? 14 : i + 4;
        printk("sp[%d]=r%d=%x\n", i, r, sp[i]);
    }
    clean_reboot();
}

void rpi_sleep(unsigned t) {
    unsigned clk = timer_get_usec();
    sleep_until[cur_thread->tid] = clk + t;
    printk("rpi_sleep thread %d sleep until %d\n", cur_thread->tid, sleep_until[cur_thread->tid]);
    rpi_yield();
}

// void rpi_init_trampoline(void (*code)(void *arg), void *arg, void *func_addr) {
//     // call the function.
//     // printk("calling code=%p, arg=%p\n", code, arg);
//     printk("code=%x, arg=%x\n", code, arg);
//     printk("func_addr=%x\n", func_addr);
//     rpi_thread_t *t = rpi_cur_thread();
//     printk("cpp rpi_init_trampoline tid=%d\n", t->tid);
//     printk("cpp rpi_init_trampoline code=%x, arg=%x\n", t->saved_sp[R5_OFFSET], t->saved_sp[R4_OFFSET]);

//     // // if it returns, call exit.
//     // rpi_exit(0);
// }