#ifndef HDRMRLPCIF_H
#define HDRMRLPCIF_H
/*===========================================================================


                         H I G H     D A T A     R A T E

                    MULTI-FLOW  CONFIGURATION INTERFACE FILE 
             
                         

GENERAL DESCRIPTION

Copyright (c) 2004-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrmrlpcif.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
09/04/09   ss      CMI SU level modifications.
03/05/07   sk      Added support for ANSupported Profile change
12/21/06   spn     Accessor functions to get the FlowNNTransmitAbortTimerRev
                   value
05/04/06   ksu     EMPA: RSP part 2
03/30/06   spn     Fixed Featurization for FEATURE_HDR_QOS
03/06/06   spn     Modified the hdrmrlpcif_get_an_sup_qos_prof_attrib() 
                   to return the profiles after parsing Attribute Structure
                   returned from the hdrmrlpc module.
01/27/06   ksu     EMPA: RoHC testing
01/20/06   ksu     EMPA: link up hdrmrlpcif to hdrmrlpc
12/12/05   spn     Added support for DoS Attribute
11/22/05   ksu     EMPA packet stream: segment sequencing (forward link only)
11/21/05   ksu     EMPA packet stream: out of order delivery to route protocol forward
11/03/05   ksu     EMPA: packet stream, octet sequencing (reverse link only)
10/27/05   ksu     EMPA: octet stream, octet sequencing
08/07/05   mpa     Preliminary updates for ResKK format
04/04/04   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#ifdef FEATURE_HDR_QOS
#include "dsrlp_v.h"
#include "hdrcom_api.h"
#include "hdrmrlp_api.h"

/*===========================================================================

                        FUNCTION  DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION HDRMRLPCIF_RESET_IDLESTATE_VERNO

DESCRIPTION
  This is the utility function to reset the version number to invalid
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpcif_reset_idlestate_verno(uint8 reservation_label, ds707_direction_enum_type dir);

/*===========================================================================
FUNCTION HDRMRLPCIF_RESET_RESP_VERNO

DESCRIPTION
  This is the utility function to reset the version number to invalid
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpcif_reset_resp_verno(uint8 reservation_label, ds707_direction_enum_type dir);

/*===========================================================================
FUNCTION HDRMRLPCIF_RESET_REQ_VERNO

DESCRIPTION
  This is the utility function to reset the version number to invalid
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpcif_reset_req_verno(uint8 reservation_label, ds707_direction_enum_type dir);

/*===========================================================================

FUNCTION HDRMRLPCIF_INIT_CONFIGURATION

DESCRIPTION
  Initializes the configuration for the client specified.
  This needs to be called when the session has been configured.
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpcif_init_configuration
( 
  hdrmrlpcif_client_id_type client_id
);

/*===========================================================================

FUNCTION HDRMRLPCIF_GET_ACTIVE_FWD_RLPS

DESCRIPTION
  Gets the number of Active Forward RLPs
    
DEPENDENCIES
  None

RETURN VALUE
  Whether operation succeeded or failed.

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_active_fwd_rlps
(
  hdrmrlpcif_client_id_type client_id,
  uint8 *num_active_fwd_rlps,
  uint8 *rlp_flow
);

/*===========================================================================

FUNCTION HDRMRLPCIF_GET_ACTIVE_REV_RLPS

DESCRIPTION
  Gets the number of Active Reverse RLPs
  
DEPENDENCIES
  None

RETURN VALUE
  Whether operation succeeded or failed.

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_active_rev_rlps
(
  hdrmrlpcif_client_id_type client_id,
  uint8 *num_active_rev_rlps,
  uint8 *rlp_flow
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_IDENTIFICATION_ATTRIB

DESCRIPTION
  Gets the value of the current Flow Identification Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_identification_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  hdrcp_mrlpc_flow_identification_type * value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_TIMERS_ATTRIB

DESCRIPTION
  Gets the value of the current Flow Timers Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_timers_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  hdrcp_mrlpc_flow_timers_type * value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_RESERVATION_ATTRIB

DESCRIPTION
  Gets the value of the current Flow Reservation Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_reservation_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  hdrcp_mrlpc_flow_reservation_type * value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_RESKK_QOS_REQUEST_ATTRIB

DESCRIPTION
  Gets the value of the current ReservationKKQoSRequest Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_reskk_qos_request_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 res_label,
  dsrlp_ver_enum_type rlp_ver,
  hdrcp_mrlpc_res_qos_profile_config_type * value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_MAX_RESERVATIONS_ATTRIB

DESCRIPTION
  Gets the value of the current MAX RESERVATIONS ATTRIB. 
  It sets the value in the pointer passed in.
  NOTE: The Version is not stored, as this attribute is used on-demand basis.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_max_reservations_attrib
(
  uint8 *max_num_rsv_fwd,
  uint8 *max_num_rsv_rev,
  uint8 *max_num_open_rsv_fwd,
  uint8 *max_num_open_rsv_rev
);  

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_NAK_ENABLED_ATTRIB

DESCRIPTION
  Gets the value of the current Nak Enabled Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_nak_enabled_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  boolean *value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_HIGHER_LAYER_PROT_ATTRIB

DESCRIPTION
  Gets the value of the current Higher Layer Protocol Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_higher_layer_prot_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  uint32 * value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_PHYS_LAYER_NAK_EN_REV_ATTRIB

DESCRIPTION
  Gets the value of the current Physical Layer Nak Enabled Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_phys_layer_nak_en_rev_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  uint8   *value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_RESKK_IDLE_STATE_ATTRIB

DESCRIPTION
  Gets the value of the current ReservationKK Idle State Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_reskk_idle_state_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rsv_label,
  dsrlp_ver_enum_type rlp_dirn,
  uint32 * value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_RLP_PRIORITY

DESCRIPTION
  Gets the value of the RLP Priority. It sets the value in the pointer passed
  in.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_rlp_priority
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  uint32 * value_ptr
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_PROP_GAUP_QOS_REQUEST

DESCRIPTION
  API to send an AttributeUpdateRequest for a ReservationKKQosRequestXXX set
  of attributes
  
DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS     - AT is sending the AttributeUpdateRequest message
                  transaction_id of the message is passed back.
  
  E_NOT_ALLOWED - AT is not allowed to send a GAUP message while it is waiting
                  for a response to AttributeUpdateRequest message.
                                   
   "While the initiator is waiting for a response to an AttributeUpdateRequest 
    message, it shall not transmit another AttributeUpdateRequest message with 
    a different TransactionID field that request reconfiguration of an 
    attribute included in the original AttributeUpdateRequest message. "
  
  E_NOT_AVAILABLE - MRLP not bound to any stream
  
  E_DATA_TOO_LARGE - num_profiles greater than MAX reservations allowed
                     (20 fwd + 20 rev)
    
SIDE EFFECTS
  None
  
===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_prop_gaup_qos_request
(
  uint8 num_profile_blobs,
  hdrcp_mrlpc_gaup_qos_request_type *profile_ptr,
  uint8 * transaction_id
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_RESKK_QOS_RESPONSE_ATTRIB

DESCRIPTION
  Gets the value of the current QOS Profile Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_reskk_qos_response_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 res_label,
  //hdrcp_mrlpc_ver_enum_type rlp_dirn,
  dsrlp_ver_enum_type rlp_ver,
  hdrcp_mrlpc_res_qos_profile_config_type * value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_TWO_ROUTE_SUPPORTED_ATTRIB

DESCRIPTION
  Gets the value of the current TWO_ROUTES_SUPPORTED Attribute.
  It sets the value in the pointer passed in and also sets the version 
  number of this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  None
===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_two_route_supported_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint32 * value,
  uint8 * verno
);
/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_IDLE_STATE_ATTRIB

DESCRIPTION
  Gets the value of the current Idle State Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_idle_state_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 res_label,
  dsrlp_ver_enum_type rlp_dirn,
  uint32 * value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_DOS_ALLOWED_ATTRIB

DESCRIPTION
  Gets the value of the current Dos Allowed Attribute for Rev RLP.
  It sets the value in the pointer passed in.
  NOTE: The Version is not stored, as this attribute is used on-demand basis.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_dos_allowed_attrib
(
  uint8 rlpid,
  boolean * value_ptr
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_SEQUENCE_LENGTH_ATTRIB

DESCRIPTION
  Gets the value of the current Sequence Length Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_sequence_length_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  hdrcp_mrlpc_flow_sequence_length_type *value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_DATA_UNIT_ATTRIB

DESCRIPTION
  Gets the value of the current Data Unit Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_data_unit_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  uint8   *value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_ROUTE_PROTOCOL_SDU_ATTRIB

DESCRIPTION
  Gets the value of the current Route Protocol SDU Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_route_protocol_sdu_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  uint8   *value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_SIMULTANEOUS_DELIVERY_ON_BOTH_ROUTES_FWD_ATTRIB

DESCRIPTION
  Gets the value of the current Simultaneous Delivery On Both Routes Fwd Attribute.
  It sets the value in the pointer passed in.
  NOTE: The Version is not stored, as this attribute is used on-demand basis.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  None

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_simultaneous_delivery_on_both_routes_fwd_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  uint8   *value_ptr
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_OUT_OF_ORDER_DELIVERY_TO_ROUTE_PROTOCOL_FWD_ATTRIB

DESCRIPTION
  Gets the value of the current Out Of Order Delivery To Route Protocol Fwd Attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_out_of_order_delivery_to_route_protocol_fwd_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  uint8   *value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_FLOW_PROTOCOL_PARAMETERS_FWD_ATTRIB

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_flow_protocol_parameters_fwd_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  hdrcp_mrlpc_flow_prot_parm_fwd_type* value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_FLOW_PROTOCOL_PARAMETERS_REV_ATTRIB

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_flow_protocol_parameters_rev_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  hdrcp_mrlpc_flow_prot_parm_rev_type* value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_ROUTE_PROTOCOL_PARAMETERS_FWD_ATTRIB

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_route_protocol_parameters_fwd_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  hdrcp_mrlpc_route_prot_parm_fwd_type* value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_ROUTE_PROTOCOL_PARAMETERS_REV_ATTRIB

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_get_route_protocol_parameters_rev_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  hdrcp_mrlpc_route_prot_parm_rev_type* value_ptr,
  boolean *changed
);

/*===========================================================================

FUNCTION  HDRMRLPCIF_GET_TRANSMIT_ABORT_TIMER_REV_ATTRIB

DESCRIPTION
  Gets the value of the current transmit_abort timer rev attribute.
  It sets the value in the pointer passed in and also sets the flag to 
  indicate whether the value is different from the last time the particular
  client accessed this attribute.
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the operation succeeded or failed.

SIDE EFFECTS
  The version number of the attribute stored by this module for the 
  particular client is updated.

===========================================================================*/
extern hdrcom_errno_enum_type hdrmrlpcif_get_transmit_abort_timer_rev_attrib
(
  hdrmrlpcif_client_id_type client_id,
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_dirn,
  uint32 *value_ptr,
  boolean *changed
);

