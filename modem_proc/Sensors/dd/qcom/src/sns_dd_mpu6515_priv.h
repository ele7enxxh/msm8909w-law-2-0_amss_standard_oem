/**
 * @file sns_dd_mpu6515_priv.h
 * @brief Common definitions for the MPU-6515 driver.
 *
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2014-02-27 lka  Added macros for DMP current consumptions.
  2014-02-12 lka  Added debug macros.
  2013-12-19 lka  Modified function prototypes.
  2013-12-09  sd  Added general FIFO suport
  2013-10-11 lka  Added a copy of ODR before switching to MD interrupt.
  2013-09-30 lka  Added SMGR and DD FIFO struct in state variable.
  2013-09-13  pn  Upped DMP_MAX_NUM_SAMPLES
  2013-09-09  ad  Fix compiler warnings
  2013-09-06  pn  DMP constants moved over from source file
  2013-08-28 lka  Added debug function prototypes. Fixed a bit name.
  2013-08-26  pn  Added boolean field int_registered.
  2013-08-21  sd  Added current_thres field
  2013-07-14 lka  Added entire MPU6515 register map.
  2013-07-12 lka  Changes to state structure
  2013-07012  asr Generalized support for akm8963/bmp280 i2c slave devices in DMP Passthrough configuration and cleaned up code.
  2013-07-12  sd  Added axis map for each sensor
  2013-07-11  pn  Minor changes for HW selftest
  2013-06-28 lka  Added I2C master-specific definitions.
                  Added support for AKM8963 pass-through.
  2013-06-20 lka  Initial version

==============================================================================*/

#ifndef SNS_DD_MPU6515__H
#define SNS_DD_MPU6515__H

#include "fixed_point.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_util.h"
#include "sns_debug_str.h"

#include "sns_dd_mag_akm8963_priv.h"
#include "sns_dd_alt_bmp280.h"

/*------------------------------------------------------------------------------
  Definitions for driver configuration
------------------------------------------------------------------------------*/
/* Using ADVANCED DEBUG MESSAGES */
//#define ADVANCED_DEBUG_MSG

/* the valid values are 1, 5, 20, 40*/
#define MPU6050_CYCLE_MODE_LIMIT_HZ            5    /* 0 means no cycle mode is used */
#define MPU6050_CYCLE_MODE_MD_HZ               20   /* cycle mode freq for MD */

/*------------------------------------------------------------------------------
  Register definitions
------------------------------------------------------------------------------*/
#define MPU6050_REG_PRODUCT_ID                   0x0C
    #define PRODUCT_REV_MASK                     0xF
    #define REV_C_1                              0x4
    #define REV_C_2                              0x5
    #define REV_D_1                              0x6
    #define REV_D_2                              0x7
    #define REV_D_3                              0x8

/* MPU6050 self-test register mapping */
#define MPU6050_REG_SELF_TEST_X                  0x0D
#define MPU6050_REG_SELF_TEST_Y                  0x0E
#define MPU6050_REG_SELF_TEST_Z                  0x0F
    #define CODE_GYRO(v)                         ((v) & 0x1F)
    #define CODE_ACCEL(v)                        (((v) & 0xE0) >> 3)

#define MPU6050_REG_SELF_TEST_ACCEL              0x10
    #define SELF_TEST_ACCEL_Z(v)                 (((v) & 0x03) >> 0)
    #define SELF_TEST_ACCEL_Y(v)                 (((v) & 0x0C) >> 2)
    #define SELF_TEST_ACCEL_X(v)                 (((v) & 0x30) >> 4)

/* MPU6500 self-test register mapping */
#define MPU6500_REG_SELF_TEST_GX                 0x00
#define MPU6500_REG_SELF_TEST_GY                 0x01
#define MPU6500_REG_SELF_TEST_GZ                 0x02
#define MPU6500_REG_SELF_TEST_AX                 0x0D
#define MPU6500_REG_SELF_TEST_AY                 0x0E
#define MPU6500_REG_SELF_TEST_AZ                 0x0F

#define MPU6050_REG_XG_OFFSET_H                  0x13
#define MPU6050_REG_XG_OFFSET_L                  0x14
#define MPU6050_REG_YG_OFFSET_H                  0x15
#define MPU6050_REG_YG_OFFSET_L                  0x16
#define MPU6050_REG_ZG_OFFSET_H                  0x17
#define MPU6050_REG_ZG_OFFSET_L                  0x18

#define MPU6050_REG_SMPRT_DIV                    0x19
    // 8-bit unsigned value. The Sample Rate is determined by dividing the
    // gyroscope output rate by this value.
    #define SMPLRT_DIV                           0

#define MPU6050_REG_CONFIG                       0x1A
    // Configures the Digital Low Pass Filter setting for accel. These are
    // directly tied to gyro setting, i.e. modifying one, will modify the other.
    #define DLPF_CFG                             0
        #define DLPF_CFG_ACCEL_260HZ             (0 << DLPF_CFG)
        #define DLPF_CFG_ACCEL_184HZ             (1 << DLPF_CFG)
        #define DLPF_CFG_ACCEL_94HZ              (2 << DLPF_CFG)
        #define DLPF_CFG_ACCEL_44HZ              (3 << DLPF_CFG)
        #define DLPF_CFG_ACCEL_21HZ              (4 << DLPF_CFG)
        #define DLPF_CFG_ACCEL_10HZ              (5 << DLPF_CFG)
        #define DLPF_CFG_ACCEL_5HZ               (6 << DLPF_CFG)

        #define DLPF_CFG_GYRO_256HZ              DLPF_CFG_ACCEL_260HZ
        #define DLPF_CFG_GYRO_188HZ              DLPF_CFG_ACCEL_184HZ
        #define DLPF_CFG_GYRO_98HZ               DLPF_CFG_ACCEL_94HZ
        #define DLPF_CFG_GYRO_42HZ               DLPF_CFG_ACCEL_44HZ
        #define DLPF_CFG_GYRO_20HZ               DLPF_CFG_ACCEL_21HZ
        #define DLPF_CFG_GYRO_10HZ               DLPF_CFG_ACCEL_10HZ
        #define DLPF_CFG_GYRO_5HZ                DLPF_CFG_ACCEL_5HZ

    // Configures the FSYNC pin sampling.
    #define EXT_SYNC_SET                         3
        #define EXT_SYNC_SET_DISABLED            (0 << EXT_SYNC_SET)
        #define EXT_SYNC_SET_TEMP_OUT_L          (1 << EXT_SYNC_SET)
        #define EXT_SYNC_SET_GYRO_XOUT_L         (2 << EXT_SYNC_SET)
        #define EXT_SYNC_SET_GYRO_YOUT_L         (3 << EXT_SYNC_SET)
        #define EXT_SYNC_SET_GYRO_ZOUT_L         (4 << EXT_SYNC_SET)
        #define EXT_SYNC_SET_ACCEL_XOUT_L        (5 << EXT_SYNC_SET)
        #define EXT_SYNC_SET_ACCEL_YOUT_L        (6 << EXT_SYNC_SET)
        #define EXT_SYNC_SET_ACCEL_ZOUT_L        (7 << EXT_SYNC_SET)

#define MPU6500_REG_CONFIG             0x1A
    //Gyro Low pass filter configurations for MPU6500
    #define MPU6500_DLPF_CFG_GYRO_250HZ          (0 << DLPF_CFG)
    #define MPU6500_DLPF_CFG_GYRO_184HZ          (1 << DLPF_CFG)
    #define MPU6500_DLPF_CFG_GYRO_92HZ           (2 << DLPF_CFG)
    #define MPU6500_DLPF_CFG_GYRO_41HZ           (3 << DLPF_CFG)
    #define MPU6500_DLPF_CFG_GYRO_20HZ           (4 << DLPF_CFG)
    #define MPU6500_DLPF_CFG_GYRO_10HZ           (5 << DLPF_CFG)
    #define MPU6500_DLPF_CFG_GYRO_5HZ            (6 << DLPF_CFG)
    #define MPU6500_DLPF_CFG_GYRO_3600HZ         (7 << DLPF_CFG)
    //Used to bypass the DLPF configuration in the configuration register 1A
    #define MPU6500_FCHOICE_B                    0
        //Enable DLPF is inferred 00 from inversion of 11 from section 4.5
        #define MPU6500_FCHOICE_B_ENABLE_DLPF    (0 << MPU6500_FCHOICE_B)
        // The values 1,2 and 3 corresponds to diable DLPF
        #define MPU6500_FCHOICE_B_DISABLE_DLPF   (3 << MPU6500_FCHOICE_B)

    // EXT_SYNC_SET is the same as the MPU6050, see above
      // Configures the FSYNC pin sampling.

    // Determines the FIFO mode
    // Set to '1', when the FIFO is full, additional writes will not be written to FIFO
    // Set to '0', when the FIFO is full, additional writes will be written to the FIFO, replacing the oldest data
    #define FIFO_MODE                            6
        #define FIFO_FULL_WRITE                  (0 << FIFO_MODE)
        #define FIFO_FULL_NO_WRITE               (1 << FIFO_MODE)

#define MPU6050_REG_GYRO_CONFIG                  0x1B
    // Selects the full scale range of gyroscopes.
    #define FS_SEL                               3
        #define FS_SEL_250DPS                    (0 << FS_SEL)
        #define FS_SEL_500DPS                    (1 << FS_SEL)
        #define FS_SEL_1000DPS                   (2 << FS_SEL)
        #define FS_SEL_2000DPS                   (3 << FS_SEL)
    // The Z- Axis gyroscope performs self test.
    #define ZG_ST                                5
        #define ZG_ST_DISABLE                    (0 << ZG_ST)
        #define ZG_ST_ENABLE                     (1 << ZG_ST)
    // The Y- Axis gyroscope performs self test.
    #define YG_ST                                6
        #define YG_ST_DISABLE                    (0 << YG_ST)
        #define YG_ST_ENABLE                     (1 << YG_ST)
    // The X- Axis gyroscope performs self test.
    #define XG_ST                                7
        #define XG_ST_DISABLE                    (0 << XG_ST)
        #define XG_ST_ENABLE                     (1 << XG_ST)

#define MPU6500_REG_GYRO_CONFIG                  0x1B
    // The FCHOICE is used to bypass the DLPF.
    #define MPU6500_FCHOICE_B                    0

#define MPU6050_REG_ACCEL_CONFIG                 0x1C
    // Selects the Digital High Pass Filter configuration.
    #define ACCEL_HPF                            0
        #define ACCEL_HPF_None                   (0 << ACCEL_HPF)
        #define ACCEL_HPF_5HZ                    (1 << ACCEL_HPF)
        #define ACCEL_HPF_2_5HZ                  (2 << ACCEL_HPF)
        #define ACCEL_HPF_1_25HZ                 (3 << ACCEL_HPF)
        #define ACCEL_HPF_0_63HZ                 (4 << ACCEL_HPF)
        #define ACCEL_HPF_HOLD                   (7 << ACCEL_HPF)
    // Selects the full scale range of accelerometers.
    #define AFS_SEL                              3
        #define AFS_SEL_2G                       (0 << AFS_SEL)
        #define AFS_SEL_4G                       (1 << AFS_SEL)
        #define AFS_SEL_8G                       (2 << AFS_SEL)
        #define AFS_SEL_16G                      (3 << AFS_SEL)
    // Performs self test for the accelerometer Z- Axis
    #define ZA_ST                                5
        #define ZA_ST_DISABLE                    (0 << ZA_ST)
        #define ZA_ST_ENABLE                     (1 << ZA_ST)
    // Performs self test for the accelerometer Y- Axis
    #define YA_ST                                6
        #define YA_ST_DISABLE                    (0 << YA_ST)
        #define YA_ST_ENABLE                     (1 << YA_ST)
    // Performs self test for the accelerometer X- Axis
    #define XA_ST                                7
        #define XA_ST_DISABLE                    (0 << XA_ST)
        #define XA_ST_ENABLE                     (1 << XA_ST)

#define MPU6500_REG_ACCEL_CONFIG_2               0x1D
    //Accel config 2 normal mode
    #define A_DLPFCFG                            0

        //Accel
        #define MPU6500_ACCEL_DLPFCFG_NORMAL_460HZ         (0 << A_DLPFCFG)
        #define MPU6500_ACCEL_DLPFCFG_NORMAL_184HZ         (1 << A_DLPFCFG)
        #define MPU6500_ACCEL_DLPFCFG_NORMAL_92HZ          (2 << A_DLPFCFG)
        #define MPU6500_ACCEL_DLPFCFG_NORMAL_41HZ          (3 << A_DLPFCFG)
        #define MPU6500_ACCEL_DLPFCFG_NORMAL_20HZ          (4 << A_DLPFCFG)
        #define MPU6500_ACCEL_DLPFCFG_NORMAL_10HZ          (5 << A_DLPFCFG)
        #define MPU6500_ACCEL_DLPFCFG_NORMAL_5HZ           (6 << A_DLPFCFG)
        #define MPU6500_ACCEL_DLPFCFG_NORMAL_460HZ_REPEAT  (7 << A_DLPFCFG)

    //Accel FChoice B used to decide LP or normal for DLPF. refer section 4.8
    #define MPU6500_ACCEL_FCHOICE_B               2    //was 3
        #define MPU6500_ACCEL_FCHOICE_B_NORMAL    (0 << MPU6500_ACCEL_FCHOICE_B)
        #define MPU6500_ACCEL_FCHOICE_B_LP        (1 << MPU6500_ACCEL_FCHOICE_B)

    // Fifo size control. Note: after the FIFO size is set, the FIFO should be reset
    #define FIFO_SIZE                             6
        #define FIFO_SIZE_512BYTES                (0 << FIFO_SIZE)
        #define FIFO_SIZE_1KB                     (1 << FIFO_SIZE)
        #define FIFO_SIZE_2KB                     (2 << FIFO_SIZE)
        #define FIFO_SIZE_4KB                     (3 << FIFO_SIZE)

