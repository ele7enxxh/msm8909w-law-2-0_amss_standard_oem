/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     GMM<->LLC Interface (GMMLLCM.C)

GENERAL DESCRIPTION
  This file contains an implementation of the GMM<->LLC Interface module

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmmllcm.c_v   1.5   18 Jul 2002 11:42:44   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmmllcm.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/12   am      Back off timer code
12/21/11   gps     FREE FLOAT FEATURE changes           
10/10/11   gps     Initiated some primitives with all zeros
05/20/08   jbk     Stopping ready timer as we dont send TLLI unassignment 
                   anymore to LLC while leaving G RAT 
06/13/06   ss      NAS Events in GMM(GSM side)
03/22/06   vdr     Resume LLC when evnt IMSI_DETACH_FINISHED is received.
15/02/06   vdr     Now suspending LLC only when GMM moves into Limited Service 
                   and TLLI has not been unassigned
01/18/06   hj      Now sending PCCCH_CAMP_REQ if attached for PS services upon
                   receiving PLMN_SELECT_CNF.
12/23/05   vdr     Now guarding transmission of the DETACH ACCEPT message by 
                   suspension of LLC
12/14/05   abe     Supported changes for FEATURE_GSM_DTM
12/05/05   vdr     Now sending GRR service_cnf upon reception of LLC Suspension
                   confirmation if service confirmation has not been sent earlier
                   due to the GMMLLC module internal state
10/05/05   ajt     Send Authentication & Ciphering Failure Msg Unciphered in GSMImaginary Buffer Line
09/21/05   vdr     Added processing of the new GMM events GMM_TRIGGER_LLC_SUSPENSION
                   and GMM_TRIGGER_LLC_RESUMPTION
09/15/05   npr     Move the sending of service_cnf into the processing of pending
                   GMM commands, to avoid spurious service_cnf sending.
09/15/05   jca     Reassign TLLI upon reception of RR_PLMN_SELECT_CNF if GMM
                   is in REGISTERED state.
08/24/05   hj      Modified handling of PCCCH_CAMP_CNF in accordance with changelist 226764,
                   also updated handling of rr_process_service_ind to not trigger CELL UPDATE
                   on a cell with forbidden LAI.
08/14/05   hj      Added handling for monitoring CCCH paging channel when detached from
                   GPRS Services.
08/09/05   vdr     Reverted GMM state conditions when sending RESUME request to LLC
                   in order to complete Cell Update/Notification
08/08/05   vdr     Added processing of the new GMM event GMM_TRIGGER_DETACH_STARTED
08/08/05   vdr     Removed GMM state conditions when sending SERVICE_CNF to GRR
                   Removed GMM state conditions when sending RESUME request to LLC
                   in order to complete Cell Update/Notification
06/17/05   vdr     Added GMM_TRIGGER_TLLI_UNASSIGNMENT
06/13/05   jca     Now GMM sends LLC resume request when GMM substate is currently
                   set to GMM_ATTEMPTING_TO_UPDATE_MM.
04/21/05   vdr     Now sending Cell Update upon Cell Reselection during RAU Procedure
12/17/04   vdr     o Changed gmmllc_reinit() in order to send
                     RR_GMM_GPRS_STATE_CHANGE_REQ to RR unconditionally
                   o Now assigning FOREIGN TLLI if RAI is changed or
                     gmm_update_status is not equal to GMM_GU1_UPDATED
11/18/04   vdr     Now preventing RR_SERVICE_IND triggering GPRS ATTACH or an
                   RAU Requests to be treated as a cell reselection indicator
                   initiating cell update
10/25/04   vdr     Removed transition to STANDBY state upon activation of "force to stanby"
                   Corrected RR_SERVICE_IND handling.
10/06/04   npr     Corrected Force to Standby handling. GLIF state now forced to
                   STANDBY state to prevent Cell Updates under these conditions.
09/10/04   npr     Reverse previous timer handling edit. Remove attach request
                   msg buffering as unnecessary, as the ordering of ATTACH_STARTED
                   and Attach Request message have been swapped in gmm_reg.c.
                   General Revamp.
09/05/04   jca     Pass T3311/T3302 timer events through GRR event handler so that
                   queued ATTACH REQUEST message will be txed on timer expiration.09/02/04   npr     Handle ATTACH_STARTED trigger in GMM Standby and GMM Ready
                   states also.
08/25/04   npr     Buffer Attach Request until service_ind or resumption_ind.
                   Now handle new message RR_RESELECTION_IND from GRR, and
                   store ready timer state. This permits Cell Update to be sent
                   even if ready timer has expired by the time of service_ind.
                   Only notifiy ciphering algorithm if CKSN is not deleted.
08/11/04   ks      Fixed the Random TLLI offset, Sending the LLC suspend and
                   SERVICE_CNF to GRR only after TLLI has been assigned.
07/21/04   vdr     Now sending Cell Update/Notification only when current cell
                   is not barred (MM_AS_ACCESS_NORMAL_ONLY, MM_AS_ACCESS_ALL_CALLS),
                   otherwise GRR Service Confirm is to be sent
06/23/04   ks      Making sure glif_llc_connection_state is LLC_SUSPENSION_CONFIRMED
                   before sending a cell update procedure when RAU in progress.
05/25/04   vdr     Now sending Cell Update/Notification when cell is changed
                   during RAU attempt
03/11/04   ks      Resetting the glif_rau_in_progress when detached, also added
                   revision level check during Cell update procedure.
02/19/04   vdr     o Added processing required by READY timer negotiation procedure
                   o Now sending RR_GMM_GPRS_SERVICE_CNF upon RAU initiation if
                     LLC connection is in LLC_SUSPENSION_CONFIRMED state
                   o Added new function providing interface to assign TLLI
01/06/04   jca     Do not transition to READY state when GLLCMM_LLGMM_TRIGGER_IND
                   received if GPRS service is not available.
12/18/03   vdr     Removed recording of the RAI in which we last successfully updated
10/13/03   jca     Moved glif_exit_mm_non_drx_mode() function calls to
                   gmm_set_state() function to ensure calls to reenable DRX
                   mode are made on ALL valid designated GMM state transitions.
10/09/03   jca     Now check whether GMM is in the proper state/substate before
                   resuming PS data services at completion of GPRS Attach or
                   RA Update procedure.
09/30/03   npr     Notification of MM Non-DRX and RA CAP IE to GRR is now
                   RAU request message driven rather than state driven.
07/24/03   vdr     Fixed processing of the RR_GMM_GPRS_RESUMPTION_IND in READY
                   state
07/18/03   vdr     Added processing of the RR_GMM_GPRS_RESUMPTION_IND
07/09/03   vdr     Fixed TLLI management in order to assign FOREIGN TLLI when
                   RA boundary is crossed for the first time.
                   Maid some optimisations related to RAU procedure handling
07/08/03   jca     Changed ERR message to MSG_HIGH.
06/24/03   vdr     Now GMM sends LLC resume request to LLC when it goes to IDLE
                   state
06/19/03   vdr     Added state variable to track LLC link status
                   Now Suspention/Resumption mechanism is driven by GMM events
                   according to the LLC link state
06/05/03   vdr     Now GMM DETACH REQUEST message processing is guarded by
                   suspension/resumption procedures
05/23/03   jca     Changed MSG_ERRORs to MSG_HIGHs.
05/16/03   cd      Now initializing old and new tlli to 0xffffffff in gmmllc_init()
05/15/03   vdr     Now invalidation of an OLD TLLI is performed only upon
                   reception of the LLC primitive which was triggered upon
                   reception of the OTA LLC message
05/15/03   vdr     o Now resetting LLC & GRR state machines by setting new TLLI
                     equal to 0xFFFFFFFF upon sending GMM ATTACH REQUEST
                   o Fixed P-TMSI (TLLI) exchange procedure when new P-TMSI
                     has been assigned
05/12/03   vdr     Now notifying to GRR that GPRS GMM state has been changed
                   during W2G handover or cell reselection
04/30/03   vdr     Now TLLI assignment is controlled by glif_force_local_tlli
                   when GMM sends RAU request message
03/27/03   vdr     Fixed gmmllc_reinit()
03/21/03   vdr     Fixed compilation error
03/18/03   vdr     Featured RAU Request message processing
03/14/03   vdr     Fixed processing RAU Request and Cell Update/Notification
03/07/03   vdr     Fixed processing of the RAU Request message
03/07/03   vdr     Relocated message transmition mechanism and fixed page response
                   processing during UMTS to GSM RAT change
03/04/03   vdr     Added processing of the GLLCMM_LLGMM_SUSPEND_CNF primitive
                   Now Cell Update/Cell Notification is guarded by sending
                   LLC Suspend/Resume request
02/27/03   vdr     Added pocessing required by UMTS to GSM RAT change
02/10/03   cd      Fixed lint errors
01/22/03   vdr     o Added processing of the GLLCSMS_LL_XID_IND primitive
                   o Added de-multiplexing of the GLLCMM_LL_UNITDATA_IND and
                     GLLCSMS_LL_UNITDATA_IND primitives into one RRC primitive
                   o Fixed specifing the priority level of the outgoing SMS messages
                   o Fixed determination of whether to request Cell Notify or Cell Update
                     procedure
12/02/02   vdr     o TLLI handling has been corrected
                   o MS RA CAP IE Notification to GRR now triggered off the sending of an
                     attach request or routing area update request rarther than TLLI assignment.
                   o F3 messaging cleaned up to give a better indication of what's going on
                     within the limited string length of QXDM.
                   o Unexpected message handling corrected.
                   o Idle state handler added for LLC messages in that state.
11/20/02   vdr     Added Non-DRX Mode Support
                   Added MS Radio Access Capabilities IE inquiry
11/18/02   ks      Added processing for MM_RR_WCDMA_GSM_HO sync reason processing in
                   glif_grrif_ready_state_handler().
11/14/02   ks      Replaced MSCR with SGSNR for GPRS Revision info.
11/08/02   cd      In send_rr_ready_timer_state_request() and glif_grr_state_exchange()
                   corrected setting the message length in the header
09/11/02   vdr     Fixed RAU timer management
09/10/02   vdr     Added the following features:
                   o Handling of the Periodic RAU timer
                   o New GMM event GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE
                   o Cell Notification
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
07/15/02   vdr     Fixed criteria to request ciphering of the current GMM
                   outgoing message
03/14/02   vdr     Initial version
12/06/11   gps     sending RAU request as a page response
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include <stringl/stringl.h>
#ifdef FEATURE_GSM_GPRS

#include <stdlib.h>

#include "err.h"

#include "gllclient.h"

#include "gllc.h"
#include "gllc_v.h"

#include "gmm_int_v.h"

#include "gmm_msg.h"

#include "mmauth.h"

#include "smtask.h"
#include "smtask_v.h"

#include "mm_rr.h"

#include "gmmllcm.h"

#include "sys_cnst_v.h"

#include "reg_sim_v.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "gmmsuspend.h"

#include "mm_coord.h"
#ifdef FEATURE_LTE
#include "emm.h"
#endif
#include "mm_sim.h"

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_GSM_GPRS)
#include "geran_multi_sim_api.h" 
#endif /* FEATURE_DUAL_SIM && FEATURE_GSM_GPRS */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#ifdef FEATURE_DUAL_DATA
glif_tlli_type tlli_gmmllcm_sim[MAX_AS_IDS];
#define tlli_gmmllcm tlli_gmmllcm_sim[mm_as_id]
#else
glif_tlli_type tlli_gmmllcm;
#endif


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define GLIF_DETACH_TIMER_VALUE 4000


#if (!defined(FEATURE_WTOG_PS_HANDOVER) && !defined(FEATURE_GTOW_PS_HANDOVER))
extern gmm_ie_LV_ms_radio_access_caps_T get_ms_radio_access_capability
(
  boolean gprs_pkt_res_req_ms_ra_cap
);
#endif

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
extern void gmm_handle_gprs_detach_low_level_failure( void );
#endif


void glif_grr_send_service_req( void );
void glif_process_pended_gmm_commands( void );
extern void mm_log_mm_msg_info(byte pd,byte msg_id);


/* TLLI Format Identifiers */
typedef enum glif_tlli_id_tag
{
  GLIF_FOREIGN_TLLI_TYPE,
  GLIF_LOCAL_TLLI_TYPE,
  GLIF_DISABLE_TLLI_TYPE
} glif_tlli_id_type ;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 

extern   auth_gsm_cipherkey_type   gsm_gprs_kc_128bit_sim[MAX_NAS_STACKS] ;
/* GMM<->LLC Interface module State variable */
static gmm_llc_if_state_type glif_state_sim[MAX_NAS_STACKS] ;

/* Old TLLI variable */
static glif_tlli_type glif_old_tlli_sim[MAX_NAS_STACKS] = { {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF} } ;

/* Current TLLI variable */
glif_tlli_type glif_tlli_sim[MAX_NAS_STACKS] = { {0xFF, 0xFF, 0xFF, 0xFF }, {0xFF, 0xFF, 0xFF, 0xFF } } ;

/* Internal TLLI Xlater for use with messaging for LLC, GRR and SM */
#define GLIF_XLATE_TLLI(x) \
  (uint32)(((x)[0] << 24) | ((x)[1] << 16) | ((x)[2] <<  8) | ((x)[3] <<  0))

/* TLLI type extractor */
#define GLIF_GET_TLLI_TYPE(x) (((x >> 24) & 0xC0) >> 6)

/*
** TLLI comparator. This macro returns TRUE is both TLLIs are equal and FALSE
** otherwise.X is an array of byte containing first TLLI, Y is an uint32
** containing another one
*/
#define GLIF_TLLI_CMP(x,y) (GLIF_XLATE_TLLI(x) == y)

/*
** ATTACH indicator.
*/
static boolean glif_attach_in_progress_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** DETACH indicator.
*/
static boolean glif_detach_in_progress_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** RAU indicator. This indicator blocks the Cell Update procedure when needed.
** The Cell Update Procedure is idle when the variable indicates
** RAU-In-Progress (glif_rau_in_progress == TRUE).
*/
static boolean glif_rau_in_progress_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** The Force-To-Standby indicator is used in order to prevent processing of
** the Cell Update procedure even the GMM-LLC interface module is in GPRS
** READY state
*/
static boolean glif_force_to_standby_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** Current value of READY timer in ms
** The default value is taken from
** TS 24.008 Table 11.3a
*/
dword glif_ready_timer_value_sim[MAX_NAS_STACKS] = { 44000, 44000 } ;

static boolean glif_ready_timer_deactivated_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** An appropriately coded MS Radio Access Capability IE is required by the
** GPRS Radio Resource management layer to permit it's inclusion by MAC in
** Packet Resource Request messages. This form of the IE is constructed
** with no spare bits or padding.
*/

static gmm_ie_LV_ms_radio_access_caps_T  rr_gmm_ms_radio_access_cap_ie_sim[MAX_NAS_STACKS];

/*
** The purpose of the radio priority information element is to specify
** the priority level that the MS shall use at the lower layers for
** transmission of data related to a PDP context or for mobile originated
** SMS transmission.
*/
byte    sms_radio_priority_sim[MAX_NAS_STACKS] ;

/* The purpose of the Cell Notification Indicator is to indicate that
** the Cell Notification is supported by the network and shall be
** used by MS
*/
boolean cell_notification_ind_sim[MAX_NAS_STACKS] ;

extern  auth_gsm_cipherkey_type     gsm_gprs_kc_128bit_sim[MAX_NAS_STACKS] ;

/* Buffer for Pended GMM Message */
#ifdef FEATURE_MODEM_HEAP 
static byte* glif_pended_gmm_msg_sim[MAX_NAS_STACKS]={NULL, NULL};
#else
static byte glif_pended_gmm_msg_sim[MAX_NAS_STACKS][MAX_MESSAGE_SIZE] ;
#endif

/* Pended message length */
static dword glif_buffered_message_length_sim[MAX_NAS_STACKS] ;

/* Buffer for Pended DETACH Message */
#ifdef FEATURE_MODEM_HEAP 
static byte* glif_pended_detach_sim[MAX_NAS_STACKS]={NULL, NULL};
#else
static byte glif_pended_detach_sim[MAX_NAS_STACKS][MAX_MESSAGE_SIZE] ;
#endif

/* Pended DETACH message length */
static dword glif_buffered_detach_length_sim[MAX_NAS_STACKS] ;

/* Foreign TLLI indicator */
static boolean glif_foreign_tlli_assigned_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** Cell Update Pended Indicator, it is TRUE when there is pended
** LLC Cell Update which has to be transmitted after
** LLC suspension will be completed
*/
static boolean glif_pended_cell_update_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** Initial Cell Update Pended Indicator, it is TRUE when there is an initial
** LLC Cell Update pended (i.e. cell notification with non-NULL Frame)
** which has to be transmitted after
** LLC suspension will be completed
*/
static boolean glif_pended_cell_update_is_initial_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** Attach Pended Indicator, it is TRUE when there is pended
** Attach Request message which has to be transmitted after
** GRR camping for Packet services will be completed
*/
static boolean glif_pended_attach_req_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** RAU Pended Indicator, it is TRUE when there is pended
** RAU Request message which has to be transmitted after
** LLC suspension will be completed
*/
static boolean glif_pended_rau_req_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/*
** Detach Pended Indicator, it is TRUE when there is pended
** Detach Request which has to be transmitted after
** LLC suspension will be completed
*/
static boolean glif_pended_detach_req_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

#ifdef FEATURE_FORCE_LOCAL_TLLI_RAU
       boolean glif_force_local_tlli_sim[MAX_NAS_STACKS] = { TRUE, TRUE } ;
#else
       boolean glif_force_local_tlli_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
#endif /* #ifndef FEATURE_FORCE_LOCAL_TLLI_RAU */

/* GRR Camp state variable */
glif_grr_camp_state_type glif_grr_camp_state_sim[MAX_NAS_STACKS] = { GLIF_CAMP_ON_CCCH, GLIF_CAMP_ON_CCCH } ;

/* GRR Service req state variable */
static glif_grr_ser_req_state_type glif_grr_ser_req_state_sim[MAX_NAS_STACKS] = { GLIF_NO_SERVICE, GLIF_NO_SERVICE } ;

/* LLC connection state variable */
static glif_llc_connection_state_type glif_llc_connection_state_sim[MAX_NAS_STACKS] = { LLC_SUSPENSION_CONFIRMED, LLC_SUSPENSION_CONFIRMED } ;

routing_area_id_T         glif_ra_changed_rau_completed_ra_sim[MAX_NAS_STACKS] ;

/* Ready Timer state flags */
static boolean glif_rdy_tmr_active_sim[MAX_NAS_STACKS]                = { FALSE, FALSE } ;
static boolean glif_rdy_tmr_active_at_reselection_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/* GMM message indicator */
static boolean glif_sending_gmm_message_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/* Service Confirm pended indicator */
#ifndef FEATURE_NAS_FREE_FLOAT
static boolean glif_service_confirm_pended_ind_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
#endif
/* Service Request pended indicator */
static boolean glif_service_req_pended_ind_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
/* MT Detach Accept buffer length */
#define GLIF_BUFFERED_DETACH_ACCEPT_LENGTH 2

/* Pended MT Detach Accept indicator */
static boolean glif_pended_mt_detach_accept_ind_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;

/* MT Detach Accept buffer */
static byte glif_pended_mt_detach_accept_sim[MAX_NAS_STACKS][GLIF_BUFFERED_DETACH_ACCEPT_LENGTH] ;



#define glif_state (glif_state_sim[mm_as_id])
#define glif_old_tlli (glif_old_tlli_sim[mm_as_id])
#define glif_attach_in_progress (glif_attach_in_progress_sim[mm_as_id])
#define glif_detach_in_progress (glif_detach_in_progress_sim[mm_as_id])
#define glif_rau_in_progress (glif_rau_in_progress_sim[mm_as_id])
#define glif_force_to_standby (glif_force_to_standby_sim[mm_as_id])
#define glif_ready_timer_value (glif_ready_timer_value_sim[mm_as_id])

#define glif_ready_timer_deactivated (glif_ready_timer_deactivated_sim[mm_as_id])

#define rr_gmm_ms_radio_access_cap_ie (rr_gmm_ms_radio_access_cap_ie_sim[mm_as_id])
#define glif_pended_gmm_msg (glif_pended_gmm_msg_sim[mm_as_id])
#define glif_buffered_message_length (glif_buffered_message_length_sim[mm_as_id])
#define glif_pended_detach (glif_pended_detach_sim[mm_as_id])
#define glif_buffered_detach_length (glif_buffered_detach_length_sim[mm_as_id])
#define glif_foreign_tlli_assigned (glif_foreign_tlli_assigned_sim[mm_as_id])
#define glif_pended_cell_update (glif_pended_cell_update_sim[mm_as_id])
#define glif_pended_cell_update_is_initial (glif_pended_cell_update_is_initial_sim[mm_as_id])
#define glif_pended_attach_req (glif_pended_attach_req_sim[mm_as_id])
#define glif_pended_rau_req (glif_pended_rau_req_sim[mm_as_id])
#define glif_pended_detach_req (glif_pended_detach_req_sim[mm_as_id])

#define glif_llc_connection_state (glif_llc_connection_state_sim[mm_as_id])

#define glif_grr_ser_req_state (glif_grr_ser_req_state_sim[mm_as_id])
#define glif_rdy_tmr_active (glif_rdy_tmr_active_sim[mm_as_id])
#define glif_rdy_tmr_active_at_reselection (glif_rdy_tmr_active_at_reselection_sim[mm_as_id])
#define glif_sending_gmm_message (glif_sending_gmm_message_sim[mm_as_id])
#ifndef FEATURE_NAS_FREE_FLOAT
#define glif_service_confirm_pended_ind (glif_service_confirm_pended_ind_sim[mm_as_id])
#endif
#define glif_service_req_pended_ind (glif_service_req_pended_ind_sim[mm_as_id])
#define glif_pended_mt_detach_accept_ind (glif_pended_mt_detach_accept_ind_sim[mm_as_id])
#define glif_pended_mt_detach_accept (glif_pended_mt_detach_accept_sim[mm_as_id])

extern ue_rev_level_indicator_type get_revision_level_indicator_per_nas_stacks
(
  sys_modem_as_id_e_type as_id 
);
#define get_revision_level_indicator() get_revision_level_indicator_per_nas_stacks((sys_modem_as_id_e_type)mm_as_id)


#else /* FEATURE_DUAL_SIM*/
extern  auth_gsm_cipherkey_type     gsm_gprs_kc_bit128 ;
/* GMM<->LLC Interface module State variable */
static gmm_llc_if_state_type glif_state ;

/* Old TLLI variable */
static glif_tlli_type glif_old_tlli = {0xFF, 0xFF, 0xFF, 0xFF} ;

/* Current TLLI variable */
glif_tlli_type glif_tlli = {0xFF, 0xFF, 0xFF, 0xFF } ;

/* Internal TLLI Xlater for use with messaging for LLC, GRR and SM */
#define GLIF_XLATE_TLLI(x) \
  (uint32)(((x)[0] << 24) | ((x)[1] << 16) | ((x)[2] <<  8) | ((x)[3] <<  0))

/* TLLI type extractor */
#define GLIF_GET_TLLI_TYPE(x) (((x >> 24) & 0xC0) >> 6)

/*
** TLLI comparator. This macro returns TRUE is both TLLIs are equal and FALSE
** otherwise.X is an array of byte containing first TLLI, Y is an uint32
** containing another one
*/
#define GLIF_TLLI_CMP(x,y) (GLIF_XLATE_TLLI(x) == y)

/*
** ATTACH indicator.
*/
static boolean glif_attach_in_progress = FALSE ;

/*
** DETACH indicator.
*/
static boolean glif_detach_in_progress = FALSE ;

/*
** RAU indicator. This indicator blocks the Cell Update procedure when needed.
** The Cell Update Procedure is idle when the variable indicates
** RAU-In-Progress (glif_rau_in_progress == TRUE).
*/
static boolean glif_rau_in_progress = FALSE ;

/*
** The Force-To-Standby indicator is used in order to prevent processing of
** the Cell Update procedure even the GMM-LLC interface module is in GPRS
** READY state
*/
static boolean glif_force_to_standby = FALSE ;

/*
** Current value of READY timer in ms
** The default value is taken from
** TS 24.008 Table 11.3a
*/
dword glif_ready_timer_value = 44000 ;

static boolean glif_ready_timer_deactivated = FALSE ;



/*
** An appropriately coded MS Radio Access Capability IE is required by the
** GPRS Radio Resource management layer to permit it's inclusion by MAC in
** Packet Resource Request messages. This form of the IE is constructed
** with no spare bits or padding.
*/

static gmm_ie_LV_ms_radio_access_caps_T  rr_gmm_ms_radio_access_cap_ie;

/*
** The purpose of the radio priority information element is to specify
** the priority level that the MS shall use at the lower layers for
** transmission of data related to a PDP context or for mobile originated
** SMS transmission.
*/
byte    sms_radio_priority ;

/* The purpose of the Cell Notification Indicator is to indicate that
** the Cell Notification is supported by the network and shall be
** used by MS
*/
boolean cell_notification_ind ;

/* Buffer for Pended GMM Message */
#ifdef FEATURE_MODEM_HEAP
static byte *glif_pended_gmm_msg = NULL;
#else
static byte glif_pended_gmm_msg[MAX_MESSAGE_SIZE] ;
#endif

/* Pended message length */
static dword glif_buffered_message_length ;

/* Buffer for Pended DETACH Message */
#ifdef FEATURE_MODEM_HEAP
static byte *glif_pended_detach = NULL;
#else
static byte glif_pended_detach[MAX_MESSAGE_SIZE] ;
#endif

/* Pended DETACH message length */
static dword glif_buffered_detach_length ;

/* Foreign TLLI indicator */
static boolean glif_foreign_tlli_assigned = FALSE ;

/*
** Cell Update Pended Indicator, it is TRUE when there is pended
** LLC Cell Update which has to be transmitted after
** LLC suspension will be completed
*/
static boolean glif_pended_cell_update = FALSE ;

/*
** Initial Cell Update Pended Indicator, it is TRUE when there is an initial
** LLC Cell Update pended (i.e. cell notification with non-NULL Frame)
** which has to be transmitted after
** LLC suspension will be completed
*/
static boolean glif_pended_cell_update_is_initial = FALSE ;

/*
** Attach Pended Indicator, it is TRUE when there is pended
** Attach Request message which has to be transmitted after
** GRR camping for Packet services will be completed.
** glif_pended_gmm_msg buffer is used to store pended ATTACH request.
*/
static boolean glif_pended_attach_req = FALSE ;

/*
** RAU Pended Indicator, it is TRUE when there is pended
** RAU Request message which has to be transmitted after
** LLC suspension will be completed.
** glif_pended_gmm_msg buffer is used to store pended RAU request.
*/
static boolean glif_pended_rau_req = FALSE ;

/*
** Detach Pended Indicator, it is TRUE when there is pended
** Detach Request which has to be transmitted after
** LLC suspension will be completed
*/
static boolean glif_pended_detach_req = FALSE ;

#ifdef FEATURE_FORCE_LOCAL_TLLI_RAU
       boolean glif_force_local_tlli = TRUE ;
#else
       boolean glif_force_local_tlli = FALSE ;
#endif /* #ifndef FEATURE_FORCE_LOCAL_TLLI_RAU */

/* GRR Camp state variable */
glif_grr_camp_state_type glif_grr_camp_state = GLIF_CAMP_ON_CCCH ;

/* GRR Service req state variable */
glif_grr_ser_req_state_type glif_grr_ser_req_state = GLIF_NO_SERVICE ;

/* LLC connection state variable */
static glif_llc_connection_state_type glif_llc_connection_state = LLC_SUSPENSION_CONFIRMED ;

routing_area_id_T         glif_ra_changed_rau_completed_ra ;

/* Ready Timer state flags */
static boolean glif_rdy_tmr_active                = FALSE;
static boolean glif_rdy_tmr_active_at_reselection = FALSE;

