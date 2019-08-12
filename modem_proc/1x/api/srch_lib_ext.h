#ifndef SRCH_LIB_EXT_H
#define SRCH_LIB_EXT_H
/*====*====*====*====*====*====*====*========*====*====*====*====*====*====*==*

        S E A R C H   T A S K  --
        S E A R C H   L I B R A R Y   E X T E R N A L   H E A D E R

GENERAL DESCRIPTION
  This file contains library interfaces from non-librarized searcher code
  into the search library.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2004 - 2015 Qualcomm Technologies, Inc.
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srch_lib_ext.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/06/15   srk     Add additional warmup slacks when LTE is in traffic.
01/15/14   as      BOLT Timeline optimizations
09/23/13   bb      Changes to split DSDS overheads between pre and post
                   RF tune processin
08/19/13   cjb     Mainline FEATURE_1X_SRCH_ROT_ONLY_AFC and
                   FEATURE_1X_SRCH_AFC_SRL ( Removed VCTCXO support )
05/09/13   cjb     Changes to export the antenna tuner periodicity macro.
02/13/13   as      Change timeline calculations to use units of microseconds
02/05/13   as      Optimized the sleep timelines
01/02/13   ab      Added Enhanced RDDS Support
06/20/12   adw     Separate is2000 and qpch reacq overheads.
03/26/12   sst     Add srch_lib_def ovrd of JCDMA API for qpch ch est thresh
02/10/12   adw     Optimize timelines and modernize overhead defines.
01/13/12   vks     Add support to request system exit on reacq fail.
12/20/11   srk     Removed reference to obsolete function srch_diag_aset_walsh
04/13/11   vks     Moved the srch task pri variables to srchzz_tl_common.c as
                   these are not being used anywhere outside idle module.
                   Added support for new task priority api.
02/16/11   vks     Moved AFC_*_SRL_* macros from custsrch.h to srch_lib_ext.h.
12/09/10   vks     Remove mdsp_app_type from the common defines as it is not
                   being used anywhere.
11/09/10   sst     Add ofs_thresh
11/09/10   jtm     Initial SU API split.
07/15/10   sst     Add in DSDS overhead time to rf warmup/sleep time calcs
07/14/10   adw     Removed featurization around aset_walsh() prototype.
07/13/10   cjb     Added Rx Tune API which accepts a callback function
                   to notify end of tuning to FTM.
07/08/10   sst     Update to resolve pack issue with walsh diag command
02/18/10   sst     Added TC AGC check flags and thresholds
10/07/09   sst     Added method to compare structure sizes inside and outside
                   of the library
09/18/09   adw     Removed srch_afc_init and moved functionality inside of
                   newly created afc_entry function to support dynamic memory
                   allocation. No longer necessary to export init function.
09/01/09   vks     Added PCH->QPCH channel estimator threshold type
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Cleaned up incs and eliminated implicit includes.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
02/10/09   adw     Merge from 2h08 branch, based on VU_MODEM_1X_SRCH.13.00.01
01/30/09   adw     Removed obsolete tcxo related warmup times.
11/19/08   adw     Merge from CMI sandbox, based on CL 787995
11/19/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_SRCH.12.00.13
06/25/08   adw     Added min_sleep_setup_time_safewin_ms and _chips
06/20/08   mca     Merge from //depot
03/13/08   sst     Modify FTS version to be determined at runtime
01/11/08   aps     Add RTR6500_WARMUP_OVERHEAD outside of srch library
11/16/07   mca     Fixed compiler warnings
10/29/07   sst     Add (en/dis)able of GOVERN_ACQ_FAIL outside of srch library
                   Unfeaturize all code GOVERN_ACQ_FAIL code except init
06/21/07   sst     Added cgps_sw_overhead_*
06/18/07   mca     Removed use of nv items defines from library
02/22/07   aps     Removed merge errors in previous checkin
02/21/07   aps     Added VCO gain and SRL multiplier knob.
01/23/07   rkc     Removed prototype for srch_srm_init().
11/20/06   aps     Added prototype for srch_ftm_rx_tune().
11/08/06   tjc      Moved tunable RX diversity autoswitching parameters int
                     srch_lib_ext.c/h
07/11/06   tjc     Modified the traffic state to use the state machine module
06/15/06   pa      Added prototypes for functions exported by AFC and Srch SRM
03/22/06   kwo     Added prototype for srch_slot_get_current_pg_slot()
11/07/05   kwo     Removed srch_fing_force_deassign_now()
11/02/05   kwo     Added prototype for srch_get_gen_pilot_set()
09/23/05   ejv     Remove prototypes which are unused or included directly.
05/31/05   rng     Merged in from SHDR Sandbox.
05/24/05   ejv     Added additional prototypes for finger interface functions.
05/18/05   kwo     Updated prototypes for library
05/04/05   kwo     Added prototypes
02/17/05   sst     Moved reference to MDSP_APP_SRCH into srch_lib_ext.c/h to
                    facilite custumer compiles will differing Feature set
01/25/05   kwo     Exported some sched parms outside of library
12/28/04   bt      implementation, first cut

=============================================================================*/


