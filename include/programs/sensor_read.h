#pragma once
#include <stdbool.h>
#include <kernel/process.h>

#define SENSOR_READ_STACK_SIZE 500

#define I2C_BUS_NUM 0

#define GYRO_SCALE 0.01750f
#define ACC_SCALE 0.000488f
#define MAG_SCALE 0.0004384f

//#define IMU_CALIBRATION_MODE
#ifdef IMU_CALIBRATION_MODE
    #define MAG_OFFSET_X 0.0f
    #define MAG_OFFSET_Y 0.0f
    #define MAG_OFFSET_Z 0.0f
    #define ACC_OFFSET_X 0.0f
    #define ACC_OFFSET_Y 0.0f
    #define ACC_OFFSET_Z 0.0f
    #define GYRO_OFFSET_X 0.0f
    #define GYRO_OFFSET_Y 0.0f
    #define GYRO_OFFSET_Z 0.0f
#else
    #define MAG_OFFSET_X 0.0032f
    #define MAG_OFFSET_Y 0.0032f
    #define MAG_OFFSET_Z 0.0041f
    #define ACC_OFFSET_X 0.031325f
    #define ACC_OFFSET_Y 4.005787f
    #define ACC_OFFSET_Z 2.039449f
    #define GYRO_OFFSET_X 0.819525f
    #define GYRO_OFFSET_Y -5.738950f
    #define GYRO_OFFSET_Z -3.725575f
#endif

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
