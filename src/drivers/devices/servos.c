#include <MK64F12.h>
#include <stdint.h>
#include <drivers/kinetis/ftm.h>
#include <drivers/devices/status_leds.h>
#include <drivers/devices/servos.h>

/*****************
 * CONFIG STRUCTS
 *****************/
static ftm_ch_config_t ftm3_ch5_conf = {
    .ftm_num = 3u,
    .channel = 5u,
    .port_clock_gate_base = &(SIM->SCGC5),
    .port_clock_gate_mask = SIM_SCGC5_PORTC_MASK,
    .pcr = &(PORTC->PCR[9]),
    .alt = 3u
};

static ftm_ch_config_t ftm3_ch4_conf = {
    .ftm_num = 3u,
    .channel = 4u,
    .port_clock_gate_base = &(SIM->SCGC5),
    .port_clock_gate_mask = SIM_SCGC5_PORTC_MASK,
    .pcr = &(PORTC->PCR[8]),
    .alt = 3u
};

/************
 * FUNCTIONS
 ************/
int servo_init() {
    ftm_ch_epwm_init(ftm3_ch4_conf);
    ftm_ch_epwm_init(ftm3_ch5_conf);

    return 0;
}

int lservo_set_cnt(uint16_t value) {
    /* bounds check for the servos */
    value = (value > LEFT_SERVO_UP) ? LEFT_SERVO_UP : value;
    value = (value < LEFT_SERVO_DOWN) ? LEFT_SERVO_DOWN : value;

    /* set the value */
    ftm_ch_CnV_set(FTM3_NUM, CH4, value);

    return 0;
}

int rservo_set_cnt(uint16_t value) {
    /* bounds check for the servos */
    value = (value > RIGHT_SERVO_DOWN) ? RIGHT_SERVO_DOWN : value;
    value = (value < RIGHT_SERVO_UP) ? RIGHT_SERVO_UP : value;

    /* set the value */
    ftm_ch_CnV_set(FTM3_NUM, CH5, value);

    return 0;
}

int lservo_set_scaled(uint8_t value) {
    int errnum = 0;

    /* ensure that the value is safe */
    if(value > 100) {
        errnum = -1;
        goto failout;
    }
    else if(value < 0) {
        errnum = -1;
        goto failout;
    }

    /* apply the scalar and set the count */
    lservo_set_cnt(LEFT_SERVO_DOWN + (LEFT_SERVO_SCALE * value));

failout:
    return errnum;
}

int rservo_set_scaled(uint8_t value) {
    int errnum = 0;

    /* ensure that the value is safe */
    if(value > 100) {
        errnum = -1;
        goto failout;
    }
    else if(value < 0) {
        errnum = -1;
        goto failout;
    }

    /* apply the scalar and set the count */
    rservo_set_cnt(RIGHT_SERVO_DOWN - (RIGHT_SERVO_SCALE * value));

failout:
    return errnum;
}
