#include <stdint.h>
#include <kernel/process.h>
#include <kernel/private/static_memory.h>
#include <kernel/schedule.h>
#include "print_demo.h"
#include "blink_demo.h"

int init_main(void) {
    /* init the processes */
#ifdef TASK_EN_PRINT_DEMO
    process_init(&print_demo_app, print_demo_main,
                 print_demo_stack, PRINT_DEMO_STACK_WORDS,
                 false, false);
    schedule_process(&process_table, &print_demo_app);
#endif
#ifdef TASK_EN_BLINK_DEMO
    process_init(&blink_demo_app, blink_demo_main,
                 blink_demo_stack, PRINT_DEMO_STACK_WORDS,
                 false, false);
    schedule_process(&process_table, &blink_demo_app);
#endif
    return 0;
}
