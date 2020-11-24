#include <environment.h>
#include <nongnu/unistd.h>
#include <stdint.h>
#include <algo/ahrs.h>
#include <programs/sensor_read.h>
#include <programs/sensor_fusion.h>

pcb_t sensor_fusion_app;
uint32_t sensor_fusion_stack[SENSOR_FUSION_STACK_SIZE];

int sensor_fusion_main(void) {
    /* initialize the ahrs variables */
    ahrs_init();

    for(;;) {
        if(true == sensor_data_ready) {
            /* set ready to false and run the algorithm */
            sensor_data_ready = false;

            /* run the fusion algorithm */
            ahrs_update(gx, gy, gz, ax, ay, az, mx, my, mz);

            /* calculate roll, pitch, yaw */
            rpy_update();

#ifdef IMU_CALIBRATION_MODE
            /* print the quaternions */
            write(uart0_fileno, &gx, 4);
            write(uart0_fileno, ",", 1);
            write(uart0_fileno, &gy, 4);
            write(uart0_fileno, ",", 1);
            write(uart0_fileno, &gz, 4);
            write(uart0_fileno, ",", 1);
            write(uart0_fileno, &q3, 4);
            write(uart0_fileno, "\n", 1);
#else
            /* print the quaternions */
            write(uart0_fileno, &q0, 4);
            write(uart0_fileno, ",", 1);
            write(uart0_fileno, &q1, 4);
            write(uart0_fileno, ",", 1);
            write(uart0_fileno, &q2, 4);
            write(uart0_fileno, ",", 1);
            write(uart0_fileno, &q3, 4);
            write(uart0_fileno, "\n", 1);
#endif
        }
        for(volatile int i = 0; i < 100000; i++);
        yield();
    }
    return 0;
}
