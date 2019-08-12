#ifndef HDRMRLPC_H
#define HDRMRLPC_H

/*===========================================================================

          M U L T I - F L O W   R A D I O   L I N K   P R O T O C O L 
                  C O N F I G U R A T I O N    F I L E

DESCRIPTION
  

Copyright (c) 2004,2005,2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007,2008      by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrmrlpc.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/11   smd     SU API cleanup.
12/20/09   pba     Send ULN only when data session is transferable
10/30/09   pba     Added ehrpd support.
07/22/09   wsh     Check FlowNNTimers and MaxAbortTimer before send ConfResp
11/04/08   sju     Accepted KKIDLEState atribute for ResLabel 0xff in SCP phase 
10/03/07   wsh     MRLPC GAUP pending queue not getting freed
09/13/07   sju     Changed the size of profile in hdrmrlpc_pending_gaup_req struct
07/30/07   pba     Obsoleted feature FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
07/17/07   pba     RelB MMPA support.
05/29/07   wsh     Disallow flow00 to be deactivated for MPA
04/09/07   sju     Added support to handle back to back Qos Request.
02/23/07   wsh     Disallow flow01 to be activated and flow00 to be deactivated
01/11/07   pba     Modified the interfaces that are used to get active forward
                   and reverse RLPs, to maintain backward compatibility.
11/21/06   yll     Added the structure tag for MSM7500
11/16/06   yll     Don't include EMPA attributes in AT-initiated Config 
                   Request if EMPA is not negotiated.
08/08/06   ksu/yll EMPA: v2.0 session negotiation
07/18/06   yll     Added support for RoHC.
07/06/06   yll     Changed the attrib_len from uint8 to uint16.
05/10/06   yll     Added EMPA support.
03/30/06   yll     Changed the setting of MaxReservations to one less than
                   max number of reservations supported.
03/30/06   spn     Fixed Featurization for FEATURE_HDR_QOS
02/28/06   yll     Removed the obsolete attributes. 
01/11/06   mpa     Changed max AN supported profiles to 255.
12/13/05   pba     Changes to support multiple personality negotiation
12/07/05   yll     Changed the max reservation to 16.
11/21/05   yll     When AT initiates the release of QoS GAUP request and 
                   AN accepts it, remove the RES qos_request entry from 
                   the current, in_config and config_result table. 
11/16/05   yll     Added support for new attributes: XmitAbortTimer, 
                   DOSAllowed, MaxRLPFlows and MaxReservations.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
08/07/05   mpa     Added AT Initiated GAUP support + new ResKK format.
11/22/04   vas     Added value for uninitialized version number 
10/25/04   mpa     Added GAUP support
09/09/04   mpa     Added Session Config support
03/08/04   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_HDR_QOS)
#include "hdrmrlpci.h"
#include "hdrerrno.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "dsm.h"
#include "hdrcp_api.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* The maximum number of activated forward and reverse RLP Flows */
#define HDRMRLP_MAX_FWD_RLPS 8 
#define HDRMRLP_MAX_REV_RLPS 8

/* The standard specifies the field of MaxNumReservationsFwd/Rev 
 * should be set to indicate one less than the maximum total number
 * of activated and deactivated Forward/Reverse Link Reservations
 * supported.
 */
#define HDRMRLP_MAX_RESERVATIONS_SUPPORTED 16
#define HDRMRLP_MAX_FWD_RESERVATIONS  (HDRMRLP_MAX_RESERVATIONS_SUPPORTED - 1)
#define HDRMRLP_MAX_REV_RESERVATIONS  (HDRMRLP_MAX_RESERVATIONS_SUPPORTED - 1)


#define HDRMRLP_MAX_PROT_COUNT 9 /* The max protocol count for supported 
                                    higher layer protocols.
                                    Max 255 in standard.  How big?  _mpa_ */


typedef struct
{
  uint8             prot_count;
  uint8             prot_id[HDRMRLP_MAX_PROT_COUNT];
}hdrmrlpc_sup_higher_layer_prot_type;


