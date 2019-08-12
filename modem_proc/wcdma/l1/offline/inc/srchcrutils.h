#ifndef SRCHCRUTILS_H
#define SRCHCRUTILS_H
/*===========================================================================
   S R C H     C E L L     R E S E L E C T I O N     U T I L I T I E S   H E A D E R    F I L E

GENERAL DESCRIPTION
   This file contains resel utility functions called across all WCDMA L1 Files. 

RULES FOR SRCHCRUTILS
1. Common utility Fn for IDLE and FACH
2. Resel utility functions
3. Log Pkt submit functions (subset of 1)
4. No srchcr database maintenance | No writing into any data structure. 
   Only reading is allowed. 
5. All utility macros
6. Should be included only by c-files


EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  srchcr_init() should be called once we enter DRX mode.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2001-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchcrutils.h_v   1.5   11 Jul 2002 21:29:46   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchcrutils.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/11/15   psr      Fixed 0x4027 Log Packet issues and added support so that 0x4027 would only be flushed
                    when it contained measured neighbor cells
02/10/15   psr      SRCHCRUTILS_GET_MAX_TX_POWER_NV macro cleanup
02/10/15   mk       Fixing a compiler error.
02/03/14   psr      Added SRCHCRUTILS_GET_MAX_TX_POWER_NV macro
11/17/14   psr      Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "l1def.h"
#include "wl1m.h"
#include "srchfach.h"

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_CM_LTE_SEARCH) 
#include "wsrchlte.h"
#endif

/*===========================================================================

                        CONSTANT DECLARATIONS

===========================================================================*/

/* Check if the cell meets S criteria */
#define SRCHCR_CELL_MEETS_S_CRITERIA( cell_ptr )     \
 ((cell_ptr->s_qual > 0) && (cell_ptr->s_rxlev > 0))

/* Macro to indicate current meas rules status */
#define SRCHCR_MEAS_RULES_INDICATE_MEAS()       (srchcr_current_meas_rules != 0)

#define SRCHCR_MEAS_RULES_INDICATE_INTRA_MEAS() (srchcr_current_meas_rules & SRCHCR_MEAS_INTRA)
#define SRCHCR_MEAS_RULES_INDICATE_INTER_MEAS() (srchcr_current_meas_rules & SRCHCR_MEAS_INTER)
#define SRCHCR_MEAS_RULES_INDICATE_IRAT_MEAS()  (srchcr_current_meas_rules & SRCHCR_MEAS_IRAT)
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
#define SRCHCR_MEAS_RULES_INDICATE_LTE_MEAS()  (srchcr_current_meas_rules & SRCHCR_MEAS_LTE)
#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/* Macro to check if this prior is higher then serv cell */
#define SRCHCR_IS_PRIOR_HIGHER_THEN_SERVING_CELL_PRIOR(prior) (prior > srch_aset_parms.prior_info.priority) 

#define SRCHCR_FACH_LEGACY_RULES() \
  ((search_state == SRCH_FACH_STATE) && \
  (srchcr_prior_info_for_rats.fach_abs_prio_ue_cap_sib_ind == LEGACY))

#define SRCHCR_FACH_ABS_PRIO_USED() \
  ((search_state == SRCH_FACH_STATE) && \
   (srch_aset_parms.prior_used) && \
   (srchcr_prior_info_for_rats.fach_abs_prio_ue_cap_sib_ind != LEGACY) )

#define SRCHCR_DRX_ABS_PRIO_USED() \
  ((search_state == SRCH_DRX_STATE) && \
   (srch_aset_parms.prior_used))

/* Perform legacy ranking 
 *  When DRX/FACH absolute priority isnt defined and inter-freq/GSM doesnt have priorities
 *  When DRX/FACH absolute priority is defined and inter-freq/GSM doesnt have priorities
 *  When FACH absolute priority isnt defined and inter-freq/GSM have priorities
 */
#define SRCHCR_LEGACY_INTERF_RANKING_NEEDED() \
  (!(SRCHCR_INTERF_PRIO_PRESENT() && \
   (SRCHCR_FACH_ABS_PRIO_USED() || SRCHCR_DRX_ABS_PRIO_USED())))

#define SRCHCR_LEGACY_GSM_RANKING_NEEDED() \
  (!(SRCHCR_IS_GSM_PRIO_PRESENT() && \
      (SRCHCR_FACH_ABS_PRIO_USED() || SRCHCR_DRX_ABS_PRIO_USED())))

#define SRCHCR_INTERF_PRIO_PRESENT() (srchcr_prior_info_for_rats.interf_prior_info_present)
#define SRCHCR_IS_GSM_PRIO_PRESENT() (srchcr_prior_info_for_rats.gsm_prior_info_present)
#define SRCHCR_IS_LTE_PRIO_PRESENT() (srchcr_prior_info_for_rats.lte_info_present)

#endif

#ifdef FEATURE_DUAL_SIM
#define SRCHCR_CHK_SRCHES_PENDING() (srchcr_split_res_eval_meas_pending || srchcr_pending_cell_resel_bsic_verif)
#endif

/* Invalid rank RSCP/EcIo field in 0x4027 log packet */
#define INVALID_RANK_VALUE (0xff)

/* Invalid priority field in 0x4027 log packet */
#define INVALID_PRIORITY_VALUE ((int16) 0xff)

/* Max number of GSM cells that are logged */
#define SRCHCR_MAX_GSM_CELLS_FOR_LOGGING 4

#define STEP1_PEAK_ELIMINATION_POS_DIFF 32


