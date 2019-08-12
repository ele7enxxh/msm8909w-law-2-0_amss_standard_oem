/*=============================================================================

FILE:      ul_arb.c

DESCRIPTION: This file implements the NPA ICB Arbiter Node

    Copyright (c) 2010-2014 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/ul_arb.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
  $Change: 11985146 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/17/15   sds     Add support for notification interface
 09/09/14   jc      Support for NPA active-only clients and RPM semi-active set.
 07/28/14   sds     Banned API removal.
 06/20/14   sds     Extend request types to add latency request in addition to BW.
 03/11/14   sds     Extend query function - slave port count.
 11/05/13   jc      Add support for client type change.
 07/07/13   jc      Added query function.
 02/22/13   jc      Fix SAL warning.
 11/15/12   sds     Now that we handle NAS, readd RPM dependency.
                    Remove all rpm msg id handling.
                    Fix Klockwork error.
 11/09/12   sds     Temporarily remove RPM dependency, until we handle NAS better.
 10/23/12   sds     Add RPM dependency so NPA can see it, and clean up a Klocwork error.
 10/03/12   sds     Clock cleanup and master clock aggregation added.
 08/21/12   sds     remove support for npa fork/join and add support for npa's
                    fire and forget
 04/25/12   dj      add dal config support
 04/11/12   dj      double word align 64 bit heap data
 03/30/12   dj      Move transaction initilization before arbiter node initilization
 08/24/11   sds     Move helper function implementations out of the header.
 07/27/11   rng     Re-architected interface implementation to support
                    native NPA client creation functions.
 06/24/11   av      Added NULL check for client handle
 04/29/11   rng     Added support for requested bandwidth tier
 04/27/11   rng     Increased Bandwidth varuAbles to uint64
 02/11/11   rng     Create ULog for internal messages
 11/19/10   pm      Fixed round-up divisions in BW calculations
 11/16/10   pm      Fixed overflow and underflow in BW calculations
 10/04/10   pm      Added oversubscription event support
 09/01/10   pm      Added ul_commit_request() in _ul_destroy_client()
 08/24/10   pm      Changed function, type, and enum names for consistency
 08/20/10   pm      Changed filenames to ul.c and .h
 08/04/10   rng     Added oversubscription callback
 07/15/10   pm      Initial version

=============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "icbarb.h"
#include "ul_i.h"
#include "npa_resource.h"
#include "npa_transaction.h"
#include "ULogFront.h"
#include "rpmclient.h"
#include "ddiaxicfgint.h"
#include "stringl.h"

/*============================================================================
                          DEFINES
============================================================================*/
#define MICROSECONDS_PER_SECOND 1000000
#define PERCENTS_IN_A_WHOLE     100
#define MAX_VECTOR_LEN          0xFFFFFFFF
#define MAX_NAME_LEN            256

/*============================================================================
                          MACROS 
============================================================================*/
#define ALIGN8( x )     ( ((x)+7) & ~7 )

/*============================================================================
                          LOCAL TYPES
============================================================================*/

typedef struct
{
   uint32                      u32Len;
   ul_route_type             * apRoute;
   ul_bw_request_type * apRequests;
} ICBArb_MasterRouteVectorType;

/*============================================================================
                          LOG DATA
============================================================================*/
/* ICB Arbiter Internal Log Handle */
ULogHandle icbArbLog;

/* Size of Arbiter Log - Defaults to 0 so that logging is disabled */
/* Change this to 8k or 16k to enable internal logging */
uint32 icbArbLogSize = 0;

/*============================================================================
                          STATIC DATA
============================================================================*/
/* To pass errors that occur in ul_issue_request() */
static ICBArb_ErrorType eIcbarbErrorState = ICBARB_ERROR_SUCCESS;  

/* Queue Head for the commit queue */
static ul_request_type   * commit_queue = NULL;

/**
 * NPA Transaction Handle
 */
static npa_transaction_handle ul_transaction_handle;

/* *******************************************************************
 * NPA Data Definitions
 * *******************************************************************/
static npa_resource *self = NULL;

static npa_resource_plugin ul_plugin;

static npa_resource_state _ul_driver_fcn( npa_resource *resource,
                                          npa_client   *client,
                                          npa_resource_state state );

static npa_node_dependency ul_node_deps[] =
{
   {UL_MASTERS_CREATED,  NPA_NO_CLIENT, NULL},  /* Marker for all masters */
   {AXICFG_INIT_DONE,    NPA_NO_CLIENT, NULL},  /* Marker for axicfg */
   {"/protocol/rpm/rpm", NPA_NO_CLIENT, NULL},  /* NPA RPM resource, to let npa know
                                                   we use the RPM directly. */
};

static npa_resource_definition ul_node_resources[] = 
{
   { /* RESOURCE_INDEX_BUS_ARBITER */
     "/icb/arbiter", 
     "Arbitration Request",
     MAX_VECTOR_LEN,  /* Max "value" of state vector -- max len for now */
     &ul_plugin,
     ( NPA_RESOURCE_DRIVER_UNCONDITIONAL | NPA_RESOURCE_ALLOW_CLIENT_TYPE_CHANGE ),
     0,
     NULL,
     NULL,
   },
};

static npa_node_definition ul_node_defn = 
{ 
  "/icb/arbiter", 
  _ul_driver_fcn, 
  NPA_NODE_DEFAULT,
  NULL,
  NPA_ARRAY( ul_node_deps ),
  NPA_ARRAY( ul_node_resources )
};

/*============================================================================
               STATIC FUNCTION DECLARATIONS
============================================================================*/
static DALBOOL _convert_request_to_internal( ICBArb_RequestType *pRequest, 
                                             ul_bw_request_type *pInternal );

static ICBArb_ErrorType _query_resources
(
   npa_client_handle client,
   uint32 *puNumResource,
   const char **paResourceNames
);

static ICBArb_ErrorType _query_slave
(
  ICBArb_QueryTypeType   queryType,
  ICBArb_QueryUnionType *pQueryData
);

