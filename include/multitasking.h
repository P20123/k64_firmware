#pragma once
/**
 * These are convenience functions for both preemptive and cooperative
 * multitasking
 */

/**
 * Cause the calling thread to yield, thus allowing other threads to
 * run.
 */
void yield();