#ifdef FEATURE_DELAY_3G_TO_2G_RESEL
/* Number of reacq failure before we make GSM measurements for reselection */
#define SRCHCR_DELAY_GSM_RESEL_COUNT_DRX_3_7   2
#define SRCHCR_DELAY_GSM_RESEL_COUNT_DRX_8_9   1
#define SRCHCR_MIN_IO_FOR_2G_RESEL        -95 /* dBm */
#define SRCHCR_MIN_ECIO_FOR_2G_RESEL       40 /* dB (actually means -20) */
#define SRCHCR_MIN_NGBR_ECIO_FOR_2G_RESEL  30 /* dB (actually means -15) */
#endif /* FEATURE_DELAY_3G_TO_2G_RESEL */

#ifdef FEATURE_2G_RANK_Q_OFFSET_OPT
/* Minimum limit of Q-offset1 for cell reselection to GSM */
#define SRCHCR_MIN_Q_OFFSET_S_N   10
#endif

#ifdef FEATURE_IMMEDIATE_RESEL_AT_LOW_CPICH_ECIO
/* CPICH Ec/Io below which default Treselection to 0 = -16 dB */
#define SRCHCR_SERV_CPICH_THRESH_FOR_TRESEL_0 (32)
#endif /* FEATURE_IMMEDIATE_RESEL_AT_LOW_CPICH_ECIO */

/* Value of 5s in terms of 10ms frames */
#define SRCHCR_FIVE_SECONDS_IN_FRAMES   500

/* Value of 800ms in terms of FACH aset updates */
#define FACH_800_MS_IN_NUM_ASET_UPDATES (800 / (SRCH_ASET_SRCH_PERIOD * 10))

/*   
   ----------------------------------------
      Treselection Timer Max Possible Value
   ----------------------------------------
   
    For intra-freq, the actual Tresel is the Tresel times the speed_scaling factor if high-mobility state is detected;
    For inter-freq and IRAT, the actual Tresel is the Tresel times the speed_scaling factor (if high-mobility state
        is detected and if this variable is present) and the scaling factor (if present).
  
    Quote from 25.304: 
    "For inter-frequency cells and high-mobility state is detected:
      -	multiply Treselections or Treselections,PCH or Treselections,FACH by both the IEs "Speed dependent ScalingFactor
        for_Treselection" if sent on system information and "Inter-Frequency ScalingFactor for Treselection" if sent on system information."
    "For inter-RAT cells and high-mobility state is detected:
      - multiply Treselections or Treselections,PCH or Treselections,FACH by both the IEs "Speed dependent ScalingFactor for_Treselection"
        if sent on system information and "Inter-RAT ScalingFactor for Treselection" if sent on system information."

    From 25.331, the range of the parameters are summarized in below table:

       Treselections        Integer 0-31 seconds	
       Treselections,PCH    Integer 0-31 seconds          Used for UE in CELL_PCH or URA_PCH state
       Treselections,FACH	  Real (0..6.2 by step of 0.2)	Used for UE in CELL_FACH state
   
       Speed dependent Scaling Factor 	Real(0..1 by step of 0.1)       Used for UE in high mobility state as scaling factor 
       for Treselection                                                 for Treselections or Treselections,PCH or Treselections,FACH
       Inter-frequency Scaling Factor 	Real (1..4.75 by step of 0.25)	Used for UE as scaling factor for Treselections or Treselections,PCH or
       for Treselection                                                 Treselections,FACH for inter-frequency cell reselection
       Inter-RAT Scaling Factor for     Real (1..4.75 by step of 0.25)	Used by the UE as scaling factor for Treselections or Treselections,
       Treselection	                                                    PCH or Treselections,FACH for inter-RAT cell reselection

      Summary:
      For inter-freq and IRAT, the maximum Tresel = 31 seconds * 1 (speed scaling) * 4.75 = 147.25 secs.
      For intra-freq, the maximum Tresel = 31 seconds * 1 (speed scaling) = 31 secs.
 */

/* Maximum possible value of Treselection in number of frames */
#define SRCHCR_MAX_INTRA_TRESEL_FRAMES  3100

/* Maximum possible value of Treselection in number of frames */
#define SRCHCR_MAX_TRESEL_FRAMES  14800

/* 32 DRX cycles in case of DRX cycle lenght 80ms is the max t-meas-gsm value*/
#define MAX_T_MEAS_GSM_DRX_CYCLES  32 
/* 30sec / minimum DRX cycle length 80ms is max T-BSIC verif timer value*/
#define MAX_T_BSIC_VERIF_DRX_CYCLES  375  

/* Min # of frames between two normal intra freq full searches in DRX mode*/
#define SRCHCR_DRX_MIN_FRAMES_BETWEEN_NORMAL_INTRA_FULL_SRCH  3000   /* 30s */

/* Min # of frames between two panic intra freq full searches in DRX mode*/
#define SRCHCR_DRX_MIN_FRAMES_BETWEEN_PANIC_INTRA_FULL_SRCH  50   /* 500ms */

/* Min # of frames between two normal inter freq full searches in DRX mode*/
#define SRCHCR_DRX_MIN_FRAMES_BETWEEN_NORMAL_INTER_FULL_SRCH  3000   /* 30s */

/* Min # of frames between two panic inter freq full searches in DRX mode*/
#define SRCHCR_DRX_MIN_FRAMES_BETWEEN_PANIC_INTER_FULL_SRCH  100   /* 1s */

/* Min # of frames between two normal intra freq full searches in FACH mode*/
#define SRCHCR_FACH_MIN_FRAMES_BETWEEN_NORMAL_INTRA_FULL_SRCH  36  /* 360ms */

/* Num consecutive reacq failures for ignoring HCS during reselection eval */
#define NUM_REACQS_FOR_IGNORING_HCS  3


/* Max value of Sintra_search */
#define MAX_S_INTRA_SEARCH 24

#define MOBILITY_TH 5
#define PRED_MARGIN 5
#define DROP_THRESHOLD_ECIO 46
#define EXPIRY_TIME_FOR_DROP_TH 1500
/*Step1 search threshold value is -16.5 we compare with value multipled by 2*/
#define STEP1_SRCH_THRSH -33

