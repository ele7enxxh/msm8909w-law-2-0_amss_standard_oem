/*!
  @file
  hdrsrchmsg.h

  @brief
  Interface file to the HDR message router and messages.

*/

/*===========================================================================

  Copyright (c) 2009 - 2014 by Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/inc/hdrsrchmsg.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------- 
05/04/2015   mbs     Supported QTA using TRM API
10/14/2014   wsh     Changes to support Jolokia bringup
10/01/2014   arm     Changes to support HDR band avoidance.
09/11/2014   vlc     Changed parameters of 
                     hdrsrchmsg_d2l_send_timed_search_meas_req_msg().
09/01/2014   sat     Fixing Klocwork issues
07/29/2014   tnp     FR22375: Fix Medium compiler warnings in HDR
02/26/2014   vko     FR 17879: Extended EARFCN
10/11/2013   vlc     Added support for new LTE TIMED_SEARCH_MEAS_REQ API.
09/25/2013   wsh     Sleep changes for bolt
09/03/2013   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
06/26/2013   wsh     Added Selective TxD feature
05/24/2013   rmg     Added D-LNA support.
04/22/2013   arm     Added changes for DSDA coexistence manager.
02/26/2013   mbs     Fixed featurization for IRAT code
04/24/2012   ljl     Removed mpc cfg msg. 
03/29/2012   smd     Added DIME sleep updates.
03/20/2012   mbs     Added SON search changes
01/25/2012   grl     Update gRice rank for all ASP changes.
10/19/2011   smd     Added debug code for L2DO IRAT.
10/10/2011   lyl     Supported ASP update response message.
01/06/2011   vlc     Added inclusion of LTE API file, removed 
                     hdrsrchmsg_rf_script_struct_type. 
09/01/2010   vlc     Added support for DO->LTE pilot measurements/reselection.
06/21/2010   cnx     Fixed compiler warnings.
06/15/2010   ljl     Supported RF MSGR interface.
03/18/2010   smd     Added channel info in pilot meas rsp for failure cases.
03/05/2010   smd     Fixed bugs in LTE pilot measurement feature.
02/09/2010   smd     Added messages for LTE to DO pilot measurement
01/07/2010   lyl     Used the FWS API.
12/11/2009   grl     Added FW state config support
12/07/2009   ljl     Supported FWS.
11/11/2009   ljl     Removed HDRSRCH_APP_DISABLE_RSP.
10/07/2009   ljl     Added message trace.
09/14/2009   ljl     Supported srch MSGR interface.
07/29/2009   etv     Included hdrfw_msg.h.
07/15/2009   etv     Moved sw module definitions to hdrmsg.h
07/06/2009   lyl     Updated MinBestAspSinr interface.
06/30/2009   lyl     Updated for generic responses process.
                     Updated for rxAGC unlocked mode interface.
06/25/2009   wsh     Fixed MOB compile error on ARM compiler
05/18/2009   lyl     Updated for firmware MSGR interface.
01/23/2009   pmk     Initial Version
===========================================================================*/

#ifndef HDRSRCHMSG_H
#define HDRSRCHMSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h" /* Interface to the common definitions */
#include "cmd.h"    /* Interface to the command types*/
#include "msgr.h"
#include "hdrfw_msg.h"
#include "srchfw_msg.h"
#include "hdrsrch_msg.h"
#include "hdrmsg.h"
#include "hdrsrchutil.h"

#ifdef FEATURE_HDR_TO_LTE
#include "lte_cphy_irat_meas_msg.h"
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
#include "hdrmultirat.h"
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/* Data/Bit field value definition */
#define HDRSRCHMSG_CARRIER_VALID         0x1
#define HDRSRCHMSG_HO_OVERRIDE_MODE      0x1
#define HDRSRCHMSG_HO_TRIGGER_MODE       0x1
#define HDRSRCHMSG_HO_RESET_RCL          0x1
#define HDRSRCHMSG_HO_OVRD_DSP_CTRL      0
#define HDRSRCHMSG_DRC_PRED_DSP_CTRL     0
#define HDRSRCHMSG_DRC_PRED_MICRO_CTRL   0x1
#define HDRSRCHMSG_DRC_OVRD_VALID        0x1
#define HDRSRCHMSG_ENABLE_DRC            0x1
#define HDRSRCHMSG_DISABLE_DRC           0
#define HDRSRCHMSG_EN_DRC_VALID          0x1
#define HDRSRCHMSG_CC_CARRIER_VALID      0x1
#define HDRSRCHMSG_REF_CARRIER_VALID     0x1
#define HDRSRCHMSG_LOCKED_HO_MODE        0x1
#define HDRSRCHMSG_UNLOCKED_HO_MODE      0
#define HDRSRCHMSG_HO_CTRL_VALID         0x1
#define HDRSRCHMSG_RESET_QRAB_FRAB       0x1
#define HDRSRCHMSG_RESET_QRAB_ONLY       0x1
#define HDRSRCHMSG_RAB_NO_CHANGE         0x1
#define HDRSRCHMSG_RXAGC_ACQ_MODE        0x8000
#define HDRSRCHMSG_RXAGC_RESET_MODE      0

