#ifndef HDRMC_V_H
#define HDRMC_V_H

/*===========================================================================

        E X T E R N A L   H D R   S U B T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains the external declarations for the HDRMC Task.

Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrmc_v.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/26/16   vko     Fixed race condition between HDR inactive and 
                          HDRMC_STANDBY_PREF_CHGD_CMD_F
05/03/16   vko     FR 35747: Graceful handling of UE mode collision and X2L IRAT
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
11/28/15   vko     Do not reset ehrpd credential during mcfg refresh
09/14/15   vko     FR 29020: LTE Power Save Mode w/o coordination with network
07/23/15   vko     FR 29379 : Disable tuneaway during Qchat call
02/02/15   vko     Add srch window param for D2L CGI
12/28/14   vko     Debug info for hdr
07/09/14   cnx     Support NV refresh without reset.
01/16/14   cnx     Read JCDMA mode from 1x JCDMA mode NV.
09/19/13   ukl     Reduce Code Duplication HDR-CP
07/31/13   sat     Mainlining Modem Statistics Feature
06/28/13   cnx     Abort D2L if AdviseUnlock rx'ed during D2L resel.
02/17/13   arm     Supported NV enable/disable for network dtx attributes. 
03/31/13   cnx     Support LTE deprioritization.
02/05/13   ukl     Move more APIs to api header files.
12/14/12   cnx     Replace FEATURE_DUAL_SIM with FEATURE_HDR_DUAL_SIM.
11/06/12   cnx     Delay processing tunnel activation if idle handoff in progress
08/03/12   wsh     Defined QPCH feature, but disable by NV
06/19/12   cnx     AT moves back to tunnel after getting CGI if tunnel mode 
                   is enabled.
06/11/12   cnx     Fixed compiler errors.
04/27/12   cnx     Avoid Reselection from eHRPD to LTE forbidden PLMNs.
04/20/12   cnx     Unsubcribe LTE_RRC_SERVICE_IND.
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
03/08/12   wsh     Merged NLB support
03/01/12   cnx     Subscribe ATTACH COMPLETE.
02/24/12   smd     Removed feature defines from SU API header files.
01/17/12   wsh     Added support for interband HO threshold 
11/10/11   cnx     Fixed compiler errors.
10/28/11   pba     Optimized handoff phase 2 support
10/11/11   arm     SU API clean up changes for MMODE. 
10/10/11   cnx     Support LTE_RRC_SERVICE_IND.
08/22/11   cnx     Fixed compiler error.
08/19/11   wsh     Replaced depreciated MMGSDI API calls
08/04/11   cnx     Support pre-registeration information.
07/27/11   cnx     Merged Optimized Handoff changes.
05/18/11   wsh     Added network load balancing support
04/27/11   cnx     Supported callback registration.
03/25/11   pba     Removed featurization of HDRMC_CM_ABORT_LTE_RESEL_REQ
10/27/10   wsh     Added dual standby mode support
09/15/10   pxu     Added supported jcdma features to hdrmc_feature.
06/10/10   pba     IRAT Measurement feature.
06/01/10   pba     Added support for FTD stats.
05/19/10   pba     Added support for Reverse Time Transfer.
03/22/10   pba     Added support for eHRPD session negotiation based on 
                   confirming availability of USIM App
03/02/10   pba     Added support for LTE to HDR reselection
12/01/09   suren   Added QChat optimization changes.
10/26/09   wsh     Workaround for HDRMC queue full
08/06/09   wsh     Support CMI API changes in sys.h 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/27/09   wsh     Added run-time check for KDDI feature flags
02/18/09   wsh     Remove ext-AU definitions, renamed hdrmc_v
11/25/08   wsh     Featurized dynamic BCMCS related APIs
05/05/08   mpa     Added support for BCMCS2.0 phase 1 and stubs for phase 2
01/17/07   yll     Changes for HOMER interface.
01/08/07   yll     Added support for CSNA
12/19/06   pba     Reverted DOS link_flow_number to rlp_flow.
11/17/06   pba     Added support for DOS over EMPA.
08/28/05   mpa     Renamed BCMCS cmds and vars to be consistent w/ HDRBCCP
05/09/05   ar/pba  Integrated Data Over Signaling protocol code
02/28/05   ka      Adding prot_subsc_chg param to hdrmc_ph_status_chgd_cmd
11/29/04   dna     Send END_CNF to CM with call_id of user ended call.
05/18/04   mpa     Backed out BCMCS support (in BCMCS branch instead)
03/02/04   ic      Added BCMCS support. 
                   Explicitely numbered HDRMC commands. 
01/13/04   mpa     (for sj)  Added hdrmc_hdr_unlock_rf_cmd().
10/15/03    km     Moved some commands from mc.h to hdrmc.h
08/10/03   mpa     Merged in MSM6500 baseline
07/31/03   km(sj)  Added function HDRMC_MULTIMODE_SANITY_ERR_FATAL.
                   Renamed mc_digital_mode_type to hdrmc_digital_mode_type.
02/23/03   ht      Based on asw/COMMON/vcs/hdrmc.h_v   1.11
                   Add changes to support HDR 3GData call.
01/21/02   sh      Added more exit reasons
01/04/02   sh      Added hybrid operation support
09/24/01   sh      Added HDRMC_EXIT_RESET
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
07/10/01   dna     Code cleanup changes
05/01/01   dna     Made HDRMC its own task instead of a subtask.
02/02/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "mc.h"

#ifdef FEATURE_CMI
#include "mc_v.h"
#endif

#if defined (FEATURE_HDR_TO_LTE) || defined (FEATURE_LTE_TO_HDR_OH)
#include "lte_rrc_irat_msg.h"
#include "lte_rrc_ext_msg.h"
#endif /* FEATURE_HDR_TO_LTE || FEATURE_LTE_TO_HDR_OH */