#define MAX_PN_CELL_CNT_FOR_ENH_PN 10

#ifdef FEATURE_WCDMA_PANIC_SRCH_OPT
#define MAX_PN_ACM_CELL_CNT_FOR_INTRA (SRCH_SYNC_NSET_MAX_C0 + SRCHCR_USET_MAX)
#define MAX_PN_ACM_CELL_CNT_FOR_INTER  SRCH_SYNC_NSET_MAX_C0
#endif


/* Default value for meas prio */
//#define SRCHCR_MEAS_PRIO_NONE 0 /*Confirm and delete*/

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
  #define MAX_NUM_SRCHED_CELLS   (SRCH_SYNC_NSET_MAX_C0 +  \
                                  SRCH_OFREQ_CELL_MAX + \
                                  SRCH_GSM_NSET_MAX + \
                                  WSRCHLTE_NSET_MAX )
#else
  #define MAX_NUM_SRCHED_CELLS   ( SRCH_SYNC_NSET_MAX_C0 +  \
                                   SRCH_OFREQ_CELL_MAX + \
                                   SRCH_GSM_NSET_MAX )
#endif



/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/

typedef enum
{
	INTER_FREQ_0,
  INTER_FREQ_1,
	ASET_FREQ
} srchcr_freq_idx_enum_type;

/* Frequency index */
typedef enum
{
  FREQ_0,
  FREQ_1,
  FREQ_NONE
} srchcr_inter_f_idx_enum_type;

/* Neighbor cell type */
typedef enum
{
  INVALID_CELL=0x0,
  INTRA_F_CELL=0x1,
  INTER_F_CELL=0x2,
  INTER_S_GSM_CELL=0x4,
  INTER_S_LTE_CELL=0x8,
  ALL_CELLS=0xF
} srchcr_cell_type_enum_type;

typedef enum
{
  /* HIGH enum indicates criteria 1 to be calculated, when ABSOLUTE_PRIORITY is used */
  SRCH_HIGH_PRIORITY_LAYER,    
  /* EQUAL enum indicates criteria 2 to be calculated, when ABSOLUTE_PRIORITY is used */
  SRCH_EQUAL_PRIORITY_LAYER,
  /* LOW enum indicates criteria 3 to be calculated, when ABSOLUTE_PRIORITY is used */
  SRCH_LOW_PRIORITY_LAYER
}
srchcr_priority_enum_type;

typedef struct
{
  /* Indicates if the cell is Intra-F, Inter-F or GSM */
  srchcr_cell_type_enum_type    cell_type;

  /* flag to exclude cell from ranking */
  boolean exclude;

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
  /*flag to indicate if the cell is a resel candidate*/
  boolean is_fach_absprio_resel_cand;
  boolean exclude_for_legacy;
#endif

  union
  {
    /* Pointer to Intra-Freq cell information */
    srch_cell_struct_type      *wcdma_intra;
    /* Pointer to Inter-RAT GSM cell information */
    srch_gsm_cell_struct_type  *gsm;
    /* Pointer to Intra-Freq cell information */
    srch_interf_cell_struct_type *wcdma_inter;
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
    wsrchlte_cell_struct_type  *lte;
#endif
  }u;

} srchcr_rank_cell_struct_type;

typedef enum
{
  /* In high priority scheduling mode, cells with a higher priority will only be considered during reselection */
  SRCHCR_HIGH_PRIORITY_SCHEDULING_MODE,
  /* In all priority scheduling mode, all cells are considered regardless of priority */
  SRCHCR_ALL_PRIORITY_SCHEDULING_MODE,
  /*Invalid priority scheduling mode when absolute priority is not defined*/
  SRCHCR_INVALID_PRIORITY_SCHEDULING_MODE = 0xFF
} srchcr_prior_scheduler_mode_enum_type;

/* Enum to indicate meas rules type */
typedef enum {
  SRCHCR_MEAS_NONE    = 0x0,
  SRCHCR_MEAS_INTRA   = 0x1,
  SRCHCR_MEAS_INTER   = 0x2,
  SRCHCR_MEAS_IRAT    = 0x4,
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
  SRCHCR_MEAS_LTE     = 0x8,
#endif
  SRCHCR_MEAS_NON_INTRA = 0xE,
  SRCHCR_MEAS_ALL     = 0xF

} srchcr_meas_rules_type;

/* Structure used to store UE's fast moving status and fast moving end
 * time
 */
typedef struct
{
  /* Flag to indicate that UE is in fast moving state */
  boolean fast_moving;

  /* Time in s since power up when UE exits fast moving status. This 
   * time includes Tcrmaxhyst time too
   */
  dword fast_moving_end_time;
}
srchcr_ue_fast_moving_struct_type;

/* Barred Cells list of this structure type. Same as CELL_BAR_CMD struct*/
typedef struct
{
  l1_cell_bar_info_e_type l1_cell_bar_info;        /* GSM or FDD cell*/
  union
  {
    l1_gsm_cell_identity_struct gsm;
    l1_fdd_cell_identity_struct fdd;
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
    l1_eutra_cell_info_type lte;
#endif
  }id;
  uint32 t_barred;            /* T barred timer value */
#ifdef FEATURE_SUPPORT_FOR_PARTIAL_ROAMING
  boolean bar_for_resel_only; /* Unbar this barred cell info on emergency call*/
  boolean non_eq_plmn_barred; /* Unbar this on eq plmn change */
#endif

  boolean prev_freq_bar; /* Frequency barred initially from SIB.*/

#ifdef FEATURE_WCDMA_UNBAR_OPRTR_RSVD_FREQ
  /* indicates if reason for freq barring - reserved for operator use */
  boolean operator_use_barred;
#endif
#ifdef FEATURE_FEMTO_CSG
  boolean bar_non_white_csg;
#endif
  /* indicates the reason for barring - whether band mismatch or not */
  boolean band_mismatch;
} srchcr_barred_cell_struct;


