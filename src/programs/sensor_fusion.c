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
        yield();
    }
    return 0;
}
