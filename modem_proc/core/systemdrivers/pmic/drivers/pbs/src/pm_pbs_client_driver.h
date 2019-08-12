#ifndef __PM_PBS_CLIENT_DRIVER_H__
#define __PM_PBS_CLIENT_DRIVER_H__

/*! \file pm_pbs_client_driver.h
*  \n
*  \brief PBS Client driver type definitions and function prototypes.
*  \n
*  \n &copy; Copyright 2011-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/pbs/src/pm_pbs_client_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/12   hs      Changed the type for internalResourceIndex from int to uint8.
02/24/12   dy      Add API to set UICC Hotswap Polarity
06/16/11   hs      Added uiccCommonDS to hold NumofMods.
05/23/11   vk      Initial version.
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_target_information.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
typedef struct
{
    pm_register_address_type int_rt_status;
    pm_register_address_type int_set_type;
    pm_register_address_type int_polarity_high;
    pm_register_address_type int_polarity_low;
    pm_register_address_type int_latched_clr;
    pm_register_address_type int_en_set;
    pm_register_address_type int_en_clr;
    pm_register_address_type int_latched_status;
    pm_register_address_type int_pending_status;
    pm_register_address_type trig_cfg;
    pm_register_address_type trig_ctl;
    pm_register_address_type en_ctl1;
}pm_pbs_client_register_info_type;

typedef struct
{
    pm_register_address_type base_address;
    uint8 num_peripherals;
}pm_pbs_client_common_info_type;
typedef struct
{
      pm_comm_info_type        *comm_ptr;

    pm_pbs_client_common_info_type*    pbs_client_common;
    pm_pbs_client_register_info_type*  pbs_client_reg_table;
}pm_pbs_client_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_pbs_client_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_pbs_client_data_type* pm_pbs_client_get_data(uint8 pmic_index);

uint8 pm_pbs_client_get_num_peripherals(uint8 pmic_index);

#endif /* __PM_PBS_CLIENT_DRIVER_H__ */