#include "prot.h"

#ifdef FEATURE_CMI
#include "prot_v.h"
#endif

#include "cai.h"

#ifdef FEATURE_CMI
#include "cai_v.h"
#endif

#include "sys.h"
#include "cm.h"
#include "cmxll.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#ifndef FEATURE_CMI
#include "hdrtask.h"
#endif

#include "homer.h"

#include "hdrcp_api.h"
#include "hdrerrno.h"
#include "hdrhai.h"
#include "hdrmc.h"
#include "hdraddr.h"

#ifdef FEATURE_HDR_MC_MSGR
#include "hdrcp_msg.h"
#endif /* FEATURE_HDR_MC_MSGR */

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"

#ifdef FEATURE_CMI
#include "mmgsdilib_v.h"
#endif

#endif /* FEATURE_MMGSDI*/

#ifdef FEATURE_LTE_TO_HDR_OH
#include "emm_ext_msg.h"
#include "emm_irat_if_msg.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

#include "rcinit.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Address of the TCB for the HDRMC task */
extern rex_tcb_type *hdrmc_tcb_ptr;

#define HDRMC_CMD_TYPE hdrmc_msg_type


#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#else
#define HDRMC_BCMCS_MAX_FLOWS SYS_BCMCS_10_MAX_FLOWS
#endif

#ifdef FEATURE_HDR_TO_LTE
/* DO to LTE Reselection status */
#define  HDRMC_HDR_LTE_RESELECT_SUCCESS 0
#define  HDRMC_HDR_LTE_RESELECT_FAIL 1
#define  HDRMC_HDR_LTE_RESELECT_ABORT 2  
#endif

/*-------------------------------------------------------------------------*/

/* Union of all command types.  The header is always present and it        */
/* specifies the command type and attributes.  The command messages        */
/* follow in the union.                                                    */

