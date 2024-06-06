#ifndef __MPU6050_REG_MAP__H
#define __MPU6050_REG_MAP__H

#define MPU6050_SELF_TEST_X             0x0D
#define MPU6050_SELF_TEST_Y             0x0E
#define MPU6050_SELF_TEST_Z             0x0F
#define MPU6050_SELF_TEST_A             0x10

#define MPU6050_SMPLRT_DIV              0x19

#define MPU6050_CONFIG                  0x1A
#define MPU6050_GYRO_CONFIG             0x27
#define MPU6050_ACCEL_CONFIG            0x28
#define MPU6050_FIFO_EN                 0x23

#define MPU6050_ACCEL_XH                0x3B
#define MPU6050_ACCEL_XL                0x3C
#define MPU6050_ACCEL_YH                0x3D
#define MPU6050_ACCEL_YL                0x3E
#define MPU6050_ACCEL_ZH                0x3F
#define MPU6050_ACCEL_ZL                0x40
#define MPU6050_TEMP_H                  0x41
#define MPU6050_TEMP_L                  0x42
#define MPU6050_GYRO_XH                 0x43
#define MPU6050_GYRO_XL                 0x44
#define MPU6050_GYRO_YH                 0x45
#define MPU6050_GYRO_YL                 0x46
#define MPU6050_GYRO_ZH                 0x47
#define MPU6050_GYRO_ZL                 0x48

#define MPU6050_USER_CTRL               0x6A
#define MPU6050_POW_MANAGEMENT_1_REG    0x6B
#define MPU6050_POW_MANAGEMENT_2_REG    0x6C
#define MPU6050_FIFO_COUNTH             0x72
#define MPU6050_FIFO_COUNTL             0x73
#define MPU6050_FIFO_RW                 0x74
#define MPU6050_WHOAMI                  0x75

#define MPU6050_SIGNAL_PATH_RESET       0x68

#define MPU6050_INT_PIN_CFG             0x37
#define MPU6050_INT_ENABLE              0x38
#define MPU6050_INT_STATUS              0x3A
#endif