#ifndef MEMBARRIER_H
#define MEMBARRIER_H

/*============================================================================
  @file MemBarrier.h

  Memory barrier macros for multi-core support.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/rpm.bf/2.1.1/core/power/utils/inc/MemBarrier.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#ifdef WINSIM
#include "windows.h"
#else
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#ifdef WINSIM
// Windows Simulation environment memory barrier.

#define CORE_MEM_BARRIER()      MemoryBarrier()

#else
// Memory barrier for additional platforms.

#define CORE_MEM_BARRIER()     

#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif  // MEMBARRIER_H

