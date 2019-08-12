#ifndef MCCTCHO_I_H
#define MCCTCHO_I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C D M A   T R A F F I C   C H A N N E L   S T A T E
         H A N D O F F   R O U T I N E S   H E A D E R   F I L E

GENERAL DESCRIPTION
  This module externalizes the handoff support routines for the CDMA
  Sub-system, Mobile Station Control on the Traffic Channel state.

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctcho.h_v   1.5   02 Oct 2002 16:02:12   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mcctcho_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/16   ppr     1x2GTA Feature changes
09/15/12   ppr     FR#2370 - 1x memory optimization changes
05/09/12   ppr     Feature Cleanup
04/06/12   ssh     Skipped RCP transactions during non 1xA RCs to non 1xA RCs
02/15/12   srk     Feature Cleanup: Removed FEATURE_GPS_MODE
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
11/04/10   ssh     Added processing of SMCM with use_time as TRUE.
01/15/07   sb      VOIP-1X handoff updates.
10/12/06   fc      Fixed the issue of sending bogus CFSRM before the
                   completion all NSET searches.
09/30/06   fc      Fixed featurization.
07/07/05   va      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
11/05/04   va      Merge the following from REL_B_C archive:
10/20/04   an        Replaced inclusion of srch.h with srchmc.h
11/05/04   sb      Added support for Rel C DV handoffs.
10/01/04   sb      Fixed lint warning.
08/06/04   bkm     Added function mcctcho_is_hho_in_progress().
08/02/04   va      Removed mcctcho_handle_cfs_at_arrival fn prototype
06/30/04   bkm     Added the explicit void argument for below prototypes.
06/30/04   bkm     Added prototype for mcctcho_handle_ho_action_time_arrival()
                   and mcctcho_log_handoff_event().
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   sb      Release C addendum changes for PLCM.
  06/08/04   bkm     Removed declaration for mcctcho_process_txc_frame_sig().
06/28/04   va      Added new fn prototypes
04/19/04   sb      Added function mcctcho_update_long_code_mask_info.
03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
05/20/03   fc      Added function prototype save_serving_config_parms.
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   az      Removed the feature TMOBILE
10/02/02   az      Mainlined FEATURE_IS2000
05/16/01   fc      Merged the following from common archive:
                   04/10/01   jq      Modified UHDM processing to support P2 and
                                      Control Hold.
05/03/01   va      Added new_p_rev_in_use in mcctcho_data struct.
04/18/01   va      Added pilot_rec_types to mcctcho_pilot_rec_type
02/26/01   mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE to move code needed
                   only for GPS part of position location to a new #define.
08/23/00   lcc     Changed mcctcho_generate_aset to return sup_state and removed
                   transaction_num from function parameter.
07/28/00   fc      Mainline FEATURE_CS_ACTION_TIME_TRANS.
07/17/00   va      Support for 8th rate gating and rev pwr cntl delay.
06/19/00   yll     Added GPS support: Provide a function to check if a handoff
                   or single shot CFS is in progress.
06/12/00   lcc     Minor changes to mcctcho_ho_data_type as part of unifying
                   transaction processing.
05/25/00   ks      Added support for Neighbor Search Window offset.
04/13/00   lcc     Added definition for MCCTCHO_IMPLICIT_ACTION_TIME.
04/11/00   cah     Featurize CFS items in the mcctcho_pending_msg_type under
                   the FEATURE_IS95B_MAHHO define.
03/24/00   ry      Added IS2000 featurization
           ry      Merged the following from MSM3100_CP.02.00.10:
           dna     CR12461 Changed tf_aset to cf_srch_results, since it stores
                   results from CF search as well as failed handoffs. Increased
                   size from 6 to 46 to allow for CF search set to fit.
           vn      Merged in the following from trimode build TM1005:
           ck      Featurised different_band_ho under Trimode.
           ck      Added band class to mcctcho_serv_freq_config_type.
                   Added the flag different_band_ho to mcctcho_ho_data_type.
