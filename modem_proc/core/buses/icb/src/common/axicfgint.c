/**
 * @file axicfgint.c
 * @note Internal source file for Bus Configuration. Implements ddiaxicfgint.h
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/axicfgint.c#1 $ 
 * $DateTime: 2016/12/13 07:59:23 $ 
 * $Author: mplcsds1 $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 05/11/2015   sds     Add support for auxiliary throttle blocks.
 * 02/26/2015   sds     Add QoS override API
 * 12/12/2014   jc      Support for NPA active-only clients and RPM semi-active set.
 * 10/02/2014   sds     Remove unused threshold keys and fix invalidate logic for BIMC masters.
 * 08/15/2014   sds     Invalidate possible latency requests on init.
 * 06/20/2014   sds     Extend request types to add latency request in addition to BW.
 * 06/18/2014   sds     Add lockless halt/unhalt APIs to be used during error handling.
 * 03/12/2014   sds     Change the hardware base address to a uint8_t*.
 * 02/16/2014   sds     Add port halt/unhalt APIs.
 * 01/27/2014   sds     Correct QoSSetMasterBW to avoid sending messages to the RPM
 *                      for local resources without defaults.
 * 10/10/2013   sds     Remove clock gating setting - hardware defaults are correct.
 * 06/12/2013   sds     Add support for static and dynamic BKE configurations
 *                      with threshold support
 * 02/22/2012   sds     Refactor and fix initialization sequence so all masters
 *                      are properly invalidated. Reorder invalidation sequence
 *                      to avoid unnecessary RPM thrashing.
 * 11/15/2012   sds     Fix handling of RPM msg ids, while consolidating all RPM
 *                      communications in this layer.
 *                      Add support for NAS and proper sleep set usage.
 * 12/05/2011   dj      Created file
 *  
 *  
 * Copyright (c) 2011-2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
 */ 

#include "axicfgint.h"
#include "axicfgos.h"

#include "HALnoc.h"
#include "HALbimc.h"

#include "icb_rpm_types.h"
#include "npa_resource.h"
#include "rpmclient.h"
#include "stringl.h"

/*--------------------------------- types ---------------------------------*/
typedef enum
{
  AXICFGINT_STATE_UNINITIALIZED = 0, /* Initialization has not be attempted. */
  AXICFGINT_STATE_INITIALIZING,      /* Initialization in progress. */
  AXICFGINT_STATE_INITIALIZED,       /* Initialization completed. */
} AxiCfgInt_StateType;

/*------------------------------ definitions ------------------------------*/
#define AUX_THROTTLE_MAX_BW 0xFFFFFFFFFFFFFFFFULL

/*--------------------------------- macro ---------------------------------*/
#define CEILING_DIV(dividend, divisor) (((dividend)+(divisor)-1)/(divisor))

#define THRESHOLD(bw, pct) ((pct)?CEILING_DIV((bw)*(pct), 100):0)

/* RPM message id update macro
 * Ignore new message ids if zero.
 */
#define RPM_MSG_ID_UPDATE(msg_id, new_msg_id) (msg_id) = (new_msg_id)? (new_msg_id): (msg_id)

/* Copy two structures. */
#define STRUCT_COPY(dst, src) memscpy((dst), sizeof((dst)), (src), sizeof((src)))

/*------------------------- internal private data -------------------------*/
static AxiCfgInt_StateType state = AXICFGINT_STATE_UNINITIALIZED;
static struct axicfg_soc_info SocInfo;
static kvp_t * pMsgBuffer;
static unsigned uMsgId;

/*--------------------------- private functions ---------------------------*/
/**
  @brief Create KVP buffer
 */
static bool _AxiCfgInt_CreateKvpBuffer( void )
{
  pMsgBuffer = kvp_create( (8/*key space*/+sizeof(uint64_t)/*token space*/)*4/*max token*/ );
  return ( pMsgBuffer != NULL );
}

/**
 @brief Post a request to the RPM, updating our global message id correctly.

 @param[in] set      the rpm set to commit to
 @param[in] resource the resource type to use
 @param[in] id       the id number in the resource type above
 @param[in] kvps     a pointer to the kvp buffer to send
 */
static void _AxiCfgInt_PostRPMRequest( rpm_set_type set, rpm_resource_type resource, unsigned id, kvp_t *kvps )
{
  unsigned uTempId = rpm_post_request( set, resource, id, kvps );
  RPM_MSG_ID_UPDATE( uMsgId, uTempId );
}

/**
  @brief  Used to send NOC Master BW value to remote

  @param[in]  ePort         external port id
  @param[in]  u64ActiveBW   active set bandwidth 
  @param[in]  u64SemiBW     semi-active set bandwidth 
  @param[in]  u64SleepBW    sleep set bandwidth 
  @param[in]  bNextActive   active set bandwith -> next active set?
 */
static void _AxiCfgInt_SendNocMasterBW
(
  ICBId_MasterType ePort,
  uint64_t         u64ActiveBW,
  uint64_t         u64SemiBW,
  uint64_t         u64SleepBW,
  bool             bNextActive
)
{
  if( pMsgBuffer != NULL ) 
  {
    rpm_set_type active_set = !bNextActive? RPM_ACTIVE_SET: RPM_NEXT_ACTIVE_SET;

    /* Do the active/next active set request. */
    kvp_clear( pMsgBuffer ); 
    kvp_put( pMsgBuffer, ICB_MASTER_FIELD_BW, sizeof(uint64_t), (char*)&u64ActiveBW );
    _AxiCfgInt_PostRPMRequest( active_set, RPM_BUS_MASTER_REQ, ePort, pMsgBuffer );


    /* If the active request equals the semi-active request, invalidate semi-active. */
    if( u64ActiveBW == u64SemiBW )
    {
      _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_MASTER_REQ, ePort, NULL );
    }
    /* If not, send it along. */
    else
    {
    /* Do the semi-active set request. */
      kvp_clear( pMsgBuffer ); 
      kvp_put( pMsgBuffer, ICB_MASTER_FIELD_BW, sizeof(uint64_t), (char*)&u64SemiBW );
      _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_MASTER_REQ, ePort, pMsgBuffer );
    }

    /* If the active request equals the sleep request, invalidate sleep. */
    if( u64ActiveBW == u64SleepBW )
    {
      _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET, RPM_BUS_MASTER_REQ, ePort, NULL );
    }
    /* If not, send it along. */
    else
    {
      kvp_clear( pMsgBuffer ); 
      kvp_put( pMsgBuffer, ICB_MASTER_FIELD_BW, sizeof(uint64_t), (char *)&u64SleepBW );
      _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET, RPM_BUS_MASTER_REQ, ePort, pMsgBuffer );
    }
  }
}

/**
  @brief  Used to send BIMC Master BW to remote
  
  @pre   If active == sleep, active and sleep pointers should be the same.

  @param[in]  ePort         external port id
  @param[in]  pActiveBW     pointer to active set bandwidth
  @param[in]  pSemiBW       pointer to semi-active set bandwidth
  @param[in]  pSleepBW      pointer to sleep set bandwidth 
  @param[in]  bNextActive   active set bandwith -> next active set?

 */
