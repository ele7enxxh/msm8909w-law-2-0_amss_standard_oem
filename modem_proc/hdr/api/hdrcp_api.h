#ifndef HDRCP_API_H
#define HDRCP_API_H

/*===========================================================================
                    H D R    C P    D E F I N I T I O N

GENERAL DESCRIPTION
      This file contains hdr cp definitions

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/api/hdrcp_api.h#1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------- 
04/05/2016   vko     FR33944 : DLR backporting
12/10/2014   vko     External API support to retrieve PilotPN, Sector ID, 
                     Mac Index & DRC value
05/06/2013   cnx     Added hdrsmp_session_is_valid.
02/05/2013   ukl     Move more APIs to api header files.
04/24/2012   cnx     Moved more to SU API header files.
04/18/2012   smd     Added hdrcp_rx_queue_ind() 
04/06/2012   cnx     Force to close HPRD session after LTE attach.
02/24/2012   smd     Removed feature defines from SU API header files.
02/13/2012   smd     Added hdrcp_amp_send_irmi_msg. 
12/16/2011   wsh     Fixed/merged changes to make NI 3.0 build
12/05/2011   smd     Added two more QCHAT functions.
11/30/2011   smd     Added QCHAT functions.
10/24/2011   smd     Moved more to SU API header files.
09/10/2011   smd     Created this file.
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "sys.h"
#include "hdraddr.h"
#include "hdrcom_api.h"
#include "hdrl1_api.h"
#include "hdraddr.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* SUAPI_TODO: Will remove later */
#define hdris890_activate                     hdrcp_is890_activate
#define hdris890_deactivate                   hdrcp_is890_deactivate
#define hdris890_msg_cb                       hdrcp_is890_msg_cb


/* SUAPI_TODO: Will move to hdrcp header files later */
#define hdrslp_outcome_cb_type                hdrcp_slp_outcome_cb_type
#define hdrhmp_bundle_msg_enum_type           hdrcp_hmp_bundle_msg_enum_type
#define hdrhmp_bundle_preference_enum_type\
        hdrcp_hmp_bundle_preference_enum_type
#define hdridlec_slotted_mode_type            hdrcp_idlec_slotted_mode_type
#define hdrscmidle_sm_override_enum_type \
        hdrcp_scmidle_sm_override_enum_type
/*hdrscmrmac0.h*/
#define hdrscmrmac0_rate_params_mode_enum_type \
        hdrcp_scmrmac0_rate_params_mode_enum_type

/* hdrmrlpc.h*/
#define hdrmrlpc_supported_qos_record \
        hdrcp_mrlpc_supported_qos_record
#define hdrmrlpc_supported_qos_profiles \
        hdrcp_mrlpc_supported_qos_profiles
#define hdrmrlpc_flow_prot_id_type             hdrcp_mrlpc_flow_prot_id_type 
#define hdrmrlpc_prot_rohc_parm_fwd_type \
        hdrcp_mrlpc_prot_rohc_parm_fwd_type
#define hdrmrlpc_flow_prot_parm_fwd_type \
        hdrcp_mrlpc_flow_prot_parm_fwd_type
#define hdrmrlpc_prot_rohc_parm_rev_type \
        hdrcp_mrlpc_prot_rohc_parm_rev_type
#define hdrmrlpc_flow_prot_parm_rev_type \
        hdrcp_mrlpc_flow_prot_parm_rev_type
#define hdrmrlpc_ver_enum_type                hdrcp_mrlpc_ver_enum_type   
#define hdrmrlpc_flow_identification_type \
        hdrcp_mrlpc_flow_identification_type
#define hdrmrlpc_max_reservations_type \
        hdrcp_mrlpc_max_reservations_type
#define hdrmrlpc_flow_reservation_type        hdrcp_mrlpc_flow_reservation_type
#define hdrmrlpc_res_qos_profile_config_type \
        hdrcp_mrlpc_res_qos_profile_config_type
#define hdrmrlpc_route_prot_id_type           hdrcp_mrlpc_route_prot_id_type
#define hdrmrlpc_route_prot_parm_fwd_type  hdrcp_mrlpc_route_prot_parm_fwd_type
#define hdrmrlpc_route_prot_parm_rev_type  hdrcp_mrlpc_route_prot_parm_rev_type
#define hdrmrlpc_flow_sequence_length_type \
        hdrcp_mrlpc_flow_sequence_length_type
#define hdrmrlpc_attrib_name_enum_type     hdrcp_mrlpc_attrib_name_enum_type
#define hdrmrlpc_flow_timers_type          hdrcp_mrlpc_flow_timers_type 
#define hdrmrlpc_gaup_qos_request_type     hdrcp_mrlpc_gaup_qos_request_type
#define hdrmrlpc_res_qos_profile_config_type_enum_type \
        hdrcp_mrlpc_res_qos_profile_config_type_enum_type

#define HDRMRLPC_MAX_PROT_PROFILE_COUNT       HDRCP_MRLPC_MAX_PROT_PROFILE_COUNT
#define HDRMRLP_MAX_AT_SUP_PROFILE_LENGTH     HDRCP_MRLP_MAX_AT_SUP_PROFILE_LENGTH 
#define HDRMRLP_MAX_PROFILE_COUNT             HDRCP_MRLP_MAX_PROFILE_COUNT
#define HDRMRLP_MAX_PROFILE_LENGTH            HDRCP_MRLP_MAX_PROFILE_LENGTH
#define HDRMRLPC_VERNO_UNINIT                 HDRCP_MRLPC_VERNO_UNINIT
#define HDRMRLPC_NO_PREALOCATED_TRANSACTION_ID \
        HDRCP_MRLPC_NO_PREALOCATED_TRANSACTION_ID
/*hdrmrlpci.h*/
#define HDRMRLPC_N_RES_LABELS_MAX             HDRCP_MRLPC_N_RES_LABELS_MAX
#define HDRMRLPC_N_MRLP_MAX                   HDRCP_MRLPC_N_MRLP_MAX
#define HDRMRLPC_N_MRLP_MAX_FWD               HDRCP_MRLPC_N_MRLP_MAX_FWD
#define HDRMRLPC_N_MRLP_MAX_REV               HDRCP_MRLPC_N_MRLP_MAX_REV
#define HDRMRLPC_RES_LABELS_MAX_IDX           HDRCP_MRLPC_RES_LABELS_MAX_IDX 


/*hdr1xmc.h*/
#define hdr1xmc_location_type              hdrcp_1xmc_location_type

/*hdrpcp.h */
#define hdrpcp_put_data_func_type             hdrcp_pcp_put_data_func_type 
#define hdrpcp_eos_notification_func_type \
        hdrcp_pcp_eos_notification_func_type
#define hdrpcp_rx_buf_empty_notification_func_type \
        hdrcp_pcp_rx_buf_empty_notification_func_type
#define hdrpcp_get_data_size_func_type        hdrcp_pcp_get_data_size_func_type
#define hdrpcp_get_data_func_type             hdrcp_pcp_get_data_func_type
#define hdrpcp_tx_status_func_type            hdrcp_pcp_tx_status_func_type
#define hdrpcp_tx_failed_func_type            hdrcp_pcp_tx_failed_func_type

#define hdrpcp_carrier_tx_outcome_type        hdrcp_pcp_carrier_tx_outcome_type
#define HDRPCP_MAX_CARRIERS                   HDRCP_PCP_MAX_CARRIERS
#define HDRPCP_MAX_COMBOS                     HDRCP_PCP_MAX_COMBOS

#define HDRPCP_IDLE_PKT_HANDLE                HDRCP_PCP_IDLE_PKT_HANDLE
#define HDRPCP_LEGACY_PKT_HANDLE              HDRCP_PCP_LEGACY_PKT_HANDLE    
#define HDRPCP_FREE_FLOW_ENTRY                HDRCP_PCP_FREE_FLOW_ENTRY
#define HDRPCP_NO_COMBO_ID_ACCEPTED           HDRCP_PCP_NO_COMBO_ID_ACCEPTED    
#define HDRPCP_UNREGISTER_ALL_SUBSTREAMS      HDRCP_PCP_UNREGISTER_ALL_SUBSTREAMS
#define HDRPCP_INVALID_MAC_FLOW_ID            HDRCP_PCP_INVALID_MAC_FLOW_ID
#define HDRPCP_INVALID_ID                     HDRCP_PCP_INVALID_ID
#define HDRPCP_FMT_B_MAX_PKT_SIZE             HDRCP_PCP_FMT_B_MAX_PKT_SIZE
#define HDRPCP_FMT_B_PKT_HEADER_LEN           HDRCP_PCP_FMT_B_PKT_HEADER_LEN
#define HDRPCP_RMAC0_FLOW_ID                  HDRCP_PCP_RMAC0_FLOW_ID
#define HDRPCP_RMAC1_FLOW_ID                  HDRCP_PCP_RMAC1_FLOW_ID
#define HDRPCP_RMAC3_FLOW_ID                  HDRCP_PCP_RMAC3_FLOW_ID
#define HDRPCP_RMAC4_FLOW_ID                  HDRCP_PCP_RMAC4_FLOW_ID
/* hdrscp.h*/
#define HDRSCP_DEFAULT_PHYS_LAYER             HDRCP_SCP_DEFAULT_PHYS_LAYER
#define HDRSCP_SUB2_PHYS_LAYER                HDRCP_SCP_SUB2_PHYS_LAYER
#define HDRSCP_SUB3_PHYS_LAYER                HDRCP_SCP_SUB3_PHYS_LAYER
#define HDRSCP_DEFAULT_FMAC                   HDRCP_SCP_DEFAULT_FMAC              
#define HDRSCP_ENH_FMAC                       HDRCP_SCP_ENH_FMAC
#define HDRSCP_MC_FMAC                        HDRCP_SCP_MC_FMAC
#define HDRSCP_DEFAULT_RMAC                   HDRCP_SCP_DEFAULT_RMAC
#define HDRSCP_SUB1_RMAC                      HDRCP_SCP_SUB1_RMAC
#define HDRSCP_SUB3_RMAC                      HDRCP_SCP_SUB3_RMAC
#define HDRSCP_MC_RMAC                        HDRCP_SCP_MC_RMAC
#define HDRSCP_DEFAULT_CCMAC                  HDRCP_SCP_DEFAULT_CCMAC
#define HDRSCP_ENH_CCMAC                      HDRCP_SCP_ENH_CCMAC
#define HDRSCP_DEFAULT_ACMAC                  HDRCP_SCP_DEFAULT_ACMAC
#define HDRSCP_ENH_ACMAC                      HDRCP_SCP_ENH_ACMAC
#define HDRSCP_ENH_IDLE                       HDRCP_SCP_ENH_IDLE


