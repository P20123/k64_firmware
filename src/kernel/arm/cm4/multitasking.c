#include <multitasking.h>
#include <MK64F12.h>

void yield(void) {
    SCB->ICSR |= (1 << SCB_ICSR_PENDSVSET_Pos);
    asm("dsb");
    asm("isb");
}

/**
 * Force the scheduler to run and preempt the currently running process.
 * This can be disabled by disabling the systick timer.
 */
void SysTick_Handler(void) {
    yield();
}
