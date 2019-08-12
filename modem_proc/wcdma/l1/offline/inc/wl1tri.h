#ifndef WL1TRI_H
#define WL1TRI_H

/*===========================================================================
                           WL1TRI . H

GENERAL DESCRIPTION
  This file contains variables and functions to support new triage.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2014 by Qualcomm Technologies, Inc. All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1tri.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who    what, where, why
--------    ---    -------------------------------------------------------- 
10/07/14    mit    API to check if any fing has aset service in fing_ena_mask
09/11/14    sr     Changes not to issue skipped triage as part of EUL/HS chan cfg
07/03/14    sks    Changes for FCDC
03/17/14    mit    Changes to accommodate dynamic cell_db_idx allocation
03/25/14    srk    API to check if a finger has HS service enabled.
05/07/14    sm     API to find RSSI Sum for hs celld in locka given antenna 
                   id and carrier
04/16/14    bj     Remove duplicate code in split ACQ code
02/28/14    sad    SCCH order optimization check in
02/05/14    pr     Added support for DCHSUPA Finger Switching Ops.
01/27/14    sr     Changes for NV 70188 enhancement
12/16/13    as     Porting G2W TA from Triton to Dime.
10/17/13    bs     Preliminary 3C Support
09/12/13    jd     Searcher redesign
07/24/13    jd     Changes for Dual band Dual carrier
07/23/13    vs     Added support for the antenna switch diversity feature.
05/28/11   kcm     Changes to function declaration dl_tri_get_locked_fing_info
05/21/13    sad    Fixed incorrect macro definition
03/20/13    dm     Account for fingers currently being deassigned also when 
                   doing VPE update.
03/08/13    dm     MCVS feature enhancement
02/22/13    dm     R99 SCHIC changes with MVCS.
10/22/12    kcm    Reducing F3 prints in Idle Mode scenarios
11/28/12    dm     MCVS feature related changes 
12/28/12   sr      Exclude the cell under configuration from triage operation.
09/23/12    pv     Fixed compiler error.
09/23/12    pv     Added DRX PICH log pkt support.
08/29/12    pv     Remove unused APIs and added externs.
07/17/12    pv     Warning fixes.
11/28/12   gsk     Fix for compiler warnings
11/27/12   pr      Fixed compiler warnings.
11/19/12   pr      Changed number of fingers to match FW.
11/12/12   dm      Changes to register a callback with triage if any module 
                   wants to do finger related processing while triage is running.
10/17/12   kcm     Changing existing triage API to signal flushing of 
                   prepared RxD configuration
08/17/12    sr     NV item addition for Max number of fings
07/24/12    geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/11/12    pv     Re-checkin for resolving PW issues.
07/05/12    pv     Parallel reacq changes - new APIs and data structures.
05/15/12    kcm    Updated the API dl_tri_get_num_locked_fings to get num 
                   locked fingers per rx chain
04/06/12    abs    Added an API to update the ref fing pos
03/22/12    vs     Change in the finger algorithm for sidelobes.
03/07/12    vsr    Fixed merge error
02/29/12    vsr    QICE Power Management (QPM) Support
02/10/12    rgn    Nikel feature cleanup
02/01/12    gnk    Nikel Feature cleanup.
01/30/12    vs     Feature cleanup.
11/18/11    kcm    Removed the debug code added to catch the finger stuck 
                   issue.
11/15/11    hk     Added support to change combiner lock thresholds based on
                   finger combined RSSI
10/17/11    kcm    Added debug code to catch the finger stuck issue.
10/11/11    ks     Deassign Sidelobe finger if they meet the criteria for
                   N number of cycles.
09/14/11    kcm    Removed the unwanted macros
07/19/11    hk     Added a new API to fetch the earliest finger in lock state
07/06/11    hk     Added a new API to fetch the strongest finger in lock state
04/28/11    sj      upmerge from offline/main
04/25/11    geg    Replaced *.h with *_v.h
04/22/11    ks     Added prototype for dltri_update_fings_tpc
01/13/11    sv     Added support for QICE ASET cell registration
12/13/10    vsr    Kickoff triage operation immediately after clearing delay flag
12/08/10    vsr    Added support for Rx chain swap
10/22/10    ks     Changed DL_TRI_MAX_PATHS_DC macro
10/19/10    sv     Added changes to support full search for DC and QICE support.
10/08/10    vb     changes to send one demod cfg cmd during pich wake up
05/03/10    rvs    Added cell db idx param to dl_tri_get_num_locked_fings().
03/30/10    rvs    Remove FEATURE_WCDMA_CME_CONTROLLER.
03/05/10    hk     Corrected the secondary carrier max finger index
02/26/10    rvs    Fixes for Mustang.
02/27/10    hk     Added new macros for number of fingers on primary and 
                   secondary carriers
02/17/10    rgn    replaced fing_cnt with fing_enabled_mash
02/10/10    ms     Moved carrier id info to l1def.h
02/10/10    hk     Featurized the Dual carrier specific code
02/09/10    hk     Added max_paths to the carrier specific info structure
02/02/10    hk     Cleaned-up the test code
01/22/09    rgn    Increased the number of fingers for both carriers
01/13/09    rgn    Increased the number of fingers for both carriers 
11/13/09    hk     Changed the max_usable_fing_idx for both carriers
09/24/09    rvs    Added dl_tri_get_locked_fing_info().
09/09/09    rvs    Added dl_tri_get_num_locked_fings().
07/15/09    rgn    Enhanced existing s-cpich support
07/16/09    hk     Reorganized the code to remove INC/SRC violations
07/07/09    sv     Added externs for S-CPICH support.
06/03/09    ks     Changes related to mob requirements, moved some structs
                   from wl1trii.h here. Removed inclusion of wl1trii.h
05/27/09    stk    Fixed Compiler Warning
04/14/09    ks     Added rxd call back function prototype. Also added missing 
                   function prototypes to supress compiler warnings.
04/02/09    rgn    Added wrapper to get assigned fing bmsk
02/01/09    ks     Changed prototype of dltri_get_rl_ref_fing
12/17/08    ks     Mainlining FEATURE_WCDMA_OFFLINE_SEARCHER and
                   FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
12/10/08    ks     Mainlining FEATURE_WCDMA_DL_ENHANCED
12/03/08    rgn    Added support for Tx Diversity detection procedure 
10/03/08    hk     Bringing in 7k Mailine fixes
09/02/2008  ks     feature FEATURE_OFFLINE_SRCH changed to 
                   FEATURE_WCDMA_OFFLINE_SEARCHER
07/02/2008  ks     first cut of triage with new design

===========================================================================*/


/*--------------------------------------------------------------------------
                      Included Header Files
--------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "dltriparm.h"
#include "msm.h"
#include "srchset.h"

#include "timetick.h"

#include "rxdiv_v.h"
#include "edlcfg.h"
#include "hscfg.h"

#define DL_TRI_MAX_PATHS \
  ((SRCH_ASET_MAX_C0 * 6) + DL_MAX_NUM_FINGERS_PRI_CARRIER)

#ifdef FEATURE_WCDMA_DC_HSDPA
#define DL_TRI_MAX_PATHS_DC \
  ((SRCH_DC_TRIAGE_SET_MAX * 6) + DL_MAX_NUM_FINGERS_SEC_CARRIER)

#ifdef FEATURE_WCDMA_3C_HSDPA
#define DL_TRI_MAX_PATHS_3C \
  ((SRCH_3C_TRIAGE_SET_MAX * 6) + DL_MAX_NUM_FINGERS_3RD_CARRIER)
#endif /* FEATURE_WCDMA_3C_HSDPA */
#endif /* FEATURE_WCDMA_DC_HSDPA */

