#ifndef PM_UTILS_H
#define PM_UTILS_H

/*! \file pm_utils.h
 *  
 *  \brief This file contain commonly used PMIC utils related function prototype.
 *  \details This file contain PMIC timetick related helper functions and
 *           wrapper function of DALSYS_Malloc()
 *  
 *    &copy; Copyright 2012-2014 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/inc/pm_utils.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/13   hs      Adding settling time for regulators.
06/20/12   hs      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "swevent.h"
#include "time_service.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

#define PM_LOG_LEVEL_ERROR 0
#define PM_LOG_LEVEL_DEBUG 1

extern volatile uint32 pm_log_level;

#if defined(PMIC_LOG_MSG_DEBUG)
  #define PM_LOG_MSG_DEBUG(...) if(pm_log_level >= PM_LOG_LEVEL_DEBUG)\
                                {\
                                  SWEVENT(__VA_ARGS__);\
                                }
#else
  #define PM_LOG_MSG_DEBUG(...) 
#endif

#if defined(PMIC_LOG_MSG_ERROR)
  #define PM_LOG_MSG_ERROR(...) SWEVENT(__VA_ARGS__)
#else
  #define PM_LOG_MSG_ERROR(...) 
#endif

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

extern void pm_malloc(uint32 dwSize, void **ppMem);

extern uint64 pm_convert_time_to_timetick(uint64 time_us);

extern uint64 pm_convert_timetick_to_time(uint64 time_tick);

#endif // PM_UTILS_H
