#ifndef CMLL_H
#define CMLL_H
/*===========================================================================

              C A L L   M A N A G E R   L O W E R   L A Y E R

                       H E A D E R   F I L E

DESCRIPTION
GENERAL DESCRIPTION
  This source file contains the function definitions necessary for the
  Call Manager module and the Lower Layers to send commands to one
  another. This file contains only the common interfaces, i.e. interfaces
  shared by all protocol stacks. Individual protocol interfaces are
  defined in their respective header files


Copyright (c) 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/cmll.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/12/13   sm      1xSRVCC support 
06/05/13   qf      Added support for IMS rat_change between LTE and WLAN  
04/30/13   skk     Enhance Interface For Packet Attach/Detach
04/11/13   xs      FRAT support
02/11/13   xs      SD send acq fail to CM during HDR silent redial
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/07/12   xs      Added PS supplemetary service support
09/26/12   vm      Correcting numbering in cm_name_e
09/19/12   skk     Added support for "ESM_SM_backoff timer" requirements
03/01/12   fj      Added support for DRX.
04/30/12   xs      Add support of sending FPLMN list
04/04/12   xs      Codec info propagation
03/08/12   xs      Added codec info support for VOLTE
03/02/12   vk      Added feature to get DCH cell info.
02/13/12   xs      VT upgrade/downgrade support
01/20/12   gm      Data synchronization for CM Call objects
01/13/12   cl      Support SO-33 CALL_END_CNF_F from 1X-CP
01/13/12   as      support for SO 33 call limitation on SVLTE/SVDO targets
01/06/12   gm      Data synchronization changes for freefloating platform
10/26/11   xs      L->W SRVCC support
05/23/11   mj      Remove obsolete CM_LOAD_DATA_CNF, CM_SAVE_DATA_CNF
04/14/11   rm      Cell broadcast info changes
03/08/11   cl      Fusion 2 MMSS SD->CM reporting for stack status
03/08/11   cl/gm   Fusion 2 MMSS MMOC->CM reporting
03/03/11   gm      Added separate queue processing for M2M indications
02/26/11   sn/rm   Support for graceful pdp deactivation before detach.
02/24/11   rm      PS detach before change of mode/band preference
02/02/11   xs      Wait for DS notification for PWROFF/LPM oprt mode change
12/22/10   sv      Adding new report CM_DS_STAT_CHGD_CNF
12/21/10   rk      Adding new report CM_LTE_1X_IRAT_F
11/19/10   rm      Adding DSAC changes
10/19/10   fj      Added CM_BEARER_CTXT_TRANSFER_IND to indicate call object
                   transfer during LTE to GW redirection/reselection.
10/08/10   ak      Reset propagation from MSM to MDM.
09/23/10   rk      1XCSFB MO/MT call support
09/21/10   sv      Added CM_MMGSDI_SESSION_OPEN_RPT to indicate that sessions
                   have been opened successfully.
07/30/10   rn      1xCSFB Support
06/18/10   xs      Add SENTINEL metacomments for maximum value sentinel in the enum
06/17/10   rk      Added new report type required for WRLF redesign
06/10/10   aj      Support for IRAT measurement
04/27/10   rn      Added CM_LTE_DO_IRAT_F
03/31/10   cl      Add support for CM_PH_EVENT_PRL_INIT to indicate PRL is loaded
02/09/10   sg      Added CM_HANDOVER_COMPLETE_IND
01/05/10   pm      Removing UOOS
01/05/10   ay      Added CM_SUITABLE_SEARCH_END_IND
10/15/09   ks      Adding support for FER and PER reporting in 1X and HDR
09/25/09   ks      Adding changes for Nw Initiated QOS
08/07/09   am      Adding mmgsdi_card_rpt
05/27/09   aj      Adding support for passing otasp status to clients
04/03/09   aj      updated comments on emergency callback report
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
09/18/08   fj/aj   Add support for UMA-GAN feature
09/16/08   pk      Added support for WLAN UOoS
09/04/08   dm/sv   Added WPS result report type.
07/24/08   rn      Send regn reject ind to clients
07/18/08   ks      Added new info for Current Operator's Cell ID and
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
05/06/08   ak      BCMCS 2.0 changes
03/27/08   rn/ic   Added support for new command CM_PH_STATUS_CHGD_REQ
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
12/13/07   sk/ak   Added DCM.
12/07/07   ra      Added support for IMS VCC in HOMER files
10/03/07   rn      Added CM_PS_SIG_CON_REL_CNF in cm_name_type
08/17/07   ka/dm   New field to report HS call in progress
06/14/07   pk/cl   Added support for power suspend
06/06/07   pk/cl   Added support for LN status notification from HDR
04/15/07   rp      Removing FEATURE_MMGPS
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      Changes to support Voip deregistration.
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
11/17/06   jqi     Added CDMA connect Oder Ack support.
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
05/17/06   pk      Added support for DDTM during BCMCS flows
04/17/06   ka      Adding support for HSDPA and DTM indications.
03/10/06   pk      Added support for high PSIST
03/09/06   pk      Added support for Generic reports
03/02/06   pk      Added VoIP - 1X handoff reports
02/17/06   ka      Adding support for videoshare
01/06/06   ka      Adding Release 5 changes including emerg call list,
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
12/02/05   ka      Adding new report primitive CM_MM_PS_DATA_SUSPEND_IND
11/30/05   ka      CM_HPLMN_SEARCH_CNF is sent from REG in response to
                   CM_HPLMN_SRCH_REQ
11/18/05   sk      Added CM API cm_sd_rpt_free_buf() to free CM SD reports.
09/29/05   pk      Added cm_privacy_setting_type to cm_mc_rpt_type union
09/26/05   ka      Adding CM_IP_CALL_IND_MT_RING_CNF.
09/09/05   ic      Restored CM_INTERCEPT_F enum value accounting for removal
                   of CM_REORDER_F
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/22/05   ic      Obsoleted CM_REORDER_F.
                   Use CM_CALL_ORIG_FAIL_F instead with orig_fail.code set to
                   CM_CALL_ORIG_ERR_REORDER and call_ptr->end_status set to
                   CM_CALL_END_REORDER
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
04/05/05   ka      Adding CM_CIPHER_IND to cm_name_e
03/14/05   ka      Adding report types for IP apps
02/01/05   dk      Added WLAN Support.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/03/04   ws      Remote WMS support.
11/11/04   sj      Added new call end cnf report for HDR.
11/01/04   ka      New primitive for user to user data signaling
09/22/04   ka      Adding counting comments to cm_name_e
09/08/04   jqi     More ps dormant optimization.
07/28/04   ka      Added support for secondary PDP context
07/21/04   sj      Added support for umts to 1x handover.
06/30/04   dk      Added CM_PS_SIG_CON_REL_IND report.
06/25/04   dk      Changed return type of cm_call_id_deallocate() from boolean
                   to void.
06/03/04   ws      Add support for redirecting number.
05/21/04   ka      Added CM_CAMPED_IND sent by REG when phone gets camped.
04/02/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.23 and MSMSHARED_CM.01.00.10.00.28
03/15/04   ka      Merged changes from 6250.
                   Use service domain form service indicator for call orig.
                   Propagated service domain for call origination.
                   Fixed forcing service domain on the fly for GW system.
03/12/04   jqi     Use service domain form service indicator for call orig.
03/10/04   jqi     Propagated service domain for call origination.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/06/04   ws      Initial jaguar/mainline merge.
11/19/03   ic      Marked CM_HDR_RESELECT_EXIT_HDR_F as obsolete
10/14/03   sj      Added CM_HDR_RESELECT_EXIT_HDR_F report.
10/10/03   ic      Modified cm_call_id_allocate() to die if CM ran out of
                   call ids.
05/01/03   jar     Added CM_GSDI_REFRESH_FCN_IND
04/28/03   AT      Lint cleanup.
04/14/03   ws      Added BSID changes
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
                   Added card status and deep sleep reports.
10/31/02   vt      Removed handling of NDSS origination failure - really!.
                   Mainlined feature SD20.
10/28/02   vt      Removed handling of NDSS origination failure.
10/23/02   vt      Mainlined FEATURE_CM_USE_UMTS_REG_INTERFACE.
10/23/02   vt      Rearranged the order of LL reports in enum
10/05/02   vt      Made some functions external for SD20. Added new SD20
                   reports.
07/17/02   vt      Updated it for multimode.
07/08/02   vt      Added call id allocate & deallocate functions.
                   Removed inclusion of cmwll.h. cmwll.h should include
                   this file.
05/04/02   PRK     Included cmwll.h.
04/25/02   RI      Added process_uss_data_conf field to cm_rpt_type.
04/24/02   RI      Added CM_RAB_REESTAB_FAIL and renamed CM_RAB_ESTABLISH_IND to
                   CM_RAB_REESTABLISH_IND in cm_name_type.
                   Added  rabm_rab_reestablish_fail_T message and
                   renamed rabm_rab_establish_ind_T to
                   rabm_rab_reestablish_ind_T in cm_rpt_type.
02/06/02   PRK     Added cm_tc_get_buf_else_err_fatal and cm_tc_send_cmd
                   functions.
02/04/02   RI      Modifications based on mn_cm_exp.h for Supplementary Services.
                     - removed mn_cm_exp_t.h
11/21/01   RI      Added rabm_rab_release_ind_T in cm_rpt_type
                   for PS Data.
10/15/01   RI      Added Supplementary Services declarations.
08/23/01   HD      NAS header file reorg changes + RSSI reporting for GSM/WCDMA.
04/27/01   HD      Initial release.

===========================================================================*/
/*
** Reports to be sent to the Call Manager from lower layers.
*/
typedef enum cm_name_e
{
  CM_RPT_NONE=-1,
    /* FOR INTERNAL USE OF CM ONLY! */

  /*
  ** Reports from GSM/WCDMA protocol stack
  */

  CM_RPT_GW_BEGIN = 0,

  /* MN events */
  CM_MO_CALL_CONF,
  CM_MT_CALL_IND,
  CM_CALL_PROGRESS_INFO_IND,
  CM_CALL_CONNECTED_IND,

  /* 5 */
  CM_CONNECTED_NUMBER_IND,
  CM_MT_END_CALL_IND,
  CM_MT_USER_DATA_IND,
  CM_MT_SETUP_IND,

  /* 9 */
  CM_MT_MODIFY_IND,
  CM_MT_DISC_IND,
  CM_START_DTMF_CONF,
  CM_STOP_DTMF_CONF,

  /* 13 */
  CM_MANAGE_CALLS_CONF,
  CM_MODIFY_COMPLETE_CONF,
  CM_ABORT_CC_CONF,
  CM_ACTIVATE_SS_CONF,

  /* 17 */
  CM_DEACTIVATE_SS_CONF,
  CM_ERASE_SS_CONF,
  CM_INTERROGATE_SS_CONF,
  CM_REGISTER_PASSWORD_CONF,

  /* 21 */
  CM_REGISTER_SS_CONF,
  CM_PROCESS_UNSTRUCTURED_SS_CONF,
  CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF,
  CM_MT_CALL_SS_IND,

  /* 25 */
  CM_FORWARD_CHECK_SS_INDICATION_IND,
  CM_UNSTRUCTURED_SS_NOTIFY_IND,
  CM_UNSTRUCTURED_SS_IND,
  CM_RELEASE_UNSTRUCTURED_SS_IND,

  /* 29 */
  CM_CONNECTION_STATE_IND,
  CM_GET_PASSWORD_IND,
  CM_LCS_LOCATION_NOTIFICATION_IND,
  CM_LCS_MOLR_CONF,


  /* 33 */
  /* REG events */
  /* CM_LOAD_DATA_CNF,   Removed, obsolete */
  CM_MM_INFORMATION_IND = 34,
  CM_NETWORK_LIST_CNF,
  /* CM_SAVE_DATA_CNF,   Removed, obsolete */

  /* 37 */
  CM_SERVICE_CNF = 37,
  CM_SERVICE_IND,
  CM_SIM_AVAILABLE_CNF,
  CM_SIM_NOT_AVAILABLE_CNF,
  CM_DS_STAT_CHGD_CNF,

  /* 42 */
  CM_STOP_MODE_CNF,
  CM_DEEP_SLEEP_IND,
  CM_RR_STATUS_IND,
  CM_RRC_STATUS_IND,

  /* 46 */
  CM_PDP_ACTIVATE_CNF,
  CM_PDP_ACTIVATE_REJ,
  CM_PDP_ACTIVATE_IND,
  CM_PDP_DEACTIVATE_CNF,

  /* 50 */
  CM_PDP_DEACTIVATE_IND,
  CM_PDP_DEACTIVATE_REJ,
  CM_PDP_MODIFY_IND,
  CM_PDP_MODIFY_CNF,

  /* 54 */
  CM_PDP_MODIFY_REJ,
  CM_PDP_ACTIVATE_SEC_CNF,
  CM_PDP_ACTIVATE_SEC_REJ,
  CM_PDP_ACTIVATE_REJ_RSP,

  /* 58 */
  CM_PS_SIG_CON_REL_IND,
  CM_RAB_RELEASE_IND,
  CM_RAB_REESTABLISH_IND,
  CM_RAB_REESTAB_CNF,

  /* 62 */
  CM_RAB_REESTAB_REJ,
  CM_RAB_REESTAB_FAIL,
  CM_PDP_UNKNOWN_REQ,
  CM_MT_DATA_CONNECT_IND,

  /* Test Control events */
  /* 66 */
  CM_CHANGE_TEST_MODE_IND,
  CM_SIM_STATE_CHGD_F,                    /* SIM state changed.            */
  CM_SI_NETWORK_LIST_F,
  CM_GSDI_CARD_STATUS_IND,

  /* 70 */
  CM_GSDI_REFRESH_FCN_IND,                /* File Change Notification      */
  CM_DATA_SUSPEND_CHGD_F,                 /* Data suspend flag changed     */
  CM_CS_SIG_REL_IND,                      /* CS RAB release indication     */
  CM_CAMPED_IND,                          /* Camped but Loc Update pending */

  /* 74 */
  CM_UMTS_CDMA_HANDOVER_REQ,
  CM_PDP_DEACTIVATE_PROMOTE_IND,          /* Primary deact,Sec Promoted to primary */
  CM_PDP_DEACTIVATE_PROMOTE_CNF,          /* Primary deact,Sec Promoted to primary */
  CM_USER_CONGESTION_IND,                 /* MN is full, cannot accept user to user data */

  /* 78 */
  CM_CB_DATA_IND_F,
  CM_CB_CELL_CHANGE_IND_F,
  CM_CB_DUPLICATION_DETECTION_IND_F,

  /* 81 */
  CM_SM_DATA_IND_F,
  CM_SM_REPORT_IND_F,
  CM_SM_LINK_CONTROL_IND_F,

  /* 84 */
  CM_CIPHER_IND,                          /* Begin of ciphering ind, GW only*/
  CM_5_SEND_CONF,                         /* Resp to 5_SEND_REQ, CCBS */
  CM_ERASE_CC_ENTRY_CONF,                  /* Cnf to CM_ERASE_CC_ENTRY_REQ */
  CM_CCBS_INFO_IND,               /* Informs if CCBS is allowed on the call */

  /* 88 */
  CM_RECALL_IND,                    /* After CCBS is activated netowork
                                    ** indication to re-originate the call
                                    */
  CM_HPLMN_SEARCH_CNF,              /* Sent in response to
                                    ** CM_HPLMN_SEARCH_REQ.
                                    */
  CM_MM_PS_DATA_SUSPEND_IND,        /* Sent by MM to indicate that data is
                                    ** suspended. Given by MM only when
                                    ** data suspend cannot be sent using
                                    ** CM_SERVICE_IND or CM_SERVICE_CNF.
                                    */
  CM_EMERGENCY_NUM_LIST_IND,        /* Emergency no list send by network */

  /* 92 */
  CM_CELL_SERVICES_IND,             /* Emergency no list sent by network */
  CM_HSPA_CALL_STATUS_IND,          /* HS call progress indication */
  CM_PS_SIG_CON_REL_CNF,            /* PS Signaling release ack */
  CM_PH_STATUS_CHGD_CNF,            /* Phone status changed confirmation
                                    ** sent by NAS. Sent in response to
                                    ** CM_PH_STATUS_CHGD_REQ
                                    */

  /* 96 */
  CM_CONN_MODE_SERVICE_STATUS_IND,  /* Report service status Information
                                    ** when UE is OOS in Connected Mode
                                    */
  CM_RR_CELL_INFO_IND,              /* Reoprt Cell_info Ind from RR */
  CM_RRC_CELL_INFO_IND,             /* Reoprt Cell_info Ind from RRC */


  /* MBMS */
  /* 99 */
  CM_RPT_MBMS_ACT_CNF,       /* ACT_CNF given in resp to MBMS Act Req */
  CM_RPT_MBMS_DEACT_CNF,     /* DEACT_CNF given in resp to MBMS Deact Req */
  CM_RPT_MBMS_ACT_IND,       /* ACT_IND sent asyncly to report MBMS status */
  CM_RPT_MBMS_GENERIC,       /* Generic MBMS report sent by lower layers */

  /* 103 */
  CM_REG_REJECT_IND,        /* Registration Reject Indication */

  /* GAN */
  /* 104 */
  CM_GAN_INFO_IND,           /* Report gan_info Ind from GAN library */

  /* 105 */
  CM_MMGSDI_CARD_STATUS_IND,  /* card status ind from MMGSDI */
  CM_MMGSDI_REFRESH_FCN_IND,  /* refresh event from MGSDI */

  /* 107 */
  CM_PDP_ACTIVATE_SEC_IND,    /* MT Sec PDP Activation Indication */
  CM_PDP_NW_MODIFY_REQ,       /* MT Sec PDP Modify Request */
  CM_SUITABLE_SEARCH_END_IND, /* End of LTE/WCDMA Connected Mode */
  CM_HANDOVER_COMPLETE_IND,   /* Handover complete indication for eCall application */

  /* 111 */
  CM_DISABLE_BPLMN_IND,       /* Disable BPLMN during WCDMA RLF*/
  CM_BEARER_CTXT_TRANSFER_IND, /* Call object transfer indication
                               ** during LTE to GW redirection/reselection.
                               */
  CM_CELL_ACCESS_IND,         /* Domain Specific Access Control (DSAC)
                                 indication */

  /* 114 */
  CM_PS_DETACH_CNF,          /* PS detach confirmation from ll */
  CM_PDP_GRACEFUL_ABORT_CNF, /* Graceful PDP abort conf from ll */

  CM_RR_CELL_BROADCAST_INFO_IND,  /* Report Cell Broadcast info indication from RR */
  CM_RRC_CELL_BROADCAST_INFO_IND, /* Report Cell Broadcast info indication from RRC */

  /* 118 */
  CM_RR_CODEC_INFO_IND,              /* Reoprt codecl_info Ind from RR */
  CM_RRC_CODEC_INFO_IND,             /* Reoprt codec_info Ind from RRC */
  CM_NAS_SRVCC_HANDOVER_COMPLETE_IND, /* Report SRVCC handover complete ind from W-NAS*/
  CM_MM_SRVCC_HANDOVER_FAILURE_IND,   /* Report SRVCC handover failure ind from W-NAS*/
  /* 122 */
  CM_RRC_DCH_CELL_INFO_CNF,          /*Report Cell info in DCH */
  CM_FPLMN_LIST_IND,                 /* Report forbidden PLMN list from NAS */
  CM_SET_DRX_CNF,                    /* set DRX confirmation from ll */
  CM_GET_DRX_CNF,                    /* get DRX confirmation from ll */

  /* 126 */
  CM_UNBLOCK_ALL_APN,                /* Inform to unblock all APN */
  CM_TD_RRC_CODEC_INFO_IND,       /* Reoprt codecl_info Ind from TD RRC */
  CM_SIM_STATE_UPDATE_IND,   /* SIM update indication on T3245 expiry from REG*/
  CM_GET_HPLMN_SEARCH_TIMER_CNF,        /* get HPLMN timer cnf */
  CM_PS_ATTACH_FAILED_IND,         /* PS Attach failed indication from NAS*/

  CM_STACK_DEACT_IND,                /* NAS indicate to CM that stack is deactivated */
  CM_STACK_ACT_IND,                  /* NAS indicate to CM that stack is activated */
  CM_SERVICE_STAT_IND,               /* NAS service indicaiton while ps transfer is in progress */

  CM_MT_CALL_PAGE_FAIL_IND,           /* Indication for MT call failure after Page is received and before call setup completes */
  CM_HOME_NODEB_NAME_IND,             /* Indicate HNodeB name to clients */

  CM_RRC_PCH_STATE_IND,           /* WRRC PCH STATE indication */
  CM_GET_SRVCC_CONTEXT_REQ,           /* NAS request SRVCC call context from CM */
  CM_MULTIMODE_SUBS_CHGD_CNF,     /* Report multimode subs chgd*/
  CM_MO_CALL_ALERT_IND,  /* Mobile call alert indicator */
  CM_CSG_SEARCH_TRIGGER_IND,
  CM_CSG_NW_SEL_MODE_UPDATE_IND,
  CM_NAS_T3346_TIMER_IND, /* NAS indication to CM about timer T3346 status */
  CM_PLMN_LIST_CHANGE_IND, /* NAS indication for change in PLMN list type*/
  CM_PSM_READY_RSP,              /* Response to indicate if NAS is ready to enter PSM */
  CM_PSM_READY_IND,              /* Indication to indicate that NAS is ready to enter PSM */ 

  CM_RPT_GW_END,


  /*
  ** Reports from CDMA/HDR protocol stack
  */

  /* 300 */
  CM_RPT_1X_BEGIN = 300,
  CM_ABRV_ALERT_F,               /* Generate a CDMA/AMPS abbreviated alert */
  CM_ABRV_REORDER_F,               /* Generate an AMPS abbreviated reorder */
  CM_ABRV_INTERCEPT_F,           /* Generate an AMPS abbreviated intercept */

  /* 304 */
  CM_ALERT_F,                         /* Analog display/process alert info */
  CM_INCOMING_CALL_F,                                /* CDMA Incoming call */
  CM_STOP_ALERT_F,             /* Stop the incoming alert or pending alert */
  CM_CODEC_INFO_F,

  /* 308 */
  CM_INTERCEPT_F = 308,                      /* Generate an intercept tone */
  CM_FLASH_F,                  /* Analog display/process flash information */
  CM_DTMF_BURST_F,     /* Play a series of DTMFs - FEATURE_HANDLE_FWD_DTMF */
  CM_DTMF_CONT_START_F,               /* Start playing a continuous DTMF - **
                                                   FEATURE_HANDLE_FWD_DTMF */

  /* 312 */
  CM_DTMF_CONT_STOP_F,                 /* Stop playing a continuous DTMF - **
                                                    FEATURE_HANDLE_FWD_DTMF*/
  CM_CALL_RELEASE_F,                          /* Base station call release */
  CM_CALL_FADE_F,                             /* Call has ended abnormally */
  CM_MARKOV_F,                                         /* Markov Call made */

  /* 316 */
  CM_CALL_CONNECTED_F,                        /* Call has been established */
  CM_DISPLAY_F,                                /* Display CDMA information */
  CM_CALLED_PARTY_F,                           /* Called Party information */
  CM_CALLING_PARTY_F,                         /* Calling Party information */

  /* 320 */
  CM_CONNECTED_NUM_F,                      /* Responding party information */
  CM_SIGNAL_F,                                       /* Signal information */
  CM_UPCALL_F,                              /* Immediately into voice call */
  CM_LOCK_F,                                    /* CDMA Lock order command */

  /* 324 */
  CM_UNLOCK_F,                                      /* CDMA unlock command */
  CM_MAINTREQ_F,                      /* CDMA Maintenance Required command */
  CM_N_VC_MSG_F,              /* NAMPS Short Message rcvd on Voice Channel */
  CM_N_VC_CLI_F,              /* NAMPS Call Line Id recvd on Voice Channel */

  /* 328 */
  CM_OTASP_STATUS_F,       /* Notify the OTASP call status - FEATURE_OTASP*/
  CM_OTASP_COMMIT_STATUS_F,   /* Notify if an OTASP commit is in progress -**
                                                             FEATURE_OTASP */
  CM_VOICE_AS_DATA_F,            /* Notify voice_as_data attribute changed */
  CM_LINE_CTRL_F,               /* Line control message - FEATURE_IS95B_WLL*/

  /* 332 */
  CM_SRV_SYS_INFO_F,                      /* Serving system parameter info */
  CM_CALL_ORIG_FAIL_F,                         /* Call origination failure */
  CM_MO_DBM_STATUS_F,                       /* Data burst message status - **
                                             FEATURE_CDSMS  || FEATURE_DBM */
  CM_MT_DBM_DATA_F,                           /* Data burst message data - **
                                              FEATURE_CDSMS || FEATURE_DBM */

  /* 336 */
  CM_MSG_WAITING_F,                  /* Display number of messages waiting */
  CM_N_VOICE_F,                                        /* NAMPS Voice mail */
  CM_N_MSG_F,                                       /* NAMPS Short Message */
  CM_N_CLI_F,                                /* NAMPS Call Line Identifier */

  /* 340 */
  CM_EXT_DISPLAY_F,    /* Extended Display record - FEATURE_IS95B_EXT_DISP */
  CM_NDSS_REDIRECTION_F,  /* NDSS redirection is started-FEATURE_IS95B_NDSS*/
  CM_HDR_CALL_ORIG_FAIL_F,                 /* HDR Call origination failure */
  CM_HDR_INCOMING_CALL_F,                             /* HDR Incoming call */

  /* 344 */
  CM_HDR_CALL_RELEASE_F,                  /* HDR Base station call release */
  CM_HDR_CALL_FADE_F,                     /* HDR Call has ended abnormally */
  CM_HDR_CALL_CONNECTED_F,                /* HDR Call has been established */
  CM_HDR_CONNECTION_DENY_F,                       /* HDR connection denied */

  /* 348 */
  CM_HDR_REDIRECT_F,                                     /* HDR Redirected */
  CM_HDR_PRL_NOT_PREFERRED_F,            /* System not preferred by System **
                          Determination due to QC redirect or ANID change  */
  CM_HDR_DEACTIVATE_ACK_F,                         /* HDRMC Deactivate Ack */
  CM_HDR_RESELECT_EXIT_HDR_F,      /* Obsolete Exit HDR due to reselection */

  /* 352 */
  CM_HDR_BCMCS_FLOW_STATUS_F,            /* Flow status report             */
  CM_HDR_BCMCS_BROADCAST_RESET_F,        /* Broadcast reset report         */
  CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F, /* Broadcast discontinued ACK     */
  CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F,/* Flow request success rprt */

  /* 356 */
  CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F,/* Flow request failure rprt */

  CM_IDLE_PGSLOT_CHANGED_F,           /* CDMA idle paging slot has changed */
  CM_GPS_MODE_GRANTED_F,     /* Entering GPS mode - FEATURE_GPSONE_MSBASED */
  CM_GPS_MODE_EXIT_F,        /* Aborting GPS mode - FEATURE_GPSONE_MSBASED */

  /* 360 */
  CM_EXT_BRST_INTL_F, /* Extended Burst Type - International - FEATURE_T53 */
  CM_NSS_CLIR_F,   /* National Supplementary services - CLIR - FEATURE_T53 */
  CM_NSS_REL_F, /* National Supplementary services - Release - FEATURE_T53 */
  CM_NSS_AUD_CTRL_F,  /* National Supplementary services - Audio Control - **
                                                               FEATURE_T53 */

  /* 364 */
  CM_SND_BRST_DTMF_ACK_F,          /* L2 Ack for send Burst DTMF message - **
                                                               FEATURE_T53 */
  CM_CALL_HOLD_L2ACK_F, /* Sent to CM after L2 Ack receipt for Call Hold - **
                                                               FEATURE_T53 */
  CM_REG_SUCCESS_F,                /* Registration Success - FEATURE_JCDMA */
  CM_REG_FAILURE_F,                /* Registration Failure - FEATURE_JCDMA */

  /* 368 */
  CM_CALL_FAIL_RETRY_ORDER_F,/* Retry order for call origination failure - **
                                                            FEATURE_IS2000 */
  CM_CALL_EMERGENCY_FLASHED_F,              /* Emergency request flashed - **
                                                            FEATURE_IS2000 */
  CM_EXIT_TC_F,                      /* Traffic channel has been torn down */
  CM_BS_STATUS_RSP_F,     /* Indicate Base Station Status Response Received */

  /* 372 */
  CM_REDIRECTING_NUMBER_F,                /* Redirecting Number information */
  CM_CDMA_CAPABILITY_UPDATE_F, /* Report from MC to update CDMA capability **
                                                               information */
  CM_UMTS_CDMA_HANDOVER_STATUS_F, /* Report from MC to inform the status of  **
                                                         UMTS->1x handover */
  CM_HDR_CALL_END_CNF_F, /* Report from HDRMC to confirm the call end req  */

  /* 376 */
  CM_PRIVACY_SETTING_F, /* Report from MC to inform of the privacy setting */
  CM_TUNNEL_MSG_F,                    /* Report to send a tunneled message */
  CM_HDR_TUNNEL_MSG_F,         /* Report to initiate HDR VoIP - 1X handoff */
  CM_PSIST_F,                  /* Report from MC informing the psist value */

  /* 380 */
  CM_HDR_BCMCS_FLOW_MONITORING_START_F,   /* HDR is monitoring BCMCS flows */
  CM_HDR_BCMCS_FLOW_MONITORING_END_F,   /* HDR is not motoring BCMCS flows */
  CM_CDMA_CONNECT_ORDER_ACK_F,               /* CDMA connect order ACK msg */
  CM_END_VOIP_CALL_F,           /* End the VoIP call, 1X Needs the vocoder */

  /* 384 */
  CM_HDR_LOCATION_NOTIFICATION_STATUS_F, /**< HDR location notification status */
  CM_GET_CSNA_MSG_CONFIG_F,       /* Get the list of allowed CSNA messages */
  CM_UPDATE_CSNA_MSG_CONFIG_F,         /* Update the allowed CSNA messages */
  CM_TUNNEL_MSG_STATUS_F,         /* Confirmation for the tunneled message */

  /* 388 */
  CM_HDR_SESSION_OPEN_FAILED,          /**< HDR session open failed. */
  CM_HDR_SESSION_CLOSED,               /**< HDR session was closed. */
  CM_HDR_BCMCS_REG_STATUS_F,      /**< Report for BCMCS_CMD_REG_REQUEST */

  /* 391 */
  CM_MMGSDI_REFRESH_FCN_IND_F,    /* refresh event from MGSDI */

  /* 392 */
  CM_HDR_PER_INFO_IND_F,          /* PER Ind from HDR CP */
  CM_FER_INFO_IND_F,              /* FER Ind from 1X CP */
  CM_HDR_MEAS_MODE_F,             /* Measurement mode report from HDRCP */
  CM_HDR_MEAS_RESP_F,             /* Measurement response from HDR CP */

  /* 396 */
  CM_1XCSFB_STATUS_F,             /* 1XCSFB status report from 1xCP */
  CM_1XCSFB_DEACT_CNF_F,          /* 1XCSFB Deactivated Cnf from 1xCP */
  CM_1XCSFB_ORIG_RES_F,           /* 1XCSFB MO call response */
  CM_1XCSFB_CALL_END_CNF_F,       /* 1XCSFB call end confirm */

  /* 400 */
  CM_1XCSFB_INCOMING_CALL_F,      /* MT call indication in 1XCSFB mode */
  CM_INCOMING_PAGE_F,             /* MT 1x incoming page */
  CM_INCOMING_PAGE_FAIL_F,        /* MT 1x incoming page failure ( sent only
                                  ** if CM_INCOMING_PAGE_F was sent */
  CM_CALL_END_CNF_F,              /* 1X-CP confirms call is ended */
  CM_MT_CALL_END_IN_ACCESS_STATE_F, /* 1X-CP report for page miss in access state */
  CM_MT_SMS_CALL_RSP,             /* Response for SO76 CSFB call */

  /* 406 */  
  CM_DEREGISTER_RSP_F,            /* Response for explicit deregister req */

  CM_1XSRVCC_HO_EUTRA_IND_F,      /* 1XSRVCC HO START Ind from 1xCP,sent during ORM request */

  CM_1XSRVCC_HO_COMPLETE_IND_F,      /* 1XSRVCC HO_COMPLETE Ind from 1xCP */

  CM_1XSRVCC_HO_FAIL_IND_F,      /* 1XSRVCC HO_FAILURE Ind from 1xCP */

  /* 410 */ 
  CM_1XSRVCC_DEACT_CNF_F,         /* 1XSRVCC Deactivated Cnf from 1xCP */
  CM_MC_PSM_READY_RSP_F,          /* Response to indicate if MC is ready to enter PSM */
  CM_MC_PSM_READY_IND_F,          /* Indication to indicate that MC is ready to enter PSM */
  CM_HDR_PSM_READY_RSP_F,        /* Response to indicate if HDR is ready to enter PSM */
  CM_HDR_PSM_READY_IND_F,        /* Indication to indicate that HDR is ready to enter PSM */
  CM_1XDRVCC_HO_COMPLETE_IND_F, /* 1X DRVCC HO Complete ind from 1XCP */
  CM_1XDRVCC_ORM_SUCCESS_IND_F,     /* 1x drvcc, ORM was sent during call attempt by 1XCP */
  CM_1XDRVCC_CSFB_ORM_SUCCESS_IND_F, /* 1x drvcc, ORM was sent during call attempt by 1XCP 
via tunneled LTE*/ 


  CM_RPT_1X_END,

  /*
  ** Reports from SD20
  */

  /* 500 */
  CM_RPT_SD_BEGIN = 500,
  CM_SRV_IND_INFO_F,                     /* Service indicators information */
  CM_ACQ_FAIL_F,                    /* Acquisition failed over all systems */
  CM_OK_TO_ORIG_F,                       /* Ok to send origination command */

  /* 504 */
  CM_NAM_CHANGED_F,                    /* NAM changed via Auto NAM command */
  CM_E911_CALLBACK_F,                     /* enter/exit E911 callback mode */
  CM_STANDBY_SLEEP_F,                     /* Entering powerdown sleep mode */
  CM_STANDBY_WAKE_F,                       /* Exiting powerdown sleep mode */

  /* 508 */
  CM_LPM_OK_F,                          /* Command to enter low power mode */
  CM_OFFLINE_OK_F,               /* Indicates OFFLINE completion from MMOC */
  CM_IP_APP_REG_STATUS,               /* IP app's registration status      */

  /* 511 */
  CM_SRV_LOST_F,                   /* Service is lost, used to sync stacks */
  CM_PRL_INIT_F,              /* Indicates if the PRL is completely loaded */

  /* 513 */
  CM_LTE_DO_IRAT_F,                 /* Indicates LTE-DO IRAT is successful */
  CM_RESET_OK_F,         /* Indicates OK to continue with RESET processing */
  CM_LTE_1X_IRAT_F,                 /* Indicates LTE-1X IRAT is successful */
  CM_MM_SCAN_BLOCKED,          /* MMoC-->CM to request the scan permission */

  /* 517 */
  CM_SS_STACK_STATUS_F,              /* Indicates the SD's stacks status     */
  CM_SUSPEND_SS_F,                   /* Indicates the MMOC suspend status  */
  CM_OPT_SR_HDR_ACQ_FAIL_F,           /* */
  CM_PWROFF_OK_F,              /* Indicates OK to continue with POWER OFF processing */
  CM_IRAT_HOLD_USER_ACT,              /* Indication to hold user activity during LTE- > 1XIRAT */
  CM_IRAT_UNHOLD_USER_ACT,            /* indication to un hold user activity during LTE- > 1XIRAT */
  CM_IRAT_COMPLETE,                   /* This is indication for to tell CMSS that IRAT across the stack
                                         has been completed. */
  CM_FULL_SRV_LOST_F,  /* Full Service is lost, used to switch to global mode */
  CM_HYBR_BSR_TO_HDR_F,         /* Indicats BSR from LTE to HDR START/END */                        
  CM_MMOC_SUBSCRIPTION_FAILURE,/*subsc-chngd failed due to SIM BUSY , 
                                                        ** CM need to recover  when SIM is ready */                                
  CM_BSR_STAT_CHGD_F,  /* BSR progress status changed */
  CM_KICK_HYBR2_F,    /*Indicate get net done on main in acq stack kick hybr2 */                                                                    
  CM_STANDBY_PREF_CHG_OK_F, /* Indicate MMOC is done processing standby pref chgd */   
  CM_CS_EMERG_SCAN_FAIL_F,   /* Indicate CS EMERG scan request failed */
  CM_RPT_SD_END,

  /*
  ** Reports from Voip Applications
  */

  /* 600 */
  CM_RPT_IP_BEGIN = 600,
  CM_IP_CALL_IND_MT_INVITE,        /* indicating invite message from network */
  CM_IP_CALL_IND_MT_RING_CNF,      /* Remote party has been informed about
                                   ** start of local ringing */
  CM_IP_CALL_IND_MO_PROGR_INFO,    /* progress of mobile originated call */

  /* 604 */
  CM_IP_CALL_IND_CONNECTED,        /* Message indicating call connection */
  CM_IP_CALL_IND_CALL_END,         /* Report for call end */
  CM_IP_CALL_IND_CALL_HOLD,        /* Call being put on hold indication */
  CM_IP_CALL_IND_CALL_RETRIEVE,    /* Call being retrieved indication   */

  /* 608 */
  CM_IP_CALL_IND_VS_STATUS,        /* Videoshare availability           */
  CM_IP_CALL_IND_SUPS_CONF,        /* Confirmation given by ipapp for call rel sups */
  CM_IP_CALL_IND_TRANSFER,         /* Indicates call being transferred to another party*/
  CM_IP_CALL_IND_CONNECTED_NUM,    /* Updates remote number call is connected to */

  /* 612 */
  CM_IP_APP_ALLAPPS_OPRT_CNF,         /* All IP app's deregistered
                                      ** Common to GW, CDMA,   so
                                      ** grouped with sd
                                      */

  CM_IP_CALL_IND_CALL_WAIT,        /* second incoming call is received
                                   ** in busy state
                                   */
  CM_IP_APP_UPDATE_DDTM,           /* Change DDTM settings after IP Apps
                                   ** are disabled */


  CM_IP_CALL_IND_MODIFY_COMPLETE,       /*call type being modified cnf */

  /* 616 */
  CM_IP_CALL_IND_MODIFY_IND,       /* call modify indication */

  CM_IP_APP_AUDIO_CODEC_INFO,      /* To pass the audio codec info to Apps for Audio
                                   ** management and UI indication.
                                   ** payload:cmipapp_audio_codec_s_type
                                   */

  CM_IP_CALL_START_DTMF_CONF,     /* start DTMF confirm */

  CM_IP_CALL_STOP_DTMF_CONF,      /* stop DTMF confirm */

  /* 620 */
  CM_IP_CALL_JOIN_CONF_CALL,     /* Participant acknowledgement to join conf call */
  CM_IP_CALL_CONF_INFO,
                                 /* Conference server sends the information about the
                                  conference call at regular intervals of time */
  /* 622 */
  CM_IP_SUPS_ACTIVATE_SS_CONF,    /* Acivate SS confrmation for IP supplementary service */
  CM_IP_SUPS_DEACTIVATE_SS_CONF, /* Deacivate SS confrmation for IP supplementary service */
  CM_IP_SUPS_ERASE_SS_CONF,       /* Erase SS confrmation for IP supplementary service */
  CM_IP_SUPS_INTERROGATE_SS_CONF,     /* Interrogate SS confrmation for IP supplementary service */

  /* 626 */
  CM_IP_SUPS_REGISTER_SS_CONF,      /* Register SS confrmation for IP supplementary service */
  CM_IP_SUPS_UNSTRUCTURED_SS_IND,
  CM_IP_SUPS_RELEASE_UNSTRUCTURED_SS_IND,
  CM_IP_SUPS_PROCESS_UNSTRUCTURED_SS_CONF,

  /* 630 */
  CM_IP_IND_CALL_RAT_CHANGE,        /* IMS indication to CM for call RAT change*/
  
  CM_IP_APP_ALLAPSS_SUBSC_CNF,      /* IMS confirmation after done deregistration upon subscription not available */
  CM_IP_CALL_IND_CALL_DEFLECTION,	/* IMS indication to CM for call deflection status */
  CM_IP_CALL_IND_ANSWER_REJECT,     /* IMS indication for ANSWER command reject without processing */
   
 /* 634 */
  CM_IP_APP_ALLAPPS_SRV_DOM_CNF,         /* All IP app's cnf after srv domain change */
  CM_IP_CALL_IND_CALL_UPDATE_INFO,   /*This indication is sent by IMS for indicating caller name in MT Call*/


  /* 635 */
  CM_IP_APP_AUDIO_SESSION_REL_CNF,       /* IP confirmation that audio session has been released during SRVCC */
  
  CM_IP_CALL_IND_CALL_LOCAL_HOLD,        /* Call being put on local hold indication */
  CM_IP_CALL_IND_CALL_LOCAL_RETRIEVE,      /* Call being local retrieved indication */

  CM_IP_APP_CALL_SWITCH_CODEC_CNF,  /* IP APP confirming about the switch codec confirmation for given call */ 
  CM_IP_CALL_IND_FORWARD_INFO,     /* This indication from IMS contains History Info coming in Response 181 */
  CM_IP_CALL_IND_BARRING,            /*This indication is sent by IMS when MO Call is informed by NW that it's barred*/
  CM_IP_CALL_START_DTMF_IND,  /* DTMF IMD sent by IMS for starting MT DTMF */
  CM_IP_CALL_STOP_DTMF_IND,   /* DTMF IMD sent by IMS for stopping MT DTMF */
  CM_IP_SUPS_UNSTRUCTURED_SS_NOTIFY_IND, /*Downlink USSD NOTIFY indication from IMS */

  CM_RPT_IP_END,

  /* 650 */
  CM_RPT_IP_SMS_BEGIN = 650,
  CM_IP_SMS_IND_MO_SMS_STATUS,     /* MO SMS status */
  CM_IP_SMS_IND_MT_SMS,            /* Incoming SMS */

  CM_RPT_IP_SMS_END,

  /*
  ** Generic reports
  */

  /* 800 */

  CM_RPT_GENERIC_BEGIN = 800,

  /* DEM reports
  */
  CM_DEM_RPT,                   /* Report from Dynamic Environment Manager */

  CM_MMGSDI_CARD_RPT,   /* For Card and PIN related events from MMGSDI*/

  CM_MMGSDI_SESSION_OPEN_RPT,

  /* 804 */

  CM_DATA_PACKET_STATE_RPT,      /* DS traffic tear down status report */

  CM_DROP_CHARGED_CALLS,         /* Indication to drop all charged calls when
                                 ** MMGSDI/GSDI read/write operations fail */

  CM_CALL_CTRL_RPT,             /*will be sent during call ctrl*/

  CM_SOA_MSG_MCC,                   /* Sent by SD --> CM indicates current MCC */

  /* 808 */

  CM_SOA_REGPRX_SRV_REQ_PROCEED,        /* Sent by CMREGPRX --> CMSOA asking for permission on SRV_REQ */

  CM_MODEM_RESET_TIMER_RPT,     /*modem reset*/

  CM_MCFG_REFRESH_RPT,     /* Indication from MCFG that NV/EFS needs to be refreshed*/

  CM_AUTO_REG_TIMER_RPT,
  CM_QSH_CFG_RPT,     /* QSH report to cfg metrics & evts */

  CM_ATTACH_COMPLETE_RPT, /* ATTACH COMPLETE indication from SD */

  CM_RPT_GENERIC_END,

  /*
  ** End of definition of CM reports from lower layers
  */

  CM_NUM_CM_COMMANDS                                 /* Maximum CM reports */

} cm_name_type;
/*~ SENTINEL cm_name_type.CM_NUM_CM_COMMANDS */

#endif /* CMLL_H */
