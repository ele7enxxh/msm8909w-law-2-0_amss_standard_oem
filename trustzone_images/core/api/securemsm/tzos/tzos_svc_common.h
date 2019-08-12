#ifndef TZOS_SVC_COMMON_H
#define TZOS_SVC_COMMON_H

/**
@file tzos_svc_common.h
@brief TZOS Service common header file

The TZOS Service Common header file contains common data types which will
be common to the different secure TZOS services, such as Recho, PIL Auth etc.

This file contains the methods and the related data types, of the secure
services running inside TZ
*/
/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/tzos/tzos_svc_common.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
06-02-10   na       Initial version
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES

============================================================================*/
#include "comdef.h"
#include "IxErrno.h"

/*------------------------------------------------------------------------
                            DEFINES
------------------------------------------------------------------------*/
/**
 * @name TZOS Service Common Header Type
 *
 * Common Header structure type. Supporting this structure in a
 * service is optional.
 */
typedef struct
{
  /** Session ID that must match an open session to a service */
  uint32 session_id;
  /** Request/response ID. Request ID is set by the non-secure
   * client. A TZOS service will set the response ID */
  uint32 msg_id;
  /** Length of the Message in bytes */
  uint32 msg_len;
} tzos_svc_common_hdr_type;

/**
 * @name TZOS Service get version Request Header Type Message
 *
 * Structure for requesting service version numbers. Supporting this
 * structure in a service is optional.
 */
typedef struct
{
   tzos_svc_common_hdr_type common;  /**<-- Common header*/
} tzos_svc_get_version_req_type;

/**
 * @name TZOS Service get version Response Header Type
 *
 * Structure for receiving service version number from a
 * service. Supporting this structure in a service is optional.
 */
typedef struct
{

   tzos_svc_common_hdr_type common;  /**<-- Common header */
   uint32 err;  /**<-- Service error status */
   uint32 version_major; /**<-- Major version number of the Security service*/
   uint32 version_minor; /**<-- Minor version number of the Security service*/
} tzos_svc_get_version_rsp_type;

#endif /* TZOS_SVC_COMMON_H */
