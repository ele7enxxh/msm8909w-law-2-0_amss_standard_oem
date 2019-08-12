#ifndef RRCMMIF_H
#define RRCMMIF_H
/*===========================================================================

                    R R C    M M    I F

DESCRIPTION
  This file contains global declarations and external references
  for the RRC NAS Interface.


  
  Copyright (c) 2000-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcmmif.h_v   1.31   07 Jun 2002 17:51:04   xgao  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rrcmmif.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/16   sn      Changes for FR30881
12/10/14   sp      Made changes to revert reordering of structures
12/03/14   sp      Made changes to save one byte status in cmd_ptr to crash debug global
07/23/14   bc      Made changes to camp on EHPLMN when NAS requested PLMN is not available.
07/11/14   vi      Made changes to convert earfcn type from uint16 to unti32
06/24/14   bc      Added code to expose rrcrcr_is_proc_active_sub API to NAS.
06/19/14   bc      Added new service request cause RRC_MO_CS_ECALL and its handling.
06/19/14   gv      Added a new field "earfcn" to the structure rrc_avail_plmn_type for TDS requirement
06/04/14   vi      Made changes to remove message header from some commands since NSA doesnt use it
03/26/14   sr      Optimization to release lock when conn reject is received
03/14/14   bc      Changes to display partial plmn results during Manual PLNN search 
                   including LTE interface too.
09/02/13   sr      Made changes for BST optimizations
03/18/14   sr      Changes to add interface changes for TDS requirement
01/30/14   sn      WRRC changes for FR 15265
01/07/14   rmn     Made changes to provide current camped band info in 
                   CM_RRC_DCH_CELL_INFO_CNF & CM_RRC_CELL_INFO_IND
12/12/13   vi     Added code for FR: Service Search with scan scope ACQ DB only.
12/06/13   vi      Added new command handling RRC_MUTLIMODE_SUBS_CHGD_IND from NAS to set the correct AS ID
11/14/13   sp      Made changes for QMI Extension for Detailed Protocol Stack Information 
03/22/13   ad      Added fields for tx_agc,rx_agc and dl_bler in modem statistics interface
09/25/13   bc      Added code to support exchange of AS ID when LTE feature is enabled
08/13/13   gv      Added support for FEATURE_TRIPLE_SIM
08/13/13   gv      Made changes to perform acq db only scan when requested so by higher layers
08/05/13   sp      Made changes to notify PCH state info to NAS
07/03/13   sa      Fixed the compilation error
05/30/13   gv      Made changes to perform acq db only scan when requested so by higher layers
05/23/13   geg     WSW decoupling (except for component of QCHAT)
05/22/13   vi      Added new structure UE_MODE_IND_TYPE and modified enum rrc_stop_cause_e_type for SGLTE
02/19/13   gv      Made changes to indicate NAS with new abort cause based on the domain when Access Class is barred
12/28/12   esr     Made changes to provide a new rej cause for an open sess when conn rel is in prog
12/07/12   sn      CSG feature changes
11/08/12   sn      Changes to include LTE cells in modem statistics
10/24/12   gv      Changed the value of RRC_MAX_LAI from 20 to 30
09/27/12   sr      Added code changes to introduce an api used by NAS to reset MCM activation cause 
                   if registration won't be triggerred because of IRAT reselection when ISR is active
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/11/12   rv      Added prototype to let CM inform W-RRC about deep sleep during idle mode OOS 
                   when LTE is the last RAT on which service search has happened
05/24/12   sks     Added support for QCHAT specific dynamic DRX.
05/01/12   sks     Added changes to indicate NAS that NO SERVICE is because of no
                   resource from TRM
04/12/12   as      Added code to support feature NB/WB handover support
02/20/12   sks     Added support for SRVCC failure indication.
01/12/12   gv      Added code to support FEATURE_PPAC
12/25/11   xl      Roll back to #12
12/21/11   xl      Merge from MPSS.NI.1.1
11/29/11   gv      Added new cause RL Failure to close session cause
11/18/11   mn      Made changes to disable reselection to LTE after attach reject from network
                   with cause 14.
07/29/11   ad      Made changes for CMI Phase 4
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/10/11   rl      Added changes for supporting NAS API's after removal of 
                   GSDI /GSTK APIs.
04/26/11   sks     Added LAI abort type
03/29/11   rl      Defined the RRC_MAX_PLMNS to macro SYS_PLMN_LIST_MAX_LENGTH
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11   ad      Added changes to refeaturize the API files
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/21/11   ad      Made changes to fix compilation errors on 76xx target for SW decoupling
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/01/10   rm      Moved prototypes of rrc_register_serving_cell_change_ind_cb and
                   rrc_deregister_serving_cell_change_ind_cb to rrcmmif.h from rrccsp.h
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/07/10   rm      Added interface strucures for 3GPP CSFB
09/27/10   ss      Added code to support as_id changes for Fast Dormancy
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
08/24/10   ss      Added code to incorporate L2W Idle mode MAPPED security keys
08/17/10   sks     Added an API to inform upper layers if PS abort is needed or not
                   in a dual SIM scenario.
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/15/10   rm      Reordered the fields of rrc_mm_service_req_cause_e_type
06/14/10   rm      Made changes to stop full frequency scan skip timer, on service
                   request for emergency call
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
03/09/10   rm      Added interface support for FEATURE_RAT_PRIORITY_LIST
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/14/09   rm      Added support to report GSM monitored cells for FEATURE_MODEM_STATISTICS
11/02/09   rm      Added support for FEATURE_MODEM_STATISTICS
10/01/09   gkg     Added new field in RRC_SYNC_IND so that CS RAB Type can be provided
                   to NAS while a RAB is established/reconfigured.
08/17/09   ss      Added code to provide ciphering ON/OFF indication to UI 
                   under feature flag FEATURE_UMTS_VOICE_CIPHERING_IND.
08/03/09   kp      Changes to obsolete gs_alloc/gs_free under feature flag FEATURE MODEM HEAP
07/13/09   rm      Added prototype for mm_rrc_registration_in_progress
05/20/09   rm      Added boolean grat_scan_done to rrc_service_req_type, so that NAS can indicate
                   RRC to skip GSM search
05/15/09   rm      Added freq info in cell_info tpe
05/08/09   ss      Updated Copyright Information
11/27/08   ns      Took the definition of call_type out of the feature flag
                   FEATURE_WCDMA_REL7 
10/29/08   ns      Fixed compilation errors
23/10/08   gkg     Added RRC_ABORT_CAUSE_TYPE_MAX enum in rrc_abort_cause_e_type. This
                   is added to support event EVENT_WCDMA_CONN_REL_CAUSE.
09/12/08   ns      Added code for REl7 under FEATURE_WCDMA_REL7
08/25/08   sks     Added a new field in rrc_conn_mode_service_status_ind_type for indicating
                   connected mode OOS.
04/21/08   ns      Fixed klockwork errors
03/13/08   vm/pk   Added support for feature FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE.
03/11/08   pk      Added support to report current cell id to NAS through RRC_SERVICE_CNF,
                   RRC_SERVICE_IND and RRC_SERVICE_IND_FROM_OTA.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/21/07   da      Support for FEATURE_MBMS
12/05/07   pk      Added a new field for serving PSC in cell_info_type which is needed by ATCOP.
11/23/07   pk      Added rai field in RRC_EST_REQ which is the NAS known currently
                   camped RAI under the feature flag FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH.
11/19/07   kp      Added support for UE initiated dormancy under feature
                   flag FEATURE_UE_INITIATED_DORMANCY.
10/26/07   vm      Moved ue_oos var outside of FEATURE_UMTS_UNIFORM_OOS_HANDLING as
                   part of fix for CR 129966.
10/23/07   pk      Moved the function mm_rrc_plmn_lai_allowed() prototype out of the
                   feature flag FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT.  
10/23/07   kp      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE_NETWORK_SHARING.
07/27/07   da      Put code changes for HSPA call status ind under feature 
                   flag FEATURE_HSPA_CALL_STATUS_IND
07/13/07   da      Added support for HSPA call status indication.
06/15/07   vm      Added support for FEATURE_CONNECTED_MODE_UMTS_UNIFORM_OOS_HANDLING_UI_UPDATE
06/05/07   kp      New failure cause RRC_OPEN_SESS_FAILURE_AC_BARD added to enum 
                   rrc_open_session_status_e_type to indicate Access Class is bard 
                   in OPEN_SESSION_CNF
05/28/07   kp      Added an enum rrc_call_type_in_est_req_e_type which indicate various 
                   call types needed for implementation of spec cr – 2765 “Indication of 
                   HSPA capability and conversation call type for CS”.
04/12/07   vm      Added support for feature FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH
03/30/07   kp      Moved MAX_NAS_MESSAGES_FOR_CS_DOMAIN from rrcdt.c as its value is
                   a functionality of NAS.Also increased UDT buffer size for CS domain i.e.
                   MAX_NAS_MESSAGES_FOR_CS_DOMAIN from 8 to 14.Since NAS can send as 
                   may as 14 messages back to back in case of multi party call before 
                   RLC can transmit these messages to N/W. So UDT buffer size should be
                   big enough to store all of them till RLC Acks these messages.
03/16/07   vm      Added fix for CR 109422. Added var ue_oos in RRC_SERVICE_CNF under
                   existing feature definition FEATURE_UMTS_UNIFORM_OOS_HANDLING.
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
11/23/06   segu    Added HARD ABORT cause in RRC_ABORT_REQ from NAS
11/21/06   vm      Added support for the feature FEATURE UMTS BACKGROUND FULL SERVICE SEARCH.
                   Added new field "service_search" of type "sys_service_search_e_type"
                   in rrc_plmn_list_req_type and rrc_plmn_list_cnf_type.
10/13/06   sgk     Added 7600 support under feature
                   FEATURE_UMTS_1X_HANDOVER_UMTSMSM
09/20/06   tkk     Added new release cause to indicate to NAS during OOS.
09/11/06   kp      Added an enum in rrc_cn_domain_identity_e_type to indicate both
                   cn-domain. 
08/28/06   kp      Added support of protocol discriminator & Transaction I.D
                   in nas_msg_typ
08/25/06   vm      Added support for FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT.
08/21/06   kp      Added support to provide new cause for OOS in close_cnf to MM
07/21/06   vm      Added support for the feature UOOSI.
06/13/06   segu    Added support for DSAC feature implementation
05/02/06   segu    Added support for single to dual mode changes
04/11/06   vk      Added new structure rrc_hsdpa_hsupa_cell_ind_type that defines 
                   HSDPA capability status under FEATURE HSDPA
01/23/06   kp      Added support for Transaction I.D in BPLMN Abort feature 
12/14/05   siva    redefine RRC_MAX_PLMNS to 40 instead of 10
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search. 
                   Updated NAS/RRC i/f for plmn_list_req and plmn_list_cnf.
09/30/05   da      Defined struct cell_info_type that contains cell id, plmn id 
                   and lac info.  Also moved cell id notification cb type
                   from rrccsp.h to rrcmmif.h
09/09/05   kc      Removed featurization flag for emergency call indication
09/07/05   da      Added boolean flag "loc_info_invalid" to struct home_zone_ind_type.
                   This flag is set to TRUE whenever there is cell loss or RRC is
                   unable to extract LAC_ID.    
09/02/05   ss      Added establishment cause in opensession req and rrc_data_req
09/02/05   vm      Added new variable rrc_rel_cause_e_type in rrc_abort_cnf_type.
07/25/05   vr      Added a new type rrc_wtog_cco_complete_ind_type to support
                   feature FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
06/30/05   vr      Added a new value RRC_WTOG_CC_ORDER in rrc_stop_cause_e_type
                   to support FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
06/06/05   da      Defined a new struct and callback func type for home zone
05/09/05   vm/ab   Added new command structure rrc_nw_sel_mode_reset_ind_type
                   for RRC_NW_SEL_MODE_RESET_IND command
03/31/05   bu      Added auth_failed_bar_cells to rrc_abort_req_type.
12/03/04   da      Added support for FEATURE_HOME_ZONE_SERVICE
11/11/04   vk      Added delay_sig_con_release flag in abort_req that RCE receives
                   from MM
06/01/04   bu      Added band pref in service_req_type and plmn_list_req_type from MM.
                   Removed any_cell_selection_mode from rrc_activation_rsp_type.
03/22/04   kc      Added gtow PCCO support.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
12/16/03   bu      Added sys_network_selection_mode_e_type to rrc_service_req_type.
12/08/03   bu      Added a boolean gsm_map_common_info_incl in rrc_service_info_type.
11/17/03   kc      Added new cause RRC_DIRECTED_SIGNALLING_REESTABLISHMENT for
                   rrc release procedure.
10/21/03   kc      Added START values for CS/PS domain for DEACT_CNF, ABORT_IND
                   and ABORT_CNF
08/18/03   kc      Added RRC_CONNECTION_NOT_AVAILABLE to ABORT_IND.
04/10/03   bu      Added sys_stop_mode_reason_e_type to rrc_deact_req.
01/30/03   vn      Removed compiler flags from around rrc_avail_plmn_list_type
                   and rrc_avail_plmn_type to fix linting errors.
12/04/02   xfg     Added a new command structure rrc_change_mode_ind_type
                   to support mode change
11/11/02   xfg     Added a status field in rrc_plmn_list_cnf_type
11/08/02   rj      Fixed a typo.
11/08/02   rj      Added data structure rrc_close_session_cnf_type to support 
                   RRC_CLOSE_SESSION_CNF comamnd.
11/04/02   xfg     Added rrc_stop_wcdma_mode_req_type to support deep sleep
10/29/02   xfg     Removed FEATURE INTERRAT HANDOVER WTOG for changes made to
                   rrc_stop_wcdma_mode_cnf_type to avoid a compilation error for
                   single mode build because MM did not featurilize its changes. 
10/23/02   xfg     Added RRC_HANDOVER to rrc_sync_cause_e_type to support audio
                   control in GSM to WCDMA HO
10/11/02   xfg     Added Start Values in rrc_stop_wcdma_mode_cnf_type
08/06/02   xfg     Added command structures for RRC_ACTIVATION_IND and 
                   RRC_ACTIVATION_RSP for inter-RAT handover/cell reselection.
08/06/02   rj      Added command structures for  RRC_SIM_INSERTED_REQ, and 
                   RRC_SIM_UPDATE_REQ and removed comamnd structures for 
                   RRC_PAGING_CTRL_REQ and RRC_CIPHER_REQ for new RRC-MM 
                   interface. Deleted Ciphering Info from the RRC_EST_REQ
                   comamnd structure and deleted IMSI,PTMSI,TMSI and Access
                   Class Info from the RRC_SERVICE_REQ comamnd structure.
06/07/02   xfg     Removed sys_plmn_selection.h from includes and change 
                   mm_as_type.h to mm_as_types.h.
05/28/02   xfg     Changed rrc_service_status_e_type to mm_as_service_state_T,
                   rrc_cell_access_status_e_type to mm_as_cell_access_status_T, 
                   and added status(mm_as_status_T) in RRC_SERVICE_CNF.
04/29/02   xfg     Added new interface changes for Cheetah 1.0. All changes 
                   are marked with FEATURE DUALMODE BASELINE.
05/02/02   rj      Fixed a typo in RRC_INTEGRITY_CIPHERING_ESTBALISHED.
04/24/02   rj      Added addional enums  RRC_PAGE_CAUSE_UNKNOWN and 
                   RRC_EST_TERM_UNKNOWN to rrc_paging_cause_e_type  and
                   rrc_establish_cause_e_type respectively.
04/22/02   rj      Added additional enum RRC_INTEGRITY_CIPHERING_ESTBALISHED to
                   rrc_sync_cause_e_type.
03/07/02   rj      Added additional RRC_EST_TERM_HI_SIGNALLING, and 
                   RRC_EST_TERM_LOW_SIGNALLING fields to enum
                   rrc_establish_cause_e_type.
02/04/02   xfg     Added RRC_LOOPBACK_ESTABLISHED in rrc_sync_cause_e_type for
                   loopback test
01/30/02   vn      Added Ciphering Algorithm type to rrc_cn_ciphering_info_type.
11/19/01   rj      Modified interface to RRC_OPEN_SESSION_REQ command as per the
                   June Specifcation.
09/13/01   vn      Changed RRC_MAX_GSMMAP_RAC_LENGTH to 1 since RAC is only 1 byte.
06/21/01   vn      Changed representation of HFN to a byte array in all messages.
                   This is in line with the representation in USIM. Removed
                   definitions for rrc_close_session_cnf_type and rrc_unit_data_ind_type.
05/24/01   vn      Added definition for rrc_close_session_cnf_type.
05/04/01   jca     Updated rrc_plmn_identity_type to have digit arrays, added 
                   a new abort cause - RRC_ACCESS_CLASS_NOT_ALLOWED and prototype
                   for rrc_act_cnf_type and rrc_act_req_type.
04/29/01   rj      Updated rrc_paging_cause_e_type with
                   RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING and 
                   RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING for December Specification.
03/30/01   rj      Updated rrc_establish_cause_e_type to December Specification.
                   Added cause Unsupported Initial UE Identity to RRC_ABORT_IND.
                   Changed LAC and RAC fields to array two bytes from uint32. 
                   Changed rrc_imei_gsm_map_type to array of uint32 from array
                   of uint8 to be compatible with latest LED based rrcasn1.h                   
03/27/01   rj      Modified rrc_tmsi_gsm_map_type and rrc_ptmsi_gsm_map_type
                   to array of 4 bytes from uint32 to be compatible for Lean
                   decoder and encoder. Modified rrc_lai_list_req to 
03/20/01   vn      Split rrc_service_response_type into rrc_service_cnf and 
                   rrc_service_ind. Re-structured the Service Ind and Cnf,
                   rrc_lai_list_type, rrc_sync_ind_type and rrc_imsi_gsm_map_type. 
03/14/01   vn      Changes after extensive review of the interface.
02/28/01   vn      Added cell access status to rrc_service_ind and rrc_service_cnf 
                   RRC_UNIT_DATA_REQ. Added rrc_forbid_lai_list_update_req.
                   Added NAS system info to rrc_service_ind and rrc_service_cnf.
                   Changed AVAIL_PLMNS to AVAIL_LAIS. Removed access classes.
                   from rrc_est_req. Changed MAX_CN_DOMAINS to 4 as per RRC spec.
                   Updated enum types to remove not needed ASN1 values.
02/13/01   vn      Fixed rrc_lai_rai_u_type to have valid outside the union. 
01/25/01   rj      Added new enum RRC_EST_ORIG_SUBSCRIBED_TRF_CALL to 
                   rrc_establish_cause_e_type
01/19/01   rj      The value for RRC_MAX_MSIN_DIGITS is set to 9.
01/19/01   vn      Renamed the union within rrc_selected_plmn_type and 
                   rrc_requested_plmn_type for ease with readability. 
01/02/01   vn/rj   Added rrc_est_cnf_type with a status and a failure cause. 
                   Also corrected errors from the previous version. 
12/21/00   vn      Added Access Class in Service Req. Access Class is needed
                   to determine Cell Barred Status for Operator reserved cell.
                   Split Access class into Lo and Hi Access Classes.
12/14/00   vn      Added cmd structure for RRC Close Session Req.
12/13/00   vn      Added IMH_T headers in all commands going to MM, added RRC
                   prefix to all #defs, redefined LAI and RAI forbidden lists 
                   and defined a union of LAI and RAI.
12/05/00   vn      Changed PLMN definition to ease logical operations.
12/01/00   vn      Changes for REX++ migration as per ver 0.5 of "Usage of Cmd 
                   Queues" document.
11/08/00   vn      Changes to fix compiler errors/warnings.
10/31/00   vn      Updates from last Interface review.
10/20/00   vn      Created file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/* Include the following two NAS header files to work with IMH_T */
//#include "sys_cnst.h"
#include "sys_stru.h"
#include "sys_v.h"
#include "mm_as_types.h"
#include "sys_plmn_selection.h"
#include "rrcmnif.h"
#include "sys.h"
#include "l1rrcif.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===================================================================
                        CONSTANTS

