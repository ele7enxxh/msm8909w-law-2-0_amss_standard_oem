#ifndef HDRPACQNM_H
#define HDRPACQNM_H
/*===========================================================================

            HDR QOS NEGOTIATION MANAGER (QNM)

GENERAL DESCRIPTION
  This module is responsible for negotiating the QoS parameters OTA. It 
  maintains the state of the different parameters required for the QoS Grant
  to be satisfied. 

EXTERNALIZED FUNCTIONS
  hdrpacqnm_powerup_init()
    Called only once at mobile power-up to initialize variables
    and data structures.

  hdrpacqnm_register_for_notifications()
    Qos_flow_mgr registers callbacks with qnm if it wants to be
    informed about qnm events like qos granted/released, etc.

  hdrpacqnm_deregister_notifications()
    Qos_flow_mgr deregisters callbacks with qnm if it's no longer
    interested in them.

  hdrpacqnm_process_cmd()
    Processes commands issued to qnm.

  hdrpacqnm_request_qos()
    Called to request qos.

  hdrpacqnm_release_qos()
    Called to release qos.

  hdrpacqnm_release_qos_resv_labels()
    Called to release qos for the particular resv labels.

  hdrpacqnm_qos_request_status_notification_cb()
    Called to notify qnm whether qos request was accepted.

  hdrpacqnm_qos_resp_notification_cb()
    Called to notify qnm response for a qos request.

  hdrpacqnm_resv_bound_to_active_rlp_notification()
    Called to notify qnm a resv has been bound to an RLP.

  hdrpacqnm_rlp_active_notification()
    Called to notify qnm an rlp has been activated

  hdrpacqnm_get_response_list()
    Called to get a list of active reservation labels for a particular direction

  hdrpacqnm_is_qos_granted()
    Returns true if all the conditions for a grant has been satisfied.

  hdrpacqnm_is_grant_pending()
    Returns true if a request has been made and the grant conditions are 
    not met. But still the AT is waiting on the conditions to be 
    satisfied.

  hdrpacqnm_is_qos_release_pending()
    Returns true if a qos_release has been requested and the AT is waiting
    on the Acknowledgement from the AN for the release.

  hdrpacqnm_qos_granted_setid_non_zero()
    Returns True if the GrantedSet ID is non zero for a particular Qos 
    request.

  hdrpacqnm_deactivate_all_rmac
    Updates the internal data structure to reflect the
    deactivation of all RTCMACs.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrpacqnm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
03/09/09   sa      AU level CMI modifications.
11/15/07   sk      Handle QoS modify Reject
05/01/07   sk      QoS Modify support
05/03/07   sk      Support to process bundle response message from AN
07/06/06   sk      QoS 2.0 Support
05/23/06   spn     The secondary flows are bounded by (DS707_N_RESV_MAX_FWD-1)
                   instead of DS707_N_RESV_MAX_FWD. Similarly for Reverse 
                   direction also. Did the required changes.
02/27/06   spn     Added a function to deactivate all rmacs in the internal
                   data structure of QNM.
11/11/05   spn     Changed the signature of the release_qos/request_qos to 
                   take an array of flow pointers.
10/24/05   spn     Added methods which can be used to query QNM regarding the 
                   state of the QoS.
07/22/05   spn     Added a new method hdrpacqnm_get_qos_status()
06/30/05   atp     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "IxErrno.h"
#include "dstask_v.h"

#ifdef FEATURE_HDR_QOS
#include "ps_qos_defs.h"
#include "ps_flow.h"
#include "ds707_qosdefs.h"
#include "hdrpacrlpctl.h"
#include "ds707_pkt_mgri.h"

/*===========================================================================
      Constants & Macros
===========================================================================*/

/*===========================================================================
      Typedefs
===========================================================================*/

/*---------------------------------------------------------------------------
  Call Back function prototypes that flow_mgr registers with QNM to get 
  notifications on QoS Status
---------------------------------------------------------------------------*/
typedef void (* hdrpacqnm_qos_status_notify_cb_type ) 
                                           (  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier, 
                                             uint8 resv_label, 
                                             ds707_direction_enum_type dirn );

typedef void (* hdrpacqnm_resv_bound_cb_type ) 
                                          ( uint8 resv_label,
                                            ds707_direction_enum_type dirn );

