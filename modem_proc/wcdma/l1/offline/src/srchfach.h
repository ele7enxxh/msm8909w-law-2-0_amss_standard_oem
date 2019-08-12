#ifndef SRCHFACH_H
#define SRCHFACH_H
/*===========================================================================

  S R C H    F A C H    S T A T E   R E L A T E D   D E C L A R A T I O N S

DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2001-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchfach.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/15   ap      Initial check-in for WL1 QSH framework 
05/27/15   nga     Updating the log packet 0x4721 with correct H/C-RNTI
11/19/14   gsk     FACH FMO log packet and adding support for 0x4027 for FE-FACH
11/17/14   psr     Moved srchcrutils_find_highest_priority_rat to srchcrutils
10/03/14   rsr     Added Changes to ensure that RxD avoids collision with FMO
                   in FACH State.
10/03/14   gsk     Resume FMO only after all clients requested for resuming the FMO.
09/05/14   cc      Shift GSM measurement timline by 1 slot earlier for DSDA
08/01/14   rs      Fixed compiler warnings 
07/01/14   ar      extern function srchfach_check_cell_ref_age_and_trim_mset_uset
06/19/14   cc      IRAT DR-DSDS check in
05/30/14   dp      W2W restructure with mult step1 peak search in pn-acq gaps
05/06/14   ar      Prototype for querying channel SFN
04/27/14   jkb     Add check for Feature dual sim with feature qta
04/11/14   mm      Added support for updating interf timers
04/04/14   gsk     Change prototype of srchfach_inter_f_do_meas to include srch type
04/03/14   ar      Cleanup the macro SRCHFACH_NONDRX_FACH
03/27/14   mk      Removed featurization for SRCH_FACH_QTA
03/10/14   ks      Bolt FED API checkin
01/31/14   mm      Updated cleanup callback function for W2G FACH module cleanup
01/30/14   ar      Add macros for enabling InterF and GSM FMO's in FACH
01/24/14   as      Adding reason for FACH QTA cancellation in the debug message.
01/21/14   ar      Add support for handling QICE during W2L measurements in FACH
01/07/14   ar      Change srchfach_gsm_proc_meas_cmds to return a boolean
01/07/14   amj     Clean up old ASET update code.
01/06/14   sr      Suspending periodic ops before CPHY req processing
12/28/13   ar      Define SRCHFACH_NONDRX_FACH
12/15/13   ar      FEFACH (FR2759) absolute priority measurements in FACH
11/27/13   hk      Unification of non-intra module cleanup procedures
11/15/13   ar      FEFACH (FR2759) absolute priority measurements in FACH - ranking and evals
10/31/13   ar      Removed nastt unmon peak processing function and moved to srchcrmeas
10/28/13   ar      Removed code under FEATURE_DL_IGNORE_MODE_FACH_MEAS
10/25/13   ar      Introduce FMO suspend resume support
10/19/13   ar      Support for FEFACH (FR2759) absolute priority measurements in FACH
10/14/13   cc      Fix wrong cell into RECONF list issue
09/29/13   as      Continuing port of Dual Sim from Triton to Dime.
09/23/13   as      Porting Dual Sim from Triton to Dime.
09/12/13   jd      Searcher redesign
08/12/13   cc      DBDC CM W2W feature check in
08/08/13   cc      Fixed compilation errors
08/01/13   cc      Add new cb system to avoid FACH cleanup race condition
03/26/13   ar      Skip the GSM FMO if the local cmd handling for FMO is not handled in the same frame
01/25/13   sr      Changes to set intraf FMO gap frames correctly during non-edrx mode cleanup
09/10/12   stk     Added support for Cell reselection optimizations with HSFACH/HSRACH
11/28/12   amj     MCVS feature related changes   
12/12/12   sr      Fixed compiler errors
08/27/12   sr      Backing up srchfach substate for resuming searches
08/14/12   ms      Updated the protype for srchfach_get_sccpch_offset
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/27/12   hk      Added support to vote for RF ON during SIB decode
06/11/12   vs      MBMS feature cleanup.
06/04/12   stk     Also added support for HSFACH infinite reservations.
                   Added API to query for HSFACH reservation.
05/15/12   ms      Added enum SRCH_FACH_HSDPA
04/13/12   cc      Adding an additional parameter oksn_total_qs in srchfach_gsm_meas_parms_struct_type to count the gap length in qs
04/12/12   stk     Added support to send FMO GSM cmd 1 frame before to FW, when HS FACH is configured.
03/27/12   hk      Added support for FEATURE_WCDMA_HS_FACH_DRX
02/08/12   vs      Feature cleanup.
01/10/12   sj      Added support for RxD in FACH signaling
04/28/11   yh      Added RxLM support for FMO
11/09/10   stk     Move the FACH mutex initialization to WL1_TASK init, to prevent collision from fach_init & 
                   SCCPCH_evt IST that occurs at every frame boundry .
06/21/10   uk      Compiler warning fix
05/03/10   sv      Fixed compiler warnings.
08/14/09   uk      Added new API to indicate when GSM measurments ongoing in FACH.
06/15/09   uk      Support for sending START_FACH_MEAS cmd to fw before 
                   starting a FMO gap for GSM meas
05/08/09   sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                   srchlog.h to wdec.h, wenci.h, wl1m_v.h, wsrch.h, wsrchi.h, wsrchlog.h
04/22/09   stk     Compiler warning fixes
12/09/08   uk      Mainling FEATURE_WCDMA_DL_ENHANCED, FEATURE_ENHANCED_CELL_RESELECTION, 
                   FEATURE_WCDMA_DRX_MANAGER, FEATURE_WCDMA_OFFLINE_SEARCHER 
                   and FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT 
10/03/08   hk      Bringing in 7k Mailine fixes
10/26/07   sv      Added searcher support for FEATURE_WCDMA_DL_ENHANCED.
07/25/07   sk      Added the variable num_gaps_blocked to indicate number of gaps
                   blocked by RACH
05/05/07   sk      Revert the previous checkin.
04/04/07   sk      Added extern for srch_fach_cleanup_pending under FEATURE_T_BARRED_SUPPORT.
01/23/07   sk      Function definition to get FMO if was done in a sfn.
01/08/07   sk      Add fn definition for SRCHFACH_GSM_GET_NEXT_NSET_FOR_RSSI
11/29/05   ub      Add fn definition for querying FMO inter reservation
11/02/05   ub      Make srchfach_gsm_meas_cleanup_pending global 
08/18/05   gv      Added proto for the function srchfach_inter_f_list_srch_cmd()
06/16/05   rc      Added proto srchfach_update_sfn_sfn_val().
06/08/05   cpe     Added support for calculating and reporting SFN-SFN difference 
                   in FACH.
04/22/05   yiz     Distinguish SFN ctr and ref_sfn for FACH inter-RAT schedule
04/15/05   yiz     At AICH ACK, change infinite FACH reserv for RACH to finite.
                   Change FACH reserv for inter-F SW advance from 1 to 3 frames.
12/03/04   yiz     Add T_identify_GSM and T_reconfirm_GSM to FACH GSM globals.
10/07/04   sh      Added proto srchfach_cm_init().
09/22/04   sh      Added proto srchfach_wait_for_sib_mod_update().
08/16/04   yiz     Simplify scheduling of FACH GSM BSIC ID cells.
05/10/04   gs      Updated externalization of function srchfach_inter_f_do_meas
                   Externalized function srchfach_interf_proc_meas
04/21/04   cpe     Changed srchfach_process_immediate_meas_req to include 
                   RACH measurement quantity
04/15/04   yiz     Separate features for FACH meas oksn and FACH inter-RAT CR
04/08/04   yiz     Mod FACH meas oksn to support InterFreq.
03/19/04   yiz     Add srchfach_cleanup_oksn_info().
02/27/04   yiz/src Brought over field changes for FACH-GSM cleanup.
12/23/03   src     Added extern declarations for srchfach_aset_srch_aborted and
                   srchfach_aset_srch_done.
12/15/03   yiz     Add interface to suspend/resume FACH GSM meas upon BCH 
                   setup/drop through FACH meas reservation manager.
12/10/03   yiz     Add FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
11/12/03   sh      Added substates for inter-frequency searching in Idle.
07/28/03   sh      Defined macro SRCHFACH_IS_MEAS_OCCASION_SPECIFIED() to
                   return false since measurement occasions are not supported.
03/12/03   sh      Added proto srchfach_proc_srch_done().
01/31/03   sh      Created File.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "srchcmd.h"
#include "srchset.h"
#include "l1rrcif.h"
#include "wl1m.h"
#include "l1mathutil.h"
#include "srchgsmdrv.h"
#include "srchsched.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*Max number of FMO resume callbacks we support now*/
#define SRCHFACH_MAX_FMO_CB_CNT 2