/* Max RLP Flows attribute */
typedef struct
{
  uint8 max_num_rlp_flows_fwd;
  uint8 max_num_rlp_flows_rev;
  uint8 max_activated_rlp_flows_fwd;
  uint8 max_activated_rlp_flows_rev;
} hdrmrlpc_max_rlp_flows_type;

/*---------------------------------------------------------------------------
  AT Supported Protocol Parameters PP attribute
---------------------------------------------------------------------------*/
typedef struct
{
  uint16      max_max_cid;        /* MaxSupportedMaxCID*/
  boolean     large_cid_supported; /* LargeCIDSupported */
  uint16      max_mrru;           /* MaxSupportedMRRU */
  boolean     timer_comp_supported; /* TimerBasedCompressionSupported */
  uint8       profile_count;      /* SupportedProfileCount */
  uint16      profile[HDRMRLPC_MAX_PROT_PROFILE_COUNT];

} hdrmrlpc_sup_prot_rohc_parm_value_type;

typedef struct
{
  boolean     prot_supported;   /* If Flow Protocol PP is supported */
  uint8       value_len;        /* Length of Supported protocol values */

  union
  {
    hdrmrlpc_sup_prot_rohc_parm_value_type  rohc;
  } value;

} hdrmrlpc_at_sup_prot_parm_pp_type;


typedef struct
{
  uint8             rlp_flow;    
    /* RLP Flow. Identifies the RLP. RLP sig msg are tagged using this flow */

  hdrmrlpc_ver_enum_type rlp_ver; 
    /* Identifies Forward/Reverse Link HDR MultiFlow RLP */
  
  /*Simple Attributes */
  boolean           nak_enabled; 
    /* Are Naks enabled for this RLP */
  
  uint8             higher_layer_protocol; 
    /* Should always be set to 0x00 = Octet based HDLC framing */

  uint8             phys_link_nak_enabled_rev; 
    /* Is RLP to retransmit octets when 
       PhysicalLayer.ReverseTrafficePacketMissed ind is rcvd. 
       Only for Reverse RLP (transmitter) */

  uint16            xmit_abort_timer;
    /* Maximum delay for transmission of a 
       higher layer octet for RLP NN */

  uint8             dos_allowed;
    /* Is AT allowed to send DataOverSignalling 
       messages for this RLP flow*/

  uint8             activated;
    /* Flow NN is activated or not */

  uint8             simple_sequence_length;
    /* Length of a sequence number */

  uint8             flow_prot_sdu;
    /* Flow NN provides an octet stream or packet 
       stream to the flow protocol */

  uint8             data_unit;
    /* Data unit for FlowNN is octets or RLP packet payload */

  uint8             route_prot_sdu;
    /* Each route of flow NN provides a octet stream or
       packet stream to the route protocol */

  uint8             sim_del_both_routes;
    /* FlowNN delivers the flow protocol payload 
       in order or out-of-order */

  uint8             ooo_del_route_prot;
    /* Each route of FlowNN delivers Route Protocol
       payload in order or out-of-order. */

  uint8             nak_delay_time;

  /* Complex Attributes */
  hdrmrlpc_flow_identification_type  flow_id;

  hdrmrlpc_flow_timers_type          flow_timer;  

  hdrmrlpc_flow_reservation_type     flow_rsv;

  /* Only defined for MMPA for EMPA/MFPA it is a simple attribute */
  hdrmrlpc_flow_sequence_length_type complex_seq_len;

  uint8             rlp_priority; 
  /*Note that even though this is not part of the RLP negotiated structure,
     we are storing the RLP priority here. */
} hdrmrlpc_flow_nn_config_type;


typedef struct
{
  uint8 rsv_label;
    /* Reservation Label (resKK) - we only allow 20 concurrent reservations
       but we still allow the full range of reservation labels (0-255).  
       Save the reservation label to do an index->rsv_label mapping.
       Alternatives:
       - we could use a big lookup table instead that stores ptrs to reskk 
         entries (too space consuming) 
       - consider using a Hash table instead of inserting the entries 
         sequencially (faster lookup time) */

  uint8 idle_state; 
    /* 0x00 RLP flow does not change state after connection is closed */

  hdrmrlpc_res_qos_profile_config_type    qos_request;

  hdrmrlpc_res_qos_profile_config_type    qos_response;

} hdrmrlpc_reservation_kk_config_type;

