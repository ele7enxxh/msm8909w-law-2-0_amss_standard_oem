#ifndef HDRL1_API_H
#define HDRL1_API_H

/*===========================================================================
                  H D R    L1    D E F I N I T I O N

GENERAL DESCRIPTION
      This file contains hdr srch and mac definitions

  Copyright (c) 2011-2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/api/hdrl1_api.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------- 
04/05/2016   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
07/23/2015   vko     FR 29379 : Disable tuneaway during Qchat call
06/12/2015   bb/vlc  Added hdrl1_srch_is_in_suspend_state(). 
05/11/2015   kss     Added support for modem tx power stats.
02/25/2015   ljl     Fixed compiler warning. 
12/17/2014   vke     Added API to handle the flow control command from DS
12/03/2014   ljl     Declared system lost if CC packet HSTR was incorrect.
03/31/2014   mbs     Resolved compilation error on removal of errno.h
03/11/2014   tnp     Increase maximum number of supported MAC flows to 8
01/14/2014   tnp     Reduce maximum number of supported MAC flows to 5
12/19/2013   rmv     Added interface to obtain Cost of Modem Power estimate
12/10/2013   rmg     Added interface to obtain RL rate info.
10/31/2013   ukl     Added a new syslost reason.
02/14/2013   dsp     Added hdrsrch_acq_search_active(). 
08/23/2012   ukl     Added one more update for SU API cleanup.
06/04/2012   dsp     Added pa_gain_state variable in hdrl1_srch_tx_pwr_info_struct_type.
04/10/2012   dsp     Added hdrl1_srch_time_until_tune_complete(). 
04/09/2012   smd     Added two new SU APIs.
02/24/2012   smd     Removed feature defines from SU API header files.
02/16/2012   smd     Added hdrl1_rmac_start_flow_control.
02/13/2012   smd     Added hdrl1_rmac_stop_flow_control in header file. 
11/15/2011   smd     Added hdrl1_srch_get_band,hdrl1_enc_eram_lock/unlock. 
10/24/2011   smd     Moved more to SU API header files.
10/19/2011   smd     SU API cleanup update.
10/07/2011   smd     Added a few more updates for SU API cleanup.
09/10/2011   smd     Created this file.
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "sys.h"
#include "IxErrno.h"
#include "lte_as.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/* SUAPI_TODO: move to hdrsrchtypes.h later */

#define HDRSRCH_ASET_MAX_SIZE                    HDRL1_SRCH_ASET_MAX_SIZE
#define HDRSRCH_MAX_SUBASET_SIZE                 HDRL1_SRCH_MAX_SUBASET_SIZE
#define HDRSRCH_MAX_NUM_SUBASETS                 HDRL1_SRCH_MAX_NUM_SUBASETS
#define HDRSRCH_MAX_NUM_SCHDGRPS                 HDRL1_SRCH_MAX_NUM_SCHDGRPS
#define HDRSRCH_CREATE_BIT_MASK                  HDRL1_SRCH_CREATE_BIT_MASK
#define hdrsrch_schdgrp_gid_type                 hdrl1_srch_schdgrp_gid_type              
#define hdrsrch_schdgrp_info_type                hdrl1_srch_schdgrp_info_type
#define hdrsrch_schdgrp_rpt_type                 hdrl1_srch_schdgrp_rpt_type
#define hdrsrch_tc_handdown_mode_enum_type       hdrl1_srch_tc_handdown_mode_enum_type
#define hdrsrch_tc_mode_enum_type                hdrl1_srch_tc_mode_enum_type
#define hdrsrch_link_id_type                     hdrl1_srch_link_id_type
#define hdrsrch_system_lost_enum_type            hdrl1_srch_system_lost_enum_type
#define hdrsrch_wakeup_reason_enum_type          hdrl1_srch_wakeup_reason_enum_type
#define hdrsrch_fast_acq_enum_type               hdrl1_srch_fast_acq_enum_type
#define hdrsrch_tran_sys_time_enum_type          hdrl1_srch_tran_sys_time_enum_type
#define hdrsrch_rx_div_enum_type                 hdrl1_srch_rx_div_enum_type
#define hdrsrch_rx_diversity_bit_mask_enum_type  hdrl1_srch_rx_diversity_bit_mask_enum_type
#define hdrsrch_rx_chain_info_struct_type        hdrl1_srch_rx_chain_info_struct_type
#define hdrsrch_rx_pwr_info_struct_type          hdrl1_srch_rx_pwr_info_struct_type
#define hdrsrch_rx_chain_enum_type               hdrl1_srch_rx_chain_enum_type
#define hdrsrch_tx_pwr_info_struct_type          hdrl1_srch_tx_pwr_info_struct_type
/*hdrmac.h*/
#define hdrmac_outage_notification_data_type     hdrl1_mac_outage_notification_data_type
/* hdrrmac.h */
#define HDRRMAC_MAX_APP_FLOWS                 HDRL1_RMAC_MAX_APP_FLOWS
#define hdrrmac_flow_mapping_struct_type      hdrl1_rmac_flow_mapping_struct_type
#define hdrrmac_app_flow_struct_type          hdrl1_rmac_app_flow_struct_type
#define hdrrmac_caller_id_enum_type           hdrl1_rmac_caller_id_enum_type
#define hdrrmac_rl_rate_info_struct_type      hdrl1_rmac_rl_rate_info_struct_type

