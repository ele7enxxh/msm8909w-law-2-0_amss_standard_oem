/**********************************************************************
 * mca_npa_qdsp.c
 *
 * This module implements the NPA Resource for MCA
 *
 *
 * Copyright (C) 2009-2015 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*========================================================================
  EDIT HISTORY FOR MODULE
  
  $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/mca_npa_qdsp.c#1 $
  $DateTime: 2016/12/13 07:59:23 $

  when       who     what, where, why
  --------   ----    ---------------------------------------------------
========================================================================*/
#include "BuildFeatures.h"
#ifndef DEFEATURE_DCVS_NPA_QDSP

#include "mca_internal.h"
#include "CoreString.h"
#include "CoreVerify.h"
#include "dcvs_npa.h"
#include "coremca.h"
#include "ULogFront.h"
#include "ClockDefs.h"
#include "icbarb.h"
#include "icbid.h"
#include "CoreHeap.h"
#include "DDIPlatformInfo.h"

#define FUDGE_MULTIPLICATION_FACTOR 1000
#define BW_MULTIPLICATION_FACTOR 1048576
//(mpps*cpp limit)*1000 and corresponding fudge factor levels
//TBD - can be read from nv items rather than hardcoding here
#define MPPSCPP_LIMIT 288000

//Fudge factor level1 (1/0.6)*1000
#define FUDGE_FACTOR_LEVEL1 1666

//fudge factor level2 (1/.85) * 1000
#define FUDGE_FACTOR_LEVEL2 1176

#ifdef MCA_ENABLE_DIAG_HOOKS
npa_client_handle mcaDiagClient;
unsigned mcaHookMode = 0;
unsigned mcaNextParamIsQ6 = 0;
unsigned mcaNextParamIsIBW = 0;
unsigned mcaNextParamIsABW = 0;
unsigned mcaNextParamIsLatency = 0;
unsigned mcaNextParamIsValue = 0;
unsigned mcaFixedQ6 = 0;
unsigned mcaFixedIBW = 0;
unsigned mcaFixedABW = 0;
unsigned mcaFixedLatency = 0;
unsigned mcaDiagLogInitialized = 0;
#endif

static mcaInternalDataType mcaData;

/*=======================================================================

                      STATIC FUNCTION DEFINITIONS 

========================================================================*/
static npa_resource_state core_mca_bus_driver_fcn( npa_resource * resource,
                                                   npa_client * client,
                                                   npa_resource_state state )
{

  if ( client->type == NPA_CLIENT_INITIALIZE )
  {
    /* We create the initial states for the bus arbiter */
    /* There are 2 master/slave pairs which are affected by MCA */
    ICBArb_MasterSlaveType msArray[] =
    {
      { ICBID_MASTER_MSS_PROC, ICBID_SLAVE_EBI1 }
    };

    /* Create a client for the bus arbiter */
    mcaData.icbArbClientHandle = icbarb_create_suppressible_client_ex( "/node/core/mca/bus",
                                                                       msArray,
                                                                       1, NULL );

    CORE_VERIFY_PTR( mcaData.icbArbClientHandle );

    /* Indicate we are using request type 3 */
    mcaData.vecReq.arbType = ICBARB_REQUEST_TYPE_3_LAT;

    return state;
  }

  /* Set up the vector of request states for the bus arbiter . */
  mcaData.vecReq.arbData.type3.uAb = (uint64)mcaData.mcaState.ab * BW_MULTIPLICATION_FACTOR;
  mcaData.vecReq.arbData.type3.uIb = (uint64)mcaData.mcaState.ib * BW_MULTIPLICATION_FACTOR;
  mcaData.vecReq.arbData.type3.uLatencyNs = mcaData.mcaState.latency;

#ifdef MCA_ENABLE_DIAG_HOOKS
  if ( mcaHookMode )
  {
    mcaData.vecReq.arbData.type3.uAb = (uint64)mcaFixedABW * BW_MULTIPLICATION_FACTOR;
    mcaData.vecReq.arbData.type3.uIb = (uint64)mcaFixedIBW * BW_MULTIPLICATION_FACTOR;
    mcaData.vecReq.arbData.type3.uLatencyNs = mcaFixedLatency;
  }
#endif

  //pass request attributes if any
  npa_pass_request_attributes( client, mcaData.icbArbClientHandle );

  // Issue request to /icb/arbiter
  icbarb_issue_request( mcaData.icbArbClientHandle,
                        &mcaData.vecReq,
                        1 );
  return state;

}

