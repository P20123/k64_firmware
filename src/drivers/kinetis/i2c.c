#include <MK64F12.h>
#include <drivers/kinetis/i2c.h>
#include <stdbool.h>
#include <stdint.h>

/* SCL clock divider table for baudrate calculation */
static const uint16_t SCL_LUT[] = {
    20,  22,  24,  26,   28,   30,   34,   40,   28,   32,   36,   40,   44,   48,   56,   68,
    48,  56,  64,  72,   80,   88,   104,  128,  80,   96,   112,  128,  144,  160,  192,  240,
    160, 192, 224, 256,  288,  320,  384,  480,  320,  384,  448,  512,  576,  640,  768,  960,
    640, 768, 896, 1024, 1152, 1280, 1536, 1920, 1280, 1536, 1792, 2048, 2304, 2560, 3072, 3840};

/* global variable to store the channel parameters */
volatile I2C_Channel i2c_chs[I2C_NUM_DEVICES];

/* global variable to store the base pointers */
static I2C_Type *i2c_bases[] = I2C_BASE_PTRS;

void i2c_init(i2c_config_t config, uint32_t clk_f_hz) {
    /* get the base pointer */
    I2C_Type *base = i2c_bases[config.i2c_num];

    /* get the irq pointers */
    uint32_t i2c_irqs[] = I2C_IRQS;

    /* route clocks to I2C module */
    switch(config.i2c_num) {
        case 0:
            SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
            break;
        case 1:
            SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
            break;
        case 2:
            SIM->SCGC1 |= SIM_SCGC1_I2C2_MASK;
            break;
    }

    /* reset the module */
    base->A1 = 0;
    base->F = 0;
    base->C1 = 0;
    base->S = 0x80;
    base->FLT = 0;
    base->C2 = 0;
    base->RA = 0;

    /* disable i2c for config and clear all status flags */
    base->C1 &= ~I2C_C1_IICEN_MASK;
    base->S = I2C_S_ARBL_MASK | I2C_S_IICIF_MASK;

    /* set baud rate with least error */
    uint32_t rate;
    uint32_t abserr;
    uint32_t besterr = UINT32_MAX;
    uint8_t bestmult, besticr;
    for(uint8_t mult = 0; mult <= 2u; mult++) {
        for(uint8_t icr = 0; icr < sizeof(SCL_LUT) / sizeof(uint16_t); icr++) {
            rate = clk_f_hz / (SCL_LUT[icr] << mult);
            abserr = config.baud > rate ? config.baud - rate : rate - config.baud;

            if(abserr < besterr) {
                bestmult = mult;
                besticr = icr;
                besterr = abserr;

                if(0 == abserr) {
                    break;
                }
            }
        }
        if (0 == besterr) {
            break;
        }
    }
    base->F = I2C_F_MULT(bestmult) | I2C_F_ICR(besticr);

    /* configure stop-hold enable */
    if(true == config.stophold) {
        base->FLT |= I2C_FLT_SHEN_MASK;
    }

    /* configure glitch filter */
    base->FLT |= I2C_FLT_FLT(config.filter_clocks);

    /* enable interrupts */
    if (true == config.interrupt) {
        asm("cpsid i");
        NVIC_EnableIRQ(i2c_irqs[config.i2c_num]);
        NVIC->IP[i2c_irqs[config.i2c_num]] = config.priority;
        asm("cpsie i");
    }

    /* enable i2c */
    base->C1 |= I2C_C1_IICEN_MASK;

    /* initialize the channel to be available */
    i2c_chs[config.i2c_num].seq = 0;
    i2c_chs[config.i2c_num].seq_end = 0;
    i2c_chs[config.i2c_num].received_data = 0;
    i2c_chs[config.i2c_num].reads_ahead = 0;
    i2c_chs[config.i2c_num].txrx = 0;
    i2c_chs[config.i2c_num].status = I2C_AVAILABLE;
    i2c_chs[config.i2c_num].callback = 0;
    i2c_chs[config.i2c_num].args = 0;
}

int i2c_send_seq(uint32_t ch_num, i2c_seq_t *seq, uint32_t seq_len, uint8_t *received_data, void (*callback)(void *), void *args) {
    /* get the channel and the base from the globals */
    volatile I2C_Channel *ch = &i2c_chs[ch_num];
    I2C_Type *base = (I2C_Type *)i2c_bases[ch_num];

    /* initialize the errno to zero */
    uint8_t errno = 0;

    /* error out if the peripheral is busy */
    if(I2C_BUSY == ch->status) {
        errno = -1;
        goto exit;
    }

    /* initializing transfer */
    ch->seq = seq;
    ch->seq_end = seq + seq_len;
    ch->received_data = received_data;
    ch->status = I2C_BUSY;
    ch->txrx = I2C_WRITING;
    ch->callback = callback;
    ch->args = args;

    /* clear IICIF by writing a 1 to it */
    base->S |= I2C_S_IICIF_MASK;

    /* enable interrupts */
    base->C1 = (I2C_C1_IICEN_MASK | I2C_C1_IICIE_MASK);

    /* generate a start condition and set to transmit */
    base->C1 |= (I2C_C1_MST_MASK | I2C_C1_TX_MASK);

    if(base->S & I2C_S_ARBL_MASK) {
        errno = -1;
        goto i2c_send_seq_cleanup;
    }

    /* write the address */
    base->D = (*ch->seq).addr;
    ch->seq++;
    goto exit;

i2c_send_seq_cleanup:
    /* generate a STOP, disable interrupts, change to RX */
    base->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_IICIE_MASK | I2C_C1_TX_MASK);
    ch->status = I2C_ERROR;

