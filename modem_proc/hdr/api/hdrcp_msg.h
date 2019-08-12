/*!
  @file
  hdrcp_msg.h

  
  Defines the external message interface for the HDRCP module

*/

/*===========================================================================

  Copyright (c) 2008 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/api/hdrcp_msg.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/16   vko     FR 35747: Graceful handling of UE mode collision and X2L IRAT
09/14/15   vko     FR 29020: LTE Power Save Mode w/o coordination with network
07/24/15   vko     Skip earfcn for D2L reselection, if reselection failed with forbidden TA
                   reason
07/23/15   vko     FR 29379 : Disable tuneaway during Qchat call
02/02/15   vko     Add srch window param for D2L CGI
07/18/14   cnx     Remove mcfg_refresh.h from hdrcp_msg.h.
07/09/14   cnx     Support NV refresh without reset.
02/26/14   vko     FR 17879: Extended EARFCN
02/21/14   vko     Support 128 FPLMNID's
05/01/13   cnx     Support 64 EARFCN in the LTE deprioritized list.
03/31/13   cnx     Support LTE deprioritization.
07/16/12   cnx     Resize HDR_CP_MAX_PN_LIST_SIZE to 32.
04/27/12   cnx     Avoid Reselection from eHRPD to LTE forbidden PLMNs.
04/20/12   cnx     Unsubcribe LTE_RRC_SERVICE_IND.
03/23/12   cnx     Support getting Pilot strength for CGI response.
02/24/12   smd     Removed feature defines from SU API header files.
12/21/11   cnx     Fixed compiler error. 
11/22/11   mm      Added definitions for CGI info request from LTE
10/28/11   pba     Optimized handoff phase 2 support
10/11/11   arm     SU API clean up changes for MMODE.
10/10/11   cnx     Support LTE_RRC_SERVICE_IND.
08/04/11   cnx     Support pre-registeration information.
07/27/11   cnx     Merged Optimized Handoff changes.
06/02/10   smd     Included hdrmsg.h.
05/12/10   pba     Interface updates for DO to LTE - Reverse time transfer.
03/02/10   pba     Added support for LTE to HDR reselection
08/16/09   pba     Initial rev.
===========================================================================*/

#ifndef HDRCP_MSG_H
#define HDRCP_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"
#include <sys.h>
#include "hdrmsg.h"
#include "hdraddr.h"
#include "lte_rrc_irat_msg.h"
#include "lte_irat_types.h"
#include "lte_rrc_ext_msg.h"
#include "prot.h"
#include "prot_v.h"
#include "cm.h"
#include "homer.h"
#include "sys_v.h"
#include "cm_msgr_msg.h"

#ifdef __cplusplus
}  // extern "C"
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! \brief The text string for the external HDRCP Task Queue. */
#define HDRCP_MSG_Q_NAME              "HDRCP_Q"

#define HDRCP_MAX_CAP_CONTAINER_LENGTH 1000 /*! \brief MAX container size for HRPD capabilities container*/

#ifdef FEATURE_FPLMN_MAX_LENGTH /* support for 128 PLMN */
#define HDRCP_MAX_FPLMN_LIST_SIZE ((SYS_PLMN_LIST_MAX_LENGTH*2) + SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH)
#else
#define HDRCP_MAX_FPLMN_LIST_SIZE SYS_PLMN_LIST_MAX_LENGTH*3 
#endif /* FEATURE_FPLMN_MAX_LENGTH */


#define HDRCP_BCMCS_MAX_FLOWS SYS_BCMCS_20_MAX_FLOWS

#define HDRCP_MAX_LTE_DEPRI_FREQ LTE_IRAT_MAX_EUTRAN_FREQ /*! \brief MAX number of EUTRA freq deprioritized */

/*-----------------------------------------------------------------------*/
/*     HDRCP Supervisory                                                 */
/*-----------------------------------------------------------------------*/

/*! \brief Supervisory commands the HDR CP module receives.  */
enum
{
  /*! \brief Not used for now, sent from APPMGR */
  MSGR_DEFINE_UMID(HDR, CP, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_struct_type),
  /*! \brief Not used for now, send to APPMGR */
  MSGR_DEFINE_UMID(HDR, CP, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_struct_type) 

};

/*-----------------------------------------------------------------------*/

