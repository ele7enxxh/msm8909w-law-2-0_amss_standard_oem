/*=============================================================================
  @file sns_smgr_hw.c

  This file contains hw related features used by SMGR.

*******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_hw.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-08-19  pn   Simplified powerrails control
  2015-07-29  agk  Fixes for power rail changes  
  2015-07-07  rt   For 8996, update the SSC TLMM GPIO config states
  2015-06-29  agk  Added changes for power rail
  2014-11-19  pn   Only call DAL_DeviceAttach() when necessary
  2014-10-21  jc   Changed sar_int GPIO pin setting
  2014-10-14  MW   Added sns_hw_pnoc_vote()
  2014-10-10  agk  Lowered PNOC BW to more realistic value
  2014-10-02  jhh  Add pnoc vote on/off based on sensor stream activity
  2014-10-02  MW   Fix crash if sns_reset is not used
  2014-09-22  sc   Fix KW error
  2014-09-17  ak   Added i2c GPIO config
  2014-09-15  jms  Added SPI support
  2014-09-08  pn   Moved sns_hw_send_powerrail_msg() from uImage 
  2014-08-26  MW   Made sns_get_i2c_bus_instance() public
  2014-08-08  pn   Processes new SSI registry entries for GPIOs
  2014-07-31  pn   Enabled power control; removed obsolete code
  2014-07-14  VY   Fixed compiler warnings
  2014-06-20  ps   Update sns_i2c_bus_table type
  2014-06-11  ps   Disable I2C GPIO pins configuration
  2014-06-05  vy   Further refactored to support uImage  
  2014-05-09  sc   Added changes for uImage I2C driver integration
  2014-04-23  pn   Initial version
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DDITlmm.h"
//#include "uTlmm.h"

#include "sns_rh_util.h"
#include "sns_smgr_hw.h"
#include "sns_smgr_main.h"
#include "sns_smgr_pm_if.h"
#include "sns_smgr_reg.h"
#include "sns_smgr_util.h"
#include "sns_smgr_ddf_priv.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

static I2cDrv_I2cBusId sns_i2c_bus_table[] =
{
  I2CDRV_I2C_MIN,
  I2CDRV_I2C_1,
  I2CDRV_I2C_2,
  I2CDRV_I2C_3,
  I2CDRV_I2C_4,
  I2CDRV_I2C_5,
  I2CDRV_I2C_6,
  I2CDRV_I2C_7,
  I2CDRV_I2C_8,
  I2CDRV_I2C_9,
  I2CDRV_I2C_10,
  I2CDRV_I2C_11,
  I2CDRV_I2C_12
};


/*----------------------------------------------------------------------------
 *  External Variables/Functions
 * -------------------------------------------------------------------------*/
extern sns_hw_s   sns_smgr_hw;
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define PNOC_AB  10000  
#define PNOC_IB  10000  


/*===========================================================================

  FUNCTION:   sns_hw_set_inactive_gpio_config

===========================================================================*/
/*!
  @brief Configuring GPIO pins for inactive at DSPS initialization 

  @param [in] gpio_num : the GPIO to configure to inactive state
 
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_hw_set_inactive_gpio_config(uint16_t gpio_num, DALGpioPullType pull)
{
#if !defined(SNS_QDSP_SIM)
  DALResult status = DAL_ERROR_INVALID_PARAMETER;

  if ( (gpio_num != SNS_SMGR_HW_INVALID_GPIO) && (gpio_num != 0) 
       && (sns_smgr_hw.gpio_handle != NULL))
  {
    DALGpioSignalType port_config = DAL_GPIO_CFG(gpio_num, 0, DAL_GPIO_INPUT, 
                                                 pull, DAL_GPIO_2MA);
    DalTlmm_SetInactiveConfig(sns_smgr_hw.gpio_handle, gpio_num , port_config);
    status = DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, port_config, 
                                DAL_TLMM_GPIO_DISABLE);
  }
  SNS_SMGR_PRINTF2(MED, "set_inactive_gpio - gpio_num=0x%x result=%d", gpio_num, status);
#endif /* !defined(SNS_QDSP_SIM) */
}

