
/**
 * @file sns_smgr_ddf_priv.h
 * @brief Common private definitions used across the SMGR and Sensors
 *  	  Device Driver Framework.
 *  
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *  
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2015-11-03 pn   Moved declarations from snd_ddf_signal.h and sns_ddf_util.h
  2014-09-02 jms  Common definitions shared by smgr and sns_ddf_comm.c
==============================================================================*/

#ifndef SNS_SMGR_DDF_PRIV_H
#define SNS_SMGR_DDF_PRIV_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/** ----- SSI registry uint16_t bus_address/bus_id bit mask definition ------
 *  bus_instance[15:12] = type of bus sns_ddf_bus_e
 *  bus_instance[11:4]  = unused
 *  bus_instance[3:0]   = bus ID
 */
#define SNS_DDF_COMM_BUS_TYPE(reg_bus_addr) ( (0xF000&(reg_bus_addr))>>12 )
#define SNS_DDF_COMM_BUS_ID(reg_bus_addr)   ( (0x000F&(reg_bus_addr))     )
//---------------------------------------------------------------------------

#define SNS_DDF_COMM_BUS_TYPE_IS_SPI(reg_bus_addr) \
  ( SNS_DDF_BUS_SPI == (SNS_DDF_COMM_BUS_TYPE(reg_bus_addr)) )

#define SNS_DDF_COMM_BUS_TYPE_IS_I2C(reg_bus_addr) \
  ( SNS_DDF_BUS_I2C == (SNS_DDF_COMM_BUS_TYPE(reg_bus_addr)) )

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Public Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

//============================================================================
/**
 * @brief  Sets a communication bus default configuration.
 *
 * @param[in/out] dev_access      Bus config ptr. This config
 *                                is given to drivers as part of initialization.
 * @param[in]     bus_instance    Configuration data for the bus address and bus type.
 * @param[in]     bus_slave_addr  Configuration data for the bus slave address.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_comm_bus_init_cfg
(
    sns_ddf_device_access_s*  dev_access,
    uint16_t                  bus_instance,
    uint32_t                  bus_slave_addr
);

//============================================================================
/**
* @brief  Frees a communication bus default configuration.
*
* @param[in/out] dev_access      Bus config ptr. This config
*                                is given to drivers as part of initialization.
*
* @return SNS_DDF_SUCCESS if the operation was done successfully.
*         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
*         SNS_DDF_EFAIL to indicate an error has occurred.
*/
sns_ddf_status_e sns_ddf_comm_bus_free_cfg
(
    sns_ddf_device_access_s*  dev_access
);


//============================================================================
/**
 * @brief Posts the signal into the signal database 
 *  
 * @note Executes in GPIO ISR context 
 *  
 * @param[i] sig_num - signal ID
 *  
 * @return none
 */
void sns_ddf_signal_post(uint32_t sig_num);


//============================================================================
/**
 * @brief Dispatches the handler function for interrupts that have just fired
 *  
 * @note Executes in SMGR task context 
 *  
 * @param none
 *  
 * @return none
 */
void sns_ddf_signal_dispatch(void);


//============================================================================
/**
 * @brief Posts the given timer.
 * 
 * @note Executes in timer ISR context
 *  
 * @param[in] timer_id - id of the timer that has just expired 
 *  
 * @return none
 */
void sns_ddf_timer_post(void* timer_id);


//============================================================================
/**
 * @brief Dispatches the callback functions of expired timers
 * 
 * @note Executes in SMGR task context
 *  
 * @param none 
 *  
 * @return none
 */
void sns_ddf_timer_dispatch(void);

#endif