/* the maximum number of possible radio link sets, this is just the max
   number of cells in the active set */
#define DL_TRI_MAX_RL_SETS SRCH_ASET_MAX_C0


#define DL_TRI_PRI_CARR_MIN_FING_IDX (0)
#define DL_TRI_PRI_CARR_MAX_FING_IDX ((DL_MAX_NUM_FINGERS_PRI_CARRIER) - 1)

#define DL_TRI_PRI_CARR_MAX_FINGS \
  ((1) + DL_TRI_PRI_CARR_MAX_FING_IDX - DL_TRI_PRI_CARR_MIN_FING_IDX)

#ifdef FEATURE_WCDMA_DC_HSDPA
#define DL_TRI_SEC_CARR_MIN_FING_IDX (DL_MAX_NUM_FINGERS_PRI_CARRIER)
#define DL_TRI_SEC_CARR_MAX_FING_IDX ((DL_MAX_NUM_FINGERS_PRI_CARRIER + DL_MAX_NUM_FINGERS_SEC_CARRIER) - 1)

#define DL_TRI_SEC_CARR_MAX_FINGS \
  ((1) + DL_TRI_SEC_CARR_MAX_FING_IDX - DL_TRI_SEC_CARR_MIN_FING_IDX)

#ifdef FEATURE_WCDMA_3C_HSDPA
#define DL_TRI_3RD_CARR_MIN_FING_IDX (DL_MAX_NUM_FINGERS_PRI_CARRIER + DL_MAX_NUM_FINGERS_SEC_CARRIER)
#define DL_TRI_3RD_CARR_MAX_FING_IDX ((DL_MAX_NUM_FINGERS_PRI_CARRIER + DL_MAX_NUM_FINGERS_SEC_CARRIER + DL_MAX_NUM_FINGERS_3RD_CARRIER) - 1)

#define DL_TRI_3RD_CARR_MAX_FINGS \
  ((1) + DL_TRI_3RD_CARR_MAX_FING_IDX - DL_TRI_3RD_CARR_MIN_FING_IDX)
#endif /* FEATURE_WCDMA_3C_HSDPA */
#endif /* FEATURE_WCDMA_DC_HSDPA */

#define DL_TRI_PRI_CARR_MAX_USABLE_FING_IDX (DL_TRI_PRI_CARR_MAX_FING_IDX)
#define DL_TRI_SEC_CARR_MAX_USABLE_FING_IDX (DL_TRI_SEC_CARR_MAX_FING_IDX)
#ifdef FEATURE_WCDMA_3C_HSDPA
#define DL_TRI_3RD_CARR_MAX_USABLE_FING_IDX (DL_TRI_3RD_CARR_MAX_FING_IDX)
#endif /* FEATURE_WCDMA_3C_HSDPA */

#define FING_TPC_IDX(fing) \
  (demod_fing_status_dbase[(fing)].tpc)

#define FING_CELL_DB_IDX(fing) (demod_fing_status_dbase[(fing)].cell_db_idx)

/* to check if HS service enable on the cell */
#define DLTRI_IS_HS_SVC_ENABLE(cell_db_idx) ((dltri_rl_info[cell_db_idx].assoc_services & DL_TRI_HSDPA_SERVICE) ? (TRUE) : (FALSE))

/* to check if HS service enable on the cell */
#define DLTRI_IS_QICE_SVC_ENABLE(cell_db_idx) ((dltri_rl_info[cell_db_idx].assoc_services & (DL_TRI_QICE_ASET_SERVICE | DL_TRI_QICE_NBR_SERVICE)) ? (TRUE) : (FALSE))


/* Services that could be using the per RL feature */
#define DL_TRI_HSDPA_SERVICE        0x01     /* HSDPA is active on this RL */
#define DL_TRI_HSUPA_SERVICE        0x02     /* HSUPA is active on this RL */
#define DL_TRI_R99_ASET_SERVICE     0x04    /* R99 channel active on this RL */
#define DL_TRI_R99_NBCH_SERVICE     0x08    /* R99 NBCH active on this RL */
#define DL_TRI_MBMS_SERVICE         0x10    /* MBMS channel active on this RL */
#define DL_TRI_STTD_DET_SERVICE     0x20    /* sttd detection enabled on this RL */
#define DL_TRI_QICE_NBR_SERVICE     0x40    /* QICE neighbor enabled on this RL */
#define DL_TRI_QICE_ASET_SERVICE    0x80    /* QICE ASET enabled on this RL */

#define DL_TRI_ALL_SERVICES         DL_TRI_HSDPA_SERVICE|DL_TRI_HSUPA_SERVICE|DL_TRI_R99_ASET_SERVICE| \
                                    DL_TRI_R99_NBCH_SERVICE|DL_TRI_MBMS_SERVICE|DL_TRI_STTD_DET_SERVICE| \
                                    DL_TRI_QICE_NBR_SERVICE|DL_TRI_QICE_ASET_SERVICE

/* Define module events for other modules to pass to triage 
   These will be used as a bitmask to set/clear the corresponding module's bit
   in the finger manager status word */
#define DL_TRI_FING_MGR_PICH_BT_ENA    0x01
#define DL_TRI_FING_MGR_RXD_ENA            0x02
#define DL_TRI_FING_MGR_ALL                     0xFF
#define DL_MAX_RL     L1_ASET_MAX

/* Side lobe finger max count after which the finger will be deassigned */
#define  DL_TRI_SIDELOBE_DEASSIGN_CNT  5
#define  DL_TRI_SIDELOBE_DEASSIGN_CNT_GOOD_CONDS  1

/*Switch Finger Macros for DC HSUPA*/
#define DL_TRI_SEC_CARR_ADD_FING1 10
#define DL_TRI_SEC_CARR_ADD_FING2 11

#define DL_TRI_PRI_CARR_MAX_SWITCH_FING_IDX 9
#define DL_TRI_PRI_CARR_MAX_SWITCH_FINGS 10
#define DL_TRI_PRI_CARR_MAX_SWITCH_USABLE_FING_IDX (DL_TRI_PRI_CARR_MAX_SWITCH_FINGS - 1)

#define DL_TRI_SEC_CARR_MIN_SWITCH_FING_IDX 10
#define DL_TRI_SEC_CARR_MAX_SWITCH_FINGS 8
/*Switch Finger Macros for DC HSUPA*/

/* Structure to keep a running average of the frequency error of the strongest
** finger roughly every 40 msec (triage rate).  GPS uses this average to aid
** in GPS fixes by compensating for the frequency error. */
typedef struct
{
  int32          accum;
  uint16         num_samples;
  timetick_type  begin_sample_time;
  timetick_type  end_sample_time;
} dl_tri_freq_error_struct_type;

/* Maximum number of samples ever contained in the above sum.  Sampling at
** 40 msec (triage rate), this is 2 seconds worth of data. */
#define DL_TRI_MAX_FREQ_ERROR_SAMPLES  50

typedef enum
{
  /*primary cpich as phase ref*/
  DL_FING_PHASE_REF_P_CPICH = WFW_DEMOD_PHCH_PHASE_REF_PRIMARY,
  /*secondary cpich as phase ref for r99*/
  DL_FING_PHASE_REF_S_CPICH = WFW_DEMOD_PHCH_PHASE_REF_SEC,
  /*secondary cpich as phase ref for mimo*/
  DL_FING_PHASE_REF_S_CPICH_MIMO = WFW_DEMOD_PHCH_PHASE_REF_MIMO_SCPICH
} dl_tri_ref_fing_phase_ind;