typedef void (* hdrpacqnm_resv_unbound_cb_type ) 
                                          ( uint8 resv_label,
                                            ds707_direction_enum_type dirn );


    
/*===========================================================================
      Variables
===========================================================================*/


/*===========================================================================
      Functions
===========================================================================*/
/*===========================================================================

FUNCTION      HDRPACQNM_POWERUP_INIT

DESCRIPTION   Called only once at mobile power-up to initialize variables
              and data structures.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type hdrpacqnm_powerup_init ( void );

/*===========================================================================

FUNCTION      HDRPACQNM_REGISTER_FOR_NOTIFICATIONS

DESCRIPTION   Qos_flow_mgr registers callbacks with qnm if it wants to be
              informed about qnm events like qos granted/released, etc.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type hdrpacqnm_register_for_notifications
(
  hdrpacqnm_qos_status_notify_cb_type qos_status_notification_cb,
  hdrpacqnm_resv_bound_cb_type resv_bound_notification_cb,
  hdrpacqnm_resv_unbound_cb_type resv_unbound_notification_cb
);

/*===========================================================================

FUNCTION      HDRPACQNM_DEREGISTER_NOTIFICATIONS

DESCRIPTION   Qos_flow_mgr deregisters callbacks with qnm if it's no longer
              interested in them.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type hdrpacqnm_deregister_notifications ( void );

/*===========================================================================

FUNCTION      HDRPACQNM_PROCESS_CMD

DESCRIPTION   Processes commands issued to qnm.

DEPENDENCIES  This should be called from DS task.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================

FUNCTION      HDRPACQNM_REQUEST_QOS

DESCRIPTION   Called to request Qos for the first time or to modify an
              existing Qos.

DEPENDENCIES  Assumes Qos_flow_mgr is calling this in DS context.
              The caller should pass in the list of qos specs, but don't free
              it afterwards. 

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type hdrpacqnm_request_qos 
( 
  uint8 num_ds_flows,        /* Number of PS Flows included. Each can contain *
                              * upto 2 QoS Requests, one in Fwd, one in Rev   *
                              * Direction                                     */
  ds707_flow_type *ds_flow_ptr_request[],
  boolean is_new_request,
  boolean is_ue_resync
);

/*===========================================================================

FUNCTION      HDRPACQNM_RELEASE_QOS

DESCRIPTION   Called to release Qos.

DEPENDENCIES  Assumes Qos_flow_mgr is calling this in DS context. The caller
              should pass in the list of Qos_specs. 
              
RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type hdrpacqnm_release_qos
(
  uint8 num_qos_releases, /* Number of qos to be released in the list below*/
  ds707_flow_type *ds_flow_ptr_request[]
);

/*===========================================================================

FUNCTION      HDRPACQNM_RELEASE_QOS_RESV_LABELS

DESCRIPTION   Called to release Qos, given resv labels. The function does 
              not update any book keeping for the requested releases.

              This function is to be used either on power cycle or when the 
              AT returns to a QoS Aware system, from an UnAware system, to 
              release Qos of ResvLabels, whose DS FLOWS have been deleted.

DEPENDENCIES  
              
RETURN VALUE  E_NOT_AVAILABLE: If MFPA is not negotiated
              E_DATA_TOO_LARGE:If such a big GAUP request is not supported
              E_NOT_ALLOWED:   If This request is not allowed
              E_SUCCESS:       Returned on Success

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type  hdrpacqnm_release_qos_resv_labels
(
  uint8 label_count,
  ds707_resv_label_type resv_label[(DS707_N_RESV_MAX_FWD-1) + (DS707_N_RESV_MAX_REV-1)]
);

/*===========================================================================

FUNCTION      HDRPACQNM_QOS_REQUEST_STATUS_NOTIFICATION_CB

DESCRIPTION   Called to notify qnm whether qos request was accepted.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_qos_request_status_notification_cb
(
  uint8 trans_id,                          /* Transaction id               */
  ds707_qos_status_enum_type gaup_ind   /* GAUP indication    */
);