#define HDRLUP_LOC_REQ_MSG                    HDRCP_LUP_LOC_REQ_MSG
#define HDRLUP_LOC_ASSIGN_MSG                 HDRCP_LUP_LOC_ASSIGN_MSG

#define HDRLUP_SNID_REQ_MSG                   HDRCP_LUP_SNID_REQ_MSG
#define HDRLUP_SNID_ASSIGN_MSG                HDRCP_LUP_SNID_ASSIGN_MSG

#define HDRDOS_DATA_OVER_SIGNALING_MSG_ID     HDRCP_DOS_DATA_OVER_SIGNALING_MSG_ID
#define HDRDOS_DATA_OVER_SIGNALING_ACK_MSG_ID \
        HDRCP_DOS_DATA_OVER_SIGNALING_ACK_MSG_ID


#define HDRFCP_XON_RSP_MSG_ID                 HDRCP_FCP_XON_RSP_MSG_ID
#define HDRFCP_XOFF_RSP_MSG_ID                HDRCP_FCP_XOFF_RSP_MSG_ID
#define HDRFCP_DATA_READY_MSG_ID              HDRCP_FCP_DATA_READY_MSG_ID 
/*-----------------------------------------------------------------------*/
/* Physical Layer protocol subtypes */
#define HDRCP_SCP_DEFAULT_PHYS_LAYER 0x0000U
#define HDRCP_SCP_SUB2_PHYS_LAYER 0x0002U
#define HDRCP_SCP_SUB3_PHYS_LAYER 0x0003U

/* MAC Layer protocols subtypes */
#define HDRCP_SCP_DEFAULT_FMAC 0x0000U
#define HDRCP_SCP_ENH_FMAC 0x0001U
#define HDRCP_SCP_MC_FMAC 0x0002U
#define HDRCP_SCP_DEFAULT_RMAC 0x0000U
#define HDRCP_SCP_SUB1_RMAC 0x0001U
#define HDRCP_SCP_SUB3_RMAC 0x0003U
#define HDRCP_SCP_MC_RMAC 0x0004U
#define HDRCP_SCP_DEFAULT_CCMAC 0x0000U
#define HDRCP_SCP_ENH_CCMAC 0x0001U
#define HDRCP_SCP_DEFAULT_ACMAC 0x0000U
#define HDRCP_SCP_ENH_ACMAC 0x0001U

#define HDRCP_SCP_ENH_IDLE 0x0001U



#define HDRCP_LUP_LOC_REQ_MSG    0x03
#define HDRCP_LUP_LOC_ASSIGN_MSG 0x05

#define HDRCP_LUP_SNID_REQ_MSG    0x10
#define HDRCP_LUP_SNID_ASSIGN_MSG 0x12


#define HDRCP_DOS_DATA_OVER_SIGNALING_MSG_ID      0x14
#define HDRCP_DOS_DATA_OVER_SIGNALING_ACK_MSG_ID  0x15


#define HDRCP_FCP_XON_RSP_MSG_ID         0x08
#define HDRCP_FCP_XOFF_RSP_MSG_ID        0x0A
#define HDRCP_FCP_DATA_READY_MSG_ID      0x0B


#define HDRCP_MRLPC_N_RES_LABELS_MAX 20 /* Maximum number of reservations allowed */
 
#define HDRCP_MRLPC_N_MRLP_MAX 32  /* Maximum total number of Activated and 
                                      Deactivated RLP Flows */
/* We bound the MAX to 32 for Fwd and Rev */
#define HDRCP_MRLPC_N_MRLP_MAX_FWD     HDRCP_MRLPC_N_MRLP_MAX 
#define HDRCP_MRLPC_N_MRLP_MAX_REV     HDRCP_MRLPC_N_MRLP_MAX 
#define HDRCP_MRLPC_RES_LABELS_MAX_IDX 0xFF /* Range of reservation labels allowed
                                            0-255 */

#define HDRCP_MRLPC_MAX_PROT_PROFILE_COUNT 8

/* Value for an uninitalized attribute version number */
#define HDRCP_MRLPC_VERNO_UNINIT 0

#define HDRCP_MRLPC_NO_PREALOCATED_TRANSACTION_ID  0xFF



#define HDRCP_MRLP_MAX_AT_SUP_PROFILE_LENGTH 2 
  /* The max profile length in octets.
     ANSupportedQoSProfiles and ATSupportedQoSProfiles 
     attribute are allowed only with ProfileType 0x02 (2 octets max). */

#define HDRCP_MRLP_MAX_PROFILE_COUNT 255


#define HDRCP_MRLP_MAX_PROFILE_LENGTH 64 
  /* The max profile length in octets. 
     Using 512/8.  IS835D length for 1x will restrict */

#define HDRCP_CAI_IRAT_MEAS 0x0a
  /* call end reason for IRAT measurement */

/*
 * The following two definitions should be defined in a 
 * general area that all modules have access to, rather
 * than in the PCP layer.
 *
 */
#define HDRCP_PCP_MAX_CARRIERS 3
#define HDRCP_PCP_MAX_COMBOS   4


#define HDRCP_PCP_IDLE_PKT_HANDLE         0x0000
#define HDRCP_PCP_LEGACY_PKT_HANDLE       0xFFFF
#define HDRCP_PCP_FREE_FLOW_ENTRY         0xFFFF
#define HDRCP_PCP_NO_COMBO_ID_ACCEPTED      0xFF
#define HDRCP_PCP_UNREGISTER_ALL_SUBSTREAMS 0xFF
#define HDRCP_PCP_INVALID_MAC_FLOW_ID       0xFF
#define HDRCP_PCP_INVALID_ID                0xFF


/*
 * Format B packets are at most 255 bytes, and the Format B
 * packet headers are 1 byte in length.
 *
 */
#define HDRCP_PCP_FMT_B_MAX_PKT_SIZE 255
#define HDRCP_PCP_FMT_B_PKT_HEADER_LEN 1

/*
 * If RMAC0 or RMAC1 is active, packet applications will
 * register their transmit callbacks with the following
 * MAC flow IDs.
 *
 */
#define HDRCP_PCP_RMAC0_FLOW_ID 0xFE
#define HDRCP_PCP_RMAC1_FLOW_ID 0xFD
#define HDRCP_PCP_RMAC3_FLOW_ID 0xFF
#define HDRCP_PCP_RMAC4_FLOW_ID 0xFF


                                         
/********** Callback function type **********/
typedef void (*hdrcp_slp_outcome_cb_type)( void*, hdrcom_errno_enum_type );


/* This ENUM is a bit mask i.e. each value must be a power of 2.
 * It is used to check if messages are bundled in same connection
 * layer packet.
 */
typedef enum
{
  HDRHMP_BUNDLE_NONE_MSG = 0,
  HDRHMP_BUNDLE_CR_MSG = 1,
  HDRHMP_BUNDLE_DOS_MSG = 2,
  HDRHMP_BUNDLE_ROR_MSG = 4,
} hdrcp_hmp_bundle_msg_enum_type;

/* This enum is used in external API called by DS/Application layer
* to restrict the bundling combinations*/
typedef enum
{
  HDRHMP_BUNDLE_NONE = HDRHMP_BUNDLE_NONE_MSG,        
    /* no bundle is needed */
  HDRHMP_BUNDLE_WITH_CR = HDRHMP_BUNDLE_CR_MSG,     
    /* bundle with CR message */
  HDRHMP_BUNDLE_WITH_CR_DOS = (HDRHMP_BUNDLE_CR_MSG|HDRHMP_BUNDLE_DOS_MSG), 
    /* bundle with CR and DOS messages */
} hdrcp_hmp_bundle_preference_enum_type;


typedef struct
{
  uint8 slot_cycle1;
  uint8 slot_cycle2;
  uint8 slot_cycle3;
  uint8 wake_count1;
  uint8 wake_count2;
} hdrcp_idlec_slotted_mode_type;