/*============================================================================
                          STATIC FUNCTIONS
============================================================================*/

/**=============================================================================

  _ul_init_cb

  @brief Performs post ICB arbiter node creation initialization

  @param None

  @return None

==============================================================================*/
static void _ul_init_cb( void         *context,
                         unsigned int  event_type,
                         void         *data,
                         unsigned int  data_size )
{
   /* Unused parameters */
   (void)context;
   (void)event_type;
   (void)data;
   (void)data_size;

   /* Tell AxiCfg to init the hardware. */
   AxiCfgInt_Reset();

   /* Save our handle to ourselves */
   self = ul_node_resources[0].handle;
}

/** _ul_request_commit
 * 
 *  Commits all requests in the request queue
 * 
 * @author ngibson (12/8/2011)
 * 
 * @param client_fork_pref 
 */
static void _ul_request_commit( npa_client_handle client )
{
  ul_request_type    * req  = commit_queue;
  bool                 wait_for_response;

  /* Open the transaction so we can add clock reqeusts */
  npa_begin_transaction(ul_transaction_handle);

  /* Loop through the entire request queue */
  while( NULL != req )
  {
    /* Dequeue the request */
    req->bQueued = FALSE;
    commit_queue = req->next;
    req->next = NULL;

    /* Issue the request (actually gets caught in the transaction queue) */
    if( NULL != req->req_func )
    {
      req->req_func( req );
    }

    /* Next item in list */
    req = commit_queue;
  }

  /* Now close the transaction to issue the requests */
  npa_end_transaction( ul_transaction_handle );

  /* Wait for the last RPM msg sent, if we're not issuing a
  ** fire and forget request. */
  wait_for_response = !(client->request_attr & NPA_REQUEST_FIRE_AND_FORGET);
  AxiCfgInt_Commit( wait_for_response );
}

/*==============================================================================

  FUNCTION   _ul_driver_fcn

  DESCRIPTION 
    Empty function -- all request actions handled by update function below.

  PARAMETERS
    resource - Resource information on this node
    client   - Client making the request
    state    - the state

  RETURN VALUE    
    The state that was passed into the function

==============================================================================*/
static npa_resource_state _ul_driver_fcn( npa_resource *resource,
                                          npa_client   *client,
                                          npa_resource_state state ) 
{
   /* Unused parameters */
   (void)client;
   (void)state;

   /* Currently the only state passed by update is NORMAL so just return
   ** the current state.
   */
   return resource->active_state;
}


/*==============================================================================

  FUNCTION   _ul_update_fcn

  DESCRIPTION 
    Passes along the client request to the driver function

  PARAMETERS
    resource - Resource information on this node
    client   - Client making the request

  RETURN VALUE    
    The requested flow

==============================================================================*/
static npa_resource_state _ul_update_fcn( npa_resource      *resource,
                                          npa_client_handle  client )
{
   uint32 i, j;
   ul_route_type             *pRoute;
   ul_bw_request_type *pRequest;
   DALBOOL bCompleteRequest = FALSE;

   ICBArb_MasterRouteVectorType *pMasterRouteVector = 
      (ICBArb_MasterRouteVectorType *)(client->resource_data);

   uint64 uRequest = (uint64)(NPA_PENDING_REQUEST( client ).state) / 
                       sizeof(ICBArb_RequestType);
   ICBArb_RequestType *aRequest  = 
      (ICBArb_RequestType *)(NPA_PENDING_REQUEST( client ).pointer.vector);

   ul_bw_request_type bw;
   bw.hClient = client;
   bw.clientType = client->type;

   /* Unused parameters */
   (void)resource;

   /* Check for 0 length vector, which means reset all bandwidths to zero */
   if ( 0 == uRequest )
   {
      bCompleteRequest = TRUE;
      uRequest = pMasterRouteVector->u32Len;
   }

   /* Loop through master clients and issue requests */
   for ( i = 0; i < uRequest; i++ )
   {
      if ( !bCompleteRequest )
      {
         /* Convert units to internal Ib & Ab */
         if ( !_convert_request_to_internal( &aRequest[i], &bw ) )
         {
            /* Invalid request type or attempted divide-by-zero due to
               zero time interval */

            /* We need to undo all the requests so far so there are no
             * remnants of the failed request */
            bCompleteRequest = TRUE;
            i = 0;
            eIcbarbErrorState = ICBARB_ERROR_INVALID_ARG;
            continue;
         }
      }
      else
      {
         /* Set BW to zero if CompleteRequest */
         bw.uIb        = 0;
         bw.uAb        = 0;
         bw.uLatencyNs = 0;
      }
      
      pRoute = &pMasterRouteVector->apRoute[i];
      pRequest = &pMasterRouteVector->apRequests[i];

      for( j = 0;
           j < pRoute->pRouteList->u32NumPairs;
           j++ )
      {
         ul_issue_pair_request(
            client,
           &bw,
            pRequest,
           &pRoute->pRouteList->aMasterSlavePairs[j],
            j==0);
      }

      /* Store the new request */
      pRequest->uIb        = bw.uIb;
      pRequest->uAb        = bw.uAb;
      pRequest->uLatencyNs = bw.uLatencyNs;
      pRequest->clientType = bw.clientType;  /* Retained to support client type change request */
   }

   /* "commit" all queued reqeusts */
   _ul_request_commit( client );

   /* Return the normal state.  It will be updated by callbacks if necessary */
   return ICBARB_STATE_NORMAL;   
}