static void _AxiCfgInt_SendBimcMasterBW
(
  ICBId_MasterType           ePort,
  HAL_bimc_QosBandwidthType *pActiveBW,
  HAL_bimc_QosBandwidthType *pSemiBW,
  HAL_bimc_QosBandwidthType *pSleepBW,
  bool                       bNextActive
)
{
  if( pMsgBuffer != NULL ) 
  {
    rpm_set_type active_set = !bNextActive? RPM_ACTIVE_SET: RPM_NEXT_ACTIVE_SET;

    kvp_clear( pMsgBuffer ); 
    kvp_put( pMsgBuffer, ICB_MASTER_FIELD_BW, sizeof(uint64_t), (char*)&pActiveBW->uBandwidth );
    _AxiCfgInt_PostRPMRequest( active_set, RPM_BUS_MASTER_REQ, ePort, pMsgBuffer );  


    /* If the active request equals the semi-active request, invalidate semi-active. */
    if( pActiveBW->uBandwidth == pSemiBW->uBandwidth )
    {
      _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_MASTER_REQ, ePort, NULL );
    }
    /* If not, send it along. */
    else
    {
      kvp_clear( pMsgBuffer ); 
      kvp_put( pMsgBuffer, ICB_MASTER_FIELD_BW,            sizeof(uint64_t), (char*)&pSemiBW->uBandwidth );
      _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_MASTER_REQ, ePort, pMsgBuffer );  
    }

    /* If the active request equals the sleep request, invalidate sleep. */
    if( pActiveBW->uBandwidth == pSleepBW->uBandwidth )
    {
      _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET, RPM_BUS_MASTER_REQ, ePort, NULL );
    }
    /* If not, send it along. */
    else
    {
      kvp_clear( pMsgBuffer ); 
      kvp_put( pMsgBuffer, ICB_MASTER_FIELD_BW, sizeof(uint64_t), (char*)&pSleepBW->uBandwidth );
      _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET, RPM_BUS_MASTER_REQ, ePort, pMsgBuffer );
    }
  }
}

/**
  @brief  Used to send Master latency value to remote

  @param[in]  ePort             external port id
  @param[in]  u32ActiveLatency  active set latency 
  @param[in]  u32SemiLatency    semi-active set latency 
  @param[in]  u32SleepLatency   sleep set latency 
  @param[in]  bNextActive       active set latency -> next active set?
 */
static void _AxiCfgInt_SendMasterLatency
(
  ICBId_MasterType ePort,
  uint32_t         u32ActiveLatency,
  uint32_t         u32SemiLatency,
  uint32_t         u32SleepLatency,
  bool             bNextActive
)
{
  if( pMsgBuffer != NULL ) 
  {
    rpm_set_type active_set = !bNextActive? RPM_ACTIVE_SET: RPM_NEXT_ACTIVE_SET;

    /* Do the active/next active set request. */
    kvp_clear( pMsgBuffer ); 
    kvp_put( pMsgBuffer, ICB_LATENCY_MASTER_FIELD_NS, sizeof(uint32_t), (char*)&u32ActiveLatency );
    _AxiCfgInt_PostRPMRequest( active_set, RPM_BUS_MASTER_LATENCY_REQ, ePort, pMsgBuffer );

    /* If the active request equals the semi-active request, invalidate semi-active. */
    if( u32ActiveLatency == u32SemiLatency )
    {
      _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_MASTER_LATENCY_REQ, ePort, NULL );
    }
    /* If not, send it along. */
    else
    {
      kvp_clear( pMsgBuffer ); 
      kvp_put( pMsgBuffer, ICB_LATENCY_MASTER_FIELD_NS, sizeof(uint32_t), (char*)&u32SemiLatency );
      _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_MASTER_LATENCY_REQ, ePort, pMsgBuffer );
    }

    /* If the active request equals the sleep request, invalidate sleep. */
    if( u32ActiveLatency == u32SleepLatency )
    {
      _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET, RPM_BUS_MASTER_LATENCY_REQ, ePort, NULL );
    }
    /* If not, send it along. */
    else
    {
      kvp_clear( pMsgBuffer ); 
      kvp_put( pMsgBuffer, ICB_LATENCY_MASTER_FIELD_NS, sizeof(uint32_t), (char *)&u32SleepLatency );
      _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET, RPM_BUS_MASTER_LATENCY_REQ, ePort, pMsgBuffer );
    }
  }
}

/**
  @brief  Used to send any slave BW value to remote

  @param[in]  eSlave        external port id
  @param[in]  u64ActiveBW   active set bandwidth 
  @param[in]  u64SemiBW     semi-active set bandwidth 
  @param[in]  u64SleepBW    sleep set bandwidth 
  @param[in]  bNextActive   active set bandwith -> next active set?
 */
static void _AxiCfgInt_SendSlaveBW
(
  ICBId_SlaveType eSlave,
  uint64_t        u64ActiveBW,
  uint64_t        u64SemiBW,
  uint64_t        u64SleepBW,
  bool            bNextActive
)
{
  if( pMsgBuffer != NULL ) 
  {
    rpm_set_type active_set = !bNextActive? RPM_ACTIVE_SET: RPM_NEXT_ACTIVE_SET;

    /* Do the active/next active set request. */
    kvp_clear( pMsgBuffer ); 
    kvp_put( pMsgBuffer, ICB_SLAVE_FIELD_BW, sizeof(uint64_t), (char*)&u64ActiveBW );
    _AxiCfgInt_PostRPMRequest( active_set, RPM_BUS_SLAVE_REQ, eSlave, pMsgBuffer );

    /* If the active request equals the semi-active request, invalidate semi-active. */
    if( u64ActiveBW == u64SemiBW )
    {
      _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_SLAVE_REQ, eSlave, NULL );
    }
    /* If not, send it along. */
    else
    {
      kvp_clear( pMsgBuffer ); 
      kvp_put( pMsgBuffer, ICB_SLAVE_FIELD_BW, sizeof(uint64_t), (char*)&u64SemiBW );
      _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_SLAVE_REQ, eSlave, pMsgBuffer );
    }

    /* If the active request equals the sleep request, invalidate sleep. */
    if( u64ActiveBW == u64SleepBW )
    {
      _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET, RPM_BUS_SLAVE_REQ, eSlave, NULL );
    }
    /* If not, send it along. */
    else
    {
      kvp_clear( pMsgBuffer ); 
      kvp_put( pMsgBuffer, ICB_MASTER_FIELD_BW, sizeof(uint64_t), (char *)&u64SleepBW );
      _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET, RPM_BUS_SLAVE_REQ, eSlave, pMsgBuffer );
    }
  }
}

/**
  @brief  Used to invalidate slave bandwidth req to remote

  @param[in]  eSlave  external slave id
 */
static void _AxiCfgInt_InvalidateSlaveBW( ICBId_SlaveType eSlave )
{
  _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET,  RPM_BUS_SLAVE_REQ, eSlave, NULL );  
  _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_SLAVE_REQ, eSlave, NULL );  
  _AxiCfgInt_PostRPMRequest( RPM_ACTIVE_SET, RPM_BUS_SLAVE_REQ, eSlave, NULL );  
}

/**
  @brief  Used to invalidate master ports on initialization

  @param[in]  ePort         external port id
 */
