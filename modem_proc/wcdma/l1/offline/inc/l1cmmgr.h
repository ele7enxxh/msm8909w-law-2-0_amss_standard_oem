#ifndef L1CMMGR_H
#define L1CMMGR_H

/*============================================================================
                                  L 1 C M M G R . H
DESCRIPTION

Copyright (c) 2009-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1cmmgr.h_v   1.2   18 Jul 2002 09:25:44   djm  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1cmmgr.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
11/15/16    vn      API to get slots to start CM
02/24/15    ap      Initial check-in for WL1 QSH framework 
12/29/15    svh     Added HS client to compressed mode activity notification framework
11/26/14    hdk     Removing FEATURE_WCDMA_JOLOKIA_MODEM from DSDA ASDiv changes.
06/24/14    hdk     Featurizing DSDA ASDiv changes under FEATURE_WCDMA_JOLOKIA_MODEM
06/13/14    hdk     DSDA ASDiv feature
05/22/14    wz      Validate new DL config against current active CM config
                    when CPHY_SETUP_REQ does not contain CM parameters.
04/27/14    jkb     Add check for Feature dual sim with feature qta 
09/23/13    as      Porting Dual Sim from Triton to Dime.
07/23/13    vs      defined the API l1_cm_query_cm_in_prog
07/02/13    ks      Add ability to freeze cm measurements through qxdm
01/28/13    dp      Added support for Antenna Tuner
11/28/12    db      MCVS feature related changes  
09/14/12    cc      Move index overflow check from macro into line by line
08/09/12    hk      Streamlined FACH and DCH module cleanup procedures
08/14/12    dp      Added qxdm event for CM state changes
08/07/12    dp      W2L PSHO time improvements
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/07/12    vs      MBMS feature cleanup.
05/03/12    vsr     FET/DED/ED-V2 Feature checkin
04/09/12    dp      Added normal frame indication (NFI) system.
03/23/12    dp      CM act/deact notification interface unification
01/30/12    vs      Feature cleanup.
01/26/12    vs      Nikel feature cleanup.Removed the feature FEATURE_WCDMA_EQ_CONTROLLER .
01/23/12    dm      Added return type to the function l1_cm_freeze_cm_on_normal_frame.
01/19/12    zr      Mainlined FEATURE_MCAL_WCDMA and FEATURE_WCDMA_INTRAF_RESERVE_IN_CM
01/17/12    vsr     Added ARD Algorithm Support
01/11/12    mn      Added return type to the function l1_cm_freeze_cm_on_normal_frame.
12/21/11    xl      Merge from MPSS.NI.1.1
11/30/11    rc      Added support for FEATURE_WCDMA_DSR_SUPPORT 
11/06/11    sv      Adde code changes for intraf cell crossing optimization.
11/02/01    kcm     Added one more parameter in the function 
                    l1_cm_find_frame_to_cleanup
08/29/11    kcm     Added function l1_cm_find_frame_to_cleanup which looks for a normal frame 
                    and sends a suspend/freeze to CM                  
04/28/11    sj      upmerge from offline/main
04/25/11    mr      Updated the API L1_TGMP_VALID to support TGMP_LTE 
04/07/11    mm      Prototype for callback function to enable CME after receiving 
                    LTE DEINIT CNF
12/08/10    mr      Configuring sample server for WCDMA when CM pattern is not active 
                    and TGMP is not present in active pending list 
05/28/10    mr      Do not call mcalwcdma_cm_init during l1_cm_init
03/30/10    rvs     Remove FEATURE_WCDMA_CME_CONTROLLER.
03/26/10    hk      Freeze/UnFreeze CM during ASU procedure
08/21/09    yh      Check with CME if it's off before requesting low clock.
08/18/09    yh      Create function to replace use of CM stat and tgp_seq info global vars.
08/14/09    yh      Added new API to indicate when GSM patterns activated during CM.
07/15/09    rvs     Edited feature flags to include
                    FEATURE_WCDMA_QICE_CONTROLLER.
04/20/09    yh      Compiler warning fixes.
12/19/08    yh      Call CME callback from local command
11/20/08    yh      Add CME support when activating/deactivating CM.
11/13/08    yh      Add CM support
10/03/08    hk      Bringing in 7k Mailine fixes
10/24/07    vsr     Mainlining MBMS and Enhanced DL changes 
08/14/07    vb      moved prototype l1_cm_query_compressed_mode_active and
                    l1_cm_query_num_of_cm_patterns_active to l1rrcif.h
08/08/07    vsr     Moved CM related definitions to l1sapcommon.h & l1dlphychancfg.h
                    Also moved in definitions from dldem.h
03/27/07    bbd     Added API for any pattern active status.
02/27/07    bbd     Added API to return current number of active or pending
                    active patterns.
01/26/07    yh      Added support for MCAL CM
10/02/06    bbd     Added support for MCAL.
08/01/06    yh      Added support to have a counter to keep track of how many
                    RSSI gaps can be given away to intraf.
03/16/06    mc      Added support for Equalizer under the feature flag
                    FEATURE_WCDMA_EQ_CONTROLLER
12/13/05    mc      Moved NORMALIZE and NORMALIZE_CFN to l1utils.h and 
                    l1mathutil.h
10/03/05    bbd     Added l1_cm_query_bsic_reconf_abort_timer().
08/23/05    bbd     Added sequence number field to pending info.
05/12/05    gs      Change the TGMP in CM init structure to UINT16. This is
                    required by mDSP interface to be UINT16.
05/11/05    kps     lint error fix
05/06/05    bbd     Added option to save pending info during suspend CM request.
04/14/05    bbd     Added l1_cm_hho_suspend_info_struct_type to store TGP info
                    and adv stat at suspend in case of failed HHO.
08/18/04    scm     Change l1_cm_get_ul_params() to return void.
06/07/04    gs      Added macro L1_CM_INTRA_FREQ_RESERVE_FORCE_FR_ADV
03/03/04    gs      Added another set of frame advance for intra-freq reserve
02/11/04    gs      Externalized function l1_cm_query_compressed_mode_active
01/08/04    gs      Removed feature define around function declaration
                    for l1_cm_query_tgmp_for_gsm_meas
12/04/03    gs      Added prenthesis around macro argument tgp_state_ctrl_info
                    of macro L1_CM_NUM_FR_IN_TGPRC
                    Added structure member init_tgprc_advance_num_fr in
                    strcuture l1_cm_tgp_seq_info_struct_type. This is L1
                    internal use only.
                    Externalized function l1_cm_get_last_effective_tgps_info   
11/06/03    gs      Added typedef for L1_CM_SUSPEND_DONE_CB_FUNC
                    Updated declaration for l1_cm_suspend_cm and
                    l1_cm_resume_cm
10/24/03    gs      Externalized function l1_cm_tgmp_active
10/10/03    gs      Changed the cfn from uint8 to uint16 for freeze
09/24/03    gs      Externalized functions under feature
                    FEATURE_WCDMA_L1_CM_FREEZE_CM
08/18/03    gs      Changed declaration of function l1_cm_update_cm_params.
                    It returns uint8 rather than void.
08/03/03    gs      Externalized functions l1_cm_set_finger_cfg_event_state,
                    l1_cm_cleanup_fing_cfg_update.
                    Removed FEATURE_WCDMA_L1_CM_SUSPEND_RESUME_SUPPORTED
                    feature define aroud suspend/resume related functions.
07/11/03    gs      Externalized function l1_cm_query_gsm_clk_turned_on.
05/29/03    gs      Added macro L1_CM_ACTION_TIME_NOW_ADVANCE for setting
                    action CFN for action time type NOW.
04/23/03    gs      Externalized globals that save extended information when UL
                    compressed mode info is queried.
04/10/03    gs      Added macro L1_CM_PENDING_INFO_MIN_ACT_START_ADV_TOBE_REPLACED
                    for pending TGPS replacement.
                    Defined macros UNNORMALIZE_CFN, L1_CM_NUM_FR_IN_TGPRC
                    Added structure member activate_action_cfn_valid and
                    activate_action_cfn to structure l1_cm_tgp_seq_info_struct_type
                    for TGPS activate action time.
                    Added member activate_action_time_current in TGPS pending
                    info structure.
02/27/03    gs      Externalized globals l1_cm_delta_sir_applied and
                    l1_cm_ebnt_adj_val.
                    Externalized function l1_cm_query_delta_sir_applied_in_tti
01/30/03    gs      - Rearranged members of enum l1_cm_tgps_state_enum_type
                      in order of states in pattern 1 gaps followed by pattern
                      2 for easy position check.
                    - Added prev_meas_purpose member of type
                      l1_cm_tg_meas_purpose_enum_type in structure definition
                      l1_cm_tgp_seq_info_struct_type. This is for internal use
                      only.
                    - Added member ovsf_code_change for each RL to structure
                      l1_cm_dl_param_struct_type for HLS support.
                    - Changed prototype of function corresponding to l1cmmgr.c
                      changes and externalized new functions defined in
                      l1cmmgr.c
12/04/02    gs      Added enum definition for TGPS deact reason.
                    Added function types for ACT DEACT indications.
                    Externalized funtions for registering/deregistering ACT
                    DEACT callbacks and getting TGPS status.
10/11/02    gs      Added the macro, enum and struct definition related
                    to feature FEATURE_WCDMA_INTRAF_RESERVE_IN_CM.
                    Externalized the functions related to this feature.
09/27/02    gs      Increased the number of TGPS info to 2 x the max TGPS.
09/19/02    gs      Externalized rge function to support for blocking the
                    CM GAP freq switch for intra-frequency switch.
                    Added the starting slot to the UL CM info get function.
08/28/02    gs      Change history for previous checkin:
                    Added macro L1_CM_TGPS_FR_COUNT_INFINITE
                    Added structure member tgps_fr_len_left in
                    l1_cm_tgps_stat_struct_type
08/09/02    gs      Changed the number of TGP seq to 7. The TGPSI are from 1 to 6
08/02/02    djm     moved ifdef FEATURE_CM_SUPPORTED back up, with l1rrcif.h
                    mods for support.
07/31/02    gs      TGPRC max count corrected to 511
                    Changed the type of tgp_rep_count to uint16
                    Added macro for CM run time check.
                    Added new TGPS state TGPS_COLLIDED and new structure member
                    to TGPS state for collision TGPSI Id.
                    Added new structure member to CM info structure
                    action needed.
                    Added structure for pending activate and deactivate
                    command.
                    Changed the externalization of l1_cm_change_cm_state_control
                    function to return boolean.
07/24/02    gs      Corrected the macro L1_CM_DL_FING_PHCH_CFG.
07/18/02    djm     moved ifdef FEATURE_CM_SUPPORTED below data type declaration
                    this will allow inclusion in l1rrcif.h without concern for
                    feature defines.
07/10/02    gs      Added the macros for CM advanced info and CM measurement .
                    Changed TGPRC infinite value, same as in RRC spec 25.331
                    Corrcted TGSN min to 0 as starting slot can be 0.
                    Changed number of CM init params from 6 to 7
                    Changed the TGMP enum definition
                    Added n_identify abort and t_reconf_abort in TGP seq info.
                    Added more element in TGPS stat ifo for tracking the gap state.
                    Put CM action related global variable in l1_cm_action_info_struct_type
                    Added srch index field in CM init structure.
                    Added new function externalization.
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "l1sapcommon.h"
#include "l1utils.h"
#ifndef FEATURE_CM_UNIT_TEST
#include "l1task_v.h"
#include "l1rrcif.h"
#endif /* FEATURE_CM_UNIT_TEST */
#include "l1dlphychancfg.h"
#include "custwcdma.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* This correspond to 8 advanced stat/action info +
   1 extra stat info for calculating the last (latest) action info
   1 present stat/action info +
   4 previous CFN info */
