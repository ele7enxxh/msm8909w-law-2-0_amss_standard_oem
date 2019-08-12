/**
 * @file ul_int.c
 *
 * @note This file implements the internal arbitration functions
 *
 *  Copyright (c) 2010-2014 Qualcomm Technologies Incorporated.
 *             All Rights Reserved.
 *          QUALCOMM Proprietary/GTDR
 *
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/ul_int.c#1 $
 * $DateTime: 2016/12/13 07:59:23 $
 * $Author: mplcsds1 $
 * $Change: 11985146 $
 *                     EDIT HISTORY FOR FILE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * when       who     what, where, why
 * --------   ---     -----------------------------------------------------------
 * 06/18/15   sds     Add SCHEME_1 support.
 * 05/18/15   sds     Add support for notification interface.
 * 04/04/15   jc      Do not modify suppressible AB number before calculating active only AB.
 * 03/10/15   sds     Add support for new clock div types and align div method with RPM/Sim
 * 02/26/15   sds     Add APIs to fetch node aggregation state.
 * 12/12/14   jc      Support for NPA active-only clients and RPM semi-active set.
 * 10/02/14   sds     Add number of ports for aggregation purposes to masters/slaves.
 * 07/28/14   sds     Banned API removal.
 * 06/20/14   sds     Extend request types to add latency request in addition to BW.
 * 03/21/14   sds     Modify AGG_SCHEME_0 slightly - don't divide IB by ports.
 * 03/11/14   sds     Extend query function - slave port count.
 * 11/05/13   jc      Add support for client type change.
 * 07/12/13   sds     Add support for BKE thresholding and static configuration.
 * 04/29/13   sds     Fix AB/IB aggregation to clock frequency. Make sure to divide
 *                    the AB by the number of ports.
 * 11/27/12   sds     Fix NAS attribute check.
 * 11/15/12   sds     Fix NAS handling and move all RPM interactions out of this layer.
 *                    Also remove usage of unnecessary slave id array.
 * 10/03/12   sds     Clock cleanup and master clock aggregation added.
 * 08/21/12   sds     remove support for npa fork/join and add support for npa's
 *                    fire and forget
 * 04/25/12   dj      add dal config support
 * 03/16/12   pm      Removed SIGN macro to eliminate compiler warning
 * 04/29/11   rng     Added support for requested bandwidth tier
 * 04/27/11   rng     Changed bandwidth to 64 bits
 * 04/27/11   sds     Cleanup compiler warning
 * 02/11/11   rng     Add debug log messages
 * 08/17/10   rng     Slave resources cannot have subnode dependencies
 * 07/15/10   pm      Revisions to get basic functionality on WINSIM
 * 04/27/10   rng     Initial version
 *
 */

#include "ul_i.h"
#include "ddiaxicfgint.h"
#include "icb_rpm_types.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "ULogFront.h"
#include "ul_tree.h"
#include "stringl.h"

/*============================================================================
                          DEFINES
============================================================================*/

/* Divide and round up to the nearest integer */
#define CEIL_DIV(dividend, divisor) (((dividend)/(divisor))+(((dividend)%(divisor))?1:0))

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

#define BW_TO_CLK_FREQ_KHZ( bw, bus_width ) \
          (uint32)(CEIL_DIV((bw),((bus_width) * 1000)))

#define BW_TO_CLK_FREQ_RAW( bw, bus_width ) \
          (uint32)(CEIL_DIV((bw),((bus_width) & ~ICB_WIDTH_TYPE_MASK)))

#define UL_BW_REQUEST_LIST_SIZE    10

#define UL_INT_BW_ORDER_COUNT       3

/**
 * Slave Request Type
 *
 * Structure of Ib and Ab values, to pass to a Slave node when
 * requesting bandwidth 
 */
typedef struct
{
  int32                i32IntMasterID;
  int32                i32IntSlaveID;
  uint64               uIb;       /**< Instantaneous BW request in Bytes/Sec */
  uint64               uAb;       /**< Arbitrated BW request in Bytes/Sec */
} ul_slave_bw_request_type;

typedef struct
{
  ul_treeNodeType       * routeTreeRoot; /* Route tree root */
  uint32                  uNumMasters;
  uint32                  uNumSlaves;
  ul_master_data_type  ** aMasterList;
  ul_slave_data_type   ** aSlaveList;
} ul_int_data_type;

typedef enum
{
  UL_INT_CLOCK_REQUEST_REQUIRED = 0,
  UL_INT_CLOCK_REQUEST_SUPPRESSIBLE,
  UL_INT_CLOCK_REQUEST_ACTIVEONLY,

  /* Insert new request types above here */
  UL_INT_CLOCK_REQUEST_MAX,

  UL_INT_CLOCK_REQUEST_PLACEHOLDER = 0x7FFFFFFF  /* Force to 32 bits */
} ul_int_clock_request_type;

typedef struct
{
  ul_int_clock_request_type clkReq;
  uint64 uBW;
} ul_int_bw_order_type;

/* forward declarations */
void process_master_latency_request( ul_request_type * );
void process_master_request( ul_request_type * );
void process_slave_request( ul_request_type * );

/* extern the Arbiter Internal Log */
extern ULogHandle icbArbLog;

/*============================================================================
                          STATIC INTERNAL DATA
============================================================================*/

static ul_int_data_type ul_int;