static void  _AxiCfgInt_InvalidateMasterBW( ICBId_MasterType ePort )
{
  _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET,  RPM_BUS_MASTER_REQ, ePort, NULL );  
  _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_MASTER_REQ, ePort, NULL );  
  _AxiCfgInt_PostRPMRequest( RPM_ACTIVE_SET, RPM_BUS_MASTER_REQ, ePort, NULL );  
}

/**
  @brief  Used to invalidate master latency requests on initialization

  @param[in]  ePort         external port id
 */
static void  _AxiCfgInt_InvalidateMasterLatency( ICBId_MasterType ePort )
{
  _AxiCfgInt_PostRPMRequest( RPM_SLEEP_SET,  RPM_BUS_MASTER_LATENCY_REQ, ePort, NULL );  
  _AxiCfgInt_PostRPMRequest( RPM_SEMI_ACTIVE_SET, RPM_BUS_MASTER_LATENCY_REQ, ePort, NULL );  
  _AxiCfgInt_PostRPMRequest( RPM_ACTIVE_SET, RPM_BUS_MASTER_LATENCY_REQ, ePort, NULL );  
}

/**
  @brief  Used to update local qos settings.

  @pre The node *must* be local. This function will not check.

  @param[in]  pMaster    A pointer to the master definition structure.
  @param[in]  uTotalBW   The summary bandwidth request for this node (for thresholds)
  @param[in]  uActiveBW  The active bandwidth request for this node (for regulator/limiter bandwidth setting)

 */
static void _AxiCfgInt_QoSSetLocal
(
  icb_master_def_type *pMaster,
  uint64_t             uTotalBW,
  uint64_t             uActiveBW
)
{
  void *pQosPolicy;

  /* If we have no QoS ports, bail out. */
  if( NULL == pMaster->pQosPortIds )
  {
    return;
  }

  /* We're about to modify hardware, acquire our lock. */
  AxiCfgOs_MutexLock(); 

  /* If we have an overide policy, use that. */
  if( pMaster->pQosPolicyOverride)
  {
    pQosPolicy = pMaster->pQosPolicyOverride;
  }
  /* Otherwise, use the default policy, if we have one. */
  else if( pMaster->pDefaults )
  {
    pQosPolicy = pMaster->pDefaults;
  }
  else
  {
    /* Unlock and return if we have no policy to apply. */
    AxiCfgOs_MutexUnlock();
    return;
  }

  switch( pMaster->pBus->busType )
  {
    /* If this is a NOC master, handle it that way. */
    case ICB_BUS_NOC:
    {
      icb_noc_master_defaults_type *pQos = (icb_noc_master_defaults_type *)pQosPolicy;
      HAL_noc_QosModeType mode = pQos->nocMode;
      unsigned int port;

      /* If this setting has been previously applied, bail.
       * Settings that are not static and regulator/limiter will always show up as not applied. */
      if( pQos->bApplied )
      {
        break;
      }

      /* If we have an auxiliary throttle, we may need to change some things. */
      if( NULL != pMaster->pAuxThrottle )
      {
        uint64_t uLimit;

        /* If we're limiter mode, this means we need to use the aux throttle.
         * Set the mode for NoC QoS generator to be fixed mode.
         * Update the throttle value. */
        if( NOC_QOS_MODE_LIMITER == pQos->nocMode )
        {
          mode = NOC_QOS_MODE_FIXED;

          /* If we're static, use the limit in our QoS policy. */
          if( (pQos->reqType & ICB_BW_REQ_TYPE_STATIC) != 0 )
          {
            uLimit = CEILING_DIV( pQos->nocBandwidth.uBandwidth, pMaster->pAuxThrottle->uNumPorts );
          }
          /* Otherwise, use the voted bandwidth. */
          else
          {
            /* Safety check here.
            * If we're in limiter with a dynamic limit and a zero bandwidth value, that's bogus. */
            if( 0 == uActiveBW )
            {
              uLimit = AUX_THROTTLE_MAX_BW;
            }
            else
            {
              uLimit = CEILING_DIV( uActiveBW, pMaster->pAuxThrottle->uNumPorts );
            }
          }
        }
        /* Not in limiter mode. Set no limit by making it really high. */
        else
        {
          uLimit = AUX_THROTTLE_MAX_BW;
        }

        /* Apply the limit we need. */
        for( port = 0; port < pMaster->pAuxThrottle->uNumPorts; port++ )
        {
          HAL_throttle_SetBW( &pMaster->pAuxThrottle->info,
                              pMaster->pAuxThrottle->pPortIds[port],
                              pMaster->pAuxThrottle->uLevel,
                              uLimit );
        }
      }

      /* In the case of multiple ports per master, program them both. */
      for( port = 0; port < pMaster->uNumPorts; port++ )
      {
        /* If we're not in bypass, we have more to do. */
        if( NOC_QOS_MODE_BYPASS != mode )
        {
          /* Safety check here.
           * If we're in limiter with a dynamic limit and a zero bandwidth value, that's bogus.
           * Set the port to fixed mode, priority zero instead. */
          if( mode == NOC_QOS_MODE_LIMITER &&
              (pQos->reqType & ICB_BW_REQ_TYPE_STATIC) == 0 &&
              uActiveBW == 0 )
          {
            HAL_noc_QosPriorityType priority;

            mode = NOC_QOS_MODE_FIXED;
            priority.p0 = 0;
            priority.p1 = 0;

            HAL_noc_SetQosPriority( pMaster->pBus->pInfo, pMaster->pQosPortIds[port], mode, &priority );
          }
          else
          {
            /* Set priority */
            HAL_noc_SetQosPriority( pMaster->pBus->pInfo, pMaster->pQosPortIds[port], mode, &pQos->nocPriority );
          }

          /* If we're not in fixed, we have more to do. */
          if( NOC_QOS_MODE_FIXED != mode )
          {
            HAL_noc_QosBandwidthType BW;
            uint64_t uBW;

            /* If this is a static request, use the value in the qos policy. */
            if( pQos->reqType & ICB_BW_REQ_TYPE_STATIC )
            {
              uBW = pQos->nocBandwidth.uBandwidth;
            }
            else
            {
              uBW = uActiveBW;
            }

            BW.uBandwidth =  CEILING_DIV( uBW, pMaster->uNumPorts );
            BW.uWindowSize = pQos->nocBandwidth.uWindowSize;

            /* Set bandwidth. */
            HAL_noc_SetQosBandwidth( pMaster->pBus->pInfo, pMaster->pQosPortIds[port], &BW );
          }
        }

        /* Program mode. */
        HAL_noc_SetQosMode( pMaster->pBus->pInfo, pMaster->pQosPortIds[port], mode );
      }

      /* Set applied status if the requested settings are:
       * Static or Bypass mode or Fixed mode.
       * No reason to ever set more than once in those modes. */
      if( (pQos->reqType & ICB_BW_REQ_TYPE_STATIC) != 0 || 
          pQos->nocMode == NOC_QOS_MODE_BYPASS ||
          pQos->nocMode == NOC_QOS_MODE_FIXED )
      {
        pQos->bApplied = true;
      }
      break;
    }
    /* If this is a BIMC master, handle it. */
    case ICB_BUS_BIMC:
    {
      uint32_t port, mode_idx;
      icb_bimc_master_defaults_type *pQos = (icb_bimc_master_defaults_type *)pQosPolicy;
      HAL_bimc_QosModeType mode;

      /* Figure out which mode index to use, and update the mode if necessary. */
      if( pQos->reqType & ICB_BW_REQ_TYPE_THRESHOLD )
      {
        /* Greater than the threshold, use the second mode. */
        if( uTotalBW > pQos->bwThreshold )
        {
          mode_idx = 1;
        }
        /* Less than or equal to than the threshold, use the first mode. */
        else
        {
          mode_idx = 0;
        }
      }
      /* We're not using thresholds. Use the first mode. */
      else
      {
        mode_idx = 0;
      }

      /* If this setting has been previously applied, bail.
       * Settings that are not static and regulator/limiter will always show up as not applied. */
      if( pQos->bApplied[mode_idx] )
      {
        break;
      }

      /* Get the mode we want to go to. */
      mode = pQos->bimcMode[mode_idx];

      /* In the case of multiple ports per master, program them both. */
      for( port = 0; port < pMaster->uNumPorts; port++ )
      {
        /* If we're not in bypass, we have more to do. */
        if( BIMC_QOS_MODE_BYPASS != mode )
        {
          /* Safety check here.
           * If we're in limiter with a dynamic limit and a zero bandwidth value, that's bogus.
           * Set the port to fixed mode, priority zero instead. */
          if( mode == BIMC_QOS_MODE_LIMITER &&
              (pQos->reqType & ICB_BW_REQ_TYPE_STATIC) == 0 &&
              uActiveBW == 0 )
          {
            HAL_bimc_QosPriorityType priority;

            mode = BIMC_QOS_MODE_FIXED;
            priority.fixed.uPriorityLvl       = 0;
            priority.fixed.uAReqPriorityRead  = 0;
            priority.fixed.uAReqPriorityWrite = 0;

            HAL_bimc_SetQosPriority( pMaster->pBus->pInfo, pMaster->pQosPortIds[port], mode, &priority );
          }
          else
          {
            HAL_bimc_SetQosPriority( pMaster->pBus->pInfo, pMaster->pQosPortIds[port], mode, &pQos->bimcPriority );
          }

          /* If we're not in fixed, we have more to do. */
          if( BIMC_QOS_MODE_FIXED != mode )
          {
            HAL_bimc_QosBandwidthType BW;
            uint64_t uBW;

            /* If this is a static request, use the value in the qos policy. */
            if( pQos->reqType & ICB_BW_REQ_TYPE_STATIC )
            {
              uBW = pQos->bimcBandwidth.uBandwidth;
            }
            else
            {
              uBW = uActiveBW;
            }
                
            BW.uBandwidth       = CEILING_DIV( uBW, pMaster->uNumPorts );
            BW.uWindowSize      = pQos->bimcBandwidth.uWindowSize;
            BW.uThresholdHigh   = THRESHOLD( BW.uBandwidth, pQos->bimcBandwidth.uThresholdHighPct );
            BW.uThresholdMedium = THRESHOLD( BW.uBandwidth, pQos->bimcBandwidth.uThresholdMedPct );
            BW.uThresholdLow    = THRESHOLD( BW.uBandwidth, pQos->bimcBandwidth.uThresholdLowPct );

            /* Set bandwidth. */
            HAL_bimc_SetQosBandwidth( pMaster->pBus->pInfo, pMaster->pQosPortIds[port], &BW );
          }
        }

        /* Program the mode. */
        HAL_bimc_SetQosMode( pMaster->pBus->pInfo, pMaster->pQosPortIds[port], mode );

        /* Clear any applied status.
         * Set applied status again if the requested settings are:
         * Static or Bypass mode or Fixed mode.
         * No reason to ever set more than once in those modes. */
        memset(pQos->bApplied, 0, sizeof(pQos->bApplied));
        if( (pQos->reqType & ICB_BW_REQ_TYPE_STATIC) != 0 || 
            mode == BIMC_QOS_MODE_BYPASS ||
            mode == BIMC_QOS_MODE_FIXED )
        {
          pQos->bApplied[mode_idx] = true;
        }
        break;
      }
    }
    default:
      /* Shouldn't get here, we only have two master types that touch hardware. */
      break;
  }

  /* Done, release our lock. */
  AxiCfgOs_MutexUnlock();
}

