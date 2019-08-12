#ifndef AOSTLM_API_H
#define AOSTLM_API_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    A O S T L M   S U B S Y S T E M

GENERAL DESCRIPTION
API Provided to AOST features for licensing


EXTERNALIZED FUNCTIONS
aostlm_request_access 
aostlm_req_metadata 
aostlm_status 
aostlm_register_connected_callback 
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                                USAGE
 
 The AOSTLM API allows features to access the AOSTLM Server, via a
 proxy or Remote Client for licensing.  Any feature that wishes to verify
 that a valid license has been installed may use this api to request
 permission to run.
 
 The client must connect to the AOSTLM Server before it is able to process
 requests.  It has three states of operation:
 
 STOPPED:       The client has not been started.  It is not connected, nor
                is it attempting to connect.
 CONNECTING:    The client has been started and is attempting to connect to
                the server.  
 CONNECTED      The client is connected to the server and may field access
                requests.
 
          +---------+
          | STOPPED |       CANNOT FIELD REQUESTS OF ANY TYPE
          +---------+
               |            On Boot platform initializes AOSTLM Client
               V
         +------------+
         | CONNECTING |     Client attempts to connect to Server. Delay may be long
         +------------+     
               |            Server connects to client
               V                
          +-----------+
          | CONNECTED |     Client may field access requests
          +-----------+
 
 It may take some time before the client connects to the server.  A request
 for access will not be granted until the server has connected.  Features have
 3 options:
 
 1.  Call aostlm_req_access.  If the server has connected, this operation will
     return a GRANT or DENY as appropriate.  If the server has not connected,
     the operation will block the calling thread until it does.
 
 2.  Poll.  aostlm_status returns the current state of the client.  Once
     CONNECTED, a feature may expect to call aostlm_req_access without blocking
 
 3.  Register a callback.  Features may register a callback through the
     aostlm_register_connected_callback operation.  If the Client has already
     connected to the server, the callback will be called ON THE REGISTERING
     THREAD CONTEXT, so be careful to avoid deadlock (don't leave a mutex locked
     when you call the register function that you also lock in the callback).
     If the client has not connected to the server, the callback will be called
     upon connection, at which point the feature may request access without
     blocking
          
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/aostlm/api/aostlm_api.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/2014  djc     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*=====================================================================
                              Typedefs
======================================================================*/
#define AOSTLM_MAX_METADATA 2024

/* Feature Id Type */
typedef uint32 aostlm_feature_id_t;

/* Meta Data */
typedef struct aostlm_meta_data_s
{
    uint16 size;
    uint8* data_ptr;
} aostlm_meta_data_t;

/* request result type*/
typedef enum 
{
  AOSTLM_GRANT_COMM   = 0,     /* Access Granted for Valid Commercial License */
  AOSTLM_GRANT_EVAL,           /* Access Granted for Valid Evaluation License */
  AOSTLM_DENY,                 /* Access Denied */
  AOSTLM_INVALID_RSP   = 0xF   /* Reserved - Not a Valid Access Response Type */
} aostlm_access_t;

/* aostlm client state*/
typedef enum
{
    AOSTLM_STOPPED = 0,     /* Cannot use AOSTLM, it needs to be initialized */
    AOSTLM_CONNECTING,      /* Cannot use AOSTLM, it has been initialized, but has not
                               yet connected to the server */ 
    AOSTLM_CONNECTED,       /* AOSTLM is live and may be used.*/
    AOSTLM_CONNECT_FAIL     /* AOSTLM failed to connect */
} aostlm_client_state_t;

/* Function pointer for Connected callback registration */
typedef void (*aostlm_connected_cb)(void);

/*=====================================================================
                              Functions
======================================================================*/

/**
 * @brief Allows AOST Features to request access.
 * 
 * @details If the AOSTLM client is not yet connected this call will block
 *          until the server is connected AND the client has fetched license
 *          information from the server. This operation will also block if
 *          another process is requesting access.
 * 
 * @param [in] feature_id: The identifier of the feature requesting access
 *         
 * @return aostlm_access_t
 *          AOSTLM_GRANT_COMM if a valid commercial license is present
 *          AOSTLM_GRANT_EVAL if a valid evaluation license is present and
 *             uses remain
 *          AOSTLM_DENY if there is no valid license for this feature
 *
 *
 */
aostlm_access_t aostlm_req_access(aostlm_feature_id_t feature_id);

/**
 * @brief Allows AOST Features to request metadata associated 
 *        with a license.
 *
 * @param [in] feature_id: The identifier of the feature 
 *       requesting meta data
 * @param [out] meta_ptr:  The metadata fetched.  Will be set to
 *       null if none was found.
 *  
 * @details Caller allocates an unpopulated aostlm_meta_data_t 
 *          struct.  Operation will allocate the metadata buffer
 *          and set meta_ptr's data pointer to the data
 *          allocated.  Caller assumes ownership of the 
 *          metadata buffer, and must free when complete.
 *          
 *  
 * @return TRUE if metadata was found, FALSE if there is no 
 *          metadata for the specified feature
 */
boolean aostlm_req_metadata(aostlm_feature_id_t feature_id,
                            aostlm_meta_data_t* meta_ptr );

/**
 * @brief Allows client to query for aostlm status
 *
 *  
 * @return status structure with information regarding aostlm 
 *         connectivity
 *
 */
aostlm_client_state_t aostlm_status(void);

/**
 * @brief Allows feature to register a function to be called 
 *        when the AOSTLM Client is connected to the server and
 *        may begin processing requests.
 *  
 * @details When a feature wishes to request access, but does 
 *          not want to block until the server is connected, it
 *          may register a callback to be called upon
 *          connection.  If the system is already connected, the
 *          callback will be called immediately from the thread
 *          that called aostlm_register_connected_callback.  The
 *          implementation of the callback should be light
 *          weight and return quickly so as not to hold up
 *          notification of other registered components.  
 *  
 * @param cb - [in] pointer to a function that will be called 
 *                  when we connect 
 *
 */
void aostlm_register_connected_callback(aostlm_connected_cb cb);

#endif /* AOSTLM_API_H */
