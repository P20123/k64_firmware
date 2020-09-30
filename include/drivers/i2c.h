#pragma once
#include <MK64F12.h>
#include <stdbool.h>

#define I2C_NUM_DEVICES 1

#define I2C_AVAILABLE 0
#define I2C_BUSY 1
#define I2C_ERROR 2

#define I2C_WRITING 0
#define I2C_READING 1

#define I2C_WRITE_ADDR(x)   (i2c_seq_t){.opcode=I2C_WRITE, .addr=(x << 1)}
#define I2C_READ_ADDR(x)    (i2c_seq_t){.opcode=I2C_WRITE, .addr=(x << 1) | 1}
#define I2C_WRITE_REG(x)    (i2c_seq_t){.opcode=I2C_WRITE, .reg=x}
#define I2C_WRITE_VALUE(x)  (i2c_seq_t){.opcode=I2C_WRITE, .value=x}
#define I2C_SEND_READ       (i2c_seq_t){.opcode=I2C_READ}
#define I2C_SEND_RESTART    (i2c_seq_t){.opcode=I2C_RESTART}

/**
 * i2c configuration parameters
 */
typedef struct {
    uint8_t i2c_num;
    uint32_t baud;
    bool stophold;
    bool interrupt;
    uint8_t filter_clocks;  /* 0-15 for number of clocks */
    //uint8_t priority;
} i2c_config_t;

/**
 * struct to store each sequence
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
 * i2c channel specific parameters
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
 * global variable to keep track of state
 */
extern volatile I2C_Channel i2c_chs[I2C_NUM_DEVICES];

/**
 * initialize the i2c for master mode
 */
void init_m_i2c(i2c_config_t config, uint32_t clk_f_hz);

/**
 * function to send a sequence using the i2c module
 */
uint32_t i2c_send_seq(uint32_t ch_num, i2c_seq_t *seq, uint32_t seq_len, uint8_t *received_data, void (*callback)(void *), void *args);


/**
 * generic handler that all the i2c isr will call.
 */
uint32_t i2c_irq_handler(uint8_t ch_num);
