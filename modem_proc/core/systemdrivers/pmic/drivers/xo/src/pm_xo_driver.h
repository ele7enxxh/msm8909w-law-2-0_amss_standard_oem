#ifndef PM_XO_DRIVER__H
#define PM_XO_DRIVER__H

/*! \file
*  \n
*  \brief  pm_xo_driver.h
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting XO pin services for the Qualcomm
*  PMIC chip set.
*  \n &copy; Copyright 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/xo/src/pm_xo_driver.h#1 $

when         who      what, where, why
--------     ---      ----------------------------------------------------------
12/10/13   rh      File created
========================================================================== */

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
typedef struct
{
    pm_register_address_type    base_address;
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    voltage_ctl1;   //0x40
    pm_register_address_type    mode_ctl1;      //0x44
    pm_register_address_type    en_ctl;         //0x46
    pm_register_address_type    xo_timer1;      //0x4C
    pm_register_address_type    xo_timer2;      //0x4D
    pm_register_address_type    xo_stepper;     //0x5A
    pm_register_address_type    xo_adj;         //0x5C
    pm_register_address_type    xo_ctl;         //0x5E
}pm_xo_core_register_info_type;

typedef struct
{
    uint16             delay_max;  //Maximum TCXO buffer delay
    uint8              trim_max; //Maximum load capacitance.
}pm_xo_core_specific_info_type;

typedef struct
{
    pm_comm_info_type        *comm_ptr;

    pm_xo_core_register_info_type* xo_core_register;
    pm_xo_core_specific_info_type* xo_core_common;

    uint8 num_of_peripherals;
}pm_xo_core_data_type;
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_xo_core_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_xo_core_data_type* pm_xo_core_get_data(uint8 pmic_index);
uint8 pm_xo_core_get_num_peripherals(uint8 pmic_index);

#endif //PM_XO_DRIVER__H
