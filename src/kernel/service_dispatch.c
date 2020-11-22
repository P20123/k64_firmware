#include <stddef.h>
#include <stdint.h>
#include <kernel/stream_io.h>
#include <kernel/mutex.h>

/**
 * Service call dispatcher
 * When a service call comes in from the service call entry point,
 * this function determines which kernel service needs to be called,
 * and what arguments it should have.
 * The service is executed immediately.
 */
void svcall_main(uint32_t *args, char svnum) {
    switch(svnum) {
        case 0:
        break;
        case 1:
            // write
            *(uint32_t *)args[3] = kernel_write(
                args[0], (char*)args[1], args[2]
            );
        break;
        case 2:
            // read
            *(uint32_t*)args[3] = kernel_read(args[0], (char*)args[1], args[2]);
        break;

        case 3:
            // mutex lock/unlock
            if(args[1] == 0) {
                *(uint32_t*)args[3] = kernel_mutex_lock(args[0]);
            }
            else {
                *(uint32_t*)args[3] = kernel_mutex_unlock(args[0]);
            }
        default:
        break;
    }
}
