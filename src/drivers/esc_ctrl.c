/*
 * Servo control
 */
#include "MK64F12.h"
#include "drivers/esc_ctrl.h"

/*From clock setup 0 in system_MK64f12.c*/
#ifndef GCC
    #define DEFAULT_SYSTEM_CLOCK SystemCoreClock /* Default System clock value */
#endif

// 20.9 MHz clock, 
#define CORRECTION              50 //3200 //do we even need correction?
#define CLOCK					SystemCoreClock //DEFAULT_SYSTEM_CLOCK
#define PWM_FREQUENCY			50
#define FTM0_MOD_VALUE		    (CLOCK/(2*PWM_FREQUENCY)) + CORRECTION
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
 * Change the angle of the left servo, maybe duty cycle would be better?
 */
void esc_set_leftduty(unsigned int duty) {
    //These values correspond to the servo limits
    //value = (value < 2485) ? 2485 : value;
    //value = (value > 3285) ? 3285 : value;

    // change the duty cycle
    FTM0->CONTROLS[0].CnV = (uint16_t) ((FTM0->MOD * duty) / 100);
}

/*
 * Change the angle of the right servo
 */
void esc_set_rightduty(unsigned int duty){
    //These values correspond to the servo limits
    //value = (value < 2485) ? 2485 : value;
    //value = (value > 3285) ? 3285 : value;

    //change the duty cycle
    FTM0->CONTROLS[1].CnV = (uint16_t) ((FTM0->MOD * duty) / 100);
}

/*
 * Initialize the ESC IO
 */
void esc_init_io() {
    // Enable clock on PORT C
    SIM -> SCGC5 |= SIM_SCGC5_PORTC_MASK;

    // route ftm0 to port c pins
    PORTC->PCR[1]  = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK;  
    PORTC->PCR[2]  = PORT_PCR_MUX(4)  | PORT_PCR_DSE_MASK;
}

/*
 * Initialize the esc pwm, use a 100 Hz frequency to start
 */
void esc_init_pwm() {
    // 12.2.13 Enable the Clock to the FTM0 Module
    SIM -> SCGC6 |= SIM_SCGC6_FTM0_MASK;

    // 39.3.10 Disable Write Protection
    FTM0->MODE |= FTM_MODE_WPDIS_MASK;
    
    // 39.3.4 FTM Counter Value: Initialize the CNT to 0 before writing to MOD
    FTM0->CNT = 0;

    // 39.3.8 Set the Counter Initial Value to 0
    FTM0->CNTIN = 0;

    // 39.3.5 Set the Modulo resister
    FTM0->MOD = FTM0_MOD_VALUE;

    // 39.3.6 Set the Status and Control of both channels
    // Used to configure mode, edge and level selection
    // See Table 39-67,  Edge-aligned PWM, High-true pulses (clear out on match)
    FTM0->CONTROLS[0].CnSC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    FTM0->CONTROLS[0].CnSC &= ~FTM_CnSC_ELSA_MASK;

    FTM0->CONTROLS[1].CnSC |= FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK; 
    FTM0->CONTROLS[1].CnSC &= ~FTM_CnSC_ELSA_MASK;

    // 40.3.3 FTM Setup
    // Set prescale value to 4
    // Chose system clock source
    // Timer Overflow Interrupt Enable
    FTM0->SC = FTM_SC_PS(5) | FTM_SC_CLKS(1) | FTM_SC_TOIE_MASK;
}
