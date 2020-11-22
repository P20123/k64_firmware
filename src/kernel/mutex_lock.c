#include <kernel/mutex.h>
#include <kernel/process.h>

int kernel_mutex_lock(int *lock) {
    int status = 0;
    if(*lock == 0) {
        *lock = get_syscaller_pid();
    }
    else {
        status = -1;
    }
    return status;
}

int kernel_mutex_unlock(int *lock) {
    int status = 0;
    if(get_syscaller_pid() == *lock) {
        *lock = 0;
    }
    else {
        status = -1;
    }
    return status;
}