typedef enum
{
    /*Triage in Parallel Reacq mode*/
  DL_TRIAGE_PARALLEL_REACQ,
  /*Triage in Sequential Reacq mode*/
  DL_TRIAGE_SEQUENTIAL_REACQ,
 /*Triage in Sequential Reacq mode for PICH*/
  DL_TRIAGE_SEQUENTIAL_REACQ_FOR_PICH,
  /*Triage in Regular mode*/
  DL_TRIAGE_REGULAR_MODE,
  /*Max num of modes*/
  DL_TRIAGE_MAX_NUM_SVC_MODES
} dl_tri_svc_mode_enum_type;

typedef enum 
{
  DL_TRI_PATH_FOR_PULLIN,
  DL_TRI_PATH_FOR_TRIAGE
}dl_tri_path_purpose;

typedef enum
{
  DL_TRI_SWITCH_FINGERS_OFF,
  DL_TRI_SWITCH_FINGERS_ON
}dl_tri_switch_fing_for_dcupa_type;

/******************************************
    POOL TYPE
*******************************************/
/* enum type for pools */
typedef enum {
  /* Idle pool */
  DL_FING_IDLE_POOL,
  /* MFD pool */  
  DL_FING_MFD_POOL,
  /* Dip pool */ 
  DL_FING_DIP_POOL,
  /* MFA pool */  
  DL_FING_MFA_POOL,
  /* ASSN pool */
  DL_FING_ASSN_POOL,
  /* Ool pool */
  DL_FING_OOL_POOL,
  /* reserved pool */
  DL_FING_RSVD_POOL,

  DL_FING_NUM_POOL
} dl_tri_fing_pool_mgr_enum_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: DLTRI_RL_SET_STRUCT_TYPE

This structure contains information on radio link sets.  A radio link set
consists of one or more radio links each transmitting the same TPC code.
Power control commands received from the radio links in a radio link set
are soft combined at the UE before making the power control decision.

This is directly analogous to IS-95/2000 where sectors (radio links or cells
in WCDMA) transmitting the same Power Combiner Index (TPC in WCDMA) make up
a single cell (radio link set in WCDMA).
--------------------------------------------------------------------------*/
typedef struct
{
  /* how many fingers are currently assigned to this RL set */
  uint16 fing_enabled_mask;

  /* Weighted NtIo filter sum */
  uint32 weighted_ntio_filt_sum;

  /* IIR filed Weighted NtIo filter sum */
  uint32 filted_ntio_sum;

  /* Up TPC threshold */
  int32 up_tpc_thresh;

  /* Down TPC threshold */
  int32 dn_tpc_thresh;
} dl_tri_rl_set_struct_type;

/* RxD callback, will be called once cnf comes for all the fingers whose config went due to RxD OFF operations  */
typedef void DL_TRI_RXD_TRANS_IND_CB_FUNC_TYPE (boolean tri_rxd_trans_success);

/*--------------------------------------------------------------------------
                   STRUCTURE: DL_PATH_STRUCT_TYPE

This struture contains the information needed to describe a coherent energy
path.
--------------------------------------------------------------------------*/
typedef struct
{
  /* the cell index that corresponds to the cell */
  uint8   cell_db_idx;

  /* which finger, if any, is currently assigned to this path */
  uint8   fing;

  /* the energy of the path */
  uint32  eng;

  /* factor used to bias finger allocation toward this path */
  uint8   bias_factor;

  /* the PN position of the path in chipX8 */
  uint32  pos;

  /* TRUE if this path is a virtual path created from a finger */
  boolean virtual_path;

  /* the cell that is transmitting this path */
  srch_cell_struct_type *cell;

  /* To identify the antenna type of the path: PRIM/DIV*/
  uint8 antenna;
} dl_tri_path_struct_type;

typedef void DL_TRI_FING_PHCH_UPDATE_CB_FUNC_TYPE(
  uint8 cell_db_idx,
  uint8 fing_index,
  boolean is_assigned);

typedef void DL_TRI_REF_FING_CHANGE_UPDATE_CB_FUNC_TYPE(
  uint8 cell_db_idx,
  uint8 old_fing_index,
  uint8 new_fing_index);


/* The structure used to hold the HSDPA function pointers for 
   per RL callbacks */
typedef struct
{
  /* finger asigned or deassigned to this PSC update CB function */
  DL_TRI_FING_PHCH_UPDATE_CB_FUNC_TYPE *fing_phch_update_cb;
  /* Change in reference finger */
  DL_TRI_REF_FING_CHANGE_UPDATE_CB_FUNC_TYPE *ref_fing_change_cb;
} dltri_fing_update_cb_struct_type;


typedef struct {
  dltri_fing_update_cb_struct_type r99_update_cb;
  dltri_fing_update_cb_struct_type hs_update_cb;
  dltri_fing_update_cb_struct_type eul_update_cb;
  } dltri_per_rl_cb_struct_type;


typedef struct
{
  /* indicate per PSC triage active with this information */
  boolean enabled;
  /* PSC of cell */
  uint16 psc;
  /* reference finger */
  uint8  ref_fing;
  /* what are the services associated with this RL */
  uint8 assoc_services;
  /* enabled fingers bitmask */
  uint32 fing_ena_bmsk;
  /* fingers that are in deassign in progress for this RL */
  uint32 fing_deassign_in_prog_bmsk;
  /* Pointer to the cell structure of the cell */
  srch_cell_struct_type *cell;

  /* information used internally by triage 
     *************************************
   */
  /* reference finger chosen in triage processing */
  uint8 ref_fing_chosen;
  /* early finger pos diff */
  uint32 ear_pos_diff;
  /* strongest finger in finger set */
  uint8 strongest_assigned_fing;
  /* path bias weights */
  uint8 path_bias_factor[SRCH_MAX_PILOT_PEAKS];
  /* function table for finger update events */
  dltri_per_rl_cb_struct_type fing_update_table;

  dl_tri_carr_id_enum_type carr_idx;
} dl_tri_per_rl_triage_info_struct_type;


/* ???? */
typedef enum {
  /* want idle finger for ASET primary path assignment (normal mode of operation) */
  ASET_MODE,
  /* want idle finger for neighbor triage */
  NBR_MODE,
  /* the finger is assigned but out of lock */
  DIVERSITY_MODE
} dl_tri_idle_fing_mode_enum_type;

/*--------------------------------------------------------------------------
               STRUCTURE: DL_TRI_PARMS_STRUCT_TYPE

 This structure contains the various constants used by the triage algorithm
---------------------------------------------------------------------------*/
typedef struct
{
  /* When trying to match up existing finger positions with
     paths found, the maximum offset that will be accepted
     as a match */
  uint16  path_gran;

  /* When checking if two fingers are so close together that
     they aren't both useful, the minimum offset that will
     be accepted as distinct. */
  uint16  fing_gran;

  /* The minimum value by which we will adjust the window
     centering.  If it is off by less than this value, we
     will just let it be. */
  uint16  win_gran;

  /* The minimum energy in searcher units that will be accepted
     as a possibly useful path. Corresponds to -19 dB Ec/Io. */
  uint16 path_thresh;

  /* The minimum duration for which a finger must stay locked
     before we will make it the reference finger. */
  uint16 lock_min;

  /* When checking for the earliest "usable" finger, if the lock age of a
     later finger is greater than an earlier one AND the finger's RSSI is
     larger in energy by this ratio, choose it as the "earliest" usable fing */
  uint16 rssi_ratio;

  /* When assigning fingers to maximize coverage of the Active
     Set sectors, it is possible for the only finger on sector A
     to be re-assigned to sector B.  This can only happen if the
     (strongest) path in sector B is more than path_ratio1
     stronger than the (currently demodulated) path in sector A.
     If this ratio is too low, the fingers will be re-assigned
     too often, in case there are sectors of near-equal strength
     and not enough fingers to go around.  */
  uint16 path_ratio;

  /* the threshold which controls the minimum RSSI the finger
     can tolerate before going out of lock */
  uint16 fing_lock_lo_thresh;

  /* the max threshold which controls the max RSSI the finger has
     to reach to get into lock */
  uint16 fing_lock_hi_thresh;

  /* soft lock threshold i.e. software lock threshold */
  uint16 soft_lock_thresh;


  /* number of cells needed to be covered in order to meet the
     cell diversity requirements */
  uint16 cell_div_req_cnt;

  /* maximum number of triages the finger is allowed to remain unlocked
     before it is de-assigned */
  uint16 max_unlock_age;

  /* a path has to be at least this stronger than the finger that is to
     be re-assigned to this path */
  uint16 fing_to_path_ratio;

  /* the initial RSSI value that is programmed to a finger when a uP
     commanded slew is performed */
  uint16 init_rssi;

  /* the initial rotator value that is programmed to a finger when a uP
     commanded slew is performed */
  uint16 init_rot;

  uint16 min_distance;
  /* minimum cx8 distance between paths for one to be considered a sidelobe 
     of the other */

  uint16 max_distance;
  /* maximum cx8 distance between paths for one to be considered a sidelobe 
     of the other */

  uint8 fing_min_dist;
  /* fing_min_distance between two fingers*/

} dl_tri_parms_struct_type;