#define L1_CM_NUM_FRAME_ADV_ACTION_INFO 8
#define L1_CM_NUM_FRAME_ADV_STAT_INFO (L1_CM_NUM_FRAME_ADV_ACTION_INFO + 1)
#define L1_CM_NUM_FRAME_PREV_ACTION_INFO 4
#define L1_CM_NUM_FRAME_INFO (L1_CM_NUM_FRAME_ADV_STAT_INFO + 1 + \
                              L1_CM_NUM_FRAME_PREV_ACTION_INFO)

#define L1_CM_TGPSI_INVALID 0xFF
#define L1_CM_TGPS_FR_COUNT_INFINITE 0xFFFFFFFFL

#define L1_CM_TGPRC_INFINITE    0
#define L1_CM_TGD_UNDEFINED     0
#define L1_CM_TGL_NA            0xFF


/* Min and Max values */
/* ------------------ */
#define L1_CM_TGSPI_MIN 1
#define L1_CM_TGSPI_MAX 6
#define L1_CM_TGRPC_MIN 1
#define L1_CM_TGRPC_MAX 511
#define L1_CM_TGSN_MIN  0
#define L1_CM_TGSN_MAX  14
#define L1_CM_TGL_MIN   1
#define L1_CM_TGL_MAX   14
#define L1_CM_TGD_MIN   15
#define L1_CM_TGD_MAX   269
#define L1_CM_TGPL_MIN  1
#define L1_CM_TGPL_MAX  144
#define L1_CM_DELTA_SIR_MIN 0
#define L1_CM_DELTA_SIR_MAX 30

#define L1_CM_MIN_TXED_SLOTS_PER_FR 8
#define L1_CM_SLOT_FMT_A_OFFSET 17
#define L1_CM_SLOT_FMT_B_OFFSET 34

#define L1_CM_GSM_BSIC_IDENTIFY_ABORT_MIN 1
#define L1_CM_GSM_BSIC_IDENTIFY_ABORT_MAX 128
#define L1_CM_GSM_BSIC_RECONF_ABORT_MAX 20

/* number of frames after tgmp de-activation we can first notify clients that 
   CM is deactive for that tgmp */
#define L1_CM_FRAMES_WAIT_NOTIFY_DEACT 2
/* number of frames before tgmp activation we can notify clients that CM is 
   active for that tgmp */
#define L1_CM_FRAMES_NOTIFY_ACT_PRECEED_ACT 3

/* Min time advance required for compressed mode validation to pass if the TGPS
   is being replaced in pending info */
#define L1_CM_PENDING_INFO_MIN_ACT_START_ADV_TOBE_REPLACED 12
/* action time now minimum advance required. It is desirable that UTRAN should
   keep the TGCFN well far in future for this kind of delay */
#define L1_CM_ACTION_TIME_NOW_ADVANCE (L1_CM_PENDING_INFO_MIN_ACT_START_ADV_TOBE_REPLACED + 2)

/* Maximum pending info sequence number */
#define L1_CM_PENDING_INFO_MAX_SEQ_NUM  0xFF
  
/* CM init command field shifts and bit masks */
#define CM_INIT_RPP_BMSK 0x0004
#define CM_INIT_RPP_SHFT 2
#define CM_INIT_ITP_BMSK 0x0002
#define CM_INIT_ITP_SHFT 1
#define CM_INIT_DLF_BMSK 0x0001
#define CM_INIT_DLF_SHFT 0

/* Macros defined for MCVS */
/* Default TGPL = 8 Frames */
#define L1_CM_TGPL_DEFAULT  8
/* Max duration of CM is 336ms */
#define L1_CM_DCM_MAX  336
/* The inter frequency step1 timer (in terms of frames) */
#define L1_CM_ONE_INTERF_CARRIER_STEP1_TIMER  1500
#define L1_CM_TWO_INTERF_CARRIER_STEP1_TIMER  500
#define INVALID_OH_PS1  0xFFFF
#define L1_CM_GAPS_PER_TGPL_FACTOR  30

#define L1_CM_NUM_FR_IN_TGPRC(tgp_state_ctrl_info) \
  (((((tgp_state_ctrl_info)->tgp_rep_count + 1) / 2) * (tgp_state_ctrl_info)->tgp_len_1) + \
   (((tgp_state_ctrl_info)->tgp_rep_count / 2) * (tgp_state_ctrl_info)->tgp_len_2))

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

#define L1_TGMP_VALID(tgmp)        ( ((tgmp) > L1_CM_TGMP_NONE) \
                                     && ((tgmp) < L1_CM_NUM_TGMP_TYPE))


/* This structure is for L1 CM init parameters */
typedef struct
{
  /* Bit 2 = RPP, Bit 1 = ITP, Bit 0 = DL format type */
  uint16 power_control_bits;

  /* UL compressed mode method */
  uint16 ul_cm_method;

  /* DL compressed mode method */
  uint16 dl_cm_method;

  /* First compressed slot */
  uint16 first_cm_slot;

  /* transmission gap length */
  uint16 tgl;

  /* serach index for different frequency */
  uint16 srch_idx;

  /* transmission gap measurement purpose */
  uint16 tgmp;
} l1_cm_init_parms_struct_type;

/* structure to store currect active TGPSI and their parameters */
typedef struct
{
  uint8 tgp_seq_id; /* TGPSI (1..6) */
  /* transmission gap length in slots (1..14)
     This may not be specified. RRC must set TGL2 = TGL1 */
  uint8 tg_len_1;
  uint8 tg_len_2;
  /* transmission gap pattern length in frames (1..144)
     This may not be specified. RRC must set TGPL2 = TGPL1 */
  uint8 tgp_len_1;

}l1_cm_tgp_state_info_struct_type;

typedef struct
{
  /* Holds number of TGPSI's active */
  uint8 l1_cm_num_tgpsi_active;
  l1_cm_tgp_state_info_struct_type active_tgpsi_info[L1_CM_MAX_TGP_SEQ_COUNT];
}l1_cm_active_tgpsi_info_struct_type;

