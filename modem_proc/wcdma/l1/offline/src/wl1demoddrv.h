#ifndef WL1DEMODDRV_H
#define WL1DEMODDRV_H

/*===========================================================================
                              D E M O D D R V . H


DESCRIPTION

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2000 - 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1demoddrv.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $
Imaginary Buffer Line

when        who     what, where, why
--------    ---     --------------------------------------------------------
06/30/16    sl      F3 reduction
02/24/15    ap      Initial check-in for WL1 QSH framework 
12/22/14    pr      F3 Reduction
07/03/14    sks     Changes for FCDC
06/30/14    pr      Modify the SW criteria for detecting thr TxD status by taking 
                    the rssi strength of both primary and secondary in consideration.
05/21/14    sl      Changes for 0x4186 log packet version8
03/18/14    mit     F3 cleanup, enabling via NV's
04/23/14    srk     Monitor mode changes for HS only TPC Feature
04/10/14    sr      Changes to cleanup "antenna" from finger database
03/25/14    srk     Exit criteria change for HS only TPC feature
03/11/14    srk     SW changes for HS only TPC feature
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/10/14    sr      Changes to cleanup "antenna" from finger database
05/30/13    pr      Set FTL mode to TRK/ACQ.
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
04/11/13    pr      Added logging changes for 0x4186 for debug purposes.
04/01/13    hk      Mainlined FEATURE_SHO_TPC_THRESH
09/23/12    kcm     Added Fing RSSI and TCPF logging.
09/26/12    kcm     Changes to reslam OOL fingers under certain criterion
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12    pv      Parallel reacq changes.
06/13/12    kcm     TPC rejection algorithm ehancements
06/25/12    pr      Added logging for sixTapEn in 0x4186
06/11/12    vs      MBMS feature cleanup.
05/14/12    sr      Compute the CPICH RSCP from primary antenna fingers only
04/12/12    zr      Added support for txd detection based on the prim and div
                    finger rssi.
02/29/12    vsr     QICE Power Management (QPM) Support
02/13/12    hk      Nikel feature cleanup
02/10/12    rgn     Nikel feature cleanup
01/31/12    kcm     Changes to update the fing_ena_bmsk during the triage 
                    add per rl info.
02/01/12    gnk     Nikel feature cleanup
01/29/12    vs      Feature cleanup.
11/15/11    hk      Added support to change combiner lock thresholds based on
                    finger combined RSSI
04/28/11    sj      upmerge from offline/main
04/26/11    mc      SW decoupling
04/13/11    ks      Changed prototype of demod_set_tpc_threshold function
10/22/10    ks      Reduced finger init rssi by 3dB on genesis
08/17/10    ks      Corrected macro to write PICH paging indicator PN count
03/30/10    rvs     IRAT sample-server config changes for Chetak and
                    general clean-up.
01/22/09    rgn     SSID to be used for fingers and antenna id to be used for paths
09/24/09    hk      Porting of AICH resetup feature to Offline
08/10/09    rgn     Replaced intlocks with mutexes
07/16/09    hk      Reorganized the code to remove INC/SRC violations
05/05/09    rgn     Sync up cell info in fing status dbase after addition of 
                    service to an RL
05/01/09    vc      Added support for additional finger info ttlstepsize, apfAlpha.
03/31/09    rgn/uk  DSD Design change for Idle mode
03/18/09    yh      Add support to check if there's any fing in ant1 for CM phase 1
01/23/09    ks      Added FING_FREQ macro to get fing's cell freq
12/08/08    rmak    Mainline FEATURE_PARALLEL_QXT_NCP
11/20/08    rgn     Added demod status dump lite function
11/10/08    ks      Added new fing state FING_RESERVED, removed prototype for
                    demod_disable_all_fingers
11/05/08    yh      Add cleanup status dump
11/04/08    rgn     New demod status dump designchanges
10/28/08    mmr     Changed the initial fing rssi to Q16units in Taxis.
10/21/08    scm     Fix MDSP_READ_COMBINED_FREQ_EST() definition.
10/20/08    mg      Set indPnCount to zero
10/03/08    hk      Bringing in 7k Mailine fixes
10/02/08    ks      Corrected RSSI_TO_ENG macro
09/29/08    ks      Shuffled members of fing_status_struct_type to ease debugging
09/26/08    vb      added FING_NUM_STATES
09/15/08    mg      Add apis for getting freqcommon and tcxoaccum
08/08/08    ks      fing_state_enum_type moved here from demoddrv.h
                    IS_ASSIGNED and IS_FING_CONFIGURED macros redefined.
                    Few more macros added
08/04/08    scm     Adjust PICH macros for FEATURE_WFW_SW_INTERFACE.
07/17/08    mg      Support for Cx32 TTL
08/07/07    vsr     Moved in finger ECIO related functions from dldem.c
06/18/07    rgn     Added cell db idx to the list of finger assign params
05/07/07    rgn     Changed the prototype of demod_fing_cfg_update_in_cm to support
                    compressed mode with enhanced dl
03/29/07    bbd     Removed demod_assign_fing_phch().
03/14/07    bbd     Fixed hwch ref finger and TT enable/disable.
01/12/07    rgn     Added macros to overcome hwch4 disconnect problem in firmware
01/04/07    bbd     Modified ASAP finger interface to accept bitmask.
11/08/06    bbd     Added F-DPCH support.
10/06/06    bbd     Added demod init routine.
09/19/06    bbd     Added support for MMSE.
08/28/06    vp      Added new member nbr_rl_idx in figer_status_struct
08/14/06    ub      Added support for HSUPA.
07/12/06    mc      Changed the ref fing bitmask to 8 bits.
06/22/06    bbd     Added bitmask for f?Common.
05/30/06    bbd     Updated RSSI to searcher energy conversion.
05/18/06    mc      Added support for HS+SSC. Removed HS enable bit field
                    from finger common config.
03/20/06    bbd     Added support for paired finger assignment and RxD changes.
12/16/05    bbd     Modified finger assignment to use new QXT interface.
11/07/05    bbd     Changed demod_assign_finger() to use parameter struct.
09/27/05    bbd     Mainlined HW FMP feature and removed unused finger dbase
                    elements associated with SW/FW FMP.
08/19/05    mc      Added a field in the finger status database to remember 
                    the hardware channel it is associated with. 
07/12/05    vp      Added functions demod_nbr_fing_asn_imdiate and
                    demod_xfer_nbr_fing_parms_imdiate to send the finger
                    assignment command for N-PCCPCH immediately without
                    awaiting next CM finger update event
07/07/05    bbd     Added definition for workaround DCH state TTL beta.
05/31/05    gv      Changes to remove Lint errors
05/12/05    bbd     Under FEATURE_FTL_INIT_ON_FIRST_FINGER_ONLY, changed initial
                    finger RSSI to 73 and added first_finger field to finger
                    database.  This indicates that the finger is the first
                    finger assigned to its respective radio link.
04/04/05    bbd     More modification of TTL_BETA. Now set to 1 in DCH state.
03/29/05    bbd     Added fing_adv_ret_enum_type and advance/retard values
                    for finger nudging.
03/28/05    bbd     Clean up the TTL Beta code.
03/25/05    bbd     When FEATURE_SRCH_GRANULARITY_WORKAROUND defined, set non-acq
                    TTL_BETA value to 2 instead of 3.
02/26/05    gs      Added HS related structure members hs_enabled, hs_ref_fing
                    hs_cell_idx to finger status structure.
                    Added macros to access and set the value of these elements
                    Externalized functions demod_drv_update_fing_hs_cell_idx
                    and demod_drv_update_hs_cell_idx
02/03/05    bbd     Mainlined FEATURE_FING_PERF_ENHANCE.
12/30/04    gs      Updated prototype definition of functions
                    demod_disable_fing_hs and demod_enable_fing_hs
12/27/04    src     Increased polling retrials to 6 for status-dump command and
                    reduced to 133 the upfront microseconds needed there. Also,
                    increased polling retrials to 6 for finger-config command.
11/11/04    vb      Code update to support to set use best pilot in CLTD case
                    under FEATURE_USE_BEST_PILOT
10/22/04    bbd     Modified demod_set_fing_ttl_beta() to accept a TTL Beta
                    value as a parm.
08/30/04    gs      Declared HS enable/disable related functions
                    Added offset, bit mask for HS enable and HS cell index
08/26/04    src     Merged in the latest CLTD changes with featurisation under
                    the feature flag FEATURE_TRANSMIT_DIVERSITY_SUPPORT.
06/01/04    scm     Add extern to demod_is_at_least_one_finger_in_lock().
04/19/04    bbd     Added support to set TTL_BETA values for HW CPP under
                    FEATURE_SET_FING_TTL_BETA.
04/13/04    scm     Add extern to demod_is_at_least_one_finger_assigned().
09/23/03    src     Changed several finger configuration functions to return a
                    boolean (TRUE/FALSE);
09/16/03    cpe     Added a macro to initialize the finger init bit
08/03/03    gs      Externalized following functions
                    - demod_xfer_fing_cfg_shadow_cm
                    - demod_fing_cfg_update_in_cm
                    - demod_fing_cfg_update_cm_cleanup
07/14/03    src     Created a new fing_cfg_cmd_status_enum_type enumerant type
                    and the extern declaration for demod_fing_cfg_cmd_status to
                    keep track of the progress of finger config command.
07/02/03    sh      Removed demod_serv_cell_last_known_pos.
06/17/03    src     Corrected bit-mask FINGPARM_PHYS_TTOFFSET_BMSK (to 0xFF).
06/05/03    scm     Remove unused demod_use_tcxo_freqhypo. Add () to 2 macros.
06/02/03    yus     Define macros SET_MAX_NUM_FINGERS(void) and
                    SET_NUM_FINGS_FOR_LOW_MDSP_CLK(void).
05/20/03    src     Added hash-defined symbols to support giving more time for
                    the mDSP to finish the status dump and finger config.
04/30/03    src     Added a new hash-symbol DEMOD_FINGER_CONF_MICROSECS_NEEDED
                    for the expected number of microseconds that a finger con-
                    figuration command may require in the worst case scenario.
04/28/03    src     Merged-in changes again corresponding to physical channel
                    configuration in the event context. Also, made sure that
                    there are a "..._wait" function and a "..._pause" function
                    for both finger-config and status-dump commands.
04/23/03    yus     Added support for FEATURE_FING_PERF_ENHANCE.
04/15/03    src     Added declaration for demod_send_finger_cfg_cmd_wait (at
                    least until it is removed later due to non-use).
04/14/03    src     Merged-in changes and code re-organisation from the single-
                    mode branch corresponding to elimination of inline L1 wait,
                    while keeping the old interfaces as operationally similar
                    as possible.
02/04/03    yus     Add the support for new TPC threshold algorithm.
                    It's defined under FEATURE_SHO_TPC_THRESH.
11/05/02    cpe     Updated the copyright for the file
                    Corrected the value of FINGPARM_PHYS_SSCRCODE_SHFT from 12
                    to 9 as per the finger physical channel layout
10/14/02    scm     Extern demod_use_tcxo_freqhypo for SUSPEND/RESUME.
09/12/02    src     Added FEATURE_FINGER_REUSE as an additional condition to
                    check for when hash-defining IS_FING_ASSIGNED_NBCH() macro.
07/24/02    gs      Added bit field, mask values for finger phch fields
                    for alternate scrambling code change.
07/19/02    yus     Added support for finger merge protection
06/19/02    yus     Changed INITIAL_FINGER_RSSI to 240.
06/12/2002  yus     Changed RSSI_TO_ENG to deal with 32 bits srch eng value.
05/19/2002  src     Added hash-macros to compute status dump parameters such as
                    Tx system time and to read pre-adjusted reference count.
05/18/2002  ma      Added demod_serv_cell_last_known_pos and IS_FING_ASSIGNED_NBCH()
                    for cell reselection support.
05/17/2002  yus     Change RSSI_TO_ENG formula to match the rssi bitwidth.
05/14/2002  src     Corrected a pair of hash-defined macros that had syntactic
                    errors.
05/08/2002  src     Added a new macro to read the newly assigned PN position
                    of a finger. The macro name is PN_ASSG_POS() and takes the
                    finger as an argument to index into the finger database.
01/28/2002  yus     Add SET_TT_DISABLE() macros.
11/05/2001  cpe     Corrected the MACRO definitions for  REF_FING_TTOFFSET_BMSK
                    and REF_FING_TTOFFSET_SHFT as per the change in mdsp interface
11/02/2001  cpe     Added dltriparm.h to the includes
09/19/2001  asr     removed obsolete pich-related mdsp macros
09/12/2001  mdl     fixed pich-related mdsp macros
08/24/2001  mdl     removed MDSP_WRITE_AICH_THRESH() and added
                    MDSP_WRITE_AICH_THRESHOLDS() which writes both the ACK
                    and NAK detection thresholds
07/18/2001  mdl     added MDSP_WRITE_AICH_THRESH() macro
06/13/2001  mdl     modified prototype of demod_assign_finger, added
                    FING_ROTATOR macro.
06/11/2001  mdl     added a few scrambling code conversion macros
03/19/2001  mdl     changed MSM5200*.h to msm5200*.h in #include statements
02/21/2001  mdl     Removed prototypes for demod_clr_update_bits() and
                    demod_xfer_fing_cfg_shadow().
                    Added prototype for demod_send_fing_cfg_cmd().

01/31/2001  mdl     Added macros FING_CLR_ASSIGN_UPDATE_BIT and
                    FING_CLR_PHYCHAN_CFG_UPDATE_BIT to clear the update bits
                    in the finger assignment and finger phychan config
                    portions of the finger config record shadow.

                    added prototype for demod_clr_update_bits()

01/24/2001  mdl     added externs for reference finger index variables
                    demod_nghbr_cell_ref_finger and
                    demod_serv_cell_ref_finger

                    added prototype for demod_disable_all_fingers()

01/17/2001  mdl     changed FING_SOFT_LOCK_RSSI_THRESH from a hard constant
                    to refer to the variable dl_tri_parms.soft_lock_thresh

                    added path_idx to the fing_status_struct_type which is
                    the array index of the path that the finger is assigned
                    to

                    added prototype for demod_disable_finger()

01/12/2001  mdl     changed finger lock bitmask bit definitions to use the
                    definitions in msm5200mdsp.h definitions directly

                    added FING_STATE macro, change IS_IN_LOCK to
                    IS_IN_COMB_LOCK, changed COMB_RSSI to FING_COMB_RSSI

                    added SET_PILOT_SYM_FILT_GAIN macro to write the pilot
                    symbol filter gain parameter to the mDSP

01/11/2001  mdl     added lost_status_dump field to demod_status_dump_info
                    structure.

                    changed return type of demod_status_dump from void to
                    boolean in the prototype

                    added some comments

01/09/2001  mdl     added macros SET_FING_STATE, SET_FING_NEW_ASSIGN,
                    SET_FING_NEW_CELL

12/19/2000  mdl     added field assg_pos in finger status database to indicate
                    the position that a finger is newly assigned to.  Added
                    FING_SOFT_LOCK_RSSI_THRESH

12/14/2000  mdl     included dltri.h for path structure definition
                    included srchset.h for cell struture definition
                    added bitmask contants for interpreting finger lock status
                    added cell and path struct pointers to finger status
                    records
                    added boolean in_soft_lock to finger status record
                    added a bunch of utility macros for accessing finger
                    status record parameters
                    changed phch_deskew to comb_cnt in finger status record
10/19/2000  mdl     added demod_ to the fing_status_dbase, status_dump_info
                    and fing_cfg_shadow variables
8/19/2000   mdl     first cut
===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msm.h"
#include "dltriparm.h"
#include "mdsputil.h"
#include "mdspsync.h"
#include "srchset.h"
#include "seq_v.h"
#include "l1sapcommon.h"
#include "l1const.h"
#include "dltri.h"
#include "whl1mcalif.h"

#include "mcalwcdma_demod.h"
#include "wl1api.h"

/* FEATURE_WCDMA_HS_ONLY_TPC */
#define DL_HS_ONLY_TPC_NUM_SAMPLES_SUBFRAMES   64         /* 64 subframes = 5.33 frames */

