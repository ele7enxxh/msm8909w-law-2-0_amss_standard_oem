/**********************************************************************
 * disabled.c
 *
 * This module implements the disabled algorithm for multicore
 * Dynamic Clock and Voltage Scaling(DCVS).
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2011-2012 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/algorithms/disabled/disabled.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    --------------------------------------------------------------------
03/23/12   nj      Removed unused include file 
04/26/11   ss      Fixing Q6 compiler warnings.
02/18/11   ss      Adding context based processing for DCVS.
01/24/11   ss      Removing the OAL component of the algorithm.
10/08/10   ss      Using the new DCVS clock plan data structure in the disabled algorithm.
01/18/10   bjs     Initial check in.
01/20/10   bjs     Cleanup
========================================================================================*/

#include "BuildFeatures.h"
#ifndef DEFEATURE_DCVS_ALGO_DISABLED
#include "dcvs_os.h"
#include "dcvs_algorithm.h"
#include "dcvs_core.h"
#include "ULogFront.h"
#include "CoreVerify.h"

/* Disabled algorithm specific members */
typedef struct
{
  /* The fixed mips the system needs to be set to. */
  uint32 mips;
}DCVS_ALGO_DISABLED;

// No parameters
//typedef enum
//{
//} DCVS_PARAMS_ALG_DISABLED;

static const DCVS_NV_PARAM_ENTRY dcvsParamsAlgDisabled[] =
{
  DCVS_NV_PARAM_LIST_END
};

/**
  @brief DCVSCoreAlgoDisabledInitialize
  
  This function is used to initialize the Disabled DCVS algorithm specific members.

  @param context: The DCVS data context .

  @return : Initial mips of the system

*/
static uint32 DCVSCoreAlgoDisabledInitialize(void* context)
{  
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_DISABLED* algoDataContext;
  DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* clockPlan;

  /* Allocate the memory for the algorithm data context. */
  if(DALSYS_Malloc(sizeof(DCVS_ALGO_DISABLED),  
                   (void**) &coreContext->algoDataContext)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }
  algoDataContext = (DCVS_ALGO_DISABLED*)coreContext->algoDataContext;

  /* Get the current clock plan of the target */
  clockPlan = DCVSCoreClockPlanGet(context);

  algoDataContext->mips = clockPlan->clockPlanTable[clockPlan->numLevels-1].reqMips;

  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoDisabledBuildParams
  
  This function is used to get the parameters for the Disabled DCVS algorithm.

  @param context: The DCVS data context.

  @return: None

*/
static void DCVSCoreAlgoDisabledBuildParams(void* context)
{
  /* Empty implementation. */
}/* DCVSCoreAlgoDisabledBuildParams */


/**
  @brief DCVSCoreAlgoDisabledReset
  
   This function is used to reset the Disabled algorithm specific members .

  @param context: The DCVS data context.

  @return : The Disabled mips requested by the client.

*/
static uint32 DCVSCoreAlgoDisabledReset(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_DISABLED* algoDataContext = (DCVS_ALGO_DISABLED*)coreContext->algoDataContext;

  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoDisabledEnable
  
  This function is used to set the state of the Disabled algorithm specific members
  when enabling the algorithm.

  @param  context: The DCVS data context.

  @return : The Disabled mips requested by the client.

*/
static uint32 DCVSCoreAlgoDisabledEnable(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_DISABLED* algoDataContext = (DCVS_ALGO_DISABLED*)coreContext->algoDataContext;

  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoDisabledDisable
  
  This function is used to reset the state of the Disabled algorithm specific members
  when disabling the algorithm.

  @param context: The DCVS data context.

  @return : The maximum mips of the system

*/
static uint32 DCVSCoreAlgoDisabledDisable(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_DISABLED* algoDataContext = (DCVS_ALGO_DISABLED*)coreContext->algoDataContext;
  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoDisabledDecideMips
  
  @param context: The DCVS data context.

  @return : The Disabled mips the system needs to be set to.

*/
static uint32 DCVSCoreAlgoDisabledDecideMips(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_DISABLED* algoDataContext = (DCVS_ALGO_DISABLED*)coreContext->algoDataContext;

  return(algoDataContext->mips);
}/* DCVSCoreAlgoDisabledDecideMips */


DCVS_CORE_ALGO disabledAlgorithm = 
{
  "Disabled",
  dcvsParamsAlgDisabled,
  DCVSCoreAlgoDisabledInitialize,
  DCVSCoreAlgoDisabledBuildParams,
  DCVSCoreAlgoDisabledDecideMips,
  DCVSCoreAlgoDisabledReset,
  DCVSCoreAlgoDisabledEnable,
  DCVSCoreAlgoDisabledDisable,
};
#endif //DEFEATURE_DCVS_ALGO_DISABLED