/* maximum number of WCDMA frames that can be reserved for intra
   frequency action purposes */
#define L1_CM_NUM_INTRAF_MAX_FR_RESERVE 25

/* number of frame/delta advance at which the frames in CM
   will be reserved */
#define L1_CM_INTRA_FREQ_RESERVE_FR_CASE1_ADV 1
#define L1_CM_INTRA_FREQ_RESERVE_FR_CASE2_ADV 2
#define L1_CM_INTRA_FREQ_RESERVE_FR_CASE1_2_DELTA \
  (L1_CM_INTRA_FREQ_RESERVE_FR_CASE2_ADV - L1_CM_INTRA_FREQ_RESERVE_FR_CASE1_ADV)
  
#define L1_CM_INTRA_FREQ_RESERVE_FORCE_FR_ADV 3
  
/* This enum defines the intra freqency reserve req Id that can
   used to reserve the frames in CM */
typedef enum
{
  L1_CM_GAP_RESERVE_INTRAF_SRCH,
  L1_CM_GAP_RESERVE_INTRAF_NBCH_DECODE,
  L1_CM_GAP_NUM_RESERVE_REQ_ID
} l1_cm_gap_reserve_req_id_enum_type;

/* This structure for keeping the intra frequency req Id reserve
   info */
typedef struct
{
  /* number of frames reserved for intraf purpose */
  uint8 num_frames;
  /* starting CFN of reserve */
  uint8 start_cfn;

  /* number of frames remaining */
  uint8 num_frames_remaining;
} l1_cm_gap_reserve_req_info_struct_type;

typedef struct
{
  uint8 suspend_cfn;  

  /* backup transmission gap pattern sequence info.
  l1_cm_tgp_seq_info is copied to this structure during HHO so that in the 
  event of a failed HHO, we can resume previous CM actions */
  l1_cm_tgp_seq_info_struct_type tgp_seq_info[L1_CM_MAX_TGP_SEQ_COUNT];

   /* Back up stat at suspend CFN time.
   l1_cm_tgps_stat_adv for the suspend CFN is copied to this structure
   during HHO so that in the event of a failed HHO, we can resume previous
   CM actions */
  l1_cm_tgps_stat_struct_type tgps_stat_at_suspend[L1_CM_MAX_TGP_SEQ_COUNT];  

  l1_cm_pending_tgp_seq_info_struct_type pending_tgp_seq_info[L1_CM_MAX_TGP_SEQ_COUNT];
}l1_cm_hho_suspend_info_struct_type;

/* nfi (normal frame indication) definitions.  Used to notify nfi clients via 
   cb during normal (non CM) frames */
typedef void (l1_cm_nfi_cb_type)(void);
typedef enum
{
  L1_CM_CHNL_SETUP_NFI,
  L1_CM_NUM_NFI_CLIENTS
} l1_cm_nfi_client_enum_type;

/* A generic bit-field type used to store some form of boolean state 
   for each of the various tgmp types, one per bit.  The tgmp-type to bit 
   position mapping goes by the l1_cm_tg_meas_purpose_enum_type such that the 
   2^n bit position corresponds to the nth enumeration.  (In other words, 
   right-most bit corresponds to 0, with more leftward bits corresponding to 
   higher enumerations). */
typedef uint8 l1_cm_tgmp_states_bfld_type;

/* a generic typedef for something being active or deactive */
typedef enum 
{
  L1_CM_DEACTIVE=0,
  L1_CM_ACTIVE
} l1_cm_active_deactive_enum_type;

/* This enum defines the client types used to reserve their callback functions when 
   CM pattern is going active/deact */
typedef enum
{
  L1_CM_CLIENT_LTE=0,
  L1_CM_CLIENT_CME,
  L1_CM_CLIENT_DSR,
  L1_CM_CLIENT_RXD,
  L1_CM_CLIENT_FET,
  L1_CM_CLIENT_QXDM_EVENTS,
  L1_CM_CLIENT_ATUNER,
  L1_CM_CLIENT_HS,
#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
  L1_CM_CLIENT_ASDIV,
#endif
  L1_CM_NUM_CLIENTS
} l1_cm_act_deact_client_enum_type;

/* A generic bit-field type used to store some form of boolean state 
   for each of the various clients, one per bit.  The client-type to bit 
   position mapping goes by the l1_cm_act_deact_client_enum_type such that the 
   bit position storing value 2^n corresponds to the nth enumeration.  (In 
   other words, right-most bit corresponds to enum 0, with more leftward bits 
   corresponding to higher enumerations). */
typedef uint8 l1_cm_client_states_bfld_type;

/* Call back function pointer types used with act/deact notifications and 
   client status checks  */
typedef void (l1_cm_act_deact_notif_func_type)(l1_cm_active_deactive_enum_type dep_patterns_active);
typedef boolean (l1_cm_client_status_func_type)(void);

typedef struct
{
  /* pointer to a callback function l1cmmgr should call just before one of the
     client-dependent tgmp patterns goes active or just after all 
     client-dependent tgmp patterns go deactive */
  l1_cm_act_deact_notif_func_type *cm_act_deact_notif_cb_ptr;

  /* pointer to a callback function l1cmmgr can use to check client's status.
     This field must be populated if a client has notification dependency info */
  l1_cm_client_status_func_type *client_status_cb_ptr;
  
} l1_cm_client_registration_info_struct_type;

/* states describing which exclusion set group is currently in control (if any) */
typedef enum
{
  /* Nobody in control */
  L1_CM_NONE_CTRL,
  /* CM in Control */
  L1_CM_CTRL,
  /* External Control */
  L1_CM_EXT_CTRL
} l1_cm_excl_group_ctrl_state_enum_type;

/* Structure which stores info on two exclusively-on groups of clients.  The 
   info is used to ensure that only one group can have ON-status clients at a 
   time.  One group has higher priority than the other, and will kick the 
   other's group's clients to off if it wants to take control and have its own
   clients turn on */
typedef struct
{
  /* list of clients in the higher priority exclusion group */
  uint8 num_cm; 
  l1_cm_act_deact_client_enum_type cm_clients[L1_CM_NUM_CLIENTS];

  /* list of clients in the lower priority exclusion group */
  uint8 num_ext;
  l1_cm_act_deact_client_enum_type ext_clients[L1_CM_NUM_CLIENTS];

  /* Keeps track of which group is currently in control (has clients active) */
  l1_cm_excl_group_ctrl_state_enum_type ctrl_state;

  /* indicates if currently-in-control group is giving up control.  Each bit 
     being 1 corresponds to a client that is still ON while giving up control */
  l1_cm_client_states_bfld_type pending_clients_for_none_ctrl;

  /* indicates if control is trying to transition from the in-control group to 
     the not-in-control group. */
  boolean pending_ctrl_switch;

} l1_cm_excl_client_set_info_struct_type;

#define L1_CM_EXCLSET_CM_GRP_IDX_TO_CLIENT(set_ptr, idx_in) (set_ptr)->cm_clients[idx_in]
#define L1_CM_EXCLSET_EXT_GRP_IDX_TO_CLIENT(set_ptr, idx_in) (set_ptr)->ext_clients[idx_in]

/* data on clients that should be hard coded before compilation */
typedef struct
{
  /* ctrl bitmask describing the client's notification dependencies across the
     various TGMP types */
  l1_cm_tgmp_states_bfld_type tgmp_dep_bmsk;

  /* if true, when client is active l1cmmgr must block searches/measurements
     during cm gaps */
  boolean blocks_cm_searches;

  /* index of the exclusivity info this client belongs to.  If a client is a 
     member of an exclusivity group, there are additional restrictions on when 
     it can be on.  A client can only be part of a single exclusion group. */
  boolean in_excl_set;
  uint8 excl_set;

} l1_cm_static_client_info_struct_type;

/* Stores internal notification paramaters and state for a single client.  This 
   information is used when patterns go active and deactive to see whether a 
   client should be notified of CM's status */
typedef struct
{
  /* --- static info --- */

  /* will be set at l1cmmgr initialization to the static */
  l1_cm_static_client_info_struct_type *static_info_ptr;

  /* --- dynamic info ---  */

  /* Whether the client is registered.  All other dynamic info is invalid if 
     this is false */
  boolean is_registered;

  /* the parameters the client registered with */
  l1_cm_client_registration_info_struct_type reg_info;

  /* describes which callback (act or deact) we called last*/  
  l1_cm_active_deactive_enum_type prev_notified;

} l1_cm_act_deact_client_info_struct_type; 

/* describes what the notifyable state is for each tgmp.  Knowing this for 
   every tgmp type will help decide which clients need to be notified. */
typedef enum
{
  L1_CM_NDEACT,
  L1_CM_NACT_PENDING_NDEACT,
  L1_CM_NACT
} l1_cm_tgmp_notif_state;

