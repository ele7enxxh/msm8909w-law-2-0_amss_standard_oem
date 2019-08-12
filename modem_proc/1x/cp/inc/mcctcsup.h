#ifndef MCCTCSUP_H
#define MCCTCSUP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C D M A   T R A F F I C   C H A N N E L   S T A T E
         S U P P O R T   R O U T I N E S   H E A D E R   F I L E

GENERAL DESCRIPTION
  This module externalizes the support routines for the CDMA Sub-system,
  Mobile Station Control on the Traffic Channel state.

  Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctcsup.h_v   1.15   02 Oct 2002 16:04:50   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mcctcsup.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/16   ppr     1x2GTA Feature changes
10/04/13   ppr     New API to process one rxtx cmd when queue is full
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
05/24/12   ppr     Feature Cleanup
02/15/12   srk     Feature Cleanup: Removed FEATURE_GPS_MODE
02/04/12   ppr     Feature Cleanup
01/19/12   ppr     Reverting feature cleanup changes for
                   FEATURE_IS2000_REL_A
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
05/02/11   ssh     Added Diag request support for getting SPM, ESPM, ITSPM.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
12/22/10   ag      Removed unused function so_conv().
11/15/10   jtm     Initial SU API split for CP.
07/22/10   ag      Added support for GEM reject cases.
09/28/09   ag      1X Advanced Signaling Support - Phase 2
04/13/09   adw     Added tdso_v.h include for CMI.
12/14/07   an      Added Passport Mode
01/15/07   sb      VOIP-1X handoff updates.
05/17/06   fh      Merged in new development of AKA/AES features
01/20/06   fc      Merged the following :
           fh      Added support for AKA and MI.
08/20/05   va      Made tc_inform_scr_change_init_service external
07/11/05   fh      Cleaned up header file including from MUX:
                     removed including of rxc.h and txc.h
07/07/05   va      Merged the following from MSMSHARED:
07/05/05   sb       Removed tc_get_plc (no longer used).
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/03/05   sb      Removed tc_get_plc.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
11/05/04   va      Merged following from REL_B_C archive
11/04/04   fc        Fixed lint errors.
11/05/04   sb      Removed tc_populate_scr_info.
06/15/04   pg      Added support for Rate Change Message.
05/17/04   sb      Added mcctcsup_update_off_time_info.
03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
11/11/02   va      Corrected featurization around process_erm()
11/07/02   az      Mainlined FEATURE_IS95B
10/15/02   yll     Merged in control hold changes.
10/02/02   az      Mainlined FEATURE_IS2000
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/01/02   az      Added FEATURE_CDSMS
05/02/02   lh      Fixed UI/CM/MC out of sync for the 2nd call.
04/17/02   ph      Removed mcctcsup_validate_scm() as the functionality is now captured
                   in srv_process_con_msg().
01/23/02   ph      Added EOM error code if SNM initialization fails.
01/14/02   lh      Changed return interface for CC info processing in SCM and UHDM.
01/11/02   va      Fixes to make code work when SVD is not defined.
08/28/01   lh      Support for concurrent service.
06/26/01   va      Merged following from MSM_CP.05.00.81
                   Added authentication interfaces for new UIM server.
06/14/01   va      Changed prototype to send_rej_order.
05/17/01   ph      Merged from MSM_CP.05.00.76
                   04/10/01   jq      Added msg processing releated to P2 & control hold
04/09/01   va      Added prototype for validate_pilot_rec_type function.
03/28/01   va      Added prototype for populate_pil_rec_info_in_rtc_msg.
02/26/01   mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE to move code needed
                   only for GPS part of position location to a new #define.
01/24/01   ych     Merged T-53/JCDMA features for SK.
12/15/00   lh      Add support for MS originated Long Code Transistion Request
                   Order
11/30/00   yll     Removed FEATURE_GPSONE_TAWD_TEST_ONLY.
11/28/00   ck      Changed the return type and added another parameter for
                   tc_auth_rpt().
11/04/00   ks      Added prototypes for send_srv_ctl_tdso_ctl_dir() and
                   send_srv_ctl_tdso_stat().
10/27/00   jq      Merged T_AUTH to FEATURE_AUTH.
09/11/00   yll     Added support for Position Determination DBM.
08/18/00           Added prototype for process_retry_order().
08/02/00   yll     Added GPSONE TAWD (tuning away w/o drop) testing support.
06/19/00   yll     Added extern definition for tc_block_srch_gps_visit(),
                   tc_gps_in_progress, tc_gps_init().
06/01/00   fc      Added prototype for process_outer_loop_report_order().
04/13/00   lcc     Added prototype for tc_trans_done_rpt.
03/13/00   ry      Added IS2000 featurization
11/15/99   va      IS2000 Changes:
                   tc_get_plc added for getting the value of PLC
                   tc_populate_scr_info added to populate the fields of SCR info
                   in the interfaces to lower layers from pending service config.
                   process_pwr_ctrl_msg added to proces the enhanced PCM.
08/30/99   ry      Added extern definition for send_plc_req_ord()
08/13/99   jq      Added tc_send_flash_info_msg().
07/28/99   lh      CM and two-way SMS support added.
07/08/99   ych     Added support for Periodic Pilot Strength Measurment
                   Message
