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

#define GYRO_SCALE 0.007477f
#define ACC_SCALE 0.0000610f
#define MAG_SCALE 0.0001221f

#define ALP_WINDOW 7
#define ALP_WINDOW_INIT {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

//#define RAWPLOT
#ifdef RAWPLOT
    #define DELAY 15000 * 10
    #define CAL_CYCLES 500 / 10
#else
    #define DELAY 30000
    #define CAL_CYCLES 500
#endif

const float DEG2RAD_SCALE  = (float)M_PI/180.0f;

// tracking variable
bool i2c_free;
uint8_t imu_state;

// i2c data
uint8_t gxl_status;
uint8_t mag_status;
uint8_t bar_status;

volatile uint8_t gxl_data[12];
volatile uint8_t mag_data[6];
volatile uint8_t bar_data[2];

// calibration variables
float moffx, moffy, moffz;
float goffx, goffy, goffz;
float aoffx, aoffy, aoffz;


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

void calibrate_mag() {
    // calibration variables
    float mx, my, mz;
    float min_x, max_x;
    float min_y, max_y;
    float min_z, max_z;

    I2C_TX_WAIT(0);
    altimu_read_mag(I2C_BUS_NUM, mag_data, &i2c_free);

    /* mag data */
    mx = (float)(int16_t)((mag_data[0] | (mag_data[1] << 8)));
    my = (float)(int16_t)((mag_data[2] | (mag_data[3] << 8)));
    mz = (float)(int16_t)((mag_data[4] | (mag_data[5] << 8)));
    mx *= MAG_SCALE;
    my *= MAG_SCALE;
    mz *= MAG_SCALE;

    min_x = mx;
    min_y = my;
    min_z = mz;
    max_x = mx;
    max_y = my;
    max_z = mz;

    for(volatile int j = 0; j < DELAY; j++);

    for(int i = 0; i < CAL_CYCLES * 2; i++) {
        altimu_read_mag(I2C_BUS_NUM, mag_data, &i2c_free);
        I2C_TX_WAIT(0);

        /* mag data */
        mx = (float)(int16_t)((mag_data[0] | (mag_data[1] << 8)));
        my = (float)(int16_t)((mag_data[2] | (mag_data[3] << 8)));
        mz = (float)(int16_t)((mag_data[4] | (mag_data[5] << 8)));
        mx *= MAG_SCALE;
        my *= MAG_SCALE;
        mz *= MAG_SCALE;

        min_x = mx < min_x ? mx : min_x;
        min_y = my < min_y ? my : min_y;
        min_z = mz < min_z ? mz : min_z;

        max_x = mx > max_x ? mx : max_x;
        max_y = my > max_y ? my : max_y;
        max_z = mz > max_z ? mz : max_z;
        
        for(volatile int j = 0; j < DELAY; j++);
    }
    
    moffx = (max_x + min_x) / 2.0f;
    moffy = (max_y + min_y) / 2.0f;
    moffz = (max_z + min_z) / 2.0f;
}