#define MPU6500_REG_LP_ACCEL_ODR                  0x1E
    //  Newly added register in MPU6500 for low power output data rate
    //  in cycle mode
    #define MPU6500_LP_ACCEL_ODR_FREQ_0_24HZ         0
    #define MPU6500_LP_ACCEL_ODR_FREQ_0_49HZ         1
    #define MPU6500_LP_ACCEL_ODR_FREQ_0_98HZ         2
    #define MPU6500_LP_ACCEL_ODR_FREQ_1_95HZ         3
    #define MPU6500_LP_ACCEL_ODR_FREQ_3_91HZ         4
    #define MPU6500_LP_ACCEL_ODR_FREQ_7_81HZ         5
    #define MPU6500_LP_ACCEL_ODR_FREQ_15_63HZ        6
    #define MPU6500_LP_ACCEL_ODR_FREQ_31_25HZ        7
    #define MPU6500_LP_ACCEL_ODR_FREQ_62_50HZ        8
    #define MPU6500_LP_ACCEL_ODR_FREQ_125HZ          9
    #define MPU6500_LP_ACCEL_ODR_FREQ_250HZ          10
    #define MPU6500_LP_ACCEL_ODR_FREQ_5001HZ         11

    // Sets the frequency of waking up the chip to take a sample of accel data
    // - the low power accel Output Data Rate.
    #define LPOSC_CLKSEL                             0
        #define LPOSC_CLKSEL_0_24HZ                  (0 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_0_49HZ                  (1 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_0_98HZ                  (2 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_1_95HZ                  (3 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_3_91HZ                  (4 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_7_81HZ                  (5 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_15_63HZ                 (6 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_31_25HZ                 (7 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_62_50HZ                 (8 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_125HZ                   (9 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_250HZ                   (10 << LPOSC_CLKSEL)
        #define LPOSC_CLKSEL_500HZ                   (11 << LPOSC_CLKSEL)

#define MPU6050_REG_MOT_THR                      0x1F
    // Configures the detection threshold for Motion interrupt generation. The
    // unit of MOT_THR is 1 LSB = 1 mg.
    #define MOT_THR                              0
  //  SAME REG -called WOM threshold IN MPU6500 - 1 LSB = 32mg

#define MPU6500_REG_MOT_THR                      0x1F
    // Configures the detection threshold for Motion interrupt generation for MPU6500.
    //  SAME REG as 6050 -called WOM threshold IN MPU6500 - 1 LSB = 4mg
    //#define MOT_THR                              0




#define MPU6050_REG_MOT_DUR                      0x20
    // Configures the duration counter threshold for Motion interrupt
    // generation. The duration counter ticks at 1 kHz, therefore MOT_DUR has a
    // unit of 1 LSB = 1 ms.
    #define MOT_DUR                              0

#define MPU6515_REG_INT_ENABLE                   0x38
#define MPU6500_REG_INT_ENABLE                   0x20
    // MPU6050 compatible mode only!
    #define ACCEL_INTEL_DISABLE                  0
    #define ACCEL_INTEL_ENABLE                   1

#define MPU6500_REG_FIFO_EN                      0x23
    // Enables the I2C slave0 to write to the FIFO.
    #define FIFO_SLV0                            0
        #define SLV0_ENABLE                      (1 << SLV0)
        #define SLV0_DISABLE                     (0 << SLV0)
    // Enables the I2C slave1 to write to the FIFO.
    #define FIFO_SLV1                            1
        #define SLV1_ENABLE                      (1 << SLV1)
        #define SLV1_DISABLE                     (0 << SLV1)
    // Enables the I2C slave2 to write to the FIFO.
    #define FIFO_SLV2                            2
        #define SLV2_ENABLE                      (1 << SLV2)
        #define SLV2_DISABLE                     (0 << SLV2)
    // Enables all 3 axes of the ACCEL sensor to write to the FIFO.
    #define FIFO_ACCEL                           3
        #define ACCEL_ENABLE                     (1 << ACCEL)
        #define ACCEL_DISABLE                    (0 << ACCEL)
    // Enables the Z-axis of the GYRO sensor to write to the FIFO.
    #define FIFO_GYRO_ZOUT                       4
        #define GYRO_ZOUT_ENABLE                 (1 << GYRO_ZOUT)
        #define GYRO_ZOUT_DISABLE                (0 << GYRO_ZOUT)
    // Enables the Y-axis of the GYRO sensor to write to the FIFO.
    #define FIFO_GYRO_YOUT                       5
        #define GYRO_YOUT_ENABLE                 (1 << GYRO_YOUT)
        #define GYRO_YOUT_DISABLE                (0 << GYRO_YOUT)
    // Enables the X-axis of the GYRO sensor to write to the FIFO.
    #define FIFO_GYRO_XOUT                       6
        #define GYRO_XOUT_ENABLE                 (1 << GYRO_XOUT)
        #define GYRO_XOUT_DISABLE                (0 << GYRO_XOUT)
    // Enables the TEMP sensor to write to the FIFO.
    #define TEMP_FIFO_EN                         7
        #define TEMP_FIFO_EN_ENABLE              (1 << TEMP_FIFO_EN)
        #define TEMP_FIFO_EN_DISABLE             (0 << TEMP_FIFO_EN)

#define MPU6515_REG_I2C_MST_CTRL                0x24
#define MPU6515_BIT_WAIT_FOR_ES			            (1 << 6)
#define MPU6515_I2C_MST_CLK_400                 0x0D

// common bitmasks
#define MPU6515_BIT_I2C_SLV_READ        (1 << 7)         

#define MPU6515_BIT_I2C_SLV_EN          (1 << 7)

#define MPU6515_REG_I2C_SLV0_ADDR       0x25
#define MPU6515_REG_I2C_SLV0_REG        0x26
#define MPU6515_REG_I2C_SLV0_CTRL       0x27
#define MPU6515_REG_I2C_SLV0_DO         0x63

#define MPU6515_REG_I2C_SLV1_ADDR       0x28
#define MPU6515_REG_I2C_SLV1_REG        0x29
#define MPU6515_REG_I2C_SLV1_CTRL       0x2A
#define MPU6515_REG_I2C_SLV1_DO         0x64

#define MPU6515_REG_I2C_SLV2_ADDR       0x2B
#define MPU6515_REG_I2C_SLV2_REG        0x2C
#define MPU6515_REG_I2C_SLV2_CTRL       0x2D
#define MPU6515_REG_I2C_SLV2_DO         0x65

#define MPU6515_REG_I2C_SLV3_ADDR       0x2E
#define MPU6515_REG_I2C_SLV3_REG        0x2F
#define MPU6515_REG_I2C_SLV3_CTRL       0x30
#define MPU6515_REG_I2C_SLV3_DO         0x66

#define MPU6515_REG_I2C_SLV4_CTRL       0x34


#define MPU6050_REG_INT_PIN_CFG                  0x37
    #define CLKOUT_EN                            0
    #define I2C_BYPASS_EN                        1
        #define I2C_BYPASS_EN_BIT                (1 << I2C_BYPASS_EN)
    #define FSYNC_INT_EN                         2
    #define FSYNC_INT_LEVEL                      3
    // Sets when the interrupt status bit is cleared
    #define INT_RD_CLEAR                         4
        #define INT_RD_CLEAR_ON_STATUS_REG       (0 << INT_RD_CLEAR)
        #define INT_RD_CLEAR_ON_ANY_REG          (1 << INT_RD_CLEAR)
    // Enables interrupt line latching.
    #define LATCH_INT_EN                         5
        #define LATCH_INT_EN_PULSE               (0 << LATCH_INT_EN)
        #define LATCH_INT_EN_LATCH               (1 << LATCH_INT_EN)
    // Sets the interrupt type.
    #define INT_OPEN                             6
        #define INT_OPEN_PUSH_PULL               (0 << INT_OPEN)
        #define INT_OPEN_DRAIN                   (1 << INT_OPEN)
    // Sets the interrupt line level trigger.
    #define INT_LEVEL                            7
        #define INT_LEVEL_HIGH                   (0 << INT_LEVEL)
        #define INT_LEVEL_LOW                    (1 << INT_LEVEL)

#define MPU6050_REG_INT_ENABLE                   0x38
    // Enables the Data Ready interrupt, which occurs each time a write
    // operation to all of the sensor registers has been completed.
    #define DATA_RDY_EN                          0
        #define DATA_RDY_EN_ENABLE               (1 << DATA_RDY_EN)
        #define DATA_RDY_EN_DISABLE              (0 << DATA_RDY_EN)
    // Enables the DMP Data Ready interrupt, which occurs each time the
    // the DMP writes a packet to the FIFO buffer.
    #define DMP_DATA_RDY_EN                      1
        #define DMP_DATA_RDY_EN_ENABLE           (1 << DMP_DATA_RDY_INT)
        #define DMP_DATA_RDY_EN_DISABLE          (0 << DMP_DATA_RDY_INT)
    // Enables the PLL to generate an interrupt when ready.
    #define PLL_RDY_EN                           2
        #define PLL_RDY_EN_ENABLE                (1 << PLL_RDY_EN)
        #define PLL_RDY_EN_DISABLE               (0 << PLL_RDY_EN)
    // Enables any of the I2C Master interrupt sources to generate an interrupt.
    #define I2C_MST_INT_EN                       3
        #define I2C_MST_INT_EN_ENABLE            (1 << I2C_MST_INT_EN)
        #define I2C_MST_INT_EN_DISABLE           (0 << I2C_MST_INT_EN)
    // Enables a FIFO buffer overflow to generate an interrupt.
    #define FIFO_OFLOW_EN                        4
        #define FIFO_OFLOW_EN_ENABLE             (1 << FIFO_OFLOW_EN)
        #define FIFO_OFLOW_EN_DISABLE            (0 << FIFO_OFLOW_EN)
    // Enables Zero Motion detection to generate an interrupt.
    #define ZMOT_EN                              5
        #define ZMOT_EN_ENABLE                   (1 << ZMOT_EN)
        #define ZMOT_EN_DISABLE                  (0 << ZMOT_EN)
    // Enables Motion detection to generate an interrupt.
    #define MOT_EN                               6
        #define MOT_EN_ENABLE                    (1 << MOT_EN)
        #define MOT_EN_DISABLE                   (0 << MOT_EN)
    // Enables Free Fall detection to generate an interrupt.
    #define FF_EN                                7
        #define FF_EN_ENABLE                     (1 << FF_EN)
        #define FF_EN_DISABLE                    (0 << FF_EN)

#define MPU6500_REG_DMP_INT_STATUS               0x39
    #define DMP_INT_FIFO                         1 << 0
    #define DMP_INT_DSPY_ORI                     1 << 1
    #define DMP_INT_SMD                          1 << 2
    #define DMP_INT_STEP_DETECT                  1 << 3

#define MPU6050_REG_INT_STATUS                   0x3A
    // Indicates if the Data Ready interrupt has been generated.
    #define DATA_RDY_INT                         0
        #define DATA_RDY_INT_ENABLED             (1 << DATA_RDY_INT)
        #define DATA_RDY_INT_DISABLED            (0 << DATA_RDY_INT)
    // Indicates if the DMP Data Ready interrupt has been generated.
    #define DMP_DATA_RDY_INT                     1
        #define DMP_DATA_RDY_INT_ENABLED         (1 << DMP_DATA_RDY_INT)
        #define DMP_DATA_RDY_INT_DISABLED        (0 << DMP_DATA_RDY_INT)
    // Indicates if the PLL Ready interrupt has been generated.
    #define PLL_RDY_INT                          2
        #define PLL_RDY_INT_ENABLED              (1 << PLL_RDY_INT)
        #define PLL_RDY_INT_DISABLED             (0 << PLL_RDY_INT)
    // Indicates if the I2C Master interrupt has been generated.
    #define I2C_MST_INT_INT                      3
        #define I2C_MST_INT_INT_ENABLED          (1 << I2C_MST_INT_INT)
        #define I2C_MST_INT_INT_DISABLED         (0 << I2C_MST_INT_INT)
    // Indicates if the FIFO buffer overflow interrupt has been generated.
    #define FIFO_OFLOW_INT                       4
        #define FIFO_OFLOW_INT_ENABLED           (1 << FIFO_OFLOW_INT)
        #define FIFO_OFLOW_INT_DISABLED          (0 << FIFO_OFLOW_INT)
    // Indicates if the Zero Motion detection interrupt has been generated.
    #define ZMOT_INT                             5
        #define ZMOT_INT_ENABLED                 (1 << ZMOT_INT)
        #define ZMOT_INT_DISABLED                (0 << ZMOT_INT)
    // Indicates if Enables Motion detection interrupt has been generated.
    #define MOT_INT                              6
        #define MOT_INT_ENABLED                  (1 << MOT_INT)
        #define MOT_INT_DISABLED                 (0 << MOT_INT)
    // Indicates if the Free Fall detection interrupt has been generated.
    #define FF_INT                               7
        #define FF_INT_ENABLED                   (1 << FF_INT)
        #define FF_INT_DISABLED                  (0 << FF_INT)
// Data output registers (High and Low order bytes). 16-bit 2's complement.
#define MPU6050_REG_ACCEL_XOUT_H                 0x3B
#define MPU6050_REG_ACCEL_XOUT_L                 0x3C
#define MPU6050_REG_ACCEL_YOUT_H                 0x3D
#define MPU6050_REG_ACCEL_YOUT_L                 0x3E
#define MPU6050_REG_ACCEL_ZOUT_H                 0x3F
#define MPU6050_REG_ACCEL_ZOUT_L                 0x40
#define MPU6050_REG_TEMP_OUT_H                   0x41
#define MPU6050_REG_TEMP_OUT_L                   0x42
#define MPU6050_REG_GYRO_XOUT_H                  0x43
#define MPU6050_REG_GYRO_XOUT_L                  0x44
#define MPU6050_REG_GYRO_YOUT_H                  0x45
#define MPU6050_REG_GYRO_YOUT_L                  0x46
#define MPU6050_REG_GYRO_ZOUT_H                  0x47
#define MPU6050_REG_GYRO_ZOUT_L                  0x48

// Data output registers for auxiliary I2C devices
#define MPU6515_EXT_SENS_DATA_00                  0x49


#define MPU6515_I2C_MST_DELAY_CTRL                0x67
  #define MPU6515_BIT_I2C_SLV0_DLY_EN             (1 << 0)
  #define MPU6515_BIT_I2C_SLV1_DLY_EN             (1 << 1)
  #define MPU6515_BIT_I2C_SLV2_DLY_EN             (1 << 2)
  #define MPU6515_BIT_I2C_SLV3_DLY_EN             (1 << 3)
  #define MPU6515_BIT_I2C_SLV4_DLY_EN             (1 << 4)



