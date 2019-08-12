#ifndef PROT_H
#define PROT_H
/*===========================================================================

                        C O M M O N    P R O T O C O L
                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all common types required for the MMoC to
  communicate with the protocols and vice versa.

Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM_MM/APPS/vcs/prot.h_v   1.1   07 Jun 2002 13:23:22   sjaikuma  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/prot.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/19/13   jvo     Remove all MDM/MSM Fusion features
11/30/11   jh      Added feature for dynamic rat acq order change
01/13/12   as      support for SO 33 call limitation on SVLTE/SVDO targets
11/07/11   mj      Added tds_band_pref to prot_cmd_gw_get_net_s_type.
05/19/11   ak      Adding new prot state for blocked RAT on SVLTEII.
05/06/11   mp      Added lte_band_pref to prot_cmd_gw_get_net_s_type.
04/25/11   rk      Adding user mode pref in prot_cmd_pref_sys_chgd_s_type.
03/02/11   ay      TD-SCDMA support
02/24/11   rm      PS detach before change of mode/band preference
02/07/11   rm      LTE RAT-B changes
12/22/10   sv      DSDS API changes
12/02/10   rk      Adding new protocol deact reason
10/29/10   aj      support for DO LTE reselection
10/07/10   rk      Adding support for dormant GWL protocol handling in MMOC
09/23/10   rk      1XCSFB MO/MT call support
09/21/10   sv      Added msm_sys_chgd command structures
06/18/10   xs      Add SENTINEL metacomments for maximum value sentinel in the enum
12/11/09   rn      LTE-eHRPD Support
12/11/09   fj      Added lte_band_pref.
07/13/09   np      Added code for LTE support
09/10/09   rm      Removing inclusion of customer.h from /api files
02/23/09   rm      CMI Phase-2 : Removing inclusion of private header files
                   from public
02/06/09   aj      Update prot_subsc_chg_e_type
07/30/08   rm      CMI defeaturisation
09/04/08   sv      Added WLAN_WPS_START and WLAN_WPS_ABORT generic protocol
                   commands
03/13/08   sk      Added PROT_DEACT_FROM_DORMANT to support CM/CMREGPRX
                   changes for allowing CDMA scans during suspended PS data
                   call.
04/17/07   rp      Added RPC metacomments for WLAN.
01/04/07   pk      Added support for VoIP -> 1X Handoff
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/03/06   ka      Changing copyright.
02/28/05   ka      Added new enums and macros for recording protocol that
                   had a subscription change.
09/28/04   sj      Added support for WLAN.
08/20/04   sj      Added support for prl_pref.
09/03/02   SJ      Unfeaturized GSM support in MMoC.
06/07/02   SJ      Renamed PROT_TRANS_NONE_PENDING to PROT_TRANS_NONE.
03/28/02   SJ      Initial release.

===========================================================================*/
#include "comdef.h"    /* Definition for basic types and macros */
#include "sd.h"        /* Interface to System Determination 2.0 */
#include "sys.h"       /* common header file */

/* A type that defines the command sequence number type in MMOCs command to
** different protocols.
*/

typedef byte prot_trans_type;
typedef sys_sim_state_e_type prot_cmd_gw_sim_state_e_type;

/* Enumeration of protocol states in the MMoC.
*/