/*! \brief Supervisory Loopback  - This message is sent by the message router
           and is to be sent back to it in a hdrcp_spr_loopback_reply_t to
           confirm that the umb message queues are operating correctly. */
typedef msgr_spr_loopback_struct_type       hdrcp_spr_loopback_t;

/*! \brief Supervisory Loopback Reply  - This reply is sent back to the
           message router in response to receiving a hdrcp_spr_loopback_t
           message to confirm that the HDR CP message queues are operating
           correctly. */
typedef msgr_spr_loopback_reply_struct_type hdrcp_spr_loopback_reply_t;


/*! \brief Commands the HDR CP module receives. */
// Payload input for documentation only.          
enum
{
  /*! \brief The first message in the enum */
  HDR_CP_REQ_FIRST = MSGR_UMID_BASE(MSGR_HDR_CP, MSGR_TYPE_CMD),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_REDIR, 0x01, hdrcp_lte_redir_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_ABORT_REDIR, 0x02, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_1X_HRPD_CAPABILITIES, 0x03, hdrcp_lte_1x_hrpd_capabilities_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_RESEL, 0x04, hdrcp_lte_resel_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_ABORT_RESEL, 0x05, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_REVERSE_TIMING_TRANSFER, 0x06, hdrcp_lte_rev_tt_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_ABORT_REVERSE_TIMING_TRANSFER, 0x07, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_ACTIVE_HO, 0x08, hdrcp_lte_active_ho_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_ABORT_ACTIVE_HO, 0x09, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, PROT_ACT, 0x0A, hdrcp_activate_protocol_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, PROT_DEACT, 0x0B, hdrcp_deactivate_protocol_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, PROT_PH_STAT_CHGD, 0x0C, hdrcp_ph_status_chgd_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, PROT_GEN, 0x0D, hdrcp_generic_prot_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, CALL_ORIGINATION, 0x0E, hdrcp_call_origination_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, CALL_END, 0x0F, hdrcp_call_end_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, IDLE_PGSLOT_CHANGED, 0x10, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, UNLOCK_HDR_RF, 0x11, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, MEAS_MODE_ON, 0x12, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, MEAS_MODE_OFF, 0x13, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, MEAS_REQ, 0x14, hdrcp_irat_meas_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, SEND_DATA_OVER_SIGNALING, 0x15, hdrcp_send_data_over_signaling_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, TUNNEL_MSG, 0x16, hdrcp_tunnel_msg_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, CSNA_MSG_CONFIG, 0x17, hdrcp_csna_msg_config_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, SET_FTD_STATS_MASK, 0x18, hdrcp_set_ftd_stats_mask_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, BCMCS_UPDATE_FLOW , 0x19, hdrcp_bcmcs_update_flow_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, BCMCS_LEGACY_UPDATE_FLOW, 0x1A, hdrcp_bcmcs_req_msg_t), 
  MSGR_DEFINE_UMID(HDR, CP, REQ, BCMCS_DISCONTINUE_BROADCAST, 0x1B, hdrcp_bcmcs_discontinue_broadcast_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, BCMCS_LEGACY_DISCONTINUE_BROADCAST, 0x1C, hdrcp_bcmcs_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, BCMCS_SEND_REGISTRATION, 0x1D, hdrcp_bcmcs_send_registration_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, BCMCS_BOM_CACHING_SETUP, 0x1E, hdrcp_bcmcs_bom_caching_setup_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, BCMCS_ENABLE_REG_HANDOFF, 0x1F, hdrcp_bcmcs_enable_reg_handoff_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, QCHAT_OPT, 0x20, hdrcp_qchat_opt_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, REDIR_PROTOCOL_CNF, 0x21, hdrcp_redir_protocol_cnf_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, ACTIVATE_PROTOCOL_CNF, 0x22, hdrcp_activate_protocol_cnf_req_msg_t),  
  MSGR_DEFINE_UMID(HDR, CP, REQ, STANDBY_PREF_CHGD, 0x23, hdrcp_standby_pref_chgd_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, CM_ABORT_LTE_RESEL , 0x24, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_GET_CGI, 0x25, hdrcp_lte_get_cgi_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_ABORT_CGI, 0x26, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, CM_LTE_FPLMN_UPDATE, 0x27, hdrcp_cm_lte_fplmn_update_req_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, LTE_DEPRI_FREQ, 0x28, hdrcp_lte_depri_freq_req_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, MODEM_CONFIG_REFRESH, 0x29, hdrcp_modem_config_refresh_req_t),
  MSGR_DEFINE_UMID(HDR, CP, REQ, CM_CHECK_PSM_READY, 0x2a, none),
  MSGR_DEFINE_UMID(HDR, CP, REQ, UE_MODE_CHANGE, 0x2b, hdrcp_ue_mode_pref_chgd_req_msg_t),
  /*! \brief The last message in the enum */
  HDR_CP_REQ_LAST = HDR_CP_UE_MODE_CHANGE_REQ
};

