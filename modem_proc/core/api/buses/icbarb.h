#ifndef ICBARB_H
#define ICBARB_H
/*=============================================================================

FILE:      icbarb.h

DESCRIPTION: This defines the NPA ICB Arbiter Node interface

        Copyright (c) 2010-2014 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/core.mpss/3.5.c12.3/api/buses/icbarb.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
  $Change: 11985146 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/20/14   sds     Extend request types to add latency request in addition to BW.
 03/11/14   sds     Extend query function - slave port count.
 07/07/13   jc      Added query function.
 06/18/12   dj      Rename struct members & vars starting with u32
 07/27/11   rng     Re-architected interface implementation to support
                    native NPA client creation functions.
 07/14/11   rng     Added support for suppressible requests
 04/27/11   rng     Increased Bandwidth variables to uint64
 10/04/10   pm      Added oversubscription event support
 08/24/10   pm      Changed function, type, and enum names for consistency
 08/20/10   pm      Changed filename to icbarb.h
 04/16/10   pm      Initial version

=============================================================================*/
#include "npa_resource.h"
#include "icbid.h"

/*============================================================================
                          DATA TYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/* 
 *  Return codes
 */
typedef enum
{
   ICBARB_ERROR_SUCCESS = 0,
   ICBARB_ERROR_UNSUPPORTED,
   ICBARB_ERROR_INVALID_ARG,
   ICBARB_ERROR_INVALID_MASTER,
   ICBARB_ERROR_NO_ROUTE_TO_SLAVE,
   ICBARB_ERROR_VECTOR_LENGTH_MISMATCH,
   ICBARB_ERROR_OUT_OF_MEMORY,
   ICBARB_ERROR_UNKNOWN,
   ICBARB_ERROR_REQUEST_REJECTED,

   /* Insert new errors above here */
   ICBARB_ERROR_MAX,

   ICBARB_ERROR_PLACEHOLDER = 0x7FFFFFFF  /* Force to 32 bits */
} ICBArb_ErrorType;

/* 
 *  States (for oversubscription notification)
 */
typedef enum
{
   ICBARB_STATE_NORMAL = 0,
   ICBARB_STATE_OVERSUBSCRIBED,

   /* Insert new states above here */
   ICBARB_STATE_MAX,

   ICBARB_STATE_PLACEHOLDER = 0x7FFFFFFF  /* Force to 32 bits */
} ICBArb_StateType;

/* 
 *  Pair of master and slave
 */
typedef struct
{
   ICBId_MasterType eMaster;
   ICBId_SlaveType  eSlave;  /* This can be a slaveway on AXI */
} ICBArb_MasterSlaveType;

typedef struct
{
   npa_callback             callback;
   ICBArb_MasterSlaveType *aMasterSlave;
} ICBArb_CreateClientVectorType;

/* 
 *  Tells which type of arbitration settings are being represented.
 */
typedef enum
{
   /* Basic request types. */
   ICBARB_REQUEST_TYPE_1,         /* "Bursty" master */
   ICBARB_REQUEST_TYPE_2,         /* CPU-style master */
   ICBARB_REQUEST_TYPE_3,         /* Ib/Ab pair */

   /* DEPRECATED Tier 3 (no Tokens) requests DEPRECATED */
   ICBARB_REQUEST_TYPE_1_TIER_3,  /* "Bursty" master */
   ICBARB_REQUEST_TYPE_2_TIER_3,  /* CPU-style master */
   ICBARB_REQUEST_TYPE_3_TIER_3,  /* Ib/Ab pair */

   /* Basic request types additional latency request. */
   ICBARB_REQUEST_TYPE_1_LAT,     /* "Bursty" master */
   ICBARB_REQUEST_TYPE_2_LAT,     /* CPU-style master */
   ICBARB_REQUEST_TYPE_3_LAT,     /* Ib/Ab pair */

   /* Insert new types above here */
   ICBARB_REQUEST_TYPE_MAX, 

   ICBARB_REQUEST_TYPE_PLACEHOLDER = 0x7FFFFFFF  /* Force to 32 bits */
} ICBArb_RequestTypeType;

/* 
 *  "Burst" Master Parameters
 */
typedef struct
{
   uint64 uDataBurst;        // Bytes
   uint32 uTransferTimeUs;   // microseconds
   uint32 uPeriodUs;         // microseconds
   uint32 uLatencyNs;        // Latency requirement in nanoseconds
                             // Ignored for basic request types
} ICBArb_Request1Type;

/* 
 *  CPU-style Master Parameters
 */
typedef struct
{
   uint64 uThroughPut;       // Bytes/second
   uint32 uUsagePercentage;  // Whole percentage (0-100)
   uint32 uLatencyNs;        // Latency requirement in nanoseconds
                             // Ignored for basic request types
} ICBArb_Request2Type;

/* 
 *  Ib/Ab Parameters
 */