/*==============================================================================

  FUNCTION   _ul_create_client_ex

  DESCRIPTION 
    Do client-specific initialization

  PARAMETERS
    client - The new client

  RETURN VALUE    
    None

==============================================================================*/
static unsigned int _ul_create_client_ex( npa_client *client, 
                                          unsigned int data, 
                                          void *reference )
{
   uint32 i, j;
   unsigned int uError;
   ICBArb_CreateClientVectorType *vector;
   uint32 u32NumMasterSlave;
   ICBArb_MasterSlaveType *aMasterSlave;
   ul_route_type *apRoute = NULL;
   ul_bw_request_type *apRequests = NULL;
   
   vector = (ICBArb_CreateClientVectorType *)reference;
   aMasterSlave = vector->aMasterSlave;
   u32NumMasterSlave = data;
   
   /* Allocate client vector of master client handles */
   if( DAL_SUCCESS != 
       DALSYS_Malloc( ALIGN8( sizeof(ICBArb_MasterRouteVectorType) ) + 
                      ALIGN8( u32NumMasterSlave * sizeof(ul_route_type) ) + 
                      ALIGN8( u32NumMasterSlave * sizeof(ul_bw_request_type) ),
                      (void **)&( client->resource_data ) ) )
   {
      client->resource_data = NULL;
      uError = ICBARB_ERROR_OUT_OF_MEMORY;
      goto ul_create_client_ex_error;
   }

   DALSYS_memset( client->resource_data,
                  0,
                  ALIGN8( sizeof(ICBArb_MasterRouteVectorType) ) +
                  ALIGN8( u32NumMasterSlave * sizeof(ul_route_type) ) +
                  ALIGN8( u32NumMasterSlave * sizeof(ul_bw_request_type) ) );

   apRoute = (ul_route_type *)((uint8*)client->resource_data + ALIGN8( sizeof(ICBArb_MasterRouteVectorType) ) );
   apRequests = (ul_bw_request_type *)( (uint8*)apRoute + ALIGN8( u32NumMasterSlave * sizeof(ul_route_type) ) );

   /* Store away vector length */
   ((ICBArb_MasterRouteVectorType *)
      (client->resource_data))->u32Len = u32NumMasterSlave; 
   /* Store away master client array */
   ((ICBArb_MasterRouteVectorType *)
      (client->resource_data))->apRoute = apRoute;
   ((ICBArb_MasterRouteVectorType *)
      (client->resource_data))->apRequests = apRequests;

   for ( i = 0; i < u32NumMasterSlave; i++  )
   {
      /* Get Routes */
      apRoute[i].eExtMasterID = aMasterSlave[i].eMaster;
      apRoute[i].eExtSlaveID  = aMasterSlave[i].eSlave;
      apRoute[i].pRouteList   =
         ul_get_route( aMasterSlave[i].eMaster,
                                   aMasterSlave[i].eSlave );

      if ( NULL == apRoute[i].pRouteList )
      {
         uError = (unsigned int)ICBARB_ERROR_NO_ROUTE_TO_SLAVE;
         goto ul_create_client_ex_error;
      }

      for( j = 0; j < apRoute[i].pRouteList->u32NumPairs; j++ )
      {
         apRequests[i].hClient = client;
         apRequests[i].clientType = client->type;
         /* Add the request to each node's queue */
         ul_add_bw_request(
           &apRequests[i],
           &apRoute[i].pRouteList->aMasterSlavePairs[j].pSlaveData->requestList );
         ul_add_bw_request(
           &apRequests[i],
           &apRoute[i].pRouteList->aMasterSlavePairs[j].pMasterData->requestList );
      }
   }

   return 0;  /* Success */

ul_create_client_ex_error:
   /* Need to remove requests from slaves */
   if ( client->resource_data )
   {
      for ( i = 0; i < u32NumMasterSlave; i++ )
      {
         if ( NULL != apRoute[i].pRouteList )
         {
            for ( j = 0; j < apRoute[i].pRouteList->u32NumPairs; j++ )
            {
               ul_remove_bw_request(
                  &apRequests[i],
                  &apRoute[i].pRouteList->aMasterSlavePairs[j].pSlaveData->requestList);
               ul_remove_bw_request(
                  &apRequests[i],
                  &apRoute[i].pRouteList->aMasterSlavePairs[j].pMasterData->requestList);
            }
         }
      }

      DALSYS_Free(client->resource_data);
      client->resource_data = NULL;
   }
   return uError;
}

/*==============================================================================

  FUNCTION   _ul_destroy_client

  DESCRIPTION 
    Do client-specific cleanup

  PARAMETERS
    client - The client to destroy

  RETURN VALUE    
    None

==============================================================================*/
static void _ul_destroy_client( npa_client *client )
{
   uint32 i,j;
   ICBArb_MasterRouteVectorType *pMasterRouteVector = 
      (ICBArb_MasterRouteVectorType *)(client->resource_data);

   if ( pMasterRouteVector )
   {
      /* Need to remove requests from slaves */
      if ( pMasterRouteVector->apRequests )
      {
          for ( i = 0; i < pMasterRouteVector->u32Len; i++ )
          {
            if ( NULL != pMasterRouteVector->apRoute[i].pRouteList )
            {
                for ( j = 0; j < pMasterRouteVector->apRoute[i].pRouteList->u32NumPairs; j++ )
                {
                  ul_remove_bw_request(
                    &pMasterRouteVector->apRequests[i],
                    &pMasterRouteVector->apRoute[i].pRouteList->aMasterSlavePairs[j].pSlaveData->requestList);
                  ul_remove_bw_request(
                    &pMasterRouteVector->apRequests[i],
                    &pMasterRouteVector->apRoute[i].pRouteList->aMasterSlavePairs[j].pMasterData->requestList);
                }
            }
          }
      }

      /* Free the memory from the client */
      DALSYS_Free( pMasterRouteVector );
   }
   
   client->resource_data = NULL;
   return;
}


static npa_resource_plugin ul_plugin = 
{
   _ul_update_fcn,
   NPA_CLIENT_VECTOR | NPA_CLIENT_SUPPRESSIBLE_VECTOR | NPA_CLIENT_SUPPRESSIBLE2_VECTOR,
   NULL,  /* using the ex version of create */
   _ul_destroy_client,
   _ul_create_client_ex,
   NULL
};

/*============================================================================
               NPA ICB Arbiter API Functions
============================================================================*/

