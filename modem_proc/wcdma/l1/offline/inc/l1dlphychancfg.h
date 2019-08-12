#ifndef L1_DL_PHYCHAN_CFG_H
#define L1_DL_PHYCHAN_CFG_H
/*============================================================================
                     L 1 _ D L _ P H Y C H A N _ C F G _ H
DESCRIPTION

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1dlphychancfg.h_v   1.19   19 May 2002 03:21:12   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1dlphychancfg.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
05/28/14    bs      Removed validation check for num SCCH codes for DC-HSDPA
01/30/12    vs      Feature cleanup.
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
05/24/10    ks      Corrected fdpch related featurization
04/06/10    ks      Added Enhanced FDPCH support
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/27/08    hk      Lint Fixes
10/03/08    hk      Bringing in 7k Mailine fixes
12/12/07    vsr     Removed enhanced DL featurization for pri_cpich_scr_code
                    in l1_sccpch_parms_struct_type
10/24/07    vsr     Mainlining MBMS and Enhanced DL changes 
08/06/07    vsr     Moved cm related structures from dldem.h
08/08/07    vp      Added support for reading cell id of gps bch in dch state.
12/22/06    ksr     Rel-6 post-verification, skip sync A changes
10/17/06    ms      Added support for FDPCH.
20/03/06    vip     Lint Fixes
09/02/06    vip     Removal of FEATURE_SSDT_SUPPORTED code
01/06/06    scm     Change SIB sleep feature name to FEATURE_INTER_SIB_SLEEP.
09/25/05    kps     Preliminary SIB sleep optimization support.
                    Code changes are inside #ifdef FEATURE_SIB_SLEEP_OPTIMIZATION
04/29/05    scm     Add ctch_fach_tti to wl1_cbs_drx_level_1_sched_struct.
02/24/05    scm     Small mod to wl1_cbs_drx_level_1_sched_struct.
02/09/05    scm     Add CBS (Cell Broadcast Services) data on SCCPCH
                    scheduling information to l1_sccpch_parms_struct_type.
08/30/04    gs      Added interface definitions for HSDPA
08/31/04    src     Fixed a merge/featurisation issue with CLTD merges.
08/26/04    src     Merged in the latest CLTD changes with featurisation under
                    the feature flag FEATURE_TRANSMIT_DIVERSITY_SUPPORT.
06/25/04    scm     Add pccpch_s_for_cell_id to l1_pccpch_parms_struct_type.
03/31/03    m       Added po3 field to dl dpch common info struct
05/08/02    sh      Changed primary scrambling code to scrambling code
                    in l1_pccpch_parms_struct_type.
01/02/02    gs      1. Removed the enum type definifion for CM scrambling
                       code change type.
                    2. Changed the scr_code_change in DL DPCH RL info to
                       boolean from enum type.
                    3. Changed the DL common info CM info from structure to
                       l1_cm_info_struct_type structure pointer.
                    4. Moved the L1_MAX_RL macro definition to l1sapcommon.h
12/11/01    asr     Changed l1_pich_parms_struct_type to use uint64 (instead of
                    uint32) for the two "imsi" parameters (IMSIs can be 15 digits long)
12/06/01    src     Added feature FEATURE_REL_99_JUNE_2001 and/or comments
                    around the R'99 2001-06 updates from 12/05/01.
12/05/01    src     Updated the l1_pich_parms_struct_type with an additional
                    member that accounts for the IMSI-divided-by-8192 value
                    needed for paging occasion computation per R'99 2001-06.
10/15/2001  gs      defined the timer T312 and T313 for sync detection.
09/24/01    asr     Removed sec_scr_code from l1_pich_parms_struct_type (taken
                    out in the December 2000 (v3.5.0) spec)
09/04/2001  sh      updated l1_pich_parms_struct_type for DRX.
08/24/2001  mdl     removed scrambilng_code field from aich parameters
06/07/2001  mdl     changed MAX_CFN_TARGET_SFN_FRAME_OFFSET from 25 to 255

05/11/2001  mdl     removed pri_scr_code field from
                    l1_dl_dpch_info_per_rl_struct_type.  pri_cpich_scr_code
                    is used instead in l1_dl_info_per_rl_struct_type

04/27/2001  mdl     added bitfields for optional parameter inclusion indication
                    in various RRC Ordered config database structures.
                    Changed common_info to dpch_common in
                    l1_dl_common_info_struct_type

03/14/2001  mdl     added primary scrambling code to PCCPCH structure and
                    to the DPCH per radio link structure.
                    Moved the AICH parameters to this file under the feature
                    FEATURE_AICH_IN_DL_CFG.  This feature has not yet been
                    enabled.

01/04/2001  mdl     added #defines for number of paging indicators per
                    frame, i.e. PICH_PI_PER_FRAME{18, 36, 72, 144}

12/12/2000  mdl     substantially modified DPCH structures in
                    l1_dl_phychan_db_struct_type

11/27/2000  mdl     added use_pich to l1_dl_phychan_db_struct_type to
                    indicate whether a PICH should be setup when a SCCPCH
                    is being setup.

11/02/2000  mdl     initial cut
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "l1sapcommon.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                  STRUCTURE: SCCPCH_PARMS_STRUCT_TYPE

This contains the parameters for the S-CCPCH and corresponds to IE
10.3.6.61.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define SCCPCH_INFO_SEC_CPICH_INFO_PRESENT          0x01
#define SCCPCH_INFO_SEC_CPICH_SEC_SCR_CODE_PRESENT  0x02
#define SCCPCH_INFO_SEC_SCR_CODE_PRESENT            0x04


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     STRUCTURE:  PICH_PARMS_STRUCT_TYPE

This structure contains the information on the PICH and corresponds to
IE 10.3.6.41.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define PICH_PI_PER_FRAME_18 18
#define PICH_PI_PER_FRAME_36 36
#define PICH_PI_PER_FRAME_72 72
#define PICH_PI_PER_FRAME_144 144





#define L1_DL_AICH_TT_0_VAL    0
#define L1_DL_AICH_TT_1_VAL    1

/* table of DL AICH Transmission Timing values */
extern uint8 l1_dl_aich_tt_val_tab[L1_DL_NUM_AICH_TT];

