#ifndef PM_MPP_DRIVER__H
#define PM_MPP_DRIVER__H

/*! \file
*  \n
*  \brief  pm_mpp_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
declarations for supporting MPP pin services for the Qualcomm
PMIC chip set.
*  \n &copy; Copyright 2009 - 2014 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/mpp/src/pm_mpp_driver.h#1 $
$DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
12/20/10   wra     Initial check in
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "pm_target_information.h"

/*===========================================================================
                        TYPE DEFINES AND ENUMS
===========================================================================*/
typedef struct pm_mpp_register_info_type
{                  
    pm_register_address_type base_address;  
    pm_register_address_type peripheral_offset;                  
    pm_register_address_type status;                  //0x08
    pm_register_address_type int_rt_sts;              //0x10
    pm_register_address_type int_set_type;            //0x11
    pm_register_address_type int_polarity_high;       //0x12
    pm_register_address_type int_polarity_low;        //0x13
    pm_register_address_type int_latched_clr;         //0x14
    pm_register_address_type int_en_set;              //0x15
    pm_register_address_type int_en_clr;              //0x16
    pm_register_address_type int_latched_sts;         //0x18
    pm_register_address_type int_pending_sts;         //0x19
    pm_register_address_type int_mid_sel;             //0x1A
    pm_register_address_type int_priority;            //0x1B    
    pm_register_address_type mode_ctl;                //0x40
    pm_register_address_type dig_pull_ctl;            //0x42
    pm_register_address_type dig_in_ctl;              //0x43
    pm_register_address_type dig_vin_ctl;             //0x41
    pm_register_address_type en_ctl;                  //0x46
    pm_register_address_type sink_ctl;                //0x4C
    pm_register_address_type ana_in_ctl;              //0x4A
    pm_register_address_type ana_out_ctl;             //0x48
}pm_mpp_register_info_type;


typedef struct pm_mpp_specific_info_type
{
    pm_register_address_type   pm_drv_test_acc_rw; //TEST_ACCESS register
    uint8		               num_of_dbus; // Number of DBUS lines
} pm_mpp_specific_info_type;

typedef struct pm_mpp_data_type
{
	pm_comm_info_type          *comm_ptr;
    pm_mpp_register_info_type*  mpp_reg_table;
    pm_mpp_specific_info_type*  mpp_specific;
    
    int* m_dbus_busy;
    uint32* m_mpp_using_abus;
    uint32 m_abus_in_use;
    uint32 m_mpp_with_shunt_cap_list_status;
    uint8 num_of_peripherals;
} pm_mpp_data_type;

/* MPP config type */
typedef enum pm_drv_mpp_config_type
{
    PM_MPP_DIG_IN,         //- 0: Digital Input
    PM_MPP_DIG_OUT,        //- 1: Digital Output
    PM_MPP_DIG_IN_DIG_OUT, //- 2: Digital Input and Digital Output
    PM_MPP_BI_DIR,         //- 3: Bidirectional Logic
    PM_MPP_ANALOG_IN,      //- 4: Analog Input
    PM_MPP_ANALOG_OUT,     //- 5: Analog Output
    PM_MPP_I_SINK,         //- 6: Current Sink
    PM_MPP_TYPE_INVALID
} pm_drv_mpp_config_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_mpp_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_mpp_data_type* pm_mpp_get_data(uint8 pmic_index);

uint8 pm_mpp_get_num_peripherals(uint8 pmic_index);
#endif //PM_MPP_DRIVER__H