/*============================================================================
                      STATIC FUNCTIONS - CLOCKS
============================================================================*/
/**=============================================================================

  ul_clock_create_clients

  @brief Performs creation of clock handles for all clocks

  @param None

  @return None

==============================================================================*/
static void ul_clock_create_clients
(
  ul_clock_type *    clock,
  const char * const clientName
)
{
  clock->normal =
    npa_create_sync_client( clock->name, clientName, NPA_CLIENT_REQUIRED );
  clock->suppressible =
    npa_create_sync_client( clock->name, clientName, NPA_CLIENT_SUPPRESSIBLE );
  clock->activeonly =
    npa_create_sync_client( clock->name, clientName, NPA_CLIENT_SUPPRESSIBLE2 );
}

/**=============================================================================

  ul_clock_issue_request

  @brief Issues requests on the clock to the appropriate handle

  @param None

  @return None

==============================================================================*/
static void ul_clock_issue_request
(
  ul_clock_type *       clock,
  npa_request_attribute reqAttrs,
  ul_int_clock_request_type clkReq,
  uint32                uFreq
)
{
  npa_client_handle hClient;

  /* Get the proper client, accounting for suppressible clients. */
  switch( clkReq )
  {
    case UL_INT_CLOCK_REQUEST_SUPPRESSIBLE:
      hClient = clock->suppressible;
      break;
    case UL_INT_CLOCK_REQUEST_ACTIVEONLY:
      hClient = clock->activeonly;
      /* Work-around until clock driver supports NPA_CLIENT_SUPPRESSIBLE2 */
      if ( NULL == hClient )
      {
        return;
      }
      break;
    case UL_INT_CLOCK_REQUEST_REQUIRED:
    default:
      hClient = clock->normal;
      break;
  }


  /* Set our request attributes, and go. */
  npa_set_request_attribute( hClient, reqAttrs );
  npa_issue_required_request( hClient, uFreq );
}

/*============================================================================
                     STATIC FUNCTIONS - NOTIFICATIONS
============================================================================*/
/** ul_notify_evaluate
 *
 * @brief Evaluate and update the current notification status for the passed notification.
 *        May trigger a callback.
 *
 */
void ul_notify_evaluate
(
  icb_notify_type *   notify,
  ul_node_state_type *state
)
{
  uint64_t state_val;
  bool callback_required = false;

  /* Calculate new state values.
   * IB is the max of all active requests.
   * AB is the sum of all active requests.
   * Total is the per-channel total calculated by compute_bw().
   *   This does have any util/vrail_comp factors applied. */
  /* Save and update state. */
  notify->status.ib    = MAX(state->uIb, MAX(state->uSuppIb, state->uActiveOnlyIb));
  notify->status.ab    = state->uAb + state->uSuppAb + state->uActiveOnlyAb;
  notify->status.total = state->uTotalBW;
  notify->status_valid = true;

  /* Collect our pre/post type that this notification cares about. */
  switch(notify->definition.threshold_type)
  {
    case ICB_NOTIFY_AB:
      state_val = notify->status.ab;
      break;
    case ICB_NOTIFY_IB:
      state_val = notify->status.ib;
      break;
    case ICB_NOTIFY_TOTAL:
      state_val = notify->status.total;
      break;
    default:
      /* Something is seriously broken if we get here. */
      return;
  }

  /* Check to see if the high threshold was crossed.
   * Only check if the threshold wasn't already crossed. */
  if(false == notify->status.high_threshold &&
     state_val > notify->definition.high_threshold)
  {
    notify->status.high_threshold = true;
    callback_required = true;
  }

  /* Check to see if the low threshold was crossed.
   * Only check if the threshold wasn't already crossed. */
  if(false == notify->status.low_threshold &&
     state_val < notify->definition.low_threshold)
  {
    notify->status.low_threshold = true;
    callback_required = true;
  }

  /* Issue the callback if necessary. */
  if(callback_required)
  {
    /* Pass a copy of the state data.
     * We don't want to pass a pointer to the actual storage location. */
    icb_notify_status_type status = notify->status;
    notify->callback(notify, &status, notify->callback_data);
  }
}

/** ul_notify_node_update
 *
 * @brief A node has been updated.
 *        This function is called to process any notifications this node has.
 */
void ul_notify_node_update
(
  icb_notify_list_type *notify_list,
  ul_node_state_type *  state
)
{
  icb_notify_type *iter = notify_list->head;
  uint32_t i;

  for( i = 0;
       i < notify_list->count && NULL != iter;
       i++ )
  {
    /* Evaluate this notification. */
    ul_notify_evaluate(iter, state);

    /* Get the next in the list. */
    iter = iter->next;
  }
}

