/*============================================================================
  FILE:         mca_target.c

  OVERVIEW:     This file provides the target specific data and code for MCA.
 
  DEPENDENCIES: None
 
                Copyright (c) 2014 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE
  
  $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/tgt/8916/modem/mca_target.c#1 $
  $DateTime: 2016/12/13 07:59:23 $

  when       who     what, where, why
  --------   ----    ---------------------------------------------------
========================================================================*/
#include "mca_internal.h"


/* The lookup table for Fudge factors based on mpps * cpp
   Currently uses the follwing conditions:
   If CPP(i)*MPPS(i)<288, Fudge factor is 1/0.6, else 1/0.85.
   Currently using single fudgefactor recommended by Architecture team
*/
mcaFudgeFactorType mcaTargetFudgeFactorTable[] = {
  {
    288000,     // mpps * cpp value
    1176        // Fudge factor level1 (1/0.85)*1000  
  },
  {
    0xFFFFFFFF, // mpps* cpp value
    1176        // fudge factor level2 (1/.85) * 1000
  }
};

void mcaTargetInitFudgeFactors(mcaFudgerFactorArrayType *mcaFudgeFactors)
{
  if(NULL != mcaFudgeFactors)
{
    mcaFudgeFactors->nCnt = sizeof(mcaTargetFudgeFactorTable)/sizeof(mcaTargetFudgeFactorTable[0]);
    mcaFudgeFactors->pData = mcaTargetFudgeFactorTable;
  }
  return;
}
