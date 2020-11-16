#pragma once
#include <stdint.h>
#include <kernel/process.h>
#define BLINK_DEMO_STACK_WORDS 500
extern uint32_t blink_demo_stack[];
extern pcb_t blink_demo_app;
int blink_demo_main(void);