/*=============================================================================

                        INCLUDE FILES FOR MODULE

=============================================================================*/

/* Common */
#include "comdef.h"

/* Srch */
#include "srch.h"
#include "srch_rx_t.h"
#include "srchi.h"
#include "srch_diag.h"

/* Other */
#include "nv_items.h"
#include "rex.h"
#include "timetick.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* enum to describe which version of Full Time SHDR (FTS) to use */
typedef enum
{
  SRCH_USE_FTS_NONE   = 0,
  SRCH_USE_FTS_3      = 3,
  SRCH_USE_FTS_4S     = 4,
  SRCH_USE_FTS_4D     = 5
} srch_lib_ext_fts_ver_enum;

/* enum to describe which ofreq srch threshold to use */
typedef enum
{
  SRCH_USE_OFS_THRESH_LEGACY = 0,
  SRCH_USE_OFS_THRESH_1      = 1
} srch_lib_ext_ofs_thresh_enum;

/* enum to describe if system exit should be requested by SRCH on reacq fail */
typedef enum
{
  SRCH_NO_SYSTEM_EXIT_ON_REACQ_FAIL = 0,
  SRCH_SYSTEM_EXIT_ON_REACQ_FAIL = 1
} srch_lib_ext_sys_exit_on_reacq_fail_enum;

#define SRCH_USES_FTS_3 ( srch_lib_defs.common.fts_ver == SRCH_USE_FTS_3 )

/** COMMON DEFINES **/

typedef struct
{
  const int32  tpr_thresh_rc35;
  const int32  tpr_thresh_rc4;
  const int32  qual_hist_filt_gain;
  const int32  qual_hist_thr;
  const int32  qual_duration;
  const int32  prep_fpc_adj;
  #ifdef FEATURE_ENHANCED_RDDS
  const int32  sp_hist_delta_dB3;
  const int32  sp_low_delta_dB3;
  const int32  sp_high_delta_dB3;
  const int32  qof_pilots_thresh_num;
  const int32  qof_sets_thresh_num;
  const int32  quality_good_frame_thr_num;
  const int32  quality_bad_frame_thr_num;
  #endif /* FEATURE_ENHANCED_RDDS */

} srch_lib_ext_rx_div_auto_defines_type;

typedef struct
{
  const srch_lib_ext_rx_div_auto_defines_type rx_div_auto;

  /* enable and thresholds for comparing Rx0 and Rx1 AGC during
     traffic calls when diversity is enabled */
  const boolean            rx_div_agc_ck;
  const int16              rx_div_agc_dis_delta;
  const int16              rx_div_agc_en_delta;

  /* speficies the version of FTS to use */
  const srch_lib_ext_fts_ver_enum fts_ver;

  /* Periodicity of calling RF antenna tuner API (in ms) */
  const uint32             rx_rf_antenna_tuner_time_ms;

} srch_lib_ext_common_defines_type;