/*============================================================================
                          STATIC FUNCTIONS
============================================================================*/
/**=============================================================================

  ul_target_init_cb

  @brief Performs post master node creation initialization

  @param None

  @return None

==============================================================================*/
static void ul_target_init_cb
(
  void *       pContext,
  unsigned int u32EventType,
  void *       pData,
  unsigned int u32DataSize
)
{
  uint32 uIdx, uClockIdx;

  /* Unused parameters */
  (void)pContext;
  (void)u32EventType;
  (void)pData;
  (void)u32DataSize;

  /* Create necessary bus and clock NPA clients */
  for( uIdx = 0;
       uIdx < ul_int.uNumMasters;
       uIdx++ )
   {
    ul_master_data_type *pMaster     = ul_int.aMasterList[uIdx];
    pMaster->masterRequest.req_func  = process_master_request;
    pMaster->latencyRequest.req_func = process_master_latency_request;
    /* Slave clocks. */
    for( uClockIdx = 0;
         uClockIdx < pMaster->u32MstrClockCount;
         uClockIdx++ )
    {
      ul_clock_create_clients( &pMaster->aMstrClocks[uClockIdx],
                                pMaster->clientName );
    }
  }

  for( uIdx = 0;
       uIdx < ul_int.uNumSlaves;
       uIdx++ )
  {
    ul_slave_data_type *pSlave = ul_int.aSlaveList[uIdx];
    pSlave->slvRequest.req_func= process_slave_request;
    /* Slave clocks. */
    for( uClockIdx = 0;
         uClockIdx < pSlave->u32SlvClockCount;
         uClockIdx++ )
    {
      ul_clock_create_clients( &pSlave->aSlvClocks[uClockIdx],
                                pSlave->clientName );
    }

    /* Memory clocks. */
    for( uClockIdx = 0;
         uClockIdx < pSlave->u32MemClockCount;
         uClockIdx++ )          
    {
      ul_clock_create_clients( &pSlave->aMemClocks[uClockIdx],
                                pSlave->clientName );
    }
  }

  /* All master nodes have been defined so issue the marker */
  npa_define_marker( UL_MASTERS_CREATED );
}

/**=============================================================================

  compute_agg1_lookup

  @brief Look up the P0 factor for AGG1

  @param pAgg - A pointer to the agg scheme data
  @param uBW -  The associated bandwidth

  @return The P0 factor for the associated requested bandwidth

==============================================================================*/
uint32 compute_agg1_lookup( icb_agg_info_type *pAgg, uint64 uBW )
{
    /* Search the table for the first level that matches.
      * If we don't find a match, we'll use the last entry. */
    uint32 i;
    uint64 req_mbps = CEIL_DIV(uBW, 1000000);
    for( i = 0; i < (pAgg->uP0 - 1); i++ )
    {
      if( req_mbps <= pAgg->levels[i].uBW )
      {
        break;
      }
    }

    return pAgg->levels[i].uP0;
}

/**=============================================================================

  compute_bw

  @brief Performs post master node creation initialization

  @param state - a pointer to the node state to work with

  @return The total bandwidth on the node.

==============================================================================*/
uint64 compute_bw( icb_agg_info_type *pAgg, ul_node_state_type *state, uint32 uPorts )
{
  uint64 uAB, uSuppAB, uActiveOnlyAB, uTotalBW;
  uint32 ao_factor = pAgg->uP0, supp_factor = pAgg->uP0, req_factor = pAgg->uP0;
  uint32 ports = MAX(1, uPorts);

  /* First, calculate the total bandwidth per port on this node. */
  if(state->uSuppAb == 0)
  {
    uSuppAB = 0;
  }
  else
  {
    uSuppAB = state->uAb + state->uSuppAb;
  }
  if(state->uActiveOnlyAb == 0)
  {
    uActiveOnlyAB = 0;
  }
  else
  {
    uActiveOnlyAB = state->uAb + state->uSuppAb + state->uActiveOnlyAb;
  }
  uAB = state->uAb;
  uTotalBW = MAX(MAX(state->uIb, MAX(state->uSuppIb, state->uActiveOnlyIb)),
                 CEIL_DIV(MAX(uAB, MAX(uSuppAB, uActiveOnlyAB)), ports));

  /* Update the input for the aggregation scheme. */
  switch(pAgg->aggType)
  {
    case ICB_AGG_SCHEME_1:
      ao_factor   = compute_agg1_lookup(pAgg, uActiveOnlyAB);
      supp_factor = compute_agg1_lookup(pAgg, uSuppAB);
      req_factor  = compute_agg1_lookup(pAgg, uAB);
      /* Intentional fall through. */
    case ICB_AGG_SCHEME_0:
      state->uActiveOnlyAb = CEIL_DIV(CEIL_DIV(uActiveOnlyAB * ao_factor, 100), ports);
      state->uSuppAb       = CEIL_DIV(CEIL_DIV(uSuppAB * supp_factor, 100), ports);
      state->uAb           = CEIL_DIV(CEIL_DIV(uAB * req_factor, 100), ports);

      state->uActiveOnlyIb = CEIL_DIV(state->uActiveOnlyIb * 100,  pAgg->uP1);
      state->uSuppIb       = CEIL_DIV(state->uSuppIb * 100,  pAgg->uP1);
      state->uIb           = CEIL_DIV(state->uIb * 100,  pAgg->uP1);
      break;
    case ICB_AGG_SCHEME_LEGACY:
    /* Use the legacy scheme if we don't know what scheme we have. */
    default:
      state->uActiveOnlyAb = CEIL_DIV(uActiveOnlyAB, ports);
      state->uSuppAb       = CEIL_DIV(uSuppAB, ports);
      state->uAb           = CEIL_DIV(uAB, ports);
      break;
  }

  return uTotalBW;
}