=====================================================================*/
    


    
#define  RRC_MAX_GSMMAP_IMSI_DIGITS        15
#define  RRC_MAX_GSMMAP_TMSI_LENGTH        4   /* Length of TMSI in bytes */
#define  RRC_MAX_GSMMAP_PTMSI_LENGTH       4   /* Length of PTMSI in bytes */
#define  RRC_MAX_GSMMAP_LAC_LENGTH         2   /* Length of LAC in bytes */
#define  RRC_MAX_GSMMAP_RAC_LENGTH         1   /* Length of RAC in bytes */
#define  RRC_MAX_MCC_DIGITS                3
#define  RRC_MAX_MNC_DIGITS                3
/* The IMSI can have up to a  maximum 15 digits value. IMSI consists of
   MCC(3 digits), MNC( 3 digits) and MSIN up to a maximum of 9 digits */
#define  RRC_MAX_MSIN_DIGITS               9   

#define  RRC_NUM_IMEI_DIGITS               15
/* Max LAIs in the LAI List (Forbidden and Available) */
#define  RRC_MAX_LAI                       30

/* Max number of PLMNs in the acquisition DB */
#define  RRC_MAX_PLMNS                     SYS_PLMN_LIST_MAX_LENGTH

#define  RRC_MAX_GSM_INFO_OCTET_COUNT      8
#define  RRC_MAX_ANSI41_INFO_OCTET_COUNT   2048
#define  RRC_MAX_CN_DOMAINS                2

/* Defines bits for Access Classes 11 to 15 */
#define  RRC_HI_ACCESS_CLASS_ABSENT            0
#define  RRC_HI_ACCESS_CLASS_MAX               5
#define  RRC_ACCESS_CLASS_11_BIT               0x00000001
#define  RRC_ACCESS_CLASS_12_BIT               0x00000002
#define  RRC_ACCESS_CLASS_13_BIT               0x00000004
#define  RRC_ACCESS_CLASS_14_BIT               0x00000008
#define  RRC_ACCESS_CLASS_15_BIT               0x00000010

#define  RRC_MAX_HFN_LENGTH                 3
#define  RRC_MAX_CIPHERING_KEY_LENGTH      16
#define  RRC_MAX_INTEGRITY_KEY_LENGTH      16
#define  RRC_MAX_ROUTING_PARA_LENGTH       2

/* Number of NAS messages that can be stored for CS. 
  This is chosen to be 14 based on below logic
  (Max number of NAS messages which can reach RRC simultaneously 
  i.e. back to back in case of Multi party call)
  - 1 SMS message.
  - 1 SS( Supplementary service) message.
  - 7 Disconnect or Stop DTMF
  - 5 Start DTMF*/
#define MAX_NAS_MESSAGES_FOR_CS_DOMAIN 14

/* CSG whitelist size */
#define  MAX_NO_CSG_LIST SYS_CSG_LIST_MAX_LENGTH*2

/*-------------------------------------------------------------------
                     ENUM: rrc_establish_cause_e_type

This enumerates causes for RRC connection establishment request.
Note: Values for all choices may be different than defined by ASN.1 
compiler in its header file - translation needed.
--------------------------------------------------------------------*/

typedef enum 
{
  RRC_EST_ORIG_CONV_CALL,
  RRC_EST_ORIG_STREAM_CALL,
  RRC_EST_ORIG_INTERACT_CALL,
  RRC_EST_ORIG_BACKGND_CALL,
  RRC_EST_ORIG_SUBSCRIBED_TRF_CALL,
  RRC_EST_TERM_CONV_CALL,
  RRC_EST_TERM_STREAM_CALL,
  RRC_EST_TERM_INTERACT_CALL,
  RRC_EST_TERM_BACKGND_CALL,
  RRC_EST_EMERGENCY_CALL,
  RRC_EST_INTER_RAT_CELL_RESELECT,
  RRC_EST_INTER_RAT_CELL_CHANGE_ORDER,
  RRC_EST_REGISTRATION,
  RRC_EST_DETACH,
  RRC_EST_HI_PRI_SIGNALLING,
  RRC_EST_LOW_PRI_SIGNALLING,
  RRC_EST_CALL_RE_ESTABLISH,
  RRC_EST_TERM_HI_SIGNALLING,
  RRC_EST_TERM_LOW_SIGNALLING,
  RRC_EST_TERM_UNKNOWN
}rrc_establish_cause_e_type;


/*-------------------------------------------------------------------
                     ENUM: rrc_access_class_e_type

This enumerates possible RRC Access Classes.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_ACCESS_CLASS_0  =  0,
  RRC_ACCESS_CLASS_1  =  1,
  RRC_ACCESS_CLASS_2  =  2,
  RRC_ACCESS_CLASS_3  =  3,
  RRC_ACCESS_CLASS_4  =  4,
  RRC_ACCESS_CLASS_5  =  5,
  RRC_ACCESS_CLASS_6  =  6,
  RRC_ACCESS_CLASS_7  =  7,
  RRC_ACCESS_CLASS_8  =  8,
  RRC_ACCESS_CLASS_9  =  9,  
  RRC_LO_ACCESS_CLASS_MAX=  10
}rrc_lo_access_class_e_type;

/* The following value indicates that UE has no Low Access Class */
#define  RRC_LO_ACCESS_CLASS_ABSENT  RRC_LO_ACCESS_CLASS_MAX   