/*--------------------------------------------------------------------------
                          FACH CRITICAL SECTION
--------------------------------------------------------------------------*/
typedef rex_crit_sect_type srchfach_crit_sect_type;
extern srchfach_crit_sect_type srchfach_crit_sect;

/*--------------------------------------------------------------------------
                          FACH SEARCH SUBSTATES
--------------------------------------------------------------------------*/
typedef enum
{
  SRCHFACH_INIT,           /* Initialization */
  SRCHFACH_INACTIVE,       /* No search activity */
  SRCHFACH_ASET,           /* ASET Search */

  SRCHFACH_ASET_NBCH,      /* ASET update search */
  SRCHFACH_NSET_STEP1,     /* NSET Step 1 search */
  SRCHFACH_NSET_NASTT,      /* NSET NASTT search */
  SRCHFACH_NSET_LIST,      /* NSET List search   */

  /* NSET Inter-frequency cell reselection substates */
  SRCHFACH_OF_NSET_LIST_CR, /* List search   */
  SRCHFACH_OF_STEP1_CR,     /* Step 1 search */
  SRCHFACH_OF_STEP_NASTT_CR,     /* Step 2 search */

  SRCHFACH_DRX_RX_INACTIVE,
  SRCHFACH_DRX_SLEEP,
  
  SRCHFACH_STATE_MAX        /* Last item in enum. */

} srchfach_substate_enum_type;

/* FACH search substate */
extern srchfach_substate_enum_type  srchfach_substate;

/* Backed up snapshot of substate for resuming searches */
extern srchfach_substate_enum_type  srchfach_prev_substate;

extern srch_fach_meas_oksn_info_type srchfach_fmo_log_pkt_info[20];
extern uint8 srchfach_fmo_log_pkt_idx;


/*--------------------------------------------------------------------------
                          FACH non-intra-F measurements
--------------------------------------------------------------------------*/
/* 
 * Number of frames ahead of FACH measurement occasion for SW to schedule
 * WCDMA inter-frequency measurement
 * MUST be smaller or equal to SRCHFACH_MEAS_OKSN_SW_ADVANCE_MAX
 */

#define SRCHFACH_MEAS_OKSN_SW_ADVANCE_MAX 1

/* when in DSDA and FE-FACH, open gap 1 slot earlier */
#define SRCHFACH_RECEIVER_SWITCH_GAP_ADV_SLOT -1

#ifdef FEATURE_WCDMA_HS_FACH
extern uint8 srchfach_if_fmo_fail_cnt;
extern uint8 srchfach_gsm_fmo_fail_cnt;
#endif 
extern uint8  srchfach_s_fail_cnt;
/* For HSFACH/HSRACH: Counter to reset Tresel for all IF,GSM neighbors if FMOs
  have been blocked by RACH. This T_delay is used to calculate n_fmo_skip */
#define T_DELAY_N_FMO_SKIP_RACH 750 /* 750 ms */

/* FACH measurement occasion system types */
typedef enum
{
  SRCHFACH_MEAS_OKSN_SYS_NONE  = 0x0,
  SRCHFACH_MEAS_OKSN_SYS_FDD   = 0x2,
  SRCHFACH_MEAS_OKSN_SYS_GSM   = 0x4, 
	SRCHFACH_MEAS_OKSN_SYS_LTE   = 0x8,/*Adding LTE support for FE-FACH*/
} srchfach_meas_oksn_sys_enum_type;

#define SRCHFACH_MEAS_OKSN_SYS_NUM 3

/* Cycle table for inter-meas_oksn_system scheduling of FACH measurements */
extern srchfach_meas_oksn_sys_enum_type
  srchfach_meas_oksn_sys_tab[SRCHFACH_MEAS_OKSN_SYS_NUM]; 

typedef enum
{
  SRCHFACH_FMO_SUSPEND_PENDING_STATE,
  SRCHFACH_FMO_SUSPENDED_STATE,
  SRCHFACH_FMO_PROCEED_STATE
} srchfach_fmo_state_enum_type;



/* FACH non-intra-F measurement parameters */
typedef struct
{
  boolean  fach_meas_incl; /* Whether FACH meas info valid ? */
  boolean  k_valid;
  uint32   m_rep;    /* M_REP = 2^k only if k_valid is TRUE */
  boolean  c_rnti_valid;
  uint16   c_rnti;   /* C_RNTI value only if c_rnti_valid is TRUE */
  uint8    num_inter_sys; /* number of FACH inter-RAT meas */
  boolean  inter_f_ind; /* Inter-F FDD Meas indicator, N_FDD */
  boolean  inter_sys_gsm_ind;  /* GSM in inter_sys[]? */
  #if defined(FEATURE_WCDMA_TO_LTE_FACH) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH)
  boolean  inter_sys_lte_ind; /*LTE present in IRAT FMO config */
  #endif
  uint8    n_tti;  /* SCCPCH max TTI, N_TTI */
  uint16   next_meas_oksn_sfn; /* next FACH meas oksn SFN */
  uint32   t_meas_rep; /* Ntti * M_REP frame, 0 is invalid */
  uint8    num_meas_oksn_sys;/* Total # of RAT's for FMO */
  uint16   t_meas; /* num_meas_oksn_sys * t_meas_rep */
  uint16   sccpch_offset;
  uint8    sccpch_hwch_idx;
  srchfach_meas_oksn_sys_enum_type fach_oksn_type;

  uint16   sccpch_sfn; /* frame counter/timer */
  
  /* snap shot of current time in SEQ_SRCH_FACH_SCCPCH_EVENT ISR */
  uint16   ref_sccpch_sfn;
  uint32   ref_stmr_offset_chipx8;
  uint16   ref_gstmr_gfn;
  uint32   ref_gstmr_offset_qs;
  uint8    fmo_gap_frms;
  uint16    fmo_gap_num_slots;
  /* For HSFACH/HSRACH: Counter to reset Tresel for all IF,GSM neighbors if FMOs
	 have been blocked by RACH */
  uint8    n_fmo_skip;
  
} srchfach_n_intra_meas_parms_struct_type;
extern srchfach_n_intra_meas_parms_struct_type  srchfach_n_intra_meas_parms;

typedef enum
{
  FMO_CLIENT_RNTI_UPD,
  FMO_CLIENT_L1M,
  FMO_CLIENT_MEAS_MODULE,
  FMO_CLIENT_RXD_MODULE,
  /* RNTI_UPD doesnt need a CB so make sure the last enum value has the 
     same value as the last module */
  FMO_MAX_CB_CLIENTS = FMO_CLIENT_RXD_MODULE
}srchfach_fmo_op_client_enum_type;

/*data needed for handling FMO suspend resume state machine*/
typedef struct
{
  srchfach_fmo_state_enum_type cur_fmo_state;
  SRCHFACH_FMO_DONE_CB_FUNC fmo_done_cb_func[FMO_MAX_CB_CLIENTS];
  uint8  fmo_done_cb_cnt;
  uint8  cur_fmo_done_cb_idx;
  uint8  client_bmsk;
}srchfach_fmo_state_mc_info_struct_type;

