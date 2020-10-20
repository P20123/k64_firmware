#include <MK64F12.h>
#include <drivers/kinetis/i2c.h>

/*************
 * CONSTANTS
 *************/

/* STM Masks */
#define STM_AUTO_INCR_MASK 0x80u

/* AltIMU-10 v5 Gyroscope and Accelerometer Constants */
#define ALTIMU_GXL_SADDR 0b01101011
#define GXL_INT1_CTRL 0x0Du
#define GXL_WHOAMI 0x0Fu
#define GXL_CTRL1_XL 0x10u
#define GXL_CTRL2_G 0x11u
#define GXL_CTRL9_XL 0x18u
#define GXL_CTRL10_C 0x19u
#define GXL_STATUS_REG 0x1Eu
#define GXL_OUTX_L_G 0x22u             /* First Data Register */
#define GXL_WHOAMI_VALUE 0x69u
#define GXL_STATUS_REG_GDA_MASK 0x02u
#define GXL_ERROR_NOT_FOUND -3

/* AltIMU-10 v5 Magnetometer Constants */
#define ALTIMU_MAG_SADDR 0b00011110
#define MAG_WHOAMI 0x0Fu
#define MAG_CTRL_REG1 0x20u
#define MAG_CTRL_REG2 0x21u
#define MAG_CTRL_REG3 0x22u
#define MAG_CTRL_REG4 0x23u
#define MAG_STATUS_REG 0x27u
#define MAG_OUT_X_L 0x28u               /* First Data Register */
#define MAG_WHOAMI_VALUE 0x3Du
#define MAG_STATUS_REG_ZYXDA_MASK 0x08u
#define MAG_ERROR_NOT_FOUND -4

/* AltIMU-10 v5 Barometer Constants */
#define ALTIMU_BAR_SADDR 0b01011101
#define BAR_WHOAMI 0x0Fu
#define BAR_CTRL_REG1 0x20u
#define BAR_CTRL_REG2 0x21u
#define BAR_CTRL_REG3 0x22u
#define BAR_CTRL_REG4 0x23u
#define BAR_STATUS_REG 0x27u
#define BAR_PRESS_OUT_L 0x29u          /* First Data Register */
#define BAR_WHOAMI_VALUE 0xBDu
#define BAR_STATUS_REG_PDA_MASK 0x02u
#define BAR_ERROR_NOT_FOUND -5

/*************
 * EXTERNS
 *************/
extern i2c_seq_t GXL_POKE_SEQ[];
extern i2c_seq_t GXL_STATUS_SEQ[];
extern i2c_seq_t MAG_POKE_SEQ[];
extern i2c_seq_t MAG_STATUS_SEQ[];
extern i2c_seq_t BAR_POKE_SEQ[];
extern i2c_seq_t BAR_STATUS_SEQ[];

/*************
 * FUNCTIONS
 *************/

/*
 * This function probes a register for a specific value on the specified bus and
 * address.
 * Note: This function does not act asynchronously and will block the CPU.
 * @param i2cnum I2C Channel that is connected to the AltIMU.
 * @param stat_seq Pointer to the sequence to be sent to query the register.
 * @param expected Expected value of the queried register.
 * @param errcode Error code to be returned if the register does not match.
 * @return 0 if no errors, errcode if not found, or I2C errors.
 */
int altimu_regcmp(uint8_t i2cnum, i2c_seq_t *stat_seq, uint8_t expected, int errcode);

/*
 * This function initializes the gyroscope and accelerometer of the AltIMU.
 * Note: This function does not act asynchronously and will block the CPU.
 * @param i2cnum I2C Channel that is connected to the AltIMU.
 * @return 0 if no errors, GXL_ERROR_NOT_FOUND if not found, I2C errors if
 *         transmission fails.
 */
int altimu_gxl_init(uint8_t i2cnum);

/*
 * This function initializes the magnetometer of the AltIMU.
 * Note: This function does not act asynchronously and will block the CPU.
 * @param i2cnum I2C Channel that is connected to the AltIMU.
 * @return 0 if no errors, MAG_ERROR_NOT_FOUND if not found, I2C errors if
 *         transmission fails.
 */
int altimu_mag_init(uint8_t i2cnum);

/*
 * This function initializes the barometer of the AltIMU.
 * Note: This function does not act asynchronously and will block the CPU.
 * @param i2cnum I2C Channel that is connected to the AltIMU.
 * @return 0 if no errors, BAR_ERROR_NOT_FOUND if not found, I2C errors if
 *         transmission fails.
 */
int altimu_bar_init(uint8_t i2cnum);

/*
 * This function queries a specific device for its data status.
 * Uses the following sequences: BAR_STATUS_SEQ, MAG_STATUS_SEQ, GXL_STATUS_SEQ
 * @param i2cnum I2C Channel that is connected to the AltIMU.
 * @param seq A pointer to the sequence to be sent by the function.
 * @param status A pointer to a one byte buffer to store the status data.
 * @param done A pointer to a boolean specifying whether or not the transfer is done.
 * @return 0 if no errors, I2C errors if transmission fails.
 */
int altimu_read_status(uint8_t i2cnum, i2c_seq_t *seq, uint8_t *status, bool *done);

/*
 * This function reads data from the gyro/accel.
 * @param i2cnum I2C Channel that is connected to the AltIMU.
 * @param seq A pointer to the sequence to be sent by the function.
 * @param status A pointer to a 12 byte buffer to store the status data.
 * @param done A pointer to a boolean specifying whether or not the transfer is done.
 * @return 0 if no errors, I2C errors if transmission fails.
 */
int altimu_read_gxl(uint8_t i2cnum, uint8_t *data, bool *done);

/*
 * This function reads data from the magnetometer.
 * @param i2cnum I2C Channel that is connected to the AltIMU.
 * @param seq A pointer to the sequence to be sent by the function.
 * @param status A pointer to a 6 byte buffer to store the status data.
 * @param done A pointer to a boolean specifying whether or not the transfer is done.
 * @return 0 if no errors, I2C errors if transmission fails.
 */
int altimu_read_mag(uint8_t i2cnum, uint8_t *data, bool *done);

/*
 * This function reads data from the barometer.
 * @param i2cnum I2C Channel that is connected to the AltIMU.
 * @param seq A pointer to the sequence to be sent by the function.
 * @param status A pointer to a 2 byte buffer to store the status data.
 * @param done A pointer to a boolean specifying whether or not the transfer is done.
 * @return 0 if no errors, I2C errors if transmission fails.
 */
int altimu_read_bar(uint8_t i2cnum, uint8_t *data, bool *done);
