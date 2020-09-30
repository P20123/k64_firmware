#include <stdint.h>
#include <nongnu/unistd.h>
#include <kernel/kernel_ftab.h>
#include <environment.h>
#include <stdbool.h>
#include <drivers/uart.h>
#include <drivers/i2c.h>
#include <string.h>
#include <MK64F12.h>

//#define PRINT(x) write(0, x, strlen(x));
#define PRINT(x)
#define I2C_TX_WAIT(x) while(i2c_chs[x].status != I2C_AVAILABLE); 

#define STM_AUTO_INCR_MASK 0x80u

// AltIMU-10 v5 ADDRESSES
#define ALTIMU_GXL_SADDR 0b01101011
#define GXL_INT1_CTRL 0x0Du
#define GXL_WHOAMI 0x0Fu
#define GXL_CTRL1_XL 0x10u
#define GXL_CTRL2_G 0x11u
#define GXL_CTRL9_XL 0x18u
#define GXL_CTRL10_C 0x19u
#define GXL_STATUS_REG 0x1Eu
#define GXL_OUTX_L_G 0x22u // first of the output registers >:)
#define GXL_WHOAMI_VALUE 0x69u // WHOAMI VALUE
#define GXL_STATUS_REG_GDA_MASK 0x02u

#define ALTIMU_MAG_SADDR 0b00011110
#define MAG_WHOAMI 0x0Fu
#define MAG_CTRL_REG1 0x20u
#define MAG_CTRL_REG2 0x21u
#define MAG_CTRL_REG3 0x22u
#define MAG_CTRL_REG4 0x23u
#define MAG_STATUS_REG 0x27u
#define MAG_OUT_X_L 0x28u // first of the output registers >:)
#define MAG_WHOAMI_VALUE 0x3Du //WHOAMI VALUE
#define MAG_STATUS_REG_ZYXDA_MASK 0x08u

#define ALTIMU_BAR_SADDR 0b01011101
#define BAR_WHOAMI 0x0Fu
#define BAR_CTRL_REG1 0x20u
#define BAR_CTRL_REG2 0x21u
#define BAR_CTRL_REG3 0x22u
#define BAR_CTRL_REG4 0x23u
#define BAR_STATUS_REG 0x27u
#define BAR_PRESS_OUT_L 0x29u // start reading here
#define BAR_WHOAMI_VALUE 0xBDu //WHOAMI VALUE
#define BAR_STATUS_REG_PDA_MASK 0x02u

// ToF Distance Sensor ADDRESSES
#define TOF_L_SADDR 0b00101001
#define TOF_R_SADDR 0b00101001
#define TOF_C_SADDR 0b00101001
#define TOF_WHOAMI_H 0x01u
#define TOF_WHOAMI_L 0x0Fu
#define TOF_WHOAMI_VALUE 0xEA // WHOAMI VALUE


#define WHOAMI_SEQ_LEN 5
static i2c_seq_t gxl_whoami_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_WHOAMI),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_GXL_SADDR),
    I2C_SEND_READ
};

static i2c_seq_t mag_whoami_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_MAG_SADDR),
    I2C_WRITE_REG(MAG_WHOAMI),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_MAG_SADDR),
    I2C_SEND_READ
};

static i2c_seq_t bar_whoami_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_BAR_SADDR),
    I2C_WRITE_REG(BAR_WHOAMI),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_BAR_SADDR),
    I2C_SEND_READ
};

#define TOF_WHOAMI_LEN 6
static i2c_seq_t tof_l_whoami_seq[] = {
    I2C_WRITE_ADDR(TOF_L_SADDR),
    I2C_WRITE_REG(TOF_WHOAMI_H),
    I2C_WRITE_REG(TOF_WHOAMI_L),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(TOF_L_SADDR),
    I2C_SEND_READ
};

#define BURST_WRITE_TWO_LEN 4
static i2c_seq_t gxl_opmode_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_CTRL1_XL),
    I2C_WRITE_VALUE(0x60u),
    I2C_WRITE_VALUE(0x60u)
};

static i2c_seq_t gxl_axes_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_CTRL9_XL),
    I2C_WRITE_VALUE(0x38u),
    I2C_WRITE_VALUE(0x38u)
};