typedef enum 
{

  /*-------------------------------------------------------------------------
    All MSGR based messages are addded below. MSGR glue code 
    translates MSGR messages and adds it to the legacy task event queue. Here 
    the MSGR UMID's are assigned to the enums of command that task already 
    receives.
    -------------------------------------------------------------------------*/

#ifdef FEATURE_LTE_TO_HDR
  /* LTE to HDR redirection request. */
  HDRMC_LTE_REDIRECT_REQ = HDR_CP_LTE_REDIR_REQ,

  /* Abort LTE to HDR redirection request. */
  HDRMC_LTE_ABORT_REDIRECT_REQ = HDR_CP_LTE_ABORT_REDIR_REQ,

  /* 1x - HRPD capabilities request from LTE */
  HDRMC_LTE_1X_HRPD_CAPABILITIES_REQ = HDR_CP_LTE_1X_HRPD_CAPABILITIES_REQ,

  /* LTE to HDR reselection request. */
  HDRMC_LTE_RESELECT_REQ = HDR_CP_LTE_RESEL_REQ,

  /* Abort LTE to HDR reselection request. */
  HDRMC_LTE_ABORT_RESELECT_REQ = HDR_CP_LTE_ABORT_RESEL_REQ,

  /* Timing transfer request from LTE. */
  HDRMC_LTE_REVERSE_TIMING_TRANSFER_REQ = HDR_CP_LTE_REVERSE_TIMING_TRANSFER_REQ,

  /* Abort timing transfer request. */
  HDRMC_LTE_ABORT_REVERSE_TIMING_TRANSFER_REQ = HDR_CP_LTE_ABORT_REVERSE_TIMING_TRANSFER_REQ,
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDR_TO_LTE
  /* LTE reselect failed response. */
  HDRMC_LTE_RESEL_FAILED_RSP = LTE_RRC_eHRPD_RESEL_FAILED_RSP,

  /* Abort LTE reselect response. */
  HDRMC_LTE_ABORT_RESEL_RSP = LTE_RRC_eHRPD_ABORT_RESEL_RSP,
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
  /* LTE reselect failed response. */
  HDRMC_LTE_REDIR_FAILED_RSP = LTE_RRC_eHRPD_REDIR_FAILED_RSP,

  /* Abort LTE redirect response. */
  HDRMC_LTE_ABORT_REDIR_RSP = LTE_RRC_eHRPD_ABORT_REDIR_RSP,

#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */

#if defined FEATURE_HDR_TO_LTE_REDIRECTION || defined FEATURE_HDR_TO_LTE

/* ACQ DB EARFCN info from LTE-RRC */
HDRMC_LTE_RRC_ACQ_DB_EARFCN_RSP = LTE_RRC_ACQ_DB_EARFCN_RSP,

#endif /* defined FEATURE_HDR_TO_LTE_REDIRECTION || defined FEATURE_HDR_TO_LTE */

#ifdef FEATURE_LTE_TO_HDR_OH
  /* LTE attach complete indication */
  HDRMC_LTE_ATTACH_COMPLETE_IND = NAS_EMM_ATTACH_COMPLETE_IND,
  
  /* SIB8 indication from LTE */
  HDRMC_LTE_RRC_HDR_SIB8_INFO_IND = LTE_RRC_HDR_SIB8_INFO_IND,

   /* Pre-Registration information from LTE */
  HDRMC_LTE_RRC_HDR_PRE_REG_INFO_IND = LTE_RRC_HDR_PRE_REG_INFO_IND,

  /* LTE to HDR Active Handoff Request */
  HDRMC_LTE_ACTIVE_HO_REQ = HDR_CP_LTE_ACTIVE_HO_REQ,

  /* LTE to HDR Abort Active Handoff Request */
  HDRMC_LTE_ABORT_ACTIVE_HO_REQ = HDR_CP_LTE_ABORT_ACTIVE_HO_REQ,
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Abort LTE reselection req from CM */
  HDRMC_CM_ABORT_LTE_RESEL_REQ = HDR_CP_CM_ABORT_LTE_RESEL_REQ,

  /* MMOC command to activate HDR protocol.
  */
  HDRMC_PROT_ACT_REQ = HDR_CP_PROT_ACT_REQ,

  /* MMOC command to deactivate HDR protocol.
  */
  HDRMC_PROT_DEACT_REQ = HDR_CP_PROT_DEACT_REQ,

  /* MMoC command to inform ph status changed.
  */
  HDRMC_PROT_PH_STAT_CHGD_REQ = HDR_CP_PROT_PH_STAT_CHGD_REQ,

  /* Generic command from MMOC.
  */
  HDRMC_PROT_GEN_REQ = HDR_CP_PROT_GEN_REQ,

  /* Call origination 
  */
  HDRMC_ORIGINATION_REQ = HDR_CP_CALL_ORIGINATION_REQ,

  /* Call end.
  */
  HDRMC_END_REQ = HDR_CP_CALL_END_REQ, 

  /* CDMA idle pging slot changed
  */
  HDRMC_IDLE_PGSLOT_CHANGED_REQ = HDR_CP_IDLE_PGSLOT_CHANGED_REQ,

  /* Advise HDR that 1x needs the lock
  */
  HDRMC_UNLOCK_HDR_RF_REQ = HDR_CP_UNLOCK_HDR_RF_REQ,

  /* BCMCS - request flows with HDR
  */
  HDRMC_BCMCS_UPDATE_FLOW_REQ = HDR_CP_BCMCS_UPDATE_FLOW_REQ,

  /* BCMCS - request flows with HDR
  */
  HDRMC_BCMCS_LEGACY_UPDATE_FLOW_REQ = HDR_CP_BCMCS_LEGACY_UPDATE_FLOW_REQ,

  /* BCMCS - deregister flows that had been previousely
  ** registered with HDR
  */
  HDRMC_BCMCS_DISCONTINUE_BROADCAST_REQ = HDR_CP_BCMCS_DISCONTINUE_BROADCAST_REQ,

  /* BCMCS - deregister flows that had been previousely
  ** registered with HDR
  */
  HDRMC_BCMCS_LEGACY_DISCONTINUE_BROADCAST_REQ = HDR_CP_BCMCS_LEGACY_DISCONTINUE_BROADCAST_REQ,
  
  /* Send an mobile originated data over signaling message.
  */
  HDRMC_SEND_DATA_OVER_SIGNALING_REQ = HDR_CP_SEND_DATA_OVER_SIGNALING_REQ,

  /* Tunneled message from CM
  */
  HRDMC_TUNNEL_MSG_REQ = HDR_CP_TUNNEL_MSG_REQ,

  /* Allowed/Supported CSNA Messages
  */
  HDRMC_CSNA_MSG_CONFIG_REQ = HDR_CP_CSNA_MSG_CONFIG_REQ, 

  /* BCMCS 2.0 - Request to send a Registration message.
  */
  HDRMC_BCMCS_SEND_REGISTRATION_REQ = HDR_CP_BCMCS_SEND_REGISTRATION_REQ, 

#if defined(FEATURE_HDR_BCMCS_2p0) && defined(FEATURE_HDR_BCMCS_DYNAMIC_MODEL)
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 && FEATURE_HDR_BCMCS_DYNAMIC_MODEL */

#ifdef FEATURE_HDR_QCHAT
  /* QChat otimization
  */
  HDRMC_QCHAT_REQ = HDR_CP_QCHAT_OPT_REQ,

#endif /*FEATURE_HDR_QCHAT*/

#ifdef FEATURE_LTE_TO_HDR
  /* Conf from SD for LTE to HDR redirection */
  HDRMC_PROT_LTE_TO_HDR_HO_CNF_REQ = HDR_CP_REDIR_PROTOCOL_CNF_REQ,

  /* Conf from SD for auto HDR activation */
  HDRMC_PROT_ACT_CNF_REQ = HDR_CP_ACTIVATE_PROTOCOL_CNF_REQ,
 
#endif /* FEATURE_LTE_TO_HDR */

  /* Start collecting and reporting FTD statistics  
  */
  HDRMC_SET_FTD_STATS_MASK_REQ = HDR_CP_SET_FTD_STATS_MASK_REQ,


#ifdef FEATURE_IRAT_PILOT_MEAS
  /* Activate measurement mode
  */
  HDRMC_MEAS_MODE_ON_REQ = HDR_CP_MEAS_MODE_ON_REQ,

  /* De-activate measurement mode
  */
  HDRMC_MEAS_MODE_OFF_REQ = HDR_CP_MEAS_MODE_OFF_REQ,

  /* IRAT measurement request 
  */
  HDRMC_MEAS_REQ_CMD_REQ = HDR_CP_MEAS_REQ, 

#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_HDR_DUAL_SIM
  /* Standby Preference Change Cmd 
  */
  HDRMC_STANDBY_PREF_CHGD_REQ = HDR_CP_STANDBY_PREF_CHGD_REQ,

#endif /* FEATURE_HDR_DUAL_SIM */

#ifdef FEATURE_LTE_TO_HDR
  /* LTE to HDR CGI request. */
  HDRMC_LTE_GET_CGI_REQ = HDR_CP_LTE_GET_CGI_REQ,

  /* Abort LTE to HDR CGI request. */
  HDRMC_LTE_ABORT_CGI_REQ = HDR_CP_LTE_ABORT_CGI_REQ,

#endif /* FEATURE_LTE_TO_HDR */

  /* Update LTE forbidden PLMN list */
  HDRMC_CM_LTE_FPLMN_UPDATE_REQ = HDR_CP_CM_LTE_FPLMN_UPDATE_REQ,

#ifdef FEATURE_LTE_DEPRIORITIZATION
  /* Update LTE deprioritized frequency */
  HDRMC_LTE_DEPRI_FREQ_REQ = HDR_CP_LTE_DEPRI_FREQ_REQ,
#endif /* FEATURE_LTE_DEPRIORITIZATION */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  HDRMC_MODEM_CONFIG_REFRESH = HDR_CP_MODEM_CONFIG_REFRESH_REQ,
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

HDRMC_CM_TRM_PRIORITY_IND = MM_CM_TRM_PRIORITY_IND,

HDRMC_CM_CHECK_PSM_READY_REQ = HDR_CP_CM_CHECK_PSM_READY_REQ,

HDRMC_UE_MODE_CHANGE_REQ = HDR_CP_UE_MODE_CHANGE_REQ,

} hdrmc_name_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Status returned from the Main Control task */