/*===========================================================================

                   EXTERNAL VARIABLE DECLARATIONS

===========================================================================*/
extern uint16 srchcr_barred_cell_cnt;

extern uint16 srchcr_barred_cell_list_first_index;

extern srchcr_barred_cell_struct srchcr_barred_cell_list[WL1_MAX_BARRED_CELLS]; 

extern srchcr_rank_cell_struct_type srchcr_srched_cells[MAX_NUM_SRCHED_CELLS];

extern uint16 srchcr_cell_res_eval_sfn;

extern uint16 srchcr_ngbr_srch_opt_buffer_size_tab[10];

extern boolean srchcr_resp_early_rf_slp_allowed;

extern uint16   srchcr_srched_cells_cnt;

/*Flag inidicating whether serving is barred*/
extern boolean srchcr_pending_immdt_resel;

/*Flag indication whether reacq has been failed atleast once due to
barred cell*/
extern boolean srchcr_barred_cell_reacq_failed;  

/*Flag indication whether to ignore frequency bar or not*/
extern boolean srchcr_ignore_barred_freqs;

#ifdef FEATURE_DUAL_SIM
extern boolean wtow_bplmn_in_progress;
#endif


#ifdef FEATURE_FEMTO_CSG
extern boolean srchcr_csg_mem_allocated;
#endif
/*===========================================================================

                         VARIABLE DECLARATIONS
                                
===========================================================================*/


#ifdef FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
uint16 srchcr_tuneaway_dur_frms;
#endif

SRCH_NGBR_SRCH_OPT_LOG_PKT_type *srch_ngbr_srch_opt_log_pkt_ptr;

/* Log packet for logging rank information of cells searched */
SRCH_CELL_RESEL_RANK_LOG_PKT_type* srchcr_rank_log_ptr;
SRCH_CELL_RESEL_RANK_LOG_PKT_type srchcr_rank_log_pkt;

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
SRCH_ABS_PRIO_CELL_RESEL_RANK_LOG_PKT_type* srchcr_rank_abs_prio_log_ptr;
SRCH_ABS_PRIO_CELL_RESEL_RANK_LOG_PKT_type srchcr_rank_abs_prio_log_pkt;
#endif

#ifdef FEATURE_FEMTO_CSG
SRCH_CSG_CELL_RESEL_LOG_PKT_type *srchcr_csg_cell_resel_log_pkt_ptr;
#endif

/*===========================================================================

                                MACROS
                                
===========================================================================*/

/* Define an ASSERT which will ERR_FATAL() */
#define ASSERT_CR( x )                      \
  if ( !(x) )                               \
  {                                         \
    ERR_FATAL( "ASSERT failed!", 0, 0, 0 ); \
  }

#define SRCHCR_INTER_F_CELLS_PRESENT( ) \
  (srchsetutil_get_interf_sync_nset_count() || srchsetutil_get_interf_async_nset_count())
#define SRCHCR_INTER_F_SYNC_CELLS_PRESENT_FREQ_0( ) \
   (srch_ofreq_sync_nset[FREQ_0].cell_cnt > 0)
#define SRCHCR_INTER_F_SYNC_CELLS_PRESENT_FREQ_1( ) \
  (srch_ofreq_sync_nset[FREQ_1].cell_cnt > 0)  
#define SRCHCR_INTER_F_SYNC_CELLS_PRESENT( )  (srchsetutil_get_interf_sync_nset_count() > 0)
#define SRCHCR_INTER_F_ASYNC_CELLS_PRESENT( ) (srchsetutil_get_interf_async_nset_count() >0)

#define SRCHCR_INTER_S_CELLS_PRESENT()  (srchsetutil_get_gsm_nset_count() > 0)

#ifdef FEATURE_DUAL_SIM
#define SRCHCR_MAX_POSSIBLE_BSIC_DECODES() (WL1_IN_G2W_TA_MODE() ? 2 : 4)
#else
#define SRCHCR_MAX_POSSIBLE_BSIC_DECODES() 4
#endif

#ifdef FEATURE_REL5

#define TRESEL_ACCURACY_IDLE 100 /* 1s in frames */
#define TRESEL_ACCURACY_FACH 20 /* 0.2 s in frames*/

/* Round up to the next 0.2 s in FACH or 1s in idle*/
#define ROUND_UP(number,accuracy) \
   ( ( (number+(accuracy-1)) / accuracy) * accuracy)

/* Round up depending on state.
   In IDLE round off to nearest second. 
   In FACH round off to the nearest 0.2 sec.*/
#define SRCHCR_TRESEL_ACCURACY() \
  ((l1m_state == L1M_FACH)? TRESEL_ACCURACY_FACH : TRESEL_ACCURACY_IDLE)

#endif

#define SRCHCR_IS_TRESEL_RUNNING(cell_ptr) (cell_ptr->evt_cr_ts.enabled)

#define CHECK_WRAP_AROUND(A)   ( A= (A>1) ? 0: A)

#define SRCHCR_INTER_F_ASYNC_CELLS_PRESENT_PER_FREQ(freq_index)  (srch_ofreq_async_nset[freq_index].cell_cnt > 0)
#define SRCHCR_INTER_F_SYNC_CELLS_PRESENT_PER_FREQ(freq_index)  (srch_ofreq_sync_nset[freq_index].cell_cnt > 0)

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY

#define SRCHCR_HIGH_PRIO_SCHED_MODE() (srchcr_curr_prior_scheduling_mode == SRCHCR_HIGH_PRIORITY_SCHEDULING_MODE)

#endif

