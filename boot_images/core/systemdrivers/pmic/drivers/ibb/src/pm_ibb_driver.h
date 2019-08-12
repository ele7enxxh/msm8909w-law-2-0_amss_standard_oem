#ifndef PM_IBB_DRIVER__H
#define PM_IBB_DRIVER__H

/*! \file pm_ibb _driver.h
 *  \n
 *  \brief This file contains functions prototypes and variable/type/constant
*          declarations for supporting SMBB peripheral  
 *  \n  
 *  \n &copy;
 *  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/ibb/src/pm_ibb_driver.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/20/14   al      Updating comm lib 
04/30/14   al      Initial revision  
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target_information.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/


/************************************************************************/
/* register definitions                                                                     */
/************************************************************************/

typedef struct
{
    uint32                      base_address;                        //0xDC00
    pm_register_address_type    status1;                             //0x008
    pm_register_address_type    status2;                             //0x009
    pm_register_address_type    status3;                             //0x00A
    pm_register_address_type    output_voltage;                      //0x041
    pm_register_address_type    ring_suppression_ctl;                //0x042
    pm_register_address_type    vref_sel;                            //0x043
    pm_register_address_type    lcd_amoled_sel;                      //0x044
    pm_register_address_type    module_rdy;                          //0x045
    pm_register_address_type    enable_ctl;                          //0x046
    pm_register_address_type    pd_ctl;                              //0x047
    pm_register_address_type    clk_div;                             //0x048
    pm_register_address_type    iplimit_startup_ctl_1;               //0x049
    pm_register_address_type    iplimit_startup_ctl_2;               //0x04A
    pm_register_address_type    current_limit;                       //0x04B
    pm_register_address_type    current_sense;                       //0x04C
    pm_register_address_type    loop_comp_ctl;                       //0x04D
    pm_register_address_type    slope_comp_current;                  //0x04E
    pm_register_address_type    inzero_ctl;                          //0x04F
    pm_register_address_type    ps_ctl;                              //0x050
    pm_register_address_type    pwm_ctl_1;                           //0x051
    pm_register_address_type    pwm_ctl_2;                           //0x052
    pm_register_address_type    rdson_mngmnt;                        //0x053
    pm_register_address_type    short_circuit_ctl;                   //0x054
    pm_register_address_type    vreg_ok_ctl;                         //0x055
    pm_register_address_type    nonoverlap_time_1;                   //0x056
    pm_register_address_type    nonoverlap_time_2;                   //0x057
    pm_register_address_type    pwrup_pwrdn_ctl_1;                   //0x058
    pm_register_address_type    pwrup_pwrdn_ctl_2;                   //0x059
    pm_register_address_type    swire_ctl;                           //0x05A
    pm_register_address_type    output_voltage_intl;                 //0x05B
    pm_register_address_type    output_slew_ctl;                     //0x05C
    pm_register_address_type    output_voltage_dis_afvc_threshold;   //0x05D
    pm_register_address_type    ramp_offset;                         //0x05E
    pm_register_address_type    soft_start_ctl;                      //0x05F
    pm_register_address_type    spare_ctl;                           //0x060
    pm_register_address_type    nlimit_dac;                          //0x061
    pm_register_address_type    sec_access;                          //0x0D0
}pm_ibb_register_info_type; 


typedef struct
{
    pm_comm_info_type            *comm_ptr;
    pm_ibb_register_info_type    *ibb_reg_table;
    uint8                        num_of_peripherals;
}pm_ibb_data_type;


/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_ibb_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_ibb_data_type* pm_ibb_get_data(uint8 pmic_index);

uint8 pm_ibb_get_num_peripherals(uint8 pmic_index);

#endif // PM_IBB_DRIVER__H
