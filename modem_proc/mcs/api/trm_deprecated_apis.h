#ifndef TRM_DEPRECATED_APIS_H
#define TRM_DEPRECATED_APIS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

              Transceiver Resource Manager Deprecated APIs Header File

GENERAL DESCRIPTION

  This file provides some common definitions for trm.cpp & trmlog.cpp


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/trm_deprecated_apis.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
01/22/2015   jm      Remove deprecated APIs not referenced in CRM build (2nd round)
12/17/2014   jm      Adding deprecated attribute to all APIs
12/10/2014   jm      Remove deprecated APIs not referenced in CRM build
07/23/2014   sr      GERAN using deprecated priority causing L2G resel/redir/CCO 
                     failure in DR-DSDS mode (700576)
06/24/2014   mn      Adding support for num pending callbacks in req and not,
                      req and not enhanced and unlock callbacks. (CR: 684638).
05/15/2014   mn      Initial version.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

#ifndef TEST_FRAMEWORK
#define TRM_DEPRECATED_API __attribute__ ((deprecated))
#else
#error code not present
#endif /* TEST_FRAMEWORK */

/*============================================================================
 

FUNCTION TRM_GET_SIMULT_CAP
 TRM get simultaneous capability

DESCRIPTION
  This api will return a 8 bit value where
  Bit 0 = 1 indicates SHDR is enabled.  
  Bit 1 = 1 indicates SVDO is enabled.
  Bit 2 = 1 indicates SVLTE is enabled 
  Bit 3 = 1 is reserved.
  Bit 4 = 1 indicates DSDA is enabled 
  Bit 5 = 1 indicates SGLTE and SGTDS are enabled 

  If SVDO bit(bit 1) is set, then the SHDR bit(bit 0) should be treated as
  ‘don’t care’. Following macros can be used to identify the bits :
   TRM_SHDR_IS_ENABLED        (0x01)
   TRM_SVDO_IS_ENABLED        (0x02)
   TRM_SVLTE_IS_ENABLED       (0x04)
   TRM_SVLTE_IS_DSDA          (0x10)
   TRM_SGLTE_SGTDS_IS_ENABLED (0x20)
   
  Note: Value 0x8 is reserved.
 

DEPENDENCIES
  trm_init() should have been called by tmc prior to using this api
 

RETURN VALUE
 
SIDE EFFECTS
 

============================================================================*/
uint8 trm_get_simult_cap( void );

/*============================================================================
CALLBACK TRM_BAND_GRANT_CALLBACK_T

DESCRIPTION
  The prototype for band callback functions, used by the Transceiver manager
  to inform the clients of transceiver management events.
  
DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
typedef void (* trm_band_grant_callback_type)
(
trm_client_enum_t client,    /* Client requested for the band change */
trm_band_type band,          /* Band class */
trm_band_grant_return_type granted,   /* Status of band grant */
trm_request_tag_t tag
);

/*============================================================================

FUNCTION TRM_GET_REASON

DESCRIPTION
  This function returns the reason from the internal TRM structure table which
  will be the last reason with which the client requested lock. If the client 
  is not holding the chain nor has made any request/reservation, then 
  TRM_NUM_REASONS will be returned.  
  
DEPENDENCIES
  None

RETURN VALUE
  trm_reason_enum_t

SIDE EFFECTS
  None

============================================================================*/

trm_reason_enum_t trm_get_reason
(
trm_client_enum_t       client_id
);

/*============================================================================

CALLBACK TRM_CHANGE_CALLBACK_T

DESCRIPTION
  The prototype for change callback functions, used by the Transceiver
  Resource Manager to inform the clients when it's change request is
  satisfied.
  
DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*trm_change_callback_t)
(
/* The client which is being informed of an event */
trm_client_enum_t               client,

/* The new changed reason */
trm_reason_enum_t               reason,

/* The result being sent to the client */
trm_change_result_enum_t        result,

/* Anonymous payload echoed from trm_change_reason() */
trm_request_tag_t               tag
);