/*-------------------------------------------------------------------
                     ENUM: rrc_initial_ue_id_choice_e_type

This enumerates Initial UE ID choices.
Note: Values for all choices may be different than defined by ASN.1 
compiler in its header file - translation needed.
--------------------------------------------------------------------*/      
typedef enum
{
  RRC_IMSI_UE_ID,
  RRC_TMSI_LAI_UE_ID,
  RRC_PTMSI_RAI_UE_ID,
  RRC_IMEI_UE_ID
}rrc_initial_ue_id_choice_e_type;

/*-------------------------------------------------------------------
                     ENUM: rrc_page_ue_id_choice_e_type

This enumerates UE ID choices in Paging
Note: Values for all choices may be different than defined by ASN.1 
compiler in its header file - translation needed.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_PAGE_WITH_IMSI,
  RRC_PAGE_WITH_TMSI,
  RRC_PAGE_WITH_PTMSI  
}rrc_page_ue_id_choice_e_type;

/*-------------------------------------------------------------------
                     ENUM: rrc_cn_domain_identity_e_type

This enumerates CN Domain types. 
Note: Values for all choices may be different than defined by ASN.1 
compiler in its header file - translation needed.
--------------------------------------------------------------------*/
typedef enum 
{
  RRC_CS_DOMAIN_CN_ID,
  RRC_PS_DOMAIN_CN_ID
}rrc_cn_domain_identity_e_type;


/*-------------------------------------------------------------------
                    ENUM: rrc_requested_plmn_choice_e_type

This enumerates PLMN types as requested by MM.
--------------------------------------------------------------------*/
typedef enum
{
  /* A specific GSM MAP PLMN */
  RRC_GSM_MAP_SPECIFIC_PLMN_REQ,
  /* Any PLMN within GSM MAP */
  RRC_GSM_MAP_ANY_PLMN_REQ
}rrc_requested_plmn_choice_e_type;

/*-------------------------------------------------------------------
                    ENUM: rrc_selected_plmn_choice_e_type

This enumerates PLMN types as selected by RRC.
TBD: How can a GSM_MAP_ANSI_41 PLMN be handled ?
Note: Values for all choices may be different from ASN1 compiler 
      definition. Translation needed.
--------------------------------------------------------------------*/
typedef enum
{
  /* GSM MAP PLMN selected */
  RRC_GSM_MAP_PLMN_SEL,
  /* GSM MAP and ANSI 41 PLMN selected */
  RRC_GSM_MAP_ANSI_41_PLMN_SEL
}rrc_selected_plmn_choice_e_type;



/*-------------------------------------------------------------------
                     ENUM: rrc_paging_cause_e_type

This enumerates the causes for paging (that MM cares) 
Note: Values for all choices may be different than defined by ASN.1 
compiler in its header file - translation needed.
--------------------------------------------------------------------*/
typedef enum 
{
  RRC_PAGE_CAUSE_TERM_CONV_CALL,
  RRC_PAGE_CAUSE_TERM_STREAM_CALL,
  RRC_PAGE_CAUSE_TERM_INTERACT_CALL,
  RRC_PAGE_CAUSE_TERM_BACKGND_CALL,
  RRC_PAGE_CAUSE_SMS,
  RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING,
  RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING,
  RRC_PAGE_CAUSE_UNKNOWN
}rrc_paging_cause_e_type;


/*-------------------------------------------------------------------
                     ENUM: rrc_nas_msg_priority_e_type

This enumerates the priority of NAS signalling message
--------------------------------------------------------------------*/
typedef enum
{
  RRC_NAS_MSG_HIGH_PRIO,
  RRC_NAS_MSG_LOW_PRIO
}rrc_nas_msg_priority_e_type;


/*-------------------------------------------------------------------
                     ENUM: rrc_open_session_status_e_type

This enumerates the status of an Open Session Confirm from RRC
--------------------------------------------------------------------*/
typedef enum
{
  RRC_OPEN_SESS_SUCCESS,
  RRC_OPEN_SESS_FAILURE,
/* New failure cause to indicate Access Class is bard*/
  RRC_OPEN_SESS_FAILURE_AC_BARD,
/* New failure cause to indicate that conn rel in progress*/
  RRC_OPEN_SESS_FAILURE_CONN_REL_IN_PROG
}rrc_open_session_status_e_type;

/*-------------------------------------------------------------------
                     ENUM: rrc_close_session_status_e_type

This enumerates the status of an Close Session Confirm from RRC
--------------------------------------------------------------------*/
typedef enum
{
  RRC_CLOSE_SESS_SUCCESS,
  RRC_CLOSE_SESS_FAILURE
  /* Close session cnf failuer cause when RRC to include this 
   *cause in Close_session_cnf when UE is in OOS 
   */
  ,
  RRC_CLOSE_SESS_UE_OOS
  /*Close session cnf success cause used by RRC to indicate
   * domain is closed, This may happen in cross over secnario
   * where NAS posted Close_req & N/W releas this CN domain 
   * using SCR message. It's used when close_sess_req is received
   * with cause as RRC_UE_INITIATED_DORMANCY*/
  ,
  RRC_CLOSE_SESS_SUCCESS_UE_INIT_DORMANCY
  /*Close session cnf Failuer cause used by RRC to indicate
   * when it can close domain Bcoz of OOS, In this case CN domain
   * is open at both RRC & NAS.It's used when close_sess_req is received
   * with cause as RRC_UE_INITIATED_DORMANCY*/
  ,
  RRC_CLOSE_SESS_FAILURE_UE_INIT_DORMANCY_UE_IN_OOS
}rrc_close_session_status_e_type;

/*-------------------------------------------------------------------
                     ENUM: rrc_abort_cause_e_type

This enumerates the causes for an RRC abort.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_FAIL_TO_CAMP_ON,
  RRC_INTERNAL_ERROR,
  RRC_UNSUPPORTED_REDIRECTION,
  RRC_MAX_ACCESS_FAILURE,
  RRC_FAILURE_DUE_TO_CONGESTION,
  RRC_FAILURE_UNSPECIFIED,
  RRC_UNSUPPORTED_INITIAL_UE_ID,
  /* Below cause is still present for legacy events */
  RRC_ACCESS_CLASS_NOT_ALLOWED,
  RRC_CONNECTION_NOT_AVAILABLE,
  RRC_PCCO_FAILURE,
  RRC_NO_RESOURCE_AVAILABLE,
  RRC_ABORT_LAI_RAI_CHANGED,
  RRC_AC_NOT_ALLOWED_CS_DOMAIN,
  RRC_AC_NOT_ALLOWED_PS_DOMAIN,
  RRC_ABORT_CONN_REJECT_WAIT_TMR,
  /*New abort causes should be added here.*/  
  RRC_ABORT_CAUSE_TYPE_MAX  
}rrc_abort_cause_e_type;


/*-------------------------------------------------------------------
                     ENUM: rrc_rel_cause_e_type

This enumerates the causes of an RRC Connection Release.
Note: Values for all choices may be different than defined by ASN.1 
compiler in its header file - translation needed.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_REL_CAUSE_NORMAL,
  RRC_REL_CAUSE_UNSPEC,
  RRC_REL_CAUSE_PRE_EMPTIVE,
  RRC_REL_CAUSE_CONGESTION,
  RRC_REL_CAUSE_RE_ESTABLISH_REJECT,
  RRC_REL_USER_INACTIVITY,
  RRC_REL_DEEP_FADE,
  RRC_DIRECTED_SIGNALLING_REESTABLISHMENT,
  RRC_REL_CAUSE_OOS,
  /* RRC to incude this cause in abort_cnf 
   * If UE is in OOS & 
   * It receives abort_req with casue as 
   * RRC_UE_INITIATED_DORMANCY. 
   * Please note that in this case RRC connection is intact
   * even after sending abort_cnf to MM.
   */
  RRC_UE_INIT_DORMANCY_UE_IN_OOS,
  /* RRC to incude this cause in abort_cnf 
   * If RRC connection is not present & 
   * It receives abort_req with casue as 
   * RRC_UE_INITIATED_DORMANCY. 
   * This may happen if RRC connection release is 
   * in process & NAS has posted Abort_req to RRC
   */
  RRC_UE_INIT_DORMANCY_ABORT_SUCCESS,
  RRC_REL_DUAL_STANDBY_PS_ABORT_SUCCESS ,
  RRC_CONN_REL_CAUSE_MAX
}rrc_rel_cause_e_type;

/*-------------------------------------------------------------------
                     ENUM: rrc_sync_cause_e_type

This enumerates the causes when sending the Sync primitive to NAS.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_RAB_ESTABLISHED,
  RRC_CIPHERING_ESTABLISHED,
  RRC_INTEGRITY_ESTABLISHED,
  RRC_INTEGRITY_CIPHERING_ESTABLISHED,  
  RRC_LOOPBACK_ESTABLISHED,
  RRC_HANDOVER,
  RRC_CIPHERING_CHANGE
}rrc_sync_cause_e_type;

/*-------------------------------------------------------------------
                     ENUM: rrc_session_close_cause_e_type

This enumerates the causes of an RRC Session close.
TBD: The cause does not come in UTRAN message, values are TBD.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_SESSION_CLOSE_RL_FAILURE,
  RRC_MAX_SESSION_CLOSE_CAUSE
}rrc_session_close_cause_e_type;

/*-------------------------------------------------------------------
                     ENUM: rrc_rab_action_e_type

This enumerates the types of actions taken on a RAB.
--------------------------------------------------------------------*/
typedef enum
{
  RAB_ESTABLISHED,
  RAB_RELEASED,
  RAB_RECONFIGURED
}rrc_rab_action_e_type; 


/*-------------------------------------------------------------------
                     ENUM: rrc_cipher_algo_e_type

This enumerates the Ciphering Algorithms that can be used.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_NO_CIPHERING,
  RRC_KASUMI_ALGORITHM
}rrc_cipher_algo_e_type ;

/*-------------------------------------------------------------------
                     ENUM: rrc_sim_update_e_type

This enumerates the details of SIM for Security.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_SIM_UPDATE_VALUE_CHANGED,
  RRC_SIM_UPDATE_VALUE_VALID,
  RRC_SIM_UPDATE_VALUE_LTE_MAPPED,
  RRC_SIM_UPDATE_VALUE_INVALID
}rrc_sim_update_e_type ;

/*-------------------------------------------------------------------
                     ENUM: rrc_call_type_in_est_req_e_type

This enumerates the details of Call type in EST_REQ from NAS.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_CALL_TYPE_SPEECH,
  RRC_CALL_TYPE_VIDEO,
  RRC_CALL_TYPE_OTHERS,
  RRC_CALL_TYPE_NONE
}rrc_call_type_in_est_req_e_type ;

  /*-------------------------------------------------------------------
                     ENUM: rrc_mm_req_cause_e_type

This enumerates the details of mm req cause from NAS.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_UE_INITIATED_DORMANCY,
  RRC_UE_HIGHER_PRIO_CALL, /*This cause is for Higher priority call on other SIM*/
  RRC_DUAL_STANDBY_PS_ABORT,
  RRC_OTHERS
}rrc_mm_req_cause_e_type;


/*===================================================================
                        DATA STRUCTURES

====================================================================*/

/*-------------------------------------------------------------------
STRUCTURE: rrc_hi_access_class_type

This structure defines the High Access Class (11 to 15) of the UE.
As per 22.011, section 4.2, UE may be a member of more than one 
Hi-Access Classes. A bit mask is used to implement it. 
--------------------------------------------------------------------*/
typedef  uint32  rrc_hi_access_class_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_msin_type

This structure defines the Mobile Subscriber Identification Number
within a PLMN.
--------------------------------------------------------------------*/
typedef struct
{
  /* Length in number of octets */
  uint32   length;
  uint8    value[RRC_MAX_MSIN_DIGITS];
}rrc_msin_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_ue_mode_ind_type

This structure defines the UE mode indication
Note: Used by TDS
--------------------------------------------------------------------*/
typedef struct 
{
  sys_ue_mode_e_type             ue_mode;
  sys_modem_as_id_e_type         as_id;
}rrc_ue_mode_ind_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_plmn_identity_type

This structure defines the PLMN.
Note: Derived from PLMN definition in 23.003
--------------------------------------------------------------------*/
typedef struct
{
  /* Mobile Country Code - one digit per byte range 0..9)  */
  byte mcc[RRC_MAX_MCC_DIGITS];
  /* Number of MNC digits */
  byte num_mnc_digits;
  /* Mobile Network Code - one digit per byte (range 0..9) */
  byte mnc[RRC_MAX_MNC_DIGITS];
}rrc_plmn_identity_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_multimode_subs_chgd_ind_type