#define DL_HS_ONLY_TPC_NUM_SAMPLES_SLOTS           192        /*192 slots = 64 subframes = 5.33 frames */

#define DL_HS_ONLY_TPC_EXIT_TIMER                          30

#define DL_HS_ONLY_TPC_MONITOR_TIMER                     100

#define DL_HS_ONLY_TPC_IMBALANCE_THRESHOLD       demod_hs_only_tpc_db.dl_tpc_imbalance_threshold      //77

#define DL_HS_ONLY_TPC_DUP_THRESHOLD                  demod_hs_only_tpc_db.hs_dec_dup_imbalance_threshold      // 0.4*64

/* If a finger has just been assigned, its weightedNtIoFilt will
   take some time (<10 ms) to stablize. In the interim, SW should
   use Ecp/Io = -20 dbm Nt/Io = 0 db
*/
#define FING_WT_NTIO_FILT_INIT    415
#define NTIO_FILT_GAIN            2048

/*--------------------------------------------------------------------------
              INITIAL FINGER FREQUENCY AND RSSI PARAMETERS
--------------------------------------------------------------------------*/
#define INITIAL_FINGER_ROTATOR 0

/* In Q16 units*/
#define INITIAL_FINGER_RSSI   60

/*In Q16 units, translates to Ec/Io of -29.2dB 
 
This comes from the calculatioin 
RSSI init = (Fing_lock_thresh - 0.0234 * Raw energy threshold)/ 0.9766   
**/ 
#define DEMOD_INIT_FING_RSSI_VIRT_PATH_OFFLINE_PICH(sttd) (((67106 * DL_TRI_COMB_LOCK_THRESH_29) - (1570 * ENG_TO_RSSI(SRCH_REACQ_SRCH_THRESH_ENG(sttd)))) >> 16)

