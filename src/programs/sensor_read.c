#include <drivers/devices/altimu.h>
#include <programs/sensor_read.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

// calibration variables
static float moffx, moffy, moffz;
static float goffx, goffy, goffz;
static float aoffx, aoffy, aoffz;

// process variables
pcb_t sensor_read_app;
uint32_t sensor_read_stack[SENSOR_READ_STACK_SIZE];

// sensor data
volatile float gx, gy, gz;
volatile float ax, ay, az;
volatile float mx, my, mz;

// read variables
volatile bool sensor_data_ready;

// constant to scale radians
const float DEG2RAD_SCALE  = (float)M_PI/180.0f;

int sensor_read_main(void) {
    uint8_t gxl_data[12];
    uint8_t mag_data[6];
//    uint8_t bar_data[2];
    float filt_ax, filt_ay, filt_az;
    bool i2c_free;

    /* initialize the sensor offsets */
    goffx = GYRO_OFFSET_X;
    goffy = GYRO_OFFSET_Y;
    goffz = GYRO_OFFSET_Z;

    aoffx = ACC_OFFSET_X;
    aoffy = ACC_OFFSET_Y;
    aoffz = ACC_OFFSET_Z;

    moffx = MAG_OFFSET_X;
    moffy = MAG_OFFSET_Y;
    moffz = MAG_OFFSET_Z;

    /* initialize accelerometer low pass filter */
    filt_ax = 0.0f;
    filt_ay = 0.0f;
    filt_az = 0.0f;

    /* initialize sensor ready to false */
    sensor_data_ready = false;

    /* continuously read values */
    for(;;) {
        /* perform the i2c reads */
        altimu_read_gxl(I2C_BUS_NUM, gxl_data, &i2c_free);
        I2C_TX_YIELD(0);
        altimu_read_mag(I2C_BUS_NUM, mag_data, &i2c_free);
        I2C_TX_YIELD(0);

        /* gyro data */
        gx = (float)(int16_t)((gxl_data[0] | (gxl_data[1] << 8)));
        gy = (float)(int16_t)((gxl_data[2] | (gxl_data[3] << 8)));
        gz = (float)(int16_t)((gxl_data[4] | (gxl_data[5] << 8)));
        gx = ((gx * GYRO_SCALE) * DEG2RAD_SCALE) - goffx;
        gy = ((gy * GYRO_SCALE) * DEG2RAD_SCALE) - goffy;
        gz = ((gz * GYRO_SCALE) * DEG2RAD_SCALE) - goffz;

        /* accel data */
        ax = (float)(int16_t)((gxl_data[6] | (gxl_data[7] << 8)));
        ay = (float)(int16_t)((gxl_data[8] | (gxl_data[8] << 8)));
        az = (float)(int16_t)((gxl_data[10] | (gxl_data[11] << 8)));
        ax = (ax * ACC_SCALE) - aoffx;
        ay = (ay * ACC_SCALE) - aoffy;
        az = (az * ACC_SCALE) - aoffz;

        /* accelerometer filter */
        filt_ax += A_FF_X * (ax - filt_ax);
        filt_ay += A_FF_Y * (ay - filt_ay);
        filt_az += A_FF_Z * (az - filt_az);
        ax = filt_ax;
        ay = filt_ay;
        az = filt_az;

        /* mag data */
        mx = (float)(int16_t)((mag_data[0] | (mag_data[1] << 8)));
        my = (float)(int16_t)((mag_data[2] | (mag_data[3] << 8)));
        mz = (float)(int16_t)((mag_data[4] | (mag_data[5] << 8)));
        mx = (mx * MAG_SCALE) - moffx;
        my = (my * MAG_SCALE) - moffy;
        mz = (mz * MAG_SCALE) - moffz;

        /* signal that data is ready */
        sensor_data_ready = true;
    }
    return 0;
}
