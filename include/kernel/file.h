#pragma once

/**
 * Function typedefs for read, write, and close functions used by the
 * kernel file descriptor table.
 */
typedef unsigned int(*read_fn)(void *context, void *buf, unsigned int bytes);
typedef unsigned int(*write_fn)(void *context, void *buf, unsigned int bytes);
typedef unsigned int(*close_fn)(void *context);

/**
 * Datatype describing a single entry in the file descriptor table
 */
typedef struct {
    void *context;
    read_fn read;
    write_fn write;
    close_fn close;
    struct {
        char read_pid;
        char write_pid;
    } locks;
} ftab_entry_t;

#define FDT_SIZE 15

/**
 * File descriptor table fields
 */
typedef struct {
    int next_fd;
    ftab_entry_t entries[FDT_SIZE];
} ftab_t;

// global, located in kernel_support.c
extern ftab_t file_descriptor_table;
// needed for asm linkage
extern void *fdt_address;

// functions

/**
 * I couldn't find a way to do this automagically with GCC, so call this
 * function to zero-out the FDT.
 */
void ftab_init();

// return: file descriptor number
int ftab_open(ftab_entry_t entry);

// return: 0 on success
int ftab_close(int fd);