typedef enum {

  PROT_STATE_NULL,                     /* Initial state upon powerup. */


  PROT_STATE_ONLINE_CDMA,              /* Phone is in ONLINE mode with CDMA
                                       ** as one of the active protocols.
                                       */

  PROT_STATE_OFFLINE_CDMA,             /* Phone is in OFFLINE mode with CDMA
                                       ** as the active protocol.
                                       */

  PROT_STATE_ONLINE_AMPS,              /* Phone is in ONLINE mode with AMPS
                                       ** as one of the active protocols.
                                       */

  PROT_STATE_OFFLINE_AMPS,             /* Phone is in OFFLINE mode with CDMA
                                       ** as the active protocol.
                                       */

  PROT_STATE_ONLINE_GWL,                /* Phone is in ONLINE mode with GSM/
                                       ** WCDMA /LTE as one of the active
                                       ** protocols.
                                       */

  PROT_STATE_ONLINE_HDR,               /* Phone is in ONLINE mode with HDR
                                       ** as one of the active protocols.
                                       */

  PROT_STATE_OFFLINE,                  /* Phone is in OFFLINE mode with no
                                       ** active protocols.
                                       */

  PROT_STATE_FTM,                      /* Phone is in FTM mode. Here CDMA is
                                       ** the active protocol, since FTM is
                                       ** implemented in MC task.
                                       */

  PROT_STATE_PWR_SAVE,                 /* Phone is in Power save mode with no
                                       ** active protocols.
                                       */


  PROT_STATE_ONLINE_DED_MEAS,          /* Phone is doing signal strength
                                       ** measurement in dedicated mode.
                                       ** Here CDMA is active protocol since
                                       ** the logic is implemented in MC
                                       ** task.
                                       */

  PROT_STATE_RESET,                    /* Phone is in RESET mode with no
                                       ** active protocols.
                                       */

  PROT_STATE_LPM,                      /* Phone is in LPM mode with no
                                       ** active protocols.
                                       */

  PROT_STATE_PWR_DOWN,                 /* Phone is powering down.
                                       */

  PROT_STATE_GPSONE_MSBASED,           /* Phone is in GPS state.
                                       */

  PROT_STATE_DORMANT_GWL,               /* Phone is in ONLINE mode, with
                                       ** GW protocol in DORMANT state
                                       */

  PROT_STATE_BLOCKED,                  /* Internal protocol state for
                                       ** MMSS arbitration.
                                       */

  PROT_STATE_MAX                       /* For internal use. */


} prot_state_e_type;
/*~ SENTINEL prot_state_e_type.PROT_STATE_MAX */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of protocol activate reason type for CDMA, AMPS, HDR, GSM/WCDMA
** protocols.
*/
typedef enum {

  PROT_ACT_ONLINE_ACQ,                 /* Activate a protocol to acquire a
                                       ** system, acquisition parameters given
                                       ** by SD20.
                                       ** Applicable for CDMA, AMPS, HDR,
                                       ** GSM/WCDMA protocols.
                                       */

  PROT_ACT_OFFLINE,                    /* Activate the protocol and place it
                                       ** in OFFLINE mode.
                                       ** Applicable for CDMA and AMPS
                                       ** protocol.
                                       */

  PROT_ACT_FTM,                        /* Activate the CDMA protocol and
                                       ** place it in FTM(Factory Test
                                       ** mode).
                                       ** Applicable for CDMA protocol.
                                       */

  PROT_ACT_CDMA_AMPS_HANDOFF,          /* Activate the AMPS protocol for the
                                       ** reason of performing a CDMA to AMPS
                                       ** handoff. The handoff is provided in
                                       ** activate protocol command.
                                       ** Applicable for AMPS protocol.
                                       */

  PROT_ACT_GET_NETWORKS,               /* Activate the protocol for the
                                       ** reason of performing a Get networks
                                       ** command.
                                       ** Applicable for GSM/WCDMA protocol.
                                       */

  PROT_ACT_DED_MEAS_REQUEST,           /* Activate the CDMA protocol for the
                                       ** reason of initiating a measurement
                                       ** request to SRCH in dedicated mode.
                                       ** Applicable for CDMA protocol.
                                       */

  PROT_ACT_PSEUDO_ONLINE,              /* Activate the protocol to acquire a
                                       ** system but no Tx should be done by
                                       ** the protocol.
                                       */

  PROT_ACT_ONLINE_HDR_CDMA_HO,         /* Activate CDMA for Handoff
                                       */

  PROT_ACT_GWL_REDIR,                  /* Activate HDR/1x for Redirection
                                       ** from LTE
                                       */

  PROT_ACT_LTE_HANDOVER,               /* Activate 1X for Handover
                                       ** from LTE
                                       */

  PROT_ACT_MAX                         /* For internal use */

} prot_act_e_type;
/*~ SENTINEL prot_act_e_type.PROT_ACT_MAX */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of protocol deactivate reason type for CDMA, AMPS, HDR,
** GSM/WCDMA protocols.
*/
typedef enum {
  /* 0 */
  PROT_DEACT_PWR_DOWN,                 /* Deactivate a protocol in order to
                                       ** perform power down command from UI
                                       ** Applicable for all protocols.
                                       */

  PROT_DEACT_RESET,                    /* Deactivate a protocol in order to
                                       ** perform reset command from UI
                                       ** Applicable for all protocols.
                                       */

  PROT_DEACT_LPM,                      /* Deactivate a protocol in order to
                                       ** perform LPM command from UI
                                       ** Applicable for all protocols.
                                       */

  PROT_DEACT_OFFLINE,                  /* Deactivate a protocol in order to
                                       ** perform offline command from UI
                                       ** Applicable for all protocols.
                                       */
  /* 4 */
  PROT_DEACT_CDMA_AMPS_HANDOFF,        /* Deactivated a protocol in order to
                                       ** perform a CDMA to AMPS handoff.
                                       ** Generated by CDMA protocol only.
                                       */

  PROT_DEACT_ACQ_OTHERS,               /* Generic deactivated protocol reason.
                                       ** Applicable for all protocols.
                                       */


  PROT_DEACT_SUBSC_CHGD,               /* Protocol should be deactivated
                                       ** because of change in subscription.
                                       */

  PROT_DEACT_FTM,                      /* Protocol should be deactivated
                                       ** because of FTM.
                                       */
  /* 8 */
  PROT_DEACT_FROM_DORMANT,             /* Protocol should be deactivated
                                       ** because dormancy is no longer
                                       ** required.
                                       */

  PROT_DEACT_GWL_REDIR,                /* Protocol should be deactivated
                                       ** because network issued Redirection
                                       */

  PROT_DEACT_REDIR_ABORT,              /* Protocol should be deactivated
                                       ** because Redirection was aborted
                                       */

  PROT_DEACT_LTE_HANDOVER,             /* Protocol should be deactivated
                                       ** because network issued Handover
                                       */


  /* 12 */
  PROT_DEACT_DUAL_SWITCH,              /* Protocol should be deactivated
                                       ** to inform dual standby pref switch.
                                       */

  PROT_DEACT_LOCAL_DETACH,             /* Protocol should be deactivated
                                       ** locally. No need to inform network.
                                       */
  PROT_DEACT_NO_SYS_TO_ACQ,            /* Protocol should declare no service
                                        ** locally. No need to inform network.
                                        */

  PROT_DEACT_DSDX_SUSP,               /* Protocol should declare no service 
                                          ** locally  for eg. due to emerg call 
                                          ** on other stack.
                                        */

  PROT_DEACT_CAP_CHANGE,              /* Stack capability is changed
                                           ** due to multimode subs change
                                           */
  PROT_DEACT_MAX                       /* For internal use */

} prot_deact_e_type;
/*~ SENTINEL prot_deact_e_type.PROT_DEACT_MAX */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of generic command for CDMA, AMPS, HDR, GSM/WCDMA
** protocols.
*/
typedef enum {

  /* 0 */
  PROT_GEN_CMD_SS_TIMER_EXP,           /* Reselection timer expired command.
                                       */

  PROT_GEN_CMD_PREF_SYS_CHGD,          /* Preferred system selection changed
                                       ** command.
                                       */

  PROT_GEN_CMD_GET_NETWORKS_GW,        /* Get networks command for GSM/WCDMA.
                                       */

  PROT_GEN_CMD_TERM_GET_NETWORKS_GW,   /* Terminate Get networks command for
                                       ** GSM/WCDMA.
                                       */
  /* 4*/
  PROT_GEN_CMD_GET_NETWORKS_WLAN,      /* Get networks command for WLAN.
                                       */

  PROT_GEN_CMD_TERM_GET_NETWORKS_WLAN, /* Terminate Get networks command for
                                       ** WLAN.
                                       */

  PROT_GEN_CMD_WLAN_WPS_START,         /**< WPS start command for WLAN.
                                       */

  PROT_GEN_CMD_WLAN_WPS_ABORT,         /**< WPS abort command for WLAN.
                                       */
  /* 8 */
  PROT_GEN_CMD_IRAT_TO_GWL,            /**< IRAT Reselection to GWL.
                                       */

  PROT_GEN_CMD_GW_SIM_STATE,          /**< T3245 timer expiry from NAS
                                       */


  PROT_GEN_CMD_LOCAL_DEACT,            /**< Service Transfer:G(CS) -> G(CS+PS)
                                       */
  
  PROT_GEN_CMD_LOCAL_ACT,              /**< Service Transfer:G(CS+PS) -> L(PS)+G(CS)
                                       */
  /* 12 */
  PROT_GEN_CMD_WAKEUP_FROM_PWR_SAVE,   /**< Wakeup from power save command.
                                       */

  PROT_GEN_CMD_MAX                     /* For internal use */

} prot_gen_cmd_e_type;
/*~ SENTINEL prot_gen_cmd_e_type.PROT_GEN_CMD_MAX */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of operating mode status in MMoC.
*/
typedef enum
{
  PROT_OPRT_MODE_NULL,                    /* Operating mode status when the
                                          ** phone has powered up.
                                          */

  PROT_OPRT_MODE_PWROFF,                  /* Operating mode status when the
                                          ** phone was powered down.
                                          */

  PROT_OPRT_MODE_OFFLINE,                 /* Operating mode status when the
                                          ** phone moved to offline mode.
                                          */


  PROT_OPRT_MODE_OFFLINE_AMPS,            /* Operating mode status when the
                                          ** phone moved to offline AMPS mode.
                                          */


  PROT_OPRT_MODE_OFFLINE_CDMA,            /* Operating mode status when the
                                          ** phone moved to offline CDMA mode.
                                          */


  PROT_OPRT_MODE_ONLINE,                  /* Operating mode status when the
                                          ** phone moved to online mode.
                                          */

  PROT_OPRT_MODE_LPM,                     /* Operating mode status when the
                                          ** phone moved to LPM mode.
                                          */


  PROT_OPRT_MODE_RESET,                   /* Operating mode status when the
                                          ** phone moved to reset mode.
                                          */


  PROT_OPRT_MODE_FTM,                     /* Operating mode status when the
                                          ** phone moved to FTM mode.
                                          */


  PROT_OPRT_MODE_MAX                      /* For internal use.
                                          */

} prot_oprt_mode_e_type;
/*~ SENTINEL prot_oprt_mode_e_type.PROT_OPRT_MODE_MAX */

