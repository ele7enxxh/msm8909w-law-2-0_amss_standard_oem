#ifndef NASLOG_V_H
#define NASLOG_V_H

/*===============================================================================================
                             NASLOG.H

DESCRIPTION
This file contains or includes files that contain NAS log packet structure definitions, 
prototypes for NAS logging functions, any constant definitions that are needed, and any external
variable declarations needed for NAS logging.

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
================================================================================================*/


/*================================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/naslog.h_v   1.15   22 Mar 2002 15:44:10   vtawker  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/naslog_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ------------------------------------------------------------------------------
09/27/12    am      Missing handling of undefined values for MBR and GBR by the MS
10/05/09    hnam    Added support for forbidden tracking are list & GUTI in EMM STATE log packet
08/20/09    hnam    Removed REG_EVENTS, as this is now supported in log_lte_nas_emm_incoming_event_type &
                    log_lte_nas_emm_incoming_event_type events.
08/14/09    hnam    Added support NAS events
07/30/09    hnam    Added log_version,std/major/minor version to LTE NAS OTA log packets
07/23/09    hnam    Changed SDF_ID type from "byte" to "word"
07/7/09     hnam    Made EMM Native security context structure to be compliant with Dec Spec and 
                    added a WIN32 macro (to take care of byte alignment)
07/1/09     hnam    Swapped EPS context tag & eps context len fields in the structure 
                    (as it was causing problems to Diag, while reading the diag buffer)
06/23/09    hnam    Added NAS related log packets and events
08/18/06   rk       Increased MAX_ACTIVE_PS_CONNECTIONS from 16 to 256 because of Extended TI Feature
4/12/01     ts      Initial Revision
06/11/01    TS      Updated log packets based on peer review.
08/09/01    JC      Added LOG_WCDMA_BASE_C for log codes.
08/14/01    JC      Adjusted log code values.                
09/07/01    CD      Modifed CS call info packet to contain single call information
                    Added log packets for CS call release and CS call change
09/18/01    sbk     The log offsets are changed to reflect the new LOG BASE value
09/24/01    jca     Removed redundant #defines to quiet Lint.
11/21/01    TS      Removed uneeded #include.
11/28/01    sbk     IMEISV size is corrected
1/28/02     CD      Replaced LOG_WCDMA_BASE_C by LOG_UMTS_BASE_C
2/15/02     CD      Removed all log packet definitions and included log_codes_umts.h
                    Changed log codes prefix from WCDMA_ to LOG_UMTS_
                    
                    Fixed typo

22-Mar-2002  VT     In the definition of calling_party_bcd_type, replaced 
                       boolean calling_party_bcd_present;  with
                       uint8 calling_party_bcd_length;
                    In the definition of called_party_bcd_type, replaced 
                       boolean called_party_bcd_present;  with
                       uint8 called_party_bcd_length;

05/06/02    AB      Added CC calls statistics indicator and NAS OTA message event logging.
04/30/04    KWA     Added LOG_UMTS_NAS_EPLMN_LIST_LOG_PACKET_C.
06/28/04    AB      Increase the BCD NO length to 20 as defined in 24.008.
12/07/04    kvk     Increased Max Active PS connections to 16
12/20/04    up      Added LOG_UMTS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C,
                    LOG_UMTS_NAS_FPLMN_LIST_LOG_PACKET_C,
                    LOG_UMTS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C,
                    LOG_UMTS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C and
                    LOG_UMTS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C to define the
                    log packet for FPLMN list update, FPLMN list, HPLMN search
                    timer start and HPLMN search start/end respectively.
                    MAX_NUMBER_FORBIDDEN_PLMNS is defined as 10.
                    MAX_NUMBER_PRIORITY_PLMNS is defined as 20.
                    Structure log_plmn_rat_type(plmn, rat) 
                    and log_detailed_plmn_info_type are added.
06/15/05    HS      Added support for FEATURE_CCBS CC messages
                     NAS_OTA_CC_ESTABLISHMENT
                     NAS_OTA_CC_ESTABLISHMENT_CONFIRMED
                     NAS_OTA_RECALL
                     NAS_OTA_START_CC
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT, 
                   EVENT_SMGMM_REJECT_RECEIVED.
09/14/05   HS      LOG_CALLED_PARTY_BCD_SIZE revised from 20 to 41
06/08/06   rk       Added Release 5 specific logs for QoS
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
================================================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "gs_v.h"                /* for QUEUE_IDs */
#include "com_iei_v.h"           /* for PD     values     */