/* return AICH TT value given its enumeration */
#define L1_DL_AICH_TT_VAL(aich_tt_enum)  l1_aich_tt_val_tab[aich_tt_enum]

/* AICH scrambling code types */
typedef enum {
  /* primary scrambling code */
  L1_DL_PRIMARY_SC,
  /* secondary scrambling code */
  L1_DL_SECONDARY_SC
} l1_dl_aich_sc_enum_type;

/* AICH STTD indicator */
typedef enum {
  L1_DL_STTD_PRESENT,
  L1_DL_STTD_ABSENT
} l1_dl_aich_sttd_ind_enum_type;





/*--------------------------------------------------------------------------
                 STRUCTURE: L1_DL_DPCH_INFO_PER_RL_STRUCT_TYPE

This structure contains the parameters relating to the Downlink DPCH info
for each Radio Link and corresponds to IE 10.3.6.17.

--------------------------------------------------------------------------*/
#define DL_DPCH_INFO_PER_RL_SSDT_PRESENT                0x0001
#define DL_DPCH_INFO_PER_CLOSED_LOOP_TIMING_ADJ_PRESENT 0x0002
#define DL_DPCH_INFO_PER_RL_SEC_CPICH_INFO_PRESENT      0x0004
#define DL_DPCH_INFO_PER_RL_FRAME_OFFSET_PRESENT        0x0008
#define DL_DPCH_INFO_PER_RL_SEC_CPICH_SEC_SCR_CODE_PRESENT 0x00010

#define MAX_DPCH_FRAME_OFFSET 149

/* Compressed mode pattern sequence state.
   Some commentable illustrations */
/* ------|-------      ----|----------------|-----        ----|------
       TGCFN    ^
               TGSN
                 <---->                           <------>
                  TGL1                            TGL2
                 <-------------------------------->
                        TGD
    ^L1_CM_TGPS_WAIT_TO_START
          ^L1_CM_TGPS_IN_P1_GAP1
                            ^L1_CM_TGPS_BEFORE_P1_GAP2
                                             ^L1_CM_TGPS_IN_P1_GAP2
                                      L1_CM_TGPS_BEFORE_P2_GAP1^
   ------------------------------------------------------------------ */                                      
typedef enum
{
  L1_CM_TGPS_NOT_ACTIVE,
  L1_CM_TGPS_WAIT_TO_START,
  L1_CM_TGPS_IN_P1_GAP1,
  L1_CM_TGPS_BEFORE_P1_GAP2,
  L1_CM_TGPS_IN_P1_GAP2,
  L1_CM_TGPS_BEFORE_P2_GAP1,
  L1_CM_TGPS_IN_P2_GAP1,
  L1_CM_TGPS_BEFORE_P2_GAP2,
  L1_CM_TGPS_IN_P2_GAP2,
  L1_CM_TGPS_BEFORE_P1_GAP1,
  L1_CM_TGPS_COLLIDED
} l1_cm_tgps_state_enum_type;

/* D/L compressed mode scrambling code change type */
typedef enum
{
  L1_CM_DL_SCR_CODE_ALT_LEFT,  /* if NM OVSF code N <  SF/2 */
  L1_CM_DL_SCR_CODE_ALT_RIGHT, /* if NM OVSF code N >= SF/2 */
  L1_CM_DL_SCR_CODE_NO_CHANGE
} l1_cm_dl_scr_code_change_type_enum_type;

/* TGPS going inactive reason types */
typedef enum
{
  L1_CM_TGPS_ALREADY_DEACT,
  L1_CM_TGPS_DEACT_AT_TGPRC_OVER,
  L1_CM_TGPS_EXPICIT_DEACT,
  L1_CM_TGPS_DEACT_AT_RTE_TGPS_COLL
} l1_cm_tgps_deact_reason_enum_type;


