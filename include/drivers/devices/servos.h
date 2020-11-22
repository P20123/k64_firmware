#pragma once
#include <MK64F12.h>
#include <stdint.h>
#include <drivers/kinetis/ftm.h>
#include <drivers/devices/status_leds.h>
#include <drivers/devices/servos.h>

/************
 * CONSTANTS
 ************/
/* FTM constants */
#define FTM3_NUM 3
#define CH4 4
#define CH5 5 

/* left servo control constants */
#define LEFT_SERVO_UP     1225
#define LEFT_SERVO_DOWN   800
#define LEFT_SERVO_SCALE  ((LEFT_SERVO_UP - LEFT_SERVO_DOWN) / 100)

/* right servo control constants */
#define RIGHT_SERVO_UP    700
#define RIGHT_SERVO_DOWN  1200
#define RIGHT_SERVO_SCALE ((RIGHT_SERVO_DOWN - RIGHT_SERVO_UP) / 100)

/************
 * FUNCTIONS
 ************/

/**
 * Intializes the FTM3 channel 4/5 for the servos
 * @return 0
 */
int servo_init();

/**
 * Sets the channel CnV for the left servo.
 * Note: value is checked to be in the range of the left servo.
 * @param value CnV value to set the ftm channel to.
 * @return 0
 */
int lservo_set_cnt(uint16_t value);

/**
 * Sets the channel CnV for the right servo.
 * Note: value is checked to be in the range of the right servo.
 * @param value CnV value to set the ftm channel to.
 * @return 0
 */
int rservo_set_cnt(uint16_t value);

/**
 * This function provides a handy interface to set the elevons
 * by scaling the value of the CnV to a value between 0 and 100.
 * (left servo)
 * Note: value is checked to be in the range of zero to 100.
 * Note: This function calls the respective set_cnt functions.
 * @param value Value between 0 and 100, where 0 is down and 100 is up.
 * @return 0 if no errors, -1 if value is invalid.
 */
int lservo_set_scaled(uint8_t value);

/**
 * This function provides a handy interface to set the elevons
 * by scaling the value of the CnV to a value between 0 and 100.
 * (right servo)
 * Note: value is checked to be in the range of zero to 100.
 * Note: This function calls the respective set_cnt functions.
 * @param value Value between 0 and 100, where 0 is down and 100 is up.
 * @return 0 if no errors, -1 if value is invalid.
 */
int rservo_set_scaled(uint8_t value);