#include "mmgsdilib_common.h"

#include "naslog.h"
#include "nasutils.h"
#include "lte_nas_emm_message_description.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


#ifdef FEATURE_EXTENDED_TI
#define MAX_ACTIVE_PS_CONNECTIONS     256
#else
#define MAX_ACTIVE_PS_CONNECTIONS     16
#endif





/*----------------------------------------------------------------------------------------------
 Data structure for PS call information
 ---------------------------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_PS_CALL_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_PS_CALL_INFO_LOG_PACKET_C)
    
   /* Number of connection ids */
   uint8                     num_ps_connections;

   /* Information for each connection id */
   ps_connection_info_type   ps_connection_info[MAX_ACTIVE_PS_CONNECTIONS];  

LOG_RECORD_END









/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_NAS_GMM_STATE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_GMM_STATE_LOG_PACKET_C)
    
   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* GMM layer state         */
   uint8   gmm_state_log;
   
   /* GMM layer substate      */
   uint8   gmm_substate_log; 

   /* GMM layer update status */
   uint8   gmm_update_status_log;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_MM_STATE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_MM_STATE_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* MM layer state         */
   uint8   mm_state_log;
   
   /* MM layer idle substate */
   uint8   mm_idle_substate_log; 

   /* MM layer update statu  */
   uint8   mm_update_status;

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_REG_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_REG_LOG_PACKET_C)
    
   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* REG layer state */
   uint8   reg_state;

   /* PLMN Selection Mode: Automatic or Manual */
   uint8   plmn_selection_mode;

   /* UE Operation mode: Class A, Class B, or Class c */
   uint8   ue_operation_mode;

LOG_RECORD_END



/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DS_CS_CALL_SETUP_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_CS_CALL_SETUP_INFO_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8   as_id;

   cs_connection_info_type   cs_connection_info;  

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DS_PS_CALL_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_PS_CALL_INFO_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* Number of connection ids */
   uint8                     num_ps_connections;

   /* Information for each connection id */
   ps_connection_info_type   ps_connection_info[MAX_ACTIVE_PS_CONNECTIONS];  

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DS_MM_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_MM_INFO_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* Network Operation mode: Class I, Class II, or Class III */
   uint8   network_operation_mode;
 
   /* Available Service: Limited, CS, PS, CS and PS, No Service */
   uint8   service_type;

   /* Serving Cell PLMN */
   log_plmn_id_type   selected_plmn_id;

   /* Serving Cell LAI */
   log_location_area_id_type location_area_id;

   /* Serving Cell RAC */
   uint8   routing_area_code;

   /* Number of available plmns */
   uint8   num_available_plmns;

   /* PLMN ids for available plmns */
   log_plmn_id_type   available_plmns[MAX_NUMBER_AVAILABLE_PLMNS];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_PS_CONNECTION_QOS_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_PS_CONNECTION_QOS_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* The associated connection id       */
   uint8   connection_id;

   /* QOS: Delay class                   */
   uint8   delay_class;
  
   /* QOS: Reliability class             */
   uint8   reliability_class;
  
   /* QOS: Peak throughput               */
   uint8   peak_throughput;
  
   /* QOS: Precedence class              */
   uint8   precedence_class;
  
   /* QOS: Mean throughput               */
   uint8   mean_throughput;
  
   /* QOS: Traffic class                 */
   uint8   traffic_class;
  
   /* QOS: Delivery order                */
   uint8   deliver_order;
  
   /* QOS: Delivery of erroneous SDUs    */
   uint8   deliver_err_sdu;
  
   /* QOS: Maximum SDU size              */
   uint8   max_sdu_size;
  
   /* QOS: Max uplink bit rate           */
   uint8   max_bit_rate_uplink;
  
   /* QOS: Max downlink bit rate         */
   uint8   max_bit_rate_downlink;
  
   /* QOS: Residual Bit Error Rate       */
   uint8   residual_ber;
  
   /* QOS: SDU error ratio               */
   uint8   sdu_err_ratio;
  
   /* QOS: Transfer delay                */
   uint8   tranfer_delay;
  
   /* QOS: Traffic Handling Priority     */
   uint8   traffic_priority;
  
   /* Guaranteed uplink Bit Error Rate   */
   uint8   guarantee_ber_uplink;
  
   /* Guaranteed downlink Bit Error Rate */
   uint8   guarantee_ber_dwnlink;

