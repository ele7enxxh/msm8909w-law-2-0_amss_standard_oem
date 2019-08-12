#ifndef HDRCOM_API_H
#define HDRCOM_API_H

/*===========================================================================
               H D R    C O M M O N    D E F I N I T I O N

GENERAL DESCRIPTION
      This file contains hdr common definitions

  Copyright (c) 2011 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/api/hdrcom_api.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
10/12/2016   vko     Handle MMODE deactivate in middle of D2L measurement
06/30/2016   vko     FR 33944: Inform D2L redir success and failure to DS
04/05/2016   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
07/29/2014   tnp     FR22375: Fix Medium compiler warnings in HDR
02/26/2014   vko     FR 17879: Extended EARFCN
04/07/2014   vke     Report different indication to CP when searcher moves from 
                     3a to 3f when compared with 3a to 3
08/29/2013   vko     FR2812 - Pausing sync timer from HDRCP when HDRSRCH is in 2a state
10/25/2012   rmg     Added HDRHAI_TX to list of internal protocol ids.
10/02/2012   wsh     Fixed data stall issue after active HO
09/20/2012   cnx     Save session to EFS when powerdown in tunnel.
09/11/2012   cnx     Handle TX and RX messages during active handoff
07/16/2012   cnx     Handle connection request when SAP subtype is 0.
04/25/2012   cnx     Added HDRIND_SAP_RETURNING_TO_CLOSE.
04/24/2012   cnx     SU API cleanup.
03/09/2012   cnx     Added two traces to track connection attempt and release.
03/08/2012   wsh     Merged NLB support
02/24/2012   smd     Removed feature defines from SU API header files.
02/21/2012   smd     Fixed compiler error.
02/13/2012   smd     Fixed compiler errors.
01/27/2012   cnx     Support pre-Registration status update.
01/06/2012   cnx     Add indication HDRIND_ALMP_RESELECT_EXIT_HDR.
01/06/2012   cnx     Add parameter to HDRIND_OVHD_LTE_CELL_CHANGED.
12/16/2011   wsh     Fixed/merged changes to make NI 3.0 build
11/28/2011   cnx     Support HDRIND_OVHD_COLOR_CODE_CHANGED.	 
11/11/2011   cnx     Removed HDRHAI_CONN_FAIL_DATA_RESERVED. 
11/10/2011   cnx     Fixed compiler errors.	 
11/08/2011   cnx     Support Optimized Handoff phase 3.	 
10/24/2011   smd     Moved more to SU API header files.
10/23/2011   cnx     Changed the ID of some trace.
10/10/2011   cnx     Support HDRIND_OVHD_LTE_CELL_CHANGED.
09/10/2011   smd     Created this file.
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "event.h"
#include "IxErrno.h"
#include "nv.h"
#include "sys_v.h"
#include "dsm.h"
#include "hdrl1_api.h"
#include "lte_irat_types.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define HDRTRACE_CMD                   HDRCOM_TRACE_CMD
#define HDRTRACE_IND                   HDRCOM_TRACE_IND


/* SUAPI_TODO:Move to hdrtrace.h later */
#define HDRTRACE_SL10                      HDRCOM_TRACE_SL10  
#define HDRTRACE_SL6                       HDRCOM_TRACE_SL6 
#define HDRTRACE_CCMD                      HDRCOM_TRACE_CCMD
#define HDRTRACE_CSTATE                    HDRCOM_TRACE_CSTATE
#define HDRTRACE_CURRENT_STATE             HDRCOM_TRACE_CURRENT_STATE
#define HDRTRACE_CMSG                      HDRCOM_TRACE_CMSG
/* SUAPI_TODO: Move to trace.h later */
#define trace_id_enum_type                 hdrcom_trace_id_enum_type
/*SUAPI_TODO: Move to hdrdsm.h later */
#define HDRDSM_GET_CARRIER_ID              HDRCOM_DSM_GET_CARRIER_ID
#define HDRDSM_GET_CHAN_BIT                HDRCOM_DSM_GET_CHAN_BIT
#define HDRDSM_GET_DRC                     HDRCOM_DSM_GET_DRC
#define HDRDSM_GET_LINK_ID                 HDRCOM_DSM_GET_LINK_ID
#define HDRDSM_GET_PROT_ID                 HDRCOM_DSM_GET_PROT_ID
#define HDRDSM_GET_SCHED_GROUP_ID          HDRCOM_DSM_GET_SCHED_GROUP_ID
#define HDRDSM_SET_TRACER                  HDRCOM_DSM_SET_TRACER

#define DRC_MASK                           HDRCOM_DRC_MASK
#define DRC_LSB                            HDRCOM_DRC_LSB
#define CHAN_BIT_MASK                      HDRCOM_CHAN_BIT_MASK
#define CHAN_BIT_LSB                       HDRCOM_CHAN_BIT_LSB
#define SCHED_GROUP_ID_MASK                HDRCOM_SCHED_GROUP_ID_MASK
#define SCHED_GROUP_ID_LSB                 HDRCOM_SCHED_GROUP_ID_LSB
#define CARRIER_ID_MASK                    HDRCOM_CARRIER_ID_MASK
#define CARRIER_ID_LSB                     HDRCOM_CARRIER_ID_LSB
#define PROT_ID_MASK                       HDRCOM_PROT_ID_MASK        
#define PROT_ID_LSB                        HDRCOM_PROT_ID_LSB       
#define LINK_ID_MASK                       HDRCOM_LINK_ID_MASK
#define LINK_ID_LSB                        HDRCOM_LINK_ID_LSB

#define HDDSM_RLP_TX                       HDRCOM_DSM_RLP_TX
#define HDRDSM_RLP_TXQ                     HDRCOM_DSM_RLP_TXQ
#define HDRDSM_RLP_RTX                 HDRCOM_DSM_RLP_RTX
/*SUAPI_TODO:Move to hdrhai.h later */
#define hdrhai_protocol_type_enum_type     hdrcom_hai_protocol_type_enum_type
#define hdrhai_protocol_name_enum_type     hdrcom_hai_protocol_name_enum_type
#define hdrhai_protocol_instance_enum_type hdrcom_hai_protocol_instance_enum_type
#define hdrhai_stream_enum_type            hdrcom_hai_stream_enum_type
#define hdrhai_channel_enum_type           hdrcom_hai_channel_enum_type
#define hdrhai_abort_reason_enum_type      hdrcom_hai_abort_reason_enum_type
#define hdrhai_app_subtype_enum_type       hdrcom_hai_app_subtype_enum_type
#define hdrhai_session_change_reason_enum_type \
        hdrcom_hai_session_change_reason_enum_type
#define hdrhai_open_reason_enum_type       hdrcom_hai_open_reason_enum_type
#define hdrhai_session_close_reason_enum_type \
        hdrcom_hai_session_close_reason_enum_type
#define hdrhai_conn_layer_failure_reason_enum_type \
        hdrcom_conn_layer_failure_reason_enum_type
#define hdrhai_prereg_status_event_enum_type \
        hdrcom_prereg_status_event_enum_type
#define hdrhai_tunnel_mode_type            hdrcom_hai_tunnel_mode_type
#define hdrhai_prereg_status_enum_type     hdrcom_hai_prereg_status_enum_type
/* Move to hdrind.h later */
#define hdrind_ind_name_enum_type          hdrcom_ind_name_enum_type
#define hdrind_ind_data_union_type         hdrcom_ind_data_union_type
#define hdrind_idle_conn_fail_info_type    hdrcom_ind_idle_conn_fail_info_type
#define hdrind_lmac_wakeup_status_type     hdrcom_ind_lmac_wakeup_status_type
#define hdrind_lmac_rf_change_enum_type    hdrcom_ind_lmac_rf_change_enum_type
#define hdrind_lmac_lte_resel_info_type    hdrcom_ind_lmac_lte_resel_info_type
#define hdrind_irat_failure_ind_type       hdrcom_ind_irat_failure_ind_type
#define hdrind_irat_failure_cause_enum_type hdrcom_ind_irat_failure_cause_enum_type
#define hdrind_lmac_timing_info_type       hdrcom_ind_lmac_timing_info_type
#define hdrind_cmac_intra_scc_info_type    hdrcom_ind_cmac_intra_scc_info_type
/*SUAPI_TODO:Move to hdrerrrno.h later */
#define hdrerrno_enum_type                 hdrcom_errno_enum_type
/*SUAPI_TODO:Move to hdrlog.h later */
#define hdrlog_dipsw_enum_type             hdrcom_log_dipsw_enum_type
#define BITMASK                            HDRCOM_BITMASK
#define HDRLOG_DIPSW_NUM                   HDRCOM_LOG_DIPSW_NUM
/*hdrbuf.h*/
#define hdrbuf_ind_type                    hdrcom_buf_ind_type

#define hdrcom_srchltemeas_rpt_cell_struct_type  hdrl1_srchltemeas_rpt_cell_struct_type
#define hdrcom_srchltemeas_rpt_struct_type    hdrl1_srchltemeas_rpt_struct_type
#define hdrind_srchltemeas_rpt_struct_type    hdrcom_srchltemeas_rpt_struct_type
#define hdrhai_irat_meas_fail_reason_enum_type    hdrcom_irat_meas_fail_reason_enum_type

/*-----------------------------------------------------------------------------*/
#define hdrcom_errno_enum_type             errno_enum_type
  /* And use errno_enum_type for each hdrerrno_enum_type */



/* Macro for creating bit mask values given the position */
#define HDRCOM_BITMASK( val ) ( 1<<(val) )

#define HDRCOM_LOG_DIPSW_NUM 13
  /* Total number of dip switches, including reserved */

#define HDRCOM_DRC_MASK            0x0000F000UL
#define HDRCOM_DRC_LSB             12

#define HDRCOM_CHAN_BIT_MASK       0x00000200UL
#define HDRCOM_CHAN_BIT_LSB        9