/* GMM message indicator */
static boolean glif_sending_gmm_message = FALSE ;
/* Service Confirm pended indicator */
#ifndef FEATURE_NAS_FREE_FLOAT
static boolean glif_service_confirm_pended_ind = FALSE ;
#endif
/* Service request pended indicator*/
static boolean glif_service_req_pended_ind = FALSE ;
/* MT Detach Accept buffer length */
#define GLIF_BUFFERED_DETACH_ACCEPT_LENGTH 2

/* Pended MT Detach Accept indicator */
static boolean glif_pended_mt_detach_accept_ind = FALSE ;

/* MT Detach Accept buffer */
static byte glif_pended_mt_detach_accept[GLIF_BUFFERED_DETACH_ACCEPT_LENGTH] ;

extern ue_rev_level_indicator_type get_revision_level_indicator( void );
#endif /* FEATURE_DUAL_SIM or FEATURE_SGLTE*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
extern boolean sys_eplmn_per_nas_stacks_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type      sub_id,
  sys_modem_as_id_e_type      stack_id_nas
);
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,\
                                                                                            (sys_modem_as_id_e_type)mm_as_id)
#else
extern boolean sys_eplmn_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn
);
#endif

/*===========================================================================

FUNCTION      GLIF_LLC_SEND_SUSPEND_REQUEST

DESCRIPTION
  This function builds GMMLLC_LLGMM_SUSPEND_REQ request primitive and
  sends it to the LLC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_llc_send_suspend_request( void )
{
  gmmllc_llgmm_mes_type *glif_suspend_req_ptr ;

  /* Allocate LLC Start READY timer request */
  if ((glif_suspend_req_ptr = gmmllc_llgmm_get_mes_buf()) == NULL)
  {
    MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
  }

  /* Configure GMMLLC_LLGMM_SUSPEND_REQ request primitive */
  glif_suspend_req_ptr->mes.llgmm_suspend_req.prim = GMMLLC_LLGMM_SUSPEND_REQ ;

  /* Initiate TLLI field */
  glif_suspend_req_ptr->mes.llgmm_suspend_req.tlli =GLIF_XLATE_TLLI(glif_tlli);

#ifdef FEATURE_DUAL_SIM
  glif_suspend_req_ptr->mes.llgmm_suspend_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent LLGMM_SUSPEND_REQ to LLC") ;

  mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_suspend_req_ptr->mes.llgmm_suspend_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
       );

  gmmllc_llgmm_put_mes_buf(glif_suspend_req_ptr) ;

  glif_llc_connection_state = LLC_SUSPENSION_REQUESTED ;

} /* end glif_llc_send_suspend_request() */

/*===========================================================================

FUNCTION      GLIF_LLC_SEND_RESUME_REQUEST

DESCRIPTION
  This function builds GMMLLC_LLGMM_RESUME_REQ request primitive and
  sends it to the LLC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_llc_send_resume_request( void )
{
  gmmllc_llgmm_mes_type *glif_resume_req_ptr ;

  /* Allocate LLC Start READY timer request */
  if ((glif_resume_req_ptr = gmmllc_llgmm_get_mes_buf()) == NULL)
  {
    MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
  }

  /* Configure GMMLLC_LLGMM_RESUME_REQ request primitive */
  glif_resume_req_ptr->mes.llgmm_resume_req.prim = GMMLLC_LLGMM_RESUME_REQ ;

  /* Initiate TLLI field */
  glif_resume_req_ptr->mes.llgmm_resume_req.tlli = GLIF_XLATE_TLLI(glif_tlli) ;

#ifdef FEATURE_DUAL_SIM
  glif_resume_req_ptr->mes.llgmm_resume_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent LLGMM_RESUME_REQ to LLC") ;

  mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_resume_req_ptr->mes.llgmm_resume_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
             );

  gmmllc_llgmm_put_mes_buf(glif_resume_req_ptr) ;

  glif_llc_connection_state = LLC_RESUMED ;
  mm_llc_needs_to_be_resumed = FALSE;
} /* end glif_llc_send_resume_request() */


/*===========================================================================

FUNCTION      GLIF_GRR_SEND_CAMP_ON_PCCCH_REQUEST

DESCRIPTION
  Sends a RR_GMM_GPRS_PCCCH_CAMP_REQ primitive to GRR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/*===========================================================================

FUNCTION      GLIF_GRR_SEND_SERVICE_REQ

DESCRIPTION
  Sends a RR_GMM_GPRS_SERVICE_REQ primitive to GRR in order to allow Tx of
  layer 3 messages

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_grr_send_service_req( void )
{
  rr_gmm_gprs_service_req_T rr_gmm_service_req;

  /* Use this opportunity to reset the stored state of the ready timer as
     of last reselection indication. */
  memset(&rr_gmm_service_req,0x00,sizeof(rr_gmm_gprs_service_req_T));
  
  if((glif_grr_ser_req_state != GLIF_SERVICE_REQUESTED) && (glif_grr_ser_req_state != GLIF_NO_SERVICE))
  {
    glif_rdy_tmr_active_at_reselection = FALSE;
  }

  if(((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY) || 
      (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||
      (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY &&
       mm_serv_change_pending && 
       (gmm_state == GMM_DEREGISTERED_INITIATED
                || (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED  
                && gmm_detach_after_rau_pending))))&&
     ((mm_serving_plmn.info.plmn_service_capability == SYS_SRV_CAPABILITY_PS_SERVICE_ONLY) 
       || (mm_serving_plmn.info.plmn_service_capability == SYS_SRV_CAPABILITY_CS_PS_SERVICE))&&
     (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE)&&
     (!gmm_is_suspended)&&(gmm_substate != GMM_SUSPENDED)
    )
       
  {
  rr_gmm_service_req.message_header.message_set = MS_MM_RR;
  rr_gmm_service_req.message_header.message_id  = (int)RR_GMM_GPRS_SERVICE_REQ;

/*lint -save -e778 */
  PUT_IMH_LEN
  (
    sizeof(rr_gmm_gprs_service_req_T) - sizeof(IMH_T),
    &rr_gmm_service_req.message_header
  );
/*lint -restore */

  glif_service_req_pended_ind = FALSE ;

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent RR_GMM_GPRS_SERVICE_REQ to GRR") ;

  mm_send_message((IMH_T *)&rr_gmm_service_req,GS_QUEUE_RR) ;
  
  glif_grr_ser_req_state = GLIF_SERVICE_REQUESTED;
  }
  else
  {
    MSG_HIGH_DS_3(MM_SUB,"=MM= service request was not sent to GERAN ms_op_mode is %d, plmn capability is %d, gmm service status is %d",
                mm_serving_plmn.ms_op_mode,mm_serving_plmn.info.plmn_service_capability,mm_serving_plmn.gmm_service_state);
  }
  
} /* end of glif_grr_send_service_req() */
#ifndef FEATURE_NAS_FREE_FLOAT
void glif_grr_send_camp_on_pccch_request( void )
{
  rr_gmm_gprs_pccch_camp_req_T rr_gmm_gprs_pccch_camp_req;
  memset(&rr_gmm_gprs_pccch_camp_req,0x00,sizeof(rr_gmm_gprs_pccch_camp_req_T));

  rr_gmm_gprs_pccch_camp_req.message_header.message_set = MS_MM_RR;
  rr_gmm_gprs_pccch_camp_req.message_header.message_id  = (int)RR_GMM_GPRS_PCCCH_CAMP_REQ;

/*lint -save -e778 */
  PUT_IMH_LEN
  (
    sizeof(rr_gmm_gprs_pccch_camp_req_T) - sizeof(IMH_T),
    &rr_gmm_gprs_pccch_camp_req.message_header
  );
/*lint -restore */

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent RR_GMM_GPRS_PCCCH_CAMP_REQ to GRR" ) ;

  mm_send_message((IMH_T *)&rr_gmm_gprs_pccch_camp_req.message_header,GS_QUEUE_RR) ;

  glif_grr_camp_state = GLIF_CAMP_ON_PCCCH_REQUESTED;

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: glif_grr_camp_state = GLIF_CAMP_ON_PCCCH_REQUESTED") ;

} /* end of glif_grr_send_camp_on_pccch_request() */
#endif
#ifndef FEATURE_NAS_FREE_FLOAT
/*===========================================================================

FUNCTION      GLIF_GRR_SEND_CAMP_ON_CCCH_INDICATION

DESCRIPTION
  Sends a RR_GMM_GPRS_CCCH_CAMP_IND primitive to GRR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_grr_send_camp_on_ccch_indication( void )
{
  rr_gmm_gprs_ccch_camp_ind_T rr_gmm_gprs_ccch_camp_ind;
 memset(&rr_gmm_gprs_ccch_camp_ind,0X00,sizeof(rr_gmm_gprs_ccch_camp_ind_T));

  rr_gmm_gprs_ccch_camp_ind.message_header.message_set = MS_MM_RR;
  rr_gmm_gprs_ccch_camp_ind.message_header.message_id  = (int)RR_GMM_GPRS_CCCH_CAMP_IND;

/*lint -save -e778 */
  PUT_IMH_LEN
  (
    sizeof(rr_gmm_gprs_ccch_camp_ind_T) - sizeof(IMH_T),
    &rr_gmm_gprs_ccch_camp_ind.message_header
  );
/*lint -restore */

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent RR_GMM_GPRS_CCCH_CAMP_IND to GRR") ;

  mm_send_message((IMH_T *)&rr_gmm_gprs_ccch_camp_ind.message_header,GS_QUEUE_RR) ;

  glif_grr_camp_state = GLIF_CAMP_ON_CCCH;

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: glif_grr_camp_state = GLIF_CAMP_ON_CCCH") ;

} /* end of glif_grr_send_camp_on_ccch_indication() */



/*===========================================================================

FUNCTION      GLIF_GRR_SEND_SERVICE_CONFIRM

DESCRIPTION
  Sends a RR_GMM_GPRS_SERVICE_CNF primitive to GRR in order to allow Tx of
  layer 3 messages

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_grr_send_service_confirm( void )
{
  rr_gmm_gprs_service_cnf_T rr_gmm_service_cnf;

  /* Use this opportunity to reset the stored state of the ready timer as
     of last reselection indication. */
  memset(&rr_gmm_service_cnf,0x00,sizeof(rr_gmm_gprs_service_cnf_T));
  glif_rdy_tmr_active_at_reselection = FALSE;

  rr_gmm_service_cnf.message_header.message_set = MS_MM_RR;
  rr_gmm_service_cnf.message_header.message_id  = (int)RR_GMM_GPRS_SERVICE_CNF;

/*lint -save -e778 */
  PUT_IMH_LEN
  (
    sizeof(rr_gmm_gprs_service_cnf_T) - sizeof(IMH_T),
    &rr_gmm_service_cnf.message_header
  );
/*lint -restore */

  glif_service_confirm_pended_ind = FALSE ;

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent RR_GMM_GPRS_SERVICE_CNF to GRR" ) ;

  mm_send_message((IMH_T *)&rr_gmm_service_cnf.message_header,GS_QUEUE_RR) ;

} /* end of glif_grr_send_service_confirm() */
#endif
/*===========================================================================

FUNCTION      SEND_RR_READY_TIMER_STATE_REQUEST

DESCRIPTION
  This function builds RR_GMM_READY_TIMER_STATE_REQ request primitive and
  sends it to the GRR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void send_rr_ready_timer_state_request
(
  boolean ready_timer_state
)
{
  rr_gmm_ready_timer_state_req_T rr_ready_timer_state_req ;
  memset(&rr_ready_timer_state_req,0x00,sizeof(rr_gmm_ready_timer_state_req_T) );

  /* Ready timer state request */
  rr_ready_timer_state_req.message_header.message_set = MS_MM_RR;
  rr_ready_timer_state_req.message_header.message_id  =
                                                 (int)RR_GMM_READY_TIMER_STATE_REQ ;

  rr_ready_timer_state_req.negotiated_ready_timer_zero = ready_timer_state ;

  PUT_IMH_LEN
  (
    sizeof(rr_gmm_ready_timer_state_req_T) - sizeof(IMH_T),
    &rr_ready_timer_state_req.message_header
  ) ;

  MSG_HIGH_DS_1(MM_SUB, "=MM= GLIF: Sent Rdy Tmr State to GRR: %d",ready_timer_state) ;

  /* Send RR TLLI assignment request */
  mm_send_message
  (
    (IMH_T *)&rr_ready_timer_state_req,GS_QUEUE_RR
  ) ;
} /* end send_rr_ready_timer_state_request() */

/*===========================================================================

FUNCTION      GLIF_RESTART_T3312

DESCRIPTION
  This function restarts periodic RAU timer (T3312)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_restart_t3312
(
  void
)
{

  MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Restart PRAU Tmr: %d ms",gmm_t3312_timer_value) ;

  if (mm_timer_status[ TIMER_T3312 ] == TIMER_ACTIVE)
  {
    mm_stop_timer( TIMER_T3312 );
  }

  if (gmm_t3312_timer_value != 0)
  {
    mm_start_timer( TIMER_T3312, gmm_t3312_timer_value );
  }
} /* end glif_restart_t3312() */

/*===========================================================================

FUNCTION      GLIF_GRR_STATE_EXCHANGE_REQ

DESCRIPTION
  This function builds RR_GMM_GPRS_STATE_CHANGE_REQ request primitive and
  sends it to the GRR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_grr_state_exchange
(
  gmm_llc_if_state_type new_glif_state /* New GMM GPRS state */
)
{
  rr_gmm_gprs_state_change_req_T gmm_state_change_req ;
  memset(&gmm_state_change_req,0x00,sizeof(rr_gmm_gprs_state_change_req_T));

  gmm_state_change_req.message_header.message_set = MS_MM_RR;
  gmm_state_change_req.message_header.message_id  =
                                                 (int)RR_GMM_GPRS_STATE_CHANGE_REQ ;

  gmm_state_change_req.new_state = new_glif_state ;

  PUT_IMH_LEN
  (
    sizeof(rr_gmm_gprs_state_change_req_T) - sizeof(IMH_T),
    &gmm_state_change_req.message_header
  ) ;

  MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Sent State Change to GRR: %d",new_glif_state) ;

  mm_send_message( (IMH_T *)&gmm_state_change_req, GS_QUEUE_RR );
} /* end glif_grr_state_exchange() */

/*===========================================================================

FUNCTION      GLIF_ENTER_MM_NON_DRX_MODE

DESCRIPTION
  This function builds RR_GMM_START_MM_NON_DRX_IND request primitive and
  sends it to the GRR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_enter_mm_non_drx_mode( void )
{
  rr_gmm_start_mm_non_drx_ind_T gmm_start_mm_non_drx_ind ;
  memset(&gmm_start_mm_non_drx_ind,0x00,sizeof(rr_gmm_start_mm_non_drx_ind_T));

  gmm_start_mm_non_drx_ind.message_header.message_set = MS_MM_RR;
  gmm_start_mm_non_drx_ind.message_header.message_id  =
                                                 (int)RR_GMM_START_MM_NON_DRX_IND ;
  /*lint -save -e778 */
  PUT_IMH_LEN
  (
    sizeof(rr_gmm_start_mm_non_drx_ind_T) - sizeof(IMH_T),
    &gmm_start_mm_non_drx_ind.message_header
  );
  /*lint -restore */


  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent Start MM Non-DRX to GRR" );

  mm_send_message( (IMH_T *)&gmm_start_mm_non_drx_ind, GS_QUEUE_RR );
} /* end glif_enter_mm_non_drx_mode() */

/*===========================================================================

FUNCTION      GLIF_EXIT_MM_NON_DRX_MODE

DESCRIPTION
  This function builds RR_GMM_STOP_MM_NON_DRX_IND request primitive and
  sends it to the GRR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_exit_mm_non_drx_mode( void )
{
  rr_gmm_stop_mm_non_drx_ind_T gmm_stop_mm_non_drx_ind ;
  memset(&gmm_stop_mm_non_drx_ind,0x00,sizeof(rr_gmm_stop_mm_non_drx_ind_T));

#ifdef FEATURE_DUAL_SIM
  if(MMCNM_IS_MODE_DUAL_STANDBY)
  {
    MSG_HIGH_DS_0(MM_SUB," =MM= GMM sending un-block tuneaway ind to GERAN");
    geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_GMM, FALSE, (sys_modem_as_id_e_type)mm_sub_id);
  }
#endif

  gmm_stop_mm_non_drx_ind.message_header.message_set = MS_MM_RR;
  gmm_stop_mm_non_drx_ind.message_header.message_id  =
                                                 (int)RR_GMM_STOP_MM_NON_DRX_IND ;
  /*lint -save -e778 */
  PUT_IMH_LEN
  (
    sizeof(rr_gmm_stop_mm_non_drx_ind_T) - sizeof(IMH_T),
    &gmm_stop_mm_non_drx_ind.message_header
  );
  /*lint -restore */

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent Stop MM Non-DRX to GRR") ;

  mm_send_message( (IMH_T *)&gmm_stop_mm_non_drx_ind, GS_QUEUE_RR );
} /* end glif_exit_mm_non_drx_mode() */

/*===========================================================================

FUNCTION      GLIF_STOP_READY_TIMER

DESCRIPTION
  This function builds GMMLLC_LLGMM_STOP_RDY_TMR_REQ request primitive and
  sends it to the LLC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_stop_ready_timer
(
  boolean rau_timer_active
)
{
  gmmllc_llgmm_mes_type *glif_stop_ready_timer_ptr ;

  /* Store current state of ready timer. */
  glif_rdy_tmr_active = FALSE;

  /* Allocate LLC Stop READY timer request */
  if ((glif_stop_ready_timer_ptr = gmmllc_llgmm_get_mes_buf()) == NULL)
  {
    MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
  }

  /* Configure GMMLLC_LLGMM_STOP_RDY_TMR_REQ request primitive */
  glif_stop_ready_timer_ptr->mes.
            llgmm_stop_rdy_tmr_req.prim = GMMLLC_LLGMM_STOP_RDY_TMR_REQ ;

  /* Initiate TLLI field */
  glif_stop_ready_timer_ptr->mes.llgmm_stop_rdy_tmr_req.tlli =
    GLIF_XLATE_TLLI(glif_tlli);

#ifdef FEATURE_DUAL_SIM
  glif_stop_ready_timer_ptr->mes.llgmm_stop_rdy_tmr_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent LLGMM_STOP_RDY_TMR_REQ to LLC") ;

  mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_stop_ready_timer_ptr->mes.llgmm_stop_rdy_tmr_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
              );

  /*
  ** Send Stop READY timer request to the LLC
  */
  gmmllc_llgmm_put_mes_buf(glif_stop_ready_timer_ptr) ;

  if (rau_timer_active)
  {
    /* According to 24.008 :
    ** the timer T3312 is reset and started with its initial value,
    ** when the READY timer is stopped
    */
    glif_restart_t3312() ;
  }

} /* end glif_stop_ready_timer() */

/*===========================================================================

FUNCTION      GLIF_START_READY_TIMER

DESCRIPTION
  This function builds GMMLLC_LLGMM_STRT_RDY_TMR_REQ request primitive and
  sends it to the LLC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_start_ready_timer
(
  dword timer_value /* New READY timer values in ms */
)
{
  gmmllc_llgmm_mes_type *glif_start_ready_timer_ptr ;

  /* Store current state of ready timer. */
  glif_rdy_tmr_active = TRUE;

  /* Allocate LLC Start READY timer request */
  if ((glif_start_ready_timer_ptr = gmmllc_llgmm_get_mes_buf()) == NULL)
  {
    MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
  }

  /* Configure GMMLLC_LLGMM_STRT_RDY_TMR_REQ request primitive */
  glif_start_ready_timer_ptr->mes.
            llgmm_strt_rdy_tmr_req.prim = GMMLLC_LLGMM_STRT_RDY_TMR_REQ ;

  /* Initiate TLLI field */
  glif_start_ready_timer_ptr->mes.llgmm_strt_rdy_tmr_req.tlli =
    GLIF_XLATE_TLLI(glif_tlli);

  /* Initiate READY timer timeout value */
  glif_start_ready_timer_ptr->mes.
                         llgmm_strt_rdy_tmr_req.timeout_ms = timer_value ;

#ifdef FEATURE_DUAL_SIM
  glif_start_ready_timer_ptr->mes.llgmm_strt_rdy_tmr_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent LLGMM_STRT_RDY_TMR_REQ to LLC") ;

  mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_start_ready_timer_ptr->mes.llgmm_strt_rdy_tmr_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
             );

  /*
  ** Send Start READY timer request to the LLC
  */
  gmmllc_llgmm_put_mes_buf(glif_start_ready_timer_ptr) ;

  if (mm_timer_status[ TIMER_T3312 ] == TIMER_ACTIVE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= The RAU timer's been stoped ") ;

    mm_stop_timer( TIMER_T3312 );
  }
#if defined(FEATURE_LTE)
  mm_stop_timer(TIMER_T3323);
#endif
} /* end glif_start_ready_timer() */


/*===========================================================================

FUNCTION      GLIF_GRR_NOTIFY_MS_RA_CAP

DESCRIPTION
  Generates a coded GRR MS Radio Access Capability IE without any spare bits
  or padding, and provides this to GRR in a RR_GMM_START_MM_NON_DRX_IND
  request primitive for use in GPRS Packet Resource Request messages.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_grr_notify_ms_ra_cap( void )
{
  rr_gmm_gprs_ms_ra_cap_ie_ind_T rr_gmm_ms_ra_cap_ind;
  memset(&rr_gmm_ms_ra_cap_ind,0x00,sizeof(rr_gmm_gprs_ms_ra_cap_ie_ind_T));

  rr_gmm_ms_radio_access_cap_ie = get_ms_radio_access_capability( TRUE );

  rr_gmm_ms_ra_cap_ind.message_header.message_set = MS_MM_RR;
  rr_gmm_ms_ra_cap_ind.message_header.message_id  =
    (int)RR_GMM_GPRS_MS_RA_CAP_IE_IND;

  PUT_IMH_LEN
  (
    sizeof(rr_gmm_gprs_ms_ra_cap_ie_ind_T) - sizeof(IMH_T),
    &rr_gmm_ms_ra_cap_ind.message_header
  );

  rr_gmm_ms_ra_cap_ind.ms_ra_cap_ie          = rr_gmm_ms_radio_access_cap_ie.value;
  rr_gmm_ms_ra_cap_ind.ms_ra_cap_ie_len_bits = rr_gmm_ms_radio_access_cap_ie.length;

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent MS RA CAP IE to GRR") ;

  /* Send RR MS RA Cap Indication */
  mm_send_message(
    (IMH_T *)&rr_gmm_ms_ra_cap_ind,GS_QUEUE_RR);

} /* end of glif_grr_notify_ms_ra_cap() */

#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================

FUNCTION      glif_llc_send_psho_request

DESCRIPTION
  This function builds GMMLLC_LLGMM_PSHO_REQ message along with the NAS container parameters
  and sends them to the LLC.

  If Nas container is not present, then this is indicated by the flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_llc_send_psho_request
(
  void
)
{

  gmmllc_llgmm_mes_type *glif_psho_req_ptr;

  /* Allocate LLC ASSIGN request buffer */
  if ((glif_psho_req_ptr = gmmllc_llgmm_get_mes_buf()) == NULL)
  {
    MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
  }

  glif_psho_req_ptr->mes.llgmm_psho_req.prim = GMMLLC_LLGMM_PSHO_REQ ;
    
  /* If the GPRS CKSN is currently deleted then indicate undefined algo. */
  if ( auth_gsm_gprs_cksn == DELETE_CKSN )
  {
    glif_psho_req_ptr->mes.llgmm_psho_req.nas_container.gea = GMMLLC_GEA_UNDEFINED;
  }
  else
  {
    gmm_psho_gea_change = ((auth_ps_ciphering_algorithm != (gmm_psho_nas_container[0] & 0x07))?TRUE:FALSE);
    
    auth_ps_ciphering_algorithm = (gmm_psho_nas_container[0] & 0x07);
          
    glif_psho_req_ptr->mes.llgmm_psho_req.nas_container.gea =
      (gmmllc_gea_e_type)auth_ps_ciphering_algorithm ;
  }

  glif_psho_req_ptr->mes.llgmm_psho_req.nas_container.use_old_xid_ind = 
    ((gmm_psho_nas_container[0] & 0x10)?TRUE:FALSE);
        
  memscpy((void *)&(glif_psho_req_ptr->mes.llgmm_psho_req.nas_container.iov_ui[0]), GLLC_IOV_UI_SIZE,
      (void *)&gmm_psho_nas_container[1], PSHO_IOV_UI_SIZE ) ;

#ifdef FEATURE_DUAL_SIM
  glif_psho_req_ptr->mes.llgmm_psho_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent PSHO Req to LLC") ;

  mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_psho_req_ptr->mes.llgmm_psho_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
           );

  /*
  ** Send LLC TLLI ASSIGNMENT request
  */
  gmmllc_llgmm_put_mes_buf(glif_psho_req_ptr) ;

}
#endif
/*===========================================================================

FUNCTION      GLIF_TLLI_ASSIGN

DESCRIPTION
  This function builds GMMLLC_LLGMM_ASSIGN_REQ, RR_GMM_GPRS_TLLI_ASSIGN_REQ
  and GMMSM_TLLI_ASSIGNMENT_IND request primitives and sends them to the LLC,
  GRR and SM respectively.

  If GEA notification only is indicated then only GMMLLC_LLGMM_ASSIGN_REQ is
  sent to LLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_tlli_assign
(
  glif_tlli_type  new_tlli,        /* New TLLI */
  boolean         notify_gea_only  /* Change of GEA only */
)
{
  rr_gmm_gprs_tlli_assignment_req_T rr_gmm_tlli_assignment ;
  sm_cmd_type                       *sm_cmd ;
  memset(&rr_gmm_tlli_assignment,0x00,sizeof(rr_gmm_gprs_tlli_assignment_req_T));

  if (memcmp((void *)glif_tlli, (void *)new_tlli, PTMSI_SIZE))
  {
    gmmllc_llgmm_mes_type *glif_assign_req ;

    /* TLLI has changed */

    /* Save current TLLI */
    memscpy((void *)glif_old_tlli, PTMSI_SIZE, (void *)glif_tlli, PTMSI_SIZE) ;

    /* Initiate new TLLI */
    memscpy((void *)glif_tlli, PTMSI_SIZE, (void *)new_tlli, PTMSI_SIZE) ;

    /* Allocate LLC ASSIGN request buffer */
    if ((glif_assign_req = gmmllc_llgmm_get_mes_buf()) == NULL)
    {
      MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
    }

    glif_assign_req->mes.llgmm_assign_req.prim = GMMLLC_LLGMM_ASSIGN_REQ ;

    /* Initiate OLD TLLI field */

    glif_assign_req->mes.llgmm_assign_req.tlli_old =
      GLIF_XLATE_TLLI(glif_old_tlli);

    /* Initiate NEW TLLI field */

    glif_assign_req->mes.llgmm_assign_req.tlli_new =
      GLIF_XLATE_TLLI(glif_tlli);

    if (auth_ps_ciphering_algorithm == 0xFF)
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: GPRS Cipher Algo Undefined") ;
    }

    /* If the GPRS CKSN is currently deleted then indicate undefined algo. */

    if ( auth_gsm_gprs_cksn == DELETE_CKSN )
    {
      glif_assign_req->mes.llgmm_assign_req.gea = GMMLLC_GEA_UNDEFINED ;
    }
    else
    {
      glif_assign_req->mes.llgmm_assign_req.gea =
        (gmmllc_gea_e_type)auth_ps_ciphering_algorithm ;
    }

    memscpy((void *)glif_assign_req->mes.llgmm_assign_req.kc, sizeof(glif_assign_req->mes.llgmm_assign_req.kc),
        (void *)auth_gsm_gprs_cipherkey.value_data,
                                        auth_gsm_gprs_cipherkey.value_length) ;
    memscpy((void *)&(glif_assign_req->mes.llgmm_assign_req.kc_128bit[0]), 
            sizeof(glif_assign_req->mes.llgmm_assign_req.kc_128bit),(void *)&(gsm_gprs_kc_bit128.value_data[0]),
            gsm_gprs_kc_bit128.value_length);
#ifdef FEATURE_DUAL_SIM
    glif_assign_req->mes.llgmm_assign_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent TLLI Assignment to LLC") ;

    mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_assign_req->mes.llgmm_assign_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
            );

    /*
    ** Send LLC TLLI ASSIGNMENT request
    */
    gmmllc_llgmm_put_mes_buf(glif_assign_req) ;

    /* Build and send RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ request primitive
    ** to the GRR
    */
    rr_gmm_tlli_assignment.message_header.message_set = MS_MM_RR;
    rr_gmm_tlli_assignment.message_header.message_id  =
                                            (int)RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ ;
