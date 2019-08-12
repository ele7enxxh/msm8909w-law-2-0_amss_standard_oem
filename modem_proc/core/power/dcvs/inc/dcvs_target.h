#ifndef DCVS_TARGET_H
#define DCVS_TARGET_H

/*============================================================================
  @file dcvs_target.h
  This file exposes the target specific functionality  of the DCVS module.
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/inc/dcvs_target.h#1 $ */

/*===============================================================================================
when       who     what, where, why
--------   ----    ----------------------------------------------------------------------------
09/11/12   nj      Adding member variable for storing clkticks per second
02/03/12   nj      Adding internal DCVS resource specific members for storing priority values/flags
08/15/11   ss      Adding target specific data context to the DCVS context. 
03/25/11   ss      Adding the structure type for CPU monitor for QDSP.
02/18/11   ss      Adding context based processing for DCVS.
01/21/11   ss      Adding the resource query handle in DCVS_RES_DATA.
10/08/10   ss      Adding the new clock plan information strutures of type DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE
                   and the corresponding retrieval functions.
==================================================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "DALStdDef.h"
#include "npa_resource.h"
#include "CoreEventNotification.h"
#include "ULog.h"
#include "dcvs_algorithm.h"

/*----------------------------------------------------------------------------
 * Type Declarations/Constants
 * -------------------------------------------------------------------------*/
#define TICKS_PER_SECOND 19200000

#define BEST_EFFORT_IGNORED_REQUEST_DURATION 3000

/** 
  Structure having the mapping of requested mips, CPU frequency 
  and Bus AXI frequency. This is the primary clock plan data for the 
  particular target. This may or may not exist on the target.
  If this does not exist then the clock plan can be queried from the 
  resource or the local clock plan table. 
*/
typedef struct
{
  uint32 reqMips;  /* In MIPS */
  uint32 cpuFreq; /* In Khz. */
  uint32 busThroughput;  /* In MBps. */
}DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE;

/** 
  Clock plan data and vtable.
  Every target needs to define this structure. 
*/
typedef struct
{
  /* Pointer to the mips frequency map table. 
     The clock plan table having mappings of mips and processor frequency. */
  DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE* clockPlanTable;

  /* The number of performance levels in the clock plan for the target. */
  uint32 numLevels;

  /* Function to convert mips to Mbps on a target specific basis. 
     Can be NULL if not needed. And the default conversion is applied. */
  uint32 (*MipsToMBpsConvert)(uint32);

  /* Function to convert Mips to KHz on a target specific basis.
     Can be NULL if not needed. And the default conversion is applied. */
  uint32 (*MipsToKhzConvert)(uint32);

  /* Function to convert KHz  to Mips on a target specific basis.
     Can be NULL if not needed. And the default conversion is applied. */
  uint32 (*KhzToMipsConvert)(uint32);

  /* Number of system clock oscillations per second
     Typical sources are sclk on pre-B-Family and QTimer from B-Family
     onwards */
  uint32 clkTicksPerSec;

}DCVS_TARGET_CLOCK_PLAN_INFO_TYPE;

/* Structure to maintain the internal resource state for DCVS  */
typedef struct 
{
  /* Flag to indicate completion of node creation */
  DALBOOL nodeCreateComplete ;

  /* Client handle for the resource for the Timer or Algo mips change callback */
  npa_client_handle handleClient;

  /* Startup client handle for the resource */
  npa_client_handle startupClient;

  /* Client handle issuing zero state requests */
  npa_client_handle zeroRequestClient;

  /* Client handle to change the state of resource */
  npa_client_handle stateChangeClient;

  /* Callback event for state change of clock resource */
  DALSYSEventHandle clockChangeEvent;

  /* Flag to indicate that all components of DCVS is initialized. */
  DALBOOL dcvsInit;

  /* Max mips for the target */
  // ToDo: Probably not necessary
  uint32 maxMips;

  /* Handle to the resource . */
  npa_resource *resHandle;

  /* Handle which is used to determine whether API has been initialized 
     for DCVS NPA events. */
  Core_EventHandle eventHandle;

  /* Core_Event Structure */
  Core_Event notifyEvent;
  
  /* State variables to indicate the floor, isochronous or impulse requests*/ 
  npa_resource_state activeFloor;
  npa_resource_state activeIsochronous;
  DALBOOL activeImpulse;

  /* Flag to indicate if mips calculated by algorithm changed or
     the algorithm timer fired.  */
  DALBOOL algoMipsChangedOrTimerFired;

  /* The delta of the previous and the current client requests. */
  int32 clientDelta;

  /* The clock plan type for use to the clients  */
  DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* clockPlan;

  /* The priority for handling clock change events */
  uint32 clkChangePriority;
  uint32 clkChangePriorityInit;

}DCVS_RES_DATA;


