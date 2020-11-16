#pragma once
#include <MK64F12.h>
#include <stdbool.h>
#include <multitasking.h>

/*************
 * CONSTANTS
 *************/
/* Number of devices to configure (create global contexts for). */
#define I2C_NUM_DEVICES 1

/* Constants used to specify the status of the I2C channel */
#define I2C_AVAILABLE 0
#define I2C_BUSY 1
#define I2C_ERROR 2

/* Constants used to specify the mode of the I2C channel. */
#define I2C_WRITING 0
#define I2C_READING 1

/**********
 * MACROS
 **********/

/* Address macros specify a device address in read or write mode */
#define I2C_WRITE_ADDR(x)   (i2c_seq_t){.opcode=I2C_WRITE, .addr=(x << 1)}
#define I2C_READ_ADDR(x)    (i2c_seq_t){.opcode=I2C_WRITE, .addr=(x << 1) | 1}

/* These macros are essentially identical, they send data down the line by
   setting the data field of the sequence struct. */
#define I2C_WRITE_REG(x)    (i2c_seq_t){.opcode=I2C_WRITE, .reg=x}
#define I2C_WRITE_VALUE(x)  (i2c_seq_t){.opcode=I2C_WRITE, .value=x}

/* These macros do not use the data field of the struct and simply instruct the
   driver to expect a read or send a restart, respectively. */
#define I2C_SEND_READ       (i2c_seq_t){.opcode=I2C_READ}
#define I2C_SEND_RESTART    (i2c_seq_t){.opcode=I2C_RESTART}

/* This macro blocks until the I2C transaction in the specified channel
   is complete. Used in I2C device initialization. */
#define I2C_TX_WAIT(x) while(i2c_chs[x].status != I2C_AVAILABLE);

/* This macro blocks until the I2C transaction in the specified channel
 * is complete, but yields the CPU to other processes. */
#define I2C_TX_YIELD(x) while(i2c_chs[x].status != I2C_AVAILABLE) {yield();};

/*************
 * DATATYPES
 *************/

/* I2C Configuration struct. */
typedef struct {
    uint8_t i2c_num;        /* number I2C device to be configured */
    uint32_t baud;          /* desired baud rate */
    bool stophold;          /* finish sending messages before I2C shutdown */
    bool interrupt;         /* configures interrupts -> always use true */
    uint8_t filter_clocks;  /* 0-15 for number of clocks */
    uint8_t priority;       /* interrupt priority */
} i2c_config_t;

/* I2C Sequence struct. A sequence consists of an array of these structs. */
typedef struct {
    enum opcode_t {
        I2C_RESTART,
        I2C_READ,
        I2C_WRITE
    } opcode;               /* I2C operation to perform */
    union {
        uint8_t addr;
        uint8_t reg;
        uint8_t value;
    };                      /* union for the data for clarity */
} i2c_seq_t;

/* I2C Channel state struct. Keeps track of the state of each channel. */
typedef struct {
    i2c_seq_t *seq;             /* pointer to the sequence to send */
    i2c_seq_t *seq_end;         /* pointer to the end of the sequence */
    uint8_t *received_data;     /* pointer to the rx data buffer */
    uint8_t reads_ahead;        /* number of reads remaining */
    uint8_t txrx;               /* I2C_READING or I2C_WRITING */
    uint8_t status;             /* status of the channel: AVAILABLE, BUSY, or ERROR */
    void (*callback)(void *);   /* pointer to the callback function */
    void *args;                 /* pointer to the callback function args */
} I2C_Channel;

/*************
 * VARIABLES
 *************/

/* Global variable that keeps track of the I2C Channel states. */
extern volatile I2C_Channel i2c_chs[I2C_NUM_DEVICES];

/*************
 * FUNCTIONS
 *************/

/**
 * Initializes the I2C peripheral. Interrupts and master mode are enabled by
 *      send sequence.
 * @param config A configuration struct for the I2C peripheral.
 * @param clk_f_hz The frequency of the clock going to the I2C channel.
 *                 Usually the bus clock speed.
 */
void i2c_init(i2c_config_t config, uint32_t clk_f_hz);

/**
 * Sends a sequence using the specified I2C module.
 * @param ch_num I2C channel number to send a sequence.
 * @param seq A pointer to the sequence to be sent.
 * @param seq_len Length of the sequence to be sent (number of bytes).
 * @param received_data A pointer to where data recieved by the sequence will go.
 *                      There is no runoff protection.
 * @param callback A pointer to a callback function to be run at the conclusion of
 *                 sending a sequence.
 * @param args Arguments for the callback function.
 * @return 0 if no error, -1 if I2C is busy or arbitration is lost.
 */
int i2c_send_seq(uint32_t ch_num, i2c_seq_t *seq, uint32_t seq_len, uint8_t *received_data, void (*callback)(void *), void *args);

/**
 * Generic IRQ handler that all I2C ISRs will call.
 * @param ch_num I2C channel number that was interrupted.
 * @return 0 if no error, -1 if there is an I2C error,
 *         -2 if interrupt wasn't triggered but IRQ was entered.
 */
uint32_t i2c_irq_handler(uint8_t ch_num);

/**
 * Callback function that sets a flag for I2C transfer complete.
 * @param args Void pointer to a boolean flag.
 */
void done_cb(void *args);