#ifdef FEATURE_SGLTE
    if(MM_IS_IN_SGLTE_MODE)
    {
      rr_gmm_tlli_assignment.service_domain = SYS_SRV_DOMAIN_CS_PS;
    }
    else
#endif
    {
      if(gmm_detach_after_rau_pending && mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
      {
        rr_gmm_tlli_assignment.service_domain = SYS_SRV_DOMAIN_CS_PS;
      }
      else
      {
        rr_gmm_tlli_assignment.service_domain = mm_serving_plmn.ms_op_mode;
      }
    }

    PUT_IMH_LEN
    (
      sizeof(rr_gmm_gprs_tlli_assignment_req_T) - sizeof(IMH_T),
      &rr_gmm_tlli_assignment.message_header
    );

    /* Initiate OLD TLLI field */
    rr_gmm_tlli_assignment.oldTLLI = GLIF_XLATE_TLLI(glif_old_tlli);

    /* Initiate NEW TLLI field */
    rr_gmm_tlli_assignment.newTLLI = GLIF_XLATE_TLLI(glif_tlli);

    MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Sent TLLI Assignment to GRR  with servcie domain %d", rr_gmm_tlli_assignment.service_domain);

    /* Send RR TLLI assignment request */
    mm_send_message(
               (IMH_T *)&rr_gmm_tlli_assignment,GS_QUEUE_RR) ;

    /*
    ** If the GMM assigning LOCAL or Invalid TLLI, the interface sends
    ** GMMSM_TLLI_ASSIGNMENT_IND indication to the SM
    */
    if (((glif_tlli[0] & 0xC0) >> 6) == 3) /* TS 23.003 Table 1 */
    {
      /* Create SM GMMSM_TLLI_ASSIGNMENT_IND */
      sm_cmd = sm_get_cmd_buf() ;

      if (sm_cmd == NULL)
      {
        MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No SM Msg Buffers",0,0,0);
      }

      sm_cmd->header.message_set = MS_GMM_SM ;
      sm_cmd->header.cmd_id = GMMSM_TLLI_ASSIGNMENT_IND ;

      /*  Set applicable TLLI  */

      sm_cmd->cmd.snsm_tlli_assignment_ind.tlli =
        GLIF_XLATE_TLLI(glif_tlli);
#ifdef FEATURE_DUAL_SIM
      sm_cmd->cmd.snsm_tlli_assignment_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

      MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent TLLI Assignment to SM" );

      /* Send GMMSM_TLLI_ASSIGNMENT_IND indication to the SM task */
      sm_put_cmd( sm_cmd );
    }
  }
  else if ( notify_gea_only )
  {
    gmmllc_llgmm_mes_type *glif_assign_req ;

    /* Allocate LLC ASSIGN request buffer */
    if ((glif_assign_req = gmmllc_llgmm_get_mes_buf()) == NULL)
    {
      MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
    }

    glif_assign_req->mes.llgmm_assign_req.prim = GMMLLC_LLGMM_ASSIGN_REQ ;

    glif_assign_req->mes.llgmm_assign_req.tlli_old =
      GLIF_XLATE_TLLI(glif_old_tlli);

    glif_assign_req->mes.llgmm_assign_req.tlli_new =
      GLIF_XLATE_TLLI(glif_tlli);

    if (auth_ps_ciphering_algorithm == 0xFF)
    {
      MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: GPRS Cipher Algo Undefined") ;
    }

    /* If the GPRS CKSN is currently deleted then indicate undefined algo. */

    if ( auth_gsm_gprs_cksn == DELETE_CKSN )
    {
      glif_assign_req->mes.llgmm_assign_req.gea = GMMLLC_GEA_UNDEFINED ;
    }
    else
    {
      glif_assign_req->mes.llgmm_assign_req.gea =
        (gmmllc_gea_e_type)auth_ps_ciphering_algorithm ;
    }

    memscpy((void *)glif_assign_req->mes.llgmm_assign_req.kc,  sizeof(glif_assign_req->mes.llgmm_assign_req.kc),
        (void *)auth_gsm_gprs_cipherkey.value_data,
                                        auth_gsm_gprs_cipherkey.value_length) ;

   memscpy((void *)&glif_assign_req->mes.llgmm_assign_req.kc_128bit[0], 
            sizeof(glif_assign_req->mes.llgmm_assign_req.kc_128bit),(void *)&gsm_gprs_kc_bit128.value_data[0],
            gsm_gprs_kc_bit128.value_length) ;

#ifdef FEATURE_DUAL_SIM
    glif_assign_req->mes.llgmm_assign_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

    MSG_HIGH_DS_1
    (
      MM_SUB,
      "=MM= GLIF: Sent GEA Notification to LLC: %d",
      auth_ps_ciphering_algorithm);

    mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_assign_req->mes.llgmm_assign_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
         );

    /*
    ** Send LLC TLLI ASSIGNMENT request
    */
    gmmllc_llgmm_put_mes_buf(glif_assign_req) ;
  }

} /* end glif_tlli_assign() */

/*===========================================================================

FUNCTION      GMMLLC_UNASSIGN_OLD_TLLI

DESCRIPTION
  This function unassign OLD TLLI by setting it equal to 0xFFFFFFFF

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_unassign_old_tlli( void )
{
  gmmllc_llgmm_mes_type *glif_assign_req ;
  rr_gmm_gprs_tlli_assignment_req_T rr_gmm_tlli_assignment ;

  memset(&rr_gmm_tlli_assignment,0x00,sizeof(rr_gmm_gprs_tlli_assignment_req_T ));
  /* Invalidate OLD TLLI */
  memset((void *)glif_old_tlli,0xFF,sizeof(glif_tlli_type)) ;

  /* Allocate LLC ASSIGN request buffer */
  if ((glif_assign_req = gmmllc_llgmm_get_mes_buf()) == NULL)
  {
    MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
  }

  glif_assign_req->mes.llgmm_assign_req.prim = GMMLLC_LLGMM_ASSIGN_REQ ;

  /* Initiate OLD TLLI field */
  glif_assign_req->mes.llgmm_assign_req.tlli_old =
                                        GLIF_XLATE_TLLI(glif_old_tlli) ;

  /* Initiate NEW TLLI field */
  glif_assign_req->mes.llgmm_assign_req.tlli_new =
                                            GLIF_XLATE_TLLI(glif_tlli) ;

  if (auth_ps_ciphering_algorithm == 0xFF)
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: GPRS Cipher Algo Undefined") ;
  }
  glif_assign_req->mes.llgmm_assign_req.gea =
    (gmmllc_gea_e_type)auth_ps_ciphering_algorithm ;

  memscpy((void *)glif_assign_req->mes.llgmm_assign_req.kc, sizeof(glif_assign_req->mes.llgmm_assign_req.kc),
        (void *)auth_gsm_gprs_cipherkey.value_data,
                                        auth_gsm_gprs_cipherkey.value_length) ;
 memscpy((void *)&glif_assign_req->mes.llgmm_assign_req.kc_128bit[0], 
          sizeof(glif_assign_req->mes.llgmm_assign_req.kc_128bit),(void *)&gsm_gprs_kc_bit128.value_data[0],
          gsm_gprs_kc_bit128.value_length) ;

#ifdef FEATURE_DUAL_SIM
  glif_assign_req->mes.llgmm_assign_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent OLD TLLI Unassignment to LLC") ;

  mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_assign_req->mes.llgmm_assign_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
            );

  /*
  ** Send LLC TLLI ASSIGNMENT request
  */
  gmmllc_llgmm_put_mes_buf(glif_assign_req) ;

  /* Build and send RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ request primitive
  ** to the GRR
  */
  rr_gmm_tlli_assignment.message_header.message_set = MS_MM_RR;
  rr_gmm_tlli_assignment.message_header.message_id  =
                                            (int)RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ ;

#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_MODE)
  {
    rr_gmm_tlli_assignment.service_domain = SYS_SRV_DOMAIN_CS_PS;
  }
  else
#endif
  {
    if(gmm_detach_after_rau_pending && mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)
    {
      rr_gmm_tlli_assignment.service_domain = SYS_SRV_DOMAIN_CS_PS;
    }
    else
    {
      rr_gmm_tlli_assignment.service_domain = mm_serving_plmn.ms_op_mode;
    }
  }

  PUT_IMH_LEN
  (
    sizeof(rr_gmm_gprs_tlli_assignment_req_T) - sizeof(IMH_T),
    &rr_gmm_tlli_assignment.message_header
  );

  /* Initiate OLD TLLI field */
  rr_gmm_tlli_assignment.oldTLLI = GLIF_XLATE_TLLI(glif_old_tlli);

  /* Initiate NEW TLLI field */
  rr_gmm_tlli_assignment.newTLLI = GLIF_XLATE_TLLI(glif_tlli);

  MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Sent OLD TLLI Unassignment to GRR  with service domain %d",rr_gmm_tlli_assignment.service_domain);

  /* Send RR TLLI assignment request */
  mm_send_message(
               (IMH_T *)&rr_gmm_tlli_assignment,GS_QUEUE_RR) ;

} /* end glif_unassign_old_tlli() */

/*===========================================================================

FUNCTION      GLIF_GET_TLLI

DESCRIPTION
  This function generates TLLI value according to requirements are described
  in TS 23.003 and TS 23.060

DEPENDENCIES
  None

RETURN VALUE
  Pointer to brand new TLLI arrdess

SIDE EFFECTS
  None

===========================================================================*/
byte *glif_get_tlli
(
  glif_tlli_id_type required_tlli_type /* Required TLLI type */
)
{
  
  boolean ptmsi_present = FALSE ;
  byte index ;
  gmm_ptmsi_type ptmsi;

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);

  if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
  {
    if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI ||
          emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
          mm_is_ptmsi_valid()))
    {
      ptmsi = gmm_stored_ptmsi;
      ptmsi_present = TRUE; 
    }
    else if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI &&
                   emm_ctrl_data_ptr->emm_guti_valid ))
    {
      emm_map_guti_to_ptmsi(&ptmsi, emm_ctrl_data_ptr);
      ptmsi_present = TRUE; 
    }
    else if(mm_is_ptmsi_valid())
    {
      ptmsi = gmm_stored_ptmsi;
      ptmsi_present = TRUE;   
    }
    else if( (emm_ctrl_data_ptr->emm_guti_valid))
    {
      emm_map_guti_to_ptmsi(&ptmsi, emm_ctrl_data_ptr);
      ptmsi_present = TRUE; 
    }
  }
  else
#endif  
  {
    /* Validate existing P-TMSI */
    for (index=0; index<PTMSI_SIZE; index++)
    {
      if (gmm_stored_ptmsi.digit[index] != 0xFF)
      {
        ptmsi_present = TRUE;
        break;
      }
    }
    ptmsi = gmm_stored_ptmsi;
  }

  if (required_tlli_type == GLIF_DISABLE_TLLI_TYPE)
  {
    /* GMM-LLC interface module goes to the GPRS IDLE state
    ** TLLI shall be invalidated
    */
    memset((void *)tlli_gmmllcm,0xFF,sizeof(glif_tlli_type)) ;
  }
  else
  {
    if (ptmsi_present)
    {
      /* Initiate TLLI by setting equal to P-TMSI */
      memscpy((void *)tlli_gmmllcm,PTMSI_SIZE,(void *)ptmsi.digit,PTMSI_SIZE) ;

      switch (required_tlli_type)
      {
        case GLIF_FOREIGN_TLLI_TYPE :
          /* Generate Foreign TLLI */
          tlli_gmmllcm[ 0 ] = (tlli_gmmllcm[ 0 ] & 0x3F) | 0x80 ;
          break ;
        case GLIF_LOCAL_TLLI_TYPE :
          /* Generate Local TLLI */
          tlli_gmmllcm[ 0 ] = (tlli_gmmllcm[ 0 ] & 0x3F) | 0xC0 ;

          break ;
        default:
          MSG_FATAL_DS
          (
            MM_SUB, "GLIF: Invld TLLI Type Req: %d", (int)required_tlli_type, 0, 0
          );
      }
    }
    else
    {
      dword low_rand_value ;
      dword hi_rand_value ;
      dword rand_value ;

      /* Generate 32-bit random value */
      low_rand_value = mm_get_random_value(RAND_MAX) ;
      hi_rand_value = mm_get_random_value(RAND_MAX) ;
      rand_value = (hi_rand_value<<16) | low_rand_value ;

      /* Initiate TLLI by setting equal to random value */
      memscpy((void *)tlli_gmmllcm, PTMSI_SIZE,(void *)&rand_value,PTMSI_SIZE) ;

      /* Generate Random TLLI */
      tlli_gmmllcm[ 0 ] = (tlli_gmmllcm[ 0 ] & 0x7) | 0x78 ;
    }
  }

  return tlli_gmmllcm ;

} /* end glif_get_tlli() */

/*===========================================================================

FUNCTION      GLIF_GMMIF_STANDBY_STATE_HANDLER

DESCRIPTION
  GMM<->LLC Interface module's STANDBY state handler.

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
gmm_llc_if_state_type glif_gmmif_standby_state_handler
(
  /* Incoming GMM event */
  gmm_trigger_llc_type gmm_event
)
{
  gmm_llc_if_state_type next_glif_state = GMM_STANDBY_STATE ;

  switch (gmm_event)
  {
    case GMM_TRIGGER_PTMSI_ASSIGNED :
    case GMM_TRIGGER_GMM_ATTACHED :

      /* Assign new TLLI for the LLC and GRR layers */
      glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;

      glif_foreign_tlli_assigned = FALSE ;

      glif_attach_in_progress = FALSE;

      glif_sending_gmm_message = FALSE ;

      break ;
    case GMM_TRIGGER_GMM_DETACHED :

      //Resetting glif_ready_timer_value to default vale.

      glif_ready_timer_value = 44000 ;

      /* Unasign TLLI */
      glif_tlli_assign(glif_get_tlli(GLIF_DISABLE_TLLI_TYPE),FALSE) ;
      gmm_xid_negotiation_completed = FALSE;

      /* Packet services are no longer required. Indicate to GRR so that
      ** GRR need only camp on CCCH for GSM services.
      */
      
#ifdef FEATURE_GSM_GPRS
#ifdef FEATURE_NAS_FREE_FLOAT
      gmmllc_reset_service_state();
#else      
      glif_grr_send_camp_on_ccch_indication();
#endif 
#endif/*#ifdef FEATURE_GSM_GPRS*/
      if (mm_timer_status[ TIMER_T3312 ] == TIMER_ACTIVE)
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Stop PRAU Tmr T3312") ;

        mm_stop_timer( TIMER_T3312 );
      }
#if defined(FEATURE_LTE)
      mm_stop_timer(TIMER_T3323);
#endif

      /*
      ** Upon reception of the GMM_TRIGGER_GMM_DETACHED indication
      ** the GMM-LLC interface goes to GMM_IDLE_STATE state
      */
      next_glif_state = GMM_IDLE_STATE ;

      /*
      ** Clear the following flags because in the IDLE state UE is
      ** detached from the GPRS network and pended messages won't be
      ** transmitted again any more
      */

      glif_foreign_tlli_assigned = FALSE ;

      glif_pended_cell_update = FALSE ;

      glif_pended_cell_update_is_initial = FALSE ;

      glif_pended_attach_req = FALSE ;

      glif_pended_rau_req = FALSE ;

      glif_pended_detach_req = FALSE ;

      glif_attach_in_progress = FALSE;

      glif_rau_in_progress = FALSE;

      glif_sending_gmm_message = FALSE ;

      glif_detach_in_progress = FALSE ;
#ifdef FEATURE_MODEM_HEAP
      if(glif_pended_gmm_msg != NULL)
      {
        modem_mem_free(glif_pended_gmm_msg,MODEM_MEM_CLIENT_NAS);
        glif_pended_gmm_msg = NULL;
      }
#endif

      break ;

    case GMM_TRIGGER_DETACH_STARTED :

      glif_detach_in_progress = TRUE ;

      break ;

    case GMM_TRIGGER_IMSI_DETACH_FINISHED :

      /* 
      ** IMSI Detach has already been finished,
      ** Resume LLC
      */
      if (!(glif_pended_cell_update || 
                              glif_attach_in_progress || glif_rau_in_progress))
      {
        glif_llc_send_resume_request() ;
      }

      break ;
      
#ifdef FEATURE_GPRS_PS_HANDOVER
    case GMM_TRIGGER_PSHO_STARTED :
     
      glif_llc_send_psho_request();

      break;
#endif  

    case GMM_TRIGGER_GMM_ATTACH_STARTED:
    case GMM_TRIGGER_FORCE_TO_STANDBY :
    case GMM_TRIGGER_RAU_STARTED :
    case GMM_TRIGGER_RAU_FINISHED :
    case GMM_TRIGGER_NEW_READY_TMR_VALUE :
    case GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE :
    case GMM_TRIGGER_NO_READY_TMR_VALUE :
    case GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE :
    case GMM_TRIGGER_NEW_KC_VALUE :

       /* There is nothing to do */

      MSG_ERROR_DS_1( MM_SUB, "=MM= GLIF: Unexp GMM GPRS Stby Evt: %d", gmm_event ) ;

      break ;
    default:
      MSG_FATAL_DS( MM_SUB, "=MM= GLIF: Invld GMM GPRS Stby Evt: %d", gmm_event, 0, 0 );
      break ;
  }

  return next_glif_state ;
} /* end glif_gmmif_standby_state_handler1() */

/*===========================================================================

FUNCTION      GLIF_GMMIF_READY_STATE_HANDLER

DESCRIPTION
  GMM<->LLC Interface module's READY state handler.

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
gmm_llc_if_state_type glif_gmmif_ready_state_handler
(
  /* Incoming GMM event */
  gmm_trigger_llc_type gmm_event
)
{
  gmm_llc_if_state_type next_glif_state = GMM_READY_STATE ;

  switch (gmm_event)
  {
    case GMM_TRIGGER_PTMSI_ASSIGNED :
    case GMM_TRIGGER_GMM_ATTACHED :

      /* Assign new TLLI for the LLC and GRR layers */
      glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;

      glif_foreign_tlli_assigned = FALSE ;

      glif_attach_in_progress = FALSE;

      glif_sending_gmm_message = FALSE ;
      break ;
    case GMM_TRIGGER_GMM_DETACHED :

      //Resetting glif_ready_timer_value to default vale.

      glif_ready_timer_value = 44000 ;

      /* Unasign TLLI */
      glif_tlli_assign(glif_get_tlli(GLIF_DISABLE_TLLI_TYPE),FALSE) ;
      gmm_xid_negotiation_completed = FALSE;

      /* Packet services are no longer required. Indicate to GRR so that
      ** GRR need only camp on CCCH for GSM services.
      */
#ifdef FEATURE_GSM_GPRS 
#ifdef FEATURE_NAS_FREE_FLOAT
      gmmllc_reset_service_state ();
#else
      glif_grr_send_camp_on_ccch_indication();
#endif 
#endif/*#ifdef FEATURE_GSM_GPRS*/

      if ((glif_ready_timer_value != 0) && !glif_ready_timer_deactivated)
      {
        /* Command the LLC to stop READY timer */
        glif_stop_ready_timer(FALSE) ;
      }

      if (mm_timer_status[ TIMER_T3312 ] == TIMER_ACTIVE)
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Stop PRAU Tmr T3312") ;

        mm_stop_timer( TIMER_T3312 );
      }
#if defined(FEATURE_LTE)
      mm_stop_timer(TIMER_T3323);
#endif

      /*
      ** Upon reception of the GMM_TRIGGER_GMM_DETACHED indication
      ** the GMM-LLC interface goes to GMM_IDLE_STATE state
      */
      next_glif_state = GMM_IDLE_STATE ;

      /*
      ** Clear the following flags because in the IDLE state UE is
      ** detached from the GPRS network and pended messages won't be
      ** transmitted again any more
      */

      glif_foreign_tlli_assigned = FALSE ;

      glif_pended_cell_update = FALSE ;

      glif_pended_cell_update_is_initial = FALSE ;

      glif_pended_attach_req = FALSE ;

      glif_pended_rau_req = FALSE ;

      glif_pended_detach_req = FALSE ;

      glif_attach_in_progress = FALSE;

      glif_rau_in_progress = FALSE;

      glif_sending_gmm_message = FALSE ;

      glif_detach_in_progress = FALSE ;
#ifdef FEATURE_MODEM_HEAP
      if(glif_pended_gmm_msg != NULL)
      {
        modem_mem_free(glif_pended_gmm_msg,MODEM_MEM_CLIENT_NAS);
        glif_pended_gmm_msg = NULL;
      }
#endif

      break ;

    case GMM_TRIGGER_DETACH_STARTED :

      glif_detach_in_progress = TRUE ;

      break ;

    case GMM_TRIGGER_IMSI_DETACH_FINISHED :

      /* 
      ** IMSI Detach has already been finished,
      ** Resume LLC
      */
      if (!(glif_pended_cell_update || 
                              glif_attach_in_progress || glif_rau_in_progress))
      {
        glif_llc_send_resume_request() ;
      }

      break; 
      
#ifdef FEATURE_GPRS_PS_HANDOVER
    case GMM_TRIGGER_PSHO_STARTED :
     
      glif_llc_send_psho_request();

      break;
#endif   

    case GMM_TRIGGER_GMM_ATTACH_STARTED:
    case GMM_TRIGGER_FORCE_TO_STANDBY :
    case GMM_TRIGGER_RAU_STARTED :
    case GMM_TRIGGER_RAU_FINISHED :
    case GMM_TRIGGER_NEW_READY_TMR_VALUE :
    case GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE :
    case GMM_TRIGGER_NO_READY_TMR_VALUE :
    case GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE :
    case GMM_TRIGGER_NEW_KC_VALUE :

       /* There is nothing to do */

      MSG_ERROR_DS_1( MM_SUB, "=MM= GLIF: Unexp GMM GPRS Rdy Evt: %d", gmm_event) ;
      break ;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= GLIF: Invld GMM GPRS Rdy Evt: %d", gmm_event, 0, 0 );
      break ;
  }

  return next_glif_state ;
} /* end glif_gmmif_ready_state_handler() */

/*===========================================================================

FUNCTION      GLIF_GMMIF_IDLE_STATE_HANDLER

DESCRIPTION
  GMM<->LLC Interface module's IDLE state handler.

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
gmm_llc_if_state_type glif_gmmif_idle_state_handler
(
  /* Incoming GMM event */
  gmm_trigger_llc_type gmm_event
)
{
  gmm_llc_if_state_type next_glif_state = GMM_IDLE_STATE ;
  byte invalid_tlli[4] = {0xFF, 0xFF, 0xFF, 0xFF} ;

  switch (gmm_event)
  {
    case GMM_TRIGGER_PTMSI_ASSIGNED :

      if (!memcmp((void *)invalid_tlli, (void *)gmm_stored_ptmsi.digit, PTMSI_SIZE))
      {
        /* Unassign TLLI due to failure of the Attach procedure */
        glif_tlli_assign(glif_get_tlli(GLIF_DISABLE_TLLI_TYPE),FALSE) ;
        gmm_xid_negotiation_completed = FALSE;

        /* Packet services are no longer required. Indicate to GRR so that
        ** GRR need only camp on CCCH for GSM services.
        */
#ifdef FEATURE_GSM_GPRS 
#ifdef FEATURE_NAS_FREE_FLOAT
        gmmllc_reset_service_state ();
#else
        glif_grr_send_camp_on_ccch_indication();
#endif 
#endif/*#ifdef FEATURE_GSM_GPRS*/

      }
      else
      {
        MSG_ERROR_DS_0( MM_SUB, "=MM= GLIF: PTMSI Assigned in GPRS Idle") ;
      }
      break;

    case GMM_TRIGGER_GMM_ATTACHED :

      glif_attach_in_progress = FALSE;

      glif_sending_gmm_message = FALSE ;

      /* Assign TLLI for the LLC and GRR layers */
      glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;

      glif_foreign_tlli_assigned = FALSE ;

      if ((gmm_state == GMM_REGISTERED) &&
          ((gmm_substate == GMM_NORMAL_SERVICE) ||
           (gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM) ||
           (gmm_substate == GMM_IMSI_DETACH_INITIATED)))
      {
        /* Permit user plane transfers from now on */
        glif_llc_send_resume_request();
      }

      if (glif_ready_timer_value != 0)
      {
        if (!glif_ready_timer_deactivated)
        {
          /* Command the LLC start READY timer */
          glif_start_ready_timer(glif_ready_timer_value) ;
        }

        if (glif_ready_timer_value != 0)
        {
          send_rr_ready_timer_state_request(FALSE) ;
        }

        /*
        ** Upon reception of the GMM_TRIGGER_GMM_ATTACHED indication
        ** the GMM-LLC interface goes to GMM_READY_STATE state
        */
        next_glif_state = GMM_READY_STATE ;
      }
      else
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Rdy Tmr Zero -> GPRS Stby") ;

        glif_pended_cell_update = FALSE ;

        glif_pended_cell_update_is_initial = FALSE ;

        next_glif_state = GMM_STANDBY_STATE ;
      }
      break ;

    case GMM_TRIGGER_GMM_DETACHED :

      //Resetting glif_ready_timer_value to default vale.

      glif_ready_timer_value = 44000 ;
#ifdef FEATURE_GSM_GPRS 
#ifdef FEATURE_NAS_FREE_FLOAT
       gmmllc_reset_service_state();
#else
      if(glif_grr_camp_state == GLIF_CAMP_ON_PCCCH_REQUESTED) 
      {
        /* Packet services are no longer required. Indicate to GRR so that
        ** GRR need only camp on CCCH for GSM services.
        */
        glif_grr_send_camp_on_ccch_indication();
      }
#endif 
#endif/*#ifdef FEATURE_GSM_GPRS*/ 

      /*
      ** Clear the following flags because in the IDLE state UE is
      ** detached from the GPRS network and pended messages won't be
      ** transmitted again any more
      */

      glif_foreign_tlli_assigned = FALSE ;

      glif_pended_cell_update = FALSE ;

      glif_pended_cell_update_is_initial = FALSE ;

      glif_pended_attach_req = FALSE ;

      glif_pended_rau_req = FALSE ;

      glif_pended_detach_req = FALSE ;

      glif_attach_in_progress = FALSE;

      glif_rau_in_progress = FALSE;

      glif_sending_gmm_message = FALSE ;

      glif_detach_in_progress = FALSE ;
#ifdef FEATURE_MODEM_HEAP
      if(glif_pended_gmm_msg != NULL)
      {
        modem_mem_free(glif_pended_gmm_msg,MODEM_MEM_CLIENT_NAS);
        glif_pended_gmm_msg = NULL;
      }
#endif

      break;

    case GMM_TRIGGER_DETACH_STARTED :

      glif_detach_in_progress = TRUE ;

      break ;

    case GMM_TRIGGER_IMSI_DETACH_FINISHED :

      /* 
      ** IMSI Detach has already been finished,
      ** Resume LLC
      */
      if (!(glif_pended_cell_update || 
                              glif_attach_in_progress || glif_rau_in_progress))
      {
        glif_llc_send_resume_request() ;
      }

      break ;

    case GMM_TRIGGER_GMM_ATTACH_STARTED:
    case GMM_TRIGGER_FORCE_TO_STANDBY :
    case GMM_TRIGGER_RAU_STARTED :
    case GMM_TRIGGER_RAU_FINISHED :
    case GMM_TRIGGER_NEW_READY_TMR_VALUE :
    case GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE :
    case GMM_TRIGGER_NO_READY_TMR_VALUE :
    case GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE :
    case GMM_TRIGGER_NEW_KC_VALUE :

       /* There is nothing to do */

      MSG_ERROR_DS_1( MM_SUB, "=MM= GLIF: Unexp GMM GPRS Idle Evt: %d", gmm_event) ;

      break ;
    default:
      MSG_FATAL_DS( MM_SUB, "=MM= GLIF: Invld GMM GPRS Idle Evt: %d",gmm_event,0,0) ;
      break ;
  }

  return next_glif_state ;
} /* end glif_gmmif_idle_state_handler() */

