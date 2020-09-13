/**
 * This file is a shim to allow separation of kernel and application code if it
 * is needed in a future application.
 */

extern int main(void);

__attribute__((noreturn))
void kernel_main(const char *cmdline) {
    // parse the command line here

    // device initialization here

    // branch to main. Catch the program counter if the application main
    // exits, faults, or spuriously returns.
    main();
    for(;;);
}