#define HDRCOM_SCHED_GROUP_ID_MASK   0x0000003FUL
#define HDRCOM_SCHED_GROUP_ID_LSB    0

#define HDRCOM_CARRIER_ID_MASK       0x000003C0UL
#define HDRCOM_CARRIER_ID_LSB        6


#define HDRCOM_PROT_ID_MASK        0x7F
#define HDRCOM_PROT_ID_LSB         0

#define HDRCOM_LINK_ID_MASK        0xFF000000UL
#define HDRCOM_LINK_ID_LSB         24

#define HDRCOM_DSM_RLP_TX         0xa3       /* RLP Tx */
#define HDRCOM_DSM_RLP_TXQ        0xa4       /* RLP Transmit Queue */
#define HDRCOM_DSM_RLP_RTX        0xa5       /* RLP Re-Transmit */

#define HDRCOM_DSM_SET_TRACER( item, tid )    hdrcom_dsm_set_tracer( item )

/* This is where trace IDs are defined.  The first one must be 
** in the temporary reserverd space event.h
*/
typedef enum
{
  TRACE_FIRST_ID = EVENT_RSVD_START, /* 2048 (0x0800) */
  TRACE_HDR_STATE,                                                        
  TRACE_HDR_STATE_ENTRY,                        
  TRACE_HDR_TXMSG_FAILED,                       
  TRACE_HDR_TXMSG_ABORTED,                      
  TRACE_HDR_CMD,                                
  TRACE_HDR_CMD_IGNORED_STATE,                  
  TRACE_HDR_IND,                                
  TRACE_HDR_IND_IGNORED_STATE,                  
  TRACE_HDR_INIT_DEFAULT,                       

  TRACE_HDR_INIT_NV, /* 2058 (0x080A) */
  TRACE_HDR_ALMP_CONNECTION_IS_OPEN,            
  TRACE_HDR_ALMP_SYSTEM_IS_ACQUIRED,            
  TRACE_HDR_CON_GET_ADVERTISED_SUSPEND_TIME,    
  TRACE_HDR_HMP_WAITING_TO_ACTIVATE_ACMAC ,      
  TRACE_HDR_IDLE_RESELECT_IS_ALLOWED,           
  TRACE_HDR_OVHD_GET_SP_MSG,                    
  TRACE_HDR_OVHD_GET_AP_MSG,                    
  TRACE_HDR_OVHD_INFO_IS_UPDATED,               
  TRACE_HDR_OVHD_AP_IS_UPDATED,                 

  TRACE_HDR_OVHD_GET_COLOR_CODE, /* 2068 (0x0814) */
  TRACE_HDR_OVHD_GET_SECTOR_ID_24,              
  TRACE_HDR_OVHD_MSG_IGNORED_STATE,             
  TRACE_HDR_OVHD_MSG_IGNORED_REDUNDANT,         
  TRACE_HDR_OVHD_QC_MSG_RX,
  TRACE_HDR_OVHD_IGNORED_AP_MSG_UNEXPECTED_LINK,
  TRACE_HDR_SLP_AC_MSG_TX,                      
  TRACE_HDR_SLP_RTC_MSG_TX,                     
  TRACE_HDR_SLP_ABORT_MSG,                      
  TRACE_HDR_SLP_ACK_MSG,

  TRACE_HDR_LMAC_WAIT_FOR_ACMAC_ABORT, /* 2078 (0x081E) */         
  TRACE_HDR_LMAC_WAIT_FOR_RTCMAC_STOPTX ,        
  TRACE_HDR_LMAC_WAKEUP ,                        
  TRACE_HDR_LMAC_ACCESS_MODE_ENTERED ,           
  TRACE_HDR_LMAC_PILOT_MEAS_REQUIRED ,           
  TRACE_HDR_LMAC_CHAN_MEAS_COMPLETE ,            
  TRACE_HDR_LMAC_SLEEP ,                         
  TRACE_HDR_CMAC_IS_ACTIVE,                     
  TRACE_HDR_AMAC_START_ACCESS,                  
  TRACE_HDR_AMAC_IS_ACTIVE,                     

  TRACE_HDR_AMAC_GET_LAST_TX_PWR, /* 2088 (0x0828) */
  TRACE_HDR_AMAC_SENDING_PROBE,                 
  TRACE_HDR_FMAC_IS_ACTIVE,                     
  TRACE_HDR_RMAC_IS_ACTIVE,                     
  TRACE_HDR_SMP_T_KEEP_ALIVE_IGNORED_STATE,     
  TRACE_HDR_SMP_T_KEEP_ALIVE_DISABLED,                                    
  TRACE_HDR_SMP_T_CONFIG_RSP_IGNORED_STATE,     
  TRACE_HDR_SMP_IGNORE_KEEP_ALIVE_RSP_MSG,       
  TRACE_HDR_AMP_RECEIVE_ATI_MATCH,              
  TRACE_HDR_AMP_GET_TRANSMIT_ATI,               

  TRACE_HDR_AMP_GET_UATI, /* 2098 (0x0832) */
  TRACE_HDR_AMP_GET_SESSION_SEED,
  TRACE_HDR_SCP_PREVIOUS_SESSION_NEEDED,        
  TRACE_HDR_SCP_ATTEMPT_CONFIG_SUBTYPE,         
  TRACE_HDR_SCP_GET_SUBTYPE,                    
  TRACE_HDR_SCP_RESTORE_FROM_NV,                
  TRACE_HDR_SCP_SESSION_IS_OPEN,                
  TRACE_HDR_SCP_GET_TOKEN,   
  TRACE_HDR_SMP_SET_T_KEEP_ALIVE,
  TRACE_HDR_SMP_SET_T_KEEP_ALIVE_FROM_NV,

  TRACE_HDR_SMP_UPDATE_T_KEEP_ALIVE, /* 2108 (0x083C) */
  TRACE_HDR_SMP_CLR_T_KEEP_ALIVE,
  TRACE_HDR_SMP_DEF_T_KEEP_ALIVE,
  TRACE_HDR_BCMCS_FLOW_STATE_ENTRY, /* 2111 (0x083F) */

  /* From this point, the Trace ID value is not consecutive */
  TRACE_HDR_CSNA_DUP_3G1X_SRV_MSG = 2143,      /* 2143 (0x085F) */
  TRACE_HDR_IDLE_CONNECT_REASON,               /* 2144 (0x0860) */
  TRACE_HDR_IDLE_CONNECTION_DENIED_REASON,     /* 2145 (0x0861) */
  TRACE_HDR_IDLE_CONNECTION_SETUP_FAIL_REASON, /* 2146 (0x0862) */
  TRACE_HDR_OVHD_PRE_REG_IS_ALLOWED,           /* 2147 (0x0863) */
  TRACE_HDR_SAP_TUNNEL_IS_ENABLED,             /* 2148 (0x0864) */
  TRACE_HDR_SAP_CONNECTION_IS_OPEN,            /* 2149 (0x0865) */
  TRACE_HDR_ALMP_LTE_RESELECT,                 /* 2150 (0x0866) */
  TRACE_HDR_OVHD_ORNL_IS_UPDATED,              /* 2151 (0x0867) */
  TRACE_HDR_LTE_RESEL_STATUS,                  /* 2152 (0x0868) */

  /* From this point, the Trace ID value is not consecutive */
  TRACE_HDR_CONN_ATTEMPT_ENDED = 2186,         /* 2186 (0x0869) */
  TRACE_HDR_CONN_TERMINATED,                   /* 2187 (0x086A) */

  TRACE_MAX_ID
} hdrcom_trace_id_enum_type;

/* Table 2-1 HDR Protocol Type Values */
typedef enum
{
  HDRHAI_N_PHY_TYPE    = 0x00, 
    /* Physical Layer Protocol */

  HDRHAI_N_CCMP_TYPE   = 0x01, 
    /* Control Channel MAC Protocol */

  HDRHAI_N_ACMP_TYPE   = 0x02, 
    /* Access Channel MAC Protocol */

  HDRHAI_N_FTMP_TYPE   = 0x03, 
    /* Forward Traffic Channel MAC Protocol */

  HDRHAI_N_RTMP_TYPE   = 0x04, 
    /* Reverse Traffic Channel MAC Protocol */

  HDRHAI_N_KEP_TYPE    = 0x05, 
    /* Key Exchange Protocol */

  HDRHAI_N_AP_TYPE     = 0x06, 
    /* Authentication Protocol */

  HDRHAI_N_EP_TYPE     = 0x07, 
    /* Encryption Protocol */

  HDRHAI_N_SP_TYPE     = 0x08, 
    /* Security Protocol */

  HDRHAI_N_PCP_TYPE    = 0x09, 
    /* Packet Consolidation Protocol */

  HDRHAI_N_ALMP_TYPE   = 0x0A, 
    /* Air Link Management Protocol */

  HDRHAI_N_ISP_TYPE    = 0x0B, 
    /* Initialization State Protocol */

  HDRHAI_N_IDP_TYPE    = 0x0C, 
    /* Idle State Protocol */

  HDRHAI_N_CSP_TYPE    = 0x0D, 
    /* Connected State Protocol */

  HDRHAI_N_RUP_TYPE    = 0x0E, 
    /* Route Update Protocol */

  HDRHAI_N_OMP_TYPE    = 0x0F, 
    /* Overhead Messages Protocol */

  HDRHAI_N_SMP_TYPE    = 0x10, 
    /* Session Management Protocol */

  HDRHAI_N_AMP_TYPE    = 0x11, 
    /* Address Management Protocol */

  HDRHAI_N_SCP_TYPE    = 0x12, 
    /* Session Configuration Protocol */

  HDRHAI_N_STR_TYPE    = 0x13, 
    /* Stream Protocol */

  HDRHAI_N_APP0_TYPE   = 0x14, 
    /* Stream 0 Application (Signalling)*/

  HDRHAI_N_APP1_TYPE   = 0x15, 
    /* Stream 1 Application */

  HDRHAI_N_APP2_TYPE   = 0x16, 
    /* Stream 2 Application */

  HDRHAI_N_APP3_TYPE   = 0x17, 
    /* Stream 3 Application */

  HDRHAI_N_BCMCS_TYPE  = 0x18, 
    /* Broadcast Multicast Services */

  HDRHAI_N_VSP_TYPE    = 0x19, 
    /* Virtual Stream Protocol */

  HDRHAI_N_VSAP_TYPE   = 0x1A, 
    /* Virtual Stream Application VS (0x1Avs).
       Virtual Stream prefix where the subsequent byte specifies the virtual 
       stream ID from 0x01 to 0xFF */
                                 
  HDRHAI_N_MCD_TYPE    = 0x1B, 
    /* Generic Multimode Capability Discovery Protocol */

  HDRHAI_N_XXX_TYPE    = 0x1C,
    /* Undefined */

  HDRHAI_N_SAP_TYPE     = 0x1D,
    /* Signaling Adaptation Protocol */

  HDRHAI_PROTOCOL_TYPE_ENUM_MAX /* 0x1E */
} hdrcom_hai_protocol_type_enum_type;

