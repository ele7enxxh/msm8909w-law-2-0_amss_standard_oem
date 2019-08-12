/*! \file pm_rtc.c
*  \n
*  \brief This file contains the implementation of the public APIs for PMIC RTC module.
*  \n
*  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/drivers/rtc/src/pm_rtc.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/08/13   kt      Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rtc.h"
#include "pm_rtc_driver.h"
#include "hw_module_type.h"
#include "pm_comm.h"

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_rtc_get_time(uint8 pmic_index, uint32 *time_ptr)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_index);
    pm_register_data_type rtc_enable = 0;
    
    if (rtc_ptr == NULL)
    { 
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (time_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
       err_flag = pm_comm_read_byte_mask(rtc_ptr->comm_ptr->slave_id, PMIO_RTC_WR_EN_CTL1_ADDR, 0x80, &rtc_enable,0);

       if (err_flag == PM_ERR_FLAG__SUCCESS)
       {
          if (rtc_enable == 0)
          {
             err_flag = PM_ERR_FLAG__RTC_HALTED;
          }
       }

       err_flag |= pm_comm_read_byte_array(rtc_ptr->comm_ptr->slave_id, PMIO_RTC_WR_RDATA0_ADDR, 4, (uint8*)time_ptr, 0);
    }

    return err_flag; 
}