#define SINGLE_WRITE_LEN 3
static i2c_seq_t gxl_int_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_INT1_CTRL),
    I2C_WRITE_VALUE(0x03u)
};

#define SINGLE_READ_LEN 5
static i2c_seq_t gxl_status_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_STATUS_REG),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_GXL_SADDR),
    I2C_SEND_READ
};

#define GXL_BURST_READ_LEN 16
static i2c_seq_t gxl_output_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_OUTX_L_G),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_GXL_SADDR),
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ
};

#define MAG_INIT_LEN 6
static i2c_seq_t mag_init_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_MAG_SADDR),
    I2C_WRITE_REG(MAG_CTRL_REG1 | STM_AUTO_INCR_MASK),
    I2C_WRITE_VALUE(0x42u),
    I2C_WRITE_VALUE(0x40u),
    I2C_WRITE_VALUE(0x00u),
    I2C_WRITE_VALUE(0x08u)
};

#define MAG_BURST_READ_LEN 10
static i2c_seq_t mag_output_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_MAG_SADDR),
    I2C_WRITE_REG(MAG_OUT_X_L | STM_AUTO_INCR_MASK),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_MAG_SADDR),
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ,
    I2C_SEND_READ
};

static i2c_seq_t mag_status_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_MAG_SADDR),
    I2C_WRITE_REG(MAG_STATUS_REG),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_MAG_SADDR),
    I2C_SEND_READ
};

#define BAR_INIT_LEN 3
static i2c_seq_t bar_init_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_BAR_SADDR),
    I2C_WRITE_REG(MAG_CTRL_REG1),
    I2C_WRITE_VALUE(0xC0u)
};

static i2c_seq_t bar_status_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_BAR_SADDR),
    I2C_WRITE_REG(BAR_STATUS_REG),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_BAR_SADDR),
    I2C_SEND_READ
};

#define BAR_BURST_READ_LEN 6
static i2c_seq_t bar_output_seq[] = {
    I2C_WRITE_ADDR(ALTIMU_BAR_SADDR),
    I2C_WRITE_REG(BAR_PRESS_OUT_L | STM_AUTO_INCR_MASK),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_BAR_SADDR),
    I2C_SEND_READ,
    I2C_SEND_READ
};


void callback_test(void *args) {
    PRINT("Transaction Ended!\n\r");
    for(int i = 0; i < 1000; i++);
}

int init_altimu() {
    int errno = 0;

    /* ensure that the i2c bus is free */
    I2C_TX_WAIT(0);

    /* initialize the gyro/accel: enable all axes, select freq, enable status */
    errno = i2c_send_seq(0, gxl_axes_seq, BURST_WRITE_TWO_LEN, 0, &callback_test, 0);
    I2C_TX_WAIT(0);
    errno = i2c_send_seq(0, gxl_opmode_seq, BURST_WRITE_TWO_LEN, 0, &callback_test, 0);
    I2C_TX_WAIT(0);
    errno = i2c_send_seq(0, gxl_int_seq, SINGLE_WRITE_LEN, 0, &callback_test, 0);
    I2C_TX_WAIT(0);

    /* initialize the mag: */
    errno = i2c_send_seq(0, mag_init_seq, MAG_INIT_LEN, 0, &callback_test, 0);
    I2C_TX_WAIT(0);

    /* initialize the bar: */
    errno = i2c_send_seq(0, bar_init_seq, BAR_INIT_LEN, 0, &callback_test, 0);
    I2C_TX_WAIT(0);

    return errno;
}

int altimu_read_gxl(uint8_t *data) {
    int errno = 0;
    uint8_t status = 0;

    /* ensure that the i2c bus is free */
    I2C_TX_WAIT(0);

    /* poll for when the data is ready */
    while((status & GXL_STATUS_REG_GDA_MASK) == 0) {
        errno = i2c_send_seq(0, gxl_status_seq, SINGLE_READ_LEN, &status, &callback_test, 0);
        I2C_TX_WAIT(0);
    }

    /* send the burst read request for the gyro/accel */
    errno = i2c_send_seq(0, gxl_output_seq, GXL_BURST_READ_LEN, data, &callback_test, 0);
    I2C_TX_WAIT(0);

    return errno;
}

