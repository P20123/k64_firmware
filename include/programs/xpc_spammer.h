#pragma once
#include <tinyxpc/tinyxpc.h>
#include <tinyxpc/xpc_relay.h>
#include <kernel/process.h>

#define XPC_SPAMMER_STACK_SIZE 100

extern pcb_t xpc_spammer_app;
extern uint32_t xpc_spammer_stack[XPC_SPAMMER_STACK_SIZE];

int xpc_spammer_main(void);
