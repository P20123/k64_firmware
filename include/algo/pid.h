#pragma once
#include <stdint.h>
#define HISTORY_SIZE 3
// clamp x to a value at most max and at least min.
#define CLAMP(x, min, max) (((x) < (min)) ? (min):((x) > (max)) ? (max):(x))

/**
 * Structure to hold the state of a PID control loop
 */
typedef struct {
    int32_t kp, ki, kd;
    int32_t error_history[HISTORY_SIZE];
    int32_t error_idx;
    int32_t setpoint_low;
    int32_t setpoint_high;
    int32_t clamp_min;
    int32_t clamp_max;
} pid_context_t;

/**
 * Create a new pid_context_t object
 */
pid_context_t *pid_ctx_init(
    pid_context_t *ctx,
    int32_t kp, int32_t ki, int32_t kd,
    int32_t setpoint_low,
    int32_t setpoint_high,
    int32_t clamp_min,
    int32_t clamp_max
);

/*
 * Obtain the next correction factor using a given context and current value.
 */
int32_t pid_next(pid_context_t *ctx, int32_t current);