/*===========================================================================

FUNCTION      GLIF_SEND_LLC_TRIGGER_REQUEST

DESCRIPTION
  This function creates and sends the LLC GMMLLC_LLGMM_TRIGGER_REQ request
  primitive

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_send_llc_trigger_request
(
  gmmllc_trigger_cause_e_type trigger_cause
)
{
  gmmllc_llgmm_mes_type *glif_trigger_req_ptr ;

  /* Allocate TIGGER timer request */
  if ((glif_trigger_req_ptr = gmmllc_llgmm_get_mes_buf()) == NULL)
  {
    MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0) ;
  }

  /* Initiate specific request ID */
  glif_trigger_req_ptr->
                     mes.llgmm_trigger_req.prim = GMMLLC_LLGMM_TRIGGER_REQ ;

  /* Initiate TRIGGER's indication */
  glif_trigger_req_ptr->mes.llgmm_trigger_req.cause = trigger_cause ;

  /* Initiate TLLI field */
  glif_trigger_req_ptr->mes.llgmm_trigger_req.tlli =
    GLIF_XLATE_TLLI(glif_tlli);

#ifdef FEATURE_DUAL_SIM
  glif_trigger_req_ptr->mes.llgmm_trigger_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Sent LLGMM_TRIGGER_REQ to LLC: %d",trigger_cause) ;

  mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_trigger_req_ptr->mes.llgmm_trigger_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
          );

  /*
  ** Send LLC Trigger request
  */
  gmmllc_llgmm_put_mes_buf(glif_trigger_req_ptr) ;
}

/*===========================================================================

FUNCTION      GLIF_PROCESS_CELL_UPDATE

DESCRIPTION
  This function takes appropriate actions in order to perform Cell
  Update/Notification according to the LLC connection state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_process_cell_update
(
  boolean initial_cell_update_reqd
)
{
  ue_rev_level_indicator_type revision_level;

  switch (glif_llc_connection_state)
  {
    case LLC_RESUMED :
#ifdef FEATURE_GPRS_PS_HANDOVER
      if (gmm_psho_status == GMM_PSHO_IN_PROGRESS )
      {          
        if (glif_foreign_tlli_assigned)
        {
          /** If during the RAU, TLLI has been changed to Foreign, use
          ** Local TLLI in order to send Cell Update message
          */
          glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;

          glif_foreign_tlli_assigned = FALSE ;
        }

        /* Trigger Cell Update/Notification procedure */
        revision_level = get_revision_level_indicator();

        glif_send_llc_trigger_request(
              ((byte)revision_level & cell_notification_ind) ?
                                       GMMLLC_CELL_NOTIFY:GMMLLC_CELL_UPDATE) ;
            
        MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: LLC is not SUSPENDED - PSHO in progress") ;
        gmm_psho_status = GMM_PSHO_NONE;

      }
      else
#endif
      {
        /* Suspend MS' Tx */
        glif_llc_send_suspend_request() ;

        /*
        ** Trigger Cell Update or Cell Notification upon completion of the
        ** LLC suspension
        */
#ifdef FEATURE_NAS_FREE_FLOAT
        if(glif_grr_ser_req_state == GLIF_NO_SERVICE)
        {
          glif_service_req_pended_ind = TRUE;
        }
#endif
        glif_pended_cell_update = TRUE ;
        glif_pended_cell_update_is_initial = initial_cell_update_reqd ;
      }              
      break ;
    case LLC_SUSPENSION_REQUESTED :
      /*
      ** Trigger Cell Update or Cell Notification upon completion of the
      ** LLC suspension
      */
      
#ifdef FEATURE_NAS_FREE_FLOAT
       if(glif_grr_ser_req_state == GLIF_NO_SERVICE)
       {
         glif_service_req_pended_ind = TRUE;
       }
#endif
      glif_pended_cell_update = TRUE ;
      glif_pended_cell_update_is_initial = initial_cell_update_reqd ;
      break ;
    case LLC_SUSPENSION_CONFIRMED :
#ifdef FEATURE_NAS_FREE_FLOAT
      if((glif_grr_ser_req_state == GLIF_NO_SERVICE) &&
         (GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF))
      {
        glif_grr_send_service_req();        
        glif_pended_cell_update = TRUE ;
        glif_pended_cell_update_is_initial = initial_cell_update_reqd ;
        break;
      }
      else if(glif_grr_ser_req_state == GLIF_SERVICE_CONFIRMED)
#endif
      {

        if (glif_foreign_tlli_assigned)
        {
         /*
          ** If during the RAU, TLLI has been changed to Foreign, use
          ** Local TLLI in order to send Cell Update message
          */
          glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;

          glif_foreign_tlli_assigned = FALSE ;
        }

        /* Trigger Cell Update/Notification procedure */
        revision_level = get_revision_level_indicator();

        if (initial_cell_update_reqd == TRUE)
        {
          glif_send_llc_trigger_request(GMMLLC_CELL_UPDATE) ;
        }
        else
        {
          glif_send_llc_trigger_request(
                ((byte)revision_level && cell_notification_ind) ?
                                         GMMLLC_CELL_NOTIFY:GMMLLC_CELL_UPDATE) ;
        }
          /* Required Cell Update/Cell Notification has been completed */
        glif_pended_cell_update = FALSE ;
        glif_pended_cell_update_is_initial = FALSE ;
      }
      break ;
    default :
      MSG_ERROR_DS_1(MM_SUB, "=MM= Illegal LLC connection state (%d)",glif_llc_connection_state) ;
      break ;
  } /* switch (glif_llc_connection_state) */
} /* end glif_process_cell_update() */

/*===========================================================================

FUNCTION      GLIF_GMMIF_COMMON_HANDLER

DESCRIPTION
  This function processes GMM events regardless to current state of GMM-LLC
  interface module.

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
gmm_llc_if_state_type glif_gmmif_common_handler
(
  /* Incoming GMM event */
  gmm_trigger_llc_type gmm_event,

  /* value of GMM_TRIGGER_FORCE_TO_STANDBY request */
  boolean force_to_standby,

  /* Current READY timer value's been assigned by the network in ms */
  dword ready_timer_value
)
{
  gmm_llc_if_state_type next_glif_state = glif_state ;

#if defined(FEATURE_LTE)
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  /* Initiate particular request ID */
  switch (gmm_event)
  {
#ifdef FEATURE_NAS_FREE_FLOAT
    case GMM_TRIGGER_G_RAT_RESUMED:
      if(glif_llc_connection_state == LLC_RESUMED)
      {
        glif_llc_send_suspend_request();
      }
      else if(glif_llc_connection_state == LLC_SUSPENSION_CONFIRMED)
      {
        gmm_sm_send_rat_resume_req();
        gmm_pended_rat_resume_req = FALSE;
      }
      break;
#endif
    case GMM_TRIGGER_GMM_ATTACH_STARTED : /* GMM is starting Attach procedure */

      /*
      ** According to the TS 23.060 6.5.1, 14.3 and TS 23.003 2.6
      ** the Foreign TLLI is used during the attach procedure until
      ** a new PTMSI is allocated.
      */
      if((t3310_timeout_counter == 0) || !glif_foreign_tlli_assigned)
      {
        glif_tlli_assign(glif_get_tlli(GLIF_FOREIGN_TLLI_TYPE),FALSE) ;
      }
      else
      {
        glif_tlli_assign(glif_tlli,FALSE) ;
      }

      glif_foreign_tlli_assigned = TRUE ;

      /* Request that GRR camp for packet services on PCCCH if available
      ** before continuing with Attach sequencing.
      */
#ifdef FEATURE_NAS_FREE_FLOAT
      if ( glif_grr_ser_req_state == GLIF_NO_SERVICE)
      {
        glif_grr_send_service_req() ;
      }
#else
      if ( glif_grr_camp_state == GLIF_CAMP_ON_CCCH )
      {
        glif_grr_send_camp_on_pccch_request() ;
      }
      else if ( glif_grr_camp_state == GLIF_CAMP_ON_PCCCH_CONFIRMED )
      {
        glif_grr_send_service_confirm() ;
      }
      else
      {
        MSG_ERROR_DS_1(MM_SUB, "=MM= Illegal GRR camped state (%d)",glif_grr_camp_state) ;
      }
#endif/* end ifndef nas_free_float*/
      glif_attach_in_progress = TRUE ;

      glif_rau_in_progress = FALSE;

      glif_pended_rau_req = FALSE;

      glif_detach_in_progress = FALSE;

      break;

    case GMM_TRIGGER_RAU_STARTED : /* GMM is starting RAU procedure */

      /*
      ** According to the TS 23.060 6.5.1, 14.3 and TS 24.008 Foreign
      ** TLLI is used during RAU procedure.
      */

      MSG_HIGH_DS_1(MM_SUB, "=MM= The address's %X",(dword)&glif_force_local_tlli) ;

      if ((gmm_update_status != GMM_GU1_UPDATED) ||
          (gmm_rai_changed(glif_ra_changed_rau_completed_ra.location_area_code,
                           glif_ra_changed_rau_completed_ra.routing_area_code,
                           mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
          (gmm_plmn_changed(glif_ra_changed_rau_completed_ra.plmn_id,
                            mm_serving_plmn.info.plmn))
#if defined(FEATURE_LTE)
          || ((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI))

#endif
          ) 
      {
        if (glif_force_local_tlli)
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: FOREIGN TLLI has to be assigned here") ;
        }
        else
        {
          if ((t3330_timeout_counter == 0) || !glif_foreign_tlli_assigned)
          {
            glif_tlli_assign(glif_get_tlli(GLIF_FOREIGN_TLLI_TYPE),FALSE) ;
          }
          else
          {
            glif_tlli_assign(glif_tlli,FALSE) ;
          }
        }

        glif_foreign_tlli_assigned = TRUE ;
      }
      else
      {
        if(t3330_timeout_counter == 0)
        {
        glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;
        }
        else
        {
          glif_tlli_assign(glif_tlli,FALSE) ;
        }

        glif_foreign_tlli_assigned = FALSE ;
      }

      /* If the user plane is not suspended then we must suspend this
      ** before commencing signalling. */

      if ( glif_llc_connection_state == LLC_RESUMED )
      {
#ifdef FEATURE_GPRS_PS_HANDOVER
        if (gmm_psho_status == GMM_PSHO_IN_PROGRESS )
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: LLC is not SUSPENDED - PSHO in progress") ;        
        }
        else
#endif
        {  
          glif_llc_send_suspend_request() ;
        }

      }
      else if ( glif_llc_connection_state == LLC_SUSPENSION_CONFIRMED )
      {
#ifdef FEATURE_NAS_FREE_FLOAT
        if(glif_grr_ser_req_state == GLIF_NO_SERVICE)
        {
          glif_grr_send_service_req() ;
        }
#else
        glif_grr_send_service_confirm() ;
#endif
      }
      else
      {
        MSG_ERROR_DS_1(MM_SUB, "=MM= Illegal LLC connection state (%d)",glif_llc_connection_state) ;
      }
#ifdef FEATURE_NAS_FREE_FLOAT
      if ( (glif_llc_connection_state == LLC_SUSPENSION_REQUESTED) && (glif_grr_ser_req_state == GLIF_NO_SERVICE))
      {
        glif_service_req_pended_ind = TRUE;
      }
#endif    
      glif_rau_in_progress = TRUE ;

      break ;
    case GMM_TRIGGER_RAU_FINISHED : /* GMM finished RAU procedure */

      /*
      ** If RAU procedure made GMM to assign Foreign TLLI,
      ** GMM needs to re-assign Local TLLI back after RAU
      ** has been finished
      */
      if (glif_foreign_tlli_assigned)
      {
        glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;

        glif_foreign_tlli_assigned = FALSE ;
      }

      if ((gmm_state == GMM_REGISTERED) &&
          ((gmm_substate == GMM_NORMAL_SERVICE) ||
           (gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM) ||
           (gmm_substate == GMM_IMSI_DETACH_INITIATED)))
      {
#ifdef FEATURE_GPRS_PS_HANDOVER
        if ((gmm_psho_status != GMM_PSHO_IN_PROGRESS ) || (glif_llc_connection_state != LLC_RESUMED))
#endif        
        {
          /* Permit user plane transfers from now on */
          glif_llc_send_resume_request();
        }

      }
      glif_pended_rau_req     = FALSE;
      glif_pended_attach_req  = FALSE;
      glif_rau_in_progress    = FALSE;

#ifdef FEATURE_MODEM_HEAP
      if(glif_pended_gmm_msg != NULL)
      {
        modem_mem_free(glif_pended_gmm_msg,MODEM_MEM_CLIENT_NAS);
        glif_pended_gmm_msg = NULL;
      }
#endif

      break ;
    case GMM_TRIGGER_FORCE_TO_STANDBY :

      glif_force_to_standby = force_to_standby ;

      if (glif_force_to_standby == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Force To Standby's been activated") ;

        /* 23.060  6.1.1.4 - Force to STANDBY: The SGSN indicates an  
        ** immediate return to STANDBY state before the READY timer expires. 
        */
        if(glif_state == GMM_READY_STATE)
        {
          next_glif_state = GMM_STANDBY_STATE ;
        }

        if ((glif_ready_timer_value != 0) && !glif_ready_timer_deactivated)
        {
          /* Command the LLC to stop READY timer */
          glif_stop_ready_timer(FALSE) ;
        }
      }
      else
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Force-to-stby Deactivated") ;
      }

      break ;

    case GMM_TRIGGER_NO_READY_TMR_VALUE :
    case GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE :

      /* 24.008 4.7.2.1.1 - When the MS proposes a READY Timer value and the
      ** network does not include any ready timer value in it's answer, then
      ** the value proposed by the MS shall be applied for the GMM context by
      ** the network and by the MS. So apply the ready timer value that
      ** would have been proposed by the MS.
      */

      ready_timer_value     = GMM_REQ_READY_TIMER_VALUE_SECS * 1000UL;
      gmm_timer_deactivated = FALSE;

      /* Drop through to next case to continue processing the MS proposed
      ** ready timer value as though it had been received from the Network.
      */

    case GMM_TRIGGER_NEW_READY_TMR_VALUE :
    case GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE :

      if (!gmm_timer_deactivated)
      {
        MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: New Rdy Tmr Assigned: %d ms",ready_timer_value) ;
      }
      else
      {
        MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: New Rdy Tmr Deactivation") ;
      }

      switch (ready_timer_value)
      {
        case 0 :
          if (!gmm_timer_deactivated)
          {
            /* Upon reception of the new READY timer value
            ** is equal to ZERO, the GMM-LLC interface has
            ** to immediately cancel the running timer
            */
            if (!glif_ready_timer_deactivated)
            {
              /* Command the LLC to stop READY timer */
              glif_stop_ready_timer(TRUE) ;
            }

            glif_ready_timer_deactivated = FALSE ;

            glif_pended_cell_update = FALSE ;

            glif_pended_cell_update_is_initial = FALSE ;

            send_rr_ready_timer_state_request(TRUE) ;

            /* Does not perform Cell Update procedure */
            next_glif_state = GMM_STANDBY_STATE ;
          }
          else
          {
            if ( (glif_state == GMM_READY_STATE) &&
                 (!glif_ready_timer_deactivated)    )
            {
              /* Command the LLC to stop READY timer */
              glif_stop_ready_timer(FALSE) ;
            }

            /* Perform Cell Update procedure */
            glif_force_to_standby = FALSE ;

            /* The nework comands to deactivate READY timer */
            glif_ready_timer_deactivated = TRUE ;
          }
          break ;
        default :
          if ((glif_state == GMM_READY_STATE) &&
              (ready_timer_value != glif_ready_timer_value))
          {
            if (!glif_ready_timer_deactivated)
            {
              glif_stop_ready_timer(FALSE) ;
            }
            glif_ready_timer_deactivated = FALSE;
                       
            glif_start_ready_timer(ready_timer_value) ;

            send_rr_ready_timer_state_request(FALSE) ;
          }
          break ;
      }

      /* (TS 24.004 4.7.2.1.1) If a new READY timer value is negotiated, the
      ** MS shall upon the reception of the ATTACH ACCEPT or
      ** ROUTING AREA UPDATE ACCEPT message perform a initial cell update
      ** (either by transmitting a LLC frame or, if required, a ATTACH
      ** COMPLETE or ROUTING AREA UPDATE COMPLETE message), in order to apply
      ** the new READY timer value immediately.
      **
      ** If an ATTACH COMPLETE or ROUTING AREA UPDATE COMPLETE is going to be
      ** sent, then this will serve as a cell update
      */
      if ( (gmm_event != GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE) &&
           (gmm_event != GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE ) &&
           (!glif_force_to_standby                                ) &&
           ((ready_timer_value != 0) && (!gmm_timer_deactivated)  ) &&
           (ready_timer_value != glif_ready_timer_value           )    )
      {
        /* An Attach Complete or Routing Area Update complete message is
           not scheduled to be sent, but a new ready timer value has been
           assigned, so initiate a Cell Update to synchronise ready timer
           with the SGSN. */

        glif_process_cell_update(TRUE) ;
      }

      /* Update the stored ready timer value to the latest value if not
         deactivated. */

      if ( !gmm_timer_deactivated )
        glif_ready_timer_value = ready_timer_value;
      else
        glif_ready_timer_value = (dword)0xFFFFFFFF ;

      break ;

    case GMM_TRIGGER_NEW_KC_VALUE :
      /* Send the LLC new Kc and Ciphering algorithm */
      glif_tlli_assign(glif_tlli,TRUE) ;
      break ;

    case GMM_TRIGGER_TLLI_UNASSIGNMENT:

      glif_tlli_assign(glif_get_tlli(GLIF_DISABLE_TLLI_TYPE),FALSE) ;

      glif_foreign_tlli_assigned = FALSE ;

      gmm_xid_negotiation_completed = FALSE;

      break ;

    case GMM_TRIGGER_LLC_SUSPENSION :

      /* Suspend LLC only when P-TMSI is avaliable */
      if (( GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF ) &&
          ( glif_llc_connection_state == LLC_RESUMED ))
      {
        /* Suspend MS' Tx */
        glif_llc_send_suspend_request() ;
      }

      break ;

    case GMM_TRIGGER_LLC_RESUMPTION :
      if (!(glif_pended_cell_update || glif_attach_in_progress ||
                              glif_detach_in_progress || glif_rau_in_progress))
      {
        glif_llc_send_resume_request() ;
      }
      break ;

    case GMM_TRIGGER_GMM_STABILIZED :

      if(gmm_state == GMM_DEREGISTERED ||
         gmm_state == GMM_NULL)
      {
        next_glif_state = GMM_IDLE_STATE;
      }
      glif_pended_attach_req = FALSE;
      glif_pended_rau_req = FALSE;
      glif_attach_in_progress = FALSE;
      glif_rau_in_progress = FALSE;
      glif_pended_detach_req = FALSE;
      glif_detach_in_progress = FALSE;
#ifdef FEATURE_MODEM_HEAP
      if(glif_pended_gmm_msg != NULL)
      {
        modem_mem_free(glif_pended_gmm_msg,MODEM_MEM_CLIENT_NAS);
        glif_pended_gmm_msg = NULL;
      }
    
      if(glif_pended_detach != NULL)
      {
        modem_mem_free(glif_pended_detach,MODEM_MEM_CLIENT_NAS);
        glif_pended_detach = NULL;
      }
#endif    
      break ;

    default :
      MSG_FATAL_DS( MM_SUB, "=MM= GLIF: Invld GMM event: %d",gmm_event,0,0);
      break ;
  }

  return next_glif_state ;

} /* end glif_gmmif_common_handler() */

/*===========================================================================

FUNCTION      GLIF_EXECUTE_GMM_EVENT

DESCRIPTION
  GMM<->LLC Interface module interface function of the modul's state machine.
  This function initiate a processing of the GMM events.

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
gmm_llc_if_state_type glif_execute_gmm_event
(
  /* Incoming GMM event */
  gmm_trigger_llc_type gmm_event,

  /* Current state of the GMM<->LLC Interface module */
  gmm_llc_if_state_type current_glif_state,

  /* value of GMM_TRIGGER_FORCE_TO_STANDBY request */
  boolean force_to_standby,

  /* Current READY timer value's been assigned by the network in ms */
  dword ready_timer_value
)
{
  gmm_llc_if_state_type next_glif_state = glif_state ;
#ifdef FEATURE_NAS_FREE_FLOAT
  if(gmm_detach_in_progress == TRUE)
  { 
    MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Trigger Event: %d is  pended",gmm_event) ;
    next_glif_state = current_glif_state;
  }
  else
#endif
  {

    switch (gmm_event)
    {
      /* The following events are processed in any state */
      case GMM_TRIGGER_GMM_ATTACH_STARTED :
      case GMM_TRIGGER_RAU_STARTED :
      case GMM_TRIGGER_RAU_FINISHED :
      case GMM_TRIGGER_FORCE_TO_STANDBY :
      case GMM_TRIGGER_NEW_READY_TMR_VALUE :
      case GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE :
      case GMM_TRIGGER_NO_READY_TMR_VALUE :
      case GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE :
      case GMM_TRIGGER_NEW_KC_VALUE :
      case GMM_TRIGGER_TLLI_UNASSIGNMENT:
      case GMM_TRIGGER_LLC_SUSPENSION :
      case GMM_TRIGGER_LLC_RESUMPTION :
      case GMM_TRIGGER_G_RAT_RESUMED :
      case GMM_TRIGGER_GMM_STABILIZED :             

        next_glif_state =
          glif_gmmif_common_handler
                            (gmm_event,force_to_standby,ready_timer_value) ;

        break ;
      default :
        switch (current_glif_state)
        {
          case GMM_IDLE_STATE :
            next_glif_state = glif_gmmif_idle_state_handler(gmm_event) ;
            break ;
          case GMM_READY_STATE :
            next_glif_state = glif_gmmif_ready_state_handler(gmm_event) ;
            break ;
          case  GMM_STANDBY_STATE :
            next_glif_state = glif_gmmif_standby_state_handler(gmm_event) ;
            break ;
          default :
            MSG_FATAL_DS( MM_SUB, "=MM= GLIF: Invalid state: %d",(int)glif_state,0,0);
            break ;
        } /* switch (current_glif_state) */
        break ;
    } /* switch (gmm_event) */
  }
  return next_glif_state ;

} /* end glif_execute_gmm_event */

/*===========================================================================

FUNCTION      GMMLLC_GMM_TRIGGER_LLC_EVENT

DESCRIPTION
  This function is a GMM entry point of the GMM<->LLC interface module.
  It parses & extracts incoming GMM events from the event buffer
  and kicks GMM<->LLC Interface module state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/*
** This macro defines a number of bits which is contained in GMM event
** type (gmm_trigger_llc_type)
*/
#define MAX_EVENT_BIT_LENGTH ((sizeof(gmm_trigger_llc_type)/sizeof(byte))*8)

void gmmllc_gmm_trigger_llc_event
(
  /* Incoming GMM event buffer */
  gmm_trigger_llc_type gmm_events,

  /* value of GMM_TRIGGER_FORCE_TO_STANDBY request */
  boolean force_to_standby,

  /* Current READY timer value's been assigned by the network in ms */
  dword ready_timer_value

)
{
  dword event_index ;
  gmm_trigger_llc_type gmm_event ;

  /* Parse incumming GMM event buffer */
  for(event_index=0; event_index<MAX_EVENT_BIT_LENGTH; event_index++)
  {
    if (gmm_events == GMM_NO_EVENT)
    {
      break ; /* There is nothing to do */
    }

    /* Incoming GMM event */
    gmm_event = gmm_events & ((gmm_trigger_llc_type)0x01<<event_index) ;

    switch (gmm_event)
    {
      case GMM_TRIGGER_GMM_ATTACH_STARTED :
      case GMM_TRIGGER_GMM_ATTACHED :
      case GMM_TRIGGER_GMM_DETACHED :
      case GMM_TRIGGER_PTMSI_ASSIGNED :
      case GMM_TRIGGER_FORCE_TO_STANDBY :
      case GMM_TRIGGER_RAU_STARTED :
      case GMM_TRIGGER_RAU_FINISHED :
      case GMM_TRIGGER_NEW_READY_TMR_VALUE :
      case GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE :
      case GMM_TRIGGER_NO_READY_TMR_VALUE :
      case GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE :
      case GMM_TRIGGER_NEW_KC_VALUE :
      case GMM_TRIGGER_TLLI_UNASSIGNMENT:
      case GMM_TRIGGER_DETACH_STARTED :
      case GMM_TRIGGER_LLC_SUSPENSION :
      case GMM_TRIGGER_LLC_RESUMPTION :
      case GMM_TRIGGER_IMSI_DETACH_FINISHED :
      case GMM_TRIGGER_G_RAT_RESUMED :  
#ifdef FEATURE_GPRS_PS_HANDOVER
      case GMM_TRIGGER_PSHO_STARTED :
#endif
      case GMM_TRIGGER_GMM_STABILIZED :       
      {

          gmm_llc_if_state_type next_glif_state ;

          MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Trigger Event: %d",gmm_event) ;

          /* Execute incoming GMM event */
          next_glif_state = glif_execute_gmm_event(gmm_event,
                             glif_state, force_to_standby, ready_timer_value) ;

          if (next_glif_state != glif_state)
          {
            MSG_HIGH_DS_2( MM_SUB, "=MM= GLIF: State Change: %d to %d",
                                                glif_state,next_glif_state) ;

            glif_state = next_glif_state ;

            /* Notify GRR layer that the GPRS GMM state has been changed */
            glif_grr_state_exchange(next_glif_state) ;
          }
        }
        break ;

      default: continue ;
    }

    /* Erase recently executed GMM event ID from the event buffer */
    gmm_events = gmm_events & (gmm_trigger_llc_type)(~gmm_event) ;
  }

} /* end gmmllc_gmm_trigger_llc_event() */

/*===========================================================================

FUNCTION      GLIF_SEND_GMM_OTA_MESSAGE

DESCRIPTION
  This function sends GMM outgoing messages to the LLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_send_gmm_ota_message
(
  /* Message length */
  dword message_length,

  /* Message buffer */
  byte *message_body
#ifdef FEATURE_GPRS_PS_HANDOVER
  ,boolean purgeQueueFlag