extern dl_tri_parms_struct_type dl_tri_parms;

typedef struct
{
  uint32 pn_pos_cx8;
  uint32 eng;
} dl_tri_fing_pos_and_eng;

/* function ptr to post callback routine */
typedef void (*WL1_TRI_CFG_REQ_POSTCB)(void);

typedef enum
{
  DL_CARR_CFG_MODULE = 0x0,
  RXD_MODULE,
  MCPM_MODULE,
  SCCH_ORD_MODULE,
  WSRCH_MODULE,
  DCUPA_SWITCH_MODULE,
  MAX_MODULES 
} dl_tri_module_enum_type;


/******************************************************************************
*******************************************************************************
                             EXTERNAL VARIABLES
*******************************************************************************
******************************************************************************/
/* The path database */
extern dl_tri_path_struct_type dl_tri_paths[DL_TRI_MAX_PATHS];

/* the actual number of paths in the path database at a given time */
extern uint8 dl_tri_num_paths;

/* the RL set database. Each entry is indexed by its TPC index
   e.g. dl_tri_rl_sets[0] is the entry for TPC 0.  DL_TRI_MAX_RL_SETS is max number of ASET */
extern dl_tri_rl_set_struct_type dl_tri_rl_sets[DL_TRI_MAX_RL_SETS];

#ifdef FEATURE_WCDMA_DC_HSDPA
extern dl_tri_rl_set_struct_type dl_tri_rl_sets_dc[DL_TRI_MAX_RL_SETS];
#ifdef FEATURE_WCDMA_3C_HSDPA
extern dl_tri_rl_set_struct_type dl_tri_rl_sets_3c[DL_TRI_MAX_RL_SETS];
#endif /* FEATURE_WCDMA_3C_HSDPA */
#endif /* FEATURE_WCDMA_DC_HSDPA */

/* allocate per PSC triage for max possible cells */
extern dl_tri_per_rl_triage_info_struct_type dltri_rl_info[DL_MAX_NUM_CELLS];


/* TRUE if triage needs to be held off for some reason */
extern boolean dl_delay_triage;

/* Triage delayed from HSDPA module */
extern boolean delay_triage_for_hs_cfg;

extern boolean dl_workloop_cell_cfg_prep_started;
/******************************************************************************
*******************************************************************************
                              FUNCTION PROTOTYPES
*******************************************************************************
******************************************************************************/

 /*===========================================================================
 FUNCTION        DL_TRI_INIT
 
 FILE            wl1tri.c
 
 DESCRIPTION     This routine initializes the triage databases, path, rl_set
                 and parameters.
 
 DEPENDENCIES    None
 
 RETURN VALUE    None
 
 SIDE EFFECTS    The above mentioned databases are initialized
 ===========================================================================*/
 void dl_tri_init(void);

 /*===========================================================================
 FUNCTION        dl_tri_init_for_fast_deconfig
 
 FILE            wl1tri.c
 
 DESCRIPTION     This routine is a wrapper for dltri_init()
 
 DEPENDENCIES    None
 
 RETURN VALUE    None
 
 SIDE EFFECTS    The above mentioned databases are initialized
 ===========================================================================*/
extern void dl_tri_init_for_fast_deconfig(void);
 
/*===========================================================================
FUNCTION        DLTRI_ADD_RL_TRIAGE_INFO

FILE NAME       wl1tri.c

DESCRIPTION     This routine registers a per rl/cell instance and a service with triage.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
  dl_tri_per_rl_triage_info_struct_type* dl_tri_add_per_rl_triage_info(
    /* primary scrambling code */
     uint16 psc,
     /* cell database index */
     uint8 cell_db_idx,
     /* srch cell pointer */
     srch_cell_struct_type *cell,
     /* service using this RL */
     uint8 assoc_service);


/*===========================================================================
FUNCTION        DLTRI_REMOVE_RL_TRIAGE_INFO

FILE NAME       wl1tri.c

DESCRIPTION     This routine removes a per rl/cell instance and a service with triage.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
dl_tri_per_rl_triage_info_struct_type* dl_tri_remove_per_rl_triage_info(
  /* cell db index */
  uint8 cell_db_idx,
  /* service using this RL */
  uint8 assoc_service );


/*===========================================================================
FUNCTION        DL_TRI_UPDATE_PER_RL_SERVICE_INFO

FILE NAME        wl1tri.c

DESCRIPTION   

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
dl_tri_per_rl_triage_info_struct_type* dl_tri_update_per_rl_service_info(
/* primary scrambling code */
  uint16 psc,
  /* cell database index */
  uint8 cell_db_idx,
  /* the path weights to apply */
  uint8 factors[],
  /* ref finger change callback */
  DL_TRI_REF_FING_CHANGE_UPDATE_CB_FUNC_TYPE *ref_fing_cb
  );



/*===========================================================================
FUNCTION        DLTRI_GET_RL_TRIAGE_INFO

FILE NAME       wl1tri.c

DESCRIPTION     This routine grabs the per cell info using the supplied cell DB index.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
dl_tri_per_rl_triage_info_struct_type* dl_tri_query_per_rl_triage_info_cell_db(
  /* cell database index */
  uint8 cell_db_idx);


/*===========================================================================
FUNCTION        DLTRI_GET_RL_REF_FING

FILE NAME       wl1tri.c

DESCRIPTION     This routine returns the ref_fing at the passed in index in the per rl triage db

DEPENDENCIES    None

RETURN VALUE    reference finger index

SIDE EFFECTS    None
===========================================================================*/
uint8 dltri_get_rl_ref_fing(uint8 cell_db_idx);


/*===========================================================================
FUNCTION        DLTRI_GET_STRONGEST_FING

FILE NAME       wl1tri.c

DESCRIPTION     This function returns strongest finger from a given PSC

DEPENDENCIES    None

RETURN VALUE    reference finger index

SIDE EFFECTS    None
===========================================================================*/
uint8 dltri_get_strongest_fing(uint8 cell_db_idx);