#ifdef FEATURE_REL5
   /* QOS: Signaling Indicator                  */
   uint8   sig_ind;

   /* QOS: Source Statistics descriptor                  */
   uint8   src_stat_desc;

   /* QOS: Extended Max downlink bit rate                   */
   uint8   ext_max_bit_rate_downlink;

   /* QOS: Extended Guranteed downlink bit rate                  */
   uint8   ext_gtd_bit_rate_downlink;
   
   /* QOS: Extended Max uplink bit rate                   */
   uint8  ext_max_bit_rate_uplink ;

   /* QOS: Extended Guranteed uplink bit rate                  */
   uint8  ext_gtd_bit_rate_uplink ;
   
#endif

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_CS_CONNECTION_BC_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_CS_CONNECTION_BC_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* The associated connection id       */
   uint8   connection_id;

   uint8   radio_channel_requirement;
   uint8   information_transfer_capability;
   uint8   structure;
   uint8   duplex_mode;
   uint8   rate_adaption;
   uint8   signalling_access_protocol;
   uint8   sync_async;
   uint8   number_of_data_bits;
   uint8   number_of_stop_bits;
   uint8   user_rate;
   uint8   parity;
   uint8   modem_type;
   uint8   connection_element;
   uint8   user_information_layer_2_protocol;
   uint8   coding_standard;
   uint8   transfer_mode;
   uint8   configuration;
   uint8   NIRR;
   uint8   establishment;
   uint8   user_information_layer_1_protocol;
   uint8   negotiation;
   uint8   intermediate_rate;
   uint8   NIC_on_Tx;
   uint8   NIC_on_Rx;
   uint8   other_modem_type;
   uint8   fixed_network_user_rate;
   uint8   acceptable_channel_codings;
   uint8   maximum_number_of_trafic_channels;
   uint8   user_initiated_modification_indication;
   uint8   wanted_air_interface_user_rate;
   uint8   mt_fallback_rate;
//   uint8   speech_version_indication;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_UE_DYNAMIC_ID_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* Indicates if packet has TMSI or PTMSI */
   uint8 identity_type;
    
   /* TMSI or PTMSI */
   uint8 identity[LOG_TMSI_SIZE];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_UE_STATIC_ID_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_UE_STATIC_ID_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* Length of the IMSI */
   uint8 imsi_length;

   /* IMSI    */
   uint8 imsi[LOG_MAX_IMSI_SIZE];
 
   /* IMEI    */
   uint8 imei[LOG_IMEI_SIZE];
   
   /* IMEI_SV */
   uint8 imei_sv[LOG_IMEI_SV_SIZE];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_OTA_MESSAGE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_OTA_MESSAGE_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8 as_id;

   /* Indicates if this was sent to or from the UE */
   uint8 nas_message_direction;

   /* Length of the NAS ota message */
   uint32 nas_message_length;

   /* NAS ota Message */
   uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_CFA_MESSAGE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_CFA_MESSAGE_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8   as_id;

   /* NAS cfa log message */
   uint8 cfa_message[LOG_MAX_NAS_CFA_MESSAGE_SIZE];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_CS_CALL_RELEASE_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_CS_CALL_RELEASE_INFO_LOG_PACKET_C)

   uint8    as_id;

   uint8    connection_id;

   uint8    release_cause;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_CS_CALL_CHANGE_INFO_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_CS_CALL_CHANGE_INFO_LOG_PACKET_C)

   uint8    as_id;
   uint8    connection_id;  
   uint8    mpty_state;
   uint8    hold_state;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_EPLMN_LIST_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8            as_id;

   /* Registered PLMN ID */
   log_plmn_id_type rplmn;

   /* Number of equivalent PLMNs */
   uint8            num_equivalent_plmns;

   /* Equivalent PLMN IDs */
   log_plmn_id_type equivalent_plmn[MAX_NUMBER_EQUIVALENT_PLMNS];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_FPLMN_LIST_UPDATE_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8            as_id;

   /* TRUE if added in FPLMN List, FALSE if removed from List */
   uint8            is_added;

   /* Forbidden PLMN ID */
   log_plmn_id_type fplmn;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/

 
LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_FPLMN_LIST_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8            as_id;

   /* Number of PLMNs in FPLMN List */
   uint8            num_fplmns;
   
   /* List/Array of Forbidden PLMN IDs */
   log_plmn_id_type fplmns[MAX_NUMBER_FORBIDDEN_PLMNS];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_START_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8             as_id;

   /* Time Duration, in minutes, of HPLMN Search Timer */
   uint16            hplmn_search_timer;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_HPLMN_SEARCH_TIMER_EXPIRE_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8            as_id;

   /* Not used. As the log packet need to be allocated kept one byte dummy */
   uint8            dummy;

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_HPLMN_SEARCH_START_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8             as_id;

   /* Number of PLMNs in Higher PLMN List */
   uint8             num_pplmns;

   /* List/Array of higher priority PLMN IDs */
   log_plmn_rat_type pplmns[MAX_NUMBER_PRIORITY_PLMNS];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C
 ---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_UMTS_DS_NAS_HPLMN_SEARCH_END_LOG_PACKET_C)

   /* Indicates the Active subscription ID */
   uint8                       as_id;

   /* Number of PLMNs in Higher PLMN List */
   uint8                       num_available_plmns;

   /* List/Array of higher priority PLMN IDs */
   log_detailed_plmn_info_type info[MAX_NUMBER_AVAILABLE_PLMNS];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                 EVENT_NAS_MESSAGE_SENT/RECEIVED  Payload/Definitions 
 ---------------------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST{
   byte    id;          /* Message ID or Timer ID          */
   byte    set;         /* IMH_T.message_set (ms.h), Protocol Discminator, or Queue */
   byte    channel;     /* Channel type, Transaction ID, or Connection ID */
} nas_generic_identity_type;

/* ----------------------------------------------------------------------------
** Message types for logging generic events EVENT_SMGMM_REQUEST_SENT 
** and EVENT_SMGMM_REJECT_RECEIVED
** --------------------------------------------------------------------------*/

typedef enum
{
  SMGMM_EVENT_NO_MESSAGE,
  SMGMM_EVENT_LOCATION_AREA_UPDATE_REQUEST,
  SMGMM_EVENT_ROUTING_AREA_UPDATE_REQUEST,
  SMGMM_EVENT_ATTACH_REQUEST,
  SMGMM_EVENT_DETACH_REQUEST,
  SMGMM_EVENT_PDP_ACTIVATE_REQUEST,
  SMGMM_EVENT_PDP_DEACTIVATE_REQUEST,
  SMGMM_EVENT_LOCATION_AREA_UPDATE_REJECT,
  SMGMM_EVENT_ROUTING_AREA_UPDATE_REJECT,
  SMGMM_EVENT_ATTACH_REJECT,
  SMGMM_EVENT_PDP_ACTIVATE_REJECT
} smgmm_event_message_type;


/* 
 * Message Types for cc to cc peer messages.  These are as defined in
 * GSM 04.08.  Locally defines here to avoid including cc_cc.h.
 */