/*============================================================================

FUNCTION TRM_GET_CHAIN_HOLDER

DESCRIPTION
  Returns the TRM client currently holding the chain
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function does not check if the client is holding any lock before 
  comparing the priorities.

============================================================================*/

trm_client_enum_t trm_get_chain_holder
(
trm_grant_event_enum_t chain,
trm_reason_enum_t*     reason
);

/*============================================================================

CALLBACK TRM_GRANT_ENH_CALLBACK_T

DESCRIPTION
  The prototype for event callback functions, used by the Transceiver manager
  to inform the clients of transceiver management events.
  
DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*trm_grant_enh_callback_t)
(
/* The client which is being informed of an event */
trm_client_enum_t               client,

/* The event being sent to the client */
trm_grant_event_enum_t          event,

/* Anonymous payload echoed from trm_request_and_notify() */
trm_request_tag_t               tag,

/* Duration by which the lock is extended */
trm_duration_t                  duration

);

/*============================================================================

CALLBACK TRM_UNLOCK_CALLBACK_T

DESCRIPTION
  The prototype for unlock event callback functions, used by the Transceiver
  Resource Manager to inform the clients of when it should unlock a resource
  it holds.
  
DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*trm_unlock_callback_t)
(
/* The client which is being informed of an event */
trm_client_enum_t               client,

/* The event being sent to the client */
trm_unlock_event_enum_t         event,

/* Sclk timestamp for TRM_UNLOCK_BY */
uint32                          unlock_by_sclk
);

/*============================================================================

FUNCTION TRM_RETAIN_LOCK

DESCRIPTION
  Informs the Transceiver Resource Manager that the client wants to hold
  the resource indefinitely.  The TRM may inform the client that it must
  give up the lock through the supplied unlock callback.
  
DEPENDENCIES
  The client must be holding a transceiver resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_retain_lock
(
/* The client which is attempting to extend the lock duration */
trm_client_enum_t               client,

/* The required sclk extension, from "now" */
trm_unlock_callback_t           unlock_callback
) TRM_DEPRECATED_API;

/*============================================================================

FUNCTION TRM_RELEASE

DESCRIPTION
  Release the transceiver resource currently held by a client.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If no client is waiting for the resource, the RF chain will be turned off.

============================================================================*/

void trm_release
(
/* The client which is releasing the resource */
trm_client_enum_t               client
);

/*============================================================================

CALLBACK trm_unlock_callback_advanced_t

DESCRIPTION
  The prototype for unlock event callback functions, used by the Transceiver
  Resource Manager to inform the clients of when it should unlock a resource
  it holds.
  
DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*trm_unlock_callback_advanced_t)
(
trm_unlock_cb_advanced_data_t* unlock_data
);

/*============================================================================

CALLBACK TRM_GRANT_CALLBACK_T

DESCRIPTION
  The prototype for event callback functions, used by the Transceiver manager
  to inform the clients of transceiver management events.
  
DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*trm_grant_callback_t)
(
/* The client which is being informed of an event */
trm_client_enum_t               client,

/* The event being sent to the client */
trm_grant_event_enum_t          event,

/* Anonymous payload echoed from trm_request_and_notify() */
trm_request_tag_t               tag
);

/*============================================================================

FUNCTION TRM_FREQ_REQUEST

DESCRIPTION
  Specifies the given client needs the given transciever resource, for the
  given duration, for the supplied reason.

  The resource request is immediately evaluated, and the result returned.

  This may be used in conjunction with trm_reserve_at(). This is different to
  old api since it takes band as an argument. This is used to check if the
  band is valid and also if components mapped to the band don't have any
  concurrency issues.
  
DEPENDENCIES
  None

RETURN VALUE
  TRM_DENIED          - the client was denied the transceiver resources.
  TRM_GRANTED_CHAIN0  - the client can now use the primary RF chain.
  TRM_GRANTED_CHAIN1  - the client can now use the secondary RF chain.

SIDE EFFECTS
  If the client currently holds an RF chain, that chain is released before
  the request is evaluated.

============================================================================*/