/*===========================================================================

  FUNCTION:  sns_smgr_inactive_ssc_gpio_config 

===========================================================================*/
/*!
  @brief Configuring GPIO pins to low power state for inactive GPIOs

  @param 
   none
   
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_inactive_ssc_gpio_config( void )
{
/*
  uTlmmGpioSignalType gpio_config;

  gpio_config = UTLMM_GPIO_CFG(4, 2, UTLMM_GPIO_OUTPUT, UTLMM_GPIO_KEEPER, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);

  gpio_config = UTLMM_GPIO_CFG(5, 2, UTLMM_GPIO_OUTPUT, UTLMM_GPIO_KEEPER, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);
        
  gpio_config = UTLMM_GPIO_CFG(6, 2, UTLMM_GPIO_OUTPUT, UTLMM_GPIO_KEEPER, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);

  gpio_config = UTLMM_GPIO_CFG(7, 2, UTLMM_GPIO_INPUT, UTLMM_GPIO_KEEPER, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);

  gpio_config = UTLMM_GPIO_CFG(12, 1, UTLMM_GPIO_OUTPUT, UTLMM_GPIO_KEEPER, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);
        
  gpio_config = UTLMM_GPIO_CFG(13, 1, UTLMM_GPIO_INPUT, UTLMM_GPIO_KEEPER, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE); */
}

/*===========================================================================

  FUNCTION:  sns_smgr_active_ssc_gpio_config 

===========================================================================*/
/*!
  @brief Configuring active GPIO pins for the right configuration during sleep and 
  active states.

  @param [in] is_sleep : State of the Vdd IO rail. True implies IO rail is OFF.
 
  @return
   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_active_ssc_gpio_config(bool is_sleep)
{
	/*
  uTlmmGpioSignalType gpio_config;
  uTlmmGpioPullType pull = is_sleep ? UTLMM_GPIO_KEEPER : UTLMM_GPIO_NO_PULL;
  
  gpio_config = UTLMM_GPIO_CFG(0, 1, UTLMM_GPIO_OUTPUT, pull, UTLMM_GPIO_2MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);

  gpio_config = UTLMM_GPIO_CFG(2, 1, UTLMM_GPIO_OUTPUT, pull, UTLMM_GPIO_2MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);

  gpio_config = UTLMM_GPIO_CFG(3, 1, UTLMM_GPIO_OUTPUT, pull, UTLMM_GPIO_2MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);

  gpio_config = UTLMM_GPIO_CFG(8, 1, UTLMM_GPIO_OUTPUT, pull, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);

  gpio_config = UTLMM_GPIO_CFG(9, 1, UTLMM_GPIO_OUTPUT, pull, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);
        
  gpio_config = UTLMM_GPIO_CFG(10, 1, UTLMM_GPIO_OUTPUT, pull, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);

  gpio_config = UTLMM_GPIO_CFG(11, 1, UTLMM_GPIO_INPUT, pull, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);
        
  gpio_config = UTLMM_GPIO_CFG(14, 1, UTLMM_GPIO_OUTPUT, pull, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);
        
  gpio_config = UTLMM_GPIO_CFG(15, 1, UTLMM_GPIO_INPUT, pull, UTLMM_GPIO_8MA);
  uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE);*/
}


/*===========================================================================

  FUNCTION:   sns_hw_power_rail_config

===========================================================================*/
/*!
  @brief Power rail state check before sending RPM message

  @param[in] rail_info_ptr - power rail whose state to change
  @param[in] state:  the state of power rail needs to be updated to.

  @return
   none
 */
