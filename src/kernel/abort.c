#include <kernel/abort.h>

void abort(void) {
    ((void (*)(void))0)();
}