/*! \brief Response the HDR CP module sends. */
// Payload input for documentation only.          
enum
{
  HDR_CP_RSP_FIRST = MSGR_UMID_BASE(MSGR_HDR_CP, MSGR_TYPE_RSP),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_REDIR_FAILED, 0x01, hdrcp_lte_redir_failed_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_REDIR_ABORT, 0x02, none),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_1X_HRPD_CAPABILITIES, 0x03, hdrcp_lte_1x_hrpd_capabilities_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_RESEL_FAILED, 0x04, hdrcp_lte_resel_failed_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_RESEL_ABORT, 0x05, none),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_REVERSE_TIMING_TRANSFER, 0x06, hdrcp_lte_rev_tt_rsp_msg_t),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_REVERSE_TIMING_TRANSFER_ABORT, 0x07, none),
  MSGR_DEFINE_UMID(HDR, CP, IND, RESELECT_EXIT_HDR, 0x08, none),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_ACTIVE_HO_FAILED, 0x09, hdrcp_lte_active_ho_fail_rsp_t),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_ACTIVE_HO_ABORT, 0x0A, none),
  MSGR_DEFINE_UMID(HDR, CP, RSP, LTE_GET_CGI, 0x0B, hdrcp_lte_get_cgi_rsp_msg_t),
  /*! \brief The last message in the enum */
  HDR_CP_RSP_LAST = HDR_CP_LTE_GET_CGI_RSP
};

/*! \brief indication the HDR CP module sends. */
// Payload input for documentation only.          
enum
{
  HDR_CP_IND_FIRST = MSGR_UMID_BASE(MSGR_HDR_CP, MSGR_TYPE_IND),
  MSGR_DEFINE_UMID(HDR, CP, IND, HRPD_PREREG_STATUS, 0x01, hdrcp_hrpd_prereg_status_ind_msg_t),
  /*! \brief The last message in the enum */
  HDR_CP_IND_LAST = HDR_CP_HRPD_PREREG_STATUS_IND
};

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR COMMANDS RECEIVED FROM LTE                    */
/*-----------------------------------------------------------------------*/

#define HDR_CP_MAX_CDMA2000_BANDS                   32
#define HDR_CP_MAX_CDMA2000_FREQUENCIES_PER_BAND    16
#define HDR_CP_MAX_PN_LIST_SIZE                     32

typedef struct
{
  /*!< Band Class */
  sys_band_class_e_type band_class;

  /*!< Num frequencies per band*/
  uint8 freq_count;

  /*!< Frequency - 0..2047 */
  sys_channel_num_type 
      freq_list[HDR_CP_MAX_CDMA2000_FREQUENCIES_PER_BAND];

} hdrcp_redir_freq_per_band_list;

typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  /*!< Num neighboring bands*/
  uint8 band_count; 

  /*!< Channel info */
  hdrcp_redir_freq_per_band_list 
      freq_per_band_list[HDR_CP_MAX_CDMA2000_BANDS];

} hdrcp_lte_redir_req_msg_t;

typedef struct
{
  /*!< brief Message header */
  msgr_hdr_struct_type           hdr;

  /*!< Band and Frequency  */
  sys_channel_type               resel_info;

  /*!< Cell Id - 0..maxPNOffset(511) */
  sys_pilot_pn_type              pilot_pn;

  /*!< LTE RTC counter */
  uint32                         lte_ostmr;

  /*!< CDMA System time */
  uint64                         cdma_sys_time;

} hdrcp_lte_resel_req_msg_t;

