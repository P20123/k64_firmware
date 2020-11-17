#pragma once
#include <stdbool.h>
#include <kernel/process.h>

#define SENSOR_READ_STACK_SIZE 500

#define I2C_BUS_NUM 0

#define GYRO_SCALE 0.007477f
#define ACC_SCALE 0.0000610f
#define MAG_SCALE 0.0001221f

#define MAG_OFFSET_X -0.002f
#define MAG_OFFSET_Y -0.054f
#define MAG_OFFSET_Z -0.004f

#define ACC_OFFSET_X -0.01f
#define ACC_OFFSET_Y 0.872f
#define ACC_OFFSET_Z 1.028f

#define GYRO_OFFSET_X 0.010f
#define GYRO_OFFSET_Y -0.085f
#define GYRO_OFFSET_Z -0.047f

#define A_FF_X 0.0625f
#define A_FF_Y 0.0625f
#define A_FF_Z 0.0625f

extern volatile float gx, gy, gz;
extern volatile float ax, ay, az;
extern volatile float mx, my, mz;

extern volatile bool sensor_data_ready;

extern pcb_t sensor_read_app;
extern uint32_t sensor_read_stack[SENSOR_READ_STACK_SIZE];

int sensor_read_main(void);