/*===========================================================================
FUNCTION        DLTRI_GET_STRONGEST_FING_IN_LOCK

FILE NAME       wl1tri.c

DESCRIPTION     This function returns strongest finger in lock from a given PSC

DEPENDENCIES    None

RETURN VALUE    strongest in-lock finger

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dltri_get_strongest_fing_in_lock(uint8 cell_db_idx);

/*===========================================================================
FUNCTION        DLTRI_SELECT_REF_FING_FOR_RL

FILE NAME       wl1tri.c

DESCRIPTION     This function chooses a new ref finger for a given radio link 

DEPENDENCIES    global flag set by searcher

RETURN VALUE    None

SIDE EFFECTS    reference finger for the RL structure is updated
===========================================================================*/
void dl_tri_select_ref_fing_for_rl(
    /* cell index */
    uint8 cell_db_idx,
    /* force hwch update even if ref finger didn't change */
    boolean force_update);

/*===========================================================================
FUNCTION        DL_TRI_SAVE_FING_INFOR (temp name kept same for integration purpose)

FILE NAME       wl1tri.c

DESCRIPTION     This function does a finger status dump and save the current
                finger database.Then disable all fingers.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    All finger are disabled.
===========================================================================*/
void dl_tri_save_fing_infor(void);

/*===========================================================================
FUNCTION        DL_TRI_GET_AND_CLEAR_FREQ_ERROR_ACCUM

FILE NAME       wl1tri.c

DESCRIPTION     This routine returns the freq error accumulation maintained
                in triage, then clears the accum.

DEPENDENCIES    None

RETURN VALUE    Structure with freq error accum and number of samples.

SIDE EFFECTS    None
===========================================================================*/
dl_tri_freq_error_struct_type dl_tri_get_and_clear_freq_error_accum(void);

/*===========================================================================
FUNCTION        dl_tri_eng_sort_comp_fn

FILE NAME       dltri.c

DESCRIPTION     This function acts as a comparator function for the quick
                sort routine when sorting paths by energy.

DEPENDENCIES    None

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
int8 dl_tri_eng_sort_comp_fn(
  const void *p1,
  const void *p2);

/*===========================================================================
FUNCTION        DL_TRIAGE

FILE NAME       wl1tri.c

DESCRIPTION     This routine executes the required operations in a given triage cycle

DEPENDENCIES  

RETURN VALUE    None

SIDE EFFECTS     Fingers assignment may get changed
===========================================================================*/
void dl_triage(void);

/*============================================================================

FUNCTION        DL_TRIAGE_PARAMS_INIT

DESCRIPTION     This is a wrapper to support old triage code

DEPENDENCIES    None

INPUT PARAMS    None

RETURN VALUE    None

SIDE EFFECTS    None
============================================================================*/
void dl_triage_parms_init(void);

/*===========================================================================
FUNCTION        DL_TRI_CELL_DEACTIVATION_COMPLETE

FILE NAME       wl1tri.c

DESCRIPTION     This function gets the cell_db_idx and moves all
                the fingers on that particular cell to unassigned
                state and updates the fing cnt db.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
void dl_tri_cell_deactivation_complete(
  /* cell db idx */
  uint8 cell_db_idx);

/*===========================================================================
FUNCTION        dl_tri_get_per_rl_svc

FILE NAME       dltri.c

DESCRIPTION     This routine returns the associated svc mask for the given cell db idx

DEPENDENCIES    None

RETURN VALUE    svc mask.

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_tri_get_per_rl_svc(uint8 cell_db_idx);

/*===========================================================================
FUNCTION        dl_tri_get_assigned_fings_bmsk

FILE NAME       dltri.c

DESCRIPTION     This routine returns a bitmask that represents all the fingers 
                that are assigned

DEPENDENCIES    None

RETURN VALUE    svc mask.

SIDE EFFECTS    None
===========================================================================*/
extern uint32 dl_tri_get_assigned_fings_bmsk(dl_tri_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION        DL_TRI_FET_FING_FOR_CELL_CONFIG

FILE NAME       dltri.c

DESCRIPTION     This function gets the cell_db_idx,pn pos. Gets a idle
                a idle fing and sends a assing cmd.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern uint8 dl_tri_get_fing_for_cell_config(
  /* cell ptr */
  srch_cell_struct_type *cell_ptr,
	/* phase reference type*/
  dl_tri_ref_fing_phase_ind phase_ref_type);

/*===========================================================================
FUNCTION        DLTRI_LIMIT_PATH_SPREAD

FILE            wl1tri.c

DESCRIPTION     This function modifies the path db based on path spread

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void dltri_limit_path_spread(dl_tri_carr_id_enum_type triage_mode);

/*===========================================================================
FUNCTION        DL_TRI_FIND_PATHS

FILE NAME       wl1tri.c

DESCRIPTION     This function populates the path database from the most
                recent set of active set search results.  All peaks
                greater than the cell's path threshhold in energy are
                inserted into the path database.

DEPENDENCIES    The search active set which is passed in as an array of
                pointers to cell structures.

RETURN VALUE    The number of paths found.  The global variable
                dl_tri_num_paths is also set to this value

SIDE EFFECTS    The current path database is overwritten with new paths.
                All existing paths are deleted.
===========================================================================*/
extern uint8 dl_tri_find_paths(srch_cell_struct_type *aset[], uint8 num_cells, dl_tri_path_purpose path_purpose);

/*===========================================================================
FUNCTION        DLTRI_CREATE_PATH_db

FILE NAME       wl1tri.c

DESCRIPTION     This function creates path database from the most recent set of active & nbr set search results.
                          All peaks greater than the cell's path threshhold in energy are inserted into the path database.

DEPENDENCIES    The search set which is passed in as an array of pointers to cell structures.

RETURN VALUE    The number of paths found.  The global variable dl_tri_num_paths is also set to this value

SIDE EFFECTS    The current path database is overwritten with new paths. All existing paths are deleted.
===========================================================================*/
extern uint8 dltri_create_path_db(dl_tri_carr_id_enum_type triage_mode,
  srch_cell_struct_type *cell_set[], uint8 num_cells, dl_tri_path_purpose path_purpose);


/*===========================================================================
FUNCTION        DL_TRI_GET_IDLE_FINGER

FILE NAME       wl1tri.c

DESCRIPTION     This function finds an "idle", i.e. unassigned, or the finger with the largest unlock age.

DEPENDENCIES   The finger status database

RETURN VALUE    The index of the best candidate for an idle finger

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_tri_get_idle_finger(
  /*carrier index*/
  dl_tri_carr_id_enum_type carr_idx,
  /* enum indicating what this finger will be used for */
  dl_tri_idle_fing_mode_enum_type mode);

/*===========================================================================
    FUNCTION        DL_TRI_DEASSIGN_FINGER
    
    FILE NAME       wl1tri.c
    
    DESCRIPTION     interface function for other modules for finger deassignment
    
    DEPENDENCIES    None
    
    RETURN VALUE    None
    
    SIDE EFFECTS    None
 ===========================================================================*/
extern void dl_tri_deassign_finger(uint8 fing);

/*===========================================================================
FUNCTION        DL_TRI_PER_RL_TRIAGE_GET_REF_FING

FILE NAME       wl1tri.c

DESCRIPTION     This routine returns the ref_fing at the passed in index in the per rl triage db

DEPENDENCIES    None

RETURN VALUE    reference finger index

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_tri_per_rl_triage_get_ref_fing(
    /* the per cell info index */
    uint16 cell_db_idx);

/*===========================================================================
FUNCTION        DL_TRI_GET_FINGERS_PER_RL

FILE NAME       wl1tri.c

DESCRIPTION     This function returns a bitmask of enabled fingers for the
                specified cell.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern uint32 dl_tri_get_fingers_per_rl(uint8 cell_db_idx);

/*===========================================================================
FUNCTION        DL_TRI_RESUME_PER_RL

FILE NAME        wl1tri.c

DESCRIPTION     This function resumes triage operations for the specified
                radio link.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_tri_resume_per_rl(
  /* cell database index */
  uint8 cell_db_idx,
  /* service requesting suspend */
  uint8 assoc_service);

  /*===========================================================================
FUNCTION        DL_TRI_SUSPEND_PER_RL

FILE NAME       wl1tri.c

DESCRIPTION     This function suspends triage operations for the specified
                radio link.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_tri_suspend_per_rl(
  /* cell database index */
  uint8 cell_db_idx,
  /* service requesting suspend */
  uint8 assoc_service);