This structure  carry AS ID info when multimode sub is changed

--------------------------------------------------------------------*/
typedef struct
{
   subs_capability_e_type  subs_capability[MAX_AS_IDS];
} rrc_multimode_subs_chgd_ind_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_lai_type

This structure defines the Location Area Identification (LAI)
Note: Derived from LAI definition in 23.003
--------------------------------------------------------------------*/
typedef struct
{
  /* PLMN */
  rrc_plmn_identity_type      plmn;
  /* Location Area Code - LAC within a PLMN. FFFE and 0000 are
   * reserved values of LAC for special cases */
  uint8                     lac[RRC_MAX_GSMMAP_LAC_LENGTH];
}rrc_lai_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_rai_type

This structure defines the Routing Area Identifier.
Note: Derived from RAI definition in 23.003
--------------------------------------------------------------------*/
typedef struct
{
  /* Location Area Identifier */
  rrc_lai_type  lai;
  /* Routing Area code within a Location Area */
  uint8 rac[RRC_MAX_GSMMAP_RAC_LENGTH];  
}rrc_rai_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_lai_list_type

This structure defines the list of LAIs.
--------------------------------------------------------------------*/
typedef struct
{
  uint32   num_lais;
  rrc_lai_type  lai[RRC_MAX_LAI];
}rrc_lai_list_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_avail_plmn_type

This structure defines the available PLMN type.
--------------------------------------------------------------------*/
typedef struct
{
  /* PLMN Identifier */
  rrc_plmn_identity_type  plmn;
  /* Radio Access Technology: SYS_RAT_UMTS_RADIO_ACCESS */
  sys_radio_access_tech_e_type rat;
  /* Service capability: CS or PS */
  sys_srv_capability_e_type  service_capability;
  /* Signal quality */
  sys_signal_quality_e_type  signal_quality;
  /* Signaling strength of the strongest cell in PLMN */
  sys_signal_strength_type  signal_strength;

/*CSG info structure will have csg id and hnb name(not supported)
  in phase-1. if CSG info is not included then RRC will set
  csg_id=0xFFFFFFFF*/
  sys_csg_info_s_type csg_info;

  /* E-UTRA frequency for the PLMN */ 
  /* TDS requirement for psuedo reselection */
  uint32 earfcn;
}rrc_avail_plmn_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_avail_plmn_list_type

This structure defines the list of avaliable PLMNs.
--------------------------------------------------------------------*/
typedef struct
{
  uint32   num_plmns;
  rrc_avail_plmn_type plmn[RRC_MAX_PLMNS];
}rrc_avail_plmn_list_type;

/*-------------------------------------------------------------------
STRUCTURE:  

This structure defines the GSM MAP TMSI
Note: Derived from TMSI definition in 23.003
--------------------------------------------------------------------*/
typedef struct
{
  /* 4 octets - all hex values allowed. FFFF FFFF signifies no
  * available TMSI in SIM */
  uint8  value[RRC_MAX_GSMMAP_TMSI_LENGTH];
}rrc_tmsi_gsm_map_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_ptmsi_gsm_map_type

This structure defines the GSM MAP P-TMSI
Note: Derived from P-TMSI definition in 23.003
--------------------------------------------------------------------*/
typedef struct
{
  /* Value - Lower 24 bits to be used */
  uint8  value[RRC_MAX_GSMMAP_PTMSI_LENGTH];
}rrc_ptmsi_gsm_map_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_imsi_gsm_map_type

This structure defines the GSM MAP IMSI
Note: Derived from IMSI definition in 23.003
--------------------------------------------------------------------*/
typedef struct
{
  uint32  num_digits;
  uint8  value[RRC_MAX_GSMMAP_IMSI_DIGITS];
}rrc_imsi_gsm_map_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_tmsi_lai_gsm_map_type

This structure defines the TMSI and LAI as an Initial UE Identity.
--------------------------------------------------------------------*/
typedef struct 
{
  /* TMSI */
  rrc_tmsi_gsm_map_type   tmsi;
  /* Location Area Identification */
  rrc_lai_type        lai;
}rrc_tmsi_lai_gsm_map_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_ptmsi_rai_gsm_map_type

This structure defines the P-TMSI and RAI as an Initial UE Identity.
--------------------------------------------------------------------*/
typedef struct
{
  /* P-TMSI */
  rrc_ptmsi_gsm_map_type    ptmsi;
  /* Roting Area Identification */
  rrc_rai_type      rai;
}rrc_ptmsi_rai_gsm_map_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_imei_gsm_map_type

This structure defines the (International Mobile Equipment Identity)
- IMEI of the UE. 
Note: The format of IMEI is not of any value to RRC.
--------------------------------------------------------------------*/
typedef struct
{
  uint8 value[RRC_NUM_IMEI_DIGITS];
}rrc_imei_gsm_map_type;

/*-------------------------------------------------------------------
UNION: rrc_initial_ue_id_type

This is a union of all possible Initial UE Identifications types.
--------------------------------------------------------------------*/
typedef union
{
  rrc_imsi_gsm_map_type         imsi;
  rrc_tmsi_lai_gsm_map_type     tmsi_lai;
  rrc_ptmsi_rai_gsm_map_type    ptmsi_rai;
  rrc_imei_gsm_map_type         imei;
}rrc_initial_ue_id_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_cn_ciphering_info_type

This structure defines the ciphering information to be passed to RRC
for a CN domain.
--------------------------------------------------------------------*/
typedef struct
{
  /* CN Domain Identity */
  rrc_cn_domain_identity_e_type       domain_id;
  byte  ciphering_key[RRC_MAX_CIPHERING_KEY_LENGTH];
  byte  integrity_key[RRC_MAX_INTEGRITY_KEY_LENGTH];
  rrc_cipher_algo_e_type  cipher_algo; 
  /* Hyper Frame Number */
  byte                  hfn[RRC_MAX_HFN_LENGTH];
}rrc_cn_ciphering_info_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_requested_plmn_type

This structure defines the requested PLMN. The PLMN could be either
GSM MAP or ANSI 41 type. 
--------------------------------------------------------------------*/
typedef struct
{
  /* Type of PLMN - Specific or Any */
  rrc_requested_plmn_choice_e_type  plmn_type;
  /* GSM MAP PLMN Identity */
  rrc_plmn_identity_type      plmn_id;
  
  sys_csg_id_type   csg_id;
}rrc_requested_plmn_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_selected_plmn_type

This structure defines the selected PLMN (selected by RRC). The PLMN 
could be either GSM MAP or ANSI 41 type or both.
TBD: How to handle when PLMN type is GSM_MAP_AND_ANSI_41 ? 
--------------------------------------------------------------------*/
typedef struct
{
  /* Type of PLMN - GSM MAP, ANSI 41 and GSM MAP or No PLMN */
  rrc_selected_plmn_choice_e_type plmn_type;
  /* GSM MAP PLMN Identity */
  rrc_plmn_identity_type      plmn_id;
}rrc_selected_plmn_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_gsm_map_nas_sys_info_type

This structure defines the GSM MAP NAS System Info as received 
in the SIBs
--------------------------------------------------------------------*/
typedef struct
{
  /* Number of Octets - 1 to 8 allowed [10.3.1.9] */
  uint32 length;
  /* NAS System Information */
  uint8  value[RRC_MAX_GSM_INFO_OCTET_COUNT];
}rrc_gsm_map_nas_sys_info_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_ansi41_nas_info_type

This structure defines the ANSI 41 NAS System Info as received in 
the SIBs.
TBD: Since total of upto 2048 octetes[10.3.9.4]  - transfer is thru' pointer ?
--------------------------------------------------------------------*/
typedef struct
{
  /* Number of Octets - max - RRC_MAX_ANSI41_INFO_OCTET_COUNT */
  uint32   length;
  /* Pointer to data */
  uint8    *value;
}rrc_ansi41_nas_info_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_cn_domain_sys_info_type

This structure defines CN Domain System Information
TBD: When CN Domain ID is Don't care - which one of GSM MAP, ANSI 41 
Info to be filled ?
--------------------------------------------------------------------*/
typedef struct
{
  /* CN Domain ID - CS, PS or don't care */
  rrc_cn_domain_identity_e_type       cn_domain_id;
  union
  {
    /* GSM MAP NAS System Information */
    rrc_gsm_map_nas_sys_info_type   gsm_map_info;
    /* ANSI 41 NAS System Info */
    rrc_ansi41_nas_info_type      ansi41_info;
  }cn_domain_specific_info;
}rrc_cn_domain_sys_info_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_nas_msg_type

This structure defines the NAS message type. Due to NAS message length
of upto 4095 octets [10.3.1.8], the message is passed thru' a pointer.
--------------------------------------------------------------------*/
typedef struct
{
  /* Length (in Octets) of NAS Message */
  uint32    length;
  /* Pointer to data */
  uint8     *value;
/*NAS OTA's Protocol discriminator & Transaction I.D*/
  byte pd_ti;
/*NAS OTA's message type*/
  byte msg_type;
}rrc_nas_msg_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_rab_id_type
It contains the RAB ID. 
--------------------------------------------------------------------*/
typedef uint32  rrc_rab_id_type;

typedef enum
{
  MM_AS_CS_PS_ALLOWED,/* Allow both CS and PS paging response irrespective of DSAC/AC */
  MM_AS_PS_ALLOWED,/* Allow PS response only irrespective of PS-DSAC/AC. For CS response legacy behavior */
  MM_AS_CS_ALLOWED,/* Allow CS response only irrespective of CS-DSAC/AC. For PS response legacy behavior */
  MM_AS_NONE /* No preference. Follow legacy behavior */
} mm_as_page_rsp_e_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_service_info_type
It contains the selected PLMN, NAS system info and cell access info
based on the selected cell.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_selected_plmn_type         selected_plmn;

  /* Boolean indicating whether gsm_map_common_info is included or not */
  boolean                        gsm_map_common_info_incl;
  /* System info common to CN domains */
  rrc_gsm_map_nas_sys_info_type  gsm_map_common_info;
  /* System info specific to CN domains */
  uint32                         num_cn_domains;
  rrc_cn_domain_sys_info_type    cn_domain_info[RRC_MAX_CN_DOMAINS];
  /* Cell Access Status */
  mm_as_cell_access_status_T     cell_access;

  /* This Structure is defined for the implmentation of Domain Specific
       Access Control Barring feature
  */

  /* if present normal cell_access class(es) should not be used */
  boolean dsac_present;
  
  /* Cell Access Status for CS domain */
  mm_as_cell_access_status_T     cs_cell_access;

  /* Cell Access Status for PS domain */
  mm_as_cell_access_status_T     ps_cell_access;

  boolean                        ppac_present;
  boolean                        cs_reg_allowed;
  boolean                        ps_reg_allowed;
  mm_as_page_rsp_e_type          page_rsp_allowed;
  
/*if UE camped on a CSG cell then CSG info structure will have csg id 
  and hnb name(not supported in phase-1). if UE campes on a macro cell
  then RRC will set  csg_id=0xFFFFFFFF*/
  sys_csg_info_s_type csg_info;
  
}rrc_service_info_type;

/* Defines the codec type that is configured in MVS.
  * This info is passed to NAS, so that the correct codec can be enabled. 
  */