05/26/99   fc      Added support for closed loop power control step size.
05/19/99   doj     Moved extern declaration of SEND_HANDOFF_COMPLETE_MSG,
                   PROCESS_FM_HO_MSG and HANDOFF_TO_CURRENT_ASET to
                   mcctcho.h.  Removed extern declaration of
                   INIT_FRAME_OFFSET_HANDOFF, PROCESS_HO_DIR_MSG,
                   PROCESS_EXT_HO_DIR_MSG, GHDM_VALID_AND_HANDOFF_OK and
                   PROCESS_GEN_HO_DIR_MSG and consolidated them in the
                   extern declaration for MCCTCHO_PROCESS_GENERIC_HANDOFF_MSG
                   in mcctcho.h.
02/13/99   doj     Added support for General Handoff Direction Message (GHDM)
                   and FEATURE_SPECIAL_MDR.
08/25/98   ck      Externalized the functions tc_dh_rpt.
09/21/96   rdh     ROM savings by consolidating header fill-ins of RXTX mssgs.
08/08/96   dna     process_tc_data_burst() now can change next_state if too
                   many OTASP subsidy unlock attempts fail.
06/25/96   dna     Externalized SEND_TC_DATA_BURST for FEATURE_OTASP.
05/30/96   dna     Created header.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "caii.h"
#include "comdef.h"
#include "mc.h"
#include "mccrx.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"
#include "qw.h"
#include "rxtx.h"
#include "rxtx_v.h"

#include "auth.h"
#include "tdso.h"
#include "tdso_v.h"

#include "diagpkt.h"
#ifdef FEATURE_IS2000_REL_A_SVD
#include "mcctctag.h"
#include "mccsrid.h"
#endif /* FEATURE_IS2000_REL_A_SVD */

#include "dh.h"

#if defined(FEATURE_IS2000_REL_C)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C */

#include "auth_v.h"
#include "dh_v.h"
#include "mccdma_i.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* MC uses this flag to determine wheter the PPSMM is enabled or not. If
 * mcctcsup_ppsmm_enabled is set to TRUE, it means that PPSMM Processing
 * is enabled, else disabled.  This flag is used to determine whether
 * to really send a SRCH_STOP_PPSM_F to SRCH in case of GHDM, EHDM
 * processing and one time PPSMM. MC will only send SRCH_STOP_PPSM_F if
 * PPSMM is enable. This way MC will not unneccsarily send CMDS to SRCH
 */
extern boolean mcctc_ppsm_enabled;

/* This struct contains all the info needed for a Reject
Order on the Traffic Channel */
typedef struct
{
  byte rej_msg_type;
    /* Message type of rejected message */
  byte rej_code;
    /* Indicator of reason why message was rejected */
  byte rej_order;
    /* Order type of rejected message - only filled in if
       rej_msg_type == Order */
  byte rej_ordq;
     /* Order qualification code of rejected message - only filled in if
        rej_msg_type == Order */
  word rej_param_id;
    /* Parameter if of rejected message - only filled in if rej_msg_type ==
       Set Parameters or Retrieve Parameters */
  byte rej_record;
    /* Record type of rejected information record only filled in if
       rej_msg_type == Flash with Information or Alert with information */
  byte con_ref;
     /* Needed only if ORDQ is 0x10, 0x11, 0x12, 0x13 or if it
        a CC specific order*/
  byte tag;
     /* Needed only if ORDQ is  0x13 */

} tc_rej_ord_parms_type;

#ifdef FEATURE_1XCP_FTD

/* Global structure to hold the in traffic system parameters. */
extern caii_tc_sysparm_type            itspm;

/* Boolean to track if the in traffic system parameter message has been received.
 */
extern boolean itspm_rxed;

#endif /* FEATURE_1XCP_FTD */
/* <EJECT> */
/*===========================================================================

FUNCTION MCCTC_FILL_RXTX_HDR_ACK_Q

DESCRIPTION
  This function fills in the header of the RXTX_TC_MSG command for
  messages which need to be returned to the ack queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcctc_fill_rxtx_hdr_ack_q
(
  rxtx_cmd_type *cmd_ptr
   /* Pointer to command to send to layer 2 task */
);

/* <EJECT> */
/*===========================================================================

FUNCTION MCCTC_FILL_RXTX_HDR_FREE_Q

DESCRIPTION
  This function fills in the header of the RXTX_TC_MSG command for
  messages which do NOT need to be returned to the ack queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcctc_fill_rxtx_hdr_free_q
(
  rxtx_cmd_type *cmd_ptr
   /* Pointer to command to send to layer 2 task */
);