typedef struct
{
   uint64 uIb;        // Instantaneous bandwidth in Bytes/second
   uint64 uAb;        // Arbitrated bandwidth in Bytes/second
   uint32 uLatencyNs; // Latency requirement in nanoseconds
                      // Ignored for basic request types
} ICBArb_Request3Type;

/* 
 *  Union of all Arb Types
 */
typedef union
{
   ICBArb_Request1Type type1;
   ICBArb_Request2Type type2;
   ICBArb_Request3Type type3;
} ICBArb_RequestUnionType;

/* 
 *  Structure that contains one master/slave combo worth of Arb data
 */
typedef struct
{
   ICBArb_RequestTypeType  arbType;  /* Which type of arb data is it? (enum) */
   ICBArb_RequestUnionType arbData;  /* The actual data (union) */
} ICBArb_RequestType;

/* 
 *  Query codes
 */
typedef enum
{
   /* Get count of resources for the specified route */
   ICBARB_QUERY_RESOURCE_COUNT = 0,

   /* Get name strings of the resources for the specified route */
   ICBARB_QUERY_RESOURCE_NAMES,

   /* Get the number of ports for the specified slave */
   ICBARB_QUERY_SLAVE_PORT_COUNT,

   /* Insert new queries above here */
   ICBARB_QUERY_MAX,

   ICBARB_QUERY_PLACEHOLDER = 0x7FFFFFFF  /* Force to 32 bits */
} ICBArb_QueryTypeType;

/* 
 *  Resource count query data for ICBARB_QUERY_RESOURCE_COUNT
 */
typedef struct
{
   npa_client_handle client;    /* Client making the request */
   uint32 uNumResource;         /* Resource count (output) */
} ICBArb_QueryResourceCountType;

/* 
 *  Resource names query data for ICBARB_QUERY_RESOURCE_NAMES
 *
 *  Note: uNumResource value must match that returned by a
 *        ICBARB_QUERY_RESOURCE_COUNT query for the master/slave combo.
 */
typedef struct
{
   npa_client_handle client;    /* Client making the request */
   const char **aResourceNames; /* Pointer to resource names array (output) */
   uint32 uNumResource;         /* Number of elements in names array (input) */
} ICBArb_QueryResourceNamesType;

/*
 * Slave port count query data for ICBARB_QUERY_SLAVE_PORT_COUNT
 */
typedef struct
{
   ICBId_SlaveType eSlaveId; /* The slave to query */
   uint32 uNumPorts;         /* Number of ports for the slave */
} ICBArb_QuerySlavePortCountType;

/* 
 *  Union of all request types
 */
typedef union
{
   ICBArb_QueryResourceCountType  resourceCount;
   ICBArb_QueryResourceNamesType  resourceNames;
   ICBArb_QuerySlavePortCountType slavePortCount;
} ICBArb_QueryUnionType;

/* 
 *  Query specification structure
 */
typedef struct
{
   ICBArb_QueryTypeType  queryType;  /* Which type of query is it? (enum) */
   ICBArb_QueryUnionType queryData;  /* The actual data (union) */
} ICBArb_QueryType;

/*============================================================================
                          FUNCTIONS
============================================================================*/

/**=============================================================================

   @brief icbarb_init - Initialize and define the ICB arbiter node

==============================================================================*/
void icbarb_init( void );

/**=============================================================================

   @brief ICBARB_CREATE_CLIENT_VECTOR

   This macro is used to fill in the client_value and client_ref fields of any
   npa_create_*_client_ex() function.

   To correctly use this you must allocate a vector to be passed into the npa call:

   ICBArb_CreateClientVectorType  <your_vector>;

   It is acceptable to allocate this vector on the stack as it will not be needed
   to persist once the call to npa_create_*_client_ex() has returned.

   Then you can call the npa client creation function.  All vector based client
   types are supported.  Below is an example using npa_create_sync_client_ex():

   npa_create_sync_client_ex( "/icb/arbiter",
                              "/client/MY_CLIENT",
                              NPA_CLIENT_VECTOR,
                              ICBARB_CREATE_CLIENT_VECTOR(
                                &<your_vector>,
                                <MasterSlaveArray>,
                                NumMasterSlavePairs,
                                <your_npa_callback_function>) );

   Note that the npa_callback function at the end can be NULL if you do not wish
   to receive oversubscription notifications.

   @param psVector           : Pointer to a ICBArb_CreateClientVectorType structure
   @param aMasterSlave       : An ICBArb_MasterSlaveType array
   @param uNumMasterSlaves   : Number of master/slave pairs in the aMasterSlave array
   @param npa_callback       : Oversubscription callback function (may be NULL)

==============================================================================*/
#define ICBARB_CREATE_CLIENT_VECTOR( psVector, aMasterSlave, uNumMasterSlave, npa_callback ) \
        uNumMasterSlave,                                                     \
        icbarb_fill_client_vector( psVector, aMasterSlave, npa_callback )

