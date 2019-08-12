/**
 * @file axicfg.c
 * @note Internal source file for Bus Configuration
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/axicfg.c#1 $ 
 * $DateTime: 2016/12/13 07:59:23 $ 
 * $Author: mplcsds1 $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 02/26/2015   sds     Add QoS override API
 * 06/18/2014   sds     Add lockless halt/unhalt APIs to be used during error handling.
 * 02/16/2014   sds     Add port halt/unhalt APIs.
 * 12/05/2011   dj      Created file
 *  
 *  
 * Copyright (c) 2011-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 */ 
#include "ddiaxicfg.h"
#include "axicfgos.h"
#include "ddiaxicfgint.h"
#include "axicfgint.h"
#include "HALbimc.h"
#include "ul_i.h"


/**
  @brief Configure the clock gating for the indicated master.

  @param[in] eMaster    The master port identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetMasterClockGating( ICBId_MasterType eMaster, AxiCfg_ClockGatingType * pGating)
{
  if( AxiCfgInt_Init() )
  {
    uint32_t uPortCount;
    uint32_t * puPortId; 

    struct axicfg_master_search_info SearchInfo;
    SearchInfo.ePort = eMaster;

    if( AxiCfgInt_SearchPort( &SearchInfo ) && 
        AxiCfgInt_GetBimcMasterInfo( &SearchInfo, &puPortId, &uPortCount ) && 
        !AxiCfgInt_IsRemoteMaster( &SearchInfo ) )
    {
      HAL_bimc_ClockGatingType Gating;
      unsigned int i;
      HAL_bimc_InfoType *pHalInfo=NULL;

      Gating.bCoreClockGateEn = pGating->bCoreClockGateEn;
      Gating.bArbClockGateEn = pGating->bArbClockGateEn;
      Gating.bPortClockGateEn = pGating->bPortClockGateEn;

      AxiCfgInt_GetMasterHalInfo( &SearchInfo, (void **)&pHalInfo);
      AxiCfgOs_MutexLock( ); 
      /* configure each port associated with external master id */
      for( i = 0; i < uPortCount; i++ )
      {
        HAL_bimc_SetMasterClockGating( pHalInfo, puPortId[i], &Gating );
      }
      AxiCfgOs_MutexUnlock( ); 
    }
  }
}

/**
  @brief Configure the clock gating for the indicated slave.

  @param[in] eSlave     The slave way identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetSlaveClockGating( ICBId_SlaveType eSlave, AxiCfg_ClockGatingType * pGating)
{
  if( AxiCfgInt_Init() )
  {
    uint32_t uPortCount;
    uint32_t * puPortId; 

    struct axicfg_slave_search_info SearchInfo;
    SearchInfo.eSlave = eSlave;

    if( AxiCfgInt_SearchSlave( &SearchInfo ) &&
        AxiCfgInt_GetBimcSlaveInfo( &SearchInfo, &puPortId, &uPortCount ) && 
        !AxiCfgInt_IsRemoteSlave( &SearchInfo ) )
    {
      HAL_bimc_ClockGatingType Gating;
      unsigned int i;
      HAL_bimc_InfoType *pHalInfo = NULL;

      Gating.bCoreClockGateEn = pGating->bCoreClockGateEn;
      Gating.bArbClockGateEn = pGating->bArbClockGateEn;
      Gating.bPortClockGateEn = pGating->bPortClockGateEn;

      AxiCfgInt_GetSlaveHalInfo( &SearchInfo, (void **)&pHalInfo);
    
      AxiCfgOs_MutexLock( ); 
      /* configure each port associated with external slave id */
      for( i = 0; i < uPortCount; i++ )
      {
        HAL_bimc_SetSlaveClockGating( pHalInfo, puPortId[i], &Gating );
      }
      AxiCfgOs_MutexUnlock( ); 
    }
  }
}

/**
  @copydoc AxiCfg_HaltMasterPort
 */
AxiCfg_ReturnCodeType AxiCfg_HaltMasterPort( ICBId_MasterType eMaster )
{
  AxiCfg_ReturnCodeType ret = AXICFG_ERROR;
  if( AxiCfgInt_Init() )
  {
    ret = AxiCfgInt_HaltMasterPort( eMaster, true );
  }
  return ret;
}

/**
  @copydoc AxiCfg_HaltMasterPort_OnError
 */
AxiCfg_ReturnCodeType AxiCfg_HaltMasterPort_OnError( ICBId_MasterType eMaster )
{
  AxiCfg_ReturnCodeType ret = AXICFG_ERROR;
  if( AxiCfgInt_Init() )
  {
    ret = AxiCfgInt_HaltMasterPort( eMaster, false );
  }
  return ret;
}

/**
  @copydoc AxiCfg_UnhaltMasterPort
 */
AxiCfg_ReturnCodeType AxiCfg_UnhaltMasterPort( ICBId_MasterType eMaster )
{
  AxiCfg_ReturnCodeType ret = AXICFG_ERROR;
  if( AxiCfgInt_Init() )
  {
    ret = AxiCfgInt_UnhaltMasterPort( eMaster, true );
  }
  return ret;
}

/**
  @copydoc AxiCfg_UnhaltMasterPort_OnError
 */
AxiCfg_ReturnCodeType AxiCfg_UnhaltMasterPort_OnError( ICBId_MasterType eMaster )
{
  AxiCfg_ReturnCodeType ret = AXICFG_ERROR;
  if( AxiCfgInt_Init() )
  {
    ret = AxiCfgInt_UnhaltMasterPort( eMaster, false );
  }
  return ret;
}

/**
  @copydoc AxiCfg_SetQosPolicy
 */
AxiCfg_ReturnCodeType AxiCfg_SetQosPolicy
(
  ICBId_MasterType      eMaster,
  AxiCfg_QosPolicyType *pQosPolicy
)
{
  AxiCfg_ReturnCodeType ret = AXICFG_ERROR_INVALID;
  if( AxiCfgInt_Init() )
  {
    struct axicfg_master_search_info SearchInfo;
    SearchInfo.ePort = eMaster;

    /* Find the associated master and make sure it's local. */
    if( AxiCfgInt_SearchPort( &SearchInfo ) && 
        !AxiCfgInt_IsRemoteMaster( &SearchInfo ) )
    {
      ul_node_state_type *pState;

      /* Fetch the state for the node. */
      pState = ul_get_master_state( eMaster );
      if( NULL != pState )
      {
        axicfg_node_state_type nodeState;

        nodeState.uTotalBW      = pState->uTotalBW;
        nodeState.uIb           = pState->uIb;
        nodeState.uSuppIb       = pState->uSuppIb;
        nodeState.uActiveOnlyIb = pState->uActiveOnlyIb;
        nodeState.uAb           = pState->uAb;
        nodeState.uSuppAb       = pState->uSuppAb;
        nodeState.uActiveOnlyAb = pState->uActiveOnlyAb;

        ret = AxiCfgInt_SetQosPolicy( &SearchInfo, pQosPolicy, &nodeState );
      }
    }
  }
  return ret;
}