#define NAS_OTA_ALERTING           0x01
#define NAS_OTA_CALL_CONFIRMED     0x08
#define NAS_OTA_CALL_PROCEEDING    0x02
#define NAS_OTA_CONNECT            0x07
#define NAS_OTA_CONNECT_ACK        0x0f
#define NAS_OTA_EMERGENCY_SETUP    0x0e
#define NAS_OTA_PROGRESS           0x03
#define NAS_OTA_SETUP              0x05
#define NAS_OTA_MODIFY             0x17
#define NAS_OTA_MODIFY_COMPLETE    0x1f
#define NAS_OTA_MODIFY_REJECT      0x13
#define NAS_OTA_USER_INFORMATION   0x10
#define NAS_OTA_HOLD               0x18
#define NAS_OTA_HOLD_ACK           0x19
#define NAS_OTA_HOLD_REJECT        0x1a
#define NAS_OTA_RETRIEVE           0x1c
#define NAS_OTA_RETRIEVE_ACK       0x1d
#define NAS_OTA_RETRIEVE_REJ       0x1e
#define NAS_OTA_DISCONNECT         0x25
#define NAS_OTA_RELEASE            0x2d
#define NAS_OTA_RELEASE_COMPLETE   0x2a
#define NAS_OTA_CONGESTION_CONTROL 0x39
#define NAS_OTA_NOTIFY             0x3e
#define NAS_OTA_STATUS_MSG         0x3d
#define NAS_OTA_STATUS_ENQUIRY     0x34
#define NAS_OTA_START_DTMF         0x35
#define NAS_OTA_STOP_DTMF          0x31
#define NAS_OTA_STOP_DTMF_ACK      0x32
#define NAS_OTA_START_DTMF_ACK     0x36
#define NAS_OTA_START_DTMF_REJ     0x37
#define NAS_OTA_FACILITY           0x3a
#define NAS_OTA_REGISTER           0x3b
#define NAS_OTA_CP_DATA            0x01
#define NAS_OTA_CP_ACK             0x04
#define NAS_OTA_CP_ERROR           0x10
#ifdef FEATURE_CCBS
#define NAS_OTA_CC_ESTABLISHMENT   0x04
#define NAS_OTA_CC_ESTABLISHMENT_CONFIRMED   0x06
#define NAS_OTA_RECALL             0x0b
#define NAS_OTA_START_CC           0x09
#endif /* FEATURE_CCBS */

/*----------------------------------------------------------------------------------------------
                 EVENT_UMTS_CALLS_STATISTICS  Payload/Definitions 
 ---------------------------------------------------------------------------------------------*/
typedef enum 
{
    /* MO call setup */
    MO_NORMAL_CALL_ATTEMPTED          = 01,  /* 01 - Mobile initiated normal voice call attempted */
    MO_EMERGENCY_CALL_ATTEMPTED,             /* 02 - Mobile initiated emergency voice call attempted */ 
    MO_CS_DATA_CALL_ATTEMPTED,               /* 03 - Mobile initiated circuit switch data call attempted */
    MO_NORMAL_CALL_CONNECTED,                /* 04 - MO call, connected */
    #ifdef FEATURE_CCBS
    MO_NIMO_CALL_NOTIFIED,                   /* 05 - Network Initiated Mobile Origination call request notified to UE */
    #endif /* FEATURE_CCBS */
                                             /* 06 - 10 Reserved */
    /* MO call clearing */
    MO_MS_CALL_CLEARING_ATTEMPTED     = 11,  /* 11 - MO call, mobile initiated call clearing from conversatation state */
    MO_NW_CALL_CLEARING_ATTEMPTED,           /* 12 - MO call, network initiated call clearing from connected state */
    MO_MS_CALL_ABORTED,                      /* 13 - MO call, mobile aborted before the call is connected */
    MO_NW_CALL_ABORTED,                      /* 14 - MO call, network aborted before the call is connected */
    MO_NORMAL_CALL_ENDED,                    /* 15 - MO call, ended */
    MO_ABNORMAL_CALL_ENDED,                  /* 16 - MO call, unexpected call ended from radio link failure (RRC_REL_IND) */
                                             /* 17 - 20 Reserved */
    /* MT call setup */
    MT_NORMAL_CALL_ATTEMPTED          = 21,  /* 21 - Network initiated normal voice or data call attempted */
    MT_NORMAL_CALL_CONNECTED,                /* 22 - MT call, connected */
                                             /* 23 - 30 Reserved */
    /* MT call clearing */
    MT_MS_CALL_CLEARING_ATTEMPTED     = 31,  /* 31 - MT call, mobile initiated call clearing */
    MT_NW_CALL_CLEARING_ATTEMPTED,           /* 32 - MT call, network initiated call clearing */
    MT_MS_CALL_ABORTED,                      /* 33 - MT call, mobile aborted before the call is connected */
    MT_NW_CALL_ABORTED,                      /* 34 - MT call, network aborted before the call is connected */
    MT_NORMAL_CALL_ENDED,                    /* 35 - MT call, ended */
    MT_ABNORMAL_CALL_ENDED,                  /* 36 - MT call, unexpected call ended from radio link failured (RRC_REL_IND) */
                                             /* 37 - 40 Reserved */ 

    MAX_CALL_STATISTIC_INDICATORS     = 255  /* 41 - 255 Reserved */

} nas_calls_statistics_T;

