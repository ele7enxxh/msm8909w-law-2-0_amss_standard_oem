#ifndef PM_MPP_DRIVER__H
#define PM_MPP_DRIVER__H

/*! \file
*  \n
*  \brief  pm_mpp_driver.h
*  \details  This file contains functions prototypes and variable/type/constant
declarations for supporting MPP pin services for the Qualcomm
PMIC chip set.
*  \n &copy; Copyright 2009 - 2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/mpp/src/pm_mpp_driver.h#1 $
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/12   hs      Updated the driver.
04/10/12   hs      Changed the type for internalResourceIndex from int to uint8.
                   Removed deprecated APIs.
03/02/12   hs      Updated the driver.
08/18/11   wra     Fixing Klocwork Warnings
04/05/11   hs      Refactored the code.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "CoreVerify.h"

#include "pm_err_flags.h"
#include "pm_dal_prop_ids.h"
#include "pm_target_information.h"
#include "pm_malloc.h"


/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/
typedef struct pm_mpp_register_info_type
{
    pm_register_address_type    base_address;
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    status;                  //0x08
    pm_register_address_type    mode_ctl;                //0x40
    pm_register_address_type    dig_pull_ctl;            //0x42
    pm_register_address_type    dig_in_ctl;              //0x43
    pm_register_address_type    dig_vin_ctl;             //0x41
    pm_register_address_type    en_ctl;                  //0x46
    pm_register_address_type    sink_ctl;                //0x4C
    pm_register_address_type    ana_in_ctl;              //0x4A
    pm_register_address_type    ana_out_ctl;             //0x48
}pm_mpp_register_info_type;

typedef struct pm_mpp_specific_info_type
{
    pm_register_address_type   pm_drv_test_acc_rw; //TEST_ACCESS register
    uint8                      num_of_dbus; // Number of DBUS lines
} pm_mpp_specific_info_type;

typedef struct pm_mpp_data_type
{
    pm_comm_info_type        *comm_ptr;

    pm_mpp_register_info_type* mpp_register;
    pm_mpp_specific_info_type* mpp_common;

    int* m_dbus_busy;
    uint32* m_mpp_using_abus;
    uint32 m_abus_in_use;
    uint32 m_mpp_with_shunt_cap_list_status;
    uint8 num_of_peripherals;
}pm_mpp_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_mpp_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_mpp_data_type* pm_mpp_get_data(uint8 pmic_index);

uint8 pm_mpp_get_num_peripherals(uint8 pmic_index);

#endif //PM_MPP_DRIVER__H
