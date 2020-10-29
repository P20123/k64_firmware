#pragma once
#include <kernel/process.h>

#define SENSOR_FUSION_STACK_SIZE 500

extern pcb_t sensor_fusion_app;
extern uint32_t sensor_fusion_stack[SENSOR_FUSION_STACK_SIZE];

int sensor_fusion_main(void);