extern srchfach_fmo_state_mc_info_struct_type srchfach_fmo_state_info;

#if defined(FEATURE_WCDMA_TO_LTE_FACH) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH)
#define SRCHFACH_LTE_FMO_ENABLED() (srchfach_n_intra_meas_parms.inter_sys_lte_ind == TRUE)
#endif

/*--------------------------------------------------------------------------
                          FACH GSM measurements
--------------------------------------------------------------------------*/
#define SRCHFACH_MEAS_GSM_OKASN_CYCLE 8

/* FACH GSM measurement occasion types */
typedef enum
{
	SRCHFACH_MEAS_OKSN_GSM_BSIC_ID,
	SRCHFACH_MEAS_OKSN_GSM_BSIC_RC,
	SRCHFACH_MEAS_OKSN_GSM_RSSI,
	SRCHFACH_MEAS_OKSN_GSM_NUM
} srchfach_meas_oksn_gsm_enum_type;

/* Cycle table for scheduling of FACH GSM measurement occasions */
extern const srchfach_meas_oksn_gsm_enum_type
	srchfach_meas_gsm_oksn_tab[SRCHFACH_MEAS_GSM_OKASN_CYCLE];

/* FACH GSM measurement state types */
typedef enum
{
  SRCHFACH_GSM_MEAS_STATE_INITIAL,
  SRCHFACH_GSM_MEAS_STATE_READY,
  SRCHFACH_GSM_MEAS_STATE_GOING,
  SRCHFACH_GSM_MEAS_STATE_NUM
} srchfach_gsm_meas_state_enum_type;
extern srchfach_gsm_meas_state_enum_type srchfach_gsm_meas_state;

/* FACH GSM measurement parameters */
typedef struct
{
  boolean  rssi_valid;     /* Initial 3 RSSI samples per carrier DONE */
  uint8    num_oksn_done_in_cycle;
           /* # of oksns completed in cycle, 0 starts new cycle */
  uint8    oksn_tab_idx;   /* GSM oksn cycle tab index */
  uint8    next_oksn_gfn;  /* % SRCH_GSM_MULTI_FRAME_COUNT */
  uint32   oksn_offset_qs; /* offset within GSTMR frame */
  uint8    oksn_end_gfn;   /* maybe >= SRCH_GSM_MULTI_FRAME_COUNT */
  uint32   oksn_end_offset_qs;
  uint32   t_meas_gsm;     /* WCDMA frame between succesive GSM oksn */
  uint32   oksn_total_qs;  /* Total oksn window size in GSM timing frame */
  uint32   time_to_oksn_stmr_chipx8;
} srchfach_gsm_meas_parms_struct_type;
extern srchfach_gsm_meas_parms_struct_type  srchfach_gsm_meas_parms;

/* FACH GSM RSSI measurement parameters */
typedef struct
{
  boolean gsm_rssi_started;
          /* Indicator whether first time to do GSM RSSI */
  uint8  num_rssi_samples;
         /* counter for RSSI samples done per carrier */
  uint8  next_cell_idx;
         /* Next GSM cell index in srch_gsm_nset_ptr[] */
  uint8  first_cell_idx;
         /* First Cell index to retrive RSSI results */
  uint32 timer_last_rank;
         /* # frames since last GSM RSSI rank */
} srchfach_gsm_rssi_parms_struct_type;
extern srchfach_gsm_rssi_parms_struct_type srchfach_gsm_rssi_parms;

/* Max number of GSM cells for BSIC ID or Reconf */
#define SRCHFACH_NUM_GSM_BSIC_CELLS 6

/* FACH GSM BSIC initial ID parameters */
typedef struct
{
  boolean  bsic_id_cell_valid; /* The cell ptr is valid */
  srch_gsm_cell_struct_type* cell_ptr; /* cell in BSIC ID process */
  boolean  tone_detected; /* This cell's tone has been detected */
  uint32   bsic_id_timer;
  uint32   t_identify_gsm;
  uint8 num_gaps_blocked;
  /* cells that failed earlier BSIC ID within timer */
  uint8  num_failed_cells;
  srch_gsm_cell_struct_type* failed_cells[SRCHFACH_NUM_GSM_BSIC_CELLS];
} srchfach_gsm_bsic_id_parms_struct_type;
extern srchfach_gsm_bsic_id_parms_struct_type srchfach_gsm_bsic_id_parms;

/* FACH GSM BSIC Reconf parameters */
typedef struct
{
  uint8 num_bsic_recf_cells;
        /* Number of GSM cells that need BSIC reconfirmation */
  uint8 recf_suit_cell_idx;
        /* Suitable GSM cell for reconf in next 8 GSM oksn, 0xFF invalid */
  uint8 recf_suit_oksn_idx;
        /* Suitable occasion index in next cycle of 8 GSM oksn */
  uint8 recf_nbr_sch_gfn;
        /* suitable SCH GFN only if recf_suit_cell_idx is valid */
  srch_gsm_cell_struct_type* cell_ptr;
      /* The cell ptr for the current RECONF search */
  srch_gsm_cell_struct_type* cell_ptrs[SRCHFACH_NUM_GSM_BSIC_CELLS];
        /* Queue of BSIC ID'ed GSM cells, idx=0 is least recent ID'ed */
  uint32 bsic_recf_timers[SRCHFACH_NUM_GSM_BSIC_CELLS];
        /* Timers per cell to compare with T_recf_GSM, in frames */
  boolean prev_fail_flags[SRCHFACH_NUM_GSM_BSIC_CELLS];
        /* Flags per cell for previous recf attempt */
  uint32 t_reconfirm_gsm;
} srchfach_gsm_bsic_rec_parms_struct_type;
extern srchfach_gsm_bsic_rec_parms_struct_type srchfach_gsm_bsic_rc_parms;

/* Whether need to wait for FACH GSM meas cleanup to finish */
extern boolean srchfach_gsm_meas_cleanup_pending;

/* ----------------------------------------------------- */
/* FACH measurement reservation manager type declaration */
/* ----------------------------------------------------- */

typedef enum
{
  SRCH_FACH_INTRAF_SEARCHER,
  SRCH_FACH_INTRAF_RACH,
  SRCH_FACH_INTER_MEAS,
  SRCH_FACH_RACH_RXD,
  SRCH_FACH_HSDPA, /* ID for HS-FACH. HSFACH module will reserve with this ID for HS channel config/reconfig/deconfig */
  SRCH_FACH_QTA,
  SRCH_FACH_NUM_REQ_ID
} srch_fach_reserve_req_id_enum_type;

#define SRCH_FACH_RESERVE_FRAME_INFINITE 0xFFFF

#define SRCH_FACH_MIN_GAP_SIZE_SLOTS 15

#define SRCH_FACH_RESERVE_REQ_ID_BMSK(req_id) \
  (1 << ((int) req_id))

#define SRCH_FACH_RESERVE_ALL_INTRA_REQ_BMSK \
  (SRCH_FACH_RESERVE_REQ_ID_BMSK(SRCH_FACH_INTRAF_SEARCHER) | \
   SRCH_FACH_RESERVE_REQ_ID_BMSK(SRCH_FACH_INTRAF_RACH) | \
   SRCH_FACH_RESERVE_REQ_ID_BMSK(SRCH_FACH_RACH_RXD) | \
   SRCH_FACH_RESERVE_REQ_ID_BMSK(SRCH_FACH_HSDPA))

#ifdef FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
/* global to indicate that UE has resumed back to W from Tuneaway */
extern boolean srchfach_resuming_fach_tuneaway;

/* global to indicate that srchcr_init() is pending to be called, 
** since L1_ENA_CELL_RESEL_TIMER_SIG expired when UE was tuned away and MDSP was not active */
extern boolean srchfach_pending_srchcr_init;
#endif