typedef enum
{
  HDRSCMIDLE_SM_OVERRIDE_NONE,      /* SlottedMode AT and AN configured */
  HDRSCMIDLE_SM_OVERRIDE_426_MSECS, /* SlottedMode override with SCI 426 ms */
  HDRSCMIDLE_SM_OVERRIDE_213_MSECS, /* SlottedMode override with SCI 213 ms */
  HDRSCMIDLE_SM_OVERRIDE_1280_MSECS, /* SlottedMode override with SCI 1280 ms */
  HDRSCMIDLE_SM_OVERRIDE_INVALID,
  HDRSCMIDLE_SM_MAX_NUM_SCIS
} hdrcp_scmidle_sm_override_enum_type;


/* Defines the Rate Parameters Mode (Normal or Video Telephony) */
typedef enum
{
  HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE = 0,     /* Normal Rate parameters */
  HDRSCMRMAC0_RATE_PARAMS_VT_MODE,             /* Video Telephony Rate params */
  HDRSCMRMAC0_NUM_RATE_PARAMS_MODES
} hdrcp_scmrmac0_rate_params_mode_enum_type;

/*
 * hdrpcp_put_data_func_type
 *
 * Callback to give received forward link data.
 *
 */
typedef void ( *hdrcp_pcp_put_data_func_type )
(
  dsm_item_type *rx_data_ptr,
  void *user_data_ptr
);

/*
 * hdrpcp_carrier_tx_outcome_type
 *
 * Struct used to provide the status of a particular tranmission,
 * giving the TX set id, the status (tx_set_accpeted) and the last RRI.
 *
 */
typedef struct
{
  void *tx_set_id;
  errno_enum_type tx_set_accepted;
  uint8 last_rri;
  uint8 carrier_id;

} hdrcp_pcp_carrier_tx_outcome_type;

/*
 * hdrpcp_eos_notification_type
 *
 * Callback to give the End-of-Slot notification.
 *
 */
typedef void ( *hdrcp_pcp_eos_notification_func_type )
(
  void *user_data_ptr
);


/*
 * hdrpcp_rx_buf_empty_notification_func_type
 *
 * Callback to give RX Buf empty notification.
 *
 */
typedef void ( *hdrcp_pcp_rx_buf_empty_notification_func_type )
(
  void *user_data_ptr
);

/*
 * hdrpcp_get_data_size_func_type
 *
 * Callback to determine the data pending for transmission.
 *
 */
typedef uint32 ( *hdrpcp_get_data_size_func_type )
(
  uint32 *num_pkts_pending,
  void *user_data_ptr
);

/*
 * hdrpcp_get_data_func_type
 *
 * Callback to get data for transmission.
 *
 */
typedef uint16 ( *hdrcp_pcp_get_data_func_type )
(
  uint8 combo_id,
  uint8 carrier_id,
  uint16 max_data_size,
  dsm_item_type **tx_data_ptr,
  void **tx_set_id,
  void *user_data_ptr
);

/*
 * hdrcp_pcp_tx_status_func_type
 *
 * Callback to report the status of the previous transmission.
 *
 */
typedef void ( *hdrcp_pcp_tx_status_func_type )
(
  uint8 combo_id,
  hdrcp_pcp_carrier_tx_outcome_type carrier_tx_outcome[],
  uint8 num_outcomes,
  void *user_data_ptr
);

/*
 * hdrcp_pcp_tx_failed_func_type
 *
 * Callback to give the M-ARQ indication.
 *
 */
typedef void ( *hdrcp_pcp_tx_failed_func_type )
(
  void *tx_set_id,
  void *user_data_ptr
);


/* HDR callback types */
typedef enum {
  HDRMC_CB_EVENT_SESSION_CLOSE,         // Callback when session closes
  HDRMC_CB_EVENT_UATI_UPDATE,           // Callback when UATI updates
  HDRMC_CB_EVENT_ENUM_MAX               // Number of available cb event
} hdrcp_mc_cb_event_enum_type;

/* SESSION CLOSE callback parameters */
typedef struct {
  hdrcp_mc_cb_event_enum_type            cb_type;
  hdrhai_session_close_reason_enum_type  close_reason;
} hdrcp_mc_cb_session_close_params_type;

/* UATI UPDATE callback parameters */
typedef struct {
  hdrcp_mc_cb_event_enum_type  cb_type;
  hdraddr_type                 full_uati;    // 128 bits UATI
} hdrcp_mc_cb_uati_update_params_type;

typedef union {
  hdrcp_mc_cb_event_enum_type           cb_type;  // Used to check the event type 
  hdrcp_mc_cb_session_close_params_type session_close;
  hdrcp_mc_cb_uati_update_params_type   uati_update;
} hdrcp_mc_cb_params_rsp_union_type;

/* Type for a hdr callback function. A client must register for that function 
 * with HDR if it wants to be notified. QMI_NAS has to use this new Type
 */
typedef void (*hdrcp_mc_cb_f_type) ( 
  hdrcp_mc_cb_params_rsp_union_type *cb_data_ptr,
  void                           *client_data_ptr 
);

/* Registration enum type */
typedef enum {
  HDRMC_CB_REG,                   // Registration
  HDRMC_CB_DEREG                  // Deregistration
} hdrcp_mc_cb_reg_enum_type;

/* Client enum type */
typedef enum {
  HDRMC_CB_CLIENT_QMI,            // QMI
  HDRMC_CB_CLIENT_MAX             // Total number of cb clients
} hdrcp_mc_cb_client_id_enum_type;

typedef struct
{
  
  uint8 profile_type;
  uint8 profile_length;
  uint8 profile_value[HDRMRLP_MAX_AT_SUP_PROFILE_LENGTH];

} hdrcp_mrlpc_supported_qos_record;

typedef struct
{
  uint8 profile_count;
  hdrcp_mrlpc_supported_qos_record profile[HDRMRLP_MAX_PROFILE_COUNT];
} hdrcp_mrlpc_supported_qos_profiles;


/* Flow Protocol ID type, see Table 2.9.2.15-1 */
typedef enum
{
  HDRMRLPC_FLOW_PROT_ID_NULL      = 0,
  HDRMRLPC_FLOW_PROT_ID_HDLC      = 1,
  HDRMRLPC_FLOW_PROT_ID_IPV4      = 2,
  HDRMRLPC_FLOW_PROT_ID_IPV6      = 3,
  HDRMRLPC_FLOW_PROT_ID_ROHC      = 4,
  HDRMRLPC_FLOW_PROT_ID_IPV4_IPV6 = 5,  /* defined in TSB58 */
  HDRMRLPC_FLOW_PROT_ID_RSV1      = 6,
  HDRMRLPC_FLOW_PROT_ID_VSNP      = 7,
  HDRMRLPC_FLOW_PROT_ID_PDNMUX    = 8,
  HDRMRLPC_FLOW_PROT_ID_MAX

} hdrcp_mrlpc_flow_prot_id_type;

/* Definition of protocol Parms Fwd when Flow Protocol 
 * or Route Protocol is ROHC - 2.9.2.15.1 */
typedef struct
{
  uint16            max_cid;    /* Max CID for this ROHC channel */
  boolean           large_cids; 
  boolean           feed_back_for_incl; 
    /* If ROHC feedback associated with another link flow (ROHC 
     * channel) is sent on this link flow (ROHC channel) */
  uint8             feed_back_for;
    /* The link flow number (ROHC channel) to which ROHC feedback
     * sent on this link flow (ROHC chanel) refers */
  uint16            mrru;
  uint8             profile_count;  /* Number of ROHC profiles sup */
  uint16            profile[HDRCP_MRLPC_MAX_PROT_PROFILE_COUNT];  

} hdrcp_mrlpc_prot_rohc_parm_fwd_type;


/* Flow NN Flow Protocol Parameters Fwd Attributes */
typedef struct hdrcp_mrlpc_flow_prot_parm_fwd
{
  hdrcp_mrlpc_flow_prot_id_type  id;    /* Protocol ID */
  uint8                       length;

  union
  {
    hdrcp_mrlpc_prot_rohc_parm_fwd_type  rohc;
    /*~ DEFAULT hdrmrlpc_flow_prot_parm_fwd.parm.rohc */
  } parm;
  /*~ FIELD hdrmrlpc_flow_prot_parm_fwd.parm DISC 
                              hdrmrlpc_flow_prot_parm_fwd.id */

} hdrcp_mrlpc_flow_prot_parm_fwd_type;


/* Definition of protocol Parms Rev when Flow Protocol 
 * or Route Protocol is ROHC - 2.9.2.16.1 */
typedef struct
{
  uint16            max_cid;    /* Max CID for this ROHC channel */
  boolean           large_cids; 
  boolean           feed_back_for_incl; 
    /* If ROHC feedback associated with another link flow (ROHC 
     * channel) is sent on this link flow (ROHC channel) */
  uint8             feed_back_for;
    /* The link flow number (ROHC channel) to which ROHC feedback
     * sent on this link flow (ROHC chanel) refers */
  uint16            mrru;
  boolean           timer_based_comp; 
    /* Timer based compression is enabled or not */
  uint8             profile_count;  /* Number of ROHC profiles sup */
  uint16            profile[HDRMRLPC_MAX_PROT_PROFILE_COUNT];

} hdrcp_mrlpc_prot_rohc_parm_rev_type;