#define HDRRMAC_MAX_NUM_MAC_FLOWS             HDRL1_RMAC_MAX_NUM_MAC_FLOWS 
#define HDRRMAC_FLOW_ID_DEFAULT               HDRL1_RMAC_FLOW_ID_DEFAULT
/* hdrrmac3.h */
#define hdrrmac3_mac_info_type                hdrl1_rmac3_mac_info_type
#define HDRRMAC3_MAX_NUM_MAC_FLOWS            HDRL1_RMAC3_MAX_NUM_MAC_FLOWS
/* hdrfmac.h*/
#define hdrfmac_drc_req_entity_type           hdrl1_fmac_drc_req_entity_type
#define hdrfmac_ack_mode_enum_type            hdrl1_fmac_ack_mode_enum_type
#define hdrfmac_flow_control_pattern_enum_type \
        hdrl1_fmac_flow_control_pattern_enum_type
/*hdrrx.h*/
#define HDRRX_RLP_ABT_TIMER_SIG              HDRL1_RX_RLP_ABT_TIMER_SIG
#define HDRRX_RLP_LOGGING_SIG                HDRL1_RX_RLP_LOGGING_SIG 
#define HDRRX_RLP_CTA_EXP_SIG                HDRL1_RX_RLP_CTA_EXP_SIG 
#define HDRRX_RLP_NAK_DELAY_TIMER_SIG        HDRL1_RX_RLP_NAK_DELAY_TIMER_SIG
/*hdrtx.h*/
#define HDRTX_RLP_TXQ_SIG                     HDRL1_TX_RLP_TXQ_SIG
#define HDRTX_MRLP_SPD_SIG                    HDRL1_TX_MRLP_SPD_SIG  
#define HDRTX_MRLP_STATS_SIG                  HDRL1_TX_MRLP_STATS_SIG
/*hdrlmac.h*/
#define hdrlmac_cost_of_modem_power_enum     hdrl1_lmac_cost_of_modem_power_enum
#define hdrlmac_modem_api_data_type          hdrl1_lmac_modem_api_data_type

#define hdrsrchltemeas_rpt_cell_struct_type    hdrl1_srchltemeas_rpt_cell_struct_type
#define hdrsrchltemeas_rpt_struct_type         hdrl1_srchltemeas_rpt_struct_type

/*--------------------------------------------------------------------------------*/
/** Sig for RLP logging          */
#define  HDRL1_RX_RLP_LOGGING_SIG                                0x00020000
/** Sig for RLP NAK abort */ 
#define HDRL1_RX_RLP_ABT_TIMER_SIG                               0x00000010
/** Sig for RLP CTA expiration */
#define HDRL1_RX_RLP_CTA_EXP_SIG                                 0x00010000
/** Sig for RLP Delayed Nak */
#define HDRL1_RX_RLP_NAK_DELAY_TIMER_SIG                         0x00001000
#define HDRL1_TX_RLP_TXQ_SIG             0x00020000 /* Signal to free the items
                                                     from RLP's txq        */

#define  HDRL1_TX_MRLP_SPD_SIG            0x00010000 /* Signal to drop stale 
                                                                   packets */
#define  HDRL1_TX_MRLP_STATS_SIG          0x00040000 /* Signal to calculate the 
                                                  Fwd and Rev link data rate */
#define HDRL1_RMAC_MAX_NUM_MAC_FLOWS           8

#define HDRL1_RMAC_FLOW_ID_DEFAULT           511

#define HDRL1_RMAC3_MAX_NUM_MAC_FLOWS         HDRRMAC_MAX_NUM_MAC_FLOWS

#define HDRL1_SRCH_ASET_MAX_SIZE        6
  /* Maximum number of Active Set pilots */

#define HDRL1_SRCH_MAX_SUBASET_SIZE           HDRL1_SRCH_ASET_MAX_SIZE
  /* maximum size of subaset */

#define HDRL1_SRCH_MAX_NUM_SUBASETS           3
  /* maximum number of subasets */
#define HDRL1_SRCH_MAX_NUM_SCHDGRPS \
                     ( HDRL1_SRCH_MAX_NUM_SUBASETS * HDRL1_SRCH_MAX_SUBASET_SIZE ) 
  /* MAX number of the scheduler groups */

#define HDRL1_RMAC_MAX_APP_FLOWS              8


/* Macro for creating a bitmask by shifting '1' left by a  
    Specified number indicated by 'val'. 
*/
#define HDRL1_SRCH_CREATE_BIT_MASK( val ) ( 1<< ((int)(val)) )




typedef enum
{
  HDRSRCH_1X_HANDDOWN_DISABLED_MODE,
    /* Handdown to 1x disabled during HDR traffic connection */

  HDRSRCH_1X_HANDDOWN_ENABLED_MODE
    /* Handdown to 1x enabled during HDR traffic connection */
}
hdrl1_srch_tc_handdown_mode_enum_type;

