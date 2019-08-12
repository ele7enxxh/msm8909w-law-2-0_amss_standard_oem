/**********************************************************************
 * fixed.c
 *
 * This module implements the fixed algorithm for multicore
 * Dynamic Clock and Voltage Scaling(DCVS).
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009 - 2011, 2012 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/algorithms/fixed/fixed.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ---------------------------------------------------
03/23/12   nj     Removed unused include file
09/24/11   sg     Fixed compiler warnings.
04/26/11   ss     Fixing Q6 compiler warnings.
01/21/11   ss     Removing the OAL component of the algorithm.
10/29/09   ss     Initial check in.
01/20/10   bjs     Cleanup
========================================================================*/

#include "BuildFeatures.h"
#ifndef DEFEATURE_DCVS_ALGO_FIXED
#include "dcvs_os.h"
#include "dcvs_algorithm.h"
#include "dcvs_core.h"
#include "ULogFront.h"
#include "CoreVerify.h"

/* Fixed algorithm specific members */
typedef struct
{
  /* Members specific to the fixed DCVS algorithm */

  /* The fixed mips the system needs to be set to. */
  uint32 mips;

}DCVS_ALGO_FIXED;

typedef enum
{
  DCVS_PARAMS_ALG_FIXED_MIPS = 0,
} DCVS_PARAMS_ALG_FIXED;


static const DCVS_NV_PARAM_ENTRY dcvsParamsAlgFixed[] =
{
  {"MIPS", DCVS_NV_PARAM_UINT32, {(void *)128}},
  DCVS_NV_PARAM_LIST_END
};



/**
  @brief DCVSCoreAlgoFixedInitialize
  
  This function is used to initialize the fixed DCVS algorithm specific members.

  @param context: The The DCVS data context.

  @return : Initial mips of the system

*/
uint32 DCVSCoreAlgoFixedInitialize(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_FIXED* algoDataContext;

  /* Allocate the memory for the algorithm data context. */
  if(DALSYS_Malloc(sizeof(DCVS_ALGO_FIXED),  
                   (void**) &coreContext->algoDataContext)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }
  algoDataContext = (DCVS_ALGO_FIXED*)coreContext->algoDataContext;
  algoDataContext->mips = 0;
  DcvsNvParamUint32Read(0, 
                        "Fixed", 
                        &dcvsParamsAlgFixed[DCVS_PARAMS_ALG_FIXED_MIPS], 
                        &algoDataContext->mips);

  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoFixedBuildParams
  
  This function is used to get the parameters for the fixed DCVS algorithm.

  @param context: The The DCVS data context.

  @return : None

*/
void DCVSCoreAlgoFixedBuildParams(void* context)
{
}/* DCVSCoreAlgoFixedBuildParams */


/**
  @brief DCVSCoreAlgoFixedReset
  
   This function is used to reset the fixed algorithm specific members .

  @param context: The The DCVS data context.

  @return : The fixed mips requested by the client.

*/
uint32 DCVSCoreAlgoFixedReset(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_FIXED* algoDataContext = (DCVS_ALGO_FIXED*)coreContext->algoDataContext;

  return( algoDataContext->mips );
}


/**
  @brief DCVSCoreAlgoFixedEnable
  
  This function is used to set the state of the fixed algorithm specific members
  when enabling the algorithm.

  @param context: The The DCVS data context.

  @return : The fixed mips requested by the client.

*/
uint32 DCVSCoreAlgoFixedEnable(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_FIXED* algoDataContext = (DCVS_ALGO_FIXED*)coreContext->algoDataContext;
  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoFixedDisable
  
  This function is used to reset the state of the fixed algorithm specific members
  when disabling the algorithm.

  @param context: The The DCVS data context.

  @return : The maximum mips of the system

*/
uint32 DCVSCoreAlgoFixedDisable(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_FIXED* algoDataContext = (DCVS_ALGO_FIXED*)coreContext->algoDataContext;
  return(algoDataContext->mips);
}


/**
  @brief DCVSCoreAlgoFixedDecideMips
  
  @param context: The The DCVS data context. 

  @return : The fixed mips the system needs to be set to.

*/
uint32 DCVSCoreAlgoFixedDecideMips(void* context)
{
  DCVS_CONTEXT* coreContext = (DCVS_CONTEXT*)context;
  /* The algorithm data context. */
  DCVS_ALGO_FIXED* algoDataContext = (DCVS_ALGO_FIXED*)coreContext->algoDataContext;

  /* Ignore the Impulse, ActiveFloor and Isochronous requests */
  return( algoDataContext->mips );

}/* DCVSCoreAlgoFixedDecideMips */

DCVS_CORE_ALGO fixedAlgorithm = 
{
  "Fixed",
  dcvsParamsAlgFixed,
  DCVSCoreAlgoFixedInitialize,
  DCVSCoreAlgoFixedBuildParams,
  DCVSCoreAlgoFixedDecideMips,
  DCVSCoreAlgoFixedReset,
  DCVSCoreAlgoFixedEnable,
  DCVSCoreAlgoFixedDisable
};
#endif //DEFEATURE_DCVS_ALGO_FIXED