03/06/00   lcc     Included ho_msg in mcctcho_data_type and removed unnecessary
                   members.
12/20/99   va      Support for restoring fpc_subchan_gain correctly.
11/15/99   va      IS2000 Changes:
                   mcctcho_ho_data has been extended so that all parameters needed for
                   post processing HO is stored there.
10/28/99   ry      Updated mcctcho_cand_freq_srch_off_time_type such that all
                   off times are defined as byte, instead of word
10/12/99   ry      Added srv_cfg to mcctcho_serv_freq_config_type
           ry      Added service_included and restart_periodic_srch to
                   mcctcho_ho_data_type
10/11/99   ry      Removed aset from mcctcho_serv_freq_config_type
10/05/99   ry      Removed mcctcho_action_time_pending()
08/19/99   ry      Added mcctcho_cand_freq_srch_off_time_type
           ry      Added last_srch_msg_seq to mcctcho_cand_freq_srch_parm_type
           ry      Renamed cfs_msg_seq to cur_srch_seq_num
08/13/99   doj     Added pwr_cntl_step to mcctcho_ho_data_type.
08/09/99   ry      Updated mcctcho_cand_freq_srch_parm_type to support both
                   single and periodic Candidate Frequency Search
           ry      Added tf_band_class to mcctcho_ho_data_type
           ry      Added extern definitions for mcctcho_start_cf_meas() and
                   mcctcho_process_cand_freq_srch_ctrl_msg()
08/08/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
           ry      Changed FEATURE_IS95B_CFS to FEATURE_IS95B_MAHHO
           ry      Reuse the structure mcc_ho_cb_type inside
                   mcctcho_serv_freq_config_type, instead of defining each
                   of the handoff parameters separately
           ry      Moved mcctcho_hard_handoff_in_progress,
                   mcctcho_hard_handoff_successful, and
                   mcctcho_cf_srch_completed into the structure
                   mcctcho_ho_data_type
06/09/99   ry      Added new data structures to support IS95B hard handoff and
                   candidate frequency search
           ry      Added extern definitions for the following functions:
                     mcctcho_action_time_pending()
                     mcctcho_process_failed_hho()
                     mcctcho_return_to_serv_freq()
                     mcctcho_post_process_handoff()
                     mcctcho_process_txc_frame_sig()
                     mcctcho_process_srch_cf_nset_rpt()
                     mcctcho_process_cand_freq_srch_req_msg()
05/18/99   doj     Created module from pieces of MCCTCSUP.H to consolidate
                   handoff processing.  Look in MCCTCSUP.H (v1.2) for history
                   prior to this date.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "target.h"
#include "customer.h"
#include "mccccl.h"
#include "mccdma.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"
#include "srchmc.h"
#include "mcctcsup.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define MCCTCHO_IMPLICIT_ACTION_TIME  0xff

/* The no of 80 ms frames to consider to determine if an action time rollover
 * occured due to delayed reception of an OTA */
#define TA_ACTION_TIME_ROLLOVER_BUFFER 10  

typedef enum
{
  REJECT_HANDOFF_MESSAGE, /* We should reject this HO message. */
  OKAY_TO_HANDOFF,        /* It is okay to handoff right away. */
  WAIT_TO_HANDOFF         /* We need to wait for proper time to handoff. */
} handoff_action_type;

typedef struct
{
  word    pilot_pn;          /* Pilot PN sequence offset index */
  boolean pwr_comb_ind;      /* Power control symbol combining indicator */
  byte    code_chan;         /* Fundamental Code channel index */
  word    ecio;              /* Pilot strength - used to report the TF Active
                                Set in the case of failed HHO */
#ifdef FEATURE_IS2000_REL_A
  cai_pilot_rec_type pilot_rec_type; /* Pilot record type */
  caii_add_pilot_rec_type pilot_rec; /* Additional pilot record if it is not
                                     a common pilot */
#endif
} mcctcho_pilot_type;