typedef enum
{
/*--------------------------*/
/* Based on standard values */
    HDRHAI_PHYSICAL_LAYER_PROTOCOL       = HDRHAI_N_PHY_TYPE,
    HDRHAI_CC_MAC_PROTOCOL               = HDRHAI_N_CCMP_TYPE,
    HDRHAI_AC_MAC_PROTOCOL               = HDRHAI_N_ACMP_TYPE,
    HDRHAI_FTC_MAC_PROTOCOL              = HDRHAI_N_FTMP_TYPE,
    HDRHAI_RTC_MAC_PROTOCOL              = HDRHAI_N_RTMP_TYPE,
    HDRHAI_KEY_EXCHANGE_PROTOCOL         = HDRHAI_N_KEP_TYPE,
    HDRHAI_AUTHENTICATION_PROTOCOL       = HDRHAI_N_AP_TYPE,
    HDRHAI_ENCRYPTION_PROTOCOL           = HDRHAI_N_EP_TYPE,
    HDRHAI_SECURITY_PROTOCOL             = HDRHAI_N_SP_TYPE,
    HDRHAI_PACKET_CONSOLIDATION_PROTOCOL = HDRHAI_N_PCP_TYPE,
    HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL  = HDRHAI_N_ALMP_TYPE,
    HDRHAI_INITIALIZATION_STATE_PROTOCOL = HDRHAI_N_ISP_TYPE,
    HDRHAI_IDLE_STATE_PROTOCOL           = HDRHAI_N_IDP_TYPE,
    HDRHAI_CONNECTED_STATE_PROTOCOL      = HDRHAI_N_CSP_TYPE,
    HDRHAI_ROUTE_UPDATE_PROTOCOL         = HDRHAI_N_RUP_TYPE,
    HDRHAI_OVERHEAD_MESSAGES_PROTOCOL    = HDRHAI_N_OMP_TYPE,
    HDRHAI_SESSION_MANAGEMENT_PROTOCOL   = HDRHAI_N_SMP_TYPE,
    HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL   = HDRHAI_N_AMP_TYPE,
    HDRHAI_SESSION_CONFIGURATION_PROTOCOL = HDRHAI_N_SCP_TYPE,
    HDRHAI_STREAM_PROTOCOL               = HDRHAI_N_STR_TYPE,
    HDRHAI_SIGNALING_LINK_PROTOCOL       = HDRHAI_N_APP0_TYPE,
    HDRHAI_BROADCAST_PROTOCOL            = HDRHAI_N_BCMCS_TYPE,
    HDRHAI_VIRTUAL_STREAM_PROTOCOL       = HDRHAI_N_VSP_TYPE,
    HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL  = HDRHAI_N_MCD_TYPE, 
    /* 0x1C */
    HDRHAI_SIGNALING_ADAPTATION_PROTOCOL = HDRHAI_N_SAP_TYPE,
    HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX, /* 0x1E */
  
    /*--------------------*/
    /* Defined internally */


    /* Protocols defined internally are moved to a different range to avoid 
     * compatibility issues with QXDM as more protocols are added in the 
     * standard. QXDM payload for events using the protocol ID are defined
     * as 2 byte values, so add new internal protocols at the min value.
     *
     * Current protocol max = 0x1E
     * Current internal protocol min = 0x25
    */
    HDRHAI_INTERNAL_PROTOCOL_MIN                = 0x25,
    HDRHAI_TX                                   = 0x25,
    HDRHAI_MODULATOR                            = 0x26,
    HDRHAI_SAP_TX_MODULE                        = 0x27,
    HDRHAI_SAP_RX_MODULE                        = 0x28,
    HDRHAI_EXTERNAL_MODULE                      = 0x29,
    HDRHAI_DECODER                              = 0x2A,
    HDRHAI_CM                                   = 0x2B,
    HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL      = 0x2C,
    HDRHAI_LOGGING_MODULE                       = 0x2D,
    HDRHAI_RESERVATION_PROTOCOL                 = 0x2E,
    HDRHAI_SESSION_CONFIG_MANAGER_DATABASE      = 0x2F,
    HDRHAI_HDRMC                                = 0x30,
    HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL        = 0x31,
    HDRHAI_DATA_OVER_SIGNALING_PROTOCOL         = 0x32,
    HDRHAI_SESSION_CONFIG_MANAGER               = 0x33,
    HDRHAI_BROADCAST_MAC_PROTOCOL               = 0x34,
    HDRHAI_BROADCAST_FRAMING_PROTOCOL           = 0x35,

    HDRHAI_RADIO_LINK_PROTOCOL                  = 0x36,
    HDRHAI_RX                                   = 0x37,
    HDRHAI_HDR_MESSAGING_PROTOCOL               = 0x38,
    HDRHAI_LOCATION_UPDATE_PROTOCOL             = 0x39,
    HDRHAI_LINK_MANAGEMENT_AND_CONTROL_PROTOCOL = 0x3A,
    HDRHAI_FLOW_CONTROL_PROTOCOL                = 0x3B,
    HDRHAI_PACKET_APPLICATION_CONTROLLER        = 0x3C,
    HDRHAI_IS890_FTAP                           = 0x3D,
    HDRHAI_IS890_RTAP                           = 0x3E,
    HDRHAI_AN_AUTHENTICATION_STATE_MODULE       = 0x3F,
    HDRHAI_PROTOCOL_NAME_ENUM_MAX               = 0x40,
    /* Don't add new protocols here, add them at HDRHAI_INTERNAL_PROTOCOL_MIN */
    HDRHAI_PROTOCOL_NAME_FORCE_32_BIT           = 0x7FFFFFFF
    /* To force the size of the enum to 32-bits */
} hdrcom_hai_protocol_name_enum_type;

typedef enum
{
  HDRHAI_IN_USE,            /* In-use instance of protocol */
  HDRHAI_IN_CONFIG,             /* In-config instance of protocol */
  HDRHAI_PROTOCOL_INSTANCE_ENUM_MAX,
  HDRHAI_PROTOCOL_INSTANCE_FORCE_32_BIT = 0x7FFFFFFF
  /* To force the size of the enum to 32-bits */
} hdrcom_hai_protocol_instance_enum_type;

/* Enumeration of streams */
typedef enum
{
  HDRHAI_STREAM0 = 0,           /* Stream 0 */
  HDRHAI_STREAM1 = 1,           /* Stream 1 */
  HDRHAI_STREAM2 = 2,           /* Stream 2 */
  HDRHAI_STREAM3 = 3           /* Stream 3 */
} hdrcom_hai_stream_enum_type;

/* Enumeration of channels */
typedef enum
{
  HDRHAI_CC, /* Control Channel */
  HDRHAI_AC, /* Access Channel */
  HDRHAI_FTC, /* Forward Traffic Channel */
  HDRHAI_RTC, /* Reverse Traffic Channel */
  HDRHAI_AC_OR_RTC, /* Either Access or Reverse Traffic Channel */
  HDRHAI_UNKNOWN_CHANNEL /* Unknown channel */
} hdrcom_hai_channel_enum_type;

/* Enumeration for the abort reason */
typedef enum
{
  HDRHAI_ABORT_NOT_ABORTING,
  HDRHAI_ABORT_UNSPECIFIED,
  HDRHAI_ABORT_RESELECTION,        /* reselection to another system/mode */
  HDRHAI_ABORT_MAX_ACC_PROBE,      /* Access Failure due to max probes   */
  HDRHAI_ABORT_SYS_LOST,           /* System lost                        */
  HDRHAI_ABORT_REDIRECT_CHG_HDR,   /* Redirect to another HDR system     */
  HDRHAI_ABORT_REDIRECT_EXIT_HDR,  /* Redirection to Non HDR system      */
  HDRHAI_ABORT_NOT_PREF_CHG_HDR,   /* SD PRL wants new HDR system        */
  HDRHAI_ABORT_NOT_PREF_EXIT_HDR,  /* SD PRL wants non-HDR system        */

  HDRHAI_ABORT_USER,               /* User wants to abort                */
  HDRHAI_ABORT_NORMAL_CLOSE,       /* Connection closed normally         */
  HDRHAI_ABORT_OFFLINE,
  HDRHAI_ABORT_PREF_SYS_CHANGE,
  HDRHAI_ABORT_NAM_CHANGE,
  HDRHAI_ABORT_POWERDOWN,
  HDRHAI_ABORT_LPM,
  HDRHAI_ABORT_USER_RUDE_CLOSE,    /* Close conenction right away        */
  HDRHAI_ABORT_RESELECT,           /* Reselect system (CloseReason 011)  */
  HDRHAI_ABORT_MSG,                /* Abort all messages  */
  HDRHAI_ABORT_ERROR,              /* Connection Error (CloseReason 010) */
  HDRHAI_ABORT_SAR_LIMIT,          /* Connection closed due to sar limit */
  HDRHAI_ABORT_ACQ_OTHERS,         /* Connection closed due to acq attempt
                                      of another RAT */
  HDRHAI_ABORT_IRAT_TO_GWL,        /* Reselection to GWL */
  HDRHAI_ABORT_EXIT_TUNNEL,        /* Deactivate command received when 
                                      exiting tunnel mode */
   HDRHAI_ABORT_D2L_FAILURE         /* D2L failure or abort */
} hdrcom_hai_abort_reason_enum_type;

