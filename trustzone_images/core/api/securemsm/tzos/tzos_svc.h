#ifndef TZOS_SVC_H
#define TZOS_SVC_H

/**
@file TZOS_SVC.h
@brief Trustzone Service API

@section S1 Common header for TZOS service implementations

The Trustzone Service API provides an API for services to register
with Trustzone core and provide their function table for receiving
notifications from HLOS for request processing.

To add a service:
<ol>
<li> Add your service identifier in \c tzos_svc_id.h </li>
<li> Add an entry in the service registry in \c tzos_svc_def.h </li>
</ol>

Requirement for implementing a service:
<ol>
<li> Add a service as mentioned above for the TZOS core to recognize
your service. </li>
<li> Implement an init function as prototyped by \c
tzos_svc_init_fn_type \c &lt;svc_name&gt;_init where &lt;svc_name&gt; is as
defined in the registry entry in \c tzos_svc_def.h. </li>
<li> Register your service in the TZOS runtime registry through
\c tzos_svc_register. </li>
</ol>

Contains the public methods and related data types.
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/tzos/tzos_svc.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
02/25/10   sl       Initial version
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "IxErrno.h"

/*===========================================================================

                              DEFINES

===========================================================================*/\

/** Init Service function prototype */
typedef IxErrnoType (*tzos_svc_init_fn_type) ( uint32 svc_id );

/** Call back function for open */
typedef IxErrnoType (*tzos_svc_open_cb_type) ( uint32 session_id,
                                               uint8* msg_q,
                                               uint32 len );

/** Call back function for close */
typedef IxErrnoType (*tzos_svc_close_cb_type) ( uint32 session_id );

/** Call back function for process message */
typedef IxErrnoType (*tzos_svc_process_cb_type) ( uint32 session_id );

/** Call back function for abort */
typedef IxErrnoType (*tzos_svc_abort_cb_type) ( uint32 session_id );

/** Call back function for deinit */
typedef IxErrnoType (*tzos_svc_deinit_cb_type) ( uint32 svc_id );

/**
 * Service function table. Upon initialization a service will fill
 * the function table to publish its functions to TZOS.
 */
typedef struct
{
  /** Function to open a session to the service. Cannot be \c NULL. */
  tzos_svc_open_cb_type       open_cb;
  /** Function to close a session to the service. Cannot be \c NULL. */
  tzos_svc_close_cb_type      close_cb;
  /** Function to process a service request. Cannot be \c NULL. */
  tzos_svc_process_cb_type    process_cb;
  /** Function to abort an ongoing service request. Can be \c NULL, if
   * the service doesn't support asynchronous services. */
  tzos_svc_abort_cb_type      abort_cb;
  /** Function to deinitialize the service. Resources allocated in the
   * service init can be released here. Cannot be \c NULL. TODO:
   * Should be possible to set this NULL? */
  tzos_svc_deinit_cb_type     deinit_cb;
} tzos_svc_fn_tbl_type;

/**
 * Registry entry type that services should pass on to TZOS during
 * registration.
 */
typedef struct
{
  /** The unique service ID. */
  uint32                      svc_id;
  /** Function table of the service. */
  tzos_svc_fn_tbl_type        fn_tbl;
} tzos_svc_reg_ent_type;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
  This function allows services to register themselves with the TZOS core.
  All services are required to register with the TZOS core. If a service
  is a static configuration utility only requiring initialization, then
  the service can register a NULL fn ptrs for open, close, process, abort
  callbacks.

  @param [in] svc Service to be registered.

  @return None E_SUCCESS on success, appropriate error code on failure.
*/
IxErrnoType tzos_svc_register ( tzos_svc_reg_ent_type*  svc );

#endif  /* TZOS_SVC_H */