#endif    
)
{
  gmmllc_ll_mes_type *glif_unitdata_req ;
  boolean             ciphering_indicator = FALSE ;
  byte                message_id  ;   
  boolean             gmm_unit_data_req_tobe_sent = TRUE;
#ifdef FEATURE_DUAL_SIM
  boolean             tds_active_on_other_sub = FALSE;
  int i;
#endif
  if ( message_body == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: NULL pointer ", 0,0,0 );
  }
  else
  {
    message_id = message_body[1];
  } 
  
  if (auth_gsm_gprs_cksn != DELETE_CKSN) /* If the CKSN is valid */
  {
    ciphering_indicator = TRUE ; /* encode an outgoing message */
  }
  else
  {
    ciphering_indicator = FALSE ; /* do not encode otherwise */
  }

  /* Set current TLLI */
  switch (message_id)
  {
    case GMM_ATTACH_REQUEST :
#ifdef FEATURE_NAS_FREE_FLOAT
      switch (glif_grr_ser_req_state)
#else
      switch (glif_grr_camp_state)
#endif
      {
#ifdef FEATURE_NAS_FREE_FLOAT
        case GLIF_NO_SERVICE:
          MSG_ERROR_DS_0(MM_SUB, "=MM= GLIF: Attach Request is canceled - no GRR service.") ;
          return ;
#else
        case GLIF_CAMP_ON_CCCH :
          MSG_ERROR_DS_0(MM_SUB, "=MM= GLIF: Attach Request is canceled - GRR is not pkt camped.") ;
          return ;
#endif
#ifdef FEATURE_NAS_FREE_FLOAT
        case GLIF_SERVICE_REQUESTED :
#else
        case GLIF_CAMP_ON_PCCCH_REQUESTED :
#endif

          glif_sending_gmm_message = TRUE ;

          glif_pended_attach_req = TRUE ;

          /* Save outgoing ATTACH request message */
#ifdef FEATURE_MODEM_HEAP
          if(( glif_pended_gmm_msg == NULL ) &&
             (( glif_pended_gmm_msg = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
          {
            mm_check_for_null_ptr((void*)glif_pended_gmm_msg);
          }
#endif
          memscpy((void *)glif_pended_gmm_msg, MAX_MESSAGE_SIZE,message_body,message_length) ;
          glif_buffered_message_length = message_length;
          return ;
#ifdef FEATURE_NAS_FREE_FLOAT
        case GLIF_SERVICE_CONFIRMED :
#else
        case GLIF_CAMP_ON_PCCCH_CONFIRMED :
#endif
        
          /* According to TS 24.008 4.7.1.2 this message is always sent
            unciphered. */
          ciphering_indicator = FALSE ;

          /* Attach Request already txed */
          glif_pended_attach_req = FALSE ;

          /* Build a packed MS RA CAP IE without spare bits and pass this
             to GRR for use in GPRS MAC Packet Resource Request messages. */
          glif_grr_notify_ms_ra_cap();

#ifdef FEATURE_DUAL_SIM
          if(MMCNM_IS_MODE_DUAL_STANDBY)
          {
            MSG_HIGH_DS_0(MM_SUB," =MM= Blocking tuneaway for ATTACH procedure");
            geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_GMM, TRUE, (sys_modem_as_id_e_type)mm_sub_id);
          }

#endif
          /* TS 4.60 5.5.1.5 Discontinuous reception (DRX).*/
          glif_enter_mm_non_drx_mode();
          break ;

        default :
#ifdef FEATURE_NAS_FREE_FLOAT
          MSG_ERROR_DS_1(MM_SUB, "=MM= GLIF: Illegal GRR camp state (%d)",glif_grr_ser_req_state) ;
          return ;
#else
          MSG_ERROR_DS_1(MM_SUB, "=MM= GLIF: Illegal GRR camp state (%d)",glif_grr_camp_state) ;
          return ;
#endif
      }
      break ;

    case GMM_ROUTING_AREA_UPDATE_REQUEST :
      gmm_rau_tx_is_acked = FALSE;
      switch (glif_llc_connection_state)
      {
        case LLC_RESUMED :
#ifdef FEATURE_GPRS_PS_HANDOVER         
         if (gmm_psho_status == GMM_PSHO_IN_PROGRESS)
         {
            /*  glif_grr_send_service_confirm() is not sent here as LLC is not being SUSPENDED*/

            /* According to TS 24.008 4.7.1.2 this message is always sent
              unciphered. */
            ciphering_indicator = FALSE ;

            /*be sure??*/
            glif_pended_rau_req = FALSE ;
          
            /* Build a packed MS RA CAP IE without spare bits and pass this
               to GRR for use in GPRS MAC Packet Resource Request messages. */
            glif_grr_notify_ms_ra_cap();
#ifdef FEATURE_DUAL_SIM
            if(MMCNM_IS_MODE_DUAL_STANDBY)
            {
              for(i=0; i < MAX_AS_IDS; i++)
              {
                if(mm_serving_plmn_sim[i].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
                {
                  tds_active_on_other_sub = TRUE;
                }
              }
              if(!tds_active_on_other_sub)
              {
                MSG_HIGH_DS_0(MM_SUB," =MM= Blocking tuneaway for RAU procedure");
                geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_GMM, TRUE, (sys_modem_as_id_e_type)mm_sub_id);
              }
            }
#endif
            /* TS 4.60 5.5.1.5 Discontinuous reception (DRX).*/
            glif_enter_mm_non_drx_mode();
         }
         else
#endif          
         {
          MSG_ERROR_DS_0(MM_SUB, "=MM= RAU Request is canceled, LLC's not been suspended.") ;
          gmm_unit_data_req_tobe_sent = FALSE;
         }
         break;
        case LLC_SUSPENSION_REQUESTED :
          glif_sending_gmm_message = TRUE ;

          glif_pended_rau_req = TRUE ;

          /* Save outgoing RAU request message */
#ifdef FEATURE_MODEM_HEAP
          if(( glif_pended_gmm_msg == NULL ) &&
             (( glif_pended_gmm_msg = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
          {
            mm_check_for_null_ptr((void*)glif_pended_gmm_msg);
          }
#endif
          memscpy((void *)glif_pended_gmm_msg, MAX_MESSAGE_SIZE,message_body,message_length) ;
          glif_buffered_message_length = message_length;

          return ;
        case LLC_SUSPENSION_CONFIRMED :
#ifdef FEATURE_NAS_FREE_FLOAT
          if( glif_grr_ser_req_state != GLIF_SERVICE_CONFIRMED)
          {
            glif_sending_gmm_message = TRUE ;
            glif_pended_rau_req = TRUE ;

            /* Save outgoing RAU request message */
#ifdef FEATURE_MODEM_HEAP
            if(( glif_pended_gmm_msg == NULL ) &&
               (( glif_pended_gmm_msg = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
            {
              mm_check_for_null_ptr((void*)glif_pended_gmm_msg);
            }
#endif
            memscpy((void *)glif_pended_gmm_msg, MAX_MESSAGE_SIZE,message_body,message_length) ;
            glif_buffered_message_length = message_length;
            return ;
          }
          else
#endif
          {

            
            /* According to TS 24.008 4.7.1.2 this message is always sent
              unciphered. */
            ciphering_indicator = FALSE ;

            /* RAU Request already txed */
            glif_pended_rau_req = FALSE ;

            /* Build a packed MS RA CAP IE without spare bits and pass this
               to GRR for use in GPRS MAC Packet Resource Request messages. */
            glif_grr_notify_ms_ra_cap();

#ifdef FEATURE_DUAL_SIM
            if(MMCNM_IS_MODE_DUAL_STANDBY)
            {
              for(i=0; i < MAX_AS_IDS; i++)
              {
                if(mm_serving_plmn_sim[i].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
                {
                  tds_active_on_other_sub = TRUE;
                }
              }
              if(!tds_active_on_other_sub)
              {
                MSG_HIGH_DS_0(MM_SUB," =MM= Blocking tuneaway for RAU procedure");
                geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_GMM, TRUE, (sys_modem_as_id_e_type)mm_sub_id);
              }
            }
#endif
            /* TS 4.60 5.5.1.5 Discontinuous reception (DRX).*/
            glif_enter_mm_non_drx_mode();
          }
          break ;
        default :
          MSG_ERROR_DS_1(MM_SUB, "=MM= Illegal LLC connection state (%d)",glif_llc_connection_state) ;
          return ;
      }

      break ;

    case GMM_AUTHENTICATION_AND_CIPHERING_RESPONSE:
    case GMM_IDENTITY_RESPONSE:
    case GMM_AUTHENTICATION_AND_CIPHERING_FAILURE:

      /* According to TS 24.008 4.7.1.2 these messages are always sent
         unciphered. */

      ciphering_indicator = FALSE ;

      break ;
    case GMM_ROUTING_AREA_UPDATE_COMPLETE :
      if (glif_foreign_tlli_assigned)
      {
        /*
        ** If during the RAU TLLI has been changed to Foreign, use
        ** Local TLLI in order to send RAU Complete message
        */
        glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;

        glif_foreign_tlli_assigned = FALSE ;
      }
      break ;
    case GMM_DETACH_REQUEST:
      switch (glif_llc_connection_state)
      {
        case LLC_RESUMED :
#ifdef FEATURE_GPRS_PS_HANDOVER         
          if ( (gmm_psho_status != GMM_PSHO_NONE) && (message_length > 2) && (message_body[2] & 0x02) )
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sending DETACH REQ Without Suspending LLC") ;    
          }
          else            
#endif
          {
            glif_pended_detach_req = TRUE ;

            /* Suspend MS' Tx */
            glif_llc_send_suspend_request() ;

            /* Save outgoing Detach Request message */
#ifdef FEATURE_MODEM_HEAP
            if((glif_pended_detach == NULL) &&
               ((glif_pended_detach = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
            {
              mm_check_for_null_ptr((void*)glif_pended_detach);
            }
#endif
            memscpy((void *)glif_pended_detach, MAX_MESSAGE_SIZE,message_body,message_length) ;
            glif_buffered_detach_length = message_length;

            gmm_unit_data_req_tobe_sent = FALSE;
          }
          break;
        case LLC_SUSPENSION_REQUESTED :
          glif_pended_detach_req = TRUE ;

          /* Save outgoing Detach Request message */
#ifdef FEATURE_MODEM_HEAP
          if((glif_pended_detach == NULL) &&
             ((glif_pended_detach = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
          {
            mm_check_for_null_ptr((void*)glif_pended_detach);
          }
#endif
          memscpy((void *)glif_pended_detach, MAX_MESSAGE_SIZE,message_body,message_length) ;
          glif_buffered_detach_length = message_length;

          return ;
        case LLC_SUSPENSION_CONFIRMED :

          /* DETACH Request already txed */
          glif_pended_detach_req = FALSE ;
          break ;
        default :
          MSG_ERROR_DS_1(MM_SUB, "=MM= Illegal LLC connection state (%d)",glif_llc_connection_state) ;
          return ;
      }

      break ;
    case GMM_DETACH_ACCEPT:
      gmm_detach_in_progress = TRUE;
      switch (glif_llc_connection_state)
      {
        case LLC_RESUMED :
          glif_pended_mt_detach_accept_ind = TRUE ;

          /* Suspend MS' Tx */
          glif_llc_send_suspend_request() ;

          /* Save outgoing Detach Accept message */
          memscpy((void *)glif_pended_mt_detach_accept, GLIF_BUFFERED_DETACH_ACCEPT_LENGTH,
                             message_body,GLIF_BUFFERED_DETACH_ACCEPT_LENGTH) ;

          return ;
        case LLC_SUSPENSION_REQUESTED :
          glif_pended_mt_detach_accept_ind = TRUE ;

          /* Save outgoing Detach Accept message */
          memscpy((void *)glif_pended_mt_detach_accept, GLIF_BUFFERED_DETACH_ACCEPT_LENGTH,
                             message_body,GLIF_BUFFERED_DETACH_ACCEPT_LENGTH) ;
          return ;
        case LLC_SUSPENSION_CONFIRMED :
          /* DETACH Accept already txed */
          glif_pended_mt_detach_accept_ind = FALSE ;
#ifdef FEATURE_NAS_FREE_FLOAT
          if(gmm_processed_mt_imsi_detach_request)
          {
            MSG_HIGH_DS_0( MM_SUB, "=MM= Timer 51 - TIMER_NW_INIT_DETACH is not started for IMSI detach");
          }
          else
          {
              mm_start_timer( TIMER_NW_INIT_DETACH,GLIF_DETACH_TIMER_VALUE);
          }
          gmm_processed_mt_imsi_detach_request = FALSE;
#endif   
          break ;
        default :
          MSG_ERROR_DS_1(MM_SUB, "=MM= Illegal LLC connection state (%d)",glif_llc_connection_state) ;
          return ;
      }

      break ;
    default :
      /* Currently there is nothing to do */
      break ;
  }

  /* Create LLC buffer */

  if (gmm_unit_data_req_tobe_sent)
  {
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM  
    uint8 transaction_id = 0;  

    uint8 protocol_discriminator = (message_body[0] & 0x0F);

    if (protocol_discriminator == PD_GPRS_SM) 
    { 
      transaction_id = (message_body[0] & 0xF0) >> 4;
    }
    else if(protocol_discriminator == PD_GMM)
    {
      transaction_id = ++gmm_transaction_id; 
      gmm_last_signalling_message_type = message_body[1]; 
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB,"Unknown protocol_discriminator %d",protocol_discriminator);
    }
   
#endif  /* FEATURE_GPRS_LAYER3_TX_CONFIRM */    
  
    if ((glif_unitdata_req = gmmllc_ll_get_mes_buf()) == NULL)
    {
      MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
    }

    /* Construct GMMLLC_LL_UNITDATA_REQ request */
    glif_unitdata_req->mes.ll_unitdata_req.prim = GMMLLC_LL_UNITDATA_REQ ;

    /* Allocate memory space for L3 message */
#ifndef FEATURE_MODEM_HEAP
    glif_unitdata_req->mes.ll_unitdata_req.l3_pdu = (uint8 *)gs_alloc(message_length) ;
#else
    glif_unitdata_req->mes.ll_unitdata_req.l3_pdu = (uint8 *)modem_mem_calloc(1,message_length, MODEM_MEM_CLIENT_NAS) ;
#endif
    if (glif_unitdata_req->mes.ll_unitdata_req.l3_pdu == NULL)
    {
      MSG_FATAL_DS( MM_SUB, "=MM= GLIF: Heap Exhaustion", 0, 0, 0 );
    }

    /* Put outgoing message into LLC unit data request */
    glif_unitdata_req->mes.ll_unitdata_req.l3_pdu_len =
                                                 (unsigned short)message_length ;
    memscpy((void *)glif_unitdata_req->mes.ll_unitdata_req.l3_pdu, message_length,
                                          (void *)message_body,message_length) ;

    /* Assign currently available TLLI */
    glif_unitdata_req->mes.ll_unitdata_req.tlli =
      GLIF_XLATE_TLLI(glif_tlli);

    /* Set ciphering indicator for current outgoing message */
    glif_unitdata_req->mes.ll_unitdata_req.ciphered = ciphering_indicator ;
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM      
    glif_unitdata_req->mes.ll_unitdata_req.protocol_disc = protocol_discriminator ;
    glif_unitdata_req->mes.ll_unitdata_req.transaction_id = transaction_id ;

    MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: Sent GMMLLC_LL_UDATA_REQ TO LLC %d, %d",protocol_discriminator, transaction_id);
#else
    MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Sent GMMLLC_LL_UDATA_REQ TO LLC");
#endif  /* FEATURE_GPRS_LAYER3_TX_CONFIRM */        

#ifdef FEATURE_DUAL_SIM
    glif_unitdata_req->mes.ll_unitdata_req.as_id= (sys_modem_as_id_e_type)mm_sub_id;
#endif

    
    /* for tracking the RAT on which last signalling information is sent */
   gmm_last_signalling_sent_rat = SYS_RAT_GSM_RADIO_ACCESS;
   mm_log_mm_msg_info((byte)(message_body[0] & 0x0F),message_body[1]);
   mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_unitdata_req->mes.ll_unitdata_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
        );

    /*
    ** Send the GMM's (upper layer's) signalling as LLC DATAUNIT request
    */
    gmmllc_ll_put_mes(glif_unitdata_req) ;
  }
  return;
  
} /* end glif_send_gmm_ota_message() */

/*===========================================================================

FUNCTION      GLIF_SEND_SMS_OTA_MESSAGE

DESCRIPTION
  This function sends SMS outgoing messages to the LLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_send_sms_ota_message
(
  dword message_length, /* Message length */
  byte *message_body    /* Message */
)
{
  gsmsllc_ll_mes_type *glif_unitdata_req ;

  /* Create LLC primitive */
  if ((glif_unitdata_req = gsmsllc_ll_get_mes_buf()) == NULL)
  {
    MSG_FATAL_DS(MM_SUB, "=MM= GLIF: No LLC Msg Buffers",0,0,0);
  }

  /* Construct GSMSLLC_LL_UNITDATA_REQ request primitive */
  glif_unitdata_req->mes.ll_unitdata_req.prim = GSMSLLC_LL_UNITDATA_REQ ;

  /* Allocate memory space for SMS message */
#ifndef FEATURE_MODEM_HEAP
  glif_unitdata_req->mes.ll_unitdata_req.l3_pdu = (uint8 *)gs_alloc(message_length) ;
#else
  glif_unitdata_req->mes.ll_unitdata_req.l3_pdu = (uint8 *)modem_mem_calloc(1,message_length, MODEM_MEM_CLIENT_NAS) ;
#endif
  if (glif_unitdata_req->mes.ll_unitdata_req.l3_pdu == NULL)
  {
    mm_check_for_null_ptr((void*)glif_unitdata_req->mes.ll_unitdata_req.l3_pdu);
  }

  /* Put the outgoing message into LLC unit data request */
  glif_unitdata_req->mes.ll_unitdata_req.l3_pdu_len =
                                               (unsigned short)message_length ;
  memscpy((void *)glif_unitdata_req->mes.ll_unitdata_req.l3_pdu,message_length,
                                        (void *)message_body,message_length) ;

  /* Assign appropriate TLLI to the outgoing LLC UNITDATA request */
  glif_unitdata_req->mes.ll_unitdata_req.tlli = GLIF_XLATE_TLLI(glif_tlli);

  /* Set ciphering indicator for current outgoing message */
  glif_unitdata_req->mes.ll_unitdata_req.ciphered = TRUE ;

  /* Set SMS radio priority */
  glif_unitdata_req->mes.ll_unitdata_req.radio_priority = sms_radio_priority ;

#ifdef FEATURE_DUAL_SIM
  glif_unitdata_req->mes.ll_unitdata_req.as_id = (sys_modem_as_id_e_type)mm_sub_id ;
#endif

  MSG_HIGH_DS_0(MM_SUB, "=MM= Sent GSMSLLC_LL_UDATA_REQ to LLC") ;

  /* for tracking the RAT on which last signalling information is sent */
  gmm_last_signalling_sent_rat = SYS_RAT_GSM_RADIO_ACCESS;

  mm_add_message_to_debug_buffer(MS_LLC_GMM,
              (dword)(glif_unitdata_req->mes.ll_unitdata_req.prim),
              FROM_MM
              ,(sys_modem_as_id_e_type)mm_as_id
       );

  /*
  ** Send SMS message as a LLC DATAUNIT request
  */
  gsmsllc_ll_put_mes(glif_unitdata_req) ;
} /* end glif_send_sms_ota_message() */

/*===========================================================================

FUNCTION      GMMLLC_GMM_SEND_OTA_MESSAGE

DESCRIPTION
  This is GMM-LLC module's interface function which is used in order to send
  any upper layer messages out to the GSM network in GPRS mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmmllc_gmm_send_ota_message
(
  dword message_length, /* message length */
  byte *message_body    /* message */
#ifdef FEATURE_GPRS_PS_HANDOVER
  ,boolean purgeQueueFlag
#endif  
)
{
  byte protocol_discriminator = 0;
  if(message_body != NULL)
  {
    protocol_discriminator = (byte) (message_body[0] & 0x0F);
  }

  /* Evaluate Protocol Descriminator */
  switch (protocol_discriminator)
  {
    case PD_SMS: /* Send SMS message */
      glif_send_sms_ota_message(message_length,message_body) ;
      break ;
    case PD_GMM: /* Send GMM message */
    case PD_GPRS_SM: /* Send SM message */
      glif_send_gmm_ota_message(message_length,
                                message_body
#ifdef FEATURE_GPRS_PS_HANDOVER
                                ,purgeQueueFlag
#endif  
                               ) ;
      break ;
    default :
      MSG_ERROR_DS_0(MM_SUB, "=MM= Unexpected message is to be sent") ;
      break ;
  }

  if ((protocol_discriminator == PD_SMS ||
      protocol_discriminator == PD_GMM ||
      protocol_discriminator == PD_GPRS_SM) &&
     (message_length <= LOG_MAX_NAS_OTA_MESSAGE_SIZE))
  {
    /* Send the NAS signalling message log packet to diag */
    mm_send_ota_msg_log_packet(UE_TO_CN_DIRECTION,message_length,message_body);
  }    
  if((message_body != NULL) && (protocol_discriminator == PD_GPRS_SM))
  {
#ifndef FEATURE_MODEM_HEAP
    gs_free( message_body);
    message_body = NULL;
#else
    modem_mem_free( message_body , MODEM_MEM_CLIENT_NAS);
    message_body = NULL;
#endif
  }
} /* end gmmllc_gmm_send_ota_message() */

/*===========================================================================

FUNCTION      GLIF_PROCESS_GRR_PAGING_INDICATION

DESCRIPTION
  This function processes GRR Paging indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_process_grr_page_indication
(
  /* Pointer to the GRR Paging Indication */
  mm_cmd_type     *primitive
)
{
  if (primitive->cmd.rr_paging_ind.paging_type == PS_PAGING)
  {
    if ((gmm_rau_is_required) || ((!gmm_plmn_changed(gmm_previous_registration_serving_rai.plmn_id,mm_serving_plmn.info.plmn)) &&
                                    (!gmm_rai_changed( gmm_previous_registration_serving_rai.location_area_code,
                                    gmm_previous_registration_serving_rai.routing_area_code,mm_serving_plmn.lac, mm_serving_plmn.rac ))
                                    &&(gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED)
                                    &&((mm_timer_status[ TIMER_T3311] == TIMER_ACTIVE) || (mm_timer_status[ TIMER_T3302] == TIMER_ACTIVE)
#ifdef FEATURE_NAS_REL10
                                       || (mm_timer_status[ TIMER_T3346] == TIMER_ACTIVE)
#endif
                                   ))
  )
    {
      /*
      ** If during RAT change MS is paged, GMM shall perform RAU
      ** procedure upon reception of the RR Paging Indication before
      ** sending any other signaling messages
      */

#ifdef FEATURE_NAS_REL10
      if((mm_timer_status[ TIMER_T3346] == TIMER_STOPPED) || 
         ((gmm_state == GMM_REGISTERED) &&
          (gmm_substate == GMM_ATTEMPTING_TO_UPDATE)))
#endif
      {
        /* Initiate RAU */
        if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
            (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
        {
          gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
        }
        else
        {
          gmm_ra_update_type = GMM_RA_UPDATING;
        }
           /**--------------------------------------------------------------------------
        **The normal routing area updating procedure is initiated due to the reception
        **of the paging indication using PTMSI while T3346 is running, 
        **the "follow-on request pending" indication shall be set to 1. 
        **(24.008 section 4.7.5.1) --------------------------------------------------*/

#ifdef FEATURE_NAS_REL10
                mm_stop_timer(TIMER_T3346);
                gmm_is_congestion_due_to_rau = FALSE;
                gmm_follow_on_request = TRUE;
#endif
        gmm_initiate_routing_area_update() ;
      }
    }
    else if ((gmm_update_status != GMM_GU1_UPDATED) ||
                 (gmm_rai_changed(gmm_stored_rai.location_area_code, gmm_stored_rai.routing_area_code, 
                        mm_serving_plmn.lac, mm_serving_plmn.rac)) ||
                 (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Ignoring GRR PAGE IND rai changed or not updated in curr rai") ;    
    }
    else if ((gmm_state == GMM_DEREGISTERED_INITIATED) && (gmm_power_down_active))
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Ignoring GRR PAGE IND: POWER OFF DEREG INITIATED") ;
    }
    else
    {
      /*
      ** Upon reception of the GRR Paging indication when the GRR
      ** indicates the TLLI paging mode the GMM-LLC interface sends
      ** the LLC Trigger request with GMMLLC_PAGE_RESPONSE indication
      */
#ifdef FEATURE_NAS_REL10
       mm_stop_timer(TIMER_T3346);
       gmm_is_congestion_due_to_rau = FALSE;
#endif
       glif_send_llc_trigger_request(GMMLLC_PAGE_RESPONSE);
    }
  }
  else
  {
    /* --------------------------------------------------------------------------
    ** Upon reception of a paging indication for GPRS services using IMSI, the MS
    ** shall locally deactivate any active PDP contexts and locally detach from
    ** GPRS.  The local detach includes deleting any RAI, P-TMSI, P-TMSI
    ** signature and GPRS ciphering key sequence number stored, setting the
    ** GPRS update status to GU2 NOT UPDATED and changing state to
    ** GMM-DEREGISTERED.
    ** -------------------------------------------------------------------------- */
    mm_send_sm_unblock_all_apn();
    gmm_process_abnormal_page_ind() ;
  }
} /* end glif_process_grr_page_indication() */


/*===========================================================================

FUNCTION      GLIF_PROCESS_RR_SERVICE_INDICATION

DESCRIPTION
  This function processes RR service indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void glif_process_rr_service_indication
(
  /* Pointer to the RR Service Indication */
  mm_cmd_type     *primitive
)
{

#ifndef FEATURE_NAS_FREE_FLOAT
  ue_rev_level_indicator_type revision_level;
#endif
  boolean routine_man_selection_failed = FALSE ; 

  boolean is_cell_forbidden = FALSE;

  if (!((PLMN_MATCH( gmm_current_lai.PLMN_id.identity,
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
        (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
         sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))  &&
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    routine_man_selection_failed = TRUE;
    MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
  }

  if( (mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
      ((routine_man_selection_failed) ||
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                               SYS_NETWORK_SELECTION_MODE_LIMITED)||
        ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) ||
          ((!gmm_manual_mode_user_selection_procedure)&&
           ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_MANUAL) ||
            (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))))&& 
          (
#ifdef FEATURE_DUAL_SIM
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
          (reg_sim_per_subs_plmn_forbidden(gmm_current_lai.PLMN_id,(sys_modem_as_id_e_type)mm_sub_id )) ||
#else
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) ||
          (reg_sim_plmn_forbidden(gmm_current_lai.PLMN_id)) ||
#endif
          (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
          (mm_check_forbidden_regional_roaming( &gmm_current_lai ))))))
  {
    is_cell_forbidden = TRUE;

    MSG_HIGH_DS_0(MM_SUB, "=MM= Cell is forbidden");
  }
  
  if (glif_sending_gmm_message == FALSE)
  {
    if (
         (primitive->cmd.rr_service_ind.service_state  == MM_AS_SERVICE_AVAILABLE           ) &&
#ifdef FEATURE_GSM_DTM
         ( (mm_state == MM_IDLE) || (mm_serving_plmn.dtm_supported == TRUE)                 ) &&
#else
         (mm_state                                     == MM_IDLE                           ) &&
#endif /* FEATURE_GSM_DTM */
         (mm_serving_plmn.info.plmn_service_capability != SYS_SRV_CAPABILITY_CS_SERVICE_ONLY) &&
         (  gmm_state                                    != GMM_DEREGISTERED                ) &&
         (mm_serving_plmn.gmm_service_state            == SERVICE_AVAILABLE                 ) &&
         (!is_cell_forbidden)                               &&
         (
           (primitive->cmd.rr_service_ind.cell_access == MM_AS_ACCESS_ALL_CALLS  ) ||
           (primitive->cmd.rr_service_ind.cell_access == MM_AS_ACCESS_NORMAL_ONLY)
         )
       )
    {
      /* If GMM Substate is not suspended due to CS activities (eg LAU) then a decision
      ** on cell update can be taken immediately. However, to cater for a race condition
      ** whereby GMM has not been notified of GPRS Suspended condition yet before processing
      ** this message, further qualify this condition with checks for GMM having already
      ** taken a decision to hold Attach, RAU or detach activities pending until resumption
      ** (this can be taken as an indication that entry to the GMM suspended state is
      ** expected imminently).
      */

      if ( (gmm_substate != GMM_SUSPENDED) &&
           (!gmm_is_suspended) &&
           (!gmm_gprs_attach_pending     ) &&
           (!gmm_ra_update_pending       ) &&
           (!gmm_detach_after_rau_pending) &&
           (!gmm_gprs_detach_pending     )    )
      {
          /* 04.18 3.5.2.1.2 - Packet Access Procedures -
             If the Ready Timer was running when reselection was indicated, and has since
             expired and changed GMM state to Standby, we must still consider processing
             a Cell Update. */

        if ( (primitive->cmd.rr_service_ind.cell_changed) &&
             (!glif_attach_in_progress                  ) &&
             (!glif_rau_in_progress                     ) &&
             (!glif_detach_in_progress               ) &&
             (!glif_force_to_standby                    ) &&
             (
               ((glif_state == GMM_STANDBY_STATE) && glif_rdy_tmr_active_at_reselection) ||
               (glif_state == GMM_READY_STATE                                          )
             )
           )
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Starting Cell Update") ;

          glif_process_cell_update(FALSE) ;
        }

        /* 24.008 clause 4.7.3.1.5 (e): In GSM, if a cell change occurs within the same RA,
           when the MS is in state GMM-REGISTERED-INITIATED, the cell update
           procedure is performed, before completion of the attach procedure.

           24.008 clause 4.7.5.1.5 (f): In GSM, if a cell change occurs within the same RA,
           when the MS is in state GMM-ROUTING-AREA-UPDATE-INITIATED, the cell update
           procedure is performed, before completion of the routing area updating procedure.

           This condition checks for a cell change within the same RA during ongoing
           Attach or RAU procedure. */

        else if
        (
          (primitive->cmd.rr_service_ind.cell_changed           ) &&
          (glif_attach_in_progress || glif_rau_in_progress || glif_detach_in_progress) &&
          (!glif_force_to_standby                               ) &&
#ifdef FEATURE_GPRS_PS_HANDOVER
          (((gmm_psho_status == GMM_PSHO_IN_PROGRESS) && (glif_llc_connection_state == LLC_RESUMED)) ||
           ((gmm_psho_status == GMM_PSHO_NONE) && (glif_llc_connection_state == LLC_SUSPENSION_CONFIRMED))) &&
#else
          (glif_llc_connection_state == LLC_SUSPENSION_CONFIRMED) &&
#endif 
          (!((gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                               gmm_previous_serving_rai.routing_area_code,
                               mm_serving_plmn.lac, mm_serving_plmn.rac)) ||
             (gmm_plmn_changed( gmm_previous_serving_rai.plmn_id,
                                mm_serving_plmn.info.plmn)))
          )                     
        )
        {           
          /* Due to the ongoing Attach or RAU Procedure, the LLC connection state is already
             suspended, and so we simply need to confirm the service indication to
             GRR and trigger an explicit Cell Update request to handle this case,
             whilst allowing the ongoing Attach or RAU procedure to continue. */
#ifdef FEATURE_NAS_FREE_FLOAT
          glif_grr_send_service_req() ;
          glif_pended_cell_update = TRUE ;
#else
          glif_grr_send_service_confirm() ;

          revision_level = get_revision_level_indicator();

          glif_send_llc_trigger_request(
                ((byte)revision_level & cell_notification_ind) ?
                                         GMMLLC_CELL_NOTIFY:GMMLLC_CELL_UPDATE) ;
          glif_pended_cell_update = FALSE ;
#endif
          glif_pended_cell_update_is_initial = FALSE ;
            
#ifdef FEATURE_GPRS_PS_HANDOVER                             
          MSG_HIGH_DS_0(MM_SUB, "=MM= cell update sent without suspending LLC - PSHO in progress") ;
#endif 

        }
#ifdef FEATURE_NAS_FREE_FLOAT
        else
        {
           MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: trying to send service request") ;

          if ((glif_llc_connection_state != LLC_SUSPENSION_REQUESTED) &&
              (GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF)
#ifdef FEATURE_GPRS_PS_HANDOVER        
              &&  (gmm_psho_status != GMM_PSHO_IN_PROGRESS)
#endif
             )
          {
            glif_grr_send_service_req() ;
          }
          else
          {
            glif_service_req_pended_ind = TRUE ;

            MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service request (%d,%d)",glif_grr_ser_req_state,glif_llc_connection_state) ;
          }
        }
#else
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: trying to send service confirm") ;

          if ((glif_grr_camp_state != GLIF_CAMP_ON_PCCCH_REQUESTED) &&
              (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED) 
#ifdef FEATURE_GPRS_PS_HANDOVER        
              &&  (gmm_psho_status != GMM_PSHO_IN_PROGRESS)
#endif
             )
          {
            glif_grr_send_service_confirm() ;
          }
          else
          {
            glif_service_confirm_pended_ind = TRUE ;

            MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service confirm (%d,%d)",glif_grr_camp_state,glif_llc_connection_state) ;
          }
        }
#endif
      }
      else if ( primitive->cmd.rr_service_ind.cell_changed )
      {
        /* 04.18 3.5.2.1.2 - Packet Access Procedures -
        ** If the Ready Timer was running when reselection was indicated, and has since
        ** expired and changed GMM state to Standby, we must still consider processing
        ** a Cell Update.

        ** Cell has changed but GMM is in the Suspended substate,
        ** so postpone any decision on cell update until GPRS resumption
        ** is signalled.
        */

        if((!glif_attach_in_progress && !glif_rau_in_progress && !glif_detach_in_progress && !glif_force_to_standby)&&
           (((glif_state == GMM_STANDBY_STATE) && glif_rdy_tmr_active_at_reselection) ||
            (glif_state == GMM_READY_STATE)))
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: cell update pended") ;
          glif_pended_cell_update = TRUE;
        }
        else if((glif_attach_in_progress || glif_rau_in_progress || glif_detach_in_progress) &&
                (!glif_force_to_standby) &&
#ifdef FEATURE_GPRS_PS_HANDOVER
                (((gmm_psho_status == GMM_PSHO_IN_PROGRESS) && (glif_llc_connection_state == LLC_RESUMED)) ||
                 ((gmm_psho_status == GMM_PSHO_NONE) && (glif_llc_connection_state == LLC_SUSPENSION_CONFIRMED))) &&
#else
               (glif_llc_connection_state == LLC_SUSPENSION_CONFIRMED) &&
#endif 
               (!((gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                                   gmm_previous_serving_rai.routing_area_code,
                                   mm_serving_plmn.lac, mm_serving_plmn.rac)) ||
                  (gmm_plmn_changed( gmm_previous_serving_rai.plmn_id,
                                    mm_serving_plmn.info.plmn)))
              ))
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: cell update pended") ;
          glif_pended_cell_update = TRUE;
        }
        else
        {
#ifdef FEATURE_NAS_FREE_FLOAT
          MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service req (%d,%d)",glif_grr_ser_req_state,glif_llc_connection_state) ;
#else
          MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service confirm (%d,%d)",glif_grr_camp_state,glif_llc_connection_state) ;
#endif
        }
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Current cell has not been changed") ;
      }
    }
    else
    {
      if (
           (primitive->cmd.rr_service_ind.cell_changed           ) &&
           (primitive->cmd.rr_service_ind.service_state  == MM_AS_SERVICE_AVAILABLE           ) &&
#ifdef FEATURE_GSM_DTM
           ( (mm_state != MM_IDLE) && (mm_serving_plmn.dtm_supported == FALSE)                 ) &&
#else
           (mm_state                                     == MM_IDLE                           ) &&
#endif /* FEATURE_GSM_DTM */
           (mm_serving_plmn.info.plmn_service_capability != SYS_SRV_CAPABILITY_CS_SERVICE_ONLY) &&
           (  gmm_state                                    != GMM_DEREGISTERED                ) &&
           (mm_serving_plmn.gmm_service_state            == SERVICE_AVAILABLE                 ) &&
           (!is_cell_forbidden)                               &&
           (
             (primitive->cmd.rr_service_ind.cell_access == MM_AS_ACCESS_ALL_CALLS  ) ||
             (primitive->cmd.rr_service_ind.cell_access == MM_AS_ACCESS_NORMAL_ONLY)
           ) &&
           ( ((glif_state == GMM_STANDBY_STATE) && glif_rdy_tmr_active_at_reselection) ||
             (glif_state == GMM_READY_STATE)                                              )           
         )
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: cell update pended - rr_srv_ind_ota_msg with cell chnge in non-dtm cell") ;

        glif_pended_cell_update = TRUE;      
      }
#ifdef FEATURE_NAS_FREE_FLOAT
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: trying to send service request") ;

        if ((glif_llc_connection_state != LLC_SUSPENSION_REQUESTED) &&
            (GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF))
        {
          glif_grr_send_service_req() ;
        }
        else
        {
          glif_service_req_pended_ind = TRUE ;

          MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service req (%d,%d)",glif_grr_ser_req_state,glif_llc_connection_state) ;
        }
      }
