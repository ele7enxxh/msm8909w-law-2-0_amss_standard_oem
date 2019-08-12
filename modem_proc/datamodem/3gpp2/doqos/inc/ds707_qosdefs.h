#ifndef DS707_QOSDEFS_H
#define DS707_QOSDEFS_H

/*===========================================================================
                            DS707 QOS DEFINITIONS
                            
GENERAL DESCRIPTION
  This file contains the typedefs, enums and definitions and declarations
  that are common to various modules using QOS on 1X/DO system

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_qosdefs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/09   sk      eHRPD: UE initiated QoS
05/01/07   sk      QoS Modify support
03/26/07   sk      Added EMPA MPA handoff 
03/05/07   sk      Added DS707_NETWORK_ACQUIRE indication to exchange and process
                   the NETWORK ACQUIRE indication
03/05/07   sk      Added support for ANSupported Profile change
07/06/06   sk      QoS 2.0 Support
05/08/06   az      Changes for code re-structuring so that failure cases 
                   in request/release of QoS are handled appropriately
02/27/06   sk      Added DS FLOW states
02/23/06   spn     Added an enum to ds707_qos_status_enum_type
11/07/05   sk      Added API bundling
07/14/05   vas     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

/*===========================================================================
      Constants & Macros
===========================================================================*/
#ifdef FEATURE_HDR_QOS
/* Maximum number of simultaneous Forward and Reverse Reservations */
#define DS707_N_RESV_MAX_FWD 16
#define DS707_N_RESV_MAX_REV 16

#define DS707_MAX_N_RESV_PER_DIR  16

#ifdef FEATURE_DATA_3GPP2_ULT
#define DS707_MAX_DS_FLOWS 1
#else
#define DS707_MAX_DS_FLOWS  (DS707_N_RESV_MAX_FWD + DS707_N_RESV_MAX_REV)
#endif /*FEATURE_DATA_3GPP2_ULT*/
/*---------------------------------------------------------------------------
  May need to revisit the MAX_SIZE. For now we are assuming a MAX_SIZE = 100
  to be sufficient
---------------------------------------------------------------------------*/
#define HDRPACQNM_MAX_SUB_BLOB_SIZE 100

/*---------------------------------------------------------------------------
  Maximum profiles per flow
---------------------------------------------------------------------------*/
#define DS707_MAX_PROFILE_PER_FLOW  7

#endif /*FEATURE_HDR_QOS*/
/*===========================================================================
      Typedefs
===========================================================================*/
/*---------------------------------------------------------------------------
  State of DS flow. 
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_DS_FLOW_STATE_FREE              = 0,
  DS707_DS_FLOW_STATE_ALLOCATED         = 1
} ds707_ds_flow_state_enum_type;

/*---------------------------------------------------------------------------
  Status of what is happening to filters that belong to a ds_flow

  Fltr state will be DS707_FLOW_FLTR_NULL if DQOS operations haven't been
  performed on a ds_flow
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_FLOW_FLTR_STATE_MIN   = 0,
  DS707_FLOW_FLTR_NULL        = DS707_FLOW_FLTR_STATE_MIN,
  DS707_FLOW_FLTR_INSTALLING  = 1,
  DS707_FLOW_FLTR_INSTALLED   = 2,
  DS707_FLOW_FLTR_DELETING    = 3,
  DS707_FLOW_FLTR_DELETED     = 4,
  DS707_FLOW_FLTR_STATE_MAX
} ds707_flow_fltr_state_enum_type;

/*---------------------------------------------------------------------------
  Status of what is happening to air link QoS of a ds_flow

  Air link QoS state will be DS707_FLOW_AIR_LINK_QOS_NULL if semi-dynamic QoS
  operations haven't been performed on a ds_flow
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_FLOW_AIR_LINK_QOS_STATE_MIN   = 0,
  DS707_FLOW_AIR_LINK_QOS_NULL        = DS707_FLOW_AIR_LINK_QOS_STATE_MIN,
  DS707_FLOW_AIR_LINK_QOS_REQUESTING  = 1,
  DS707_FLOW_AIR_LINK_QOS_CONFIGURED  = 2, /* AN sent GAUP accept to
                                              kkQoSRequest                 */
  DS707_FLOW_AIR_LINK_QOS_GRANTED     = 3,
  DS707_FLOW_AIR_LINK_QOS_SUSPENDED   = 4,

  DS707_FLOW_AIR_LINK_QOS_RELEASING   = 5,
  DS707_FLOW_AIR_LINK_QOS_RELEASED    = 6,
  DS707_FLOW_AIR_LINK_QOS_STATE_MAX
} ds707_flow_air_link_qos_state_enum_type;