/* Variable list of dependencies of "node/core/mca/bus" node to other nodes */
static npa_node_dependency core_mca_bus_deps[] =
{
  { "/icb/arbiter", NPA_NO_CLIENT },
};

/*Definition of /core/mca/bus resource*/
static npa_resource_definition core_mca_bus_res[] =
{
  { "/core/mca/bus",
    "MB/s",
    576 * 4,                     // Max mips * 4
    &npa_identity_plugin,        // The resource plugin.
    NPA_RESOURCE_SINGLE_CLIENT |
    NPA_RESOURCE_DRIVER_UNCONDITIONAL,  // The resource attributes.
    NULL }                        // The user data field.
};

/*Definition of /core/mca/bus node*/
static npa_node_definition node_mca_bus =
{
  "node/core/mca/bus",                        /* name */
  core_mca_bus_driver_fcn,                    /* driver_fcn */
  NPA_NODE_DEFAULT,                       /* attributes */
  NULL,                                       /* data */
  NPA_ARRAY( core_mca_bus_deps ),
  NPA_ARRAY( core_mca_bus_res )
};



/* Array of /core/mca dependencies */
static npa_node_dependency targetNodeDepsMCA[] =
{
  /* Clock Resource */
  {
    "/clk/cpu", /* Name of resource */
    NPA_CLIENT_REQUIRED, /* Type of client to create */
    NULL /* Client Handle - created by NPA */
  },
  /* Bus resource */
  {
    "/core/mca/bus", /* Name of resource */
    NPA_CLIENT_INTERNAL, /* Type of client to create */
    NULL /* Client Handle - created by NPA */
  }
};

/*=======================================================================

    LOCAL FUNCTION DEFINITIONS -- CORE CPU RESOURCE RELATED FUNCTIONS

========================================================================*/
/**
  @brief core_mca_calculateFudgeFactor
  
  This function calculates the fudge factor depends on mpps and cpp multiplicatin value
  passed as input.

  @param mppscpp: The product of the active client's MPPS and CPP result.
  @return : returns fudge factor value.
*/
static unsigned int core_mca_calculateFudgeFactor( uint32 value )
{
  uint32 fudgeFactor = 1, i = 0;

  //check
  if ( 0 != value && NULL != mcaData.fudgeFactors.pData )
  {
    for ( i = 0; i < mcaData.fudgeFactors.nCnt; i++ )
    {
      if ( value <= mcaData.fudgeFactors.pData[i].value )
      {
        fudgeFactor = mcaData.fudgeFactors.pData[i].fudgeFactor;
        break;
      }
    }
  }
  return fudgeFactor;
}