typedef struct
{
  uint8                               max_abort_timer;
  uint8                               ppp_free_authentication_support;
  uint8                               two_routes_supported;
  uint8                               at_nak_delay_supported;

  hdrmrlpc_sup_higher_layer_prot_type sup_higher_layer_prot;

  /* _mpa_ at_sup_qos_profile not supported */
  hdrmrlpc_supported_qos_profiles     an_sup_qos_profiles;

  hdrmrlpc_max_rlp_flows_type         max_rlp_flows;
  hdrmrlpc_max_reservations_type      max_reservations;

  hdrmrlpc_flow_nn_config_type        fwd_rlp_config_info[HDRMRLPC_N_MRLP_MAX_FWD];
  hdrmrlpc_flow_nn_config_type        rev_rlp_config_info[HDRMRLPC_N_MRLP_MAX_REV];

  hdrmrlpc_flow_prot_parm_fwd_type    fwd_flow_prot[HDRMRLPC_N_MRLP_MAX_FWD];
  hdrmrlpc_flow_prot_parm_rev_type    rev_flow_prot[HDRMRLPC_N_MRLP_MAX_REV];
  hdrmrlpc_route_prot_parm_fwd_type   fwd_route_prot[HDRMRLPC_N_MRLP_MAX_FWD];
  hdrmrlpc_route_prot_parm_rev_type   rev_route_prot[HDRMRLPC_N_MRLP_MAX_REV];

  /* Keep track of the total number of reservations we have stored.
     (not part of configuration) */
  uint8                               num_fwd_rsv;
  hdrmrlpc_reservation_kk_config_type fwd_res_info[HDRMRLPC_N_RES_LABELS_MAX_FWD];

  uint8                               num_rev_rsv;
  hdrmrlpc_reservation_kk_config_type rev_res_info[HDRMRLPC_N_RES_LABELS_MAX_REV];

  hdrmrlpc_at_sup_prot_parm_pp_type   at_sup_flow_prot[HDRMRLP_MAX_PROT_COUNT];
  hdrmrlpc_at_sup_prot_parm_pp_type   at_sup_route_prot[HDRMRLP_MAX_PROT_COUNT];

  /* Reservation Label 0xFF is used by Flow00, it's a special case, treat it 
   * differently (not using fwd_res_info[], rev_res_info[]*/
  hdrmrlpc_reservation_kk_config_type fwd_res_info_ff;
  hdrmrlpc_reservation_kk_config_type rev_res_info_ff;

} hdrmrlpc_config_params_type;
 
typedef struct
{
  q_link_type link;
  uint8 num_profile_blobs;                     
    /* original value paseed from DS(QNM)*/
  hdrmrlpc_gaup_qos_request_type profile[HDRMRLPC_N_RES_LABELS_MAX * 2]; 
    /* content of qos profile (fwd + rev)*/
  uint8 transaction_id;                        
    /* value of transaction_id*/
} hdrmrlpc_pending_gaup_req_type;