/** END COMMON DEFINES **/

/** INIT DEFINES **/

typedef struct
{
  const timetick_type cd_govern_dur_ms;
  const boolean       cd_govern;
  const timetick_type acq_govern_dur_ms;
  const boolean       acq_govern;
} srch_lib_ext_init_defines_type;

/** END INIT DEFINES **/

/** IDLE DEFINES **/

typedef struct
{
  /* The following types must never be changed to
     uint32 because then some of the comparisons
     with int32's in the code will fail due to
     promotion from int32 to uint32 */

  /* RF prep overhead times */
  const uint16 rf_prep_clk_overhead_us;
  const uint16 rf_prep_fw_overhead_us;
  const uint16 rf_prep_rf_overhead_us; /* Overridden if "use_rf_warmup_api" set */
  const uint16 rf_prep_sw_overhead_us;
  const uint16 rf_prep_sw_offtl_overhead_us;
  const uint16 rf_prep_sw_dsds_overhead_us; /* DSDS overhead before RTC ON */
  const uint16 rf_prep_sw_lte_ca_overhead_us; /* LTE CA overheads */

  /* RF exec overhead times */
  const uint16 rf_exec_rf_overhead_us; /* Overridden if "use_rf_warmup_api" set */
  const uint16 rf_exec_sw_overhead_us;
  const uint16 rf_exec_sw_offtl_overhead_us;
  const uint16 rf_exec_sw_dsds_overhead_us; /* DSDS overhead after RTC ON */
  const uint16 rf_exec_sw_lte_ca_overhead_us; /* LTE CA overheads */

  /* Reacq overhead times */
  const uint16 reacq_time_overhead_is2000_us;
  const uint16 reacq_time_overhead_qpch_ontl_us;
  const uint16 finga_time_overhead_us;

  /* Misc overhead times */
  const uint16 cgps_sw_overhead_us;
  const uint16 min_sleep_setup_time_safewin_us;
  const uint16 sleep_processing_delay_us;

  /* Use RF warmup API */
  const boolean use_rf_warmup_api;

} srch_lib_ext_warmup_defines_type;

typedef struct
{

  const uint32  reacq_win_noise_thresh_lecio;
  const int32   fast_raho_srch_penalty;
  const int32   fast_raho_reg_penalty;
  const uint32  failed_reacq_scan_time_ms;
  const srch_lib_ext_ofs_thresh_enum ofs_thresh;
  const srch_lib_ext_sys_exit_on_reacq_fail_enum sys_exit_on_reacq_fail;

} srch_lib_ext_sched_defines_type;

/* QPCH channel estimator threshold
   Note, by choosing TYPE_OLD, it will override the JCDMA/Passport mode API
   use of the TYPE_NEW will allow the JCDMA/Passport mode API to selectively
   switch between the two thesholds */
typedef enum
{
  THRESH_TYPE_OLD,                /* note this overrides the Passport API */
  THRESH_TYPE_NEW
}srch_lib_ext_ch_est_thresh_tab_type;

typedef struct
{
  const srch_lib_ext_ch_est_thresh_tab_type  ch_est_thresh_tab_type;
} srch_lib_ext_qpch_defines_type;


typedef struct
{
  const srch_lib_ext_warmup_defines_type warmup;
  const srch_lib_ext_sched_defines_type  sched;
  const srch_lib_ext_qpch_defines_type   qpch;

} srch_lib_ext_idle_defines_type;

/** END IDLE DEFINES **/