typedef enum 
{
  HDRMC_DONE_S,            /* General OK return */
  HDRMC_BUSY_S,            /* Command is being processed */
  HDRMC_BADCMD_S,          /* Invalid command */
  HDRMC_BADPARM_S          /* Bad Parameters */
} hdrmc_status_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Command header type.  Each command to MC starts with this header.       */

typedef struct 
{
  cmd_hdr_type        cmd_hdr;      /* Command header */
  hdrmc_name_type     cmd;          /* Command name */
  hdrmc_status_type   status;       /* Completion code status */
} hdrmc_hdr_type;

#ifdef FEATURE_LTE_TO_HDR_OH
typedef struct
{
  emm_irat_3gpp2_dl_msg_ind_type msg_payload; 
  /* Payload of active handoff */

  boolean free_dsm_item;
  /* Flag indicating whether we need to free DSM item or not */

}hdrmc_lte_to_hdr_active_ho_info_type;
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_MC_MSGR
/* Structure for messages received through MSG Router module */
typedef struct
{
  hdrmc_hdr_type  hdr; 
    /* Header */

  hdrcp_msg_req_u msg;
    /* Message structure */

} hdrmc_msgr_s_type;
#endif /* FEATURE_HDR_MC_MSGR */

typedef union 
{
  hdrmc_hdr_type              hdr; /* Header                         */

#ifdef FEATURE_HDR_MC_MSGR
  hdrmc_msgr_s_type               msgr_cmd;
#endif /* FEATURE_HDR_MC_MSGR */

} hdrmc_msg_type;