/**
  @brief  Used to set QOS master defaults
 */
static void _AxiCfgInt_SetMasterDefaults( void )
{
  unsigned int i;
  for( i = 0; i <  SocInfo.uMasterCount; i++ )
  {
    icb_master_def_type *pMaster = SocInfo.pMasterList[i];

    /* If this master has a cousin latency node on the RPM, invalidate
     * any previous requests. */
    if( pMaster->bLatency )
    {
      _AxiCfgInt_InvalidateMasterLatency( pMaster->masterId );
    }

    /* If this master isn't remote, we may have defaults to set locally. */
    if( !pMaster->bRemote )
    {
      _AxiCfgInt_QoSSetLocal(pMaster, 0, 0);
    }
    /* Otherwise the master is remote, and we need to check for defaults to send out. */
    else
    {
      _AxiCfgInt_InvalidateMasterBW( pMaster->masterId );
    }
  }
}

/**
  @brief  Used to set slave defaults
 */
static void _AxiCfgInt_SetSlaveDefaults( void )
{
  unsigned int i;
  for( i = 0; i < SocInfo.uSlaveCount; i++ )
  {
    icb_slave_def_type *pSlave = SocInfo.pSlaveList[i];

    /* Skip, if there are no defaults for this slave or if they are remoted */
    if( NULL != pSlave->pDefaults && 
        !pSlave->bRemote )
    {
      switch( pSlave->pBus->busType )
      {
        case ICB_BUS_BIMC:
        {

          icb_bimc_slave_defaults_type *pDefs = (icb_bimc_slave_defaults_type *)pSlave->pDefaults;
          HAL_bimc_InfoType *pInfo = (HAL_bimc_InfoType *)pSlave->pBus->pInfo;

          /* In the case of multiple ports per slave, program them both. */
          unsigned int port;
          for( port = 0; port < pSlave->uNumPorts; port++ )
          {
            AxiCfgOs_MutexLock( ); 

            HAL_bimc_ArbitrationEnable( pInfo, pSlave->pPortIds[port], pDefs->bArbEn );
            
            AxiCfgOs_MutexUnlock( ); 
          }
          break;
        }
        case ICB_BUS_NOC:
          /* No defaults to set. */
          break;
        default:
          /* TODO: Bad stuff here. */
          break;
      }
    }
    else if( pSlave->bRemote ) 
    {
      AxiCfgOs_MutexLock( ); 
      _AxiCfgInt_InvalidateSlaveBW( pSlave->slaveId );
      AxiCfgOs_MutexUnlock( ); 
    }
  }

}

/**
  @brief  Initialization callback.

	@param[in] context    unused
	@param[in] event_type unused
	@param[in] data       unused
	@param[in] data_size  unused
 */