/*===========================================================================
FUNCTION MCCTC_PROC_RXTX_Q_FULL

DESCRIPTION
  This function will process one rxtx cmd to make room for one more cmd.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcctc_proc_rxtx_q_full( void );

/* <EJECT> */
/*===========================================================================

FUNCTION SEND_REJ_ORDER

DESCRIPTION
  This function builds and sends a Mobile Station Reject Order to the
  layer 2 task to be sent to the Base Station.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void send_rej_order
(
  tc_rej_ord_parms_type * rej_parms_ptr
);


#ifdef FEATURE_UIM_RUIM
/* <EJECT> */
/*===========================================================================

FUNCTION MC_BS_CHALLENGE

DESCRIPTION
  This function sends a command to the Authentication Task to perform a Base
  Station challenge.  This is to start the SSD Update process when using an UIM
  card for authentication.  It then sends a BS Challenge command to auth and does
  NOT wait for the response.  The response is processed by tc_auth_rpt.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The response is processed by tc_auth_rpt.  This is where the SSD update
  command is sent as well as the Base Station Challenge order.

===========================================================================*/
void mc_bs_challenge
(
  qword randssd
    /* Random number given by base station for use in SSD generation */
);
#endif /* FEATURE_UIM_RUIM */

/* <EJECT> */
/*===========================================================================

FUNCTION START_SSD_UPDATE

DESCRIPTION
  This function sends a command to the Authentication Task to begin the
  SSD Update process.  It then sends a BS Challenge Order, which forces
  the base station to authenticate itself before we agree to update our SSD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the SSD Update can not be started, the SSD Update Message is rejected.

===========================================================================*/
extern void start_ssd_update
(
  qword randssd,
    /* Random number given by base station for use in SSD generation */
  dword randbs
    /* Random number from mobile for use in SSD generation. */
);

/* <EJECT> */
/*===========================================================================

FUNCTION SEND_AUTH_RESP_MSG

DESCRIPTION
  This function sends an Authentication Response Message with the
  auth signature value passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void send_auth_resp_msg
(
  dword authu
    /* Authentication Signature to be included with message */
);

/* <EJECT> */
/*===========================================================================

FUNCTION SEND_SSD_UPDATE_ORD

DESCRIPTION
  This function queries the authentication task to find out if the BS
  Challenge was successful, and sends an SSD Update Accepted or SSD
  Update Rejected order as appropriate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void send_ssd_update_ord
(
  dword authbs
    /* Value of AUTHBS calculated by Base Station */
);

/* <EJECT> */
/*===========================================================================

FUNCTION SEND_PARAM_UPDATE_ORD

DESCRIPTION
  This function sends a Parameter Update Confirmation order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void send_param_update_ord
(
  byte ordq
    /* Value of ordq field of parameter update order */
);

/* <EJECT> */
/*===========================================================================

FUNCTION SEND_PLC_REQ_ORD

DESCRIPTION
  This function sends a Long Code Transition Request Order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void send_plc_req_ord
(
  byte req_ordq
    /* Value of ordq field of Long Code Transition Request Order */
);

/*===========================================================================

FUNCTION PROCESS_PRIVACY_PREF

DESCRIPTION
  This function processes a privacy preferenc request from CM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_privacy_pref
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
);

/* <EJECT> */
/*===========================================================================

FUNCTION START_AUTH_SIGNATURE

DESCRIPTION
  This function sends a command to AUTH to perform the auth_signature
  for an auth challenge message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If we can not perform the auth signature we reject the auth challenge
  message.

===========================================================================*/

extern void start_auth_signature ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION TC_AUTH_RPT

DESCRIPTION
  This function processes a report from the Authentication task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern word tc_auth_rpt
(
  auth_rpt_type *auth_rpt_ptr,
    /* Pointer to authentication report */
  word next_state
    /* Next state */
);


/*===========================================================================

FUNCTION GET_ABS_ACTION_TIME

DESCRIPTION
  This function converts an action time in units of mod 64 super frame
  into an absolute time in frames for lower layers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The input argument abs_action_time is passed by address and contains the return value

===========================================================================*/

extern void get_abs_action_time
(
  qword abs_action_time,
  byte action_time
);

/*===========================================================================

FUNCTION GET_TA_ABS_ACTION_TIME

DESCRIPTION
  This function converts an action time in units of mod 64 super frame into 
  an absolute time in frames for lower layers. This should be used only if TA
  is enabled since it takes into account the delays in receiving message due
  to message loss in TA Gaps.
  Due to message loss in TA Gap, by the time UE receives the message in one
  of the retries by BS, the action time might have passed and if we calculate
  absolute action time using legacy abs action time API, then transaction might
  be delayed by upto 5.12s due to the action time rollover.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The input argument abs_action_time is passed by address and contains the return value

===========================================================================*/

extern void get_ta_abs_action_time
(
  qword abs_action_time,
  byte action_time
);

/*===========================================================================

FUNCTION tc_send_rel_to_rxc_txc

DESCRIPTION
This function sends release command to RXC/TXC making them release their
Service Option (SO)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void tc_send_rel_to_rxc_txc( void );


/* <EJECT> */
/*===========================================================================

FUNCTION SEND_ORIG_C_MSG

DESCRIPTION
  This function sends an Origination Continuation Message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void send_orig_c_msg( void );

/* <EJECT> */
/*===========================================================================

FUNCTION TC_ACTION_CHK

DESCRIPTION
  This function checks the action time in a message.  If the action time
  is valid, it sets the action_timer to wake up on the action time and
  copies the received message to the tc_action_msg buffer.  If the action
  time is invalid, it sends a Mobile Station Reject Order to the base station
  indicating that the message was invalid.  If the function finds that the
  action timer is already set, this function will send a Mobile Station
  Reject Order since IS-95 only allows for one message with an outstanding
  action time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See function description.

===========================================================================*/