/*---------------------------------------------------------------------------
  Enumeration for state of system as regards QoS
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_QOS_INVALID_QOS_STATE,
  DS707_QOS_SESSION_OPENED, /* HDR Session opened */
  DS707_QOS_SESSION_CLOSED, /* HDR Session closed */
  DS707_QOS_SESSION_PERSONALITY_SWITCHED /* Personality switch */
} ds707_sys_qos_state_enum_type;

/*---------------------------------------------------------------------------
  Enumeration of valid Config Types for the Sub blobs as specified by the
  standard,  both while building and receiving
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_PROFILE_CONFIG_TYPE_NONE = 0x00,
  DS707_PROFILE_CONFIG_TYPE_BUILD_SUB_BLOB = 0x01,
  DS707_PROFILE_CONFIG_TYPE_GRANTED = 0x03
} ds707_profile_config_type_enum_type;

/*---------------------------------------------------------------------------
  Parameter passed to qnm request status notification callback function
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_QOS_CB_NOT_SPECIFIED,
  DS707_QOS_GAUP_ACCEPTED,
  DS707_QOS_GAUP_REJECTED,
  DS707_QOS_GAUP_FAILED,
} ds707_qos_status_enum_type; // see if we can use this ... needed for PACQNM - protocol interaction


/*---------------------------------------------------------------------------
  Parameter passed by flow manager to identify the callback
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_QOS_REQUEST_ACCEPTED      = 0,  /* GAUP ACCEPTED */
  DS707_QOS_REQUEST_REJECTED      = 1,  /* GAUP REJECTED */
  DS707_QOS_REQUEST_TIMEDOUT      = 2,  /* GAUP TIMEDOUT */
  DS707_QOS_MODIFIED              = 3,  /* QOS MODIFIED */
  DS707_QOS_UNAWARE               = 4,  /* QOS UNAWARE */
  DS707_QOS_MODIFIED_TO_NULL      = 5,  /* QOS MODIFIED TO NULL */
  DS707_QOS_SETID_ZERO            = 6,  /* AN SENT SET ID ZERO */
  DS707_QOS_RELEASED              = 7,  /* QOS RELEASE REQUEST ACCEPTED */
  DS707_QOS_RESERVATION_REJECTED  = 8,  /* RESERVATION ON/OFF REQUEST REJECTED */
  DS707_QOS_RESERVATION_TIMED_OUT = 9,  /* RESERVATION ON/OFF REQUEST TIMED OUT */
  DS707_QOS_RESERVATION_ON_SENT   = 10, /* RESERVATION ON SENT SUCCESSFULLY */
} ds707_sec_pkt_flow_mgr_cb_enum_type; 

