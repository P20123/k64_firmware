#include <multitasking.h>
#include <MK64F12.h>

void yield(void) {
    SCB->ICSR |= (1 << SCB_ICSR_PENDSVSET_Pos);
    asm("dsb");
    asm("isb");
}