/* Flow NN Flow Protocol Parameters Rev Attributes */
typedef struct hdrcp_mrlpc_flow_prot_parm_rev
{
  hdrcp_mrlpc_flow_prot_id_type  id;    /* Protocol ID */
  uint8                       length;

  union
  {
    hdrcp_mrlpc_prot_rohc_parm_rev_type  rohc;
    /*~ DEFAULT hdrmrlpc_flow_prot_parm_rev.parm.rohc */
  } parm;
  /*~ FIELD hdrmrlpc_flow_prot_parm_rev.parm 
                               DISC hdrmrlpc_flow_prot_parm_rev.id */

} hdrcp_mrlpc_flow_prot_parm_rev_type;

/* RLP version (forward or reverse) */
typedef enum
{
  HDRMRLPC_FWD_RLP = 0,
  HDRMRLPC_REV_RLP = 1,

  HDRMRLPC_INVALID = 0xFF
} hdrcp_mrlpc_ver_enum_type;

typedef struct
{
  uint8             is_active;   /* Is the RLP Active or not */
  uint8             seq_len;     /* Size of Sequence space in units of bits */
  uint8             rlp_id_len;  /* Length, in units of bits of the rlp_id */
  uint8             rlp_id;      /* Identifier for this RLP flow. Data pkts are tagged with this */
}hdrcp_mrlpc_flow_identification_type;

/* Max Reservation attribute */
typedef struct
{
  uint8 max_num_rsv_fwd;
  uint8 max_num_rsv_rev;
  uint8 max_num_open_rsv_fwd;
  uint8 max_num_open_rsv_rev;
} hdrcp_mrlpc_max_reservations_type;

typedef struct
{
  uint8             rsv_count;       /* Num of reservervations associated with this RLP */
  uint8             rsv_label[HDRCP_MRLPC_N_RES_LABELS_MAX]; /* Reservations associated with the RLP */
}hdrcp_mrlpc_flow_reservation_type;


typedef struct
{
  uint8             type;
  uint16            length;
  uint8             value[HDRCP_MRLP_MAX_PROFILE_LENGTH];
} hdrcp_mrlpc_res_qos_profile_config_type;


/*---------------------------------------------------------------------------
  Flow NN Flow / Route Protocol Parameters attribute
---------------------------------------------------------------------------*/

/* Route Protocol ID type, see Table 2.9.2.17-1 */
typedef enum
{
  HDRMRLPC_ROUTE_PROT_ID_NULL  = 0,
  HDRMRLPC_ROUTE_PROT_ID_RSV1  = 1,   /* Reserved Protocol ID */
  HDRMRLPC_ROUTE_PROT_ID_RSV2  = 2,   /* Reserved Protocol ID */
  HDRMRLPC_ROUTE_PROT_ID_RSV3  = 3,   /* Reserved Protocol ID */
  HDRMRLPC_ROUTE_PROT_ID_ROHC  = 4,
  HDRMRLPC_ROUTE_PROT_ID_RSV4  = 5,
  HDRMRLPC_ROUTE_PROT_ID_RSV5  = 6,
  HDRMRLPC_ROUTE_PROT_ID_RSV6  = 7,
  HDRMRLPC_ROUTE_PROT_ID_RSV7  = 8,
  HDRMRLPC_ROUTE_PROT_ID_MAX

} hdrcp_mrlpc_route_prot_id_type;

/*  Flow NN Route Protocol Parameters Fwd attribute */
typedef struct hdrcp_mrlpc_route_prot_parm_fwd
{
  hdrcp_mrlpc_route_prot_id_type   id;    /* Protocol ID */
  uint8                         length;

  union
  {
    hdrcp_mrlpc_prot_rohc_parm_fwd_type  rohc;
    /*~ DEFAULT hdrmrlpc_route_prot_parm_fwd.parm.rohc */
  } parm;
  /*~ FIELD hdrmrlpc_route_prot_parm_fwd.parm 
                               DISC hdrmrlpc_route_prot_parm_fwd.id */

} hdrcp_mrlpc_route_prot_parm_fwd_type;


/*  Flow NN Route Protocol Parameters Rev attribute */
typedef struct hdrcp_mrlpc_route_prot_parm_rev
{
  hdrcp_mrlpc_route_prot_id_type   id;    /* Protocol ID */
  uint8                         length;

  union
  {
    hdrcp_mrlpc_prot_rohc_parm_rev_type  rohc;
    /*~ DEFAULT hdrmrlpc_route_prot_parm_rev.parm.rohc */
  } parm;
  /*~ FIELD hdrmrlpc_route_prot_parm_rev.parm 
                                DISC hdrmrlpc_route_prot_parm_rev.id */

} hdrcp_mrlpc_route_prot_parm_rev_type;


typedef struct
{
  uint8             sar_seq_len; 
  uint8             sar_seq_len_short;
}hdrcp_mrlpc_flow_sequence_length_type;


/* List of all MRLPC attribute types */
typedef enum
{

  /*=====================*
   *  Simple Attributes  *
   *=====================*/
  HDRMRLPC_ATTRIB_MAX_ABORT_TIMER,  
    /* 0xFFFC - MaxAbortTimer                   */

  HDRMRLPC_ATTRIB_PPP_FREE_AUTHENTICATION_SUPPORT,
    /* 0xFFFB - PPPFreeAuthenticationSupport    */

  HDRMRLPC_ATTRIB_TWO_ROUTES_SUPPORTED,
    /* 0xFFFA - TwoRoutesSupported              */

  HDRMRLPC_ATTRIB_AT_NAK_DELAY_SUPPORTED,
    /* 0xFFF9 - NakDelaySupported               */

  HDRMRLPC_ATTRIB_NAK_EN_FWD,  
    /* 0xFENN - FlowNNNakEnableFwd              */

  HDRMRLPC_ATTRIB_NAK_EN_REV,  
    /* 0xFDNN - FlowNNNakEnableRev              */

  HDRMRLPC_ATTRIB_HIGH_LAYER_PROT_FWD,  
    /* 0xFCNN - FlowNNHigherLayerProtocolFwd    */

  HDRMRLPC_ATTRIB_HIGH_LAYER_PROT_REV,  
    /* 0xFBNN - FlowNNHigherLayerProtocolRev    */

  HDRMRLPC_ATTRIB_PHYS_LAYER_NAK_REV,  
    /* 0xFANN - FlowNNPhysicalLayerNakEnableRev */

  HDRMRLPC_ATTRIB_RESKK_IDLE_STATE_FWD,
    /* 0xF9NN - ReservationKKIdleStateFwd       */

  HDRMRLPC_ATTRIB_RESKK_IDLE_STATE_REV,
    /* 0xF8NN - ReservationKKIdleStateRev       */

  HDRMRLPC_ATTRIB_TRANSMIT_ABORT_TIMER_REV,
    /* 0xF7NN - FlowNNTransmitAbortTimerRev     */

  HDRMRLPC_ATTRIB_DATA_OVER_SIG_ALLOWED_REV,
    /* 0xF6NN - FlowNNDataOverSignalingAllowedRev  */

  HDRMRLPC_ATTRIB_ACTIVATED_FWD,
    /* 0xF3NN - FlowNNAckEnableRev                 */

  HDRMRLPC_ATTRIB_ACTIVATED_REV,
    /* 0xF2NN - FlowNNActivatedFwd                 */

  HDRMRLPC_ATTRIB_SIMPLE_SEQUENCE_LENGTH_FWD,
    /* 0xF1NN - FlowNNSequenceLengthFwd (EMPA)     */

  HDRMRLPC_ATTRIB_SEQUENCE_LENGTH_REV,
    /* 0xF0NN - FlowNNSequenceLengthRev            */

  HDRMRLPC_ATTRIB_FLOW_PROT_SDU_FWD,
    /* 0xEFNN - FlowNNFlowProtocolSDUFwd           */

  HDRMRLPC_ATTRIB_FLOW_PROT_SDU_REV,
    /* 0xEENN - FlowNNFlowProtocolSDURev           */

  HDRMRLPC_ATTRIB_DATA_UNIT_FWD,
    /* 0xEDNN - FlowNNDataUnitFwd                  */

  HDRMRLPC_ATTRIB_DATA_UNIT_REV,
    /* 0xECNN - FlowNNDataUnitRev                  */

  HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_FWD,
    /* 0xEBNN - FlowNNRouteProtocolSDUFwd          */

  HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_REV,
    /* 0xEANN - FlowNNRouteProtocolSDURev          */

  HDRMRLPC_ATTRIB_SIM_DEL_BOTH_ROUTES_FWD,
    /* 0xE9NN - FlowNNSimultaneousDeliveryOnBothRoutesFwd */

  HDRMRLPC_ATTRIB_OOO_DEL_ROUTE_PROT_FWD,
    /* 0xE8NN - FlowNNOutOfOrderDeliveryToRouteProtocolFwd */

  HDRMRLPC_ATTRIB_RLP_PRIORITY_FWD,
    /* RLP Priority Fwd. Not defined in the std */

  HDRMRLPC_ATTRIB_RLP_PRIORITY_REV,
    /* RLP Priority Rev. Not defined in the std */

  HDRMRLPC_ATTRIB_FLOW_NAK_DELAY_TIME_FWD,
    /* 0xE7NN - FlowNNNakDelayTimeFwd */

  /*=====================*
   * Complex Attributes  *
   *=====================*/
  HDRMRLPC_ATTRIB_IDENTIFICATION_FWD,
    /* 0x01NN - FlowNNIdentificationFwd         */

  HDRMRLPC_ATTRIB_IDENTIFICATION_REV,
    /* 0x02NN - FlowNNIdentificationRev         */

  HDRMRLPC_ATTRIB_COMPLEX_SEQUENCE_LENGTH_FWD,
    /* 0x02NN - FlowNNSequenceLengthFwd (MMPA ) */

  HDRMRLPC_ATTRIB_TIMERS_FWD,
    /* 0x03NN - FlowNNTimersFwd                 */

  HDRMRLPC_ATTRIB_TIMERS_REV,
    /* 0x04NN - FlowNNTimersRev                 */

  HDRMRLPC_ATTRIB_RESERV_FWD,     
    /* 0x05NN - FlowNNReservationFwd            */

  HDRMRLPC_ATTRIB_RESERV_REV,
    /* 0x06NN - FlowNNReservationRev            */

  HDRMRLPC_ATTRIB_RESKK_QOS_REQ_FWD,
    /* 0x07NN - ReservationKKQoSRequestFwd      */

  HDRMRLPC_ATTRIB_RESKK_QOS_REQ_REV,
    /* 0x08NN - ReservationKKQosRequestRev      */
  
  HDRMRLPC_ATTRIB_RESKK_QOS_RSP_FWD,
    /* 0x09NN - ReservationKKQoSResponseFwd     */

  HDRMRLPC_ATTRIB_RESKK_QOS_RSP_REV,
    /* 0x0ANN - ReservationKKQosResponseRev     */

  HDRMRLPC_ATTRIB_FLOW_PROT_PARM_FWD,
    /* 0x0BNN - FlowNNFlowProtocolParametersFwd */

  HDRMRLPC_ATTRIB_FLOW_PROT_PARM_REV,
    /* 0x0CNN - FlowNNFlowProtocolParametersRev */

  HDRMRLPC_ATTRIB_ROUTE_PROT_PARM_FWD,
    /* 0x0DPP/NN - FlowNNRouteProtocolParametersFwd */

  HDRMRLPC_ATTRIB_ROUTE_PROT_PARM_REV,
    /* 0x0EPP/NN - FlowNNRouteProtocolParametersRev */

  HDRMRLPC_ATTRIB_AT_SUP_FLOW_PROT_PARM,
    /* 0x0FPP - ATSupportedFlowProtocolParametersPP */

  HDRMRLPC_ATTRIB_AT_SUP_ROUTE_PROT_PARM,
    /* 0x10PP - ATSupportedRouteProtocolParametersPP */

  HDRMRLPC_ATTRIB_SUP_HIGH_LAYER_PROT,
    /* 0x0000 - SupportedHigherLayerProtocols   */

  HDRMRLPC_ATTRIB_AT_SUP_QOS_PROFILE,
    /* 0x0002 - AT Supported QoS Profiles       */

  HDRMRLPC_ATTRIB_AN_SUP_QOS_PROFILE,
    /* 0x0003 - AN Supported QoS Profiles       */

  HDRMRLPC_ATTRIB_MAX_RLP_FLOWS,
    /* 0x0004 - Max RLP Flows                   */

  HDRMRLPC_ATTRIB_MAX_RESERVATIONS,
    /* 0x0005 - Max Reservations                */

  HDRMRLPC_ATTRIB_MAX
    /* Maximum number of Attributes */

}hdrcp_mrlpc_attrib_name_enum_type;