typedef struct
{
  msgr_hdr_struct_type           hdr;

  sys_band_mask_type             cdma_band_preference;

} hdrcp_lte_1x_hrpd_capabilities_req_msg_t;

typedef struct
{
  /*!< brief Message header */
  msgr_hdr_struct_type           hdr;

  /*!< Band and Frequency  */
  sys_channel_type               sys_info;

  /*!< Number of pn's in list */
  uint8                          pn_list_size;      

  /*!< Cell Id - 0..maxPNOffset(511) */
  sys_pilot_pn_type              pilot_pn[HDR_CP_MAX_PN_LIST_SIZE];

  /*!< Maximum time allowed by LTE for DO to do brute-force acq and sync.*/
  uint16                         time_allowed_ms;

} hdrcp_lte_rev_tt_req_msg_t;

/*!
    @brief
    Payload of Handover request received by  HDR CP from LTE.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type  hdr;

  /* Handover Message Container */
  msgr_attach_s         dsm_attach; 

  /*! LTE RTC counter, needed for Time Transfer */
  uint32                lte_ostmr;

  /*! CDMA System time,needed for Time Transfer */
  uint64                cdma_sys_time;

} hdrcp_lte_active_ho_req_msg_t;

/*!
    @brief
    Payload of Get CGI request received by HDR CP from LTE.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type  hdr;

  /*!< Band and Frequency of the cell for which CGI is requested */
  sys_channel_type      sys_info;

  /*!< Cell Id - 0..maxPNOffset(511) for the cell for which CGI is requested */
  sys_pilot_pn_type     pilot_pn;

  /*! LTE RTC counter, needed for Time Transfer */
  uint32                lte_ostmr;

  /*! CDMA System time */
  uint64                cdma_sys_time;

  /* Time in ms that HDR CP has, for acquiring CGI info; HDR CP has to send a 
    response back to LTE with a success/failure within this time */
  uint32                time_available_for_cgi;  


  /*! CDMA search_window_size, to be used by the AT for searching the pilots 
   in the neighbor pilot. Corresponding to each value for search_window_size,
   there is an associated PN Chips  If Value is 0xFF, then DO CP should use
   default parameters*/	
  uint8 search_window_size;
		
} hdrcp_lte_get_cgi_req_msg_t;


typedef struct 
{
  /*!< brief Message header */
  msgr_hdr_struct_type           hdr;   

  /*!< Reason for activating the CDMA protocol. */
  prot_act_e_type                act_reason;
    
} hdrcp_activate_protocol_req_msg_t;

typedef struct
{
  /*!< brief Message header */
  msgr_hdr_struct_type           hdr;
  
  /*!< Reason for sending activate confirmation to HDR protocol */
  prot_act_e_type                act_reason;   
    
} hdrcp_activate_protocol_cnf_req_msg_t;

typedef struct
{ 
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;
  
  /*!< Reason for deactivating the CDMA protocol. example, PWR_DOWN,RESET etc.*/
  prot_deact_e_type              deact_reason;
   
  /*!< Transaction Id for this request. */
  prot_trans_type                trans_id;   

} hdrcp_deactivate_protocol_req_msg_t;

typedef struct
{ 
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;
  
  /*!< Generic command information. */
  prot_gen_cmd_s_type            info;
   
} hdrcp_generic_prot_req_msg_t;

typedef struct
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;
  
  /*!< Type of phone status change( NAM change, operating mode change) */  
  prot_ph_stat_chg_e_type        chg_type;
  
  /*!< Current NAM selected by the user. */
  byte                           nam;
    
  /*!< Current CDMA subscription availability status. */    
  boolean                        is_cdma_subs_avail;

  /*!< Current operating mode of the phone. */
  prot_oprt_mode_e_type          oprt_mode;
    
  /*!< Transaction Id for this request. */
  prot_trans_type                trans_id;

  /*!< Enumeration of protocols that have a change in subscription available status */
  prot_subsc_chg_e_type          prot_subsc_chg;
    
} hdrcp_ph_status_chgd_req_msg_t;

typedef struct
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;
   
  /*!< Transaction ID to identify this command */
  prot_trans_type                trans_id;

  /*!< Standby preference */
  sys_modem_dual_standby_pref_e_type   info;

} hdrcp_standby_pref_chgd_req_msg_t;

