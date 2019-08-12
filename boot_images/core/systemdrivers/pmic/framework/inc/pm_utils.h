#ifndef PM_UTILS_H
#define PM_UTILS_H

/*! \file
 *  
 *  \brief  pm_utils.h ----This file contain PMIC wrapper function of DALSYS_Malloc()
 *  \details his file contain PMIC wrapper function of DALSYS_Malloc()
 *  
 *    &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/inc/pm_utils.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/14   aab     Added pm_boot_adc_get_mv()
06/11/13   hs      Adding settling time for regulators.
06/20/12   hs      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
//#include "time_service.h"
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "AdcBoot.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

extern void pm_malloc(uint32 dwSize, void **ppMem);

extern uint64 pm_convert_time_to_timetick(uint64 time_us);

extern uint64 pm_convert_timetick_to_time(uint64 time_tick);


pm_err_flag_type
pm_boot_adc_get_mv(const char *pszInputName, uint32 *battery_voltage);

pm_err_flag_type
pm_clk_busy_wait ( uint32 uS );

#endif // PM_UTILS_H