#define MPU6050_REG_SIGNAL_PATH_RESET            0x68
    // Resets the temperature sensor analog and digital signal paths.
    #define TEMP_RESET                           0
        #define TEMP_RESET_ENABLE                (1 << TEMP_RESET)
    // Resets the accelerometer analog and digital signal paths.
    #define ACCEL_RESET                          1
        #define ACCEL_RESET_ENABLE               (1 << ACCEL_RESET)
    // Resets the gyroscope analog and digital signal paths.
    #define GYRO_RESET                           2
        #define GYRO_RESET_ENABLE                (1 << GYRO_RESET)

#define MPU6500_REG_MOT_DETECT_CTR               0x69
    // WOM=Wake-On-Motion
    #define  WOM_ACCEL_INTEL_ENABLE_MPU6500_SAMPLE_MODE        192  //11000000
    #define  WOM_ACCEL_INTEL_ENABLE_MPU6050_SAMPLE_MODE        128  //10000000
    #define  WOM_ACCEL_INTEL_DISABLE                  0

#define MPU6050_REG_USER_CTRL                    0x6A
#define MPU6515_REG_USER_CTRL                    0x6A

#define MPU6500_REG_USER_CTRL                    0x6A
    // Resets the FIFO (Advanced Hardware Features) (this bit auto-clears)
    #define FIFO_RST                             2
        #define FIFO_RST_RESET                   (1 << FIFO_RST)
    // Resets the DMP (Advanced Hardware Features) (this bit auto-clears)
    #define DMP_RST                              3
        #define DMP_RST_RESET                    (1 << DMP_RST)
    #define I2C_MST_EN                           5
        #define BIT_I2C_MST_EN                   (1 << I2C_MST_EN)
    // Allows buffering with the FIFO (Advanced Hardware Features)
    #define FIFO_EN                              6
        #define FIFO_EN_ENABLED                  (1 << FIFO_EN)
        #define FIFO_EN_DISABLED                 (0 << FIFO_EN)
    // Allows processing on the DMP (Advanced Hardware Features)
    #define DMP_EN                               7
        #define DMP_EN_ENABLED                   (1 << DMP_EN)
        #define DMP_EN_DISABLED                  (0 << DMP_EN)

#define MPU6050_REG_PWR_MGMT_1                   0x6B
    // Specifies the clock source of the device.
    #define CLKSEL                               0
        #define CLKSEL_INTERNAL                  (0 << CLKSEL)
        #define CLKSEL_PLL_XGYRO                 (1 << CLKSEL)
        #define CLKSEL_PLL_YGYRO                 (2 << CLKSEL)
        #define CLKSEL_PLL_ZGYRO                 (3 << CLKSEL)
        #define CLKSEL_PLL_32_768KHZ             (4 << CLKSEL)
        #define CLKSEL_PLL_19_2MHZ               (5 << CLKSEL)
        #define CLKSEL_STOP                      (7 << CLKSEL)
    // Disables the temperature sensor.
    #define TEMP_DIS                             3
        #define TEMP_DIS_DISABLE                 (0 << TEMP_DIS)
        #define TEMP_DIS_ENABLE                  (1 << TEMP_DIS)
    // Cycles between sleep mode and waking up to take a single sample of data
    // from active sensors at a rate determined by LP_WAKE_CTRL.
    #define CYCLE                                5
        #define CYCLE_DISABLE                    (0 << CYCLE)
        #define CYCLE_ENABLE                     (1 << CYCLE)
    // Puts the MPU-6050 into sleep mode.
    #define SLEEP                                6
        #define SLEEP_ENABLE                     (1 << SLEEP)
        #define SLEEP_DISABLE                    (0 << SLEEP)
    // Resets all internal registers to their default values. Automatically
    // cleared once the reset is done.
    #define DEVICE_RESET                         7
        #define DEVICE_RESET_ENABLE              (1 << DEVICE_RESET)

#define MPU6050_REG_PWR_MGMT_2                   0x6C
    // Puts the Z axis gyroscope into standby mode.
    #define STBY_ZG                              0
        #define STBY_ZG_ENABLE                   (1 << STBY_ZG)
        #define STBY_ZG_DISABLE                  (0 << STBY_ZG)
    // Puts the Y axis gyroscope into standby mode.
    #define STBY_YG                              1
        #define STBY_YG_ENABLE                   (1 << STBY_YG)
        #define STBY_YG_DISABLE                  (0 << STBY_YG)
    // Puts the X axis gyroscope into standby mode.
    #define STBY_XG                              2
        #define STBY_XG_ENABLE                   (1 << STBY_XG)
        #define STBY_XG_DISABLE                  (0 << STBY_XG)
    // Puts the Z axis accelerometer into standby mode.
    #define STBY_ZA                              3
        #define STBY_ZA_ENABLE                   (1 << STBY_ZA)
        #define STBY_ZA_DISABLE                  (0 << STBY_ZA)
    // Puts the Y axis accelerometer into standby mode.
    #define STBY_YA                              4
        #define STBY_YA_ENABLE                   (1 << STBY_YA)
        #define STBY_YA_DISABLE                  (0 << STBY_YA)
    // Puts the X axis accelerometer into standby mode.
    #define STBY_XA                              5
        #define STBY_XA_ENABLE                   (1 << STBY_XA)
        #define STBY_XA_DISABLE                  (0 << STBY_XA)
    // Configures the wake frequency when the device is in Cycle Mode.
    #define LP_WAKE_CTRL                         6
        #define LP_WAKE_CTRL_FREQ_1_25HZ         (0 << LP_WAKE_CTRL)
        #define LP_WAKE_CTRL_FREQ_5HZ            (1 << LP_WAKE_CTRL)
        #define LP_WAKE_CTRL_FREQ_20HZ           (2 << LP_WAKE_CTRL)
        #define LP_WAKE_CTRL_FREQ_40HZ           (3 << LP_WAKE_CTRL)

#define MPU6XXX_REG_FW_START_ADDR                0x70

#define MPU6XXX_REG_FIFO_COUNTH                  0x72
#define MPU6XXX_REG_FIFO_COUNTL                  0x73
#define MPU6XXX_REG_FIFO_R_W                     0x74

#define MPU6050_REG_WHO_AM_I                     0x75
    // Contains the upper 6 bits of the I2C address of the device (110100b).
    #define WHO_AM_I                             1

#define MPU6500_REG_SELF_TEST_X_GYRO              0x0
#define MPU6500_REG_SELF_TEST_Y_GYRO              0x1
#define MPU6500_REG_SELF_TEST_Z_GYRO              0x2

#define MPU6500_REG_WHO_AM_I                     0x75
    // 8-bit device ID. The default value is 0x70
    #define WHO_AM_I                             1

#define MPU6XXX_REG_WHO_AM_I                     0x75
    // 8-bit device ID. The default value is 0x70
    #define WHO_AM_I                             1

#define MPU6500_REG_XA_OFFSET_H                  0x77
#define MPU6500_REG_XA_OFFSET_L                  0x78
#define MPU6500_REG_YA_OFFSET_H                  0x7A
#define MPU6500_REG_YA_OFFSET_L                  0x7B
#define MPU6500_REG_ZA_OFFSET_H                  0x7D
#define MPU6500_REG_ZA_OFFSET_L                  0x7E

/*------------------------------------------------------------------------------
  Digital Motion Processor (DMP) Registers
------------------------------------------------------------------------------*/

#define MPU6500_REG_DMP_CTRL_1                   0x6D
// Write the HIGH byte of the address to this register
#define MPU6500_REG_DMP_CTRL_2                   0x6E
// Write the LOW byte of the address to this register
#define MPU6500_REG_DMP_CTRL_3                   0x6F
// Write the byte to be written to the addressed register
// e.g. If one wanted to write '{0x4, 0x5, 0x6}' to the hardware registers '0xABC'-'0xABE', one would follow this procedure:
// 1) Write 0xA to MPU6500_REG_DMP_CTRL_1
// 2) Write 0xBC to MPU6500_REG_DMP_CTRL_2
// 3) Write {0x4, 0x5, 0x6} to MPU6500_REG_DMP_CTRL_3

// DMP Firmware Start Value (Start Address)
#define MPU6500_REG_FW_START                     0x70
    #define MPU6500_REG_FW_START_1               0x70
    #define MPU6500_REG_FW_START_2               0x71

/* Orientation and Tap Gesture Registers */
#define MPU6500_REG_DMP_ORIENT_EN                0x6AA
#define DMP_ORIENT_ENABLE                        0xD9

#define MPU6500_REG_DMP_TAP_EN                   2224
#define DMP_TAP_ENABLE                           0xF8

#define MPU6500_REG_DMP_ORIENT_TAP_EN            2742
#define DMP_ORIENT_TAP_ENABLE                    0x20

// Write MPU6500_DMP_ORIENT_ENABLE to MPU6500_REG_DMP_ORIENT_EN_ADDR and write
// MPU6500_DMP_ORIENT_TAP_ENABLE to MPU6500_REG_DMP_ORIENT_TAP_EN_ADDR to enable Android Orientation
// Write MPU6500_DMP_TAP_ENABLE to MPU6500_REG_DMP_TAP_EN_ADDR and write
// MPU6500_DMP_ORIENT_TAP_ENABLE to MPU6500_REG_DMP_ORIENT_TAP_EN_ADDR to enable Tap Gesture
// Writing MPU6500_DMP_TAP_AXES_ENABLE will enable all three axes of the Tap Gesture

#define MPU6500_REG_DMP_TAP_AXES_EN              (256 + 72) //0x148
#define DMP_TAP_ALL_AXES_ENABLE                  0x3F
    #define DMP_TAP_X_AXIS_ENABLE                0x30
    #define DMP_TAP_Y_AXIS_ENABLE                0x0C
    #define DMP_TAP_Z_AXIS_ENABLE                0x03

// Tap Threshold X-Axis
#define MPU6500_REG_DMP_TAP_THR_X_1              (256 + 36) //0x124
#define MPU6500_REG_DMP_TAP_THR_X_2              0x125
#define MPU6500_REG_DMP_TAP_THR_X_3              468 //0x1D4
#define MPU6500_REG_DMP_TAP_THR_X_4              0x1D5

// Tap Threshold Y-Axis
#define MPU6500_REG_DMP_TAP_THR_Y_1              (256 + 40) //0x128
#define MPU6500_REG_DMP_TAP_THR_Y_2              0x129
#define MPU6500_REG_DMP_TAP_THR_Y_3              472 //0x1D8
#define MPU6500_REG_DMP_TAP_THR_Y_4              0x1D9

// Tap Threshold Z-Axis
#define MPU6500_REG_DMP_TAP_THR_Z_1              (256 + 44) //0x12C
#define MPU6500_REG_DMP_TAP_THR_Z_2              0x12D
#define MPU6500_REG_DMP_TAP_THR_Z_3              476 //0x1DC
#define MPU6500_REG_DMP_TAP_THR_Z_4              0x1DD

// Tap Time Threshold
#define MPU6500_REG_DMP_TAP_TIME_THR_1           478 //0x1DE
#define MPU6500_REG_DMP_TAP_TIME_THR_2           479 //0x1DF

// Set Multi-Tap
#define MPU6500_REG_DMP_MULTI_TAP_SET            (256 + 79) //0x14F
  #define DMP_SINGLE_TAP                         0
  #define DMP_DOUBLE_TAP                         1
  #define DMP_TRIPLE_TAP                         2
  #define DMP_QUAD_TAP                           3

// Multi-Tap Time Threshold
#define MPU6500_REG_DMP_MULTI_TAP_THR_1          (256 + 218) //0x1DA
#define MPU6500_REG_DMP_MULTI_TAP_THR_2          0x1DB

// Shake Reject Time Threshold
#define MPU6500_REG_DMP_SHAKE_REJECT_TIME_THR_1             (256 + 90) //0x158
#define MPU6500_REG_DMP_SHAKE_REJECT_TIME_THR_2             0x159

// Shake Reject Timeout Threshold
#define MPU6500_REG_DMP_SHAKE_REJECT_TIMEOUT_THR_1          (256 + 88) //0x15A
#define MPU6500_REG_DMP_SHAKE_REJECT_TIMEOUT_THR_2          0x15B

// Shake Reject Threshold
#define MPU6500_REG_DMP_SHAKE_REJECT_THR_1                  (256 + 92) //0x15C
#define MPU6500_REG_DMP_SHAKE_REJECT_THR_2                  0x15D
#define MPU6500_REG_DMP_SHAKE_REJECT_THR_3                  0x15E
#define MPU6500_REG_DMP_SHAKE_REJECT_THR_4                  0x15F

/* Pedometer-Related Registers */
// Pedometer Minimum Step Buffer Threshold
#define MPU6500_REG_DMP_PEDO_MIN_STEP_BUFFER_THR_1          0x328
#define MPU6500_REG_DMP_PEDO_MIN_STEP_BUFFER_THR_2          0x329

// Pedometer Minimum Step Time
#define MPU6500_REG_DMP_PEDO_MIN_STEP_TIME_1                0x32A
#define MPU6500_REG_DMP_PEDO_MIN_STEP_TIME_2                0x32B

// Pedometer Maximum Step Buffer Time
#define MPU6500_REG_DMP_PEDO_MAX_STEP_BUFFER_TIME_1         0x32C
#define MPU6500_REG_DMP_PEDO_MAX_STEP_BUFFER_TIME_2         0x32D

// Pedometer Minimum Step Buffer Threshold
#define MPU6500_REG_DMP_PEDO_MAX_STEP_TIME_1                0x32E
#define MPU6500_REG_DMP_PEDO_MAX_STEP_TIME_2                0x32F

// Pedometer Step Count
#define MPU6500_REG_DMP_PEDO_STEP_COUNT_1                   0x360
#define MPU6500_REG_DMP_PEDO_STEP_COUNT_2                   0x361
#define MPU6500_REG_DMP_PEDO_STEP_COUNT_3                   0x362
#define MPU6500_REG_DMP_PEDO_STEP_COUNT_4                   0x363

// Pedometer Peak Threshold
#define MPU6500_REG_DMP_PEDO_PEAK_THR_1                     0x398
#define MPU6500_REG_DMP_PEDO_PEAK_THR_2                     0x399
#define MPU6500_REG_DMP_PEDO_PEAK_THR_3                     0x39A
#define MPU6500_REG_DMP_PEDO_PEAK_THR_4                     0x39B