/*--------------------------------------------------------------------------
  Enum for possible HDR traffic modes
  --------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCH_DEFAULT_TC_MODE,
    /* Standard HDR traffic connection */ 

  HDRSRCH_S1XHDR_HP_TC_MODE,
    /* High Priority Traffic Mode where HDR forces 1x paging onto chain 1,
       and preempts 1x acquisition. */

  HDRSRCH_DISABLE_TA_TC_MODE,
    /* TC mode to disable tune aways during traffic */
  
  HDRSRCH_ENABLE_TA_TC_MODE,
    /* TC mode to enable tune aways during traffic */

  HDRSRCH_HDR_NUM_MODES
    /* Number of SHDR Modes */
} 
hdrl1_srch_tc_mode_enum_type;

typedef enum
{
  HDRSRCH_SYSLOST_LOST_ACQ_AFTER_SLEEP,
    /* Could not reacquire after sleep */

  HDRSRCH_SYSLOST_RUP_PILOT_SUPERVISION,
    /* [6.6.5.5.2]  The strongest pilot is too weak for too long */

  HDRSRCH_SYSLOST_SYNC_MSG_TIMEOUT,
    /* Too long in sync state waiting for sync message */

  HDRSRCH_SYSLOST_MDSP_HALTED,
    /* MDSP halted - Internal Error */

  HDRSRCH_SYSLOST_NO_RESOURCES,
    /* Lock in RF Resources was absent for too long */

  HDRSRCH_SYSLOST_MSTR_SLAM_TIMEOUT,
    /* MSTR slam could not be completed for too long due to unlocked fingers */

  HDRSRCH_SYSLOST_RESELECT,
    /* HDR signal is weak - may be moving out of coverage */

  HDRSRCH_SYSLOST_ADVISE_UNLOCK_RF,
    /* HDR was told to release the RF lock, if it held it */

  HDRSRCH_SYSLOST_SYNC_WEAK_HDR_COVERAGE,
    /* HDR coverage is weak at sync state */

  HDRSRCH_SYSLOST_DSP_NOT_READY,
    /* DSP application is not ready       */

  HDRSRCH_SYSLOST_INVALID_REVB_TCA,
    /* Invalid TCA command */

  HDRSRCH_SYSLOST_FW_RESET_REQ,
    /* SSBI task queue stuck */

  HDRSRCH_SYSLOST_BAD_RF_CAL
    /* Bad RF cal */

#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
  , 
  HDRSRCH_SYSLOST_AVOIDED_CH_IN_ASET
  /* Avoided Channel in ASET */
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */

  ,HDRSRCH_SYSLOST_BAD_CC_HSTR
    /* Odd hstr for CC packets */

}
hdrl1_srch_system_lost_enum_type;


/*--------------------------------------------------------------------------
  HDRSRCHSLEEP wake-up reasons
--------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCH_WAKEUP_NO_WAKEUP_PENDING    = 0x0,
    /* There is no pending wakeup, i.e., AT is no sleeping  */

  HDRSRCH_WAKEUP_PAGE                 = 0x1,
    /* wake-up for regular HDR pages              */

  HDRSRCH_WAKEUP_BCMCS_OVHD           = 0x2,
    /* wake-up for HDR BCMCS overhead             */

  HDRSRCH_WAKEUP_BCMCS_DATA           = 0x4,
    /* Wake-up from BCMCS sleep                   */

  HDRSRCH_WAKEUP_FAST                 = 0x8,
    /* Fast wake-up from sleep                    */

  HDRSRCH_WAKEUP_FAKE                 = 0x10,
    /* fake wake-up when no sufficient sleep time */

  HDRSRCH_WAKEUP_OTHER_RAT_NL         = 0x20,
    /* Wake up to receive Other RAT Neighbor list */
}
hdrl1_srch_wakeup_reason_enum_type;

typedef uint8 hdrl1_srch_schdgrp_gid_type;
  /* Scheduler group ID type */

typedef uint8 hdrl1_srch_link_id_type;           /* Link ID Type */

typedef struct
{
  hdrl1_srch_schdgrp_gid_type gid;
    /* Scheduler group id issued by hdrsrch */
    
  uint8 num_carriers;
    /* The number of carriers used for this scheduler group */
} hdrl1_srch_schdgrp_info_type;

typedef struct
{
  uint8 num_schdgrps;
    /* The number of outstanding scheduler groups */

  hdrl1_srch_schdgrp_info_type schdgrp_list[HDRL1_SRCH_MAX_NUM_SCHDGRPS];
    /* The following list lists the information for each outstanding 
     * scheduler groups */
} hdrl1_srch_schdgrp_rpt_type;
  /* Info to report to the RLP */


/*--------------------------------------------------------------------------
  Fast Acquisition success/failure indication
--------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCH_FAST_ACQ_FAILED,
  HDRSRCH_FAST_ACQ_SUCCEEDED
}
hdrl1_srch_fast_acq_enum_type;


/*--------------------------------------------------------------------------
 Time Transfer success/failure indication
--------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCH_TRAN_SYS_TIME_FAILED,
  HDRSRCH_TRAN_SYS_TIME_SUCCEEDED
}
hdrl1_srch_tran_sys_time_enum_type;


typedef enum
{
  HDRSRCH_RX_DIV_OFF        = 0x00,
    /* Turn off diversity */

  HDRSRCH_RX_DIV_ON         = 0x01,
    /* Turn on diversity */

  HDRSRCH_SECOND_CHAIN_ONLY = 0x02
    /* Turn only second chain on */
  
} hdrl1_srch_rx_div_enum_type;
  /* HDR SRCH rx diversity type */