/*=========================================================================*/
void sns_hw_power_rail_config(
  sns_pm_pwr_rail_info_s* rail_info_ptr, 
  sns_pm_pwr_rail_e       state )
{
#if !defined(SNS_EXCLUDE_POWER)
  uint32_t remaining_time = 0;

  sns_em_timer_get_remaining_time(rail_info_ptr->tmr_obj, &remaining_time);

  SNS_SMGR_PRINTF4(
     MED, "power_rail_config - rail=%u curr/want=0x%02x time_remain=%u pending_state=%u",
     rail_info_ptr->pwrrail, (rail_info_ptr->powerrail_state << 4) | state, 
     remaining_time, rail_info_ptr->powerrail_pending_state );

  if ( (remaining_time > 0) && (rail_info_ptr->powerrail_pending_state != state) )
  {
    sns_em_cancel_timer(rail_info_ptr->tmr_obj);
    SNS_SMGR_PRINTF1(
      MED, "power_rail_config - CANCELLED TIMER rail=%u", rail_info_ptr->pwrrail);
    remaining_time = 0;
  }

  if ( remaining_time == 0 )
  {
    rail_info_ptr->powerrail_pending_state = state;
    if ( state > rail_info_ptr->powerrail_state )
    {
      /* higher power needed now */
      sns_hw_send_powerrail_msg(rail_info_ptr, state);
    }
    else if ( state < rail_info_ptr->powerrail_state )
    {
      SNS_SMGR_PRINTF1(
        MED, "power_rail_config - STARTING TIMER rail=%u", rail_info_ptr->pwrrail);
        
      /* lower power needed, can wait */
      if ( SNS_SUCCESS != 
           sns_em_register_timer(rail_info_ptr->tmr_obj, 
                                 SNS_SMGR_POWERRAIL_HYSTERESIS_TIMER_VALUE_TICK))
      {
        SNS_SMGR_PRINTF0(FATAL, "power_rail_config - failed new timer register");
      }
    }
    /* else, already at the right power state */
  }
  /* else, already waiting to change power to 'state' */
#endif /* !defined(SNS_EXCLUDE_POWER) */
}


/*===========================================================================

  FUNCTION:   sns_hw_all_powerrails_config

===========================================================================*/
/*!
  @brief Sets all powerrails to given state

  @param[in] state:  the state of power rail needs to be updated to.

  @return none
 */