/* Enumeration of phone status change type for CDMA, AMPS, HDR, GSM/WCDMA
** protocols. Applicable whenever there is a change in SIM availability,
** NAM selection change or operating mode change. The protocols will do
** specific housekeeping actions based on this type.
*/
typedef enum {

  PROT_PH_STAT_CHG_OPRT_MODE,          /* Phone status changed due to change
                                       ** in operating mode.
                                       */

  PROT_PH_STAT_CHG_SUBSC,              /* Phone status changed due to change
                                       ** in subscription.
                                       */

  PROT_PH_STAT_CHG_NAM,                /* Phone status changed due to change
                                       ** in NAM selection.
                                       */

  PROT_PH_STAT_CHG_MAX                 /* For internal use */

} prot_ph_stat_chg_e_type;
/*~ SENTINEL prot_ph_stat_chg_e_type.PROT_PH_STAT_CHG_MAX */

/* A type for parameters that are associated with the preferred system
** selection changed command, part of generic system selection command to
** the protocols.
*/
typedef struct {

  /* Reason for changing the SS-Preference.  */
  sd_ss_pref_reas_e_type               pref_reas;

  /* New origination mode (normal, OTASP, emergency, other, etc.).
  */
  sd_ss_orig_mode_e_type               orig_mode;

  /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
  ** orig_mode = OTASP or emergency.
  */
  sd_ss_mode_pref_e_type               mode_pref;

  /* New band preference (BC0, BC1, any, etc.) - ignored when
  ** orig_mode = OTASP  or emergency.
  */
  sd_ss_band_pref_e_type               band_pref;

  /* New LTE band preference - ignored when
  ** orig_mode = OTASP  or emergency.
  */
  sys_lte_band_mask_e_type             lte_band_pref;

  /*
  ** TDS-CDMA band preference
  */
  sd_ss_band_pref_e_type               tds_band_pref;

  /* New prl preference.
  */
  sd_ss_prl_pref_e_type                prl_pref;

  /* New roam preference (any, home, affiliated, etc.) - ignored
  ** when orig_mode = OTASP or emergency.
  */
  sd_ss_roam_pref_e_type               roam_pref;

  /* New hybrid preference (none, CDMA/HDR).
  */
  sd_ss_hybr_pref_e_type               hybr_pref;

  /* The band-class that is associated with OTASP origination mode.
  ** Note that this parameter is ignored if orig_mode != OTASP.
  */
  sd_band_e_type                       otasp_band;

  /* The PCS frequency block/Cellular system that is associated
  ** with OTASP origination mode. Note that this parameter is
  ** ignored if orig_mode != OTASP.
  */
  sd_blksys_e_type                     otasp_blksys;

  /* Type of the system to be avoided, valid only if
  ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
  */
  sd_ss_avoid_sys_e_type               avoid_type;

  /* Time in seconds for which the system is to be avoided,
  ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
  */
  dword                                avoid_time;

  /* Information about the manual system.
  */
  sys_manual_sys_info_s_type           manual_sys_info;

  /* New service domain preference (CS, PS, CS_PS etc).
  ** Applicable for GSM/WCDMA modes.
  */
  sd_ss_srv_domain_pref_e_type         domain_pref;

  /* Preference for the order of acquisition between modes.
  ** ( WCDMA before GSM, GSM before WCDMA etc ).
  */
  sd_ss_acq_order_pref_e_type          acq_order_pref;

  /* Indicate ss pref update reason
  */
  sd_ss_pref_update_reason_e_type      pref_update_reas;

  /*  To indicate sub action to be performed by MMOC during
  **  pref sys chgd cmd
  */
  sys_addtl_action_s_type               addl_action;

  /* The user/phone mode preference
  */
  sd_ss_mode_pref_e_type                user_mode_pref;

  /* Rat acquisition preference order including LTE
  */
  sd_rat_acq_order_s_type               rat_acq_order;

  /* Request Id
  */
  uint16                                sys_sel_pref_req_id;

  /* Indicate ss pref camp mode
  */
  sd_ss_pref_camp_mode_e_type           camp_mode_pref;

  /* CSG identifier
  */
  sys_csg_id_type                       csg_id;

  /* RAT specified for CSG
  */
  sys_radio_access_tech_e_type          csg_rat;

  sys_voice_domain_pref_e_type          voice_domain_pref;
  /* voice domain pref 
  */

} prot_cmd_pref_sys_chgd_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type for parameters that are associated with the GSM/WCDMA get networks
** command.
*/
typedef struct {

  /* Mode preference for the Get networks command. Can be GSM_ONLY,
  ** WCDMA_ONLY or GW_ONLY only.
  */
  sd_ss_mode_pref_e_type               mode_pref;


  /* Band preference that is associated with the Get networks
  ** command.
  */
  sd_ss_band_pref_e_type               band_pref;

  /* Band preference that is associated with the Get networks
  ** command for LTE
  */
  sys_lte_band_mask_e_type             lte_band_pref;

  /* Band preference that is associated with the Get networks
  ** command for TD-SCDMA
  */
  sd_ss_band_pref_e_type               tds_band_pref;

  /* Network list type for Get networks command
  */
  sd_network_list_type_e_type         network_list_type;


} prot_cmd_gw_get_net_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type for parameters that are associated with the acquired system information
** from MSM.
*/
typedef struct {

  /* The band class of the new system.
  */
  sd_band_e_type            band;

  /* The CDMA channel of the new system.
  */
  sd_chan_type              chan;

  /* Serving system's SID.
  */
  uint16                      sid;

  /* Serving system's NID.
  */
  uint16                      nid;

} prot_cmd_msm_sys_chgd_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type for parameters that are associated generic system selection command
** to the protocols.
*/
typedef struct {

  /* Generic command type (reselection timeout, get networks etc).
  */
  prot_gen_cmd_e_type                  cmd_type;

  /* Transaction Id for the generic command.
  ** Note! trans_id is used only when communicating with the protocols.
  ** Invalid when it is in the MMoC's command queue.
  */
  prot_trans_type                      trans_id;


  /* Generic command to this SD SS Instance.
  */
  sd_ss_e_type                         ss;


  /* MMoC's current protocol state for ss.
  */
  prot_state_e_type                    prot_state;

  /* Parameters associated with the above ss command type.
  */
  union
  {

    prot_cmd_pref_sys_chgd_s_type        pref_sys_chgd;

    prot_cmd_gw_get_net_s_type           gw_get_net;

    prot_cmd_msm_sys_chgd_s_type         msm_sys_chgd;

    prot_cmd_gw_sim_state_e_type         gw_sim_state;

  } param;

} prot_gen_cmd_s_type;
/*~ FIELD prot_gen_cmd_s_type.param DISC prot_gen_cmd_s_type.cmd_type */
/*~ CASE PROT_GEN_CMD_PREF_SYS_CHGD prot_gen_cmd_s_type.param.pref_sys_chgd */
/*~ CASE PROT_GEN_CMD_GET_NETWORKS_GW prot_gen_cmd_s_type.param.gw_get_net */
/*~ DEFAULT prot_gen_cmd_s_type.param.void */

#endif /* PROT_H */
