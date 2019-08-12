/** 
 * This module provides access to device interrupts. Drivers register as 
 * listeners for a particular GPIO. Signals are then delivered in the SMGR 
 * task context. 
 *  
 * @note Currently only one listener per GPIO is supported. 
 *  
 * Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ddf/inc/sns_ddf_signal.h#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2015-11-03 pn   Moved declaration of private functions out
  2014-12-10 MW   Moved internal struct to private file
  2014-07-29 vy   Moved register and deregister apis to bigImage 
  2013-12-18 MW   Added sns_ddf_signal_gpio_read
  2013-04-06 ag   Remove APDS DRI workaround
  2013-01-18 pn   Workaround for APDS DRI
  2011-04-13 br   Inserted sns_ddf_signal_post(). Changed prototype of sns_ddf_signal_dispatch()
  2011-12-26 yk   Changed interface to use GPIO numbers instead of abstract IRQ.
  2011-06-09 ag   added signal for alsprx interrupt
  2011-04-07 yk   Initial revision
==============================================================================*/

#ifndef SNS_DDF_SIGNAL_H
#define SNS_DDF_SIGNAL_H

#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"


/**
 * Interrupt trigger types used to configure the PIC.
 */
typedef enum
{
    SNS_DDF_SIGNAL_IRQ_HIGH,
    SNS_DDF_SIGNAL_IRQ_LOW,
    SNS_DDF_SIGNAL_IRQ_RISING,
    SNS_DDF_SIGNAL_IRQ_FALLING,
    SNS_DDF_SIGNAL_IRQ_DUAL_EDGE
    
} sns_ddf_signal_irq_trigger_e;

/**
 * Indicates if interrupts are available on the current platform. If interrupt 
 * functionality is not supported, all other functions in this file will return 
 * a failed status.
 *  
 * @return True if interrupts are enabled on this platform. 
 */
bool sns_ddf_signal_irq_enabled(void);


/**
 * Registers a driver to receive signal notification when an interrupts fires.
 * Note that the actual interrupt is picked up by SMGR which then forwards a
 * a signal, in its own context, to the driver.
 *
 * @param[in] gpio_num      GPIO number to register for notifications.
 * @param[in] dd_handle     Requesting driver's instance.
 * @param[in] dd_interface  Interface of the driver to be notified.
 * @param[in] trigger       Interrupt trigger type to configure the PIC.
 *
 * @return SNS_DDF_SUCCESS if registeration was completed successfully.
 *         SNS_DDF_EINVALID_PARAM if the irq has already been registered.
 *         SNS_DDF_EFAIL if a problem occurred with the underlying service.
 */
sns_ddf_status_e sns_ddf_signal_register(
    uint32_t                      gpio_num,
    sns_ddf_handle_t              dd_handle,
    sns_ddf_driver_if_s*          dd_interface,
    sns_ddf_signal_irq_trigger_e  trigger);


/**
 * Deregisters the current handler for a particular interrupt such that it will
 * no longer receive notifications when the interrupt occurs.
 *
 * @param[in] gpio_num  GPIO number to de-register from signal notifications.
 *
 * @return SNS_DDF_SUCCESS if deregisteration was successful.
 */
sns_ddf_status_e sns_ddf_signal_deregister(uint32_t gpio_num);

/*===========================================================================*/
/*!
  @brief  Read GPIO pin
  @detail

  @param[i] gpio number
  @param[0] gpio status
  @return
   DAL_SUCCESS -- GPIO was successfully configured as requested.\n
   DAL_ERROR -- Either an invalid GPIO number in the configuration or ownership
   cannot be obtained from a secure root.
 */
/*=========================================================================*/
sns_ddf_status_e sns_ddf_signal_gpio_read(uint16_t gpio_num, uint32_t *gpio_status);


#endif