/* This structure stores notification state information specific to a single 
   tgmp.  This data will be used when when figuring out which client's should 
   be notified in any given cfn. */
typedef struct
{
  /* indicates whether the tgmp is notifyable as active or deactive, given the 
     timing parameters and the recent past/future acts and deacts */
  l1_cm_tgmp_notif_state notif_state;

  /* the cfn in which this tgmp went deactive last (assuming 
     there are no pending activations of this tgmp seen in the future within 
     the notify-active window). Field only valid if pending_notify_deact is TRUE */
  uint8 deact_cfn_for_pending_notify_deact;
  
} l1_cm_tgmp_act_deact_notification_state_struct_type;

/* This struct is for storing the info received from 
   QXDM to enable or disable skip_cm_measurements */
typedef struct
{
  boolean valid;
  boolean qxdm_skip_cm_meas;
} l1_cm_qxdm_skip_cm_meas_info_params_struct_type;

#define L1_CM_CME_NONE  0
#define L1_CM_CME_ACT   1
#define L1_CM_CME_DEACT 2
/* Callback function definition */
typedef void L1_CM_CME_CB_FUNC_TYPE(boolean);
typedef boolean L1_CM_CME_STATUS_CB_FUNC_TYPE(void);

typedef void L1_CM_SUSPEND_DONE_CB_FUNC(void);

typedef void (l1_cm_cleanup_done_cb_type)(void *);

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* fing cfg update function */
typedef void L1_CM_FING_CFG_UPDATE_FUNC_TYPE(void);
extern L1_CM_FING_CFG_UPDATE_FUNC_TYPE* l1_cm_fing_cfg_update_func;

/* cmEbNtAdj history for logging purpose */
extern boolean l1_cm_delta_sir_applied[16];
extern uint16 l1_cm_ebnt_adj_val[16];

/* compressed mode parameters */

/* This variable will indicate if the compressed mode info is present */
extern boolean l1_cm_info_present;
/* This variable saves the compressed mode DPCH info */
extern l1_cm_info_struct_type *l1_phychan_db_cm_info;
/* This variable indicate the validity of compressed mode information */
extern boolean l1_cm_info_invalid;
/* indicate that compressed mode in DL can't be activated with the given config info */
extern boolean l1_cm_method_invalid;

/******************************************************************************
      VARIABLES TO SUPPORT COMPRESSED MODE SOFT HAND-OVER OPERATIONS
******************************************************************************/
/* CFN at which intra-frequency switching reservation starts */
extern uint8   l1_cm_intraf_rsv_start_cfn;
/* Flag indicating the CFN given above is a valid indicator of reservation */
extern boolean l1_cm_intraf_rsv_cfn_valid;
/* Flag indicating whether a reserve or unreserve request is successful */
extern boolean l1_cm_intraf_rsv_status_ok;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION l1_cm_init

DESCRIPTION
  This function initializes the L1 compressed mode manager module data
  structure to initial values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_init(boolean init);

/*===========================================================================
FUNCTION l1_cm_tgmp_conflict_validate

DESCRIPTION
  This function checks for the TGMP validation conflict. It checks that there
  should not be any TGPS that is activating when another active TGPS that can
  be active at the same time based on the already accepted parameters.
  The already accepted TGPS can be with pending ACT or DEACT or can be
  currently running.
  
  This facilitates the setting of INVALID_CONFIG by RRC in case of failure as
  per Spec 8.6.6.15 for validating DPCH compressed mode info.
  
  One of the potential reason for this validation seems to be that in case
  of collision of TGPSI with same TGMP it will not be possible to decide
  whose measurement specific parameters should be taken. For example
  N_identify_abort and T_reconfim_abort for GSM measurement.
  
  This function assumes that the TGPS is marked active with info valid
  and have TGMP other than TGMP_NONE.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on validation result.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_tgmp_conflict_validate(
  /* TGP seq info of the new TGPS to be set */
  l1_cm_info_struct_type *cm_info);

/*===========================================================================
FUNCTION l1_cm_validate_tgp_seq_info

DESCRIPTION
  This function validates the transmission gap pattern sequence information.
  It verifies the following:
  1. If the compressed mode information is not existing in this structure
     passed by higher layer and pattern sequence is set to active then the
     compressed mode info must be existing already.
  2. All the parameters should be with in range.
  3. Only supported compressed mode types are used.
  4. There is sufficient time so that compressed mode can be configured. This
     is set to 3 frames for now.
  5. Verify that TGSN, TGLx and TGD are appropriately positioned to not to
     break the CM state machine. The reason for this is the range allowed
     for these variable. The conditions checked are:
     - TGL1 > TGD if TGD > 0
     - TGSN + TGL > (TGPL * 15)
     - TGSN + TGD + TGL2 > (TGPL * 15) if TGD > 0

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE as per validation result.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_validate_tgp_seq_info(
  /* compressed mode information */
  l1_cm_info_struct_type *cm_info);

/*===========================================================================
FUNCTION l1_cm_initial_process_cm_info

DESCRIPTION
  This function processes the CM info for any TGPS activate or deactivate
  request. It saves the activate/deactivate request. This is called by
  function l1_cm_update_info for this primary job. In addition to that
  is also sets the action needed field for newly added TGPS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_initial_process_cm_info(
  /* compressed mode information */
  l1_cm_info_struct_type *cm_info);

/*===========================================================================
FUNCTION l1_cm_update_info_tgps_activate_common

DESCRIPTION
  This function updates the information when any TGPS is activated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_update_info_tgps_activate_common(
  /* TGPS Id */
  uint8 tgpsi);

/*===========================================================================
FUNCTION l1_cm_update_info_tgps_deactivate_common

DESCRIPTION
  This function updates the information when any TGPS is deactivated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_update_info_tgps_deactivate_common(
  /* TGPS Id */
  uint8 tgpsi);

/*===========================================================================
FUNCTION l1_cm_update_info

DESCRIPTION
  This function updates the compressed mode information for each compressed
  mode pattern sequence in the compressed mode info passed in as the argument
  to this function. It checks if the compressed mode parameters for a pattern
  sequence exists then it copies complete structure otherwise it gets only
  active state and starting CFN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_update_info(
  /* compressed mode information */
  l1_cm_info_struct_type *cm_info);

/*===========================================================================
FUNCTION l1_cm_init_tgps_stat

DESCRIPTION
  This function initializes the compressed mode internal stats that are used
  in compressed mode update. The following parameters are initialized.
  - TGPS position = 0 (in frames)
  - TG1 end position        = (TGSN + TGL1 - 1) / 15
  If TGD is non zero then
    - TG2 start position    = (TGSN + TGD) / 15
    - TG2 start slot number = (TGSN + TGD) % 15
    - TG2 end position      = (TGSN + TGD + TGL2 - 1) / 15

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_init_tgps_stat(
  /* TGP seq info structure pointer */
  l1_cm_tgp_seq_info_struct_type *tgp_seq_info,
  /* CM TGPS stta structure pointer */
  l1_cm_tgps_stat_struct_type *tgp_seq_stat);

/*===========================================================================
FUNCTION l1_cm_end_compressed_mode

DESCRIPTION
  This function is called when compressed mode is to be terminated.
  This happens only when DPCH is dropped. Demod module should call this
  function on DPCH drop. This initialize all the state and internal stats
  for the compressed mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_end_compressed_mode(void);

/*===========================================================================
FUNCTION l1_cm_init_tgps_stat_adv

DESCRIPTION
  This function gets initializes the advanced info. This should be called
  at the very begining when compressed mode pattern is set or when ever
  it is activated or deactivated. It will recalculate the TGPS CM state
  and stats based on active status of the pattern sequence.
  It assumes that variable l1_cm_current_stat_head is pointing at the
  first advanced info index. It will iterate from this index upto number
  of advanced info index defined by L1_CM_NUM_FRAME_ADV_STAT_INFO.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  All the information for TBPS stat for the given TGPS will be updated.
  The advanced action should be recalculated after calling this function.
===========================================================================*/
extern void l1_cm_init_tgps_stat_adv(
  /* CM pattern seq index */
  uint8 tgps_index,
  /* starting CFN */
  uint8 cm_gap_cfn,
  /* active seq state type to start with */
  l1_cm_tgps_state_enum_type active_start_state,
  /* advanve stat start index */
  uint8 adv_stat_info_start_idx,
  /* advanced stat depth (= number of stats to init) */
  uint8 adv_stat_depth);

