#pragma once

/************
 * CONSTANTS
 ************/
#define sampleFreq  529.0f          // sample frequency in Hz
#ifdef ALGO_EN_MAHONY
    #define Kp 90.0f
    #define Ki 30.5f
    #define twoKpDef    (2.0f * Kp) // 2 * proportional gain
    #define twoKiDef    (2.0f * Ki) // 2 * integral gain
#endif

#ifdef ALGO_EN_MADGWICK
    #define BETA_DEF     20.0f        // 2 * proportional gain
#endif
/**********
 * GLOBALS
 **********/
extern volatile float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame
extern volatile float roll, pitch, yaw;

#ifdef ALGO_EN_MAHONY
    extern volatile float twoKp;			// 2 * proportional gain (Kp)
    extern volatile float twoKi;			// 2 * integral gain (Ki)
#endif

#ifdef ALGO_EN_MADGWICK
    extern volatile float beta;				// algorithm gain
#endif

/*************
 * PROTOTYPES
 *************/
void ahrs_init();
void rpy_update();
void ahrs_update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void ahrs_update_imu(float gx, float gy, float gz, float ax, float ay, float az);

