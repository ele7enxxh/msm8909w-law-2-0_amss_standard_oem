#ifndef PM_COINCELL_DRIVER__H
#define PM_COINCELL_DRIVER__H

/*! \file pm_coincell_driver.h
*  \n
*  \brief  This file contains functions prototypes and variable/type/constant
*         declarations for supporting coincell peripheral.
*
*  \n &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/coincell/src/pm_coincell_driver.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
01/29/15   mr       Added support for Coincell Driver (CR-688196)
09/15/14   akm      Created
========================================================================== */

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_target_information.h"
#include "pm_comm.h"


/*===========================================================================

                COINCELL TYPES AND STRUCTURES

===========================================================================*/
typedef struct
{
  pm_register_address_type base_address;
  pm_register_address_type peripheral_offset;
  pm_register_address_type en_ctl;                  //0x046
  pm_register_address_type coin_chg_rset;           //0x044
  pm_register_address_type coin_chg_vset;           //0x045

} pm_coincell_register_info_type;


typedef struct
{
    pm_comm_info_type               *comm_ptr;
    pm_coincell_register_info_type  *coincell_reg_table;
    uint8                           num_of_peripherals;
} pm_coincell_data_type;

/*===========================================================================

                     FUNCTION DECLARATION

===========================================================================*/
void pm_coincell_driver_init(pm_comm_info_type *comm_ptr,
                             peripheral_info_type * peripheral_info,
                             uint8 pmic_index);

pm_coincell_data_type* pm_coincell_get_data(uint8 pmic_index);

#endif    /* PM_COINCELL_DRIVER__H */