/*===========================================================================
FUNCTION l1_cm_get_next_state

DESCRIPTION
  This function gets the next state of the CM state machine. It takes the
  of TGPSI, CFN and the current TGPS stat. Fron this information it generates
  next state. It calls the function to function to change the stats of the
  CM as per next calculated. The new state ans stat and updated in to the
  TGPS stat pointer passed.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  The TGPS stat structure pointer by parameter passed to function is
  updated as per the next state.
===========================================================================*/
extern void l1_cm_get_next_state(
  /* CM pattern seq index */
  uint8 tgps_index,
  /* the next CFN for the TGPSI */
  uint8 next_cfn,
  /* CM TGPS stta structure pointer */
  l1_cm_tgps_stat_struct_type *this_tgp_seq_stat);

/*===========================================================================
FUNCTION l1_cm_change_state

DESCRIPTION
  This function changes the state of the compressed mode pattern sequence Id
  passed as parameter in the function. While doing so it does the following:
  - Verifies that if the next state is going to be compressed mode then
    no other pattern sequence should have already occupied the next frame
    for compressed mode. RRC is supposed to validate it before configuring
    L1 for compressed mode.
  - If the above condition is ment and compressed mode pattern sequence state
    is in come transmission gap it set the occupied flag to TRUE and set the
    compressd mode prep flag to TRUE and also set the pattern sequence Id of
    which compressed mode parametes are used for configuring the compressed
    mode.
  - If the next state is not in any transmission gap then it sets the
    compressed mode exit flag to TRUE.
  - If any pattern sequence is entering in to any transmission gap 1 then
    it initializes pattern sequence position for that pattern to 0 in units
    of frames.
  - If any pattern sequence is transitioning from L1_CM_TGPS_WAIT_TO_START to
    L1_CM_TGPS_IN_P1_GAP1 it initialize the transmission gap pattern repetition
    count (TGPRC count) to 0. 
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_change_state(
  /* TGPS Id */
  uint8 tgps_index,
  /* the current CFN */
  uint8 this_cfn,
  /* previous state of this TGPS */
  l1_cm_tgps_state_enum_type prev_state,
  /* next state of this TGPS */
  l1_cm_tgps_state_enum_type next_state,
  /* CM TGPS stta structure pointer */
  l1_cm_tgps_stat_struct_type *tgp_seq_stat);

/*===========================================================================
FUNCTION l1_cm_state_update

DESCRIPTION
  This function is called to update the compressed state machine and to
  configure the DL and UL for the compressed frame if it is compming up in
  next frame. This function is called as the result of SEQ_CM_UPDATE_EVENT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_state_update(void);

/*===========================================================================
FUNCTION l1_cm_state_update_advance

DESCRIPTION
  This function update compressed mode advanced information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_state_update_advance(
  /* CFN at the first advanced info of the advanced info buffer */
  uint8 adv_info_first_cfn,
  /* depth of advanced info stat buffer */
  uint8 adv_info_depth,
  /* advanced info stat starting index */
  uint8 adv_info_start_stat_idx,
  /* indicate for TGPS active stat advanced info init what kind of
     starting state to be used. It can be a start of that TGPS or
     just creating advanced info from previos left one */
  boolean *stat_init_use_prev_stat_info);

/*===========================================================================
FUNCTION l1_cm_set_ebnt_adj_val

DESCRIPTION
  This function sets the DL outer loop power control adjust value for
  compressed mode. There are 3 types of power control gain to be adjusted
  for compressed mode.
  
  deltaP_compression:
    For SF/2 it is 3 dB
    For higher layer scheduling it is 0 dB
  deltaP_coding:
    This is specified by UTRAN ranges from 0 to 3 dB in step of 0.1 dB
  deltaP_scaling:
    This is for adjusting the scaling factor for linear value for dB
    The scaling factor includes the variable SF and Npilot^2
    
    For SF/2 it is 3 dB
    For higher layer scheduling it is 0 dB
    
    cmFpcAdjust =
     1024 * 10^(0.1 x [deltaP_compression + deltaP_coding + deltaP_scaling])
    = 1024 * 10^(0.1 deltaP_compression) * 10^(0.1 deltaP_coding) + deltaP_scaling])
    = 1024 * 10^(0.1 deltaP_compression) *
             10^(0.1 deltaP_coding) *
             10^(0.1 deltaP_scaling)
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_set_ebnt_adj_val(void);

/*===========================================================================
FUNCTION l1_cm_update_adv_action_info

DESCRIPTION
  This function updates the advanced action info at a given adv info index.
  It assumes the previous info available at the previous index (to advanced
  info index). At the begining this info is valid. Caller must ensure to
  initialize the previous info.
  It update the advanced action info in the following order:
  - It checks if the previous action info. If this indicate the frame occupied
    by TGPSI then if updates the CM state control change for the TGPSI that has
    occupied the frame so that it may update the the new action state to be
    occupied or free.
  - It tries to find the TGPSI that can occupy the frame for GAP by calling
    the function l1_cm_change_cm_state_control.
  - If the TGPSI that will be active in next action has a GAP that spans
    multiframe, check if this GAP is prempted by next frame GAP of a lower
    TGPSI. If this is the case then don't create this GAP.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_update_adv_action_info(
  /* CFN at which action info need to be updated */
  uint8 cfn,
  /* advanced info index */
  uint8 adv_cm_info_index);

/*===========================================================================
FUNCTION l1_cm_change_cm_state_control

DESCRIPTION
  This function changes the CM state control info of the compressed mode
  pattern sequence Id passed as parameter in the function. While doing so
  it does the following: 
  - Verifies that if the next state is going to be compressed mode then
    no other pattern sequence should have already occupied the next frame
    for compressed mode. RRC is supposed to validate it before configuring
    L1 for compressed mode.
  - If the above condition is ment and compressed mode pattern sequence state
    is in come transmission gap it set the occupied flag to TRUE and set the
    compressd mode prep flag to TRUE and also set the pattern sequence Id of
    which compressed mode parametes are used for configuring the compressed
    mode.
  - If the next state is not in any transmission gap then it sets the
    compressed mode exit flag to TRUE.
  - If any pattern sequence is entering in to any transmission gap 1 then
    it initializes pattern sequence position for that pattern to 0 in units
    of frames.
  - If any pattern sequence is transitioning from L1_CM_TGPS_WAIT_TO_START to
    L1_CM_TGPS_IN_P1_GAP1 it initialize the transmission gap pattern repetition
    count (TGPRC count) to 0. 
    
DEPENDENCIES
  None

RETURN VALUE
  TRUE if this TGPSI has no collision with lower TGPSI active next action
  otherwise returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_change_cm_state_control(
  /* TGPS Id */
  uint8 tgps_index,
  /* previous state of this TGPS */
  l1_cm_tgps_state_enum_type prev_state,
  /* next state of this TGPS */
  l1_cm_tgps_state_enum_type next_state,
  /* previous action info */
  l1_cm_action_info_struct_type *prev_action,
  /* next action info */
  l1_cm_action_info_struct_type *next_action);

/*===========================================================================
FUNCTION l1_cm_begin_cm

DESCRIPTION
  This function configures the DL and UL in to compressed mode. It checks
  the compressed mode type for both UL and DL and if the compressed mode type
  is it gets the compressed mode parametee. For DL it calls the appropriate
  function to send the physical channel and finger config command to mDSP.
  For UL it prepares the global variable that holds the compressed mode state
  of next frame and parameter for that.
  Finally it send the CM init mDSP command to mDSP to tell it that next frame
  is compressed a dn it also sets the compressed mode parameters.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_begin_cm(
  /* TGPSI active in next frame */
  uint8 tgpsi);

/*===========================================================================
FUNCTION l1_cm_query_cm_in_prog

DESCRIPTION
  This function is used to enquire if CM is in progress. CM is in progress
  at the moment a CM CFG is sent to mdspsync until the 2nd slot of the frame
  that CM has finished.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_query_cm_in_prog(void);

/*===========================================================================
FUNCTION l1_cm_exit_cm

DESCRIPTION
  This function configures the DL and UL in to normal mode. It sends the
  physical channel and finger config command to mDSP. For UL it prepares
  reset the CM state the global variable.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_exit_cm(void);

/*===========================================================================
FUNCTION l1_cm_get_last_effective_tgps_info

DESCRIPTION
  This function gets the last effective comprsesed mode information.
  
  If current pattern is active then in lookup order for information should be
  inactive pending followed by active pending followed by current OTHERWISE
  
  If current pattern is inactive then in lookup order for information should be
  active pending followed by inactive pending followed by current

DEPENDENCIES
  None

RETURN VALUE
  Pointer to information if available otherwise NULL

SIDE EFFECTS
  None
===========================================================================*/
extern l1_cm_tgp_seq_info_struct_type* l1_cm_get_last_effective_tgps_info(
  /* TGPSI of the pattern to search for last effcetive information */
  uint8 tgpsi);