typedef struct
{
  word num_pilots;              /* Number of PNs in the Active Set */
  mcctcho_pilot_type pilot[SRCH_NSET_MAX + SRCH_ASET_MAX];
                                /* Store search results of from handoff active
                                 * set and/or candidate frequency search set */
} mcctcho_cf_srch_result_type;


typedef enum
{
  MCCTCHO_HANDOFF_TIMER_EXPIRE,
  MCCTCHO_DIFF_RX_PWR_TOO_LOW,
  MCCTCHO_TOTAL_PILOT_EC_IO_TOO_LOW
} mcctcho_hho_failure_reason_type;


typedef struct
{

  boolean private_lcm;


  cai_plcm_type public_lcm_type;
  qword bs_plcm_39;

} mcctcho_serv_freq_config_type;


/* The various off times that are associated with a CFS */
typedef struct
{
  byte tot_off_time_fwd;
                /* Total time that the MS is off the Forward Traffic Channel */
  byte max_off_time_fwd;
              /* Maximum time that the MS is off the Forward Traffic Channel */
  byte tot_off_time_rev;
                /* Total time that the MS is off the Reverse Traffic Channel */
  byte max_off_time_rev;
              /* Maximum time that the MS is off the Reverse Traffic Channel */
} mcctcho_cand_freq_srch_off_time_type;

/* Parameters that are received from the CFSReqM or CFSCtrlM */
typedef struct
{
  byte last_srch_msg_seq;                     /* CFSRM_SEQ/CFSCM_SEQ/HDM_SEQ */
  byte cf_band_class;                      /* Candidate Frequency Band Class */
  word cf_cdma_freq;                       /* Candidate Frequency assignment */
  byte tf_wait_time;             /* CDMA Candidate Frequence Total Wait Time */
  word num_pilots;                  /* Number of pilots in the CF Search Set */
  byte cur_srch_type;                                 /* Current Search Type */
  byte cur_srch_seq_num;                   /* Current Search Sequence Number */
  byte srch_period;            /* Time between successive searches on the CF */
  mcctcho_cand_freq_srch_off_time_type off_times;               /* Off times */
  boolean cf_srch_offset_incl;   /* CF neighbor pilot search window offset
                                    included indicator. */
  byte align_timing;     /* Align timing for CF Search */
  byte mod0_sf_total_ec_thresh;   /* Serving Frequency total pilot Ec threshold */
  byte mod0_sf_total_ec_io_thresh;/* Serving Frequency total pilot Ec/Io thresh */
} mcctcho_cand_freq_srch_parm_type;

