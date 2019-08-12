#ifndef PM_LAB_DRIVER__H
#define PM_LAB_DRIVER__H

/*! \file pm_lab _driver.h
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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/lab/src/pm_lab_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/14   al      Updating comm lib 
05/20/14   al      Architecture update 
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
    uint32                   base_address;         //0xDE00
    pm_register_address_type status1;              //0x08
    pm_register_address_type output_voltage;       //0x41
    pm_register_address_type ring_suppression_ctl; //0x42
    pm_register_address_type vref_sel;             //0x43
    pm_register_address_type lcd_amoled_sel;       //0x44
    pm_register_address_type module_rdy;           //0x45
    pm_register_address_type enable_ctl;           //0x46
    pm_register_address_type pd_ctl;               //0x47
    pm_register_address_type clk_div;              //0x48
    pm_register_address_type ibb_en_rdy;           //0x49
    pm_register_address_type current_limit;        //0x4B
    pm_register_address_type current_sense;        //0x4C
    pm_register_address_type loop_comp_ctl;        //0x4D
    pm_register_address_type slope_comp_current;   //0x4E
    pm_register_address_type ip_zero_detector;     //0x4F
    pm_register_address_type ps_ctl;               //0x50
    pm_register_address_type pwm_ctl_1;            //0x51
    pm_register_address_type pwm_ctl_2;            //0x52
    pm_register_address_type rdson_mngmnt;         //0x53
    pm_register_address_type short_circuit_ctl;    //0x54
    pm_register_address_type vreg_ok_ctl;          //0x55
    pm_register_address_type afvc;                 //0x5D
    pm_register_address_type precharge_ctl;        //0x5E
    pm_register_address_type soft_start_ctl;       //0x5F
    pm_register_address_type spare_ctl;            //0x60
    pm_register_address_type sec_access;           //0xD0
    pm_register_address_type debouncer_override;   //0xEB
}pm_lab_register_info_type; 


typedef struct
{
    pm_comm_info_type            *comm_ptr;
    pm_lab_register_info_type    *lab_reg_table;
    uint8                        num_of_peripherals;
}pm_lab_data_type;


/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_lab_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_lab_data_type* pm_lab_get_data(uint8 pmic_index);

uint8 pm_lab_get_num_peripherals(uint8 pmic_index);



#endif // PM_LAB_DRIVER__H