typedef enum 
{
  RRC_CODEC_NONE,
  RRC_CODEC_NB_AMR,
  RRC_CODEC_WB_AMR,
  RRC_CODEC_MAX
} rrc_codec_type_e_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_rab_info_type
It contains the RAB info that indicates the action taken by RRC on a
RAB and the corresponding RAB ID.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_rab_action_e_type  action;
  rrc_rab_id_type   rab_id;
  rrc_codec_type_e_type codec;
  /*Basically this will tell MM about the CS RAB type i.e. Voice or Data*/
  rrcmn_rab_status_T cs_rab_type;    
  umts_wcdma_cs_voice_bearer_enum_T  cs_voice_type;
}rrc_rab_info_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_corenetwork_e_type
This identifies to which Core Network that Intra Domain NAS Node 
Selector is associated with.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_GSM_MAP,   /* Indentifes GSM MAP Core network */
                 /* ANSI-41 Core network will be added here in future 
                    implementation*/
  RRC_MAX_CORE_NETWORK 
} rrc_corenetwork_e_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_routing_basis_e_type
This identifies type of routing basis for GSM MAP core network 
--------------------------------------------------------------------*/
typedef enum
{
  RRC_LOCAL_TMSI_OR_PTMSI, /* If this basis is selected, then routing
                              parameter should contains the bits from
                              b14 through b23 of either TMSI or 
                              PTMSI. */  
  RRC_TMSI_OR_PTMSI_SAME_PLMN_AND_DIFFERENT_RA_OR_LA,
                           /* If this basis is selected, then routing
                              parameter should contains the bits from
                              b14 through b23 of either TMSI or
                              PTMSI. */  
  RRC_TMSI_OR_PTMSI_OF_DIFFERENT_PLMN,
                           /* If this basis is selected, then routing
                              parameter should contains the bits from
                              b14 through b23 of either TMSI or
                              PTMSI */ 

  RRC_IMSI_RESP_TO_IMSI_PAGING,
                           /* If this basis is selected, then routing
                              parameter should contains DecimalToBinary
                              [(IMSI div 10) mod 1000]. The bits of
                              result are numbered from b0 through b9,
                              with b0 being LSB. */
  RRC_IMSI_UE_INITIATED_EVENT,
                           /* If this basis is selected, then routing
                              parameter should contains DecimalToBinary
                              [(IMSI div 10) mod 1000]. The bits of
                              result are numbered from b0 through b9,
                              with b0 being LSB. */
  RRC_IMEI,
                           /* If this basis is selected, then routing
                              parameter should contains DecimalToBinary
                              [(IMEI div 10) mod 1000]. The bits of
                              result are numbered from b0 through b9,
                              with b0 being LSB. */
  
  RRC_MAX_ROUTING_BASIS 
} rrc_routing_basis_e_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_gsmmap_route_info_type
This contains the GSM MAP routing information for Initial Direct
Transfer message to establish Signalling Connection 
--------------------------------------------------------------------*/
typedef struct
{
  rrc_routing_basis_e_type routing_basis; 
                             /* Identifies the routing  basis */
  uint8 routing_parameter[RRC_MAX_ROUTING_PARA_LENGTH];
                             /* Routing paramter is 10 bits length 
                                98765432 10xxxxxx*/
  boolean entered_parameter;  /* Entered_parameter will be set to TRUE 
                                if most significant byte of the current
                                LAI/RAI is different compared to the
                                most significant byte of the LAI/RAI
                                stored in SIM. Otherwise it will
                                set to FALSE */
                                 
} rrc_gsmmap_route_info_type;


/*-------------------------------------------------------------------
STRUCTURE: rrc_intra_domain_nas_node_sel_type
It contains the Routing Info for the UTRAN to route Initial
Direct Transfer during Signalling Connection Establishment 
--------------------------------------------------------------------*/
typedef struct
{

  rrc_corenetwork_e_type         corenetwork_choice;
  rrc_gsmmap_route_info_type     routing_info;
  /* ANSI-41 routing info type needs to be added here in future */
  
}  rrc_intra_domain_nas_node_sel_type;

/*-------------------------------------------------------------------
STRUCTURE: rrc_security_update_req_type
It contains the updated ciphering information for a particular 
CN domain. 
--------------------------------------------------------------------*/
typedef struct
{

  rrc_sim_update_e_type          security_update_status;
  rrc_cn_domain_identity_e_type  domain_id;
  rrc_cn_ciphering_info_type     ciphering_info;
  
}  rrc_security_update_req_type;

/*-------------------------------------------------------------------
ENUM: rrc_stop_cause_e_type
It defines the casue why a RRC_STOP_WCDMA_MODE_REQ is sent from MM.
Note: currently, only two causes, RAT Change and Deep Sleep, are defined
and more causes may be defined in the future.
--------------------------------------------------------------------*/
typedef enum
{
  RRC_RAT_CHANGE,
  RRC_DEEP_SLEEP,
  RRC_WTOG_CC_ORDER,
  RRC_DUAL_SWITCH,  /*For FEATURE_DUAL_SIM*/
  RRC_USER_RAT_CHANGE,
  RRC_UE_SGLTE_MODE_SWITCH
} rrc_stop_cause_e_type;

typedef enum
{
  RRC_NORMAL_ACQ,
  RRC_MO_CS_CALL,
  RRC_MO_PS_CALL,
  RRC_MO_CS_ECALL
} rrc_mm_service_req_cause_e_type;

/*===================================================================
       MM to RRC Requests 

=====================================================================*/

/*-------------------------------------------------------------------
MM to RRC Request: RRC_EST_REQ
Used by MM to request RRC Connection Establishment
--------------------------------------------------------------------*/
typedef struct
{
  rrc_establish_cause_e_type       cause;
  /* Choice of Initial UE ID */
  rrc_initial_ue_id_choice_e_type  ue_id_choice;
  /* Initial UE ID */
  rrc_initial_ue_id_type           initial_ue_id;
  /*CN Domain Identity*/
  rrc_cn_domain_identity_e_type     cn_domain_id;
  /*Call Type*/
  rrc_call_type_in_est_req_e_type  call_type;
  /* Routing Area ID */
  rrc_rai_type         rai;
  sys_modem_as_id_e_type as_id;
}rrc_est_req_type;


/*-------------------------------------------------------------------
MM to RRC Request: RRC_SERVICE_REQ
Used by MM to request Service Acquisition by RRC.
--------------------------------------------------------------------*/
typedef struct
{
  /* Type of PLMN requested */
  rrc_requested_plmn_type     requested_plmn;
  /* Forbidden LAI list */
  rrc_lai_list_type           forbid_lai_list;

  boolean                     dual_mode;


  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  /* User band preference */
  sys_band_mask_type          band_pref;

  /* Determines if service search should be done according to the uniform
   * out of service operation or according to the default behaviour.
   * TRUE: Uniform OOS operation needs to be followed. 
   * FALSE: Otherwise
   */
  boolean  use_timer;

  /* Determines if the service request is new or not. If it is new then
   * no stored information related to frequency scanning should be used; 
   * otherwise stored information should be used.
   */
  boolean  new_scan;

  /* Timer in milliseconds that determines how long UE should search for 
   * service on W
   */
  uint32   wcdma_scan_time;

  sys_acq_scan_type_e_type acq_scan_type;

  /*Boolean to indicate if GSM search is done or not*/
  boolean grat_scan_done;

  rrc_mm_service_req_cause_e_type service_req_cause;

  sys_rat_pri_list_info_s_type    rat_pri_list_info; 
  sys_modem_as_id_e_type as_id;
  /*Used by NAS to trigger ACQ_DB only search*/
  sys_scan_scope_e_type   scan_scope;

  /*TDS RRC requirement for PS time transfer*/
  uint32   trm_timeout_value;

  boolean is_ehplmn_to_be_camped;

}rrc_service_req_type;


/*-------------------------------------------------------------------
MM to RRC Request: RRC_FORBID_LAI_LIST_UPDATE_REQ
Used by MM to update the forbidden LAI list with RRC. 
--------------------------------------------------------------------*/
typedef struct
{
  /* Forbidden LAI list */
  rrc_lai_list_type         forbid_lai_list;
  sys_modem_as_id_e_type as_id;
}rrc_forbid_lai_list_update_req_type;

/*-------------------------------------------------------------------
MM to RRC Request: RRC_SIM_INSERTED_REQ
MM sends this primitive when SIM is inserted or swapped
--------------------------------------------------------------------*/
typedef struct
{
  /* UE's Low Access Class */
  rrc_lo_access_class_e_type  lo_access_class;
  /* UE's High Access Class bitmask */
  rrc_hi_access_class_type    hi_access_class;
  /* Indicates if valid TMSI is present */
  boolean                     tmsi_present;
  /* Indicates if valid PTMSI is present */
  boolean                     ptmsi_present;
  /* Indicates if valid IMSI is present */
  boolean                     imsi_present;
  rrc_tmsi_gsm_map_type       tmsi;
  rrc_ptmsi_gsm_map_type      ptmsi;
  rrc_imsi_gsm_map_type       imsi;
  /* Ciphering information */
  rrc_cn_ciphering_info_type  cipher_info[RRC_MAX_CN_DOMAINS];
  sys_modem_as_id_e_type as_id;
  byte admin_data;
}rrc_sim_inserted_req_type;

/*-------------------------------------------------------------------
MM to RRC Request: RRC_SIM_UPDATE_REQ
MM sends this primitive when it updates LAI or RAI info in the SIM.
--------------------------------------------------------------------*/
typedef struct
{
  /* UE's Low Access Class */
  rrc_lo_access_class_e_type  lo_access_class;
  /* UE's High Access Class bitmask */
  rrc_hi_access_class_type    hi_access_class;
  /* Indicates if valid TMSI is present */
  boolean                     tmsi_present;
  /* Indicates if valid PTMSI is present */
  boolean                     ptmsi_present;
  rrc_tmsi_gsm_map_type       tmsi;
  rrc_ptmsi_gsm_map_type      ptmsi;
  /* Security Updated Information */
  rrc_security_update_req_type security_update_info;
  sys_modem_as_id_e_type as_id;
}rrc_sim_update_req_type;

/*-------------------------------------------------------------------
MM to RRC Request: RRC_ACTIVATION_RSP
MM sends this primitive to respond RRC_ACTIVATION_IND
-------------------------------------------------------------------*/
typedef struct
{
  rrc_plmn_identity_type      plmn_id;
  sys_modem_as_id_e_type as_id;
} rrc_activation_rsp_type;

/*-------------------------------------------------------------------
MM to RRC Request: RRC_DATA_REQ
Used by MM to transfer Signalling data to CN via existing RRC Connection
--------------------------------------------------------------------*/
typedef struct
{
  /* CN domain for which the message is intended */
  rrc_cn_domain_identity_e_type   cn_domain_id;
  rrc_nas_msg_priority_e_type     priority;
  boolean                         is_emergency_call;
  rrc_nas_msg_type    nas_msg;
  sys_modem_as_id_e_type as_id;
}rrc_data_req_type;

/*-------------------------------------------------------------------
MM to RRC Request: RRC_OPEN_SESSION_REQ
Used by MM to open a Signalling Session via existing RRC Connection.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_intra_domain_nas_node_sel_type intra_domain_nas_node_selector;
  /* The CN domain to open session for */
  rrc_cn_domain_identity_e_type     cn_domain_id;
  rrc_nas_msg_type    nas_msg;
  rrc_establish_cause_e_type est_cause;

  /* GSM MAP PLMN Identity */
  rrc_plmn_identity_type      selected_plmn_id;

  /*Call Type*/
  rrc_call_type_in_est_req_e_type  call_type;
  sys_modem_as_id_e_type as_id;
}rrc_open_session_req_type;


/*-------------------------------------------------------------------
MM to RRC Request: RRC_CLOSE_SESSION_REQ
Used by MM to close a Signalling Session.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_cn_domain_identity_e_type     cn_domain_id;
  /* Close session request cause ::-
   * MM to set this cause to RRC_UE_INITIATED_DORMANCY
   * in Close session req  if UE wants to close the CN doamin
   * Bcoz of UE INITIATED DORMANCY,
   * else MM to set this casue to RRC_OTHERS
   */
  rrc_mm_req_cause_e_type close_session_cause;
  sys_modem_as_id_e_type as_id;
}rrc_close_session_req_type;


/*-------------------------------------------------------------------
MM to RRC Request: RRC_ACT_REQ
Used by MM to activate RRC for providing emergency call service.
--------------------------------------------------------------------*/
typedef struct
{
  boolean dummy;
}rrc_act_req;


/*-------------------------------------------------------------------
MM to RRC Request: RRC_DEACT_REQ
Used by MM to deactivate RRC and lower layers.
--------------------------------------------------------------------*/
typedef struct
{
  sys_stop_mode_reason_e_type deact_reason;
  sys_modem_as_id_e_type as_id;
}rrc_deact_req;


/*-------------------------------------------------------------------
MM to RRC Request: RRC_PLMN_LIST_REQ
Used by MM to request a list of available PLMNs.
--------------------------------------------------------------------*/
typedef struct
{
  /* User band preference */
  sys_band_mask_type              band_pref;

  /* Search should be done on both RATs if dual_mode is set to TRUE */
  boolean                         dual_mode;

  /* Indicates whether Automatic or Manual PLMN search should be performed */
  sys_network_selection_mode_e_type network_select_mode;

  /* Contains the list of higher priority PLMNs in sorted order for Automatic
   * PLMN Search
   */
  sys_plmn_rat_list_s_type plmn_list;
  uint8         trans_id;

  /* Informs RRC whether PLMN_LIST_REQ is for "full service search"
   * or "plmn list search"
   */
  sys_service_search_e_type service_search;

  /*Informs RRC of the original RAT priority information */
  sys_rat_pri_list_info_s_type    rat_pri_list_info;

  /*Informs RRC of the BST based RAT priority information */
  sys_rat_pri_list_info_s_type    mcc_mode_band_filter;

  sys_modem_as_id_e_type as_id;
  sys_scan_scope_e_type   scan_scope;

  /*Informs RRC of the timer values for Manual PLMN Search Speedup */
  sys_manual_search_timer_value_type    search_timer_value;  

}rrc_plmn_list_req_type;