/* Enumeration of RX chains supported */
typedef enum {

  HDRSRCH_RX_CHAIN_NUM_0 = 0,
    /* RX chain 0 */
  HDRSRCH_RX_CHAIN_NUM_1,
    /* RX chain 1 */
  HDRSRCH_RX_CHAIN_NUM_MAX
    /* For internal use only */    
}hdrl1_srch_rx_chain_enum_type;


/* Enumeration of RX chain diversity info
*/
typedef enum{

  HDRSRCH_RX_CHAIN_UNAVAIL = 0,
    /* Signal info are not available on all RX chains */
  HDRSRCH_RX_CHAIN_0_AVAIL = HDRL1_SRCH_CREATE_BIT_MASK(HDRSRCH_RX_CHAIN_NUM_0),
    /* Signal info on RX_CHAIN_0 is available */
  HDRSRCH_RX_CHAIN_1_AVAIL = HDRL1_SRCH_CREATE_BIT_MASK(HDRSRCH_RX_CHAIN_NUM_1),
    /* Signal info on RX chain 1 is available */
  HDRSRCH_RX_CHAIN_ALL_AVAIL = HDRL1_SRCH_CREATE_BIT_MASK(HDRSRCH_RX_CHAIN_NUM_0) |
                                HDRL1_SRCH_CREATE_BIT_MASK(HDRSRCH_RX_CHAIN_NUM_1)
    /* Signal info on both RX chains are available */
}hdrl1_srch_rx_diversity_bit_mask_enum_type;

/* Structure of RX signal info parameters per each RX chain */
typedef struct{

  boolean             is_radio_tuned;
    /* Whether RX is tuned to a channel. If radio is tuned,
       instantenuous values will be set for below signal info  
       fields.If radio is not tuned, delayed or invalid values 
       will be set depending on each technology.
    */

  int32               rx_pwr;
    /* RX power value in 1/10 dbm resolution */

  int32               ecio;
    /* Ec/Io in negtive 0.1 dbm. */
 
}hdrl1_srch_rx_chain_info_struct_type;

/* Structure of RX power related info
*/
typedef struct {

  hdrl1_srch_rx_diversity_bit_mask_enum_type  rx_diversity_indicator;
    /* Diversity bit mask to show which RX chain has valid 
       signal info. 
    */

  hdrl1_srch_rx_chain_info_struct_type           rx_pwr_chain[HDRSRCH_RX_CHAIN_NUM_MAX];
    /* Rx signal info */

}hdrl1_srch_rx_pwr_info_struct_type;


/* Structure of TX power related info */
typedef struct {

  boolean             is_in_traffic;
    /* Whether device is in traffic. tx_pwr is only meanful 
       when in traffic. If no traffic tx_pwr is invalid
    */

  uint8               pa_gain_state;
   /* PA gain state */

  int32               tx_pwr;
    /* TX power value in 1 dbm */
}hdrl1_srch_tx_pwr_info_struct_type;


/* Data Outage notification information */
typedef struct
{
  uint16 time_to_outage_ms;
    /* Number of Milliseconds to outage */

  uint16 outage_duration_ms;
    /* Number of Milliseconds of outage */

} hdrl1_mac_outage_notification_data_type;


/* 
 * Structure describing an Application flow 
 */
typedef struct
{
  uint16                            stream_id;
  uint8                             sub_stream_id;
} hdrl1_rmac_app_flow_struct_type;


/* 
 * Structure describing the association between the Application Flows 
 * to a given MAC flow
 */
typedef struct
{
  uint8                                mac_flow_id;
  uint8                                flow_count;
  hdrl1_rmac_app_flow_struct_type      app_flows[HDRL1_RMAC_MAX_APP_FLOWS];
} hdrl1_rmac_flow_mapping_struct_type;


/* Type used to track entities requesting that RL throttling be
   disabled. (See hdrrmac_disable_throttling(). ) */
typedef enum
{
  HDRRMAC_FTAP_ID  = 0x01,
  HDRRMAC_DIPSW_ID = 0x02,
  HDRRMAC_RTAP_ID  = 0x04,
  HDRRMAC_FTM_ID   = 0x08
} 
hdrl1_rmac_caller_id_enum_type;

/* Reverse link rate information struct type */
typedef struct
{
  /* Estimated reverse link throughput */
  uint32   est_rl_tput;
  
  /* TRUE if throughputs greater than estimated value may be possible */
  boolean  plus_flag;

  /* Current reverse link throughput */
  uint32   curr_rl_tput;

  /* RLP queue length */
  uint32   q_len;

} hdrl1_rmac_rl_rate_info_struct_type;