// Pedometer Walk Time
#define MPU6500_REG_DMP_PEDO_WALK_TIME_1                    0x3C4
#define MPU6500_REG_DMP_PEDO_WALK_TIME_2                    0x3C5
#define MPU6500_REG_DMP_PEDO_WALK_TIME_3                    0x3C6
#define MPU6500_REG_DMP_PEDO_WALK_TIME_4                    0x3C7

/* Mounting Matrix Registers */
// Gyro Mounting Matrix Configuration
#define MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG_1          0x426
#define MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG_2          0x427
#define MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG_3          0x428

// Accel Mounting Matrix Configuration
#define MPU6500_REG_DMP_ACCEL_MOUNT_MATRIX_CONFIG_1         0x42A
#define MPU6500_REG_DMP_ACCEL_MOUNT_MATRIX_CONFIG_2         0x42B
#define MPU6500_REG_DMP_ACCEL_MOUNT_MATRIX_CONFIG_3         0x42C

// Accel Mounting Matrix Sign Configuration
#define MPU6500_REG_DMP_ACCEL_MOUNT_MATRIX_CONFIG_SIGN_1    0x434
#define MPU6500_REG_DMP_ACCEL_MOUNT_MATRIX_CONFIG_SIGN_2    0x435
#define MPU6500_REG_DMP_ACCEL_MOUNT_MATRIX_CONFIG_SIGN_3    0x436

// Gyro Mounting Matrix Sign Configuration
#define MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG_SIGN_1     0x456
#define MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG_SIGN_2     0x457
#define MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG_SIGN_3     0x458


// 3-Axis Low Power Quaternion Enable
#define MPU6500_REG_DMP_3A_LPQ_EN_1                         0xA9D
  #define DMP_3A_LPQ_ENABLE_1                             0xC0
#define MPU6500_REG_DMP_3A_LPQ_EN_2                         0xA9E
  #define DMP_3A_LPQ_ENABLE_2                             0xC2
#define MPU6500_REG_DMP_3A_LPQ_EN_3                         0xA9F
  #define DMP_3A_LPQ_ENABLE_3                             0xC4
#define MPU6500_REG_DMP_3A_LPQ_EN_4                         0xAA0
  #define DMP_3A_LPQ_ENABLE_4                             0xC6

// 6-Axis Low Power Quaternion Enable
#define MPU6500_REG_DMP_6A_LPQ_EN_1                         0xAA3
  #define DMP_6A_LPQ_ENABLE_1                             0x20
#define MPU6500_REG_DMP_6A_LPQ_EN_2                         0xAA4
  #define DMP_6A_LPQ_ENABLE_2                             0x28
#define MPU6500_REG_DMP_6A_LPQ_EN_3                         0xAA5
  #define DMP_6A_LPQ_ENABLE_3                             0x30
#define MPU6500_REG_DMP_6A_LPQ_EN_4                         0xAA6
  #define DMP_6A_LPQ_ENABLE_4                             0x30

// Raw Data Enable
#define MPU6500_REG_DMP_RAW_DATA_EN_1                       0xAAB
  #define DMP_RAW_DATA_ENABLE_1                           0xA3
#define MPU6500_REG_DMP_RAW_DATA_EN_2                       0xAAC
  #define DMP_RAW_DATA_ENABLE_2                           0xC0
#define MPU6500_REG_DMP_RAW_DATA_EN_3                       0xAAD
  #define DMP_RAW_DATA_ENABLE_3                           0xC8
#define MPU6500_REG_DMP_RAW_DATA_EN_4                       0xAAE
  #define DMP_RAW_DATA_ENABLE_4                           0xC2
#define MPU6500_REG_DMP_RAW_DATA_EN_5                       0xAAF
  #define DMP_RAW_DATA_ENABLE_5                           0xC4
#define MPU6500_REG_DMP_RAW_DATA_EN_6                       0xAB0
  #define DMP_RAW_DATA_ENABLE_6                           0xCC
#define MPU6500_REG_DMP_RAW_DATA_EN_7                       0xAB1
  #define DMP_RAW_DATA_ENABLE_7                           0xC6
#define MPU6500_REG_DMP_RAW_DATA_EN_8                       0xAB2
  #define DMP_RAW_DATA_ENABLE_8                           0xA3
#define MPU6500_REG_DMP_RAW_DATA_EN_9                       0xAB3
  #define DMP_RAW_DATA_ENABLE_9                           0xA3
#define MPU6500_REG_DMP_RAW_DATA_EN_10                      0xAB4
  #define DMP_RAW_DATA_ENABLE_10                          0xA3

// FIFO Rate Divider
#define MPU6500_REG_DMP_FIFO_RATE_DIV_H                     0x216
#define MPU6500_REG_DMP_FIFO_RATE_DIV_L                     0x217

// FIFO Rate Divider Enable
// 0xFE,0xF2,0xAB,0xC4,0xAA,0xF1,0xDF,0xDF,0xBB,0xAF,0xDF,0xDF
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_1                  0xAC1
  #define DMP_FIFO_RATE_DIV_ENABLE_1                      0xFE
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_2                  0xAC2
  #define DMP_FIFO_RATE_DIV_ENABLE_2                      0xF2
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_3                  0xAC3
  #define DMP_FIFO_RATE_DIV_ENABLE_3                      0xAB
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_4                  0xAC4
  #define DMP_FIFO_RATE_DIV_ENABLE_4                      0xC4
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_5                  0xAC5
  #define DMP_FIFO_RATE_DIV_ENABLE_5                      0xAA
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_6                  0xAC6
  #define DMP_FIFO_RATE_DIV_ENABLE_6                      0xF1
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_7                  0xAC7
  #define DMP_FIFO_RATE_DIV_ENABLE_7                      0xDF
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_8                  0xAC8
  #define DMP_FIFO_RATE_DIV_ENABLE_8                      0xDF
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_9                  0xAC9
  #define DMP_FIFO_RATE_DIV_ENABLE_9                      0xBB
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_10                 0xACA
  #define DMP_FIFO_RATE_DIV_ENABLE_10                     0xAF
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_11                 0xACB
  #define DMP_FIFO_RATE_DIV_ENABLE_11                     0xDF
#define MPU6500_REG_DMP_FIFO_RATE_DIV_EN_12                 0xACC
  #define DMP_FIFO_RATE_DIV_ENABLE_12                     0xDF

/*
 * DMP Current consumption, version 0.6 (current values in uA)
 *
 * Note: Current consumptions of DMP outputs are not additive.
 *       In general,
 *         Total Current(uA) = max { of all DMP outputs, uA }
 */
#define MPU6515_ACTIVE_CURRENT_ACCEL_4k_ODR    450
#define MPU6515_ACTIVE_CURRENT_DMP             100
#define MPU6515_ACTIVE_CURRENT_DMP_ACCEL       400
#define MPU6515_ACTIVE_CURRENT_DMP_STEP_EVENT  555
#define MPU6515_ACTIVE_CURRENT_DMP_STEP_COUNT  555
#define MPU6515_ACTIVE_CURRENT_DMP_SMD         555
#define MPU6515_ACTIVE_CURRENT_DMP_GYRO        3200
#define MPU6515_ACTIVE_CURRENT_DMP_ACCEL_GYRO  4000
#define MPU6515_ACTIVE_CURRENT_DMP_GAME_RV     4000

#define MPU6515_LOWPOWER_CURRENT               6


/*------------------------------------------------------------------------------
  Digital Motion Processor (DMP) Output Masks
------------------------------------------------------------------------------*/
#define TAP_EVENT                                           0
  #define TAP_EVENT_MASK                                  (0x01 << TAP_EVENT)
#define TAP_NUMBER                                          0
  #define TAP_NUMBER_MASK                                 (0x07 << TAP_NUMBER)
#define TAP_DIRECTION                                       3
  #define TAP_DIRECTION_MASK                              (0x07 << TAP_DIRECTION)

/*------------------------------------------------------------------------------
  Digital Motion Processor (DMP) Default Settings
------------------------------------------------------------------------------*/
#define MPU6500_DEFAULT_SHAKE_REJECT_THR                    200  // 200 dps
#define MPU6500_DEFAULT_SHAKE_REJECT_TIMEOUT_THR            10   // 10 ms
#define MPU6500_DEFAULT_SHAKE_REJECT_TIME_THR               40   // 40 ms
#define MPU6500_DEFAULT_MULTI_TAP_THR                       500  // 500 ms
#define MPU6500_DEFAULT_TAP_TIME_THR                        100  // 100 ms
#define MPU6500_DEFAULT_TAP_THR_X_1                         8192  // 0.1 G/ms
#define MPU6500_DEFAULT_TAP_THR_X_2                         6144  // 0.1 G/ms
#define MPU6500_DEFAULT_TAP_THR_Y_1                         8192  // 0.1 G/ms
#define MPU6500_DEFAULT_TAP_THR_Y_2                         6144  // 0.1 G/ms
#define MPU6500_DEFAULT_TAP_THR_Z_1                         8192  // 0.1 G/ms
#define MPU6500_DEFAULT_TAP_THR_Z_2                         6144  // 0.1 G/ms
#define MPU6500_DEFAULT_MULTI_TAP_NUM                       2    // Double-Tap

/*------------------------------------------------------------------------------
  Digital Motion Processor (DMP) Settings UPPER Limits (Inclusive)
------------------------------------------------------------------------------*/
#define MPU6500_UPPER_LIMIT_SHAKE_REJECT_THR                2000    // 2000 dps
#define MPU6500_UPPER_LIMIT_SHAKE_REJECT_TIMEOUT_THR        0xFFFF  // 65536 ms
#define MPU6500_UPPER_LIMIT_SHAKE_REJECT_TIME_THR           0xFFFF  // 65536 ms
#define MPU6500_UPPER_LIMIT_MULTI_TAP_THR                   0xFFFF  // 65536 ms
#define MPU6500_UPPER_LIMIT_TAP_TIME_THR                    0xFFFF  // 65536 ms
//#define MPU6500_UPPER_LIMIT_TAP_THR_X                       799 //800     // 0.8 G/ms
//#define MPU6500_UPPER_LIMIT_TAP_THR_Y                       799 //800     // 0.8 G/ms
//#define MPU6500_UPPER_LIMIT_TAP_THR_Z                       799 //800     // 0.8 G/ms
#define MPU6500_UPPER_LIMIT_MULTI_TAP_NUM                   4       // Quadruple-Tap

/*------------------------------------------------------------------------------
  Digital Motion Processor (DMP) Settings LOWER Limits (Inclusive)
------------------------------------------------------------------------------*/
#define MPU6500_LOWER_LIMIT_SHAKE_REJECT_THR                0       // 0 dps
#define MPU6500_LOWER_LIMIT_SHAKE_REJECT_TIMEOUT_THR        0       // 0 ms
#define MPU6500_LOWER_LIMIT_SHAKE_REJECT_TIME_THR           0       // 0 ms
#define MPU6500_LOWER_LIMIT_MULTI_TAP_THR                   0       // 0 ms
#define MPU6500_LOWER_LIMIT_TAP_TIME_THR                    0       // 0 ms
//#define MPU6500_LOWER_LIMIT_TAP_THR_X                       0       // 0 G/ms
//#define MPU6500_LOWER_LIMIT_TAP_THR_Y                       0       // 0 G/ms
//#define MPU6500_LOWER_LIMIT_TAP_THR_Z                       0       // 0 G/ms
#define MPU6500_LOWER_LIMIT_MULTI_TAP_NUM                   1       // Single-Tap

/*

*/
#define DMP_HDR_INVALID       0xFFFF
#define DMP_MAX_PKT_SIZE      16      // [bytes]
#define DMP_MIN_HDR_SIZE      2       // [bytes]
#define DMP_MIN_BODY_SIZE     6       // [bytes]
#define DMP_FIFO_SIZE         1024    // [bytes]
//in bytes
#define MAX_DMP_FIFO_SIZE_2_USE  900 
#define DMP_MAX_BATCH_THRES   MAX_DMP_FIFO_SIZE_2_USE  // [bytes]
#define DMP_MAX_BURST_SIZE    250     // [bytes]
#define DMP_MIN_NUM_BYTES     MIN(DMP_MIN_HDR_SIZE,DMP_MIN_BODY_SIZE)
#define DMP_MAX_NUM_SAMPLES   128     // DMP_FIFO_SIZE/8
#define DMP_U16_MAX           0xFFFF
#define DMP_U32_MAX           0xFFFFFFFF
#define DMP_PI                3.1415928
#define DMP_QUAT_SCALE        30

#define DMP_SENSOR_INVALID   0
#define DMP_SENSOR_3QUAT     1
#define DMP_SENSOR_6QUAT     2
#define DMP_SENSOR_PQUAT     3      // with Step Enable/Disable
#define DMP_SENSOR_RAW_ACCEL 4
#define DMP_SENSOR_RAW_GYRO  5
#define DMP_SENSOR_STEPDET   6
#define DMP_SENSOR_COMPASS   7
#define DMP_SENSOR_PRESSURE  8
#define DMP_SENSOR_SMD       9      // SMD = special case; beware
#define DMP_SENSOR_DOUBLETAP 10
#define DMP_SENSOR_STEPCNT   11
#define DMP_NO_RESET_WITHIN_HALF_SECOND_IN_TICK              16384


/*------------------------------------------------------------------------------
  Useful shorthands for register values
------------------------------------------------------------------------------*/

#define MPU6050_ACCEL_STBY_DISABLE  (STBY_ZA_DISABLE | STBY_YA_DISABLE | STBY_XA_DISABLE)
#define MPU6050_ACCEL_STBY_ENABLE   (STBY_ZA_ENABLE | STBY_YA_ENABLE | STBY_XA_ENABLE)

#define MPU6050_GYRO_STBY_DISABLE   (STBY_ZG_DISABLE | STBY_YG_DISABLE | STBY_XG_DISABLE)
#define MPU6050_GYRO_STBY_ENABLE    (STBY_ZG_ENABLE | STBY_YG_ENABLE | STBY_XG_ENABLE)

#define MPU6050_ACCEL_ST_DISABLE (ZA_ST_DISABLE | YA_ST_DISABLE | XA_ST_DISABLE)
#define MPU6050_ACCEL_ST_ENABLE  (ZA_ST_ENABLE  | YA_ST_ENABLE  | XA_ST_ENABLE)

#define MPU6050_GYRO_ST_DISABLE  (ZG_ST_DISABLE | YG_ST_DISABLE | XG_ST_DISABLE)
#define MPU6050_GYRO_ST_ENABLE   (ZG_ST_ENABLE  | YG_ST_ENABLE  | XG_ST_ENABLE)