/*==============================================================================

  FUNCTION   icbarb_init

  DESCRIPTION
    Initialize and define the ICB arbiter node.  This in turn calls the init
    functions of the subnodes and fabrics.

  PARAMETERS
    void

  RETURN VALUE    
    void

==============================================================================*/
void icbarb_init( void )
{
   unsigned int init[1] = {0,};
   uint32 u32ul_clk_node_list_size;
   const char ** ul_clock_node_list;

   AxiCfgInt_Init();
   
   /* Initialize the Arbiter Internal Log */
   ULogFront_RealTimeInit(&icbArbLog,
                          "ICB Arb Log",
                          icbArbLogSize,
                          ULOG_MEMORY_LOCAL,
                          ULOG_LOCK_NONE);

   /* Get our clock dependency information from DevCfg */
   u32ul_clk_node_list_size = *( (uint32 *)AxiCfgInt_GetProperty("icb_arb_clk_count") );
   ul_clock_node_list = (const char ** )AxiCfgInt_GetProperty("icb_arb_clock_node_list");


   /* Create the NPA transaction */
   ul_transaction_handle =
     npa_define_transaction( "/icb/arb/transaction",
                             NPA_TRANSACTION_LAZY_LOCKING,
                             u32ul_clk_node_list_size,
                             ul_clock_node_list );

   /* Explicitly call the subnode and fabric init functions */
   ul_int_init();

   /* Define the ICB Arbiter Node */
   npa_define_node_cb( &ul_node_defn, init, _ul_init_cb, NULL );
}

/*==============================================================================

  FUNCTION   icbarb_issue_request

  DESCRIPTION
     Allows a client to apply a vector of arbitration settings to the routes
     already established by a previous icbarb_create_client() call.
     The vector is a set of unions of arbitration settings of Types 1 and 2
     above (and more types in the future, as needed.)
 
     Note that the number of elements in the vector must match
     the number of master/slave pairs in the previous icbarb_create_client()
     call for that client.  If the number of elements in the
     icbarb_issue_request() call does not match, then the function
     returns an error (ICBARB_ERROR_VECTOR_LENGTH_MISMATCH.)

  PARAMETERS
     client - Client making the request
     aMasterArbCombined - Array of Arbitration info
     u32NumMasterArbCombined - number of elements in aMasterArbCombinedType
 
  RETURN VALUE
     The result of the request (e.g. success or an error code)
 
  SIDE EFFECTS
     If the result of the request is ICBARB_ERROR_REQUEST_REJECTED, then the
     request is set to zero (similar to the icbarb_complete_request()
     call below.)  This is because there is the possibility of the driver
     getting into an inconsistent state if this kind of cleanup is not done.

==============================================================================*/
ICBArb_ErrorType icbarb_issue_request( npa_client_handle client, 
                                       ICBArb_RequestType *aRequest,  
                                       uint32 u32NumRequest )
{
   ICBArb_ErrorType eError;

   ICBArb_MasterRouteVectorType *pMasterRouteVector;

   /* Check input parameters */
   if( NULL == client || NULL == aRequest )
   {
     return ICBARB_ERROR_INVALID_ARG;
   }

   pMasterRouteVector = (ICBArb_MasterRouteVectorType *)(client->resource_data);

   /* Check master client vector info. */
   if( NULL == pMasterRouteVector )
   {
     return ICBARB_ERROR_INVALID_ARG;
   }

   /* Check for vector length match */
   if ( u32NumRequest != pMasterRouteVector->u32Len )
   {
      return ICBARB_ERROR_VECTOR_LENGTH_MISMATCH;
   }

   npa_issue_vector_request( client, 
                             u32NumRequest * sizeof(ICBArb_RequestType),
                             (npa_resource_state *)aRequest );

   if ( ICBARB_ERROR_SUCCESS != eIcbarbErrorState )
   {
      eError = eIcbarbErrorState;
      eIcbarbErrorState = ICBARB_ERROR_SUCCESS;
      return eError;
   }

  return ICBARB_ERROR_SUCCESS;
}

/*==============================================================================

  FUNCTION   icbarb_complete_request

  DESCRIPTION
    The effect of this function is the same as a icbarb_issue_request() call
    where the vector elements are all 0.

  PARAMETERS
     client - Client making the request
 
  RETURN VALUE
     void

==============================================================================*/
void icbarb_complete_request( npa_client_handle client )
{
   npa_complete_request( client );

   return;
}


/*==============================================================================

  FUNCTION   icbarb_destroy_client

  DESCRIPTION
     Sets all the routes to 0 request, deletes the route information,
     and frees the associated memory.
 
     This will cancel the client request (if any) and free the client
     structure. The client handle can not be used to issue requests after
     being destroyed.

  PARAMETERS
     client - Client making the request
 
  RETURN VALUE
     void

==============================================================================*/
void icbarb_destroy_client( npa_client_handle client )
{
   /* Destroy the client */
   npa_destroy_client( client );

   return;
}

/*============================================================================
                             HELPER FUNCTIONS
============================================================================*/
ICBArb_CreateClientVectorType *icbarb_fill_client_vector
(
  ICBArb_CreateClientVectorType *psVector,
  ICBArb_MasterSlaveType        *aMasterSlave,
  npa_callback                   callback
)
{
  /* Fill in the vector fields with the parameters passed in */
  psVector->callback    = callback;
  psVector->aMasterSlave = aMasterSlave;

  return psVector;
}

/**=============================================================================

   @brief icbarb_create_client - Creates a new ICB arbiter client

   DEPRICATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param u32NumMasterSlave : number of elements in aMasterSlave
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_client
(
   const char             *pszClientName,
   ICBArb_MasterSlaveType *aMasterSlave, 
   uint32                  u32NumMasterSlave
)
{
   ICBArb_CreateClientVectorType vector;

   /* Add callback flag to vector to pass in */
   vector.callback     = NULL;
   vector.aMasterSlave = aMasterSlave;

   return npa_create_sync_client_ex( "/icb/arbiter", 
                                     pszClientName,
                                     NPA_CLIENT_VECTOR, 
                                     u32NumMasterSlave,
                                     (void *)&vector );
}