/*===========================================================================
FUNCTION l1_cm_query_cm_ul

DESCRIPTION
  This function is used to enquire about the UL compressed mode. This is
  intended to be used by encoder module to enquire about the UL compressed
  mode at start of ENC_GET_DATA event. If this returns TRUE then encoder
  module should get the compressed mode parameters for UL and should use them
  to configure the encoder to compressed frame.
  This function should be called after the SEQ_CM_UPDATE_EVENT has occured.
    
DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE as per UL compressed mode in next frame.

SIDE EFFECTS
  None
==========================================================================*/
extern boolean l1_cm_query_cm_ul(
  /* CFN of frame for which to enquire CM about */
  uint8 cfn);

/*===========================================================================
FUNCTION l1_cm_ul_param_struct_type

DESCRIPTION
  This function returns the structure pointer that has UL compressed mode
  parameters. This function should be called to get the UL compressed mode
  parameters if the function l1_cm_query_cm_ul returns TRUE.
  This function should be called after the SEQ_CM_UPDATE_EVENT has occured.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1_cm_get_ul_params(
  /* CFN for which UL params are requested */
  uint8 cfn,
  /* pointer to UL CM parameter structure */
  l1_cm_ul_param_struct_type* cm_ul_params);

/*===========================================================================
FUNCTION l1_cm_get_stat_seq_info

DESCRIPTION
  This function returns the stat info and the CM TGP seq info structure
  pointers for the CFN requested. If frame is not occupied, then original
  pointer is returned.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_get_stat_seq_info(
  /* CFN for which info is requested */
  uint8 cfn,
  /* pointer to CM info structure */
  l1_cm_tgps_stat_struct_type **l1_cm_req_stat_info,
  /* pointer to CM TGP seq info structure */
  l1_cm_tgp_seq_info_struct_type **l1_cm_req_tgp_seq_info);

/*===========================================================================
FUNCTION l1_cm_query_cm_dl

DESCRIPTION
  This function is used to enquire about the DL compressed mode. This is
  intended to be used by demod module to enquire about the DL compressed
  mode if there is any reconfig of the finger or physical channel. If this
  returns TRUE then demod module should get the compressed mode parameters
  for DL and should apply them to reconfig the finger or physical channel.
  This function should be called after the SEQ_CM_UPDATE_EVENT has occured.
    
DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE as per DL compressed mode in next frame.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_query_cm_dl(
  /* CFN of frame for which to enquire CM about */
  uint8 cfn);

/*===========================================================================
FUNCTION l1_cm_freeze_cm_action

DESCRIPTION
  This function sets the CM action freeze pending flag and saves the CFN at
  which freeze should happen. Give at least 2 frame lead time before freeze
  CFN.
  If freeze CFN is given as MAX_CFN_COUNT or more. It freezes CFN immediately.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_freeze_cm_action(
  /* CFN at which compressed mode actions should be frozen */
  uint16 cfn);

/*===========================================================================
FUNCTION l1_cm_unfreeze_cm_action

DESCRIPTION
  This function sets the CM action unfreeze pending flag and saves the CFN at
  which unfreeze should happen.  If CFN is greater than or equal to MAX_CFN_COUNT
  we unfreeze immediately.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_unfreeze_cm_action(
  /* CFN at which compressed mode actions should be unfrozen */
  uint16 cfn);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*===========================================================================
FUNCTION l1_cm_freeze_cm_for_qta

DESCRIPTION
  This function is called when CM needs to be frozen because of QTA Start.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void l1_cm_freeze_cm_for_qta(void);

/*===========================================================================
FUNCTION l1_cm_unfreeze_cm_for_qta

DESCRIPTION
  This function handles CM Unfreeze action due to QTA STOP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void l1_cm_unfreeze_cm_for_qta(void);

#endif /* #ifdef FEATURE_QTA */
/*===========================================================================
FUNCTION l1_cm_freeze_cm_on_normal_frame

DESCRIPTION
  This function will find the next normal frame and set it to freeze. 
  If there is no CM active, it freezes right away.
  If it has CM active and cannot find any valid information, it will throw
  an error message and freeze right away.
  If CM is already frozen or has pending freeze, it will NOT do anything, only
  return FALSE.

  Currently used for ASU updates.
  
DEPENDENCIES
  After this is called, a l1_cm_unfreeze_cm_action() has to be called in
  the upcoming frames or CM will be stalled.

RETURN VALUE
  TRUE if freeze on normal frame is successful
  FALSE otherwise - only happens if CM is already frozen.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_freeze_cm_on_normal_frame(void);

/*===========================================================================
FUNCTION l1_cm_normal_frame_frozen

DESCRIPTION
  This function posts a local command to handle a freeze requested on
  normal frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_normal_frame_frozen(void);

/*===========================================================================
FUNCTION l1_cm_is_tgps_active

DESCRIPTION
  This function returns the TGPS activa eor inactive status.

DEPENDENCIES
  None

RETURN VALUE
  boolean based on TGPS active status

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_is_tgps_active(
  /* TGPSI of the CM pattern sequence */
  uint8 tgpsi);

/*===========================================================================
FUNCTION l1_cm_query_gsm_clk_turned_on

DESCRIPTION
  This function returns GSM clock turn ON by this module status.

DEPENDENCIES
  None

RETURN VALUE
  boolean based GSM cock turn ON state

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_query_gsm_clk_turned_on(void);

/*===========================================================================
FUNCTION l1_cm_query_action_freeze_pending

DESCRIPTION
  This function returns compressed mode freeze action freeze pending flag.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_query_action_freeze_pending(void);

/*===========================================================================
FUNCTION l1_cm_query_tgmp_for_gsm_meas

DESCRIPTION
  This function return TRUE is the TGMP is for GSM measurement. The TGMP
  for GSM measurements are:
       L1_CM_TGMP_GSM_RSSI_SCAN,
       L1_CM_TGMP_GSM_BSIC_IDENTIFY,
       L1_CM_TGMP_GSM_BSIC_RECONF

  
    
DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on TGMP

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_query_tgmp_for_gsm_meas(
  /* TG measurement purpose type */
  l1_cm_tg_meas_purpose_enum_type tgmp);

/*===========================================================================
FUNCTION l1_cm_query_bsic_reconf_abort_timer

DESCRIPTION
  This function returns the reconfirm abort timeout value for the BSIC 
  reconfirm gap pattern.

DEPENDENCIES
  None

RETURN VALUE
  The BSIC reconfirm abort timeout value.

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 l1_cm_query_bsic_reconf_abort_timer(void);


/*===========================================================================
FUNCTION l1_cm_query_delta_sir_applied_in_tti

DESCRIPTION
  This function scans the array of boolean that stores the delta SIR applied
  for CFN mod 16 frame number. If any frame in this TTI has delta SIR applied
  it returns TRUE otherwise FALSE.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_query_delta_sir_applied_in_tti(
  /* CFN of the frame at which TTI ends */
  uint8 tti_last_frame_cfn,
  /* length of TTI in frames */
  uint8 num_fr_in_tti);

/*===========================================================================
FUNCTION l1_cm_query_tgmp_active

DESCRIPTION
  This function checks if any TGPSI with TGMP is active.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_query_tgmp_active(
  /* TGMP to query */
  l1_cm_tg_meas_purpose_enum_type tgmp);

/*===========================================================================
FUNCTION l1_cm_query_gsm_tgmp_active

DESCRIPTION
  This function checks if any TGPSI with GSM TGMP is active.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_query_gsm_tgmp_active(void);

/*===========================================================================
FUNCTION l1_cm_reserve_for_intraf_validate

DESCRIPTION
  This function the req Id and its info for reserving the CM gaps for intra
  frequency purposes.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based validation result.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_reserve_for_intraf_validate(
  /* intra-freq GAP freq switch block req Id */
  l1_cm_gap_reserve_req_id_enum_type req_id,
  /* number of frames to reserve */
  uint8 num_frames);

/*===========================================================================
FUNCTION l1_cm_reserve_for_intraf_possible

DESCRIPTION
  This function blocks the frames from switching the frequency if there
  is a GAP with purpose INTRA_F measurement. Gap with purpose of GSM search
  is not blocked.
  
  CASE 1: In case of NO GAP in N + 1 AND N + 2
                                               
         N            N + 1          N + 2          N + 3       
  |--------------|--------------|--------------|--------------|
       ^
       Frame in which this function is called
       CM actions are taking place for this frame
                 ^
                 Always Skip next frame
                                ^(POINT_A)
                                Intra frequency search
                                is posible here
                                
                                 
  CASE 2: In case of GAP in N + 1 OR N + 2
  
         N            N + 1          N + 2          N + 3       
  |--------------|--------------|--------------|--------------|
       ^
       Frame in which this function is called
       CM actions are taking place for this frame
                 ^
                 CM actions/Meas are possibly sceduled for this frame
                                ^ 
                                GSM one frame look ahead skip
                                               ^(POINT_B)
                                               Intra frequency search
                                               is posible here
  
  Delta between CASE 1 and CASE 2 = (N + 3) - (N + 2) = 1
  Case 1 is applicable in case there is no GAP or GSM meas TGMP GAP
  for frames from POINT_A to POINT_B

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on all the required consecutive frames are blocked or not.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_reserve_for_intraf_possible(
  /* intra-freq GAP freq switch block req Id */
  l1_cm_gap_reserve_req_id_enum_type req_id,
  /* number of frames required for intra freq search */
  uint8 num_reserve_frames,
  /* start CFN from where the search can be done */
  uint8 *intraf_reserve_start_cfn);