typedef struct
{

  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;
    
  /*!< Reason for sending redirection confirmation to HDR */    
  prot_act_e_type                act_reason;

  /*!< Flag indicating whether IRAT HO is allowed */
  boolean                        is_irat_ho_allowed;
    
} hdrcp_redir_protocol_cnf_req_msg_t;

typedef struct
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;
  
  /*!< FTD statistics mask */
  uint64                         bit_mask;
  
} hdrcp_set_ftd_stats_mask_req_msg_t;


/* Type for SEND_DATA_OVER_SIGNALING */
   
typedef struct
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;

  /*!< Actual data is in a static buffer to save space */ 
  long                           dos_buf_ptr;                              

  /*!< DOS ACK required ? */  
  boolean                        ack_req;      

  /*!< Corresponding Link Flow number for the DoS packet */
  uint8                          rlp_flow;   

  /*!< Route on which the DoS packet should go*/
  byte                           route;

  boolean                        pb_cr;  

} hdrcp_send_data_over_signaling_req_msg_t;

/* Enumeration of digital mode.
*/
typedef enum 
{
  HDRCP_DIGITAL_MODE_NONE, /* none */
  HDRCP_DIGITAL_MODE_CDMA,  /* cdma  mode */
  HDRCP_DIGITAL_MODE_HDR    /* HDR  mode */

} hdrcp_digital_mode_type;

typedef struct 
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;                      

  boolean                        drs;

  /*!< last active data network */
  hdrcp_digital_mode_type        last_act_data_net; 

  /*!< call id to identify call instances */
  cm_call_id_type                call_id;         

} hdrcp_call_origination_req_msg_t;

typedef struct 
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;
  
  /*!< call id to identify call instances */          
  cm_call_id_type                call_id;

  /*!< the reason for ending a call */
  byte                           end_reason;  

} hdrcp_call_end_req_msg_t;

/* Type for HDRCP_BCMCS_UPDATE_FLOW_REQ_F (version 2.0)
*/
typedef struct
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;

  /*!< Transaction id provided by CM */
  byte                           transaction_id;     

  /*!< Number of flows in the arrays below */
  uint8                          num_of_flows;           

  sys_bcmcs_flow_addr_s_type     flow_addrs[ HDRCP_BCMCS_MAX_FLOWS ];

  sys_bcmcs_reg_setup_e_type     reg_setup_list[ HDRCP_BCMCS_MAX_FLOWS ];

} hdrcp_bcmcs_update_flow_req_msg_t;

/* Type for HDRCP_BCMCS_DISCONTINUE_BROADCAST_F
*/
typedef struct
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;          

  /*!< Transaction id provided by CM */
  byte                           transaction_id;
           
} hdrcp_bcmcs_discontinue_broadcast_req_msg_t;

/* Type for HDRCP_BCMCS_SEND_REGISTRATION_F
*/
typedef struct 
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;         

  /*!< Number of flows in the array below */
  uint8                          num_of_flows;           

  sys_bcmcs_flow_addr_s_type     flow_addrs[ HDRCP_BCMCS_MAX_FLOWS ];

} hdrcp_bcmcs_send_registration_req_msg_t;

/* Type for HDRCP_BCMCS_BOM_CACHING_SETUP_F
*/
typedef struct
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;         

  /*!< BOM caching setup */
  sys_bcmcs_bom_caching_e_type	 BOM_caching_setup;
  

  /*!< multiplier used to compute the staleness of BOM entry based on the BOP.
     Only valid if strict caching is enabled. */
  uint8  		                 stale_BOM_factor;
  
} hdrcp_bcmcs_bom_caching_setup_req_msg_t;

/* Type for HDRCP_BCMCS_ENABLE_REG_HANDOFF_F
*/
typedef struct
{
  /*!< brief Message header */  
  msgr_hdr_struct_type           hdr;         

  /*!< Number of flows in the array below */
  uint8                          num_of_flows;

  sys_bcmcs_flow_addr_s_type     flow_addrs[ HDRCP_BCMCS_MAX_FLOWS ];

} hdrcp_bcmcs_enable_reg_handoff_req_msg_t;