/**=============================================================================

   @brief icbarb_create_client_ex - Creates a new ICB arbiter client with a
                                     callback function for state change
                                     notification (oversubscription notification)

   DEPRICATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.  An additional argument allows registration
   for state change notification, which allows the client to be notified 
   when the fabrics are oversubscribed.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param u32NumMasterSlave : number of elements in aMasterSlave
   @param callback: Oversubscription notification callback function pointer
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_client_ex
(
   const char             *pszClientName,
   ICBArb_MasterSlaveType *aMasterSlave, 
   uint32                  u32NumMasterSlave, 
   npa_callback            callback
)
{
   ICBArb_CreateClientVectorType vector;

   /* Add callback flag to vector to pass in */
   vector.callback     = callback;
   vector.aMasterSlave = aMasterSlave;

   return npa_create_sync_client_ex( "/icb/arbiter", 
                                     pszClientName,
                                     NPA_CLIENT_VECTOR, 
                                     u32NumMasterSlave,
                                     (void *)&vector );
}


/**=============================================================================

   @brief icbarb_create_suppressible_client_ex -
                                     Creates a new ICB arbiter client with a
                                     callback function for state change
                                     notification (oversubscription notification)

   DEPRICATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.  An additional argument allows registration
   for state change notification, which allows the client to be notified 
   when the fabrics are oversubscribed.  All clock requests from this client
   will be made with suppressible requests so that the clock requests may be
   removed when the originating processor enters low power mode.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param u32NumMasterSlave : number of elements in aMasterSlave
   @param callback: Oversubscription notification callback function pointer
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_suppressible_client_ex
(
   const char             *pszClientName,
   ICBArb_MasterSlaveType *aMasterSlave, 
   uint32                  u32NumMasterSlave, 
   npa_callback            callback
)
{
   ICBArb_CreateClientVectorType vector;

   /* Add callback flag to vector to pass in */
   vector.callback     = callback;
   vector.aMasterSlave = aMasterSlave;

   return npa_create_sync_client_ex( "/icb/arbiter", 
                                     pszClientName,
                                     NPA_CLIENT_SUPPRESSIBLE_VECTOR, 
                                     u32NumMasterSlave,
                                     (void *)&vector );
}

/**
  @copydoc ul_request_enqueue()
*/
void ul_request_enqueue( ul_request_type * req )
{
  /* Only add requests once */
  if( req->bQueued )
  {
    return;
  }

  // Add new requests to the front
  req->next = commit_queue;
  commit_queue = req;
  req->bQueued = TRUE;
}

/**=============================================================================

   @brief icbarb_query - Queries ICB parameters
   
   Provides access to ICB internal data.
   
   @param query : Pointer to query specification
   
   @return The result of the query (e.g. success or an error code)

==============================================================================*/
ICBArb_ErrorType icbarb_query( ICBArb_QueryType *pQuery )
{
   ICBArb_ErrorType eError;

   /* Check input parameters */
   if ( NULL == pQuery )
   {
     return ICBARB_ERROR_INVALID_ARG;
   }

   switch ( pQuery->queryType )
   {
      case ICBARB_QUERY_RESOURCE_COUNT:
         if ( NULL == pQuery->queryData.resourceCount.client )
         {
            return ICBARB_ERROR_INVALID_ARG;
         }
         eError = _query_resources(
            pQuery->queryData.resourceCount.client,
            &pQuery->queryData.resourceCount.uNumResource,
            NULL );
         break;

      case ICBARB_QUERY_RESOURCE_NAMES:
         if ( NULL == pQuery->queryData.resourceNames.client ||
              NULL == pQuery->queryData.resourceNames.aResourceNames )
         {
            return ICBARB_ERROR_INVALID_ARG;
         }
         eError = _query_resources(
            pQuery->queryData.resourceNames.client,
            &pQuery->queryData.resourceNames.uNumResource,
            pQuery->queryData.resourceNames.aResourceNames );
         break;
      case ICBARB_QUERY_SLAVE_PORT_COUNT:
         eError = _query_slave(
            pQuery->queryType,
            &pQuery->queryData );
         break;

      default:
         eError = ICBARB_ERROR_INVALID_ARG;
         break;
   }

   return eError;
}

/*============================================================================
                          STATIC HELPER FUNCTIONS
============================================================================*/

