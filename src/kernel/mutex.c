#include <multitasking.h>
#include <stdint.h>
#include <stddef.h>

int mutex_lock_yield(int *lock) {
    int32_t svc_retval = NULL;
    register int *r0 asm("r0") = lock;
    register int r1 asm("r1") = 0;
    register int *r3 asm("r3") = &svc_retval;
    int r = 0;
    asm(
        "svc #3;"
        : /* no output */
        : "r" (r0), "r" (r1), "r" (r3)
    );
    r = svc_retval;
    if(r != 0) yield();
    return r;
}

int mutex_lock_spin(int *lock) {
    while(mutex_lock_yield(lock) != 0);
    return 0;
}

int mutex_unlock(int *lock) {
    int32_t svc_retval = NULL;
    register int *r0 asm("r0") = lock;
    register int r1 asm("r1") = 1;
    register int *r3 asm("r3") = &svc_retval;
    int r = 0;
    asm(
        "svc #3;"
        : /* no output */
        : "r" (r0), "r" (r1), "r" (r3)
    );
    r = svc_retval;
    return r;
}
