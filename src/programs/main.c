#include <environment.h>
#include <nongnu/unistd.h>
#include <string.h>
#include <drivers/kinetis/uart.h>
#include <stdint.h>
#include <kernel/process.h>
#include <kernel/stream_io.h>
#include <kernel/private/static_memory.h>
#include <kernel/schedule.h>
#include <drivers/arm/cm4/systick.h>
#include <drivers/devices/status_leds.h>
#include <programs/sensor_read.h>
#include <programs/xpc_relay_event_loop.h>
#include <programs/xpc_spammer.h>

/**
 * Entry point for the process switcher.
 */
int init_main(void) {
#ifdef TASK_EN_SENSOR_READ
    process_init(&sensor_read_app, &sensor_read_main, sensor_read_stack,
                 SENSOR_READ_STACK_SIZE, true, false);
    schedule_process(&process_table, &sensor_read_app);
#endif

#ifdef TASK_EN_XPC_RELAY
    process_init(&xpc_relay_event_loop_app, &xpc_relay_event_loop_main,
            xpc_relay_event_loop_stack, XPC_RELAY_STACK_SIZE, false, false);
    schedule_process(&process_table, &xpc_relay_event_loop_app);
#endif
    return 0;
}