#else
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: trying to send service confirm") ;

        if ((glif_grr_camp_state != GLIF_CAMP_ON_PCCCH_REQUESTED) &&
            (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED))
        {
          glif_grr_send_service_confirm() ;
        }
        else
        {
          glif_service_confirm_pended_ind = TRUE ;

          MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service confirm (%d,%d)",glif_grr_camp_state,glif_llc_connection_state) ;
        }
      }
#endif
    }
  }
  else
  {
 #ifdef FEATURE_NAS_FREE_FLOAT
      glif_sending_gmm_message = FALSE ;
      MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: trying to send service request") ;
      if ((glif_llc_connection_state != LLC_SUSPENSION_REQUESTED) &&
           (GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF))
      {
        glif_grr_send_service_req() ;
      }
      else
      {
        glif_service_req_pended_ind = TRUE ;
        MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service req (%d,%d)",glif_grr_ser_req_state,glif_llc_connection_state) ;
      }
     
 #else
    MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: trying to send service confirm") ;

    glif_sending_gmm_message = FALSE ;

    if ((glif_grr_camp_state != GLIF_CAMP_ON_PCCCH_REQUESTED) &&
        (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED))
    {
      glif_grr_send_service_confirm() ;
    }
    else
    {
      glif_service_confirm_pended_ind = TRUE ;

      MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service confirm (%d,%d)",glif_grr_camp_state,glif_llc_connection_state) ;
    }
#endif
  }

} /* end glif_process_rr_service_indication() */


/*===========================================================================

FUNCTION      GLIF_PROCESS_RR_SERVICE_IND_FROM_OTA_MSG

DESCRIPTION
  This function processes RR service ind from OTA msg by translating to an
  RR service ind and redirecting to glif_process_rr_service_indication()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_GSM_DTM
void glif_process_rr_service_ind_from_ota_msg
(
  /* Pointer to the RR Service Indication */
  mm_cmd_type     *primitive
)
{
  mm_cmd_type *prim;
  prim = (mm_cmd_type*)mm_get_cmd_buf();

  if (prim == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= GLIF: Heap Exhaustion", 0, 0, 0 );
  }
  memscpy
  (
    &prim->cmd.rr_service_ind, sizeof(rr_service_ind_T),
    &primitive->cmd.rr_service_ind_from_ota_msg,
    sizeof(rr_service_ind_T)
  );

  glif_process_rr_service_indication( prim );
  mm_free_cmd_buf( prim );

} /* end of glif_process_rr_service_ind_from_ota_msg */
#endif /* FEATURE_GSM_DTM */


/*===========================================================================

FUNCTION      GLIF_PROCESS_RR_GPRS_RESUMPTION_INDICATION

DESCRIPTION
  This function processes RR GPRS Resumption indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void glif_process_rr_gprs_resumption_indication( boolean is_cell_update_required )
{

  glif_sending_gmm_message = FALSE ;

  /* If a decision on cell update has been postponed until GPRS resumption,
     and if on resumption an Attach or RAU has not been scheduled, then
     initiate cell update. */
      if (gmm_registration_initiated == FALSE) 
      {   
  if ((glif_pended_cell_update || is_cell_update_required) &&
#ifdef FEATURE_GSM_DTM
      ((mm_state == MM_IDLE) || (mm_serving_plmn.dtm_supported == TRUE))
#else
      (mm_state == MM_IDLE)
#endif /* FEATURE_GSM_DTM */   
     )
  {
          if(!glif_attach_in_progress && !glif_rau_in_progress && !glif_detach_in_progress && !glif_force_to_standby)
    {
            MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Starting Cell Update") ;
      glif_process_cell_update(glif_pended_cell_update_is_initial) ;
    }
          else if((glif_attach_in_progress || glif_rau_in_progress || glif_detach_in_progress) &&
                  (!((gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                                       gmm_previous_serving_rai.routing_area_code,
                                       mm_serving_plmn.lac, mm_serving_plmn.rac)) ||
                  (gmm_plmn_changed( gmm_previous_serving_rai.plmn_id,
                                     mm_serving_plmn.info.plmn)))
                  ))
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Starting Cell Update") ;
            glif_process_cell_update(glif_pended_cell_update_is_initial) ;
          }
    else
    {
#ifdef FEATURE_NAS_FREE_FLOAT

            MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: trying to send service request") ;

      if ((glif_llc_connection_state != LLC_SUSPENSION_REQUESTED) && 
          (glif_grr_ser_req_state == GLIF_NO_SERVICE) &&
          (GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF))
      {
        glif_grr_send_service_req() ;
      }
      else if (glif_llc_connection_state == LLC_SUSPENSION_REQUESTED && glif_grr_ser_req_state == GLIF_NO_SERVICE)
      {
        glif_service_req_pended_ind = TRUE ;

        MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service req (%d,%d)",glif_grr_ser_req_state,glif_llc_connection_state) ;
      }
#else

            MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: trying to send service confirm") ;

      if ((glif_grr_camp_state != GLIF_CAMP_ON_PCCCH_REQUESTED) &&
          (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED))
      {
        glif_grr_send_service_confirm() ;
      }
            else if (glif_llc_connection_state == LLC_SUSPENSION_REQUESTED && glif_grr_ser_req_state == GLIF_NO_SERVICE)
            {
              glif_service_confirm_pended_ind = TRUE ;

              MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service confirm (%d,%d)",glif_grr_camp_state,glif_llc_connection_state) ;
            }
#endif
      glif_pended_cell_update = FALSE ;
      glif_pended_cell_update_is_initial = FALSE ;
    }
  }
       else
       {
#ifdef FEATURE_NAS_FREE_FLOAT
          if ((glif_llc_connection_state != LLC_SUSPENSION_REQUESTED) && 
              (glif_grr_ser_req_state == GLIF_NO_SERVICE)&&
              (GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF))
          {
            glif_grr_send_service_req() ;
          }
          else if (glif_llc_connection_state == LLC_SUSPENSION_REQUESTED && 
                   glif_grr_ser_req_state    == GLIF_NO_SERVICE)
          {
            glif_service_req_pended_ind = TRUE ;
            MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service req (%d,%d)",glif_grr_ser_req_state,glif_llc_connection_state) ;
          }
#else
          if ((glif_grr_camp_state != GLIF_CAMP_ON_PCCCH_REQUESTED) &&
              (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED))
          {
            glif_grr_send_service_confirm() ;
          }
#endif
       }
  }
#ifdef FEATURE_NAS_FREE_FLOAT
  else if ((glif_llc_connection_state != LLC_SUSPENSION_REQUESTED) && 
           (glif_grr_ser_req_state == GLIF_NO_SERVICE) &&
           (GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF))
  {
    glif_grr_send_service_req() ;
  }
  else if (glif_llc_connection_state == LLC_SUSPENSION_REQUESTED && glif_grr_ser_req_state == GLIF_NO_SERVICE)
  {
    glif_service_req_pended_ind = TRUE ;

    MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service req (%d,%d)",glif_grr_ser_req_state,glif_llc_connection_state) ;
  }
#else
  else if ((glif_grr_camp_state != GLIF_CAMP_ON_PCCCH_REQUESTED) &&
           (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED))
  {
    glif_grr_send_service_confirm() ;
  }
  else
  {
    glif_service_confirm_pended_ind = TRUE ;

    MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service confirm (%d,%d)",glif_grr_camp_state,glif_llc_connection_state) ;
  }
#endif

  gmm_registration_initiated = FALSE;

} /* end glif_process_rr_gprs_resumption_indication() */

#ifndef FEATURE_NAS_FREE_FLOAT
/*===========================================================================

FUNCTION      GLIF_PROCESS_RR_GPRS_PCCCH_CAMP_CNF

DESCRIPTION
  This function processes RR GPRS PCCCH CAMPED confirmation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void glif_process_rr_gprs_pccch_camp_cnf( void )
{
  boolean routine_man_selection_failed = FALSE ;
  boolean is_cell_forbidden = FALSE;
  
  MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Rcvd RR_GPRS_PCCCH_CAMP_CNF") ;

  
  if (!((PLMN_MATCH( gmm_current_lai.PLMN_id.identity,
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
        (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
         sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))  &&
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    routine_man_selection_failed = TRUE;
    MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
  }
  if( (mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
      ((routine_man_selection_failed) ||
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                               SYS_NETWORK_SELECTION_MODE_LIMITED)||
        ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) ||
          ((!gmm_manual_mode_user_selection_procedure)&&
           ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_MANUAL) ||
            (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))))&& 
         (
#ifdef FEATURE_DUAL_SIM
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
          (reg_sim_per_subs_plmn_forbidden(gmm_current_lai.PLMN_id,(sys_modem_as_id_e_type)mm_sub_id )) ||
#else
          (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) ||
          (reg_sim_plmn_forbidden(gmm_current_lai.PLMN_id)) ||
#endif
          (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
          (mm_check_forbidden_regional_roaming( &gmm_current_lai ))))))
  {
    is_cell_forbidden = TRUE;

    MSG_HIGH_DS_0(MM_SUB, "=MM= Cell is forbidden");
  }

  switch (glif_grr_camp_state)
  {
    case GLIF_CAMP_ON_PCCCH_REQUESTED :

      glif_grr_camp_state = GLIF_CAMP_ON_PCCCH_CONFIRMED ;

      MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: glif_grr_camp_state = GLIF_CAMP_ON_PCCCH_CONFIRMED") ;

      /* Make sure we are not waiting for LLC User Plane Suspension Confirm before releasing
         Pending OTA messaging */
      if (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED)
      {
         if (!is_cell_forbidden)
         {
           glif_process_pended_gmm_commands() ;
         }
         else
         {
           if (glif_pended_mt_detach_accept_ind)
            {
              glif_grr_send_service_confirm() ;
                 
              /* Send pended MT Detach Accept message */
              glif_send_gmm_ota_message(GLIF_BUFFERED_DETACH_ACCEPT_LENGTH,
                                                                 glif_pended_mt_detach_accept
 #ifdef FEATURE_GPRS_PS_HANDOVER
                                                                 ,FALSE
 #endif                                                  
                                       ) ;
                 
              glif_pended_mt_detach_accept_ind = FALSE ;
            }
            else if (glif_service_confirm_pended_ind)
            {
              /* Permit user plane transfers from now on */
              glif_grr_send_service_confirm();
            }
         }

         glif_sending_gmm_message = FALSE;
      }
      break ;

    case GLIF_CAMP_ON_CCCH :
    case GLIF_CAMP_ON_PCCCH_CONFIRMED :

      MSG_ERROR_DS_0(MM_SUB, "=MM= GLIF: Unexp RR_GPRS_PCCCH_CAMP_CNF ") ;
      break;

    default :
      MSG_ERROR_DS_1(MM_SUB, "=MM= GLIF: Illegal GRR camp state (%d)",glif_grr_camp_state) ;
  }

} /* end glif_process_rr_gprs_pccch_camp_cnf() */

#endif
/*===========================================================================

FUNCTION      GLIF_PROCESS_RR_GPRS_SERVICE_CNF

DESCRIPTION
  This function processes RR GPRS PCCCH CAMPED confirmation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void glif_process_rr_gprs_service_cnf( rr_gmm_gprs_service_cnf_T rr_gprs_service_cnf)
{
  MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Rcvd RR_GPRS_SERVICE_CNF") ;

  
  switch (glif_grr_ser_req_state)
  {
    case GLIF_SERVICE_REQUESTED :

      glif_grr_ser_req_state = GLIF_SERVICE_CONFIRMED ;

      glif_sending_gmm_message = FALSE;

      MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: glif_grr_service_req_state = GLIF_SERVICE_CONFIRMED") ;

      /* Make sure we are not waiting for LLC User Plane Suspension Confirm before releasing
         Pending OTA messaging */
      if (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED && gmm_substate != GMM_SUSPENDED)
      {
        glif_process_pended_gmm_commands() ;
      }
      break ;

    case GLIF_NO_SERVICE:
    case GLIF_SERVICE_CONFIRMED :

      MSG_ERROR_DS_0(MM_SUB, "=MM= GLIF: Unexp RR_GPRS_SERVICE_CNF ") ;
      break;

    default :
      MSG_ERROR_DS_1(MM_SUB, "=MM= GLIF: Illegal GRR service req state (%d)",glif_grr_ser_req_state) ;
  }
  if(glif_service_req_pended_ind && 
     (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED) &&
     (GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF))
  {
    glif_grr_send_service_req();
  }
} /* end glif_process_rr_gprs_service_cnf() */



/*===========================================================================

FUNCTION      GLIF_GRRIF_IDLE_STATE_HANDLER

DESCRIPTION
  This is a IDLE state handler for the GRR events

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
void glif_grrif_idle_state_handler
(
  /* Incoming GRR primitive */
  mm_cmd_type     *primitive
)
{
  switch (primitive->cmd.hdr.message_id)
  {
    case RR_PLMN_SELECT_CNF:
      glif_sending_gmm_message = FALSE ;
      break ;

    case RR_RESELECTION_IND:

      /* Store the Ready Timer State at the commencement of GPRS Reselection,
         as this will influence the decision to Cell Update on completion of
         reselection. */

      MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Rcvd RR_RESELECT_IND Rdy Tmr Active=%d",glif_rdy_tmr_active) ;
      glif_rdy_tmr_active_at_reselection = glif_rdy_tmr_active;
      break;

    case RR_SERVICE_IND:

      glif_process_rr_service_indication( primitive );
      break;

#ifdef FEATURE_GSM_DTM
    case RR_SERVICE_IND_FROM_OTA_MSG:

      glif_process_rr_service_ind_from_ota_msg( primitive );
      break;
#endif /* FEATURE_GSM_DTM */

    case RR_GMM_GPRS_RESUMPTION_IND:

      glif_process_rr_gprs_resumption_indication(FALSE);
      break;
#ifdef FEATURE_NAS_FREE_FLOAT
    case RR_GMM_GPRS_SERVICE_CNF:

      glif_process_rr_gprs_service_cnf(primitive->cmd.rr_gprs_service_cnf);
      break;
#else
    case RR_GPRS_PCCCH_CAMP_CNF:

      glif_process_rr_gprs_pccch_camp_cnf();
      break;
#endif
    default:
      MSG_ERROR_DS_1
      (
        MM_SUB, "=MM= GLIF: Unexp GRR prim in GPRS Idle: %d",
        primitive->cmd.hdr.message_id
      );
      break ;
  }

} /* end glif_grrif_idle_state_handler() */

/*===========================================================================

FUNCTION      GLIF_GRRIF_STANDBY_STATE_HANDLER

DESCRIPTION
  This is a STANDBY state handler for the GRR events

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
void glif_grrif_standby_state_handler
(
  /* Incoming GRR primitive */
  mm_cmd_type     *primitive
)
{
  switch (primitive->cmd.hdr.message_id)
  {
    case RR_PAGING_IND :

      /* Process GRR Paging indication */
      glif_process_grr_page_indication(primitive) ;
      break ;

    case RR_PLMN_SELECT_CNF:
      glif_sending_gmm_message = FALSE ;
      break ;

    case RR_RESELECTION_IND:

      /* Store the Ready Timer State at the commencement of GPRS Reselection,
         as this will influence the decision to Cell Update on completion of
         reselection. */

      MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Rcvd RR_RESELECT_IND Rdy Tmr Active=%d",glif_rdy_tmr_active) ;
      glif_rdy_tmr_active_at_reselection = glif_rdy_tmr_active;
      break;

    case RR_SERVICE_IND:

      glif_process_rr_service_indication( primitive );
      break;

#ifdef FEATURE_GSM_DTM
    case RR_SERVICE_IND_FROM_OTA_MSG:

      glif_process_rr_service_ind_from_ota_msg( primitive );
      break;
#endif /* FEATURE_GSM_DTM */

    case RR_GMM_GPRS_RESUMPTION_IND:

      glif_process_rr_gprs_resumption_indication(FALSE);
      break ;
#ifdef FEATURE_NAS_FREE_FLOAT
    case RR_GMM_GPRS_SERVICE_CNF:

      glif_process_rr_gprs_service_cnf(primitive->cmd.rr_gprs_service_cnf);
      break;
#else
    case RR_GPRS_PCCCH_CAMP_CNF:

      glif_process_rr_gprs_pccch_camp_cnf();
      break;
#endif
    default:
      MSG_ERROR_DS_1
      (
        MM_SUB, "=MM= GLIF: Unexp GRR prim in GPRS Standby: %d",
        primitive->cmd.hdr.message_id
      );
      break ;
  }

} /* end glif_grrif_standby_state_handler() */

/*===========================================================================

FUNCTION      GLIF_GRRIF_READY_STATE_HANDLER

DESCRIPTION
  This is a READY state handler for the GRR events

DEPENDENCIES
  None

RETURN VALUE
  THis function returns next GMM-LLC interface state

SIDE EFFECTS
  None

===========================================================================*/
void glif_grrif_ready_state_handler
(
  /* Incoming GRR primitive */
  mm_cmd_type     *primitive
)
{
  switch (primitive->cmd.hdr.message_id)
  {
    case RR_PAGING_IND :

      /* Process GRR Paging indication */
      glif_process_grr_page_indication(primitive) ;
      break ;

    case RR_SYNC_IND :
      if ((primitive->cmd.rr_sync_ind.sync_reason == MM_RR_HANDOVER) ||
         (primitive->cmd.rr_sync_ind.sync_reason == MM_RR_WCDMA_GSM_HO ))
      {
        if ((!glif_rau_in_progress) && (!glif_force_to_standby))
        {
          glif_process_cell_update(FALSE) ;
        }
#ifdef FEATURE_NAS_FREE_FLOAT
        else if (glif_llc_connection_state == LLC_SUSPENSION_CONFIRMED)
        {
          /* Resume signalling traffic only */
          glif_grr_send_service_req() ;
        }
        else if (glif_llc_connection_state == LLC_SUSPENSION_REQUESTED)
        {
          glif_service_req_pended_ind = TRUE ;

          MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service request (%d,%d)",glif_grr_ser_req_state,glif_llc_connection_state) ;
        }
      }
#else
        else if ((glif_grr_camp_state != GLIF_CAMP_ON_PCCCH_REQUESTED) &&
                 (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED))
        {
          /* Resume signalling traffic only */
          glif_grr_send_service_confirm() ;
        }
        else
        {
          glif_service_confirm_pended_ind = TRUE ;

          MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: failed to send service confirm (%d,%d)",glif_grr_camp_state,glif_llc_connection_state) ;
        }
      }
#endif
      break ;

    case RR_RESELECTION_IND:

      /* Store the Ready Timer State at the commencement of GPRS Reselection,
         as this will influence the decision to Cell Update on completion of
         reselection. */

      MSG_HIGH_DS_1( MM_SUB, "=MM= GLIF: Rcvd RR_RESELECT_IND Rdy Tmr Active=%d",glif_rdy_tmr_active) ;
      glif_rdy_tmr_active_at_reselection = glif_rdy_tmr_active;
      break;

    case RR_PLMN_SELECT_CNF:
      glif_sending_gmm_message = FALSE ;
      break ;

    case RR_SERVICE_IND:

      glif_process_rr_service_indication( primitive );
      break ;

#ifdef FEATURE_GSM_DTM
    case RR_SERVICE_IND_FROM_OTA_MSG:

      glif_process_rr_service_ind_from_ota_msg( primitive );
      break;
#endif /* FEATURE_GSM_DTM */

    case RR_GMM_GPRS_RESUMPTION_IND:

      glif_process_rr_gprs_resumption_indication
      (
#ifdef FEATURE_GSM_DTM
        primitive->cmd.rr_gmm_gprs_resumption_ind.cell_update_required
#else
        FALSE
#endif
      );
      break ;
#ifdef FEATURE_NAS_FREE_FLOAT
    case RR_GMM_GPRS_SERVICE_CNF:

      glif_process_rr_gprs_service_cnf(primitive->cmd.rr_gprs_service_cnf);
      break;
#else
    case RR_GPRS_PCCCH_CAMP_CNF:

      glif_process_rr_gprs_pccch_camp_cnf();
      break;
#endif
    default:
      MSG_ERROR_DS_1
      (
        MM_SUB, "=MM= GLIF: Unexp GRR prim in GPRS Ready: %d",
        primitive->cmd.hdr.message_id
      );
      break ;
  }

} /* end glif_grrif_ready_state_handler() */