/**
  @brief core_mca_getNumberOfActiveClients
  
  This function finds the number of clients with a non-zero 
  mpps/Q6 min clock vote 

  @param resource : the resource that holds the client list we 
                  are interested in
   
  @return : returns the number of clients that are contributing 
          non-zero votes to the final aggregation 
*/
static unsigned int core_mca_getNumberOfActiveClients( npa_resource * resource  )
{
  CORE_VERIFY_PTR( resource );
  uint32 numActiveClients = 0;
  npa_client * client = resource->clients;
  mcaInternalStateType * clientActiveRequest;

  while ( client )
  {
    if ( client->type == NPA_CLIENT_VECTOR
         || client->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR
         || client->type == NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
    {
      clientActiveRequest = (mcaInternalStateType *)client->resource_data;
      if ( clientActiveRequest->mcaRequest || clientActiveRequest->q6Clk  )
      {
        numActiveClients++;
      }
    }
    client = client->next;
  }
  return numActiveClients;
}


/**
  @brief core_mca_adjustQ6Clk
  
  For given freqeuncy, it Returns maximun frequency supported without changing corner value

  @param  value: Frequency value
  @return      : Maximun corner frequency supported without change in corner level
*/
static unsigned int core_mca_adjustQ6Clk( uint32 value )
{
  uint32 q6Clk = value, i = 0;

  if ( NULL != mcaData.pCornerFreq )
  {
    for ( i = 0; i < mcaData.nCorners; i++ )
    {
      if ( value <= mcaData.pCornerFreq[i] )
      {
        q6Clk = mcaData.pCornerFreq[i];
        break;
      }
    }
  }
  return q6Clk;

}

/**
  @brief mcaTargetPopulateCornerMaxFreqValues
  
  This function populates max frequency values supported for each corner value

  @param  : None
  @return : None
*/
static void mcaTargetPopulateCornerMaxFreqValues( void )
{

  npa_query_handle   clkCpuQueryNpaHandle;
  npa_query_type     queryResult;
  npa_query_status   status;
  uint32 i = 0;

  clkCpuQueryNpaHandle = npa_create_query_handle( "/clk/cpu" );
  CORE_VERIFY_PTR( clkCpuQueryNpaHandle );

  CORE_VERIFY( NPA_QUERY_SUCCESS ==
                  npa_query( clkCpuQueryNpaHandle,
                             CLOCK_NPA_QUERY_NUM_CORNERS,
                             &queryResult ) );
  mcaData.nCorners = queryResult.data.value;

  //allocate memory to store MAX corner freqeuncy values
  if ( 0 != mcaData.nCorners )
  {
    mcaData.pCornerFreq = Core_Malloc( mcaData.nCorners * sizeof(uint32) );
    CORE_VERIFY_PTR( mcaData.pCornerFreq );
    memset( mcaData.pCornerFreq, 0, mcaData.nCorners * sizeof(uint32) );
  }

  for ( i = 0; i < mcaData.nCorners; i++ )
  {
    status = npa_query( clkCpuQueryNpaHandle,
                        CLOCK_NPA_QUERY_CPU_MAX_KHZ_AT_CORNER + i,
                        &queryResult );

    //currenlty clk regime returns NPA_QUERY_UNSUPPORTED_QUERY_ID for CLOCK_VREG_MSS_CORNER_LOW_MINUS
    //corner level if it's not supported
    CORE_VERIFY( (NPA_QUERY_SUCCESS == status || NPA_QUERY_UNSUPPORTED_QUERY_ID == status) );
    if ( NPA_QUERY_SUCCESS == status )
    {
      mcaData.pCornerFreq[i] = queryResult.data.value;
    }

  }
  return;
}

/**
  @brief res_core_mca_update_fcn
  
  This function is invoked by the /core/mca resource when a 
  client request is made. It gets the aggregates across all 
  clients and updates the clk and bus values accordingly.
 
  @param resource: A dynamic system element that work requests can be made against.
  @param client: The handle to the clients registered to the resource.

  @return : None.

*/
static npa_resource_state core_mca_update_fcn( npa_resource * resource,
                                               npa_client_handle client )
{
  uint32 pendingMCARequest = 0;
  uint32 q6ClkTmp = 0;

  /* Obtain MCA internal state from the resource*/
  mcaInternalStateType * mcaState = &mcaData.mcaState;

  /*variables required to extract/aggregate data from the vector request*/
  unsigned * requestDataPtr, *valuePtr;
  unsigned numKeys, key, length, i;
  unsigned requestPriority = REQ_PRIORITY_DEFAULT;

  mcaInternalStateType * activeRequest;
  npa_client_handle aggClient;

  unsigned fudgeFactorSet = 0;

  /* Ensure that the client is not NULL */
  CORE_VERIFY( client );

  //store the number of keys in request
  numKeys = NPA_PENDING_REQUEST( client ).state;

  switch ( client->type )
  {
    case NPA_CLIENT_INTERNAL:
#ifdef MCA_ENABLE_DIAG_HOOKS
      if ( mcaHookMode == 1 )
      {
        /* Request the state into the clock resource */
        npa_issue_required_request( NPA_DEPENDENCY( resource, 0 ), mcaFixedQ6 );
        npa_issue_internal_request( NPA_DEPENDENCY( resource, 1 ) );
      }
#endif
      break;
    case NPA_CLIENT_VECTOR:
    case NPA_CLIENT_SUPPRESSIBLE_VECTOR:
    case NPA_CLIENT_SUPPRESSIBLE2_VECTOR:
      //start multipart message to log mca request
      ULogFront_RealTimeMultipartMsgBegin( mcaData.ulogHandle );
      ULogFront_RealTimePrintf( mcaData.ulogHandle, 0, "MCA REQUEST RECEIVED: %m %m %m %m %m %m %m %m %m %m" ); //client, [mpps, cpp, q6, ib, ab, lat, fudge, prio], result

      activeRequest = (mcaInternalStateType *)client->resource_data;

      if ( numKeys == 0 ) //drop all votes from client if we get here
      {
        //subtract client active request,
        mcaState->mcaRequest -= activeRequest->mcaRequest;
        mcaState->ab -= activeRequest->ab;

        //reaggregate for max/min keys
        aggClient = resource->clients;
        if ( mcaState->q6Clk == activeRequest->q6Clk )
        {
          mcaInternalStateType * clientRequest;
          activeRequest->q6Clk = 0; // drop this clients vote before reaggregating
          mcaState->q6Clk = 0; // clear current q6 state of MCA
          while ( aggClient )
          {
            if ( aggClient->type == NPA_CLIENT_VECTOR
                 || aggClient->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR
                 || aggClient->type == NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
            {
              clientRequest = (mcaInternalStateType *)aggClient->resource_data;
              mcaState->q6Clk  = MAX( mcaState->q6Clk, clientRequest->q6Clk );
            }
            aggClient = aggClient->next;
          }
        }

        aggClient = resource->clients;
        if ( mcaState->ib == activeRequest->ib )
        {
          mcaInternalStateType * clientRequest;
          activeRequest->ib = 0; // drop this clients vote before reaggregating
          mcaState->ib = 0; // clear current q6 state of MCA
          while ( aggClient )
          {
            if ( aggClient->type == NPA_CLIENT_VECTOR
                 || aggClient->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR
                 || aggClient->type == NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
            {
              clientRequest = (mcaInternalStateType *)aggClient->resource_data;
              mcaState->ib  = MAX( mcaState->ib, clientRequest->ib );
            }
            aggClient = aggClient->next;
          }
        }

        aggClient = resource->clients; 
        if ( mcaState->latency == activeRequest->latency )
        {
          mcaInternalStateType * clientRequest; 
          activeRequest->latency = 0;
          mcaState->latency = 0;
          while ( aggClient )
          {
            if ( aggClient != client && (aggClient->type == NPA_CLIENT_VECTOR
                                         || aggClient->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR
                                         || aggClient->type == NPA_CLIENT_SUPPRESSIBLE2_VECTOR) )
            {
              clientRequest = (mcaInternalStateType *)aggClient->resource_data;
              if ( (clientRequest->latency != 0) &&
                   ((mcaState->latency == 0) || clientRequest->latency < mcaState->latency) )
              {
                mcaState->latency = clientRequest->latency;
              }
            }
            aggClient = aggClient->next;
          }
        }
        //clear out old request data
        DALSYS_memset( client->resource_data, 0, sizeof(mcaInternalStateType) );
        ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(Client : 0x%08x DROPPED VOTE)", client );
      }
      else
      {
        ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(Client : 0x%08x)", client );
      }

      //set pointer to begining of users request struct/first key
      requestDataPtr = (unsigned *)NPA_PENDING_REQUEST( client ).pointer.vector;
      for ( i = 0; i < numKeys; i++ ) //extract/aggregate request data for each key in struct
      {
        key = *requestDataPtr;
        requestDataPtr++; //move pointer to length section of current key
        length = *requestDataPtr;
        requestDataPtr++; //move pointer to the data section of current key
        valuePtr = requestDataPtr;

        switch ( key )
        {
          case MPPS_REQ_KEY:
            CORE_VERIFY( length == sizeof(unsigned) );

            //save clients vote
            activeRequest->mpps = *valuePtr;
            requestDataPtr++; //move request pointer to next key

            ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(MPPS : %d)", activeRequest->mpps );
            break;
          case CPP_REQ_KEY:
            CORE_VERIFY( length == sizeof(unsigned) );

            //save clients vote
            activeRequest->cpp = *valuePtr;
            requestDataPtr++; //move request pointer to next key

            ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(CPP : %d)", *valuePtr );
            break;

          case AB_REQ_KEY:
            CORE_VERIFY( length == sizeof(unsigned) );

            //handle new vote
            mcaState->ab -= activeRequest->ab;
            mcaState->ab += *valuePtr;
            activeRequest->ab = *valuePtr;

            requestDataPtr++; //move request pointer to next key

            ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(AB : %d)", *valuePtr );
            break;
          case IB_REQ_KEY:
            CORE_VERIFY( length == sizeof(unsigned) );
            //max agg for IB requests, if the client was voting for max val,
            //we need to reaggregate across all clients
            if ( *valuePtr > mcaState->ib ) //incoming value equal to current state?
            {
              mcaState->ib = *valuePtr;
            }
            else if ( activeRequest->ib == mcaState->ib ) //old vote responsible for max?
            {
              mcaInternalStateType * clientRequest;
              //replace old vote with new vote, then reaggregate from scratch
              activeRequest->ib = *valuePtr;
              mcaState->ib = 0;
              aggClient = resource->clients;
              while ( aggClient )
              {
                if ( aggClient->type == NPA_CLIENT_VECTOR
                     || aggClient->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR
                     || aggClient->type == NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
                {
                  clientRequest = (mcaInternalStateType *)aggClient->resource_data;
                  mcaState->ib = MAX( mcaState->ib, clientRequest->ib );
                }
                aggClient = aggClient->next;
              }
            }

            //save clients vote
            activeRequest->ib = *valuePtr;
            requestDataPtr++; //move request data pointer to next key

            ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(IB : %d)", activeRequest->ib );
            break;
          case LAT_REQ_KEY:
            CORE_VERIFY( length == sizeof(unsigned) );
            //min aggregation for latency key
            if ( (*valuePtr != 0) &&
                 ((mcaState->latency == 0) || *valuePtr < mcaState->latency) )
            {
              mcaState->latency = *valuePtr;
            }
            else if ( activeRequest->latency == mcaState->latency ) //prev vote responsible for current min?
            {
              mcaInternalStateType * clientRequest;
              aggClient = resource->clients;
              mcaState->latency = *valuePtr;
              /* Scan the pending list for a new minimum */
              while ( aggClient )
              {
                if ( aggClient != client && client->type == aggClient->type )
                {
                  clientRequest = (mcaInternalStateType *)aggClient->resource_data;
                  if ( (clientRequest->latency != 0) &&
                       ((mcaState->latency == 0) || clientRequest->latency < mcaState->latency) )
                  {
                    mcaState->latency = clientRequest->latency;
                  }
                }
                aggClient = aggClient->next;
              }
            }
			
            //save clients vote
            activeRequest->latency = *valuePtr;
            requestDataPtr++; //move request data pointer to next key
            ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(Latency : %d)", activeRequest->latency ); 
            break;
          case Q6_CLK_REQ_KEY:
            CORE_VERIFY( length == sizeof(unsigned) );
            //max agg for q6 requests
            //if the client was voting for max val, we need to reaggregate across all clients
            if ( *valuePtr > mcaState->q6Clk ) //incoming value equal to current state?
            {
              mcaState->q6Clk = *valuePtr;
            }
            else if ( activeRequest->q6Clk == mcaState->q6Clk ) //old vote responsible for max?
            {
              mcaInternalStateType * clientRequest;
              //replace old vote with new vote, then reaggregate from scratch
              activeRequest->q6Clk = *valuePtr;
              mcaState->q6Clk = 0;
              aggClient = resource->clients;
              while ( aggClient )
              {
                if ( aggClient->type == NPA_CLIENT_VECTOR
                     || aggClient->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR
                     || aggClient->type == NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
                {
                  clientRequest = (mcaInternalStateType *)aggClient->resource_data;
                  mcaState->q6Clk = MAX( mcaState->q6Clk, clientRequest->q6Clk );
                }
                aggClient = aggClient->next;
              }
            }

            //save clients vote
            activeRequest->q6Clk = *valuePtr;
            requestDataPtr++; //move request data pointer to next key

            ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(Q6 : %d)", activeRequest->q6Clk );
            break;
          case FUDGE_FACTOR_REQ_KEY:
            CORE_VERIFY( length == sizeof(unsigned) );
            //no aggregation, just copy value and set flag
            activeRequest->fudgeFactor = *valuePtr;
            fudgeFactorSet = 1;
            requestDataPtr++;

            ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(Fudge Factor : %d)", activeRequest->fudgeFactor );
            break;
          case REQUEST_PRIORITY_KEY:
            CORE_VERIFY( length == sizeof(unsigned) );
            //no aggregation, just copy value to local variable, no planned implementation with this yet
            requestPriority = *valuePtr;
            requestDataPtr++;

            ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(Request Priority : %d)", requestPriority );
            break;

          default:
            requestDataPtr++;
            ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(INVALID KEY : 0x%x)", key );
            break;

        } // switch (key)
      } //for(i=0;i<numKeys;i++)

      //set clients fudge factor based on mpps*cpp value
      if ( !fudgeFactorSet )
      {
        activeRequest->fudgeFactor = core_mca_calculateFudgeFactor( activeRequest->mpps * activeRequest->cpp );
        ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(Fudge Factor : %d)",  activeRequest->fudgeFactor );
        numKeys++;
      }
      pendingMCARequest = ((activeRequest->mpps * activeRequest->cpp * activeRequest->fudgeFactor) / FUDGE_MULTIPLICATION_FACTOR);

      //final aggregation
      //Adjust mcaRequest according to client current request
      //all other values have been updated to current request, but mcaRequest still reflects prev request
      mcaState->mcaRequest -= (activeRequest->mcaRequest - pendingMCARequest);
      activeRequest->mcaRequest = pendingMCARequest; //save result

      ULOG_RT_PRINTF_1( mcaData.ulogHandle, "(Result : %d)", mcaState->mcaRequest );

      for ( i = 0; i < (MAX_KEYS - numKeys); i++ )
      {
        ULOG_RT_PRINTF_0( mcaData.ulogHandle, " " );
      }
      ULogFront_RealTimeMultipartMsgEnd( mcaData.ulogHandle );
      break;
    default:

      break;
  }

  q6ClkTmp = MAX( mcaState->mcaRequest, mcaState->q6Clk );
  
  if ( core_mca_getNumberOfActiveClients( resource ) > 1 )
  {
    q6ClkTmp = core_mca_adjustQ6Clk( q6ClkTmp );
  }
  else
  {
    //to make sure Q6 runs at-least SVS max frequency
    q6ClkTmp = MAX( core_mca_adjustQ6Clk( 1 ), q6ClkTmp );
  }

  ULOG_RT_PRINTF_6( mcaData.ulogHandle, "MCA Request Output Params "
                                        "(mcaRequest : %d) (q6minclk : %d) "
                                        "(ib : %d) (ab : %d) (latency : %d) "
                                        "(adjustedQ6Clk : %d)",
                    mcaState->mcaRequest, mcaState->q6Clk, mcaState->ib, 
                    mcaState->ab, mcaState->latency, q6ClkTmp );

  return q6ClkTmp;
}


/**
  @brief node_core_cpu_driver_fcn

  This is the driver function for MCA node. This function issues
  state requests to the clocks and bus. 

  @param resource: A dynamic system element that work requests can be made against.
  @param client: The handle to the clients registered to the resource.
  @param state: Work request type.

  @return : None.

*/
static npa_resource_state core_mca_driver_fcn( npa_resource * resource,
                                               npa_client * client,
                                               npa_resource_state state )
{
  static mcaInternalStateType premcaState;
  static npa_resource_state preQ6ClkState;
  boolean bIsIbAbVoteChanged = FALSE;

  if ( client->type == NPA_CLIENT_VECTOR
       || client->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR
       || client->type == NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
  {

    //pass request attributes if any
    npa_pass_request_attributes( client, NPA_DEPENDENCY( resource, 0 ) );

#ifdef MCA_ENABLE_DIAG_HOOKS
    if ( mcaHookMode == 1 )
    {
      state =  mcaFixedQ6;
    }
#endif

    if ( state <= preQ6ClkState )
    {
      npa_set_request_attribute( NPA_DEPENDENCY( resource, 0 ), NPA_REQUEST_FIRE_AND_FORGET );
    }
    /* Request the state into the clock resource */
    npa_issue_required_request( NPA_DEPENDENCY( resource, 0 ), state );
    preQ6ClkState = state;

    //Issue request core/mca/bus only if Ib, Ab, or latency vote changes
    if ( (mcaData.mcaState.ib != premcaState.ib) ||
         (mcaData.mcaState.ab != premcaState.ab) ||
         (mcaData.mcaState.latency != premcaState.latency) )
    {
      bIsIbAbVoteChanged = TRUE;
    }

#ifdef MCA_ENABLE_DIAG_HOOKS
    if ( mcaHookMode )
    {
      bIsIbAbVoteChanged = TRUE;
    }
#endif

    if ( TRUE == bIsIbAbVoteChanged )
    {
      //pass request attributes if any
      npa_pass_request_attributes( client, NPA_DEPENDENCY( resource, 1 ) );

      //Set FIRE_AND_FOREGET if requests are less than the current active ones
      if ( (mcaData.mcaState.ib <= premcaState.ib) &&
           (mcaData.mcaState.ab <= premcaState.ab) )
      {
        npa_set_request_attribute( NPA_DEPENDENCY( resource, 1 ), NPA_REQUEST_FIRE_AND_FORGET );
      }

      /* Request the state into the bus resource */
      npa_issue_internal_request( NPA_DEPENDENCY( resource, 1 ) );
      premcaState.ib = mcaData.mcaState.ib;
      premcaState.ab = mcaData.mcaState.ab;
      premcaState.latency = mcaData.mcaState.latency;
    }
  }
  return state;
}

/**
  @brief NodeCoreMcaCreation_callback 

  This function is the callback from the single resource node 
  created while initializing MCA.  
  The callback will be invoked after the node is fully
  constructed.  The callback can be NULL if creation notification is
  unneeded. If invoked, the callback arguments are (context, 0,
  node->name, sizeof(const char *)).
  
  @param  context: NULL
  @param  event_type: The type of the event.
  @param  data: The name of the node being created.
  @param  data_size: The length of the node name.

  @return : None.

*/
static void NodeCoreMcaCreation_callback(  void         * context,
                                          unsigned int event_type,
                                          void         * data,
                                          unsigned int data_size )
{
  /* MCA Test client for config of algo output over diag */
#ifdef MCA_ENABLE_DIAG_HOOKS
  mcaDiagClient = npa_create_sync_client( "/core/mca",
                                          "MCA_DIAG_Client",
                                          NPA_CLIENT_INTERNAL );
  CORE_VERIFY( mcaDiagClient );
#endif

  /*create ulog*/
  CORE_DAL_VERIFY( ULogFront_RealTimeInit( &mcaData.ulogHandle,
                                           "MCA Log",
                                           16384,  // Initial Buffer Size
                                           ULOG_MEMORY_LOCAL,
                                           ULOG_LOCK_OS ) );

  /* Write the header to allow the visualization tool to know which parser to apply  */
  ULogCore_HeaderSet( mcaData.ulogHandle, "Content-Type: text/tagged-log-1.0; title=MCA running history" );
  mcaTargetInitFudgeFactors( &mcaData.fudgeFactors );

  //initialize Corner levels to zero
  mcaData.nCorners = 0;

  if ( (DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_RUMI) ||
	 (DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_VIRTIO) )
  {
    return ;
  }
  else
  {
    mcaTargetPopulateCornerMaxFreqValues();
  }

}


/**
  @brief core_mca_destroy_client_fcn

  Releases previously allocated memory for storing clients 
  previous request 

  @param  : None.

  @return : None.

*/
void core_mca_destroy_client_fcn( npa_client * client )
{
  if ( client->resource_data != NULL )
  {
    DALSYS_Free( client->resource_data );
    client->resource_data = NULL;
  }
}


/**
  @brief core_mca_create_client_fcn

  This function handles the various client data input. 
  If the client is a vector client, we allocate some memory 
  to keep track of the votes the client makes to the MCA 
  
  @param  client : The NPA client for the "/core/cpu" resource

  @return : None.

*/
void core_mca_create_client_fcn( npa_client * client ) //sga spaces
{
  //only need to allocate memory here if the client is a vector client
  if ( client->type == NPA_CLIENT_VECTOR
       || client->type == NPA_CLIENT_SUPPRESSIBLE_VECTOR
       || client->type == NPA_CLIENT_SUPPRESSIBLE2_VECTOR )
  {
    CORE_DAL_VERIFY( DALSYS_Malloc( sizeof(mcaInternalStateType), (void **)&client->resource_data ) );
    DALSYS_memset( client->resource_data, 0, sizeof(mcaInternalStateType) );
  }
}

/* The plugin definition for /core/mca resource */
const npa_resource_plugin res_core_mca_plugin =
{
  core_mca_update_fcn,
  NPA_CLIENT_VECTOR |
  NPA_CLIENT_SUPPRESSIBLE_VECTOR |
  NPA_CLIENT_SUPPRESSIBLE2_VECTOR |
  NPA_CLIENT_RESERVED2 |
  NPA_CLIENT_LIMIT_MAX,               /* Supported client types */
  core_mca_create_client_fcn,         /* Create client function */
  core_mca_destroy_client_fcn,        /* Destroy client function */
  NULL                                /* Extended client create function */
};


/* /core/mca resource definition */
static npa_resource_definition core_mca_res[] =
{
  { "/core/mca",
    "KVPS",
    1,                          // Will be updated upon client initialization
    &res_core_mca_plugin,  // The resource plugin.
    NPA_RESOURCE_VECTOR_STATE,  // The resource attributes.
    NULL,                  // The user data field.
    NULL }
};


/* /core/mca node definition */
static npa_node_definition core_mca_node =
{
  "/node/core/mca",                       /* name */
  core_mca_driver_fcn,                    /* driver_fcn */
  NPA_NODE_DEFAULT,                   /* attributes */
  NULL,                                   /* data */
  NPA_ARRAY( targetNodeDepsMCA ),
  NPA_ARRAY( core_mca_res )
};


/*=======================================================================

   PUBLIC FUNCTION DEFINITIONS -- CORE MCA RESOURCE RELATED FUNCTIONS

========================================================================*/
void NodeCoreMcaCreate( void )
{
  /* Initial state of the resource */
  npa_resource_state initialState[] =
  {
    0
  };

  npa_define_node( &node_mca_bus, initialState, NULL );

  npa_define_node_cb( &core_mca_node, initialState, NodeCoreMcaCreation_callback, NULL );

}

#endif 