typedef struct
{
  boolean explicit_at;    //IS2000 changes
  qword abs_action_time;  //IS2000 changes
  byte trans_num;         //IS2000 changes
  byte frame_offset;

  boolean disjoint_active_sets_ho;
  boolean different_frame_offset_ho;
  boolean different_freq_ho;
  boolean different_band_ho;
  boolean hard_handoff_in_progress;
    /* Indicator of whether a Hard Handoff is in progress */

  boolean cfs_in_progress;
    /* Indicator of whether a CFS is in progress which GPS visit
     * should not be scheduled.
     * A single shot candidate frequency search should not be interrupted
     * by a GPS frequency visit. This flag is set true when receiving a
     * CFSCtlM or CFSReqM, and is reset when CFS is done.
     * For a periodic candidate frequency search, as long as we haven't
     * started tuning away procedure yet, it can be preempted by a GPS
     * frequency visit. However, if RF is about to tuned away to
     * candidate frequency, that should not be interrupted just as single
     * shot CFS. Therefore, this flag is also set whenever MC decides to
     * tune the RF to a candidate frequency.
     */

  boolean pending_cfs_meas_rpt_to_be_sent;
    /* Indicator of whether CFS measurement report shall be sent once MS back
       to serving frequency. */

  boolean pending_scr_nnscr_changed;

  byte tf_band_class;
  word tf_cdma_freq;
  word tf_tot_rx_pwr;
  mcctcho_cf_srch_result_type cf_srch_results;
  boolean cf_srch_completed;
  boolean hard_handoff_successful;
    /* Indicator of whether a Hard Handoff is successful */
  boolean restart_periodic_srch;
  boolean fpc_subchan_gain_incl;      // FPC subchan gain included indicator
  boolean process_subchan_gain_at_ho; // Indicates if subchan gain is to be processed at action time of handoff
  qword subchan_gain_action_time;     // Absolute action time of subchan_gain if used
  boolean update_dec_sup;             // Do we need to send sups info to RXC
  boolean num_sups;                   // Num of SCCH to be informed to RXC
  boolean sups_needed;        /* Indicates if SCCH info is needed for ASET generation*/
  boolean high_rc_before_ho;         // This indicates if were in a high RC
                                       // before HO
  boolean high_rc_after_ho;         // This indicates if we'll be in a
  /* higher RC after the HO, needed to  populate action packets at AT */

  boolean new_fpc_pri_chan;           // Indicates if fpc_pri_chan changes after HO

#ifdef FEATURE_IS2000_REL_A
  byte new_p_rev_in_use; // p_rev_in_use that will be used after HO, Needed by Mux from RelA
#endif
  caii_ho_msg_type  ho_msg;       // It's much easier to put the whole message here than
  // to put piece of it, both for understanding and maintenance.

  /* List of all actions sent to lower layers for a RIF-HHO,
  All of this + more may have to be sent to lower layers with a RETURN
  txn, if the RIF-HHO fails */

  struct
  {
    mctrans_action_type actions[MCTRANS_MAX_ACTION_IN_TRANS];
  } server[ MCTRANS_SERVER_COUNT];

} mcctcho_ho_data_type;

/* Pointer hods Post hand-off data */
extern mcctcho_ho_data_type *mcctcho_ho_data_ptr;

/* The following struct definition is to avoid using mccrxtx_msg_type to
    store pending messages, as that struct takes too much memory */
typedef struct
{
  union
  {
    caii_gen_type gen;
    caii_cfs_req_type cfs_req;
    caii_cfs_ctrl_type cfs_ctrl;
  }msg;
} mcctcho_pending_msg_type;

/* Boolean to capture if its a handoff from legacy RCs to Legacy RCs. */
extern boolean mcc_is_ho_internal_to_non_1xa_rc;

/*===========================================================================

FUNCTION MCCTCHO_TC_EXIT

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcho_tc_exit( void );

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_GENERIC_HANDOFF_MSG

DESCRIPTION
  This function processes a Generic Handoff Direction Message
  (HDM, EHDM, GHDM, ...).

DEPENDENCIES
  See dependencies of called functions.

RETURN VALUE
  None.

SIDE EFFECTS
  It may cause the mobile to do a soft or hard handoff.  It may update the
  search parameters.  It may update the code channel list (both fundamental
  and supplemental channels) used by the searcher.  It may modify the forward
  supplemental channel duration timer.  It may change the CDMA frequency.  It
  may queue a reject message.  It may reset layer 2 and the forward power
  control parameters.  It may change a whole host of base station related
  parameters in the event of a hard handoff.

===========================================================================*/
extern void mcctcho_process_generic_handoff_msg
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
);