#define DETMARGIN_ECIO(meas_quan) ((meas_quan == L1_EC_NO) ? 3 : 0)
#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
#define DETMARGIN_RSCP(meas_quan) ((meas_quan == L1_RSCP)? 6 : 0)
#else
#define DETMARGIN_RSCP(meas_quan) ((meas_quan == L1_RSCP)? 5 : 0)
#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
#define IS_NOT_FACH_RESEL_CAND(idx) ((search_state == SRCH_FACH_STATE) && (!srchcr_srched_cells[idx].is_fach_absprio_resel_cand))
#endif

/* Read P_MAX from NV and increment it for correct Srxlev calculation based on 
   * UE power class. Currently NV is set to 23 dBm but we should use 24dBm for 
   * power class 3
   */
#define SRCHCRUTILS_GET_MAX_TX_POWER_NV(cell_ptr)\
    wl1_is_rf_awake() ? wl1_rfm_get_max_tx_power_nv_item(RFCOM_TXPLIM_RACH)+SRCHCR_NV_MAX_TX_PWR_INCREMENT\
                      : wl1_rfm_get_max_tx_power_in_rf_sleep(wl1_trm_get_primary_rf_device(),RFCOM_TXPLIM_RACH,wl1_rfm_convert_chan_to_band(cell_ptr->freq))+SRCHCR_NV_MAX_TX_PWR_INCREMENT;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*============================================================================
FUNCTION: SRCHCR_BYPASS_NGBR_SRCH_OPTS

DESCRIPTION:
  If the energy of the cell is less than threshold value(-25), a 15s timer is
  decremented and when the timer expires the cell is removed from the monitored
  set.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/

extern boolean srchcr_bypass_ngbr_srch_opts(srchcr_freq_idx_enum_type freq_idx);

/*===========================================================================

FUNCTION SRCHCR_CHECK_ACQ_CELL_BARRED

DESCRIPTION
  This function prevent acquistion on a barred cell
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if cell is barred.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean srchcr_check_acq_cell_barred 
(
   l1_acq_cmd_type *cmd
);

/*===========================================================================

FUNCTION SRCHCR_CHECK_CELL_BARRED

DESCRIPTION
  This function prevent check whether a fdd cell is barred.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if cell is barred.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srchcr_check_cell_barred
(
  uint16 freq,
  uint16 scr_code
);

/*===========================================================================

FUNCTION      SRCHCR_CHECK_GSM_CELL_BARRED

DESCRIPTION
    This function checks if a GSM cell is barred or not.
    Call when adding a cell to gsm nbr set.
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the cell corresponding to the input is barred.
  FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_check_gsm_cell_barred ( srch_gsm_cell_struct_type*   cell_ptr);

/*===========================================================================

FUNCTION:  SRCHCR_CHECK_INTERF_NBR_BARRED

DESCRIPTION: This function checks if the WCDMA inter frequency is barred  
  
DEPENDENCIES
  None.

RETURN VALUE
 	TRUE - If the inter frequency is barred
 	FALSE - If the inter frequency is not barred 

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcr_check_interf_nbr_barred(srchcr_inter_f_idx_enum_type freq_idx);

/*===========================================================================

FUNCTION     srchcr_chk_interf_resel_pending

DESCRIPTION
  This function checks if an inter frequency reselection is pending 
  when UE is in DRX state

DEPENDENCIES
  None.

RETURN VALUE
   TRUE -- If resel is pending when UE is in DRX state

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_chk_interf_resel_pending(void);

/*===========================================================================

FUNCTION      SRCHCR_FULL_SRCH_CONDITION_MET

DESCRIPTION
  This function checks if a full search can be performed or not in Idle mode.
  Full search can be done only if serving cell AND monitored cells.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if full search can be performed,
  FALSE, otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_full_search_condition_met( uint16 time_available );

/*===========================================================================

FUNCTION      srchcr_get_max_tx_power

DESCRIPTION
  This function returns the max transmission power of the cell whose frequency
  is passed as argument. The result is dependent on the power class of the band 
  under which the cells falls.

DEPENDENCIES
  None.

RETURN VALUE
  max transmission power of the cell 

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8  srchcr_get_max_tx_power(W_ARFCN_T frequency);

/*======================================================================
FUNCTION SRCHCR_GET_NUM_FRAMES_SINCE_PREV_EVAL

DESCRIPTION
  Function to get num frames since previous eval

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
=======================================================================*/
extern uint16 srchcr_get_num_frames_since_prev_eval(void);

/*======================================================================
FUNCTION SRCHCR_GET_WEAKEST_USET_IDX

DESCRIPTION
  Returns weakest + non target uset cell

DEPENDENCIES
  None

RETURN VALUE
    value of the index. 0xff, if none is aavailable

SIDE EFFECTS
  
=======================================================================*/
extern uint16 srchcr_get_weakest_uset_idx (void);