void process_master_latency_request
(
  ul_request_type * pRequest
)
{
  uint32 uSleepLatency, uSemiActiveLatency, uActiveLatency;
  uint32 uReqIdx;
  ul_master_data_type  * pMasterData = (ul_master_data_type*)pRequest->pRequest;
  ul_bw_request_type ** aRequestList;
  bool is_nas;

  /* See if this master supports latency request */
  if( !AxiCfgInt_IsMasterLatencySupported(pMasterData->eExtMasterID) )
  {
    return;
  }

  /* Set the nas flag */
  is_nas = (pRequest->hClient->request_attr & NPA_REQUEST_NEXT_AWAKE);
  aRequestList = pMasterData->requestList.aRequestList;

  /* Search the request list for the appropriate latencies */
  for( uSleepLatency = 0, uSemiActiveLatency = 0, uActiveLatency = 0, uReqIdx = 0;
       uReqIdx < pMasterData->requestList.uEntryCount; uReqIdx++ )
  {
    /* Sleep latencies only care about required requests. */
    if( NPA_CLIENT_VECTOR == aRequestList[uReqIdx]->hClient->type )
    {
      if( 0 == uSleepLatency ||
          (0 != aRequestList[uReqIdx]->uLatencyNs &&
           aRequestList[uReqIdx]->uLatencyNs < uSleepLatency) )
      {
        uSleepLatency = aRequestList[uReqIdx]->uLatencyNs;
      }
    }

    /* Semi-active latencies care about required and suppressible requests. */
    if( NPA_CLIENT_VECTOR == aRequestList[uReqIdx]->hClient->type ||
        NPA_CLIENT_SUPPRESSIBLE_VECTOR == aRequestList[uReqIdx]->hClient->type )
    {
      if( 0 == uSemiActiveLatency ||
          (0 != aRequestList[uReqIdx]->uLatencyNs &&
           aRequestList[uReqIdx]->uLatencyNs < uSemiActiveLatency) )
      {
        uSemiActiveLatency = aRequestList[uReqIdx]->uLatencyNs;
      }
    }

    /* Active latencies care about all requests. */
    if( 0 == uActiveLatency ||
        (0 != aRequestList[uReqIdx]->uLatencyNs &&
          aRequestList[uReqIdx]->uLatencyNs < uActiveLatency) )
    {
      uActiveLatency = aRequestList[uReqIdx]->uLatencyNs;
    }
  }

  /* Update RPM with latency request. */
  (void) AxiCfgInt_QoSSetMasterLatency( pMasterData->eExtMasterID,
                                        uActiveLatency,
                                        uSemiActiveLatency,
                                        uSleepLatency,
                                        is_nas );
}

static void order_bw_requests
(
  uint64 uBW,
  uint64 uSuppBW,
  uint64 uActiveOnlyBW,
  ul_int_bw_order_type * aBwOrder
)
{
  if (uBW >= uSuppBW && uBW >= uActiveOnlyBW)
  {
    aBwOrder[0].clkReq = UL_INT_CLOCK_REQUEST_REQUIRED;
    aBwOrder[0].uBW = uBW;
    if (uSuppBW >= uActiveOnlyBW)
    {
      aBwOrder[1].clkReq = UL_INT_CLOCK_REQUEST_SUPPRESSIBLE;
      aBwOrder[1].uBW = uSuppBW;
      aBwOrder[2].clkReq = UL_INT_CLOCK_REQUEST_ACTIVEONLY;
      aBwOrder[2].uBW = uActiveOnlyBW;
    }
    else
    {
      aBwOrder[1].clkReq = UL_INT_CLOCK_REQUEST_ACTIVEONLY;
      aBwOrder[1].uBW = uActiveOnlyBW;
      aBwOrder[2].clkReq = UL_INT_CLOCK_REQUEST_SUPPRESSIBLE;
      aBwOrder[2].uBW = uSuppBW;
    }
  }
  else if (uSuppBW >= uBW && uSuppBW >= uActiveOnlyBW)
  {
    aBwOrder[0].clkReq = UL_INT_CLOCK_REQUEST_SUPPRESSIBLE;
    aBwOrder[0].uBW = uSuppBW;
    if (uBW >= uActiveOnlyBW)
    {
      aBwOrder[1].clkReq = UL_INT_CLOCK_REQUEST_REQUIRED;
      aBwOrder[1].uBW = uBW;
      aBwOrder[2].clkReq = UL_INT_CLOCK_REQUEST_ACTIVEONLY;
      aBwOrder[2].uBW = uActiveOnlyBW;
    }
    else
    {
      aBwOrder[1].clkReq = UL_INT_CLOCK_REQUEST_ACTIVEONLY;
      aBwOrder[1].uBW = uActiveOnlyBW;
      aBwOrder[2].clkReq = UL_INT_CLOCK_REQUEST_REQUIRED;
      aBwOrder[2].uBW = uBW;
    }
  }
  else
  {
    aBwOrder[0].clkReq = UL_INT_CLOCK_REQUEST_ACTIVEONLY;
    aBwOrder[0].uBW = uActiveOnlyBW;
    if (uBW >= uSuppBW)
    {
      aBwOrder[1].clkReq = UL_INT_CLOCK_REQUEST_REQUIRED;
      aBwOrder[1].uBW = uBW;
      aBwOrder[2].clkReq = UL_INT_CLOCK_REQUEST_SUPPRESSIBLE;
      aBwOrder[2].uBW = uSuppBW;
    }
    else
    {
      aBwOrder[1].clkReq = UL_INT_CLOCK_REQUEST_SUPPRESSIBLE;
      aBwOrder[1].uBW = uSuppBW;
      aBwOrder[2].clkReq = UL_INT_CLOCK_REQUEST_REQUIRED;
      aBwOrder[2].uBW = uBW;
    }
  }
}