typedef enum
{
  HDRMC_EXIT_NORMAL,
  HDRMC_EXIT_OFFLINE,
  HDRMC_EXIT_POWER_DOWN,
  HDRMC_EXIT_RESET,
  HDRMC_EXIT_HYBR_POWERSAVE,
  HDRMC_EXIT_LPM,
  HDRMC_EXIT_HYBR_TO_NONHYBR,
  HDRMC_EXIT_PREF_SYS_CHANGED,
  HDRMC_EXIT_RESELECTION,
  HDRMC_EXIT_DEACTIVATE
} hdrmc_exit_enum_type;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  States of the HDRMC                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
typedef enum 
{
  HDRMC_ONLINE_STATE,          /* HDR online state                  */
  HDRMC_INACTIVE_STATE         /* Inactive state                   */
} hdrmc_state_type;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  Features related to JCDMA (should only be enabled for JCDMA            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
typedef enum
{
  HDRMC_FEATURE_JCDMA,
  HDRMC_FEATURE_HDR_KA_REQUEST_DISABLED,
  HDRMC_FEATURE_HDR_SESSION_TIED_TO_PPP,
  HDRMC_FEATURE_HDR_DISABLE_SUSPEND_TIMER,
  HDRMC_FEATURE_HDR_ALLOW_SPIBI_HANDOFFS,
  HDRMC_FEATURE_HDR_ACCOLC_ALTERNATE_MAPPING,
  HDRMC_FEATURE_HDR_NO_ACTIVE_HANDDOWN,
  HDRMC_FEATURE_REMOVE_OFF_BAND_NEIGHBORS,
  HDRMC_FEATURE_HDR_RL_RATE_INERTIA,
  HDRMC_FEATURE_KDDI_RESTRICTED_SESSION_CALL_FLOW,
  HDRMC_FEATURE_KDDI_OFS_HO_THRESHOLD,
  HDRMC_FEATURE_NETWORK_LOAD_BALANCING,
  HDRMC_FEATURE_MULTI_AT_PAGE,
  HDRMC_FEATURE_QUICK_IDLE,
  HDRMC_FEATURE_HDR_NETWORK_DTX,
  HDRMC_FEATURE_HDR_JCDMA_NV
} hdrmc_feature_enum_type;

#ifdef FEATURE_HDR_DEBUG_INFO

typedef enum
{
  HDR_NONE = 0,
  HDR_MMOC_RPT_DS_STAT_CHGD_CNF,
  HDR_MMOC_RPT_PROT_DEACTD_CNF,
  HDR_MMOC_RPT_PH_STAT_CHGD_CNF,
  HDR_MMOC_RPT_PROT_GEN_CMD, 
  HDR_MMOC_RPT_PROT_AUTO_ACTD_IND,
  HDR_MMOC_RPT_PROT_AUTO_DEACTD_IND,
  HDR_LTE_RRC_EHRPD_RESEL_REQ,
  HDR_LTE_RRC_EHRPD_ABORT_RESEL_REQ,
  HDR_LTE_REDIR_FAILED_RSP,
  HDR_LTE_RESEL_FAILED_RSP,
  HDR_LTE_REVERSE_TIMING_TRANSFER_RSP,
  HDR_LTE_GET_CGI_RSP,
  HDR_LTE_GET_CGI_FAILED_RSP,
  HDR_LTE_GET_CGI_ABORT_RSP, 
  HDR_LTE_ACTIVE_HO_FAILED_RSP,
  HDR_LTE_REDIR_ABORT_RSP,
  HDR_LTE_RESEL_ABORT_RSP,
  HDR_LTE_REVERSE_TIMING_TRANSFER_FAILED_RSP,
  HDR_LTE_REVERSE_TIMING_TRANSFER_ABORT_RSP,
  HDR_LTE_ACTIVE_HO_ABORT_RSP,
  HDR_LTE_1X_HRPD_CAPABILITIES_RSP
}hdrmc_sent_cmd_type;

#endif /* FEATURE_HDR_DEBUG_INFO */

#ifdef HDRMC_DEBUG
q_type *hdrmc_event_q_ptr;
q_type *hdrmc_cmd_q_ptr;
extern boolean hdrmc_online( rex_sigs_type sigs );
extern void hdrmc_enter_online( void );
#endif /* HDRMC_DEBUG */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  Features related to QMI callback functions                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* HDR callback types */
#define hdrmc_cb_event_enum_type              hdrcp_mc_cb_event_enum_type

/* SESSION CLOSE callback parameters */
#define hdrmc_cb_session_close_params_type    hdrcp_mc_cb_session_close_params_type

/* UATI UPDATE callback parameters */
#define hdrmc_cb_uati_update_params_type      hdrcp_mc_cb_uati_update_params_type

/* QMI callback parameter union */
#define hdrmc_cb_params_rsp_union_type        hdrcp_mc_cb_params_rsp_union_type


/* Type for a hdr callback function. A client must register for that function 
 * with HDR if it wants to be notified. maintained so that QMI_NAS 
 * compiles successfully has to be deleted eventually
 */
typedef void (*hdrmc_cb_f_type) ( 
  hdrmc_cb_params_rsp_union_type *cb_data_ptr,
  void                           *client_data_ptr 
);

/* Registration enum type */
#define hdrmc_cb_reg_enum_type                hdrcp_mc_cb_reg_enum_type

/* Client enum type */
#define hdrmc_cb_client_id_enum_type          hdrcp_mc_cb_client_id_enum_type

/*===========================================================================

FUNCTION HDRMC_CLIENT_CB_REG

DESCRIPTION
  This function allows a client to register/deregister a callback with HDR to
  receive registered HDR events.
  
  Only one event can be registered once. To register multiple events, call this
  function several times.

DEPENDENCIES
  None
 
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
  None

===========================================================================*/
hdrerrno_enum_type hdrmc_client_cb_reg
(
  hdrmc_cb_client_id_enum_type      client,
    /* client id */
  
  hdrmc_cb_event_enum_type          cb_event,
    /* Events that client wants to register */ 
       
  hdrmc_cb_reg_enum_type            reg_type,
    /* Register or deregister */      

  hdrmc_cb_f_type                   cb_func_ptr,
    /* Pointer to a callback function to notify the client */    

  void                              *client_data_ptr
    /* Pointer to the client data */  
);

/*===========================================================================

FUNCTION HDRMC_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  This routine also
  checks any queues that are being waited on.  If the queue has an item,
  this routine sets the signal without waiting.  This allows for the queue
  signal to be cleared after each item is removed.  If a queue signal is set,
  this routine checks the queue and clears the signal if there is nothing
  on it.  This centralizes the queue checking.

DEPENDENCIES
  None

PARAMETERS
  requested_mask - Mask of signals to wait for

RETURN VALUE
  For non-queue signals, a signal bit is set in the return mask if the signal
  was in the requested mask and is also set in the REX TCB.

  For queue signals, a signal bit is set in the return mask if the signal was
  in the requested mask and the queue associated with the signal has an item
  on it.

SIDE EFFECTS
  None

===========================================================================*/

extern rex_sigs_type hdrmc_wait
(
  rex_sigs_type requested_mask
);

/*===========================================================================

FUNCTION HDRMC_GET_CMD_BUF

DESCRIPTION
  This function allocates a HDRMC task command buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to allocated CM task command buffer if allocation succeed.
  NULL if allocation failed.

SIDE EFFECTS
  None.
===========================================================================*/
extern HDRMC_CMD_TYPE *hdrmc_get_cmd_buf(void);

/*===========================================================================

FUNCTION HDRMC_CMD

DESCRIPTION
  This is the outside world's interface to the HDRMC task and subtasks.
  this function takes an already filled out hdrmc_msg_type and places it
  on the hdrmc command queue.  The function returns to the caller after
  setting the status to busy indicaton.  This does _not_ mean that the
  command has been executed yet, just that it is waiting to be executed.

DEPENDENCIES
  The HDRMC task must have been started up already.  All of the items in
  the hdrmc_cmd_type must be already set.

RETURN VALUE
  None

SIDE EFFECTS
  The hdrmc_cmd_type is placed on the HDRMC command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/
void hdrmc_cmd
(
  HDRMC_CMD_TYPE  *msg_ptr   /* Pointer to MC message buffer */
);