/*===========================================================================

FUNCTION     SRCHCR_ISS_ON_NBR_FREQ

DESCRIPTION
  This function checks if a inter-frequency reselection is in progress and
  nbch is already up for it

DEPENDENCIES
  None.

RETURN VALUE
  True  - if condition met
  False - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_iss_on_nbr_freq(void);

/*============================================================================
FUNCTION: SRCHCR_NGBR_SRCH_OPT_LOG_PKT_FILL_AND_SUBMIT

DESCRIPTION:
  This function populates the parameters for neighbor srch optimization log packet 
Log packet sample below:
2013 Nov 13  00:30:27.765  [127/0x7F]  0x41B3  WCDMA Neighbor Search Optimization
Version               = 1
Num Freq              = 3
Quan Type             = EC_NO
Detmargin EcIo        = 3
Detmargin RSCP        = 0
Prediction Margin     = 5
Mobility State        = Mobile
Std Deviation Value   = NA
Serving Cell Min EcIo = NA
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|     |        |       |      |        |        |      |        |      |      |      |      |Rank  |Rank  |               |                |                   |                   |                |                |              |Step1     |
|WCDMA|        |       |      |        |        |Cand  |        |Squal |Srxlev|      |      |Cand  |Cand  |Cond 3:        |Cond 4:         |Cond 5:            |Cond 6:            |Cond 7:         |Cond 8:         |              |Energy    |
|RF   |        |Qualmin|Qrxlev|Qoffset1|Qoffset2|Nbr   |Cand Nbr|Cand  |Cand  |Rank_S|Rank_S|Nbr   |Nbr   |Squal_cand_ngbr|Srxlev_cand_ngbr|ECIO_Rank_cand_ngbr|Rscp_Rank_cand_ngbr|Srxlev_cand_ngbr|Srxlev_cand_ngbr|              |Thresh    |
|Freq |Priority|Min FI |Min FI|Min FI  |Min FI  |EcIo  |RSCP    |Nbr   |Nbr   |EcIo  |RSCP  |EcIo  |RSCP  |< 0            |< 0             |< ECIO_rank_serv   |< Rscp_rank_serv   |< threshx_high  |< threshx_low   |Skip Full Srch|Linear    |
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|10725|      NA|  32767| 32767|   32767|   32767|  -0.0|     0.0|     2|    50|     0|   -66|     2|-32767|          false|            true|               true|              false|              NA|              NA|           Yes|        NA|
|10837|      NA|  32767| 32767|   32767|   32767|  -0.0|     0.0|     2|    50|     0|   -66|     2|-32767|          false|            true|               true|              false|              NA|              NA|           Yes|        NA|
|10700|      NA|    -20|  -115|       0|       0|  -1.0|   -67.0|    38|    94|     0|   -66|    -2|   -67|          false|           false|               true|              false|              NA|              NA|           Yes|        NA|
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_ngbr_srch_opt_log_pkt_fill_and_submit(void);

/*===========================================================================

FUNCTION       SRCHCR_OUT_OF_SERVICE_AREA_CHECK 

DESCRIPTION
  This function is called anytime the serving cell fails the S criteria.
  If the serving cell fails consistently for 12s during PCH or for 4s
  during FACH, this function returns a boolean indicating whether an
  out of service area indication needs to be sent to RRC.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if out of service area indication has to be sent.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srchcr_out_of_service_area_check( void );

/*===========================================================================

FUNCTION      SRCHCR_RANK_LOG_PKT_SUBMIT

DESCRIPTION
  This function compresses the cell reselection rank log packet and submits
  it to loging.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_rank_log_pkt_submit(void);

/*======================================================================
FUNCTION SRCHCR_START_CR_TIMER

DESCRIPTION
  Start rex timer for cell reselection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
=======================================================================*/
extern void srchcr_start_CR_timer(void);

/*===========================================================================

FUNCTION     SRCHCR_RESP_EARLY_RF_SLEEP

DESCRIPTION
  This function checks for any pending srch or on going searches, and returns a boolean

DEPENDENCIES
  None.

RETURN VALUE
  True  - No Pending or ongng searches
  False - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_resp_early_rf_sleep(void);

/*============================================================================
FUNCTION: SRCHCRUTILS_COMPUTE_SPARAMS

DESCRIPTION:
  Compute the S critiera quantities and updates them in the cell_ptr passed 
      
DEPENDENCIES:
  None.

RETURN VALUE:
NONE

SIDE EFFECTS:
NONE
=============================================================================*/
extern void srchcrutils_compute_sparams(srchcr_cell_type_enum_type cell_type, void* cell_ptr_parm,int16 q_qualmin,int16 q_rxlevmin);

/*============================================================================
FUNCTION: SRCHCRUTILS_GET_CURR_MEAS_RULES

DESCRIPTION:
  This function is called to query the current measurement rules variable.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None. 

SIDE EFFECTS:
=============================================================================*/
void srchcrutils_get_curr_meas_rules(uint8 *prev_meas_rules);

/*============================================================================
FUNCTION: SRCHCRUTILS_GET_OFREQ_PRIO

DESCRIPTION:
  This function returns the Priority of the frequency
      
DEPENDENCIES:
  None.

RETURN VALUE:
 Priority value - if frequency has priority configured
 -1 - otherwise

SIDE EFFECTS:
NONE
=============================================================================*/
extern int8 srchcrutils_get_ofreq_prio(uint16 freq);

/*============================================================================
FUNCTION: SRCHCRUTILS_GET_PREV_MEAS_RULES

DESCRIPTION:
  This function is called to query the previous measurement rules variable.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None. 

SIDE EFFECTS:
=============================================================================*/
void srchcrutils_get_prev_meas_rules(uint8 *prev_meas_rules);

/*===========================================================================

FUNCTION SRCHCRUTILS_IS_RESEL_IN_PROGRESS

DESCRIPTION
  This function will be called from scheduler to check if Resel is in progress and issue hb searches

DEPENDENCIES
  None.

RETURN VALUE
TRUE - If Intra/Inter-freq resel is in progress
FALSE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
srchcr_cell_type_enum_type srchcrutils_is_resel_in_progress(void);

/*============================================================================
FUNCTION: SRCHCRUTILS_PANIC_MODE

DESCRIPTION: This function is called to check for panic conditions. 
Serving cell barred
S criteria fails > Nserv or 4 (for edrx case)
      
DEPENDENCIES:
None.

RETURN VALUE:
Boolean
TRUE - PANIC mode
FALSE - otherwise

SIDE EFFECTS:
=============================================================================*/
extern boolean srchcrutils_panic_mode(uint8 nserv);

