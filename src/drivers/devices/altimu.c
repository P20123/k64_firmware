#include <MK64F12.h>
#include <drivers/devices/altimu.h>
#include <stdbool.h>

#define CHECK_I2C_ERRNO() if(0 != errno) {goto failout;}
/*************
 * CONSTANTS
 *************/
#define I2C_READ1_LEN 5
#define I2C_READ2_LEN 6
#define I2C_READ6_LEN 10
#define I2C_READ12_LEN 16
#define I2C_WRITE1_LEN 3
#define I2C_WRITE2_LEN 4
#define I2C_WRITE4_LEN 6

/**********************
 * SEQUENCE VARIABLES
 **********************/

/*~~~~~~~~~~~~~
 * GYRO / ACC
 *~~~~~~~~~~~~*/

/* Gyroscope and Accelerometer WHOAMI Sequence */
i2c_seq_t GXL_POKE_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_WHOAMI),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_GXL_SADDR),
    I2C_SEND_READ
};

/* Gyroscope and Accelerometer CTRL1_XL and CTRL2_G Sequence */
static i2c_seq_t GXL_C1C2_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_CTRL1_XL),
    I2C_WRITE_VALUE(0x60u),         /* CTRL1_XL value */
    I2C_WRITE_VALUE(0x60u)          /* CTRL2_G value */
};

/* Gyroscope and Accelerometer CTRL9_XL and CTRL10_C Sequence */
static i2c_seq_t GXL_C9C10_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_CTRL9_XL),
    I2C_WRITE_VALUE(0x38u),         /* CTRL9_XL value */
    I2C_WRITE_VALUE(0x38u)          /* CTR10_C value */
};

/* Gyroscope and Accelerometer INT1_CTRL Sequence */
static i2c_seq_t GXL_INT1C_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_INT1_CTRL),
    I2C_WRITE_VALUE(0x03u)          /* INT1_CTRL value */
};

i2c_seq_t GXL_STATUS_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_STATUS_REG),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_GXL_SADDR),
    I2C_SEND_READ
};

static i2c_seq_t GXL_DATA_SEQ[] = {
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

/*~~~~~~
 * MAG
 *~~~~~*/

/* Magnetometer WHOAMI Sequence */
i2c_seq_t MAG_POKE_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_MAG_SADDR),
    I2C_WRITE_REG(MAG_WHOAMI),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_MAG_SADDR),
    I2C_SEND_READ
};

/* Magnetometer CTRL1, CTRL2, CTRL3, CTRL4 Sequence */
static i2c_seq_t MAG_C1TO4_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_MAG_SADDR),
    I2C_WRITE_REG(MAG_CTRL_REG1 | STM_AUTO_INCR_MASK),
    I2C_WRITE_VALUE(0x42u),     /* CTRL_REG1 value */
    I2C_WRITE_VALUE(0x40u),     /* CTRL_REG2 value */
    I2C_WRITE_VALUE(0x00u),     /* CTRL_REG3 value */
    I2C_WRITE_VALUE(0x08u)      /* CTRL_REG4 value */
};

i2c_seq_t MAG_STATUS_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_MAG_SADDR),
    I2C_WRITE_REG(MAG_STATUS_REG),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_MAG_SADDR),
    I2C_SEND_READ
};

static i2c_seq_t MAG_DATA_SEQ[] = {
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

/*~~~~~~
 * BAR
 *~~~~~*/

/* Barometer WHOAMI Sequence */
i2c_seq_t BAR_POKE_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_BAR_SADDR),
    I2C_WRITE_REG(BAR_WHOAMI),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_BAR_SADDR),
    I2C_SEND_READ
};

/* Barometer CTRL1 Sequence */
static i2c_seq_t BAR_C1_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_BAR_SADDR),
    I2C_WRITE_REG(MAG_CTRL_REG1),
    I2C_WRITE_VALUE(0xC0u)    /* CTRL_REG1 value */
};

i2c_seq_t BAR_STATUS_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_BAR_SADDR),
    I2C_WRITE_REG(BAR_STATUS_REG),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_BAR_SADDR),
    I2C_SEND_READ
};

static i2c_seq_t BAR_DATA_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_BAR_SADDR),
    I2C_WRITE_REG(BAR_PRESS_OUT_L | STM_AUTO_INCR_MASK),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_BAR_SADDR),
    I2C_SEND_READ,
    I2C_SEND_READ
};