/*------------------------------------------------------------------------------
  Utility macros
------------------------------------------------------------------------------*/

#define MPU6515_IS_AUX_DEV(s)    ( ((s)==SNS_DDF_SENSOR_MAG) || ((s)==SNS_DDF_SENSOR_PRESSURE) )

// Convert Degrees/sec to Radians/sec, Q16 format.
#define MPU6050_DPS_TO_RPSQ16(x)      FX_FLTTOFIX_Q16((x) * PI / 180.0)

// Returns the size of a static table (i.e., array).
#define MPU6050_TABLE_SIZE(a)         (sizeof(a) / sizeof(*a))

// Number of axes of inertial sensors
#define MPU6XXX_NUM_INERTIAL_AXES     3

// Axis index
#define X 0
#define Y 1
#define Z 2

// Number of MPU-6050 sensors supported by this driver (accel, gyro, and temp)
#define MPU6050_NUM_SENSORS           3
#define MPU6050_MAX_SUB_DEV           2

// Number of MPU-6500 sensors supported by this driver (accel, gyro, temp, and double-tap)
#define MPU6500_NUM_SENSORS           4
#define MPU6500_MAX_SUB_DEV           3

#define MPU6515_NUM_SENSORS           8 
#define MPU6515_MAX_SUB_DEV           8

// Byte alignment to be used when allocating memory for the DD state structure (must be a power of 2!)
#define MPU6XXX_STATE_BYTE_ALIGNMENT  16

// Maximum supported number of sensors
#define MPU6XXX_MAX_NUM_SENSORS       MAX(MPU6515_NUM_SENSORS, (MAX(MPU6050_NUM_SENSORS, MPU6500_NUM_SENSORS)) )

#define MPU6XXX_MAX_SUB_DEV           MPU6XXX_STATE_BYTE_ALIGNMENT

// Maximum supported number of axes for a single sensor type
#define MPU6XXX_MAX_NUM_AXES          MPU6XXX_NUM_INERTIAL_AXES

// Isolate the High byte of a 2-byte address
#define MPU6XXX_HIGH_BYTE(x)          (uint8_t) (((x) & 0xFF00) >> 8)

// Isolate the Low byte of a 2-byte address
#define MPU6XXX_LOW_BYTE(x)           (uint8_t) ((x) & 0xFF)

// Maximum number of bytes that can be burst-written at a time
#define MPU6500_MAX_BURST_WRITE_SIZE  256

// Maximum number of DOUBLETAP outputs (defined by us)
#define MPU6500_DOUBLETAP_MAX_ODR     200
#define MPU6500_DOUBLETAP_SMPLRT      200

#define MPU6050_IDLE_TO_READY_TIME      230000  // approximation (accouting for DMP setup time)
#define MPU6050_START_UP_TIME_US        100000
#define DD_MPU6050_RESET_WAIT_ONCE_US   200
#define DD_MPU6050_RESET_WAIT_LOOP_MAX  (MPU6050_START_UP_TIME_US/DD_MPU6050_RESET_WAIT_ONCE_US)


#define MPU_IS_DMP_SENSOR(ddf_sensor)   ( ((ddf_sensor)==SNS_DDF_SENSOR_ACCEL ||     \
                                           (ddf_sensor)==SNS_DDF_SENSOR_GYRO  ||     \
                                           (ddf_sensor)==SNS_DDF_SENSOR_TEMP) ? false : true )
//#define MPU_IS_DMP_SENSOR(ddf_sensor) true

#define MPU6050_DEBUG
#define MPU6050_DEBUG_MSG

#ifdef MPU6050_DEBUG
#if defined MPU6050_DEBUG_MSG
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define MPU6050_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, msg)
#define MPU6050_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1)
#define MPU6050_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2)
#define MPU6050_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2,p3)
#define MPU6050_MSG_4(level,msg,p1,p2,p3,p4)      MSG_4(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2,p3,p4)
#define MPU6050_MSG_5(level,msg,p1,p2,p3,p4,p5)   MSG_5(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2,p3,p4,p5)
#else
#define MPU6050_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define MPU6050_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define MPU6050_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define MPU6050_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#define MPU6050_MSG_4(level,msg,p1,p2,p3,p4)
#define MPU6050_MSG_5(level,msg,p1,p2,p3,p4,p5)
#define MED MEDIUM
#endif
#else
#define MPU6050_MSG_0(level,msg)
#define MPU6050_MSG_1(level,msg,p1)
#define MPU6050_MSG_2(level,msg,p1,p2)
#define MPU6050_MSG_3(level,msg,p1,p2,p3)
#define MPU6050_MSG_4(level,msg,p1,p2,p3,p4)
#define MPU6050_MSG_5(level,msg,p1,p2,p3,p4,p5)
#endif

/*------------------------------------------------------------------------------
  Definitions shared across the driver's files
------------------------------------------------------------------------------*/
typedef enum
{
  DEVICE_ID_MPU6050 = 0x68, //Who am I register field of MPU6050,
  DEVICE_ID_MPU6500 = 0x70, //who am I register field of MPU6500
  DEVICE_ID_MPU6515 = 0x74,
  DEVICE_ID_INVALID = 0xFF
} mpu6515_device_id_e;

// Defines the order of data types in the driver's data cache. These are used
// as indices and therefore the order is important.
typedef enum
{
    MPU6050_CACHE_ACCELX = 0,
    MPU6050_CACHE_ACCELY,
    MPU6050_CACHE_ACCELZ,
    MPU6050_CACHE_TEMP,
    MPU6050_CACHE_GYROX,
    MPU6050_CACHE_GYROY,
    MPU6050_CACHE_GYROZ,
    MPU6050_CACHE_MAGX,
    MPU6050_CACHE_MAGY,
    MPU6050_CACHE_MAGZ,
    MPU6050_CACHE_STEPCNT,
    MPU6050_CACHE_6QUAT1,
    MPU6050_CACHE_6QUAT2,
    MPU6050_CACHE_6QUAT3,
    MPU6050_CACHE_STEPDET,
    MPU6050_CACHE_SMD,
    MPU6050_CACHE_PRESSURE,
    MPU6050_CACHE__SIZE
} mpu6515_cache_e;


// Interrupts supported by this driver.
typedef enum {
    MPU6050_INT_DATA_READY,
    MPU6050_INT_MOTION_DETECT,
    MPU6500_INT_DMP
} mpu6515_interrupt_t;

typedef enum {
  MPU_REG_SELF_TEST_X_GYRO = 0x00,
  MPU_REG_SELF_TEST_Y_GYRO = 0x01,
  MPU_REG_SELF_TEST_Z_GYRO = 0x02,
  MPU_REG_SELF_TEST_X_ACCEL = 0x0D,
  MPU_REG_SELF_TEST_Y_ACCEL = 0x0E,
  MPU_REG_SELF_TEST_Z_ACCEL = 0x0F,
  MPU_REG_XG_OFFSET_H = 0x13,
  MPU_REG_XG_OFFSET_L = 0x14,
  MPU_REG_YG_OFFSET_H = 0x15,
  MPU_REG_YG_OFFSET_L = 0x16,
  MPU_REG_ZG_OFFSET_H = 0x17,
  MPU_REG_ZG_OFFSET_L = 0x18,
  MPU_REG_SMPLRT_DIV = 0x19,
  MPU_REG_CONFIG = 0x1A,
  MPU_REG_GYRO_CONFIG = 0x1B,
  MPU_REG_ACCEL_CONFIG = 0x1C,
  MPU_REG_ACCEL_CONFIG_2 = 0x1D,
  MPU_REG_LP_ACCEL_ODR = 0x1E,
  MPU_REG_WOM_THR = 0x1F,
  MPU_REG_FIFO_EN = 0x23,
  MPU_REG_I2C_MST_CTRL = 0x24,
  MPU_REG_I2C_SLV0_ADDR = 0x25,
  MPU_REG_I2C_SLV0_REG = 0x26,
  MPU_REG_I2C_SLV0_CTRL = 0x27,
  MPU_REG_I2C_SLV1_ADDR = 0x28,
  MPU_REG_I2C_SLV1_REG = 0x29,
  MPU_REG_I2C_SLV1_CTRL = 0x2A,
  MPU_REG_I2C_SLV2_ADDR = 0x2B,
  MPU_REG_I2C_SLV2_REG = 0x2C,
  MPU_REG_I2C_SLV2_CTRL = 0x2D,
  MPU_REG_I2C_SLV3_ADDR = 0x2E,
  MPU_REG_I2C_SLV3_REG = 0x2F,
  MPU_REG_I2C_SLV3_CTRL = 0x30,
  MPU_REG_I2C_SLV4_ADDR = 0x31,
  MPU_REG_I2C_SLV4_REG = 0x32,
  MPU_REG_I2C_SLV4_DO = 0x33,
  MPU_REG_I2C_SLV4_CTRL = 0x34,
  MPU_REG_I2C_SLV4_DI = 0x35,
  MPU_REG_I2C_MST_STATUS = 0x36,
  MPU_REG_INT_PIN_CFG = 0x37,
  MPU_REG_INT_ENABLE = 0x38,
  MPU_REG_INT_STATUS = 0x3A,
  MPU_REG_ACCEL_XOUT_H = 0x3B,
  MPU_REG_ACCEL_XOUT_L = 0x3C,
  MPU_REG_ACCEL_YOUT_H = 0x3D,
  MPU_REG_ACCEL_YOUT_L = 0x3E,
  MPU_REG_ACCEL_ZOUT_H = 0x3F,
  MPU_REG_ACCEL_ZOUT_L = 0x40,
  MPU_REG_TEMP_OUT_H = 0x41,
  MPU_REG_TEMP_OUT_L = 0x42,
  MPU_REG_GYRO_XOUT_H = 0x43,
  MPU_REG_GYRO_XOUT_L = 0x44,
  MPU_REG_GYRO_YOUT_H = 0x45,
  MPU_REG_GYRO_YOUT_L = 0x46,
  MPU_REG_GYRO_ZOUT_H = 0x47,
  MPU_REG_GYRO_ZOUT_L = 0x48,
  MPU_REG_EXT_SENS_DATA_00 = 0x49,
  MPU_REG_EXT_SENS_DATA_01 = 0x4A,
  MPU_REG_EXT_SENS_DATA_02 = 0x4B,
  MPU_REG_EXT_SENS_DATA_03 = 0x4C,
  MPU_REG_EXT_SENS_DATA_04 = 0x4D,
  MPU_REG_EXT_SENS_DATA_05 = 0x4E,
  MPU_REG_EXT_SENS_DATA_06 = 0x4F,
  MPU_REG_EXT_SENS_DATA_07 = 0x50,
  MPU_REG_EXT_SENS_DATA_08 = 0x51,
  MPU_REG_EXT_SENS_DATA_09 = 0x52,
  MPU_REG_EXT_SENS_DATA_10 = 0x53,
  MPU_REG_EXT_SENS_DATA_11 = 0x54,
  MPU_REG_EXT_SENS_DATA_12 = 0x55,
  MPU_REG_EXT_SENS_DATA_13 = 0x56,
  MPU_REG_EXT_SENS_DATA_14 = 0x57,
  MPU_REG_EXT_SENS_DATA_15 = 0x58,
  MPU_REG_EXT_SENS_DATA_16 = 0x59,
  MPU_REG_EXT_SENS_DATA_17 = 0x5A,
  MPU_REG_EXT_SENS_DATA_18 = 0x5B,
  MPU_REG_EXT_SENS_DATA_19 = 0x5C,
  MPU_REG_EXT_SENS_DATA_20 = 0x5D,
  MPU_REG_EXT_SENS_DATA_21 = 0x5E,
  MPU_REG_EXT_SENS_DATA_22 = 0x5F,
  MPU_REG_EXT_SENS_DATA_23 = 0x60,
  MPU_REG_I2C_SLV0_DO = 0x63,
  MPU_REG_I2C_SLV1_DO = 0x64,
  MPU_REG_I2C_SLV2_DO = 0x65,
  MPU_REG_I2C_SLV3_DO = 0x66,
  MPU_REG_I2C_MST_DELAY_CTRL = 0x67,
  MPU_REG_SIGNAL_PATH_RESET = 0x68,
  MPU_REG_MOT_DETECT_CTRL = 0x69,
  MPU_REG_USER_CTRL = 0x6A,
  MPU_REG_PWR_MGMT_1 = 0x6B,
  MPU_REG_PWR_MGMT_2 = 0x6C,
  MPU_REG_FIFO_COUNTH = 0x72,
  MPU_REG_FIFO_COUNTL = 0x73,
  MPU_REG_FIFO_R_W = 0x74,
  MPU_REG_WHO_AM_I = 0x75,
  MPU_REG_XA_OFFSET_H = 0x77,
  MPU_REG_XA_OFFSET_L = 0x78,
  MPU_REG_YA_OFFSET_H = 0x7A,
  MPU_REG_YA_OFFSET_L = 0x7B,
  MPU_REG_ZA_OFFSET_H = 0x7D,
  MPU_REG_ZA_OFFSET_L = 0x7E,
  MPU_REG_MAP_SIZE
} mpu6515_reg_map_t;

typedef struct
{
  uint32_t               md_dup_warning;
  uint32_t               md_err;
  uint32_t               md_state;
  uint32_t               set_odr_err;
} mpu6515_cnt_s;

typedef struct mpu6515_hw_st_s mpu6515_hw_st_s;

/**
 * This struct is used to represent the head of the singly-linked list that
 * contains all device instances related to this device driver.
 */
typedef struct
{
  sns_ddf_dev_list_t     next;              // The next device instance
  sns_ddf_driver_info_s  dd_info;           // Device driver info
} mpu6515_head_s;

/**
 *
 */
//typedef sns_dd_bmp280_state_t sns_dd_alt_bmp280_state_s;

/**
   This enumeration is used to specify which device is
   encompassed within the mpu6515_aux_dev_t structure
 */
typedef enum {
  AKM8963_AUX_DEV,
  BMP280_AUX_DEV
} mpu6515_aux_dev_type_e;

typedef struct {
  bool valid;
  uint32_t i2c_slave_addr;
  sns_ddf_handle_t st_aux_dev;  // pointer to the state structure of auxilarry device.
 } mpu6515_aux_dev_t;

/**
 * This struct represents a single sensor type that is contained within a combo
 * device. (e.g. ACCEL, GYRO, TEMP, DOUBLETAP, etc.)
 */
