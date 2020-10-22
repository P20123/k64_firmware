/**
 * This file is a shim to allow separation of kernel and application code if it
 * is needed in a future application.
 */

#include <kernel/kernel_ftab.h> /* ftab_init, temporary */

__attribute__((weak))
int main() { return 0; }

__attribute__((noreturn))
void kernel_main(const char *cmdline) {
    // parse the command line here

    // kernel structure initialization here
    ftab_init();

    // device initialization here
    for(int *i = (int *)0x1FFF0000; i < (int *)20030000; i++) {
        *i = 0;
    }

    // branch to main. Catch the program counter if the application main
    // exits, faults, or spuriously returns.
    main();
    for(;;);
}