/* For interface between RLP and MAC for VT optimization */
typedef struct
{
  uint16 pa_headroom_payload_size;  /* PA headroom limited payload size in bytes */
  uint16 bucket_level_payload_size; /* BucketLevel equivalent payload size in bytes */
  uint16 t2p_inflow_payload_size;   /* T2PInflow equivalent payload size in bytes */
} hdrl1_rmac3_mac_info_type;

typedef enum
{
  HDRFMAC_DRC_REQ_DSMWM = 0x1,
    /* DRC request triggered by Mem low/high event */  

  HDRFMAC_DRC_REQ_PSQ = 0x2,
    /* DRC request for PS Flow control */

  HDRFMAC_DRC_REQ_IS890 = 0x4,
    /* DRC request by TAP */

  HDRFMAC_DRC_REQ_WDOG = 0x8,
    /* DRC Request by Watchdog based on CPU usage */

  HDRFMAC_DRC_REQ_CBUF = 0x10,
    /* DRC Request by HDRRx task based on Circular buffer usage */

  HDRFMAC_DRC_REQ_NULL_COVER = 0x20,
    /* DRC Request when NULL cover is requested */

  HDRFMAC_DRC_REQ_SHIM = 0x40
    /* DRC Request by Shared Memory Interface */

} hdrl1_fmac_drc_req_entity_type;


/* Enumeration for Ack test modes */
typedef enum
{
  IS856_COMPLIANT = 0,       /* Normal IS-856 operation */
  IS856_NO_ACK,              /* IS-856 but with no early termination */
  IS890_MODE_0,              /* Test app mode */
  IS890_MODE_1,               /* Test app mode */

  IS890A_MODE_ACK_BPSK,      /* Always ACK with Bi-polar Keying modulation */
  IS890A_MODE_NAK_BPSK,      /* Always NAK with Bi-polar Keying modulation */
  IS890A_MODE_ACK_OOK,       /* Always ACK with ON-OFF Keying modulation */
  IS890A_MODE_NAK_OOK        /* Always NAK with ON-OFF Keying modulation */
} hdrl1_fmac_ack_mode_enum_type;


/* The enum defines various possible Null cover patterns specified in 
   (period, onDuration) pairs */
typedef enum
{
  HDRFMAC_NO_FLOW_CONTROL               = 0,  /* (0,0)  */
  HDRFMAC_FIRST_FLOW_CONTROL_PATTERN    = HDRFMAC_NO_FLOW_CONTROL,
  HDRFMAC_FLOW_CONTROL_ONE_TENTH        = 1,  /* (10,1) */
  HDRFMAC_FLOW_CONTROL_TWO_TENTH        = 2,  /* (5,1)  */
  HDRFMAC_FLOW_CONTROL_THREE_TENTH      = 3,  /* (10,3) */
  HDRFMAC_FLOW_CONTROL_FOUR_TENTH       = 4,  /* (5,2)  */
  HDRFMAC_FLOW_CONTROL_FIVE_TENTH       = 5,  /* (6,3)  */
  HDRFMAC_FLOW_CONTROL_SIX_TENTH        = 6,  /* (5,3)  */
  HDRFMAC_FLOW_CONTROL_THREE_QUARTER    = 7,  /* (4,3)  */
  HDRFMAC_FLOW_CONTROL_EIGHT_TENTH      = 8,  /* (5,4)  */
  HDRFMAC_FLOW_CONTROL_NINE_TENTH       = 9,  /* (10,9) */
  HDRFMAC_FLOW_CONTROL_ALWAYS           = 10, /* (1,1)  */
  HDRFMAC_LAST_FLOW_CONTROL_PATTERN     = HDRFMAC_FLOW_CONTROL_ALWAYS,
  HDRFMAC_NUM_FLOW_CONTROL_PATTERNS
} hdrl1_fmac_flow_control_pattern_enum_type;

typedef enum
{
  HDRLMAC_EST_COST_OF_POWER_IS_INVALID       = 0x00,
  HDRLMAC_EST_COST_OF_POWER_IS_LOW           = 0x01,
  HDRLMAC_EST_COST_OF_POWER_IS_MEDIUM        = 0x02,
  HDRLMAC_EST_COST_OF_POWER_IS_HIGH          = 0x03
} hdrl1_lmac_cost_of_modem_power_enum;

typedef struct
{
  hdrl1_lmac_cost_of_modem_power_enum cost_of_modem_power;
} hdrl1_lmac_modem_api_data_type;

/* Max number of entries in cell array for OtherRATMeasurementReport message */
#define HDRCOM_SRCH_LTE_CELL_ARR_SIZE   7
/* Max number of entries in LTE EARFCN array for OtherRATMeasurementRequest message */
#define HDRCOM_SRCH_LTE_FREQ_ARR_SIZE   7

/* Struct type to store LTE cell information array for the
   OtherRATMeasurementReport message */