extern void tc_action_chk
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to message with action time field */
);

/* <EJECT> */
/*===========================================================================

FUNCTION TC_SO_CTL

DESCRIPTION
  This function processes a received Service Option Control Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void tc_so_ctl( caii_rx_msg_type *msg_ptr );

/*===========================================================================

FUNCTION PROCESS_OUTER_LOOP_REPORT_ORDER

DESCRIPTION
  This function processes the input outer loop report request order message.
  The message will be rejected if P_REV_IN_USE is less than 6. Otherwise,
  a command is sent to RXC to request the outer loop report.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_outer_loop_report_order
(
  caii_rx_msg_type * msg_ptr
    /* Pointer to the received outer loop report request order message */
); /* process_outer_loop_report_order */

/*===========================================================================

FUNCTION PROCESS_RETRY_ORDER

DESCRIPTION
  This function processes the input retry order message. The message will be
  rejected if P_REV_IN_USE is less than 6. Otherwise, if the retry type is
  valid, it will inform DS the changes in retry delay if DS wants.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_retry_order
(
  caii_rx_msg_type * msg_ptr
); /* process_retry_order */



/* <EJECT> */
/*===========================================================================

FUNCTION TC_SRV_CTL_MSG

DESCRIPTION
  This function processes a received Service Option Control Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void tc_srv_ctl_msg( caii_rx_msg_type *msg_ptr );

/* <EJECT> */
/*===========================================================================

FUNCTION REQ_PIL_STR

DESCRIPTION
  This function asks SRCH to send a Pilot Strength Measurement message,
  in response to a Pilot strength measurement request order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void req_pil_str( void );

/* <EJECT> */
/*===========================================================================

FUNCTION REQ_PIL_STR

DESCRIPTION
  This function asks SRCH to send a Pilot Strength Measurement message,
  in response to a Pilot strength measurement request order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void req_ppm_rpt( mccrxtx_msg_type *msg_ptr );


/* <EJECT> */
/*===========================================================================

FUNCTION PROCESS_PWR_MSG

DESCRIPTION
  This function processes an input Power Control Parameters Message.
  In response to this message a Power Measurement Report message is
  formatted.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void process_pwr_msg
(
  caii_rx_msg_type *msg_ptr
   /* Pointer to received Power Control Parameters Message */
);

/* <EJECT> */
/*===========================================================================

FUNCTION TC_PROC_ACK

DESCRIPTION
  This function processes messages for which RXTX is required to signal
  MC when the acknowldegement is received for the message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Command buffer will be placed back on the rxtx_md_free_q.

===========================================================================*/

extern void tc_proc_ack
(
  rxtx_cmd_type *cmd_ptr
    /* Pointer to rxtx command taken from mc_rxtx_ack_q */
);

/* <EJECT> */
/*===========================================================================

FUNCTION TC_LOCK_ORD

DESCRIPTION
  This function processes a Lock Until Power-Cycled order, a Maintenance
  Required Order, and an Unlock Order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern word tc_lock_ord
(
  caii_ftc_ord_type *msg_ptr,
    /* Pointer to received Forward Traffic Channel Order Message */
  word curr_state
    /* Traffic Channel substate currently being processed */
);

/* <EJECT> */
/*===========================================================================

FUNCTION TC_MOB_REG

DESCRIPTION
  This function processes the Mobile Station Registered Message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The roam status and the registration variables will be updated.

===========================================================================*/
extern void tc_mob_reg
(
  caii_registered_type *msg_ptr
    /* pointer to received mobile station registered message */
);

/* <EJECT> */
/*===========================================================================

FUNCTION PROCESS_FTC_TMSI_ASSIGN

DESCRIPTION
  This function processes a TMSI Assignment Message received on the
  Forward Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void process_ftc_tmsi_assign
(
  caii_rx_msg_type *msg_ptr
    /* pointer to received Data Burst message */
);

/* <EJECT> */
/*===========================================================================

FUNCTION PROCESS_TC_DATA_BURST

DESCRIPTION
  This function processes a Data Burst Message received on the
  Foward Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
extern word process_tc_data_burst
(
  caii_rx_msg_type *msg_ptr,
    /* pointer to received Data Burst message */
  word next_state
    /* Next state to be processed */
);

#ifdef FEATURE_IS2000_REL_A
/* <EJECT> */
/*===========================================================================

FUNCTION PROCESS_TC_BS_STATUS_RSP

DESCRIPTION
  This function processes a Base Station Status Response Message received
  on the Foward Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
extern void process_tc_bs_status_rsp
(
  caii_bs_status_rsp_type *msg_ptr
    /* pointer to received Base Station Status Response message */
);

/*===========================================================================

FUNCTION SEND_BS_STATUS_RSP_FAIL_TO_CM

DESCRIPTION
  This function is used to send back a failure message to the cm and to apps
  beyond to indicate that the BSStatusReq send was a failure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void send_bs_status_rsp_fail_to_cm( void );

/*===========================================================================

FUNCTION SEND_TC_MO_BSSTATUS_REQ

DESCRIPTION
  This function sends a Base Station Status Request Message to the Base Station
  on the Reverse Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
extern void send_tc_mo_bsstatus_req
(
  uint16 num_pilots,
  /*number pilots for which we need bsid, sid, nid etc*/
  word *pilot_pn_list
  /*list of pilots for which we need bsid, sid, nid etc*/
);

