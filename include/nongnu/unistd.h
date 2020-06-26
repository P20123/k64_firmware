#pragma once
#include <kernel/kernel_ftab.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/**
 * Write the contents of buf up to nbyte to filedes.
 * Refer to man 3 write for more information.
 */
ssize_t write(int fildes, const void *buf, size_t nbyte);

/**
 * Read at most nbyte bytes from fildes into buf.
 * Refer to man 3 read for more information.
 */
ssize_t read(int fildes, void *buf, size_t nbyte);
