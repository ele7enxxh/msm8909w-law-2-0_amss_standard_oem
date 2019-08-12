#ifndef TZOS_CORE_SVC_H
#define TZOS_CORE_SVC_H

/**
@file tzos_core_svc.h
@brief Core Service implementation header file

@section S1 Core Service

Non-secure side clients can use this API to access TZOS Core services.

@subsection SS1 Core Service Version

Request \c TZOS_CORE_SVC_GET_VERSION_REQ_ID will query the current
Core service version.

@subsection SS2 HLOS Start Address

Request \c TZOS_CORE_SVC_START_ADDR_REQ_ID allows setting the HLOS
start address for cold and warm booting. Primary CPU (CPU0) cold boot
flow is not affected by these addresses.

For CPU1 cold boot and warm boot on all CPUs, TZOS will be the very
initial code executed on the CPU. After TZOS has been initialized, CPU
will be switched to non-secure mode and control is given to the HLOS
to continue booting; HLOS start addresses are needed to make this
jump.

The start addresses for CPU1 apply to other secondary CPUs in the
system. The assumption is that CPUs from CPU2 to CPUn, if present in
the system, can be handled similar to CPU1.

@defgroup tzos_core_svc_group TZOS CORE Service
@ingroup tzos_core_svc_group
@{
*/
/*===========================================================================
  Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/tzos/tzos_core_svc.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/17/10   tk       Initial version.

===========================================================================*/

/*===========================================================================

          INCLUDE FILES

============================================================================*/
#include "comdef.h"
#include "tzos_svc.h"
#include "tzos_svc_common.h"

/*------------------------------------------------------------------------
                            PUBLIC TYPES
------------------------------------------------------------------------*/

/** Core service request IDs. */

/** Request for Core service version. The related structure is \c
 * tzos_svc_get_version_req_type. */
#define TZOS_CORE_SVC_GET_VERSION_REQ_ID     0x00000001
/** Request for Core service to set HLOS warm and cold boot
 * addresses. The related structure is \c
 * tzos_core_svc_start_addr_req_type. */
#define TZOS_CORE_SVC_START_ADDR_REQ_ID      0x00000002

/** Core service response ID for \c
 * TZOS_CORE_SVC_GET_VERSION_REQ_ID. The related structure is \c
 * tzos_svc_get_version_rsp_type. */
#define TZOS_CORE_SVC_GET_VERSION_RES_ID     0xFFFF0001
/** Core service response ID for \c
 * TZOS_CORE_SVC_START_ADDR_REQ_ID. The related structure is \c
 * tzos_core_svc_start_addr_rsp_type. */
#define TZOS_CORE_SVC_START_ADDR_RES_ID      0xFFFF0002

/** @name HLOS Start Address slots
 * HLOS start address slots provided by Core service \c
 * TZOS_CORE_SVC_START_ADDR_REQ_ID. */
/*@{*/
#define TZOS_CORE_SVC_START_ADDR_CPU1_COLD   0x00000001 /**< CPU1 cold boot */
#define TZOS_CORE_SVC_START_ADDR_CPU1_WARM   0x00000002 /**< CPU1 warm boot */
#define TZOS_CORE_SVC_START_ADDR_CPU0_WARM   0x00000004 /**< CPU0 warm boot */
/*@}*/

/**
 * Request data type for setting the HLOS warm and cold boot
 * addresses.
 */
typedef struct
{
  tzos_svc_common_hdr_type common; /** Common header */

  /** Combination of \c TZOS_CORE_SVC_START_ADDR_* values. The same
   * address can be set to multiple HLOS start address slots by
   * bitwise ORing the values. Unused bits will be ignored and should
   * be kept zero. Zero value will be ignored. */
  uint32 flags;

  /** The address to be programmed. Must be physical address, can be
   * \c NULL. Addresses in the secure memory are not accepted, in such
   * a case the service will return with \c E_BAD_ADDRESS. */
  void* addr;
} tzos_core_svc_start_addr_req_type;

/**
 * Response data type from setting the HLOS warm and cold boot
 * addresses.
 */
typedef struct
{
  /** Common header */
  tzos_svc_common_hdr_type common;
  /**Contains \c E_SUCCESS if the operation was successfull, otherwise
   * contains an error code. */
  uint32 err;
} tzos_core_svc_start_addr_rsp_type;

/** @} End of tzos_core_svc_group */
#endif /* TZOS_CORE_SVC_H */