/* Enumeration of applications for streams */
typedef enum
{
  HDRHAI_DEF_SIG_APP          = 0x0000, /* Default Signaling Application */
  HDRHAI_DEF_PACKET_APP_AN    = 0x0001, /* Default Packet Application for AN */
  HDRHAI_DEF_PACKET_APP_SN    = 0x0002, /* Default Packet Application for SN */
  HDRHAI_DEF_PACKET_APP_IS890 = 0x0003, /* Default test application (IS-890) */
  HDRHAI_MULTIFLOW_PACKET_APP_AN = 0x0004, /* MultiFlow Packet Application for AN */
  HDRHAI_MULTIFLOW_PACKET_APP_SN = 0x0005, /* MultiFlow Packet Application for SN */
  HDRHAI_3G1X_CSN_APP         = 0x0006, /* 3G1X Circuit Services notification Application */
  HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_AN = 0x0008, /* Enhanced MultiFlow Packet Application for AN */
  HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN = 0x0009, /* Enhanced MultiFlow Packet Application for SN */
  HDRHAI_ENH_TEST_APP_IS890A  = 0x000A, /* Rel. A test application (IS-890A) */
  HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_AN = 0x000C, /* MultiLink MultiFlow Packet Application for AN */
  HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN = 0x000D, /* MultiLink MultiFlow Packet Application for SN */
  HDRHAI_MC_TEST_APP_IS890B   = 0x000E, /* Rel. B test application (IS-890B) */
  HDRHAI_APP_PACKET_APP_ANY   = 0xFEEE, /* Needed by ALMP, non-IS856 value */
  HDRHAI_ALT_MULTILINK_MULTIFLOW_PACKET_APP_SN = 0xFFFC, /* Alternate Multilink MultiFlow Packet 
                                                            Application for SN */
  HDRHAI_ALT_ENHANCED_MULTIFLOW_PACKET_APP_SN = 0xFFFE, /* Alternate Enhanced MultiFlow Packet Application for SN */
  HDRHAI_APP_STREAM_NOT_USED  = 0xFFFF  /* No Application - stream not used */
} hdrcom_hai_app_subtype_enum_type;


typedef enum
{
  HDRHAI_SCHG_OPENED_NORMAL = 0,           /* Session opened reason */
  HDRHAI_SCHG_OPENED_RESTORE_PRIOR,        /* Opening a prior session reason */
  HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS, /* Opening a prior session combined
                                              with personality switch reason */
  HDRHAI_SCHG_CLOSED_NORMAL,               /* Session closed reason */
  HDRHAI_SCHG_CLOSED_RESTORE_PRIOR,        /* Session closed; acquired 
                                              on new network reason */
  HDRHAI_SCHG_PERSONALITY_SWITCH           /* Personality switch reason */

} hdrcom_hai_session_change_reason_enum_type;

/* Enumeration for the open reason */
typedef enum
{
  HDRHAI_OPEN_UNSPECIFIED,    /* Unspecified reason why connection was opened */
  HDRHAI_OPEN_USER_REQ,       /* User has requested to open a connection */
  HDRHAI_OPEN_SEND_RTC_MSG,   /* A protocol needs to send a message on the RTC */
  HDRHAI_OPEN_AN,             /* AN requests to open a connection */
  HDRHAI_OPEN_NO_DATA,        /* Open a connection to provide a positive
                                 acknowledgement for the DS that data session
                                 is moved to HDR note: no user data to be sent */
  HDRHAI_OPEN_ABORT,           /* aborted connection */
  HDRHAI_OPEN_ACTIVE_HO        /* Active handoff connection */
} hdrcom_hai_open_reason_enum_type;


/* Enumeration for the session close reason */
typedef enum
{
  HDRHAI_CLOSE_NEW_NETWORK,    /* AMPfail: reacquired on a new network */
  HDRHAI_CLOSE_UATI_FAIL,      /* AMPfail: timed out 5x waiting for UATI rsp */
  HDRHAI_CLOSE_KA_EXP,         /* KeepAliveTimer not reset for TsmpClose min */
  HDRHAI_CLOSE_DEACTIVATE,     /* Internal Deactivation */
  HDRHAI_CLOSE_REPLY,          /* Rxed a session close msg from the AN */
  HDRHAI_CLOSE_CONN_OPEN_FAIL, /* Failed to establish a connection 5x to send
                                  a session config message */
  HDRHAI_CLOSE_CFG_MSG_FAIL,   /* in ATInit: Couldn't send Config msg */
  HDRHAI_CLOSE_CFG_RSP_EXP,    /* in ATInit: Timed out waiting for config
                                  response */
  HDRHAI_CLOSE_PROT_NEG_FAIL,  /* in ATInit: Bad Config Response from AN */
  HDRHAI_CLOSE_AN_INIT_EXP,    /* in ANInit: AN init setup timer expired */
  HDRHAI_CLOSE_QUICK_FAILURE,  /* in ANInit: Connection closed in ANInit */
  HDRHAI_CLOSE_CONN_OPEN_DENY, /* Failed to establish a connection 5x for
                                  sending config message.  Rxed a connection
                                  deny at least once from the network. */
  HDRHAI_CLOSE_SILENT_DEACTIVATE, /* Internal Silent Deactivation */
  HDRHAI_CLOSE_NEW_ESN,        /* AMPfail: phone ESN different from ESN
                                  associated with our current session */
  HDRHAI_CLOSE_AN_GAUP_FAIL,   /* AT Rejected AN GAUP message */
  HDRHAI_CLOSE_PERSONALITY_INDEX_INVALID, /* AN included invalid personality
                                             index in SoftCC message */
  HDRHAI_CLOSE_NOT_MAINT_UATI, /* AMP: close the session due to not 
                                  maintaining the UATI. */
  HDRHAI_CLOSE_NEW_NAI,        /* phone NAI different from NAI associated with
                                  our current session */
  HDRHAI_CLOSE_EHRPD_AVAILABILITY_CHANGED, /* eHRPD availability (APN, 
                                              network setting, eHRPD credentials) 
                                              has changed */
  HDRHAI_CLOSE_IRAT_CHANGED,   /* IRAT capability changed */
  HDRHAI_CLOSE_RESET           /* session close due to reset */
} hdrcom_hai_session_close_reason_enum_type;