void _AxiCfgInt_InitCb( void *context, unsigned int event_type, void *data, unsigned int data_size )
{
  uint32_t bus_idx, clk_idx;
  icb_bus_def_type *pBus;

  /* Unused parameters. */
  (void)context;
  (void)event_type;
  (void)data;
  (void)data_size;

  /* Create our config clock clients for our buses. */
  for( bus_idx = 0;
       bus_idx < SocInfo.uBusCount;
       bus_idx++ )
  {
    pBus = SocInfo.pBusList[bus_idx];
    for( clk_idx = 0;
         clk_idx < SocInfo.pBusList[bus_idx]->uNumCfgClks;
         clk_idx++ )
    {
      pBus->aCfgClkClients[clk_idx] = 
        npa_create_sync_client( pBus->aCfgClkNames[clk_idx],
                                "/icb/axicfg",
                                NPA_CLIENT_REQUIRED );
    }
    for( clk_idx = 0;
         clk_idx < SocInfo.pBusList[bus_idx]->uNumEnCfgClks;
         clk_idx++ )
    {
      AxiCfgOs_GetClockId( pBus->aEnCfgClkNames[clk_idx],
                           &pBus->aEnCfgClkIds[clk_idx] );
    }
  }

  /* Clocks successfully created.
   * Change our state and create the marker. */
  state = AXICFGINT_STATE_INITIALIZED;
  npa_define_marker( AXICFG_INIT_DONE );
}

/**
  @brief  Enable all config clocks for AxiCfgInt_Reset.
 */
void _AxiCfgInt_EnableAllConfigClocks( void )
{
  uint32_t bus_idx, clk_idx;
  icb_bus_def_type *pBus;

  /* Request 1 KHz on all config clocks to make sure they're on. */
  for( bus_idx = 0;
       bus_idx < SocInfo.uBusCount;
       bus_idx++ )
  {
    pBus = SocInfo.pBusList[bus_idx];
    for( clk_idx = 0;
         clk_idx < SocInfo.pBusList[bus_idx]->uNumCfgClks;
         clk_idx++ )
    {
      npa_issue_required_request( pBus->aCfgClkClients[clk_idx], 1 );
    }
    for( clk_idx = 0;
         clk_idx < SocInfo.pBusList[bus_idx]->uNumEnCfgClks;
         clk_idx++ )
    {
      AxiCfgOs_EnableClock( pBus->aEnCfgClkIds[clk_idx] );
    }
  }
}

/**
  @brief  Disable all config clocks after AxiCfgInt_Reset.
 */
void _AxiCfgInt_DisableAllConfigClocks( void )
{
  uint32_t bus_idx, clk_idx;
  icb_bus_def_type *pBus;

  /* Request 1 KHz on all config clocks to make sure they're on. */
  for( bus_idx = 0;
       bus_idx < SocInfo.uBusCount;
       bus_idx++ )
  {
    pBus = SocInfo.pBusList[bus_idx];
    for( clk_idx = 0;
         clk_idx < SocInfo.pBusList[bus_idx]->uNumCfgClks;
         clk_idx++ )
    {
      /* Make these fire-and-forget requests. We don't care when they finish. */
      npa_set_request_attribute( pBus->aCfgClkClients[clk_idx], NPA_REQUEST_FIRE_AND_FORGET );
      npa_issue_required_request( pBus->aCfgClkClients[clk_idx], 0 );
    }
    for( clk_idx = 0;
         clk_idx < SocInfo.pBusList[bus_idx]->uNumEnCfgClks;
         clk_idx++ )
    {
      AxiCfgOs_DisableClock( pBus->aEnCfgClkIds[clk_idx] );
    }
  }
}

void _AxiCfgInt_ConvertQosPolicy
(
  icb_master_def_type * pMaster,
  AxiCfg_QosPolicyType *pQosPolicy
)
{
  /* Convert to our internal bus specific type. */
  switch( pMaster->pBus->busType )
  {
    case ICB_BUS_BIMC:
    {
      icb_bimc_master_defaults_type *pOverride = (icb_bimc_master_defaults_type *)pMaster->pQosPolicyOverride;
      memset(pOverride, 0, sizeof(icb_bimc_master_defaults_type));
      pOverride->reqType     = (icb_bw_req_type)pQosPolicy->reqType;
      pOverride->bwThreshold = pQosPolicy->bwThreshold;
      STRUCT_COPY(pOverride->bimcMode,                 pQosPolicy->mode);
      STRUCT_COPY(&pOverride->bimcPriority.fixed,     &pQosPolicy->priority.bimc_fixed);
      STRUCT_COPY(&pOverride->bimcPriority.regulator, &pQosPolicy->priority.bimc_regulator);
      STRUCT_COPY(&pOverride->bimcBandwidth,          &pQosPolicy->bandwidth);
      break;
    }
    case ICB_BUS_NOC:
    {
      icb_noc_master_defaults_type *pOverride = (icb_noc_master_defaults_type *)pMaster->pQosPolicyOverride;
      memset(pOverride, 0, sizeof(icb_bimc_master_defaults_type));
      pOverride->nocMode                  = (HAL_noc_QosModeType)pQosPolicy->mode[0];
      pOverride->nocBandwidth.uBandwidth  = pQosPolicy->bandwidth.uBandwidth;
      pOverride->nocBandwidth.uWindowSize = pQosPolicy->bandwidth.uWindowSize;
      STRUCT_COPY(&pOverride->nocPriority, &pQosPolicy->priority.noc);
      break;
    }
    default:
      /* Do nothing. */
      break;
  }
}

/*------------------------------ public functions -------------------------*/

/**
  @brief Used to check if an external master id is valid

  @pre   pInfo != NULL

  @param[inout] pInfo  pointer to structure that's used to hold master search info

  @return bool
    @retval true  if valid
    @retval false if not valid
 */
bool AxiCfgInt_SearchPort( struct axicfg_master_search_info *pInfo )
{
  ICBId_MasterType ePort = pInfo->ePort;
  unsigned int i;
  
  for( i=0; i < SocInfo.uMasterCount ; i++ )
  {
    pInfo->pCurrMaster = SocInfo.pMasterList[i];    
    if( NULL != pInfo->pCurrMaster  && 
        pInfo->pCurrMaster->masterId == ePort )
    {
      return true;
    }
  }
  pInfo->pCurrMaster= NULL;
  return false;
}

/**
  @brief Used to check if an external slave id is valid

  @pre   pInfo != NULL

  @param[inout] pInfo  pointer to structure that's used to hold slave search info

  @return bool
    @retval true  if valid
    @retval false if not valid
 */
bool AxiCfgInt_SearchSlave( struct axicfg_slave_search_info *pInfo  )
{
  ICBId_SlaveType eSlave = pInfo->eSlave;
  unsigned int i;
  
  for( i=0; i < SocInfo.uSlaveCount ; i++ )
  {
    pInfo->pCurrSlave = SocInfo.pSlaveList[i];
    if( NULL != pInfo->pCurrSlave && 
        pInfo->pCurrSlave->slaveId == eSlave )
    {
      return true;
    }
  }
  pInfo->pCurrSlave = NULL;
  return false;
}

/**
  @copydoc AxiCfgInt_HaltMasterPort
 */