void process_master_request( ul_request_type * pRequest )
{
  uint64 uBW, uSuppBW, uActiveOnlyBW;
  uint32 uReqIdx, uOrderIdx;
  ul_master_data_type *pMasterData;
  bool is_nas;
  npa_request_attribute reqAttrs;
  ul_int_bw_order_type aBwOrder[UL_INT_BW_ORDER_COUNT];
  ul_node_state_type state;
  icb_agg_info_type *pAggInfo;

  pMasterData = (ul_master_data_type*)pRequest->pRequest;

  /* Process pending notifications (if any) */
  ul_notify_node_update(&pMasterData->notifications, &pMasterData->mstrState);

  /* Set the nas flag */
  is_nas = (pRequest->hClient->request_attr & NPA_REQUEST_NEXT_AWAKE);

  /* Collect request attributes to forward. */
  reqAttrs = (npa_request_attribute)pRequest->hClient->request_attr;
  reqAttrs &= ~NPA_REQUEST_CHANGED_TYPE;  /* Mask client type change attribute */


  /* Search the request list to get the MAX Ib */
  for( uBW = 0, uSuppBW = 0, uActiveOnlyBW = 0, uReqIdx = 0; uReqIdx < pMasterData->requestList.uEntryCount; uReqIdx++ )
  {
    if( NPA_CLIENT_SUPPRESSIBLE_VECTOR == pMasterData->requestList.aRequestList[uReqIdx]->hClient->type )
    {
      if (pMasterData->requestList.aRequestList[uReqIdx]->uIb > uSuppBW)
      {
        uSuppBW = pMasterData->requestList.aRequestList[uReqIdx]->uIb;
      }
    }
    else if( NPA_CLIENT_SUPPRESSIBLE2_VECTOR == pMasterData->requestList.aRequestList[uReqIdx]->hClient->type )
    {
      if (pMasterData->requestList.aRequestList[uReqIdx]->uIb > uActiveOnlyBW)
      {
        uActiveOnlyBW = pMasterData->requestList.aRequestList[uReqIdx]->uIb;
      }
    }
    else
    {
      if (pMasterData->requestList.aRequestList[uReqIdx]->uIb > uBW)
      {
        uBW = pMasterData->requestList.aRequestList[uReqIdx]->uIb;
      }
    }
  }

  pMasterData->mstrState.uActiveOnlyIb = uActiveOnlyBW;
  pMasterData->mstrState.uSuppIb = uSuppBW;
  pMasterData->mstrState.uIb = uBW;

  /* Check to see if the node overrides the bus clock calc. */
  if (ICB_AGG_SCHEME_UNUSED == pMasterData->pMasterDef->aggInfo.aggType)
  {
    pAggInfo = &pMasterData->pMasterDef->pBus->aggInfo;
  }
  /* Node overrides bus clock calculation. */
  else
  {
    pAggInfo = &pMasterData->pMasterDef->aggInfo;
  }

  /* Compute effective bandwidth based on aggregation scheme. */
  state = pMasterData->mstrState;
  pMasterData->mstrState.uTotalBW = compute_bw(pAggInfo, &state, pMasterData->pMasterDef->uNumAggPorts);
  uActiveOnlyBW = MAX(state.uActiveOnlyIb, state.uActiveOnlyAb);
  uSuppBW = MAX(state.uSuppIb, state.uSuppAb);
  uBW = MAX(state.uIb, state.uAb);

  /* Update QoS HW */
  (void) AxiCfgInt_QoSSetMasterBW( pMasterData->eExtMasterID,
                                   pMasterData->mstrState.uTotalBW,
                                   (pMasterData->mstrState.uAb +
                                    pMasterData->mstrState.uSuppAb +
                                    pMasterData->mstrState.uActiveOnlyAb),
                                   (pMasterData->mstrState.uAb +
                                    pMasterData->mstrState.uSuppAb),
                                   pMasterData->mstrState.uAb,
                                   is_nas );

  /* Issue the highest BW clock request first, to insure clock does not
   * glitch down.
   */
  order_bw_requests( uBW, uSuppBW, uActiveOnlyBW,  aBwOrder );
  for( uReqIdx = 0; uReqIdx < pMasterData->u32MstrClockCount; uReqIdx++)
  {
    for ( uOrderIdx = 0; uOrderIdx < UL_INT_BW_ORDER_COUNT; uOrderIdx++ )
    {
      uint32 clk_freq;
      if( 0 != (pMasterData->u32SlaveWidth & ICB_WIDTH_TYPE_RAW) )
      {
        clk_freq = BW_TO_CLK_FREQ_RAW(aBwOrder[uOrderIdx].uBW, pMasterData->u32SlaveWidth);
      }
      else
      {
        clk_freq = BW_TO_CLK_FREQ_KHZ(aBwOrder[uOrderIdx].uBW, pMasterData->u32SlaveWidth);
      }
      ul_clock_issue_request( &pMasterData->aMstrClocks[uReqIdx],
                               reqAttrs,
                               aBwOrder[uOrderIdx].clkReq,
                               clk_freq );
    }
  }
}