/*--------------------------------------------------------------------------
              NEW FINGER ASSIGNMENT UPDATE MASKS
   These bitmasks are used to populate the update_bmsk field of the 
   finger status database.
--------------------------------------------------------------------------*/
#define FING_ASSIGN_UPDATE_BMSK   0x01
#define FING_PAIR_CFG_UPDATE_BMSK 0x02

#ifdef FEATURE_WCDMA_DC_HSDPA
#define FING_ANT_BIT_POS_MASK 0x1
#endif

/*--------------------------------------------------------------------------
                       FINGER RSSI THRESHOLD FOR TxD
--------------------------------------------------------------------------*/
#define FING_RSSI0_THRESHOLD_TXD_23dB 325
#define FING_RSSI1_THRESHOLD_TXD_30dB 66

/*FEATURE_WCDMA_HS_ONLY_TPC*/
/* These are states of operation of WCDMA L1 */
typedef enum
{
  /*Not NV enabled */
  DL_HS_ONLY_TPC_DISABLED_BY_NV,
  /*NV enabled but HS_ONLY_TPC is off*/
  DL_HS_ONLY_TPC_OFF,
  /*NV enabled and HS_ONLY_TPC is on*/
  DL_HS_ONLY_TPC_ON
} dl_hs_only_tpc_enum_type;

typedef enum
{
  HS_DEC_DTX = 0,
  HS_DEC_PASS = 1,
  HS_DEC_FAIL = 2,
  HS_DEC_DUP = 3,
  HS_DEC_MAX_STAT
} hs_dec_stat_enum_type;

typedef struct
{
  /* Flag to store HS_ONLY_TPC mode*/
  dl_hs_only_tpc_enum_type dl_hs_only_tpc;

  /* TPC stats */
  /* TPC imbalance threshold, programmed by NV */
  uint8 dl_tpc_imbalance_threshold;
  /* number of times tpc_imbalance is observed in last 64 subframes */
  uint8 dl_tpc_imbalance_count;
  /* Total number of TPC bits actually Sampled */
  uint8 dl_tpc_imbalance_curr_idx;
  /* Sample database */
  boolean dl_tpc_imbalance_samples[DL_HS_ONLY_TPC_NUM_SAMPLES_SLOTS];

  uint8 dl_tpc_imbalance_num_samples;

  /* HS stats */
  /* hs decoder imbalance threshold, programmed by NV */
  uint8 hs_dec_dup_imbalance_threshold;
  /* dec stats DB to store HS_ONLY_TPC hs transmissions*/
  uint8 dl_hs_dec_stats[HS_DEC_MAX_STAT];
  /* HS decode samples */
  hs_dec_stat_enum_type dl_hs_dec_stat_samples[DL_HS_ONLY_TPC_NUM_SAMPLES_SUBFRAMES];

  uint8 dl_hs_dec_stat_curr_idx;
  uint8 dl_hs_dec_stat_num_samples;

  /*  T timer for HS TPC exit criteria 
       Once we are in HS only TPC mode, we need to periodically (when this timer expires)
       re-evaluate TPC imbalance and check if we need to exit*/
  uint8 dl_hs_only_tpc_timer;

  /* monitor mode timer variables */
  boolean dl_hs_only_tpc_is_monitor_mode;

  uint8 dl_hs_only_tpc_monitor_timer;
}demod_hs_only_tpc_db_struct_type;

/****************************************************************************
 ****************************************************************************
                 FINGER STATUS AND STATUS DUMP SUPPORT
 ****************************************************************************
 ***************************************************************************/
typedef enum {
  /* the finger is not assigned */
  FING_UNASSIGNED,

  /* the finger is allocated from fing db*/
  FING_ALLOCATED,

  /* the finger assignment is in prog */
  FING_ASSIGNED_IN_PROG,

  /* the finger is assigned and is in lock*/
  FING_ASSIGNED_IN_LOCK,

  /* the finger is assigned but out of lock */
  FING_ASSIGNED_OO_LOCK,

   /* the finger that will get de-assigned */
  FING_MARKED_FOR_DEASSIGN,

  /* the finger deassignment is in prog*/
  FING_DEASSIGN_IN_PROG,

  /* reserved state */
  FING_RESERVED,

  FING_NUM_STATES
} fing_state_enum_type;

/*--------------------------------------------------------------------------
            BIT ASSIGNMENTS FOR LOCK STATUS READ FROM MDSP
--------------------------------------------------------------------------*/
#define FING_LOCK_DETECT_STATE  (0x0001 << BIT_LOCK_DETECT_STATE)
#define FING_TT_LOCK            (0x0001 << BIT_TT_LOCK)
#define FING_COMB_LOCK          (0x0001 << BIT_COMB_LOCK)
#define FING_POWER_LOCK         (0x0001 << BIT_POWER_LOCK)

/*--------------------------------------------------------------------------
                     ENUMERATOR: FING_ADV_RET_ENUM_TYPE

--------------------------------------------------------------------------*/
typedef enum
{
  FING_ADVANCE,
  FING_RETARD
} fing_adv_ret_enum_type;

#define FING_RETARD_VAL           0x2
#define FING_ADVANCE_VAL          0x3
#define FING_ADV_RET_NUM_CX8_VAL  0x3

/*--------------------------------------------------------------------------
                     ENUMERATOR: FING_PAIR_ASSIGN_MODE_ENUM_TYPE

This enumerates the possible paired assignment modes of a finger
--------------------------------------------------------------------------*/
typedef enum {
  /* Unpaired */
  FING_PAIR_MODE_0,
  /* Both fingers of pair are assigned together */
  FING_PAIR_MODE_1,
  /* Finger is attached to pre-existing finger */
  FING_PAIR_MODE_2
} fing_pair_assign_mode_enum_type;

/*--------------------------------------------------------------------------
                       FINGER SOFT LOCK THRESHOLD

A finger who's combined RSSI is greater than this threshold is considered
to be in "soft" lock irregardless of what the hardware lock status is.
--------------------------------------------------------------------------*/
#define FING_SOFT_LOCK_RSSI_THRESH dl_tri_parms.soft_lock_thresh


