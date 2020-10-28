/*
 * Servo control
 */
#include "MK64F12.h"
#include "drivers/servo_ctrl.h"

/*From clock setup 0 in system_MK64f12.c*/
#ifndef GCC
    #define DEFAULT_SYSTEM_CLOCK SystemCoreClock /* Default System clock value */
#endif

// 20.9 MHz clock, 
#define CORRECTION              0 //3200 //do we even need correction?
#define CLOCK					SystemCoreClock //DEFAULT_SYSTEM_CLOCK
#define PWM_FREQUENCY			50
#define FTM3_MOD_VALUE			(CLOCK/(2*PWM_FREQUENCY)) - CORRECTION
#define SERVO_SAUCE             27

/*const uint16_t POSITIONS[9] = {
    2485, // LEFT 0
    2585,
    2685,
    2785,
    2885, // CENTER 4
    2985,
    3085,
    3185,
    3285, // RIGHT 8
}; */

/*
 * Change the angle of the left servo
 */
void servo_set_leftangle(int16_t value) {
    //These values correspond to the servo limits
    //value = (value < 2485) ? 2485 : value;
    //value = (value > 3285) ? 3285 : value;

    // change the duty cycle
    FTM3->CONTROLS[4].CnV = value;
}

/*
 * Change the angle of the right servo
 */
void servo_set_rightangle(int16_t value){
    //These values correspond to the servo limits
    //value = (value < 2485) ? 2485 : value;
    //value = (value > 3285) ? 3285 : value;

    //change the duty cycle
    FTM3->CONTROLS[5].CnV = value;
}

/*
 * Initialize the servo IO
 */
void servo_init_io() {
    // Enable clock on PORT C
    SIM -> SCGC5 |= SIM_SCGC5_PORTC_MASK;

    // route ftm3 to port c pins
    PORTC->PCR[8]  = PORT_PCR_MUX(3)  | PORT_PCR_DSE_MASK;  // ch4
    PORTC->PCR[9]  = PORT_PCR_MUX(3)  | PORT_PCR_DSE_MASK;
}

/*
 * Initialize the servo pwm, use a 50 Hz frequency
 */
void servo_init_pwm() {
    // 12.2.13 Enable the Clock to the FTM3 Module
    SIM -> SCGC3 |= SIM_SCGC3_FTM3_MASK;

    // 39.3.10 Disable Write Protection
    FTM3->MODE |= FTM_MODE_WPDIS_MASK;
    
    // 39.3.4 FTM Counter Value: Initialize the CNT to 0 before writing to MOD
    FTM3->CNT = 0;

    // 39.3.8 Set the Counter Initial Value to 0
    FTM3->CNTIN = 0;

    // 39.3.5 Set the Modulo resister
    FTM3->MOD = FTM3_MOD_VALUE;

    // 39.3.6 Set the Status and Control of both channels
    // Used to configure mode, edge and level selection
    // See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
    FTM3->CONTROLS[4].CnSC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    FTM3->CONTROLS[4].CnSC &= ~FTM_CnSC_ELSA_MASK;

    FTM3->CONTROLS[5].CnSC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK; 
    FTM3->CONTROLS[5].CnSC &= ~FTM_CnSC_ELSA_MASK;

    // 39.3.3 FTM Setup
    // Set prescale value to 4
    // Chose system clock source
    // Timer Overflow Interrupt Enable
    FTM3->SC = FTM_SC_PS(5) | FTM_SC_CLKS(1) | FTM_SC_TOIE_MASK;
}
