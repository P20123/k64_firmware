#include <MK64F12.h>
#include <drivers/devices/altimu.h>
#include <stdbool.h>

/**********
 * MACROS
 **********/
#define CHECK_I2C_ERRNO() if(0 != errnum) {goto failout;}

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

/* Gyroscope and Accelerometer WHOAMI Sequence (5) */
i2c_seq_t GXL_POKE_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_WHOAMI),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_GXL_SADDR),
    I2C_SEND_READ
};

/* Gyroscope and Accelerometer CTRL1_XL and CTRL2_G Sequence (4) */
static i2c_seq_t GXL_C1C2_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_CTRL1_XL),
    I2C_WRITE_VALUE(0x60u),         /* CTRL1_XL value */
    I2C_WRITE_VALUE(0x60u)          /* CTRL2_G value */
};

/* Gyroscope and Accelerometer CTRL9_XL and CTRL10_C Sequence (4) */
static i2c_seq_t GXL_C9C10_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_CTRL9_XL),
    I2C_WRITE_VALUE(0x38u),         /* CTRL9_XL value */
    I2C_WRITE_VALUE(0x38u)          /* CTR10_C value */
};

/* Gyroscope and Accelerometer INT1_CTRL Sequence (3) */
static i2c_seq_t GXL_INT1C_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_INT1_CTRL),
    I2C_WRITE_VALUE(0x03u)          /* INT1_CTRL value */
};

/* Gyroscope and Accelerometer INT1_CTRL Sequence (5) */
i2c_seq_t GXL_STATUS_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_GXL_SADDR),
    I2C_WRITE_REG(GXL_STATUS_REG),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_GXL_SADDR),
    I2C_SEND_READ
};

/* Gyroscope and Accelerometer Burst Read Sequence (16) */
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

/* Magnetometer MAG_STATUS_REG Sequence */
i2c_seq_t MAG_STATUS_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_MAG_SADDR),
    I2C_WRITE_REG(MAG_STATUS_REG),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_MAG_SADDR),
    I2C_SEND_READ
};

/* Magnetometer Burst Read Sequence */
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

/* Barometer STATUS_REG Sequence */
i2c_seq_t BAR_STATUS_SEQ[] = {
    I2C_WRITE_ADDR(ALTIMU_BAR_SADDR),
    I2C_WRITE_REG(BAR_STATUS_REG),
    I2C_SEND_RESTART,
    I2C_READ_ADDR(ALTIMU_BAR_SADDR),
    I2C_SEND_READ
};

/* Barometer Burst Read Sequence */
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
int altimu_regcmp(uint8_t i2cnum, i2c_seq_t *stat_seq, uint8_t expected, int errcode) {
    int errnum = 0;
    uint8_t id = 0;

    /* ensure that the i2c bus is free */
    I2C_TX_WAIT(i2cnum);

    /* try poking the device */
    errnum = i2c_send_seq(i2cnum, stat_seq, I2C_READ1_LEN, &id, 0, 0);
    CHECK_I2C_ERRNO();

    /* block until we receive something */
    I2C_TX_WAIT(i2cnum);

    /* ensure the the value matches */
    if(expected != id) {
        errnum = errcode;
    }
failout:
    return errnum;
}

int altimu_gxl_init(uint8_t i2cnum) {
    int errnum = 0;

    /* ensure that the device is connected to the bus */
    altimu_regcmp(i2cnum, GXL_POKE_SEQ, GXL_WHOAMI_VALUE, GXL_ERROR_NOT_FOUND);
    CHECK_I2C_ERRNO();

    /* ensure that the i2c bus is free */
    I2C_TX_WAIT(i2cnum);

    /* enable all axes (x, y, z) */ 
    errnum = i2c_send_seq(i2cnum, GXL_C1C2_SEQ, I2C_WRITE2_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2cnum);

    /* select freq */
    errnum = i2c_send_seq(i2cnum, GXL_C9C10_SEQ, I2C_WRITE2_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2cnum);
    
    /* enable status */
    errnum = i2c_send_seq(i2cnum, GXL_INT1C_SEQ, I2C_WRITE1_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2cnum);
failout:
    return errnum;
}

int altimu_mag_init(uint8_t i2cnum) {
    int errnum = 0;

    /* ensure that the device is connected to the bus */
    altimu_regcmp(i2cnum, MAG_POKE_SEQ, MAG_WHOAMI_VALUE, MAG_ERROR_NOT_FOUND);
    CHECK_I2C_ERRNO();

    /* initialize the mag */
    errnum = i2c_send_seq(0, MAG_C1TO4_SEQ, I2C_WRITE4_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2cnum);
failout:
    return errnum;
}

int altimu_bar_init(uint8_t i2cnum) {
    int errnum = 0;

    /* ensure that the device is connected to the bus */
    altimu_regcmp(i2cnum, BAR_POKE_SEQ, BAR_WHOAMI_VALUE, BAR_ERROR_NOT_FOUND);
    CHECK_I2C_ERRNO();

    /* initialize the bar */
    errnum = i2c_send_seq(0, BAR_C1_SEQ, I2C_WRITE1_LEN, 0, 0, 0);
    CHECK_I2C_ERRNO();
    I2C_TX_WAIT(i2cnum);
failout:
    return errnum;
}

/* Reading functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int altimu_read_status(uint8_t i2cnum, i2c_seq_t *seq, uint8_t *status, bool *done) {
    int errnum = 0;

    /* send a one byte read to the status register of choice */
    errnum = i2c_send_seq(i2cnum, seq, I2C_READ1_LEN, status, &done_cb, done);
    CHECK_I2C_ERRNO();
failout:
    return errnum;
}

int altimu_read_gxl(uint8_t i2cnum, uint8_t *data, bool *done) {
    int errnum = 0;

    /* send the burst read request for the gyro/accel */
    errnum = i2c_send_seq(i2cnum, GXL_DATA_SEQ, I2C_READ12_LEN, data, &done_cb, done);
    CHECK_I2C_ERRNO();
failout:
    return errnum;
}

int altimu_read_mag(uint8_t i2cnum, uint8_t *data, bool *done) {
    int errnum = 0;

    /* send the burst read request for the magnetometer */
    errnum = i2c_send_seq(i2cnum, MAG_DATA_SEQ, I2C_READ6_LEN, data, &done_cb, done);
    CHECK_I2C_ERRNO();
failout:
    return errnum;
}

int altimu_read_bar(uint8_t i2cnum, uint8_t *data, bool *done) {
    int errnum = 0;

    /* send the burst read request for the barometer */
    errnum = i2c_send_seq(i2cnum, BAR_DATA_SEQ, I2C_READ2_LEN, data, &done_cb, done);
    CHECK_I2C_ERRNO();
failout:
    return errnum;
}