typedef struct
{
  uint16            abort_timer_val; /* Value of the abort timer (in units of msec). Not needed for Reverse RLP (transmitter) */
  uint16            flush_timer_val; /* Value of the flush timer (in units of msec). Not needed for Forward RLP (Receiver) */
  /* This new field is defined only for MMPA, for EMPA/MFPA a new attribute 
     has been backported (HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID) */  
  uint16            nak_delay_timer_val;
}hdrcp_mrlpc_flow_timers_type;


/* GAUP ReservationKKQoSRequest type for GAUP interface
  _mpa_ pull out of hdrmrlpc.h into other interface header */
typedef struct
{
  uint8   reservation_label;
  hdrcp_mrlpc_ver_enum_type direction;
  hdrcp_mrlpc_res_qos_profile_config_type qos_profile;
} hdrcp_mrlpc_gaup_qos_request_type;


/*---------------------------------------------------------------------------
  Enumeration of valid Config Types for the Sub blobs for Reservation QoS
  Profile , both while building
  and receiving
---------------------------------------------------------------------------*/
typedef enum
{
  HDRMRLPC_RES_QOS_PROFILE_CONFIG_TYPE_NONE = 0x00,
  HDRMRLPC_RES_QOS_PROFILE_CONFIG_TYPE_BUILD_SUB_BLOB = 0x01,
  HDRMRLPC_RES_QOS_PROFILE_CONFIG_TYPE_GRANTED = 0x03
} hdrcp_mrlpc_res_qos_profile_config_type_enum_type;


/* Holds the HDR Location Information in a type compatible with IS2000-A*/
typedef struct
{
   uint16 sid;
   uint16 nid;
   uint8  pzid;
} hdrcp_1xmc_location_type;

typedef struct
{
   boolean is_sector_id_valid;
   hdraddr_type sector_id; /* sector ID */
   boolean is_pilot_pn_valid;
   sys_pilot_pn_type pilot_pn; /* Pilot PN */
   boolean is_mac_index_valid;
   uint16 mac_index; /* Mac Index */
}hdrmc_sid_pn_mac_info;

typedef struct
{
   boolean is_drc_rate_valid; /* Serving DRC rate */
   uint8 drc_rate;
}hdrmc_serv_drc_rate_info;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*===========================================================================

FUNCTION HDRCP_IS890_ACTIVATE

DESCRIPTION
  This is a a wrap function. It is called by PAC when the enhanced test 
  application is negotiated on any stream.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id  - Stream ID on which IS890A is negotiated 

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrcp_is890_activate
( 
  uint8 stream_id 
    /* the stream id at which the ETAP is negotiated */
);

/*===========================================================================

FUNCTION HDRCP_IS890_DEACTIVATE

DESCRIPTION
  This is a wrapper function. It is called when the session is closed.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id  - Stream ID on which is890a is assigned

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrcp_is890_deactivate
( 
  uint8 stream_id 
    /* The stream ID at which the is890a was negotiated */
);

/*===========================================================================

FUNCTION HDRCP_IS890_MSG_CB

DESCRIPTION
  This function is staticly registered to be called whenever HMP receives a
  message for is890a .

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrcp_is890_msg_cb
(
  dsm_item_type* msg_ptr
    /* the ptr to the DSM item containing the incoming msg */
);

/*===========================================================================

FUNCTION HDRCP_MC_QUEUE_IND

DESCRIPTION
  This is a wrapper function. 

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  an indication to protocols in the MC task at the same time.

  ind_data_size must not be larger than hdrind_ind_data_union_type, because
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  input_ind_ptr - Pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_mc_queue_ind( hdrcom_hai_protocol_name_enum_type protocol_name,
          hdrcom_ind_name_enum_type ind_name,
          void *input_ind_data_ptr
   );

/*===========================================================================

FUNCTION HDRCP_MC_QUEUE_CMD

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  commands to protocols in the MC task at the same time.

  cmd_size must not be larger than hdrmc_protocol_cmd_union_type, because
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the command is for
  input_cmd_ptr - Pointer to a union of all protocols in HDRMC task
  cmd_size - Size of the command being given

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_mc_queue_cmd( hdrcom_hai_protocol_name_enum_type protocol_name,
                          void * input_cmd_ptr, uint32 cmd_size );

/*===========================================================================

FUNCTION HDRCP_MC_QUEUE_MSG

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the message is for
  protocol_instance - Instance of the protocol the message is for
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_mc_queue_msg( hdrcom_hai_protocol_name_enum_type protocol_name,
                      hdrcom_hai_protocol_instance_enum_type protocol_instance,
                      dsm_item_type * item_ptr );
/*===========================================================================
FUNCTION HDRCP_MC_GET_STATS_INFO 

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  ftd_stats_ptr - Pointer to location where the FTD stats need to be copied.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrcp_mc_get_stats_info 
(
  sys_hdr_information_s_type *ftd_stats_ptr
);

/*===========================================================================

FUNCTION HDRCP_DOS_MSG_CB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrcp_dos_msg_cb 
( 
  dsm_item_type *msg_ptr
);
/*===========================================================================

FUNCTION HDRCP_FCP_MSG_CB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  msg_ptr -  Received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_fcp_msg_cb
(
  dsm_item_type *msg_ptr
);

/*===========================================================================

FUNCTION HDRCP_FCP_STREAM_IS_OPEN

DESCRIPTION
  This is a wrapper function
 
DEPENDENCIES
  None
  
PARAMETERS
  stream_id - Stream being queried.

RETURN VALUE
  TRUE - If the stream state is FCP_OPEN and no Xoff message has been sent or
         is being sent.
  FALSE - If the stream state is FCP_CLOSED or if a Xoff message has been sent
         or is being sent.
  
SIDE EFFECTS
  None
===========================================================================*/
boolean hdrcp_fcp_stream_is_open( hdrcom_hai_stream_enum_type stream_id );