/*===========================================================================

FUNCTION HDRMC_GET_CMD_FREE_Q_PTR

DESCRIPTION
  This function return the the HDRMC free queue pointer
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to HDRMC cmd free queue


SIDE EFFECTS
  None.
===========================================================================*/
extern q_type *hdrmc_get_cmd_free_q_ptr(void);


/*===========================================================================

FUNCTION HDRMC_SET_MODE_HYBR_PREF

DESCRIPTION
  This function set the current mode and hybr preference

DEPENDENCIES
  None

PARAMETERS
  new_mode_pref - New mode preference
  new_hybr_pref - New hybrid preference

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_set_mode_hybr_pref
(
  sd_ss_mode_pref_e_type new_mode_pref,
  sd_ss_hybr_pref_e_type new_hybr_pref
);



/*===========================================================================

FUNCTION HDRMC_SS_TIMER

DESCRIPTION
  This function enables or disables system selection timer. It is used by
  system determination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrmc_ss_timer
(
  int4 ss_timer_duration /* Duration in ms for the timer to be active */
);

/*===========================================================================

FUNCTION HDRMC_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
  The function to call the err_fatal because of the reason passed in.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_multimode_sanity_err_fatal
(
     mmoc_sanity_err_e_type     reason
);

/*===========================================================================*/
void hdrmc_set_cur_call_id
(
  cm_call_id_type call_id
);

/*===========================================================================*/
cm_call_id_type hdrmc_get_cur_call_id
(
  void
);

