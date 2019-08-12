#ifndef PM_LBC_DRIVER__H
#define PM_LBC_DRIVER__H

/*! \file
*  \n
*  \brief  pm_lbc_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting lbc peripheral 
*  
*  \n &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/lbc/src/pm_lbc_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/14   pxm     Created
========================================================================== */

/*===========================================================================
						INCLUDE FILES
===========================================================================*/
#include "pm_target_information.h"

//LBC Types
typedef enum
{
    PM_LBC_CHGR        = 0,  //0x1000
    PM_LBC_BUCK        = 1,  //0x1100
    PM_LBC_BAT_IF      = 2,  //0x1200
    PM_LBC_CHGPTH      = 3,  //0x1300
    PM_LBC_BOOST       = 5,  //0x1500
    PM_LBC_MISC        = 6,  //0x1600
    PM_LBC_FREQ        = 7,  //0x1700
    //PM_CHG_BSI          = 11, //0x1B00
    PM_LBC_TYPE_INVALID
}pm_lbc_type;

typedef struct
{                  
  pm_register_address_type base_address;  
  pm_register_address_type peripheral_offset;
  pm_register_address_type chg_option;         //0x1008
  pm_register_address_type vdd_max;            //0x1040
  pm_register_address_type vdd_safe;           //0x1041
  pm_register_address_type ibat_max;           //0x1044  
  pm_register_address_type ibat_safe;          //0x1045
  pm_register_address_type vin_min;            //0x1047
  pm_register_address_type chg_ctrl;           //0x1049
  pm_register_address_type atc_led;            //0x104D
  pm_register_address_type vbat_trkl;          //0x1050
  pm_register_address_type vbat_weak;          //0x1052
  pm_register_address_type ibat_atc_b;         //0x1055
  pm_register_address_type wdog_pet;           //0x1064
  pm_register_address_type wdog_en;            //0x1065
  pm_register_address_type sec_access;         //0x10D0
  pm_register_address_type comp_over1;         //0x10EE

  pm_register_address_type bat_pres_status;    //0x1208
  pm_register_address_type usb_chg_pth_sts;    //0x1309 
  pm_register_address_type usb_susp;           //0x1347
  pm_register_address_type enum_timer_stop;    //0x134E
  pm_register_address_type boot_done;          //0x1642

} pm_lbc_register_info_type;

typedef struct pm_lbc_data_type
{
	pm_comm_info_type  *comm_ptr;
    pm_lbc_register_info_type* lbc_reg_table;
    uint8 num_of_peripherals;
} pm_lbc_data_type;

void pm_lbc_driver_init(pm_comm_info_type *comm_ptr, uint8 pmic_index);

pm_lbc_data_type* pm_lbc_get_data(uint8 pmic_index);

#endif // PM_LBC_DRIVER__H


