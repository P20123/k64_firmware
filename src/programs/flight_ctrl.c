#include <stdint.h>
#include <math.h>
#include <nongnu/unistd.h>
#include <kernel/process.h>
#include <environment.h>
#include <drivers/devices/servos.h>
#include <drivers/devices/motors.h>
#include <programs/flight_ctrl.h>

pcb_t flight_ctrl_app;
uint32_t flight_ctrl_stack[FLIGHT_CTRL_STACK_SIZE];

int flight_ctrl_main(void) {

    for(;;) {
        lservo_set_scaled(0);
        rservo_set_scaled(0);
        lmotor_set(45);
        yield();
    }
    return 0;
}