/*---------------------------------------------------------------------------
  Parameter passed by DQOS to identify the callback
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_DQOS_REQUEST_ACCEPTED,    
  DS707_DQOS_REQUEST_REJECTED,
  DS707_DQOS_REQUEST_TIMEDOUT
}ds707_sec_pkt_dqos_cb_enum_type;

/*---------------------------------------------------------------------------
  Parameter passed by PROTOCOL to identify the callback
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_SESSION_CHANGE,    
  DS707_RLP_CHANGE,
  DS707_RESERVATION_BINDING_CHANGE,
  DS707_ANSUP_QOS_PROFILE_CHANGE,
  DS707_NETWORK_ACQUIRE,
  DS707_ATTEMPT_TO_RESTORE_PRIOR_SESSION,
  DS707_PRIOR_SESSION_RESTORED
}ds707_sec_pkt_protocol_cb_enum_type;


/*---------------------------------------------------------------------------
TYPEDEF DS707_FLOW_DIRECTION_TYPE

DESCRIPTION
  Flow Direction: Forward,Reverse or birectional

  NOTE that these values must be bit masks. There are dependencies in DQOS
  code about these values being bit masks
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_DIRECTION_NONE          = 0x00,
  DS707_DIRECTION_REV           = 0x01,
  DS707_DIRECTION_FWD           = 0x02,
  DS707_DIRECTION_BIDIRECTIONAL = 0x03
} ds707_direction_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF DS707_FLOW_MODIFY_STATE_TYPE

DESCRIPTION
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_MODIFY_STATE_CLOSE                = 0,
  DS707_MODIFY_STATE_AQOS_MODIFYING       = 1,
  DS707_MODIFY_STATE_DQOS_MODIFYING       = 2,
  DS707_MODIFY_STATE_AQOS_DQOS_MODIFYING  = 3,
  DS707_MODIFY_STATE_WAIT_ON_ACCEPT       = 4,
  DS707_MODIFY_STATE_CLOSING              = 5
} ds707_flow_modify_state_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF HDRPACQNM_RESV_LABEL_TYPE

DESCRIPTION
  resv_label: 
  dirn: Direction associated with the the resv_label
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 resv_label;
  ds707_direction_enum_type dirn;
} ds707_resv_label_type;

/*---------------------------------------------------------------------------
TYPEDEF ds707_nw_init_3gpp2_obj

DESCRIPTION 
  The ds707_nw_init_3gpp2_obj and all its subcontents are used for
  network initiated QOS
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  set_length;
  uint8  set_id;
  uint8  verbose;
  uint16 flow_profile_id;
} ds707_qos_attribute_set;

typedef struct
{
  uint8                       flow_id;
  uint8                       sub_blob_len;
  uint8                       flow_priority;
  uint8                       num_attribute_sets;
  ds707_qos_attribute_set   * att_set_list;
  uint8                       result_code;
} ds707_qos_blob;

typedef enum
{
  IPV4_SRC_ADDR_MASK      = 1,
  IPV6_SRC_ADDR_MASK      = 2,
  IPV4_DEST_ADDR_MASK     = 3,
  IPV6_DEST_ADDR_MASK     = 4,
  PROTOCOL_NEXT_HDR_MASK  = 5,
  SINGLE_DEST_PRT_MASK    = 6,
  DEST_PRT_RANGE_MASK     = 7,
  SINGLE_SRC_PRT_MASK     = 8,
  SRC_PRT_RANGE_MASK      = 9,
  IPV4_TOS_MASK           = 10,
  IPV6_TRAFFIC_MASK       = 11,
  PROTOCOL_NOT_KNOWN      = 12
} ds707_qos_filter_contents_type_mask;


typedef struct
{
  uint16   type_mask;
  uint8  * value;
} ds707_qos_filter_contents_type;

typedef struct
{
  uint8                         flow_id;
  uint8                         evaluation_precedence;
  uint16                        length;
  uint8                         pf_type;
  uint8                         contents_length;
  uint8                         num_contents;
  ds707_qos_filter_contents_type * contents;
} ds707_packet_filter_struct;

typedef struct
{
  uint8                        type_mask;
  uint16                       length;
  uint32                       IPv6_addr[4];
  uint32                       IPv4_addr;
  uint8                        direction;
  uint8                        reserved_1;
  uint8                        ns_bit;
  uint8                        sr_id;
  uint8                        reserved_2;
  uint8                        p_bit;
  uint8                        operation_code;
  uint8                        num_packet_filters;
  ds707_packet_filter_struct * packet_filter_list;
  uint16                       qos_list_len;
  uint8                        num_qos_blobs;
  ds707_qos_blob             * qos_list;
  uint8                        num_packet_filters_mod_prec;
  ds707_packet_filter_struct * mod_prec_packet_filter_list;
} ds707_nw_init_qos_ie_type;

typedef struct 
{
  uint32                              length;
  uint32                              transaction_id;
  uint8                               class_num;
  uint8                               c_type;
  uint8                               num_ies;
  ds707_nw_init_qos_ie_type         * ie_list;
} ds707_nw_init_3gpp2_obj;


/*-----------------------------------------------------------------------------
  Indicate the operation is appliable for what kinds of flows.
  DS707_UE_INIT_APPLICABLE: only applicable for UE init flow
  DS707_NW_INIT_APPLICABLE: only applicable for NW init flow
  DS707_UE_AND_NW_APPLICABLE: applicable for both UE and NW init flows.
-----------------------------------------------------------------------------*/
typedef enum
{
  DS707_UE_INIT_APPLICABLE      = 0,
  DS707_NW_INIT_APPLICABLE      = 1,
  DS707_UE_AND_NW_APPLICABLE    = 2
}ds707_flow_applicable_type;

/*===========================================================================
      Variables
===========================================================================*/

#endif  /*  DS707_QOSDEFS_H */






