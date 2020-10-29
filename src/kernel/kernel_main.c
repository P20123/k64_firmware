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
#include <drivers/devices/altimu.h>

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

    /** UART INIT **/
    uart0_conf.input_clock_rate = SystemCoreClock;
    uart0_fileno = uart_init(uart0_conf);
    /** END UART INIT **/

    /** I2C INIT **/
    i2c_init(i2c0_conf, SystemCoreClock / 2);
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB->PCR[2] |= PORT_PCR_MUX(2);
    PORTB->PCR[3] |= PORT_PCR_MUX(2);
    /** END I2C INIT **/

    // device initialization here

    /** I2C SENSOR INIT **/
    altimu_gxl_init(0);
    altimu_mag_init(0);
    altimu_bar_init(0);
    /** END I2C SENSOR INIT **/

    /** PROCESS SCHEDULER INITIALIZATION **/
    // SVCall has priority 14
    SCB->SHP[2] |= (14 << 24);
    // PendSV has priority 15
    SCB->SHP[3] |= (15 << 16);
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