/* structure to store the prim and div fing rssi*/
typedef struct {
  /*array saving primary pilot rssi*/
  uint16 rssi0[4];
  /*array saving primary pilot rssi*/
  uint16 rssi1[4];
  /*array counter*/
  uint8  array_cntr;
}rssi_for_txd_det_struct_type;


/*--------------------------------------------------------------------------
                    STRUCTURE: FING_STATUS_STRUCT_TYPE

This structure contains status information on a finger and makes up one
entry in the finger status database.
--------------------------------------------------------------------------*/
typedef struct {
  /* cell database index */
  uint8 cell_db_idx;

  /* the state of the finger */
   fing_state_enum_type fing_state;

    /* index into the path array for the path that this finger is currently
     assigned to */
  uint8 path_idx;

  /* index of the ssid*/
  uint8 ssid;

   /* the non-diversity RSSI */
  uint16 rssi0;

  /* the diversity RSSI */
  uint16 rssi1;
  
  /* the PN position of the finger as of last status dump, chipX8 */
  uint32 pos;
  
  /* the PN position of the finger as of last status dump, chipX32 */
  uint32 pos_cx32;
  
  /* the PN position the finger is newly assigned to */
  uint32 assg_pos;

  /*rl_idx and nbr_rl_idx should be removed*/
  /* index to the radio link that this finger is assigned to */
  uint8 rl_idx;

  /* index to the neighbor radio link that this finger is assigned to */
  uint8 nbr_rl_idx;

  /* the Transmit Power Control index */
  uint8 tpc;

  /* set this to TRUE if the pilot this finger uses to demodulate is using
     STTD */
  boolean sttd;

  /* indicates finger is being used for NBCH decode */
  boolean nbch;

  /* the finger rotator value */
  int16 rotator;

  /* pointer to the cell structure of the cell that this finger is
     currently assigned to */
  srch_cell_struct_type *cell;

  /* pointer to the path structure of the cell that this finger is
     currently assigned to */

  /* the lock status of the finger as of last status dump,
     see the above #defines for interpretation of this value or
     use the macros provided below */
  uint16 lock;

  /* whether the finger is in soft lock or not */
  boolean in_soft_lock;

  /* how long the finger has been in lock */
  uint16 lock_age;

  /* how long has the finger been out of lock */
  uint16 oo_lock_age;

  /*Used in OOL finger reslam cases*/
  boolean retain_ool_age;

  /* the PN position of the finger as of the last time it was seen to be in
     lock */
  uint32 last_locked_pos;

  /* the scrambling code the finger is using */
  uint16 pri_scr_code;

  /* indicate hs enabled on this finger */
  boolean hs_enabled;
  /* indicate this fing is HS ref finger or not */
  boolean hs_ref_fing;
  /* HS cell index for this finger */
  uint8 hs_cell_idx;

  /* EUL cell index for this finger */
  uint8 eul_cell_idx;

  /* the OVSF code of the pilot that this finger is using to demodulate
     0 - for the CPICH
     as told by RRC - for a secondary PICH */
  uint16 ovsf;

  /* bitmask indicating:
     FING_ASSIGN_UPDATE_BMSK    0x01
     FING_PAIR_CFG_UPDATE_BMSK  0x02 */
  uint8 update_bmsk;

  /* Value of weighted Nt/Io Filter
     Note: each finger maintains a filter that accumulates
           (CPICH Ec/Io) *(Nt/Io) */
  uint32 weighted_ntio_filt;

  /* indicates this is the first finger on this cell */
  boolean first_finger;

  /* hardware channel index to which this finger is contributing to */
  uint8 hwch;

  /* indicates that the finger is paired with another finger */
  fing_pair_assign_mode_enum_type pair_mode;
  /* index of the finger that this finger is paired with */
  uint8 paired_fing_idx;
  /* jtt enabled*/
  boolean fingerJTTEna;
  /* jft enabled*/
  boolean fingerJFTEna;
  /* mmse enabled*/
  boolean fingerMMSEEna;
  /* rxdInit*/
  boolean rxdInit;
  /* ttl step size (0 – cx8, 1 –Cx32)*/
  uint8 ttlStepSize;
  /* IIR filter tap coefficient for time-critical pilot filter */
  uint16 apfAlpha;

  uint8 vrtCellBpgOffset;

  uint8 cellRscType;

  rssi_for_txd_det_struct_type windowed_demod_rssi_info;

  /*TCPF^2 in Q16 format*/

   /* the non-diversity RSSI */
  uint16 tcpfSq0;

  /* the diversity RSSI */
  uint16 tcpfSq1;

  /*Init RSSI of the finger*/

  uint16 InitRssi;

  /*Indicate if finger start from ftl trk mode or ftl acq mode*/

  boolean ftlTrkInd;

} fing_status_struct_type;


typedef struct
{
    /*DPCH SF*/
  l1_sf_enum_type dpch_sf;
  /* Num Pilot Bits*/
  uint8 num_pilot_bits;
  /*num tpc bits */
  uint8 num_tpc_bits;
  /*num tfci bits*/
  uint8 num_tfci_bits;
 
} demod_dpch_cfg_struct_type;


extern fing_status_struct_type demod_fing_status_dbase[MSMHW_MAX_NUM_FINGERS];

/* the reference finger of an interesting neighbor cell, i.e. a neighbor cell
   whose PCCPCH is currently being demodulated */
extern uint8 demod_nghbr_cell_ref_finger;

/* the reference finger of the serving cell */
extern uint8 demod_serv_cell_ref_finger;

/*--------------------------------------------------------------------------
          UTILITY MACROS FOR USING THE FINGER STATUS DATABASE
--------------------------------------------------------------------------*/
#define SET_FING_STATE(fing, state) \
  (demod_fing_status_dbase[(fing)].fing_state = (state))

#define FING_STATE(fing) \
  (demod_fing_status_dbase[(fing)].fing_state)

#define SET_FING_NEW_ASSIGN(fing) \
  (demod_fing_status_dbase[(fing)].update_bmsk |= FING_ASSIGN_UPDATE_BMSK)

#define IS_ASSIGNED(fing) \
  ((demod_fing_status_dbase[(fing)].fing_state == FING_ASSIGNED_IN_PROG) || \
  (demod_fing_status_dbase[(fing)].fing_state == FING_ASSIGNED_IN_LOCK) || \
  (demod_fing_status_dbase[(fing)].fing_state == FING_ASSIGNED_OO_LOCK))

#define IS_FING_CONFIGURED(fing) \
  ((demod_fing_status_dbase[(fing)].fing_state == FING_ASSIGNED_IN_LOCK) || \
  (demod_fing_status_dbase[(fing)].fing_state == FING_ASSIGNED_OO_LOCK))

#define IS_IN_COMB_LOCK(fing) \
  (demod_fing_status_dbase[(fing)].lock & FING_COMB_LOCK)

#define IS_IN_SOFT_LOCK(fing) \
  (demod_fing_status_dbase[(fing)].in_soft_lock)

#define IS_IN_TTRK_LOCK(fing) \
  (demod_fing_status_dbase[(fing)].lock & FING_TT_LOCK)

#define IS_IN_POWER_LOCK(fing) \
  (demod_fing_status_dbase[(fing)].lock & FING_POWER_LOCK)

#define FING_COMB_RSSI(fing) \
  (demod_fing_status_dbase[(fing)].rssi0 + \
   demod_fing_status_dbase[(fing)].rssi1)

#define FING_PRIM_RSSI(fing) demod_fing_status_dbase[(fing)].rssi0

#define FING_DIV_RSSI(fing) demod_fing_status_dbase[(fing)].rssi1

#define FING_ROTATOR(fing) \
  (demod_fing_status_dbase[(fing)].rotator)

#define FING_LOCK_STATE(fing) \
  (demod_fing_status_dbase[(fing)].lock)

#define HAS_NEW_ASSIGNMENT(fing) \
  (demod_fing_status_dbase[(fing)].update_bmsk & FING_ASSIGN_UPDATE_BMSK)

#define UNLOCK_AGE(fing) \
  (demod_fing_status_dbase[(fing)].oo_lock_age)

