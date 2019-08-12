/**********************************************************************
 * mca_internal.h
 *
 * This is the header file for data structures related to MCA
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009-2011 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*========================================================================
  EDIT HISTORY FOR MODULE
  
  $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/inc/mca_internal.h#1 $
  $DateTime: 2016/12/13 07:59:23 $

  when       who     what, where, why
  --------   ----    ---------------------------------------------------
========================================================================*/
#ifndef MCA_INTERNAL_H
#define MCA_INTERNAL_H

#include "ULog.h"
#include "icbarb.h"
#include "icbid.h"
#include "dcvs_npa.h"

/**
 * Total number of keys supported by MCA
 */
#define MAX_KEYS 8 

/**
 * This struct is used to keep track of the current state of each key for the
 * MPPS/CPP Aggregator. This struct is used by the MCA to keep the current running
 * aggregations for each client and to keep a record of previous requests
 */
typedef struct 
{
  unsigned mpps; //Represents the Mega-packets per second, units for this field are MPPS

  unsigned cpp;  //Cycles per packet, NOTE the intended units for this field is CPP*1000

  unsigned q6Clk; //Q6 Clock floor in kHz

  unsigned ib;   //instantaneous bandwidth request

  unsigned ab;   // floor bandwidth request

  unsigned latency;  // Bus latency in ns 

  unsigned fudgeFactor;  //algorithm fudge factor, NOTE this field represents (actual fudge factor*1000) 

  unsigned mcaRequest; //this field represents the result of the MCA formula for each client(Q6 Freq in MHz)

}mcaInternalStateType;

/* Structure type to maintain fudge factor for given mpps and cpp values */
typedef struct
{
  uint32 value;

  uint32 fudgeFactor;

}mcaFudgeFactorType;


typedef struct
{
  uint32 nCnt;

  mcaFudgeFactorType *pData;

}mcaFudgerFactorArrayType;


/* Structure type to maintain the context for the data for MCA. */
typedef struct 
{
  mcaInternalStateType mcaState;

  //Fudger factors  
  mcaFudgerFactorArrayType fudgeFactors;

  //no of corner levels for clk/cpu
  uint32 nCorners;

  //Max freqeuncy values for each corener in KHz
  uint32* pCornerFreq;

  ULogHandle ulogHandle;

  npa_client_handle icbArbClientHandle;
  
  /* Vector of request states for the master/slave pairs */
  ICBArb_RequestType vecReq; 

}mcaInternalDataType;

#ifdef MCA_ENABLE_DIAG_HOOKS
extern npa_client_handle mcaDiagClient;
extern unsigned mcaHookMode;
extern unsigned mcaNextParamIsQ6;
extern unsigned mcaNextParamIsIBW;
extern unsigned mcaNextParamIsABW;
extern unsigned mcaNextParamIsLatency;
extern unsigned mcaNextParamIsValue;
extern unsigned mcaFixedQ6;
extern unsigned mcaFixedIBW;
extern unsigned mcaFixedABW;
extern unsigned mcaFixedLatency;
extern unsigned mcaDiagLogInitialized;
#endif

/*=======================================================================

                  PUBLIC FUNCTION DECLARATIONS

========================================================================*/

/**
  @brief mcaTargetIntFudgeFactors
  This functions populates fudge factor table which is particular to each target.
  This information consists of mpps * cpp and correspondong fudge factor value.
  @param none.
  @return None
*/

void mcaTargetInitFudgeFactors(mcaFudgerFactorArrayType *);
                                            
#endif /* MCA_INTERNAL_H */