/*===========================================================================

                        FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRMRLPC_IS_ATTRIB_VALID_FOR_APPSUBTYPE

DESCRIPTION
  This function checks if an attribute is valid for given app subtype.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If the attribute is valid.
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_is_attrib_valid_for_appsubtype
(
  hdrhai_app_subtype_enum_type app_subtype,
  uint16 attrib_id
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_ACTIVE_FWD_RLPS

DESCRIPTION
  This function returns the maximum number of supported forward link 
  multi-flow RLPs and their associated RLP Identifiers (RLP Flow)

DEPENDENCIES
  The module calling this function needs to make sure that memory is allocated
  for listing upto the maximum supported RLPs.
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the RLP IDs of all the forward link 
              multi-flow RLPs
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_active_fwd_rlps
(
  hdrhai_app_subtype_enum_type   app_subtype,
  uint8 *num_active_fwd_rlps,
  uint8 *rlp_flow
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_ACTIVE_REV_RLPS

DESCRIPTION
  This function returns the maximum number of supported reverse link 
  multi-flow RLPs and their associated RLP Identifiers (RLP Flow)

DEPENDENCIES
  The module calling this function needs to make sure that memory is allocated
  for listing upto the maximum supported RLPs.
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the RLP IDs of all the forward link 
              multi-flow RLPs
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_active_rev_rlps
(
  hdrhai_app_subtype_enum_type   app_subtype,
  uint8 *num_active_rev_rlps,
  uint8 *rlp_flow
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_SIMPLE_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a simple attribute.
  
  0xFFFE - RLPCountMaxFwd (No GAUP allowed)                  
  0xFFFD - RLPCountMaxRev (No GAUP allowed)
  0xFFFC - MaxAbortTimer  (No GAUP allowed)
  
  0xFENN - FlowNNNakEnableFwd (GAUP)
  0xFDNN - FlowNNNakEnableRev (GAUP)         
  0xFCNN - FlowNNHigherLayerProtocolFwd (GAUP)
  0xFBNN - FlowNNHigherLayerProtocolRev (GAUP)
  0xFANN - FlowNNPhysicalLayerNakEnableRev (GAUP)
  0xF9NN - FlowNNIdleStateFwd (GAUP)
  0xF8NN - FlowNNIdleStateRev (GAUP)
  

DEPENDENCIES
  None.
  
PARAMETERS
  attrib_name - The Attribute's Name (per type)
  AttribIdx   - FlowNN (if applicable, 
                        HDRMRLPC_FLOW_INAPPLICABLE passed in otherwise)
                ResKK
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_simple_attrib
(
  hdrmrlpc_attrib_name_enum_type attrib_name,
  uint8 attrib_idx,
  uint32 * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_IDENTIFICATION_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNIdentification attribute type. (0x01NN, 0x02NN) (GAUP)
  
DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute) 
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
           
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_identification_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_flow_identification_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_SEQLEN_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNSequenceLength attribute type.

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_seqlen_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_flow_sequence_length_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_TIMERS_ATTRIB
  
DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNTimers attribute type. (0x03NN, 0x04NN) (GAUP)
  
DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn    - FlowNN of the attribute to retrieve
  direction   - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
           
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_timers_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_flow_timers_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_RESERVATION_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNReservations attribute type. (0x05NN, 0x06NN) (GAUP)

DEPENDENCIES
  The AT only supports a maximum of:
  - 3 RLPs in the fwd direction
  - 3 RLPs in the rev direction

PARAMETERS
  rlp_flow_nn    - FlowNN of the attribute to retrieve
  direction   - RLP direction (forward or reverse attribute)                 
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
    
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_reservation_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_flow_reservation_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_RESKK_QOS_REQUEST_ATTRIB
  
DESCRIPTION
  This function retrieves the current configuration and version number for 
  a ResKKQoSRequest attribute type. (0x07NN, 0x08NN) (GAUP)

DEPENDENCIES
  None.

PARAMETERS
  res_kk - ReservationKK of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
           
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_reskk_qos_request_attrib
(
  uint8 res_kk,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_res_qos_profile_config_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_RESKK_QOS_RESPONSE_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a ResKKQoSResponse attribute type. (0x09NN, 0x0ANN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  res_kk  - ReservationKK of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_reskk_qos_response_attrib
(
  uint8 res_kk,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_res_qos_profile_config_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_SHLP_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  the SupportedHigherLayerProtocols attribute (0x0000, No GAUP allowed)

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
           
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_shlp_attrib
(
  hdrmrlpc_sup_higher_layer_prot_type * value,
  uint8 * verno
);
 

/*===========================================================================

FUNCTION HDRMRLPC_GET_MAX_RLP_FLOWS_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  the MaxRLPFlows attribute (0x0004, No GAUP allowed)

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_max_rlp_flows_attrib
(
  hdrmrlpc_max_rlp_flows_type * value,
  uint8 * verno
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_MAX_RESERVATIONS_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  the MaxReservations attribute (0x0005, GAUP allowed)

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_max_reservations_attrib
(
  hdrmrlpc_max_reservations_type * value,
  uint8 * verno
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_AN_SUP_QOS_PROF_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  the ANSupportedQoSProfiles attribute (0x0003, GAUP allowed)

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_an_sup_qos_prof_attrib
(
  hdrmrlpc_supported_qos_profiles * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_POWERUP_INIT

DESCRIPTION
  Does the powerup initialization of the MRLP configuration

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrmrlpc_powerup_init( void );

/*===========================================================================

FUNCTION HDRMRLPC_CONFIG_IS_NEEDED

DESCRIPTION
  This helper function determines whether configuration is necessary.

DEPENDENCIES
  None
  _mpa_ HDRMC task context

PARAMETERS
  None
  
RETURN VALUE
  TRUE  - Configuration is necessary
  FALSE - Configuration is not necessary

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrmrlpc_config_is_needed ( void );

/*===========================================================================

FUNCTION HDRMRLPC_CONFIG_INIT

DESCRIPTION
  Initializes the in-config values to "negotiated"
                          
DEPENDENCIES
  Assumes we are only getting one config req.  Only initialize at start of
  AN INIT otherwise.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrmrlpc_config_init(void);

/*===========================================================================

FUNCTION HDRMRLPC_PROCESS_IND

DESCRIPTION
  This function processes indications given to MRLPC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for MRLPC to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrmrlpc_process_ind 
( 
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
); 

/*===========================================================================

FUNCTION HDRMRLPC_PACK_CONFIG_REQ_ATTRIBS

DESCRIPTION
  This function packs the attributes that the AT would like to configure

DEPENDENCIES
  None

PARAMETERS
  app      - Application subtype (MFPA or EMPA)
  item_ptr - Pointer to the DSM item chain holding the message
  offset   - offset from start of message to beginning of current attrib
  
RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrmrlpc_pack_config_req_attribs
(
  hdrhai_app_subtype_enum_type app,
  dsm_item_type * item_ptr,
  uint16 *aoffset
);

/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CONFIG_REQ_ATTRIB

DESCRIPTION
  This function processes MRLP attributes included in the Configuration 
  Request Message
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_id - Attribute ID of the attribute to process
  index - Flow ID for FlowNN attributes, 
          Reservation Label for ReservationKK attributes
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  TRUE  - Message unpacked successfully
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrmrlpc_unpack_config_req_attrib
(
  dsm_item_type *item_ptr,
  uint16 attrib_id,
  uint8 index,
  uint16 attrib_len,
  uint16 attrib_offset
);

/*===========================================================================

FUNCTION HDRMRLPC_PACK_CONFIG_RSP_ATTRIBS

DESCRIPTION
  This function packs the MRLP attributes included in the Configuration 
  Response Message
  
DEPENDENCIES
  None

PARAMETERS
  send_msg_ptr - Pointer to the DSM item chain holding the message
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  None
  
SIDE EFFECTS
  (attrib_offset is updated)

===========================================================================*/
extern void hdrmrlpc_pack_config_rsp_attribs
(
  dsm_item_type *send_msg_ptr,
  uint16 *attrib_offset,
  hdrhai_stream_enum_type stream_id
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_VALIDATE_CONFIG_RSP_ATTRIB

DESCRIPTION
  This function processes the attributes the AN included in its Configuration
  Response Message (AT Init).

DEPENDENCIES
  None

PARAMETERS
  attrib_id - Attribute ID of the attribute to process 
  index     - Protocol_ID for MRLP ProtocolParameterPP attributes
  value_id  - ValueID/Value of the attribute to process
  
RETURN VALUE
  TRUE  - If the attribute is rejected 
  FALSE - otherwise (The attribute is accepted if the valueID matches the one
          the AT proposed.  Including an attribute in a Configuration Response
          that was not proposed in the Request is ground for failure.)

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrmrlpc_validate_config_rsp_attrib
(
  uint16 attrib_id,
  uint8 index,
  uint8 value_id
);


/*===========================================================================

FUNCTION HDRMRLPC_VALIDATE_GAUP_CONFIG_RSP_ATTRIBS

DESCRIPTION
  This function validates the attributes received in the GAUP Attribute
  update message.  The AT is not allowed to send an 
  AttributeUpdateReject.  If it encounters an attribute that is unacceptable 
  (for example, if it violates the boundaries defined by the standard), it
  will not send an AttributeUpdateAccept message as a way to recover.
  
DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_validate_gaup_config_rsp_attribs ( void );

/*=============================================================================
FUNCTION HDRMRLPC_PROCESS_ATTRIB_UPDATE_ACCEPT_MSG

DESCRIPTION
  This function processes an AttributeUpdateAccept message for the
  Multiflow packet app.
    
DEPENDENCIES
  The AT currently does not initiate GAUP for any of the attributes.
  It should not receive an Accept message.

PARAMETERS
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrmrlpc_process_attrib_update_accept_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
);

/* EJECT */
/*=============================================================================
FUNCTION HDRMRLPC_PROCESS_ATTRIB_UPDATE_REJECT

DESCRIPTION
  This function processes an AttributeUpdateReject message for the
  Multiflow packet app.
    
DEPENDENCIES
  The AT currently does not initiate GAUP for any of the attributes.
  It should not receive an Reject message.

PARAMETERS
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrmrlpc_process_attrib_update_reject_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
);

/*===========================================================================

FUNCTION  HDRMRLPC_GAUP_INITIATE_QOS_REQUEST

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
extern hdrerrno_enum_type hdrmrlpc_prop_gaup_qos_request
(
  uint8 num_profile_blobs,
  hdrmrlpc_gaup_qos_request_type *profile_ptr,
  uint8 *transaction_id_ptr,
  uint8 preallocated_trans_id  
);

/*===========================================================================
FUNCTION HDRMRLPC_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Multiflow
  Packet Application
    
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrmrlpc_process_timer
(
  uint32 timer_id
);

/*===========================================================================

FUNCTION HDRMRLPC_SET_CONFIG_PARAMS_DEFAULT

DESCRIPTION
  Sets the Current RLP parameters to their default values.

DEPENDENCIES
  None.

PARAMETERS
  current_config - Pointer to RLP's current configuration structure in 
                   SCMDB.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrmrlpc_set_config_params_default
( 
  hdrmrlpc_config_params_type *current_config 
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of MRLP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of MRLP

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdrmrlpc_get_inconfig_handle(void);

/*===========================================================================

FUNCTION HDRMRLPC_GET_FLOW_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNFlowProtocolParametersFwd attribute type. (0x0BNN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_flow_prot_parm_fwd_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_flow_prot_parm_fwd_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_FLOW_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNFlowProtocolParametersFwd attribute type. (0x0CNN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_flow_prot_parm_rev_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_flow_prot_parm_rev_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_ROUTE_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNFlowProtocolParametersFwd attribute type. (0x0DNN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_route_prot_parm_fwd_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_route_prot_parm_fwd_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_ROUTE_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNRouteProtocolParametersRev attribute type. (0x0ENN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrmrlpc_get_route_prot_parm_rev_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_route_prot_parm_rev_type * value,
  uint8 * verno
);

#endif /* FEATURE_HDR_QOS */

/*===========================================================================

FUNCTION HDRMRLPC_FLOW_CONFIGURED_TO_DESIRED

DESCRIPTION
  This function checks flow00/01 is configured to AT supported values.
  flow00 has to be activated while flow01 has to be deactivated.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If the attribute is EMPA attribute
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_flow_configured_to_desired( void );

/*===========================================================================

FUNCTION HDRMRLPC_CLEAR_PENDING_GAUP_QUEUE

DESCRIPTION
  Clears pending GAUP queue for QoS requests.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_clear_pending_gaup_queue( void );

/*===========================================================================

FUNCTION HDRMRLPC_FLOWNN_TIMERS_ARE_VALID

DESCRIPTION
  This function checks if the FlowNNTimers are configured to
  valid values.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If the timers are valid
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_flownn_timers_are_valid( void );


#ifdef FEATURE_EHRPD
/*===========================================================================

FUNCTION HDRMRLPC_CURRENT_PERSONALITY_IS_EHRPD

DESCRIPTION
  This function is used to check whether current personality is ehrpd or not.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE if the personality is eHRPD, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_current_personality_is_ehrpd( void );
#endif /* FEATURE_EHRPD */

#endif /* HDRMRLPC_H */