/*************************************************************************************************************
                                        LTE NAS LOG STRUCTURE DEFINITIONS
**************************************************************************************************************/
#ifdef FEATURE_LTE

#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

typedef PACK(struct)
{
  byte in_ota_raw_data[LTE_NAS_OTA_MSG_MAX_SIZE];  /*!< Incoming NAS ESM security protected OTA message */
} lte_nas_esm_sec_protected_in_ota_msg_T;

typedef PACK(struct)
{
  byte out_ota_raw_data[LTE_NAS_OTA_MSG_MAX_SIZE];  /*!< Outgoing NAS ESM security protected OTA message */
} lte_nas_esm_sec_protected_out_ota_msg_T;


typedef PACK(struct)
{
  byte log_version;
  byte std_version;
  byte std_major_version;
  byte std_minor_version;
  byte in_ota_raw_data[LTE_NAS_OTA_MSG_MAX_SIZE];  /*!< Incoming NAS ESM plain OTA message */
} lte_nas_esm_plain_in_ota_msg_T;


typedef PACK(struct)
{
  byte log_version;
  byte std_version;
  byte std_major_version;
  byte std_minor_version;
  byte in_ota_raw_data[LTE_NAS_OTA_MSG_MAX_SIZE];  /*!< Incoming NAS ESM plain protected OTA message */
} lte_nas_esm_plain_out_ota_msg_T;

typedef PACK(struct)
{
  byte log_version; /*!< Log packet version number*/
  byte bearer_id;
  byte bearer_state;
  byte connection_id;
} lte_nas_esm_bearer_context_state_T;

typedef PACK(struct)
{
  byte log_version; /*!< Log packet version number*/
  byte context_type;
  byte bearer_id;
  byte bearer_state;
  byte connection_id;
  word sdf_id;
  //byte ota_context_data[LTE_NAS_OTA_MSG_MAX_SIZE];
  boolean lbi_valid;
  byte lbi;
  byte rb_id;
  byte eps_qos[10];/* Refer logging ICD section 8.1.6 for formatting of this data*/
} lte_nas_esm_bearer_context_info_T;

typedef PACK(struct)
{
  byte log_version; /*!< Log packet version number*/
  byte instance_id;
  byte proc_state;
  byte pti;
  word sdf_id;
  byte pending_msg_id;
} lte_nas_esm_procedure_state_T;

/***************************************************************
                   EMM PACKET INFORMATION
***************************************************************/
typedef PACK(struct)
{
  byte in_ota_raw_data[LTE_NAS_OTA_MSG_MAX_SIZE];  /*!< Incoming NAS EMM security protected OTA message */
} lte_nas_emm_sec_protected_in_ota_msg_type;

typedef PACK(struct)
{
  byte out_ota_raw_data[LTE_NAS_OTA_MSG_MAX_SIZE];  /*!< Outgoing NAS EMM security protected OTA message */
} lte_nas_emm_sec_protected_out_ota_msg_type;