typedef struct
{
  lte_phy_cell_id_t                            cell_id; 
    /* Physical cell id to be reported */

  lte_earfcn_t                                 earfcn;
    /* LTE frequency to be reported */

  boolean                                      rsrq_incl;
    /* RSRQ (Reference Signal Receive Quality) included */
   
  int16                                        rsrq_val;
    /* RSRQ value combined across Tx-Rx pairs. In linear scale. 
       Range -19.5dB ... -3dB with 0.5 dB resolution 
    */

  boolean                                      rsrp_incl;
    /* RSRP (Reference Signal Receive Power) included */
   
  int16                                        rsrp_val;
    /* RSRP value combined across Tx-Rx pairs. In linear scale. 
       Range -144dBm..-44dBm with resolution of 1dBm
    */
}
hdrl1_srchltemeas_rpt_cell_struct_type;

/* Struct type to store LTE cell information reported to DO CP.
   The AT sends the OtherRATMeasurementReport message to provide the AN with LTE's
   radio link conditions.*/
typedef struct
{
  uint8                                        num_eutra_cell;
    /* Number of EUTRA cells reported to AN */

  hdrl1_srchltemeas_rpt_cell_struct_type  cell[ HDRCOM_SRCH_LTE_CELL_ARR_SIZE * HDRCOM_SRCH_LTE_FREQ_ARR_SIZE ];
    /* Array of LTE cell to report  */

}
hdrl1_srchltemeas_rpt_struct_type;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*===========================================================================

FUNCTION HDRL1_SRCH_GET_SINR_AND_RXAGC

DESCRIPTION
  This is a wrapper function. 
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/

void hdrl1_srch_get_sinr_and_rxagc 
(
  byte    *sinr, 
    /* pointer to SINR indicator value */

  int     *rxagc
    /* pointer to RxAgc value   */    
);

/*=========================================================================

FUNCTION     : HDRL1_SRCH_GET_ECIO_AND_IO

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

INPUTS
  Pointers to the locations where EcIo and Io will be stored

RETURN VALUE
  EcIo and Io are returned through the passed pointers.

SIDE EFFECTS
  None

=========================================================================*/
void hdrl1_srch_get_ecio_and_io( int16* ecio, int* io );

/*=========================================================================

FUNCTION     : HDRL1_SRCH_GET_HDR_RSSI

DESCRIPTION  : This is a wrapper function.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

byte hdrl1_srch_get_hdr_rssi
( 
  void 
);

/*=========================================================================

FUNCTION     : HDRL1_SRCH_LINEAR_ENG_TO_DB

DESCRIPTION  : This function converts linear energy to db in unit of -0.5db

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : energy in db

SIDE EFFECTS : None

=========================================================================*/

int hdrl1_srch_linear_eng_to_db
( 
  int                             lin_eng
    /* Linear energy to be converted to -0.5dB units */
);

/*============================================================================

FUNCTION HDRL1_SRCH_GET_BAND

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The band HDR is operating on.

SIDE EFFECTS
  None

============================================================================*/
sys_band_class_e_type hdrl1_srch_get_band( void );


/*=========================================================================

FUNCTION     : HDRL1_SRCH_RUNTIME_DIVERSITY_PREF_CTRL

DESCRIPTION  : This is a wrapper function. 

DEPENDENCIES : 

INPUT        : Whether diversity is on.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrl1_srch_runtime_diversity_pref_ctrl
(
  hdrl1_srch_rx_div_enum_type       div_ctrl
    /* Runtime diversity control */
);

/*=========================================================================

FUNCTION     : HDRL1_SRCH_GET_RX_TX_POWER_INFO

DESCRIPTION
  This is a wrapper function.
    
DEPENDENCIES
  None

INPUTS
  Pointers to the location where Rx and Tx power info structure will be stored

RETURN VALUE
  Rx and Tx agc value are returned through the passed hdrsrch_rx_tx_power_info_type
  pointer.

SIDE EFFECTS
  None

=========================================================================*/

void hdrl1_srch_get_rx_tx_power_info
(
  hdrl1_srch_rx_pwr_info_struct_type   *rx_power_info,
  hdrl1_srch_tx_pwr_info_struct_type   *tx_power_info
);

/*===========================================================================
FUNCTION HDRL1_FMAC_ENABLE_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None.

PARAMETERS
  mask - The entity requesting the flow control
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_fmac_enable_flow_control(uint32 mask);

/*===========================================================================
FUNCTION HDRL1_FMAC_DISABLE_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  mask - The entity requesting the flow control disabling
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_fmac_disable_flow_control(uint32 mask);

/*===========================================================================

FUNCTION HDRL1_FMAC_SET_DRC_VALUE                                EXTERNAL

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  rl_carrier_id - Reverse link Carrier ID.
  drc - DRC value to set.

RETURN VALUE
  TRUE If the given DRC is valid for current subtype.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrl1_fmac_set_drc_value
( 
  uint8 drc 
);


/*===========================================================================

FUNCTION HDRL1_FMAC_SET_ACK_MODE                                 EXTERNAL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None

PARAMETERS
  rl_carrier_id - Reverse link carrier id.
  ack_mode - The mode to enable.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrl1_fmac_set_ack_mode
( 
  hdrl1_fmac_ack_mode_enum_type ack_mode
);


/*===========================================================================

FUNCTION HDRL1_FMAC_GET_FC_PATTERN                                     EXTERNAL

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The current FC pattern.

SIDE EFFECTS
  None.
===========================================================================*/

hdrl1_fmac_flow_control_pattern_enum_type hdrl1_fmac_get_fc_pattern( void );


