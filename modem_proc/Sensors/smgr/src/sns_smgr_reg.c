/*=============================================================================
  @file sns_smgr_reg.c

  This file contains the logic for handling registry in Sensor Manager

*******************************************************************************
*   Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
*   Qualcomm Technologies Proprietary and Confidential.
*
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_reg.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-27  KL   Added pah8011
  2015-09-24  AH   Added LSM6DS3
  2015-08-25  AH   Handle reset pin in auto-detect path
  2015-08-19  pn   Simplified powerrails control
  2015-07-01  HW   Reverted Asha's power rail config hack
  2015-06-29  AGK  Added power rail info per sensor
  2015-05-20  AH   Support to request SNS_REG_GROUP_SSI_SMGR_CFG_3
  2015-04-09  pn   Added support for 56-bit timestamp
  2015-04-03  AH   Added DTS201A
  2015-03-12  AH   Added ADPD142
  2015-03-10  MW   Added HSVDDD002A
  2015-01-15  SH   Added DAF Playback Accel Driver
  2015-01-14  MW   Added APDS9960,BMP280
  2014-12-10  MW   Reshuffled smgr_sensor_fn_ptr_map for HD22
  2014-21-14  MW   Added AKM099xx FIFO
  2014-10-30  MW   Added BMI160
  2014-10-14  MW   Added call to sns_hw_pnoc_vote() before probing
  2014-10-08  MW   Added ZPA2326
  2014-09-03  jms  Added SPI support
  2014-09-03  MW   Use sns_em_convert_usec_to_dspstick() for idle_to_ready and
                   off_to_idle conversion
  2014-08-26  MW   Fixed autodetect - enable then restore power rail and I2C
                   clk before and after probe is called
  2014-08-18  pn   Removed KW error
  2014-07-31  pn   Obsoleted FEATURE_TEST_DRI
  2014-07-14  VY   Fixed compiler warnings
  2014-06-25  MW   Added ISL29033
  2014-05-28  MW   Added HSPPAD038A
  2014-05-15  sc   Added LPS25H
  2014-04-23  pn   Initial version

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_smgr_main.h"
#include "sns_smgr_hw.h"
#include "sns_reg_api_v02.h"
#include "sns_dd.h"
#include "sns_reg_common.h"
#include "sns_smgr_reg.h"
#include "sns_smgr_mr.h"
#include "sns_smgr_util.h"
#include "sns_smgr_ddf_priv.h"

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SMGR_SENSOR_FN_PTR_MAP_TBLE_SIZE sizeof(smgr_sensor_fn_ptr_map)/sizeof(smgr_sensor_fn_ptr_map[0])

#define SNS_SMGR_SSI_GET_FIRST_DEVINFO_ID()         ( smgr_ssi_devinfo[0] )
#define SNS_SMGR_SSI_DEVINFO_IDX_TO_CFG_IDX(d_idx)  ( (d_idx)/SNS_REG_SSI_SMGR_CFG_NUM_SENSORS )
#define SNS_SMGR_SSI_IS_LAST_DEVINFO(id)    ( smgr_ssi_devinfo[ARR_SIZE(smgr_ssi_devinfo)-1] == (id) )

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
typedef struct
{
  uint8_t drvuuid[16];
  sns_ddf_driver_if_s * fun_ptr;
}smgr_sensor_config_fn_ptr_map_s;

typedef struct
{
  uint16 last_requested_sensor_dep_reg_group_id;
} sns_smgr_resp_cb_data_s;

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
/* the retry counts for a REG request for DD initialization */
static const smgr_sensor_config_fn_ptr_map_s smgr_sensor_fn_ptr_map[] = {

#ifdef CONFIG_SUPPORT_LSM303D
 { SNS_REG_UUID_LSM303D, &sns_dd_lsm303d_if},
#endif
#ifdef CONFIG_SUPPORT_LPS25H
	{ SNS_REG_UUID_LPS25H, &sns_dd_press_lps25h_if},	
#endif
#ifdef CONFIG_SUPPORT_L3GD20
  { SNS_REG_UUID_L3GD20, &sns_dd_gyr_if},
#endif
#ifdef CONFIG_SUPPORT_LSM6DS3
{ SNS_REG_UUID_LSM6DS3, &sns_dd_lsm6ds3_if},
#endif
#ifndef SENSORS_DD_DEV_FLAG
#ifdef CONFIG_SUPPORT_MPU6050
  { SNS_REG_UUID_MPU6050, &sns_dd_mpu6xxx_if },
#endif
#ifdef CONFIG_SUPPORT_QDSP_SIM_PLAYBACK
  { SNS_REG_UUID_QDSP_SIM_PLAYBACK, &sns_dd_qdsp_playback_if },
#endif
#ifdef CONFIG_SUPPORT_DAF_PLAYBACK_ACCEL
  { SNS_REG_UUID_DAF_PLAYBACK_ACCEL, &sns_dd_daf_playback_accel_if },
#endif
#ifdef CONFIG_SUPPORT_MPU6500
  { SNS_REG_UUID_MPU6500, &sns_dd_mpu6xxx_if },
#endif
#ifdef CONFIG_SUPPORT_MPU6515
  { SNS_REG_UUID_MPU6515 , &sns_dd_mpu6515_if },
  { SNS_REG_UUID_MPU6515_AKM8963 , &sns_dd_mpu6515_if },
  { SNS_REG_UUID_MPU6515_BMP280 , &sns_dd_mpu6515_if },
#endif
#ifdef CONFIG_SUPPORT_ADXL
  { SNS_REG_UUID_ADXL350, &sns_accel_adxl350_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LIS3DH
  { SNS_REG_UUID_LIS3DH, &sns_dd_acc_lis3dh_if},
#endif
#ifdef CONFIG_SUPPORT_LIS3DSH
  { SNS_REG_UUID_LIS3DSH, &sns_dd_acc_lis3dsh_if},
#endif
#ifdef CONFIG_SUPPORT_BMA150
  { SNS_REG_UUID_BMA150, &sns_accel_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BMA250
  { SNS_REG_UUID_BMA250, &sns_accel_bma250_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BMG160
  { SNS_REG_UUID_BMG160,&sns_bmg160_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MPU3050
  { SNS_REG_UUID_MPU3050, &sns_gyro_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_L3G4200D
  { SNS_REG_UUID_L3G4200D, &sns_dd_gyr_l3g4200d_if},
#endif
#ifdef CONFIG_SUPPORT_AKM8963
  { SNS_REG_UUID_AKM8963, &sns_mag_akm8963_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AKM8975
  { SNS_REG_UUID_AKM8975, &sns_mag_akm8975_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AKM8973
  { SNS_REG_UUID_AKM8973, 0},
#endif
#ifdef CONFIG_SUPPORT_AMI306
  { SNS_REG_UUID_AMI306, &sns_mag_ami306_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BMP085
  { SNS_REG_UUID_BMP085, &sns_alt_bmp085_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BMP180
  { SNS_REG_UUID_BMP180, &sns_alt_bmp085_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LPS331AP
  { SNS_REG_UUID_LPS331AP, &sns_dd_press_lps331ap_if},
#endif
#ifdef CONFIG_SUPPORT_APDS99XX
  { SNS_REG_UUID_APDS99XX, &sns_dd_apds99xx_driver_if},
#endif
#ifdef CONFIG_SUPPORT_ISL29028
  { SNS_REG_UUID_ISL29028, &sns_alsprx_isl29028_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_ISL29147
  { SNS_REG_UUID_ISL29147, &sns_alsprx_isl29147_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_ISL29011
  { SNS_REG_UUID_LPS331AP, &sns_alsprx_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LSM303DLHC
  { SNS_REG_UUID_LSM303DLHC, &sns_dd_mag_lsm303dlhc_if},
#endif
#ifdef CONFIG_SUPPORT_YAS530
  { SNS_REG_UUID_YAS530, &sns_mag_yas_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MAX44009
  { SNS_REG_UUID_MAX44009, &sns_als_max44009_driver_if},
#endif
#ifdef CONFIG_SUPPORT_BMA2X2
  { SNS_REG_UUID_BMA2X2, &sns_accel_bma2x2_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LIS3DH
  { SNS_REG_UUID_LIS3DH, &sns_dd_acc_lis3dh_if},
#endif
#ifdef CONFIG_SUPPORT_BMA150
  { SNS_REG_UUID_BMA150, &sns_accel_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MPU3050
  { SNS_REG_UUID_MPU3050, &sns_gyro_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_HSCD008
  { SNS_REG_UUID_HSCD008, &sns_dd_mag_hscdtd_if},
#endif
#ifdef CONFIG_SUPPORT_AKM8975
  { SNS_REG_UUID_AKM8975, &sns_mag_akm8975_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_APDS99XX
  { SNS_REG_UUID_APDS99XX, &sns_dd_apds99xx_driver_if},
#endif
#ifdef CONFIG_SUPPORT_TMD277X
  { SNS_REG_UUID_TMD277X, &sns_alsprx_tmd277x_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LTR55X
  { SNS_REG_UUID_LTR55X, &sns_alsprx_ltr55x_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AL3320B
	{ SNS_REG_UUID_AL3320B, &sns_alsprx_al3320b_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_ISL29028
  { SNS_REG_UUID_ISL29028, &sns_alsprx_isl29028_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_SHTC1
  { SNS_REG_UUID_SHTC1, &sns_rht_shtc1_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_APDS9950
  { SNS_REG_UUID_APDS9950, &sns_dd_apds9950_driver_if},
#endif
#ifdef CONFIG_SUPPORT_MAX88120
  { SNS_REG_UUID_MAX88120, &sns_ges_max88120_driver_if},
#endif
#ifdef CONFIG_SUPPORT_TMG399X
  { SNS_REG_UUID_TMG399X, &sns_ams_tmg399x_alsprx_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_KXCJK
  { SNS_REG_UUID_KXCJK, &sns_accel_kxcjk_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MMA8452
  { SNS_REG_UUID_MMA8452, &sns_dd_acc_mma8452_if},
#endif
#ifdef CONFIG_SUPPORT_AP3216C
  { SNS_REG_UUID_AP3216C, &sns_alsprx_ap3216c_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_CM36283
	{ SNS_REG_UUID_CM36283, &sns_alsprx_cm36283_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_ISL29044A
	{ SNS_REG_UUID_ISL29044A, &sns_alsprx_isl29044a_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AKM09912
  {SNS_REG_UUID_AKM09912, &sns_mag_akm09912_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BH1721
  { SNS_REG_UUID_BH1721, &sns_als_bh1721_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_M34160PJ
	{ SNS_REG_UUID_M34160PJ, &sns_mmc3xxx_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_KXTIK
  { SNS_REG_UUID_KXTIK, &sns_accel_kxtik_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AD7146
  { SNS_REG_UUID_ADI7146, &sns_dd_sar_ad7146_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MAX44006
 { SNS_REG_UUID_MAX44006, &sns_als_rgb_max44006_driver_if},
#endif
#ifdef CONFIG_SUPPORT_BU52061NVX
 { SNS_REG_UUID_BU52061NVX, &sns_hall_bu52061_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MC3410
    { SNS_REG_UUID_MC3410, &sns_dd_acc_mc3410_if},
#endif	
#ifdef CONFIG_SUPPORT_AKM09911
	{ SNS_REG_UUID_AKM09911, &sns_mag_akm_driver_fn_list},	
#endif
#ifdef CONFIG_SUPPORT_HSPPAD038A
	{ SNS_REG_UUID_HSPPAD038A, &sns_dd_prs_hsppad_if},	
#endif
#ifdef CONFIG_SUPPORT_ISL29033
	{ SNS_REG_UUID_ISL29033, &sns_als_isl29033_driver_fn_list},	
#endif
#ifdef CONFIG_SUPPORT_ZPA2326
	{ SNS_REG_UUID_ZPA2326, &sns_dd_press_zpa2326_if},	
#endif
#ifdef CONFIG_SUPPORT_BMI160
	{ SNS_REG_UUID_BMI160, &sns_dd_fn_list_bmi160},	
#endif
#ifdef CONFIG_SUPPORT_AKM099xx_FIFO
	{ SNS_REG_UUID_AKM099xx_FIFO, &sns_dd_mag_akm09914_akm09915_fn_list},	
#endif
#ifdef CONFIG_SUPPORT_APDS9960
	{ SNS_REG_UUID_APDS9960, &sns_dd_apds9960_driver_if},	
#endif
#ifdef CONFIG_SUPPORT_BMP280
	{ SNS_REG_UUID_BMP280, &sns_alt_bmp280_driver_fn_list},	
#endif
#ifdef CONFIG_SUPPORT_HSVDDD002A
	{ SNS_REG_UUID_HSVDDD002A, &sns_dd_uva_amb_hsvddd_if},
#endif
#ifdef CONFIG_SUPPORT_ADPD142
    { SNS_REG_UUID_ADPD142, &sns_dd_ppg_adpd142_if},
#endif
#ifdef CONFIG_SUPPORT_DTS201A
    { SNS_REG_UUID_DTS201A, &sns_dd_dts201a_fn_list},
#endif
#ifdef CONFIG_SUPPORT_PAH8002
  {SNS_REG_UUID_PAH8002, &sns_dd_pah8002_if},
#endif
#ifdef CONFIG_SUPPORT_PAH8011
  {SNS_REG_UUID_PAH8011, &sns_dd_pah8011_if},
#endif


#endif // SENSORS_DD_DEV_FLAG

/* Generic configs for vendor driver development*/
#ifdef CONFIG_SUPPORT_VENDOR_1
  { SNS_REG_UUID_VENDOR_1, &sns_dd_vendor_if_1},
#endif
#ifdef CONFIG_SUPPORT_VENDOR_2
  { SNS_REG_UUID_VENDOR_2, &sns_dd_vendor_if_2},
#endif

};

/* SSI: SMGR groups */
static const uint16_t smgr_ssi_cfg[] = {
  SNS_REG_GROUP_SSI_SMGR_CFG_V02,
  SNS_REG_GROUP_SSI_SMGR_CFG_2_V02,
  SNS_REG_GROUP_SSI_SMGR_CFG_3_V02
};

/* SSI: Device Info (auto-detect) */
static const uint16_t smgr_ssi_devinfo[] = {
  SNS_REG_GROUP_SSI_DEVINFO_ACCEL_V02,
  SNS_REG_GROUP_SSI_DEVINFO_GYRO_V02,
  SNS_REG_GROUP_SSI_DEVINFO_MAG_V02,
  SNS_REG_GROUP_SSI_DEVINFO_PROX_LIGHT_V02,
  SNS_REG_GROUP_SSI_DEVINFO_PRESSURE_V02,
  SNS_REG_GROUP_SSI_DEVINFO_TAP_V02,
  SNS_REG_GROUP_SSI_DEVINFO_HUMIDITY_V02,
  SNS_REG_GROUP_SSI_DEVINFO_RGB_V02,
  SNS_REG_GROUP_SSI_DEVINFO_SAR_V02,
  SNS_REG_GROUP_SSI_DEVINFO_HALL_EFFECT_V02
};

static uint8_t smgr_sensor_cfg_cnt = 0;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*===========================================================================

  FUNCTION:   sns_smgr_uuid_to_fn_ptr

===========================================================================*/
/*!
  @brief find driver function pointer from UUID.

  @detail
  @param
   UUId - driver UUID

  @return
    driver function pointer
 */
/*=========================================================================*/
SMGR_STATIC sns_ddf_driver_if_s * sns_smgr_uuid_to_fn_ptr( const uint8_t * uuid)
{
  uint8_t i;

  for ( i=0; i<SMGR_SENSOR_FN_PTR_MAP_TBLE_SIZE; i++ )
  {
    if(SNS_OS_MEMCMP(uuid, smgr_sensor_fn_ptr_map[i].drvuuid, 16 )==0)
      return smgr_sensor_fn_ptr_map[i].fun_ptr ;
  }
  return NULL ;
}

/*===========================================================================

  FUNCTION:   sns_smgr_fn_ptr_to_uuid

===========================================================================*/
/*!
  @brief find driver function pointer from UUID.

  @detail
  @param
   fn_ptr - Pointer to driver function interface

  @return
    Pointer to static const UUID
 */
/*=========================================================================*/
SMGR_STATIC const uint8_t * sns_smgr_fn_ptr_to_uuid(const sns_ddf_driver_if_s *fn_ptr)
{
  uint8_t i;

  for ( i=0; i<SMGR_SENSOR_FN_PTR_MAP_TBLE_SIZE; i++ )
  {
    if(smgr_sensor_fn_ptr_map[i].fun_ptr == fn_ptr)
    {
      return smgr_sensor_fn_ptr_map[i].drvuuid;
    }
  }
  return NULL ;
}

/*===========================================================================

  FUNCTION:   smgr_is_valid_fac_cal

===========================================================================*/
/*!
  @brief check if factory calibration data is within the valid range

  @detail treat the value invalid when all biases values are 0
  @param
  @return
 */
/*=========================================================================*/
SMGR_STATIC bool smgr_is_valid_fac_cal(q16_t *cal_data_ptr)
{
  /* biases data validity */
  if ((0==cal_data_ptr[0]) && (0==cal_data_ptr[1]) &&  (0==cal_data_ptr[2]))
  {
    return false;
  }
  else
  {
    return true;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_cfg_idx

===========================================================================*/
/*!
  @brief  Checks if 'id' is a valid SSI CFG ID

  @param  id  : CFG ID
  @return   -1  : if 'id' does not exist in smgr_ssi_cfg[]
          >= 0  : 'id' exists, and value corresponds to the column index.
 */
/*=========================================================================*/
int sns_smgr_ssi_get_cfg_idx(uint16_t id)
{
  int i, rv = -1;

  for(i = 0; i < ARR_SIZE(smgr_ssi_cfg); i++)
  {
    if(smgr_ssi_cfg[i] == id)
    {
      rv = i;
      break;
    }
  }

  return rv;
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_is_last_cfg

===========================================================================*/
/*!
  @brief  Checks if id is the last SMGR CFG Id expected.

  @param  id  : SMGR CFG ID
  @return 'true' if so, 'false' otherwise
 */
/*=========================================================================*/
bool sns_smgr_ssi_is_last_cfg(uint16_t id)
{
  return ( smgr_ssi_cfg[ARR_SIZE(smgr_ssi_cfg)-1] == (id) );
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_cfg_id

===========================================================================*/
/*!
  @brief  Returns the 'idx'-th SMGR CFG Id in smgr_ssi_cfg table.

  @param  idx  : index
  @return   >= 0 : a valid Id
              -1 : otherwise
 */
/*=========================================================================*/
int32_t sns_smgr_ssi_get_cfg_id(uint8_t idx)
{
  return ( (idx < ARR_SIZE(smgr_ssi_cfg)) ? (int32_t)smgr_ssi_cfg[idx] : -1 );
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_devinfo_idx

===========================================================================*/
/*!
  @brief  Checks if 'id' is a valid SSI device info ID

  @param  id  : device info ID

  @return   -1  : if 'id' does not exist in smgr_ssi_devinfo[]
          >= 0  : 'id' exists, and value corresponds to the column index
 */
/*=========================================================================*/
SMGR_STATIC int sns_smgr_ssi_get_devinfo_idx(uint16_t id)
{
  int i, rv = -1;

  for(i = 0; i < ARR_SIZE(smgr_ssi_devinfo); i++)
  {
    if(smgr_ssi_devinfo[i] == id)
    {
      rv = i;
      break;
    }
  }

  return rv;
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_next_devinfo_id

===========================================================================*/
/*!
  @brief  Gets the next DEVINFO group ID

  @param  curr_id  : the current DEVINFO ID
  @return   -1  : if 'id' is the last ID, or 'id' does not exist.
          >= 0  : the next CFG group ID
 */
/*=========================================================================*/
SMGR_STATIC int32_t sns_smgr_ssi_get_next_devinfo_id(uint16_t curr_id)
{
  int32_t next_id = -1;
  int idx = sns_smgr_ssi_get_devinfo_idx(curr_id);

  if(!SNS_SMGR_SSI_IS_LAST_DEVINFO(curr_id) && (idx >= 0) &&
     ((idx+1) < ARR_SIZE(smgr_ssi_devinfo)))
  {
    next_id = (int32_t) smgr_ssi_devinfo[idx+1];
  }

  return next_id;
}

/*===========================================================================

  FUNCTION:   sns_smgr_populate_cfg_from_devinfo

===========================================================================*/
/*!
  @brief Fills in a smgr_sensor_cfg_s based on registry device info

  @detail
  @param
   Id - Registry DEVINFO group ID.
   sensor_cfg_ptr - pointer to entry in smgr_sensor_cfg
   devinfo - pointer to sns_reg_ssi_devinfo_group_s
   Ix - index into devinfo
   num_sensors - number of sensors in the sensors_list
   sensors_list - Array of sensors supported by this device
   device_select - device_select value returned by probe function
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_populate_cfg_from_devinfo ( 
  uint16_t                           Id,
  smgr_sensor_cfg_s*                 sensor_cfg_ptr,
  const sns_reg_ssi_devinfo_group_s* devinfo,
  uint8_t                            Ix,
  uint32_t                           num_sensors,
  const sns_ddf_sensor_e*            sensors_list,
  uint8_t                            device_select)
{
  int i;
  sensor_cfg_ptr->drv_fn_ptr = sns_smgr_uuid_to_fn_ptr(devinfo->uuid_cfg[Ix].drvuuid );
  SNS_OS_MEMCOPY(sensor_cfg_ptr->uuid, devinfo->uuid_cfg[Ix].drvuuid, 
                 sizeof(sensor_cfg_ptr->uuid));
  sensor_cfg_ptr->off_to_idle_time =
    (uint16_t)sns_em_convert_usec_to_dspstick(devinfo->uuid_cfg[Ix].off_to_idle);
  sensor_cfg_ptr->idle_to_ready_time =
    (uint16_t)sns_em_convert_usec_to_dspstick(devinfo->uuid_cfg[Ix].idle_to_ready);
  sensor_cfg_ptr->bus_instance = devinfo->uuid_cfg[Ix].i2c_bus;
  sensor_cfg_ptr->device_select = device_select;
  if(  devinfo->uuid_cfg[Ix].reg_group_id == 0xFFFF ) {
    sensor_cfg_ptr->driver_reg_type = SNS_SMGR_REG_ITEM_TYPE_NONE;
    sensor_cfg_ptr->driver_reg_id = 0;
  }
  else
  {
    sensor_cfg_ptr->driver_reg_type = SNS_SMGR_REG_ITEM_TYPE_GROUP;
    sensor_cfg_ptr->driver_reg_id = devinfo->uuid_cfg[Ix].reg_group_id;
  }
  if( devinfo->uuid_cfg[Ix].cal_pri_group_id == 0xFFFF ) {
    sensor_cfg_ptr->primary_cal_reg_type = SNS_SMGR_REG_ITEM_TYPE_NONE;
    sensor_cfg_ptr->primary_cal_reg_id = 0;
  } else {
    sensor_cfg_ptr->primary_cal_reg_type = SNS_SMGR_REG_ITEM_TYPE_GROUP;
    sensor_cfg_ptr->primary_cal_reg_id = devinfo->uuid_cfg[Ix].cal_pri_group_id;
  }
  sensor_cfg_ptr->first_gpio = devinfo->uuid_cfg[Ix].gpio1;
  sensor_cfg_ptr->second_gpio = -1;
  sensor_cfg_ptr->bus_slave_addr = devinfo->uuid_cfg[Ix].i2c_address;
  sensor_cfg_ptr->sensitivity_default = devinfo->uuid_cfg[Ix].sensitivity_default;
  sensor_cfg_ptr->flags = devinfo->uuid_cfg[Ix].flags;

  if (devinfo->min_ver_no > 1)
  {
    sensor_cfg_ptr->vdd_rail   = devinfo->uuid_cfg[Ix].vdd_rail;
    sensor_cfg_ptr->vddio_rail = devinfo->uuid_cfg[Ix].vddio_rail;
  }

  sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR__NONE;
  sensor_cfg_ptr->data_types[1] = SNS_DDF_SENSOR__NONE;
  sensor_cfg_ptr->range_sensor = 0;
  switch( Id ) {
    case SNS_REG_GROUP_SSI_DEVINFO_ACCEL_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_ACCEL_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] == (int)SNS_DDF_SENSOR_ACCEL ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_ACCEL;
        }
        if( (int)sensors_list[i] == (int)SNS_DDF_SENSOR_TEMP) {
          sensor_cfg_ptr->data_types[1] = SNS_DDF_SENSOR_TEMP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_GYRO_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_GYRO_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] == (int)SNS_DDF_SENSOR_GYRO ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_GYRO;
        }
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_TEMP ) {
          sensor_cfg_ptr->data_types[1] = SNS_DDF_SENSOR_TEMP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_MAG_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_MAG_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_MAG ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_MAG;
        }
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_TEMP ) {
          sensor_cfg_ptr->data_types[1] = SNS_DDF_SENSOR_TEMP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_PROX_LIGHT_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_PROX_LIGHT_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_PROXIMITY ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_PROXIMITY;
        }
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_AMBIENT ) {
          sensor_cfg_ptr->range_sensor = 1;
          sensor_cfg_ptr->data_types[1] = SNS_DDF_SENSOR_AMBIENT;
        }
      }
      SNS_SMGR_PRINTF1(HIGH, "range_sensor=%d", sensor_cfg_ptr->range_sensor);
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_PRESSURE_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_PRESSURE_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_PRESSURE ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_PRESSURE;
        }
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_TEMP ) {
          sensor_cfg_ptr->data_types[1] = SNS_DDF_SENSOR_TEMP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_IR_GESTURE_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_IR_GESTURE_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_IR_GESTURE ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_IR_GESTURE;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_TAP_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_TAP_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_DOUBLETAP ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_DOUBLETAP;
        }
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_SINGLETAP ) {
          sensor_cfg_ptr->data_types[1] = SNS_DDF_SENSOR_SINGLETAP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_HUMIDITY_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_HUMIDITY_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_HUMIDITY ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_HUMIDITY;
        }
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_AMBIENT_TEMP ) {
          sensor_cfg_ptr->data_types[1] = SNS_DDF_SENSOR_AMBIENT_TEMP;
        }
      }
      break;

    case SNS_REG_GROUP_SSI_DEVINFO_RGB_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_RGB_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_RGB ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_RGB;
        }
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_CT_C ) {
          sensor_cfg_ptr->data_types[1] = SNS_DDF_SENSOR_CT_C;
        }
      }
      break;

    case SNS_REG_GROUP_SSI_DEVINFO_SAR_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_SAR_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_SAR ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_SAR;
        }
      }
      break;

    case SNS_REG_GROUP_SSI_DEVINFO_HALL_EFFECT_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_HALL_EFFECT_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( (int)sensors_list[i] ==  (int)SNS_DDF_SENSOR_HALL_EFFECT ) {
          sensor_cfg_ptr->data_types[0] = SNS_DDF_SENSOR_HALL_EFFECT;
        }
      }
      break;

    default:
      break;
  }

}

/*===========================================================================

  FUNCTION:   sns_smgr_parse_reg_devinfo_resp

===========================================================================*/
/*!
  @brief Process SSI devinfo response for sensor autodetect

  @detail
  @param
   Id - Group ID or single item ID
   devinfo - pointer to sns_reg_ssi_devinfo_group_s
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_parse_reg_devinfo_resp(
  uint16_t                           Id,
  const sns_reg_ssi_devinfo_group_s* devinfo)
{
  int i;
  uint16_t devinfo_idx;

  devinfo_idx = sns_smgr_ssi_get_devinfo_idx(Id);

  if ( devinfo->min_ver_no < 1 ||
       SNS_SMGR_NUM_SENSORS_DEFINED <= devinfo_idx )
  {
    SNS_SMGR_PRINTF2(
      ERROR, "ssi: wrong min_ver_no: %u, devinfo_idx: %u", 
      devinfo->min_ver_no, devinfo_idx);
    return;
  }

  for ( i = 0; i < SNS_REG_SSI_DEVINFO_NUM_CFGS &&
               i < devinfo->num_uuid_dev_info_valid; i++ )
  {
    sns_ddf_driver_if_s    *drv_fn_ptr;
    sns_ddf_device_access_s dev_access;
    uint32_t                num_sensors;
    sns_ddf_sensor_e       *sensor_list;
    sns_ddf_memhandler_s    memhandler;
    sns_ddf_status_e        status;
    smgr_sensor_cfg_s      *sensor_cfg_ptr;
    drv_fn_ptr = sns_smgr_uuid_to_fn_ptr(devinfo->uuid_cfg[i].drvuuid );
    if( drv_fn_ptr == NULL )
    {
      SNS_SMGR_PRINTF2(ERROR, "ssi: No UUID for devinfo_idx[i]: %u[%u]", devinfo_idx, i);
      continue;
    }

    if( drv_fn_ptr->probe != NULL )
    {
      sns_reg_ssi_smgr_cfg_group_drvcfg_s drv_cfg_ptr;
            
      /* Read power rail info from devinfo->uuid_cfg[i]l */
      if ( devinfo->min_ver_no > 1 )
      {
        drv_cfg_ptr.vdd_rail   = devinfo->uuid_cfg[i].vdd_rail;
        drv_cfg_ptr.vddio_rail = devinfo->uuid_cfg[i].vddio_rail;
      }
      else
      {
        SNS_SMGR_PRINTF0(HIGH, "ssi autodetect: pwrrails not specified or devInfo min ver is not updated to 2 or more, using defaults");
        drv_cfg_ptr.vdd_rail = (&smgr_sensor_cfg[devinfo_idx])->vdd_rail;
        drv_cfg_ptr.vddio_rail = (&smgr_sensor_cfg[devinfo_idx])->vddio_rail;
      }

      SNS_SMGR_PRINTF2(MED, "ssi autodetect: pwrrails vdd %d, vddio %d", drv_cfg_ptr.vdd_rail, drv_cfg_ptr.vddio_rail);
      
      /* Init rails before powering them ON */
      sns_pm_pwrrail_init(drv_cfg_ptr.vdd_rail);
      sns_pm_pwrrail_init(drv_cfg_ptr.vddio_rail);
     
      bool qup_state = sns_hw_qup_clck_status();

      drv_cfg_ptr.i2c_bus = devinfo->uuid_cfg[i].i2c_bus;

      if ( SNS_DDF_COMM_BUS_TYPE_IS_I2C( drv_cfg_ptr.i2c_bus ) )
      {
        /* Populate bus instance in SMGR's HW state */
        sns_get_i2c_bus_instance(&drv_cfg_ptr);
      }

      /* Force Turn on power rail and I2C clock */
      sns_hw_power_rail_force_config(drv_cfg_ptr.vdd_rail, SNS_PWR_RAIL_ON_NPM);
      sns_hw_power_rail_force_config(drv_cfg_ptr.vddio_rail, SNS_PWR_RAIL_ON_NPM);
      
      sns_hw_set_qup_clk(true);
      // Vote for pnoc ON here in case there was a new bus added to SMGR HW
      sns_hw_pnoc_vote(true);

      SNS_SMGR_PRINTF1(MED, "ssi autodetect: dev info idx %d", devinfo_idx);
      if ( SNS_REG_GROUP_SSI_DEVINFO_MAG_V02 == Id )
      {
        sns_hw_control_reset_pin(SNS_PWR_RAIL_ON_NPM);
      }

      SMGR_DELAY_US(devinfo->uuid_cfg[i].off_to_idle);


      //---------- Init bus configuration for communication interface ----------
      dev_access.device_select   = 0;
      dev_access.port_config.bus = SNS_DDF_BUS_NONE;
      dev_access.first_gpio      = (uint32_t)devinfo->uuid_cfg[i].gpio1;
      dev_access.second_gpio     = 0;

      status = sns_ddf_comm_bus_init_cfg( &dev_access,
                                          devinfo->uuid_cfg[i].i2c_bus,
                                          devinfo->uuid_cfg[i].i2c_address );
      if (  SNS_DDF_SUCCESS != status )
      {
        SNS_SMGR_PRINTF1( HIGH, "ddf probe init bus failed status=%d", status );
        continue;
      }
      //------------------------------------------------------------------------

      sns_ddf_memhandler_init( &memhandler );
      SNS_SMGR_PRINTF2(LOW, "ssi: probing devinfo_idx[i]: %u[%u]", devinfo_idx, i);
      SNS_SMGR_PRINTF3(LOW, "ssi: bus_instance:%u gpio1:%u slave_addr:0x%x",
                       devinfo->uuid_cfg[i].i2c_bus, devinfo->uuid_cfg[i].gpio1,
                       devinfo->uuid_cfg[i].i2c_address );

      status = drv_fn_ptr->probe( &dev_access, &memhandler,
                                  &num_sensors, &sensor_list );

      sns_ddf_comm_bus_free_cfg( &dev_access ); //clean mem after bus configuration

      if( status == SNS_DDF_SUCCESS  && num_sensors != 0 )
      {
        SNS_SMGR_PRINTF2(HIGH, "ssi: devinfo_idx[i]: %u[%u] probe success", devinfo_idx, i);
        /* Populate the smgr_sensor_cfg[] array in the same order as the DEVINFO
           entries in the registry */
        sensor_cfg_ptr = &smgr_sensor_cfg[devinfo_idx];
        sns_smgr_populate_cfg_from_devinfo( Id, sensor_cfg_ptr, devinfo, i,
                                            num_sensors, sensor_list,
                                            dev_access.device_select );
        sns_ddf_memhandler_free( &memhandler );
        /* Only one sensor per type is currently supported, so return here now
           that one has been found */

        /* Force turn off power rail and I2C clock state */
        sns_hw_power_rail_force_config(drv_cfg_ptr.vdd_rail, SNS_PWR_RAIL_OFF);
        sns_hw_power_rail_force_config(drv_cfg_ptr.vddio_rail, SNS_PWR_RAIL_OFF);

        sns_hw_set_qup_clk(qup_state);

        return;
      }
      SNS_SMGR_PRINTF3(HIGH, "ssi: devinfo_idx[i]: %u[%u] probe failed error=%d",
                       devinfo_idx, i, status);

      sns_ddf_memhandler_free( &memhandler );

      /* Turn off appr rail */
      sns_hw_power_rail_force_config(drv_cfg_ptr.vdd_rail, SNS_PWR_RAIL_OFF);
      sns_hw_power_rail_force_config(drv_cfg_ptr.vddio_rail, SNS_PWR_RAIL_OFF);

      sns_hw_set_qup_clk(qup_state);

    } /* end if( drv_fn_ptr->probe != NULL ) block */
    else
    {
      /* Device does not support the probe function. Assume it's connected */
      const uint8_t default_device_select = 0;
      sns_ddf_sensor_e sensor_type[SMGR_MAX_DATA_TYPES_PER_DEVICE];

      SNS_SMGR_PRINTF2(MED, "ssi: devinfo_idx[i]: %u[%u] probe function not supported", devinfo_idx, i);

      sensor_cfg_ptr = &smgr_sensor_cfg[devinfo_idx];
      num_sensors = 1;
      sensor_list = sensor_type;
      switch( Id )
      {
        case SNS_REG_GROUP_SSI_DEVINFO_ACCEL_V02:
          sensor_type[0] = SNS_DDF_SENSOR_ACCEL;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_GYRO_V02:
          sensor_type[0] = SNS_DDF_SENSOR_GYRO;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_MAG_V02:
          sensor_type[0] = SNS_DDF_SENSOR_MAG;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_PROX_LIGHT_V02:
          num_sensors = 2;
          sensor_type[0] = SNS_DDF_SENSOR_PROXIMITY;
          sensor_type[1] = SNS_DDF_SENSOR_AMBIENT;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_PRESSURE_V02:
          sensor_type[0] = SNS_DDF_SENSOR_PRESSURE;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_IR_GESTURE_V02:
          num_sensors = 1;
          sensor_type[0] = SNS_DDF_SENSOR_IR_GESTURE;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_TAP_V02:
          sensor_type[0] = SNS_DDF_SENSOR_DOUBLETAP;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_HUMIDITY_V02:
          sensor_type[0] = SNS_DDF_SENSOR_HUMIDITY;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_RGB_V02:
          sensor_type[0] = SNS_DDF_SENSOR_RGB;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_SAR_V02:
          num_sensors = 1;
          sensor_type[0] = SNS_DDF_SENSOR_SAR;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_HALL_EFFECT_V02:
          num_sensors = 1;
          sensor_type[0] = SNS_DDF_SENSOR_HALL_EFFECT;
          break;

        default:
          num_sensors = 0;
          break;
      }
      sns_smgr_populate_cfg_from_devinfo( Id, sensor_cfg_ptr, devinfo, i,
                                          num_sensors, sensor_list,
                                          default_device_select );

      /* Only one sensor per type is currently supported, so return here now
         that one has been found */
      return;
    } /* end if( drv_fn_ptr->probe != NULL ) else block*/
  } /* end for */
}

/*===========================================================================

  FUNCTION:   sns_smgr_populate_ssi_cfg_from_smgr_cfg

===========================================================================*/
/*!
  @brief Fills in a sns_reg_ssi_smgr_cfg_group_s from a SMGR configuration

  @detail
  @param
   ssi_cfg_ptr - Pointer to an SSI SMGR configuration
   sensor_cfg_ptr - pointer to entry in smgr_sensor_cfg
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_populate_ssi_cfg_from_smgr_cfg(
  sns_reg_ssi_smgr_cfg_group_s *ssi_cfg_ptr,
  smgr_sensor_cfg_s            *sensors_cfg_ptr,
  int                          cfg_index)
{
  int i, os;
  const uint8_t *uuid;

  SNS_OS_MEMSET(ssi_cfg_ptr,
                0,
                sizeof(sns_reg_ssi_smgr_cfg_group_s));

  ssi_cfg_ptr->maj_ver_no = 1;
  ssi_cfg_ptr->min_ver_no = 2;
  ssi_cfg_ptr->reserved1 = 0;
  ssi_cfg_ptr->reserved2 = 0;
  ssi_cfg_ptr->reserved3 = 0;
  ssi_cfg_ptr->reserved4 = 0;


  os = 0;
  i = cfg_index * SNS_REG_SSI_SMGR_CFG_NUM_SENSORS;

  while(  os < SNS_REG_SSI_SMGR_CFG_NUM_SENSORS &&
          i < SNS_SMGR_NUM_SENSORS_DEFINED)
  {
    sns_reg_ssi_smgr_cfg_group_drvcfg_s *drv_ptr = &ssi_cfg_ptr->drv_cfg[os];

    uuid = sns_smgr_fn_ptr_to_uuid(sensors_cfg_ptr[i].drv_fn_ptr);
    if( uuid != NULL )
    {
      SNS_OS_MEMCOPY(drv_ptr->drvuuid, uuid, 16);
      drv_ptr->off_to_idle =
        sns_em_convert_dspstick_to_usec( sensors_cfg_ptr[i].off_to_idle_time );
      drv_ptr->idle_to_ready =
        sns_em_convert_dspstick_to_usec( sensors_cfg_ptr[i].idle_to_ready_time );
      drv_ptr->i2c_bus = sensors_cfg_ptr[i].bus_instance;
      if( sensors_cfg_ptr[i].driver_reg_type == SNS_SMGR_REG_ITEM_TYPE_NONE ) {
        drv_ptr->reg_group_id = 0xFFFF;
      } else {
        drv_ptr->reg_group_id = sensors_cfg_ptr[i].driver_reg_id;
      }
      if( sensors_cfg_ptr[i].primary_cal_reg_type == SNS_SMGR_REG_ITEM_TYPE_NONE ) {
        drv_ptr->cal_pri_group_id = 0xFFFF;
      } else {
        drv_ptr->cal_pri_group_id = sensors_cfg_ptr[i].primary_cal_reg_id;
      }
      drv_ptr->gpio1 = sensors_cfg_ptr[i].first_gpio;
      drv_ptr->gpio2 = sensors_cfg_ptr[i].second_gpio;
      drv_ptr->sensor_id = sensors_cfg_ptr[i].sensor_id;
      drv_ptr->i2c_address = sensors_cfg_ptr[i].bus_slave_addr;
      drv_ptr->data_type1 = sensors_cfg_ptr[i].data_types[0];
      drv_ptr->data_type2 = sensors_cfg_ptr[i].data_types[1];
      drv_ptr->related_sensor_index = -1;
      drv_ptr->sensitivity_default = sensors_cfg_ptr[i].sensitivity_default;
      drv_ptr->flags = sensors_cfg_ptr[i].flags;
      drv_ptr->device_select = sensors_cfg_ptr[i].device_select;
      /* AGK:  was drv_tr->reserved2 */
      drv_ptr->vdd_rail = sensors_cfg_ptr[i].vdd_rail;
      drv_ptr->vddio_rail = sensors_cfg_ptr[i].vddio_rail;      
  
      drv_ptr->reserved3 = 0;
    }

    os++;
    i++;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_sensor_dep_reg_data

===========================================================================*/
/*!
  @brief Configures default values for sensor dependent registry items after
   autodetect is complete
  @detail
  @param
   cfg_group_ptr - Pointer to a sns_reg_ssi_sensor_dep_reg_group_s struct
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_sensor_dep_reg_data(
  sns_reg_ssi_sensor_dep_reg_group_s * cfg_group_ptr)
{
  uint8_t sensor_type = cfg_group_ptr->sensor_type - 1;
  uint8_t i, j;
  if( sensor_type < SNS_SMGR_NUM_SENSORS_DEFINED )
  {
    // Get autodetected sensor of this type
    const uint8_t * uuid = sns_smgr_fn_ptr_to_uuid(smgr_sensor_cfg[sensor_type].drv_fn_ptr);
    if( uuid != NULL )
    {
      // Find default values for this sensor
      for( i = 0; i < SNS_REG_MAX_SENSORS_WITH_DEP_REG_ITEMS; ++i )
      {
        if( SNS_OS_MEMCMP( cfg_group_ptr->uuid_reg_cfg[i].drvuuid, uuid, 
                           sizeof(cfg_group_ptr->uuid_reg_cfg[i].drvuuid) ) == 0 )
        {
          for( j = 0; j < SNS_REG_MAX_SENSOR_DEP_REG_ITEMS; ++j )
          {
            if( cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].reg_item_id != 0 &&
                cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].size != 0 )
            {
              // Update reg item with new value
              sns_smgr_update_reg_data( 
                cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].reg_item_id,
                SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].size,
                (uint8_t*)cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].value );
            }
          }
        }
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_reg_devinfo

===========================================================================*/
/*!
  @brief Process a retrieved registry devinfo group.

  @detail
  @param
   Type - Group or Single item registry
   Id - Group ID or single item ID
   Length - bytes
   data_ptr - pointer to received data
   sns_resp - response error
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_reg_devinfo( 
  sns_smgr_RegItemType_e  item_type,
  uint16_t                item_id,
  uint32_t                item_len, 
  uint8_t*                data_ptr, 
  sns_common_resp_s_v01   sns_resp)
{
  uint8_t         ix;
  sns_err_code_e  err;

  if( item_id == 0 )
  {
    item_id = sns_smgr.last_received_reg_group_id;
  }

  if(sns_resp.sns_err_t == SENSOR1_SUCCESS)
  {
    sns_smgr_parse_reg_devinfo_resp( item_id, (sns_reg_ssi_devinfo_group_s*)data_ptr );
  }
  else
  {
    SNS_SMGR_PRINTF3(HIGH, "ssi: resp_err=%u, id=%u, last_id=%u", 
                     sns_resp.sns_err_t, item_id, sns_smgr.last_received_reg_group_id);
  }

  if( SNS_SMGR_SSI_IS_LAST_DEVINFO(item_id) )
  {
    sns_reg_ssi_smgr_cfg_group_s ssi_cfg;

    SNS_SMGR_PRINTF0(HIGH, "ssi: populating SSI config.");
      
    for( ix = 0; ix < ARR_SIZE(smgr_ssi_cfg); ix++ )
    {
      sns_smgr_populate_ssi_cfg_from_smgr_cfg( &ssi_cfg,
                                               smgr_sensor_cfg,
                                               ix );

      err = sns_smgr_update_reg_data( (uint16_t) sns_smgr_ssi_get_cfg_id(ix),
                                      SNS_SMGR_REG_ITEM_TYPE_GROUP,
                                      sizeof( ssi_cfg ), (uint8_t*)&ssi_cfg );
      SNS_SMGR_PRINTF3(MED, "ssi: populating SSI config. ix:%u cfg_id:%u err:%u", 
                       ix, (unsigned)sns_smgr_ssi_get_cfg_id(ix), (unsigned)err);
      sns_hw_update_ssi_reg_items(sns_smgr_ssi_get_cfg_id(ix), (uint8_t*)&ssi_cfg);
    }
    
    SNS_SMGR_PRINTF0(LOW, "ssi: last devinfo. state set to ALL_INIT_AUTODETECT_DONE");
    sns_smgr.all_init_state = SENSOR_ALL_INIT_AUTODETECT_DONE;
    err = sns_smgr_req_reg_data(SNS_REG_GROUP_SSI_SENSOR_DEP_CFG0_V02, 
                                SNS_SMGR_REG_ITEM_TYPE_GROUP);
    sns_smgr.last_requested_sensor_dep_reg_group_id = 
      SNS_REG_GROUP_SSI_SENSOR_DEP_CFG0_V02;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_next_cfg_id

===========================================================================*/
/*!
  @brief  Gets the next CFG group ID

  @param  curr_id  : the current CFG ID
  @return    -1 : if 'id' is the last CFG ID, or 'id' does not exist.
          >= 0  : the next CFG group ID
 */
/*=========================================================================*/
SMGR_STATIC int32_t sns_smgr_ssi_get_next_cfg_id(uint16_t curr_id)
{
  int32_t next_id = -1;
  int idx = sns_smgr_ssi_get_cfg_idx(curr_id);

  if( (idx>=0) && ((idx+1) < ARR_SIZE(smgr_ssi_cfg) ) )
  {
    next_id = (int32_t) smgr_ssi_cfg[idx+1];
  }

  return next_id;
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_reg_ssi_config

===========================================================================*/
/*!
  @brief Process an SSI SMGR Config registry group.

  @detail
  @param
   Type - Group or Single item registry
   Id - Group ID or single item ID
   Length - bytes
   data_ptr - pointer to received data
   sns_resp - response error
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_reg_ssi_config( 
  sns_smgr_RegItemType_e item_type, 
  uint16_t               item_id,
  uint32_t               item_len, 
  uint8_t*               data_ptr, 
  sns_common_resp_s_v01  sns_resp)
{
  int                           cfg_idx;
  uint16_t                      curr_id;
  uint32_t                      ix;
  sns_reg_ssi_smgr_cfg_group_s* cfg_group_ptr = (sns_reg_ssi_smgr_cfg_group_s*)data_ptr;
  smgr_sensor_cfg_s           * sensor_cfg_ptr;

  static bool                   valid_cfg = false;

  if( item_id == 0 )
  {
    item_id = sns_smgr.last_received_reg_group_id;
  }

  cfg_idx = sns_smgr_ssi_get_cfg_idx(item_id);

  SNS_SMGR_PRINTF2(HIGH, "ssi: proc ssi id %i cfg %i", item_id, cfg_idx);
  if(sns_resp.sns_err_t != SENSOR1_SUCCESS)
  {
    SNS_SMGR_PRINTF3(LOW, "ssi: resp_err=%u, id=%u, last_id=%u", 
                     sns_resp.sns_err_t, item_id, sns_smgr.last_received_reg_group_id);
    cfg_idx = sns_smgr_ssi_get_cfg_idx(sns_smgr.last_received_reg_group_id);
    if ( sns_smgr_ssi_is_last_cfg(sns_smgr.last_received_reg_group_id) )
    {
      SNS_SMGR_PRINTF0(LOW, "ssi: Setting init state to ALL_INIT_CONFIGURED");
      sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
    }
  }
  else if ( (cfg_group_ptr->maj_ver_no != 1) && !valid_cfg )
  {
    /* Only use the configuration if the major version is 1. 
    Otherwise autodetect sensors */
    SNS_SMGR_PRINTF1(MED, "ssi: proc ssi maj ver no = %i", cfg_group_ptr->maj_ver_no);
    sns_smgr.all_init_state = SENSOR_ALL_INIT_WAITING_AUTODETECT;

    curr_id = SNS_SMGR_SSI_GET_FIRST_DEVINFO_ID();
    (void) sns_smgr_req_reg_data(curr_id, SNS_SMGR_REG_ITEM_TYPE_GROUP);
    do
    {
      curr_id = (uint16_t) sns_smgr_ssi_get_next_devinfo_id(curr_id);
      (void) sns_smgr_req_reg_data(curr_id, SNS_SMGR_REG_ITEM_TYPE_GROUP);
    } while( ! SNS_SMGR_SSI_IS_LAST_DEVINFO(curr_id) );
  }
  else
  {
    for ( ix = 0; smgr_sensor_cfg_cnt < ARR_SIZE(smgr_sensor_cfg) &&
                  ix < ARR_SIZE(cfg_group_ptr->drv_cfg); ix++ )
    {
      sns_reg_ssi_smgr_cfg_group_drvcfg_s* drvcfg_ptr = &cfg_group_ptr->drv_cfg[ix];
      sensor_cfg_ptr = &smgr_sensor_cfg[smgr_sensor_cfg_cnt++];

      sensor_cfg_ptr->drv_fn_ptr = sns_smgr_uuid_to_fn_ptr(drvcfg_ptr->drvuuid );
      if ( sensor_cfg_ptr->drv_fn_ptr == NULL )
      {
        sensor_cfg_ptr->vdd_rail   = SNS_SMGR_INVALID_PWRRAIL;
        sensor_cfg_ptr->vddio_rail = SNS_SMGR_INVALID_PWRRAIL;
        continue;
      }

      if ( SNS_DDF_COMM_BUS_TYPE_IS_I2C(drvcfg_ptr->i2c_bus) )
      {
        sns_get_i2c_bus_instance(drvcfg_ptr);
      }

      SNS_OS_MEMCOPY(sensor_cfg_ptr->uuid, drvcfg_ptr->drvuuid, 
                     sizeof(sensor_cfg_ptr->uuid));
      sensor_cfg_ptr->off_to_idle_time = 
        (uint16_t)sns_em_convert_usec_to_dspstick(drvcfg_ptr->off_to_idle);
      sensor_cfg_ptr->idle_to_ready_time = 
        (uint16_t)sns_em_convert_usec_to_dspstick(drvcfg_ptr->idle_to_ready);
      sensor_cfg_ptr->bus_instance = drvcfg_ptr->i2c_bus;
      if ( drvcfg_ptr->reg_group_id == 0xFFFF ) 
      {
        sensor_cfg_ptr->driver_reg_type = SNS_SMGR_REG_ITEM_TYPE_NONE;
        sensor_cfg_ptr->driver_reg_id = 0;
      }
      else 
      {
        sensor_cfg_ptr->driver_reg_type = SNS_SMGR_REG_ITEM_TYPE_GROUP;
        sensor_cfg_ptr->driver_reg_id = drvcfg_ptr->reg_group_id;
      }

      if ( drvcfg_ptr->cal_pri_group_id == 0xFFFF )
      {
        sensor_cfg_ptr->primary_cal_reg_type = SNS_SMGR_REG_ITEM_TYPE_NONE;
        sensor_cfg_ptr->primary_cal_reg_id = 0;
      }
      else
      {
        sensor_cfg_ptr->primary_cal_reg_type = SNS_SMGR_REG_ITEM_TYPE_GROUP;
        sensor_cfg_ptr->primary_cal_reg_id = drvcfg_ptr->cal_pri_group_id;
      }
      sensor_cfg_ptr->first_gpio          = drvcfg_ptr->gpio1;
      sensor_cfg_ptr->second_gpio         = drvcfg_ptr->gpio2;
      sensor_cfg_ptr->sensor_id           = drvcfg_ptr->sensor_id;
      sensor_cfg_ptr->bus_slave_addr      = drvcfg_ptr->i2c_address;
      sensor_cfg_ptr->data_types[0]       = drvcfg_ptr->data_type1;
      sensor_cfg_ptr->data_types[1]       = drvcfg_ptr->data_type2;
      sensor_cfg_ptr->sensitivity_default = drvcfg_ptr->sensitivity_default;
      sensor_cfg_ptr->flags               = drvcfg_ptr->flags;

      /* populate sensor_cfg_ptr with power rail */
      /* MSB represents the type - LVS:0, LDO:1, rest of the bits represent rail num */
      if ( cfg_group_ptr->min_ver_no > 1 )
      {
        sensor_cfg_ptr->vdd_rail   = drvcfg_ptr->vdd_rail;
        sensor_cfg_ptr->vddio_rail = drvcfg_ptr->vddio_rail;
      }

      SNS_SMGR_PRINTF4(
        LOW,"ssi: idx=%2u id=%3u vdd=%3d/%3d", 
        smgr_sensor_cfg_cnt-1, sensor_cfg_ptr->sensor_id, 
        sensor_cfg_ptr->vdd_rail, sensor_cfg_ptr->vddio_rail);
      
      if ( cfg_group_ptr->min_ver_no > 0 )
      {
        sensor_cfg_ptr->device_select = drvcfg_ptr->device_select;
      }
      if( sensor_cfg_ptr->drv_fn_ptr != NULL )
      {
        valid_cfg = true;
      }
    }

    if ( !sns_smgr_ssi_is_last_cfg(item_id) )
    {
      curr_id = (uint16_t) sns_smgr_ssi_get_next_cfg_id(item_id);
      (void) sns_smgr_req_reg_data(curr_id, SNS_SMGR_REG_ITEM_TYPE_GROUP);
    }
    else
    {
      SNS_SMGR_PRINTF0(HIGH, "reg_ssi_config - state to ALL_INIT_CONFIGURED");
      sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
    }
  }  // else (no auto-detect)
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_reg_data

===========================================================================*/
/*!
  @brief Receive registry data. Find where it was requested and apply it.

  @detail
  @param
   Type - Group or Single item registry
   Id - Group ID or single item ID
   Length - bytes
   data_ptr - pointer to received data
   sns_resp - response error
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_reg_data(
  sns_smgr_RegItemType_e  item_type, 
  uint16_t                item_id,
  uint32_t                item_len, 
  uint8_t*                data_ptr, 
  sns_common_resp_s_v01   sns_resp)
{
  sns_smgr_sensor_s*        sensor_ptr;
  const smgr_sensor_cfg_s*  sensor_cfg_ptr;
  uint32_t                  ix, iy;

  SNS_SMGR_PRINTF1(LOW, "proc reg data: Id:%u", item_id );

  if ( (sns_smgr_ssi_get_devinfo_idx(item_id) != -1) ||
       ((sns_resp.sns_err_t != SENSOR1_SUCCESS) && 
        (sns_smgr_ssi_get_devinfo_idx(sns_smgr.last_received_reg_group_id) != -1)) )
  {
    SNS_SMGR_PRINTF0(MED, "calling autodetect" );
    sns_smgr_process_reg_devinfo(item_type, item_id, item_len, data_ptr, sns_resp);
    return;
  }

  if ( (sns_smgr_ssi_get_cfg_idx(item_id) != -1) ||
       ((sns_resp.sns_err_t != SENSOR1_SUCCESS) && 
        (sns_smgr_ssi_get_cfg_idx(sns_smgr.last_received_reg_group_id) != -1)) )
  {
    sns_smgr_process_reg_ssi_config(item_type, item_id, item_len, data_ptr, sns_resp);
    return;
  }

  if ( sns_resp.sns_err_t != SENSOR1_SUCCESS )
  {
    SNS_SMGR_PRINTF3(ERROR, "resp_err=%u, id=%u, last_id=%u", 
                     sns_resp.sns_err_t, item_id, sns_smgr.last_received_reg_group_id);

    if ( item_id == sns_smgr.last_requested_sensor_dep_reg_group_id )
    {
      SNS_SMGR_PRINTF1(ERROR, "invalid DEP reg id=%u", 
                       sns_smgr.last_requested_sensor_dep_reg_group_id);
    }
    else
    {
      SNS_SMGR_PRINTF1(ERROR, "Unknown reg id=%u", sns_smgr.last_received_reg_group_id);
    }
  }
  else if ( item_id == sns_smgr.last_requested_sensor_dep_reg_group_id )
  {
    sns_reg_ssi_sensor_dep_reg_group_s* cfg_group_ptr = 
      (sns_reg_ssi_sensor_dep_reg_group_s*)data_ptr;

    SNS_SMGR_PRINTF1(LOW, "SMGR processing sensor dep reg item group %d", item_id);

    if ( cfg_group_ptr->ver_no == 0 )
    {
      SNS_SMGR_PRINTF0(HIGH, "Registry group is invalid");
      sns_smgr.last_requested_sensor_dep_reg_group_id = 0xFFFF;
      sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
    }
    else
    {
      sns_smgr_process_sensor_dep_reg_data( cfg_group_ptr );
      if ( cfg_group_ptr->next_group_id != 0 )
      {
        sns_smgr_req_reg_data(cfg_group_ptr->next_group_id, 
                              SNS_SMGR_REG_ITEM_TYPE_GROUP);
        sns_smgr.last_requested_sensor_dep_reg_group_id = cfg_group_ptr->next_group_id;
      }
      else
      {
        SNS_SMGR_PRINTF0(HIGH, "proc_reg_data - state to ALL_INIT_CONFIGURED");
        sns_smgr.last_requested_sensor_dep_reg_group_id = 0xFFFF;
        sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
      }
    }
  }
  else if ( (item_id == SNS_REG_GROUP_SSI_GPIO_CFG_V02) &&
            (item_type == SNS_SMGR_REG_ITEM_TYPE_GROUP) )
  {
    sns_hw_update_ssi_reg_items(item_id, data_ptr);
  }
  else
  {
    for ( ix = 0; ix < SNS_SMGR_NUM_SENSORS_DEFINED; ix++ )
    {
      sensor_ptr = &sns_smgr.sensor[ix];
      sensor_cfg_ptr = sensor_ptr->const_ptr;

      /* skip any non-existent sensor */
      if ( NULL == SMGR_DRV_FN_PTR(sensor_ptr) )
      {
        continue;
      }

      if ( sensor_cfg_ptr->driver_reg_type == item_type &&
           sensor_cfg_ptr->driver_reg_id == item_id &&
           SENSOR_INIT_WAITING_REG == sensor_ptr->init_state)
      {
        /* Need to copy data for driver because response message will be freed
           before init can happen */
        if ( NULL != (sensor_ptr->reg_item_param.data = 
                      SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, item_len )))
        {
          for ( iy = 0; iy < item_len; iy++ )
          {
            sensor_ptr->reg_item_param.data[iy] = data_ptr[iy];
          }
          sensor_ptr->reg_item_param.nvitem_grp = item_type;
          sensor_ptr->reg_item_param.nvitem_id = item_id;
          sensor_ptr->reg_item_param.status = SNS_DDF_SUCCESS;
          sensor_ptr->reg_item_param.data_len = item_len;
          sensor_ptr->init_state = SENSOR_INIT_REG_READY;
        }
        else
        {
          /* No memory. At timeout we will discover registry request failed,
             thus init will fail. Very unlikely */
          SNS_SMGR_PRINTF2(
            HIGH, "ix=%d reg_id=%u", ix, (unsigned)sensor_cfg_ptr->driver_reg_id);
        }
      }
      else if ( (sensor_cfg_ptr->primary_cal_reg_type == item_type) &&
                (sensor_cfg_ptr->primary_cal_reg_id == item_id) &&
                (sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01] != NULL) &&
                (sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01]->
				all_cal_ptr != NULL) )
      {
        q16_t *reg_data_ptr = (q16_t *)data_ptr;
        sns_smgr_all_cal_s* all_cal_ptr = 
          sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01]->all_cal_ptr;
        SNS_ASSERT(item_len >= (sizeof(q16_t) * SNS_SMGR_SENSOR_DIMENSION_V01 * 2));    /* scale factors and biases */

        if ( item_id == SNS_REG_SCM_GROUP_MAG_FAC_CAL_PARAMS_V02 )
        {
          sns_smgr_cal_s* cal_db_ptr;
          sns_reg_mag_fac_cal_params_data_group_s* mag_reg_data_ptr =
            (sns_reg_mag_fac_cal_params_data_group_s*)data_ptr;

          /* Reset all autocal data */
          all_cal_ptr->auto_cal.used = false;
          sns_smgr_load_default_cal( &(all_cal_ptr->auto_cal) );

          cal_db_ptr = &all_cal_ptr->factory_cal;
          cal_db_ptr->used = true;
          if ( mag_reg_data_ptr->bias_valid )
          {
            cal_db_ptr->zero_bias[0] = mag_reg_data_ptr->x_fac_bias;
            cal_db_ptr->zero_bias[1] = mag_reg_data_ptr->y_fac_bias;
            cal_db_ptr->zero_bias[2] = mag_reg_data_ptr->z_fac_bias;

            SNS_SMGR_PRINTF3(
              LOW, "mag fac cal zero_bias [0]:%d, [1]:%d, [2]:%d",
              (int)cal_db_ptr->zero_bias[0],
              (int)cal_db_ptr->zero_bias[1],
              (int)cal_db_ptr->zero_bias[2]);
          }

          if ( mag_reg_data_ptr->cal_mat_valid )
          {
            cal_db_ptr->compensation_matrix_valid = true;

            cal_db_ptr->compensation_matrix[0] = mag_reg_data_ptr->compensation_matrix[0][0];
            cal_db_ptr->compensation_matrix[1] = mag_reg_data_ptr->compensation_matrix[0][1];
            cal_db_ptr->compensation_matrix[2] = mag_reg_data_ptr->compensation_matrix[0][2];
            cal_db_ptr->compensation_matrix[3] = mag_reg_data_ptr->compensation_matrix[1][0];
            cal_db_ptr->compensation_matrix[4] = mag_reg_data_ptr->compensation_matrix[1][1];
            cal_db_ptr->compensation_matrix[5] = mag_reg_data_ptr->compensation_matrix[1][2];
            cal_db_ptr->compensation_matrix[6] = mag_reg_data_ptr->compensation_matrix[2][0];
            cal_db_ptr->compensation_matrix[7] = mag_reg_data_ptr->compensation_matrix[2][1];
            cal_db_ptr->compensation_matrix[8] = mag_reg_data_ptr->compensation_matrix[2][2];

            SNS_SMGR_PRINTF3(
              LOW, "mag fac cal compensation_matrix: [0]:%u, [1]:%u, [2]:%u",
              (unsigned)cal_db_ptr->compensation_matrix[0],
              (unsigned)cal_db_ptr->compensation_matrix[1],
              (unsigned)cal_db_ptr->compensation_matrix[2]);
            SNS_SMGR_PRINTF3(
              LOW, "mag fac cal compensation_matrix: [0]:%u, [1]:%u, [2]:%u",
              (unsigned)cal_db_ptr->compensation_matrix[3],
              (unsigned)cal_db_ptr->compensation_matrix[4],
              (unsigned)cal_db_ptr->compensation_matrix[5]);
            SNS_SMGR_PRINTF3(
              LOW, "mag fac cal compensation_matrix: [0]:%u, [1]:%u, [2]:%u",
              (unsigned)cal_db_ptr->compensation_matrix[6],
              (unsigned)cal_db_ptr->compensation_matrix[7],
              (unsigned)cal_db_ptr->compensation_matrix[8]);
          }          
          all_cal_ptr->full_cal = *cal_db_ptr;
        }
        else if ( smgr_is_valid_fac_cal(reg_data_ptr) )
        {
          sns_smgr_cal_s* cal_db_ptr;
          uint32_t i;

          /* Reset all autocal data */
          all_cal_ptr->auto_cal.used = false;
          sns_smgr_load_default_cal(&all_cal_ptr->auto_cal);

          cal_db_ptr = &all_cal_ptr->factory_cal;
          cal_db_ptr->used = true;
          for ( i = 0; i < ARR_SIZE(cal_db_ptr->zero_bias); i++)
          {
            cal_db_ptr->zero_bias[i]    = reg_data_ptr[i];
            cal_db_ptr->scale_factor[i] = reg_data_ptr[i+SNS_SMGR_SENSOR_DIMENSION_V01];
          }
          all_cal_ptr->full_cal = *cal_db_ptr;
        }
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_reg_resp_msg

===========================================================================*/
/*!
  @brief Receive registry data. Find where it was requested and apply it.

  @param item_ptr - the response message
 
  @return
   none
 */
/*=========================================================================*/
void sns_smgr_process_reg_resp_msg(sns_smgr_mr_msg_s* item_ptr)
{
  /* Response from register read request of group or single item */
  if ( SNS_REG_GROUP_READ_RESP_V02 == item_ptr->header.msg_id )
  {
    sns_reg_group_read_resp_msg_v02* resp_ptr =
      (sns_reg_group_read_resp_msg_v02*)item_ptr->body_ptr;

    sns_smgr_process_reg_data(SNS_SMGR_REG_ITEM_TYPE_GROUP,
                              resp_ptr->group_id,
                              resp_ptr->data_len,
                              resp_ptr->data,
                              resp_ptr->resp);
  }
  else if ( SNS_REG_SINGLE_READ_RESP_V02 == item_ptr->header.msg_id )
  {
    sns_reg_single_read_resp_msg_v02* resp_ptr =
      (sns_reg_single_read_resp_msg_v02*)item_ptr->body_ptr;
    sns_smgr_process_reg_data(SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                              resp_ptr->item_id,
                              resp_ptr->data_len,
                              resp_ptr->data,
                              resp_ptr->resp);
  }
  else  /* response from register write */
  {
    SNS_SMGR_PRINTF2(MED, "process_msg - resp result=%d err=%d",
                     ((sns_common_resp_s_v01*)item_ptr->body_ptr)->sns_result_t,
                     ((sns_common_resp_s_v01*)item_ptr->body_ptr)->sns_err_t);
  }
  SNS_OS_FREE(item_ptr->body_ptr);
  SNS_OS_FREE(item_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_req_reg_data

===========================================================================*/
/*!
  @brief This function requests data from registry

  @detail
  @param
   Id - identifier of single item or group
   Type - option to request single item or group
  @return
   error code
 */
/*=========================================================================*/
sns_err_code_e sns_smgr_req_reg_data( 
  uint16_t item_id, 
  uint8_t  item_type)
{
  void*                 read_req_ptr = NULL;
  void*                 read_resp_ptr = NULL;
  uint16_t*             resp_cb_data_ptr = NULL;
  sns_smgr_mr_header_s  req_hdr;
  uint16_t              resp_len = 0;

  SMGR_ASSERT_AT_COMPILE(ARR_SIZE(smgr_ssi_cfg)*SNS_REG_SSI_SMGR_CFG_NUM_SENSORS >=
                         ARR_SIZE(smgr_ssi_devinfo));

  SNS_SMGR_PRINTF2(MED, "req_reg_data - item(%d) type(%d)", item_id, item_type);

  if ( SNS_SMGR_REG_ITEM_TYPE_GROUP == item_type )
  {
    req_hdr.msg_id = SNS_REG_GROUP_READ_REQ_V02;
    req_hdr.body_len = sizeof( sns_reg_group_read_req_msg_v02 );
    resp_len = sizeof(sns_reg_group_read_resp_msg_v02);
    resp_cb_data_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(uint16));
    if( resp_cb_data_ptr )
    {
      *resp_cb_data_ptr = item_id;
    }
  }
  else if ( SNS_SMGR_REG_ITEM_TYPE_SINGLE == item_type )
  {
    req_hdr.msg_id = SNS_REG_SINGLE_READ_REQ_V02;
    req_hdr.body_len = sizeof( sns_reg_single_read_req_msg_v02 );
    resp_len = sizeof(sns_reg_single_read_resp_msg_v02);
  }
  else
  {
    return SNS_ERR_FAILED;
  }

  read_req_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, req_hdr.body_len);
  read_resp_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, resp_len);

  if ( (NULL == read_req_ptr) || (NULL == read_resp_ptr) )
  {
    if (NULL != read_req_ptr)
    {
      SNS_OS_FREE(read_req_ptr);
    }
    if (NULL != read_resp_ptr)
    {
      SNS_OS_FREE(read_resp_ptr);
    }
    if (NULL != resp_cb_data_ptr)
    {
      SNS_OS_FREE(resp_cb_data_ptr);
    }
    return SNS_ERR_NOMEM;
  }

  if ( SNS_SMGR_REG_ITEM_TYPE_GROUP == item_type )
  {
    ((sns_reg_group_read_req_msg_v02*)read_req_ptr)->group_id = item_id;
  }
  else
  {
    ((sns_reg_single_read_req_msg_v02*)read_req_ptr)->item_id = item_id;
  }

  req_hdr.svc_num = SNS_REG2_SVC_ID_V01;
  return sns_smgr_mr_send_req(&req_hdr, read_req_ptr, read_resp_ptr, resp_len, 
                              (void*)resp_cb_data_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_update_reg_data

===========================================================================*/
/*!
  @brief This function updates the registry data

  @detail
  @param
   item_id   - identifier of single item or group
   item_type - option to request single item or group
   item_len  - bytes
   data_ptr  - pointer to data that will update the registry
  @return
   error code
 */
/*=========================================================================*/
sns_err_code_e sns_smgr_update_reg_data( 
  const uint16_t item_id,
  const uint8_t  item_type,
  uint32_t       item_len,
  uint8_t*       data_ptr)
{
  sns_err_code_e        sns_err = SNS_ERR_FAILED;

#ifndef SNS_PCSIM
  void*                 write_req_ptr = NULL;
  void*                 write_resp_ptr = NULL;
  uint16_t              resp_len;
  sns_smgr_mr_header_s  req_hdr;

  req_hdr.svc_num = SNS_REG2_SVC_ID_V01;
  if ( SNS_SMGR_REG_ITEM_TYPE_GROUP == item_type )
  {
    if (item_len > SNS_REG_MAX_GROUP_BYTE_COUNT_V02)
    {
      return SNS_ERR_BAD_PARM;
    }
    req_hdr.msg_id = SNS_REG_GROUP_WRITE_REQ_V02;
    req_hdr.body_len = sizeof(sns_reg_group_write_req_msg_v02);
    resp_len = sizeof(sns_reg_group_write_resp_msg_v02);
  }
  else if ( SNS_SMGR_REG_ITEM_TYPE_SINGLE == item_type )
  {
    if (item_len >= SNS_REG_MAX_ITEM_BYTE_COUNT_V02)
    {
      return SNS_ERR_BAD_PARM;
    }
    req_hdr.msg_id = SNS_REG_SINGLE_WRITE_REQ_V02;
    req_hdr.body_len = sizeof( sns_reg_single_write_req_msg_v02 );
    resp_len = sizeof(sns_reg_single_write_resp_msg_v02);
  }
  else
  {
    return SNS_ERR_FAILED;
  }

  write_req_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, req_hdr.body_len);
  write_resp_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, resp_len);
  if ( (NULL == write_req_ptr) || (NULL == write_resp_ptr) )
  {
    if (NULL != write_req_ptr)
    {
      SNS_OS_FREE(write_req_ptr);
    }
    if (NULL != write_resp_ptr)
    {
      SNS_OS_FREE(write_resp_ptr);
    }
    return SNS_ERR_NOMEM;
  }

  if ( SNS_SMGR_REG_ITEM_TYPE_GROUP == item_type )
  {
    sns_reg_group_write_req_msg_v02* req_ptr = 
      (sns_reg_group_write_req_msg_v02*)write_req_ptr;
    req_ptr->group_id = item_id;
    req_ptr->data_len = item_len;
    SNS_OS_MEMSCPY(req_ptr->data, sizeof(req_ptr->data), data_ptr, item_len);
  }
  else
  {
    sns_reg_single_write_req_msg_v02* req_ptr =
      (sns_reg_single_write_req_msg_v02*)write_req_ptr;
    req_ptr->item_id  = item_id;
    req_ptr->data_len = item_len;
    SNS_OS_MEMSCPY(req_ptr->data, sizeof(req_ptr->data), data_ptr, item_len);
  }

  sns_err = sns_smgr_mr_send_req(&req_hdr, write_req_ptr, write_resp_ptr, 
                                 resp_len, NULL);
#endif

  return sns_err;
}

/*===========================================================================

  FUNCTION:   smgr_send_reg_message_for_fac_cal

===========================================================================*/
/*!
  @brief Send registry message for factory calibration

  @Detail

  @param[i] NONE

  @return
   NONE
*/
/*=========================================================================*/
void smgr_send_reg_message_for_fac_cal(void)
{
#ifndef ADSP_STANDALONE
  sns_smgr_sensor_s*        sensor_ptr;
  const smgr_sensor_cfg_s*  sensor_cfg_ptr;
  uint32_t                  ix;

  for ( ix = 0; ix < ARR_SIZE(sns_smgr.sensor); ix++ )
  {
    sensor_ptr = &sns_smgr.sensor[ix];
    sensor_cfg_ptr = sensor_ptr->const_ptr;

    /* skip any non-existent sensor */
    if ( NULL == SMGR_DRV_FN_PTR(sensor_ptr) )
    {
      continue;
    }

    if ( SNS_SMGR_REG_ITEM_TYPE_NONE != sensor_cfg_ptr->primary_cal_reg_type )
    {
      sns_err_code_e  err;
      err = sns_smgr_req_reg_data(sensor_cfg_ptr->primary_cal_reg_id,
                                  sensor_cfg_ptr->primary_cal_reg_type);
      SNS_ASSERT( SNS_SUCCESS == err);
    }
  }
#endif
}

/*===========================================================================

  FUNCTION:   sns_smgr_save_last_rcvd_reg_group_id

===========================================================================*/
/*!
  @brief Saves the last received registry group ID

  @detail
  @param[i] group_id - the last registry group ID received
  @return   none
 */
/*=========================================================================*/
void sns_smgr_save_last_rcvd_reg_group_id(uint16_t group_id)
{
  sns_smgr.last_received_reg_group_id = group_id;
}

