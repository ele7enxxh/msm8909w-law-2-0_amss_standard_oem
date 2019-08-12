/*! \file pm_utils.c
 *  
 *  \brief This file contain commonly used PMIC utils related function implementation.
 *  \details This file contain PMIC timetick related helper functions implementation
 *           and wrapper function of DALSYS_Malloc()
 *  
 *    &copy; Copyright 2012-2014 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/src/pm_utils.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/13   hs      Adding settling time for regulators.
06/20/12   hs      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_utils.h"
#include "DALSys.h"
#include <err.h>

/* Global variable to set the current logging level. Values can be
   1) PM_LOG_LEVEL_ERROR
   2) PM_LOG_LEVEL_DEBUG
*/
volatile uint32 pm_log_level = PM_LOG_LEVEL_ERROR;

void pm_malloc(uint32 dwSize, void **ppMem)
{
    DALResult dalResult = DAL_SUCCESS;

    dalResult = DALSYS_Malloc(dwSize, ppMem);

    if( (DAL_SUCCESS != dalResult) || (NULL == *ppMem) )
    {
        ERR_FATAL( "PMIC-Memory allocation failed ", 0, 0, 0 );
    }
    
    memset(*ppMem, 0, dwSize);
}

uint64 pm_convert_time_to_timetick(uint64 time_us)
{
    return (time_us*19200000)/1000000;
}

uint64 pm_convert_timetick_to_time(uint64 time_tick)
{

    return (time_tick*1000000)/19200000;
}

