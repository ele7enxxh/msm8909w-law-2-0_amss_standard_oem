#ifndef PM_PON_DRIVER__H
#define PM_PON_DRIVER__H

/*! \file pm_pon_driver.h
 *  \n
 *  \brief This file contains functions prototypes and variable/type/constant
*          declarations for supporting PMIC PON peripheral  
 *  \n  
 *  \n &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/drivers/pon/src/pm_pon_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/13   kt      Created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_comm.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUMS

===========================================================================*/
/*----------------------------------------------------------------------------
 * PERIPHERAL: PON (PMIO)
 *--------------------------------------------------------------------------*/

#define PON_BASE                                                          0x00000800
#define PMIO_PON_PMIC_WD_RESET_S1_TIMER_ADDR                              (PON_BASE      + 0x00000054)
#define PMIO_PON_PMIC_WD_RESET_S2_TIMER_ADDR                              (PON_BASE      + 0x00000055)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL_ADDR                                (PON_BASE      + 0x00000056)
#define PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR                               (PON_BASE      + 0x00000057)
#define PMIO_PON_PMIC_WD_RESET_PET_ADDR                                   (PON_BASE      + 0x00000058)
#define PMIO_PON_PERPH_RB_SPARE_ADDR                                      (PON_BASE      + 0x0000008c)
#define PMIO_PON_DVDD_RB_SPARE_ADDR                                       (PON_BASE      + 0x0000008d)

/** 
  @struct pm_pon_data_type
  @brief  This struct is used to store the PMIC PON 
          driver related data.
          periph_exists: Flag to indicate whether peripheral
                         exists on specific device or not.
          comm_ptr: Pointer to the PMIC Communication
                    information and interface.
 */
typedef struct 
{
  pm_comm_info_type   *comm_ptr;
}pm_pon_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_pon_driver_init(pm_comm_info_type *comm_ptr, uint8 pmic_index);

pm_pon_data_type* pm_pon_get_data(uint8 pmic_index);

#endif // PM_PON_DRIVER__H