#endif /*FEATURE_IS2000_REL_A*/

/* <EJECT> */
/*===========================================================================

FUNCTION PROCESS_NLU_MSG

DESCRIPTION
  This function processes an input Neighbor List Update message. The
  Neighbor set is update accordingly and a command is sent to the
  Searcher task to use the new neighbor set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void process_nlu_msg
(
     caii_rx_msg_type *msg_ptr
       /* pointer to received Neighbor List Update message */
);

/* <EJECT> */
/*===========================================================================

FUNCTION PROCESS_SP_MSG

DESCRIPTION
  This function processes an input In-Traffic System Parameters message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void process_sp_msg
  (
     caii_rx_msg_type *msg_ptr
      /* pointer to received In-Traffic System Parameters message */
  );

/* <EJECT> */
/*===========================================================================

FUNCTION SEND_PARM_RESP

DESCRIPTION
  This function sends a Parameter Response message in response to a Retreive
  Parameters message.  If this procedure determines that it cannot supply a
  value for a parameter it will instead send a Mobile Station Reject Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void send_parm_resp
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to Retreive Parameters message */
);

/* <EJECT> */
/*===========================================================================

FUNCTION TC_SET_PARM

DESCRIPTION
  This function responds to a Set Parameters message.  If the Set Parameters
  message contains a parameter which cannot be set NONE of the parameters
  will be set.

DEPENDENCIES
  None.

RETURN VALUE
  MC_DONE_S if parameters were set.
  MC_BADPARM_S if a parameter was invalid.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tc_set_parm
(
   caii_rx_msg_type *msg_ptr
     /* Pointer to Set Parameters Message */
);

/* <EJECT> */
/*===========================================================================

FUNCTION TC_SO_REQ

DESCRIPTION
  This function processes a Service Option Request Order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void tc_so_req
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to received SOR Request message */
);

/* <EJECT> */
/*===========================================================================

FUNCTION TC_SO_RESP

DESCRIPTION
  This function processes a Service Option Response Order.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.  If it is not necessary to change the
  current state as a result of the received Service Option Response
  Order, the return state will be equal to the current state.

SIDE EFFECTS
  None

===========================================================================*/

extern word tc_so_resp
(
  caii_rx_msg_type *msg_ptr,
    /* Pointer to received SOR Response message */
  word curr_state
    /* Traffic Channel substate currently being processed */
);

/* <EJECT> */
/*===========================================================================

FUNCTION SEND_STATUS_MSG

DESCRIPTION
  This function sends a Status Message in response to a received
  Status Request Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void send_status_msg
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to Status Request Order */
);

/* <EJECT> */
/*===========================================================================

FUNCTION SEND_STATUS_RSP_MSG

DESCRIPTION
  This function sends a Status Response Message in response to a received
  Status Request Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void send_status_rsp_msg
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to Status Request Message */
);

/* <EJECT> */
/*===========================================================================

FUNCTION TC_RX_RPT

DESCRIPTION
  This function processes reports from the Receive task during the Traffic
  Channel substates.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word tc_rx_rpt
(
  mccrx_rpt_type *rpt_ptr,
    /* pointer to report received from RX */
  word next_state
    /* next state to be processed */
);

/* <EJECT> */
/*===========================================================================

FUNCTION TC_SRCH_RPT

DESCRIPTION
  This function processes the reports from the Searcher task during the
  Conversation substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern mcc_exit_name_type tc_srch_rpt
(
  mccsrch_rpt_type *rpt_ptr,
    /* report received from Searcher */
  mcc_exit_name_type curr_tc_exit
    /* Reason of pending tc exit */
);

/*===========================================================================

FUNCTION TC_TRANS_DONE_RPT

DESCRIPTION
  This function processes the transaction done reports from transaction servers
  (can be RXC, TXC, or SRCH).  Note that no report type is included in this
  report because it is not needed.  This report is currently used only to indicate
  the completion of a particular transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_trans_done_rpt( void );

/* <EJECT> */
/*===========================================================================

FUNCTION MCC_AUTO_ANSWER

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word mcc_auto_answer
(
word next_state
);

/* <EJECT> */
/*===========================================================================

FUNCTION SO_WFA

DESCRIPTION
  This function processes service option actions needed for the Mobile Station
  Control on the Traffic Channel Waiting For Answer substate.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word so_wfa
(
  word next_state
);

/* <EJECT> */
/*===========================================================================

FUNCTION SEND_TC_DATA_BURST
  This function builds an Data Burst message for Reverse Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void send_tc_data_burst
(
  byte burst_type,
    /* Type of Data Burst */
  byte chari_len,
    /* Byte length of chari data */
  byte *chari_data_ptr,
    /* Pointer to chari data */
  boolean ack_req
    /* Layer 2 ack required */
);


/* <EJECT> */
/*===========================================================================

FUNCTION SEND_TC_MO_DBM
  This function sends out a mobile originated data burst message (Position
  Determination or SMS) by calling send_tc_data_burst function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void send_tc_mo_dbm
(
  mc_mo_dbm_type mo_dbm
    /* mobile originated data burst message type to be sent over the air.*/
);



