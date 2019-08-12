/**********************************************************************
 * requests.c
 *
 * This module implements the requests algorithm for multicore 
 * Dynamic Clock and Voltage Scaling(DCVS) 
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009 - 2010 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=============================================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/algorithms/requests/requests.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ------------------------------------------------------------------------------
04/26/11   ss      Fixing Q6 compiler warnings.
02/18/11   ss     Adding context based processing for DCVS.
01/21/11   ss     Removing the OAL component of the algorithm.
10/08/10   ss     Using the new DCVS clock plan data structure in the requests algorithm.
03/30/10   ss     Initial check in.
================================================================================================*/

#include "BuildFeatures.h"
#ifndef DEFEATURE_DCVS_ALGO_REQUESTS
#include "dcvs_os.h"
#include "dcvs_algorithm.h"
#include "dcvs_core.h"
#include "ULogFront.h"
#include "CoreVerify.h"

/* Requests algorithm specific members */
typedef struct
{
  /* Members specific to the Requests DCVS algorithm */
  /* Structure which stores the clock plan for the particular target */
  DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* clockPlan;

  /* The mips the system needs to be set to. */
  uint32 mips;

}DCVS_ALGO_REQUESTS;

/* No default parameters needed for this algorithm */
static const DCVS_NV_PARAM_ENTRY dcvsParamsAlgRequests[] =
{
  DCVS_NV_PARAM_LIST_END
};

/**
  @brief DCVSCoreAlgoRequestsInitialize
  
  This function is used to initialize the Requests DCVS algorithm specific members.

  @param context: The The DCVS data context.

  @return : Initial mips of the system

*/
uint32 DCVSCoreAlgoRequestsInitialize(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_REQUESTS* algoDataContext;

  /* Allocate the memory for the algorithm data context. */
  if(DALSYS_Malloc(sizeof(DCVS_ALGO_REQUESTS),  
                   (void**) &coreContext->algoDataContext)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }
  algoDataContext = (DCVS_ALGO_REQUESTS*)coreContext->algoDataContext;
  /* Get the current clock plan of the target */
  algoDataContext->clockPlan = DCVSCoreClockPlanGet(context);

  /* Return the initial floor mips */
  algoDataContext->mips = algoDataContext->clockPlan->clockPlanTable[algoDataContext->clockPlan->numLevels-1].reqMips;
  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoRequestsBuildParams
  
  This function is used to get the parameters for the Requests DCVS algorithm.

  @param context: The The DCVS data context.

  @return : None

*/
void DCVSCoreAlgoRequestsBuildParams(void* context)
{
}/* DCVSCoreAlgoRequestsBuildParams */


/**
  @brief DCVSCoreAlgoRequestsReset
  
   This function is used to reset the Requests algorithm specific members .

  @param context : The The DCVS data context.

  @return : The current mips of the algorithm.

*/
uint32 DCVSCoreAlgoRequestsReset(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_REQUESTS* algoDataContext = (DCVS_ALGO_REQUESTS*)coreContext->algoDataContext;	

  return( algoDataContext->mips );
}


/**
  @brief DCVSCoreAlgoRequestsEnable
  
  This function is used to set the state of the Requests algorithm specific members
  when enabling the algorithm.

  @param context : The The DCVS data context.

  @return : The current mips of the algorithm.

*/
uint32 DCVSCoreAlgoRequestsEnable(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_REQUESTS* algoDataContext = (DCVS_ALGO_REQUESTS*)coreContext->algoDataContext;	

  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoRequestsDisable
  
  This function is used to reset the state of the Requests algorithm specific members
  when disabling the algorithm.

  @param context: The The DCVS data context.

  @return : The current mips of the algorithm.

*/
uint32 DCVSCoreAlgoRequestsDisable(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_REQUESTS* algoDataContext = (DCVS_ALGO_REQUESTS*)coreContext->algoDataContext;	

  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoRequestsDecideMips
  
  @param context: The DCVS data context.

  @return : The mips the system needs to be set to.

*/
uint32 DCVSCoreAlgoRequestsDecideMips(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_REQUESTS* algoDataContext = (DCVS_ALGO_REQUESTS*)coreContext->algoDataContext;	
  DCVS_RES_DATA* algoResContext = coreContext->resDataContext;

  /* Return the current floor */ 
  algoDataContext->mips = algoResContext->activeFloor;

  /* ToDo:: Will need to support Isochronous client requests */
  return( algoDataContext->mips );

}/* DCVSCoreAlgoRequestsDecideMips */


DCVS_CORE_ALGO requestsAlgorithm = 
{
  "Requests",
  dcvsParamsAlgRequests,
  DCVSCoreAlgoRequestsInitialize,
  DCVSCoreAlgoRequestsBuildParams,
  DCVSCoreAlgoRequestsDecideMips,
  DCVSCoreAlgoRequestsReset,
  DCVSCoreAlgoRequestsEnable,
  DCVSCoreAlgoRequestsDisable
};

#endif //DEFEATURE_DCVS_ALGO_REQUESTS
