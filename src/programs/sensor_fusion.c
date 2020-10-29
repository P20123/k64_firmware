#include <algo/mahony.h>
#include <environment.h>
#include <nongnu/unistd.h>
#include <stdint.h>
#include <programs/sensor_read.h>
#include <programs/sensor_fusion.h>

pcb_t sensor_fusion_app;
uint32_t sensor_fusion_stack[SENSOR_FUSION_STACK_SIZE];

int sensor_fusion_main(void) {
    /* initialize the ahrs variables */
    MahonyAHRSinit();

    for(;;) {
        if(true == sensor_data_ready) {
            /* set ready to false and run the algorithm */
            sensor_data_ready = false;

            /* run the fusion algorithm */
            MahonyAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);
        }
        else {
            yield();
        }

        for(volatile int i = 0; i < 30000; i++);

        /* print the quaternions */
        write(uart0_fileno, &q0, 4);
        write(uart0_fileno, ",", 1);
        write(uart0_fileno, &q1, 4);
        write(uart0_fileno, ",", 1);
        write(uart0_fileno, &q2, 4);
        write(uart0_fileno, ",", 1);
        write(uart0_fileno, &q3, 4);
        write(uart0_fileno, "\n", 1);
        yield();
    }
    return 0;
}
