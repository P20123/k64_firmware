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

#include <programs/sensor_fusion.h>
#include <programs/sensor_read.h>
#include <programs/flight_ctrl.h>

/**
 * Entry point for the process switcher.
 */
int init_main(void) {
    write(uart0_fileno, "PROGRAM START\r\n", 15);

    /* init the processes */
    process_init(&sensor_fusion_app, sensor_fusion_main,
                 sensor_fusion_stack, SENSOR_FUSION_STACK_SIZE,
                 true, false);
    process_init(&sensor_read_app, sensor_read_main, sensor_read_stack,
                 SENSOR_READ_STACK_SIZE, true, false);
    process_init(&flight_ctrl_app, flight_ctrl_main, flight_ctrl_stack,
                 FLIGHT_CTRL_STACK_SIZE, true, false);

    /* schedule processes :( */
    schedule_process(&process_table, &sensor_fusion_app);
    schedule_process(&process_table, &sensor_read_app);
    schedule_process(&process_table, &flight_ctrl_app);

    return 0;
}