typedef PACK(struct)
{
  byte log_version;
  byte std_version;
  byte std_major_version;
  byte std_minor_version;
  byte in_ota_raw_data[LTE_NAS_OTA_MSG_MAX_SIZE];  /*!< Incoming NAS EMM plain OTA message */
} lte_nas_emm_plain_in_ota_msg_type;

typedef PACK(struct)
{
  byte log_version;
  byte std_version;
  byte std_major_version;
  byte std_minor_version;
  byte in_ota_raw_data[LTE_NAS_OTA_MSG_MAX_SIZE];  /*!< Incoming NAS EMM plain protected OTA message */
} lte_nas_emm_plain_out_ota_msg_type;

#define PLMN_ID_LEN 3

typedef PACK(struct)
{
  byte                ue_id;
  sys_plmn_id_s_type  plmn;
  byte                      mme_group_id[2]; /* Length of Mobile Identity Group ID = 2, Ref section: 9.9.3.10.1*/
  byte                          mme_code;
  byte                      m_tmsi[4];       /* Length of Mobile Identity M-TMSI = 4, Ref section: 9.9.3.10.1*/
} lte_nas_emm_guti_log_type;

typedef PACK(struct)
{
  byte                log_version; /*!< Log packet version number*/
  byte                emm_state;
  word                emm_substate; /*Contradicts -> emm_substate is of type 'word" in emm_database.h*/
  sys_plmn_id_s_type  plmn;
  byte                       guti_valid;
  lte_nas_emm_guti_log_type  guti;
} lte_nas_emm_state_type;

typedef PACK(struct)
{
  sys_plmn_id_s_type plmn;
  word               tac;
}lte_nas_tai_lst1_log_type;

typedef PACK(struct)
{
  byte                       length;
  lte_nas_tai_lst1_log_type  diff_plmn[LTE_RRC_MAX_NUM_TRACKING_AREAS]; 
}lte_nas_tai_lst2_log_type;

typedef PACK(struct)
{
  byte                       log_version; /*!< Log packet version number*/
  lte_nas_tai_lst2_log_type  forbidden_roaming_list;
  lte_nas_tai_lst2_log_type  forbidden_service_list;
} lte_nas_emm_forbidden_tailist_type;

#define EPSLOCI_LENGTH 18
#define IMSI_LEN 8

typedef PACK(struct)
{
  byte                        log_version; /*!< Log packet version number*/
  mmgsdi_app_enum_type        usim_card_mode;
  boolean                     lte_service_supported;
  byte                        imsi[IMSI_LEN];
  byte                        epsloci[EPSLOCI_LENGTH];
} lte_nas_emm_usim_card_mode_type;

#define NAS_UL_DL_COUNT 4
#define NAS_INT_CIPHER_KEY_LEN 16
#define KASME_LEN 32

typedef PACK(struct)
{
  byte                                     log_version; /*!< Log packet version number*/
  byte                                     nas_ul_cnt[NAS_UL_DL_COUNT];     
  byte                                     nas_dl_cnt[NAS_UL_DL_COUNT];
  byte                                     nas_integrity_algo_key[NAS_INT_CIPHER_KEY_LEN];
  byte                                     nas_cipher_algo_key[NAS_INT_CIPHER_KEY_LEN];
  lte_nas_emm_integrity_protect_alg_type   nas_int_algo;
  lte_nas_emm_ciphering_alg_type           ciphering_alg;
  byte                                     nas_eksi;
  byte                                     nas_kasme[KASME_LEN];
} lte_nas_emm_current_sec_context_type;

typedef PACK(struct)
{
  byte      log_version; /*!< Log packet version number*/
  byte      keyset_id_ksips;
  byte      cipher_keys_ckps[NAS_INT_CIPHER_KEY_LEN];
  byte      int_keys[NAS_INT_CIPHER_KEY_LEN];
}lte_nas_emm_ps_domain_int_cipher_keys;

#define LTE_NAS_UST_LEN 11

typedef PACK(struct)
{
  byte log_version; /*!< Log packet version number*/
  byte ust_value[LTE_NAS_UST_LEN];
}lte_nas_emm_usim_serv_table_type;

#define MAX_KASME_VALUE_LEN 32
#define NAS_COUNT_LEN 4