AxiCfg_ReturnCodeType AxiCfgInt_HaltMasterPort( ICBId_MasterType eMaster, bool bLock )
{
  AxiCfg_ReturnCodeType ret = AXICFG_SUCCESS;
  struct axicfg_master_search_info info;

  /* Search for the correct master port structure */
  info.pCurrMaster = NULL;
  info.ePort       = eMaster;

  /* Check three things:
   * The HAL handle must not be NULL
   * The port must be found
   * The port (once found) is haltable
   */
  if(SocInfo.pTcsrInfo && 
     AxiCfgInt_SearchPort(&info) &&
     info.pCurrMaster->pHaltPorts)
  {
    if( bLock )
    {
      AxiCfgOs_MutexLock();
    }

    if(!HAL_tcsr_HaltPort(SocInfo.pTcsrInfo, info.pCurrMaster->pHaltPorts[0]))
    {
      ret = AXICFG_ERROR_TIMEOUT;
    }

    if( bLock )
    {
      AxiCfgOs_MutexUnlock();
    }
  }
  else
  {
    ret = AXICFG_ERROR_INVALID;
  }
  
  return ret;
}

/**
  @copydoc AxiCfgInt_UnhaltMasterPort
 */
AxiCfg_ReturnCodeType AxiCfgInt_UnhaltMasterPort( ICBId_MasterType eMaster, bool bLock )
{
  AxiCfg_ReturnCodeType ret = AXICFG_SUCCESS;
  struct axicfg_master_search_info info;

  /* Search for the correct master port structure */
  info.pCurrMaster = NULL;
  info.ePort       = eMaster;

  /* Check three things:
   * The HAL handle must not be NULL
   * The port must be found
   * The port (once found) is haltable
   */
  if(SocInfo.pTcsrInfo &&
     AxiCfgInt_SearchPort(&info) &&
     info.pCurrMaster->pHaltPorts)
  {
    if( bLock )
    {
      AxiCfgOs_MutexLock();
    }

    if(!HAL_tcsr_UnhaltPort(SocInfo.pTcsrInfo, info.pCurrMaster->pHaltPorts[0]))
    {
      ret = AXICFG_ERROR_TIMEOUT;
    }

    if( bLock )
    {
      AxiCfgOs_MutexUnlock();
    }
  }
  else
  {
    ret = AXICFG_ERROR_INVALID;
  }

  return ret;
}
/**
  @brief Used to check if an external master id is a BIMC master id

  @pre   AxiCfgInt_SearchPort returned true on pInfo
         NULL != puCount
         NULL != puMasterPort

  @param[inout] pInfo         pointer to structure that's used to hold master search info
  @param[out] puMasterPort  pointer to internal master port id array
  @param[out] puCount       pointer to internal master port id count

  @return bool
    @retval true  if pInfo->ePort is valid and puMasterPort, puCount are set
    @retval false if pInfo->ePort is invalid. Both puMasterPort and puCount are not set
 */
bool AxiCfgInt_GetBimcMasterInfo( struct axicfg_master_search_info *pInfo, uint32_t ** puMasterPort, uint32_t * puCount  )
{
  if( ICB_BUS_BIMC == pInfo->pCurrMaster->pBus->busType )
  {
    *puMasterPort = pInfo->pCurrMaster->pPortIds;
    *puCount = pInfo->pCurrMaster->uNumPorts;
    return true;
  }

  return false;
}

/**
  @brief Used to check if an external master id is remoted

  @pre   AxiCfgInt_SearchPort returned true on pInfo

  @param[in] pInfo  pointer to structure that's used to hold master search info
 
  @return bool
    @retval true  if pInfo->ePort is remoted
    @retval false if pInfo->ePort is not remoted or unsupported 
 */
bool AxiCfgInt_IsRemoteMaster( struct axicfg_master_search_info *pInfo )
{
  return  pInfo->pCurrMaster->bRemote;
}

/**
  @brief Used to check if an external slave id is a BIMC slave id

  @pre   AxiCfgInt_SearchSlave returned true on pInfo
         puSlaveWay != NULL
         puCount != NULL 

  @param[inout] pInfo         pointer to structure that's used to hold slave search info
  @param[out] puSlaveWay    pointer to internal slave port id array
  @param[out] puCount       pointer to internal slave port id count

  @return bool
    @retval true  if pInfo->eSlave is valid and puSlaveWay, puCount are set
    @retval false if pInfo->eSlave is invalid. Both puSlaveWay and puCount are not set
 */
bool AxiCfgInt_GetBimcSlaveInfo( struct axicfg_slave_search_info *pInfo, uint32_t ** puSlaveWay, uint32_t * puCount )
{
  if( ICB_BUS_BIMC == pInfo->pCurrSlave->pBus->busType )
  {
    *puSlaveWay = pInfo->pCurrSlave->pPortIds;
    *puCount = pInfo->pCurrSlave->uNumPorts;
    return true;
  }

  return false;
}

/**
  @brief Used to check if an external slave id is remoted

  @pre   AxiCfgInt_SearchSlave returned true on pInfo

  @param[in] pInfo         pointer to structure that's used to hold slave search info
 
  @return bool
    @retval true  if eSlave is remoted
    @retval false if eSlave is not remoted or unsupported 
 */
bool AxiCfgInt_IsRemoteSlave( struct axicfg_slave_search_info *pInfo )
{
  return pInfo->pCurrSlave->bRemote;
}
  
/**
  @brief Used to get master hal info

  @pre   AxiCfgInt_SearchPort returned true on pInfo
         pSearchInfo != NULL 
         pHalInfo != NULL 
         *pHalInfo != NULL

  @param[in]  pSearchInfo   pointer to structure that's used to hold master search info
  @param[out] pHalInfo      a pointer to master hal info
 */
void AxiCfgInt_GetMasterHalInfo( struct axicfg_master_search_info *pSearchInfo, void **pHalInfo )
{
  *pHalInfo = pSearchInfo->pCurrMaster->pBus->pInfo;
}

/**
  @brief Used to get slave hal info

  @pre   AxiCfgInt_SearchSlave returned true on pInfo
         pSearchInfo != NULL 
         pHalInfo != NULL 
         *pHalInfo != NULL

  @param[in]  pSearchInfo   pointer to structure that's used to hold slave search info
  @param[out] pHalInfo      a pointer to slave hal info
 */
void AxiCfgInt_GetSlaveHalInfo( struct axicfg_slave_search_info *pSearchInfo, void **pHalInfo )
{
  *pHalInfo = pSearchInfo->pCurrSlave->pBus->pInfo;
}

/**
  @brief  The method called to reprogram the hardware to default/reset values.

  @return bool
      @retval true  if successful
      @retval false if failed
 */
bool AxiCfgInt_Reset( void )
{
  if ( AXICFGINT_STATE_INITIALIZED == state )
  {
    /* Request all config clocks. */
    _AxiCfgInt_EnableAllConfigClocks();

    _AxiCfgInt_SetMasterDefaults();
    _AxiCfgInt_SetSlaveDefaults(); 

    /* Remove our config clock requests. */
    _AxiCfgInt_DisableAllConfigClocks();

    /* Commit changes to the RPM. */
    AxiCfgInt_Commit(true);
    return true;
  }
  return false; 
}

/**
  @brief  The first method called to initialize AxiCfg driver. 
          Will set up all the required resources including locks, memory maps

  @return bool
      @retval true  if successful
      @retval false if failed or not yet complete

  @post Wait on AXICFG_INIT_DONE marker to know when initialization is complete.
 */