#define LOCK_AGE(fing) \
  (demod_fing_status_dbase[(fing)].lock_age)

#define PN_POS(fing) \
  (demod_fing_status_dbase[(fing)].pos)

#define PN_ASSG_POS(fing) \
  (demod_fing_status_dbase[(fing)].assg_pos)

#define FING_CELL_PTR(fing) \
  (demod_fing_status_dbase[(fing)].cell)

#define FING_PRI_SCR_CODE(fing) \
  (demod_fing_status_dbase[(fing)].pri_scr_code)

#define FING_FREQ(fing) \
  (demod_fing_status_dbase[(fing)].cell->freq)
  


#define FING_HS_CELL_IDX_UPDATE_FLAG 0x80

#define FING_HS_ENABLE(fing) \
  (demod_fing_status_dbase[(fing)].hs_enabled)

#define FING_HS_REF_FING_FLAG(fing) \
  (demod_fing_status_dbase[(fing)].hs_ref_fing)

#define FING_HS_CELL_IDX(fing) \
  (demod_fing_status_dbase[(fing)].hs_cell_idx)

#define FING_SET_HS_CELL_IDX_UPDATE_FLAG(fing) \
  ((demod_fing_status_dbase[(fing)].hs_cell_idx) |= (FING_HS_CELL_IDX_UPDATE_FLAG))

#define FING_CLEAR_HS_CELL_IDX_UPDATE_FLAG(fing) \
  ((demod_fing_status_dbase[(fing)].hs_cell_idx) &= (~(FING_HS_CELL_IDX_UPDATE_FLAG)))

#define FING_IS_HS_CELL_IDX_UPDATE_REQD(fing) \
  (((demod_fing_status_dbase[(fing)].hs_cell_idx) & (FING_HS_CELL_IDX_UPDATE_FLAG)) ? TRUE : FALSE)

#define FING_EUL_CELL_IDX(fing) \
  (demod_fing_status_dbase[(fing)].eul_cell_idx)

#define IS_PAIRED(fing) \
  (demod_fing_status_dbase[(fing)].pair_mode != FING_PAIR_MODE_0)

#define PAIRED_FING(fing) \
  (demod_fing_status_dbase[(fing)].paired_fing_idx)

#ifdef FEATURE_WCDMA_DC_HSDPA
#define FING_ANTENNA(fing) \
  ((demod_fing_status_dbase[(fing)].ssid) & FING_ANT_BIT_POS_MASK)
#else
#define FING_ANTENNA(fing) \
  (demod_fing_status_dbase[(fing)].ssid )
#endif

#ifdef FEATURE_WCDMA_DC_HSDPA
#define FING_SSID(carr_idx, antenna)  ((carr_idx << 1) + (antenna))
#else
#define FING_SSID(carr_idx, antenna)  (antenna)
#endif /* FEATURE_WCDMA_DC_HSDPA */

#define IS_RXD_PARAM_UPDATED(fing) \
  (demod_fing_status_dbase[(fing)].rxdInit)

#define SET_RXD_PARAM_UPDATE(fing) \
  (demod_fing_status_dbase[(fing)].rxdInit = TRUE)

#define FING_TPC(fing) \
  (demod_fing_status_dbase[fing].tpc)

/* macro RSSI_TO_ENG() converts a finger RSSI value to an equivalent
   energy */

#define RSSI_TO_ENG(rssi) ((((uint32)9*((uint32)rssi))>>6)+4)
#define ENG_TO_RSSI(eng) ((eng <4)?41:(((eng-4)<<6)/9))
/*--------------------------------------------------------------------------
                STRUCTURE: STATUS_DUMP_INFO_STRUCT_TYPE

This structure contains the status dump information that is not related
to the fingers
--------------------------------------------------------------------------*/
typedef struct {
  /* indicates we lost the status dump */
  boolean lost_status_dump;

  /* the reference counter */
  uint32 ref_cnt;

  /* the TX system time */
  uint32 tx_sys_time;

  /* the 4 combiner counters */
  uint16 comb_cnt[DL_NUM_PHYCHAN];

} status_dump_info_struct_type;

extern status_dump_info_struct_type demod_status_dump_info;


#define DEMOD_FINGER_CONF_MICROSECS_NEEDED 1333
#define DEMOD_FINGER_CONF_MICROSECS_POLLED 67
#define DEMOD_FINGER_CONF_POLLING_RETRIALS 6

#define DEMOD_STATUS_DUMP_MICROSECS_NEEDED 133
#define DEMOD_STATUS_DUMP_MICROSECS_POLLED 67
#define DEMOD_STATUS_DUMP_POLLING_RETRIALS 6

#define STATUS_DUMP_ADJ_REF_CNT(sub) \
  (demod_status_dump_info.ref_cnt >= (sub) ? \
    demod_status_dump_info.ref_cnt - (sub) : \
    CHIPX8_PER_FRAME + demod_status_dump_info.ref_cnt - (sub))

#define STATUS_DUMP_TX_SYS_TIME() \
  ((demod_status_dump_info.tx_sys_time) & 0x7FFFF)

#define STATUS_DUMP_TX_CFN_SHORT() \
  (((demod_status_dump_info.tx_sys_time) & 0x380000) >> 19)

#define STATUS_DUMP_TX_MAX_CFN_SHORT 0x8
#define STATUS_DUMP_CFN_SHORTEN_MASK 0x7

#define RSSI_ECIO_CONV_TABLE_SIZE 64

/*--------------------------------------------------------------------------
                STRUCTURE: DL_COMMON_FING_PARM_STRUCT_TYPE

This structure contains the various parameters common to all of the
demodulator fingers.
--------------------------------------------------------------------------*/
typedef struct {
  /* upper data combiner lock threshold */
  uint16 upper_lock_thresh;

  /* lower data combiner lock threshold */
  uint16 lower_lock_thresh;

  /* soft lock threshold used for triage */
  uint16 soft_lock_thresh;

  /* the power control bit combiner lock threshold */
  uint16 power_lock_thresh;

  /* time tracking constant K1 */
  uint16 ttk1;

  /* time tracking constant K2 */
  uint16 ttk2;


  /* noise estimation filter gain */
  uint16 wntio_est_filt_gain;

  /* the pilot symbol filter gain */
  uint16 pilot_sym_filt_gain;

  /* the frequency tracking loop gain in ACQ mode */
  uint16 freq_trk_loop_gain_acq;

  /* the frequency tracking loop gain in Tracking mode */
  uint16 freq_trk_loop_gain_trk;

} dl_common_fing_parm_struct_type;

extern dl_common_fing_parm_struct_type demod_fing_common_parms;

extern const uint16 rssi_to_ecio_conv_tab [RSSI_ECIO_CONV_TABLE_SIZE];

/******************************************************************************
*******************************************************************************
               MACROS FOR SETTING MDSP ASYNC WRITE PARAMETERS

There are a small variety of demod parameters that are stored in mDSP RAMB,
also referred to as async write parameters since they may be set directly
without having to send a command to the mDSP.  These macros provide a
convenient interface for settings these parameters.
*******************************************************************************
******************************************************************************/

/*--------------------------------------------------------------------------
                MACROS FOR SETTING FINGER LOCK STATUS
--------------------------------------------------------------------------*/
/* set this bit to 1 to enable time tracking */
#define FING_CTRL_TIME_TRACK_ENA  0x0002

/* set this bit to 1 to enable data combining lock detection */
#define FING_CTRL_LOCK_ENA        0x0004

/* set this bit to 1 to enable power control bit combining lock detection */
#define FING_CTRL_PWR_LOCK_ENA    0x0008

/* set this bit to 1 to force a finger into data combining lock */
#define FING_CTRL_FORCE_LOCK      0x0010

/* set this bit to 1 to inhibit a data combining lock */
#define FING_CTRL_INHIBIT_LOCK    0x0020

/*--------------------------------------------------------------------------
        Field Definitions and Macros for Setting Reference Finger Info
--------------------------------------------------------------------------*/
// REF_FING_TTOFFSET
#define REF_FING_TTOFFSET_BMSK  0xFF00
#define REF_FING_TTOFFSET_SHFT  8

