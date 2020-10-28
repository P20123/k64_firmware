#pragma once
#include <stddef.h>
#include <sys/types.h>

size_t kernel_write(int fd, char *buf, size_t bytes);

size_t kernel_read(int fd, char *buf, size_t bytes);
