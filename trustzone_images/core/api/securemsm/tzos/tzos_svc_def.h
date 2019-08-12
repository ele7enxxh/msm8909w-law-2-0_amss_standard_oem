#ifndef TZOS_SVC_DEF_H
#define TZOS_SVC_DEF_H

/**
@file tzos_svc_def.h
@brief Trustzone Service Definitions

Defines the services that are going to run in Trustzone.

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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/tzos/tzos_svc_def.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
02/25/10   sl       Initial version
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tzos_svc_id.h"

/*===========================================================================

                              DEFINES

===========================================================================*/

/** Svc List Entry preprocessor definition */
#define TZOS_SVC_DEC(svc_name) extern IxErrnoType svc_name##_init( uint32 );

/** Svc List Entry preprocessor definition */
#define TZOS_SVC_DEF(svc_id, svc_name) svc_id, #svc_name, svc_name##_init

/** Svc List entry type */
typedef struct
{
  /** Unique ID of the service. @see tzos_svc_id.h */
  uint32                svc_id;
  /** Human readable name of the service. */
  const char*           svc_name;
  /** Service initialization function. Called when TZOS initializes
   * all services. */
  tzos_svc_init_fn_type init_fn;
} tzos_svc_list_type;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
 * REQUIRED: Service Table Declaration Entry. Each service must be
 * declared before they are added to the service table.
 *
 * Syntax: TZOS_SVC_DEC( <svc_name> )
 */
TZOS_SVC_DEC( tzos_core_svc )
TZOS_SVC_DEC( dmov_svc )

/**
 *  REQUIRED: Service Table List. Each service in this list will be
 *  initialized by TZOS.
 *
 *  Syntax: TZOS_SVC_DEF(<svc_id>, <svc_name>)
 */
const tzos_svc_list_type tzos_svc_tbl[] =
{
  TZOS_SVC_DEF( TZOS_SVC_CORE_SVC_ID, tzos_core_svc ),
  TZOS_SVC_DEF( TZOS_SVC_DMOV_SVC_ID, dmov_svc ),
  NULL, NULL, NULL /* List terminating item, must be last. */
};

#endif  /* TZOS_SVC_DEF_H */
