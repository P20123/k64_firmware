#include <stddef.h>
#include <MK64F12.h>
/**
 * This file is a shim for a real bootloader.  It acts as a way to call the
 * kernel with a command line, if any, from an already-running CPU and memory.
 */

/**
 * This function is the entry for the OS kernel or other baremetal program.
 */
extern void kernel_main(const char *cmdline);

/**
 * Provided by NXP startup code. Ensures SystemCoreClock (global var) is
 * correct.
 */
extern void SystemCoreClockUpdate(void);

static const char kernel_commandline[] = "";

volatile uint32_t *FPCCR = (uint32_t*)0xe000ef34u;


__attribute__((noreturn))
void bootloader_entry() {
    // if there are pre-kernel drivers that need initialization, those would
    // go here.
    // turn off fpu automatic stacking & lazy stacking until a proecss needs it
    // (this is a stack-space saving measure)
    *FPCCR &= ~(3 << 30);
    SystemCoreClockUpdate();

    // branch to the entry point
    kernel_main(kernel_commandline);
    // this is for safety, in case the kernel main exits or spuriously branches
    // to the link register.
    for(;;);
}