/* Enumeration of all indications */
typedef enum
{
  HDRIND_SBOOT_BOOT_COMPLETED,
  HDRIND_SBOOT_SESSION_ABORTED,
  HDRIND_SCTL_SESSION_CLOSED,
  HDRIND_ALMP_REACQUIRING,
  HDRIND_ALMP_EXIT_HDR_MODE,
  HDRIND_OVHD_INFO_UPDATED,
  HDRIND_OVHD_AP_UPDATED,
  HDRIND_OVHD_SUPERVISION_FAILED,
  HDRIND_OVHD_CHANGE_HDR_SYSTEMS,
  HDRIND_OVHD_EXIT_HDR_MODE,
  HDRIND_INIT_NETWORK_ACQUIRED,
  HDRIND_INIT_EXIT_HDR_MODE,
  HDRIND_IDLE_CONNECTION_OPENED,
  HDRIND_IDLE_CONNECTION_FAILED,
  HDRIND_IDLE_CHANGE_HDR_SYSTEMS,
  HDRIND_IDLE_EXIT_HDR_MODE,
  HDRIND_CONNECTED_CONNECTION_CLOSED,
  HDRIND_RTCMAC_SHUTDOWN_COMPLETE, /* Was HDRIND_CONNECTED_CLOSE_MSG_SENT */
  HDRIND_RUP_CONN_SETUP_FAILED,
  HDRIND_RUP_FREQ_CHANGE_INITIATED,
  HDRIND_RUP_ASSIGNMENT_REJECTED,
  HDRIND_CCMAC_MOBILE_DIRECTED_PKT,
  HDRIND_CCMAC_SUPERVISION_FAILED,
  HDRIND_CCMAC_END_OF_SYNC_CAPSULE, /* obsolete */
  HDRIND_ACMAC_TX_STARTED,
  HDRIND_ACMAC_TX_ENDED,
  HDRIND_ACMAC_TRANSMISSION_SUCCESSFUL,
  HDRIND_ACMAC_TRANSMISSION_FAILED,
  HDRIND_ACMAC_TRANSMISSION_ABORTED,
  HDRIND_ACMAC_TRANSMISSION_STOPPED,
  HDRIND_RTCMAC_DRC_TX_STOPPED, /* was HDRIND_FTCMAC_STOP_TX */
  HDRIND_RTCMAC_DRC_TX_RESTARTED, /* was HDRIND_FTCMAC_RESTART_TX */
  HDRIND_FTCMAC_SUPERVISION_FAILED,
  HDRIND_RTCMAC_LINK_ACQUIRED,
  HDRIND_RTCMAC_TX_STOPPED,
  HDRIND_LMAC_ACQUIRED,
  HDRIND_LMAC_ACQUISITION_FAILED,
  HDRIND_LMAC_NETWORK_LOST,
  HDRIND_LMAC_IDLE_HANDOFF,
  HDRIND_LMAC_AWAKE,
  HDRIND_LMAC_ACCESS_MODE_ENTERED,
  HDRIND_LMAC_AHO_REQUEST,
  HDRIND_LMAC_AHO_COMPLETE,
  HDRIND_LMAC_FREQ_CHANGE_COMPLETE,
  HDRIND_LMAC_SEND_PILOT_REPORT,
  HDRIND_LMAC_CHAN_MEAS_COMPLETE,
  HDRIND_SMP_SESSION_CLOSED,
  HDRIND_SMP_BOOT_COMPLETED,
  HDRIND_SMP_SESSION_OPENED,
  HDRIND_AMP_OPENED,
  HDRIND_AMP_FAILED_REQUEST_TIMEOUT,
  HDRIND_AMP_FAILED_NEW_NETWORK,
  HDRIND_AMP_UATI_ASSIGNED,
  HDRIND_AMP_UATI_RELEASED,
  HDRIND_SCP_RECONFIGURED, /* obsolete */
  HDRIND_SCP_PREV_SESSION_OPENED, /* obsolete */
  HDRIND_SCP_FAILED,
  HDRIND_IDLE_PROT_NEG_FAILED,
  HDRIND_IDLE_CONFIG_COMPLETE,
  HDRIND_IDLE_CONFIG_REQ,
  HDRIND_ALMP_RETURNING_TO_IDLE,
  HDRIND_CDMA_IMSI_CHANGED,
  HDRIND_LUP_PROT_NEG_FAILED, /* obsolete */
  HDRIND_LUP_CONFIG_COMPLETE, /* obsolete */
  HDRIND_LUP_CONFIG_REQ, /* obsolete */
  HDRIND_STREAM_PROT_NEG_FAILED,
  HDRIND_STREAM_CONFIG_COMPLETE,
  HDRIND_STREAM_CONFIG_REQ,
  HDRIND_FCP_DATA_READY_RCVD,
  HDRIND_RUP_CONNECTION_INITIATED,
  HDRIND_KEP_PROT_NEG_FAILED,
  HDRIND_FCP_ALL_STREAMS_CLOSED, /* obsolete */
  HDRIND_CCMAC_SLEEP_CAPSULE_DONE, /* obsolete */
  HDRIND_LMAC_ACCESS_ENTRY_HANDOFF,
  HDRIND_LMAC_IDLE_SUSPEND,
  HDRIND_LMAC_IDLE_RESUME,
  HDRIND_LMAC_CC_MISSED,
  HDRIND_LMAC_RESELECT,
  HDRIND_LUP_ALN_TRIGGER,
  HDRIND_LMAC_HDR_USABLE,
  HDRIND_LMAC_DEACTIVATE_DONE,
  HDRIND_AMP_FAILED_NEW_ESN,
  HDRIND_SCM_PROT_NEG_FAILED,
  HDRIND_SCM_CONFIG_COMPLETE,
  HDRIND_SCM_CONFIG_REQ,   
  HDRIND_BCMCS_FREQ_CHANGE_INITIATED, /* obsolete */
  HDRIND_SCM_RMAC_GAUP,
  HDRIND_SCM_MRLP_GAUP,
  HDRIND_RUP_CHANGE_HDR_SYSTEMS,
  HDRIND_RUP_EXIT_HDR_MODE,
  HDRIND_IDLE_CONNECTION_ABORTED,
  HDRIND_LMAC_TC_SUSPEND,    
  HDRIND_LMAC_TC_RESUME,
  HDRIND_CCMAC_INTERRUPTED,
  HDRIND_SCM_MRLP_AT_GAUP_ACCEPTED,
  HDRIND_SCM_MRLP_AT_GAUP_REJECTED,
  HDRIND_SCM_MRLP_AT_GAUP_FAILED,
  HDRIND_OVHD_BC_UPDATED, /* obsolete */
  HDRIND_SCM_FMAC_GAUP,
  HDRIND_SCM_SESSION_CHANGED,
  HDRIND_SCM_RMAC_AT_GAUP_ACCEPTED,
  HDRIND_SCM_RMAC_AT_GAUP_REJECTED,
  HDRIND_SCM_RMAC_AT_GAUP_FAILED,
  HDRIND_ALMP_CONNECTION_CLOSED,
  HDRIND_RMAC3_FLOW_ACTIVATED,  /* obsolete */
  HDRIND_RMAC3_FLOW_DEACTIVATED,  /* obsolete */
  HDRIND_RMAC3_FLOW_MAPPING_CHANGED,  /* obsolete */
  HDRIND_IDLE_MONITOR_CONNECTION_FAILED,
  HDRIND_GMMCDP_CONFIG_COMPLETE,
  HDRIND_GMMCD_PROT_NEG_FAILED,
  HDRIND_SCM_AMAC_GAUP,
  HDRIND_SCM_AMAC_AT_GAUP_ACCEPTED,
  HDRIND_SCM_AMAC_AT_GAUP_REJECTED,
  HDRIND_SCM_AMAC_AT_GAUP_FAILED,
  HDRIND_RUP_CONFIG_COMPLETE,
  HDRIND_RUP_PROT_NEG_FAILED,
  HDRIND_RMAC_CONFIG_COMPLETE,
  HDRIND_RMAC_PROT_NEG_FAILED,
  HDRIND_SCM_AMP_GAUP,
  HDRIND_SCP_AN_GAUP_FAILED,
  HDRIND_IDLE_AN_GAUP_FAILED,
  HDRIND_RUP_AN_GAUP_FAILED,
  HDRIND_PACC_AN_GAUP_FAILED,
  HDRIND_SCM_AN_GAUP_FAILED,
  HDRIND_SCP_CONFIG_UNLOCKED,
  HDRIND_BCMCS_SLEEP_OK,
  HDRIND_AMP_FAILED_NOT_MAINTAINING_UATI,
  HDRIND_SCP_NO_PREV_SESSION,
  HDRIND_ACMAC_READY,
  HDRIND_ACMAC_TUNE_AWAY_TIME_OK,
  HDRIND_SCM_IDLE_AT_GAUP_ACCEPTED,
  HDRIND_SCM_IDLE_AT_GAUP_REJECTED,
  HDRIND_SCM_IDLE_AT_GAUP_FAILED,
  HDRIND_IDLE_NETWORK_LOST,
  HDRIND_SCM_CSNA_AN_GAUPED,
  HDRIND_SCM_CSNA_AT_CONFIG_COMPLETE,
  HDRIND_CSNA_PROT_NEG_FAILED,
  HDRIND_CSNA_3G1X_PARM_RECEIVED,
  HDRIND_CSNA_3G1X_PARM_RESET_RECEIVED,
  HDRIND_RUP_ASSIGNMENT_FAILED,
  HDRIND_SCM_SCP_AT_GAUP_ACCEPTED,
  HDRIND_SCM_SCP_AT_GAUP_REJECTED,
  HDRIND_SCM_SCP_AT_GAUP_FAILED,
  HDRIND_AMP_OPENED_PREV_SESSION,
  HDRIND_CSNA_TX_MSG_STATUS_RECEIVED,
  HDRIND_LMAC_RF_CONDITION_CHANGED,
  HDRIND_RUP_PILOT_RPT_RECEIVED,
  HDRIND_IDLE_AN_GAUP_SLOTTEDMODE,
  HDRIND_AMP_FAILED_NEW_NAI,
  HDRIND_OUTAGE_IMMINENT,
  HDRIND_AMAC_CONFIG_COMPLETE,
  HDRIND_AMAC_PROT_NEG_FAILED,
  HDRIND_RSV1,  /* reserved for HDRIND_BCCP_CONFIG_COMPLETE */
  HDRIND_RSV2,  /* reserved for HDRIND_BCCP_PROT_NEG_FAILED */
  HDRIND_CCMAC_SCC_PKT_RXED,
  HDRIND_LMAC_IRAT_PLT_MEAS_DONE_IND,
  HDRIND_SCP_EHRPD_AVAILABILITY_CHANGED,
  HDRIND_FMAC_PROT_NEG_FAILED,
  HDRIND_FMAC_CONFIG_COMPLETE,
  HDRIND_LMAC_SCHEDULER_GROUP_CHANGED,
  HDRIND_PHY_CONFIG_COMPLETE,
  HDRIND_PHY_PROT_NEG_FAILED,
  HDRIND_RTC_RESTART_TIMER_EXPIRED,
  HDRIND_RMAC_FLOW_ACTIVATED,
  HDRIND_RMAC_FLOW_DEACTIVATED,
  HDRIND_RMAC_FLOW_MAPPING_CHANGED,
  HDRIND_LMAC_CARRIER_MAP_CHANGED,
  HDRIND_SCM_PHY_AN_GAUP,
  HDRIND_LMAC_TCA_FAILED,
  HDRIND_LMAC_CONN_ERROR,
  HDRIND_LMAC_TIME_SYNC_DONE,
  HDRIND_LMAC_FAST_ACQ_DONE,
  HDRIND_LMAC_TIMING_INFO,
  HDRIND_SCM_FMAC_AT_GAUP_ACCEPTED,
  HDRIND_SCM_FMAC_AT_GAUP_REJECTED,
  HDRIND_SCM_FMAC_AT_GAUP_FAILED,
  HDRIND_LMAC_IRAT_RESELECT,
  HDRIND_LMAC_ASET_CHANGED,
  HDRIND_OVHD_NLB_CACHE_UPDATED,
  HDRIND_MC_QOS_FLOW_STATUS_CHANGED,
  HDRIND_OVHD_PREREG_INFO_UPDATED,
  HDRIND_SAP_CONNECTION_OPENED,
  HDRIND_SAP_CONNECTION_INITIATED,
  HDRIND_SAP_CONNECTION_CLOSED,
  HDRIND_SAP_CONNECTION_FAILED,
  HDRIND_SAP_IDLE_HANDOFF,
  HDRIND_SAP_LINK_ACQUIRED,
  HDRIND_SAP_ALT_LINK_COMP_CB_RECEIVED,
  HDRIND_SAPTX_TRANSMISSION_FAILED_AC,
  HDRIND_SAPTX_TRANSMISSION_CNF_AC,
  HDRIND_SAP_BACKOFF_TIMER_EXPIRED,
  HDRIND_SAP_BACKOFF_TIMER_STOPPED,
  HDRIND_AMP_PREREG_ZONE_CHANGED,
  HDRIND_SAP_TUNNELMODE_STATUS_CHANGED,
  HDRIND_SAP_CONNECTION_ALREADY_OPENED,  
  HDRIND_IDLE_CONNECTION_ALREADY_OPENED,
  HDRIND_AMP_IRMI_ACCEPTED,
  HDRIND_AMP_IRMI_FAILED,
  HDRIND_OVHD_LTE_CELL_CHANGED,
  HDRIND_IDLE_CONNECTION_NOT_ALLOWED,
  HDRIND_OVHD_COLOR_CODE_CHANGED,
  HDRIND_ALMP_RESELECT_EXIT_HDR,
  HDRIND_SAP_PREREG_STATUS_UPDATED,
  HDRIND_SAP_RETURNING_TO_CLOSE,
  HDRIND_SAP_TRANSMISSION_STOPPED,
  HDRIND_SCP_IRAT_CAPABILITY_CHANGED,
  HDRIND_RUP_ACT_HO_COMPLETED,
  HDRIND_LMAC_SYNC_SUSPEND,
  HDRIND_LMAC_SYNC_RESUME,
  HDRIND_LMAC_IDLE_SUSPEND_TO_REACQ,
  HDRIND_LMAC_ORAT_MEAS_RPT,
  HDRIND_LMAC_ORAT_MEAS_FAILED,
  HDRIND_LMAC_ORAT_REDIR_SUCCESS,
  HDRIND_IND_NAME_ENUM_MAX
}  hdrcom_ind_name_enum_type;