typedef struct
{
  sns_ddf_sensor_e  sensor_type;  // Sensor ID based off of the sns_ddf_sensor_e enum
  uint32_t          desired_odr;  // The desired ODR (0, if there is no output requested for this sensor type)
  uint32_t          odr;          // The current ODR for this sensor
  bool              int_enabled;  // True if DRI is enabled for this particular sensor (not supported for doubletap)
  uint8_t           range;        // The full-scale range index. (only supported for gyro and accel)
  uint8_t           num_axes;     // Number of axes for this device. Also the size of @biases.
  q16_t             biases[MPU6XXX_MAX_NUM_AXES];   // Currently biases are only supported for accel and gyro
  q16_t             cur_bandwidth; /* is this q16?. This is only valid for accel and gyro, or temp(?) */
  sns_ddf_time_t    last_sample_timestamp; // sns_ddf_time of the last sample;
  mpu6515_aux_dev_t aux_dev;
  sns_ddf_axes_map_s  axes_map;
  uint16_t          watermark;
} mpu6515_sensor_s;

/**
 * This struct represents a single sub-device that is contained within a combo
 * device. (e.g. ACCEL/TEMP, GYRO/TEMP, DOUBLETAP, etc.)
 */
typedef struct
{
  sns_ddf_handle_t      smgr_handle;  // SMGR handle used to notify async events from this sub-device
  sns_ddf_powerstate_e  powerstate;   // Sub-device power state (this also dictates the power-states of all sensors within)
  uint32_t              int_gpio;     // if sub_device is configured as DRI mode. otherwise, it is 0xffff
  uint32_t              num_sensors;
  mpu6515_sensor_s      sensors[MPU6XXX_MAX_NUM_SENSORS];
} mpu6515_sub_dev_s;

// An MPU-6xxx driver instance.
typedef struct
{
  sns_ddf_dev_list_t    next;              // Pointer to next device instance in the device chain
  sns_ddf_port_id_s     port_id;

  sns_ddf_handle_t      port_handle;       // Handle used to access the I2C bus

  uint8_t               sub_device_cnt;
  mpu6515_sub_dev_s     sub_devices[MPU6515_MAX_SUB_DEV]; // All sub-devices belonging to this device instance

  uint8_t               lpf_index;         // Current lowpass filter config
  sns_ddf_time_t        data_cache_time;   // Last time data cache was updated
  q16_t                 data_cache[MPU6050_CACHE__SIZE];
  sns_ddf_status_e      data_status[MPU6050_CACHE__SIZE];
  sns_ddf_sensor_data_s sensor_data;      /* to use when notifying the data to SMGR */
  bool                  int_registered;
  bool                  md_enabled;
  uint32_t              md_accel_odr;
  bool                  cycle_mode;

  mpu6515_device_id_e   device_id;    // to identify the device within the family

  mpu6515_cnt_s         cnt;               /* counts for history */
  mpu6515_hw_st_s*      hw_st_ptr;

  sns_ddf_time_t last_batch_timestamp;
  sns_ddf_time_t last_reset_time;

  uint8_t mpu_reg[MPU_REG_MAP_SIZE];
  uint16_t       current_thres;

  sns_ddf_fifo_set_s    dd_fifo_config;
} mpu6515_s;

// DMP FIFO Rate Divider Enable Op-codes
static const uint8_t mpu6515_fifo_rate_div_en[] =
{0xFE,0xF2,0xAB,0xC4,0xAA,0xF1,0xDF,0xDF,0xBB,0xAF,0xDF,0xDF};

/*------------------------------------------------------------------------------
  DMP Firmware.
------------------------------------------------------------------------------*/
/*
 * Register addresses associated specifically in the context of DMP only.
 *
 * Rev 0.6
 */
#define CFG_OUT_3QUAT     (2382)
#define CFG_OUT_6QUAT    (2417)
#define CFG_OUT_PQUAT  (2452)
#define CFG_OUT_ACCL   (2485)
#define CFG_OUT_GYRO  (2519)
#define CFG_OUT_CPASS  (2552)
#define CFG_OUT_PRESS     (2586)
#define CFG_OUT_STEPDET  (2352)
#define CFG_PED_ENABLE   (2684)
#define CFG_PEDSTEP_DET  (2934)
#define D_PEDSTD_STEPCTR  (864)
#define CFG_DISPLAY_ORIENT_INT (2090)
#define CFG_PED_INT  (2922)
#define CFG_DR_INT   (1053)
#define CFG_FIFO_INT  (2682)
#define CFG_AUTH  (1059)
#define FCFG_1  (1078)
#define FCFG_3  (1103)
#define FCFG_2  (1082)
#define FCFG_7  (1089)
#define FCFG_6  (1121)
#define CFG_EXT_GYRO_BIAS  (1200)
#define CFG_7  (1419)
#define D_SMD_ENABLE  (288)
#define D_SMD_MOT_THLD  (320)
#define D_SMD_DELAY_THLD  (340)
#define D_SMD_DELAY2_THLD  (348)
#define CFG_GYRO_RAW_DATA  (2533)
#define D_BM_BATCH_CNTR  (436)
#define D_BM_BATCH_THLD  (440)
#define D_BM_ENABLE  (454)
#define D_BM_NUMWORD_TOFILL  (452)
#define D_EXT_GYRO_BIAS_X (976)
#define D_EXT_GYRO_BIAS_Y (980)
#define D_EXT_GYRO_BIAS_Z (984)

//   Please follow this key mapping for ODR DMP locations.
//  {KEY_CFG_3QUAT_ODR  ?  D_ODR_S1}
//  {KEY_CFG_6QUAT_ODR  ?  D_ODR_S2},
//  {KEY_CFG_PQUAT_ODR  ?  D_ODR_S3},
//  {KEY_CFG_ACCL_ODR  ?  D_ODR_S4},
//  {KEY_CFG_GYRO_ODR  ?  D_ODR_S5},
//  {KEY_CFG_CPASS_ODR ?  D_ODR_S6},
//  {KEY_CFG_PRESS_ODR   ?  D_ODR_S7},
//  {KEY_ODR_CNTR_3QUAT  ?  D_ODR_CNTR_S1},
//  {KEY_ODR_CNTR_6QUAT ?  D_ODR_CNTR_S2},
//  {KEY_ODR_CNTR_PQUAT ?  D_ODR_CNTR_S3},
//  {KEY_ODR_CNTR_ACCL  ?  D_ODR_CNTR_S4},
//  {KEY_ODR_CNTR_GYRO  ?  D_ODR_CNTR_S5},
//  {KEY_ODR_CNTR_CPASS ?  D_ODR_CNTR_S6},
//  {KEY_ODR_CNTR_PRESS ?  D_ODR_CNTR_S7}
#define D_ODR_S1  (732)
#define D_ODR_S2    (730)
#define D_ODR_S3    (734)
#define D_ODR_S4   (744)
#define D_ODR_S5  (748)
#define D_ODR_S6  (746)
#define D_ODR_S7  (750)
#define D_ODR_CNTR_S1  (724)
#define D_ODR_CNTR_S2  (722)
#define D_ODR_CNTR_S3  (726)
#define D_ODR_CNTR_S4  (736)
#define D_ODR_CNTR_S5  (740)
#define D_ODR_CNTR_S6  (738)
#define D_ODR_CNTR_S7  (742)


/*
 * Rev 0.6
 */
#define SNS_DDF_MAX_ESPIMAGE_SIZE (2969)
typedef PACK(struct) {
  uint8_t image[SNS_DDF_MAX_ESPIMAGE_SIZE];
} esp_t;

#define MPU6515_DMP_ADDR_FIRST_BYTE   32

// The Start Address may be subject to change
#define DMP_START_ADDRESS 0x0400

