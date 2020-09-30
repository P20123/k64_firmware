#pragma once
#include <stdint.h>
// the linker provides this symbol, it is the starting LOAD address of the
// debug_strs section
extern const char *__start_debug_strs;
#define MAKE_DEBUG_STR(STRING, TARGET) {\
    static const char *TARGET_STR __attribute__((section ("debug_strs"))) = STRING;\
    TARGET = &TARGET_STR - &__start_debug_strs;\
}

#define os_log(x, ...) {\
    uint16_t id;\
    MAKE_DEBUG_STR(x, id);\
    vos_log(x, id __VA_OPT__(,) __VA_ARGS__);\
}

// FIXME MOVE THIS TO ITS OWN HEADER
/**
 * Consume a printf-style format specifier.  This function takes one
 * %[flag][width][.][precision][length mod]<conversion> at the start of a format
 * string and returns the number of bytes consumed by that argument before
 * formatting (in binary form, not text form).
 *
 * See man 3 printf for information about these specifiers.
 * @param fmt the format string
 * @param endptr pointer to the last byte processed by this function, set to
 * NULL to disregard
 * @return the number of bytes of this argument.  -1 if strlen needs to be
 * called on the argument, and -2 if wcslen needs to be called on the arg.
 */
int yield_fmt_arg_bytes(char *fmt, char **endptr);