/*===========================================================================

FUNCTION      GMMLLC_HANDLE_GRR_EVENT

DESCRIPTION
  This is a GMM-LLC module's interface function for GRR generated events.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmmllc_handle_grr_event
(
  /* Incoming GRR primitive */
  mm_cmd_type     *primitive
)
{
  switch (glif_state)
  {
    case GMM_IDLE_STATE :
      glif_grrif_idle_state_handler(primitive);
      break ;
    case GMM_READY_STATE :
      glif_grrif_ready_state_handler(primitive) ;
      break ;
    case  GMM_STANDBY_STATE :
      glif_grrif_standby_state_handler(primitive);
      break ;
    default :
      MSG_FATAL_DS(MM_SUB, "=MM= GLIF: Invalid state: %d",(int)glif_state,0,0) ;
      break ;
  } /* switch (current_glif_state) */

} /* end gmmllc_handle_grr_event() */

void gmmllc_reset_service_state (void)
{
  glif_grr_ser_req_state = GLIF_NO_SERVICE;
}

/*===========================================================================

FUNCTION      gmmllc_reset_llc_conn_state

DESCRIPTION
  This function is to make the reset the LLC connection state to SUSPENSION CONFIRMED.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmmllc_reset_llc_conn_state
(
  void
)
{
  glif_llc_connection_state = LLC_SUSPENSION_CONFIRMED;
} 

/*===========================================================================

FUNCTION      GLIF_PROCESS_LL_XID_IND

DESCRIPTION

  Processes the LL_XID_IND primitive from LLC. This primitive conveys the
  LLC N201-U parameter which indicates the maximum permissable size of a
  layer 3 PDU which will be accepted by LLC.

  This primitive may be expected as re-assertion of the default setting,
  or it may change as a function of a layer 2 LLC peer-to-peer XID exchange.

  The N201-U parameter must be checked against the requirements of layer 3
  for OTA messaging.

  The primitive is capable of conveying layer3 peer-to-peer XID negotiation
  packets. It is not expected that the GMM layer would employ such a
  mechanism.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_process_ll_xid_ind
(
  /* Incoming XID indication */
  mm_cmd_type  *primitive
)
{

  gmm_xid_negotiation_completed = TRUE;

  gmm_max_L3_length = primitive->cmd.ll_xid_ind.n201_u;
  MSG_HIGH_DS_1(MM_SUB, "negotiated max_L3_message  : %d", gmm_max_L3_length );

  if ( primitive->cmd.ll_xid_ind.l3_xid_req != NULL )
  {
    MSG_ERROR_DS_0( MM_SUB, "=MM= GLIF: GMM L3 XID Unsupported" );
#ifndef FEATURE_MODEM_HEAP
    gs_free( primitive->cmd.ll_xid_ind.l3_xid_req );
#else
    modem_mem_free( primitive->cmd.ll_xid_ind.l3_xid_req , MODEM_MEM_CLIENT_NAS);
#endif

  }

} /* end glif_process_ll_xid_ind() */

/*===========================================================================

FUNCTION      GLIF_PROCESS_LLSMS_XID_IND

DESCRIPTION

  Processes the LLSMS_XID_IND primitive from LLC. This primitive conveys the
  LLC N201-U parameter which indicates the maximum permissable size of a
  layer 3 PDU which will be accepted by LLC.

  This primitive may be expected as re-assertion of the default setting,
  or it may change as a function of a layer 2 LLC peer-to-peer XID exchange.

  The N201-U parameter must be checked against the requirements of layer 3
  for SMS OTA messaging.

  The primitive is capable of conveying layer3 peer-to-peer XID negotiation
  packets. It is not expected that the SMS layer would employ such a
  mechanism.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_process_llsms_xid_ind
(
  /* Incoming XID indication */
  mm_cmd_type  *primitive
)
{
  if ( primitive->cmd.llsms_xid_ind.n201_u < MAX_LENGTH_SMS_DATA )
  {
    MSG_ERROR_DS_1
    (
      MM_SUB, "=MM= GLIF: LLSMS-XID-IND N201-U < Max SMS Len: %d",
      primitive->cmd.llsms_xid_ind.n201_u
    );
  }

  if ( primitive->cmd.llsms_xid_ind.l3_xid_req != NULL )
  {
    MSG_ERROR_DS_0( MM_SUB, "=MM= GLIF: SMS L3 XID Unsupported");
#ifndef FEATURE_MODEM_HEAP
    gs_free( primitive->cmd.llsms_xid_ind.l3_xid_req );
#else
    modem_mem_free( primitive->cmd.llsms_xid_ind.l3_xid_req , MODEM_MEM_CLIENT_NAS);
#endif
  }

} /* end glif_process_llsms_xid_ind() */

/*===========================================================================

FUNCTION      GLIF_LLCIF_IDLE_STATE_HANDLER

DESCRIPTION
  This is GMM-LLC interface IDLE state handler for processing of the LLC
  events

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
gmm_llc_if_state_type glif_llcif_idle_state_handler
(
  /* Incoming LLC primitive */
  mm_cmd_type     *primitive
)
{
  /* Next state of the GMM-LLC Interface module */
  gmm_llc_if_state_type next_glif_state = glif_state ;

  switch (primitive->cmd.hdr.message_id)
  {
    case GLLCMM_LLGMM_TRIGGER_IND:

      /* LLGMM_TRIG_IND is expected whenever LLC transmits a frame whilst
         the READY timer is not running. It is not expected that the
         READY timer would be running in GPRS Idle, but LLC will transmit
         frames in GPRS Idle for the transport of OTA messaging and LLC
         Layer2 peer-to-peer XID negotiation. Do nothing. */
      break ;

    default:
      MSG_ERROR_DS_1
      (
        MM_SUB, "=MM= GLIF: Unexp LLC prim in GPRS Idle: %d",
        primitive->cmd.hdr.message_id
      );
      break ;
  }

  return next_glif_state ;

} /* end glif_llcif_idle_state_handler() */

/*===========================================================================

FUNCTION      GLIF_LLCIF_READY_STATE_HANDLER

DESCRIPTION
  This is GMM-LLC interface READY state handler for processing of the LLC
  events

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
gmm_llc_if_state_type glif_llcif_ready_state_handler
(
  /* Incoming LLC primitive */
  mm_cmd_type     *primitive
)
{
  /* Next state of the GMM-LLC Interface module */
  gmm_llc_if_state_type next_glif_state = glif_state ;
 
  boolean routine_man_selection_failed = FALSE ;
  boolean is_cell_forbidden = FALSE;
  
  if (!((PLMN_MATCH( gmm_current_lai.PLMN_id.identity,
                    mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
       (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
        sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))  &&
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
             SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    routine_man_selection_failed = TRUE;
    MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
  }
 
  if( (mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
        ((routine_man_selection_failed) ||
         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                 SYS_NETWORK_SELECTION_MODE_LIMITED)||
          ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
             (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                  SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) ||
            ((!gmm_manual_mode_user_selection_procedure)&&
             ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                  SYS_NETWORK_SELECTION_MODE_MANUAL) ||
              (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                  SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))))&& 
           (
#ifdef FEATURE_DUAL_SIM
            (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
            (reg_sim_per_subs_plmn_forbidden(gmm_current_lai.PLMN_id,(sys_modem_as_id_e_type)mm_sub_id )) ||
#else
            (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) ||
            (reg_sim_plmn_forbidden(gmm_current_lai.PLMN_id)) ||
#endif
            (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
            (mm_check_forbidden_regional_roaming( &gmm_current_lai ))
          )
         )
       )
    )
  {
      is_cell_forbidden = TRUE;
      MSG_HIGH_DS_0(MM_SUB, "=MM= Cell is forbidden");
  } 

  switch (primitive->cmd.hdr.message_id)
  {
    case GLLCMM_LLGMM_TRIGGER_IND:

         /* LLGMM_TRIGGER_IND is expected whenever LLC transmits a frame whilst
         the READY timer is not running. If the READY timer has been
         deactivated during the attach procedure and LLC has not been
         instructed to start the READY timer, then this primitive would be
         expected when a frame is transmitted. */

      if ( (glif_ready_timer_value != 0) && !glif_ready_timer_deactivated )
      {
        MSG_ERROR_DS_1
        (
          MM_SUB, "=MM= GLIF: Unexp LLGMM_TRIGGER_IND in GPRS Ready: %d",
          primitive->cmd.hdr.message_id
        );
      }
      break ;

    case GLLCMM_LLGMM_RDY_TMR_IND :
      /*
      ** When in READY state the GMM-LLC interface receives READY timer
      ** expiry indication it goes to the STANDBY state and starts the
      ** periodic RAU timer.
      */

      /* Store current state of ready timer. */
      glif_rdy_tmr_active = FALSE;

      /* Make sure that the ready timer has not just been deactivated and
         crossed with a timer expiration that was set up earlier, in which
         case we would want to remain in Ready State. */

      if ( (glif_ready_timer_value != 0) && !glif_ready_timer_deactivated )
      {
        /* According to 24.008 4.7.2.2 :
        ** In GSM, the timer T3312 is reset and started with its initial value,
        ** when the READY timer is expires
        */
        glif_restart_t3312() ;

        /* Prevent a scheduled but pending cell update process from being
        ** cancelled by a simultaneous ready timer expiration. If GMM is
        ** waiting on a confirmation of user plane suspension then do not
        ** cancel the pending operation, otherwise the user plane will remain
        ** suspended indefinately. 
        */  

        if ( glif_llc_connection_state == LLC_RESUMED )
        {
          glif_pended_cell_update = FALSE ;
          glif_pended_cell_update_is_initial = FALSE ;
        }
        else if(glif_llc_connection_state == LLC_SUSPENSION_REQUESTED)
        {
          MSG_HIGH_DS_0(MM_SUB,"=MM= do nothing wait for suspension cnf");
        }
        else if(glif_llc_connection_state == LLC_SUSPENSION_CONFIRMED)
        {
          if(is_cell_forbidden)
          {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Cell is forbidden, clear cell update if pended");
            glif_pended_cell_update = FALSE ;
            glif_pended_cell_update_is_initial = FALSE ;                  
          }
#ifdef FEATURE_NAS_FREE_FLOAT
          else if(glif_grr_ser_req_state == GLIF_SERVICE_REQUESTED || glif_grr_ser_req_state == GLIF_NO_SERVICE)
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= If cell update was pended will be procesed after service cnf");
          }
          else if( (glif_grr_ser_req_state == GLIF_SERVICE_CONFIRMED) && 
                   (glif_pended_cell_update == TRUE) && ( gmm_state == GMM_REGISTERED) &&
                   ((gmm_substate == GMM_NORMAL_SERVICE) || (gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM)))
          {
            glif_pended_cell_update = FALSE ;
            glif_pended_cell_update_is_initial = FALSE ;
            glif_llc_send_resume_request();  
          }
#endif
        }

        next_glif_state = GMM_STANDBY_STATE ;
      }
      break ;

    default:
      MSG_ERROR_DS_1
      (
        MM_SUB, "=MM= GLIF: Unexp LLC prim in GPRS Ready: %d",
        primitive->cmd.hdr.message_id
      );
      break ;
  }

  return next_glif_state ;

} /* end glif_llcif_ready_state_handler() */

/*===========================================================================

FUNCTION      GLIF_LLCIF_STANDBY_STATE_HANDLER

DESCRIPTION
  This is GMM-LLC interface STANDBY state handler for processing of the LLC
  events

DEPENDENCIES
  None

RETURN VALUE
  New GMM-LLC interface module state

SIDE EFFECTS
  None

===========================================================================*/
gmm_llc_if_state_type glif_llcif_standby_state_handler
(
  /* Incoming LLC primitive */
  mm_cmd_type     *primitive
)
{
  /* Next state of the GMM-LLC Interface module */
  gmm_llc_if_state_type next_glif_state = glif_state ;

  switch (primitive->cmd.hdr.message_id)
  {
    case GLLCMM_LLGMM_TRIGGER_IND :

      /*
      ** Upon sending of any LLC frame which is different than NULL
      ** frame the MS shall start READY timer if START timer procedure
      ** has not been deactivated
      */
      if ((mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) &&
          (glif_ready_timer_value != 0))
      {
        if (!glif_ready_timer_deactivated)
        {
          /* Command the LLC start READY timer */
          glif_start_ready_timer(glif_ready_timer_value) ;

        }

        if (glif_ready_timer_value != 0)
        {
          send_rr_ready_timer_state_request(FALSE) ;
        }

        /* GMM-LLC interface moves to the GMM READY STATE */
        next_glif_state = GMM_READY_STATE ;
      }
      break ;

    default:
      MSG_ERROR_DS_1
      (
        MM_SUB, "=MM= GLIF: Unexp LLC prim in GPRS Standby: %d",
        primitive->cmd.hdr.message_id
      );
      break ;
  }

  return next_glif_state ;

} /* end glif_llcif_standby_state_handler() */

/*===========================================================================

FUNCTION      GLIF_SEND_RRC_PRIMITIVE

DESCRIPTION
  This function translates incoming LLC UNIT DATA indication (either GMM or SMS)
  into RRC DATA indication which will be processed by the GMM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_send_rrc_primitive
(
  word   ota_msg_len,
  byte  *ota_msg
)
{
  /* Allocate RRC buffer */
  mm_cmd_type *rrc_data_ind = mm_get_cmd_buf();
  uint16 length;

  if (rrc_data_ind == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= GLIF: Heap Exhaustion", 0, 0, 0 );
  }

  /* Translate LLC UNITDATA indication into RRC DATA indication */
  rrc_data_ind->cmd.hdr.message_set = MS_MM_RRC ;

  /* Mark new primitive as a RRC Data indication */
  rrc_data_ind->cmd.hdr.message_id = (int)MM_DATA_IND ;

  /* Put GMM message length */
  PUT_IMH_LEN(ota_msg_len,&rrc_data_ind->cmd.mm_data_ind.message_header) ;

  length = MIN(ota_msg_len,gmm_max_L3_msg_length(MM_PS_DOMAIN)) ;


#ifndef FEATURE_MODEM_HEAP
  rrc_data_ind->cmd.mm_data_ind.L3_data = (uint8*) gs_alloc(length);
#else
  rrc_data_ind->cmd.mm_data_ind.L3_data = (uint8*) modem_mem_calloc(1, length, MODEM_MEM_CLIENT_NAS);
#endif
  mm_check_for_null_ptr((void*)rrc_data_ind->cmd.mm_data_ind.L3_data);

  /* Copy LLC UNITDATA indication into RRC DATA indication */
  memscpy((void *)rrc_data_ind->cmd.mm_data_ind.L3_data, gmm_max_L3_msg_length(MM_PS_DOMAIN),
                                            (void *)ota_msg,ota_msg_len) ;

  /* Process LLC UNITDATA indication */
  gmm_handle_message(rrc_data_ind) ;
  if (rrc_data_ind->cmd.mm_data_ind.L3_data != NULL)
 {
#ifdef FEATURE_MODEM_HEAP       
   modem_mem_free(rrc_data_ind->cmd.mm_data_ind.L3_data, MODEM_MEM_CLIENT_NAS);
#else
   mem_free( &(tmc_heap), rrc_data_ind->cmd.mm_data_ind.L3_data );
#endif
 }

  /* Deallocate LLC Command buffer */
  mm_free_cmd_buf( rrc_data_ind );

} /* end glif_send_rrc_primitive() */

/*===========================================================================

FUNCTION      GLIF_PROCESS_LL_UNITDATA_IND

DESCRIPTION
  This function processes the LL_UNITDATA_IND (GMM) primitive from LLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_process_ll_unitdata_ind
(
  /* Incoming XID indication */
  mm_cmd_type  *primitive
)
{
  /*------------------------------------------------------------ 
  ** When a message is received that is too short to contain a
  ** complete message type IE, that message shall be ignored
  ** (TS 24.008 8.2)
  ** ------------------------------------------------------------*/
   if(mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) 
   {
      MSG_HIGH_DS_2(MM_SUB, "=MM= gmm_max_L3_length  : %d , XID Negotiation completed = %d ",
       gmm_max_L3_length,gmm_xid_negotiation_completed);
   }

  if (
      (primitive->cmd.ll_unitdata_ind.l3_pdu     != NULL                 ) &&
      (primitive->cmd.ll_unitdata_ind.l3_pdu_len <= gmm_max_L3_msg_length(MM_PS_DOMAIN)) &&
      (primitive->cmd.ll_unitdata_ind.l3_pdu_len > 1)
     )
  {
  
    if ((auth_ps_ciphering_algorithm              != (int) GMMLLC_GEA_UNDEFINED) &&
        (auth_ps_ciphering_algorithm              != (int) GMMLLC_GEA_NONE     ) &&
        (primitive->cmd.ll_unitdata_ind.ciphered  == FALSE               ) &&
        (primitive->cmd.ll_unitdata_ind.l3_pdu[1] == GMM_ATTACH_ACCEPT)  )
    {
      MSG_HIGH_DS_3(MM_SUB, "=MM= Received Attach Accept unciphered when Ciphering Algorithm is : %d", auth_ps_ciphering_algorithm, 0, 0);
      auth_ps_ciphering_algorithm = (int) GMMLLC_GEA_NONE;                                       
      glif_tlli_assign(glif_tlli,TRUE);
    }
        
    glif_send_rrc_primitive(primitive->cmd.ll_unitdata_ind.l3_pdu_len,
                                       primitive->cmd.ll_unitdata_ind.l3_pdu) ;
    mm_send_ota_msg_log_packet(CN_TO_UE_DIRECTION,
                               primitive->cmd.ll_unitdata_ind.l3_pdu_len,
                               primitive->cmd.ll_unitdata_ind.l3_pdu);
    
  }
  else
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Rcvd GMM OTA Msg Err");
  }

  /* Deallocate the primitives 'L3 PDU' sub-allocation. */
#ifndef FEATURE_MODEM_HEAP
  gs_free((void *)primitive->cmd.ll_unitdata_ind.l3_pdu) ;
#else
  modem_mem_free((void *)primitive->cmd.ll_unitdata_ind.l3_pdu, MODEM_MEM_CLIENT_NAS) ;
#endif
} /* end glif_process_ll_unitdata_ind() */

/*===========================================================================

FUNCTION      GLIF_PROCESS_LL_UNITDATA_IND

DESCRIPTION
  This function processes the LL_UNITDATA_IND (GMM) primitive from LLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_GPRS_PS_HANDOVER
void glif_process_llgmm_psho_cnf (void)
{
  if ( gmm_psho_status != GMM_PSHO_STARTED)
  {  
    MSG_HIGH_DS_0( MM_SUB, "=MM= This message not expected in non-PSHO cases");        
  }
  else
  {
#ifdef FEATURE_WTOG_PS_HANDOVER
    #error code not present
#endif  
    MSG_HIGH_DS_0(MM_SUB, "=MM= MM received llgmm_psho_cnf msg" );  
    mm_send_rr_psho_cnf();
  }
} /* end glif_process_llgmm_psho_cnf() */
#endif

/*===========================================================================

FUNCTION      GLIF_PROCESS_LLSMS_UNITDATA_IND

DESCRIPTION

  This function processes the LLSMS_UNITDATA_IND (SMS) primitive from LLC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_process_llsms_unitdata_ind
(
  /* Incoming XID indication */
  mm_cmd_type  *primitive
)
{
  /*------------------------------------------------------------ 
  ** When a message is received that is too short to contain a
  ** complete message type IE, that message shall be ignored
  ** (TS 24.008 8.2)
  ** ------------------------------------------------------------*/
  if(mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) 
  {
      MSG_HIGH_DS_2(MM_SUB, "=MM= gmm_max_L3_length  : %d , XID Negotiation completed = %d ",
       gmm_max_L3_length,gmm_xid_negotiation_completed);
  }
  if (
      (primitive->cmd.llsms_unitdata_ind.l3_pdu     != NULL               ) &&
      (primitive->cmd.llsms_unitdata_ind.l3_pdu_len <= MAX_LENGTH_SMS_DATA) &&
      (primitive->cmd.llsms_unitdata_ind.l3_pdu_len > 1)
     )
  {
    glif_send_rrc_primitive(primitive->cmd.llsms_unitdata_ind.l3_pdu_len,
                                    primitive->cmd.llsms_unitdata_ind.l3_pdu) ;
    mm_send_ota_msg_log_packet(CN_TO_UE_DIRECTION,
                               primitive->cmd.llsms_unitdata_ind.l3_pdu_len,
                               primitive->cmd.llsms_unitdata_ind.l3_pdu);

  }
  else
  {
    MSG_HIGH_DS_0( MM_SUB, "=MM= GLIF: Rcvd SMS OTA Msg Err");
  }

  /* Deallocate the primitives 'L3 PDU' sub-allocation. */
#ifndef FEATURE_MODEM_HEAP
  gs_free((void *)primitive->cmd.llsms_unitdata_ind.l3_pdu) ;
#else
  modem_mem_free((void *)primitive->cmd.llsms_unitdata_ind.l3_pdu , MODEM_MEM_CLIENT_NAS) ;
#endif
} /* end glif_process_llsms_unitdata_ind() */

/*===========================================================================

FUNCTION      GLIF_PROCESS_PENDED_GMM_COMMANDS

DESCRIPTION
  This function processes pended GMM commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_process_pended_gmm_commands
(
  void
)
{
  if (glif_pended_mt_detach_accept_ind)
  {
#ifndef FEATURE_NAS_FREE_FLOAT
    glif_grr_send_service_confirm() ;
#endif
    /* Send pended MT Detach Accept message */
    glif_send_gmm_ota_message(GLIF_BUFFERED_DETACH_ACCEPT_LENGTH,
                                                glif_pended_mt_detach_accept
#ifdef FEATURE_GPRS_PS_HANDOVER
                                                ,FALSE
#endif                                                  
                             ) ;

    glif_pended_mt_detach_accept_ind = FALSE ;
  }

  if (glif_pended_rau_req && glif_pended_detach_req)
  {
#ifdef FEATURE_MODEM_HEAP
    if(glif_pended_detach == NULL)
    {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for glif_pended_detach", 0,0,0 );
    }
#endif
    if (glif_pended_detach[2] & 0x08)
    {
      /* DETACH Request is sent to indicate POWER OFF */
#ifndef FEATURE_NAS_FREE_FLOAT
      glif_grr_send_service_confirm() ;
#endif
      /* Send pended Detach Request message */
      glif_send_gmm_ota_message(
                              glif_buffered_detach_length,glif_pended_detach
#ifdef FEATURE_GPRS_PS_HANDOVER
                              ,FALSE
#endif                                                  
                               ) ;                              

      /* Discard RAU Request message */
      glif_pended_rau_req = FALSE ;

      /* Discard Cell Update/Notification */
      glif_pended_cell_update = FALSE ;
      glif_pended_cell_update_is_initial = FALSE ;
    }
    else
    {
      /* Normal DETACH Request */
#ifndef FEATURE_NAS_FREE_FLOAT
      glif_grr_send_service_confirm() ;
#endif
#ifdef FEATURE_MODEM_HEAP
      if((glif_pended_gmm_msg == NULL) || (glif_pended_detach == NULL))
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for glif_pended_gmm_msg or glif_pended_detach", 0,0,0 );
      }
#endif
      /* Send pended RAU Request message */
      glif_send_gmm_ota_message(
                            glif_buffered_message_length,glif_pended_gmm_msg
#ifdef FEATURE_GPRS_PS_HANDOVER
                            ,FALSE
#endif                                                  
                               ) ;                            

      /* Send pended Detach Request message */
      glif_send_gmm_ota_message(
                            glif_buffered_detach_length,glif_pended_detach
#ifdef FEATURE_GPRS_PS_HANDOVER
                            ,FALSE
#endif                                                  
                               ) ;                              

      /* GMM needs to send Cell Update/Notification only
      ** if there is no outgoing RAU request message,
      ** if GMM is about to send RAU request message
      ** Cell Update/Notification is to be discarded
      */
      glif_pended_cell_update = FALSE ;
      glif_pended_cell_update_is_initial = FALSE ;
    }
  }
  else if (glif_pended_attach_req)
  {
#ifndef FEATURE_NAS_FREE_FLOAT
    glif_grr_send_service_confirm() ;
#endif

#ifdef FEATURE_MODEM_HEAP
    if(glif_pended_gmm_msg == NULL)
    {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for glif_pended_gmm_msg", 0,0,0 );
    }
#endif
    /* Send pended Attach Request message */
    glif_send_gmm_ota_message(
                          glif_buffered_message_length,glif_pended_gmm_msg
#ifdef FEATURE_GPRS_PS_HANDOVER
                          ,FALSE
#endif                              
                             ) ;  
                          
    /* Discard Cell Update/Notification */
    glif_pended_cell_update = FALSE ;
    glif_pended_cell_update_is_initial = FALSE ;
  }
  else if (glif_pended_rau_req)
  {
#ifndef FEATURE_NAS_FREE_FLOAT
    glif_grr_send_service_confirm() ;
#endif
#ifdef FEATURE_MODEM_HEAP
    if(glif_pended_gmm_msg == NULL)
    {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for glif_pended_gmm_msg", 0,0,0 );
    }
#endif
    /* Send pended RAU Request message */
    glif_send_gmm_ota_message(
                          glif_buffered_message_length,glif_pended_gmm_msg
#ifdef FEATURE_GPRS_PS_HANDOVER
                          ,FALSE
#endif                              
                             ) ;                              

    /* Discard Cell Update/Notification */
    glif_pended_cell_update = FALSE ;
    glif_pended_cell_update_is_initial = FALSE ;
  }
  else if (glif_pended_detach_req)
  {
#ifndef FEATURE_NAS_FREE_FLOAT
    glif_grr_send_service_confirm() ;
#endif
#ifdef FEATURE_MODEM_HEAP
    if(glif_pended_detach == NULL)
    {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for glif_pended_detach", 0,0,0 );
    }
#endif
    /* Send pended Detach Request message */
    glif_send_gmm_ota_message(
                          glif_buffered_detach_length,glif_pended_detach
#ifdef FEATURE_GPRS_PS_HANDOVER
                           ,FALSE
#endif                              
                             ) ;                                

    /* Discard Cell Update/Notification */
    glif_pended_cell_update = FALSE ;
    glif_pended_cell_update_is_initial = FALSE ;
  }
  else if (glif_pended_cell_update)
  {
#ifndef FEATURE_NAS_FREE_FLOAT
    glif_grr_send_service_confirm() ;
#endif


    glif_process_cell_update(glif_pended_cell_update_is_initial) ;

    /* Resume LLC traffic */
    if ((gmm_state == GMM_REGISTERED) &&
        (((gmm_substate == GMM_NORMAL_SERVICE) ||
        (gmm_substate == GMM_ATTEMPTING_TO_UPDATE_MM) ||
        (gmm_substate == GMM_IMSI_DETACH_INITIATED)) 
#ifdef FEATURE_GSM_DTM
         ||
        ((mm_serving_plmn.dtm_supported == TRUE) &&
         ((gmm_substate == GMM_SUSPENDED) || (gmm_is_suspended)) &&
         ((gmm_get_saved_substate() == GMM_NORMAL_SERVICE) ||
          (gmm_get_saved_substate() == GMM_ATTEMPTING_TO_UPDATE_MM) ||
          (gmm_get_saved_substate() == GMM_IMSI_DETACH_INITIATED))) 
#endif
       ))
    {
      /* Permit user plane transfers from now on */
      glif_llc_send_resume_request();
    }
     else if ((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
              (!gmm_rau_tx_is_acked))
     {
       gmm_initiate_routing_area_update();
     }
  } 
  
#ifdef FEATURE_MODEM_HEAP
  if(glif_pended_gmm_msg != NULL)
  {
    modem_mem_free(glif_pended_gmm_msg,MODEM_MEM_CLIENT_NAS);
    glif_pended_gmm_msg = NULL;
    glif_pended_attach_req = FALSE;
    glif_pended_rau_req    = FALSE;
  }

  if(glif_pended_detach != NULL)
  {
    modem_mem_free(glif_pended_detach,MODEM_MEM_CLIENT_NAS);
    glif_pended_detach = NULL;
    glif_pended_detach_req = FALSE;
  }
#endif
#ifndef FEATURE_NAS_FREE_FLOAT
  if (glif_service_confirm_pended_ind)
  {
    /* Permit user plane transfers from now on */
    glif_grr_send_service_confirm();
  }
#endif

} /* end glif_process_pended_gmm_commands() */