#define HDRSRCHMSG_TRACE_MAX             200
  /* Maximum number of UMIDs stored in trace */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
#define HDRSRCHMSG_MCS_CXM_FREQID_LIST_IND         MCS_CXM_FREQID_LIST_HDR_IND
  /* Indication received from MCS */
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

#ifdef FEATURE_HDR_BAND_AVOIDANCE
#define HDRSRCHMSG_MCS_BAND_AVOID_BLIST_IND        BAND_AVOID_BLIST_IND_HDR_ID
  /* Indication received from MCS */
#endif /* FEATURE_HDR_BAND_AVOIDANCE */

#ifdef FEATURE_HDR_TO_LTE
/*--------------------------------------------------------------------------
  DO->LTE reselection:  structure for LTE confirmation messages
  --------------------------------------------------------------------------*/

#define hdrsrch_lte_init_cnf_params_type lte_cphy_irat_meas_init_cnf_s  
  /* lte_cphy_irat_meas_init_cnf_s is used for MSGR LTE Init confirmation msg */
#define hdrsrch_lte_search_cnf_params_type lte_cphy_irat_meas_search_cnf_s
  /* lte_cphy_irat_meas_search_cnf_s is used for MSGR LTE Search confirmation msg */
#define hdrsrch_lte_meas_cnf_params_type lte_cphy_irat_meas_meas_cnf_s    
  /* lte_cphy_irat_meas_meas_cnf_s is used for MSGR LTE Meas confirmation msg */
#define hdrsrch_lte_timed_srch_meas_cnf_params_type lte_cphy_irat_meas_timed_srch_meas_cnf_s 
  /* lte_cphy_irat_timed_srch_meas_cnf_s is used for MSGR LTE Timed Srch Meas confirmation msg */

typedef union
{

  hdrsrch_lte_init_cnf_params_type      lte_init_cnf;
  hdrsrch_lte_search_cnf_params_type    lte_search_cnf;
  hdrsrch_lte_meas_cnf_params_type      lte_meas_cnf;
  hdrsrch_lte_timed_srch_meas_cnf_params_type   lte_timed_srch_meas_cnf;
}
hdrsrchmsg_lte_cnf_u;

#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
/* messages received for coex manager interactions*/
typedef union
{
  hdrmultirat_cxm_freq_id_list_type         freqid_msg;
}
hdrsrchmsg_cxm_ind_u;
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

/*--------------------------------------------------------------------------
  structure for MSGR messages
  --------------------------------------------------------------------------*/
typedef union
{
  msgr_hdr_struct_type          hdr;
  hdrfw_sw_rcv_msg_u            hdr_msg;
  srchfw_msg_rsp_u              srch_msg;

#ifdef FEATURE_HDR_TO_LTE
  hdrsrchmsg_lte_cnf_u          lte_cnf_msg;
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
  hdrsrchmsg_cxm_ind_u         cxm_msg;
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */
} hdrsrchmsg_msgr_msg_type;

typedef struct
{
  q_link_type                    qlink;
  hdrsrchmsg_msgr_msg_type       msg;

} hdrsrchmsg_s_type;

typedef enum
{
  HDRSRCH_FING_ASSIGN_RSP,
    /* Finger assignment response */

  HDRSRCH_EQ_CONFIG_RSP,
    /* Equalizer configuration response */

  HDRSRCH_STATE_CFG_RSP,
    /* HDR State config response */

  HDRSRCH_DRCFILT_RESET_RSP,
    /* DRC filter reset response */

  HDRSRCH_MSTR_SLAM_RSP,
    /* MSTR slam response */

  HDRSRCH_AFC_ACQ_RSP,
    /* AFC acquisition response */

  HDRSRCH_ACCUM_LOAD_RSP,
    /* Accum loaded response */

  HDRSRCH_SRCH_RSP,
    /* HDR srch result/abort rsp */

  HDRSRCH_PILOT_MEAS_CFG_RSP,
    /* Pilot Measurement config response */

  HDRSRCH_ASP_UPDATE_RSP,
   /* ASP update response */

#ifdef FEATURE_HDR_RUMI
  HDRSRCH_RX_START_RSP,
   /* RX START response */
  HDRSRCH_RX_STOP_RSP,
   /* RX STOP response */
#endif /* FEATURE_HDR_RUMI */

#ifdef FEATURE_HDR_QTA
  HDRSRCH_RXAGC_TRACK_QTA_RSP,
   /* FW AGC settled response after QTA  */

#ifdef FEATURE_HDR_QTA_THRU_TRM
  HDRSRCH_QTA_STATUS_QUERY_RSP,
   /* FW QTA status query response after QTA  */
#endif /* FEATURE_HDR_QTA_THRU_TRM */
#endif /* FEATURE_HDR_QTA */

  HDRSRCH_NUM_RSP
    /* Total number of responses */
}
hdrsrchmsg_rsp_e_type;