/*============================================================================

  FUNCTION   _convert_request_to_internal

  DESCRIPTION
     Converts the units of various types of input requests to the internal
     Ib & Ab representation.

  PARAMETERS
     pRequest  (in)  - Pointer to the input external request struct
     PInternal (out) - Pointer to the output internal request struct.  The
                       memory for this structure must be allocated by the
                       calling function.
 
  RETURN VALUE
     TRUE if successful
     FALSE if there is an unsupported request type, or if any time parameter
           is zero with nonzero data burst.
============================================================================*/
static
DALBOOL _convert_request_to_internal( ICBArb_RequestType *pRequest, 
                                      ul_bw_request_type *pInternal )
{
   pInternal->uLatencyNs = 0;
   switch ( pRequest->arbType )
   {
   case ICBARB_REQUEST_TYPE_1_LAT:
      pInternal->uLatencyNs = pRequest->arbData.type1.uLatencyNs;
      /* Intentional fall through */
   case ICBARB_REQUEST_TYPE_1:
   case ICBARB_REQUEST_TYPE_1_TIER_3:
      if ( 0 == pRequest->arbData.type1.uDataBurst )
      {
         /* Protect against divide-by-zero */
         pInternal->uIb = 0;
         pInternal->uAb = 0;
      }
      else if ( ( 0 == pRequest->arbData.type1.uTransferTimeUs ) || 
                ( 0 == pRequest->arbData.type1.uPeriodUs ) )
      {
         /* Protect against divide-by-zero */
         return FALSE;
      }
      else
      {
         /* All operands are nonzero.  Round-up divisions to protect against
          * underflow.
          * Formulae:  Ib = (db + tt  - 1) / tt * 1000000
          *            Ab = (db + per - 1) / per * 1000000
          */
         pInternal->uIb = ( pRequest->arbData.type1.uDataBurst + 
                              pRequest->arbData.type1.uTransferTimeUs - 1 ) / 
                            pRequest->arbData.type1.uTransferTimeUs *
                            MICROSECONDS_PER_SECOND;
         pInternal->uAb = ( pRequest->arbData.type1.uDataBurst + 
                              pRequest->arbData.type1.uPeriodUs - 1 ) / 
                            pRequest->arbData.type1.uPeriodUs *
                            MICROSECONDS_PER_SECOND;
      }
      break;
   case ICBARB_REQUEST_TYPE_2_LAT:
      pInternal->uLatencyNs = pRequest->arbData.type2.uLatencyNs;
      /* Intentional fall through */
   case ICBARB_REQUEST_TYPE_2:
   case ICBARB_REQUEST_TYPE_2_TIER_3:
      /* Round-up division to protect against underflow 
       * Formula:  Ab = ( throughput + 100 - 1 ) / 100 * percent_usage
       */
      pInternal->uIb = pRequest->arbData.type2.uThroughPut;
      pInternal->uAb = ( pRequest->arbData.type2.uThroughPut + 
                           PERCENTS_IN_A_WHOLE - 1 ) / 
                         PERCENTS_IN_A_WHOLE  *
                         pRequest->arbData.type2.uUsagePercentage;
      break;
   case ICBARB_REQUEST_TYPE_3_LAT:
      pInternal->uLatencyNs = pRequest->arbData.type3.uLatencyNs;
      /* Intentional fall through */
   case ICBARB_REQUEST_TYPE_3:
   case ICBARB_REQUEST_TYPE_3_TIER_3:
      pInternal->uIb = pRequest->arbData.type3.uIb;
      pInternal->uAb = pRequest->arbData.type3.uAb; 
      break;
   default:
      return FALSE;
   }

   return TRUE;  // Success
}

/**=============================================================================

   @brief _query_resources - Queries ICB resource parameters
   
   This function determines the number of resources in the route as well
   as compiling a list of resource names.  Since the operations involved
   in both are nearly identical, a single function is implemented.
   
   @param client : Client handle
   @param puNumResource : Pointer to number of resources (input/output)
   @param paResourceNames : Pointer to array of string pointers
                            Will be NULL for the RESOURCE_COUNT case.
   
   @return The result of the query (e.g. success or an error code)

==============================================================================*/
static ICBArb_ErrorType _query_resources
(
   npa_client_handle client,
   uint32 *puNumResource,
   const char **paResourceNames
)
{
   ICBArb_ErrorType eError;
   const char **aClkNameArray;
   uint32 uNumClocks;
   ICBArb_MasterRouteVectorType *pMasterRouteVector;
   ul_route_type *apRoute;
   ul_route_list_type *pRouteList;
   ul_pair_type *aMasterSlavePairs;
   ul_master_data_type *pMasterData;
   ul_slave_data_type *pSlaveData;
   const char *pClk;
   uint32 uRouteIdx;
   uint32 uClkIdx;
   uint32 i, j;

   /* Check input parameters */
   if ( NULL == client )
   {
     return ICBARB_ERROR_INVALID_ARG;
   }

   pMasterRouteVector = (ICBArb_MasterRouteVectorType *)(client->resource_data);

   /* Perform a first-pass to determine max number of resources */
   uNumClocks = 0;
   for ( uRouteIdx = 0,  apRoute = pMasterRouteVector->apRoute;
         uRouteIdx < pMasterRouteVector->u32Len;
         uRouteIdx++, apRoute++ )
   {
      pRouteList = apRoute->pRouteList;
      aMasterSlavePairs = pRouteList->aMasterSlavePairs;
      for ( i = 0; i < pRouteList->u32NumPairs; i++, aMasterSlavePairs++ )
      {
         pMasterData = aMasterSlavePairs->pMasterData;
         pSlaveData = aMasterSlavePairs->pSlaveData;
         uNumClocks += pMasterData->u32MstrClockCount;
         uNumClocks += pSlaveData->u32SlvClockCount;
      }
   }

   /* Allocate memory for max number of resources */
   if ( DAL_SUCCESS != DALSYS_Malloc( uNumClocks * sizeof(*aClkNameArray),
                                     (void *)&aClkNameArray ) )
   {
      return ICBARB_ERROR_OUT_OF_MEMORY;
   }

   /* Store only the unique resource names in the array, and adjust count */
   uNumClocks = 0;
   for ( uRouteIdx = 0,  apRoute = pMasterRouteVector->apRoute;
         uRouteIdx < pMasterRouteVector->u32Len;
         uRouteIdx++, apRoute++ )
   {
      pRouteList = apRoute->pRouteList;
      aMasterSlavePairs = pRouteList->aMasterSlavePairs;
      for ( i = 0; i < pRouteList->u32NumPairs; i++, aMasterSlavePairs++ )
      {
         pMasterData = aMasterSlavePairs->pMasterData;
         pSlaveData = aMasterSlavePairs->pSlaveData;
         for ( uClkIdx = 0; uClkIdx < pMasterData->u32MstrClockCount; uClkIdx++ )
         {
            pClk = pMasterData->aMstrClocks[uClkIdx].name;
            for ( j = 0; j < uNumClocks; j++ )
            {
               if ( strncmp( pClk, aClkNameArray[j], MAX_NAME_LEN ) == 0 )
               {
                  break;
               }
            }
            if ( j >= uNumClocks )
            {
               aClkNameArray[uNumClocks++] = pClk;
            }
         }
         for ( uClkIdx = 0; uClkIdx < pSlaveData->u32SlvClockCount; uClkIdx++ )
         {
            pClk = pSlaveData->aSlvClocks[uClkIdx].name;
            for ( j = 0; j < uNumClocks; j++ )
            {
               if ( strncmp( pClk, aClkNameArray[j], MAX_NAME_LEN ) == 0 )
               {
                  break;
               }
            }
            if ( j >= uNumClocks )
            {
               aClkNameArray[uNumClocks++] = pClk;
            }
         }
      }
   }

   /* Store results */
   eError = ICBARB_ERROR_SUCCESS;
   if (paResourceNames == NULL)
   {
      /* ICBARB_QUERY_RESOURCE_COUNT */
      *puNumResource = uNumClocks;
   }
   else
   {
      /* ICBARB_QUERY_RESOURCE_NAMES */
      /* Only store the list of resource names in the caller's array
       * if their array is exactly the required size.
       */
      if ( *puNumResource == uNumClocks )
      {
         memscpy( (void *)paResourceNames,
                  uNumClocks * sizeof(const char *),
                  (void *)aClkNameArray,
                  uNumClocks * sizeof(const char *) );
      }
      else
      {
         eError = ICBARB_ERROR_INVALID_ARG;
      }
   }
   DALSYS_Free( (void *)aClkNameArray );

   return eError;
}