/*===========================================================================
FUNCTION HDRL1_FMAC_FLOW_CONTROL_CMD

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd - Flow control command from DS
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void hdrl1_fmac_flow_control_cmd( uint32 cmd );


/*==========================================================================

FUNCTION HDRL1_LMAC_GET_SCHDGRP_INFO

DESCRIPTION
  Get the scheduler group report.

DEPENDENCIES

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  May cause an immediate reschedule of searches.

==========================================================================*/

void hdrl1_lmac_get_schdgrp_info
( 
  hdrl1_srch_schdgrp_rpt_type *schdgrp_info_ptr 
);

/*=========================================================================

FUNCTION     : HDRL1_RMAC_GET_CHANGED_FLOW_MAPPING

DESCRIPTION  : 
  This is a wrapper function.

Note: This function is called from DS task.

DEPENDENCIES : None

PARAMETERS   :
  unbound_flows - Array of Flow mapping structs with a set of App flows for 
                  each MAC flow that are unbound from this MAC flow due to 
                  recent GAUPing
  bound_flows   - Array of Flow mapping structs with a set of App flows for 
                  each MAC flow that are bound due to recent GAUPing

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrl1_rmac_get_changed_flow_mapping
(
  hdrl1_rmac_flow_mapping_struct_type *unbound_flows,
  hdrl1_rmac_flow_mapping_struct_type *bound_flows
);

/*===========================================================================
FUNCTION HDRL1_RMAC_GET_FLOW_MAPPING                                   EXTERNAL

DESCRIPTION
  This is a wrapper function. 

DEPENDENCIES
  None.

PARAMETERS
  mapping - Pointer to structure giving the set of upper layer flows mapped 
            to a given MAC flow. 
  flow_id - MAC Flow Id 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac_get_flow_mapping
( 
  hdrl1_rmac_flow_mapping_struct_type *mapping, 
  uint8 flow_id 
);


/*===========================================================================
FUNCTION HDRL1_RMAC_IS_FLOW_ACTIVE                                     

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  MAC flow ID in the range [0x0 - 0xF]

RETURN VALUE
  TRUE if flow is active; false otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrl1_rmac_is_flow_active( uint8 flow );

/*===========================================================================
FUNCTION HDRL1_RMAC_DISABLE_THROTTLING                                 EXTERNAL

DESCRIPTION
  This is a wrapper function

DEPENDENCIES
  None.

PARAMETERS
  caller_id       - The entity requesting that throttling be disabled.
  disable_request - TRUE indicates the caller is requesting disable.
                  - FALSE indicates the caller is ending the request. 
                    (RL throttling may be re-enabled).
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac_disable_throttling
(
  hdrl1_rmac_caller_id_enum_type caller_id,
  boolean                        disable_request
);

/*===========================================================================
FUNCTION HDRL1_RMAC_SET_FIXED_RRI                                      EXTERNAL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  rri      - The RRI to fix to. Use 0xf, or HDRRMAC_IS856_RRI for
             normal operation.

RETURN VALUE
  TRUE If the given RRI is valid for current subtype.
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrl1_rmac_set_fixed_rri
(
  byte rri
);

/*===========================================================================
FUNCTION HDRL1_RMAC_CONFIG_EARLY_TERMINATION                       EXTERNAL

DESCRIPTION
  This is a wrapper function.  

DEPENDENCIES
  None.

PARAMETERS
  config_mode: 0 - Default behavior, RL early termination on.
               1 - Forced early termination after 1 subpacket.
               2 - Forced early termination after 2 subpackets.
               3 - Forced early termination after 3 subpackets.
               4 - Forced early termination after 4 subpackets.

RETURN VALUE
  TRUE (config mode accepted),
  FALSE (config mode not valid).

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrl1_rmac_config_early_termination
(
  int config_mode
);

/*===========================================================================
FUNCTION HDRL1_RMAC_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function. 
  
DEPENDENCIES
  None.

PARAMETERS
  payload_restriction_level - This is a number from 0 to 12.  This is 
    internally mapped to the max payload level RMAC can transmit.  Number 12 
    removes payload restriction.
  enable_cpu_flow_control - When set to TRUE, data is stopped on non-SLP 
    carriers.  When set to FALSE, data is re-enabled on non-SLP carriers.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac_flow_control
(
  uint8 payload_restriction_level,
  boolean enable_cpu_flow_control
);

/*===========================================================================
FUNCTION HDRL1_RMAC_STOP_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac_stop_flow_control( void );

/*===========================================================================
FUNCTION HDRL1_RMAC_STOP_FLOW_CONTROL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac_start_flow_control( void );

/*===========================================================================
FUNCTION HDRL1_RMAC_GET_RL_RATE_INFO

DESCRIPTION
  This function populates the following in the passed struct:
  1. Estimated reverse link throughput in kbps
  2. Flag indicating if throughput greater than estimated rate may be possible
  3. Current reverse link throughput in kbps
  4. RLP queue length
  
DEPENDENCIES
  None
 
PARAMETERS
  Pointer to location where reverse link rate info is to be populated

RETURN VALUE 
  E_SUCCESS       - if valid rate information is populated at the passed 
                    pointer location
  E_NOT_AVAILABLE – Rate information is not available cannot be returned 
                    since AT is not in traffic state
  E_BAD_ADDRESS   - Passed argument is an invalid/NULL pointer

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type hdrl1_rmac_get_rl_rate_info
(
  hdrl1_rmac_rl_rate_info_struct_type * rl_info_ptr
);

/*===========================================================================
FUNCTION HDRL1_RMAC0_ENABLE_RATE_INERTIA                                EXTERNAL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac0_enable_rate_inertia( void );

/*===========================================================================
FUNCTION HDRL1_RMAC0_DISABLE_RATE_INERTIA                               EXTERNAL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrl1_rmac0_disable_rate_inertia( void );

/*===========================================================================
FUNCTION HDRL1_RMAC3_GET_MAC_INFO                                       EXTERNAL

DESCRIPTION
  This is a wrapper function.

DEPENDENCIES
  None.

PARAMETERS
  flow     - MAC Flow Id
  mac_info - Pointer to structure with a set of MAC payload size equivalents 
             for PA headroom, bucket level and T2PInflow

RETURN VALUE
  TRUE  - If the fuction succeeds in getting the requested values
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrl1_rmac3_get_mac_info
( 
  uint8 flow, 
  hdrl1_rmac3_mac_info_type *mac_info 
);


/*===========================================================================
FUNCTION HDRL1_DRIVERS_GET_HSTR_COUNT

DESCRIPTION
  This is a wrapper function to return hdr hstr number.

DEPENDENCIES
  None.

PARAMETERS
  None
 
RETURN VALUE
  current HSTR number

SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdrl1_drivers_get_hstr_count ( void );


/*============================================================================

FUNCTION HDRL1_ENC_ERAM_LOCK

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

============================================================================*/
void hdrl1_enc_eram_lock( void );