typedef struct
{
  msgr_umid_type                    msg_trace[ HDRSRCHMSG_TRACE_MAX ];
    /* UMID trace buffer */

  uint32                            offset;
    /* Offset of the latest UMID */
}
hdrsrchmsg_msg_trace_type;

typedef struct
{
  hdrfw_asp_update_msg_t            asp_update_msg;
    /* ASP update message */

  hdrfw_drc_cfg_msg_t               drc_cfg_msg;
    /* DRC config message */

  hdrfw_drc_filt_reset_msg_t        drc_filt_reset_msg;
    /* DRC filter reset message */

  hdrfw_handoff_cfg_msg_t           handoff_cfg_msg;
    /* Handoff config message */

  hdrfw_handoff_override_msg_t      handoff_ovrd_msg;        
    /* Handoff override message */

  hdrfw_tuneaway_start_msg_t        tuneaway_start_msg;
    /* Tuneaway start message */

  hdrfw_tuneaway_stop_msg_t         tuneaway_stop_msg;
    /* Tuneaway stop message */

  hdrfw_sleep_adj_msg_t             sleep_adj_msg;
    /* Sleep adjust message */

  hdrfw_fpd_msg_t                   fpd_msg;
    /* Force preamble detection message */

  hdrfw_qpch_msg_t                  qpch_msg;
    /* Quick paging message */

  hdrfw_indication_cfg_msg_t        indication_cfg_msg;
    /* Indication config message */

#ifdef FEATURE_HDR_DLNA
  hdrfw_dlna_ctl_msg_t              dlna_ctl_msg;
    /* D-LNA control message */

  hdrfw_early_pream_ctl_msg_t       early_pream_ctl_msg;
    /* Early preamble control message */

  hdrfw_force_odd_pream_msg_t       odd_pream_detect;
    /* Message to force odd preamble detection */
#endif /* FEATURE_HDR_DLNA */

#ifdef FEATURE_HDR_SELECTIVE_TXD
  /* ToDo: remove featurization after integration complete */
  cfw_rf_execute_ant_switch_msg_t   ant_switch_msg;
    /* Antenna blank/switch message */
#endif /* FEATURE_HDR_SELECTIVE_TXD */

  boolean                           waiting_for_rsp[HDRSRCH_NUM_RSP];
    /* Flags to indicate whether we are waiting for FW responses */

  hdrsrchmsg_msg_trace_type         msg_sent;
    /* Messages sent from hdr srch */

  hdrsrchmsg_msg_trace_type         msg_rcv;
    /* Messages received by hdr srch */

} hdrsrchmsg_fw_msg_struct_type;

#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
typedef struct {
  hdrsrch_lte_pilot_meas_rsp_msg_t        lte_pilot_meas_rsp;
    /* Pilot Measurement response to LTE */

  hdrsrch_rf_status_ind_msg_t             rf_status_ind;
    /* RF Status Indication */

  hdrsrch_lte_pilot_meas_abort_rsp_msg_t  lte_pilot_meas_abort_rsp;
    /* Pilot Measurement Abort Response */

} hdrsrchmsg_lte_msg_struct_type;
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */

#ifdef FEATURE_HDR_TO_LTE

/*--------------------------------------------------------------------------
   Confirmation messages from LTE via message router for DO to LTE
   measurements/reselection.
--------------------------------------------------------------------------*/