exit:
    return errno;
}

uint32_t i2c_irq_handler(uint8_t ch_num) {
#ifdef __KINETIS_I2C_BLOCKING_ISR
    asm("cpsid i");
#endif
    /* get the channel and the base from the globals */
    volatile I2C_Channel *ch = &i2c_chs[ch_num];
    I2C_Type *base = (I2C_Type *)i2c_bases[ch_num];

    /* initialize errno to zero */
    int8_t errno = 0;

    /* ensure that irq was from the current I2C module */
    if(!(base->S & I2C_S_IICIF_MASK)) {
        errno = -2;
        goto i2c_isr_exit;
    }

    /* clear IICIF by writing a 1 to it */
    I2C0->S |= I2C_S_IICIF_MASK;
    
    /* check if arbitration lost */
    if(base->S & I2C_S_ARBL_MASK) {
        base->S |= I2C_S_ARBL_MASK;
        goto i2c_isr_error;
    }

    /* handle reading */
    if(ch->txrx == I2C_READING) {
        switch(ch->reads_ahead) {
            case 0:
                /* switch to tx to avoid another read or send restart */
                base->C1 |= I2C_C1_TX_MASK;

                /* perform the final read */
                *ch->received_data++ = base->D;

                /* if repeated start */
                if((ch->seq < ch->seq_end) && ((*ch->seq).opcode == I2C_RESTART)) {
                    /* generate a repeated start! */
                    base->C1 |= I2C_C1_RSTA_MASK;

                    /* address write post repeated start */
                    ch->txrx = I2C_WRITING;
                    ch->seq++;
                    base->D = (*ch->seq).addr;
                }
                /* no repeated start */
                else {
                    goto i2c_isr_stop;
                }
                break;
            case 1:
                /* do not ack the final read */
                base->C1 |= I2C_C1_TXAK_MASK;
                *ch->received_data++ = base->D;
                break;
            default:
                /* simply perform a read */
                *ch->received_data++ = base->D;
                break;
        }
        /* decrement the number of reads */
        ch->reads_ahead--;
    }
    /* handle writing */
    else {
        /* check if sequence end */
        if(ch->seq == ch->seq_end) {
            goto i2c_isr_stop;
        }

        /* check if NACK */
        if(base->S & I2C_S_RXAK_MASK) {
            goto i2c_isr_error;
        }

        /* check if we have a restart */
        if((*ch->seq).opcode == I2C_RESTART) {
            /* generate repeated start and ensure TX */
            base->C1 |= (I2C_C1_RSTA_MASK | I2C_C1_TX_MASK);

            /* address write post repeated start */
            ch->seq++;
            base->D = (*ch->seq).addr;
        }
        /* not a repeated start */
        else {
            /* handle a read */
            if((*ch->seq).opcode == I2C_READ) {
                /* set channel status to read */
                ch->txrx = I2C_READING;

                /* at least one read ahead */
                ch->reads_ahead = 1;

                /* parse the number of reads */
                while(((ch->seq + ch->reads_ahead) < ch->seq_end) &&
                        ((*(ch->seq + ch->reads_ahead)).opcode == I2C_READ)) {
                    ch->reads_ahead++;
                }

                /* switch to rx */
                base->C1 &= ~I2C_C1_TX_MASK;

                if(ch->reads_ahead == 1) {
                    /* do not ACK the final read */
                    base->C1 |= I2C_C1_TXAK_MASK;
                }
                else {
                    /* ACK every other read */
                    base->C1 &= ~(I2C_C1_TXAK_MASK);
                }

                /* dummy read without incrementing received data pointer */
                *ch->received_data = base->D;
                ch->reads_ahead--;
            }
            /* handle a write */
            else {
                base->D = (*ch->seq).value;
            }
        }
    }
    ch->seq++;
    goto i2c_isr_exit;

i2c_isr_stop:
    /* generate a STOP, switch to RX, disable interrupts */
    base->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_TXAK_MASK | I2C_C1_IICIE_MASK);

    /* call the callback function on success if it exists */
    if(ch->callback) {
        (*ch->callback)(ch->args);
    }

    /* set channel back to available */
    ch->status = I2C_AVAILABLE;
    goto i2c_isr_exit;

i2c_isr_error:
    /* generate a STOP and disable interrupts */
    base->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_IICIE_MASK);
    ch->status = I2C_ERROR;
    errno = -1;
    goto i2c_isr_exit;

i2c_isr_exit:
    return errno;
#ifdef __KINETIS_I2C_BLOCKING_ISR
    asm("cpsie i");
#endif
}

void I2C0_IRQHandler() {
    i2c_irq_handler(0);
}

void done_cb(void *args) {
    *(bool *)args = true;
}