/*************
 * FUNCTIONS
 *************/

/* Initialization functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int altimu_reg_cmp(uint8_t i2c_ch, i2c_seq_t *stat_seq, uint8_t expected, int errcode) {
    int errno = 0;
    uint8_t id = 0;

    /* ensure that the i2c bus is free */
    I2C_TX_WAIT(i2c_ch);

    /* try poking the device */
    errno = i2c_send_seq(i2c_ch, stat_seq, I2C_READ1_LEN, &id, 0, 0);
    CHECK_I2C_ERRNO();

    /* block until we receive something */
    I2C_TX_WAIT(i2c_ch);

    /* ensure the the value matches */
    if(expected != id) {
        errno = errcode;
    }
failout:
    return errno;
}

int init_altimu_gxl(uint8_t i2c_ch) {
    int errno = 0;

    /* ensure that the device is connected to the bus */
    altimu_reg_cmp(i2c_ch, GXL_POKE_SEQ, GXL_WHOAMI_VALUE, GXL_ERROR_NOT_FOUND);
    CHECK_I2C_ERRNO();

    /* ensure that the i2c bus is free */
    I2C_TX_WAIT(i2c_ch);

    /* enable all axes (x, y, z) */ 
    errno = i2c_send_seq(i2c_ch, GXL_C1C2_SEQ, I2C_WRITE2_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2c_ch);

    /* select freq */
    errno = i2c_send_seq(i2c_ch, GXL_C9C10_SEQ, I2C_WRITE2_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2c_ch);
    
    /* enable status */
    errno = i2c_send_seq(i2c_ch, GXL_INT1C_SEQ, I2C_WRITE1_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2c_ch);
failout:
    return errno;
}

int init_altimmu_mag(uint8_t i2c_ch) {
    int errno = 0;

    /* ensure that the device is connected to the bus */
    altimu_reg_cmp(i2c_ch, MAG_POKE_SEQ, MAG_WHOAMI_VALUE, MAG_ERROR_NOT_FOUND);
    CHECK_I2C_ERRNO();

    /* initialize the mag */
    errno = i2c_send_seq(0, MAG_C1TO4_SEQ, I2C_WRITE4_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2c_ch);
failout:
    return errno;
}

int init_altimu_bar(uint8_t i2c_ch) {
    int errno = 0;

    /* ensure that the device is connected to the bus */
    altimu_reg_cmp(i2c_ch, BAR_POKE_SEQ, BAR_WHOAMI_VALUE, BAR_ERROR_NOT_FOUND);
    CHECK_I2C_ERRNO();

    /* initialize the bar */
    errno = i2c_send_seq(0, BAR_C1_SEQ, I2C_WRITE1_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2c_ch);
failout:
    return errno;
}

/* Reading functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int read_altimu_devstat(uint8_t i2c_ch, i2c_seq_t *seq, uint8_t *status, bool *done) {
    int errno = 0;

    /* send a one byte read to the status register of choice */
    errno = i2c_send_seq(i2c_ch, seq, I2C_READ1_LEN, status, &done_cb, done);
    CHECK_I2C_ERRNO();
failout:
    return errno;
}

int read_altimu_gxl(uint8_t i2c_ch, uint8_t *data, bool *done) {
    int errno = 0;

    /* send the burst read request for the gyro/accel */
    errno = i2c_send_seq(i2c_ch, GXL_DATA_SEQ, I2C_READ12_LEN, data, &done_cb, done);
    CHECK_I2C_ERRNO();
failout:
    return errno;
}

int read_altimu_mag(uint8_t i2c_ch, uint8_t *data, bool *done) {
    int errno = 0;

    /* send the burst read request for the magnetometer */
    errno = i2c_send_seq(i2c_ch, MAG_DATA_SEQ, I2C_READ6_LEN, data, &done_cb, done);
    CHECK_I2C_ERRNO();
failout:
    return errno;
}

int read_altimu_bar(uint8_t i2c_ch, uint8_t *data, bool *done) {
    int errno = 0;

    /* send the burst read request for the barometer */
    errno = i2c_send_seq(i2c_ch, BAR_DATA_SEQ, I2C_READ2_LEN, data, &done_cb, done);
    CHECK_I2C_ERRNO();
failout:
    return errno;
}
