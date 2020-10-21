/**
 * This file is a shim to allow separation of kernel and application code if it
 * is needed in a future application.
 */

#include <kernel/file.h> /* ftab_init, temporary */

__attribute__((weak))
int main() { return 0; }

__attribute__((noreturn))
void kernel_main(const char *cmdline) {
    // parse the command line here

    // kernel structure initialization here
    ftab_init();

    // device initialization here

    // branch to main. Catch the program counter if the application main
    // exits, faults, or spuriously returns.
    main();
    for(;;);
}