/*===========================================================================

FUNCTION TC_DH_RPT

DESCRIPTION
  This function processes a report from the Authentication task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tc_dh_rpt
(
  dh_rpt_type *dh_rpt_ptr
    /* Pointer to dh report */
);



/*===========================================================================

FUNCTION PROCESS_ENLU_MSG

DESCRIPTION
  This function processes an input Extended Neighbor List Update message.
  The Neighbor set is update accordingly and a command is sent to the
  Searcher task to use the new neighbor set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_enlu_msg
(
     caii_rx_msg_type *msg_ptr
       /* pointer to received Extended Neighbor List Update message */
);

/*===========================================================================

FUNCTION PROCESS_PWR_CNTL_STEP

DESCRIPTION
  This function processes the power control step within the Power Control
  Message or General Handoff Direction Message received from the Forward
  Traffic Channel. If MS supports the received power control step Size,
  then MS stores the received power control step and informs the MSM to
  use it as the Closed Loop Power Control Step Change. Otherwise, MS just
  ignores it.
  However, the processing mentioned above will only be implemented after
  Reverse Link MDR has been implemented because of benefits tradeoff.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void process_pwr_cntl_step
(
  byte pwr_cntl_step /* Power Control Step */
);

/*===========================================================================

FUNCTION PROCESS_PWR_CTRL_MSG

DESCRIPTION
  This function processes the forward and reverse power control parameters
  received in the power control message. The forward power control parameters are sent
  to the RXC and the reverse power control parameters are sent to the TXC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_pwr_ctrl_msg
(
  caii_rx_msg_type * msg_ptr
);

/*===========================================================================

FUNCTION VALIDATE_PILOT_REC_TYPE

DESCRIPTION
  This function validates the pilot rec types received in a TC message like
  G/UHDM, ESCAM.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if pilot_record is good, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean validate_pilot_rec_type
(
  cai_pilot_rec_type pilot_rec_type,
  caii_add_pilot_rec_type * pilot_rec
);



/*===========================================================================

FUNCTION TC_SEND_FLASH_INFO_MSG

DESCRIPTION
  This function sends the flash with information message during traffic
  channel processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_send_flash_info_msg
(
  mc_msg_type *msg_ptr
  /* pointer to message received from other other tasks */
);

/*===========================================================================

FUNCTION TC_PROC_CM_CALL_HOLD

DESCRIPTION
  This function sends the flash with information message during traffic
  channel processing.  This function only process the command in Call Hold
  context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_proc_cm_call_hold
(
  mc_msg_type *msg_ptr
  /* pointer to message received from other other tasks */
);

/*===========================================================================

FUNCTION SEND_SRV_CTL_TDSO_STAT

DESCRIPTION
  This function sends a Service Option Control message in response to a
  Retrieve TDSO Counters directive.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_srv_ctl_tdso_stat
(
  uint8 ctl_rec_type,
  uint8 vect_counter_id,
  uint8 con_ref
);

/*===========================================================================

FUNCTION SEND_SRV_CTL_TDSO_CTL_DIR

DESCRIPTION
  This function sends a Service Option Control message that proposes TDSO
  test parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_srv_ctl_tdso_ctl_dir
(
  tdso_tst_par_type fch_tst_par,
  tdso_tst_par_type sch0_tst_par
);

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION POPULATE_PIL_REC_INFO_IN_RTC_MSG

DESCRIPTION
  The pilot rec types that the MS/SRCH stores internally for each pilot is
  what the BS gave us but when we are reporting pilot rec types for the same pilots
  back to the BS in a Reverse Traffic Channel (RTC) message the BS is interested
  in only knowing if it is an aux pilot, they do not care about the TD info for
  these pilots as they "know" that already, This function dervives the info that
  needs to be sent back to the BS from the information we have
  stored i.e. in other words it maps pilot rec types referred by BS using table
  3.7.2.3.2.33-1 into table 2.7.2.3.34-1

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if additional pilot rec neds to be included for this, FALSE otherwise.

SIDE EFFECTS
  The destination pilot rec types and pilot rec pointers are populated with correct
  values.

===========================================================================*/

extern boolean populate_pil_rec_info_in_rtc_msg
(
  const cai_pilot_rec_type  *src_pilot_rec_type, /* Pointer to derive dst pilot rec from */
  const caii_add_pilot_rec_type  * src_pilot_rec, /* Pointer to ilot rec type for the pilot*/
  byte *dst_pilot_rec_type, /* Pointer to where pilot rec type has to be populated */
  caii_add_pilot_rec1_type *dst_pilot_rec /* Pointer where pilot rec has to be populated */
);

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION PROCESS_ERM

  This function process the extended release message

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/

word process_erm( caii_erm_type *erm, word cur_state );

/*===========================================================================

FUNCTION PROCESS_RAM

  This function process the Resource Allocation Message

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/

word process_ram( caii_ram_type *ram, word cur_state );

#ifdef FEATURE_IS2000_REL_A_SVD
typedef enum{
  EOM_DONE,                  /* EOM sent */
  EOM_INVALID_SRID,          /* No available SR_ID */
  EOM_INVALID_TAG,           /* No available TAG */
  EOM_VALIDATION_FAILURE,    /* Validation of CM supplied SR_ID failed */
  EOM_NO_RXTX_BUF,           /* out of RXTX buffer */
  EOM_SNM_INIT_FAILED        /* snm failed to initialize EOM in negotiation_info database */
} mcctc_eom_status_type;