void calibrate_gxl() {
    // calibration variables
    float gx, gy, gz;
    float ax, ay, az;
    float gmin_x, gmax_x;
    float gmin_y, gmax_y;
    float gmin_z, gmax_z;
    float amin_x, amax_x;
    float amin_y, amax_y;
    float amin_z, amax_z;

    altimu_read_gxl(I2C_BUS_NUM, gxl_data, &i2c_free);
    I2C_TX_WAIT(0);

    /* gyro data */
    gx = (float)(int16_t)((gxl_data[0] | (gxl_data[1] << 8)));
    gy = (float)(int16_t)((gxl_data[2] | (gxl_data[3] << 8)));
    gz = (float)(int16_t)((gxl_data[4] | (gxl_data[5] << 8)));
    gx = (gx * GYRO_SCALE) * DEG2RAD_SCALE;
    gy = (gy * GYRO_SCALE) * DEG2RAD_SCALE;
    gz = (gz * GYRO_SCALE) * DEG2RAD_SCALE;

    /* accelerometer data */
    ax = (float)(int16_t)((gxl_data[6] | (gxl_data[7] << 8)));
    ay = (float)(int16_t)((gxl_data[8] | (gxl_data[8] << 8)));
    az = (float)(int16_t)((gxl_data[10] | (gxl_data[11] << 8)));
    ax *= ACC_SCALE;
    ay *= ACC_SCALE;
    az *= ACC_SCALE;

    /* initialize the values */
    gmin_x = gx;
    gmin_y = gy;
    gmin_z = gz;
    gmax_x = gx;
    gmax_y = gy;
    gmax_z = gz;

    amin_x = ax;
    amin_y = ay;
    amin_z = az;
    amax_x = ax;
    amax_y = ay;
    amax_z = az;

    for(volatile int j = 0; j < DELAY; j++);

    for(int i = 0; i < CAL_CYCLES; i++) {
        altimu_read_gxl(I2C_BUS_NUM, gxl_data, &i2c_free);
        I2C_TX_WAIT(0);

        /* gyro data */
        gx = (float)(int16_t)((gxl_data[0] | (gxl_data[1] << 8)));
        gy = (float)(int16_t)((gxl_data[2] | (gxl_data[3] << 8)));
        gz = (float)(int16_t)((gxl_data[4] | (gxl_data[5] << 8)));
        gx = (gx * GYRO_SCALE) * DEG2RAD_SCALE;
        gy = (gy * GYRO_SCALE) * DEG2RAD_SCALE;
        gz = (gz * GYRO_SCALE) * DEG2RAD_SCALE;

        /* accelerometer data */
        ax = (float)(int16_t)((gxl_data[6] | (gxl_data[7] << 8)));
        ay = (float)(int16_t)((gxl_data[8] | (gxl_data[8] << 8)));
        az = (float)(int16_t)((gxl_data[10] | (gxl_data[11] << 8)));
        ax *= ACC_SCALE;
        ay *= ACC_SCALE;
        az *= ACC_SCALE;

        gmin_x = gx < gmin_x ? gx : gmin_x;
        gmin_y = gy < gmin_y ? gy : gmin_y;
        gmin_z = gz < gmin_z ? gz : gmin_z;
        gmax_x = gx > gmax_x ? gx : gmax_x;
        gmax_y = gy > gmax_y ? gy : gmax_y;
        gmax_z = gz > gmax_z ? gz : gmax_z;
        
        amin_x = ax < amin_x ? ax : amin_x;
        amin_y = ay < amin_y ? ay : amin_y;
        amin_z = az < amin_z ? az : amin_z;
        amax_x = ax > amax_x ? ax : amax_x;
        amax_y = ay > amax_y ? ay : amax_y;
        amax_z = az > amax_z ? az : amax_z;
        for(volatile int j = 0; j < DELAY; j++);
    }
    
    goffx = (gmax_x + gmin_x) / 2.0f;
    goffy = (gmax_y + gmin_y) / 2.0f;
    goffz = (gmax_z + gmin_z) / 2.0f;

    aoffx = (amax_x + amin_x) / 2.0f;
    aoffy = (amax_y + amin_y) / 2.0f;
    aoffz = (amax_z + amin_z) / 2.0f;
}

