/*! \file pm_rtc_driver.c
*  \n
*  \brief This file contains PON peripheral driver initialization during which the driver
*         driver data is stored.
*  \n
*  &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
01/20/14   kt      Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rtc_driver.h"
#include "hw_module_type.h"
#include "device_info.h"
#include "CoreVerify.h"
#include "pm_utils.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the RTC data */
static pm_rtc_data_type *pm_rtc_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

void pm_rtc_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
   pm_rtc_data_type *rtc_ptr = NULL;

   rtc_ptr =  pm_rtc_data_arr[pmic_index];
   if(rtc_ptr == NULL)
   {
     pm_malloc(sizeof(pm_rtc_data_type),(void**)&rtc_ptr);
     pm_rtc_data_arr[pmic_index] = rtc_ptr;
   }

   /* Currently the API added only supports RTC_RW peripheral subtype */
   if (peripheral_info->peripheral_subtype == PM_HW_MODULE_RTC_RTC_RW)
   {
       rtc_ptr->comm_ptr = comm_ptr;
   }
}

pm_rtc_data_type* pm_rtc_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
  {
      return pm_rtc_data_arr[pmic_index];
  }

  return NULL;
}
