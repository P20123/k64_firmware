#include <nongnu/unistd.h>
#include <kernel/file.h>
#include <MK64F12.h>
#include <stddef.h>
#define CURR_PROC_ID() ((uint8_t)(MCM->PID & 0xff))
/**
 * UNIX read/write byte streams
 */

extern void do_pendsvcall(void);

ssize_t read(int fildes, void *buf, size_t nbyte) {
    uint32_t svc_retval = NULL;
    register int r0 asm("r0") = fildes;
    register int *r1 asm("r1") = buf;
    register int r2 asm("r2") = nbyte;
    register int *r3 asm("r3") = &svc_retval;
    int r = 0;
    if(fildes > FDT_SIZE) {
        return -1;
    }
    asm(
        "svc #2;"
        : /* no output */
        : "r" (r0), "r" (r1), "r" (r2), "r" (r3)
    );
    r = svc_retval;
    return r;
}

ssize_t write(int fildes, const void *buf, size_t nbyte) {
    uint32_t svc_retval = NULL;
    register int r0 asm("r0") = fildes;
    register int *r1 asm("r1") = buf;
    register int r2 asm("r2") = nbyte;
    register int *r3 asm("r3") = &svc_retval;
    int r = 0;
    if(fildes > FDT_SIZE) {
        return -1;
    }
    asm(
        "svc #1;"
        : /* no output */
        : "r" (r0), "r" (r1), "r" (r2), "r" (r3)
    );
    r = svc_retval;
    return r;
}
