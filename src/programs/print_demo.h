#pragma once
#include <stdint.h>
#include <kernel/process.h>
#define PRINT_DEMO_STACK_WORDS 500
extern uint32_t print_demo_stack[];
extern pcb_t print_demo_app;
int print_demo_main(void);
