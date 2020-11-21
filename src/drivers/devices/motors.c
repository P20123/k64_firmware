#include <MK64F12.h>
#include <stdint.h>
#include <drivers/kinetis/ftm.h>
#include <drivers/devices/motors.h>
#include <drivers/devices/status_leds.h>

/*****************
 * CONFIG STRUCTS
 *****************/
static ftm_ch_config_t ftm0_ch0_conf = {
    .ftm_num = 0u,
    .channel = 0u,
    .port_clock_gate_base = &(SIM->SCGC5),
    .port_clock_gate_mask = SIM_SCGC5_PORTC_MASK,
    .pcr = &(PORTC->PCR[1]),
    .alt = 4u
};

static ftm_ch_config_t ftm0_ch1_conf = {
    .ftm_num = 0u,
    .channel = 1u,
    .port_clock_gate_base = &(SIM->SCGC5),
    .port_clock_gate_mask = SIM_SCGC5_PORTC_MASK,
    .pcr = &(PORTC->PCR[2]),
    .alt = 4u
};

/************
 * FUNCTIONS
 ************/
int lmotor_init() {
    /* initialize the pwm channel */
    ftm_ch_epwm_init(ftm0_ch0_conf);

    /* set the duty cycle to the max */
    ftm_ch_duty_set(FTM0_NUM, CH0, MAX_MOTOR_PWM);
    RED_LED_ON();
    GREEN_LED_ON();
    for(volatile int j = 0; j < CALIBRATION_DELAY; j++);
    RED_LED_OFF();
    GREEN_LED_OFF();

    /* set the duty cycle to the min */
    ftm_ch_duty_set(FTM0_NUM, CH0, MIN_MOTOR_PWM);
    RED_LED_ON();
    for(volatile int j = 0; j < CALIBRATION_DELAY; j++);
    RED_LED_OFF();

    return 0;
}

int rmotor_init() {
    /* initialize the pwm channel */
    ftm_ch_epwm_init(ftm0_ch1_conf);

    /* set the duty cycle to the max */
    ftm_ch_duty_set(FTM0_NUM, CH1, MAX_MOTOR_PWM);
    RED_LED_ON();
    GREEN_LED_ON();
    for(volatile int j = 0; j < CALIBRATION_DELAY; j++);
    RED_LED_OFF();
    GREEN_LED_OFF();

    /* set the duty cycle to the min */
    ftm_ch_duty_set(FTM0_NUM, CH1, MIN_MOTOR_PWM);
    RED_LED_ON();
    for(volatile int j = 0; j < CALIBRATION_DELAY; j++);
    RED_LED_OFF();

    return 0;
}

int lrmotor_init() {
    /* initialize the pwm channel */
    ftm_ch_epwm_init(ftm0_ch0_conf);
    ftm_ch_epwm_init(ftm0_ch1_conf);

    /* set the duty cycle to the max */
    ftm_ch_duty_set(FTM0_NUM, CH0, MAX_MOTOR_PWM);
    ftm_ch_duty_set(FTM0_NUM, CH1, MAX_MOTOR_PWM);
    RED_LED_ON();
    for(volatile int j = 0; j < CALIBRATION_DELAY; j++);
    RED_LED_OFF();

    /* set the duty cycle to the min */
    ftm_ch_duty_set(FTM0_NUM, CH0, MIN_MOTOR_PWM);
    ftm_ch_duty_set(FTM0_NUM, CH1, MIN_MOTOR_PWM);
    RED_LED_ON();
    for(volatile int j = 0; j < CALIBRATION_DELAY; j++);
    RED_LED_OFF();

    return 0;
}

int lmotor_set(uint8_t speed) {
    ftm_ch_duty_set(FTM0_NUM, CH0, speed);
    return 0;
}

int rmotor_set(uint8_t speed) {
    ftm_ch_duty_set(FTM0_NUM, CH1, speed);
    return 0;
}

int lrmotor_set(uint8_t speed) {
    ftm_ch_duty_set(FTM0_NUM, CH0, speed);
    ftm_ch_duty_set(FTM0_NUM, CH1, speed);
    return 0;
}