/*===========================================================================

FUNCTION      HDRPACQNM_QOS_RESP_NOTIFICATION_CB

DESCRIPTION   Called to notify qnm the response to a qos request.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_qos_resp_notification_cb
(
  uint8 resv_label,                      /* Reservation label              */
  ds707_direction_enum_type dirn,         /* Reservation Direction          */
  hdrcp_mrlpc_res_qos_profile_config_type *profile_config /* Profile Configuration */
);

/*===========================================================================

FUNCTION      HDRPACQNM_RESV_BOUND_TO_ACTIVE_RLP_NOTIFICATION

DESCRIPTION   Called to notify qnm a reservation has been bound to an RLP.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_resv_bound_to_active_rlp_notification
(
  uint8 resv_label_count,                /* Reservation label              */
  uint8 resv_label[],
  ds707_direction_enum_type dirn,        /* Reservation direction          */ 
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver,            /* RLP Version                    */
  boolean bound                          /* TRUE=bound, FALSE = unbounded  */
);

/*===========================================================================

FUNCTION      HDRPACQNM_RLP_ACTIVE_NOTIFICATION

DESCRIPTION   Called to notify qnm an RLP has been activated. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_rlp_active_notification
(
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver,               /* RLP version                    */
  boolean activated                      /* TRUE=activated,     
                                            FALSE = deactivated  */
);

#ifdef FEATURE_HDR_REVA_L1 
/*===========================================================================

FUNCTION      HDRPACQNM_RLP_BOUND_NOTIFICATION

DESCRIPTION   Called to notify QNM that RLP is bound 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_rlp_bound_notification
(
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver,               /* RLP version                    */
  uint8 rtcmac_id
);

/*===========================================================================

FUNCTION      HDRPACQNM_RLP_UNBOUND_NOTIFICATION

DESCRIPTION   Called to notify QNM that RLP is unbound. 
              The Parameter rctmac_id is not used.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_rlp_unbound_notification
(
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver,               /* RLP version                    */
  uint8 rtcmac_id
);

/*===========================================================================

FUNCTION      HDRPACQNM_RTCMAC_ACTIVATED_NOTIFICATION

DESCRIPTION   Called to notify that RTCMAC is Activated

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_rtcmac_activated_notification
(
  uint8 rtcmac_id
);
#endif /* FEATURE_HDR_REVA_L1 */

/*===========================================================================
FUNCTION       HDRPACQNM_RELEASE_FLOW

DESCRIPTION    This method is used to clean up for a ps_flow. This method 
               does not trigger an OTA message for releasing QoS.

DEPENDENCIES   None. 

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
void hdrpacqnm_release_flow
(
  ds707_flow_type *ds_flow_ptr
);

/*===========================================================================

FUNCTION      HDRPACQNM_RTCMAC_DEACTIVATED_NOTIFICATION

DESCRIPTION   Called to notify that RTCMAC is de-Activated

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_rtcmac_deactivated_notification
(
  uint8 rtcmac_id
);

/*===========================================================================

FUNCTION      HDRPACQNM_GET_RESPONSE_LIST

DESCRIPTION   Called to get a list of reservation labels for a particular 
              direction 

DEPENDENCIES  None.

RETURN VALUE  resv_label is filled up with the list, and
              num_reservation_lanels_ptr is filled with the number in the list

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnm_get_response_list
(
  uint8 resv_label[],                     /* List of reservation labels   */
  uint8 *num_reservation_labels_ptr,      /* Num of reservation labels    */
  ds707_direction_enum_type dirn          /* Direction of the reservation 
                                             labels                       */
); 

/*===========================================================================

FUNCTION      HDRPACQNM_IS_QOS_GRANTED

DESCRIPTION   Checks for the conditions for QoS for a particular reservation
              and direction and returns true if all the conditions for 
              QoS have been satisfied
             

DEPENDENCIES  None.

RETURN VALUE  TRUE implies that QoS has been granted
              FALSE implies that QoS has not been granted

SIDE EFFECTS  None.

===========================================================================*/
boolean hdrpacqnm_is_qos_granted
( 
  uint8 resv_label, 
  ds707_direction_enum_type dirn
);