/**=============================================================================

   @brief _query_slave - Queries ICB slave resource parameters
   
   This function queries various parameters from the target slave.

   @param queryType : The type of query
   @param queryData : Pointer to query data storage
   
   @return The result of the query (e.g. success or an error code)

==============================================================================*/
static ICBArb_ErrorType _query_slave
(
  ICBArb_QueryTypeType   queryType,
  ICBArb_QueryUnionType *pQueryData
)
{
  ul_int_slave_list_type slaveList;
  uint32 i;
  ICBArb_ErrorType eError = ICBARB_ERROR_UNKNOWN;

  switch( queryType )
  {
    case ICBARB_QUERY_SLAVE_PORT_COUNT:
      ul_target_get_slave_list( &slaveList );

      /* Find the slave we care about. */
      for( i = 0; i < slaveList.uNumSlaves; i++ )
      {
        if( slaveList.aSlaveList[i]->pSlaveDef->slaveId ==
            pQueryData->slavePortCount.eSlaveId )
        {
          break;
        }
      }

      /* If we've found it, update query data. */
      if( i != slaveList.uNumSlaves )
      {
        pQueryData->slavePortCount.uNumPorts =
          slaveList.aSlaveList[i]->pSlaveDef->uNumPorts;
        eError = ICBARB_ERROR_SUCCESS;
      }
      break;
    default:
      eError = ICBARB_ERROR_INVALID_ARG;
      break;
  }

  return eError;
}

/*============================================================================
                          NOTIFICATION INTERFACE
============================================================================*/
/** ul_insert_notification
 *
 * @brief Insert a notification into the given list.
 */
void ul_insert_notification
(
   icb_notify_list_type *list,
   icb_notify_type *     elem
)
{
   /* Check input parameters. */
   if( NULL == list || NULL == elem)
   {
      return;
   }

   /* Insert at the head of the list. */
   elem->next = list->head;
   list->head = elem;
   list->count += 1;
}

/** ul_remove_notification
 *
 * @brief Remove a notification from the given list.
 */
void ul_remove_notification
(
   icb_notify_list_type *list,
   icb_notify_type *     elem
)
{
   uint32_t i;
   icb_notify_type *prev, *iter;

   /* Check input parameters. */
   if( NULL == list || NULL == elem)
   {
      return;
   }

   /* Search the list for the element we want.
    * Bound our search to the number of items we know are in the list. */
   prev = NULL;
   iter = list->head;
   for( i = 0; i < list->count && NULL != iter; i++ )
   {
      /* If we found the item, remove it. */
      if( iter == elem )
      {
         /* If we're replacing the head, update our list header. */
         if( NULL == prev )
         {
            list->head = iter->next;
         }
         /* Otherwise, update the previous item in the list. */
         else
         {
            prev->next = iter->next;
         }
         list->count -= 1;
         break;
      }
      /* Otherwise, advance down the list. */
      else
      {
         prev = iter;
         iter = iter->next;
      }
   }
}

/** ul_get_notification_list
 *
 * @brief Get the notification list for a given node.
 *
 * @pre   Caller must check that definition is non-NULL.
 */
icb_notify_list_type *ul_get_notification_list
(
   icb_notify_definition_type *definition
)
{
   icb_notify_list_type *notify_list = NULL;

   if( ICB_NOTIFY_MASTER_NODE == definition->node_type )
   {
      ul_int_master_list_type master_list;
      uint32_t                i;

      /* Fetch the master list and search for the requested node. */
      ul_target_get_master_list( &master_list );
      for( i = 0; i < master_list.uNumMasters; i++ )
      {
         /* If we find what we're looking for, allocate some storage and place it on the list. */
         if( (ICBId_MasterType)definition->id == master_list.aMasterList[i]->eExtMasterID )
         {
            notify_list = &master_list.aMasterList[i]->notifications;
         }
      }
   }
   else if( ICB_NOTIFY_SLAVE_NODE == definition->node_type )
   {
      ul_int_slave_list_type slave_list;
      uint32_t               i;

      ul_target_get_slave_list( &slave_list );
      for( i = 0; i < slave_list.uNumSlaves; i++ )
      {
         /* If we find what we're looking for, allocate some storage and place it on the list. */
         if( (ICBId_SlaveType)definition->id == slave_list.aSlaveList[i]->eExtSlaveID )
         {
            notify_list = &slave_list.aSlaveList[i]->notifications;
         }
      }
   }

   return notify_list;
}

/** ul_notify_update_status
 *
 * @brief Forces an update of the notification status.
 */
