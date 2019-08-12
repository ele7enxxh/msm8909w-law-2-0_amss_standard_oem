#ifndef BOOT_EXTERN_SUBSYS_INTERFACE_H
#define BOOT_EXTERN_SUBSYS_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN CLK DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external clock drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_subsys_interface.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/12   kedara   Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "ClockPIL.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* ============================================================================
**  Function : boot_clock_setup_processor
** ============================================================================
*/
/*!
    This function will setup the clocks for desired processor

    @param
      None
    @return
      True if success, False otherwise

    @dependencies
    None.

    @sa None
*/
#ifdef FEATURE_BOOT_EXTERN_SUBSYS_COMPLETED
boolean boot_clock_setup_processor(ClockProcessorType eProcessorType);
#else
static inline boolean boot_clock_setup_processor
(
  ClockProcessorType eProcessorType
)
{
  return FALSE;
}
#endif

/* =========================================================================
**  Function : boot_clock_enable_processor
** =========================================================================*/
/**
  Brings processor out of reset.

  This function brings processor out of reset.  A client is expected to have
  called Clock_SetupProcessor before calling this function.

  @param eProcessorType [in] -- Processor to bring out of reset.

  @return
  TRUE  -- Operation is successful.\n
  FALSE -- Operation is unsuccessful.

  @dependencies
  - All required global clock control registers are accessible.
  - Processor core clocks are enabled.
  - Boot address and core memory are configured.
  - Processor core reset register is accessible.

  @sa
  Clock_SetupProcessor
*/
#ifdef FEATURE_BOOT_EXTERN_SUBSYS_COMPLETED
boolean boot_clock_enable_processor(ClockProcessorType eProcessorType);
#else
static inline boolean boot_clock_enable_processor
(
  ClockProcessorType eProcessorType
)
{
  return FALSE;
}
#endif

#endif /* BOOT_EXTERN_SUBSYS_INTERFACE_H */
