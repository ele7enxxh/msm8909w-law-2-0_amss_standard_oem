#ifndef __CLOCKTZ_H__
#define __CLOCKTZ_H__
/*
===========================================================================
*/
/**
  @file ClockTZ.h

  Public interface include file for the TZ clock driver.
*/
/*
  ====================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/ClockTZ.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"
#include "DALSys.h"

/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * Clock ID type.
 */
typedef void *ClockId;

/**
 * Clock RPM ID type.
 */
typedef void *ClockRPMId;

/**
 * Clock group.
 *
 * This type defines logical groups of related clocks.
 */
typedef enum
{
  CLOCK_GROUP_ABT,    /**< ABT group. */
  CLOCK_GROUP_BUS,    /**< Bus access group.
                           Note: Only call prior to milestone, or before a
                                 planned ERR_FATAL. */
  CLOCK_GROUP_INIT,   /**< Init group. Includes clocks for XPU/VMIDMT config.
                           Note: Only call before a planned ERR_FATAL. */
  CLOCK_GROUP_QDSS,   /**< QDSS group.
                           Note: Only call before a planned ERR_FATAL. */

  CLOCK_GROUP_TOTAL
} ClockGroupType;


/**
 * Processor type.
 *
 * This type lists core processors running on different subsystems.
 */
typedef enum
{
  CLOCK_PROCESSOR_LPASS      = 0,  /**< LPASS Processor.  */
  CLOCK_PROCESSOR_MODEM      = 1,  /**< Modem Processor   */
  CLOCK_PROCESSOR_WCN        = 2,  /**< WCN Processor.    */
  CLOCK_PROCESSOR_MAPLE      = 3,  /**< VPU/MAPLE Processor. */
  CLOCK_PROCESSOR_BCSS       = 4,  /**< BCSS Processor. */
  CLOCK_PROCESSOR_RPM        = 5   /**< RPM Processor. */
} ClockProcessorType;


/*=========================================================================
      Interface Definitions
==========================================================================*/


/*=========================================================================
      Interface Declarations
==========================================================================*/

/**
  @addtogroup clock_functions
  @{
*/

/* =========================================================================
**  Function : Clock_Init
** =========================================================================*/
/**
  Enables clocks necessary for TZ initialization.

  @dependencies
  None.
*/

DALResult Clock_Init (void);


/* =========================================================================
**  Function : Clock_TZInitDone
** =========================================================================*/
/**
  Disables clocks which were necessary for TZ initialization.

  This function disables clocks which were enabled for TZ initialization,
  but are no longer required afterward. This function should be called at the
  end of TZ initialization.

  @dependencies
  None.
*/

DALResult Clock_TZInitDone (void);


/* =========================================================================
**  Function : Clock_GetClockId
** =========================================================================*/
/**
  Get a clock ID.

  This function looks up a clock ID based on the supplied string. The ID
  can then be used to make requests on the clock.

  @dependencies
  Clock_Init() must have been called.
*/

DALResult Clock_GetClockId
(
  const char *szName,
  ClockId    *pnID
);


/* =========================================================================
**  Function : Clock_EnableClock
** =========================================================================*/
/**
  Enable a clock.

  @dependencies
  Clock_Init() must have been called.
*/

DALResult Clock_EnableClock
(
  ClockId nID
);


/* =========================================================================
**  Function : Clock_DisableClock
** =========================================================================*/
/**
  Disable a clock.

  @dependencies
  Clock_Init() must have been called.
*/

DALResult Clock_DisableClock
(
  ClockId nID
);


/* =========================================================================
**  Function : Clock_SetClockFrequency
** =========================================================================*/
/**
  Set a clock frequency.

  @dependencies
  Clock_Init() must have been called.
*/

DALResult Clock_SetClockFrequency
(
  ClockId nID,
  uint32  nMinFreqKHz,
  uint32 *nResultKHz
);


/* =========================================================================
**  Function : Clock_EnableClockGroup
** =========================================================================*/
/**
  Enable a clock group.

  This function enables all clocks in a specified group, as well as any
  necessary power domains.

  @dependencies
  Clock_Init() must have been called.
*/
DALResult Clock_EnableClockGroup (ClockGroupType eGroup);


/* =========================================================================
**  Function : Clock_DisableClockGroup
** =========================================================================*/
/**
  Disable a clock group.

  This function disables all clocks in a specified group, as well as any
  necessary power domains.

  Note: This function will not disable a clock if it was originally enabled
        before Clock_EnableClockGroup() was called.

  @dependencies
  Clock_Init() must have been called.
*/
DALResult Clock_DisableClockGroup (ClockGroupType eGroup);


/* =========================================================================
**  Function : Clock_GetRPMId
** =========================================================================*/
/**
  Get an RPM ID.

  This function looks up an RPM ID based on the supplied string. The ID
  can then be used to make requests on the RPM resource.

  @dependencies
  Clock_Init() must have been called.
*/
DALResult Clock_GetRPMId (const char *szName, ClockRPMId *pnID);


/* =========================================================================
**  Function : Clock_RPMRequest
** =========================================================================*/
/**
  Issue a clock RPM request.

  This function issues an RPM request on the resource specified by the supplied
  ID.

  @dependencies
  Clock_Init() must have been called.
*/
DALResult Clock_RPMRequest (ClockRPMId nID, uint32 nRequest);


/* =========================================================================
**  Function : Clock_SetRPMAvailable
** =========================================================================*/
/**
  Inform the clock driver that RPM has been initialized.

  @dependencies
  Clock_Init() must have been called.
  RPM must be initialized.
*/
DALResult Clock_SetRPMAvailable (void);


/* =========================================================================
**  Function : Clock_SetupProcessor
** =========================================================================*/
/**
  Sets up processor clocks.

  This function enables required clocks on a given processor.  It initializes
  required PLL (if necessary) and runs the processor core and bus clocks from
  that PLL.  In addition, the function takes HW block(s) (but NOT core) out
  of reset to setup appropriate clocks.

  @param eProcessorType [in] -- Processor to enable clocks for.

  @return
  TRUE  -- Operation is successful.\n
  FALSE -- Operation is unsuccessful.

  @dependencies
  All required global clock control registers are accessible.
*/

boolean Clock_SetupProcessor(ClockProcessorType eProcessorType);


/* =========================================================================
**  Function : Clock_EnableProcessor
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

boolean Clock_EnableProcessor(ClockProcessorType eProcessorType);


/* =========================================================================
**  Function : Clock_ResetProcessor
** =========================================================================*/
/**
  Puts processor into reset.

  This function disables clocks (setup during Clock_EnableProcessor
  function call) and then puts processor into reset.

  @param eProcessorType [in] -- Processor to put into reset.

  @return
  TRUE  -- Operation is successful.\n
  FALSE -- Operation is unsuccessful.

  @dependencies
  All required global clock control registers are accessible.

  @sa
  Clock_EnableProcessor
*/

boolean Clock_ResetProcessor(ClockProcessorType eProcessorType);


/* =========================================================================
**  Function : Clock_GetAppsAXIFrequency
** =========================================================================*/
/**
  Returns gcc_apps_axi clock frequency, read from the RPM msg RAM in pnFrequencyHz.

  @dependencies
  Clock_Init() must have been called.
*/

DALResult Clock_GetAppsAXIFrequency
(
  uint32    *pnFrequencyHz
);


#endif /* !__CLOCKTZ_H__ */