bool AxiCfgInt_Init( void )
{
  unsigned int i; 

  /* If we've never tried to initialize, and the OS layer initialized successfully,
   * continue with our initialization. */
  if( AXICFGINT_STATE_UNINITIALIZED == state && AxiCfgOs_Init() )
  {
    state = AXICFGINT_STATE_INITIALIZING;

    /* Check to see if the tcsr property is defined. */
    if( NULL != AxiCfgOs_GetProperty("icb_cfg_tcsr_info"))
    {
      /* Property defined */
      SocInfo.pTcsrInfo = *( (HAL_tcsr_InfoType **) AxiCfgOs_GetProperty("icb_cfg_tcsr_info"));

      /* The info pointer may be NULL, which means this feature isn't supported on this target.
      * If it isn't NULL, memmap the required hardware address space, and initialize the HAL. */
      if( SocInfo.pTcsrInfo != NULL )
      {
        SocInfo.pTcsrInfo->pBaseAddr = AxiCfgOs_Mmap( SocInfo.pTcsrInfo->pszBaseName,
                                                      SocInfo.pTcsrInfo->pBaseAddr,
                                                      SocInfo.pTcsrInfo->uRegionSize );
        if( NULL != SocInfo.pTcsrInfo->pBaseAddr )
        {
          AxiCfgOs_MutexLock( ); 
          HAL_tcsr_Init(SocInfo.pTcsrInfo);
          AxiCfgOs_MutexUnlock( ); 
        }
        else
        {
          /* Memory map error. */
          return false;
        }
      }
    }
    else
    {
      /* Feature not supported on this target. */
      SocInfo.pTcsrInfo = NULL;
    }

    SocInfo.uBusCount = *( (uint32_t *) AxiCfgOs_GetProperty("icb_cfg_bus_count") );  
    if( 0 == SocInfo.uBusCount )
    {
      //nothing to initialize
      state = AXICFGINT_STATE_INITIALIZED;
      return true;
    }

    /* Get the bus information. */
    SocInfo.pBusList = ( icb_bus_def_type ** )AxiCfgOs_GetProperty("icb_cfg_bus_list");

    //map the bus space
    for( i=0; i<SocInfo.uBusCount; i++ )
    {
      icb_bus_def_type * pCurrBus = SocInfo.pBusList[i];

      /* Validate bus parameters */
      if( NULL == pCurrBus || 
          (NULL == pCurrBus->pInfo && ICB_BUS_VIRT != pCurrBus->busType) )
      {
        //error
        return false;
      }

      if( pCurrBus->bRemote ) //no local resource
      {
        continue; 
      }

      /* Map memory for the bus device registers, if necessary. */
      switch( pCurrBus->busType )
      {
        case ICB_BUS_BIMC:
          ((HAL_bimc_InfoType *)pCurrBus->pInfo)->pBaseAddr = AxiCfgOs_Mmap( pCurrBus->pszBaseName, 
                                                                             pCurrBus->pBaseAddr,
                                                                             pCurrBus->uRegionSize );
          if ( NULL != ((HAL_bimc_InfoType *)pCurrBus->pInfo)->pBaseAddr )
          {
            AxiCfgOs_MutexLock( ); 
            HAL_bimc_Init( (HAL_bimc_InfoType *)pCurrBus->pInfo );
            AxiCfgOs_MutexUnlock( ); 
          }
          else
          {
            //error on mapping
            return false;
          }
          break;
        case ICB_BUS_NOC:
          ((HAL_noc_InfoType*)pCurrBus->pInfo)->pBaseAddr = AxiCfgOs_Mmap( pCurrBus->pszBaseName,
                                                                           pCurrBus->pBaseAddr,
                                                                           pCurrBus->uRegionSize );
          if( NULL != ((HAL_noc_InfoType*)pCurrBus->pInfo)->pBaseAddr )
          {
            AxiCfgOs_MutexLock( ); 
            HAL_noc_Init( (HAL_noc_InfoType*)pCurrBus->pInfo );
            AxiCfgOs_MutexUnlock( ); 
          }
          else
          {
            //error on mapping
            return false;
          }
          break;
        case ICB_BUS_VIRT:
          /* Nothing to do here. */
          continue;
        default:
          /* Invalid bus type. */
          return false;
      }
    }

    /* Get the master information. */
    SocInfo.uMasterCount = *( (uint32_t *)AxiCfgOs_GetProperty("icb_cfg_master_count") );
    SocInfo.pMasterList = ( icb_master_def_type ** )AxiCfgOs_GetProperty("icb_cfg_master_list");

    /* Map any auxiliary throttles that may be present. */
    for( i=0; i<SocInfo.uMasterCount; i++ )
    {
      icb_master_def_type *pMaster = SocInfo.pMasterList[i];

      if( NULL != pMaster->pAuxThrottle )
      {
        HAL_throttle_InfoType *pInfo = &pMaster->pAuxThrottle->info;
        pInfo->pBaseAddr = AxiCfgOs_Mmap( pInfo->pszBaseName,
                                          pInfo->pBaseAddr,
                                          pInfo->uRegionSize );
        if( NULL != pInfo->pBaseAddr )
        {
          AxiCfgOs_MutexLock( ); 
          HAL_throttle_Init(pInfo);
          AxiCfgOs_MutexUnlock( ); 
        }
        else
        {
          /* Memory map error. */
          return false;
        }
      }
    }

    /* Get the slave information */
    SocInfo.uSlaveCount = *( (uint32_t *)AxiCfgOs_GetProperty("icb_cfg_slave_count") );
    SocInfo.pSlaveList = ( icb_slave_def_type ** )AxiCfgOs_GetProperty("icb_cfg_slave_list");
    
    /* If this works, all we have left to do is wait for our clocks to become available. */
    if( _AxiCfgInt_CreateKvpBuffer() )
    {
      uint32_t     uNumCfgClocks = *( (uint32_t *) AxiCfgOs_GetProperty("icb_cfg_clk_count") );
      const char **ppszCfgClocks = ( const char **)AxiCfgOs_GetProperty("icb_cfg_clock_node_list");
    //initialization done
      /* Set our callback to fire when all our clocks are available. */
      npa_resources_available_cb( uNumCfgClocks, ppszCfgClocks, _AxiCfgInt_InitCb, NULL );
    }
  }

  return state == AXICFGINT_STATE_INITIALIZED;
}

/**
  @brief Configures the QoS Generator bandwidth for a master.
         Valid only when master QOS mode is either Limited or Regulator.

  @param[in]  ePort       Master port id
  @param[in]  u64TotalBW  Total aggregated bandwidth value for potential thresholding
  @param[in]  u64ActiveBW QOS generator bandwidth in bytes per second( BpS ) for Active/Next Active
  @param[in]  u64SemiBW   QOS generator bandwidth in bytes per second( BpS ) for Semi-Active
  @param[in]  u64SleepBW  QOS generator bandwidth in bytes per second( BpS ) for Sleep
  @param[in]  bNextActive Boolean indicating that the active request is for next active set

  @return int32_t
        @retval 0   When successfully configured 
 */