ICBArb_CreateClientVectorType *icbarb_fill_client_vector(
  ICBArb_CreateClientVectorType *psVector,
  ICBArb_MasterSlaveType        *aMasterSlave,
  npa_callback                   callback);

/**=============================================================================

   @brief icbarb_create_client - Creates a new ICB arbiter client

   DEPRECATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param uNumMasterSlave : number of elements in aMasterSlave
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_client(
   const char             *pszClientName,
   ICBArb_MasterSlaveType *aMasterSlave, 
   uint32                  uNumMasterSlave);

/**=============================================================================

   @brief icbarb_create_client_ex - Creates a new ICB arbiter client with a
                                     callback function for state change
                                     notification (oversubscription notification)

   DEPRECATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.  An additional argument allows registration
   for state change notification, which allows the client to be notified 
   when the fabrics are oversubscribed.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param uNumMasterSlave : number of elements in aMasterSlave
   @param callback: Oversubscription notification callback function pointer
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_client_ex(
   const char             *pszClientName,
   ICBArb_MasterSlaveType *aMasterSlave, 
   uint32                  uNumMasterSlave, 
   npa_callback            callback);

/**=============================================================================

   @brief icbarb_create_suppressible_client_ex -
                                     Creates a new ICB arbiter client with a
                                     callback function for state change
                                     notification (oversubscription notification)

   DEPRECATED : Use native NPA functions with ICBARB_CREATE_CLIENT_VECTOR macro instead.
   Allows a client to set up a vector of master/slave pair that then
   processed to determine the internal routing parameters, which are stored
   and associated with the client.  An additional argument allows registration
   for state change notification, which allows the client to be notified 
   when the fabrics are oversubscribed.  All clock requests from this client
   will be made with suppressible requests so that the clock requests may be
   removed when the originating processor enters low power mode.

   @param client_name : name of the client that's being created
   @param aMasterSlave : Array of Master/Slave pairs
   @param uNumMasterSlave : number of elements in aMasterSlave
   @param callback: Oversubscription notification callback function pointer
 
   @return A handle to the created NPA client, NULL if the client
           creation failed

==============================================================================*/
npa_client_handle icbarb_create_suppressible_client_ex(
   const char             *pszClientName,
   ICBArb_MasterSlaveType *aMasterSlave, 
   uint32                  uNumMasterSlave, 
   npa_callback            callback);

/**=============================================================================

   @brief icbarb_issue_request - Issues an ICB arbiter request
   
   Allows a client to apply a vector of arbitration settings to the routes
   already established by a previous icbarb_create_client() call.
   The vector is a set of unions of arbitration settings of Types 1 and 2
   above (and more types in the future, as needed.)
   
   Note that the number of elements in the vector must match
   the number of master/slave pairs in the previous icbarb_create_client()
   call for that client.  If the number of elements in the
   icbarb_issue_request() call does not match, then the function
   returns an error (ICBARB_ERROR_VECTOR_LENGTH_MISMATCH.)
   
   @param client : Client making the request
   @parama MasterArbCombined : Array of Arbitration info
   @param uNumMasterArbCombined : number of elements in aMasterArbCombinedType
   
   @return The result of the request (e.g. success or an error code)
 
  SIDE EFFECTS
     If the result of the request is ICBARB_ERROR_REQUEST_REJECTED, then the
     request is set to zero (similar to the icbarb_complete_request()
     call below.)  This is because there is the possibility of the driver
     getting into an inconsistent state if this kind of cleanup is not done.

==============================================================================*/
ICBArb_ErrorType icbarb_issue_request( 
   npa_client_handle   client, 
   ICBArb_RequestType *aRequest,  
   uint32              uNumRequest);               

/**=============================================================================

   @brief icbarb_complete_request - Completes an ICB arbiter request
   
   The effect of this function is the same as a icbarb_issue_request() call
   where the vector elements are all 0.
   
   @param client : Client making the request
 
==============================================================================*/
void icbarb_complete_request( npa_client_handle client );

/**=============================================================================

   @brief icbarb_destroy_client - Destroys an ICB arbiter client
   
   Sets all the routes to 0 request, deletes the route information,
   and frees the associated memory.
   
   This will cancel the client request (if any) and free the client
   structure. The client handle can not be used to issue requests after
   being destroyed.
   
   @param client : Client making the request

==============================================================================*/
void icbarb_destroy_client( npa_client_handle client );

/**=============================================================================

   @brief icbarb_query - Queries ICB parameters
   
   Provides access to ICB internal data.
   
   @param query : Pointer to query specification
   
   @return The result of the query (e.g. success or an error code)

==============================================================================*/
ICBArb_ErrorType icbarb_query( ICBArb_QueryType *pQuery );

#ifdef __cplusplus
}
#endif

#endif /* ICBARB_H */