/*===========================================================================

FUNCTION HDRCP_FCP_SEND_XOFF_REQ

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None
  
PARAMETERS
  sender - protocol calling this function
  stream_id - Stream for which this message applies to.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrcp_fcp_send_xoff_req
(
  hdrcom_hai_protocol_name_enum_type sender ,
  hdrcom_hai_stream_enum_type stream_id 
);

/*===========================================================================

FUNCTION HDRCP_FCP_SEND_XON_REQ

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None
  
PARAMETERS
  sender - protocol calling this function
  stream_id - Stream for which this message applies to.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrcp_fcp_send_xon_req
(
  hdrhai_protocol_name_enum_type sender,
  hdrhai_stream_enum_type stream_id 
);
/*===========================================================================

FUNCTION HDRCP_HMP_APP_SEND_MSG

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  protocol_name - Name of Protocol sending the message.
  stream - Stream associated with protocol
  chan - Channel the message must go out on (AC or RTC)
  rtc_is_reliable - Whether or not the message is reliable
  priority - Priority of the message, as defined in the HAI
  *msg_ptr - The message to be sent
  outcome_cb - Pointer to function called to announce outcome of
                    transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_hmp_app_send_msg
(
  hdrcom_hai_protocol_name_enum_type name,
  hdrcom_hai_stream_enum_type stream,
  hdrcom_hai_channel_enum_type chan,
  boolean rtc_is_reliable,
  uint8 priority,
  hdrcom_hai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * msg_ptr,
  hdrcp_slp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
);


/*===========================================================================

FUNCTION HDRCP_HMP_APP_SEND_RASM

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  name            - Name of Protocol sending the message
  stream          - Stream associated with protocol
  max_retries     - How many times to try sending the best effort msg
  retry_interval  - Time to wait for layer 3 ACK before retry, In milliseconds
  transaction_id  - For app to inform HMP that layer 3 ACK was received
  *msg_ptr        - The message to be sent
  outcome_cb      - Pointer to function called to announce outcome of
                   transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_hmp_app_send_rasm
(
  hdrcom_hai_protocol_name_enum_type name,
  hdrcom_hai_stream_enum_type stream,
  uint32 max_retries,
  uint32 retry_interval,
  uint32 transaction_id,
  dsm_item_type * msg_ptr,
  hdrcp_slp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
);


/*===========================================================================

FUNCTION HDRCP_HMP_APP_SEND_RASM_QOS_RESUME

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None

PARAMETERS
  name            - Name of Protocol sending the message
  stream          - Stream associated with protocol
  max_retries     - How many times to retry the best effort msg if no layer3 Ack
  retry_interval  - Time to wait for layer 3 ACK before retry, In milliseconds
  transaction_id  - For app to inform HMP that layer 3 ACK was received
  bundle_mask     - message bundle mask. 
  is_implicit_on_connection_setup - TRUE if ReservationKKIdleState == 2, FALSE otherwise
                                    if this argument is set to TRUE,
                                    if bundle_mask is set to BUNDLE_NONE, it 
                                    needs to be set to BUNDLE_WITH_CR 
  *msg_ptr        - The message to be sent
  outcome_cb      - Pointer to function called to announce outcome of
                    transmission
  *outcome_tag_ptr - A pointer owned by sender, returned with the callback

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_hmp_app_send_rasm_qos_resume
(
  hdrcom_hai_protocol_name_enum_type name,
  hdrcom_hai_stream_enum_type stream,
  uint32 max_retries,
  uint32 retry_interval,
  uint32 transaction_id,
  hdrcp_hmp_bundle_preference_enum_type bundle_preference,
  boolean is_implicit_on_connection_setup,
  dsm_item_type * msg_ptr,
  hdrcp_slp_outcome_cb_type outcome_cb,
  void * outcome_tag_ptr
);

/*===========================================================================

FUNCTION HDRCP_HMP_RASM_ACKED

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  Each protocol with a unique hdrhai_protocol_name_enum_type must ensure the 
  transaction_id is unique.  In other words, two outstanding best effort
  messages (regardless of MessageID) sent by the same protocol with retries 
  requested should not have the same transaction_id.

PARAMETERS
  protocol_name  - Name of Protocol sending the message.
  transaction_id - For app to inform HMP that layer 3 ACK was received.
                   TransactionID is per protocol, not per message!
RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_hmp_rasm_acked
(
  hdrcom_hai_protocol_name_enum_type name,
  uint32 transaction_id
);

/*===========================================================================

FUNCTION HDRCP_HMP_SET_AN_AUTH_STATUS
DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  auth_succeeded - TRUE if CHAP authentication passed, FALSE if it failed.
  caller - The protocol calling this function, for debug

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrcp_hmp_set_an_auth_status
(
  boolean auth_succeeded,
  hdrcom_hai_protocol_name_enum_type caller
);
/*===========================================================================

FUNCTION HDRCP_LUP_MSG_CB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrcp_lup_msg_cb (dsm_item_type * msg_ptr);


/*===========================================================================

FUNCTION HDRCP_RUP_CONTINUE_CONNECTION_SETUP

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_rup_continue_connection_setup
(
  hdrcom_hai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRCP_SMP_DEACTIVATE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  close_is_silent - TRUE if the session should be closed silently
                    FALSE if the session should be closed explicitely
                 (i.e. by sending a SessionClose message to the AN first)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_smp_deactivate
(
  hdrcom_hai_protocol_name_enum_type sender,
  boolean close_is_silent
);


/*===========================================================================

FUNCTION HDRCP_ALMP_OPEN_CONNECTION

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  open_reason - The reason the connection is being opened
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_almp_open_connection
(
  hdrcom_hai_open_reason_enum_type open_reason,
  hdrcom_hai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRCP_ALMP_CLOSE_CONNECTION

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcp_almp_close_connection
(
  hdrcom_hai_abort_reason_enum_type reason,
  hdrcom_hai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRCP_ALMP_SYSTEM_IS_ACQUIRED

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a system is acquired, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrcp_almp_system_is_acquired( hdrcom_hai_protocol_name_enum_type caller );

/*===========================================================================

FUNCTION HDRCP_IDLE_SET_PKT_DATA_SESS_STATUS

DESCRIPTION 
  This is a wrapper function. 
  
DEPENDENCIES
  None.

PARAMETERS
  is_session_on_hdr - TRUE if whether data session is on HDR or 1x
                    - FALSE otherwise

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_idle_set_pkt_data_sess_status(boolean is_session_on_hdr);


/*===========================================================================

FUNCTION HDRCP_IDLE_SET_FORCE_LONG_SLEEP

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  force_long_sleep - turn on/off force_long_sleep flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_idle_set_force_long_sleep
(
  boolean force_long_sleep
);

/*===========================================================================

FUNCTION HDRCP_IDLE_GET_FORCE_LONG_SLEEP

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - force_long_sleep flag is set. AT uses 40 seconds as sleep period in
         SLEEP state. 
  FALSE - force_long_sleep flag is not set.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_idle_get_force_long_sleep( void );

/*===========================================================================

FUNCTION HDRCP_IDLEC_GET_SLOTTED_MODE_ATTRIB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_idlec_get_slotted_mode_attrib
( 
  hdrcp_idlec_slotted_mode_type *slotted_mode_attrib_ptr
);

/*===========================================================================
FUNCTION HDRCP_RUP_SET_TC_HANDDOWN_MODE

DESCRIPTION  : This s a wrapper function. 
 
DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrcp_rup_set_tc_handdown_mode
( 
  hdrl1_srch_tc_handdown_mode_enum_type handdown_mode,
  hdrcom_hai_protocol_name_enum_type sender
);

/*===========================================================================
FUNCTION HDRCP_RUP_SET_TRAFFIC_MODE

DESCRIPTION
  This function sets the HDR traffic mode.

DEPENDENCIES
  None.

PARAMETERS
  traffic_mode - The HDRSRCH traffic mode to allow applications to tweak,
                 for example, specifics of SHDR operation while connected.
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrcp_rup_set_traffic_mode
( 
  hdrl1_srch_tc_mode_enum_type traffic_mode,
  hdrcom_hai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRCP_SCMIDLE_GAUP_SLOTTED_MODE_ATTRIB

DESCRIPTION
  This function is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  sm_override -  What type of override to start:
                 HDRSCMIDLE_SM_OVERRIDE_NONE
                 HDRSCMIDLE_SM_OVERRIDE_426_MSECS
                 HDRSCMIDLE_SM_OVERRIDE_213_MSECS

  p_trans_id - Address where the transaction ID is stored to be returned back.

RETURN VALUE
  E_SUCCESS - If a GAUP Update was queued to HDRMC successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer to send this GAUP update.
  E_FAILURE - If attempting to turn off override when no stored config is 
              available i.e. no override was done previously.
              AT couldn't retrieve its current slotted mode config.
              sm_override out of range.

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_scmidle_gaup_slotted_mode_attrib
(
  hdrcp_scmidle_sm_override_enum_type sm_override,
  uint8 *p_trans_id
);
/*===========================================================================

FUNCTION HDRCP_SCMIDLE_saved_scmidle_INFO_IS_VALID

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrcp_scmidle_saved_scmidle_info_is_valid ( void );


/*===========================================================================

FUNCTION HDRCP_SCMIDLE_SET_saved_scmidle_INFO_VALID

DESCRIPTION
  This is a wrapper function.  
  
DEPENDENCIES
  None.

PARAMETERS
  saved_scmidle_info_is_valid - if saved slottedmode info is valid 

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_scmidle_set_scmidle_info_valid ( boolean saved_scmidle_info_is_valid );

/*===========================================================================

FUNCTION HDRCP_SCP_GET_CURRENT_SUBTYPE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  protocol - Protocol type for which the subtype is returned

RETURN VALUE
  Subtype value of the requested protocol, 0xFFFF will be returned if invalid 
  protocol is requested. 

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrcp_scp_get_current_subtype
(
  hdrcom_hai_protocol_name_enum_type protocol
);

/*===========================================================================

FUNCTION HDRCP_OVHD_GET_COLOR_CODE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  color_code_ptr - Pointer to allocated memory where the color code
                   will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has not been received yet

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_ovhd_get_color_code
(
  uint8 *color_code_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*===========================================================================

FUNCTION HDRCP_MC_CLIENT_CB_REG

DESCRIPTION
  This is a wrapper function.

  This function allows a client to register/deregister a callback with HDR to
  receive registered HDR events.
  
  Only one event can be registered/deregistered once. To register/deregister
  multiple events, call this function several times.

DEPENDENCIES
  None.

PARAMETERS
  client - Modules which will register the callbacks  
  cb_event - HDR event types to be registered
  reg_type - register or deregister
  cb_func_ptr - pointer to the callback functions
  client_data_ptr - pointer to the callback client data
 
RETURN VALUE
  E_NOT_ALLOWED - parameters are invalid
  E_SUCCESS - register callbacks successfully

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mc_client_cb_reg
(
  hdrcp_mc_cb_client_id_enum_type      client,
    /* client id */
  
  hdrcp_mc_cb_event_enum_type          cb_event,
    /* Events that client wants to register */ 
       
  hdrcp_mc_cb_reg_enum_type            reg_type,
    /* Register or deregister */      

  hdrcp_mc_cb_f_type                   cb_func_ptr,
    /* Pointer to a callback function to notify the client */    

  void                              *client_data_ptr
    /* Pointer to the client data */  
);

