#ifndef CORE_TIME_H
#define CORE_TIME_H

/*==============================================================================
  @file CoreTime.h

  Time related Function/Structure Declarations

  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/rpm.bf/2.1.1/core/power/utils/inc/CoreTime.h#1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#ifdef USES_BLAST 
#include "blast.h"
#include "atomic_ops.h"
#include "qube.h"
#include "qtimer.h"
#elif (defined (WINSIM) || defined (WIN_DEBUGGER_EXTENSION))
#include "windows.h"
#else
#include "timetick.h"
#endif 

/**
   @brief CoreTimetick_Get - Get the current system
          timetick.
   
   Read the 32 khz timetick value, and return it.

   @return Current 32 KHz timetick
*/
#ifdef USES_BLAST 
#define CoreTimetick_Get             blast_system_sclk_attr_gethwticks
#elif (defined (WINSIM) || defined (WIN_DEBUGGER_EXTENSION))
#define CoreTimetick_Get             GetTickCount
#else
#define CoreTimetick_Get             timetick_get_safe
#endif


#endif /* CORE_TIME_H */