// REF_FING_FINGER
#define REF_FING_FINGER_BMSK    0x00FF
#define REF_FING_FINGER_SHFT    0

#define WL1_DEMODDRV_MSG(str, a, b, c) \
if (wl1demoddrv_debug_msg_enabled) \
{ \
  MSG_HIGH(str, a, b, c); \
}\
else \
{ \
  MSG_LOW(str, a, b, c); \
}

#define WL1_DEMODDRV_MSG_6(str, a, b, c, d, e, f) \
if (wl1demoddrv_debug_msg_enabled) \
{ \
  WL1_MSG6(HIGH,str, a, b, c, d, e, f); \
}\
else \
{ \
  WL1_MSG6(LOW,str, a, b, c, d, e, f); \
}

#define WL1_DEMODDRV_MSG_9(str, a, b, c, d, e, f, g, h, i) \
if (wl1demoddrv_debug_msg_enabled) \
{ \
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e, f, g, h, i);\
}\
else \
{ \
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e, f, g, h, i);\
}

#define WL1_DEMODDRV_MSG_6(str, a, b, c, d, e, f) \
if (wl1demoddrv_debug_msg_enabled) \
{ \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d, e, f);\
}\
else \
{ \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d, e, f);\
}

/******************************************************************************
*******************************************************************************
              Macros for reading mDSP Async Read Variables
*******************************************************************************
******************************************************************************/


/*--------------------------------------------------------------------------
Macro for reading the Frequency Error Estimate combined over all fingers
currently in data combiner lock
--------------------------------------------------------------------------*/
#define MDSP_READ_COMBINED_FREQ_EST() \
  mcalwcdma_demod_get_freqcommon()

/*--------------------------------------------------------------------------
                Macro for reading the demodulated PI bit
--------------------------------------------------------------------------*/

/* macro to read the PICH hard decision value */
#define MDSP_READ_PICH_PI_VALUE()                        \
  mcalwcdma_fwsw_intf_addr_ptr->demodInfo.pichDemodInfo. \
    result[0].indValue

/* macro to get the PICH accumulation.  This is a 32 bit value */
#define MDSP_READ_PICH_ACCUM()                           \
  mcalwcdma_fwsw_intf_addr_ptr->demodInfo.pichDemodInfo. \
    result[0].indSum

/* macro to write the PICH detection threshold */
#define MDSP_WRITE_PICH_PI_THRESH(pi_detect_thresh)      \
  mcalwcdma_fwsw_intf_addr_ptr->demodInfo.pichDemodInfo. \
    input.indThreshold = (int32)pi_detect_thresh

/* macro to write the PICH paging indicator PN count */
#define MDSP_WRITE_PICH_PI_PN_CNT(val)             \
  mcalwcdma_fwsw_intf_addr_ptr->demodInfo.pichDemodInfo. \
    input.numInd = 1;                                    \
  mcalwcdma_fwsw_intf_addr_ptr->demodInfo.pichDemodInfo. \
    input.indPnCount[0] = val;

/* macro to write the number of bits in a PICH paging indicator
** This gets set elsewhere, so just define it away. */
#define MDSP_WRITE_PICH_BITS_PER_PI(bits_per_pi)

/*--------------------------------------------------------------------------
       Macro to convert scrambling code to primary scrambling code
--------------------------------------------------------------------------*/
#define SCR_CODE_TO_PRI_SCR_CODE(scr) ((scr) >> 4)
#define SCR_CODE_TO_SEC_SCR_CODE(scr) ((scr) % 16)

#define PRI_SCR_CODE_TO_SCR_CODE(pri_scr) ((uint16)((pri_scr) << 4))
#define SCR_CODE(pri, sec) (((pri) << 4) + (sec))

/*--------------------------------------------------------------------------
                    STRUCTURE: FING_ASSIGN_PARMS_STRUCT_TYPE

This structure contains the finger common, assign, and paired config
parameters of the finger config command.
--------------------------------------------------------------------------*/
typedef struct
{
  /* finger to be assigned or otherwise modified */
  uint8 fing;

  dl_tri_fing_info_struct_type fing_assign_parms;
 
} fing_assign_parms_struct_type;

typedef struct
{
  /**header enum   */
  l1_local_cmd_hdr_type hdr;
  /* nothing for now*/
}dl_mcal_cfg_cmd_type;


#define WL1_DEMODDRV_MAX_NUM_DSD_CLIENTS      4

extern rex_crit_sect_type wl1_demoddrv_fing_assign_info_crit_sect;

/** MACROS for mutex lock and mutex free */
#define WL1_DEMODDRV_DSD_CLIENT_INFO_INTLOCK()  REX_ISR_LOCK(&wl1_demoddrv_dsd_client_info_crit_sect)
#define WL1_DEMODDRV_DSD_CLIENT_INFO_INTFREE()  REX_ISR_UNLOCK(&wl1_demoddrv_dsd_client_info_crit_sect)
#define WL1_DEMODDRV_FING_ASSIGN_INFO_INTLOCK()  REX_ISR_LOCK(&wl1_demoddrv_fing_assign_info_crit_sect)
#define WL1_DEMODDRV_FING_ASSIGN_INFO_INTFREE()  REX_ISR_UNLOCK(&wl1_demoddrv_fing_assign_info_crit_sect)

typedef void (*WL1_DEMODDRV_DSD_CLIENT_FUNC_CB_PTR)(void);

/* enum type describing the type of dsd needed*/
typedef enum
{
  /* in this mode the finger statistics are updated without send a dsd command
  to firmware. FW autonomously updates finger statistics after pich/aich demod
  completion*/
  DEMOD_STATUS_DUMP_AUTO_MODE,
  /* regular dsd command where the caller supplies callback pointer. The cb
  pointer is a function that is called to resume processing on completion f
  dsd*/
  DEMOD_STATUS_DUMP_REGULAR_MODE
}demod_status_dump_cmd_enum_type;

/*dsd request struct*/
typedef struct
{
  /*dsd mode */
  demod_status_dump_cmd_enum_type dsd_mode;
  /*cb pointer to be called on completion of dsd*/
  WL1_DEMODDRV_DSD_CLIENT_FUNC_CB_PTR dsd_clients_cb_ptr;
}demod_status_dump_cmd_request_struct_type;



typedef struct
{
  boolean enabled;
  /* array to hold */
  WL1_DEMODDRV_DSD_CLIENT_FUNC_CB_PTR dsd_clients_cb_ptr;
}wl1_dsd_clients_struct_type;

typedef struct
{
  /*current allocated idx */
  uint8 curr_allocated_idx;
  /*current idx under servicing */
  uint8 curr_servicing_idx;
  /* list of client cb ptrs*/
  wl1_dsd_clients_struct_type dsd_client_info[WL1_DEMODDRV_MAX_NUM_DSD_CLIENTS];
}wl1_demoddrv_dsd_client_info_struct_type;

typedef struct
{
  /**header enum   */
  l1_local_cmd_hdr_type hdr;
  /* nothing for now*/
}wl1_demoddrv_dsd_done_cmd_type;

/****************************************************************************
 ****************************************************************************
                            FUNCTION PROTOTYPES
 ****************************************************************************
 ***************************************************************************/
/*===========================================================================
FUNCTION        DEMOD_INIT

FILE NAME       wl1demoddrv.h

DESCRIPTION     This function initializes the demod module.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
void demod_init(void);

/*===========================================================================
FUNCTION        demod_reset_status_dump_clients

FILE NAME       wl1demoddrv.h

DESCRIPTION     This function initializes the demod status dump variables.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
void demod_reset_status_dump_clients(void);
   
/*===========================================================================
FUNCTION        DEMOD_INIT_FING_STATUS_DBASE

FILE NAME       demoddrv.c

DESCRIPTION     This function initializes the finger status database

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The finger status database is set to default initial values
===========================================================================*/
void demod_init_fing_status_dbase(void);