void ul_notify_update_status
(
   icb_notify_type *notify
)
{
   if( NULL != notify )
   {
      ul_node_state_type *state = NULL;

      /* Get the state block for the given node. */
      if( ICB_NOTIFY_MASTER_NODE == notify->definition.node_type )
      {
         state = ul_get_master_state( (ICBId_MasterType)notify->definition.id );
      }
      else if( ICB_NOTIFY_SLAVE_NODE == notify->definition.node_type )
      {
         state = ul_get_slave_state( (ICBId_SlaveType)notify->definition.id );
      }

      /* Assuming we found one, evaluate the notification conditions. */
      if( NULL != state )
      {
         ul_notify_evaluate( notify, state );
      }
   }
}

/**=============================================================================
   @copydoc icb_notify_register()
==============================================================================*/
icb_notify_handle_type icb_notify_register
(
   icb_notify_definition_type *definition,
   icb_notify_cb_type          callback,
   void                       *callback_data
)
{
   icb_notify_handle_type handle = NULL;
   icb_notify_list_type * notify_list;

   /* Check out input parameters.
    * Callback data can be NULL.
    * Make sure we're also initialized by looking at self.
    * Check the notification type as well. */
   if( NULL == self ||
       NULL == definition ||
       NULL == callback ||
       definition->threshold_type >= ICB_NOTIFY_NUM_THRESHOLD_TYPES )
   {
      return handle;
   }

   /* Try to find the notification list for the node asked for. */
   notify_list = ul_get_notification_list( definition );

   /* If we found something, malloc and add to the appropriate notification list. */
   if( NULL != notify_list )
   {
      icb_notify_type *notify;

      if( DAL_SUCCESS ==
            DALSYS_Malloc( sizeof(icb_notify_type),
                           (void **)&notify ) )
      {

         /* Copy the definition. */
         memset(notify, 0, sizeof(icb_notify_type));
         notify->definition = *definition;

         /* Store callback information. */
         notify->callback      = callback;
         notify->callback_data = callback_data;

         /* Lock the arbiter node and do the insertion. */
         npa_resource_lock(self);
         ul_insert_notification(notify_list, notify);
         notify->list = notify_list;

         /* Force an update of the notification status on this request. */
         ul_notify_update_status(notify);
         npa_resource_unlock(self);

         handle = (icb_notify_handle_type)notify;
      }
   }

   return handle; 
}

/**=============================================================================
   @copydoc icb_notify_update()
==============================================================================*/
bool icb_notify_update
(
   icb_notify_handle_type      handle,
   icb_notify_definition_type *definition
)
{
   bool ret = false;

   /* Check input parameters and make sure we're initialized. */
   if( NULL != self &&
       NULL != handle &&
       NULL != definition )
   {
      /* Try to find the notification list for the node asked for. */
      icb_notify_list_type *notify_list = ul_get_notification_list( definition );

      /* If it's valid, continue. */
      if( NULL != notify_list )
      {
         bool new = false;
         icb_notify_type *notify = (icb_notify_type *)handle;

         /* Lock the arbiter node before we change it's internal state. */
         npa_resource_lock( self );

         /* First, we need to check if the node to watch has changed. */
         if( definition->node_type != notify->definition.node_type ||
             definition->id        != notify->definition.id )
         {
            /* If they're different, remove the notification from the old node. */
            ul_remove_notification( notify->list, notify );
            new = true;
         }

         /* Copy the new definition. */
         notify->definition = *definition;

         /* Clear the status. */
         notify->status_valid = false;
         memset( &notify->status, 0, sizeof(notify->status) );

         /* If we removed it from a different node earlier, add to the new node. */
         if( true == new )
         {
            ul_insert_notification( notify_list, notify );
            notify->list = notify_list;
         }

         /* Force an update of the notification status on this request. */
         ul_notify_update_status(notify);

         /* Unlock the arbiter node. */
         npa_resource_unlock( self );

         ret = true;
      }
   }

   return ret;
}

/**=============================================================================
   @copydoc icb_notify_deregister()
==============================================================================*/
bool icb_notify_deregister
(
   icb_notify_handle_type handle
)
{
   bool ret = false;

   /* Verify input parameters and initialization status. */
   if( NULL != self &&
       NULL != handle )
   {
      icb_notify_list_type *notify_list;
      icb_notify_type *notify = (icb_notify_type *)handle;

      notify_list = ul_get_notification_list( &notify->definition );
      
      /* If we found the correct notification list, clean up. */
      if( NULL != notify_list )
      {
         /* Lock the arbiter node and do the removal. */
         npa_resource_lock( self );
         ul_remove_notification( notify_list, notify );
         npa_resource_unlock( self );

         DALSYS_Free( notify );
         ret = true;
      }
   }

   return ret;
}

/**=============================================================================
   @copydoc icb_notify_get_notification_status()
==============================================================================*/
bool icb_notify_get_notification_status
(
  icb_notify_handle_type  handle,
  icb_notify_status_type *status
)
{
   bool ret = false;
   icb_notify_type *notify = (icb_notify_type *)handle;

   /* Check input parameters and make sure we're initialized. */
   if( NULL != self &&
       NULL != notify &&
       NULL != status )
   {
      /* Lock around the access to the notification status.
       * The UL will be updating this during transactions. */
      npa_resource_lock(self);
      *status = notify->status;
      npa_resource_unlock(self);

      ret = true;
   }

   return ret;
}

/**=============================================================================
   @copydoc icb_notify_reset_notification_status()
==============================================================================*/
bool icb_notify_reset_notification_status
(
  icb_notify_handle_type  handle,
  icb_notify_status_type *status
)
{
   bool ret = false;
   icb_notify_type *notify = (icb_notify_type *)handle;

   /* Check input parameters and make sure we're initialized. */
   if( NULL != self &&
       NULL != notify &&
       NULL != status )
   {
      /* Lock around the access to the notification status.
       * The UL will be updating this during transactions. */
      npa_resource_lock( self );
      notify->status.high_threshold &= ~status->high_threshold;
      notify->status.low_threshold &= ~status->low_threshold;
      npa_resource_unlock( self );

      ret = true;
   }

   return ret;
}

/* vim: set ts=3 sts=3 sw=3 et :*/ 