/*===========================================================================

FUNCTION SEND_EOM

  This function sends an Enhanced Origination Message.

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/
mcctc_eom_status_type send_eom
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
);

/*===========================================================================

FUNCTION SEND_CLCM

  This function sends an Call Cancel Message.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void send_clcm
(
  byte tag_id,
    /* Tag id for the cancelled EOM */
  mcctctag_tag_info_type *tag_info
    /* Other Information for CLCM */
);

/*===========================================================================

FUNCTION PRE_PROCESS_CC_INFO

  This function pre-processes a CC_INFO fields received from UHDM or SCM.

DEPENDENCIES
  The CC fields passed in should be valid.

RETURN VALUE
  Reject reason code.

SIDE EFFECTS
  None

===========================================================================*/
extern byte pre_process_cc_info
(
  caii_cc_info_type *cc_info,
    /* cc_info pointer */
  tc_rej_ord_parms_type *rej_parms
    /* Rejection parameters */
);

/*===========================================================================

FUNCTION PROCESS_CC_INFO

  This function processes a CC_INFO fields received from UHDM or SCM.

DEPENDENCIES
  The CC fields passed in should be valid.

RETURN VALUE
  Reject reason code.

SIDE EFFECTS
  None

===========================================================================*/
extern void process_cc_info
(
  caii_cc_info_type *cc_info
    /* cc_info pointer */
);


/*===========================================================================

FUNCTION MCCTCSUP_CLEAN_UP_TAGS

  This function clean-up all outstanding tags. It is called before leaving
  Traffic Channel to prevent call_id leak/Stuck in calling.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Out standing tags are removed.

===========================================================================*/
void mcctcsup_clean_up_tags ( void );

#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION PROCESS_CLAM

  This function processes a Call Assignment Message.

DEPENDENCIES
  None

RETURN VALUE
  Next state.

SIDE EFFECTS
  None

===========================================================================*/
extern word process_clam
(
  caii_call_assignment_type *clam,
    /* CLAM pointer */
  word curr_state
    /* current TC state */
);
#endif

/*===========================================================================

FUNCTION MCCTCSUP_SEND_RRRM

  This function sends a Resource Release Request Message to the BS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcsup_send_rrrm
(
  boolean gating_disconnect_ind,
    /* Pilot reverse gating or service disconnect indicator */
  byte con_ref,
    /* CON_REF to release */
  boolean purge_service
    /* for PKT data call */
);


/*===========================================================================

FUNCTION SEND_PLC_RESP_ORD

DESCRIPTION
  This function sends a Long Code Transition Response Order.  The ORDQ
  value in the response is determined by the ORDQ value of the request and
  the ability of the mobile to honor the request.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we are doing what was requested, FALSE if we are refusing the
  request.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean send_plc_resp_ord
(
  byte req_ordq
    /* Value of ordq field of Long Code Transition Request Order */
);


/*===========================================================================

FUNCTION MCCTCSUP_GET_PUBLIC_LONG_CODE_MASK

  This function gets the public long code mask, based on the type of public
  long code mask which is currently in use.

DEPENDENCIES
  Assumes that cdma.long_code.public_lcm_type and cdma.long_code.bs_plcm_39
  are set correctly.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcsup_get_public_long_code_mask
(
  qword plcm
    /* return value -- public long code mask currently in use */
);

/*===========================================================================

FUNCTION MCCTCSUP_GET_LONG_CODE_MASK

  This function gets the long code mask which is currently in use.  The
  mobile could be using the private long code mask, or it could be using
  one of the public long code masks.

DEPENDENCIES
  Assumes that cdma.long_code.tc_private_lcm, cdma.long_code.public_lcm_type
  and cdma.long_code.bs_plcm_39 are set correctly.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcsup_get_long_code_mask
(
  qword plcm
    /* return value -- long code mask currently in use */
);

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION MCCTCSUP_UPDATE_OFF_TIME_INFO

  This function updates our internal variables based on Off Time information
  received from the base station.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcctcsup_update_off_time_info
(
  boolean off_time_supported,
    /* indicates whether or not CDMA Off Time Reporting is enabled */
  byte off_time_threshold_unit,
    /* unit of off_time_threshold */
  byte off_time_threshold
    /* indicates the threshold above which the mobile is to report */
);
#endif /* FEATURE_IS2000_REL_B */