int main(void) {
    ftab_init();
    SystemCoreClockUpdate();
    uart0_conf.input_clock_rate = SystemCoreClock;
    int uart0_fd = uart_init(uart0_conf);
    int calcnt = 0;
    float gx, gy, gz;
    float ax, ay, az;
    float mx, my, mz;
    float alp_x[ALP_WINDOW] = ALP_WINDOW_INIT;
    float alp_y[ALP_WINDOW] = ALP_WINDOW_INIT;
    float alp_z[ALP_WINDOW] = ALP_WINDOW_INIT;
    float alp_avg_x, alp_avg_y, alp_avg_z;
    int alp_cnt = 0;

    PRINT("START\n\r");

    // initialize i2c globals
    i2c_free = true;
    imu_state = 0;
    gxl_status = 0;
    mag_status = 0;
    bar_status = 0;

    // initialize LEDs
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[21] |= PORT_PCR_MUX(1);
    PORTB->PCR[22] |= PORT_PCR_MUX(1);
    GPIOB->PDDR |= (1 << 21);
    GPIOB->PDDR |= (1 << 22);
    GPIOB->PSOR |= (1 << 21);
    GPIOB->PCOR |= (1 << 22);

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

    /* initialize quaternion */
    MahonyAHRSinit();

    GPIOB->PSOR |= (1 << 21);
    GPIOB->PCOR |= (1 << 22);
    calibrate_mag();
    GPIOB->PCOR |= (1 << 21);
    GPIOB->PCOR |= (1 << 22);
    for(volatile int h = 0; h < 10000000; h++);
    GPIOB->PCOR |= (1 << 21);
    GPIOB->PSOR |= (1 << 22);
    calibrate_gxl();
    GPIOB->PSOR |= (1 << 21);
    GPIOB->PSOR |= (1 << 22);

    while(1) {
        altimu_read_gxl(I2C_BUS_NUM, gxl_data, &i2c_free);
        I2C_TX_WAIT(0);
        altimu_read_mag(I2C_BUS_NUM, mag_data, &i2c_free);
        I2C_TX_WAIT(0);

        /* gyro data */
        gx = (float)(int16_t)((gxl_data[0] | (gxl_data[1] << 8)));
        gy = (float)(int16_t)((gxl_data[2] | (gxl_data[3] << 8)));
        gz = (float)(int16_t)((gxl_data[4] | (gxl_data[5] << 8)));
        gx = (gx * GYRO_SCALE) * DEG2RAD_SCALE;
        gy = (gy * GYRO_SCALE) * DEG2RAD_SCALE;
        gz = (gz * GYRO_SCALE) * DEG2RAD_SCALE;
        gx -= goffx;
        gy -= goffy;
        gz -= goffz;

        /* accel data */
        ax = (float)(int16_t)((gxl_data[6] | (gxl_data[7] << 8)));
        ay = (float)(int16_t)((gxl_data[8] | (gxl_data[8] << 8)));
        az = (float)(int16_t)((gxl_data[10] | (gxl_data[11] << 8)));
        ax *= ACC_SCALE;
        ay *= ACC_SCALE;
        az *= ACC_SCALE;
        ax -= aoffx;
        ay -= aoffy;
        az -= aoffz;

        alp_x[alp_cnt] = ax;
        alp_y[alp_cnt] = ay;
        alp_z[alp_cnt] = az;
        alp_cnt = alp_cnt == ALP_WINDOW ? 0 : alp_cnt + 1;
        alp_avg_x = 0.0f;
        alp_avg_y = 0.0f;
        alp_avg_z = 0.0f;
        for(int alp_lv = 0; alp_lv < ALP_WINDOW; alp_lv++) {
            alp_avg_x += alp_x[alp_lv];
            alp_avg_y += alp_y[alp_lv];
            alp_avg_z += alp_z[alp_lv];
        }
        ax = alp_avg_x / (float)ALP_WINDOW;
        ay = alp_avg_y / (float)ALP_WINDOW;
        az = alp_avg_z / (float)ALP_WINDOW;
        

        /* mag data */
        mx = (float)(int16_t)((mag_data[0] | (mag_data[1] << 8)));
        my = (float)(int16_t)((mag_data[2] | (mag_data[3] << 8)));
        mz = (float)(int16_t)((mag_data[4] | (mag_data[5] << 8)));
        mx *= MAG_SCALE;
        my *= MAG_SCALE;
        mz *= MAG_SCALE;
        mx -= moffx;
        my -= moffy;
        mz -= moffz;

        for(volatile int i = 0; i < DELAY; i++);

//        altimu_read_bar(I2C_BUS_NUM, bar_data, &i2c_free);
//        for(volatile int i = 0; i < DELAY; i++);

        MahonyAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);

        write(0, &q0, 4);
        write(0, ",", 1);
        write(0, &q1, 4);
        write(0, ",", 1);
        write(0, &q2, 4);
        write(0, ",", 1);
        write(0, &q3, 4);
        write(0, "\n", 1);
    }
    
    for(;;);
    return 0;
}