/*===========================================================================
FUNCTION        DL_TRI_DEASSIGN_FING_FOR_RL

FILE NAME       wl1tri.c

DESCRIPTION     This routine deassigns all the fingers on a particular
                radio link, specified by the PSC.

DEPENDENCIES    The finger status database.

RETURN VALUE    None

SIDE EFFECTS    Fingers may be deassigned
===========================================================================*/
extern void dl_tri_deassign_fing_for_rl(uint16 psc);

/*===========================================================================
FUNCTION        DL_TRI_SET_PER_RL_PATH_BIAS

FILE NAME       wl1tri.c

DESCRIPTION     This routine sets path biasing factors for a specified PSC.

DEPENDENCIES    None

RETURN VALUE    TRUE if able to find the per PSC triage instance.

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_tri_set_per_rl_path_bias(
  /* cell_db_idx related to the cell whose paths should be biased */
  uint8 cell_db_idx,
  /* the path weights to apply */
  uint8 factors[]);

/*===========================================================================
FUNCTION        DL_TRI_ASSIGN_FINGER_TO_PATH

FILE NAME       wl1tri.c

DESCRIPTION     This function performs the bookkeeping associated with
                assigning a given finger to a given path.  This affects
                only the finger status database, it does not actually
                write any finger assignments to the firmware.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The finger status database will be updated and the path
                in the path database will have it's finger assignment
                updated.
===========================================================================*/
extern void dl_tri_assign_finger_to_path(
  /* TRUE if assigning finger to primary antenna */
  boolean primary,
  /* the finger to be assigned */
  uint16 fing,
  /* the path to which the finger should be assigned */
  uint16 path);  

/*===========================================================================
FUNCTION        DL_TRI_FING_ASSIGNED_TO_STRONGEST_PATH 

FILE NAME       wl1tri.c

DESCRIPTION     This function returns a finger assigned to the strongest  path
                
DEPENDENCIES    finger database. 

RETURN VALUE    A assigned finger

SIDE EFFECTS    None.
===========================================================================*/
extern uint8 dl_tri_fing_assigned_to_strongest_path(dl_tri_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION        DL_TRI_FIND_EARLIEST_FINGER

FILE NAME       dltri.c

DESCRIPTION     This routine returns the earliest finger in a particular
                radio link, specified by the PSC.

DEPENDENCIES    The finger status database, the radio link database

RETURN VALUE    Earliest finger in radio link

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_tri_find_earliest_finger (uint8 cell_db_idx);

/*===========================================================================
FUNCTION        DL_TRI_FIND_EARLIEST_FINGER_IN_LOCK

FILE NAME       dltri.c

DESCRIPTION     This routine returns the earliest in-lock finger in a particular
                radio link, specified by the PSC.

DEPENDENCIES    The finger status database, the radio link database

RETURN VALUE    Earliest in-lock finger in radio link

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_tri_find_earliest_finger_in_lock(uint8 cell_db_idx);

/*===========================================================================
FUNCTION        DL_TRI_FING_STANDOFF_STATUS_CLEAR

FILE NAME       wl1tri.c

DESCRIPTION     added for intergation purpose. acts like a wrapper for dltri_standoff_table_clear_for_fing()

DEPENDENCIES    Finger standoff status table, finger status dbase

RETURN VALUE    None

SIDE EFFECTS    Finger standoff status table counts and the virtual path
                boolean is cleared for this finger.
===========================================================================*/
extern void dl_tri_fing_standoff_status_clear(uint8 fing);

/*===========================================================================
FUNCTION        DL_TRI_FING_SIDELOBE_STATUS_CLEAR

FILE NAME       dltri.c

DESCRIPTION     This routine clears the sidelobe count values for all finger
                pairs associated with the specified finger. It is called by
                demod_disable_finger().

DEPENDENCIES    Finger sidelobe status table

RETURN VALUE    None

SIDE EFFECTS    Finger sidelobe status table counts.
===========================================================================*/
extern void dl_tri_fing_sidelobe_status_clear(uint8 fing);

/*===========================================================================
FUNCTION        DLTRI_PROCESS_FING_AVAIL_CMD

DESCRIPTION
  This function is called as a result of a local command posted by dl_tri_fing_cfg_cmd_done when
  it gets the fing cfg done isr from mcal 

DEPENDENCIES 

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dltri_process_fing_avail_cmd(void);

/*===========================================================================
FUNCTION        DL_TRIAGE_WITHOUT_STATUS_DUMP

FILE NAME       wl1tri.c

DESCRIPTION     This function calls dl_triage() after disabling status dumps.

DEPENDENCIES    The search active set database, the finger status database

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern void dl_triage_without_status_dump(boolean pich_only_wakeup);

/*===========================================================================
FUNCTION        DL_TRI_PCH_INIT

FILE NAME       wl1tri.c

DESCRIPTION     This routine initializes the fatpath detection logic and
                should be called upon entry to PCH state.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_tri_pch_init(void);

/*===========================================================================
FUNCTION        DL_TRI_SET_FING_TO_STRONGEST_PATH 

FILE NAME       wl1tri.c

DESCRIPTION     This function assigns the global variable the finger index tracking the strongest path.
                
DEPENDENCIES    finger database. 

RETURN VALUE    A assigned finger

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_tri_set_fing_to_strongest_path(uint8 fing);

extern void dl_tri_post_cell_deconfig_check(dl_tri_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION        DL_TRI_ASSIGN_FING

FILE NAME       wl1tri.c

DESCRIPTION     This function takes in path_idx, fing_idx, pn pos
                and cell_db idx as the params. Usually only two of 
                these param's are valid. This func fills in the
                required params and sends a demod assign fing command.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_tri_assign_fing(
  /* path idx */
  uint8 path_idx,
  /* fing idx */
  uint8 fing_idx,
  /* pn pos when no path info available */
  uint32 pnpos,
  /* cell db idx when no path info available*/
  uint8 cellidx);

/*===========================================================================
FUNCTION        DL_TRI_FING_CFG_CMD_DONE

FILE NAME       wl1tri.c

DESCRIPTION     This function is called when a demod done cmd is sent from mcal.
                Fing status db and Fing cnt db gets updated.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_tri_fing_cfg_cmd_done(
  /* bit mask set for demod cfg done fingers */
  uint32 fing_bmsk,
  /* status indicating if demod cfg passed/failed*/
  int8 status);

/*===========================================================================
FUNCTION        DLTRI_PROCESS_RXD_UPDATE_CMD

FILE NAME       wl1tri.c

DESCRIPTION     

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS   
===========================================================================*/
extern void dltri_process_rxd_update_cmd(void);

/*===========================================================================
FUNCTION        DL_TRI_UPDATE_RXD_INFO

FILE NAME       wl1tri.c

DESCRIPTION     This routine updates the available finger count based on
                specific events. When the finger count is reduced, it runs
                pseudo triage to create a path database from all the fingers
                and then assign them to the fewer number of fingers based on
                priority.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    dl_tri_fing_count_db gets updated and some fingers may get
                disabled.
===========================================================================*/
extern void dl_tri_update_rxd_info(boolean enabled, boolean flush_rxd_cfg,
                              DL_TRI_RXD_TRANS_IND_CB_FUNC_TYPE *rxd_proc_tri_cb);