/*===========================================================================
FUNCTION  HDRMRLPCIF_IS_AN_SUP_PROF_VER_CHANGED

DESCRIPTION
  This is utility function to see if the an supported profile list has been
  changed. 
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: if the profile has been change
  FALSE: otherwise

SIDE EFFECTS
  None
===========================================================================*/

hdrcom_errno_enum_type hdrmrlpcif_is_an_sup_prof_ver_changed(hdrmrlpcif_client_id_type client_id,
                                              boolean *changed);

/*===========================================================================
FUNCTION  HDRMRLPCIF_SET_AN_SUP_PROFILE_VER_ZERO

DESCRIPTION
  This is utility function to set the an supported profile version to zero. 
  This utility should be called only if there is no synchronization between
  DS and PROTOCOL
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The version number is set to zero.
===========================================================================*/
void hdrmrlpcif_set_an_sup_profile_ver_zero(void);

/*===========================================================================
FUNCTION  HDRMRLPCIF_COPY_AN_SUP_PROF

DESCRIPTION
  This function copies the an supported profile version number and the list 
  of ANSupported profiles in the local copy.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The local copy is synchronized with the copy in protocol
===========================================================================*/
hdrcom_errno_enum_type hdrmrlpcif_copy_an_sup_prof(
  hdrmrlpcif_client_id_type client_id, 
  hdrmrlpcif_an_supported_qos_profile_type *an_supported_profiles
);

#endif  /* FEATURE_HDR_QOS */
#endif /* HDRMRLPCIF_H */


