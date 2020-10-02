#pragma once
#include <MK64F12.h>
#include <stdbool.h>

/*************
 * CONSTANTS
 *************/
#define I2C_NUM_DEVICES 1

#define I2C_AVAILABLE 0
#define I2C_BUSY 1
#define I2C_ERROR 2

#define I2C_WRITING 0
#define I2C_READING 1

/**********
 * MACROS
 **********/
#define I2C_WRITE_ADDR(x)   (i2c_seq_t){.opcode=I2C_WRITE, .addr=(x << 1)}
#define I2C_READ_ADDR(x)    (i2c_seq_t){.opcode=I2C_WRITE, .addr=(x << 1) | 1}
#define I2C_WRITE_REG(x)    (i2c_seq_t){.opcode=I2C_WRITE, .reg=x}
#define I2C_WRITE_VALUE(x)  (i2c_seq_t){.opcode=I2C_WRITE, .value=x}
#define I2C_SEND_READ       (i2c_seq_t){.opcode=I2C_READ}
#define I2C_SEND_RESTART    (i2c_seq_t){.opcode=I2C_RESTART}

#define I2C_TX_WAIT(x) while(i2c_chs[x].status != I2C_AVAILABLE);

/**
 * I2C Configuration struct.
 */
typedef struct {
    uint8_t i2c_num;
    uint32_t baud;
    bool stophold;
    bool interrupt;
    uint8_t filter_clocks;  /* 0-15 for number of clocks */
    uint8_t priority;
} i2c_config_t;

/**
 * I2C Sequence struct. A sequence consists of an array of these structs.
 */
typedef struct {
    enum opcode_t {
        I2C_RESTART,
        I2C_READ,
        I2C_WRITE
    } opcode;
    union {
        uint8_t addr;
        uint8_t reg;
        uint8_t value;
    };
} i2c_seq_t;

/**
 * I2C Channel state struct. Keeps track of the state of each channel.
 */
typedef struct {
    i2c_seq_t *seq;
    i2c_seq_t *seq_end;
    uint8_t *received_data;
    uint8_t reads_ahead;
    uint8_t txrx;
    uint8_t status;
    void (*callback)(void *);
    void *args;
} I2C_Channel;

/**
 * Global variable that keeps track of the I2C Channel states.
 */
extern volatile I2C_Channel i2c_chs[I2C_NUM_DEVICES];

/**
 * Initializes the I2C peripheral. Interrupts and master mode are enabled by
 *      send sequence.
 * @param config A configuration struct for the I2C peripheral.
 * @param clk_f_hz The frequency of the clock going to the I2C channel.
 *                 Usually the bus clock speed.
 */
void init_m_i2c(i2c_config_t config, uint32_t clk_f_hz);

/**
 * Sends a sequence using the specified I2C module.
 * @param ch_num I2C channel number to send a sequence.
 * @param seq A pointer to the sequence to be sent.
 * @param seq_len Length of the sequence to be sent.
 * @param received_data A pointer to where data recieved by the sequence will go.
 *                      There is no runoff protection.
 * @param callback A pointer to a callback function to be run at the conclusion of
 *                 sending a sequence.
 * @param args Arguments for the callback function.
 * @return 0 if no error, -1 if I2C is busy or arbitration is lost.
 */
uint32_t i2c_send_seq(uint32_t ch_num, i2c_seq_t *seq, uint32_t seq_len, uint8_t *received_data, void (*callback)(void *), void *args);


/**
 * Generic IRQ handler that all I2C ISRs will call.
 * @param ch_num I2C channel number that was interrupted.
 * @return 0 if no error, -1 if there is an I2C error,
 *         -2 if interrupt wasn't triggered but IRQ was entered.
 */
uint32_t i2c_irq_handler(uint8_t ch_num);