/*===========================================================================
FUNCTION        DEMOD_IS_AT_LEAST_ONE_FINGER_ASSIGNED

FILE NAME       demoddrv.c

DESCRIPTION     This function returns whether or not at least one finger in
                the status database is assigned.

DEPENDENCIES    None

RETURN VALUE    TRUE if at least one finger is assigned.
                FALSE if all fingers are unassigned.

SIDE EFFECTS    None
===========================================================================*/
extern boolean demod_is_at_least_one_finger_assigned(void);

/*===========================================================================
FUNCTION        DEMOD_IS_AT_LEAST_ONE_FINGER_IN_LOCK

FILE NAME       demoddrv.c

DESCRIPTION     This function returns whether or not at least one finger in
                the status database is in lock.

DEPENDENCIES    None

RETURN VALUE    TRUE if at least one finger is in lock.
                FALSE if no fingers are in lock.

SIDE EFFECTS    None
===========================================================================*/
extern boolean demod_is_at_least_one_finger_in_lock(void);

/*===========================================================================

FUNCTION        INIT_FING_CFG_SHADOW

DESCRIPTION     This function zeroes out the finger config record.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The finger config record will be overwritten with 0's.

===========================================================================*/
void demod_init_fing_cfg_shadow(void);

/*===========================================================================
FUNCTION        DEMOD_SEND_FINGER_CFG_CMD

FILE NAME       demoddrv.c

DESCRIPTION     This function sends a finger config mDSP command and quits.

DEPENDENCIES    Assumes the mDSP is alive enough to perform the status dump
                command.

RETURN VALUE    TRUE or FALSE if or not the command has been sent successfully.

SIDE EFFECTS    A mDSP finger config command is sent.
===========================================================================*/
boolean demod_send_finger_cfg_cmd(void);

/*===========================================================================
FUNCTION        DEMOD_STATUS_DUMP

DESCRIPTION     This function takes an input client ptr and then adds it ot a
                stack and calls a fw routine to return us finger details

DEPENDENCIES    Assumes the mDSP is alive enough to perform the status dump
                command.
                DONT DONT do a demod status dump in INTERRUPT/IST context.

RETURN VALUE    TRUE or FALSE if or not the command has been sent successfully.

SIDE EFFECTS    The finger status database and the status dump info table
                will be updated with fresh information.
===========================================================================*/
extern boolean demod_status_dump(
  /* dsd client cb ptr*/
    demod_status_dump_cmd_request_struct_type *dsd_req_ptr);

/*===========================================================================
FUNCTION        DEMOD_STATUS_DUMP_LITE

DESCRIPTION     This function reads the fw demod status dump buffer and returns
                the results

DEPENDENCIES    This will give updated finger info only when called after pich
                demod done isr & rach done isr for now. when these isrs complete
                firmware asynchronously(without a demod status dump command from sw)
                updates the shared mem buffer with the new finger stats in other 
                cases calling this function will return the finger stats that were 
                updated due to the last demod status dump command. 

RETURN VALUE    None

SIDE EFFECTS    The finger status database and the status dump info table
                will be updated with fresh information.
===========================================================================*/
extern void demod_status_dump_lite(void);

/*===========================================================================
FUNCTION        mcalwcdma_demod_post_dsd_done_cmd

DESCRIPTION     This function posts a local command following a dsd command to
                dsp firmware 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mcalwcdma_demod_post_dsd_done_cmd(void);

/*===========================================================================
FUNCTION        DEMOD_SET_FING_COMMON_PARMS

FILE            demoddrv.c

DESCRIPTION     This function sets the common finger parameters including
                lock thresholds, filter gains, etc.

DEPENDENCIES    The finger common parameter value table

RETURN VALUE    None

SIDE EFFECTS    The finger parameters are updated in the mDSP
===========================================================================*/
void demod_set_fing_common_parms(void);

/*===========================================================================

FUNCTION        DEMOD_ASSIGN_FINGER

DESCRIPTION     This routine updates the finger config record shadow so as
                to assign a finger as specified by the input parameters.
                Note that access to individual finger fields is also available
                via macros specified in l1_demod.h.

DEPENDENCIES

RETURN VALUE    TRUE or FALSE depending upon if the operation succeeded.

SIDE EFFECTS

===========================================================================*/
boolean demod_assign_finger(fing_assign_parms_struct_type *parms);

/*===========================================================================
FUNCTION        DEMOD_ASSIGN_TPC_FOR_FINGER

FILE            demoddrv.c

DESCRIPTION     This routine updates the finger config record shadow only for
                tpc field of a finger. The function demod_send_finger_cfg_cmd()
                needs to be called after this.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The finger config record is updated
===========================================================================*/
extern void demod_assign_tpc_for_finger(
  /* finger to be assigned or otherwise modified */
  uint8 fing,
  /* the Transmit Power Control index */
  uint8 tpc);

/*===========================================================================
FUNCTION        DEMOD_DISABLE_FINGER

FILE            demoddrv.c

DESCRIPTION     This function updates the finger config record and various
                bookkeeping status variables to effect the disable of a
                finger.

                This function DOES NOT issue the mDSP command to do the
                actual finger update.  This is deferred so that multiple
                fingers can be disabled using a single mDSP command.

DEPENDENCIES    None

RETURN VALUE    TRUE or FALSE depending upon if the operation succeeded.

SIDE EFFECTS    The finger config record shadow and the finger status
                database are updated
===========================================================================*/
boolean demod_disable_finger(uint8 fing);

/*===========================================================================
FUNCTION        DEMOD_SET_TPC_THRESHOLD

FILE NAME       demoddrv.c

DESCRIPTION     This function calculates and sets up and down TPC thresholds
                into firmware registers.

DEPENDENCIES    finger database.

RETURN VALUE    None

SIDE EFFECTS    None.
==========================================================================*/
void demod_set_tpc_threshold(boolean efdpch_enable, wl1_ul_carr_id_enum_type carr_idx);


/*===========================================================================
FUNCTION        GET_FING_ECIO

FILE NAME       demoddrv.c

DESCRIPTION     This function computes the finger ecio

DEPENDENCIES    None

RETURN VALUE    Ec/Io

SIDE EFFECTS    None
===========================================================================*/
extern int16 get_fing_ecio(void);

/*===========================================================================
FUNCTION        GET_FING_ECIO_FOR_RSCP

FILE NAME       demoddrv.c

DESCRIPTION     This function computes the CPICH RSCP from primary antenna fingers only

DEPENDENCIES    None

RETURN VALUE    Ec/Io

SIDE EFFECTS    None
===========================================================================*/
extern int16 get_fing_ecio_for_rscp(void);

/*===========================================================================

FUNCTION FING_RSSI_TO_ECIO

DESCRIPTION
  This function converts an energy into the 6-bit Ec/Io value required
  for pilot strength measurements. It works by doing a binary search on
  the ecio_tab to find the closest value (without going over), and returning
  the index in the ecio_tab.

DEPENDENCIES
  None.

RETURN VALUE
  6-bits Ec/Io value. If the energy is out of range,  the return value is
  clipped to 0 or 63.

SIDE EFFECTS   None.

===========================================================================*/

extern uint16  fing_rssi_to_ecio
(
  word eng,
    /* The energy to be converted */

  const uint16 ecio_tab[]
    /* Pointer to the conversion table to be used */
);

/*===========================================================================

FUNCTION mcalwcdma_demod_cfg_cmd_done

DESCRIPTION
  This function handles the isr from mcal and routes the results to cell and finger
  config modules. If there is any pending config in mcal shadow this function 
  posts a local command to commit the pending configuration. It also signals if
  there an error had been signalled by dsp for this config. The errors are also
  routed to cell config and triage modules for propagation upstream

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  A local command may be posted

===========================================================================*/
extern void mcalwcdma_demod_cfg_cmd_done(
  /** fingers completed bmsk */
  uint32 fing_done_bmsk,
  /** cell completed bmsk */
  uint16 cell_done_bmsk, 
  /** additional commit needed for pending configs */
  boolean commit_needed,
  /** status of the command */
  mcal_status_type ret_status);