/*===========================================================================

FUNCTION HDRMC_SEND_END_CNF

DESCRIPTION
  This function sends a orig fail report to the CM task.

DEPENDENCIES
  None.

PARAMETERS
  call_id - The call ID of the call that was ended.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_send_end_cnf
(
  cm_call_id_type call_id 
);

/*===========================================================================
FUNCTION HDRMC_SEND_ORIG_FAILURE

DESCRIPTION
  This function sends a orig fail report to the CM task.

DEPENDENCIES
  None.

PARAMETERS
  err_code - The reason for the origination failure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_send_orig_failure
( 
  cm_call_orig_fail_e_type err_code
);

/*===========================================================================

FUNCTION HDRMC_FEATURE_IS_ENABLED

DESCRIPTION
  The function checks if a JCDMA sub-feature is enabled.

DEPENDENCIES
  None  

PARAMETERS
  feature - JCDMA related feature to check against

RETURN VALUE
  True - If the feature is enabled
  FALSE - Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_feature_is_enabled
(
  hdrmc_feature_enum_type feature
);

/*===========================================================================
FUNCTION HDRMC_SET_FEATURE_JCDMA

DESCRIPTION
  The function enable/disable a JCDMA

DEPENDENCIES
  None

PARAMETERS
  enabled - To enable/disable JCDMA

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_set_feature_jcdma
(
  boolean enabled
);

/*===========================================================================

FUNCTION HDRMC_EVENT_Q_OVER_THRESHOLD

DESCRIPTION
  The function checks if HDRMC event Q has more items than a predetermined
  threshold.

DEPENDENCIES
  HDRMC queue must be initialized

RETURN VALUE
  TRUE - If HDRMC event Q is over threshold
  FALSE - Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_event_q_over_threshold
( 
  hdrhai_protocol_name_enum_type protocol_name 
);

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_HO_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether LTE to HDR is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if LTE to HDR HO is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_ho_is_in_progress( void );

/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_RESEL_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether reselection is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if reselection is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_resel_is_in_progress( void );

/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_GET_CGI_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether get CGI is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if get CGI is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_get_cgi_is_in_progress( void );

/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_REDIR_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether redirection is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if redirection is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_redir_is_in_progress( void );

/*===========================================================================

FUNCTION HDRMC_SEND_IRAT_HO_FAILURE_IND_TO_LTE

DESCRIPTION
  This function returns failure response to LTE when HDR redirection/
  reselection attempt fails.

DEPENDENCIES
  back_to_tunnel - whether HDR needs to go back to tunnel.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_send_irat_ho_failure_ind_to_lte
(
  boolean back_to_tunnel
);

/*===========================================================================

FUNCTION HDRMC_SEND_ABORT_DONE_TO_LTE

DESCRIPTION
  This function returns LTE->HDR redirection abort done response to LTE stack.

DEPENDENCIES
  back_to_tunnel - whether HDR needs to go back to tunnel.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_send_abort_done_to_lte
(
  boolean back_to_tunnel
);

/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_HO_IS_ABORTED

DESCRIPTION
  This function returns whether LTE to HDR handoff was aborted.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - if LTE to HDR handoff was aborted. FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_ho_is_aborted( void );

/*===========================================================================

FUNCTION HDRMC_GET_RESELECT_TIMING_INFO

DESCRIPTION
  This function is used to retrieve the reselection timing information passed
  to HDR by LTE stack.

DEPENDENCIES
  None.

PARAMETERS
  lte_ostmr - LTE RTC counter
  cdma_sys_time - CDMA System time

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_reselect_timing_info 
(
  uint32 *lte_ostmr,
  uint64 *cdma_sys_time
);

/*===========================================================================

FUNCTION HDRMC_GET_RESELECT_PILOT_PN

DESCRIPTION
  This function is used to retrieve the reselection pilot pn passed to HDR by 
  LTE stack.

DEPENDENCIES
  None.

PARAMETERS
  pilot_pn - Pointer to HDR pilot pn

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_reselect_pilot_pn 
(
  sys_pilot_pn_type *pilot_pn
);

/*===========================================================================

FUNCTION HDRMC_SEND_TIMING_INFO_TO_LTE

DESCRIPTION
  This function is called to send the timing information back to LTE.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_send_timing_info_to_lte( void );

/*===========================================================================

FUNCTION HDRMC_SEND_CGI_TO_LTE

DESCRIPTION
  This function is called to send the CGI back to LTE.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_send_cgi_to_lte( void );

/*===========================================================================
FUNCTION HDRMC_HDR_TO_LTE_TIME_TRANSFER_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether LTE to HDR is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if LTE to HDR HO is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_hdr_to_lte_time_transfer_is_in_progress( void );

/*===========================================================================

FUNCTION HDRMC_GET_TIMING_TRANSFER_PN_LIST

DESCRIPTION
  This function is used to retrieve the timing transfer pn list passed to HDR
  by LTE stack.

DEPENDENCIES
  None.

PARAMETERS
  pn_list_size - Number of pn's in list
  pilot_pn - Pointer to HDR pilot pn

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_timing_transfer_pn_list 
(
  uint8             *pn_list_size,
  sys_pilot_pn_type *pilot_pn
);

/*===========================================================================

FUNCTION HDRMC_GET_DURATION_ALLOWED_FOR_TIME_TRANSFER

DESCRIPTION
  This function is used to retrieve the Maximum time allowed by LTE for DO 
  to do brute-force acq and sync.

DEPENDENCIES
  None.

PARAMETERS
  time_allowed_ms - Maximum time allowed by LTE for DO to do brute-force 
  acq and sync.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_duration_allowed_for_time_transfer 
(
  uint16 *time_allowed_ms
);

/*===========================================================================

FUNCTION HDRMC_GET_CGI_SRCH_WINDOW_SIZE

DESCRIPTION
  This function is used to retrieve the cgi search window size information passed
  to HDR by LTE stack.

DEPENDENCIES
  None.

PARAMETERS
  srch_window_size - Pointer to SRCH window size

RETURN VALUE
  SRCH window size

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_cgi_srch_window_size 
(
  uint8 *srch_window_size
);

#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_HDR_NEW_GSDI_API
/*===========================================================================

FUNCTION HDRMC_GET_REG_MMGSDI_SID

DESCRIPTION
  This function returns the session_id associated with MMGSDI.

DEPENDENCIES
  mmgsdi_client_id_reg should have been called already.

PARAMETERS
  None
  
RETURN VALUE
  MMGSDI client id.
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmc_get_reg_mmgsdi_sid
( 
  mmgsdi_session_id_type* sid
);
#else
/*===========================================================================

FUNCTION HDRMC_GET_REG_MMGSDI_CID

DESCRIPTION
  This function returns the client_id associated with MMGSDI.

DEPENDENCIES
  mmgsdi_client_id_reg should have been called already.

PARAMETERS
  None
  
RETURN VALUE
  MMGSDI client id.
  
SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_client_id_type hdrmc_get_reg_mmgsdi_cid( void );
#endif /* FEATURE_HDR_NEW_GSDI_API */
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION HDRMC_GET_STATS_INFO 