/*============================================================================
FUNCTION: SRCHCRUTILS_UPDATE_TRESEL_PARAMS

DESCRIPTION:
  Starts/Updates Tresel timers 
      
DEPENDENCIES:
  None.

RETURN VALUE:
NONE

SIDE EFFECTS:
NONE
=============================================================================*/
extern void srchcrutils_update_tresel_params(srch_evt_ts_struct_type* cell_evt_cr_ts);


#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY

/*===========================================================================

FUNCTION      srchcr_abs_prio_log_pkt_submit

DESCRIPTION
  This function submits the absolute priority cell reselection log packet (0x4027).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_abs_prio_log_pkt_submit(void);

/* ================================================================================= 
FUNCTION       srchcr_calculate_tresel_criteria_for_abs_prio 

DESCRIPTION
  This function is used only for REL8 UE's when Absolute Priotity 
  is used for resel eval. 

  This fn calculates the corresponding criteria based on the priority level used. 

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
=================================================================================== */
boolean srchcr_calculate_tresel_criteria_for_abs_prio (srchcr_priority_enum_type priority_level, 
                                                       int16 nbr_s_rxlev, 
                                                       uint8 nbr_threshx_high, 
                                                       uint8 nbr_threshx_low, 
                                                       srchcr_cell_type_enum_type cell_type);