#define HDRSRCH_LTE_IRAT_MEAS_D2L_INIT_CNF    LTE_CPHY_IRAT_MEAS_D2L_INIT_CNF  
#define HDRSRCH_LTE_IRAT_MEAS_D2L_DEINIT_CNF  LTE_CPHY_IRAT_MEAS_D2L_DEINIT_CNF 
#define HDRSRCH_LTE_IRAT_MEAS_D2L_SEARCH_CNF  LTE_CPHY_IRAT_MEAS_D2L_SEARCH_CNF 
#define HDRSRCH_LTE_IRAT_MEAS_D2L_MEAS_CNF    LTE_CPHY_IRAT_MEAS_D2L_MEAS_CNF   
#define HDRSRCH_LTE_IRAT_MEAS_D2L_CLEANUP_CNF LTE_CPHY_IRAT_MEAS_D2L_CLEANUP_CNF
#define HDRSRCH_LTE_IRAT_MEAS_D2L_ABORT_CNF   LTE_CPHY_IRAT_MEAS_D2L_ABORT_CNF  
#define HDRSRCH_LTE_IRAT_MEAS_D2L_TIMED_SRCH_MEAS_CNF   LTE_CPHY_IRAT_MEAS_D2L_TIMED_SRCH_MEAS_CNF  

   
/*--------------------------------------------------------------------------
  LTE frequency bandwidth (mapped to lte_bandwidth_e enum)
--------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCHMSG_BW_NRB_6    = 6,
    /* 1.4MHz bandwidth */
  HDRSRCHMSG_BW_NRB_15   = 15,
    /* 3MHz bandwidth */
  HDRSRCHMSG_BW_NRB_25   = 25,
    /* 5MHz bandwidth */
  HDRSRCHMSG_BW_NRB_50   = 50,
    /* 10MHz bandwidth */
  HDRSRCHMSG_BW_NRB_75   = 75,
    /* 15MHz bandwidth */
  HDRSRCHMSG_BW_NRB_100  = 100,
    /* 20MHz bandwidth */
  HDRSRCHMSG_BW_MIN      = HDRSRCHMSG_BW_NRB_6,
    /* Minimum supported bandwidth */
  HDRSRCHMSG_BW_MAX      = HDRSRCHMSG_BW_NRB_100,
    /* Maximum supported bandwidth */
  HDRSRCHMSG_BW_UNKNOWN

} hdrsrchmsg_lte_bandwidth_enum_type;
  
#endif /* FEATURE_HDR_TO_LTE */

/*===========================================================================

                    EXTERNAL VARIABLE PROTOTYPES

===========================================================================*/

extern hdrsrchmsg_fw_msg_struct_type  hdrsrchmsg;
  /* Global variable of srch messages struct sent to firmware */

#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
extern hdrsrchmsg_lte_msg_struct_type  hdrsrchmsg_lte;
  /* Global variable to maintain HDR SRCH messages sent to LTE */
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*=========================================================================

FUNCTION     : HDRSRCHMSG_INIT

