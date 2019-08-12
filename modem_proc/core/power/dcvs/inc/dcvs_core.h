#ifndef DCVS_CORE_H
#define DCVS_CORE_H
/**********************************************************************
 * dcvs_core.h
 *
 * This module has the declarations for the implementation of DCVS 
 * algorithm
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009 - 2011 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/inc/dcvs_core.h#1 $
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ----------------------------------------------------------------------------

09/16/11   sg      Added prototype for DCVSCoreMipsToMBpsConvert().
02/18/11   ss      Adding context based processing for DCVS.
10/22/10   ss      Adding DCVSCoreClockPlanIndexGet API.
10/08/10   ss      Adding a new clock table structure of type DCVS_TARGET_CLOCK_PLAN_INFO_TYPE 
                   and removing the old structure.
06/17/10   ss      Removing unused interfaces DCVSCoreMonitorEnable, 
                   DCVSCoreMipsSet, DCVSCoreDisable, DCVSCoreEnable,
                   DCVSCoreMonitorDisable.
03/30/10   ss      Removing DCVSCoreCpuMonitor as a public API
02/21/10   ss      Adding the interface DCVSCoreCPUMonitor and updating 
                   the comments for headers of the other interfaces.
01/20/10   bjs     Cleanup
08/25/09   ss      Initial check in.
===============================================================================================*/

/*=======================================================================

                          INCLUDE FILES

========================================================================*/

#include "DALSys.h"
#include "DALStdDef.h"
#include "dcvs_algorithm.h"
#include "ULog.h"
#include "dcvs_target.h"



/*=======================================================================

                  PUBLIC FUNCTION DECLARATIONS

========================================================================*/


/**
  @brief DCVSCoreStart
  
  Function to start the DCVS algorithm .

  @param context: The DCVS data context.

  @return : None

*/
void DCVSCoreStart(DCVS_CONTEXT *context);

/**
  @brief DCVSCoreInitialize

  This function is used to initialize teh core processing module of DCVS.

  @param context: DCVS data context. .

  @return : None

*/
void DCVSCoreInitialize(DCVS_CONTEXT* context);

/**
  @brief DCVSCoreMipsGet
  
  Function to calculate the new mips of the system by applying the actual DCVS algorithm.

  @param context: The DCVS data context.

  @return : The new mips of the system.

*/
uint32 DCVSCoreMipsGet (DCVS_CONTEXT* context);

/**
  @brief DCVSCoreCheckEnabled
  
  This function is used to check if the DCVS algorithm is enabled.

  @param context: The DCVS data context.

  @return : TRUE : If Algorithm is enabled.
            FALSE: If Algorithm is disabled.

*/
uint32 DCVSCoreCheckEnabled(DCVS_CONTEXT* context);

/**
  @brief DCVSCoreTargetMaxFreqGet
  
  Function to get the maximum frequeny in mips supported by a target.

  @param context: The DCVS data context.

  @return : The maximum frequency supported by this target in mips.

*/
uint32 DCVSCoreTargetMaxFreqGet(DCVS_CONTEXT* context);


/**
  @brief DCVSCoreClockPlanGet
  
  This function returns the pointer to clock plan information for the target.

  @param context: The DCVS data context.
  @return pointer to clock plan information for the target.

*/
DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* DCVSCoreClockPlanGet(DCVS_CONTEXT* context);


/**
  @brief DCVSCoreLogHandleGet
  
  This function is used to return the log handle of ULOG for a particular core.
  Each core logs the information using its induvidual logging mechanism. 

  @param context: The DCVS data context.
  @return ULogHandle which is initialized and usable by the algorithms.

*/
ULogHandle DCVSCoreLogHandleGet(DCVS_CONTEXT* context);


/**
  @brief DCVSTargetClockPlanInfoGet
  This functions returns the clock plan information which is particular to 
  each target.
  This information consists of the target defined clock plan table ( if any ) 
  and the target defined conversion functions ( if any )
  @param none.
  @return The clock plan info for the target.
*/
DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* DCVSTargetClockPlanInfoGet(void);

/**
  @brief DCVSCoreClockPlanIndexGet

  This function is used to lookup the mips from the clock plan of the target . 
  The Mips searched for would be equal to or the next nearest mips value from the clock plan table.

  @param  : mips value to be looked up.
  @param context : The DCVS data context.

  @return : index where the mips value is found in the table. If the index 
          returned is negative, the aggregated MIPS is zero and DCVS will relinquish its votes.

*/
int32 DCVSCoreClockPlanIndexGet(uint32 mips, DCVS_CONTEXT* context);


/**
  @brief DCVSCoreMipsToMBpsConvert
  This function is the default conversion from Mips to MBps ( Mega Bytes per second ).
  This function is used when the target doesnt define the conversion function.

  @param mips  : The input mips which needs to be converted.
  @return      : The value in MBps.
	
*/
uint32 DCVSCoreMipsToMBpsConvert(uint32 mips);

#endif /* DCVS_CORE_H */