/* compressed mode internal state structure for each compressed mode sequence */
typedef struct
{
  /* CFN for which this state is valid */
  uint8 cfn;

  /* transmission gap pattern sequence state */
  l1_cm_tgps_state_enum_type cm_tgps_state;

  /* collided TGP seq id, only valid if cm_tgps_state is L1_CM_TGPS_COLLIDED */
  uint8 coll_tgpsi;

  /* transmission gap pattern repetition count */
  uint16 tgp_rep_count;
  /* transmission gap pattern sequence position in frames.
     The starting frame is the sequence is the 0th frame */
  uint8 tgps_pos;

  /* compressed slot in a frame info */
  boolean frame_compressed;
  boolean multiframe_gap;
  uint8 num_txed_slots;
  uint8 first_cm_slot;

  /* indicate that gap start in this frame */
  boolean gap_starts_here;

  /* Transmission gap start/end position. Note that these position
     will be same for both patterns */
  uint8 tg1_end_fr_pos;     /* gap 1 ending frame pos */
  uint8 tg2_start_fr_pos;   /* gap 2 starting frame pos */
  uint8 tg2_start_slot_num; /* gap 2 starting slot */
  uint8 tg2_end_fr_pos;     /* gap 2 ending frame pos */

  /* number of frames left in TGPS to finish TGPRC.
     Valid only if TGPRC is not infinite */
  uint32 tgps_fr_len_left;
} l1_cm_tgps_stat_struct_type;

/* CM action information */
typedef struct
{
  /* indicate if this info is valid or not */
  boolean valid;

  /* next frame CFN */
  uint8 next_frame_cfn;

  /* indicate that next frame is a compressed frame and will be using the
     compressed mode pattern sequence info from sequence given by Id
     l1_cm_tgps_id_active_in_next_frame */
  boolean next_frame_occupied;
  
  /* transmission gap pattern sequence Id active in next frame. Valid only
     when l1_cm_next_frame_occupied is TRUE */
  uint8 tgpsi_active_in_next_frame;
  /* indicate that next frame should be configured for compressed mode */
  boolean cm_prep_reqd;
  /* this indicate that some of the compressed mode pattern sequence are out of
     compressed mode and if there is no other pattern sequence that ask for the
     next frame to be compressed frame then normal mode parameters must be restored */
  boolean cm_exit_reqd;
} l1_cm_action_info_struct_type;

/* Pending TGPS update info */
typedef struct
{
  /* CM info sequence number to indicate when pending CM info was
     received. At time of CM suspend, a snapshot of the global seq
     number will be taken and on (deact all) resume all ACT before
     that will be discarded */
  uint8   seq_num;
  /* indicate if activate or deactivate is pending or not */
  boolean inactive_pending;
  boolean active_pending;

  /* If in TGPS info activate_action_time is valid then this flag
     is set to false and is set to TRUE on that action CFN.
     Otherwise this is set to TRUE to begin with.
     Applicable to TGPS only being activated */
  boolean activate_action_time_current;

  /* Deactivate TGP seq info */
  l1_cm_tgp_seq_info_struct_type tgp_seq_inactive;
  /* Activate TGP seq info */
  l1_cm_tgp_seq_info_struct_type tgp_seq_active;
} l1_cm_pending_tgp_seq_info_struct_type;

/* UL compressed mode parameters for encoder module */
typedef struct
{
  /* If TRUE SF is divide by 2 */
  boolean sf_div_by_2;
  /* number of compressed slots in next frame */
  uint8 num_cm_slots_txed;
  /* first CM slot. Valid if txed slot less that MAX_SLOT_PER_FRAME */
  uint8 first_cm_slot;
} l1_cm_ul_param_struct_type;

/* DL compressed mode parameters for demod module */
typedef struct
{
  /* CCTrCh change information */
  boolean cctrch_change;
  uint8 cctrch_index;

  /* D/L compressed mode Slot format index */
  uint8  cm_slot_fmt_idx;

  /* indicate if there is OVSF code change or not */
  boolean ovsf_code_change[L1_MAX_RL];
  /* D/L compressed mode OVSF code */
  uint16 cm_ovsf_code[L1_MAX_RL];
  /* D/L compressed mode scrambling code change type */
  l1_cm_dl_scr_code_change_type_enum_type scr_code_change_type[L1_MAX_RL];
} l1_cm_dl_param_struct_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                   ENUM: PHYCHAN_DB_CFG_TYPE_ENUM_TYPE

This enumerates the two pages of the physical channel database, the ordered
config page, and the current config page.  Parameters are stored in the
ordered config page until the action time of the associated message and
then are transferred to the current config page as the configuration is
changed.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum {
  ORDERED_CFG,
  CURRENT_CFG,
  NUM_PHYCHAN_DB_CFG_TYPES
} phychan_db_cfg_page_enum_type;

#endif /* L1_DL_PHYCHAN_CFG_H */