void process_slave_request( ul_request_type * pRequest )
{
  uint32 uReqIdx, uOrderIdx;
  ul_slave_data_type  * pSlaveData;
  bool is_nas;
  npa_request_attribute reqAttrs;
  uint64 uBW, uSuppBW, uActiveOnlyBW;
  ul_int_bw_order_type aBwOrder[UL_INT_BW_ORDER_COUNT];
  ul_node_state_type state;
  icb_agg_info_type *pAggInfo;

  pSlaveData = (ul_slave_data_type*)pRequest->pRequest;

  /* Process pending notifications (if any) */
  ul_notify_node_update(&pSlaveData->notifications, &pSlaveData->slvState);

  /* set the nas flag */
  is_nas = (pRequest->hClient->request_attr & NPA_REQUEST_NEXT_AWAKE);

  /* Collect request attributes to forward. */
  reqAttrs = (npa_request_attribute)pRequest->hClient->request_attr;
  reqAttrs &= ~NPA_REQUEST_CHANGED_TYPE;  /* Mask client type change attribute */

  /* Update BW to config layer. */
  (void)AxiCfgInt_QoSSetSlaveBW( pSlaveData->eExtSlaveID,
                                 (pSlaveData->slvState.uAb +
                                  pSlaveData->slvState.uSuppAb +
                                  pSlaveData->slvState.uActiveOnlyAb),
                                 (pSlaveData->slvState.uAb +
                                  pSlaveData->slvState.uSuppAb),
                                 pSlaveData->slvState.uAb,
                                 is_nas );

  /* Search the request list to get the MAX Ib */
  for( uBW = 0, uSuppBW = 0, uActiveOnlyBW = 0, uReqIdx = 0; uReqIdx < pSlaveData->requestList.uEntryCount; uReqIdx++ )
  {
    if( NPA_CLIENT_SUPPRESSIBLE_VECTOR == pSlaveData->requestList.aRequestList[uReqIdx]->hClient->type )
    {
      if( pSlaveData->requestList.aRequestList[uReqIdx]->uIb > uSuppBW )
      {
        uSuppBW = pSlaveData->requestList.aRequestList[uReqIdx]->uIb;
      }
    }
    else if( NPA_CLIENT_SUPPRESSIBLE2_VECTOR == pSlaveData->requestList.aRequestList[uReqIdx]->hClient->type )
    {
      if( pSlaveData->requestList.aRequestList[uReqIdx]->uIb > uActiveOnlyBW )
      {
        uActiveOnlyBW = pSlaveData->requestList.aRequestList[uReqIdx]->uIb;
      }
    }
    else
    {
      if( pSlaveData->requestList.aRequestList[uReqIdx]->uIb > uBW )
      {
        uBW = pSlaveData->requestList.aRequestList[uReqIdx]->uIb;
      }
    }
  }

  pSlaveData->slvState.uActiveOnlyIb = uActiveOnlyBW;
  pSlaveData->slvState.uSuppIb = uSuppBW;
  pSlaveData->slvState.uIb = uBW;

  /* Check to see if the node overrides the bus clock calc. */
  if (ICB_AGG_SCHEME_UNUSED == pSlaveData->pSlaveDef->aggInfo.aggType)
  {
    pAggInfo = &pSlaveData->pSlaveDef->pBus->aggInfo;
  }
  /* Node overrides bus clock calculation. */
  else
  {
    pAggInfo = &pSlaveData->pSlaveDef->aggInfo;
  }

  /* Compute effective bandwidth based on aggregation scheme. */
  state = pSlaveData->slvState;
  pSlaveData->slvState.uTotalBW = compute_bw(pAggInfo, &state, pSlaveData->pSlaveDef->uNumAggPorts);
  uActiveOnlyBW = MAX(state.uActiveOnlyIb, state.uActiveOnlyAb);
  uSuppBW = MAX(state.uSuppIb, state.uSuppAb);
  uBW = MAX(state.uIb, state.uAb);

  /* Issue the highest BW clock request first, to insure clock does not
   * glitch down.
   */
  order_bw_requests( uBW, uSuppBW, uActiveOnlyBW,  aBwOrder );
  for( uReqIdx = 0; uReqIdx < pSlaveData->u32SlvClockCount; uReqIdx++)
  {
    for ( uOrderIdx = 0; uOrderIdx < UL_INT_BW_ORDER_COUNT; uOrderIdx++ )
    {
      uint32 clk_freq;
      if( 0 != (pSlaveData->u32SlaveWidth & ICB_WIDTH_TYPE_RAW) )
      {
        clk_freq = BW_TO_CLK_FREQ_RAW(aBwOrder[uOrderIdx].uBW, pSlaveData->u32SlaveWidth);
      }
      else
      {
        clk_freq = BW_TO_CLK_FREQ_KHZ(aBwOrder[uOrderIdx].uBW, pSlaveData->u32SlaveWidth);
      }
      ul_clock_issue_request( &pSlaveData->aSlvClocks[uReqIdx],
                               reqAttrs,
                               aBwOrder[uOrderIdx].clkReq,
                               clk_freq );
    }
  }

  for( uReqIdx = 0; uReqIdx < pSlaveData->u32MemClockCount; uReqIdx++)
  {
    for ( uOrderIdx = 0; uOrderIdx < UL_INT_BW_ORDER_COUNT; uOrderIdx++ )
    {
      uint32 clk_freq;
      if( 0 != (pSlaveData->u32SlaveWidth & ICB_WIDTH_TYPE_RAW) )
      {
        clk_freq = BW_TO_CLK_FREQ_RAW(aBwOrder[uOrderIdx].uBW, pSlaveData->u32SlaveWidth);
      }
      else
      {
        clk_freq = BW_TO_CLK_FREQ_KHZ(aBwOrder[uOrderIdx].uBW, pSlaveData->u32SlaveWidth);
      }
      ul_clock_issue_request( &pSlaveData->aMemClocks[uReqIdx],
                               reqAttrs,
                               aBwOrder[uOrderIdx].clkReq,
                               clk_freq) ;
    }
  }
}

/**
 * @copydoc
 */