/*===========================================================================

FUNCTION HDRCP_SCP_LTE_IS_ATTACHED

DESCRIPTION
  This function is a wrapper function of hdrscp_lte_is_attached().
 
  It resets ehrpd_was_advertised flag after LTE successful attach
  so that session will be renegotiated if current session is HRPD.
 
  This function should be called only after a successful LTE attach.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_scp_lte_is_attached( void );
/*===========================================================================

FUNCTION HDRCP_SCP_SET_DATA_EHRPD_CAPABILITY

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  TRUE - upper layer's eHRPD capability is valid. 
  FALSE - upper layer's eHRPD capability is invalid.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_scp_set_data_ehrpd_capability
( 
  boolean data_ehrpd_capability
);
/*===========================================================================

FUNCTION HDRCP_SLP_SEND_MESSAGE

DESCRIPTION
  This is a wrapper function. 

DEPENDENCIES
  None

PARAMETERS
  channel            - Channel SDU has to be transmitted on
  reliable           - Set to true if reliable delivery transport is required
  priority           - Priority of the message
  message_body       - dsm item chain containing the signaling message
  slp_outcome_cbPtr  - Function to be called after message has been
                       transmitted successfully aor transmission was aborted
  outcome_tag_ptr    - User data for callback function

RETURN VALUE
  Enqueueing status (E_SUCCESS or E_PROT_Q_FULL)

SIDE EFFECTS

===========================================================================*/

hdrcom_errno_enum_type hdrcp_slp_send_msg
(
  hdrcom_hai_channel_enum_type  channel,
  boolean                   reliable,
  uint8                     priority,
  dsm_item_type*            msg_ptr,
  hdrcp_slp_outcome_cb_type    slp_outcome_cb_ptr,
  void*                     outcome_tag_ptr 
);

/*===========================================================================

FUNCTION HDRCP_SMP_SESSION_IS_OPEN

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is open
  FALSE - the session is not open

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrcp_smp_session_is_open
(
  hdrcom_hai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRCP_SMP_SESSION_IS_VALID

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is valid
  FALSE - the session is not valid

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrcp_smp_session_is_valid
(
  hdrcom_hai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRCP_STREAM_GET_STREAM

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES

PARAMETERS
  app_subtype - The application subtype who's stream is desired.
  stream_id - Pointer to location in which to return the stream_id, if 
              one is found.

RETURN VALUE
  TRUE - There is a stream configured with the Service Network
  FALSE - There is no stream configured with the Service Network

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrcp_stream_get_stream  
( 
  hdrcom_hai_app_subtype_enum_type app_subtype,
  hdrcom_hai_stream_enum_type *stream_id
);


/*===========================================================================

FUNCTION HDRCP_STREAM_GET_STREAM_CONFIG

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None
 
PARAMETERS
  stream_ptr - first element of a four-application array.

RETURN VALUE
  TRUE - streams were returned
  FALSE - streams were not returned
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_stream_get_stream_config 
(
  hdrcom_hai_app_subtype_enum_type * stream_ptr
);


/*===========================================================================

FUNCTION HDRCP_STREAM_STREAM_IS_ASSIGNED

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  app_subtype -  Application sub-type to check streams for

RETURN VALUE
  TRUE  - Stream has been allocation for a Packet Application
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_stream_stream_is_assigned
(
  hdrcom_hai_app_subtype_enum_type   app_subtype
);


/*===========================================================================

FUNCTION HDRCP_PCP_NOTIFY_APP_HAS_HP_DATA

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  substream_id   - The substream ID of the packet app
  stream_id      - The stream ID of the packet app
  user_data_ptr  - Pointer to the packet app's control block

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_pcp_notify_app_has_hp_data
(
  uint8 substream_id,
  uint8 stream_id,
  void *user_data_ptr
);


/*=============================================================================

FUNCTION HDRCP_PCP_REGISTER_RECEIVE_CALLBACKS

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  stream_id           - The stream associated with the packet app
  put_data_func_ptr   - The callback to handle received data
  eos_notif_func_ptr  - The callback to give an End-of-Slot indication
  user_data_ptr       - Pointer to the packet app's control block

RETURN VALUE
  TRUE if the packet app/stream is successfully registered; FALSE otherwise.

SIDE EFFECTS
  None.
  
=============================================================================*/
boolean hdrcp_pcp_register_receive_callbacks
(
  uint8 stream_id,
  hdrcp_pcp_put_data_func_type put_data_func_ptr,
  hdrcp_pcp_eos_notification_func_type eos_notif_func_ptr,
  hdrcp_pcp_rx_buf_empty_notification_func_type rx_buf_empty_notif_func_ptr,
  void *user_data_ptr
);


/*=============================================================================

FUNCTION HDRCP_PCP_UNREGISTER_RECEIVE_CALLBACKS

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  stream_id  - The ID of the stream being unregistered.

RETURN VALUE
  A boolean TRUE indicates the unregistration was successful; a boolean FALSE
  indicates the unregistration failed.

SIDE EFFECTS
  After a successful unregistration, the callbacks for the unregistered stream
  will be set to NULL.
  
=============================================================================*/
boolean hdrcp_pcp_unregister_receive_callbacks
(
  uint8 stream_id
);
/*=============================================================================

FUNCTION HDRCP_PCP_UNREGISTER_TRANSMIT_CALLBACKS

DESCRIPTION
  This function is a wrapper function.
 
DEPENDENCIES
  None.

PARAMETERS
  stream_id     - The stream ID of the registered packet app to unregister
  substream_id  - The substream ID of the registered packet app to unregister

RETURN VALUE
  A boolean TRUE indicates the unregistration was successful; a boolean FALSE
  indicates the unregistration failed.

SIDE EFFECTS
  None.
  
=============================================================================*/
boolean hdrcp_pcp_unregister_transmit_callbacks
(
  uint8 stream_id,
  uint8 substream_id
);
/*=============================================================================

FUNCTION HDRCP_PCP_REGISTER_TRANSMIT_CALLBACKS

DESCRIPTION
  This is a wrapper function. 
  
DEPENDENCIES
  None.

PARAMETERS
  substream_id   - The substream ID of the packet app
  mac_flow_id    - The MAC flow ID of the registering packet app
  stream_id      - The stream ID of the registering packet app
  get_data_size_func_ptr  - The callback to determine the data pending
  get_hp_data_func_ptr    - The callback to request high priority data
  get_lp_data_func_ptr    - The callback to request low priority data
  tx_status_func_ptr      - The callback to report the status of a tranmission
  tx_failed_func_ptr      - The callback to give an M-ARQ indication
  always_notify_tx_status - Set if the packet app always wants the status
                            notification regardless of transmitting data
  user_data_ptr           - Pointer to the packet app's control block
  
RETURN VALUE
  TRUE if the packet app is sucessfully registered; FALSE otherwise.

SIDE EFFECTS
  None.
  
=============================================================================*/
boolean hdrcp_pcp_register_transmit_callbacks
(
  uint8 substream_id,
  uint8 mac_flow_id,
  uint8 stream_id,
  hdrcp_pcp_get_data_size_func_type get_data_size_func_ptr,
  hdrcp_pcp_get_data_func_type get_hp_data_func_ptr,
  hdrcp_pcp_get_data_func_type get_lp_data_func_ptr,
  hdrcp_pcp_tx_status_func_type tx_status_func_ptr,
  hdrcp_pcp_tx_failed_func_type tx_failed_func_ptr,
  boolean always_notify_tx_status,
  void *user_data_ptr
);
/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_SIMPLE_ATTRIB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None.
  