/*===========================================================================

FUNCTION demoddrv_commit_pending_demod_config

DESCRIPTION
  This function calls mcal commit to flush any pending mcal demod commands

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void demoddrv_commit_pending_demod_config(void);

/*===========================================================================

FUNCTION demoddrv_retrieve_tcxoaccum

DESCRIPTION
  This function tretrieves accum tcxo adjust value

DEPENDENCIES
  None.

RETURN VALUE
  int32 accum tcxo adjust value

SIDE EFFECTS
  None
===========================================================================*/
extern int32 demoddrv_retrieve_tcxoaccum(void);


/*===========================================================================
FUNCTION        demod_status_dump_cmd_done_handler

DESCRIPTION     This function is the lcoal command handler that handles 
                the WL1_DEMODDRV_DSD_DONE_CMD local command. All clients
                who have registered for dsd info will be called.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void demod_status_dump_cmd_done_handler(void);

/*===========================================================================

FUNCTION demod_syncup_finger_cell_info

DESCRIPTION
  This function changes the cell pointer that the finger db holds to the input
  ptr

DEPENDENCIES
  None.

RETURN VALUE
  Returns the updated fing enable bmask for the particular cell.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 demod_syncup_finger_cell_info(srch_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION        DEMOD_GET_COMB_FING_RSSI

DESCRIPTION     This function computes the combiner RSSI of all the fingers
                on the primary carrier

DEPENDENCIES    None

RETURN VALUE    RSSI

SIDE EFFECTS    None
===========================================================================*/
extern uint32 demod_get_comb_fing_rssi(void);

/*===========================================================================
FUNCTION        demod_check_fing_rssi_diff_for_txd_det

DESCRIPTION     This function calculates the TxD status of the cell based on 
                the prim and div fing rssi

DEPENDENCIES    None

RETURN VALUE    TxD status

SIDE EFFECTS    None
===========================================================================*/
extern uint8 demod_check_fing_rssi_diff_for_txd_det(uint8 cell_idx);

/*===========================================================================
FUNCTION        DEMOD_SAVE_DPCH_CONFIG

DESCRIPTION     This function saves the dpch cfg params

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void demod_save_dpch_config(demod_dpch_cfg_struct_type *inp);

/*===========================================================================
FUNCTION        DEMOD_REVERT_FING_CFG

FILE            demoddrv.c

DESCRIPTION     This function updates the finger config record and various
                bookkeeping status variables to effect the revert of a
                finger. This function also reverts back the prepared config
                in the mcalwcdma_demod interface as well.
 
                Used in very specific conditions.

                

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The finger config record shadow and the finger status
                database are updated
===========================================================================*/
extern boolean demod_revert_finger_cfg(uint8 fing);

/*===========================================================================

FUNCTION dl_set_wl1demoddrv_nv_values

DESCRIPTION
  Function controls enabling of debug F3s for wl1demoddrv

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_set_wl1demoddrv_nv_values(void);

#ifdef FEATURE_WCDMA_HS_ONLY_TPC
/*===========================================================================
FUNCTION             demod_enter_hs_only_tpc

FILE                     demoddrv.c

DESCRIPTION        This function enables HS only TPC mode in FW
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS        None
===========================================================================*/
extern void demod_enter_hs_only_tpc(void);

/*===========================================================================
FUNCTION             demod_exit_hs_only_tpc

FILE                     demoddrv.c

DESCRIPTION        This function disables HS only TPC mode in FW
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS        None
===========================================================================*/
extern void demod_exit_hs_only_tpc(void);

/*===========================================================================
FUNCTION             demod_hs_tpc_imbalance_stat

FILE                     demoddrv.c

DESCRIPTION        This function updates HS only TPC database with new TPC imbalance stat
                           Currently it gets called only from CRC status command while will compute TPC imbalance 
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS        None
===========================================================================*/
extern void demod_hs_tpc_imbalance_stat(boolean imbalance_detected);

/*===========================================================================
FUNCTION             demod_hs_tpc_imbalance_stat

FILE                     demoddrv.c

DESCRIPTION        This function updates HS only TPC database with new HS decode stat per subframe basis (DUP/PASS/FAIL)
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS        None
===========================================================================*/
extern void demod_hs_decoded_stat(hs_dec_stat_enum_type hs_dec_stat);

/*===========================================================================
FUNCTION             demod_hs_throughput_loss_detected

FILE                     demoddrv.c

DESCRIPTION        This function checks if HS DUP count in last 64 subframes is over the threshold.
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      TRUE: 1) We have atleast 64 HS decoder samples logged in out database and
                                    2) We have HS DUP count greater than the threshold
                           FALSE: if any of the above two conditions are not met

SIDE EFFECTS        None
===========================================================================*/
extern boolean demod_hs_throughput_loss_detected(void);

/*===========================================================================
FUNCTION             demod_hs_tpc_imbalance_detected

FILE                     demoddrv.c

DESCRIPTION        This function checks if TPC imbalance count in last 192 slots is over the threshold.
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      TRUE: 1) We have atleast 192 TPC samples logged in out database and We have TPC count greater than the threshold
                                                      OR
                                    2) We are in Monitor mode and we have half of the TPC imbalance needed
                           FALSE: if both of the above two conditions are not met

SIDE EFFECTS        None
===========================================================================*/
extern boolean demod_hs_tpc_imbalance_detected(void);

/*===========================================================================
FUNCTION             demod_init_hs_tpc_db

FILE                     demoddrv.c

DESCRIPTION        Purges samples collected in HS only TPC database and disables HS only TPC mode in FW if it is enabled.
                           Currently this function is called during HS repointing, HS enable and disable
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS        None
===========================================================================*/
extern void demod_init_hs_tpc_db(void);

/*===========================================================================
FUNCTION             demod_check_hs_only_tpc

FILE                     demoddrv.c

DESCRIPTION        Checks entry and exit criteria of HS only TPC mode and enables/disable HS only TPC mode in FW whenever needed
                           This function is called every frame from dl_trblk_crc_status_cmd after TPC imbalance stats are updated in demod_hs_only_tpc_db
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS        None
===========================================================================*/
extern void demod_check_hs_only_tpc(void);

/*===========================================================================
FUNCTION             demod_is_hs_only_tpc_enabled_by_nv

FILE                     demoddrv.c

DESCRIPTION        API to check if HS only TPC mode is enabled by NV
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      BOOLEAN

SIDE EFFECTS        None
===========================================================================*/
extern boolean demod_is_hs_only_tpc_enabled_by_nv(void);

/*===========================================================================
FUNCTION             demod_is_hs_only_tpc_mode_off

FILE                     demoddrv.c

DESCRIPTION        API to check if HS only TPC mode is disabled
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      BOOLEAN

SIDE EFFECTS        None
===========================================================================*/
extern boolean demod_is_hs_only_tpc_mode_off(void);

/*===========================================================================
FUNCTION             demod_get_hs_serving_fing_tpc_idx

FILE                     demoddrv.c

DESCRIPTION        Thsi function queries finger database and returns TPC index of HS serving cell

DEPENDENCIES      None

RETURN VALUE      UINT8 value of TPC index of HS serving cell. MCALWCDMA_MAX_TPC_INDEX if there is no HS serving cell found

SIDE EFFECTS        None
===========================================================================*/
extern uint8 demod_get_hs_serving_fing_tpc_idx(void);
#endif /*FEATURE_WCDMA_HS_ONLY_TPC*/

/*===========================================================================
FUNCTION             demod_is_hs_only_tpc_mode_on

FILE                     demoddrv.c

DESCRIPTION        API to check if HS only TPC mode is enabled
                           Should be called only in L1 task contect

DEPENDENCIES      None

RETURN VALUE      BOOLEAN

SIDE EFFECTS        None
===========================================================================*/
extern boolean demod_is_hs_only_tpc_mode_on(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION             wl1_qsh_demod_dump_cb

FILE                     demoddrv.c

DESCRIPTION        This function fill in the DEMOD cmd related info for QSH

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS        None
===========================================================================*/
extern void wl1_qsh_demod_dump_cb(void *write_ptr);
#endif /* FEATURE_QSH_DUMP */
#endif  /* #ifndef WL1DEMODDRV_H     */