void glif_execute_pended_gmm_triggers(void)
{
  gmm_detach_in_progress = FALSE;
  gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_GMM_DETACHED, FALSE, 0 );
  gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_PTMSI_ASSIGNED,FALSE,0) ;
  gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_TLLI_UNASSIGNMENT,FALSE,0);
  if(gmm_gprs_attach_pending == TRUE)
  {
    gmm_initiate_gprs_attach();
  }
}


#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM      
/*===========================================================================

FUNCTION      GLIF_PROCESS_LLGMM_STATUS_IND

DESCRIPTION
  This function processes LLC STATUS Message indicating that a NAS message has not been sent out.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void glif_process_llgmm_status_ind
(
  /* Incoming STATUS indication */
  mm_cmd_type  *primitive
)
{
  sm_cmd_type              *sm_cmd;

  MSG_HIGH_DS_1(MM_SUB, "=MM= GLIF: Received STATUS IND with cause = %d ", primitive->cmd.llgmm_status_ind.cause);

  if (primitive->cmd.llgmm_status_ind.protocol_disc == PD_GPRS_SM)       
  {
    if (primitive->cmd.llgmm_status_ind.cause == GLLCMM_L2_TX_FAILURE)
    {
      sm_cmd = sm_get_cmd_buf();

      if (sm_cmd == NULL)
      {
        MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
      }
      sm_cmd->header.message_set = MS_GMM_SM;
      sm_cmd->header.cmd_id      = GMMSM_STATUS_IND;
      sm_cmd->header.connection_id = primitive->cmd.llgmm_status_ind.transaction_id;
      #ifdef FEATURE_DUAL_SIM
      sm_cmd->cmd.gmm_status_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      #endif

      MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMSM_STATUS_IND");

      sm_put_cmd( sm_cmd );
    }
    else
    {
      MSG_HIGH_DS_1(MM_SUB, "=MM= GGLIF: Ignore STATUS IND for protocol_disc %d", primitive->cmd.llgmm_status_ind.protocol_disc );
    }
  }
  else if ((primitive->cmd.llgmm_status_ind.protocol_disc == PD_GMM) && 
           (primitive->cmd.llgmm_status_ind.transaction_id == gmm_transaction_id))  
  { 
    if (primitive->cmd.llgmm_status_ind.cause == GLLCMM_L2_TX_FAILURE)
    {
      if (gmm_last_signalling_message_type == GMM_ATTACH_REQUEST) 
      { 
        if(mm_timer_status[TIMER_T3311] == TIMER_ACTIVE || mm_timer_status[TIMER_T3302] == TIMER_ACTIVE) 
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= IRAT happened and no need to retransmit attach");
        }
        else
      { 
        gmm_initiate_gprs_attach(); 
      } 
      } 
      else if (gmm_last_signalling_message_type == GMM_ROUTING_AREA_UPDATE_REQUEST) 
      { 
        if(mm_timer_status[TIMER_T3311] == TIMER_ACTIVE || mm_timer_status[TIMER_T3302] == TIMER_ACTIVE) 
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= IRAT happened and no need to retransmit RAU");
        }
        else
        { 
        gmm_initiate_routing_area_update(); 
      }
      }
      else if ((gmm_last_signalling_message_type == GMM_ATTACH_COMPLETE) || 
               (gmm_last_signalling_message_type == GMM_ROUTING_AREA_UPDATE_COMPLETE) )
      { 
        if ((mm_pending_lu_type == MM_LU_CAUSE_IMSI_ATTACH)||
            (mm_pending_lu_type == MM_LU_CAUSE_NORMAL))
        {
          if(mm_serving_plmn.info.active_rat== SYS_RAT_GSM_RADIO_ACCESS && mm_waiting_for_stop_mode_cnf == FALSE)
          {
          mm_initiate_lu( mm_pending_lu_type );
        }
          else
          {
            mm_lu_pended_nmo_unknown = TRUE;
          }
        }
#ifdef FEATURE_DUAL_DATA
        mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_LOW);
#endif 
        gmm_last_signalling_message_type = 0;
      }
#ifdef FEATURE_NAS_FREE_FLOAT
      else if(gmm_last_signalling_message_type == GMM_DETACH_ACCEPT)
      {
        if (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_PS_INVALID)
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= ACK FAILURE Marking SIM as ILLEGAL");
          mmsim_mark_sim_as_illegal();
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= Resending Detach accept message");
          gmm_send_mt_detach_accept_message();
        }
      }
#endif  
      else if(gmm_last_signalling_message_type == GMM_DETACH_REQUEST)
      {
         if(gmm_power_down_active && (mm_timer_status[TIMER_POWERDOWN] == TIMER_ACTIVE))
         {
           if (mmcoord_pending_reg_message.cmd.hdr.message_set == MS_MM_REG &&
                mmcoord_pending_reg_message.cmd.hdr.message_id == MMR_STOP_MODE_REQ)
           {
             MSG_HIGH_DS_0(MM_SUB, "=MM= Retrying for Detach until TIMER_POWERDOWN expires\n");
             gmm_initiate_gprs_detach( &mmcoord_pending_reg_message );
           }
           else
           {
             MSG_HIGH_DS_0( MM_SUB, "=MM= Not retrying GPRS Detach\n");
           }
           
         }
      }
      else
      { 
        MSG_HIGH_DS_1(MM_SUB, "=MM= GLIF: Ignore STATUS IND for message type = %d ",gmm_last_signalling_message_type);
      } 
    }
    else if (primitive->cmd.llgmm_status_ind.cause == GLLCMM_L2_TX_SUCCESS )
    {

#if defined(FEATURE_LTE)
      /* Once we get status indication with success then rach is successful, that means CCO is over. If we havent set 
         the SG association flag on combined RAU start because of CCO then set it to TRUE now.*/
      if(((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)&&
         ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) || (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))&&
         (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_NOT_REQ)))
      {
        emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
        MSG_HIGH_DS_0(MM_SUB,"=MM= Set SGs assosicaiton flag - Combined RAU, CCO completed");
      }
#endif
      if ((gmm_last_signalling_message_type == GMM_ATTACH_COMPLETE  ) ||
          (gmm_last_signalling_message_type == GMM_ROUTING_AREA_UPDATE_COMPLETE)) 
      { 
#ifdef FEATURE_DUAL_DATA
        mm_update_data_priority(MM_DATA_PRI_CLIENT_GMM, SYS_DATA_PRIORITY_LOW);
#endif 
        if ((mm_pending_lu_type == MM_LU_CAUSE_IMSI_ATTACH)||
           (mm_pending_lu_type == MM_LU_CAUSE_NORMAL))
        {
          if(mm_serving_plmn.info.active_rat== SYS_RAT_GSM_RADIO_ACCESS && mm_waiting_for_stop_mode_cnf == FALSE)
          {
          mm_initiate_lu( mm_pending_lu_type );
        }
          else
          {
            mm_lu_pended_nmo_unknown = TRUE;
          }
        }
        gmm_last_signalling_message_type = 0;
      }
      else if (gmm_last_signalling_message_type == GMM_DETACH_REQUEST) 
      { 
        if (gmm_power_down_active)
        {
          mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,
                                                           MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
          gmm_handle_gprs_detach_low_level_failure();
        }
      }
#ifdef FEATURE_NAS_FREE_FLOAT
      else if(gmm_last_signalling_message_type == GMM_DETACH_ACCEPT) 
      { 
#ifdef FEATURE_LTE
        if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
        {
           mm_send_mmr_ps_detach_cnf();
           mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
        }
#endif
        mm_stop_timer(TIMER_NW_INIT_DETACH);
        glif_execute_pended_gmm_triggers();

        if (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_PS_INVALID)
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= ACK SUCC Marking SIM as ILLEGAL");
          mmsim_mark_sim_as_illegal();
        }
      }
#endif    
      else if(gmm_last_signalling_message_type == GMM_ROUTING_AREA_UPDATE_REQUEST)
      {
        gmm_rau_tx_is_acked = TRUE;
      }
      else
      {
        MSG_HIGH_DS_1(MM_SUB, "=MM= GLIF: Ignore STATUS IND for message type = %d ",gmm_last_signalling_message_type );
      } 
    }
  } 
  else
  {
    MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF: Rcvd STATUS for PD = %d Msg and trasaction id = %d",
                           primitive->cmd.llgmm_status_ind.protocol_disc, 
                          primitive->cmd.llgmm_status_ind.transaction_id );
  }

}

#endif /* #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM */

/*===========================================================================

FUNCTION      GMMLLC_HANDLE_LLC_EVENT

DESCRIPTION
  This is an GMM-LLC module's interface function for processing of the
  incoming LLC events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmmllc_handle_llc_event
(
  /* Incoming LLC primitive */
  mm_cmd_type     *primitive
)
{
  boolean routine_man_selection_failed = FALSE ;
  boolean is_cell_forbidden = FALSE;

  if (!((PLMN_MATCH( gmm_current_lai.PLMN_id.identity,
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
        (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
         sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))  &&
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    routine_man_selection_failed = TRUE;
    MSG_HIGH_DS_0(MM_SUB, "=MM= Different PLMN found during MAN_LIMITED_ACQ");
  }
  
  switch (primitive->cmd.hdr.message_id)
  {
    /*
    ** The following primitives are triggered by reception of the various OTA messages
    */
    case GLLCMM_LL_UNITDATA_IND :
    case GLLCSMS_LL_UNITDATA_IND :
    case GLLCMM_LL_XID_IND :
    case GLLCSMS_LL_XID_IND :
    case GLLCMM_LLGMM_NEW_TLLI_IND:
      {
        uint32 current_tlli = primitive->cmd.llgmm_new_tlli_ind.new_tlli ;

        /*
        ** If the incoming LLC primitive was addressed by Local TLLI which has been
        ** derived from new P-TMSI and old TLLI has not been invalidated yet OLD TLLI
        ** unassignment is to be applied
        */
        if ((GLIF_GET_TLLI_TYPE(current_tlli) == 0x03) &&
            GLIF_TLLI_CMP(glif_tlli,current_tlli) &&
            (!GLIF_TLLI_CMP(glif_old_tlli,(uint32)0xFFFFFFFF)))
        {
          glif_unassign_old_tlli() ;
        }
      }
      break ;
    default:

      /* nothing to do */

      break ;
  }

  switch (primitive->cmd.hdr.message_id)
  {
    case GLLCMM_LL_UNITDATA_IND :
      glif_process_ll_unitdata_ind(primitive);

      if ((primitive->cmd.ll_unitdata_ind.l3_pdu[1] == GMM_DETACH_REQUEST) &&
          (gmm_state == GMM_REGISTERED_INITIATED))
      {
        glif_sending_gmm_message = FALSE ;
      }

      break;

    case GLLCSMS_LL_UNITDATA_IND :
      glif_process_llsms_unitdata_ind(primitive);
      break;

    case GLLCMM_LL_XID_IND :
      glif_process_ll_xid_ind(primitive) ;
      break ;

    case GLLCSMS_LL_XID_IND :
      glif_process_llsms_xid_ind(primitive) ;
      break;
    case GLLCMM_LLGMM_SUSPEND_CNF :

      MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Rcvd GLLCMM_LLGMM_SUSPEND_CNF") ;

      switch (glif_llc_connection_state)
      {
        case LLC_RESUMED :
          MSG_ERROR_DS_0(MM_SUB, "=MM= LLC has not been suspended") ;
          return ;
        case LLC_SUSPENSION_REQUESTED :
          {
            if( (mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                  ((routine_man_selection_failed) ||
                   (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                           SYS_NETWORK_SELECTION_MODE_LIMITED)||
                    ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
                       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                            SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) ||
                      ((!gmm_manual_mode_user_selection_procedure)&&
                       ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                            SYS_NETWORK_SELECTION_MODE_MANUAL) ||
                        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                            SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))))&& 
                     (
#ifdef FEATURE_DUAL_SIM
                      (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id )) ||
                      (reg_sim_per_subs_plmn_forbidden(gmm_current_lai.PLMN_id,(sys_modem_as_id_e_type)mm_sub_id )) ||
#else
                      (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) ||
                      (reg_sim_plmn_forbidden(gmm_current_lai.PLMN_id)) ||
#endif
                      (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
                      (mm_check_forbidden_regional_roaming( &gmm_current_lai ))))))
            {
                is_cell_forbidden = TRUE;
                MSG_HIGH_DS_0(MM_SUB, "=MM= Cell is forbidden");
            } 
            glif_llc_connection_state = LLC_SUSPENSION_CONFIRMED ;

            /* Make sure we are not waiting for PCCCH Camp Confirm before releasing
               Pending OTA messaging */
#ifdef FEATURE_NAS_FREE_FLOAT
            if(gmm_pended_rat_resume_req)
            { 
                gmm_sm_send_rat_resume_req();
                gmm_pended_rat_resume_req = FALSE;
            }
            if (glif_grr_ser_req_state != GLIF_SERVICE_REQUESTED)
#else
            if (glif_grr_camp_state != GLIF_CAMP_ON_PCCCH_REQUESTED)
#endif
            {
              if (!is_cell_forbidden)
              {
#ifdef FEATURE_NAS_FREE_FLOAT
                if(glif_service_req_pended_ind == TRUE)
                {
                  glif_grr_send_service_req() ;
                }

                if( glif_grr_ser_req_state == GLIF_SERVICE_CONFIRMED)
#endif
                {
                  glif_process_pended_gmm_commands() ;
                }
              }
              else
              {
                if (glif_pended_mt_detach_accept_ind)
                {
#ifndef FEATURE_NAS_FREE_FLOAT
                  glif_grr_send_service_confirm() ;
#endif
                
                  /* Send pended MT Detach Accept message */
                  glif_send_gmm_ota_message(GLIF_BUFFERED_DETACH_ACCEPT_LENGTH,
                                                                 glif_pended_mt_detach_accept
#ifdef FEATURE_GPRS_PS_HANDOVER
                                                                 ,FALSE
#endif                                                  
                                           ) ;
            
                  glif_pended_mt_detach_accept_ind = FALSE ;
                }
#ifndef FEATURE_NAS_FREE_FLOAT
                else if (glif_service_confirm_pended_ind)
                {
                  /* Permit user plane transfers from now on */
                  glif_grr_send_service_confirm();
                }
#endif
              }
            }
          }
          break ;
        case LLC_SUSPENSION_CONFIRMED :
          MSG_HIGH_DS_0(MM_SUB, "=MM= GLIF: Suspension duplicate detection") ;
          return ;
        default :
          MSG_ERROR_DS_1(MM_SUB, "=MM= Illegal LLC connection state (%d)",glif_llc_connection_state) ;
          return ;
      }
      break ;

#ifdef FEATURE_GPRS_PS_HANDOVER     
    case GLLCMM_LLGMM_PSHO_CNF :
      glif_process_llgmm_psho_cnf ();
      break;     
#endif 

    case GLLCMM_LLGMM_STATUS_IND :
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM              
      glif_process_llgmm_status_ind(primitive); 
      break;
#endif /* #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM */      
      
    case GLLCMM_LL_STATUS_IND :
    case GLLCMM_LL_RESET_IND :
    case GLLCSMS_LL_RESET_IND :
    case GLLCSMS_LL_STATUS_IND :
    case GLLCMM_LLGMM_NEW_TLLI_IND:
      /* These messages are to be expected, but no action need be taken. */
      break;

    default:
      {
        gmm_llc_if_state_type next_glif_state = glif_state ;

        switch (glif_state)
        {
          case GMM_IDLE_STATE :
            next_glif_state = glif_llcif_idle_state_handler(primitive) ;
            break ;
          case GMM_READY_STATE :
            next_glif_state = glif_llcif_ready_state_handler(primitive) ;
            break ;
          case  GMM_STANDBY_STATE :
            next_glif_state = glif_llcif_standby_state_handler(primitive) ;
            break ;
          default :
            MSG_FATAL_DS(MM_SUB, "=MM= Invalid state (%d)",(int)glif_state,0,0);
            break ;
        } /* switch (glif_state) */

        if (glif_state != next_glif_state)
        {
          MSG_HIGH_DS_2(MM_SUB, "=MM= GLIF state change from %d to %d\n",
                                                glif_state,next_glif_state) ;

          glif_state = next_glif_state ;

          /* Notify GRR layer that the GPRS GMM state has been changed */
          glif_grr_state_exchange(next_glif_state) ;
        }

      }
      break ;
  } /* switch (message->cmd.hdr.message_id) */

} /* end gmmllc_handle_llc_event() */

/*===========================================================================

FUNCTION      GMMLLC_INIT

DESCRIPTION
  This is GMM-LLC module's interface function is to be called upon power up
  of the protocol stack in order to attach the GMM layer to the LLC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmmllc_init
(
  void
)
{
  /* Initialize GMM-LLC service state */
  glif_state = GMM_IDLE_STATE ;
  memset (glif_old_tlli, 0xff, sizeof(glif_tlli_type));
  memset (glif_tlli, 0xff, sizeof(glif_tlli_type));
  glif_ready_timer_value = 44000 ;
#ifdef FEATURE_NAS_FREE_FLOAT
  glif_service_req_pended_ind = FALSE ;
  glif_grr_ser_req_state = GLIF_NO_SERVICE;
#else
  glif_service_confirm_pended_ind = FALSE ;
  glif_grr_camp_state = GLIF_CAMP_ON_CCCH;
#endif

  glif_llc_connection_state = LLC_SUSPENSION_CONFIRMED ;

  glif_foreign_tlli_assigned = FALSE ;

  glif_pended_cell_update = FALSE ;

  glif_pended_cell_update_is_initial = FALSE ;

  glif_pended_attach_req = FALSE ;

  glif_pended_rau_req = FALSE ;

  glif_pended_detach_req = FALSE ;

  glif_attach_in_progress = FALSE;

  glif_rau_in_progress = FALSE;

  glif_sending_gmm_message = FALSE ;

  glif_detach_in_progress = FALSE ;

#ifdef FEATURE_MODEM_HEAP
  if(glif_pended_gmm_msg != NULL)
  {
    modem_mem_free(glif_pended_gmm_msg,MODEM_MEM_CLIENT_NAS);
    glif_pended_gmm_msg = NULL;
  }
  if(glif_pended_detach != NULL)
  {
    modem_mem_free(glif_pended_detach,MODEM_MEM_CLIENT_NAS);
    glif_pended_detach = NULL;
  }
#endif
} /* end gmmllc_init() */

/*===========================================================================

FUNCTION      GMMLLC_REINIT

DESCRIPTION
  This is GMM-LLC module's interface function is to be called in order to
  re-initialize the interface' state variable into particular state :
  o GMM_IDLE_STATE
  o GMM_STANDBY_STATE
  o GMM_READY_STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes GMM/GPRS interface' state.

===========================================================================*/
void gmmllc_reinit
(
  gmm_llc_if_state_type required_state
)
{
  if (glif_state != required_state)
  {
    switch (required_state)
    {
      case GMM_IDLE_STATE:

        if ( !glif_ready_timer_deactivated )
        {
          glif_stop_ready_timer(FALSE) ; 
        }
#ifdef FEATURE_GSM_GPRS 

#ifdef FEATURE_NAS_FREE_FLOAT
        /* Packet services are no longer required. Indicate to GRR so that
        ** GRR need only camp on CCCH for GSM services. 
        */ 
        gmmllc_reset_service_state(); 
#else
        glif_grr_send_camp_on_ccch_indication();
#endif 
#endif /*#ifdef FEATURE_GSM_GPRS*/

        glif_state = GMM_IDLE_STATE ;

        break ;
      case GMM_STANDBY_STATE:
        /* Initiate LLC & GRR by sending TLLI assignment request to the LLC */

        if ((gmm_update_status != GMM_GU1_UPDATED) ||
            (gmm_rai_changed(glif_ra_changed_rau_completed_ra.location_area_code,
                             glif_ra_changed_rau_completed_ra.routing_area_code,
                             mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
            (gmm_plmn_changed(glif_ra_changed_rau_completed_ra.plmn_id,
                              mm_serving_plmn.info.plmn)))
        {
          glif_tlli_assign(glif_get_tlli(GLIF_FOREIGN_TLLI_TYPE),FALSE) ;

          glif_foreign_tlli_assigned = TRUE ;
        }
        else
        {
          glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;
        }

        glif_pended_cell_update = FALSE ;
        glif_pended_cell_update_is_initial = FALSE ;

        /* Request that GRR camp for packet services on PCCCH if available
        ** in anticipation of an impending registration attempt.
        */
#ifndef FEATURE_NAS_FREE_FLOAT
        if ( glif_grr_camp_state == GLIF_CAMP_ON_CCCH )
        {
           glif_grr_send_camp_on_pccch_request() ;
        }
#endif

        glif_state = GMM_STANDBY_STATE ;
        break ;

      case GMM_READY_STATE:
        /*
        ** Initiate LLC by sending TLLI assignment request to the LLC
        ** and start READY timer
        */
        /* Initiate LLC & GRR by sending TLLI assignment request to the LLC */
        if ((gmm_rai_changed(glif_ra_changed_rau_completed_ra.location_area_code,
                             glif_ra_changed_rau_completed_ra.routing_area_code,
                             mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
            (gmm_plmn_changed(glif_ra_changed_rau_completed_ra.plmn_id,
                              mm_serving_plmn.info.plmn)))
        {
          glif_tlli_assign(glif_get_tlli(GLIF_FOREIGN_TLLI_TYPE),FALSE) ;

          glif_foreign_tlli_assigned = TRUE ;
        }
        else
        {
          glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;
        }

        if (!glif_ready_timer_deactivated)
        {
          /* Command the LLC start READY timer */
          glif_start_ready_timer(glif_ready_timer_value) ;
        }

        /* Request that GRR camp for packet services on PCCCH if available
        ** in anticipation of an impending registration attempt.
        */
#ifdef FEATURE_NAS_FREE_FLOAT
        if ( glif_grr_ser_req_state == GLIF_NO_SERVICE)
        {
          glif_grr_send_service_req() ;
        }
#else
        if ( glif_grr_camp_state == GLIF_CAMP_ON_CCCH )
        {
          glif_grr_send_camp_on_pccch_request() ;
        }
#endif

        glif_state = GMM_READY_STATE ;
        break ;

      default :
        MSG_FATAL_DS(MM_SUB, "=MM= GLIF: unknown state %d",(int)required_state,0,0) ;
        break ;
    } /* switch (required_state) */
  }

  /* Notify GRR layer that the GPRS GMM state has been changed */
  glif_grr_state_exchange(glif_state) ;

} /* end gmmllc_reinit() */

/*===========================================================================

FUNCTION      GMMLLC_GMM_TLLI_ASSIGNMENT

DESCRIPTION
  This is GMM-LLC module's interface function is to be called in order to
  assign new TLLI to LLC/GRR entitides

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmmllc_gmm_tlli_assignment
(
  void
)
{
  /* Initiate LLC & GRR by sending TLLI assignment request to the LLC */
  if ((gmm_rai_changed(glif_ra_changed_rau_completed_ra.location_area_code,
                          glif_ra_changed_rau_completed_ra.routing_area_code,
                                    mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
      (gmm_plmn_changed(glif_ra_changed_rau_completed_ra.plmn_id,
                                    mm_serving_plmn.info.plmn)))
  {
    glif_tlli_assign(glif_get_tlli(GLIF_FOREIGN_TLLI_TYPE),FALSE) ;

    glif_foreign_tlli_assigned = TRUE ;
  }
  else
  {
    glif_tlli_assign(glif_get_tlli(GLIF_LOCAL_TLLI_TYPE),FALSE) ;
  }
} /* end gmmllc_gmm_tlli_assignment() */

/*===========================================================================

FUNCTION      gmmllc_get_glif_state

DESCRIPTION
  This function is to make the static variable glif_state accessible to other files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
gmm_llc_if_state_type gmmllc_get_glif_state
(
  void
)
{
  return glif_state;
} /*gmmllc_get_glif_state*/

/*==========================================================================

FUNCTION      GMMLLC_GET_GLIF_GRR_CAMP_STATE

DESCRIPTION
  This function facilitates the use of glif_grr_camp_state in files other than gmmllcm.c 

DEPENDENCIES
  None

RETURN VALUE
  glif_grr_camp_state_type

SIDE EFFECTS
  None
  
==========================================================================*/
glif_grr_camp_state_type gmmllc_get_glif_grr_camp_state
(
  void
) 
{
  return glif_grr_camp_state;
} /*gmmllc_get_glif_grr_camp_state*/
/*==========================================================================
FUNCTION      GMMLLC_GET_GLIF_GRR_SERVICE_REQ_STATE
     
DESCRIPTION
  This function facilitates the use of glif_grr_service_request_state in files other than gmmllcm.c
     
DEPENDENCIES
  None
     
RETURN VALUE
  glif_grr_ser_req_state_type
     
SIDE EFFECTS
  None
==========================================================================*/
glif_grr_ser_req_state_type gmmllc_get_glif_grr_service_req_state
(
  void
)
{
  return glif_grr_ser_req_state;
}
/*==========================================================================
FUNCTION      GMMLLC_GET_GLIF_LLC_CONN_STATE
DESCRIPTION
  This function facilitates the use of glif_service_conn_state in files other than gmmllcm.c
DEPENDENCIES
  None
RETURN VALUE
  glif_llc_connection_state_type
SIDE EFFECTS
  None
=========================================================================*/
glif_llc_connection_state_type gmmllc_get_glif_llc_conn_state
(
  void
)
{
  return glif_llc_connection_state;
}
#ifdef FEATURE_NAS_FREE_FLOAT
/*==========================================================================

FUNCTION      GMMLLC_SEND_SERVICE_REQ_IF_NEEDED

DESCRIPTION
  This function sends service request, if rr plmn select cnf is received, and no service request 
  was sent

DEPENDENCIES
  None

RETURN VALUE
  gmmllc_send_service_req_if_needed

SIDE EFFECTS
  None
  
==========================================================================*/

void gmmllc_send_service_req_if_needed()
{
  if((glif_grr_ser_req_state == GLIF_NO_SERVICE) && 
     (glif_llc_connection_state != LLC_SUSPENSION_REQUESTED) &&
     (GLIF_XLATE_TLLI(glif_tlli) != 0xFFFFFFFF))
  {
    glif_grr_send_service_req();
  }
}
#endif
#endif /* FEATURE_GSM_GPRS */