/* Legacy type for HDRCP_BCMCS_UPDATE_FLOW_REQ_F & HDRCP_BCMCS_DISCONTINUE_BROADCAST_F
*/
typedef struct 
{

  /*!< brief Message header */
  msgr_hdr_struct_type           hdr;

  /*!< Transaction id provided by CM */
  byte                           transaction_id;

  /*!< Number of flows in the array below */
  uint8                          num_of_flows;

  sys_bcmcs_flow_addr_s_type     flow_addrs[ HDRCP_BCMCS_MAX_FLOWS ];

} hdrcp_bcmcs_req_msg_t;

/* Type for hdrcp_qchat_opt_req_msg_t
*/
typedef struct 
{
  /*!< brief Message header */
  msgr_hdr_struct_type           hdr;

  unsigned int                   keep_awake_timer; 

} hdrcp_qchat_opt_req_msg_t;


typedef struct
{
  /*!< brief Message header */
  msgr_hdr_struct_type           hdr;

  homer_tunnel_msg_s_type        payload;

} hdrcp_tunnel_msg_req_msg_t;

typedef struct
{
  /*!< brief Message header */
  msgr_hdr_struct_type           hdr;

  /*!< P_rev of the 1X */
  byte                           p_rev; 

  /*!< Number of valid forward messages */
  int                            num_valid_cch_fwd_csna_msg; 

  /*!< fwd message configuration */
  homer_csna_msg_config_s_type   cch_fwd_csna_msg[ HOMER_MAX_CSNA_MSG ]; 
                                              
   /*!< Number of valid forward messages */
  int                            num_valid_ded_fwd_csna_msg;

  /*!< fwd message configuration */
  homer_csna_msg_config_s_type   ded_fwd_csna_msg[ HOMER_MAX_CSNA_MSG ]; 
                                      
  /*!< Number of valid reverse messages */        
  int                            num_valid_cch_rev_csna_msg;  

  /*!< Rev message configuration */
  homer_csna_msg_config_s_type   cch_rev_csna_msg[ HOMER_MAX_CSNA_MSG ];
      
  /*!< Number of valid reverse messages */      
  int                            num_valid_ded_rev_csna_msg;  

  /*!<  Rev message configuration */
  homer_csna_msg_config_s_type   ded_rev_csna_msg[ HOMER_MAX_CSNA_MSG ];
      
} hdrcp_csna_msg_config_req_msg_t;

typedef struct
{
  /*!< brief Message header */
  msgr_hdr_struct_type           hdr;

  cm_ss_hdr_info_s_type          pilot_meas;

} hdrcp_irat_meas_req_msg_t;


typedef struct
{ 
  msgr_hdr_struct_type hdr;

  /*!< Number of forbidden LTE PLMNs in the list */
  uint32 plmn_list_size;

  /*!< Forbidden LTE PLMN list */
  sys_plmn_id_s_type plmn[HDRCP_MAX_FPLMN_LIST_SIZE]; 

} hdrcp_cm_lte_fplmn_update_req_msg_t;

typedef struct
{
  /*! brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! Indicates if all EUTRA neighbor freqs should be deprioritized */
  boolean                  depri_all_freq;

  /*! Number of freqs being deprioritized; Will be set to a valid value 
      only if the depri_all_freq boolean is set to FALSE */
  uint8                    num_depri_freq;

  /*! Set of deprioritized EUTRA frequencies. This will be valid only 
    if the depri_all_freq boolean is set to FALSE */
  lte_earfcn_t             depri_freqs[HDRCP_MAX_LTE_DEPRI_FREQ];

} hdrcp_lte_depri_freq_req_t;

typedef struct
{
  /*! brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! Refresh for slot or sub specific items */
  uint8                    mcfg_refresh_type;  

  /*! Slot index for this refresh */          
  uint8                    mcfg_refresh_slot_index;
  
  /*! Subscription index for refresh */
  uint8                    mcfg_refresh_sub_index;

  /*! Reference ID for refresh indication */
  uint32                   reference_id;

} hdrcp_modem_config_refresh_req_t;

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR RESPONSE MESSAGES TO LTE RRC                  */
/*-----------------------------------------------------------------------*/

typedef enum
{
  HDRCP_LTE_RESEL_FAILED_PILOT_ACQ,
  HDRCP_LTE_RESEL_FAILED_SYNC_TIMEOUT,
  HDRCP_LTE_RESEL_FAILED_INVALID_PARAM,

} hdrcp_lte_resel_failed_rsp_reason_type;

typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

} hdrcp_lte_redir_failed_rsp_msg_t;

typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type               hdr;
  hdrcp_lte_resel_failed_rsp_reason_type failure_reason;

} hdrcp_lte_resel_failed_rsp_msg_t;

typedef struct
{
  msgr_hdr_struct_type hdr;

  uint32               capabilities_container_length;
  uint8                capabilities_container[HDRCP_MAX_CAP_CONTAINER_LENGTH];
} hdrcp_lte_1x_hrpd_capabilities_rsp_msg_t;


typedef enum
{
  HDRCP_LTE_REVERSE_TIME_TRANSFER_FAILED,
  HDRCP_LTE_REVERSE_TIME_TRANSFER_SUCCEEDED

} hdrcp_lte_rev_tt_rsp_status_type;

typedef struct
{
  /*!< brief Message header */
  msgr_hdr_struct_type                     hdr;

  /*!< Status fo time transfer */
  hdrcp_lte_rev_tt_rsp_status_type         status;

  /*!< Band and Frequency  */
  sys_channel_type                         sys_info;

  /*!< LTE RTC counter */
  uint32                                   lte_ostmr;

  /*!< CDMA System time */
  uint64                                   cdma_sys_time;

}hdrcp_lte_rev_tt_rsp_msg_t;

/*!
    @brief
    Payload of Response sent by HDR CP if handover fails.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type  hdr;

} hdrcp_lte_active_ho_fail_rsp_t;  

/*!
    @brief
    Payload of Get CGI response sent by HDR CP to LTE. This can be used as a 
    response for abort CGI request as well.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type  hdr;

  /*!< Band and Frequency of the cell for which CGI is requested */
  sys_channel_type      sys_info;

  /*! Cell Id - 0..maxPNOffset(511) */
  sys_pilot_pn_type     pilot_pn;

  /*! Pilot strength based on Measurements on the PN (0..63), a valid value 
    should always be included in the response, in -0.5dB */
  int16                pilot_strength;

  /* TRUE, if CGI acquisition suceeded and CGI info is included, 
    FALSE otherwise */
  boolean               cgi_info_included;     

  /*! CGI info in HDR - includes Access Terminal Identifier (ATI), Sector ID, 
    and Subnet ID*/
  hdraddr_type          cgi_info;

} hdrcp_lte_get_cgi_rsp_msg_t;

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR INDICATION MESSAGES SENT by HDR CP            */
/*-----------------------------------------------------------------------*/

typedef struct
{
  /*!< brief Message header */
  msgr_hdr_struct_type   hdr;

  /*!< Prereg status */
  boolean                ue_is_registered;

}hdrcp_hrpd_prereg_status_ind_msg_t;

typedef struct
{
  /*!< brief Message header */ 
  msgr_hdr_struct_type           hdr;
   
  /*!< Transaction ID to identify this command */
  prot_trans_type                trans_id;

  /* Indicate the current ue_mode of operation */
  sys_ue_mode_e_type             ue_mode;

  /* Indicates, if Ue is in SRTE dual satck mode or not */
  boolean                        is_sub_state_srlte;  

} hdrcp_ue_mode_pref_chgd_req_msg_t;