#define NAS_INT_ENC_ALGO_LEN 16

typedef PACK(struct)
{
  byte log_version; /*!< Log packet version number*/
  byte eps_nas_sec_context_tag;
  word eps_nas_sec_context_len;
  byte ksi_tag;
  word ksi_len;
  byte ksi_value;
  byte kasme_tag;
  word kasme_len;
  byte kasme_value[MAX_KASME_VALUE_LEN];
  byte ul_nas_count_tag;
  word ul_nas_count_len;
  byte ul_nas_count_value[NAS_COUNT_LEN];
  byte dl_nas_count_tag;
  word dl_nas_count_len;
  byte dl_nas_count_value[NAS_COUNT_LEN];
  byte nas_int_key_tag;
  word nas_int_key_len;
  byte nas_int_key_value[NAS_INT_ENC_ALGO_LEN];
  byte nas_enc_key_tag;
  word nas_enc_key_len;
  byte nas_enc_key_value[NAS_INT_ENC_ALGO_LEN];
  byte nas_algo_tag;
  word nas_algo_len;
  byte nas_algo_value;
}lte_nas_emm_native_sec_context_type;

/***************************************************************
                   ESM EVENT INFORMATION
***************************************************************/

/*
typedef enum
{
  LTE_NAS_ESM_RECEIVED_CM_MSG  = 0,
  LTE_NAS_ESM_MSG_SENT_TO_CM   = 1,
  LTE_NAS_ESM_RECEIVED_EMM_MSG = 2,
  LTE_NAS_ESM_MSG_SENT_TO_EMM  = 3  
}lte_nas_in_out_esm_msg_event_type;

typedef enum
{
  LTE_NAS_ESM_TIMER_START = 0,
  LTE_NAS_ESM_TIMER_EXPIRE = 1
}lte_nas_esm_timer_status_event_type;

typedef enum
{
  LTE_NAS_EMM_RECEIVED_REG_MSG = 0,
  LTE_NAS_EMM_MSG_SENT_TO_REG  = 1,
  LTE_NAS_EMM_RECEIVED_ESM_MSG = 2,
  LTE_NAS_EMM_MSG_SENT_TO_ESM  = 3  
}lte_nas_in_out_emm_msg_event_type;

typedef enum
{
  LTE_NAS_EMM_TIMER_START  = 0,
  LTE_NAS_EMM_TIMER_EXPIRE = 1
}lte_nas_emm_timer_status_event_type;

typedef PACK(struct)
{
  lte_nas_in_out_esm_msg_event_type  esm_event;
}log_lte_nas_esm_msg_event_type;

typedef PACK(struct)
{
  lte_nas_esm_timer_status_event_type  esm_timer_status;
}log_lte_nas_esm_timer_status_event_type;

typedef PACK(struct)
{
  lte_nas_in_out_emm_msg_event_type  emm_event;
}log_lte_nas_emm_msg_event_type;

typedef PACK(struct)
{
  lte_nas_emm_timer_status_event_type  emm_timer_status;
}log_lte_nas_emm_timer_status_event_type;
*/

typedef PACK(struct)
{
  byte  esm_timer_event;
}log_lte_nas_esm_timer_event_type;

typedef PACK(struct)
{
  byte  emm_timer_event;
}log_lte_nas_emm_timer_event_type;

typedef PACK(struct)
{
  dword  esm_in_msg;
}log_lte_nas_esm_incoming_msg_event_type;

typedef PACK(struct)
{
  dword  esm_out_msg;
}log_lte_nas_esm_outgoing_msg_event_type;

typedef PACK(struct)
{
  dword  emm_in_msg;
}log_lte_nas_emm_incoming_event_type;

typedef PACK(struct)
{
  dword  emm_out_msg;
}log_lte_nas_emm_outgoing_msg_event_type;

typedef PACK(struct)
{
  byte out_msg;
}log_lte_nas_ota_out_msg_event_type;

typedef PACK(struct)
{
  byte in_msg;
}log_lte_nas_ota_in_msg_event_type;

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

#endif/*#ifndef FEATURE_LTE*/
#endif