/*===========================================================================
FUNCTION TC_INFORM_SCR_CHANGE_INIT_SERVICE

DESCRIPTION
  This function initializes test service options, if needed

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void tc_inform_scr_change_init_service
(
  word new_so
);

/*===========================================================================

FUNCTION TC_INFORM_RXTX_ABOUT_TX

DESCRIPTION
  This function sends a message to the RXTX (L2) task  that
  the TX is either ON or OFF

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void tc_inform_rxtx_about_tx( void );

#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION PROCESS_RATCHGM

  This function process the rate change message

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void process_ratchgm
(
  caii_rate_change_type *ratchgm
);
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

FUNCTION MCCTCSUP_CHECK_IF_TCH_SRDM_IS_OKAY

DESCRIPTION
  This function validates the service redirection message received on traffic
  channel to see if it can be supported by the MS.

DEPENDENCIES
  None

RETURN VALUE
  TRUE indicates MS supports the service redirection. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcctcsup_check_if_tch_srdm_is_okay
(
  caii_ftc_srv_redir_type *serv_redir_ptr
);

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION PROCESS_RCPM

  This function processes a Radio Configurarion Parameteres Message

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void process_rcpm
(
  caii_radio_config_type *rcpm
    /* RCPM pointer */
);

/*===========================================================================

FUNCTION PROCESS_TC_GEM

  This function processes a traffic channel General Extension Message

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/

void process_tc_gem
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
);

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TC_GEN_EXTN_RECORDS

DESCRIPTION
  This function processes the general extension records carried in the traffic channel
  general extension message.

DEPENDENCIES
  None.

RETURN VALUE
  Error code.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 mcctcsup_proc_tc_gen_extn_records
(
  caii_tc_gen_extn_type *tc_gem
    /* Pointer to General Extension Message whose records are to be processed */
);
#endif /* FEATURE_IS2000_1X_ADV */

/*===========================================================================

FUNCTION MCCTCSUP_INITIALIZE_CP

DESCRIPTION
  This function calls the functions necessary to initialize Call Processing
  for Traffic Channel Initialization state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcctcsup_initialize_cp (void);

/*===========================================================================

FUNCTION MCCTCSUP_INIT_SEND_SRCH_TC

DESCRIPTION
  This function tells Receive Task to tune to the Traffic Channel and begin
  sending frame quality metrics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcctcsup_init_send_srch_tc
(
  mccdma_chnasn_type *chn_assign,
  /* Details about the channel assignment */
  qword long_code_mask

#if defined(FEATURE_IS2000_REL_C)
  , boolean restore_ind
  /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
  /* Service configuration to be restored */
#endif /* FEATURE_IS2000_REL_C */
);

/*===========================================================================

FUNCTION MCCTCSUP_INIT_SEND_RXC_TCI

DESCRIPTION
  This function sends TCI command to Receive Task to have it tune to the
  Traffic Channel and begin sending frame quality metrics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcctcsup_init_send_rxc_tci
(
  mccdma_chnasn_type *chn_assign
  /* Details about the channel assignment */
#if defined(FEATURE_IS2000_REL_C)
  , boolean restore_ind
  /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
  /* Service configuration to be restored */
#endif /* FEATURE_IS2000_REL_C */
);

/*===========================================================================

FUNCTION MCCTCSUP_INIT_SEND_TXC_TC

DESCRIPTION
  This function sends the TC command to Transmit Task to have it go to
  Traffic channel which will cause it to start sending Traffic Channel
  preambles.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcctcsup_init_send_txc_tc
(
  mccdma_chnasn_type *chn_assign,
  /* Details about the channel assignment */
  qword long_code_mask
#if defined(FEATURE_IS2000_REL_C)
  , boolean restore_ind
  /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
  /* Service configuration to be restored */
#endif /* FEATURE_IS2000_REL_C */
);

/*===========================================================================

FUNCTION MCCTCSUP_INIT_INSTANTIATE_CC

DESCRIPTION
  This function instantiates call control instances.

DEPENDENCIES
  NEGOTIATION_INFO must be initialized (srv_init does it).


RETURN VALUE
  TRUE if call control instances are created successfully.
  Otherwise, return FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mcctcsup_init_instantiate_cc
(
  mccdma_chnasn_type *chn_assign,
  /* Details about the channel assignment */
  cm_call_id_type *call_id_ptr
  /* Pointer to allocated Call IDs */
#if defined(FEATURE_IS2000_REL_C)
  , uint8 *num_call_id_ptr  /*lint -esym(715, num_call_id_ptr) */
  /* Pointer to the number of allocated Call IDs */
  , boolean restore_ind
  /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
  /* Service configuration to be restored */
#endif /* FEATURE_IS2000_REL_C */
);

/*===========================================================================

FUNCTION MCCTCSUP_INIT_CDMA_PARM_FROM_OVHD

DESCRIPTION
  This function initializes CDMA parameters used in traffic channel
  processing from overheads.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcctcsup_init_cdma_parm_from_ovhd
(
  void
);

/*===========================================================================

FUNCTION MCCTCSUP_CP_EXIT_PROCESSING

DESCRIPTION
  This function performs the exit processing for CP.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcctcsup_cp_exit_processing (void);


#ifdef FEATURE_1X_TO_LTE
/*===========================================================================

FUNCTION MCCIDL_PROC_ALT_TECH_INFO_MSG

DESCRIPTION
  This procedure processes a received Alternative Technologies Information
  Message. This will send commands to Layer1.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_alt_tech_info_msg
(
  word new_state
    /* Current state being processed */
);

/*===========================================================================

FUNCTION PROCESS_TC_ATIM

  This function processes a traffic channel Alternative Technology
  Information Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_tc_atim
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
);

#endif /* FEATURE_1X_TO_LTE */

#endif /* MCCTCSUP_H */

