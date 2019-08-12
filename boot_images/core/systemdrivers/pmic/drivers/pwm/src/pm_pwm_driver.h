#ifndef __PM_PWM_DRIVER_H__
#define __PM_PWM_DRIVER_H__

/*! \file
 *  \n
 *  \brief  ipwm.h 
 *  \details  
 *  \n &copy; Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
              Qualcomm Technologies Proprietary and Confidential.
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/pwm/src/pm_pwm_driver.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/20/14   al      Updating comm lib 
06/20/14   al      Updated coypright info 
12/11/12   al      Created.
========================================================================== */

#include "pm_target_information.h"


typedef struct 
{
    pm_register_address_type base_address;  
    pm_register_address_type peripheral_offset;

    pm_register_address_type pwm_size_clk;             //  0x041
    pm_register_address_type pwm_freq_prediv_clk;      //  0x042
    pm_register_address_type pwm_type_config;          //  0x043
    pm_register_address_type pwm_value_lsb;            //  0x044
    pm_register_address_type pwm_value_msb;            //  0x045
    pm_register_address_type enable_control;           //  0x046
    pm_register_address_type pwm_sync;                 //  0x047
    pm_register_address_type sec_access;               //  0x0D0//
    pm_register_address_type perph_reset_ctl1;         //0x0D8
    pm_register_address_type perph_reset_ctl2;         //0x0D9
    pm_register_address_type perph_reset_ctl3;         //0x0DA
    pm_register_address_type perph_reset_ctl4;         //0x0DB
}pm_pwm_register_info_type;

typedef struct
{
    pm_comm_info_type          *comm_ptr;
    pm_pwm_register_info_type  *pwm_reg_table;
    uint8                       num_of_peripherals;
} pm_pwm_data_type;


/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_pwm_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_pwm_data_type* pm_pwm_get_data(uint8 pmic_index);

uint8 pm_pwm_get_num_peripherals(uint8 pmic_index);

#endif // __PM_PWM_DRIVER_H__