typedef struct
{
  const nv_items_enum_type nv_cdma_rx_diversity_ctrl_i;
  const nv_items_enum_type nv_cdma_rx_chain_sel_thresh_i;
  const nv_items_enum_type nv_srch_dbg_mask_i;
  const nv_items_enum_type nv_cdma_rx_div_bc_enable_mask_i;

} srch_lib_ext_nv_items_defines_type;

/** TRAFFIC DEFINES **/
/** END TRAFFIC DEFINES **/

typedef struct
{
  const srch_lib_ext_common_defines_type   common;
  const srch_lib_ext_init_defines_type     init;
  const srch_lib_ext_idle_defines_type     idle;
  const srch_lib_ext_nv_items_defines_type nv_items;

} srch_lib_ext_defines_type;


/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/
extern const srch_lib_ext_defines_type srch_lib_defs;


/*-------------------------------------------------------------------------
      Externalized functions from srchidle_ctl

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern srch_state_enum_type srch_idle_ctl_state(void);

/*-------------------------------------------------------------------------
      Externalized functions from srch_idle_util

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_idle_init(void);
extern boolean srch_idle_slotted_mode_active(void);


/*-------------------------------------------------------------------------
      Externalized functions from srch_slot

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern int16 srch_slot_get_next_slot_offset(void);

/* REVISIT: This function is being called by customers,
   we need to add a public interface for this */
extern uint16 srch_slot_get_current_pg_slot(void);


/*-------------------------------------------------------------------------
      Externalized functions from srch_chan

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_chan_reset (void);

/*-------------------------------------------------------------------------
      Externalized functions from srchzz_qpch_util

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern uint32 srchzz_get_qpch_ch_est(void);


/*-------------------------------------------------------------------------
      Externalized functions from srch_pcg

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern srch_state_enum_type  srch_pcg_state ( void );


/*-------------------------------------------------------------------------
      Externalized functions from l1m

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void l1m_init (void);


/*-------------------------------------------------------------------------
      Externalized functions from srch_fing_driver

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_fing_reset_chans( void );
extern uint32 srch_fing_get_max_fing_cnt( void );
extern void srch_fing_disable( uint32 first_fing, uint32 last_fing );
extern void srch_fing_init( void );
extern void srch_fing_set_dmod_pilot_gain( srch_rx_freq_range_type freq_range );


/*-------------------------------------------------------------------------
      Externalized functions from srch_diag

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_diag_init (void);
extern void srch_get_gen_pilot_set ( void *pkt_ptr );
extern void srch_diag_get_pilot_set ( srch_pilot_set_struct_type *pkt_ptr );

/*-------------------------------------------------------------------------
      Externalized functions from srchtc

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void  srch_tc_save_fpc_info ( boolean fpc_enabled, uint16 mode );

extern srch_state_enum_type  srch_tc_state ( void );

/*-------------------------------------------------------------------------
      Externalized functions from srch afc

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern int16 srch_afc_read_vco_accum(void);

/*-------------------------------------------------------------------------
      Externalized functions from srch ftm

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_ftm_rx_tune
(
  srch_rx_band_type band,        /* band class     */
  srch_rx_chan_type chan         /* channel number */
);

extern void srch_ftm_rx_tune_w_cb
(
  srch_rx_band_type band,        /* band class     */
  srch_rx_chan_type chan,        /* channel number */
  void (*cb) (void)              /* call back      */
);

/*-------------------------------------------------------------------------
      Externalized functions from srch_lib_ext
-------------------------------------------------------------------------*/
/*=============================================================================

FUNCTION SRCH_LIB_EXT_CHECK_STRUCTS

DESCRIPTION    This functions is used to compare the sizes of srch structures
               inside and outside of the srch library.

DEPENDENCIES   This function must match srch_lib_int_check_structs which is
               inside the library

RETURN VALUE   Size of structures used inside and outside the srch library

SIDE EFFECTS   None

=============================================================================*/
extern uint32 srch_lib_ext_check_structs( void );

#endif /* SRCH_LIB_EXT_H */