PARAMETERS
  attrib_name - The Attribute's Name (per type)
  attrib_idx  - FlowNN or ResKK (if applicable, 
                HDRMLPC_ATTRIB_INVALID passed in otherwise)
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
           
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrcom_errno_enum_type hdrcp_mrlpc_get_simple_attrib
(
  hdrcp_mrlpc_attrib_name_enum_type attrib_name,
  uint8 attrib_idx,
  uint32 * value,
  uint8 * verno
);
/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_ACTIVE_FWD_RLPS

DESCRIPTION
  This is a wrapper function.
  
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
hdrcom_errno_enum_type hdrcp_mrlpc_get_active_fwd_rlps
(
  hdrcom_hai_app_subtype_enum_type   app_subtype,
  uint8 *num_active_fwd_rlps,
  uint8 *rlp_flow
);

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_ACTIVE_REV_RLPS

DESCRIPTION
  This is a wrapper function.
 
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
hdrcom_errno_enum_type hdrcp_mrlpc_get_active_rev_rlps
(
  hdrcom_hai_app_subtype_enum_type   app_subtype,
  uint8 *num_active_rev_rlps,
  uint8 *rlp_flow
);

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_AN_SUP_QOS_PROF_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_an_sup_qos_prof_attrib
(
  hdrcp_mrlpc_supported_qos_profiles * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_FLOW_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This is a wrapper function.
 
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
hdrcom_errno_enum_type hdrcp_mrlpc_get_flow_prot_parm_fwd_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_flow_prot_parm_fwd_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_FLOW_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_flow_prot_parm_rev_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_flow_prot_parm_rev_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_IDENTIFICATION_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_identification_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_flow_identification_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_MAX_RESERVATIONS_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_max_reservations_attrib
(
  hdrcp_mrlpc_max_reservations_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_RESERVATION_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_reservation_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_flow_reservation_type * value,
  uint8 * verno
);


/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_RESKK_QOS_REQUEST_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_reskk_qos_request_attrib
(
  uint8 res_kk,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_res_qos_profile_config_type * value,
  uint8 * verno
);


/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_RESKK_QOS_RESPONSE_ATTRIB

DESCRIPTION
  This is a wrapper function.
 
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
hdrcom_errno_enum_type hdrcp_mrlpc_get_reskk_qos_response_attrib
(
  uint8 res_kk,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_res_qos_profile_config_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_ROUTE_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_route_prot_parm_fwd_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_route_prot_parm_fwd_type * value,
  uint8 * verno
);
/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_ROUTE_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_route_prot_parm_rev_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_route_prot_parm_rev_type * value,
  uint8 * verno
);

/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_SEQLEN_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_seqlen_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_flow_sequence_length_type * value,
  uint8 * verno
);


/*===========================================================================

FUNCTION HDRCP_MRLPC_GET_TIMERS_ATTRIB

DESCRIPTION
  This is a wrapper function.

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
hdrcom_errno_enum_type hdrcp_mrlpc_get_timers_attrib
(
  uint8 rlp_flow_nn,
  hdrcp_mrlpc_ver_enum_type direction,
  hdrcp_mrlpc_flow_timers_type * value,
  uint8 * verno
);


/*===========================================================================

FUNCTION  HDRCP_MRLPC_PROP_GAUP_QOS_REQUEST

DESCRIPTION
  This is a wrapper function.
  
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
hdrcom_errno_enum_type hdrcp_mrlpc_prop_gaup_qos_request
(
  uint8 num_profile_blobs,
  hdrcp_mrlpc_gaup_qos_request_type *profile_ptr,
  uint8 *transaction_id_ptr,
  uint8 preallocated_trans_id  
);

/*===========================================================================

FUNCTION HDRCP_1XMC_GET_LOCATION

DESCRIPTION
  This is a wrapper function

DEPENDENCIES
  None

PARAMETERS
  location_ptr - A pointer to the location value of the current HDR system

RETURN VALUE
  TRUE - The location is valid and returned in location_ptr
  FALSE - The location is unknown or in a format unknown to IS2000-A

SIDE EFFECTS
  None
===========================================================================*/

boolean hdrcp_1xmc_get_location ( hdrcp_1xmc_location_type * location_ptr );


/*===========================================================================
FUNCTION HDRCP_PAC_FLOW_ENABLE

DESCRIPTION
  This is a wrapper function .

DEPENDENCIES
  None

PARAMETERS
   app_subtype - Application Subtype whose flow is to be enabled

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_pac_flow_enable
(
hdrcom_hai_app_subtype_enum_type app_subtype
);

/*===========================================================================
FUNCTION HDRCP_PAC_FLOW_DISABLE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
   app_subtype - Application Subtype whose flow is to be disabled

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_pac_flow_disable
(
hdrcom_hai_app_subtype_enum_type app_subtype
);
/*===========================================================================
FUNCTION HDRCP_PAC_APP_FLOW_IS_ENABLED

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If the stream corresponding to the RLP instance is Open
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_pac_app_flow_is_enabled
(
hdrcom_hai_app_subtype_enum_type app_subtype
);

/*===========================================================================

FUNCTION HDRCP_QCHATUPK_PAC_MDM_HANDLE_CASE

DESCRIPTION
  This is a wrapper function

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if QChat optimization procedure was successfully completed, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrcp_qchatupk_pac_mdm_handle_case(uint16 msg_type, dsm_item_type* msg_ptr);


/*===========================================================================

FUNCTION HDRCP_QCHATUPK_HANDLE_DATA_OVER_SIGNALLING_MSG_ID

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void hdrcp_qchatupk_handle_data_over_signalling_msg_id (void);

/*===========================================================================

FUNCTION HDRCP_QCHATUPK_MCS_ENABLED

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_qchatupk_mcs_enabled(void);

/*===========================================================================

FUNCTION HDRCP_QCHATUPK_MOD_ENABLED

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_qchatupk_mod_enabled(void);


/*===========================================================================
 
FUNCTION HDRCP_AMP_SEND_IRMI_MSG

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If IRMI is allowed to be sent
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrcp_amp_send_irmi_msg
( 
  hdrcom_hai_protocol_name_enum_type caller
);

/*===========================================================================

FUNCTION HDRCP_RX_QUEUE_IND

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  input_ind_ptr - Pointer to the indication data
  ind_data_size - The size of the data associated with this indication.
                  This value is undefined if the input_ind_ptr
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrcp_rx_queue_ind
(
  hdrcom_hai_protocol_name_enum_type  protocol_name,
  hdrcom_ind_name_enum_type           ind_name,
  void                                *input_ind_data_ptr
);

/*=============================================================================

FUNCTION HDRCP_OVHD_GET_SUBNET

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None
  
PARAMETERS
  subnet_ptr - Pointer to allocated memory where the subnet will get copied.
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has changed and we have not been received yet
              the new subnet from the SP message yet.
  
SIDE EFFECTS
  None.
=============================================================================*/
hdrcom_errno_enum_type hdrcp_ovhd_get_subnet
( 
  hdraddr_type subnet,
  hdrcom_hai_protocol_name_enum_type caller
);

/*=============================================================================
 
FUNCTION HDRCP_OVHD_PREREG_IS_ALLOWED

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  TRUE - pre-registration is allowed 
  FALSE - pre-registration is not allowed

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrcp_ovhd_prereg_is_allowed 
( 
  hdrcom_hai_protocol_name_enum_type caller
);

/*===========================================================================
 
FUNCTION HDRCP_SAP_GET_CURRENT_TUNNEL_MODE_VALUE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  caller - protocol that is invoking the function

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
hdrcom_hai_tunnel_mode_type hdrcp_sap_get_current_tunnel_mode_value 
( 
  hdrcom_hai_protocol_name_enum_type caller 
);

/*===========================================================================
FUNCTION HDRCP_SAP_SET_PREREG_STATUS

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  hdrhai_prereg_status_event_enum_type - what type of event it is

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrcp_sap_set_prereg_status
( 
  hdrcom_prereg_status_event_enum_type prereg_status_event
);

/*===========================================================================
 
FUNCTION HDCP_MC_GET_SID_PN_MAC

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  hdrmc_sid_pn_mac_info - info holding sector id, pilot PN, Mac index
  
RETURN VALUE
  E_SUCCESS - if API executed succesfully
  E_FAILURE - Failure cases like NULL pointer being passed

SIDE EFFECTS
  None
===========================================================================*/
hdrerrno_enum_type hdrcp_mc_get_sid_pn_mac
( 
  hdrmc_sid_pn_mac_info* sid_pn_mac
);

/*===========================================================================
 
FUNCTION HDRCP_MC_GET_SERV_DRC_VALUE

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  hdrmc_serv_drc_rate_info - info holding the value of serv drc rate

RETURN VALUE
  E_SUCCESS - if API executed succesfully
  E_FAILURE - Failure cases like NULL pointer being passed

SIDE EFFECTS
  None
===========================================================================*/
hdrerrno_enum_type hdrcp_mc_get_serv_drc_rate
( 
  hdrmc_serv_drc_rate_info* serv_drc_rate
);

/*=============================================================================

FUNCTION HDRCP_ALMP_CHECK_REDIR_TO_LTE_ALLOWED

DESCRIPTION
  This function checks if it is ok to redirect to LTE. 

DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  TRUE is ok to redirect to LTE. Otherwise FALSE.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrcp_almp_check_redir_to_lte_allowed(void);

#endif /* HDRCP_API_H */
