#ifndef TZOS_SVC_ID_H
#define TZOS_SVC_ID_H

/**
@file tzos_svc_id.h
@brief Trustzone Service Registry

@section S1 TZOS Service IDs

The TZOS Service registry that defines the list of services supported
in Trustzone.

*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/tzos/tzos_svc_id.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
02/25/10   sl       Initial version
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                              DEFINES

===========================================================================*/

/** Base service ID for Service Identifier Registry */
#define TZOS_SVC_BASE_SVC_ID            (0x00000000)

/** Core Service: Provides informational/control service for TZOS */
#define TZOS_SVC_CORE_SVC_ID            (0x00000001)

/** Peripheral Image Authentication Service: Provides authentication
 * services for verifying integrity and/or authenticity of peripheral
 * binaries before enabling the peripheral */
#define TZOS_SVC_PIL_AUTH_SVC_ID        (0x00000003)

/** Data Mover Driver Service */
#define TZOS_SVC_DMOV_SVC_ID            (0x00000004)

/** Test Service */
#define TZOS_SVC_TEST_SVC_ID            (TZOS_SVC_MAX_SVC_ID - 2)

/** Slave Service (for testing) */
#define TZOS_SVC_SLAVE_SVC_ID           (TZOS_SVC_MAX_SVC_ID - 1)

/** Last service ID for Service Identifier Registry */
#define TZOS_SVC_MAX_SVC_ID             (0x7FFFFFFF)

#endif  /* TZOS_SVC_ID_H */
