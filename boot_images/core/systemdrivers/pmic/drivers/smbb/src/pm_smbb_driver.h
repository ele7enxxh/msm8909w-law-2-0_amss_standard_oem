#ifndef PM_SMBB_DRIVER__H
#define PM_SMBB_DRIVER__H

/*! \file
*  \n
*  \brief  pm_smbb_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting smbb peripheral 
*  
*  \n &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/smbb/src/pm_smbb_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/13   aab     Added pm_smbb_chg_gset_comp_override() 
08/06/13   aab     Added pm_smbb_chg_get_chg_gone_rt_status()
06/17/13   aab     Created
========================================================================== */

/*===========================================================================
						INCLUDE FILES
===========================================================================*/
#include "pm_target_information.h"



//SMBB Types
typedef enum
{
    PM_SMBB_CHGR        = 0,  //0x1000
    PM_SMBB_BUCK        = 1,  //0x1100
    PM_SMBB_BAT_IF      = 2,  //0x1200
    PM_SMBB_USB_CHGPTH  = 3,  //0x1300
    PM_SMBB_DC_CHGPATH  = 4,  //0x1400
    PM_SMBB_BOOST       = 5,  //0x1500
    PM_SMBB_MISC        = 6,  //0x1600
    PM_SMBB_FREQ        = 7,  //0x1700
    PM_CHG_BSI          = 11, //0x1B00
    PM_SMBB_TYPE_INVALID
}pm_smbb_type;



typedef struct
{                  
  pm_register_address_type base_address;  
  pm_register_address_type peripheral_offset;
  pm_register_address_type vbat_status;        //0x100B
  pm_register_address_type ibat_max;           //0x1044  
  pm_register_address_type chg_ctrl;           //0x1049
  pm_register_address_type atc_ctrl;           //0x104B
  pm_register_address_type atc_led;            //0x104D
  pm_register_address_type vbat_trkl;          //0x1050
  pm_register_address_type vbat_weak;          //0x1052
  pm_register_address_type ibat_atc_a;         //0x1054 
  pm_register_address_type wdog_pet;           //0x1064
  pm_register_address_type sec_access;         //0x10D0
  pm_register_address_type comp_over1;         //0x10EE

  pm_register_address_type bat_pres_status;    //0x1208
  pm_register_address_type usb_chg_pth_sts;    //0x1309 
  pm_register_address_type chg_gone_rt_status; //0x1310
  pm_register_address_type iusb_max_en;        //0x1343
  pm_register_address_type iusb_max;           //0x1344
  pm_register_address_type flcb_iusb_max_lim;  //0x1351
  pm_register_address_type dc_chg_pth_sts;     //0x140A 
  pm_register_address_type boot_done;          //0x1642

} pm_smbb_register_info_type;

typedef struct pm_smbb_data_type
{
	pm_comm_info_type  *comm_ptr;
    pm_smbb_register_info_type* smbb_reg_table;
    uint8 num_of_peripherals;
} pm_smbb_data_type;

void pm_smbb_driver_init(pm_comm_info_type *comm_ptr, uint8 pmic_index);
pm_smbb_data_type* pm_smbb_get_data(uint8 pmic_index);


#endif // PM_SMBB_DRIVER__H


