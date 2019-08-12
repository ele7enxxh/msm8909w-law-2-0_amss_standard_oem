#ifndef PM_LPG_DRIVER__H
#define PM_LPG_DRIVER__H

/*! \file
*  \n
*  \brief  pm_lpg_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting LPG peripheral 
*  
 *  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/lpg/src/pm_lpg_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
03/04/13   aab     Created
========================================================================== */

/*===========================================================================
						INCLUDE FILES
===========================================================================*/

#include "pm_target_information.h"

/*===========================================================================
						LPG TYPE DEFINES AND ENUMS
===========================================================================*/
typedef struct
{
    pm_register_address_type base_address;  
    pm_register_address_type peripheral_offset;
    pm_register_address_type perph_subtype;                //  0x005    
    pm_register_address_type lpg_pattern_config;           //  0x040
    pm_register_address_type lpg_pwm_size_clk;             //  0x041
    pm_register_address_type lpg_pwm_freq_prediv_clk;      //  0x042
    pm_register_address_type lpg_pwm_type_config;          //  0x043
    pm_register_address_type pwm_value_lsb;                //  0x044
    pm_register_address_type pwm_value_msb;                //  0x045
    pm_register_address_type enable_control;               //  0x046
    pm_register_address_type pwm_sync;                     //  0x047
    pm_register_address_type ramp_step_duration_lsb;       //  0x050
    pm_register_address_type ramp_step_duration_msb;       //  0x051
    pm_register_address_type pause_hi_multiplier_lsb;      //  0x052
    pm_register_address_type pause_hi_multiplier_msb;      //  0x053
    pm_register_address_type pause_lo_multiplier_lsb;      //  0x054
    pm_register_address_type pause_lo_multiplier_msb;      //  0x055
    pm_register_address_type hi_index;                     //  0x056
    pm_register_address_type lo_index;                     //  0x057
    pm_register_address_type lut_lsb;                      //  0x040
    pm_register_address_type lut_msb;                      //  0x041                                                                                                                     
    pm_register_address_type ramp_control;                  // 0x0C8
} pm_lpg_register_info_type;



typedef struct
{
    pm_comm_info_type          *comm_ptr;
    pm_lpg_register_info_type  *lpg_reg_table;
    uint8                       num_of_peripherals;
} pm_lpg_data_type;


typedef struct
{
    //Number of LPG LUT
    unsigned char              NumOfLut;
} lpg_common_ds;


/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_lpg_driver_init(pm_comm_info_type *comm_ptr, uint8 pmic_index);

pm_lpg_data_type* pm_lpg_get_data(uint8 pmic_index);

#endif // PM_LPG_DRIVER__H
