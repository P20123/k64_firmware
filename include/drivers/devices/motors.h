#pragma once
#include <MK64F12.h>
#include <stdint.h>
#include <drivers/kinetis/ftm.h>
#include <drivers/devices/motors.h>
#include <drivers/devices/status_leds.h>

/************
 * CONSTANTS
 ************/
#define FTM0_NUM 0
#define CH0 0
#define CH1 1
#define MAX_MOTOR_PWM 99
#define MIN_MOTOR_PWM 35
#define CALIBRATION_DELAY 5000000

/*************
 * PROTOTYPES
 *************/

/**
 * Initializes the left motor FTM channel and calibrates the ESC.
 * @return 0
 */
int lmotor_init();

/**
 * Initializes the right motor FTM channel and calibrates the ESC.
 * @return 0
 */
int rmotor_init();

/**
 * Initializes both motor FTM channels and calibrates both ESCs.
 * @return 0
 */
int lrmotor_init();

/**
 * Sets the left motor to a specific speed.
 * Note: speed is checked to be in the bounds of MIN/MAX_MOTOR_PWM.
 * @param speed A value from MIN_MOTOR_PWM to MAX_MOTOR_PWM.
 * @return 0 if no errors, -1 if invalid number.
 */
int lmotor_set(uint8_t speed);

/**
 * Sets the right motor to a specific speed.
 * Note: speed is checked to be in the bounds of MIN/MAX_MOTOR_PWM.
 * @param speed A value from MIN_MOTOR_PWM to MAX_MOTOR_PWM.
 * @return 0 if no errors, -1 if invalid number.
 */
int rmotor_set(uint8_t speed);

/**
 * Sets both motors to a specific speed.
 * Note: speed is checked to be in the bounds of MIN/MAX_MOTOR_PWM.
 * @param speed A value from MIN_MOTOR_PWM to MAX_MOTOR_PWM.
 * @return 0 if no errors, -1 if invalid number.
 */
int lrmotor_set(uint8_t speed);