/*-------------------------------------------------------------------
MM to RRC Request: RRC_ABORT_REQ
Used by MM to request aborting of RRC Establishment Request.
--------------------------------------------------------------------*/
typedef struct
{
  boolean delay_sig_con_release;
  boolean auth_failed_bar_cells;
  /* Added for Hard abort handling during RAT change done by user
     during CS/PS session */
  boolean hard_abort;
  /* ABORT REQ Type ::-
   * MM to set this cause to RRC_UE_INITIATED_DORMANCY
   * in abort req if UE wants to close the RRC connection 
   * Bcoz of UE INITIATED DORMANCY,
   * else MM to set this casue to RRC_OTHERSY
   */
  rrc_mm_req_cause_e_type abort_cause;
  sys_modem_as_id_e_type as_id;
}rrc_abort_req_type;

/*-------------------------------------------------------------------
MM to RRC Request: RRC_STOP_WCDMA_MODE_REQ
Used by MM to request stopping WCDMA mode.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_stop_cause_e_type cause;
  sys_modem_as_id_e_type as_id;
} rrc_stop_wcdma_mode_req_type;

/*-------------------------------------------------------------------
MM to RRC Request: RRC_CHANGE_MODE_IND
Used by MM to change the mode of operation
This is used by NAS when the mode is changed 
from 3G only to Automatic becuase of User selection
--------------------------------------------------------------------*/
typedef struct
{
  boolean                     dual_mode;
  /* added band_pref for dual mode change handling*/
  sys_band_mask_type              band_pref;

  sys_rat_pri_list_info_s_type    rat_pri_list_info; 
  sys_modem_as_id_e_type as_id;
} rrc_change_mode_ind_type;

/*===================================================================
        RRC to MM CNF and IND types

=====================================================================*/

/*-------------------------------------------------------------------
RRC to MM: RRC_REL_IND
Used by RRC to indicate Release of an RRC Connection. It is sent only
if an RRC connection was up before (was established and indicated to 
MM through RRC_EST_REQ before).
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  /* Release cause type */
  rrc_rel_cause_e_type    rel_cause;
  /* Highest HFN in CS Domain */
  byte            hfn_cs[RRC_MAX_HFN_LENGTH];
  /* Highest HFN in PS Domain */
  byte            hfn_ps[RRC_MAX_HFN_LENGTH];
  sys_modem_as_id_e_type as_id;
}rrc_rel_ind_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_SYNC_IND
Used by RRC to synchronize RRC and MM.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  rrc_cn_domain_identity_e_type   cn_domain_id;
  boolean             rab_id_present;
  rrc_rab_info_type   rab_info;
  rrc_sync_cause_e_type   cause;
  /*NAS should look at this field only if the CN domain is CS. Since the requirement is for voice call.
   * RRC will not set this field properly for PS call currently.
   * ciphering_enabled = TRUE :- indicate ciphering is enabled for CS call
   * ciphering_enabled = FALSE :- indicate ciphering is disabled for CS call
   */
  boolean ciphering_enabled; 
  sys_modem_as_id_e_type as_id;
}rrc_sync_ind_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_SERVICE_CNF
Used by RRC to indicate Service Confirmation. This is sent in response
to RRC_SERVICE_REQ.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;

  /* Service status indicates service availibility */
  mm_as_service_state_T       service_status;

  /* Service confirmation status */
  mm_as_status_T              status;

  rrc_service_info_type       service_info;

  /* Avaliable PLMN list */
  rrc_avail_plmn_list_type    avail_plmn_list;

  sys_band_mask_e_type        active_band;

  /* This variable is used to inform NAS whether W
   * search was completed or not in the alloted time.
   * TRUE  => Search Done.
   * FALSE => Otherwise.
   * The value of this variable is valid only when uoos_use_timer
   * in RRC_SERVICE_REQ was set to TRUE and when 
   * available PLMN list is empty.
   */
  boolean                     wcdma_search_done;

  /* This var has been added to inform NAS (using var ue_oos in RRC_SERVICE_CNF) 
   * that service is available. NAS looks at this var, ue_oos, to determine if 
   * limited service request should be sent to RRC or not.
   * Notice that with UOOS, SD always sends Automatic Service Request and it 
   * sends Limited Service Request only when NAS informs SD that Limited Service 
   * is available.
   */
  boolean                     ue_oos;

  /* NAS needs cell id in some of the scenarios.
   * e.g. If LAU is rejected by the network for MAX_NUM_TRIES then, MM is 
   * not allowed to do any more LAUs unless the UE is reselected to a new cell.
   */

  /* MM is supposed to look at the cell_id only when service_status is 
   * MM_AS_SERVICE_AVAILABLE.
   */
  boolean      is_cell_id_valid;

  uint32      cell_id;
  sys_modem_as_id_e_type as_id;

  /* Added to indicate if NO_SERVICE is because of no TRM resource */
  sys_acq_status_e_type  acq_status;

}rrc_service_cnf_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_SERVICE_IND
Used by RRC to indicate Service Indication.
Service Indication is sent whenever a new cell is selected or when
the NAS system info on the camped on cell changes. It is also sent
when the camped on cell is lost and no new cell is found.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  /* Service status indicates service availibility */
  mm_as_service_state_T       service_status;
  rrc_service_info_type       service_info;
  sys_band_mask_e_type        active_band;

  /* Timer in milliseconds that determines how much more UE should search for service 
   * before going to sleep.
   * The value returned to NAS in this var is valid only when service_status =
   * MM_AS_NO_SERVICE
   */
  uint32   uoos_rem_awake_time;

  /* Timer in milliseconds that determines how much more UE should sleep before 
   * searching for service.
   * The value returned to NAS in this var is valid only when service_status =
   * MM_AS_NO_SERVICE
   */
  uint32   uoos_rem_sleep_time;

  /* NAS needs cell id in some of the scenarios.
   * e.g. If LAU is rejected by the network for MAX_NUM_TRIES then, MM is 
   * not allowed to do any more LAUs unless the UE is reselected to a new cell.
   */

  /* MM is supposed to look at the cell_id only when service_status is 
   * MM_AS_SERVICE_AVAILABLE.
   */
  boolean      is_cell_id_valid;

  uint32      cell_id;
  sys_modem_as_id_e_type as_id;

  /* Added to indicate if NO_SERVICE is because of no TRM resource */
  sys_acq_status_e_type  acq_status;

  /* Added to indicate if NO SERVICE is due to RLF or not. */
  sys_no_svc_cause_e_type  no_service_cause;

  /* Time in ms for which NAS should initiate suitable search requests.
     During RLF value is 30s. */
  dword                    suitable_search_time;
}rrc_service_ind_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_INTERRAT_CELL_CHANGE_IND
Used by RRC to indicate Service Indication.
Service Indication is sent whenever a new cell is selected or when
the NAS system info on the camped on cell changes. It is also sent
when the camped on cell is lost and no new cell is found.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  /* Service status indicates service availibility */
  mm_as_service_state_T       service_status;
  
  rrc_service_info_type       service_info;
}rrc_interrat_cell_change_ind_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_ACTIVATION_IND
RRC sends this primitive to inform MM it is not the active RAT after 
RRC completes an GSM to WCDMA inter-RAT operation.
-------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T                   message_header;

  /* status */
  mm_as_status_T          status;
  sys_modem_as_id_e_type as_id;
} rrc_activation_ind_type;



/*-------------------------------------------------------------------
RRC to MM: RRC_PLMN_LIST_CNF
Used by RRC to report available PLMNs. This is sent as a response to 
RRC_PLMN_LIST_REQ.
--------------------------------------------------------------------*/
typedef struct
{  
  /* Message header needed by MM */
  IMH_T           message_header;

  /* Available LAI list */
  rrc_avail_plmn_list_type    avail_plmn_list;

  /* Status */
  mm_as_status_T              status;

  /* Indicates whether plmn_list_cnf is for automatic PLMN search or 
   * manual PLMN search
   */
  sys_network_selection_mode_e_type network_select_mode;
  uint8             trans_id;

  /* RRC just updates this var with the value received from NAS in PLMN_LIST_REQ
   * for service_search.
   */
  sys_service_search_e_type service_search;
  sys_modem_as_id_e_type as_id;
}rrc_plmn_list_cnf_type;


/*-------------------------------------------------------------------
RRC to MM: RRC_ABORT_IND
Used by RRC to indicate an aborting of RRC Connection Establishment. 
This is sent when RRC Connection establishment procedure needs to be 
aborted before an RRC connection has been established. 
NOTE: This is not used once an RRC Connection is established and has 
been indicated to MM through RRC_EST_CNF.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T               message_header;
  rrc_abort_cause_e_type    abort_cause;

  /* Connection reject wait timer after which NAS should retry connection Range : 1 to 15 seconds */
  /* NAS should look into this value only when abort cause is RRC_ABORT_CONN_REJECT_WAIT_TMR */
  uint8           conn_rej_wait_timer;

  /* Highest HFN in CS Domain */
  byte            hfn_cs[RRC_MAX_HFN_LENGTH];
  /* Highest HFN in PS Domain */
  byte            hfn_ps[RRC_MAX_HFN_LENGTH];
  sys_modem_as_id_e_type as_id;
}rrc_abort_ind_type;


/*-------------------------------------------------------------------
RRC to MM: RRC_PAGE_IND
Used by RRC to indicate an upcoming paging message.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  rrc_cn_domain_identity_e_type   cn_domain_id;
  rrc_paging_cause_e_type         paging_cause;
  /* The UE ID used for paging */
  rrc_page_ue_id_choice_e_type    page_ue_id;
  sys_modem_as_id_e_type as_id;
}rrc_page_ind_type;


/*-------------------------------------------------------------------
RRC to MM: RRC_DATA_IND
Used by RRC to send signalling data to MM, received through existing
RRC connection.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  /* CN domain to which the data belongs */
  rrc_cn_domain_identity_e_type    cn_domain_id;
  rrc_nas_msg_type                 nas_msg;
  sys_modem_as_id_e_type as_id;
}rrc_data_ind_type;


/*-------------------------------------------------------------------
RRC to MM: RRC_OPEN_SESSION_CNF
Used by RRC in response to RRC_OPEN_SESSION_REQ .
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  rrc_open_session_status_e_type      status;
  /* CN domain for which signalling session is opened */
  rrc_cn_domain_identity_e_type       cn_domain_id;
  sys_modem_as_id_e_type as_id;
}rrc_open_session_cnf_type;


/*-------------------------------------------------------------------
RRC to MM: RRC_CLOSE_SESSION_IND
Used by RRC to indicate that a Signalling session is closed.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  rrc_session_close_cause_e_type    cause;
  /* CN domain for which signalling session is closed */
  rrc_cn_domain_identity_e_type     cn_domain_id;
  sys_modem_as_id_e_type as_id;
}rrc_close_session_ind_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_CLOSE_SESSION_CNF
Used by RRC to indicate that a Signalling session is closed. This 
command is sent in response RRC_CLOSE_SESSION_REQ
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  rrc_close_session_status_e_type    status;
  /* CN domain for which signalling session is closed */
  rrc_cn_domain_identity_e_type     cn_domain_id;
  sys_modem_as_id_e_type as_id;
}rrc_close_session_cnf_type;



/*-------------------------------------------------------------------
RRC to MM: RRC_EST_CNF
Used by RRC to indicate either success or failure of RRC Connection 
Establishment. This is sent in response to RRC_EST_REQ.
--------------------------------------------------------------------*/                                                          
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  sys_modem_as_id_e_type as_id;
}rrc_est_cnf_type;