static const esp_t dmp_image  __attribute__ ((section (".data.espimage"))) = {
  /* 0x0BF6, */ /* 0x400, */ {
    /* bank # 0 */
    0x20, 0x00, 0x30, 0xc3, 0x0e, 0x8c, 0x8c, 0xe9, 0x14, 0xd5, 0x40, 0x02, 0x13, 0x71, 0x0f, 0x8e,
    0x38, 0x83, 0xf8, 0x83, 0x30, 0x00, 0xf8, 0x83, 0x25, 0x8e, 0xf8, 0x83, 0x30, 0x00, 0xf8, 0x83,
    0xff, 0xff, 0xff, 0xff, 0x0f, 0xfe, 0xa9, 0xd6, 0x24, 0x00, 0x04, 0x00, 0x1a, 0x82, 0x79, 0xa1,
    0x00, 0x00, 0x00, 0x3c, 0xff, 0xff, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x38, 0x83, 0x6f, 0xa2,
    0x00, 0x3e, 0x03, 0x30, 0x40, 0x00, 0x00, 0x00, 0x02, 0xca, 0xe3, 0x09, 0x3e, 0x80, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c, 0x18, 0x6e, 0x00, 0x00, 0x06, 0x92, 0x0a, 0x16, 0xc0, 0xdf,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x0b, 0x2b, 0x00, 0x00, 0x16, 0x57, 0x00, 0x00, 0x03, 0x59,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0xfa, 0x00, 0x02, 0x6c, 0x1d, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x42, 0xb5, 0x00, 0x00, 0x39, 0xa2, 0x00, 0x00, 0xb3, 0x65,
    0xd9, 0x0e, 0x9f, 0xc9, 0x1d, 0xcf, 0x4c, 0x34, 0x30, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
    0x3b, 0xb6, 0x7a, 0xe8, 0x00, 0x64, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* bank # 1 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x72, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x00, 0x0f,
    0x08, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb2, 0x6a, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x16, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x10, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf4, 0x00, 0x00, 0x10, 0x00,
    /* bank # 2 */
    0x00, 0x28, 0x00, 0x00, 0xff, 0xff, 0x45, 0x81, 0xff, 0xff, 0xfa, 0x72, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x05, 0xba, 0xc6, 0x00, 0x47, 0x78, 0xa2,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x14,
    0x00, 0x00, 0x25, 0x4d, 0x00, 0x2f, 0x70, 0x6d, 0x00, 0x00, 0x05, 0xae, 0x00, 0x0c, 0x02, 0xd0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x64, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x05, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x05, 0x00, 0x05,
    0xff, 0xe5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* bank # 3 */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x24, 0x26, 0xd3,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x10, 0x00, 0x96, 0x00, 0x3c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x0a, 0x4e, 0x68, 0xcd, 0xcf, 0x77, 0x09, 0x50, 0x16, 0x67, 0x59, 0xc6, 0x19, 0xce, 0x82,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0xd7, 0x84, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0x93, 0x8f, 0x9d, 0x1e, 0x1b, 0x1c, 0x19,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x18, 0x85, 0x00, 0x00, 0x40, 0x00,
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x67, 0x7d, 0xdf, 0x7e, 0x72, 0x90, 0x2e, 0x55, 0x4c, 0xf6, 0xe6, 0x88,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* bank # 4 */
    0xd8, 0xb0, 0xb5, 0xb9, 0xf3, 0xa6, 0xf8, 0xf9, 0xd1, 0xd9, 0x81, 0x96, 0xf8, 0xf7, 0x3e, 0xd8,
    0xf3, 0xb1, 0x86, 0x96, 0xa3, 0x31, 0xd1, 0xda, 0xf1, 0xff, 0xd8, 0xb3, 0xb7, 0xbb, 0x8e, 0x9e,
    0xae, 0xf1, 0x32, 0xf5, 0x1b, 0xf1, 0xb4, 0xb8, 0xb0, 0x80, 0x97, 0xf1, 0xa9, 0xdf, 0xdf, 0xdf,
    0xaa, 0xdf, 0xdf, 0xdf, 0xf2, 0xaa, 0xc5, 0xcd, 0xc7, 0xa9, 0x0c, 0xc9, 0x2c, 0x97, 0xf1, 0xa9,
    0x89, 0x26, 0x46, 0x66, 0xb2, 0x89, 0x99, 0xa9, 0x2d, 0x55, 0x7d, 0xb0, 0x8a, 0xa8, 0x96, 0x36,
    0x56, 0x76, 0xf1, 0xba, 0xa3, 0xb4, 0xb2, 0x80, 0xc0, 0xb8, 0xa8, 0x97, 0x11, 0xb2, 0x83, 0x98,
    0xba, 0xa3, 0xf0, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xb2, 0xb9, 0xb4, 0x98, 0x83, 0xf1, 0xa3,
    0x29, 0x55, 0x7d, 0xba, 0xb5, 0xb1, 0xa3, 0x83, 0x93, 0xf0, 0x00, 0x28, 0x50, 0xf5, 0xb2, 0xb6,
    0xaa, 0x83, 0x93, 0x28, 0x54, 0x7c, 0xf1, 0xb9, 0xa3, 0x82, 0x93, 0x61, 0xba, 0xa2, 0xda, 0xde,
    0xdf, 0xdb, 0x81, 0x9a, 0xb9, 0xae, 0xf5, 0x60, 0x68, 0x70, 0xf1, 0xda, 0xba, 0xa2, 0xdf, 0xd9,
    0xba, 0xa2, 0xfa, 0xb9, 0xa3, 0x82, 0x92, 0xdb, 0x31, 0xba, 0xa2, 0xd9, 0xba, 0xa2, 0xf8, 0xdf,
    0xb9, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xb8, 0xa2, 0xdf, 0xdf, 0xdf, 0xba, 0xa0, 0xdf, 0xdf, 0xdf,
    0xd8, 0xf1, 0xb9, 0xa1, 0xb2, 0x80, 0xc0, 0xb0, 0x89, 0xa0, 0xc3, 0xc5, 0xc7, 0xb1, 0x81, 0xb4,
    0x97, 0xa0, 0x11, 0xb5, 0x90, 0xa1, 0xf0, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xf1, 0x90, 0x81,
    0xa0, 0x2d, 0x55, 0x3d, 0xf2, 0xa5, 0xd0, 0xf8, 0xf9, 0xd1, 0xd9, 0xf8, 0xf1, 0x80, 0xa2, 0xc3,
    0xc5, 0xc7, 0xd8, 0xf3, 0xa2, 0xde, 0xf1, 0x82, 0x90, 0xa4, 0x2d, 0x55, 0x7d, 0x84, 0x94, 0xf5,
    /* bank # 5 */
    0xa1, 0x20, 0xd9, 0xf3, 0xa2, 0xf8, 0xd8, 0xf5, 0xa1, 0x40, 0xd9, 0xf3, 0xa2, 0xf8, 0xd8, 0xf5,
    0xa1, 0x60, 0xd9, 0xf3, 0xa2, 0xf8, 0xd8, 0xf2, 0xb9, 0xa2, 0xf8, 0xf9, 0xd1, 0xd9, 0xa6, 0xde,
    0xa5, 0xd0, 0xde, 0xf4, 0x1a, 0xd8, 0xf2, 0x86, 0x96, 0xa1, 0x09, 0xd1, 0xdb, 0xf4, 0x11, 0xd8,
    0xf2, 0x11, 0xd1, 0xdb, 0xf4, 0x1c, 0xd8, 0xf2, 0x19, 0xd1, 0xdb, 0xf4, 0x14, 0xd8, 0xf4, 0x10,
    0xd8, 0xf3, 0xa2, 0xf8, 0xf9, 0xd1, 0xda, 0xf2, 0xa6, 0xf8, 0xf1, 0xa5, 0xde, 0xd8, 0xf4, 0xa1,
    0x16, 0x16, 0xd8, 0xf1, 0xa5, 0xf8, 0xa1, 0x85, 0x95, 0x09, 0xd9, 0xf1, 0xa5, 0xde, 0xf2, 0xa5,
    0xd0, 0xde, 0xa6, 0xf8, 0xd8, 0xf4, 0xa1, 0x09, 0xd8, 0xf3, 0xa2, 0xf8, 0xf9, 0xd1, 0xf4, 0xd9,
    0x08, 0x19, 0xda, 0x42, 0xf2, 0xa2, 0xde, 0xf4, 0x0b, 0xd8, 0xf1, 0xa5, 0xf8, 0xa1, 0x85, 0x95,
    0x19, 0xda, 0xf4, 0x07, 0xd8, 0xf2, 0xa6, 0xde, 0xa5, 0xd0, 0xde, 0xd8, 0xf1, 0xb8, 0xaa, 0xb3,
    0x8d, 0xb4, 0x98, 0x0d, 0x35, 0x5d, 0xb2, 0xb6, 0xba, 0xaf, 0x8c, 0x96, 0x19, 0x8f, 0x9f, 0xa7,
    0x0e, 0x16, 0x1e, 0xb4, 0x9a, 0xb8, 0xaa, 0x87, 0x2c, 0x54, 0x7c, 0xba, 0xa4, 0xb0, 0x8a, 0xb6,
    0x91, 0x32, 0x56, 0x76, 0xb2, 0x84, 0x94, 0xa4, 0xd0, 0xcd, 0xa4, 0xd0, 0xc8, 0x30, 0xd8, 0xb8,
    0xb4, 0xb0, 0xf1, 0x99, 0x82, 0xa8, 0x2d, 0x55, 0x7d, 0x98, 0xa8, 0x0e, 0x16, 0x1e, 0xa2, 0x2c,
    0x54, 0x7c, 0x92, 0xa4, 0xf0, 0x2c, 0x50, 0x78, 0xf1, 0x84, 0xa8, 0x98, 0xc4, 0xcd, 0x88, 0x94,
    0xd8, 0x0d, 0xdb, 0xa8, 0x84, 0x98, 0x2d, 0xf3, 0xd9, 0xba, 0xa6, 0xf8, 0xda, 0xba, 0xa6, 0xde,
    0xd8, 0xba, 0xb2, 0xb6, 0x86, 0x96, 0xa6, 0xd0, 0xf3, 0xc8, 0x41, 0xda, 0xa6, 0xc8, 0xf8, 0xd8,
    /* bank # 6 */
    0xb0, 0xb4, 0xb8, 0x82, 0xa8, 0x92, 0xf5, 0x2c, 0x54, 0x88, 0x98, 0xf1, 0x35, 0xd9, 0xf4, 0x18,
    0xd8, 0xf1, 0xa2, 0xd0, 0xf8, 0xf9, 0xa8, 0x84, 0xd9, 0xc7, 0xdf, 0xf8, 0xf8, 0x83, 0xc5, 0xda,
    0xdf, 0x69, 0xdf, 0x83, 0xc1, 0xd8, 0xf4, 0x01, 0x14, 0xf1, 0xa8, 0x82, 0x4e, 0xa8, 0x84, 0xf3,
    0x11, 0xd1, 0x82, 0xf5, 0xd9, 0x92, 0x28, 0x97, 0x88, 0xf1, 0x09, 0xf4, 0x1c, 0x1c, 0xd8, 0x84,
    0xa8, 0xf3, 0xc0, 0xf9, 0xd1, 0xd9, 0x97, 0x82, 0xf1, 0x29, 0xf4, 0x0d, 0xd8, 0xf3, 0xf9, 0xf9,
    0xd1, 0xd9, 0x82, 0xf4, 0xc2, 0x03, 0xd8, 0xde, 0xdf, 0x1a, 0xd8, 0xf1, 0xa2, 0xfa, 0xf9, 0xa8,
    0x84, 0x98, 0xd9, 0xc7, 0xdf, 0xf8, 0xf8, 0xf8, 0x83, 0xc7, 0xda, 0xdf, 0x69, 0xdf, 0xf8, 0x83,
    0xc3, 0xd8, 0xf4, 0x01, 0x14, 0xf1, 0x98, 0xa8, 0x82, 0x2e, 0xa8, 0x84, 0xf3, 0x11, 0xd1, 0x82,
    0xf5, 0xd9, 0x92, 0x50, 0x97, 0x88, 0xf1, 0x09, 0xf4, 0x1c, 0xd8, 0x84, 0xa8, 0xf3, 0xc0, 0xf8,
    0xf9, 0xd1, 0xd9, 0x97, 0x82, 0xf1, 0x49, 0xf4, 0x0d, 0xd8, 0xf3, 0xf9, 0xf9, 0xd1, 0xd9, 0x82,
    0xf4, 0xc4, 0x03, 0xd8, 0xde, 0xdf, 0xd8, 0xf1, 0xad, 0x88, 0x98, 0xcc, 0xa8, 0x09, 0xf9, 0xd9,
    0x82, 0x92, 0xa8, 0xf5, 0x7c, 0xf1, 0x88, 0x3a, 0xcf, 0x94, 0x4a, 0x6e, 0x98, 0xdb, 0x69, 0x31,
    0xda, 0xad, 0xf2, 0xde, 0xf9, 0xd8, 0x87, 0x95, 0xa8, 0xf2, 0x21, 0xd1, 0xda, 0xa5, 0xf9, 0xf4,
    0x17, 0xd9, 0xf1, 0xae, 0x8e, 0xd0, 0xc0, 0xc3, 0xae, 0x82, 0xc6, 0x84, 0xc3, 0xa8, 0x85, 0x95,
    0xc8, 0xa5, 0x88, 0xf2, 0xc0, 0xf1, 0xf4, 0x01, 0x0e, 0xf1, 0x8e, 0x9e, 0xa8, 0xc6, 0x3e, 0x56,
    0xf5, 0x54, 0xf1, 0x88, 0x72, 0xf4, 0x01, 0x15, 0xf1, 0x98, 0x45, 0x85, 0x6e, 0xf5, 0x8e, 0x9e,
    /* bank # 7 */
    0x04, 0x88, 0xf1, 0x42, 0x98, 0x5a, 0x8e, 0x9e, 0x06, 0x88, 0x69, 0xf4, 0x01, 0x1c, 0xf1, 0x98,
    0x1e, 0x11, 0x08, 0xd0, 0xf5, 0x04, 0xf1, 0x1e, 0x97, 0x02, 0x02, 0x98, 0x36, 0x25, 0xdb, 0xf9,
    0xd9, 0x85, 0xa5, 0xf3, 0xc1, 0xda, 0x85, 0xa5, 0xf3, 0xdf, 0xd8, 0x85, 0x95, 0xa8, 0xf3, 0x09,
    0xda, 0xa5, 0xfa, 0xd8, 0x82, 0x92, 0xa8, 0xf5, 0x78, 0xf1, 0x88, 0x1a, 0x84, 0x9f, 0x26, 0x88,
    0x98, 0x21, 0xda, 0xf4, 0x1d, 0xf3, 0xd8, 0x87, 0x9f, 0x39, 0xd1, 0xaf, 0xd9, 0xdf, 0xdf, 0xfb,
    0xf9, 0xf4, 0x0c, 0xf3, 0xd8, 0xfa, 0xd0, 0xf8, 0xda, 0xf9, 0xf9, 0xd0, 0xdf, 0xd9, 0xf9, 0xd8,
    0xf4, 0x0b, 0xd8, 0xf3, 0x87, 0x9f, 0x39, 0xd1, 0xaf, 0xd9, 0xdf, 0xdf, 0xf4, 0x1e, 0xf3, 0xd8,
    0xfa, 0x8f, 0x97, 0xa8, 0x69, 0xf9, 0xf9, 0xaf, 0xd0, 0xda, 0xde, 0xfa, 0xd9, 0xf8, 0x8f, 0x9f,
    0xa8, 0xf1, 0xcc, 0xf3, 0x98, 0xdb, 0x45, 0xd9, 0xaf, 0xdf, 0xd0, 0xf8, 0xd8, 0xf1, 0x8f, 0x9f,
    0xa8, 0xca, 0xf3, 0x88, 0x09, 0xda, 0xaf, 0x8f, 0xcb, 0xf8, 0xd8, 0xf2, 0xad, 0x97, 0x8d, 0x0c,
    0xd9, 0xa5, 0xdf, 0xf9, 0xba, 0xa6, 0xf3, 0xfa, 0xf4, 0x12, 0xf2, 0xd8, 0x95, 0x0d, 0xd1, 0xd9,
    0xba, 0xa6, 0xf3, 0xfa, 0xda, 0xa5, 0xf2, 0xc1, 0xba, 0xa6, 0xf3, 0xdf, 0xd8, 0xf1, 0xba, 0xb2,
    0xb6, 0x86, 0x96, 0xa6, 0xd0, 0xca, 0xf3, 0x49, 0xda, 0xa6, 0xcb, 0xf8, 0xd8, 0xb0, 0xb4, 0xb8,
    0xd8, 0xad, 0x84, 0xf2, 0xc0, 0xdf, 0xf1, 0x8f, 0xcb, 0xc3, 0xa8, 0xb2, 0xb6, 0x86, 0x96, 0xc8,
    0xc1, 0xcb, 0xc3, 0xf3, 0xb0, 0xb4, 0x88, 0x98, 0xa8, 0x21, 0xdb, 0x71, 0x8d, 0x9d, 0x71, 0x85,
    0x95, 0x21, 0xd9, 0xad, 0xf2, 0xfa, 0xd8, 0x85, 0x97, 0xa8, 0x28, 0xd9, 0xf4, 0x08, 0xd8, 0xf2,
    /* bank # 8 */
    0x8d, 0x29, 0xda, 0xf4, 0x05, 0xd9, 0xf2, 0x85, 0xa4, 0xc2, 0xf2, 0xd8, 0xa8, 0x8d, 0x94, 0x01,
    0xd1, 0xd9, 0xf4, 0x11, 0xf2, 0xd8, 0x87, 0x21, 0xd9, 0xf4, 0x0a, 0xd8, 0xf2, 0x84, 0x98, 0xa8,
    0xc8, 0x01, 0xd1, 0xd9, 0xf4, 0x06, 0xd8, 0xf3, 0xa4, 0xc8, 0xf4, 0x41, 0xf1, 0xd8, 0xb8, 0xb4,
    0xb0, 0x97, 0x86, 0xa8, 0x31, 0x9b, 0x06, 0x99, 0x07, 0xab, 0x97, 0x28, 0x88, 0x9b, 0xf0, 0x0c,
    0x20, 0x14, 0x40, 0xb0, 0xb4, 0xb8, 0xf0, 0xa8, 0x8a, 0x9a, 0x28, 0x50, 0x78, 0xb7, 0x9b, 0xa8,
    0x29, 0x51, 0x79, 0x24, 0x70, 0x59, 0x44, 0x69, 0x38, 0x64, 0x48, 0x31, 0xf1, 0xbb, 0xab, 0x88,
    0x00, 0x2c, 0x54, 0x7c, 0xf0, 0xb3, 0x8b, 0xb8, 0xa8, 0x04, 0x28, 0x50, 0x78, 0xf1, 0xb0, 0x88,
    0xb4, 0x97, 0x26, 0xa8, 0x59, 0x98, 0xbb, 0xab, 0xb3, 0x8b, 0x02, 0x26, 0x46, 0x66, 0xb0, 0xb8,
    0xf0, 0x8a, 0x9c, 0xa8, 0x29, 0x51, 0x79, 0x8b, 0x29, 0x51, 0x79, 0x8a, 0x24, 0x70, 0x59, 0x8b,
    0x20, 0x58, 0x71, 0x8a, 0x44, 0x69, 0x38, 0x8b, 0x39, 0x40, 0x68, 0x8a, 0x64, 0x48, 0x31, 0x8b,
    0x30, 0x49, 0x60, 0x88, 0xf1, 0xac, 0x00, 0x2c, 0x54, 0x7c, 0xf0, 0x8c, 0xa8, 0x04, 0x28, 0x50,
    0x78, 0xf1, 0x88, 0x97, 0x26, 0xa8, 0x59, 0x98, 0xac, 0x8c, 0x02, 0x26, 0x46, 0x66, 0xf0, 0x89,
    0x9c, 0xa8, 0x29, 0x51, 0x79, 0x24, 0x70, 0x59, 0x44, 0x69, 0x38, 0x64, 0x48, 0x31, 0xa9, 0x88,
    0x09, 0x20, 0x59, 0x70, 0xab, 0x11, 0x38, 0x40, 0x69, 0xa8, 0x19, 0x31, 0x48, 0x60, 0x8c, 0xa8,
    0x3c, 0x41, 0x5c, 0x20, 0x7c, 0x00, 0xf1, 0x87, 0x98, 0x19, 0x86, 0xa8, 0x6e, 0x76, 0x7e, 0xa9,
    0x99, 0x88, 0x2d, 0x55, 0x7d, 0xd8, 0xf1, 0xb3, 0x8b, 0xb4, 0x97, 0xbb, 0xab, 0xf8, 0xf9, 0xb9,
    /* bank # 9 */
    0xa3, 0xda, 0xc3, 0xc5, 0xc7, 0xd9, 0x2d, 0x4d, 0x6d, 0xd8, 0xb0, 0x8c, 0xb8, 0xac, 0xf8, 0xf9,
    0xb9, 0xa9, 0xda, 0xc3, 0xc5, 0xc7, 0xd9, 0x2d, 0x4d, 0x6d, 0xd8, 0xf3, 0xb9, 0xac, 0xde, 0xd8,
    0xb1, 0xb6, 0xb9, 0xf1, 0xa8, 0xf8, 0xf3, 0xb9, 0xaa, 0xfa, 0xf9, 0xd1, 0xda, 0xf2, 0x8a, 0xca,
    0xf4, 0x0e, 0xf3, 0xb1, 0x88, 0xdd, 0xc2, 0xc2, 0xf1, 0x88, 0xc0, 0xdc, 0xf3, 0xb9, 0xac, 0xf8,
    0xd8, 0xf2, 0xb2, 0xb6, 0xba, 0xad, 0xfa, 0x8d, 0x9d, 0xab, 0x39, 0xd9, 0xad, 0xdf, 0xf4, 0x13,
    0xdd, 0xf2, 0xb1, 0x87, 0xb5, 0x9a, 0x08, 0x08, 0xf1, 0xb1, 0x83, 0xc2, 0xc4, 0xc6, 0xdc, 0xf3,
    0xb9, 0xac, 0xf8, 0xd8, 0xf3, 0xb2, 0xb6, 0xba, 0xad, 0xf8, 0x8d, 0x9d, 0xab, 0x11, 0xd9, 0xad,
    0xde, 0xf4, 0x13, 0xdd, 0xf2, 0xb1, 0x87, 0xb5, 0x9a, 0x28, 0x28, 0xf1, 0xb1, 0x89, 0xc2, 0xc4,
    0xc6, 0xdc, 0xf3, 0xb9, 0xac, 0xf8, 0xd8, 0xf3, 0xb2, 0xb6, 0xba, 0xad, 0xfa, 0x8d, 0x9d, 0xab,
    0x39, 0xd9, 0xad, 0xdf, 0xf4, 0x11, 0xdd, 0xf3, 0xb1, 0x87, 0xb5, 0x9a, 0x08, 0xf2, 0x88, 0xc2,
    0xc4, 0xc6, 0xdc, 0xf3, 0xb9, 0xac, 0xf8, 0xd8, 0xf2, 0xb2, 0xb6, 0xba, 0xae, 0xf8, 0x8e, 0x9e,
    0xab, 0x11, 0xd9, 0xae, 0xde, 0xf4, 0x12, 0xdd, 0xf3, 0xb1, 0x87, 0xb5, 0x9a, 0x68, 0xf2, 0xb0,
    0x80, 0xc0, 0xc8, 0xc2, 0xdc, 0xf3, 0xb9, 0xac, 0xf8, 0xd8, 0xf2, 0xb2, 0xb6, 0xba, 0xae, 0xfa,
    0x8e, 0x9e, 0xab, 0x39, 0xd9, 0xae, 0xdf, 0xf4, 0x11, 0xdd, 0xf2, 0xb1, 0x87, 0xb5, 0x9a, 0x68,
    0xb0, 0x80, 0xc4, 0xcc, 0xc6, 0xdc, 0xf3, 0xb9, 0xac, 0xf8, 0xd8, 0xf3, 0xb2, 0xb6, 0xba, 0xae,
    0xf8, 0x8e, 0x9e, 0xab, 0x11, 0xd9, 0xae, 0xde, 0xf4, 0x12, 0xdd, 0xf3, 0xb1, 0x87, 0xb5, 0x9a,
    /* bank # 10 */
    0x48, 0xf2, 0xb0, 0x81, 0xc0, 0xc8, 0xc2, 0xdc, 0xf3, 0xb9, 0xac, 0xf8, 0xd8, 0xf3, 0xb2, 0xb6,
    0xba, 0xae, 0xfa, 0x8e, 0x9e, 0xab, 0x39, 0xd9, 0xae, 0xdf, 0xf4, 0x12, 0xdd, 0xf2, 0xb1, 0x87,
    0xb5, 0x9a, 0x48, 0xf2, 0xb0, 0x81, 0xc4, 0xcc, 0xc6, 0xdc, 0xf3, 0xb9, 0xac, 0xf8, 0xd8, 0xf2,
    0xb1, 0x89, 0xb9, 0xa8, 0xc3, 0xc5, 0xc7, 0xd8, 0xf3, 0xb9, 0xac, 0xf8, 0xf9, 0xd1, 0xd9, 0xf4,
    0x1f, 0xd8, 0xf1, 0xb9, 0xaa, 0xdf, 0xf3, 0xac, 0xfa, 0xf9, 0xd1, 0xd9, 0xf4, 0x1a, 0xd8, 0xf2,
    0xb1, 0xb5, 0x8c, 0xac, 0xf8, 0xf9, 0xd1, 0xd9, 0xc2, 0xd8, 0xf2, 0xf9, 0xd9, 0xde, 0xf4, 0x09,
    0x13, 0xda, 0xf2, 0xdd, 0xc6, 0xdc, 0xf4, 0x31, 0x11, 0xd8, 0xf2, 0xab, 0xfa, 0x8b, 0x9b, 0xa3,
    0x49, 0xd9, 0xf4, 0x07, 0x06, 0xda, 0xf2, 0xb9, 0xab, 0xdf, 0xfe, 0xd8, 0xf1, 0xbb, 0xb3, 0xb7,
    0xaa, 0xf9, 0xda, 0xff, 0xd9, 0x80, 0x9a, 0xaa, 0x28, 0xb4, 0x80, 0x98, 0xa7, 0x20, 0xb7, 0x97,
    0x87, 0xa8, 0x66, 0x88, 0xf0, 0x79, 0x51, 0xf1, 0x90, 0x2c, 0x87, 0x0c, 0xa7, 0x81, 0x97, 0x62,
    0x93, 0xf0, 0x71, 0x71, 0x60, 0x85, 0x94, 0x01, 0x29, 0x51, 0x79, 0x90, 0xa5, 0xf1, 0x28, 0x4c,
    0x6c, 0x87, 0x0c, 0x95, 0x18, 0x85, 0x78, 0xa3, 0x83, 0x90, 0x28, 0x4c, 0x6c, 0x88, 0x6c, 0xd8,
    0xf3, 0xa2, 0x82, 0x00, 0xf2, 0x10, 0xa8, 0x92, 0x19, 0x80, 0xa2, 0xf2, 0xd9, 0x26, 0xd8, 0xf1,
    0x88, 0xa8, 0x4d, 0xd9, 0x48, 0xd8, 0x96, 0xa8, 0x39, 0x80, 0xd9, 0x3c, 0xd8, 0x95, 0x80, 0xa8,
    0x39, 0xa6, 0x86, 0x98, 0xd9, 0x2c, 0xda, 0x87, 0xa7, 0x2c, 0xd8, 0xa8, 0x89, 0x95, 0x19, 0xa9,
    0x80, 0xd9, 0x38, 0xd8, 0xa8, 0x89, 0x39, 0xa9, 0x80, 0xda, 0x3c, 0xd8, 0xa8, 0x2e, 0x85, 0xf5,
    /* bank # 11 */
    0x75, 0xda, 0xff, 0xd8, 0x71, 0x80, 0xa9, 0xda, 0xf1, 0x26, 0xff, 0xd8, 0x90, 0xa8, 0x0d, 0x89,
    0x99, 0xa8, 0x10, 0x80, 0x98, 0x21, 0xda, 0x2e, 0xd8, 0x89, 0x99, 0xa8, 0x31, 0x80, 0xda, 0x2e,
    0xd8, 0xa8, 0x86, 0x96, 0x31, 0x80, 0xda, 0x2e, 0xd8, 0xa8, 0x87, 0x31, 0x80, 0xda, 0x2e, 0xd8,
    0xa8, 0x82, 0x92, 0xf3, 0x41, 0x80, 0xf1, 0xd9, 0x2e, 0xd8, 0xa8, 0x82, 0xf3, 0x19, 0x80, 0xf1,
    0xd9, 0x2e, 0xd8, 0x82, 0xac, 0xf3, 0xc0, 0xa2, 0x80, 0x22, 0xf1, 0xa6, 0x2e, 0xa7, 0x2e, 0xa9,
    0x22, 0x98, 0xa8, 0x29, 0xda, 0xac, 0xde, 0xff, 0xd8, 0xa2, 0xf2, 0x2a, 0xf1, 0xa9, 0x2e, 0x82,
    0x92, 0xa8, 0xf2, 0x31, 0x80, 0xa6, 0x96, 0xf1, 0xd9, 0x00, 0xf1, 0xf1, 0xf1, 0xac, 0x8c, 0x9c,
    0x0c, 0x30, 0xac, 0xde, 0xd0, 0xde, 0xf3, 0xf3, 0xb9, 0xaa, 0xfa, 0xf1, 0xff, 0xd8, 0x8c, 0x9c,
    0xac, 0xd0, 0x10, 0xac, 0xde, 0x80, 0x92, 0xa2, 0xf2, 0x4c, 0x82, 0xa8, 0xf1, 0xca, 0xf2, 0x35,
    0xf1, 0x96, 0x88, 0xa6, 0xd9, 0x00, 0xd8, 0xff, 0xff
  }
};


