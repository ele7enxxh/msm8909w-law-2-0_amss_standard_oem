#ifndef __CLOCKCOMMON_H__
#define __CLOCKCOMMON_H__

/*=========================================================================

        C O M M O N    C O D E    D E F I N I T I O N S

 GENERAL DESCRIPTION
   This file contains the common feature between multiple hardwares

 Copyright (c) 2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/src/ClockCommon.h#1 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
03/04/13   vph     Initial release.

==========================================================================*/ 

/*=========================================================================
      Includes
==========================================================================*/


/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/


// Clock Software Event for Processor Sleep
typedef enum
{
  CLOCK_SWE_SLEEP_ENTER = 0,
  CLOCK_SWE_SLEEP_DONE,
} ClockSWEventProcessorSleep;


// Clock Software Event for Processor Sleep
typedef enum
{
  CLOCK_SWE_PROCESSOR_ENTER = 0,
  CLOCK_SWE_PROCESSOR_DONE,
} ClockSWEventProcessor;


// Software Event for BIMC power collapsed
typedef enum
{
  CLOCK_SWE_BIMC_ENTER = 0,
  CLOCK_SWE_BIMC_ENTER_SF,
  CLOCK_SWE_BIMC_ENTER_PC,
  CLOCK_SWE_BIMC_DONE,
  CLOCK_SWE_BIMC_DDR,
  CLOCK_SWE_BIMC_DDR_DONE,
  CLOCK_SWE_BIMC_PWR_DM_ENABLE,
  CLOCK_SWE_BIMC_PWR_DM_DISABLE,
  CLOCK_SWE_BIMC_DEHR_SAVE,
  CLOCK_SWE_BIMC_DEHR_RESTORE,
  CLOCK_SWE_BIMC_DEHR_DONE,
} ClockSWEventBIMC;

/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_SetClockConfigEx
** =========================================================================*/
/**
  Extend function for set a clock configuration with QDSS clock event

  This function configures a clock domain to a particular configuration.
  It is used internally only.

  @param *pClock [in]     -- Clock to configure.
  @param *pNewConfig [in] -- New configuration to use.

  @return
  None

  @dependencies
  The clock mutex must be locked.
*/
void Clock_SetClockConfigEx( ClockNodeType *pClock, ClockMuxConfigType *pNewConfig );

#endif  /* __CLOCKCOMMON_H__ */ 
