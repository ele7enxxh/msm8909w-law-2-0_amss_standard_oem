#ifndef PM_RTC_DRIVER__H
#define PM_RTC_DRIVER__H

/*! \file pm_rtc_driver.h
 *  \n
 *  \brief This file contains functions prototypes and variable/type/constant
 *          declarations for supporting RTC peripheral  
 *  \n  
 *  \n &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/rtc/src/pm_rtc_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
11/08/13   kt      Created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_comm.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/
/*----------------------------------------------------------------------------
 * PERIPHERAL: RTC_WR (PMIO)
 *--------------------------------------------------------------------------*/

#define RTC_WR_BASE                                     0x00006000
#define PMIO_RTC_WR_EN_CTL1_ADDR                        (RTC_WR_BASE      + 0x00000046)
#define PMIO_RTC_WR_RDATA0_ADDR                         (RTC_WR_BASE      + 0x00000048)
#define PMIO_RTC_WR_RDATA1_ADDR                         (RTC_WR_BASE      + 0x00000049)
#define PMIO_RTC_WR_RDATA2_ADDR                         (RTC_WR_BASE      + 0x0000004a)
#define PMIO_RTC_WR_RDATA3_ADDR                         (RTC_WR_BASE      + 0x0000004b)

/** 
  @struct pm_rtc_data_type
  @brief  This struct is used to store the PMIC RTC 
          driver related data.
          periph_exists: Flag to indicate whether peripheral
                         exists on specific device or not.
          comm_ptr: Pointer to the PMIC Communication
                    information and interface.
 */
typedef struct 
{
  pm_comm_info_type   *comm_ptr;
}pm_rtc_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_rtc_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_rtc_data_type* pm_rtc_get_data(uint8 pmic_index);

#endif // PM_RTC_DRIVER__H