/*-------------------------------------------------------------------
RRC to MM: RRC_ABORT_CNF
Used by RRC as a sync point to indicate to MM that Abort Request has 
been processed. This is sent in response to RRC_ABORT_REQ.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;

  /* Highest HFN in CS Domain */
  byte            hfn_cs[RRC_MAX_HFN_LENGTH];
  /* Highest HFN in PS Domain */
  byte            hfn_ps[RRC_MAX_HFN_LENGTH];

  /* RRC Connection Release Cause */
  rrc_rel_cause_e_type conn_rel_cause;
  sys_modem_as_id_e_type as_id;
}rrc_abort_cnf_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_DEACT_CNF
Used by RRC as a sync point to indicate to MM that Abort Request has 
been processed. This is sent in response to RRC_DEACT_CNF.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;

  /* Highest HFN in CS Domain */
  byte            hfn_cs[RRC_MAX_HFN_LENGTH];
  /* Highest HFN in PS Domain */
  byte            hfn_ps[RRC_MAX_HFN_LENGTH];
  sys_modem_as_id_e_type as_id;
}rrc_deact_cnf_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_STOP_WCDMA_MODE_CNF
Used by RRC when it deactivates the WCDMA Mode of operation. This is 
sent in response to RRC_STOP_WCDMA_MODE_REQ.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  
  /* Highest HFN in CS Domain */
  byte            start_cs[RRC_MAX_HFN_LENGTH];
  /* Highest HFN in PS Domain */
  byte            start_ps[RRC_MAX_HFN_LENGTH];
  sys_modem_as_id_e_type as_id;
} rrc_stop_wcdma_mode_cnf_type;

/*-------------------------------------------------------------------
MM to RRC Indication: RRC_NW_SEL_MODE_RESET_IND
Used by MM to indicate to reset the N/W mode to AUTOMATIC when FULL service
is acquired in LIMITED_ROUTINE_ACQ n/w selection mode.
--------------------------------------------------------------------*/
typedef struct
{
  /* Indicates N/W selection type, set to AUTO */
  sys_network_selection_mode_e_type network_select_mode;
  sys_modem_as_id_e_type as_id;
}rrc_nw_sel_mode_reset_ind_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_WTOG_CCO_COMPLETE_IND
Used by RRC to indicate the successful completion of WtoG CCO 
--------------------------------------------------------------------*/                                                          
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  sys_modem_as_id_e_type as_id;
}rrc_wtog_cco_complete_ind_type;


/*-------------------------------------------------------------------
RRC to MM: RRC_HSDPA_HSUPA_CELL_IND
Used by RRC to indicate if cell supports HSDPA/HSUPA
--------------------------------------------------------------------*/                                                          
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  sys_hs_ind_e_type hsdpa_hsupa_ind;
  sys_modem_as_id_e_type as_id;
}rrc_hsdpa_hsupa_cell_ind_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_PCH_STATE_IND
Used by RRC to indicate if cell_PCH state info to MM
--------------------------------------------------------------------*/  
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  sys_pch_state_s_type pch_state_info;
  sys_modem_as_id_e_type as_id;
}rrc_pch_state_ind_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_HSDPA_HSUPA_CALL_STATUS
Used by RRC to indicate if HSDPA and HSUPA calls are in progress or not
--------------------------------------------------------------------*/                                                          
typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;
  sys_hs_ind_e_type hs_call_status_ind;
  sys_modem_as_id_e_type as_id;
}rrc_hspa_call_status_ind_type;

/*-------------------------------------------------------------------
MM to RRC Request: RRC_PSEUDO_LTE_LIST_UPDATE_IND
Used by MM to indicate change in LTE neighbor frequencies change
--------------------------------------------------------------------*/
typedef struct 
{
  sys_modem_as_id_e_type         as_id;
  boolean                        can_use_pseudo;
} rrc_pseudo_lte_list_update_ind_type;



/*-------------------------------------------------------------------
Typedef of callback function pointers used to indicate current rrc state and 
serving cell ID to upper layers. 
--------------------------------------------------------------------*/

/* struct to indicate all the necessary
 * rrc state change ind and location
 * information
 */

typedef struct
{
  /* Boolean to indicate whether location info provided is valid or not */
  boolean loc_info_invalid;                                  
  /* Indicates the previous RRC state */
  uint32 prev_state;
  /* Indicates the current RRC state */
  uint32 curr_state;
  /* Indicates the service cell ID */
  uint32 cell_id;
  /* Indicates the selected PLMN */
  rrc_plmn_identity_type plmn_id;
  /* Indicates the LAC ID */
  uint8                     lac[RRC_MAX_GSMMAP_LAC_LENGTH];
  sys_modem_as_id_e_type as_id;
} home_zone_ind_type;

typedef 
void RRC_HOME_ZONE_IND_CB_TYPE(home_zone_ind_type hzi);


typedef struct
{
  /* Indicates the service cell ID */
  uint32 cell_id;
  /* Indicates the selected PLMN */
  rrc_plmn_identity_type plmn_id;
  /* Indicates the LAC ID */
  uint8                     lac[RRC_MAX_GSMMAP_LAC_LENGTH];
  /* Indicates the serving PSC */
  uint16 psc;
  /*DL UARFCN*/
  uint16 uarfcn_dl;
  /*UL UARFCN*/
  uint16 uarfcn_ul;
  sys_modem_as_id_e_type as_id;
  sys_band_mask_e_type        current_band;
} cell_info_type;

typedef struct
{
  /* Message header needed by MM */
  IMH_T  message_header;

  /* Service status indicates service availibility */
  sys_srv_status_e_type  service_status;

  /* PLMN */
  rrc_selected_plmn_type  selected_plmn;

  /* RAT */
  sys_radio_access_tech_e_type rat;

  /* Signal quality */
  sys_signal_quality_e_type  signal_quality;

  /* Signaling strength of the strongest cell in PLMN */
  sys_signal_strength_type  signal_strength;
  /*boolean to say if RRC is in connected mode OOS*/
  boolean is_connected_mode_oos;
  sys_modem_as_id_e_type as_id;
} rrc_conn_mode_service_status_ind_type;

/* Callback function type for cell ID change notification */
typedef 
void RRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE(cell_info_type cell_info);

typedef struct 
{
  boolean                          dual_standby_mode;
  sys_modem_dual_standby_pref_e_type  dual_standby_mode_info;
  sys_modem_device_mode_e_type   device_mode;
}rrc_ds_status_change_ind_type;

typedef struct
{
  sys_modem_as_id_e_type as_id;
}rrc_subscription_id_type;

typedef struct 
{
  boolean  tuneaway_status; /* FALSE indicate disable tuneaway and TRUE indicates enable tuneway*/
} rrc_ds_tuneaway_status_change_ind_type;


typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;

  sys_modem_as_id_e_type  as_id;
}rrc_suitable_search_end_ind_type;

typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;

  /* TRUE:    no periodic BPLMN search
     FALSE:   enable periodic BPLMN search */
  boolean         disable_status;

  sys_modem_as_id_e_type  as_id;
}rrc_disable_bplmn_ind_type;

typedef struct
{
  boolean csfb_call_status; /*TRUE if call is activated, FALSE if call is cancelled*/
  sys_modem_as_id_e_type  as_id;
} rrc_csfb_call_status_ind_type;

typedef struct
{

  boolean lte_reselection_allowed; /*TRUE if reselection is allowed, FALSE if reselection is not allowed*/
  sys_modem_as_id_e_type  as_id;
} rrc_lte_reselection_allowed_status_ind_type;

typedef struct
{
  /* Message header needed by MM */
  IMH_T           message_header;

  sys_modem_as_id_e_type  as_id;
}rrc_srvcc_handover_failure_ind_type;

typedef struct
{
  sys_drx_cn_coefficient_s1_e_type   drx_coefficient;
  sys_modem_as_id_e_type  as_id;
}rrc_set_drx_ind_type;

#define WCDMA_INVALID_CELL_ID 0xFFFFFFFF
#define WCDMA_INVALID_URA 0xFFFFFFFF

#define WCDMA_MAX_ASET_CELLS 6
#define WCDMA_MAX_MONITORED_FREQ 3
#define WCDMA_MAX_MONITORED_CELLS_PER_FREQ 8
#define WCDMA_MAX_MONITORED_GSM_CELLS 8
#define WCDMA_MAX_MONITORED_LTE_CELLS 16

typedef struct
{
  /*Primary Scrambling Code*/
  uint16 psc;

  /*RSCP*/
  int16 rscp;

  /*EC/Io*/
  int16 ecio;

    /*S_qual */
  int16 squal;

  /*S_rxlev */
  int16 srxlev;
  
  /* Rank of the cell */
  int16 rank;

  /* Set of the cell */
  uint8 set;

} wcdma_cell_info_type;

typedef struct
{

  /*Serving Cell DL UARFCN*/
  uint16 uarfcn;

  /*Serving Cell Primay Scrambling Code*/
  uint16 psc;

  /*PLMN ID*/
  rrc_plmn_identity_type plmn_id;

  /*Indicate the LAC*/
  uint8 lac[RRC_MAX_GSMMAP_LAC_LENGTH];
  
  /*Boolean indicated is RAC is present*/
  boolean is_rac_present;
  /*Indicate the RAC*/
  uint8 rac[RRC_MAX_GSMMAP_RAC_LENGTH];

  /*URA*/
  uint32 ura;

  /* Indicates the service cell ID */
  uint32 cell_id;

  /*Network Operation mode*/
  sys_network_op_mode_e_type nw_operation_mode;
} wcdma_serving_cell_info_type;



typedef struct
{
  /*Serving Cell Primary Scrambling Code*/
  uint16 psc;

  /*RSCP*/
  int16 rscp;

  /*EC/Io*/
  int16 ecio;

    /*RxAGC (in dBm)*/
  int16 rx_agc;

  /*TxAGC (in dBm)*/
  int16 tx_agc;

  /* BLER on DL (unit?)*/
  uint16 dl_bler;
  
  /*S_qual */
  int16 squal;

  /*S_rxlev */
  int16 srxlev;
  
}wcdma_serving_cell_power_info_type;

typedef struct
{
  /*DL UARFCN*/
  uint16 uarfcn;
  /*PSC*/
  uint16 psc;

  /*PLMN ID*/
  rrc_plmn_identity_type plmn_id;

  /*Indicate the LAC*/
  uint8 lac[RRC_MAX_GSMMAP_LAC_LENGTH];
  
  /*Boolean indicated is RAC is present*/
  boolean is_rac_present;
  /*Indicate the RAC*/
  uint8 rac[RRC_MAX_GSMMAP_RAC_LENGTH];

  uint32 cell_id;
} wcdma_active_set_reference_rl_info_type;

typedef struct
{
  /*Number of RLs in active set*/
  uint8 num_rls;
  /*Primary Scrambling codes*/
  uint16 psc[WCDMA_MAX_ASET_CELLS];

  uint32 cell_id[WCDMA_MAX_ASET_CELLS];

  /*RSCP*/
  int16 rscp[WCDMA_MAX_ASET_CELLS];

  /*EC/Io*/
  int16 ecio[WCDMA_MAX_ASET_CELLS];

    /*RxAGC (in dBm)*/
  int16 rx_agc;

  /*TxAGC (in dBm)*/
  int16 tx_agc;

  /* BLER on DL (unit?)*/
  uint16 dl_bler;
  
  /* spreading factor of the channel */
  l1_sf_enum_type sf;

  /* Physical channel type FDPCH/DPCH */
  l1_dl_phychan_enum_type phy_chan_type;

  /* Slot format for the channel */
  uint8 slot_format;

  /*indicates CM On or not*/
  boolean is_compressed_mode_on;
  
} wcdma_active_set_info_type;

typedef struct
{
  /*UARFCN*/
  uint16 uarfcn;

  /*Number of cells in the frequency*/
  uint8 num_of_cells;

  /*Cells info*/
  wcdma_cell_info_type cell[WCDMA_MAX_MONITORED_CELLS_PER_FREQ];
} wcdma_monitored_freq_info_type;

typedef struct
{
  /*ARFCN*/
  uint16 arfcn;      /* Range 0-1023 */
  /*BSIC-NCC*/
  uint8 bsic_ncc;    /* Range 0-7, 0XFF is invalid */
  /*BSIC-BCC*/
  uint8 bsic_bcc;    /* Range 0-7, 0XFF is invalid */
  /*RSSI*/
  int16 rssi;

  /*Srxlev of the cell */
  int16 srxlev;

  /* Rank of the cell */
  int16 rank;
} wcdma_gsm_cell_info_type;

typedef struct
{
  /*EARFCN*/
  uint32 earfcn;      

  /* The physical cell id of the detected cell */
  uint16 cell_id;
  /* Max RSRP comb.value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrp;
  /* Max RSRQ value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrq;
  /* LTE cell rxlev */
  int16  s_rxlev;

  boolean is_tdd_freq; /* is this a LTE TDD cell */
} wcdma_lte_cell_info_type;