/*============================================================================

FUNCTION HDRL1_ENC_ERAM_UNLOCK

DESCRIPTION
  This is a wrapper function.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

============================================================================*/
void hdrl1_enc_eram_unlock( void );

/*============================================================================

FUNCTION HDRL1_SRCH_TIME_UNTIL_TUNE_COMPLETE

DESCRIPTION
  This is a wrapper function. If the HDR tune is happening within 1x tune duration,
  return the time that tune will be completed. Otherwise, return 0ms.
  
DEPENDENCIES
  None
 
PARAMETERS
 tune_duration_1x_us: 1x tune duration in usec

RETURN VALUE
  0: There is no conflict between 1x and HDR RF tune.
  >0 : There is conflict and the time in usec until the HDR tune completes
 
SIDE EFFECTS
  None

============================================================================*/
uint32 hdrl1_srch_time_until_tune_complete
( 
  uint32 tune_duration_1x_us
    /* 1x tune duration in usec*/
);

/*=========================================================================

FUNCTION HDRL1_ACQ_SEARCH_ACTIVE

DESCRIPTION 
  This is wrapper function which returns whether HDR Acq search is active.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Acq srch is active.
  FALSE: Otherwise.
  
SIDE EFFECTS
  None.

=========================================================================*/
boolean hdrl1_acq_search_active(void);

/*=========================================================================

FUNCTION     : HDRL1_LMAC_GET_MODEM_API_DATA

DESCRIPTION
  Retrieves Modem API data.
  During connection, data is collected at that instant.
  In idle, data collected is from last wakeup.
  Can be called from any task.
 
DEPENDENCIES
  None

INPUTS
  Pointer to structure in which Modem API data values to be populated
 
RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrl1_lmac_get_modem_api_data
(
  hdrl1_lmac_modem_api_data_type * modem_api_data
);

/*============================================================================

FUNCTION  HDRL1_SRCH_IS_IN_SUSPEND_STATE

DESCRIPTION
  This function provide a boolean value indicating if HDR is in suspend
  states.  Currently, there are 3 suspend states as listed below.
  
DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  TRUE:  HDR is in one of these states:
         HDRSRCH_SUSPENDED_TC_STATE    
         HDRSRCH_SUSPENDED_IDLE_STATE
         HDRSRCH_SUSPENDED_SYNC_STATE
  FALSE: HDR is in another state
 
SIDE EFFECTS
  None
 
============================================================================*/
boolean hdrl1_srch_is_in_suspend_state(void);

/*===========================================================================
FUNCTION HDRL1_CONFIG_MODEM_POWER_STATS

DESCRIPTION
  This function enables or disables tx duration logging.
 
DEPENDENCIES
  None

PARAMETERS
  enable - Whether to start or stop logging

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void hdrl1_config_modem_power_stats
(
  boolean enable
    /* Enable/disable statistic collection */  
);

/*===========================================================================
FUNCTION HDRL1_GET_MODEM_POWER_STATS

DESCRIPTION
  This function fills Tx duration values into the provided table.
 
DEPENDENCIES
  None

PARAMETERS
  tx_power_bins - array of max_bins items, to be filled with duration stats.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void hdrl1_get_modem_power_stats
(

  uint32 tx_power_bins[]
    /* Pointer to the bin array */
);
#endif /* HDRL1_API_H */