/*=========================================================================*/
void sns_hw_all_powerrails_config(sns_pm_pwr_rail_e state)
{
  uint8_t i;
  SNS_SMGR_PRINTF1(HIGH, "all_powerrails_config - state=%u", state);
  for ( i=0; i<ARR_SIZE(sns_smgr_hw.pwr_rail_info); i++ )
  {
    if ( sns_smgr_hw.pwr_rail_info[i].pwrrail != SNS_SMGR_INVALID_PWRRAIL )
    {
      SNS_SMGR_PRINTF2(MED, "all_powerrails_config - idx=%u rail=%u",
                       i, sns_smgr_hw.pwr_rail_info[i].pwrrail);
      sns_hw_power_rail_config(&sns_smgr_hw.pwr_rail_info[i], state);
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_hw_power_rail_force_config

===========================================================================*/
/*!
  @brief Configures power rail to requested state regardless of what state it is currently in. 

  @detail This is generally only used during auto detecting physical sensors when SMGR has not populated rail information 
  completely. Once sensors are detected and SMGR has populated the rails, clients should only use the 
  regular power_rail_config API. It is not recommended to use this function unless absolutely necessary as 
  this would cause ADSP to send a message to RPM regardless of the state.

  @param[in] rail: power rail identifier
  @param[in] state:  the state of power rail needs to be updated to.

  @return
   none
 */
/*=========================================================================*/
void sns_hw_power_rail_force_config( uint8_t rail, sns_pm_pwr_rail_e state )
{
#if !defined(SNS_EXCLUDE_POWER)

   SNS_SMGR_PRINTF2(
     MED, "power_rail_force_config: rail %d, state %d", rail, state);
   
   sns_smgr_pm_vote_powerrail(rail, state);
  
#endif /* !defined(SNS_EXCLUDE_POWER) */
}

/*===========================================================================

  FUNCTION:   sns_hw_populate_rail_info

===========================================================================*/
/*!
  @brief Initializes the given rail

  @param[in] rail: power rail identifier
 
  @return none
 */
/*=========================================================================*/
SMGR_STATIC sns_pm_pwr_rail_info_s* sns_hw_init_rail_info(uint8_t rail)
{
  uint8_t i;
  sns_pm_pwr_rail_info_s* rail_info_ptr = NULL;

  for (i=0; i<ARR_SIZE(sns_smgr_hw.pwr_rail_info) && rail_info_ptr == NULL; i++)
  {
    if (rail == sns_smgr_hw.pwr_rail_info[i].pwrrail)
    {
      rail_info_ptr = &sns_smgr_hw.pwr_rail_info[i];
    }
  }

  if ( rail_info_ptr == NULL )
  {
    /* populate the rail in first emply slot */
    for (i=0; i<ARR_SIZE(sns_smgr_hw.pwr_rail_info); i++)
    {
      if (sns_smgr_hw.pwr_rail_info[i].pwrrail == SNS_SMGR_INVALID_PWRRAIL)
      {
        rail_info_ptr = &sns_smgr_hw.pwr_rail_info[i];
        rail_info_ptr->pwrrail                 = rail;
        rail_info_ptr->powerrail_state         = SNS_PWR_RAIL_OFF;
        rail_info_ptr->powerrail_pending_state = SNS_PWR_RAIL_OFF;
        sns_em_create_timer_obj(&sns_hw_timer_cb, rail_info_ptr, 
                                SNS_EM_TIMER_TYPE_ONESHOT, 
                                &rail_info_ptr->tmr_obj);
        sns_pm_pwrrail_init(rail_info_ptr->pwrrail);
        break;
      }
    }
  }
  return rail_info_ptr;
}

/*===========================================================================

  FUNCTION:   sns_hw_init_pwrrails

===========================================================================*/
/*!
  @brief Initializes power rails after obtaining power rail information from registry.

  @param[in]:   none
 
  @return none
 */
/*=========================================================================*/
void sns_hw_init_pwrrails( void )
{
  uint8_t i;

  SNS_SMGR_PRINTF0(HIGH, "init_pwrrails");

  /* Init power rails to 0xff */
  for (i=0; i<ARR_SIZE(sns_smgr_hw.pwr_rail_info); i++)
  {
    sns_smgr_hw.pwr_rail_info[i].pwrrail = SNS_SMGR_INVALID_PWRRAIL;
  }

  /* Find out power rails by iterating through sensor list and call PM API to init rails */
  for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
  {
    sns_smgr_sensor_s*       sensor_ptr = &sns_smgr.sensor[i];
    const smgr_sensor_cfg_s* sensor_cfg_ptr = sensor_ptr->const_ptr;
  
    if ( sensor_cfg_ptr->vdd_rail   != SNS_SMGR_INVALID_PWRRAIL ||
         sensor_cfg_ptr->vddio_rail != SNS_SMGR_INVALID_PWRRAIL )
    {
      sensor_ptr->vdd_rail_ptr   = sns_hw_init_rail_info(sensor_cfg_ptr->vdd_rail);
      sensor_ptr->vddio_rail_ptr = sns_hw_init_rail_info(sensor_cfg_ptr->vddio_rail);
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_get_i2c_bus_instance

===========================================================================*/
/*!
  @brief Read the I2C bus instances from registry and update the max instances
         on this platform.

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_get_i2c_bus_instance(
  const sns_reg_ssi_smgr_cfg_group_drvcfg_s* drv_cfg_ptr)
{
  int j, new_bus_instance;
  
  for ( j = 0, new_bus_instance = 1; 
        j < ARR_SIZE(sns_smgr_hw.i2c_bus_instances) && 
        sns_smgr_hw.i2c_bus_instances[j].i2c_bus != 0; 
        j++ ) 
  {
    if(drv_cfg_ptr->i2c_bus == sns_smgr_hw.i2c_bus_instances[j].i2c_bus) 
    {
      new_bus_instance = 0;
      break;
    }
  }
  if ( new_bus_instance && j < ARR_SIZE(sns_smgr_hw.i2c_bus_instances) ) 
  {
    uint8_t bus_idx = (uint8_t)SNS_DDF_COMM_BUS_ID( drv_cfg_ptr->i2c_bus );

    sns_smgr_hw.i2c_bus_instances[j].i2c_bus      = bus_idx;

    if ( bus_idx < ARR_SIZE(sns_i2c_bus_table) )
    {
      sns_smgr_hw.i2c_bus_instances[j].i2c_bus_id = sns_i2c_bus_table[bus_idx];
    } else
    {
      sns_smgr_hw.i2c_bus_instances[j].i2c_bus_id = I2CDRV_I2C_DEV_INVALID;  //bus disabled
    }

    sns_smgr_hw.i2c_bus_count++;

    SNS_SMGR_PRINTF3( MED, "i2c_bus_instance - bus=%d bus_id=%d #bus=%u",
                      bus_idx, sns_smgr_hw.i2c_bus_instances[j].i2c_bus_id,
      sns_smgr_hw.i2c_bus_count);
  }
}

/*===========================================================================

  FUNCTION:   sns_hw_update_reg_items

===========================================================================*/
/*!
  @brief Updates the registry items for reg group SNS_REG_GROUP_SSI_GPIO_CFG_V02

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_hw_update_ssi_reg_items(uint16_t item_id, uint8_t* data_ptr)
{
  if( sns_smgr_ssi_get_cfg_idx(item_id) >= 0 )
  {
    sns_reg_ssi_smgr_cfg_group_s* cfg_group_ptr = 
      (sns_reg_ssi_smgr_cfg_group_s*)data_ptr;
    if( cfg_group_ptr )
    {
      uint32_t ix;
      uint8_t null_uuid[16] = SNS_REG_UUID_NULL;
      for ( ix = 0; ix < ARR_SIZE(cfg_group_ptr->drv_cfg); ix++ )
      {
        sns_reg_ssi_smgr_cfg_group_drvcfg_s* drv_cfg_ptr = &cfg_group_ptr->drv_cfg[ix];
        if( SNS_OS_MEMCMP(drv_cfg_ptr->drvuuid, null_uuid, 16) != 0 )
        {
          SNS_SMGR_PRINTF2(
            MED, "update_ssi_reg - idx=%u sensor=%u", ix, drv_cfg_ptr->sensor_id);
          sns_get_i2c_bus_instance(drv_cfg_ptr);
        }
      }
    }
  }
  else if( item_id == SNS_REG_GROUP_SSI_GPIO_CFG_V02 )
  {
    sns_reg_ssi_gpio_cfg_group_s *cfg_group_ptr = 
      (sns_reg_ssi_gpio_cfg_group_s*)data_ptr;
    if( cfg_group_ptr )
    {
      SNS_SMGR_PRINTF1(
        HIGH, "update_ssi_reg - reset pin = 0x%x", cfg_group_ptr->sns_reset);
      sns_smgr_hw.sensor_reset_gpio = cfg_group_ptr->sns_reset;
      sns_smgr_hw.i2c_sda_1 = cfg_group_ptr->i2c_sda_1;
      sns_smgr_hw.i2c_scl_1 = cfg_group_ptr->i2c_scl_1;
      sns_smgr_hw.i2c_sda_2 = cfg_group_ptr->i2c_sda_2;
      sns_smgr_hw.i2c_scl_2 = cfg_group_ptr->i2c_scl_2;
      /* SMGR may not even have the rail info at this time - check */
      sns_hw_control_reset_pin(SNS_PWR_RAIL_OFF);
      sns_smgr_inactive_ssc_gpio_config();

      if ( cfg_group_ptr->maj_ver_no >=1 && cfg_group_ptr->min_ver_no >=1 )
      {
        /* version 1.1 or later will have the following entries */
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_test, DAL_GPIO_NO_PULL);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_accel_md, DAL_GPIO_NO_PULL);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_accel_dri, DAL_GPIO_NO_PULL);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_gyro_dri, DAL_GPIO_NO_PULL);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_mag_dri, DAL_GPIO_NO_PULL);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_alsp_int, DAL_GPIO_NO_PULL);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_gest_int, DAL_GPIO_NO_PULL);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_press_int, DAL_GPIO_NO_PULL);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_sar_int, DAL_GPIO_PULL_DOWN);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_fp_int, DAL_GPIO_NO_PULL);
        sns_hw_set_inactive_gpio_config(cfg_group_ptr->sns_hall_int, DAL_GPIO_NO_PULL);
      }

      if ( SNS_SUCCESS != sns_smgr_req_reg_data( (uint16_t) sns_smgr_ssi_get_cfg_id(0), 
                                                 SNS_SMGR_REG_ITEM_TYPE_GROUP ) )
      {
        SNS_SMGR_PRINTF1(FATAL, "Failed reading registry group %d", 
                         (uint16_t) sns_smgr_ssi_get_cfg_id(0));
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_hw_read_ssi_data_from_reg

===========================================================================*/
/*!
  @brief Reads the registry items for reg groups SNS_REG_GROUP_SSI_GPIO_CFG_V02 &
         SNS_REG_GROUP_SSI_SMGR_CFG_V02

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_hw_read_ssi_data_from_reg(void)
{
#ifndef ADSP_STANDALONE
  int err = sns_smgr_req_reg_data(SNS_REG_GROUP_SSI_GPIO_CFG_V02, 
                                  SNS_SMGR_REG_ITEM_TYPE_GROUP);
  if ( SNS_SUCCESS == err )
  {
    sns_smgr.all_init_state = SENSOR_ALL_INIT_WAITING_CFG;
  }
  else
  {
    SNS_SMGR_PRINTF2(FATAL, "Failed reading registry. Group %d err %d", 
                     SNS_REG_GROUP_SSI_GPIO_CFG_V02, err);
  }
#else
#ifdef BRINGUP_8952
  sns_smgr_hw.i2c_bus_count = 1;
  sns_smgr_hw.i2c_bus_instances[0].i2c_bus_id = I2CDRV_I2C_4;
#else
  sns_smgr_hw.i2c_bus_count = 2;
  sns_smgr_hw.i2c_bus_instances[0].i2c_bus_id = I2CDRV_I2C_2;
  sns_smgr_hw.i2c_bus_instances[1].i2c_bus_id = I2CDRV_I2C_3;
#endif
#endif
}

/*===========================================================================

  FUNCTION:   sns_hw_bd_config

===========================================================================*/
/*!
  @brief SMGR init HW related variables

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_hw_bd_config(void)
{
#if !defined(SNS_QDSP_SIM)
  sns_smgr_hw.i2c_clk.qupclk_on = false;
#else
  sns_smgr_hw.i2c_clk.qupclk_on = true;
#endif

  sns_smgr_hw.gpio_handle       = NULL;
  sns_smgr_hw.sensor_reset_gpio = SNS_SMGR_HW_INVALID_GPIO;
  sns_smgr_hw_read_ssi_data_from_reg();
}


/*===========================================================================

  FUNCTION:   sns_hw_control_reset_pin

===========================================================================*/
/*!
  @brief Configures reset pin 

  @param State of power rail: ON/OFF/LPM
  @return
   none
 */
/*=========================================================================*/
void sns_hw_control_reset_pin(sns_pm_pwr_rail_e state)
{
#if !defined(SNS_QDSP_SIM)
  DALResult result = DAL_SUCCESS;
  if ( sns_smgr_hw.gpio_handle == NULL )
  {
    result = DAL_DeviceAttach(DALDEVICEID_TLMM, &sns_smgr_hw.gpio_handle);
  }
  if ( (sns_smgr_hw.sensor_reset_gpio != SNS_SMGR_HW_INVALID_GPIO) &&
       (sns_smgr_hw.gpio_handle != NULL) && (result == DAL_SUCCESS) )
  {
    /* configuring sensor reset pin, it will stay high during LD09 and 
       DSPS power on/off, this also puts it in low power mode */

    DALGpioValueType out_val = (state == SNS_PWR_RAIL_OFF) ?
      DAL_GPIO_LOW_VALUE : DAL_GPIO_HIGH_VALUE;
    uint32_t gpio_config =
      DAL_GPIO_CFG_OUT(sns_smgr_hw.sensor_reset_gpio, 0, DAL_GPIO_OUTPUT, 
                       DAL_GPIO_NO_PULL, DAL_GPIO_2MA, out_val);
    DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, gpio_config, DAL_TLMM_GPIO_ENABLE);
  }
#endif /* !defined(SNS_QDSP_SIM) */
}