DESCRIPTION  : This function creates MSGR clients and queues and 
               registers the messages with MSGR.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_init( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_STOP

DESCRIPTION  : This function deletes the MSGR clients.

DEPENDENCIES : None

INPUTS       : msg - The message pointer
               len - The message length 

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_stop( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_CONFIG_INDICATIONS

DESCRIPTION  : This function configures the indications that HDR SRCH 
               will receive.

DEPENDENCIES : None

INPUTS       : None 

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_config_indications( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_SEND_MSG

DESCRIPTION  : This function sends the message to MSGR.

DEPENDENCIES : None

INPUTS       : msg - The message pointer
               len - The message length 

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_send_msg
( 
  void                       *msg,
    /* Message to send */

  uint32                     len
    /* The message lenght */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_GET_MSG_NAME

DESCRIPTION  : This function returns the message name.

DEPENDENCIES : None

INPUTS       : umid: UMID of the message 

RETURN VALUE : Name of the message.

SIDE EFFECTS : None

=========================================================================*/

const char *hdrsrchmsg_get_msg_name
(
  msgr_umid_type             id
    /* umid */
);

/*=========================================================================

FUNCTION     : HDRSRCHMSG_INIT_ASP_UPDATE_MSG

DESCRIPTION  : This function init the ASP update message.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_init_asp_update_msg( void );

/*=========================================================================

FUNCTION     : HDRSRCHMSG_INIT_HANDOFF_CFG_MSG

DESCRIPTION  : This function init handoff config message.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_init_handoff_cfg_msg( void );

/*=========================================================================

FUNCTION     : HDRSRCHMSG_INIT_HANDOFF_OVRD_MSG

DESCRIPTION  : This function init handoff override message.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_init_handoff_ovrd_msg( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_HO_OVRD_MSG_SET_TRIGGER_MODE

DESCRIPTION  : This function sets the handoff trigger mode for handoff 
               override message

DEPENDENCIES : None

INPUTS       : c       - The carrier;
               asp_idx - The ASP index;

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_ho_ovrd_set_trigger_mode
(
  int8       c,
    /* The carrier */

  uint8      asp_idx
    /* The ASP index */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_HO_OVRD_SET_fw_ctrl

DESCRIPTION  : This function sets the handoff firmare control in handoff 
               override message

DEPENDENCIES : None

INPUTS       : c       - The demod carrier;
               ctrl    - contrl mode;

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_ho_ovrd_set_fw_ctrl
(
  int8       c,
    /* The demod carrier */

  uint8      ctrl
    /* The handoff control */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_INIT_HANDOFF_CFG_MSG

DESCRIPTION  : This function init handoff config message.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_init_drc_cfg_msg( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_DRC_CFG_SET_DRC_OVRD

DESCRIPTION  : This function sets DRC override in DRC config message.

DEPENDENCIES : None

INPUTS       : c - carrier;
               drc_ovrd - DRC override mode

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_drc_cfg_set_drc_ovrd
(
  int8             c,
    /* the carrier */

  int8             drc_ovrd
    /* DRC override mode */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_DRC_CFG_SET_DICT_DRC

DESCRIPTION  : This function sets dictated drc value in DRC config message.

DEPENDENCIES : None

INPUTS       : c - carrier;
               dict_drc - dictated DRC value;

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrsrchmsg_drc_cfg_set_dict_drc
(
  int8             c,
    /* the carrier */

  uint8             dict_drc
    /* Dictated DRC value */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_DRC_CFG_ENABLE_DRC

DESCRIPTION  : This function sets enable drc field in DRC config message.

DEPENDENCIES : None

INPUTS       : c       - The carrier;
               enable  - The enable flag;

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_drc_cfg_enable_drc
(
  int8      c,
    /* The carrier */

  int8      enable
    /* Enable flag */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_INIT_DRC_FILT_RESET_MSG

DESCRIPTION  : This function init DRC filter reset message.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_init_drc_filt_reset_msg( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_SET_DRC_FILT_RESET_VALUE

DESCRIPTION  : This function sets DRC filter reset message.

DEPENDENCIES : None

INPUTS       : carrier - carrier index
               value - DRC filter reset value

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_set_drc_filt_reset_value
(
  int8              carrier,
    /* carrier */

  uint16            value
    /* drc filter reset value */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_SEND_DRC_FILT_RESET_MSG

DESCRIPTION  : This function sends DRC filter reset message.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_send_drc_filt_reset_msg( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_ASP_UPDATE_SET_CARRIER_MASK

DESCRIPTION  : This function sets the demod carrier mask in the ASP  
               update message.

DEPENDENCIES : None

INPUTS       : c       - demod carrier

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_asp_update_set_carrier_mask
(
  uint16      c 
    /* Demod carrier */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_ASP_UPDATE_SET_ASP_MASK

DESCRIPTION  : This function sets the ASP mask in ASP update message. 

DEPENDENCIES : None

INPUTS       : c       - demod carrier
               asp_idx - asp index

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_asp_update_set_asp_mask
(
  uint16      c, 
    /* Demod carrier */

  uint8       asp_idx
    /* ASP index */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_SET_ASP_UPDATE_MASK

DESCRIPTION  : This function set the demod carrier and ASP mask in ASP  
               update message.

DEPENDENCIES : None

INPUTS       : c       - demod carrier
               asp_idx - asp index

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrsrchmsg_set_asp_update_mask
(
  uint16      c, 
    /* Demod carrier */

  uint8       asp_idx
    /* ASP index */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_SEND_MULTI_ASP_UPDATE_MSG

DESCRIPTION  : This function sends ASP update message to the FW.

DEPENDENCIES : None

INPUTS       : refresh_rank - Whether to rerank the ASET.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_send_multi_asp_update_msg
( 
  boolean refresh_rank  
    /* Whether to refresh the gRICE ranking. */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_SEND_ASP_UPDATE_MSG

DESCRIPTION  : This function sends ASP update message.

DEPENDENCIES : None

INPUTS       : demod_idx - demod index
               asp_idx   - asp index

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_send_asp_update_msg
(
  uint8              demod_idx,
    /* Demod index */

  uint8              asp_idx
    /* ASP index */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_SET_RAB_INIT_CTRL

DESCRIPTION  : This function sets the rab init contrl bit in ASP update message.

DEPENDENCIES : None

INPUTS       : demod_idx - demod index;
               asp_idx - asp index;
               rab_init_ctrl - contrl value to set;

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrsrchmsg_set_rab_init_ctrl
( 
  int8                             demod_idx,
    /* demod index */

  int8                             asp_idx,
    /* asp index */

  hdrsrchutil_rab_init_ctrl_enum_t rab_init_ctrl
    /* QRab/FRabSoft init contrl */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_BASP_HO_OVRD

DESCRIPTION  : This function sends handoff override message.

DEPENDENCIES : None

INPUTS       : demod_idx - demod index
               asp_idx   - asp index

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_basp_ho_ovrd
(
  uint8              demod_idx,
    /* Demod index */

  uint8              asp_idx
    /* ASP index */
);


#ifdef FEATURE_HDR_DIME_MODEM

/*=========================================================================

FUNCTION     : HDRSRCHMSG_SEND_SLEEP_ADJ_MSG

DESCRIPTION  : This function sets the sleep adjustment value and sends the 
               sleep adjust message.

DEPENDENCIES : None

INPUTS       : sleep_adj - Sleep adjustment

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_send_sleep_adj_msg
(
  uint64      online_univ_stmr,
    /* univ stmr at online interrupt */

  uint64      online_cdma_sys_time,
    /* cdma system time at online interrupt */

  uint32      online_hdr_stmr
    /* HDR RTC value at online interrupt */
);
#else /* FEATURE_HDR_DIME_MODEM */
/*=========================================================================

FUNCTION     : HDRSRCHMSG_SEND_SLEEP_ADJ_MSG

DESCRIPTION  : This function sets the sleep adjustment value and sends the 
               sleep adjust message.

DEPENDENCIES : None

INPUTS       : sleep_adj - Sleep adjustment

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_send_sleep_adj_msg
(
  uint16      sleep_adj
    /* Sleep adjustment */
);
#endif /* FEATURE_HDR_DIME_MODEM */

/*=========================================================================

FUNCTION     : HDRSRCHMSG_START_DRC_RAMPDOWN

DESCRIPTION  : This function sets tuneaway mode and tuneaway time, then sends
               the tuneaway start message.

DEPENDENCIES : None

INPUTS       : tuneaway_mode - The tuneaway mode
               tuneaway_time - The tuneaway time

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_start_drc_rampdown
(
  uint16    tuneaway_mode,
    /* Tuneaway mode */

  uint16    tuneaway_time
    /* Tuneaway time of HSTR */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_SET_RAMPDOWN_MODE

DESCRIPTION  : This function sets tuneaway mode and tuneaway time, then sends
               the tuneaway start message.

DEPENDENCIES : None

INPUTS       : tuneaway_mode - The tuneaway mode

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_set_rampdown_mode
(
  uint16    tuneaway_mode
    /* Tuneaway mode */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_PROCESS_RCVD_RSP

DESCRIPTION  : This function resets the response waiting flag after 
               receiving the response.

DEPENDENCIES : None

INPUTS       : rsp - which response received

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_process_rcvd_rsp
(
  hdrsrchmsg_rsp_e_type     rsp
    /* FW responses */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_RSP_IS_RCVD

DESCRIPTION  : This function checks whether the response is received or not.
               
DEPENDENCIES : None

INPUTS       : rsp - response to check

RETURN VALUE : TRUE  - the response is received
               FALSE - otherwise

SIDE EFFECTS : None

=========================================================================*/

boolean hdrsrchmsg_rsp_is_rcvd
(
  hdrsrchmsg_rsp_e_type       rsp
    /* FW responses */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_SET_RSP_WAITING

DESCRIPTION  : This function sets the response in waiting to be true or false
               based on the flag.
               
DEPENDENCIES : None

INPUTS       : rsp   - which FW response
               flag  - boolean value to be set

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_set_rsp_waiting
(
  hdrsrchmsg_rsp_e_type       rsp,
    /* The FW responses */

  boolean                     flag
    /* The flag value to be set */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_GET_HIGH_PRI_RSP

DESCRIPTION  : This function gets a high priority response.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : The response.

SIDE EFFECTS : None

=========================================================================*/

hdrsrchmsg_s_type *hdrsrchmsg_get_high_pri_rsp( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_FREE_HIGH_PRI_RSP

DESCRIPTION  : This function frees the message.

DEPENDENCIES : None

INPUTS       : msg - The message pointer

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_free_high_pri_rsp
(
  hdrsrchmsg_s_type *        msg
    /* Message */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_GET_LOW_PRI_RSP_IND

DESCRIPTION  : This function gets a message from low priority response 
               and indication queue.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : Response/indication 

SIDE EFFECTS : None

=========================================================================*/

hdrsrchmsg_s_type *hdrsrchmsg_get_low_pri_rsp_ind( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_FREE_LOW_PRI_RSP_IND

DESCRIPTION  : This function frees low priority rsponse/indication.

DEPENDENCIES : None

INPUTS       : msg - The message pointer

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_free_low_pri_rsp_ind
(
  hdrsrchmsg_s_type *        msg
    /* Message */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_GET_SRCH_RSP

DESCRIPTION  : This function gets a message from srch response queue.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : Response 

SIDE EFFECTS : None

=========================================================================*/

hdrsrchmsg_s_type *hdrsrchmsg_get_srch_rsp( void );


/*=========================================================================

FUNCTION     : HDRSRCHMSG_FREE_SRCH_RSP

DESCRIPTION  : This function frees srch rsp.

DEPENDENCIES : None

INPUTS       : msg - The message pointer

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_free_srch_rsp
(
  hdrsrchmsg_s_type *        msg
    /* Message */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_TRACE

DESCRIPTION  : This function stores UMID into trace buffer.

DEPENDENCIES : None

INPUTS       : msg_trace - The message buffer
               id    - UMID

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchmsg_trace
(
  hdrsrchmsg_msg_trace_type *msg_trace,
    /* Trace buffer */

  msgr_umid_type id
    /* UMID */
);

#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
/*===========================================================================

FUNCTION HDRSRCHMSG_PREPARE_LTE_PILOT_MEAS_RSP_REPORT

DESCRIPTION
  This function prepares pilot measurement response

DEPENDENCIES
  None

PARAMETERS
    pilot_meas_status   :   pilot measurement status

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchmsg_prepare_lte_pilot_meas_rsp_report
( 
  hdrsrch_lte_pilot_meas_status_type   pilot_meas_status,
    /* pilot measurement status */

  sys_channel_type                     chan
    /* band and channel number */
);

/*===========================================================================

FUNCTION HDRSRCHMSG_SEND_LTE_PILOT_MEAS_RSP

DESCRIPTION
  This function sends pilot measurement response back to LTE

DEPENDENCIES
  None

PARAMETERS
    pilot_meas_status   :   pilot measurement status
    chan                :   channel information

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchmsg_send_lte_pilot_meas_rsp
( 
  hdrsrch_lte_pilot_meas_status_type   pilot_meas_status,
    /* pilot measurement status */

  sys_channel_type                     chan
    /* band and channel number */
);

#ifdef FEATURE_HDR_IRAT_DEBUG
/*===========================================================================

FUNCTION HDRSRCHTIMESYNC_SEND_FAKE_LTE_PILOT_MEAS_RSP

DESCRIPTION
  This function sends a fake pilot measurement response back to LTE. This is
  for debug purpose when real gap is skipped.

DEPENDENCIES
  None

PARAMETERS
    pilot_meas_status   :   pilot measurement status

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchmsg_send_fake_lte_pilot_meas_rsp
( 
  hdrsrch_lte_pilot_meas_status_type   pilot_meas_status,
    /* pilot measurement status */

  sys_channel_type                     chan
    /* band and channel number */
);
#endif /* FEATURE_HDR_IRAT_DEBUG */

/*===========================================================================

FUNCTION HDRSRCHTIMESYNC_SEND_RF_STATUS_IND

DESCRIPTION
  This function sends RF STATUS Indication

DEPENDENCIES
  None

PARAMETERS
    rf_status   :   the rf status

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchmsg_send_rf_status_ind
(
   hdrsrch_irat_rf_status_type       irat_rf_status
     /* rf status */
);
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */


/*=========================================================================

FUNCTION     : HDRSRCHMSG_HO_CFG_SET_REF_CARRIER

DESCRIPTION  : This function set handoff config message's reference carrier.

DEPENDENCIES : None

PARAMETERS:  : c - Demod carrier

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrsrchmsg_ho_cfg_set_ref_carrier
(
  uint8        c
    /* Demod carrier */
);


/*=========================================================================

FUNCTION     : HDRSRCHMSG_HO_CFG_SET_HO_CTRL

DESCRIPTION  : This function set handoff config message's handoff control.

DEPENDENCIES : None

PARAMETERS:  : ho_ctrl - Handoff control mode

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void hdrsrchmsg_ho_cfg_set_ho_ctrl
(
  uint8        ho_ctrl
    /* Handoff control mode */
);

#ifdef FEATURE_HDR_TO_LTE
/*===========================================================================

FUNCTION HDRSRCHMSG_D2L_SEND_INIT_REQ_MSG 
 
DESCRIPTION
  This function sends the INIT_REQ message with the appropriate header
  and parameters to the LTE MSGR interface.  This message is sent as part of
  the init phase for LTE pilot measurements.  The init phase is required for
  LTE to enable LTE FW app and other processing prior to pilot measurements.
  When done, HDR must also send DEINIT_REQ message.
 
DEPENDENCIES
  None

PARAMETERS 
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchmsg_d2l_send_init_req_msg( void );

/*===========================================================================

FUNCTION HDRSRCHMSG_D2L_SEND_DEINIT_REQ_MSG
 
DESCRIPTION
  This function sends the DEINIT_REQ message with the appropriate header
  and parameters to the LTE MSGR interface.  This message is sent once
  LTE pilot measurements are no longer required.  Upon receiving this message
  LTE SW disables LTE FW app among other shut down processing.  An INIT_REQ
  message is required before any future LTE pilot measurement requests can
  be processed.
 
DEPENDENCIES
  None

PARAMETERS 
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchmsg_d2l_send_deinit_req_msg( void );

/*===========================================================================

FUNCTION HDRSRCHMSG_D2L_SEND_STARTUP_CMD_MSG
 
DESCRIPTION
  This function sends the STARTUP_CMD message with the appropriate header
  and parameters to the LTE MSGR interface.  This message is sent to start
  an LTE pilot measurement process as part of the LTE IRAT interface.
 
DEPENDENCIES
  None

PARAMETERS 
  LTE Frequency to measure
  Frequency bandwidth
  RF script to tune back

RETURN VALUE 
  E_SUCCESS command is successful
  E_FAILURE encounter error, return without executing command

SIDE EFFECTS
  None

===========================================================================*/
errno_enum_type hdrsrchmsg_d2l_send_startup_cmd_msg
(
  lte_earfcn_t                           earfcn, 
    /* EARFCN - Evolved Absolute Radio Frequency Channel Number
       or simply the LTE frequency to search and measure */

  hdrsrchmsg_lte_bandwidth_enum_type     bandwidth,
    /* Bandwidth of the freq */

  lte_cphy_irat_meas_startup_rf_params_s *rf_tuneback_script_ptr
    /* Tune back script to be used when returning to DO */
);

/*===========================================================================

FUNCTION HDRSRCHMSG_D2L_SEND_SEARCH_REQ_MSG
 
DESCRIPTION
  This function sends the SEARCH_REQ message with the appropriate header
  and parameters to the LTE MSGR interface. Typically, LTE needs to search
  once every 3 pilot measurements to ensure that timing is accurate for the
  measurements.
 
DEPENDENCIES
  None

PARAMETERS 
  None 

RETURN VALUE 
  None 

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchmsg_d2l_send_search_req_msg( void );

/*===========================================================================

FUNCTION HDRSRCHMSG_D2L_SEND_MEAS_REQ_MSG
 
DESCRIPTION
  This function sends the MEAS_REQ message with the appropriate header
  and parameters to the LTE MSGR interface. 
 
  The following was extracted from LTE documentation of this message:
  "This message triggers LTE measurements on already detected LTE cells.
  Cells must be detected via an lte_cphy_irat_meas_search_req_s prior to
  measuring them.  Measurements are performed on the RS tones in the LTE
  frequency grid."
 
DEPENDENCIES
  None

PARAMETERS 
  None 

RETURN VALUE 
  E_SUCCESS command is successful
  E_FAILURE encounter error, return without executing command

SIDE EFFECTS
  None

===========================================================================*/
errno_enum_type hdrsrchmsg_d2l_send_meas_req_msg
( 
  hdrsrch_lte_search_cnf_params_type    *meas_req_ptr
);

/*===========================================================================

FUNCTION HDRSRCHMSG_D2L_SEND_CLEANUP_REQ_MSG
 
DESCRIPTION
  This function sends the CLEANUP_REQ message with the appropriate header
  and parameters to the LTE MSGR interface.  This message is sent after LTE
  pilot measurement is received as part of the LTE IRAT interface.
 
DEPENDENCIES
  None

PARAMETERS 
  None 

RETURN VALUE 
  None 

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchmsg_d2l_send_cleanup_req_msg( void );

/*===========================================================================

FUNCTION HDRSRCHMSG_D2L_SEND_ABORT_REQ_MSG
 
DESCRIPTION
  This function sends the ABORT_REQ message to abort any ongoing search or                                      
  measurement operation.
 
DEPENDENCIES
  None

PARAMETERS 
  None 

RETURN VALUE
  None

SIDE EFFECTS
  Note:
  It is possible that LTE may still send a search or measurement operation
  after HDR sends this command due to the multithreaded nature of the system.

===========================================================================*/
void hdrsrchmsg_d2l_send_abort_req_msg( void );

/*===========================================================================

FUNCTION HDRSRCHMSG_D2L_SEND_TIMED_SEARCH_MEAS_REQ_MSG
 
DESCRIPTION
  This function sends the TIMED_SEARCH_MEAS_REQ message which is a combined
  message replacing:
  STARTUP_CMD
  SEARCH_REQ
  MEAS_REQ
  CLEANUP_REQ
 
DEPENDENCIES
  None

PARAMETERS 
  LTE Frequency to measure
  LTE Frequency bandwidth
  LTE Frequency TRM data
  RF script to tune back

RETURN VALUE 
  E_SUCCESS command is successful
  E_FAILURE encounter error, return without executing command
 
SIDE EFFECTS
  None

===========================================================================*/

errno_enum_type hdrsrchmsg_d2l_send_timed_search_meas_req_msg
(
  lte_earfcn_t                                  earfcn, 
    /* EARFCN - Evolved Absolute Radio Frequency Channel Number
       or simply the LTE frequency to search and measure */

  hdrsrchmsg_lte_bandwidth_enum_type      bandwidth,
    /* Bandwidth of the freq */

  trm_set_client_state_output_data        *set_client_state_output_ptr,
    /* TRM data */

  lte_cphy_irat_meas_startup_rf_params_s *rf_tuneback_script_ptr
    /* Tune back script to be used when returning to DO */
);

#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_SELECTIVE_TXD
/*===========================================================================

FUNCTION HDRSRCHMSG_SEND_ANT_BLANK_SWITCH_MSG
 
DESCRIPTION
  This function sends msg to FW to do blank/switch of antenna to
  the given position.
 
DEPENDENCIES
  None

PARAMETERS 
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchmsg_send_ant_blank_switch_msg( void );
#endif /* FEATURE_HDR_SELECTIVE_TXD */

#endif /* HDRSRCHMSG_H */