typedef enum
{
  HDRHAI_TUNNEL_MODE_DISABLED  = 0,
  HDRHAI_TUNNEL_MODE_ENABLED   = 1,
  HDRHAI_TUNNEL_MODE_ACTIVE_HO = 2,
  HDRHAI_TUNNEL_MODE_NO_SRV    = 3 /* In LTE coverage but no service */
} hdrcom_hai_tunnel_mode_type;

/* At what point the UE considers itself to be pre-Registered and
 * accordingly set pre-Registration status to TRUE */
typedef enum
{
  HDRHAI_PREREG_STATUS_UATI_ASSIGNED,        /* When UATI address has been assigned */
  HDRHAI_PREREG_STATUS_EVDO_SESSION_OPENED,  /* When EVDO session has been opened */
  HDRHAI_PREREG_STATUS_DEFAULT = HDRHAI_PREREG_STATUS_EVDO_SESSION_OPENED,
  HDRHAI_PREREG_STATUS_PPP_SESSION_OPENED,   /* When PPP session has been opened */
  HDRHAI_PREREG_STATUS_IP_SESSION_OPENED,    /* When IP context has been created */
  HDRHAI_PREREG_STATUS_FULL_CONTEXT,         /* When EVDO/PPP/IP/QOS context have been created */
  HDRHAI_PREREG_STATUS_MAX

} hdrcom_hai_prereg_status_enum_type;

typedef enum
{
  HDRIND_LMAC_RF_CONDITION_DONOT_CARE = 0,
  HDRIND_LMAC_RF_CONDITION_BAD        = 1,
  HDRIND_LMAC_RF_CONDITION_GOOD       = 2,
  HDRIND_LMAC_RF_CONDITION_MAX
} hdrcom_ind_lmac_rf_change_enum_type;


typedef struct
{
  /*!< whether the timing info is available or not */
  boolean status;

  /*!< LTE RTC counter */
  uint32          lte_ostmr;

  /*!< CDMA System time */
  uint64          cdma_sys_time;

} hdrcom_ind_lmac_timing_info_type;

typedef struct
{
  lte_earfcn_t     earfcn;
    /* Frequency of the LTE cell to be selected */
  
  uint16           cell_id;
    /* Cell id of the LTE cell to be selected */

} hdrcom_ind_lmac_lte_resel_info_type;

/* Intra SCC information */
typedef struct
{
  boolean last_pkt;
    /* Flag if the last packet is received */

  boolean sleep_caps_done;
    /* Flag if the packet has SleepStateCapsuleDone bit set. */

  uint16 cc_start;
    /* CC start in slots */

} hdrcom_ind_cmac_intra_scc_info_type;

typedef struct
{
  hdrl1_srch_wakeup_reason_enum_type wakeup_reason;
  boolean reacq_succeded;

} hdrcom_ind_lmac_wakeup_status_type;

/* Indicate failure cause type */
typedef enum
{
  HDRIND_IRAT_TEMP_FAILURE = 0,       /* Temporary failure with backoff timer */
  HDRIND_IRAT_TEMP_FAILURE_TIMER = 1, /* Temporary failure withour timer */
  HDRIND_IRAT_PERM_FAILURE = 2        /* Permanent failure */
}hdrcom_ind_irat_failure_cause_enum_type;

/* Indicate failure reason type */
typedef enum
{
  HDRIND_IRAT_MEAS_GENERAL_FAILURE = 0,       /* D2L measurement failed */
  HDRIND_IRAT_MEAS_HARD_FAILURE = 1, /* D2L measurement passed, however LTE returned failure to DO */
  HDRIND_IRAT_MEAS_ABORTED = 2 /* D2L measurements aborted */
}hdrcom_irat_meas_fail_reason_enum_type;


typedef struct
{
  /* Failure cause type */
  hdrcom_ind_irat_failure_cause_enum_type      cause_type;

  /* backoff time before next retry */ 
  uint64                                   backoff_time;
}hdrcom_ind_irat_failure_ind_type;

/* Enumeration for Connection Layer Failure Reasons for Connection Attempt */
typedef enum
{
  HDRHAI_CONN_FAIL_IDLE_CONN_DENY = 0x1,        /* Receive ConnectionDeny */
  HDRHAI_CONN_FAIL_IDLE_CR_ROR = 0x2,           /* ROR msg failure */
  HDRHAI_CONN_FAIL_SAP_L2_FAILED = 0x4,         /* SAP L2 ACK failure */
  HDRHAI_CONN_FAIL_SAP_L3_FAILED = 0x8,         /* SAP L3 Response failure */
  HDRHAI_CONN_FAIL_SAP_TIMEOUT = 0x10,          /* SAP time out */
  HDRHAI_CONN_FAIL_SAP_NO_SESSION = 0x20,       /* No HDR session */
  HDRHAI_CONN_FAIL_SAP_PREREG_NOT_ALLOW = 0x40, /* PreReg is not allowed */
  HDRHAI_CONN_FAIL_SAP_ACTIVE_HO = 0x80,        /* Active Handoff in progess */
  HDRHAI_CONN_FAIL_SAP_NO_SRV = 0x100,          /* tunnel mode, LTE is OOS */
  HDRHAI_CONN_FAIL_SAP_NOT_ACTIVE = 0x200       /* SAP subtype is 0x0000 */

} hdrcom_conn_layer_failure_reason_enum_type;

/* The events that could trigger the change of preRegistration status */
typedef enum
{
  HDRHAI_PREREG_STATUS_EVENT_UATI_ASSIGNED,        /* UATI address has been assigned */
  HDRHAI_PREREG_STATUS_EVENT_EVDO_SESSION_OPENED,  /* EVDO session has been opened */
  HDRHAI_PREREG_STATUS_EVENT_EVDO_SESSION_CLOSED,  /* EVDO session has been closed */
  HDRHAI_PREREG_STATUS_EVENT_PPP_SESSION_OPENED,   /* PPP session has been opened */
  HDRHAI_PREREG_STATUS_EVENT_PPP_SESSION_CLOSED,   /* PPP session has been closed */
  HDRHAI_PREREG_STATUS_EVENT_VSNCP_ESTABILISHED,   /* VSNCP has been established */
  HDRHAI_PREREG_STATUS_EVENT_VSNCP_RELEASED,       /* VSNCP has been released */
  HDRHAI_PREREG_STATUS_EVENT_QOS_ESTABILISHED,     /* QOS context has been created */
  HDRHAI_PREREG_STATUS_EVENT_QOS_RELEASED,         /* QOS context has been released */
  HDRHAI_PREREG_STATUS_EVENT_MAX

} hdrcom_prereg_status_event_enum_type;

typedef struct
{
  hdrcom_conn_layer_failure_reason_enum_type failure_reason;
  uint64                                     backoff_time;

} hdrcom_ind_idle_conn_fail_info_type;