/*------------------------------------------------------------------------------
  Declarations for functions defined in one file and used in another.
------------------------------------------------------------------------------*/

sns_ddf_status_e mpu6515_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors);

sns_ddf_status_e mpu6515_get_attr(
   sns_ddf_handle_t       dd_handle,
   sns_ddf_sensor_e       sensor,
   sns_ddf_attribute_e    attrib,
   sns_ddf_memhandler_s*  memhandler,
   void**                 value,
   uint32_t*              num_elems);

sns_ddf_status_e mpu6515_get_data(
   sns_ddf_handle_t       dd_handle,
   sns_ddf_sensor_e       sensors[],
   uint32_t               num_sensors,
   sns_ddf_memhandler_s*  memhandler,
   sns_ddf_sensor_data_s* data[]);

sns_ddf_status_e mpu6515_reset_device(mpu6515_s* state);

sns_ddf_status_e mpu6515_reset(sns_ddf_handle_t dd_handle);

#ifdef FEATURE_MPU6050_INT_PIN_CONFIG
sns_ddf_status_e sns_dd_mpu3050_int_disable(void);
#endif

sns_ddf_status_e mpu6515_enable_interrupt(
    mpu6515_s*           state,
    mpu6515_sub_dev_s*          sub_dev,
    mpu6515_sensor_s*           sensor,
    mpu6515_interrupt_t  interrupt);

sns_ddf_status_e mpu6515_disable_interrupt(
    mpu6515_s*           state,
    mpu6515_sub_dev_s*          sub_dev,
    mpu6515_sensor_s*           sensor,
    mpu6515_interrupt_t  interrupt);

sns_ddf_status_e mpu6515_set_lowpass(
   mpu6515_s* state,
   uint8_t           lpf_index);

void mpu6515_handle_st_timer(mpu6515_s* state);

sns_ddf_status_e mpu6515_run_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err);

mpu6515_s* mpu6515_get_device(sns_ddf_handle_t dd_handle);

mpu6515_sub_dev_s* mpu6515_get_sub_dev(sns_ddf_handle_t dd_handle);

uint8_t mpu6515_get_sub_dev_idx(sns_ddf_handle_t dd_handle);

mpu6515_sensor_s* mpu6515_get_sensor(
    sns_ddf_handle_t dd_handle, sns_ddf_sensor_e sensor);

mpu6515_sensor_s* mpu6515_search_for_sensor(
    mpu6515_s* state, sns_ddf_sensor_e sensor, mpu6515_sub_dev_s** sub_dev_out);

sns_ddf_status_e mpu6515_set_powerstate(
   mpu6515_s*    state,
   uint8_t              sub_dev_idx,
   sns_ddf_powerstate_e powerstate);

sns_ddf_status_e mpu6515_set_odr (
    mpu6515_s* state,
    mpu6515_sensor_s* sensor, uint32_t odr);

sns_ddf_status_e mpu6515_set_config_register(
   mpu6515_s* state,
   mpu6515_sensor_s* sensor,
   uint8_t           range_index,
   uint8_t           accel_hpf,
   boolean           enable_selftest);

sns_ddf_status_e mpu6515_write_dmp_register(mpu6515_s * state,
    uint16_t address, uint8_t * value, uint16_t size);

sns_ddf_status_e mpu6515_load_dmp_firmware(mpu6515_s * state,
    const unsigned char * image, uint16_t image_size);

sns_ddf_status_e mpu6515_get_data_from_device(
   mpu6515_s* state, bool map_axes);

uint8_t sns_dd_dmp_get_sensor(sns_ddf_sensor_e s);

uint16_t sns_dd_dmp_compute_batch_threshold(uint8_t percent, uint8_t pktsize);
uint16_t dmp_get_pkt_size(uint8_t dmp_sensor );

bool mpu6515_check_dmp_heartbeat_reg(mpu6515_s * state);

sns_ddf_status_e mpu6515_read_dmp_firmware(mpu6515_s * state);
sns_ddf_status_e mpu6515_read_dmp_register(mpu6515_s * state, uint16_t address, uint8_t * value, uint16_t size);
sns_ddf_status_e mpu6515_read_dmp_reg(mpu6515_s * state, uint16_t address, uint8_t *buf, uint8_t buf_len);

sns_ddf_status_e mpu6515_validate_device_id(mpu6515_s* state);
sns_ddf_status_e mpu6515_set_device_powerstate(mpu6515_s* state, sns_ddf_powerstate_e powerstate);
bool mpu6515_is_dmp_active(mpu6515_s* state);
sns_ddf_handle_t mpu6515_get_sensor_handle(sns_ddf_handle_t dd_handle, sns_ddf_sensor_e sensor);
sns_ddf_status_e mpu6515_set_attr(
   sns_ddf_handle_t    dd_handle,
   sns_ddf_sensor_e    sensor_type,
   sns_ddf_attribute_e attrib,
   void*               value);

extern uint8_t mpu6515_accel_range_reg[];
extern uint8_t mpu6515_gyro_range_reg[];
extern const q16_t mpu6515_gyro_sensitivity[];
extern q16_t mpu6515_accel_sensitivity[];
extern const q16_t mpu6515_temp_sensitivity;

#endif