/* Structure to maintain the internal state of the DCVS core module processing
   mechanism */
typedef struct 
{
  uint32 mips;

  /* DCVS enabled or not */
  DALBOOL enabled;

  /* Log handle for algorithm specific log */
  ULogHandle logHandle;

  const DCVS_CORE_ALGO* currentAlgorithm;

  /* Query handle to clock resource */
  npa_query_handle clockQueryHandle;

  /* Query result of clock  resource */
  npa_query_type clkCpuQueryResult;

  /* The resource handle of the clock resource used by the algorithm.  */
  npa_resource *clockResourceHandle;

  /* Structure holding the target specific clock plan information */
  DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* clockPlan;

  /* Variable to indicate that the clock plan data has been queried. */
  DALBOOL clockPlanDataInit;

}DCVS_CORE_DATA;


/* Structure type to maintain the context for the data for DCVS. */
typedef struct 
{
  /* The context for the resource. */
  npa_resource_definition *resourceContext;
  /* The resource data context. */
  DCVS_RES_DATA *resDataContext;
  /* The core processing data context.  */
  DCVS_CORE_DATA *coreDataContext;
  /* The cpu monitor data context */
  void *cpuMonContext;
  /* The algorithm data context. */
  void *algoDataContext;
  /* The OS specific data context. */
  void *osDataContext;
  /* The target specific data context. */
  void *targetDataContext;

  /* The name of the clock dependency .  */
  const char* clockDependency;
  /* The names of external system dependencies */
  const char** sysDeps;
  /* The names of external system dependencies */
  uint32 numSysDeps;

  /* The identifier for this core. */
  uint32 coreId;

}DCVS_CONTEXT;

/** 
  Structure having the local clock plan table of frequencies in KHz supported on the target. 
  This may or may not exist on the target. The order of clock plan table look up would be ::

  *) Primary clock plan table of target.
  *) Clock plan table from the resource.
  *) Local clock plan table of target.
*/
typedef struct
{
  uint32 freqKhz; // In KHz. 
} DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief DCVSTargetSpecificInit - Target specific
         initialization.
  
  This function completes any init that is necessary for a
  specific target.
	
*/
void DCVSTargetSpecificInit (void);


void MCATargetSpecificInit(void);

/**
  @brief DCVSTargetClockPlanLocalGet
  This function returns the clock plan ( local table of
  clock plan frequencies.) This function is invoked 
  *) if the lookup table of mips and frequencies for the target doesnt exist.
	 *) the clock plan cannot be queried from the resource itself.

  @param none.

  @return the local clock plan data  of the target.
*/
DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE* DCVSTargetClockPlanLocalGet(void);


/**
  @brief DCVSTargetClockPlanLocalNumLevelsGet
  This functions returns the number of performance levels of the local clock plan
  of the target. 
  This function is invoked 
  *) if the lookup table of mips and frequencies for the target doesnt exist.
	 *) the clock plan cannot be queried from the resource itself.

  @param none.

  @return The number of levels of the local clock plan of target.
*/
uint32 DCVSTargetClockPlanLocalNumLevelsGet(void);


#endif /*DCVS_TARGET_H*/