int32_t AxiCfgInt_QoSSetMasterBW
( 
  ICBId_MasterType ePort,
  uint64_t         u64TotalBW,
  uint64_t         u64ActiveBW,
  uint64_t         u64SemiBW,
  uint64_t         u64SleepBW,
  bool             bNextActive
)
{
  struct axicfg_master_search_info SearchInfo;

  /* If the port search fails, return. */
  SearchInfo.ePort       = ePort;
  SearchInfo.pCurrMaster = NULL;
  if( !AxiCfgInt_SearchPort( &SearchInfo ) )
  {
    return AXICFG_INT_UNSUPPORTED;  
  }

  if( !AxiCfgInt_IsRemoteMaster( &SearchInfo ) )
  {
    _AxiCfgInt_QoSSetLocal(SearchInfo.pCurrMaster, u64TotalBW, u64ActiveBW);
  }
  else
  {
    if( ICB_BUS_NOC == SearchInfo.pCurrMaster->pBus->busType )
    {
      AxiCfgOs_MutexLock( ); 
      _AxiCfgInt_SendNocMasterBW( ePort, u64ActiveBW, u64SemiBW, u64SleepBW, bNextActive );
      AxiCfgOs_MutexUnlock( ); 
    }
    /* If this is a BIMC master, handle it. */
    else if( ICB_BUS_BIMC == SearchInfo.pCurrMaster->pBus->busType )
    {
      HAL_bimc_QosBandwidthType BW;
      HAL_bimc_QosBandwidthType SemiBW;
      HAL_bimc_QosBandwidthType SleepBW;

      /* Set bandwidths. Thresholds are unused. */
      BW.uBandwidth      = u64ActiveBW;
      SemiBW.uBandwidth  = u64SemiBW;
      SleepBW.uBandwidth = u64SleepBW;

      AxiCfgOs_MutexLock( ); 
      _AxiCfgInt_SendBimcMasterBW( ePort, &BW, &SemiBW, &SleepBW, bNextActive );
      AxiCfgOs_MutexUnlock( );
    }
  }

  return AXICFG_INT_SUCCESS;
}

/**
 @copydoc AxiCfgInt_QoSSetSlaveBW
 */
int32_t AxiCfgInt_QoSSetSlaveBW
( 
  ICBId_SlaveType eSlave,
  uint64_t        u64ActiveBW,
  uint64_t        u64SemiBW,
  uint64_t        u64SleepBW,
  bool            bNextActive
)
{
  struct axicfg_slave_search_info info;

  /* Make sure we have this slave and it is remote. */
  info.eSlave     = eSlave;
  info.pCurrSlave = NULL;
  if( !AxiCfgInt_SearchSlave( &info ) || !AxiCfgInt_IsRemoteSlave( &info ) )
  {
    return AXICFG_INT_UNSUPPORTED;  
  }

  _AxiCfgInt_SendSlaveBW( eSlave, u64ActiveBW, u64SemiBW, u64SleepBW, bNextActive );
  return AXICFG_INT_SUCCESS;
}

/**
  @brief Return whether or not the master supports latency requests.

  @param[in] eMaster Master port id

  @return bool
        @retval true  When the master supports a latency request
        @retval false Otherwise
 */
bool AxiCfgInt_IsMasterLatencySupported
(
  ICBId_MasterType eMaster
)
{
  bool retval = false;
  struct axicfg_master_search_info SearchInfo;
  SearchInfo.ePort       = eMaster;
  SearchInfo.pCurrMaster = NULL;

  if( AxiCfgInt_SearchPort( &SearchInfo ) )
  {
    retval = SearchInfo.pCurrMaster->bLatency;
  }

  return retval;
}

/**
  @brief Configures the latency request for a master.

  @param[in]  eMaster          Master port id
  @param[in]  u32ActiveLatency Latency in nanoseconds for Active/Next Active
  @param[in]  u32SemiLatency   Latency in nanoseconds for Semi-Active
  @param[in]  u32SleepLatency  Latency in nanoseconds for Sleep
  @param[in]  bNextActive      Boolean indicating that the active request is for next active set

  @return int32_t
        @retval AXICFG_INT_SUCCESS        When successfully configured 
        @retval AXICFG_INT_UNSUPPORTED    When the port is unsupported
 */
int32_t AxiCfgInt_QoSSetMasterLatency
(
  ICBId_MasterType eMaster,
  uint32_t         u32ActiveLatency,
  uint32_t         u32SemiLatency,
  uint32_t         u32SleepLatency,
  bool             bNextActive
)
{
  struct axicfg_master_search_info SearchInfo;
  SearchInfo.ePort       = eMaster;
  SearchInfo.pCurrMaster = NULL;

  if( !AxiCfgInt_SearchPort( &SearchInfo ) ||
      !SearchInfo.pCurrMaster->bLatency )
  {
    return AXICFG_INT_UNSUPPORTED;  
  }

  _AxiCfgInt_SendMasterLatency(eMaster, u32ActiveLatency, u32SemiLatency, u32SleepLatency, bNextActive );
  return AXICFG_INT_SUCCESS;
}

/**
 @copydoc AxiCfgInt_Commit
 */
void AxiCfgInt_Commit( bool bWait )
{
  /* Barrier on the current message id, but only if they want us to wait. */
  if( bWait )
  {
    rpm_barrier( uMsgId );
  }

  /* Clear the message id, now that we're done. */
  uMsgId = 0;
}

/**
  @copydoc AxiCfgInt_SetQosPolicy
 */
AxiCfg_ReturnCodeType AxiCfgInt_SetQosPolicy
(
  struct axicfg_master_search_info *pInfo,
  AxiCfg_QosPolicyType *            pQosPolicy,
  axicfg_node_state_type *          pState
)
{
  AxiCfg_ReturnCodeType ret = AXICFG_ERROR_INVALID;
  icb_master_def_type *pMaster = pInfo->pCurrMaster;

  AxiCfgOs_MutexLock();

  do
  {
    /* Verify we have previous default settings and ports to set. */
    if( ( NULL != pMaster->pDefaults ) &&
        ( NULL != pMaster->pQosPortIds ) )
    {
      /* Check for policy clear. */
      if( NULL == pQosPolicy )
      {
        /* Nothing to do unless we had a previous override. */
        if( NULL != pMaster->pQosPolicyOverride )
        {
          DALSYS_Free(pMaster->pQosPolicyOverride);
          pMaster->pQosPolicyOverride = NULL;

          /* Reconfigure the old defaults. */
          _AxiCfgInt_QoSSetLocal( pMaster, pState->uTotalBW, pState->uAb + pState->uSuppAb + pState->uActiveOnlyAb ); 
        }
      }
      else
      {
        /* If we did not have a previous override, allocate a new one.
        * Otherwise, just overwrite it. */
        if( NULL == pMaster->pQosPolicyOverride )
        {
          uint32_t size;
          
          switch( pMaster->pBus->busType )
          {
            case ICB_BUS_BIMC:
              size = sizeof(icb_bimc_master_defaults_type);
              break;
            case ICB_BUS_NOC:
              size = sizeof(icb_noc_master_defaults_type);
              break;
            default:
              return ret;
          }

          if(DAL_SUCCESS !=
            DALSYS_Malloc( size, &pMaster->pQosPolicyOverride ))
          {
            break;
          }
        }

        /* Convert and configure the new defaults. */
        _AxiCfgInt_ConvertQosPolicy( pMaster, pQosPolicy );
        _AxiCfgInt_QoSSetLocal( pMaster, pState->uTotalBW, pState->uAb + pState->uSuppAb + pState->uActiveOnlyAb ); 
      }

      ret = AXICFG_SUCCESS;
    }
  } while(false);

  AxiCfgOs_MutexUnlock();
  return ret;
}