void ul_issue_pair_request
(
  npa_client_handle    client,
  ul_bw_request_type * pBWReq,
  ul_bw_request_type * pOldReq,
  ul_pair_type       * pMSPair,
  DALBOOL              bFirst
)
{
  ul_slave_data_type  * pSlaveData;
  ul_master_data_type * pMasterData = pMSPair->pMasterData;
  bool is_changed;

  /* Set the client type change flag */
  is_changed = (client->request_attr & NPA_REQUEST_CHANGED_TYPE);

  /* Check if the latency request is different than the previous request,
   * but only if this is the first hop in the route. */
  if( bFirst && 
      (pBWReq->uLatencyNs != pOldReq->uLatencyNs || is_changed) )
  {
    /* Log the request */
    ULOG_RT_PRINTF_2(icbArbLog,
                    "Issue Master Latency Request (MID: %d) (Latency: 0x%4d)",
                    pMasterData->eExtMasterID,
                    pBWReq->uLatencyNs );

    /* Enqueue the latency request. */
    pMasterData->latencyRequest.hClient = client;
    ul_request_enqueue( &pMasterData->latencyRequest );
  }

  /* Check if the new bandwidth request is different from the previous request */
  if( ( pBWReq->uAb == pOldReq->uAb && pBWReq->uIb == pOldReq->uIb ) && !is_changed )
  {
    /* Nothing to do so return */
    return;
  }

  /* Enqueue the slave request */
  pSlaveData = pMSPair->pSlaveData;
  pSlaveData->slvRequest.hClient = client; 
  ul_request_enqueue( &pSlaveData->slvRequest );

  /* Enqueue the master request */
  pMasterData->masterRequest.hClient = client;
  ul_request_enqueue( &pMasterData->masterRequest );

  /* Log the request */
  ULOG_RT_PRINTF_4(icbArbLog,
                   "Issue Pair Request (MID: %d, SID: %d) (request: Ib: 0x%08x Ab: 0x%08x)",
                   pMasterData->eExtMasterID,
                   pSlaveData->eExtSlaveID,
                   (uint32)pBWReq->uIb,
                   (uint32)pBWReq->uAb );

  /* Update this request */
  if( NPA_CLIENT_SUPPRESSIBLE_VECTOR == client->type )
  {
    if( !is_changed )
    {
      pMasterData->mstrState.uSuppAb -= pOldReq->uAb;
      pSlaveData->slvState.uSuppAb   -= pOldReq->uAb;
    }
    else
    {
      if ( NPA_CLIENT_SUPPRESSIBLE2_VECTOR == pOldReq->clientType )
      {
        pMasterData->mstrState.uActiveOnlyAb -= pOldReq->uAb;
        pSlaveData->slvState.uActiveOnlyAb   -= pOldReq->uAb;
      }
      else
      {
        pMasterData->mstrState.uAb -= pOldReq->uAb;
        pSlaveData->slvState.uAb   -= pOldReq->uAb;
      }
    }
    pMasterData->mstrState.uSuppAb += pBWReq->uAb;
    pSlaveData->slvState.uSuppAb   += pBWReq->uAb;
  }
  else if( NPA_CLIENT_SUPPRESSIBLE2_VECTOR == client->type )
  {
    if( !is_changed )
    {
      pMasterData->mstrState.uActiveOnlyAb -= pOldReq->uAb;
      pSlaveData->slvState.uActiveOnlyAb   -= pOldReq->uAb;
    }
    else
    {
      if ( NPA_CLIENT_SUPPRESSIBLE_VECTOR == pOldReq->clientType )
      {
        pMasterData->mstrState.uSuppAb -= pOldReq->uAb;
        pSlaveData->slvState.uSuppAb   -= pOldReq->uAb;
      }
      else
      {
        pMasterData->mstrState.uAb -= pOldReq->uAb;
        pSlaveData->slvState.uAb   -= pOldReq->uAb;
      }
    }
    pMasterData->mstrState.uActiveOnlyAb += pBWReq->uAb;
    pSlaveData->slvState.uActiveOnlyAb   += pBWReq->uAb;
  }
  else
  {
    if( !is_changed )
    {
      pMasterData->mstrState.uAb -= pOldReq->uAb;
      pSlaveData->slvState.uAb   -= pOldReq->uAb;
    }
    else
    {
      if ( NPA_CLIENT_SUPPRESSIBLE2_VECTOR == pOldReq->clientType )
      {
        pMasterData->mstrState.uActiveOnlyAb -= pOldReq->uAb;
        pSlaveData->slvState.uActiveOnlyAb   -= pOldReq->uAb;
      }
      else
      {
        pMasterData->mstrState.uSuppAb -= pOldReq->uAb;
        pSlaveData->slvState.uSuppAb   -= pOldReq->uAb;
      }
    }
    pMasterData->mstrState.uAb += pBWReq->uAb;
    pSlaveData->slvState.uAb   += pBWReq->uAb;
  }
}

/** ul_add_bw_request
 * 
 * @brief Adds a BW request to the passed request list
 * 
 * @author seansw (09/24/2012)
 * 
 * @param pBWReq 
 * @param pRequestList
 */