/*===========================================================================
FUNCTION l1_cm_force_reserve_for_intraf

DESCRIPTION
  This function force on the blocking of frequency switch for number frame
  requseted .

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE. False can be returned if req Id has already got pending
  GAP that are reserved.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_force_reserve_for_intraf(
  /* intra-freq GAP freq switch block req Id */
  l1_cm_gap_reserve_req_id_enum_type req_id,
  /* number of frames required for intra freq search */
  uint8 num_reserve_frames,
  /* start CFN from where the search can be done */
  uint8 *intraf_reserve_start_cfn);

/*===========================================================================
FUNCTION l1_cm_query_req_id_reserved_for_intraf

DESCRIPTION
  This function returns the frame blocked/reserved status by this req Id.

DEPENDENCIES
w  None

RETURN VALUE
  TRUE or FALSE based on GAP reserve status by this req Id.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_query_req_id_reserved_for_intraf(
  /* intra-freq GAP freq switch block req Id */
  l1_cm_gap_reserve_req_id_enum_type req_id,
  /* frame CFN at which to check for reserve */
  uint8 cfn);

/*===========================================================================
FUNCTION l1_cm_unreserve_for_intraf

DESCRIPTION
  This function unreserve the frames blocked by this req Id. If any other
  req Id has already blocked or reserved this frame then only this Id is
  marked to release that frame but frame still remains reserved. Otherwise
  this frame is unreserved a can be used by regular CM activities.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE. False can be returned if req Id is not valid.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_unreserve_for_intraf(
  /* intra-freq GAP freq switch block req Id */
  l1_cm_gap_reserve_req_id_enum_type req_id);

/*===========================================================================
FUNCTION l1_cm_suspend_cleanup

DESCRIPTION
  This function is called to cleanup the compressed mode at suspend.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_suspend_cleanup(void);

/*===========================================================================
FUNCTION l1_cm_resume_init

DESCRIPTION
  This function is called to init the compressed mode at resume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_resume_init(void);

/*===========================================================================
FUNCTION l1_cm_suspend_cm

DESCRIPTION
  This function is called in task context to suspend the comprsesed mode.
  It set a flag for CM suspend received that is checked at next CM state update
  event to suspend the compressed mode state machine update.
  If suspend CFN is given as MAX_CFN_COUNT or more. It suspend immediately.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if already suspended, TRUE otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_suspend_cm(
  /* CFN at which compressed mode actions should be suspended */
  uint16 suspend_cfn,
  /* call back function to be called after suspend is done */
  L1_CM_SUSPEND_DONE_CB_FUNC *cb_func,
  /* indicates that CM manager should save the pending info */
  boolean save_pending_info);

/*===========================================================================
FUNCTION l1_cm_resume_cm

DESCRIPTION
  This function is called in task context to resume the comprsesed mode.
  It set a flag for CM resume received that is checked at next CM state update
  event to resume the compressed mode state machine update.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_resume_cm(
  /* indicate DEACT all TGPS at resume */
  boolean deact_all_tgps);

/*===========================================================================
FUNCTION l1_cm_syncup_cm_state_at_resume

DESCRIPTION
  This function sync up the CM TGP seq state from suspended CFN to current
  CFN for number of suspended frames. It calculates the advanced information
  from the current frame CFN. It also take into account any pending ACT
  or DEACT existing during the suspended state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_syncup_cm_state_at_resume(void);

/*===========================================================================
FUNCTION l1_cm_find_frame_to_cleanup

DESCRIPTION
  This function looks for a normal frame in CM and sends a suspend or freeze
  to CM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if able to perform action
  ELSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_find_frame_to_cleanup(uint16 *frame_to_cleanup, 
                                           boolean save_old_cm_config);


/*===========================================================================
FUNCTION l1_cm_set_rssi_allowed_cnt

DESCRIPTION
  This function sets the variable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1_cm_set_rssi_allowed_cnt(boolean reset);

/*===========================================================================
FUNCTION l1_cm_query_rssi_for_intraf

DESCRIPTION
  This function tells if this gap can be given to intra frequency.

DEPENDENCIES
  None

RETURN VALUE
  True if we can give away
  False if we must use it for RSSI

SIDE EFFECTS
  None
===========================================================================*/
boolean l1_cm_query_rssi_for_intraf(void);

/*===========================================================================
FUNCTION l1_cm_get_active_tgpsi_info

DESCRIPTION
  This function queries for active tgpsi

DEPENDENCIES
  None

RETURN VALUE
  sNone

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_get_active_tgpsi_info(l1_cm_active_tgpsi_info_struct_type* tgpsi_info);

#ifndef FEATURE_CM_UNIT_TEST
/*===========================================================================
FUNCTION        L1_CM_CM_INFO_PRESENT

DESCRIPTION     This function gets teh compressed mode information from
                CPHY_SETUP_REQ external command. It checks for the type of DPCH
                req (add or reconfig) and checks the appropriate physical channel
                database. If compressed mode information is present in DL common
                info it saves the information pointer to l1_phychan_db_cm_info
                global variable and returns TRUEotherwise returns FALSE.
                
                If there is no add/cfg req or its not for DPCHphysical channel
                this function returns FALSE.

DEPENDENCIES    None

RETURN VALUE    TRUE or FALSE

SIDE EFFECTS    l1_phychan_db_cm_info is updated with compressed mode info
                structure pointer if present otherwise it is set to NULL.
===========================================================================*/
extern boolean l1_cm_is_cm_info_present(
  /* command parameters struct pointer */
  l1_ext_cmd_type *ext_cmd);

/*===========================================================================
FUNCTION        L1_CM_INFO_UPDATE_CM_ACTIVATION_TIME

DESCRIPTION     This function updates the CPHY_SETUP_REQ activation time in
                compressed mode information as action tine for activate and
                deactivate patterns.

DEPENDENCIES    Uses the variable l1_phychan_db_cm_info set in function call
                l1_cm_is_cm_info_present

RETURN VALUE    Adjusted activation time in case missed flag is set otherwise
                return thr original activation time.

SIDE EFFECTS    Updates variable dl_cm_activation_time_invalid. This is used
                in actual compressed mode validation.
===========================================================================*/
extern uint8 l1_cm_info_update_cm_activation_time(
  /* Activation time type */
  l1_act_time_enum_type activation_time_type,
  /* Activation time */
  uint8 activation_time,
  /* Whether or not the activation_time above was missed */
  boolean missed_activation_time);

/*===========================================================================
FUNCTION        L1_CM_VALIDATE_CM_METHOD

DESCRIPTION     This function verifies that compressed mode can be activated
                with current physical channel and CCTrCh config. Folloing
                configuration is not allowed
                1. DL physical channsl configuration SF4 for CM by SF reduction
                2. CCrCh info DTX method FIXED_POS for CM by HLS

DEPENDENCIES    Uses the variable l1_phychan_db_cm_info and l1_phychan_db_cm_info
                set in function call l1_cm_is_cm_info_present

RETURN VALUE    None

SIDE EFFECTS    Updates variable l1_cm_method_invalid. This is used in actual
                compressed mode validation.
===========================================================================*/
extern void l1_cm_validate_cm_method(
  /* Pointer to external command with cphy setup request */
  l1_setup_cmd_type* cmd);

/*===========================================================================
FUNCTION        L1_CM_SEND_CM_INFO

DESCRIPTION     This function send compressed mode information to l1cmmeas
                module for further send to l1cmmgr module. This is used when
                L1 is laredy in DCH state and compressed mode may be activated
                at activation tine it self. For that reason info ned to send in
                advance.

DEPENDENCIES    Uses l1_phychan_db_cm_info and l1_cm_info_present set in
                earlier function calls.

RETURN VALUE    None

SIDE EFFECTS    l1_cm_info_present is set to FALSE
===========================================================================*/
extern void l1_cm_send_cm_info(void);


/*===========================================================================
FUNCTION          l1_cm_phychan_cm_config

DESCRIPTION       This function does tgps config to CM MGR.

DEPENDENCIES      None

RETURN VALUE      None.

SIDE EFFECTS      None
===========================================================================*/
extern void l1_cm_phychan_cm_config(void);

/*===========================================================================
FUNCTION        DL_INTRAF_CM_RESERVE

DESCRIPTION     This function reserves or unreserves compressed-mode gaps for
                intra-frequency depending on whether the supplied argument is
                TRUE or FALSE respectively.

DEPENDENCIES

RETURN VALUE    None

SIDE EFFECTS    Compressed-mode gaps get reserved or unreserved.
===========================================================================*/
extern boolean l1_cm_intraf_cm_reserve (boolean reserve);

