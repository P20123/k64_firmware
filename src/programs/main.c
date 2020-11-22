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
#ifdef TASK_EN_SENSOR_READ
    #include <programs/sensor_read.h>
#endif
#ifdef TASK_EN_SENSOR_FUSION
    #include <programs/sensor_fusion.h>
#endif
#ifdef TASK_EN_FLIGHT_CTRL
    #include <programs/flight_ctrl.h>
#endif
#ifdef TASK_EN_XPC_RELAY
    #include <programs/xpc_relay_event_loop.h>
#endif

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

#ifdef TASK_EN_FLIGHT_CTRL
    process_init(&flight_ctrl_app, &flight_ctrl_main,
                 flight_ctrl_stack, FLIGHT_CTRL_STACK_SIZE, true, false);
    schedule_process(&process_table, &flight_ctrl_app);
#endif

#ifdef TASK_EN_SENSOR_FUSION
    process_init(&sensor_fusion_app, &sensor_fusion_main,
                 sensor_fusion_stack, SENSOR_FUSION_STACK_SIZE, true, false);
    schedule_process(&process_table, &sensor_fusion_app);
#endif
    return 0;
}
