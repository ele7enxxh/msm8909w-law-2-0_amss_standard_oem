#ifndef SNS_SAM_PROV_H
#define SNS_SAM_PROV_H

/*============================================================================
  @file sns_sam_prov.h

  @brief
  SAM Framework module charged with handling sensor Providers.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_queue.h"
#include "sns_sam.h"
#include "sns_sam_prov_api.h"
#include "sns_sam_init.h"

/*============================================================================
  External Objects
  ===========================================================================*/

/* Callback API for use in Provider API function calls.
 * Defined in sns_sam_prov_cb.c */
extern sns_sam_prov_callback provCB;

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
 * Whether this service object is explicitly managed by a provider.
 *
 * @param[i] serviceObj
 *
 * @return true if managed, false otherwise
 */
bool sns_sam_prov_managed( qmi_idl_service_object_type serviceObj );

/**
 * Initialize and add new sensors that are provided by the newly available
 * QMI service.
 *
 * @param[i] initData Service object; whether there was a timeout
 */
void sns_sam_prov_init_client( sns_sam_client_init_data *initData );

/**
 * Initialize all state associated with sensor Providers.  Start query
 * process to determine which sensors are available.
 */
sns_sam_err sns_sam_prov_init_fw( void );

#endif /* SNS_SAM_PROV_H */

