/**
 * This file contains the kernel initialization.  There are no kernel threads
 * or processes, only exceptions, so this file is only concerned with
 * initialization.
 */

#include <kernel/file.h> /* ftab_init, temporary */
#include <drivers/arm/cm4/systick.h> /* ARM SysTick timer, for proc. sched. */
#include <kernel/schedule.h> /* Process scheduler */
#include <kernel/private/static_memory.h> /* Scheduler memory allocations */
#include <environment.h>
#include <drivers/kinetis/i2c.h>
#include <drivers/kinetis/uart.h>
#include <drivers/kinetis/ftm.h>
#include <drivers/devices/altimu.h>
#include <drivers/devices/status_leds.h>
#include <drivers/devices/motors.h>
#include <drivers/devices/servos.h>

/**
 * Safety check - deadloop if there is no init process linked.
 */
__attribute__((weak))
int init_main() { for(;;); }

__attribute__((noreturn))
void kernel_main(const char *cmdline) {
    // parse the command line here

    // kernel structure initialization here
    ftab_init();

    // initialize status leds first
#ifdef DEVICE_EN_STATUS_LEDS
    /** STATUS LED INIT **/
    status_leds_init();
#endif

    // peripheral initialization here
#ifdef KINETIS_USE_UART
    /** UART INIT **/
    uart0_conf.input_clock_rate = SystemCoreClock;
    uart0_fileno = uart_init(uart0_conf);
#endif

#ifdef KINETIS_USE_I2C
    /** I2C INIT **/
    i2c_init(i2c0_conf, SystemCoreClock / 2);
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[2] |= PORT_PCR_MUX(2);
    PORTB->PCR[3] |= PORT_PCR_MUX(2);
#endif

#ifdef KINETIS_USE_FTM
    ftm_init(ftm3_conf, SystemCoreClock);
    ftm_init(ftm0_conf, SystemCoreClock);
#endif

    // device initialization here

    /** LPTMR WAIT **/
    uint16_t time;
    uint16_t prev_time;

    /* status LED to denote LPTMR wait */
    GREEN_LED_ON();

    // enable clock for LPTMR
    SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK;
    // LPO 1kHz input for lptmr
    LPTMR0->PSR |= (1 << LPTMR_PSR_PCS_SHIFT);
    // bypass prescaler
    LPTMR0->PSR |= (1 << LPTMR_PSR_PBYP_SHIFT);

    // free-running mode
    LPTMR0->CSR |= (1 << LPTMR_CSR_TFC_SHIFT);
    // enable lptmr as a 16-second period counter without interrupts.
    LPTMR0->CSR |= (1 << LPTMR_CSR_TEN_SHIFT);

    // page 1104 of the ref. manual.  write before read... why?
    LPTMR0->CNR = 0;

    time = LPTMR0->CNR;
    prev_time = time;

    while((time - prev_time) < 1000) {
        LPTMR0->CNR = 0;
        time = LPTMR0->CNR;
    }

    /* status LED to denote LPTMR wait */
    GREEN_LED_OFF();

#ifdef DEVICE_EN_ALTIMU
    /** ALTIMU INIT **/
    altimu_gxl_init(0);
    altimu_mag_init(0);
    altimu_bar_init(0);
#endif

#ifdef DEVICE_EN_SERVOS
    /** SERVO INIT **/
    servo_init();
#endif

#ifdef DEVICE_EN_MOTORS
    /** MOTOR INIT **/
    lrmotor_init();
#endif

    /** PROCESS SCHEDULER INITIALIZATION **/
    // SysTick has priority 15
    SCB->SHP[3] |= (15 << 24);
    // PendSV has priority 14
    SCB->SHP[3] |= (14 << 16);
    // SVCall has priority 13
    SCB->SHP[2] |= (13 << 24);
    scheduler_init(&process_table, &process_list);

    /** USER PROCESS START **/
    // NOTE: INITIALIZATION MUST BE COMPLETE BEFORE THIS CALL.
    // in a real kernel, this is where the init daemon/process would be started
    // as the first userland process.  Since there is no fork(), malloc(), or
    // concept of dynamic processes, this is just a function to start all the
    // other processes.
    // NOTE: THIS PROCESS MUST NOT yield() AND SHOULD NOT BLOCK.
    init_main();
#ifndef KERNEL_MT_PURE_COOPERATIVE
    systick_init((1000)*((SystemCoreClock)/1000000), 1, 1, 1);
#else
    yield();
#endif
    for(;;);
}