typedef union hdrcom_ind_data_union
{
  hdrcom_hai_session_change_reason_enum_type change_reason;
  /*~ IF ( _DISC_ == HDRIND_SCM_SESSION_CHANGED )
          hdrind_ind_data_union.change_reason */

  hdrl1_srch_link_id_type link_id;
  /*~ IF ( _DISC_ == HDRIND_LMAC_IDLE_HANDOFF || 
           _DISC_ == HDRIND_LMAC_AHO_COMPLETE ||
           _DISC_ == HDRIND_LMAC_FREQ_CHANGE_COMPLETE ) 
           hdrind_ind_data_union.link_id */

  hdrl1_srch_system_lost_enum_type sys_lost_reason;
  /*~ IF ( _DISC_ == HDRIND_LMAC_NETWORK_LOST )
           hdrind_ind_data_union.sys_lost_reason */

  boolean ovhd_updated_is_first_after_acq; /* This boolean indicates if the
                                              OVHD_INFO_UPDATED ind is the
                                              first one after acquistion */
  /*~ IF (_DISC_ == HDRIND_OVHD_INFO_UPDATED)
         hdrind_ind_data_union.ovhd_updated_is_first_after_acq */

  hdrcom_ind_idle_conn_fail_info_type conn_fail_info; /* indicates the connection failure reason
                             including ConDeny */
  /*~ IF (_DISC_ == HDRIND_IDLE_CONNECTION_FAILED ||
          _DISC_ == HDRIND_SAP_CONNECTION_FAILED)
         hdrind_ind_data_union.conn_fail_info */

  uint8 rup_conn_fail_reason; /* indicates the connection failure reason */
  /*~ IF (_DISC_ == HDRIND_RUP_CONN_SETUP_FAILED)
         hdrind_ind_data_union.rup_conn_fail_reason */

  boolean access_inhibited;  /* indicates that the access probe failed
                                because the apersistence parameter is
                                set to inhibit */
  /*~ IF (_DISC_ == HDRIND_ACMAC_TRANSMISSION_FAILED)
         hdrind_ind_data_union.access_inhibited */

  void *scp_fail_ptr; /* any protocol which receives a SCP.Failed Ind
                         must be in the same or higher task context.  If
                         a protocol from a lower task context queues the
                         indication, the data must be copied. */
  /*~ IF ( _DISC_ == HDRIND_SCP_FAILED ||
           _DISC_ == HDRIND_IDLE_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_STREAM_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_KEP_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_SCM_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_GMMCD_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_RUP_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_RMAC_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_SCP_AN_GAUP_FAILED ||
           _DISC_ == HDRIND_IDLE_AN_GAUP_FAILED ||
           _DISC_ == HDRIND_RUP_AN_GAUP_FAILED ||
           _DISC_ == HDRIND_PACC_AN_GAUP_FAILED ||
           _DISC_ == HDRIND_SCM_AN_GAUP_FAILED ||
           _DISC_ == HDRIND_AMAC_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_CSNA_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_FMAC_PROT_NEG_FAILED ||
           _DISC_ == HDRIND_PHY_PROT_NEG_FAILED )
         hdrind_ind_data_union.scp_fail_ptr */

  hdrcom_hai_open_reason_enum_type open_reason; /* reason connection was opened */
  /*~ IF ( _DISC_ == HDRIND_IDLE_CONNECTION_OPENED )
         hdrind_ind_data_union.open_reason */

  hdrcom_hai_stream_enum_type stream_id; /* Stream associated with Data Ready Msg*/
  /*~ IF ( _DISC_ == HDRIND_FCP_DATA_READY_RCVD )
         hdrind_ind_data_union.stream_id */

  hdrcom_hai_session_close_reason_enum_type close_reason; /* reason session was closed */
  /*~ IF ( _DISC_ == HDRIND_SMP_SESSION_CLOSED ) 
         hdrind_ind_data_union.close_reason */

  hdrcom_hai_protocol_name_enum_type prot_name; /* Protocol Name */
  /*~ IF ( _DISC_ == HDRIND_SCM_CONFIG_COMPLETE ||
           _DISC_ == HDRIND_SCM_CONFIG_REQ ||
           _DISC_ == HDRIND_GMMCDP_CONFIG_COMPLETE ||
           _DISC_ == HDRIND_RMAC_CONFIG_COMPLETE || 
           _DISC_ == HDRIND_AMAC_CONFIG_COMPLETE ||
           _DISC_ == HDRIND_FMAC_CONFIG_COMPLETE ||
           _DISC_ == HDRIND_PHY_CONFIG_COMPLETE )
         hdrind_ind_data_union.prot_name */
                                 
  uint8 gaup_trans_id;  /* Transaction ID associated with GAUP rsp */
  /*~ IF ( _DISC_ == HDRIND_SCM_MRLP_AT_GAUP_ACCEPTED ||
           _DISC_ == HDRIND_SCM_MRLP_AT_GAUP_REJECTED ||
           _DISC_ == HDRIND_SCM_MRLP_AT_GAUP_FAILED ||
           _DISC_ == HDRIND_SCM_RMAC_AT_GAUP_ACCEPTED || 
           _DISC_ == HDRIND_SCM_RMAC_AT_GAUP_REJECTED ||
           _DISC_ == HDRIND_SCM_RMAC_AT_GAUP_FAILED ||
           _DISC_ == HDRIND_SCM_IDLE_AT_GAUP_ACCEPTED ||
           _DISC_ == HDRIND_SCM_IDLE_AT_GAUP_REJECTED ||
           _DISC_ == HDRIND_SCM_IDLE_AT_GAUP_FAILED ||
           _DISC_ == HDRIND_SCM_AMAC_AT_GAUP_ACCEPTED ||
           _DISC_ == HDRIND_SCM_AMAC_AT_GAUP_REJECTED ||
           _DISC_ == HDRIND_SCM_AMAC_AT_GAUP_FAILED ||
           _DISC_ == HDRIND_SCM_SCP_AT_GAUP_ACCEPTED ||
           _DISC_ == HDRIND_SCM_SCP_AT_GAUP_REJECTED ||
           _DISC_ == HDRIND_SCM_SCP_AT_GAUP_FAILED)
         hdrind_ind_data_union.gaup_trans_id */

  uint8 flow_id; /* Flow ID associated with RMAC3 flows */
  /*~ IF ( _DISC_ == HDRIND_RMAC_FLOW_ACTIVATED ||
           _DISC_ == HDRIND_RMAC_FLOW_DEACTIVATED ||
           _DISC_ == HDRIND_RMAC_FLOW_MAPPING_CHANGED )
         hdrind_ind_data_union.flow_id  */

  boolean ta_time_ok;
    /* Tune away time is ok or not */
/*~ IF ( _DISC_ == HDRIND_ACMAC_TUNE_AWAY_TIME_OK ) 
      hdrind_ind_data_union.ta_time_ok */

  hdrcom_ind_lmac_wakeup_status_type wakeup_status;
    /* whether reacq succeeded during this wakeup or not, 
       along with the wakeup reason */
/*~ IF ( _DISC_ == HDRIND_IDLE_NETWORK_LOST ) 
      hdrind_ind_data_union.wakeup_status */

  boolean hho_is_in_progress; 
    /* indicates if the hho is in progress */
/*~ IF ( _DISC_ == HDRIND_CONNECTED_CONNECTION_CLOSED ) 
      hdrind_ind_data_union.hho_is_in_progress */

  void * csna_status_ptr;
    /* CSNA 3G1XServices message transmission status */
/*~ IF ( _DISC_ == HDRIND_CSNA_TX_MSG_STATUS_RECEIVED ) 
      hdrind_ind_data_union.csna_status_ptr */

  hdrcom_ind_lmac_rf_change_enum_type hdr_rf_condition;
    /* HDR RF condition measurement */
/*~ IF ( _DISC_ == HDRIND_LMAC_RF_CONDITION_CHANGED ) 
      hdrind_ind_data_union.hdr_rf_condition */

  hdrl1_mac_outage_notification_data_type outage_info;
    /* Contains traffic outage duration and time to outage in ms
       for VOIP optimization  */
/*~ IF ( _DISC_ == HDRIND_OUTAGE_IMMINENT ) 
      hdrind_ind_data_union.outage_info */

  uint8 carrier_id;
    /* Contains the carrier ID that pertains to the indication given */
/*~ IF ( _DISC_ == HDRIND_RTC_RESTART_TIMER_EXPIRED )
      hdrind_ind_data_union.carrier_id */

/*~ DEFAULT hdrind_ind_data_union.void */

  hdrcom_ind_cmac_intra_scc_info_type intra_scc_info; 
  /*~ IF (_DISC_ == HDRIND_CCMAC_SCC_PKT_RXED)
         hdrind_ind_data_union.intra_scc_info */

  hdrl1_srch_tran_sys_time_enum_type tran_sys_time_result;

  hdrl1_srch_fast_acq_enum_type fast_acq_result;

  hdrcom_ind_lmac_timing_info_type timing_info;

  hdrcom_ind_lmac_lte_resel_info_type lte_resel_info;

  boolean sar_power_limited; 
    /* indicates if supervision failed due to SAR power limit  */
/*~ IF ( _DISC_ == HDRIND_FTCMAC_SUPERVISION_FAILED ) 
      hdrind_ind_data_union.sar_power_limited */

  sys_link_type nlb_serving_sector;
/*~ IF ( _DISC_ == HDRIND_OVHD_NLB_CACHE_UPDATED ) 
      hdrind_ind_data_union.nlb_serving_sector */

  boolean prereg_is_allowed;
  boolean last_prereg_is_allowed;
  boolean first_cell_changed_ind;
  hdrcom_ind_irat_failure_ind_type irat_failure_info;
  uint8 tunnel_mode_value;
  uint8 sap_open_reason;
  uint8 color_code;
  hdrcom_prereg_status_event_enum_type prereg_status_event;
  hdrcom_srchltemeas_rpt_struct_type redirect_rpt;
  hdrcom_irat_meas_fail_reason_enum_type irat_meas_fail_reason;
} hdrcom_ind_data_union_type;

