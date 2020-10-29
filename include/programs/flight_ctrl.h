#pragma once
#include <kernel/process.h>

#define FLIGHT_CTRL_STACK_SIZE 500

extern pcb_t flight_ctrl_app;
extern uint32_t flight_ctrl_stack[FLIGHT_CTRL_STACK_SIZE];

int flight_ctrl_main(void);