void ul_add_bw_request
(
  ul_bw_request_type      * pBWReq,
  ul_bw_request_list_type * pRequestList
)
{
  void *  oldList;

  if( NULL == pRequestList || NULL == pBWReq )
  {
    return;
  }

  /* Increase the size of the list if necessary */
  if( pRequestList->uEntryCount + 1 >= pRequestList->uListSize )
  {
    /* Save the old list */
    oldList = pRequestList->aRequestList;

    if(DAL_SUCCESS !=
       DALSYS_Malloc( (pRequestList->uListSize + UL_BW_REQUEST_LIST_SIZE) * sizeof(void*),
                      (void **)&pRequestList->aRequestList ))
    {
      pRequestList->aRequestList = oldList;
      return;
    }

    if( NULL != oldList )
    {
      /* Copy and free old buffer. */
      memscpy(
        (void*)pRequestList->aRequestList,
        (pRequestList->uListSize + UL_BW_REQUEST_LIST_SIZE)* sizeof(void*),
        oldList,
        pRequestList->uListSize * sizeof(void*));
      DALSYS_Free( oldList );
    }
    pRequestList->uListSize += UL_BW_REQUEST_LIST_SIZE;
  }

  pRequestList->aRequestList[pRequestList->uEntryCount++] = pBWReq;
}

/** ul_remove_bw_request
 * 
 * @brief Removes a BW request from the passed request list
 * 
 * @author seansw (09/24/2012)
 * 
 * @param pBWReq 
 * @param pRequestList
 */
void ul_remove_bw_request
(
  ul_bw_request_type      * pBWReq,
  ul_bw_request_list_type * pRequestList
)
{
  uint32 i;

  if( NULL == pRequestList || NULL == pBWReq )
  {
    return;
  }

  for( i = 0; i < pRequestList->uEntryCount; i++ )
  {
    if( pRequestList->aRequestList[i] == pBWReq )
    {
      /* We found the request to remove */
      break;
    }
  }

  if( i < pRequestList->uEntryCount )
  {
    memsmove(&pRequestList->aRequestList[i],
             (pRequestList->uEntryCount - i)*sizeof(void*),
             &pRequestList->aRequestList[i+1],
             (pRequestList->uEntryCount - i - 1)*sizeof(void*));
    pRequestList->uEntryCount--;
  }
}

/*==============================================================================

  FUNCTION   ul_int_init

  DESCRIPTION 
    Initialize and define the bus arbiter subnodes

  PARAMETERS
    None

  RETURN VALUE    
    None

==============================================================================*/
void ul_int_init( void )
{
  uint32 u32ul_clk_node_list_size;
  const char ** ul_clock_node_list;

  /* Retrieve from DAL properties. */
  ul_int.routeTreeRoot     = *( (ul_treeNodeType **) AxiCfgInt_GetProperty( "icb_arb_route_tree" ) );
  ul_int.uNumMasters       = *((uint32 * ) AxiCfgInt_GetProperty( "icb_arb_master_count" ) );
  ul_int.uNumSlaves        = *((uint32 * ) AxiCfgInt_GetProperty( "icb_arb_slave_count" ) );
  ul_int.aMasterList       = (ul_master_data_type **)AxiCfgInt_GetProperty( "icb_arb_master_list" );
  ul_int.aSlaveList        = (ul_slave_data_type **) AxiCfgInt_GetProperty( "icb_arb_slave_list" );
  u32ul_clk_node_list_size = *( (uint32 *)AxiCfgInt_GetProperty("icb_arb_clk_count") );
  ul_clock_node_list       = (const char ** )AxiCfgInt_GetProperty("icb_arb_clock_node_list");

  /* Set a callback once all clock nodes have been created */
  npa_resources_available_cb( u32ul_clk_node_list_size,
                              ul_clock_node_list,
                              ul_target_init_cb,
                              NULL );
}

/** @copydoc */
ul_route_list_type *ul_get_route
(
  ICBId_MasterType eMaster,
  ICBId_SlaveType  eSlave
)
{
  uint64   uHashIdx;

  /* Check for invalid masters and slaves */
  if( eMaster & ~0xFFFFFFFF || eSlave & ~0xFFFFFFFF )
  {
    /* This is an internal master or slave and cannot be used as an endpoint */
    return NULL;
  }

  /* Find the route in the tree */
  uHashIdx =  ((uint64)eMaster & 0xFFFFFFFF) << 32;
  uHashIdx |= ((uint64)eSlave & 0xFFFFFFFF);

  return (ul_route_list_type*)ul_TreeLookup( ul_int.routeTreeRoot, uHashIdx );
}

/* @copydoc ul_target_get_master_list */
void ul_target_get_master_list
(
  ul_int_master_list_type *pMasterList
)
{
  pMasterList->uNumMasters = ul_int.uNumMasters;
  pMasterList->aMasterList = ul_int.aMasterList;
}


/* @copydoc ul_target_get_slave_list */
void ul_target_get_slave_list
(
  ul_int_slave_list_type *pSlaveList
)
{
  pSlaveList->uNumSlaves = ul_int.uNumSlaves;
  pSlaveList->aSlaveList = ul_int.aSlaveList;
}

/** @copydoc ul_get_master_state */
ul_node_state_type *ul_get_master_state
(
  ICBId_MasterType eExtMasterID
)
{
  ul_node_state_type *state = NULL;
  uint32 i;

  for( i = 0; i < ul_int.uNumMasters; i++ )
  {
    if( eExtMasterID == ul_int.aMasterList[i]->eExtMasterID )
    {
      state = &ul_int.aMasterList[i]->mstrState;
      break;
    }
  }

  return state;
}

/** @copydoc ul_get_slave_state */
ul_node_state_type *ul_get_slave_state
(
  ICBId_SlaveType eExtSlaveID
)
{
  ul_node_state_type *state = NULL;
  uint32 i;

  for( i = 0; i < ul_int.uNumSlaves; i++ )
  {
    if( eExtSlaveID == ul_int.aSlaveList[i]->eExtSlaveID )
    {
      state = &ul_int.aSlaveList[i]->slvState;
      break;
    }
  }

  return state;
}
