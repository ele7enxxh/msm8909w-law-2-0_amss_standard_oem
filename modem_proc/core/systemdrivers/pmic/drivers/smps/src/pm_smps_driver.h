#ifndef PM_SMPS_DRIVER__H
#define PM_SMPS_DRIVER__H

/*! \file pm_smps_driver.h
 *  \n
 *  \brief
 *  \details
 *  \n &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/smps/src/pm_smps_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Re-architecture module driver to peripheral driver

========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_dal_prop_ids.h"
#include "pm_target_information.h"
#include "pm_pwr_alg.h"
#include "pm_malloc.h"


/*===========================================================================

                     SMPS TYPES AND STRUCTURES

===========================================================================*/
typedef enum
{
    SMPS_PERPH_SUBTYPE_0X,
    SMPS_PERPH_SUBTYPE_1X,
    SMPS_PERPH_SUBTYPE_2X,
    SMPS_PERPH_SUBTYPE_3X,
    SMPS_PERPH_SUBTYPE_INVALID,
}pm_smps_perph_subtype_type;

typedef struct
{
   pm_register_data_type smps_ilimit_reg_data_lut;
   uint16 smps_ilimit_2x_lut;
   uint16 smps_ilimit_3x_lut;
}SmpsILimDS;

typedef struct
{
    pm_comm_info_type        *comm_ptr;
    pm_pwr_data_type  pm_pwr_data;
} pm_smps_data_type;

/*===========================================================================

                     FUNCTION DECLARATION

===========================================================================*/
void pm_smps_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_smps_data_type* pm_smps_get_data(uint8 pmic_index);
uint8 pm_smps_get_num_peripherals(uint8 pmic_index);

#endif // PM_SMPS_DRIVER__H
