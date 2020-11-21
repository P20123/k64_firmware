#include <algorithms/pid.h>
#include <stdint.h>

pid_context_t *pid_ctx_init(
    pid_context_t *ctx,
    int32_t kp, int32_t ki, int32_t kd,
    int32_t setpoint_low,
    int32_t setpoint_high,
    int32_t clamp_min,
    int32_t clamp_max
) {
    ctx->kp = kp;
    ctx->ki = ki;
    ctx->kd = kd;
    for(int i = 0; i < HISTORY_SIZE; i++) {
        ctx->error_history[i] = 0;
    }

    ctx->setpoint_low = setpoint_low;
    ctx->setpoint_high = setpoint_high;
    ctx->clamp_min = clamp_min;
    ctx->clamp_max = clamp_max;
    ctx->error_idx = 0;
    return ctx;
}

// okay, here's the breakdown
// current error goes to zero when we are in tune, but then we return
// CLAMP(0, > 0, > 0), so we get the min value, thus introducing a constant
// error.
// So, we either need to keep track of the current value and return it if we are
// within bounds, or do something else, like shift this to return an adjustment
// from some mean.
int32_t pid_next(pid_context_t *ctx, int32_t current) {
    // error is defined as:
    // 0 if we are within the bounds
    // distance from average of bounds otherwise.
    int32_t current_error = 0;
    int32_t correction_factor = 0;
    if(current < ctx->setpoint_high && current > ctx->setpoint_low) {
        current_error = 0;
    }
    else {
        current_error =
            ((ctx->setpoint_low + ctx->setpoint_high) >> 1) - current;

    }
    ctx->error_history[ctx->error_idx] = current_error;
    ctx->error_idx = (ctx->error_idx + 1) % HISTORY_SIZE;
    if(ctx->kp != 0) {
        correction_factor = ctx->kp*current_error;
    }
    if(ctx->ki != 0) {
        int32_t sum = 0;
        for(int i = 0; i < HISTORY_SIZE; i++) {
            sum += ctx->error_history[i];
        }
        correction_factor += ctx->ki*sum;
    }
    if(ctx->kd != 0) {
        int32_t diff = ctx->error_history[(ctx->error_idx - 1) % HISTORY_SIZE]
            - ctx->error_history[ctx->error_idx];
        correction_factor += ctx->kd*diff;
    }
    return CLAMP(correction_factor, ctx->clamp_min, ctx->clamp_max);
}