/*===========================================================================

FUNCTION MCCTCHO_POST_PROCESS_HANDOFF

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcho_post_process_handoff( void );

/*===========================================================================

FUNCTION SAVE_SERVING_CONFIG_PARMS

DESCRIPTION
  This function saves the current configuration data, which will be restored
  in the case of a failed Return-If-Fail HHO

DEPENDENCIES
  This function is called for a Return-If-Fail HHO at the beginning of the
  processing of a GHDM.  This function is also called before a vist to the
  Candidate Frequency; however, note that it is not necessary to call its
  counterpart restore_serving_config_parms() after the CFS visit

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void save_serving_config_parms(void);

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_TX_STOP

DESCRIPTION
  This function informs SRCH and RXC that HHO is about to begin
  and tunes RF and informs RXC and SRCH again.


DEPENDENCIES
  This function is only called on a HHO.

RETURN VALUE
  None.

SIDE EFFECTS
  See description

===========================================================================*/
extern void mcctcho_process_tx_stop(void);

/*===========================================================================

FUNCTION DO_SUCCESSFUL_HANDOFF_OPERATIONS

DESCRIPTION
  This function performs all the operations associated with a successful
  handoff

DEPENDENCIES
  For a soft handoff, this function is always called.  For a HHO, this
  function is called only if the handoff is successful

RETURN VALUE
  None

SIDE EFFECTS
  This function may update the service negotiation type, packet zone ID,
  p_rev_in_use, and power control step.  It will also send a handoff
  completion message

===========================================================================*/
extern void do_successful_handoff_operations( void );

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_FAILED_HHO

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcho_process_failed_hho
(
  mcctcho_hho_failure_reason_type hho_failure_reason
);

/*===========================================================================

FUNCTION MCCTCHO_RETURN_TO_SERV_FREQ

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcho_return_to_serv_freq
(
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to the failure report received from SRCH */
);

/*===========================================================================

FUNCTION MCCTCHO_START_CF_MEAS

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcho_start_cf_meas
(
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to the failure report received from SRCH */
);

/*===========================================================================

FUNCTION MCCTCHO_EXECUTE_CFS_MEAS

DESCRIPTION
  This API will execute procedures/txns for CFS meansurements.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
 void mcctcho_execute_cfs_meas(void);

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_CAND_FREQ_SRCH_REQ_MSG

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void mcctcho_process_cand_freq_srch_req_msg
(
  caii_cfs_req_type *cfs_req
    /* Pointer to message received from the base station */
);

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_CAND_FREQ_SRCH_CTRL_MSG

DESCRIPTION
  This function processes a Candidate Frequency Search Control Message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void mcctcho_process_cand_freq_srch_ctrl_msg
(
  caii_cfs_ctrl_type *cfs_ctrl
    /* Pointer to message received from the base station */
);

/*===========================================================================

FUNCTION MCCTCHO_UPDATE_CFS_PARAMS

DESCRIPTION
  This function updates CFS parameters locally stored in MC based on the
  CFS Req or the CFS Ctrl message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void mcctcho_update_cfs_params
(
  const mctrans_pending_action_msg_type *cfs_msg_ptr
);

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_SRCH_CF_NSET_RPT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcho_process_srch_cf_nset_rpt
(
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to the CF_NSET_R report received from SRCH */
);

/*===========================================================================

FUNCTION MCCTCHO_GET_HO_DATA

DESCRIPTION
  This function returns a pointer to the mcctcho_ho_data data structure.

DEPENDENCIES
  None.

RETURN VALUE
  Constant pointer to the mcctcho_ho_data.

SIDE EFFECTS
  None.

===========================================================================*/
mcctcho_ho_data_type *mcctcho_get_ho_data
(
  void
);

/*===========================================================================

FUNCTION mcctcho_generate_aset

DESCRIPTION
  This function updates code channel list table and returns aset in
  the parameters passed as references

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Code Channel list table in mccccl.c could get modified.

===========================================================================*/
mccccl_sup_state_type mcctcho_generate_aset
(
  word * aset_count,
  srch_tc_aset_type * aset
    /* Pointer to active set parameters */
);

