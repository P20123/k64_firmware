#pragma once
#include <stdint.h>
#include <math.h>
#include <nongnu/unistd.h>
#include <kernel/process.h>
#include <environment.h>
#include <drivers/devices/servos.h>
#include <drivers/devices/motors.h>

#define FLIGHT_CTRL_STACK_SIZE 500

extern pcb_t flight_ctrl_app;
extern uint32_t flight_ctrl_stack[FLIGHT_CTRL_STACK_SIZE];

int flight_ctrl_main(void);
