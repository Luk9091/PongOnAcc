#ifndef __MPU6050__H
#define __MPU6050__H

#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>

#include "i2c.h"
#include "axis.h"

// LOW address of MPU6050
#define I2C_MPU6050_ADDRESS 0x68
#define GRAVITY_CONTAIN 9.80665f


/*
 * If true then make calibration rotate acceleration.
 * Under calibration process MPU has not to move! 
 * 
 * If false then use default parameter
*/
#define MPU6050_CALIBRATE_GYRO false

static const axisf_t MPU6050_CALIBRATION_DATA_GYRO_OFFSET    = {-630, 311,  65};
static const axisf_t MPU6050_CALIBRATION_DATA_GYRO_THRESHOLD = {10, 12, 10};

// #define MPU6050_CALIBRATION_ACC false

// #define MPU6050_CALIBRATION_DATA_ACC_OFFSET_X  1000
// #define MPU6050_CALIBRATION_DATA_ACC_OFFSET_Y  -400
// #define MPU6050_CALIBRATION_DATA_ACC_OFFSET_Z  -880

typedef enum{
    RANGE_2G = 1,
    RANGE_4G = 2,
    RANGE_8G = 3,
    RANGE_16G= 4,
} MPU6050_ACC_RANGE_t;

// DPS = degree per second
typedef enum{
    RANGE_250DPS  = 0,
    RANGE_500DPS  = 1,
    RANGE_1000DPS = 2,
    RANGE_2000DPS = 3,
} MPU6050_GYRO_RANGE_t;


typedef enum {
    MPU6050_OK  = 0,
} MPU6050_ERROR_t;

typedef enum {
    MPU6050_INT_DATA_RDY_EN = 0,
    MPU6050_INT_FIFO_OVERFLOW = 4,
    MPU6050_INT_I2C_MST_INT = 3,
} MPU6050_INT_t;


void mpu6050_Init();
void mpu6050_enableInt(MPU6050_INT_t intType);

/*
 * Read linear acceleration in mm/s²
*/

void mpu6050_readRawAcc(axis16_t *acc);
void mpu6050_readRawGyro(axis16_t *gyro);

void mpu6050_readAcc(axisf_t *acc);
void mpu6050_readGyro(axisf_t *gyro);
void mpu6050_readTemp(float *temp);


/*
 * Linear acceleration in mm/s² ax int16
 * Rotate acceleration in deg/s² ax int16
 * Temperature in float
*/
void mpu6050_readData(axisf_t *acc, axisf_t *gyro, float *temp);


void mpu6050_gyro_calibration();
void mpu6050_acc_calibration();

#endif