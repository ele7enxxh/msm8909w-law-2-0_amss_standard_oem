#ifndef __PM_BUA_DRIVER_H__
#define __PM_BUA_DRIVER_H__

/*! \file pm_bua_driver.h
*  \n
*  \brief BUA driver type definitions and function prototypes.
*  \n
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/bua/src/pm_bua_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/13   kt      Initial version
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "CoreVerify.h"

#include "pm_err_flags.h"
#include "hw_module_type.h"
#include "pm_target_information.h"
#include "pm_comm.h"
#include "pm_malloc.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
typedef struct
{
    pm_register_address_type status1;
    pm_register_address_type status2;
    pm_register_address_type int_rt_status;
    pm_register_address_type int_set_type;
    pm_register_address_type int_polarity_high;
    pm_register_address_type int_polarity_low;
    pm_register_address_type int_latched_clr;
    pm_register_address_type int_en_set;
    pm_register_address_type int_en_clr;
    pm_register_address_type int_latched_status;
    pm_register_address_type int_pending_status;
    pm_register_address_type en_ctl1;
}pm_bua_register_info_type;

typedef struct
{
    uint8 num_alarms_supported;
}pm_bua_specific_info_type;

typedef struct
{
    pm_register_address_type base_address;
    uint8 num_peripherals;
}pm_bua_common_info_type;

typedef struct
{
    pm_comm_info_type           *comm_ptr;

    pm_bua_common_info_type*    bua_common;
    pm_bua_register_info_type*  bua_reg_table;
    pm_bua_specific_info_type*  bua_specific;
}pm_bua_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_bua_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info,uint8 pmic_index);

pm_bua_data_type* pm_bua_get_data(uint8 pmic_index);

uint8 pm_bua_get_num_peripherals(uint8 pmic_index);

#endif /* __PM_BUA_DRIVER_H__ */