/*===========================================================================

FUNCTION      HDRPACQNM_IS_QOS_MODIFY_REJECTED

DESCRIPTION   Checks if the QoS modify was rejected.

DEPENDENCIES  None.

RETURN VALUE  TRUE implies that QoS MODIFY was rejected
              FALSE implies that QoS MODIFY was not rejected

SIDE EFFECTS  None.

===========================================================================*/
boolean hdrpacqnm_is_qos_modify_rejected
( 
  uint8 resv_label, 
  ds707_direction_enum_type dirn
);
/*===========================================================================

FUNCTION      HDRPACQNM_IS_QOS_GRANT_PENDING

DESCRIPTION   This function returns is QoS request has been sent, but the 
              Conditions for grant is still pending. 
              1) QoS request has been sent and accepted Or
              2) QoS has been granted but the other conditions have not 
                 been satistied
             

DEPENDENCIES  None.

RETURN VALUE  TRUE implies that QoS Grant is pending
              FALSE implies that QoS has been granted

SIDE EFFECTS  None.

===========================================================================*/
boolean hdrpacqnm_is_qos_grant_pending
( 
  uint8 resv_label, 
  ds707_direction_enum_type dirn
);


/*==========================================================================
FUNCTION      HDRPACQNM_IS_QOS_RELEASE_PENDING

DESCRIPTION   This function returns is QoS release has been made 
              and has not been released yet
             

DEPENDENCIES  None.

RETURN VALUE  TRUE  implies that QoS RELEASE is pending
              FALSE implies that QoS is either not released, or released
                    or is an invalid query

SIDE EFFECTS  None.

===========================================================================*/
boolean hdrpacqnm_is_qos_release_pending
( 
  uint8 resv_label, 
  ds707_direction_enum_type dirn
);

/*==========================================================================
FUNCTION      HDRPACQNM_QOS_GRANTED_SETID_ZERO

DESCRIPTION   Returns whether the granted SetID is zero or not.  
             

DEPENDENCIES  None.


RETURN VALUE  If ret_value is filled up correctly then E_SUCCESS is returned.
              otherwise E_FAILURE is returned.

              TRUE  implies that the granted setid is non zero
              FALSE implies that either the QoS was never granted, 
                    or QoS Granted is non-zero.

SIDE EFFECTS  None.
===========================================================================*/
errno_enum_type hdrpacqnm_qos_granted_setid_non_zero 
(
  uint8 resv_label,
  ds707_direction_enum_type dirn,
  boolean *ret_value
);

/*==========================================================================
FUNCTION      HDRPACQNM_QOS_GET_GRANTED_SETID

DESCRIPTION   Returns the granted SetID
             

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
errno_enum_type hdrpacqnm_qos_get_granted_setid(
  uint8 resv_label,
  ds707_direction_enum_type dirn,
  uint8 *granted_setid
);

/*==========================================================================
FUNCTION      HDRPACQNM_QOS_GET_START_SETID

DESCRIPTION   Returns the start SetID
             

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
errno_enum_type hdrpacqnm_qos_get_start_setid(
  uint8 resv_label,
  ds707_direction_enum_type dirn,
  uint8 *start_setid
);
/*==========================================================================
FUNCTION      HDRPACQNM_DEACTIVATE_ALL_RMAC

DESCRIPTION   Updates the internal data structure to reflect the
              deactivation of all RTCMACs.
             
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacqnm_deactivate_all_rmac 
(
  void
);

/*===========================================================================

FUNCTION      HDRPACQNM_IS_QOS_REQUEST_ACCEPTED_BY_AN

DESCRIPTION   Returns if the AN has replied with a GAUP ACCEPT or not for 
              a QoS Request
             

DEPENDENCIES  None.

RETURN VALUE  TRUE  implies that a GAUP Accept has been received
              FALSE implies that a GAUP Accept has not been received

SIDE EFFECTS  None.

===========================================================================*/
boolean hdrpacqnm_is_qos_request_accepted_by_an
( 
  uint8 resv_label, 
  ds707_direction_enum_type dirn
);

/*==========================================================================
FUNCTION      HDRPACQNM_SET_PREV_STATE

DESCRIPTION   Sets previous state
             
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean hdrpacqnm_set_prev_state(uint8 resv_label, ds707_direction_enum_type dirn);
#endif  /* FEATURE_HDR_QOS */
#endif  /*  HDRPACQNM_H */