#define SRCHFACH_NONDRX_FACH() ((search_state == SRCH_FACH_STATE) && (!srchfachdrx_db.edrx_active))

#define SRCHFACH_ENABLE_GSM_FMO_MEAS() \
  ((search_state == SRCH_FACH_STATE) ? (srchfach_n_intra_meas_parms.inter_sys_gsm_ind ? SRCHCR_MEAS_IRAT : SRCHCR_MEAS_NONE): SRCHCR_MEAS_NONE)
  
#define SRCHFACH_ENABLE_INTERF_FMO_MEAS() \
  ((search_state == SRCH_FACH_STATE) ? (srchfach_n_intra_meas_parms.inter_f_ind ? SRCHCR_MEAS_INTER : SRCHCR_MEAS_NONE): SRCHCR_MEAS_NONE)


#if defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH) && defined(FEATURE_WCDMA_TO_LTE_FACH)
/*Define the CME callback function type. This is used for CME
CB registrations with SRCHFACH*/
typedef void srchfach_cme_cb_func_type(boolean);
extern srchfach_cme_cb_func_type* srchfach_cme_cb_func_ptr;

#endif


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION SRCH_FACH_INIT

DESCRIPTION
  This function performs search module FACH initializations. It is called
  by L1 manager when L1 enters the "FACH" state. 
  
  For now, search operation in FACH is similar to that in BCH/PCH with
  the following exceptions:
  
  1. Inter-frequency neihgbor cell search will be scheduled differently.
     In FACH, inter-frequency neighbor searches are scheduled based on 
     "FACH occasion" conveyed by the base station. (This item will be
     implemented in the future. Currently, we don't do inter-frequency 
     neighbor search in PCH either.)
     
  2. In FACH, SRCH module processes "CPHY_IMMEDIATE_MEAS_REQ" commands,
     and responds, with a report to the command immediately based on 
     available measurement results on intra-frequency neighbor cells.
     This is implemented as a function call.
     
  3. Search rates/parameters may need to be adjusted. (TBD.)     
     

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_fach_init( void );