/*===========================================================================

  FUNCTION:   sns_hw_update_rail_state

===========================================================================*/
/*!
  @brief Sets given rail to given state if necessary
 
  @param [i/o]  rail_ptr   - the rail whose state is being updated
  @param [i]    rail_state - the desired state

  @return none
*/
/*=========================================================================*/
SMGR_STATIC void sns_hw_update_rail_state(
  sns_pm_pwr_rail_info_s* rail_ptr, 
  sns_pm_pwr_rail_e       rail_state)
{
  uint8_t i;
  sns_pm_pwr_rail_e state = rail_state;

  if ( state < rail_ptr->powerrail_pending_state )
  {
    /* Find out if sensors on this rail still need rail at higher state */
    for (i=0; i < ARR_SIZE(sns_smgr.sensor); i++)
    {
      sns_smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];
      if ( SMGR_SENSOR_IN_USE(sensor_ptr) &&
           (sensor_ptr->vdd_rail_ptr   == rail_ptr ||
            sensor_ptr->vddio_rail_ptr == rail_ptr) )
      {
        state = sensor_ptr->vdd_rail_ptr->powerrail_pending_state;
        break;
      }
    }
  }

  if ( state != rail_ptr->powerrail_pending_state )
  {
    sns_hw_power_rail_config(rail_ptr, state);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_update_power_state

===========================================================================*/
/*!
  @brief Votes for appropriate power state based on needs
 
  @details When only Accel is in use votes for low power state; when any other
           sensor is in use votes for high power state.
 
  @param  none

  @return none
*/
/*=========================================================================*/
void sns_smgr_update_power_state(sns_smgr_sensor_s* sensor_ptr)
{
  bool sensor_in_use = SMGR_SENSOR_IN_USE(sensor_ptr);
  sns_pm_pwr_rail_e active_state = 
    SMGR_SENSOR_IS_ACCEL(sensor_ptr) ? SNS_PWR_RAIL_ON_LPM : SNS_PWR_RAIL_ON_NPM;
  sns_pm_pwr_rail_e state = sensor_in_use ? active_state  : SNS_PWR_RAIL_OFF;

  SNS_SMGR_PRINTF3(
    LOW, "update_power_state - sensor=%u in_use=%u railstate=%u",
    SMGR_SENSOR_ID(sensor_ptr), sensor_in_use, state);

  sns_hw_update_rail_state(sensor_ptr->vdd_rail_ptr, state);
  if ( sensor_ptr->vdd_rail_ptr != sensor_ptr->vddio_rail_ptr )
  {
    sns_hw_update_rail_state(sensor_ptr->vddio_rail_ptr, state);
  }
}


/*===========================================================================

  FUNCTION:   sns_hw_update_pnoc_vote

===========================================================================*/
/*!
  @brief Updates PNOC vote if necessary

  @detail Turns on pnoc as soon as first rail is on and
          turns it off when all rails are off

  @param[in] state:  the new state of power rail
 
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_hw_update_pnoc_vote(uint8_t new_rail_state)
{
  /* Turns on pnoc as soon as first rail is on; 
     turns it off when all rails are off */

  if ( !sns_smgr_hw.pnoc_on && new_rail_state > SNS_PWR_RAIL_OFF )
  {
    /* first rail is on */
    sns_smgr_hw.pnoc_on = true;
    sns_hw_pnoc_vote(sns_smgr_hw.pnoc_on);
    sns_smgr_active_ssc_gpio_config(false);
    SNS_SMGR_PRINTF0(MED, "update_pnoc_vote: PNOC voting true");
  }
  else if ( sns_smgr_hw.pnoc_on && new_rail_state == SNS_PWR_RAIL_OFF )
  {
    uint8_t i;
    for (i=0; i<ARR_SIZE(sns_smgr_hw.pwr_rail_info); i++)
    {
      sns_pm_pwr_rail_info_s* rail_info_ptr = &sns_smgr_hw.pwr_rail_info[i];
      if ( rail_info_ptr->pwrrail != SNS_SMGR_INVALID_PWRRAIL && 
           rail_info_ptr->powerrail_state > SNS_PWR_RAIL_OFF )
      {
        SNS_SMGR_PRINTF1(
          LOW, "update_pnoc_vote: rail %u is still on", rail_info_ptr->pwrrail);
        break;
      }
    }
    if ( i >= ARR_SIZE(sns_smgr_hw.pwr_rail_info) )
    {
      /* all rails are off */
      sns_smgr_hw.pnoc_on = false;
      sns_hw_pnoc_vote(sns_smgr_hw.pnoc_on);

      // This is yet another platform dependent entity impacting power only a little bit
      // Should not have been mainlined
      sns_smgr_active_ssc_gpio_config(true);

      SNS_SMGR_PRINTF0(MED, "update_pnoc_vote: PNOC vote FALSE!!!!");
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_hw_send_powerrail_msg

===========================================================================*/
/*!
  @brief Sending RMP message to change the power state of power rail

  @detail

  @param[in] index: index at which power rail was found in pwrrail_info structure
  @param[in] state:  the state of power rail needs to be updated to.
  @return
   none
 */
/*=========================================================================*/
void sns_hw_send_powerrail_msg(
  sns_pm_pwr_rail_info_s* rail_info_ptr, 
  sns_pm_pwr_rail_e       state)
{
#if !defined(SNS_QDSP_SIM)
  if ( sns_smgr_pm_vote_powerrail(rail_info_ptr->pwrrail, state) == SNS_SUCCESS )
  {
    sns_pm_pwr_rail_e old_state = rail_info_ptr->powerrail_state;
    rail_info_ptr->powerrail_state = state;

    SNS_SMGR_PRINTF4(
      MED, "send_powerrail_msg: rail=%u, %u ==> %u, pnoc=%u", 
      rail_info_ptr->pwrrail, old_state, state, sns_smgr_hw.pnoc_on);

    if ( state == SNS_PWR_RAIL_OFF || old_state == SNS_PWR_RAIL_OFF )
    {
      uint8_t i;
      for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
      {
        sns_smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];

        if ( (rail_info_ptr == sensor_ptr->vdd_rail_ptr) || 
             (rail_info_ptr == sensor_ptr->vddio_rail_ptr) )
        {
          if ( SMGR_SENSOR_IS_MAG(sensor_ptr) )
          {
            sns_hw_control_reset_pin(state);
          }

          if ( state == SNS_PWR_RAIL_OFF &&
               sensor_ptr->sensor_state != SENSOR_STATE_FAILED )
          {
            sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_OFF);
            sensor_ptr->event_done_tick = 0;
          }
        }
      }
      sns_hw_update_pnoc_vote(state);
    }
  }
#endif /* !defined(SNS_QDSP_SIM) */
}

/*===========================================================================

  FUNCTION:   sns_hw_send_powerrail_msg_tmr_cb_proc

===========================================================================*/
/*!
  @brief Power rail timer expired.  It's time to change its state.
  @param    none
  @return   none
 */
/*=========================================================================*/
void sns_hw_send_powerrail_msg_tmr_cb_proc(void)
{
  uint8_t index = 0;

  for ( index=0; index<ARR_SIZE(sns_smgr_hw.pwr_rail_info); index++ )
  {
    sns_pm_pwr_rail_info_s* rail_info_ptr = &sns_smgr_hw.pwr_rail_info[index];
        
    if ( (rail_info_ptr->pwrrail != SNS_SMGR_INVALID_PWRRAIL) && 
         (rail_info_ptr->powerrail_pending_state != rail_info_ptr->powerrail_state) )
    {
      SNS_SMGR_PRINTF2(
        HIGH, "powerrail_msg_tmr_cb - rail=%u curr/want=0x%02x", 
        rail_info_ptr->pwrrail,
        (rail_info_ptr->powerrail_state << 4) | rail_info_ptr->powerrail_pending_state);
      sns_hw_send_powerrail_msg(rail_info_ptr, rail_info_ptr->powerrail_pending_state);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_hw_pnoc_vote

===========================================================================*/
/*!
  @brief On/Off pnoc vote

  @detail

  @param[in] on_flag: vote on/off flag
  @return
   none
 */
/*=========================================================================*/
void sns_hw_pnoc_vote(bool on_flag)
{
#if !defined(SNS_QDSP_SIM)
  uint8_t     i;
  int32 i2c_ret = I2C_RES_SUCCESS;
  I2cDrvCmdType  pnoc_cmd;
  I2cDrvPnocVoteData vote_data;

  if ( on_flag )
  {
    pnoc_cmd = I2cDrvCmd_Vote_PnocOn;
    vote_data.uPnocAb = PNOC_AB;
    vote_data.uPnocIb = PNOC_IB;
  }
  else
  { 
    pnoc_cmd = I2cDrvCmd_Vote_PnocOff;
    vote_data.uPnocAb = 0;
    vote_data.uPnocIb = 0;
  }

  for ( i=0; i<sns_smgr_hw.i2c_bus_count && i2c_ret==I2C_RES_SUCCESS && 
             i<ARR_SIZE(sns_smgr_hw.i2c_bus_instances); i++ )
  {
    i2c_ret = I2cDrv_Open(sns_smgr_hw.i2c_bus_instances[i].i2c_bus_id, 
                          &(sns_smgr_hw.i2c_bus_instances[i].config), 
                          0);

    i2c_ret = ( (i2c_ret == I2C_RES_SUCCESS) && 
                 I2cDrv_Cmd ( &(sns_smgr_hw.i2c_bus_instances[i].config), 
                             pnoc_cmd, &vote_data) );
  }
#endif /* !defined(SNS_QDSP_SIM) */
}