typedef enum
{
  HDRLOG_DIPSW_RESERVED        = BITMASK(0), 
    /* 0 - reserved, not compatible with current QXDM interface */

  HDRLOG_DIPSW_40S_SLEEP_OFF   = BITMASK(1), 
    /* 0 - 40s sleep enabled,             1 - disabled          */

  HDRLOG_DIPSW_SLEEP_OFF       = BITMASK(2),
    /* 0 - Sleep enabled, 1 - disable sleeping all together     */

  HDRLOG_DIPSW_RPC_OFF         = BITMASK(3),
    /* 0 - Reverse Power Control enabled, 1 - disabled          */

  HDRLOG_DIPSW_THROTTLING_OFF  = BITMASK(4),
    /* 0 - Throttling enabled,            1 - disabled          */

  HDRLOG_DIPSW_EARLY_TERM_OFF  = BITMASK(5),
    /* 0 - Early termination enabled,     1 - disabled          */

  HDRLOG_DIPSW_RL_RATE_INERTIA = BITMASK(6),
    /* 0 - RL Rate Inertia disabled,      1 - enabled          */

  HDRLOG_DIPSW_ANT1_TEST_MODE  = BITMASK(7),
    /* 0 - Ant1 test mode disabled,       1 - enabled          */

  HDRLOG_DIPSW_DDTM            = BITMASK(8),
    /* 0 - DDTM disabled,                 1 - enabled           */

  HDRLOG_DIPSW_VT_RATE_INERTIA = BITMASK(9),
    /* 0 - RMAC1 rate_params normal,      1 - rate_params VT    */

  HDRLOG_DIPSW_TAP_TCOFS       = BITMASK(10),
    /* 0 - TC OFS is disabled,            1 - enabled          */

  HDRLOG_DIPSW_FLAT_OFF        = BITMASK(11),
    /* 0 - FLAT off is disabled (FLAT is on), 1 - enabled      */

  HDRLOG_DIPSW_DISABLE_MUP_FILTERING  = BITMASK(12),
    /* 0 - MUP Filtering Enabled, 1 - MUP filtering Disabled */

  HDRLOG_DIPSW_MAX = BITMASK(HDRCOM_LOG_DIPSW_NUM - 1) 
    /* up to 32 bits max, for bounds checking */

} hdrcom_log_dipsw_enum_type;
/* list of available dip switches */

/* Indication buffer type */
typedef struct
  {
    hdrcom_ind_name_enum_type ind_name;  /* Global enum of indications */
    hdrcom_ind_data_union_type ind_data; /* Global union for ind. data */
  }
hdrcom_buf_ind_type;
/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/


/*===========================================================================

FUNCTION HDRCOM_TRACE_REPORT4

DESCRIPTION
  This is the wrapper function of hdrtrace_report4

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcom_trace_report4(
  hdrcom_trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size,
  uint32 item4,
  uint8 item4_size
);
/*===========================================================================

FUNCTION HDRCOM_TRACE_REPORT3

DESCRIPTION
  This the wrapper function of hdrtrace_report3

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcom_trace_report3(
  hdrcom_trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size
);
/*===========================================================================

FUNCTION HDRCOM_TRACE_REPORT2

DESCRIPTION
  This function is a wrapper function of hdrtrace_report2

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcom_trace_report2(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size
);

#define HDRCOM_TRACE_SL10(num)  ( ((uint32 )(num)) <<10 )
#define HDRCOM_TRACE_SL6(num)   ( ((uint32 )(num)) <<6 )

#define HDRCOM_TRACE_CCMD(prot, subtype, id)\
       (HDRCOM_TRACE_SL10(prot) | HDRCOM_TRACE_SL6(subtype) | id )


/*lint -emacro(506,HDRTRACE_CMSG)   Const val Boolean */
/* Message ID 0x50 ~ 0x54 are commond for all protocol/subtype*/
#define HDRCOM_TRACE_CMSG(prot, subtype, id)\
     (uint32)( (id < 0x50 || id > 0x54) ?  (int)( HDRCOM_TRACE_SL10(prot) | HDRCOM_TRACE_SL6(subtype) | id )\
      : ( (int)id-0x50) )


/* composite state */
#define HDRCOM_TRACE_CSTATE(prot, subtype, id)\
       (HDRCOM_TRACE_SL10(prot) | HDRCOM_TRACE_SL6(subtype) | id)
	   
#define HDRCOM_TRACE_CURRENT_STATE( protocol, state )\
   hdrcom_trace_report2( TRACE_HDR_STATE, \
              (uint32) protocol, 1, \
              (uint32) HDRCOM_TRACE_CSTATE(protocol, 0, state), 2)

#define HDRCOM_TRACE_CMD( protocol, cmd, sender, state )\
   hdrcom_trace_report4( TRACE_HDR_CMD, \
              (uint32) HDRCOM_TRACE_CCMD(protocol, 0, cmd), 2, \
              (uint32) HDRCOM_TRACE_CSTATE(protocol, 0, state), 2, \
              (uint32) sender, 1, \
              (uint32) protocol, 1)

#define HDRCOM_TRACE_IND( protocol, ind_name, state )\
   hdrcom_trace_report3( TRACE_HDR_IND, \
              (uint32) ind_name, 1, \
              (uint32) protocol, 1, \
              (uint32) HDRCOM_TRACE_CSTATE(protocol, 0, state), 2)



#define HDRTRACE_TXMSG_ACKED( protocol, msg, state )\
   hdrcom_trace_report2( (hdrcom_trace_id_enum_type) EVENT_HDR_TXMSG_ACKED, \
                     (uint32)  HDRCOM_TRACE_CMSG(protocol, 0, msg), 2, \
                     (uint32)  protocol, 1); \
   HDRCOM_TRACE_CURRENT_STATE(protocol, state)

#define HDRTRACE_TXMSG_FAILED( protocol, msg, state )\
   hdrcom_trace_report3( TRACE_HDR_TXMSG_FAILED, \
              (uint32) HDRCOM_TRACE_CMSG(protocol, 0, msg), 2, \
              (uint32) protocol, 1, \
              (uint32) HDRCOM_TRACE_CSTATE(protocol, 0, state), 2)

/*===========================================================================
MACRO HDRCOM_DSM_GET_CARRIER_ID

DESCRIPTION
  This macro gets the carrier ID field from the specified DSM item header.
    
PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Carrier ID

SIDE EFFECTS
  None
===========================================================================*/
#define HDRCOM_DSM_GET_CARRIER_ID( item_ptr )     \
          (((uint32)((item_ptr)->app_ptr) & CARRIER_ID_MASK) >> CARRIER_ID_LSB)

/*===========================================================================
MACRO HDRCOM_DSM_GET_CHAN_BIT

DESCRIPTION
  This macro gets the current value of the channel bit from the specified dsm 
  item structure. 

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Channel bit. ( 1 = Control Channel, 0 = Forward Traffic Channel )

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRCOM_DSM_GET_CHAN_BIT( item_ptr )\
   ( ( (item_ptr)->app_field & CHAN_BIT_MASK ) >> CHAN_BIT_LSB )

/*===========================================================================
MACRO HDRCOM_DSM_GET_DRC

DESCRIPTION
  This macro gets the value of the DRC field from the specified
  dsm item header. 

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  DRC value.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRCOM_DSM_GET_DRC( item_ptr )\
   ( ( (item_ptr)->app_field & DRC_MASK ) >> DRC_LSB )

 /* EJECT */
/*===========================================================================
MACRO HDRCOM_DSM_GET_LINK_ID

DESCRIPTION
  This macro gets the current value of the link ID field from the specified
  dsm item header. 

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Link ID value.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRCOM_DSM_GET_LINK_ID( item_ptr )\
   ( ( (item_ptr)->app_field & LINK_ID_MASK ) >> LINK_ID_LSB )

/*===========================================================================
MACRO HDRCOM_DSM_GET_PROT_ID

DESCRIPTION
  This macro gets the current value of the protocol ID field from the 
  specified dsm item structure. 

PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Protocol ID value.

SIDE EFFECTS
  None.
===========================================================================*/
#define HDRCOM_DSM_GET_PROT_ID( item_ptr )\
   ( ( (item_ptr)->kind & PROT_ID_MASK ) >> PROT_ID_LSB )


 /* EJECT */
/*===========================================================================
MACRO HDRCOM_DSM_GET_SCHED_GROUP_ID

DESCRIPTION
  This macro gets the Scheduler Group ID field from the specified DSM item 
  header.
    
PARAMETERS
  item_ptr - address of dsm item

DEPENDENCIES
  None.

RETURN VALUE
  Scheduler Group ID

SIDE EFFECTS
  None
===========================================================================*/
#define HDRCOM_DSM_GET_SCHED_GROUP_ID( item_ptr )     \
          (((uint32)((item_ptr)->app_ptr) & SCHED_GROUP_ID_MASK) >> \
           SCHED_GROUP_ID_LSB)

/*=========================================================================

FUNCTION HDRCOM_LOG_SET_DIPSW_CB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  dipsw - The dip switch which is to be associated with callbacks.
  set  - Function to call when dip switch is set.
  reset - Function to call when dip switch is reset (cleared).

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrcom_log_set_dipsw_cb
(
  hdrcom_log_dipsw_enum_type dipsw,
  void (*set)(void),
  void (*reset)(void)
);


/*===========================================================================

FUNCTION HDRCOM_IND_GET_IND_DATA_SIZE

DESCRIPTION
  This function returns the data size of the specificed indication name

DEPENDENCIES
  None
  
PARAMETERS
  ind_name - indication name

RETURN VALUE
  Size of the indication data

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrcom_ind_get_ind_data_size 
(
  hdrcom_ind_name_enum_type ind_name
);

/*===========================================================================

FUNCTION HDRCOM_UTIL_READ_NV_ITEM

DESCRIPTION
  This function reads an item from NV

DEPENDENCIES
  None.

RETURN VALUE
  NV command return value

SIDE EFFECTS
  ERR_FATAL if the command return is not DONE or NOTACTIVE

===========================================================================*/
nv_stat_enum_type hdrcom_util_read_nv_item
(
  nv_items_enum_type item_code, /* item to read */
  nv_item_type * item_ptr /* pointer to where to put the item */
);


/*============================================================================
  HDRCOM_RX_QUEUE_IND
    This is a wrapper function.

  @context HDRRX task. 

  @sideeffects None 
============================================================================*/
void hdrcom_rx_queue_ind
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
    /**< Who the indication is for */
  hdrcom_ind_name_enum_type ind_name,
    /**< Name of the indication given */
  void *input_ind_data_ptr
    /**< Size of the data associated with this indication */
);

/*===========================================================================

FUNCTION HDRCOM_DSM_SET_TRACER

DESCRIPTION
  This function sets dsm tracer

DEPENDENCIES
  None
  
PARAMETERS
  item_ptr - A pointer to an item to be reset

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void hdrcom_dsm_set_tracer 
(
  dsm_item_type * item_ptr
);
#endif /* HDRCOM_API_H */