typedef struct
{
  /*Number of frequencies*/
  uint8 num_of_freqs;

  /*Freq info*/
  wcdma_monitored_freq_info_type freq[WCDMA_MAX_MONITORED_FREQ];

  /*Number of GSM cells*/
  uint8 num_gsm_cells;  /* Range 0-WCDMA_MAX_MONITORED_GSM_CELLS */

  /*GSM cell info*/
  wcdma_gsm_cell_info_type gsm_cell[WCDMA_MAX_MONITORED_GSM_CELLS];

  /*Number of LTE cells */
  uint8 num_lte_cells;  /* Range 0-WCDMA_MAX_MONITORED_LTE_CELLS */

  /* LTE cell info */
  wcdma_lte_cell_info_type lte_cell[WCDMA_MAX_MONITORED_LTE_CELLS];

} wcdma_monitored_set_info_type;


typedef struct
{
  wcdma_serving_cell_info_type wcdma_serving_cell_info;
  wcdma_serving_cell_power_info_type wcdma_serving_cell_power_info;
  wcdma_active_set_reference_rl_info_type  wcdma_active_set_reference_rl_info;
  wcdma_active_set_info_type wcdma_active_set_info;
  wcdma_monitored_set_info_type wcdma_monitored_set_info;
  sys_wcdma_rrc_state_e_type  wcdma_rrc_state;
} wcdma_event_info_type;


typedef struct
{
  /*Bitmask*/
  uint16 bitmask;

  wcdma_event_info_type wcdma_event_info;
} wcdma_UI_event_info_type;

/***********************************************************************/
/************************ SYS_MODE_CODEC_HO_INFO ************************/
/***********************************************************************/

/*Indicates WCDMA and INTER-RAT voice handover progress and the current codec being used*/
  
/* Callback function type for CODEC info and ho type notification */
typedef 
void RRC_CODEC_HO_INFO_CB_FUNC_TYPE(sys_codec_ho_info eng_mode_codec_ho_info);

/*===========================================================================

FUNCTION rrc_register_rrc_codec_ho_info_cb

DESCRIPTION
  This function is the API to register the function pointer for CM call back .

DEPENDENCIES


RETURN VALUE
  log_message_type

SIDE EFFECTS
  None
===========================================================================*/
void rrc_register_rrc_codec_ho_info_cb
(
  RRC_CODEC_HO_INFO_CB_FUNC_TYPE *cb_ptr
);
/*===========================================================================

FUNCTION rrc_deregister_rrc_codec_ho_info_cb

DESCRIPTION
  This function is the API to de-register the function pointer for CM call back.

DEPENDENCIES


RETURN VALUE
  log_message_type

SIDE EFFECTS
  None
===========================================================================*/
void rrc_deregister_rrc_codec_ho_info_cb (void);
/*===========================================================================

FUNCTION WCDMA_SET_MODEM_STATS_MASK 

DESCRIPTION
  This function updates Events bitmask stored at RRC

DEPENDENCIES
  None

RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/
void wcdma_set_modem_stats_mask (uint16 bit_mask);

/*===========================================================================

FUNCTION WCDMA_GET_MODEM_STATS_INFO 

DESCRIPTION
  This function copies the WCDMA event fields to the stucture provided as arguments

DEPENDENCIES
  None

RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/
void wcdma_get_modem_stats_info (wcdma_UI_event_info_type *wcdma_info);

/*===========================================================================

FUNCTION MM_RRC_PLMN_LAI_ALLOWED

DESCRIPTION
  This function checks if the specified PLMN/LAI is alowed for CS or PS registration.
  This function checks if PLMN capability is matching with UE capability and
  the service domain is allowed (not barred) for registration or not.
  Moreover, in AUTOMATIC mode registration is allowed if LAI/PLMN is not in forbidden list.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if either CS or PS registration is possible/allowed in this LAI/PLMN.
  FALSE: Otherwise.


SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_rrc_plmn_lai_allowed(rrc_service_info_type service_info);
/*===========================================================================

FUNCTION rrc_malloc_for_external_cmd

DESCRIPTION
  This function is called by the external modules like NAS in order to allocate 
  dynamic memory. This function simply calls mem_malloc and selects suitable
  heap and returs pointer to the allocated memory.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

===========================================================================*/
extern void * rrc_malloc_for_external_cmd
(
size_t size
);


/*===========================================================================

FUNCTION MM_RRC_REGISTRATION_IN_PROGRESS

DESCRIPTION
  This function returns registration status
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE : registration in progress
  FALSE: registration in progress


SIDE EFFECTS
  This function shall be called only iff LAI is allowed according to mm_rrc_plmn_lai_allowed

===========================================================================*/

boolean mm_rrc_registration_in_progress(void);

/*===========================================================================

FUNCTION RRCC_REGISTER_SERVING_CELL_CHANGE_IND_CB

DESCRIPTION

This function enables upper layer to register a callback function if notification
of the serving cell id is required.  Once the function is registered, it will be 
called whenever there is a serving cell change.


DEPENDENCIES

  None.
  
RETURN VALUE
  
  BOOLEAN:  TRUE signifies that registration has succeeded.  FALSE signifies
            that registration failed.

SIDE EFFECTS

  None.

===========================================================================*/
extern boolean rrc_register_serving_cell_change_ind_cb 
(
  RRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE* cb_ptr
);


/*===========================================================================

FUNCTION RRC_DEREGISTER_SERCING_CELL_CHANGE_IND_CB

DESCRIPTION

This function enables upper layer to deregister the callback function that was
registered for notification of serving cell id.


DEPENDENCIES

  None.
  
RETURN VALUE
  
  BOOLEAN:  TRUE signifies that deregistration has succeeded.  FALSE signifies
            that deregistration failed.

SIDE EFFECTS

  None.

===========================================================================*/
extern boolean rrc_deregister_serving_cell_change_ind_cb 
(
  RRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr
);


/*===========================================================================

FUNCTION MM_RRC_PER_SUBS_PLMN_LAI_ALLOWED

DESCRIPTION
  This function checks if the specified PLMN/LAI is alowed for CS or PS registration.
  This function checks if PLMN capability is matching with UE capability and
  the service domain is allowed (not barred) for registration or not.
  Moreover, in AUTOMATIC mode registration is allowed if LAI/PLMN is not in forbidden list.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if either CS or PS registration is possible/allowed in this LAI/PLMN.
  FALSE: Otherwise.


SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_rrc_per_subs_plmn_lai_allowed(rrc_service_info_type service_info, sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION MM_PER_SUBS_RRC_REGISTRATION_IN_PROGRESS

DESCRIPTION
  This function returns registration status
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE : registration in progress
  FALSE: registration in progress


SIDE EFFECTS
  This function shall be called only iff LAI is allowed according to mm_rrc_plmn_lai_allowed

===========================================================================*/

boolean mm_per_subs_rrc_registration_in_progress(sys_modem_as_id_e_type as_id);

/*-------------------------------------------------------------------
 ENUM: rrc_fast_dormancy_req_cause_e_type
 This enumerates the fast dormancy request cause from NAS to RRC 
--------------------------------------------------------------------*/
typedef enum
{
  /* NAS is requesting for fast dormancy trigger to RRC*/
  RRC_FAST_DORMANCY_INIT
}rrc_fast_dormancy_req_cause_e_type;

/*-------------------------------------------------------------------
MM to RRC: RRC_FAST_DORMANCY_REQ
Used by MM to request fast dormancy request processing to RRC
--------------------------------------------------------------------*/
typedef struct
{
  /* request cause for RRC_FAST_DORMANCY _REQ.*/
  rrc_fast_dormancy_req_cause_e_type cause;
  sys_modem_as_id_e_type as_id;
}rrc_fast_dormancy_req_type;

/*-------------------------------------------------------------------
 ENUM: rrc_fast_dormancy_cnf_status_e_type
 This enumerates the status of a fast dormancy Confirm from RRC
--------------------------------------------------------------------*/
typedef enum
{
  /* RRC sent SCRI which also close PS domain.
   * No need for application to further retry
  */
  RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY,
 
  /* RRC sent SCRI to indicate that UE has finished sending data 
   * to N/W in uplink with new cause for fast dormancy. So NAS-RRC not to 
   * close PS domain when this cause is used, application need to further retry. 
  */
  RRC_FAST_DORMANCY_SUCCESS_APP_RETRY,
 
  /* RRC can't send SCRI for fast dormancy, because of reasons like 
   * CS domain is also open, UE not campped,RRC is in a state where 
   * it cant process the command etc. Application need to further retry.
  */
  RRC_FAST_DORMANCY_FAILURE_APP_RETRY
}rrc_fast_dormancy_cnf_status_e_type;

/*-------------------------------------------------------------------
RRC to MM: RRC_FAST_DORMANCY_CNF
Used by RRC in response to RRC_FAST_DORMANCY _REQ.
--------------------------------------------------------------------*/
typedef struct
{
  /* Message header needed by MM */
  IMH_T			message_header;
  
  /* status of RRC_FAST_DORMANCY_REQ.*/
  rrc_fast_dormancy_cnf_status_e_type status;
  sys_modem_as_id_e_type as_id;
}rrc_fast_dormancy_cnf_type;

/*===========================================================================

FUNCTION          rrc_is_ps_abort_required

DESCRIPTION       This function is to be called by CM/SD to check if PS abort can be attemped on W
                         or G, for CS call on the other RAT.
                         1. In PCH/Connected Mode OOS states do not allow PS Abort
                         2. In FACH/DCH state allow PS abort.
                         3. If W is INACTIVE or PS domain in W is not active, then this API returns TRUE to 
                             allow PS abort on G.

DEPENDENCIES      None

RETURN VALUE      TRUE: To allow PS abort. and FALSE otherwise.

SIDE EFFECTS      None
===========================================================================*/
boolean rrc_is_ps_abort_required(void);

/*===========================================================================

FUNCTION rrcrcr_is_proc_active_sub

DESCRIPTION
  Indicates whether RCR is active or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if RCR is active.
  FALSE if RCR is not active.

SIDE EFFECTS
  None
===========================================================================*/

boolean rrcrcr_is_proc_active_sub(sys_modem_as_id_e_type as_id);

/*-------------------------------------------------------------------
MM to RRC Request: RRC_TUNEAWAY_ALLOWED_IND
Used by SM/MM to indicate tuneaway status
--------------------------------------------------------------------*/
typedef struct
{
  boolean                 sm_ok_to_tuneaway;

  sys_modem_as_id_e_type  as_id;
}rrc_sm_tuneaway_allowed_ind_type;

/*-------------------------------------------------------------------
MM to RRC Indication: RRC_LAI_REJECT_LIST_IND
Used by MM to indicate LAI Reject list ind.
--------------------------------------------------------------------*/
typedef struct
{
  lai_reject_list_T       nas_lai_reject_list;

  sys_modem_as_id_e_type  as_id;
}rrc_lai_reject_list_ind_type;

/*-------------------------------------------------------------------
MM to RRC Indication: RRC_CSG_WHITE_LIST_UPDATE_IND
Used by MM for whitelist update indication
--------------------------------------------------------------------*/
typedef struct
{
  sys_csg_id_type    csg_id;  
  sys_plmn_id_s_type plmn_id;
}rrc_csg_info_type;

typedef struct
{
  rrc_csg_info_type    csg_list[MAX_NO_CSG_LIST];
  uint8                length;
  sys_modem_as_id_e_type as_id;
}rrc_csg_white_list_type;

/*===========================================================================

FUNCTION RRC_SET_UE_IN_DEEP_SLEEP

DESCRIPTION

  This function is to be called by CM to inform RRC that UE is going to DEEP SLEEP
  state.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
extern void rrc_set_ue_in_deep_sleep(boolean value);

/*===========================================================================
FUNCTION rrc_reset_mcm_irat_resel_act_cause

DESCRIPTION
  Used by NAS to reset activation cause in case registration is 
  not going to be triggered by NAS immediately after IRAT reselection 
  due to ISR being active at NAS. NAS might call this after every service indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrc_reset_mcm_irat_resel_act_cause
(
  void
);
/*******************************************************************************
 *
 *  Function name:   ghdi_get_sampling_rate()
 *  --------------
 *
 *  Description:
 *  ------------
 *    RRC/TDSRRC/RR can use this API to get the sampling rate given by MVS for current codec.
 *    MVS can give sampling rate of 16kHz when NB codec is used.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns: sampling rate stored in GHDI cache.
 *  --------
 *
 ******************************************************************************/

extern uint32  ghdi_get_sampling_rate(void);


/*******************************************************************************
 *
 *  Function name:   ghdi_reset_sampling_rate()
 *  --------------
 *
 *  Description:
 *  ------------
 *    RRC/TDSRRC/RR/NAS can use this API to reset the sampling rate in GHDI cache.
 *    NAS does it at call initiation and call clearing (when no other call is in progress).
 *    AS does it when codec changes to a non narrow band type.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns: None
 *  --------
 *
 ******************************************************************************/
extern void    ghdi_reset_sampling_rate(void);

#endif