int altimu_read_mag(uint8_t *data) {
    int errno = 0;
    uint8_t status = 0;

    /* ensure that the i2c bus is free */
    I2C_TX_WAIT(0);

    /* poll for when the data is ready */
    while((status & MAG_STATUS_REG_ZYXDA_MASK) == 0) {
        errno = i2c_send_seq(0, mag_status_seq, SINGLE_READ_LEN, &status, &callback_test, 0);
        I2C_TX_WAIT(0);
    }

    /* send the burst read request for the magnetometer */
    errno = i2c_send_seq(0, mag_output_seq, MAG_BURST_READ_LEN, data, &callback_test, 0);
    I2C_TX_WAIT(0);

    return errno;
}

int altimu_read_bar(uint8_t *data) {
    int errno = 0;
    uint8_t status = 0;

    /* ensure that the i2c bus is free */
    I2C_TX_WAIT(0);

    /* poll for when the data is ready */
    while((status & BAR_STATUS_REG_PDA_MASK) == 0) {
        errno = i2c_send_seq(0, bar_status_seq, SINGLE_READ_LEN, &status, &callback_test, 0);
        I2C_TX_WAIT(0);
    }

    /* send the burst read request for the barometer */
    errno = i2c_send_seq(0, bar_output_seq, BAR_BURST_READ_LEN, data, &callback_test, 0);
    I2C_TX_WAIT(0);

    return errno;
}

int i2c0_check_ids() {
    int errno = 0;
    uint8_t id = 0;

    /* ensure that the i2c bus is free */
    I2C_TX_WAIT(0);

    /* try poking the gyro/accel */
    errno = i2c_send_seq(0, gxl_whoami_seq, WHOAMI_SEQ_LEN, &id, &callback_test, 0);
    I2C_TX_WAIT(0);
    if(GXL_WHOAMI_VALUE != id) {
        errno = -1;
        goto failout;
    }

    /* try poking the mag */
    errno = i2c_send_seq(0, mag_whoami_seq, WHOAMI_SEQ_LEN, &id, &callback_test, 0);
    I2C_TX_WAIT(0);
    if(MAG_WHOAMI_VALUE != id) {
        errno = -1;
        goto failout;
    }

    /* try poking the baro */
    errno = i2c_send_seq(0, bar_whoami_seq, WHOAMI_SEQ_LEN, &id, &callback_test, 0);
    I2C_TX_WAIT(0);
    if(BAR_WHOAMI_VALUE != id) {
        errno = -1;
        goto failout;
    }

    /* try poking the left tof */
    errno = i2c_send_seq(0, tof_l_whoami_seq, TOF_WHOAMI_LEN, &id, &callback_test, 0);
    I2C_TX_WAIT(0);
    if(TOF_WHOAMI_VALUE != id) {
        errno = -1;
        goto failout;
    }

failout:
    return errno;
}

int main(void) {
    ftab_init();
    SystemCoreClockUpdate();
    uart0_conf.input_clock_rate = SystemCoreClock;
    int uart0_fd = uart_init(uart0_conf);

    PRINT("START\n\r");

    // i2c data
    uint8_t data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // initialize the i2c module
    i2c_config_t config = {
        .i2c_num = 0,
        .baud = 115200,
        .stophold = false,
        .interrupt = true,
        .filter_clocks = 0
    };
    init_m_i2c(config, 60000000);
    PRINT("Successfully Initialized I2C0\n\r");

    // enable clocks to the port
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // configure the pins
    PORTE->PCR[24] |= PORT_PCR_MUX(5);
    PORTE->PCR[25] |= PORT_PCR_MUX(5);

    // errno
    int errno = 0;

    PRINT("Running I2C Test\n\r");
    errno = i2c0_check_ids();
    errno = init_altimu();
    errno = altimu_read_gxl(data);
    errno = altimu_read_mag(data);
    errno = altimu_read_bar(data);

    if(errno != 0) {
        PRINT("===== ERROR =====\n\r");
    }

    PRINT("END\n\r");
    for(;;);
    return 0;
}