/*===========================================================================
FUNCTION          l1_cm_validate_cm_params

DESCRIPTION       This function validates CM parameters for dpch.

DEPENDENCIES      None

RETURN VALUE      None.

SIDE EFFECTS      None
===========================================================================*/
extern boolean l1_cm_validate_cm_params(uint16 opt_parm_presence_bitmask, l1_dl_common_info_struct_type *dl_common);



#endif /* FEATURE_CM_UNIT_TEST */

/*===========================================================================
FUNCTION l1_cm_client_off_done

DESCRIPTION
  This function speeds up the transition between exclusive client groups when 
  one goes off.  Though not necessary, for speed purposes this function should 
  ideally be called anytime a client in an exclusive set turns OFF 
  (potentially allowing the other group in the excl set to gain control).

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void l1_cm_client_off_done(l1_cm_act_deact_client_enum_type client);

/*===========================================================================
FUNCTION l1_cm_register_client_for_act_deact_notif

DESCRIPTION
  This function registers a client for receiving CM pattern active/deactive 
  notifications.  It can also be used to de-register any previous such 
  registrations.

  Each client has a hard-coded dependency bitmask which determines which tgmps 
  going active can trigger a CM-ACTIVE notification to the client.  This 
  dependency bitmask is specified in each client's static info.

  Registration is done by giving this function valid callback functions.  

  Here's how they are used:  
  ==== the activation/deactivation callback function ====
    Anytime l1cmmgr goes from no client-dependent CM patterns active to at 
    least one client-dependent CM pattern active, it will call this function in 
    interrupt context with L1_CM_ACTIVE as the argument. For clients with 
    CM-blocking services, this will let them know they must turn off those
    services in order for CM to proceed.

    Anytime l1cmmgr goes from at least one client-dependent CM pattern active 
    to no client-dependent CM patterns active, it will call this function in 
    interrupt context with L1_CM_DEACTIVE as the argument. For clients with 
    an on/off status, this will indicate to the client when it's ok to turn
    those services back on again (if so desired).

    In order to not turn ON after cm indicates a client should be off, special
    care must be taken for clients transfering control to task context from 
    these callbacks.  In this example, we consider a client that turns on when 
    CM is off.  The steps are sequential in time.

    1) (intrpt context) 
      a) l1cmmgr sends L1_CM_DEACTIVE because of an initial CM state change 
      b) client posts local command to go active in response to 1a 
    2) (intrpt context) 
      a) l1cmmgr sends L1_CM_ACTIVE because of another sudden CM state change
      b) client posts local command to go deactive in response to 2a
    3) (task context) 
      a) client's local command to go active gets serviced by task
         - This local command must not let the client go active, since the most
           recently received cm status was L1_CM_ACTIVE.
    4) (task context) 
      a) client's local command to go deactive gets serviced by task
         - If the rules of step 3a are abided, this local command will end up
           doing nothing, since the client will already be deactive.

    Because of this, it's best for clients to keep a global variable indicating 
    the cm state (rather than pass cm state through the local command itself).

  ==== the client status function ====
    This is a function l1cmmgr uses to query whether a client is on or off.  It
    is required for any clients that block CM, or are part of an exclusion set 
    (as defined in the client's static info in this file).  This registered 
    function, when called, should return true if the client is IN THE PROCESS
    OF TURNING ON, or is ALREADY ON (aka anything but steady-state off). 
    Otherwise it should return false.  If the client does not have a status, 
    this field may be left NULL. 

  Unregistering is done by passing both callback functions the value of NULL.


DEPENDENCIES
  WE ASSUME CLIENT IS OFF BEFORE IT REGISTERS

RETURN VALUE
  TRUE for successful registration/de-registration.
  FALSE for unsuccesful
 
SIDE EFFECTS
  Lots.  Read function description.  Updates l1_cm_client_info[] for the 
  specified client, which causes the notification back-end to trigger the rest 
  of the stuff mentioned.
===========================================================================*/
boolean l1_cm_register_client_for_act_deact_notif( 
  /* the client to register/de-register */
  l1_cm_act_deact_client_enum_type client,
  /* the activation/deactivation callback function */
  l1_cm_act_deact_notif_func_type *cm_act_deact_notif_cb_ptr,
  /* the client status callback function */
  l1_cm_client_status_func_type *client_status_cb_ptr);

/*===========================================================================
FUNCTION l1_cm_register_cme_cb_func
         l1_cm_deregister_cme_cb_func

DESCRIPTION
  These functions register and deregister the CME callback function for CM
  activate and deactivate and a calback to check the status of CME.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_register_cme_cb_func(
  /* Call back function pointer */
  L1_CM_CME_CB_FUNC_TYPE *func_ptr,
  /* Call back function for CME status check */
  L1_CM_CME_STATUS_CB_FUNC_TYPE *status_func_ptr);

extern void l1_cm_deregister_cme_cb_func(void);

/*===========================================================================
FUNCTION l1_cm_cme_handler

DESCRIPTION
  This function handles the local command put by l1cmmgr to call CME callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_cm_cme_handler(void);

/*===========================================================================
FUNCTION l1_cm_request_nfi

DESCRIPTION
  This function lets interested parties register a NFI (normal frame 
  indication) cb function that gets called inside the start of the next normal
  frame (aka non-CM frame).

  If the current frame is normal, we indicate so immediately.

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_request_nfi(
  /* the client id calling this function */
  l1_cm_nfi_client_enum_type nfi_client_idx, 
  /* cb function called in interrupt context when in the next normal frame */
  l1_cm_nfi_cb_type *cb);


/*===========================================================================
FUNCTION l1_cm_cleanup_req

DESCRIPTION
  This function, when called by external modules, will cause l1cmmgr to freeze
  further CM actions and notify the caller when all previously configured
  compressed frames are completed.  This is done via a callback, which supports
  a single void * argument that can be used however the caller of this function
  wishes.

DEPENDENCIES
  Must be called in Task context.

RETURN VALUE
  TRUE - if cleanup request succesfully scheduled
  FALSE - otherwise (probably because cleanup already in progress)

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_cleanup_req(
  /* the callback l1cmmgr should call when it's done cleaning up */
  l1_cm_cleanup_done_cb_type *cleanup_done_cb,
  /* optional pointer that will be passed back in the cleanup cb */
  void *cb_ptr_arg,
  /* whether or not to save the old cm config if in a suspend scenario */
  boolean save_old_cm_config);

/*===========================================================================
FUNCTION l1_cm_finish_cleanup

DESCRIPTION
  TODO

DEPENDENCIES
  Caller must not intlock around calls to this function.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void l1_cm_finish_cleanup(void);

/*===========================================================================
FUNCTION l1_cm_duration_cm

DESCRIPTION
  This function calculates the compressed mode duration for MCVS.
  
DEPENDENCIES
  Called every 180 or 360ms from Searcher module.

RETURN VALUE
  Dcm = Compressed Mode Duration
 
SIDE EFFECTS
  None
===========================================================================*/
extern uint32 l1_cm_duration_cm(void);

/*===========================================================================
FUNCTION l1_cm_parallel_step1_overhead

DESCRIPTION
  This function calculates the overhead for parallel step1 search due to
  inter-frequency search in compressed mode.
  
DEPENDENCIES

RETURN VALUE
  OHps1
 
SIDE EFFECTS
  None
===========================================================================*/
extern uint32 l1_cm_parallel_step1_overhead(uint8 num_interf_carrier);

/*===========================================================================
FUNCTION  L1_CM_SET_SKIP_CM_MEAS_THROUGH_QXDM

DESCRIPTION
  This function sets the L1CMMGR variable "skip_cm_measurements"
  through diag command. This function is a part of the callback function for
  diag command dispatch table.

DEPENDENCIES
  This function is called when "send_data 75 4 55 0 <val>" is issued from cmd
  line interface from QXDM

RETURN VALUE
  None

SIDE EFFECTS
  The L1CMMGR variable "skip_cm_measurements" can be enabled or disabled 
  based on the val received through QXDM.
===========================================================================*/
extern void l1_cm_set_skip_cm_meas_through_qxdm(uint8 val);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION  wl1_irat_dump_cb

DESCRIPTION
  This function is to collect the irat meas info for debugging. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_irat_dump_cb(void* addr_ptr);
#endif /* FEATURE_QSH_DUMP */

/*===========================================================================
FUNCTION l1_cm_find_next_cm_gap

DESCRIPTION
  This function returns the occurance of the next CM gap.

DEPENDENCIES
 NONE

RETURN VALUE
 Next instance of CM gap params

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_cm_find_next_cm_gap (uint8 *cm_gap_cfn, uint8 *cm_gap_start_slot);
#endif /* L1CMMGR_H */
