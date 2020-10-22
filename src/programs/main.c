#include <stdint.h>
#include <nongnu/unistd.h>
#include <kernel/kernel_ftab.h>
#include <environment.h>
#include <drivers/kinetis/uart.h>
#include <drivers/kinetis/i2c.h>
#include <drivers/devices/altimu.h>
#include <programs/MahonyAHRS.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <MK64F12.h>

#define PRINT(x) write(0, x, strlen(x));
#define I2C_BUS_NUM 0u
#define GYRO_SCALE 0.00747703482f
#define ACC_SCALE 0.00006103701f
#define MAG_SCALE 0.00012207403f


const float DEG2RAD_SCALE  = M_PI/180.0f;

// tracking variable
bool i2c_free;
uint8_t imu_state;

// i2c data
uint8_t gxl_status;
uint8_t mag_status;
uint8_t bar_status;

uint8_t gxl_data[12];
uint8_t mag_data[6];
uint8_t bar_data[2];

void i2c0_port_init() {
/*    // enable clocks to the port
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // configure the pins
    PORTE->PCR[24] |= PORT_PCR_MUX(5);
    PORTE->PCR[25] |= PORT_PCR_MUX(5);*/

    // enable clocks to the port
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // configure the pins
    PORTB->PCR[2] |= PORT_PCR_MUX(2);
    PORTB->PCR[3] |= PORT_PCR_MUX(2);
}

int main(void) {
    ftab_init();
    SystemCoreClockUpdate();
    uart0_conf.input_clock_rate = SystemCoreClock;
    int uart0_fd = uart_init(uart0_conf);
    int16_t *dataptr = 0;
    float gx, gy, gz;
    float ax, ay, az;
    float mx, my, mz;

    PRINT("START\n\r");

    // initialize i2c globals
    i2c_free = true;
    imu_state = 0;
    gxl_status = 0;
    mag_status = 0;
    bar_status = 0;

    // initialize the i2c module
    i2c_config_t config = {
        .i2c_num = I2C_BUS_NUM,
        .baud = 115200,
        .stophold = true,
        .interrupt = true,
        .filter_clocks = 0,
        .priority = 0
    };
    i2c_init(config, SystemCoreClock / 2);

    i2c0_port_init();

    PRINT("SUCCESSFUL I2C INIT\n\r");

    altimu_gxl_init(I2C_BUS_NUM);

    PRINT("SUCCESSFUL GXL INIT\n\r");

    altimu_mag_init(I2C_BUS_NUM);
    
    PRINT("SUCCESSFUL MAG INIT\n\r");
    
    altimu_bar_init(I2C_BUS_NUM);

    PRINT("SUCCESSFUL BAR INIT\n\r");

#define DELAY 75000
    while(1) {
        altimu_read_gxl(I2C_BUS_NUM, gxl_data, &i2c_free);
        for(volatile int i = 0; i < DELAY; i++);
        altimu_read_mag(I2C_BUS_NUM, mag_data, &i2c_free);
        for(volatile int i = 0; i < DELAY; i++);
        altimu_read_bar(I2C_BUS_NUM, bar_data, &i2c_free);
        for(volatile int i = 0; i < DELAY; i++);

        /* gyro data */
        dataptr = (int16_t *)gxl_data;
        gx = (float)(dataptr[0]/GYRO_SCALE) * DEG2RAD_SCALE;
        gy = (float)(dataptr[1]/GYRO_SCALE) * DEG2RAD_SCALE;
        gz = (float)(dataptr[2]/GYRO_SCALE) * DEG2RAD_SCALE;

        /* accel data */
        ax = (float)(dataptr[4]/MAG_SCALE);
        ay = (float)(dataptr[5]/MAG_SCALE);
        az = (float)(dataptr[6]/MAG_SCALE);

        /* mag data */
        dataptr = (int16_t *)mag_data;
        mx = (float)(dataptr[0]/MAG_SCALE);
        my = (float)(dataptr[1]/MAG_SCALE);
        mz = (float)(dataptr[2]/MAG_SCALE);

        MahonyAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);
        write(0, &q0, 4);
        write(0, &q1, 4);
        write(0, &q2, 4);
        write(0, &q3, 4);
        write(0, "\n", 1);
//        write(0, gxl_data, 12);
//        write(0, ",", 1);
//        write(0, mag_data, 6);
//        write(0, ",", 1);
//        write(0, bar_data, 2);
//        write(0, "\n", 1);
    }

/*    while(1) {
        if(i2c_free) {
            switch(imu_state) {
                case 0:
                    altimu_read_status(I2C_BUS_NUM, GXL_STATUS_SEQ, &gxl_status, &i2c_free);
                    if(0 != (gxl_status & GXL_STATUS_REG_GDA_MASK)) {
                        imu_state++;
                    }
                    else {
                        imu_state = 2;
                    }
                    break;
                case 1:
                    altimu_read_gxl(I2C_BUS_NUM, gxl_data, &i2c_free);
                    imu_state++;
                    break;
                case 2:
                    altimu_read_status(I2C_BUS_NUM, MAG_STATUS_SEQ, &mag_status, &i2c_free);
                    if(0 != (mag_status & MAG_STATUS_REG_ZYXDA_MASK)) {
                        imu_state++;
                    }
                    else {
                        imu_state = 4;
                    }
                    break;
                case 3:
                    altimu_read_mag(I2C_BUS_NUM, mag_data, &i2c_free);
                    imu_state++;
                    break;
                case 4:
                    altimu_read_status(I2C_BUS_NUM, BAR_STATUS_SEQ, &bar_status, &i2c_free);
                    if(0 != (bar_status & BAR_STATUS_REG_PDA_MASK)) {
                        imu_state++;
                    }
                    else {
                        imu_state = 0;
                    }
                    break;
                case 5:
                    altimu_read_bar(I2C_BUS_NUM, bar_data, &i2c_free);
                    imu_state = 0;
                    break;
            }
            i2c_free = false;

            if(imu_state == 0 || imu_state == 2 || imu_state == 4) {
                write(0, gxl_data, 12);
                write(0, ",", 1);
                write(0, mag_data, 6);
                write(0, ",", 1);
                write(0, bar_data, 2);
                write(0, "\n", 1);
            }
        }
    }*/
    
    for(;;);
    return 0;
}