/*===========================================================================
FUNCTION        DL_TRI_DEALLOCATE_FINGS_ON_CELLS

FILE NAME       wl1tri.c

DESCRIPTION     This function gets the cell_db_idx and finds all
                the fingers on that particular cell and deassigns
                them.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_tri_deallocate_fings_on_cells(
  /* cell db idx */
  uint8 cell_db_idx);

/*===========================================================================
FUNCTION        DL_TRI_TA_LOG

FILE NAME       dltri.c

DESCRIPTION     This routine logs the finger and path status to the QXDM
                using the TA screen log packet

DEPENDENCIES    The finger status dbase, the path dbase, the radio link
                dbase

RETURN VALUE    None

SIDE EFFECTS    A TA packet may be logged
===========================================================================*/
extern void dl_tri_ta_log(void);

/*===========================================================================
FUNCTION        dl_tri_mark_cell_for_phase_ref

FILE NAME       wl1tri.c

DESCRIPTION     This function updates the phase reference used for each RL.
                "phase_ind_per_cell" is used when communicating reference finger 
                change to cell cfg module.

DEPENDENCIES    None.

RETURN VALUE    TRUE if the phase ref has changed false if it hasnt.

SIDE EFFECTS    None
===========================================================================*/
extern boolean dl_tri_mark_cell_for_phase_ref(
  uint8 cell_db_idx, 
  uint8 phase);

/*===========================================================================
FUNCTION        dl_tri_get_num_locked_fings

FILE NAME       wl1tri.c

DESCRIPTION     This function gets the number of locked fingers on a
                particular cell.

DEPENDENCIES    None.

RETURN VALUE    The number of locked fingers on that cell.

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_tri_get_num_locked_fings(uint8 cell_db_idx,
                                         uint8 *prim_fing_ctr,
                                         uint8 *div_fing_ctr);

/*===========================================================================
FUNCTION        dl_tri_get_locked_fing_info

FILE NAME       wl1tri.c

DESCRIPTION     This function gets the position and energy of locked fingers
                for a particular cell.

DEPENDENCIES    None.

RETURN VALUE    The number of locked fingers for that cell.

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_tri_get_locked_fing_info(
  uint8 cell_db_idx,
  dl_tri_fing_pos_and_eng * pos_and_eng, boolean fw_tracked_fings_only);

/*===========================================================================
FUNCTION        dl_tri_get_strongest_fing_rssi

FILE NAME       wl1tri.c

DESCRIPTION     This function gets the rssi of strongest locked finger
                for a particular cell.

DEPENDENCIES    None.

RETURN VALUE    The ecio of strongest locked finger

SIDE EFFECTS    None
===========================================================================*/
extern uint16 dl_tri_get_strongest_fing_rssi( uint8 cell_db_idx);

/*===========================================================================
FUNCTION        dltri_fing_ant_swap

FILE NAME       wl1tri.c

DESCRIPTION     This routine swaps the antenna of all the assigned fingers and
                commit to FW

DEPENDENCIES  

RETURN VALUE    None

SIDE EFFECTS    After call of this function antenna ID of fingers will be swapped
                i.e Prim -> Div/Div -> Prim
===========================================================================*/
void dltri_fing_ant_swap(void);

/*===========================================================================
FUNCTION        dl_tri_query_per_rl_triage_svc_info

FILE NAME       wl1tri.c

DESCRIPTION     

DEPENDENCIES     

RETURN VALUE    TRUE if service is enabled on cell, else FALSE

SIDE EFFECTS    None
===========================================================================*/
boolean dl_tri_query_per_rl_triage_svc_info(
  /* cell database index */
  uint8 cell_db_idx, uint8 service);

/*===========================================================================
FUNCTION        dltri_update_fings_tpc

FILE NAME       wl1tri.c

DESCRIPTION     

DEPENDENCIES  

RETURN VALUE    None

SIDE EFFECTS     None
===========================================================================*/
extern void dltri_update_fings_tpc(void);

/*===========================================================================
FUNCTION        DL_TRI_IDLE_INIT

FILE            wl1tri.c

DESCRIPTION     This routine initializes the needed triage databases
                and parameters during IDLE state transition.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The above mentioned databases are initialized
===========================================================================*/
extern void dl_tri_idle_init(void);

/*===========================================================================

FUNCTION dl_tri_is_triage_suspended

DESCRIPTION
  This function returns the boolean saying whether the Triage is suspended or not.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the  status of triage suspension.

SIDE EFFECTS
  None
 ===========================================================================*/
extern boolean dl_tri_is_triage_suspended(void);

/*===========================================================================

FUNCTION dl_tri_get_fing_pos

DESCRIPTION
  This function returns the value of Reference finger position in CX8.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the  Reference finger position in CX8.

SIDE EFFECTS
  None
 ===========================================================================*/
 extern uint32 dl_tri_get_fing_pos(uint8 fing);

/*===========================================================================
FUNCTION        DLTRI_TOTAL_FING_RSSI

FILE NAME       wl1tri.c

DESCRIPTION     This function calculates the total RSSI of the finger.  This
                in general just returns the combined RSSI (rssi0 and rssi1)
                but when the finger is paired it will return the sum of the 
                two combined RSSI.

DEPENDENCIES    None

RETURN VALUE    The total RSSI for that finger.

SIDE EFFECTS    None
===========================================================================*/
extern uint32 dltri_total_fing_rssi(uint8 fing);
/*===========================================================================
FUNCTION        dl_tri_get_fing_rot

FILE NAME       wl1tri.c

DESCRIPTION     This routine is used to get the finger rotator
  
RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern int16 dl_tri_get_fing_rot(uint8 fing);

#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
/*===========================================================================

FUNCTION dl_tri_perform_ant_switch_ops

DESCRIPTION
  This function performs the antenna switching operations.

DEPENDENCIES
  None.

RETURN VALUE
  BOOLEAN

SIDE EFFECTS
  None
 ===========================================================================*/
extern boolean dl_tri_perform_ant_switch_ops(uint32 *finger_bmsk_for_commit);
#endif

/*===========================================================================
FUNCTION        dl_tri_setup_workloop_config

FILE NAME       dltri.c

DESCRIPTION     Routine to setup the workloop to run triage in the case of 
                parallel reacq.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_tri_setup_workloop_config(void);

extern void dl_tri_work_loop_event_handler(void);

extern dl_tri_svc_mode_enum_type dl_tri_get_svc_mode(void);

/*===========================================================================
FUNCTION        dl_tri_set_svc_mode

FILE NAME       dltri.c

DESCRIPTION     Routine to set the Triage service mode 
 
DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_tri_set_svc_mode(dl_tri_svc_mode_enum_type svc_mode);

/*===========================================================================
FUNCTION        dl_tri_flush_rxd_cfg_prll_reacq

FILE NAME       dltri.c

DESCRIPTION     Routine to flush off the prepared rxd config

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
===========================================================================*/
extern void dl_tri_flush_rxd_cfg_prll_reacq(uint8 cell_idx);

/*===========================================================================
FUNCTION        dl_tri_is_at_least_one_rl_active

FILE NAME       dltri.c

DESCRIPTION     This routine checks if any one RL is configured.

DEPENDENCIES    None.

RETURN VALUE    TRUE if any one RL is configured. FALSE if none of the 
                RLs are configured.

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dl_tri_is_at_least_one_rl_active(void);

/*===========================================================================
FUNCTION        dltri_enable_rxd

FILE NAME       dltri.c

DESCRIPTION     Routine to enable the RxD

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.
 ===========================================================================*/
extern void dltri_enable_rxd(void);

