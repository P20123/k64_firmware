#include <environment.h>
#include <nongnu/unistd.h>
#include <stdint.h>
#include <programs/flight_ctrl.h>
#include <algo/mahony.h>
#include <drivers/devices/servos.h>
#include <math.h>

pcb_t flight_ctrl_app;
uint32_t flight_ctrl_stack[FLIGHT_CTRL_STACK_SIZE];

int flight_ctrl_main(void) {
    float w, x, y, z, pitch;


    for(;;) {
        w = q0;
        x = q1;
        y = q2;
        z = q3;

        pitch = asinf(-2.0f*(x*z - w*y));

        if(pitch > 0) {
            servo_set_leftangle(800);
            servo_set_rightangle(800);
        }
        else {
            servo_set_leftangle(1100);
            servo_set_rightangle(1100);
        }
        yield();
    }
    return 0;
}