trm_chain_grant_return_type trm_freq_request
(
/* The client which needs the transceiver resource */
trm_client_enum_t               client,

/* The transceiver resource which is being requested */
trm_resource_enum_t             resource,

/* How long the resource will be needed for (in sclks) */
trm_duration_t                  duration,

/* Why the resource is needed (used for priority decisions) */
trm_reason_enum_t               reason,

trm_frequency_type_t            freq_info
);

/*============================================================================
FUNCTION TRM_REQUEST_FREQUENCY_TUNE

DESCRIPTION
  Request for a new band tune before retuning RF.
  
DEPENDENCIES
  NONE

RETURN VALUE
  TRM_BAND_REQUEST_GRANTED - Client can go through with the RF tune
  TRM_BAND_REQUEST_PENDING - Client needs to wait for the band grant cb

SIDE EFFECTS
  Sets the client's band class in TRM

============================================================================*/
trm_band_grant_return_type trm_request_frequency_tune
(
trm_client_enum_t          client,      /* Client requesting a band change */
trm_frequency_type_t       freq_info,         /* New band that the client wants to use */
trm_band_grant_callback_type band_grant_cb,  /* Band Grant Callback */
trm_request_tag_t tag
) TRM_DEPRECATED_API;

/*============================================================================

FUNCTION TRM_CHANGE_PRIORITY

DESCRIPTION
  When a client changes what it is doing, it should change the advertised
  reason for holding the transceiver resource, so its priority will change.

  Eg) A client request the transceiver resource for listening for a PAGE.  If
  it receives one, it would change its priority to PAGE_RESPONSE and attempt
  to respond to the page, and eventually change its priority to TRAFFIC.
  
DEPENDENCIES
  The client must be holding a transceiver resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void trm_change_priority
(
/* The client whose priority is to be changed */
trm_client_enum_t               client,

/* The new reason why the RF lock is held (used for priority decisions) */
trm_reason_enum_t               reason
) TRM_DEPRECATED_API;

trm_grant_event_enum_t trm_get_granted
(
/* The client who's RF chain ownership is desired */
trm_client_enum_t               client
);

rfm_device_enum_type trm_get_rf_device
(
/* The client who's RF chain ownership is desired */
trm_client_enum_t               client
);


/*============================================================================

FUNCTION TRM_FREQ_RESERVE_AT

DESCRIPTION
  Specifies the given client needs the given transceiver resource at the given
  time, for the given duration, for the supplied reason. This is different to
  old api since it takes band as an argument. This is used to check if the
  band is valid and also if components mapped to the band don't have any
  concurrency issues.

  This would be used with the "trm_request()" function, below.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the client currently holds an RF chain, the chain is released.

============================================================================*/
void trm_freq_reserve_at
(
/* The client which needs the RF resource */
trm_client_enum_t               client,

/* The RF resource which is being requested */
trm_resource_enum_t             resource,

/* When the resource will be needed (sclks timestamp) */
trm_time_t                      when,

/* How long the resource will be needed for (in sclks) */
trm_duration_t                  duration,

/* Why the resource is needed (used for priority decisions) */
trm_reason_enum_t               reason,

trm_frequency_type_t            freq_info
);

/*============================================================================

FUNCTION TRM_GET_PRIORITY_INV_ENABLED

DESCRIPTION
  This function returns the boolean value indicating the status of priority 
  inversion for the reason passed as an argument. 
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if priority inversion is set for the reason.
  FALSE otherwise

SIDE EFFECTS
  This function does not check if the client is holding any lock before 
  comparing the priorities.

============================================================================*/

boolean trm_get_priority_inv_enabled 
(
/* Cleint Id */
trm_client_enum_t       client,

/* Reason for which the inversion status to be checked */
trm_reason_enum_t       reason
);

#endif /* TRM_DEPRECATED_APIS_H */