/*===========================================================================
FUNCTION dl_tri_get_assigned_fing_cnt

DESCRIPTION
  This function returns the number of fingers assigned for a given carrier index.

DEPENDENCIES
  

RETURN VALUE
  

SIDE EFFECTS
  None
 ===========================================================================*/
extern uint8 dl_tri_get_assigned_fing_cnt(uint8 carr_idx);
/*===========================================================================

FUNCTION dl_tri_get_total_fing_cnt

DESCRIPTION
  This function returns the number of fingers assigned for a given carrier index. This includes the fingers
  currently being assigned and currently being deassigned.

DEPENDENCIES
  

RETURN VALUE
  

SIDE EFFECTS
  None
 ===========================================================================*/
extern uint8 dl_tri_get_total_fing_cnt(uint8 carr_idx);
/*===========================================================================

FUNCTION dltri_set_schic_for_mcvs

DESCRIPTION
  This function checks if the below conditions for full SCHIC are satified and sets FW interface variable to 1 
  if yes, else 0 .
  This function will be called when sending demod config command.

  1 SHO cell.
  |RxD config |TxD config |Max number of fingers|
  |No           |No            |8                            |
  |Yes          |No            |7                            |
  |No           |Yes           |6                            |
  |Yes          |Yes           |6                            |
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_tri_set_schic_for_mcvs(void);

/*===========================================================================

FUNCTION dl_tri_get_fing_cnt_for_mcvs

DESCRIPTION
  This function returns the number of fingers which are not unassigned (assigned, assign in progress, 
  deassign in progress) and which have ASET service on it.

DEPENDENCIES
  

RETURN VALUE
  

SIDE EFFECTS
  None
 ===========================================================================*/
extern uint8 dl_tri_get_aset_fing_cnt(uint8 carr_idx);
/*===========================================================================
FUNCTION        dl_tri_is_fing_cfg_in_progress

FILE NAME       wl1tri.c

DESCRIPTION     Returns TRUE if fing config is in progress. Else returns FALSE.

DEPENDENCIES     

RETURN VALUE    
SIDE EFFECTS   None
===========================================================================*/
extern boolean dl_tri_is_fing_cfg_in_progress(void);

/*===========================================================================

FUNCTION dltri_post_mcpm_update_cmd

DESCRIPTION
  This function calls MCPM API for updating the number of fingers and changing VPE freq 
  if needed.

DEPENDENCIES
  

RETURN VALUE
  

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dltri_post_mcpm_update_cmd(void);

/*===========================================================================
FUNCTION        dl_tri_register_callback

FILE NAME       wl1tri.c

DESCRIPTION     If triage is running and other modules also wanted to do finger related operations,  they register
a call back with triage. Once triage is done, it will call each of the call backs.

DEPENDENCIES     

RETURN VALUE    
SIDE EFFECTS   None
===========================================================================*/
extern void dl_tri_register_callback(
  dl_tri_module_enum_type module,
  WL1_TRI_CFG_REQ_POSTCB post_cb
  );

/*===========================================================================
FUNCTION        dltri_post_tri_cb_cmd

FILE NAME       wl1tri.c

DESCRIPTION     This function posts a local cmd to call the callback registered 
while triage was running.

DEPENDENCIES     

RETURN VALUE    
SIDE EFFECTS   None
===========================================================================*/
extern void dltri_post_tri_cb_cmd(void);

/*===========================================================================
FUNCTION        dltri_process_tri_cb_cmd

FILE NAME       wl1tri.c

DESCRIPTION     
 This function calls the call backs registered while triage was running.

DEPENDENCIES     

RETURN VALUE    
SIDE EFFECTS   None
===========================================================================*/
extern void dltri_process_tri_cb_cmd(void);

/*===========================================================================

FUNCTION dl_tri_cleanup_fings_upon_fast_cell_decfg

DESCRIPTION
  This function handles the finger state transitions for fast cell disable scenarios.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_tri_cleanup_fings_upon_fast_cell_decfg(uint8 cell_db_idx);

/*===========================================================================
FUNCTION  dl_tri_check_cell_needs_fing

DESCRIPTION
 This function Checks if a cell configuration is waiting for free finger resorce

DEPENDENCIES 

RETURN VALUE
  TRUE: If the cell_idx is part of cell_cfg_cell_need_fing_mask
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean dl_tri_check_cell_needs_fing(uint16 cell_db_idx);

/*===========================================================================

FUNCTION dl_tri_cleanup_fings_upon_fast_cell_decfg

DESCRIPTION
  This function handles the finger state transitions for fast cell disable scenarios.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_tri_cleanup_fings_upon_fast_cell_decfg(uint8 cell_db_idx);

/*===========================================================================

FUNCTION dl_tri_enable_dbg_msgs

DESCRIPTION
  This function controls the enabling/disabling of debug msgs in idle mode

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/

extern void dl_tri_enable_dbg_msgs(boolean enable);

#ifdef FEATURE_WCDMA_DC_HSDPA
/*===========================================================================

FUNCTION dl_tri_ops_upon_scch_ord_reception

DESCRIPTION
  This function handles the triage operations upon receiving the HS SCCH Order to activate/de-activate DBDC.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_tri_ops_upon_scch_ord_reception(boolean);
#endif /*FEATURE_WCDMA_DC_HSDPA */

 /*===========================================================================

FUNCTION dl_tri_modify_fing_alloc

DESCRIPTION
  This function switches the finger allocation on Primary and Secondary carrier after deallocating them as necessary
  case.
  If switch Status is ON - Need to allocate fingers 10 and 11 from Prim Carr. 
                                    Fings for Sec Carr as (10-17) from (12-17).
                                    Fings for Prim Carr as (0-9) from (0-11)

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern boolean  dl_tri_modify_fing_alloc(boolean switch_status);
 
/*===========================================================================

FUNCTION dl_tri_set_triage_nv_values

DESCRIPTION
  Function populates the num of fingers to be set thro NV, controls enabling of debug F3s for triage

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_tri_set_triage_nv_values(void);


/*===========================================================================

FUNCTION dl_tri_get_total_hs_locked_fing_rssi

DESCRIPTION
  RSSI for HS fingers in lock for a given antenna and carrier

DEPENDENCIES
  None.

RETURN VALUE
  uint32

SIDE EFFECTS
  None
 ===========================================================================*/
extern uint32 dl_tri_get_total_hs_locked_fing_rssi(uint8 ant_id, uint8 carr_idx);

/*===========================================================================

FUNCTION dl_tri_is_hs_enabled

DESCRIPTION
  This function checks if HS is enabled on a finger.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If hs is enabled on finger provided. FALSE: otherwise

SIDE EFFECTS
  None
 ===========================================================================*/
extern boolean dl_tri_is_hs_enabled(uint8 fing);
/*===========================================================================
FUNCTION dl_tri_disable_rxd_transition_ops

DESCRIPTION
  This function is called when rxd teardown needs to happen in ISR context,
  to initialize rxd related variables, so that demod cmd is not sent to FW
  when handling it.

DEPENDENCIES
  To be used only with FCDC

RETURN VALUE
  NONE

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_tri_disable_rxd_transition_ops(void);

/*===========================================================================
FUNCTION          dl_tri_any_fing_has_ASETService

DESCRIPTION       API to check if cell has R99_ASET_SERVICE configured with any of the fing in the
                  fing_ena_mask passed as argument

DEPENDENCIES      None

RETURN VALUE      BOOLEAN
                     TRUE if cell has R99_ASET_SERVICE configured with any of the fing_ena_mask
                     FALSE otherwise

SIDE EFFECTS        None
===========================================================================*/
extern boolean dl_tri_is_any_fing_has_aset_service(uint16 fing_ena_mask);
#endif /* WL1TRI_H */