/*===========================================================================
FUNCTION        SRCHFACH_CALCULATE_TM

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
    void.

SIDE EFFECTS    
===========================================================================*/
extern uint16 srchfach_calculate_tm(srch_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION        SRCHFACH_ADJUST_OFF

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
    void.

SIDE EFFECTS    
===========================================================================*/
extern uint16 srchfach_adjust_off(srch_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION        SRCHFACH_PROCESS_IMMEDIATE_MEAS_REQ

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
    void.

SIDE EFFECTS    
===========================================================================*/
extern void srchfach_process_immediate_meas_req
(
  l1_immediate_meas_type *meas,
  l1_meas_quan_enum_type meas_quan
);

/*===========================================================================
FUNCTION        SRCHFACH_SCHEDULE_NBR_SFN_DECODE

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  

SIDE EFFECTS 
===========================================================================*/ 
extern void srchfach_schedule_sfn_decode_cmd
(
  srch_cell_struct_type *cell_ptr
);

/*===========================================================================
FUNCTION        SRCHFACH_SCHEDULE_NBR_SFN_DECODE

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  

SIDE EFFECTS 
===========================================================================*/   
extern void srchfach_schedule_nbr_sfn_decode(void);

/*===========================================================================
FUNCTION        SRCHFACH_PROCESS_SFN_SFN_DIFF_CMD

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  

SIDE EFFECTS    
===========================================================================*/
extern void srchfach_process_sfn_sfn_diff_cmd
(
  srch_sfn_cfn_diff_cmd_type *srch_cmd
);

/*===========================================================================

FUNCTION     SRCHFACH_ASET_UPDATE_SRCH_DONE

DESCRIPTION
  This function handles the Active Set search done needed for an aset
  update during n-BCH setup and during Cell Transition.

DEPENDENCIES
  None.

RETURN VALUE
  None.
SIDE EFFECTS
  The function may change flag srch_dmod_set_changed.

===========================================================================*/
void srchfach_aset_update_srch_done( srch_done_cmd_type *cmd_ptr );

/*===========================================================================

FUNCTION       SRCHFACH_SEND_OOS

DESCRIPTION
  This function sends an out of service area indication to RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchfach_send_oos( void );

/*===========================================================================

FUNCTION       SRCHFACH_DECLARE_REACQ_STATUS

DESCRIPTION
  This function checks if  out of service area indication needs to be send to RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchfach_declare_reacq_status( void );

/*===========================================================================
FUNCTION        SRCH_FACH_CLEANUP

FILE NAME       

DESCRIPTION     
  This function performs search module FACH cleanup. It is called by L1 manager 
  when L1 exits the FACH state. 

DEPENDENCIES    

RETURN VALUE    
  Boolean value TRUE.

SIDE EFFECTS    
===========================================================================*/
extern boolean srch_fach_cleanup( l1m_state_enum_type next_state );

/*===========================================================================
FUNCTION        SRCHFACH_UPDATE_SFN_SFN_VAL

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  

SIDE EFFECTS 
===========================================================================*/ 
extern void srchfach_update_sfn_sfn_val(srch_sfn_wrc_diff_cmd_done_type *srch_cmd);

/*===========================================================================

FUNCTION      SRCHFACH_GSM_QUERY_BSIC_ID_CELL_POWER_LEVEL

DESCRIPTION
  This function queries the cell power including it's RSSI and signal level 
  based on some criteria.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  Cell's power is good for BSIC ID search, no need to pick the next 
          cell for BSIC ID 
  FALSE - Cell's power drops down and it's not good for BSIC ID, will go 
          picking the next cell for BSIC ID 
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchfach_gsm_query_bsic_id_cell_power_level( srch_gsm_cell_struct_type *this_cell);

/*===========================================================================
FUNCTION        SRCHFACH_MEAS_N_INTRA_PARMS_UPDATE

FILE NAME       

DESCRIPTION     update non-intra-F measurement parameters for FACH

DEPENDENCIES     

RETURN VALUE    
  void

SIDE EFFECTS    FACH GSM meas will be started
===========================================================================*/
void srchfach_meas_n_intra_parms_update
( 
  l1_fach_meas_info_struct_type *parms_ptr,
  boolean fach_meas_info_incl,
  boolean is_lte_fmo_configured
);

/*===========================================================================
FUNCTION        SRCHFACH_MEAS_CALC_OKSN

FILE NAME       

DESCRIPTION     Calculate FACH measurement occasions

DEPENDENCIES    k and C_RNTI must have been obtained from RRC 
                SCCPCH SFN must have been initialized
                
RETURN VALUE    FALSE if meas oksn parameters are not right

SIDE EFFECTS    
===========================================================================*/
boolean srchfach_meas_calc_oksn( void );

/*===========================================================================
FUNCTION        SRCHFACH_MEAS_N_INTRA_INIT

FILE NAME       

DESCRIPTION     Initialize CELL_FACH non-intra-F cell reselection

DEPENDENCIES    

RETURN VALUE    void
  

SIDE EFFECTS    
===========================================================================*/
void srchfach_meas_n_intra_init( boolean clearall );

/*===========================================================================
FUNCTION        SRCHFACH_GSM_MEAS_INIT

FILE NAME       

DESCRIPTION     Initialize CELL_FACH GSM cell reselection

DEPENDENCIES    

RETURN VALUE    
  void

SIDE EFFECTS    
===========================================================================*/
void srchfach_gsm_meas_init( void );

/*===========================================================================

FUNCTION        SRCHFACH_GSM_MEAS_START

DESCRIPTION     Start FACH GSM cell reselection measurements 

DEPENDENCIES    None

RETURN VALUE    FALSE if cannot start
  
SIDE EFFECTS    None

===========================================================================*/
boolean srchfach_gsm_meas_start( void );

/*===========================================================================

FUNCTION        SRCHFACH_GSM_MEAS_PAUSE

DESCRIPTION     Pause FACH GSM cell reselection measurements 

DEPENDENCIES    None

RETURN VALUE    FALSE if cannot start
  
SIDE EFFECTS    None

===========================================================================*/
boolean srchfach_gsm_meas_pause( void );

/*===========================================================================

FUNCTION        SRCHFACH_GSM_MEAS_CLEANUP

DESCRIPTION     Cleanup FACH GSM cell reselection measurements 

DEPENDENCIES    None

RETURN VALUE    FALSE if cannot finish immediately
  
SIDE EFFECTS    None

===========================================================================*/
boolean srchfach_gsm_meas_cleanup( void );

/*===========================================================================

FUNCTION        SRCHFACH_GSM_MEAS_CMD_CLEANUP_DONE

DESCRIPTION     Callback function installed to be called when GSM mdsp status is 
                no longer awake, to post a CM GSM module cleanup in case of FACH.
                Takes number of commands pending in srchgsm_mdsp_cmd_buf_used as
                argument.

DEPENDENCIES    None

RETURN VALUE    None
  
SIDE EFFECTS    None

===========================================================================*/
extern void srchfach_gsm_meas_cmd_cleanup_done(uint8 num_cmd_pending);

/*===========================================================================

FUNCTION        SRCHFACH_GSM_PROC_MEAS_CMDS

DESCRIPTION     Schedule and perform GSM measurements at GSM occasions. 

DEPENDENCIES    Must be called at SW FACH GSM measurememnt occasion

RETURN VALUE    Boolean
True - If FMO was issued
False - otherwise
  
SIDE EFFECTS    None

===========================================================================*/
boolean srchfach_gsm_proc_meas_cmds( void );

/*===========================================================================

FUNCTION      SRCHFACH_GSM_PROCESS_RSSI_MEAS_RESULT

DESCRIPTION
  This function will update the RSSI results for all the GSM Cells for
  which the RSSI measurements were made and then sort the GSM cells
  according to the averaged RSSI.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchfach_gsm_process_rssi_meas_result( void );

/*===========================================================================

FUNCTION        SRCHFACH_GSM_PROCCESS_BSIC_ID_RESULT

DESCRIPTION     Process FACH GSM BSIC initial ID results 

DEPENDENCIES    None

RETURN VALUE    void
  
SIDE EFFECTS    None

===========================================================================*/
void srchfach_gsm_process_bsic_recf_result( boolean verified );

/*===========================================================================

FUNCTION        SRCHFACH_GSM_PROCCESS_BSIC_ID_RESULT

DESCRIPTION     Process FACH GSM BSIC initial ID results 

DEPENDENCIES    None

RETURN VALUE    void
  
SIDE EFFECTS    None

===========================================================================*/
void srchfach_gsm_process_bsic_id_result( boolean verfied );

/*===========================================================================

FUNCTION        SRCHFACH_GSM_BSIC_ID_RECORD_FAILED_CELL

DESCRIPTION     Record a GSM cell as FACH GSM BSIC ID timer expired cell 

DEPENDENCIES    None

RETURN VALUE    void
  
SIDE EFFECTS    Update the failed cell queue

===========================================================================*/
void srchfach_gsm_bsic_id_record_failed_cell
(
  srch_gsm_cell_struct_type* cell_ptr
);

/*===========================================================================

FUNCTION        SRCHFACH_GSM_BSIC_ID_CLEAR_FAILED_CELL_RECORD

DESCRIPTION     Clear record of FACH GSM BSIC ID timer expired cells 

DEPENDENCIES    None

RETURN VALUE    void
  
SIDE EFFECTS    Clear record of FACH GSM BSIC ID timer expired cells

===========================================================================*/
void srchfach_gsm_bsic_id_clear_failed_cell_record(void);


/*===========================================================================

FUNCTION        SRCHFACH_GSM_BSIC_ID_DID_CELL_FAIL_EARLIER

DESCRIPTION     Did the GSM cell fail FACH GSM BSIC ID earlier in record? 

DEPENDENCIES    None

RETURN VALUE    boolean
  
SIDE EFFECTS    None

===========================================================================*/
boolean srchfach_gsm_bsic_id_did_cell_fail_earlier
(
  const srch_gsm_cell_struct_type* cell_ptr
);

/*===========================================================================

FUNCTION        SRCHFACH_GSM_BSIC_ID_PICK_NEXT_CELL

DESCRIPTION     Pick a GSM cell for FACH GSM BSIC ID. 

DEPENDENCIES    None

RETURN VALUE    void
  
SIDE EFFECTS    Update related info

===========================================================================*/
void srchfach_gsm_bsic_id_pick_next_cell(void);


/*===========================================================================

FUNCTION      SRCHFACH_GSM_GET_NEXT_NSET_FOR_RSSI

DESCRIPTION
  Gets the next GSM arfcn for which rssi measurement need to be done.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 srchfach_gsm_get_next_nset_for_rssi( uint8 cell_idx);


/*===========================================================================

FUNCTION        srchfach_update_rnti

DESCRIPTION     Called by HS module to update HRNTI

DEPENDENCIES    

RETURN VALUE    None
  
SIDE EFFECTS    None

===========================================================================*/
extern void srchfach_update_rnti(uint16 rnti);

/*===========================================================================

FUNCTION        SRCHFACH_SCCPCH_EVT_HANDLER

DESCRIPTION     ISR for CELL_FACH SCCPCH frame boundary event for inter-RAT 
                and inter-F measurments at measurement occasions 

DEPENDENCIES    Only to be enabled in CELL_FACH state

RETURN VALUE    None
  
SIDE EFFECTS    None

===========================================================================*/
extern void srchfach_sccpch_evt_handler( void );

/*===========================================================================

FUNCTION        SRCHFACH_DISABLE_SCCPCH_EVT

DESCRIPTION     In CELL_FACH state, this function disables
                SEQ_SRCH_FACH_SCCPCH_EVENT and resets SCCPCH SFN

DEPENDENCIES    Valid only in CELL_FACH state

RETURN VALUE    None
  
SIDE EFFECTS    As said above

===========================================================================*/
void srchfach_disable_sccpch_evt( void );

/*===========================================================================

FUNCTION        SRCHFACH_ENABLE_SCCPCH_EVT

DESCRIPTION     In CELL_FACH state, this function updates measurement occasion
                parameters, initializes SCCPCH SFN, and enables
                SEQ_SRCH_FACH_SCCPCH_EVENT 

DEPENDENCIES    Valid only in CELL_FACH state

RETURN VALUE    None
  
SIDE EFFECTS    SEQ_SRCH_FACH_SCCPCH_EVENT is enabled

===========================================================================*/
void srchfach_enable_sccpch_evt( void );

/*===========================================================================
FUNCTION        SRCHFACH_MEAS_UPDATE_OKSN

FILE NAME       

DESCRIPTION     Update srchfach_n_intra_meas_parms.next_meas_oksn_sfn

DEPENDENCIES    Must be called after scheduling FACH meas. 

RETURN VALUE    void

SIDE EFFECTS    next_meas_oksn_sfn is updated for next non-intra-F meas oksn

===========================================================================*/
void srchfach_meas_update_oksn( void );

/*===========================================================================

FUNCTION        SRCH_FACH_MEAS_OKSN_DUE

DESCRIPTION     Is FACH measurement occasion approached ? 

DEPENDENCIES    None

RETURN VALUE    TRUE if FACH measurement occasion is pre-def'ed frames away.
  
SIDE EFFECTS    None

===========================================================================*/
boolean srch_fach_meas_oksn_due( void );

/*===========================================================================

FUNCTION        SRCH_FACH_UPDATE_TIMERS

DESCRIPTION     Update FACH CR timers in SCCPCH events ISR

DEPENDENCIES    None

RETURN VALUE    void
  
SIDE EFFECTS    None

===========================================================================*/
void srch_fach_update_timers( void );

/*===========================================================================
FUNCTION       SRCHFACH_RESERVE_INTER_MEAS_OKSN

DESCRIPTION    Reserves required number of frames for FACH meas OKSN for
               inter-RAT or inter-Freq.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on reservation success.

SIDE EFFECTS
  Frames required (w.r.t. Rx timelines) for FACH meas OKSN will be reserved
  if possible and will not be available for use by any intra activity if
  reserrved
===========================================================================*/
boolean srchfach_reserve_inter_meas_oksn(srchfach_meas_oksn_sys_enum_type type, uint8 num_frames);


/*===========================================================================
FUNCTION        SRCHFACH_WAIT_FOR_SIB_MOD_UPDATE

DESCRIPTION
  This function is called to clean up FACH measurement occasion and
  ongoing searches during SIB modification updates.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if caller needs to wait to apply SIB mod changes,
  FALSE otherwise

SIDE EFFECTS
  FACH measurement occasions and searches are cleared

===========================================================================*/
boolean srchfach_wait_for_sib_mod_update(void);

#define SRCHFACH_IS_MEAS_OCCASION_SPECIFIED \
            srchfach_n_intra_meas_parms.c_rnti_valid

/* --------------------------------------------------------- */
/* FACH measurement reservation manager function declaration */
/* --------------------------------------------------------- */

/*===========================================================================

FUNCTION        SRCH_FACH_RESERVE_MGR_INIT

DESCRIPTION     This function initializes FACH reservation manager related
                variables

DEPENDENCIES    Should be called at every FACH state init before FACH measurement
                related reserve functionality is used.

RETURN VALUE    None
  
SIDE EFFECTS    Reset all the variable to reserve none.

===========================================================================*/

extern void srch_fach_reserve_mgr_init(void);

/*===========================================================================

FUNCTION        SRCH_FACH_QUERY_INTER_FACH_MEAS_OKSN_EXISTS

DESCRIPTION     This function checks if the requested number of frame starting
                next frame has a frame with FACH measurement occasion. FACH
                meas OKSN can be started before next frame and extends in next
                frame onward or can be in future that in with in frame window
                quried.

DEPENDENCIES    srchfach_n_intra_meas_parms must be valid

RETURN VALUE    TRUE of FALSE
  
SIDE EFFECTS    None

===========================================================================*/

extern boolean srch_fach_query_inter_fach_meas_oksn_exists(
  /* number of frames to check */
  uint8 num_fr);

/*===========================================================================

FUNCTION        SRCH_FACH_RESERVE_FRAME

DESCRIPTION     This function reserves the required number of frame for
                request Id if it can.

DEPENDENCIES    All variables related to FACH reservation manager must be
                initialzed before.

RETURN VALUE    TRUE of FALSE
  
SIDE EFFECTS    If reserved successfully, it will update reservation variable
                WCDMA_INTLOCK during this function execution.

===========================================================================*/

extern boolean srch_fach_reserve_frame(
  /* entity that is trying to reserve the frames */
  srch_fach_reserve_req_id_enum_type req_id,
  /* number of frames required for intra freq search */
  uint16 num_reserve_frames,
  /* start CFN from where the search can be done
     or required to be done based in req Id.
     
     For req Id INTER (INTERF/INTERRAT) thi is input.
     
     For all intra it is OUTPUT
       applicable if number of frames to reserve are not infinite */
  uint8 *reserve_start_cfn,
  /* indicate reserve mode for intra req Ids only */
  boolean req_mode_force);

/*===========================================================================

FUNCTION        SRCH_FACH_UNRESERVE_FRAME

DESCRIPTION     This function unreserves any remaning number of frame for
                request Id if there are any.

DEPENDENCIES    All variables related to FACH reservation manager must be
                initialzed before.

RETURN VALUE    None
  
SIDE EFFECTS    Update reserve related variable for the request Id.

===========================================================================*/

extern void srch_fach_unreserve_frame(
  /* entity that is trying to unreserve the frames */
  srch_fach_reserve_req_id_enum_type req_id);

/*===========================================================================

FUNCTION        SRCH_FACH_UPDATE_RESERVE_INFO

DESCRIPTION     This function need to called peridically to clear and update
                request Id that are reserved for this CFN.

DEPENDENCIES    All variables related to FACH reservation manager must be
                initialzed before.

RETURN VALUE    None
  
SIDE EFFECTS    Clear reserve info for all request Id in this frame.
                Clear reserved bit mask if remaining frames are 0
                Update remaining number of frame if finite reserve.
                
===========================================================================*/

extern void srch_fach_update_reserve_info(
  /* current frame CFN */
  uint8 this_fr_cfn);


/*===========================================================================

FUNCTION        SRCH_FACH_QUERY_INTER_S_RESERVATION

DESCRIPTION     This function is called when L1 receives CPHY_SETUP_REQ for
                setting up NBCH for ngbr cell for cell reselection. This 
				function checks if there is a pending reservation for inter-s
				meas, and if so returns the cfn of the first free frame after
				the pending inter-s meas completes.

DEPENDENCIES    All variables related to FACH reservation manager must be
                initialzed before.

RETURN VALUE    TRUE if there is pending inter-s reservation, FALSE otherwise
  
SIDE EFFECTS    None.
                WCDMA_INTLOCK during this function execution.

===========================================================================*/
extern boolean srch_fach_query_inter_s_reservation(uint8 *first_free_cfn);

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
/*===========================================================================

FUNCTION        SRCH_FACH_WAS_FMO_ON

DESCRIPTION     This function check if FMO was on dring the sfn and sfn+1 given as input.

DEPENDENCIES    .

RETURN VALUE    TRUE of FALSE
  
SIDE EFFECTS    
===========================================================================*/
extern boolean srch_fach_was_fmo_on(uint16 sfn);

#else
/*===========================================================================

FUNCTION        SRCHFACH_SUSPEND_INTERS_MEAS_FOR_BCH

DESCRIPTION     This function suspends GSM and inter-F meas in FACH state
                through FACH meas reservation manager for BCH setup and
                decoding.

DEPENDENCIES    All variables related to FACH reservation manager must be
                initialzed before.

RETURN VALUE    None
  
SIDE EFFECTS    sets srchfach_inters_meas_suspended to TRUE
                
===========================================================================*/
extern void srchfach_suspend_inters_meas_for_bch(void);

/*===========================================================================

FUNCTION        SRCHFACH_RESUME_INTERS_MEAS_FOR_BCH

DESCRIPTION     This function resumes GSM and inter-F meas in FACH state
                through FACH meas reservation manager for BCH setup and
                decoding.

DEPENDENCIES    All variables related to FACH reservation manager must be
                initialzed before.

RETURN VALUE    None
  
SIDE EFFECTS    sets srchfach_inters_meas_suspended to FALSE
                
===========================================================================*/
extern void srchfach_resume_inters_meas_for_bch(void);
#endif
/*===========================================================================

FUNCTION        SRCHFACH_CLEANUP_OKSN_INFO

DESCRIPTION     This function cleans up FACH meas occasion info

DEPENDENCIES    None

RETURN VALUE    None
  
SIDE EFFECTS    As said
                
===========================================================================*/
extern void srchfach_cleanup_oksn_info(void);


/*===========================================================================
FUNCTION        SRCHFACH_CM_INIT

DESCRIPTION
  This function initializes the search CM driver used for inter-frequency
  measurements during FACH measurement occasions. This function is called
  after we get a new inter-frequency neighbor list from RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchfach_cm_init( void );

/*===========================================================================

FUNCTION        SRCHFACH_INTER_F_DO_MEAS

DESCRIPTION     This function issues CELL_FACH inter-freq meas commands

DEPENDENCIES    Must be called in SCCPCH event ISR

RETURN VALUE    None
  
SIDE EFFECTS    As said
                
===========================================================================*/
extern boolean srchfach_inter_f_do_meas(boolean relentless_detect);

/*===========================================================================

FUNCTION        SRCHFACH_INTERF_PROC_MEAS

DESCRIPTION     This function is called in task context in response to
                compressed mode inter-freq search done in FACH state.

DEPENDENCIES    None

RETURN VALUE    None
  
SIDE EFFECTS    None
                
===========================================================================*/

extern void srchfach_interf_proc_meas(void);


/*===========================================================================

FUNCTION        SRCHFACH_INTER_F_LIST_SRCH_CMD

DESCRIPTION     This function issues a list search on the inter
                frequency cell reselection target cell when the neighbor
                BCH is being read. This list search is initiated from the 
                80ms NSET event from the search heart beat event
                handler.

DEPENDENCIES    NONE

RETURN VALUE    NONE
  
SIDE EFFECTS    None

===========================================================================*/

extern void srchfach_inter_f_list_srch_cmd(void);


extern void srchfach_proc_sync_nset(void);

void srchfach_gsm_bsic_parms_update(void);
uint8 srchfach_get_sccpch_offset ( uint8 *hwch );

/*===========================================================================

FUNCTION        SRCHFACH_SEND_CM_FACH_INIT

DESCRIPTION     This function sends a FACH CM Init command to MCal

DEPENDENCIES    None

RETURN VALUE    None
  
SIDE EFFECTS    As said
                
===========================================================================*/
extern void srchfach_send_cm_fach_init(uint8 wtr_index,
                                       uint16 tgmp,
                                       uint32 rxlm_home_buf_idx,
                                       uint32 rxlm_away_buf_idx
                                       );

/*===========================================================================

FUNCTION SRCHFACH_IS_FACH_GSM_MEAS_ONGOING

DESCRIPTION:
  Return true if GSM measurments are in progress in FACH state , returns
  FALSE if they aren't

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchfach_is_fach_gsm_meas_ongoing( void );

#ifdef FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
/*===========================================================================
FUNCTION        srchfach_l1m_resuming_from_fach_tuneaway_resume_gsm

FILE NAME       srchfach.c

DESCRIPTION     This function resumes the GSM searches and states that were set, before Tuneaway.

DEPENDENCIES    

RETURN VALUE    
    void.

SIDE EFFECTS    
===========================================================================*/
extern void srchfach_l1m_resuming_from_fach_tuneaway_resume_gsm(void );
#endif

/*===========================================================================

FUNCTION        SRCHFACH_GET_ACCURATE_REF_TIME

DESCRIPTION     Get reference time for GSM meas scheduling 

DEPENDENCIES    Called only in srchfach_sccpch_evt_handler()

RETURN VALUE    None
  
SIDE EFFECTS    Update srchfach_n_intra_meas_parms

===========================================================================*/
extern void srchfach_get_accurate_ref_time(void);

/*===========================================================================

FUNCTION        SRCHFACH_GET_SCCPCH_LONGEST_FACH_TTI

DESCRIPTION     Get UE monitored SCCPCH longest TTI 

DEPENDENCIES    None

RETURN VALUE    FALSE for failure
  
SIDE EFFECTS    Assign to srchfach_n_intra_meas_parms.n_tti

===========================================================================*/
extern boolean srchfach_get_sccpch_longest_fach_tti( void );

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*=====================================================================

FUNCTION       SRCHFACH_CONFIRM_QTA_RESEVRATION

DESCRIPTION
  This function is called by the QTA module to confirm reservation for a QTA.

PARAMETERS
  Cfn - 	CFN at which QTA starts
  Force_reserve. TRUE – if reservation has to be guaranteed
                 FALSE - otherwise
  uint8 *client_bmask - Updates the aborted clients when QTA is allowed.
                        Updates the active client when QTA is blocked.

RETURN VALUE
  TRUE – If reservation is granted
  FALSE - Otherwise

SIDE EFFECTS
  All frames starting from CFN cfn are marked as in use for for infinte reservation

=====================================================================*/
extern boolean srchfach_confirm_qta_reservation (uint8 cfn, boolean force_reserve, uint8 *client_bmask);

/*=====================================================================

FUNCTION       SRCHFACH_HANDLE_SRCH_ABORT

DESCRIPTION
  This function is called when search is aborted before starting QTA
  Function make sures that the FACH globals are reset as part of abort

PARAMETERS
  
RETURN VALUE
  
SIDE EFFECTS
=====================================================================*/
extern void srchfach_handle_srch_abort(void);

/*=====================================================================

FUNCTION       SRCHFACH_REQUEST_FUTURE_QTA_RESEVRATION

DESCRIPTION
  This function is called by the QTA module to request reservation for a QTA.

PARAMETERS
  Cfn - 	CFN at which QTA starts
  Force_reserve. TRUE – if reservation has to be guaranteed
                 FALSE - otherwise

RETURN VALUE
  None.

SIDE EFFECTS
  If force_reserve is set to TRUE, it will cause FRM to start denying any future reservations
  that are not guaranteed to end at cfn (cfn - Tqta_advance2)

=====================================================================*/
extern void srchfach_request_future_qta_reservation (uint8 cfn, boolean force_reserve);

/*=====================================================================

FUNCTION       SRCHFACH_FRAME_RESV_WITH_QTA_ACTIVE

DESCRIPTION
  This function is called when a module requests for reservation after
  QTA module has done its initial reservation.

PARAMETERS
  req_id - Module which requires reservation
  reserve_start_cfn - The frame num at which the module starts the activity
  num_reserve_frames - Number of frames the module requires the reservation for
 
RETURN VALUE
  TRUE - Allows the new module to have the reservation as the activiy of the new module
         will be completed by the N - 3 frames where Nth frame is start of QTA
 
  FALSE - Reservation for the new module is denied.

SIDE EFFECTS

=====================================================================*/
boolean srchfach_frame_resv_with_qta_active(srch_fach_reserve_req_id_enum_type reqid, uint8 *reserve_start_cfn,uint16 num_reserve_frames);

/*===========================================================================
FUNCTION        SRCH_FACH_QUERY_CLIENT_RESERVATION

FILE NAME       srchfach.c

DESCRIPTION     This function returns status of RACH reservation.
                

DEPENDENCIES    

RETURN VALUE    
    boolean. TRUE - Client is reserved. FALSE -Client reservation is not active 

SIDE EFFECTS    
===========================================================================*/
extern boolean srch_fach_query_client_reservation(srch_fach_reserve_req_id_enum_type req_id);
#endif /* FEATURE_QTA */

#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*===========================================================================
FUNCTION        SRCH_FACH_QUERY_RACH_RESERVATION

FILE NAME       srchfach.c

DESCRIPTION     This function returns status of RACH reservation.
                

DEPENDENCIES    

RETURN VALUE    
    boolean. TRUE - RACH is reserved. FALSE -RACH reservation is not active 

SIDE EFFECTS    
===========================================================================*/
extern boolean srch_fach_query_rach_reservation(void);

#endif /* FEATURE_WCDMA_HS_FACH_DRX */

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================
FUNCTION        SRCH_FACH_QUERY_HSFACH_RESERVATION

FILE NAME       srchfach.c

DESCRIPTION     This function returns status of HSFACH reservation.
                

DEPENDENCIES    

RETURN VALUE    
    boolean. TRUE - HSFACH is reserved. FALSE - otherwise 

SIDE EFFECTS    
===========================================================================*/
extern boolean srch_fach_query_hsfach_reservation(void);
#endif /* FEATURE_WCDMA_HS_FACH */

extern uint32 srchfach_get_mrep(void);

/*===========================================================================

FUNCTION SRCHFACH_STEP1_DONE

DESCRIPTION
  This function processes step_1 Search dump during BCH. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchfach_step1_done(srchsched_step1_nastt_status_struct_type *step1_nastt_status);

/*===========================================================================

FUNCTION SRCHFACH_NASTT_DONE

DESCRIPTION
  This function processes step_3 Search dump during BCH. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchfach_nastt_done (srchsched_step1_nastt_status_struct_type *step1_nastt_status);

/*===========================================================================

FUNCTION SRCHFACH_NSET_SRCH_DONE

DESCRIPTION
  This function processes LPQ neighbor list search Search dump during BCH. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchfach_nset_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION        SRCHCR_ASET_SRCH_DONE

DESCRIPTION
  This function handles the ASET search done command interrupt, which was
  programmed to evaluate cell reselection criteria.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchfach_aset_search_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION        SRCHFACH_CAN_STEP1_SEARCH_BE_ISSUED

DESCRIPTION
  This function returns TRUE if the full search reservation is successful and Step1 can be issued right away,
  FALSE otherwise.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchfach_can_step1_search_be_issued( void );

/*===========================================================================

FUNCTION      SRCHFACH_CELL_RESELECT_EVAL

DESCRIPTION
 Perform cell reselection evaluation procedures in FACH
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void srchfach_start_cell_reselect_eval(void);


/*===========================================================================

FUNCTION     SRCHFACH_CAN_SCHEDULER_START_SEARCH

DESCRIPTION
  This function is called by the search scheduler before starting a search. It is upto FACH module 
to decide if the search can proceed based on conditions specific to the search type
   
DEPENDENCIES
  None.

RETURN VALUE
boolean

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchfach_can_scheduler_start_search(srch_ops_code_enum_type srch_type);

/*===========================================================================
FUNCTION        SRCHFACH_SUSPEND_FMO

DESCRIPTION     This function returns the current FMO status. It is used to block 
the CPHY command if a FMO is already scheduled and cmd sent to FW. Otherwise 
we will block future FMO and allow CPHY commands to proceed. 
                

DEPENDENCIES    

RETURN VALUE    
    FALSE - Block CPHY processing . TRUE - otherwise, FMO's will be blocked.

SIDE EFFECTS    
===========================================================================*/
extern boolean srchfach_suspend_fmo(SRCHFACH_FMO_DONE_CB_FUNC post_fmo_cb_func, 
srchfach_fmo_op_client_enum_type client_id);

/*===========================================================================
FUNCTION        SRCHFACH_RESUME_FMO

DESCRIPTION     This function will be called by DL module once the CPHY 
processing is completed and FMO's can be scheduled.
                

DEPENDENCIES    

RETURN VALUE    
    NONE

SIDE EFFECTS    
===========================================================================*/
void srchfach_resume_fmo(srchfach_fmo_op_client_enum_type client_id);

/*===========================================================================
FUNCTION        SRCHFACH_RESUME_MEAS_PARM_UPDATE_CB

DESCRIPTION    Callback function to resume the measurement parm update 
which was paused by the FMO suspend
                

DEPENDENCIES    

RETURN VALUE    
    NONE

SIDE EFFECTS    
===========================================================================*/

void srchfach_resume_meas_parm_update_cb(void);

/*===========================================================================
FUNCTION        SRCHFACH_RESUME_CLIENT_OPS_AFT_FMO

DESCRIPTION     This function blocks the FMO and resumes the client operations. 
                It is triggered through a local command as soon as a FMO is completed.
                
DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void srchfach_resume_client_ops_aft_fmo(void);

/*===========================================================================
FUNCTION        SRCHFACH_CALC_NEXT_FMO_RAT_TYPE

FILE NAME       srchfach.c

DESCRIPTION     This function calculates the RAT type for next fmo oksn
                

DEPENDENCIES    

RETURN VALUE    
RAT type for next FMO oksn

SIDE EFFECTS    
===========================================================================*/
srchfach_meas_oksn_sys_enum_type srchfach_calc_next_fmo_rat_type(void);

extern uint16 srchfach_get_allowed_srch_mask(void);

#if defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH) && defined(FEATURE_WCDMA_TO_LTE_FACH)
/*===========================================================================
FUNCTION        SRCHFACH_REGISTER_ACT_DEACT_CME_CB_FUNC

FILE NAME       srchfach.c

DESCRIPTION     This function is called from CME module to register for 
CME CB when QICE is active
                

DEPENDENCIES    

RETURN VALUE    
None

SIDE EFFECTS    
None
===========================================================================*/
extern void srchfach_register_act_deact_cme_cb_func(srchfach_cme_cb_func_type* cb_func);

/*===========================================================================
FUNCTION        SRCHFACH_DEREGISTER_ACT_DEACT_CME_CB_FUNC

FILE NAME       srchfach.c

DESCRIPTION     This function is called from CME module to deregister for 
CME CB when QICE is deactivated
                

DEPENDENCIES    

RETURN VALUE    
None

SIDE EFFECTS    
None
===========================================================================*/
extern void srchfach_deregister_act_deact_cme_cb_func(void);

/*===========================================================================

FUNCTION        SRCHFACH_START_LTE_MEAS

DESCRIPTION     This function issues CELL_FACH W2L meas commands. As part of it, 
                it sends START FACH CMD to WFW, allocates RF buffers, updates the
                FACH W2L parameters to be sent to LTE and issues searches.

DEPENDENCIES    Must be called in SCCPCH event ISR

RETURN VALUE    None
  
SIDE EFFECTS    None
                
===========================================================================*/
extern boolean srchfach_start_lte_meas(void);
#endif /* FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH && FEATURE_WCDMA_TO_LTE_FACH*/

/*===========================================================================
FUNCTION        SRCHFACH_UPDATE_INTERF_TIMERS

FILE NAME       srchfach.c

DESCRIPTION     This function is called when FMO is configured to update driver with InterF meas timers
                

DEPENDENCIES    

RETURN VALUE    
None

SIDE EFFECTS    
None
===========================================================================*/
extern void srchfach_update_interf_timers(void);


/*===========================================================================
FUNCTION        SRCHFACH_GET_CH_SFN

FILE NAME       srchfach.c

DESCRIPTION     Function calculates the TLM SFN
                
DEPENDENCIES    

RETURN VALUE    
CH SFN value

SIDE EFFECTS    
None
===========================================================================*/
extern void srchfach_get_ch_sfn(uint16 *sfn,uint32 *cx8_offset_qty);

/*===========================================================================

FUNCTION SRCHFACH_CHECK_REF_AGE_AND_TRIM_MSET_USET

DESCRIPTION This function will identify which weak neighbor cells can be moved 
out of neighbor lists so that srch time can be reduced. SYNC_N cells if no usable path
after 50 list searches it will be moved to ASYNC_N. Similarly USET cells will be deleted 
to empty set if no usable path after 10 list searches.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchfach_check_cell_ref_age_and_trim_mset_uset(void);


/*===========================================================================
FUNCTION        SRCHFACH_LOG_MEAS_OKSN_INFO(0x4721)

FILE NAME       

DESCRIPTION     Calculate FACH measurement occasions in TASK context

DEPENDENCIES    k and C_RNTI must have been obtained from RRC
                SCCPCH SFN must have been initialized 
                FACH longest TTI on SCCPCH has been obtained

RETURN VALUE    FALSE if meas oksn parameters are not right

SIDE EFFECTS    FACH GSM meas will be started
 
===========================================================================*/
extern void srchfach_log_meas_oksn_info( boolean submit_log_pkt);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================

FUNCTION WL1_POPULATE_DEBUG_SRCHFACH_INFO_STRUCT_TYPE

DESCRIPTION
   This function dumps state info into a global strcuture as part of a crash

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
void wl1_populate_debug_srchfach_info_struct_type(wl1_cell_resel_mini_debug_info_struct_type * srchfach_cell_resel_dump_ptr);
#endif /* FEATURE_QSH_DUMP */
#endif /* SRCHFACH_H */