/*===========================================================================

FUNCTION       SRCHCR_CHECK_HIGH_PRIO)RSSI_TMEASGSM_SRCH_DUE

DESCRIPTION
  

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcr_check_high_prio_rssi_tmeasgsm_srch_due(void);

/*===========================================================================

FUNCTION     SRCHCR_CHECK_IF_NBR_SRCHED_THIS_DRX

DESCRIPTION
  This function checks if search type specified by the input param
  was done this drx cycle

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If they were done
  FALSE - If not done

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_check_if_nbr_srched_this_drx(srchcr_cell_type_enum_type cell_type);

/*===========================================================================
FUNCTION       srchcr_find_cell_prio ()

DESCRIPTION
  This function is used only for REL8 UE's when Absolute Priotity 
  is used for resel eval. 

  This function is called from srchcr_find_best_ranked_abs_prio(), and returns the
  cell's priority based on the cell_type.

DEPENDENCIES
  None.

RETURN VALUE
  cell_priority.

SIDE EFFECTS
  None.
===========================================================================*/
extern int8 srchcr_find_cell_prio(srchcr_rank_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION       srchcr_print_srched_cells ()

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_print_srched_cells(void);

/*============================================================================
FUNCTION: SRCHCR_PRIOR_INFO_GIVEN_FOR_G_ARFCN

DESCRIPTION:
  This fn checks if a given garfcn is already present in gsm priority info sent 
  as a part of 3rd_MEAS_REQ 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:   
  None. 
=============================================================================*/

extern boolean srchcr_prior_info_given_for_g_arfcn(int8 idx,l1_meas_priority_info_type *prior_info,uint32 *prior_idx);

/*===========================================================================

FUNCTION       SRCHCRUTILS_COMPUTE_SCHEDULING_MODE

DESCRIPTION
  This function checks the following for the serving cell and returns the scheduling
  mode to be used in the current drx cycle
 
  if (srxlev > s_prioritysearch1) && (Squal > s_prioritysearch2)
    Use High Prior Scheduling
  else
    Use All Prior Scheduling


DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern srchcr_prior_scheduler_mode_enum_type srchcrutils_compute_scheduling_mode(void);

/*===========================================================================

FUNCTION       SRCHCR_COMPUTE_W_G_MEAS_RULES_FOR_HIGH_PRIOR_SCHEDULING

DESCRIPTION
  

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
srchcr_meas_rules_type srchcrutils_compute_w_g_meas_rules_for_high_prior_scheduling(void);

/*===========================================================================

FUNCTION       SRCHCR_COMPUTE_W_G_MEAS_RULES_FOR_ALL_PRIOR_SCHEDULING

DESCRIPTION
  

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

srchcr_meas_rules_type srchcrutils_compute_w_g_meas_rules_for_all_prior_scheduling(void);

#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH

/*============================================================================
FUNCTION: SRCHCRUTILS_FIND_HIGHEST_PRIORITY_RAT

DESCRIPTION:
  This function is called to query the current and previous measurement rules variables.
  For non-priority cases, the values are initialized to INT8_MIN's. Also this function will
  only be called when the change in meas rules is exclusive. For eg. MeasRule = LW --> G Only.
  The routine covers the highest priority update with the measurement rules so we will return
  what is included in meas rules only. 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  Return the high priority RAT from current meas rules bitmask. 

SIDE EFFECTS:
=============================================================================*/
extern srchfach_meas_oksn_sys_enum_type srchcrutils_find_highest_priority_rat(uint8 cur_meas_bmsk);

/*============================================================================
FUNCTION: SRCHCRUTILS_GET_OFREQ_PRIO_INFO

DESCRIPTION:
  This function returns the Priority Info pointer for the requested frequency
      
DEPENDENCIES:
  None.

RETURN VALUE:
 PriorityInfo pointer - if frequency has priority configured
 NULL - otherwise

SIDE EFFECTS:
NONE
=============================================================================*/
extern srch_inter_freq_priority_info_type* srchcrutils_get_ofreq_prio_info(uint16 freq);

#endif

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
/* ================================================================================= 
FUNCTION       srchcr_calculate_tresel_criteria_4_5_for_abs_prio 

DESCRIPTION
  This function is used only for REL8 UE's when Absolute Priotity 
  is used for resel eval. 

  This fn calculates the corresponding criteria based on the priority level used. 
   

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
=================================================================================== */
boolean srchcr_calculate_tresel_criteria_4_5_for_abs_prio (srchcr_priority_enum_type priority_level, 
                                                           srchcr_cell_type_enum_type cell_type,
 						                                               int16 nbr_s_qual, 
 						                                               uint8 nbr_threshx_high2, 
 						                                               uint8 nbr_threshx_low2 );

/*===========================================================================

FUNCTION:  SRCHCR_CHECK_LTE_FREQ_BARRED

DESCRIPTION: This function checks if the LTE frequency is barred 
 
DEPENDENCIES
  None.

RETURN VALUE
 	TRUE - If the LTE frequency is barred
 	FALSE - If the LTE frequency is not barred 

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcr_check_lte_freq_barred(uint32 earfcn);

/*============================================================================
FUNCTION: srchcr_is_lte_cell_blacklisted

DESCRIPTION:
  This function checks whether the particular set of LTE EARFCN and Physical Cell ID is 
   blacklsited or not.

   Have this function as generic, to be used outside CSG if needed
  
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE: Blacklisted
  FALSE:Not Blacklisted

SIDE EFFECTS:
  None

=============================================================================*/
extern boolean srchcr_is_lte_cell_blacklisted(uint32 earfcn, uint16 phy_cell_id);

/*============================================================================
FUNCTION: SRCHCRUTILS_IS_LTE_TRESEL_RUNNING

DESCRIPTION: This function checks if any LTE frequencies have Tresel started and returns a bitmask 
containing the indices of the frequencies which have Tresel active
      
DEPENDENCIES:
None.

RETURN VALUE:
UINT8 - Bitmask containing the frequencies with active Tresel on their cell[s]

SIDE EFFECTS:
=============================================================================*/
uint8 srchcrutils_is_lte_tresel_running(void);

/*===========================================================================

FUNCTION     SRCHCRUTILS_IS_TRESEL_RUNNING_FOR_ANY_LTE_CELLS

DESCRIPTION
  This function checks whether Tresel is running for any detected LTE cells 
  across all LTE EARFCNs.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if Tresel is running on any LTE cell
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcrutils_is_tresel_running_for_any_lte_cells(void);


#endif


#ifdef FEATURE_FEMTO_CSG

/*============================================================================
FUNCTION: SRCHCR_ALLOCATE_CSG_MEMORY()

DESCRIPTION:
  This function allocates memory for intra and interf CSG cells
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void srchcr_allocate_csg_memory(void);

/*============================================================================
FUNCTION: srchcr_csg_asf_meas_req_log_pkt_submit()

DESCRIPTION:
  This function logs ASF measurement request parameters 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_csg_asf_meas_req_log_pkt_submit(void);

/*============================================================================
FUNCTION: srchcr_csg_cell_reselection_log_pkt_add_intra_cells()

DESCRIPTION:
  This function populates intra frequency cells for CSG Cell Reselection log packet 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_csg_cell_reselection_log_pkt_add_intra_cells(void);

/*============================================================================
FUNCTION: srchcr_csg_cell_reselection_log_pkt_add_inter_cells()

DESCRIPTION:
  This function populates inter-freq cells measured for CSG Cell Reselection log packet 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_csg_cell_reselection_log_pkt_add_inter_cells(srch_interf_cell_struct_type *cell_ptr);

/*============================================================================
FUNCTION: srchcr_csg_cell_reselection_log_pkt_submit()

DESCRIPTION:
  This function populates and submits the CSG Cell Reselection log packet 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_csg_cell_reselection_log_pkt_submit(void);

/*============================================================================
FUNCTION: SRCHCR_DEALLOCATE_CSG_MEMORY()

DESCRIPTION:
  This function de-allocates memory for intra and interf CSG cells
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_deallocate_csg_memory(void);

/*============================================================================
FUNCTION: SRCHCR_FIND_INTRA_CELL_IN_SRCHED_LIST()

DESCRIPTION:
  This function finds the intra cell that was searched in this DRX cycle based
  on the Freq, psc that is passed. 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - if the cell is found.
  FALSE - otherwise. 

SIDE EFFECTS:
=============================================================================*/
boolean srchcr_find_intra_cell_in_srched_list(uint16 freq, uint16 scr_code);

/*============================================================================
FUNCTION: srchcr_get_csg_id()

DESCRIPTION:
  This function 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  Retuns the csg_id mapping to freq, psc that is passed.

SIDE EFFECTS:
=============================================================================*/
extern uint32 srchcr_get_csg_id(uint16 freq, uint16 psc, srchcr_cell_type_enum_type cell_type);

/*============================================================================
FUNCTION: SRCHCR_GET_CSG_INTERF_INDEX()

DESCRIPTION:
  This function 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  Retuns the index in the inter-f array corresponding to freq, psc that is passed.

SIDE EFFECTS:
=============================================================================*/
extern uint8 srchcr_get_csg_interf_index(uint16 freq, uint16 scr_code );

#endif

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================

FUNCTION     SRCHCR_ALLOW_HS_RACH

DESCRIPTION 
  For cell reselections that are spanning across states PCH and FACH:On entering FACH state,
  the HS RACH controller will call a SRCHCR API to find if a cell reselection in ongoing from PCH state.
  If that is the case, then HS RACH controller will ensure that RACH procedure does not get started.
  The RACH Abort state machine will ensure this.
  Only if this function returns true, UL can start RACH Tx.
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - If no reselections or Intraf resel is ongoing.
  False - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean srchcr_allow_hs_rach();

#endif

#ifdef FEATURE_WCDMA_UNBAR_OPRTR_RSVD_FREQ
/*============================================================================
FUNCTION: SRCHCR_CHECK_STEP3_BEST_PILOT_BARRED_CELL

DESCRIPTION: This function is called to check if the best pilot
detected in ACQ step3 is part of the barred cell list or not. If the
best pilot is barred then we cannot camp on the cell. If the best pilot
is not the barred cell, we can camp on the freq even though bar
type was freq barred

      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern boolean srchcr_check_nastt_best_pilot_barred_cell(void);
#endif


#endif /* SRCHCRUTILS_H */	