/*! \brief Union of all request message received by HDR CP */
typedef union {

  /*! \brief Message header */
  msgr_hdr_struct_type                     hdr;

  hdrcp_lte_redir_req_msg_t                lte_redirect;
  hdrcp_lte_resel_req_msg_t                lte_reselect;
  hdrcp_lte_1x_hrpd_capabilities_req_msg_t lte_capabilities_req;
  hdrcp_lte_rev_tt_req_msg_t               lte_rev_tt;
  hdrcp_lte_get_cgi_req_msg_t              lte_get_cgi_req;

  lte_rrc_hdr_sib8_info_ind_s              sib8_info;
  lte_rrc_hdr_pre_reg_info_ind_s           pre_reg_info;
  hdrcp_lte_active_ho_req_msg_t            lte_active_ho;

  lte_rrc_acq_db_earfcn_rsp_s              lte_rrc_acq_db_earfcn_rsp;

  hdrcp_activate_protocol_req_msg_t              activate_protocol;
  hdrcp_activate_protocol_cnf_req_msg_t          activate_protocol_cnf;
  hdrcp_deactivate_protocol_req_msg_t            deactivate_protocol;
  hdrcp_generic_prot_req_msg_t                   generic_prot;
  hdrcp_ph_status_chgd_req_msg_t                 ph_status_chgd;
  hdrcp_standby_pref_chgd_req_msg_t              sys_standby_cmd;
  hdrcp_redir_protocol_cnf_req_msg_t             redir_protocol_cnf;
  hdrcp_set_ftd_stats_mask_req_msg_t             ftd_stats;
  hdrcp_call_origination_req_msg_t               origination;
  hdrcp_call_end_req_msg_t                       call_end;
  hdrcp_bcmcs_update_flow_req_msg_t              bcmcs_update_flow;
  hdrcp_bcmcs_discontinue_broadcast_req_msg_t    bcmcs_discontinue_bc;
  hdrcp_bcmcs_send_registration_req_msg_t        bcmcs_send_reg;                       
  hdrcp_bcmcs_bom_caching_setup_req_msg_t        bcmcs_bom_caching;
  hdrcp_bcmcs_enable_reg_handoff_req_msg_t       bcmcs_reg_handoff;
  hdrcp_bcmcs_req_msg_t                          bcmcs_update_flow_legacy;
  hdrcp_bcmcs_req_msg_t                          bcmcs_discontinue_bc_legacy;
  hdrcp_send_data_over_signaling_req_msg_t       mo_dos;
  hdrcp_tunnel_msg_req_msg_t                     tunnel_msg;
  hdrcp_csna_msg_config_req_msg_t                csna_msg_config;
  hdrcp_qchat_opt_req_msg_t                      qchat_opt;
  hdrcp_irat_meas_req_msg_t                      irat_meas;

  hdrcp_cm_lte_fplmn_update_req_msg_t      cm_lte_fplmn;
  hdrcp_lte_depri_freq_req_t               lte_depri_freq_req;
  lte_rrc_eHRPD_resel_failed_rsp_s         lte_resel_failed_rsp;
  hdrcp_modem_config_refresh_req_t         modem_config_refresh;
  cm_trm_priority_ind_s_type               cm_trm_prioirty;
  hdrcp_ue_mode_pref_chgd_req_msg_t        ue_mode_chg_req;
  
} hdrcp_msg_req_u;

/*! \brief Union of all response messages sent by HDR CP */
typedef union {
  /*! \brief Message header */
  msgr_hdr_struct_type                     hdr;

  hdrcp_lte_redir_failed_rsp_msg_t         lte_redirect_failed;
  hdrcp_lte_resel_failed_rsp_msg_t         lte_reselect_failed;
  hdrcp_lte_1x_hrpd_capabilities_rsp_msg_t lte_capabilities_rsp;
  hdrcp_lte_rev_tt_rsp_msg_t               lte_rev_tt;
  hdrcp_lte_get_cgi_rsp_msg_t              lte_get_cgi_rsp;

  hdrcp_lte_active_ho_fail_rsp_t           lte_active_ho_failed;

} hdrcp_msg_rsp_u;

/*! \brief Union of all indication messages sent by HDR CP */
typedef union {
  /*! \brief Message header */
  msgr_hdr_struct_type                                   hdr;

  hdrcp_hrpd_prereg_status_ind_msg_t         hrpd_prereg_status;

} hdrcp_msg_ind_u;

/*\}*/

#define HDRCP_MAX_REQ_MSG_SIZE sizeof(hdrcp_msg_req_u)
#define HDRCP_MAX_RSP_MSG_SIZE sizeof(hdrcp_msg_rsp_u)
#define HDRCP_MAX_IND_MSG_SIZE sizeof(hdrcp_msg_ind_u)

#define HDRCP_NUM_REQ_MSG       (HDR_CP_REQ_LAST - HDR_CP_REQ_FIRST + 1)
#define HDRCP_NUM_RSP_MSG       (HDR_CP_RSP_LAST - HDR_CP_RSP_FIRST + 1)
#define HDRCP_NUM_IND_MSG       (HDR_CP_IND_LAST - HDR_CP_IND_FIRST + 1)

#endif /* HDRCP_MSG_H */