DESCRIPTION
  This function is used by CM get the FTD statistics.

DEPENDENCIES
  None.

PARAMETERS
  ftd_stats_ptr - Pointer to location where the FTD stats need to be copied.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmc_get_stats_info 
(
  sys_hdr_information_s_type *ftd_stats_ptr
);

#ifdef FEATURE_HDR_REVC
/*===========================================================================
FUNCTION HDRMC_SET_QOS_FLOW_STATUS

DESCRIPTION
  This function is to be called by DS layer when QoS flow status changes:
    1) when any QoS flow becomes active on HDR tech
    2) when all QoS flow become inactive/unrequested

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmc_set_qos_flow_status
(
  const boolean active
);

/*===========================================================================
FUNCTION HDRMC_IS_QOS_FLOW_ACTIVE

DESCRIPTION
  This function returns current QoS flow status set by DS

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  True  - if any QoS flow is active
  False - if no QoS flow is active

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmc_is_qos_flow_active( void );
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION HDRMC_GET_ACTIVE_HANDOFF_PAYLOAD

DESCRIPTION
  This function returns the payload (HSP, OLP and TCA) received from LTE for 
  handoff.

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
hdrmc_lte_to_hdr_active_ho_info_type* hdrmc_get_active_handoff_payload( void );

/*===========================================================================
FUNCTION HDRMC_TUNNEL_ACTIVATION_IS_PENDING

DESCRIPTION
  This function returns the value of tunnel_activation_is_pending flag.

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  boolean value of flag tunnel_activation_is_pending

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_tunnel_activation_is_pending( void );

/*===========================================================================
FUNCTION HDRMC_SET_TUNNEL_ACTIVATION_IS_PENDING

DESCRIPTION
  This function sets the vaule of tunnel_activation_is_pending flag

DEPENDENCIES 
  None 
 
PARAMETERS 
  value - boolean value of flag tunnel_activation_is_pending
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_set_tunnel_activation_is_pending
(
  boolean value
);
#endif /* FEATURE_LTE_TO_HDR_OH */


/*===========================================================================
FUNCTION HDRMC_IS_FIRST_INIT

DESCRIPTION
  This function tells whether the task has been initialized or not

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  TRUE if HDRMC is not initialized yet. FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_is_first_init( void );

#ifdef FEATURE_HDR_DEBUG_INFO

/*===========================================================================
FUNCTION HDRMC_LOG_RCVD_RPT

DESCRIPTION
  This function logs the rcvd event from MMOC/LTE in hdr debug buffer

DEPENDENCIES 
  None 
 
PARAMETERS 
  Transaction id - transaction id used only for MMODE commands
  cmd - name of the command receieved
 
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void hdrmc_log_rcvd_rpt(prot_trans_type transaction_id, hdrmc_name_type cmd);

/*===========================================================================
FUNCTION HDRMC_LOG_SENT_RPT

DESCRIPTION
  This function logs the sent event to MMOC/LTE in hdr debug buffer

DEPENDENCIES 
  None 
 
PARAMETERS 
  Transaction id - transaction id used only for MMODE commands
  cmd - name of the command sent
 
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void hdrmc_log_sent_rpt(prot_trans_type transaction_id, hdrmc_sent_cmd_type cmd);

#endif /* FEATURE_HDR_DEBUG_INFO */


/*===========================================================================
FUNCTION HDRMC_MCFG_REFRESH_IN_PROGRESS

DESCRIPTION
  This function checks if mcfg refresh is in progress

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  value - boolean value of flag 

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_mcfg_refresh_in_progress(void);

/*===========================================================================
FUNCTION HDRMC_SET_HDR_EXITING

DESCRIPTION
  This function sets the vaule of hdr exiting flag

DEPENDENCIES 
  None 
 
PARAMETERS 
  value - boolean value of flag 
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_set_hdr_exiting
(
  boolean value
);
#endif /* HDRMC_V_H */