/*===========================================================================

FUNCTION MCCTCHO_DIFFERENT_LONG_CODE_MASK

DESCRIPTION
  This function determines if the long code mask specified in the handoff
  message is different from the one we are currently using.  Note that UHDM,
  GHDM and EHDM can switch between the private and public long code masks.
  UHDM can switch between public long code mask types.

DEPENDENCIES
  None.

RETURN VALUE
  Yes if handoff message will change long code mask
  No if handoff message will not change long code mask

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mcctcho_different_long_code_mask
(
 caii_ho_msg_type * ho_msg
);

/*===========================================================================

FUNCTION MCCTCHO_UPDATE_LONG_CODE_MASK_INFO

DESCRIPTION
  This function updates our internal long code mask information based
  on a handoff direction message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcctcho_update_long_code_mask_info
(
 caii_ho_msg_type * ho_msg
);

/*===========================================================================

FUNCTION MCCTCHO_SAVE_ACTIONS_SENT_TO_LOWER_LAYERS

DESCRIPTION
  This saves all actions sent to lower layers so that they can be resent with
  "old" parameters if we return upon a HHO failure

DEPENDENCIES
  This function is called for a Return-If-Fail HHO  at the HO
  txn pre-processor

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcho_save_actions_sent_to_lower_layers
(
   mctrans_trans_info_type * ho_trans
);

/*===========================================================================

FUNCTION mcctcho_update_ccl_with_new_aset

DESCRIPTION
  This function updates code channel list table , this is called by the txn
  pre-processor in the server context

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Code Channel list table in mccccl.c gets modified.

===========================================================================*/
extern void mcctcho_update_ccl_with_new_aset( void );

/*===========================================================================

FUNCTION MCCTCHO_HANDLE_HO_ACTION_TIME_ARRIVAL

DESCRIPTION
  This fn does processing related to HO action time arrival. For SHO this
  pretty much completes the HO processing, For HHO just a little bit of
  book keeping is done as the HO itself is not quite complete yet

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  See description

===========================================================================*/
extern void mcctcho_handle_ho_action_time_arrival(void);

/*===========================================================================

FUNCTION MCCTCHO_LOG_HANDOFF_EVENT

DESCRIPTION
  This function performs the event reporting of soft and hard handoff.

DEPENDENCIES
  This function is only called on a HHO and after CCL is updated.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcho_log_handoff_event(void);

/*===========================================================================

FUNCTION MCCTCHO_IS_HHO_IN_PROGRESS

DESCRIPTION
  This function determines if a HHO is in progress.

DEPENDENCIES
  None

RETURN VALUE
  True, if HHO is in progress.  False otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mcctcho_is_hho_in_progress( void );

/*===========================================================================

FUNCTION VALIDATE_HANDOFF_MSG

DESCRIPTION
  This function validates the (E|G|U)HDM by checking for reject reasons
  defined by IS95 (6.6.6.2.5.1)

DEPENDENCIES
  None

RETURN VALUE
  0 if message is OK; CAI reject code otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 validate_handoff_msg
(
  caii_rx_msg_type *msg_ptr,
  /* Pointer to message received from the base station */
  tc_rej_ord_parms_type *rej_parms
  /* Rejection parameters */
);

/*===========================================================================

FUNCTION MCCTCHO_DEFAULT_HANDOFF_MSG_FIELDS

DESCRIPTION
  This function defaults fields in the Handoff Direction Messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The Handoff Direction Message passed in will have fields defaulted.

===========================================================================*/
extern void mcctcho_default_handoff_msg_fields
(
  caii_ho_msg_type *ho
);

/*===========================================================================

FUNCTION MCCTCHO_TC_INIT

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcctcho_tc_init( void );

/*===========================================================================

FUNCTION MCCTCHO_CREATE_ECAM_FROM_CURR_CFG

DESCRIPTION
  This function creates an ECAM from the current SNM configuration.
  It only sets the fields required to be sent to TXC in TC_F.

DEPENDENCIES
  A valid SNM configuration should already exist.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcho_create_ecam_from_curr_cfg
(
  mccdma_chnasn_type *chn_assign
);


#endif /* MCCTCHO_I_H */

