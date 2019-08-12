#ifndef L1_SC_INT_H
#define L1_SC_INT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              l1_sc_int.h

GENERAL DESCRIPTION
   This module contains I/f to the SCE. Functions starting with l1_sc_
   are implemented in the SCE. Functions starting with l1_scdrv_ are
   callbacks to be called by the SCE and are implemented in the L1.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013, 2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_sc_int.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ---------------------------------------------------------- 
06/06/16     km      CR1017936 FR 35777: Enable Samsung 'Big Data' feature on 8917 - GERAN GL1
29/03/16   akm       CR739561 SCE opt top2/top6 mode, disable pwr msr of blacklisted cells
13/11/15   df        CR935037 Allow SCE stop/start in non-idle modes with FR23269 M2M devices
09/10/15   df        CR921075 FR23269 M2M devices: Handle RR->GL1 MPH_UPDATE_IDLE_DRX_IND
03/10/14   aga       CR732455 Changes required to tune away framework for TH 1.0 single sim build 
23/09/14   ab        CR727993 : GBTA - update Power mon state when callback are installed.
16/09/14   cgc       CR723214 Avoid forced stale irat measurements when sub1 is blocked by sub2 MO/MT.
12/09/14   nm        CR723570 Port GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
06/08/14   jj        CR 705509 reducing scell meaurement when BA list is empty in dedicated mode
31/07/14   xz        CR697883 Not allow GBTA if current idle frame is doing TDS IRAT in xfer mode
20/06/14   jj        CR656872 fix critical gl1 kw error  
08/08/14   cgc       CR682787 Hopping conflict fix for G2X on SDCCH propagation from DI2.3
25/07/14   am        CR698669 Transfer mode SCE should not be triggered until all g2l_timed_searchs are completed
10/07/14   cgc       CR631007 FR21768 enhancement force G2X measurement MPH_SPECIFIC_IRAT_FREQ_UPDATE_REQ
29/04/14   pjr       CR600612 Dedicated WTR Hop initial code drop
06/03/14   br        CR626780, RACE condition between TRANSFER G sub and IDLE G sub, issuing SCH in G2x gap.
19/12/13   jk        CR503359 - GL1 changes to support Rx diversity
12/12/13   ssh       CR588188: ISR overrun recovery mechanism
04/12/13   ap        CR584084 Addressing WCDMA reconfirmation search whilst GERAN in Transfer
05/11/13   cja       CR523987 Add FEATURE_GSM_DED_SCELL_MONITOR for extra SCell monitor in voice call
08/10/13   sp        CR561007: Start SC meas based on mode
01/11/13   ss        CR569849: Declare l1_sc_wcdma_ded_tick_abort_stuck_srch in a header file
08/10/13   sp        CR561007: Start SC meas based on mode
24/07/13   ss        CR515896  Set wcdma_list_update_inprogress to TRUE in msg_handler routine 
15/08/13   sk        CR524039 GBTA bringup changes
21/06/13   cs        Major Triton TSTS Syncup
29/05/13   dv        CR492212 Introduce SM to handle G2W abort 
20/05/13   npt       CR417466 - Ensure that opt3 is not kicking in
                     if change to GSM only mode is being processed
13/09/12   ws        CR 399455 Fix compile errors when FEATURE_LTE_TO_GSM is disabled
19/04/12   pa        CR349239: G2T handling of idle mode IRAT measurement using FM
28/01/11   og        Stop rogue WCDMA reports from being processed by RR when
                     there are no WCDMA cells / UARFCNs stored in L1.
09/12/10   cs        CR264682 Enable the asynch int processing for sleep
25/06/10   pg        CR244125: Add running avg meas_buf ptr sanity check for Q6
21/06/07   og        Ensure the afc information ise set correctly when doing G2G
                     non-blind handover. CR122525 (CR120843).
05/03/07   cs        Making CR112989 calculation consistent with sc updates
29/09/05   og        Merge to fixes of CR74437 and CR70332 from the mainline branch
                     Fixes inter RAT TC and 42.4.8.4.5 respectively.
08/12/05   ws        Added initial FEATURE_GSM_DTM support
08/03/05   sv        Updating and adding prototype functions.
07/29/05   gfr       Support for NPL1 DTM interface changes
02/02/05   bm        Support for ID searches in TBF
01/26/05   sv        Add support for Frame Manager.
01/05/05   dp        Merged dedicated timeline changes for audio optimizations
                     to mainline
10/20/04   sv        Optimization of Ncell SCE awake time
10/13/04   kf        Changes for time slot slamming in transfer.
06/22/04   dp        Lint cleanup
06/21/04   gsc       Pass in CBCH gap to l1_sci_tick_idle along with ccch gap.
                     Changed the data type for the parameters to uint32.
03/16/04   gw        Removed prototypes for some obsolete functions.
02/25/04   gw        Lint clean up.
02/03/04   jc        Chnages to support new meas reporting for gtow HO.
02/02/04   kf        Added gprs_gpl1_do_serve_cell_bcch_trans to
                     gpl1_gprs_meas_mode_T.
01/28/04   gw        Changes to support reselection optimizations.
1/20/04    jc        Updated gtow function prototype.
11/05/03   gfr       Support for quad-band.
09/03/03   sw        Additional prototypes for WCDMA/transfer
08/18/03   sw        Added WCDMA measurements during transfer
08/15/03   dp        Added prev_rx_power_sum and prev_rx_power_count
12/08/03   kf        Removed sch_timing_error.
07/08/03   kf        lint changes.
08/07/03   tb        Added prototype for l1_sc_wcdma_sdcch_tick().
08/01/03   tb        Added prototype for l1_sc_wcdma_ded_pending().
07/14/03   gw        Added prototype for l1_sci_go().
07/11/03   gfr       Lint cleanup.
07/08/03   gw        GRR/L1 SCE cleanup. Changes to prototypes related to ncell BCCH decoding.
06/06/03   tb        Added support for dedicated mode gtow searching.
29/05/03   kf        ncell pbcch read changes
05/12/03   dp        Modified l1_sc_balist_gprs_set_sync interface to handle
                     timeslot reconfig
05/01/03   dp        Changes to preserve Ncell BSIC across transitions
05/02/03   gw        Merged in power transfer changes from branch.
05/01/03   ag        Updates to ncell pbcch reading
04/28/03   tb        Added l1_sc_wcdma_set_mode()
04/28/03   gw        Changes to support optimization of code for preserving
                     power measure info.
04/16/03   sw        Added initial changes for WCDMA neighbour cell search
                     during packet idle on PCCCH
04/09/03   gw        Removed proto. for l1_sc_add_idle_serving_cell_to_balist().
04/04/03   gw        Added declarations to support transferring ncell power info
                     between GSM modes.
04/02/03   gw        Changes to reflect modifications in monscan module.
02/28/03   kf        Added support for ncell pbcch reading
17/02/03   kf        Changes for running average in transfer.
02/13/03   tb        Changed return type of l1_sc_wcdma_leave_idle
02/07/03   ag        Updates for FEATURE_GPRS_TBF_SUSPEND
01/28/03   tb        More defines for INTERRAT_GTOW
01/10/03   gw        Moved/renamed some BA list functions.  Removed elements
                     rank_rx_power and prev_rank_rx_power from l1_sc_bs_T,
                     added prev_rx_power.
12/18/02   ag        Added timing_error to gpl1_gprs_meas_mode_T under switch
                     FEATURE_GPRS_TBF_SUSPEND
12/03/02   mk        General GPRS featurization cleanup.
12/02/02   ws        Added new structures for sb burst scheduling
11/13/02   mk        Updated GPRS baseline.
11/03/02   tb        Added support for INTERRAT_GTOW
10/08/02   kf        Added PL1 GPRS measurement code hooks under switch
                     FEATURE_GPRS_MEAS.
07/26/02   dp        Added rank_rx_power and prev_rank_rx_power to l1_sc_bs_T
07/15/02   dp        Added prototype for l1_sc_set_sync_dedi.  Added
                     sch_wrap_direction to l1_sc_bs_T
07/02/02   gw        Moved running average declarations here from other header
                     files.  Added running_avg_struct to l1_sc_bs_T.
06/27/02   gw        Changes to support running averages of ncell power
                     measurements in Idle Mode.
05/21/02   gw        Removed expected_power from l1_sc_bs_T.
03/28/02   JC/dp     Temp put in l1_sci_do_monscan_ded till we consolidate with idle
03/20/02   JC        Added AGC_T to l1_sc_bs_T to support AGC on mon chan's.
02/07/02   gw        Changes to support improvement of SCE aborts.
12/21/01   gw        Changed sb_struct to support DRX scheduling.
10/24/01   dp        Added field tone_detected to l1_sc_bs_T to support separate
                     SC tone detection and SCH decodes on the TCH
10/16/01   gw        Changed prototpye for l1_sci_do_mon_scan.  Removed protos
                     for functions that have been removed.
09/25/01   mk        Mainlined MONITOR_DEBUG.
09/11/01   JC        Added QCT template functions. Replaced sword with int16.
08/17/01   mk/gw     Added Ncell support, featurized via MONITOR_DEBUG.
08/16/01   mk        Initial version.  Minor Cleanup and Header Addition.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Alphabetical Order */
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gl1_msg.h"
#include "gl1_msg_g.h"
#include "gl1_sys_algo.h"
#include "rr_l1.h"
#include "rr_l1_g.h"

#include "geran_dual_sim.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_GSM_DED_SCELL_MONITOR
#error code not present
#endif /*  FEATURE_GSM_DED_SCELL_MONITOR */
typedef enum
{
   L1SCModeNull,
   L1SCModeIdle,
   L1SCModeDediSDCCh,
   L1SCModeDediTCh,
   L1SCModeSameAsBefore,
   L1SCModeGprsIdle,
   L1SCModeGprsTrans
#ifdef FEATURE_GSM_DTM
   ,L1SCModeDediDTM
#endif
} l1_sc_mode_T;

typedef enum
{
   L1SCMeasNone,
   L1SCMeasNew,
   L1SCMeasOld
} l1_sc_meas_T;

/* Type for implementing running averages of measurements */
typedef struct
{
   dBx16_T  *meas_buf;
   uint8    buf_len;
   uint8    avg_len;
   uint8    num_meas;
   int32    total;
   uint8    oldest_ptr;
   uint8    newest_ptr;
   boolean  just_stuffed;
} running_avg_struct;

typedef enum
{
    gprs_idleminus1,
    gprs_idle,
    gprs_idleplus1
}tick_sch_T;

typedef struct
{
   boolean  nc_mode;
   boolean  allowed_to_go;
   boolean  load_handler;
   boolean    tick_fch;
   tick_sch_T tick_sch;
   boolean   hopping;
   boolean gprs_gpl1_do_serve_cell_bcch_trans;
   boolean leave_trans_adjust_sync;
   boolean enter_trans_adjust_sync;
   boolean nc_new_ba_list_pending;
} gpl1_gprs_meas_mode_T;

gpl1_gprs_meas_mode_T* gl1_ms_switch_meas_mode_data(gas_id_t gas_id);

/*
 * Example for fn_lag and qbit_lag in l1_sc_bs_T
 *
 * neighbour BS
 * . . | . . . 7 . . . | . . . 8 . . . | . . . 9 . . . |
 * current BS
 * | . . . 8 . . . | . . . 9 . . . | . . . 10. . . | . .
 *
 * fn_lag   = 1
 * qbit_lag = 1250
 */

typedef struct
{
   ARFCN_T            arfcn;
   byte               TN;
   int16              qbit_lag;       /* only valid if bsic_known == TRUE */
   dword              fn_lag;         /* only valid if bsic_known == TRUE */
   byte               sb_data[ 4 ];   /* only valid if bsic_known == TRUE */
   int16              sch_wrap_direction; /* only valid if bsic_known == TRUE */
   byte               bsic;           /* only valid if bsic_known == TRUE */
   boolean            bsic_known;
   boolean            tone_detected;
   dBx16_T            rx_power;       /* last averaged power level        */
   dBx16_T            prev_rx_power;  /* previous averaged power level    */
   sys_algo_agc_T     agc;
#ifdef FEATURE_GSM_RX_DIVERSITY
   sys_algo_agc_T     agc_divrx;
#endif
   l1_sc_meas_T       meas_state;
   int32              rx_power_sum;   /* for integrate and dump           */
   int32              prev_rx_power_sum;   /* for integrate and dump           */
   uint16             rx_power_count; /*   in Dedicated Mode              */
   uint16             prev_rx_power_count; /*   in Dedicated Mode              */
   dBx16_T            rx_power_instant; /* instantaneous power */
   running_avg_struct ra;             /* for running avg in Idle Mode     */

   /* @@OG: fine freq */
   int16              frequency_offset;
} l1_sc_bs_T;


typedef enum
{
    FORCE_MEAS_NULL        =0x00,
    FORCE_MEAS_ACTIVE      =0x10,
    FORCE_MEAS_COMPLETED   =0x80,
    FORCE_MEAS_MASK        =0xF0,
    FORCE_MEAS_COUNT_MASK  =0x0F,
    
} g2x_forced_meas_e;



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
void         l1_sc_init( gas_id_t gas_id );
void         l1_sc_shutdown( gas_id_t gas_id );
void         l1_sc_start( l1_sc_mode_T mode, gas_id_t gas_id );
void         l1_sc_stop( gas_id_t gas_id );
void         l1_sci_go( gas_id_t gas_id );
void         l1_sci_tick_idle( gas_id_t gas_id );
void         l1_sci_post_tick_idle( gas_id_t gas_id );
void         l1_sci_tick_dedi_sdcch( uint32 available_frames, boolean stopping, gas_id_t gas_id );
void         l1_sci_tick_dedi_tch( gas_id_t gas_id );
void         l1_sci_paging_block_tick( gas_id_t gas_id );
boolean      l1_sci_check_ok_to_sleep( uint32 gap_to_next_activity, gas_id_t gas_id );

void         l1_sc_wcdma_init( gas_id_t gas_id );
void         l1_sc_wcdma_set_mode(boolean gsm_only, gas_id_t gas_id );

void         l1_sc_ded_wcdma_update_list(mph_dedicated_wcdma_cell_update_list_req_T* msg,
                                         gas_id_t gas_id );
void         l1_sci_start_wcdma_srch(gas_id_t gas_id);
void         l1_sc_wcdma_enter_dedi(gas_id_t gas_id);
void         l1_sc_wcdma_leave_dedi(gas_id_t gas_id);
boolean      l1_sc_wcdma_ded_pending(gas_id_t gas_id);
boolean      l1_sc_wcdma_ded_ready(boolean high_priority_only, gas_id_t gas_id);
void         l1_sc_wcdma_ded_tick(boolean srch_pending, boolean srch_scheduled, gas_id_t gas_id);
void         l1_sc_wcdma_sdcch_tick(gas_id_t gas_id);
void         l1_sc_wcdma_abort_ded_search(gas_id_t gas_id);
void         L1_send_MPH_SURROUND_WCDMA_DED_MEAS_IND( word num_cells,
                                                      wcdma_dedicated_meas_T cells[], gas_id_t gas_id);
void         l1_sc_wcdma_ded_tick_abort_stuck_srch(gas_id_t gas_id);

void         l1_sc_wcdma_enter_xfer(gas_id_t gas_id);
void         l1_sc_wcdma_leave_xfer(gas_id_t gas_id);
boolean      l1_sc_wcdma_xfer_pending(gas_id_t gas_id);
boolean      l1_sc_wcdma_xfer_ready(boolean high_priority_only, gas_id_t gas_id);
void         l1_sc_wcdma_xfer_tick(boolean wcdma_srch_pending,
                                   boolean wcdma_srch_scheduled, gas_id_t gas_id );

void         L1_send_MPH_SURROUND_WCDMA_XFER_MEAS_IND( word num_cells,
                                                      wcdma_dedicated_meas_T cells[], gas_id_t gas_id);

void         l1_sc_wcdma_abort_xfer_search(gas_id_t gas_id);




void         l1_sc_wcdma_set_paging_multiframes(uint16 pg_mfrms, gas_id_t gas_id );
boolean      l1_sc_get_wcdma_list_update_inprogress(gas_id_t gas_id);
void         l1_sc_set_wcdma_list_update_inprogress(boolean update_inprogress  , gas_id_t gas_id );
void         l1_sc_wcdma_update_list( mph_wcdma_cell_update_list_req_T* msg, gas_id_t gas_id );
void         l1_sc_wcdma_ignore_neighbors( mph_ignore_wcdma_neighbors_req_T* msg );
boolean      l1_sc_wcdma_get_best_six( best_six_wcdma_info_T* best_six, gas_id_t gas_id );
void         l1_sc_wcdma_enter_idle( gas_id_t gas_id );
boolean      l1_sc_wcdma_leave_idle( gas_id_t gas_id );
void         l1_sc_wcdma_receive( void ( *done )( void*,gas_id_t gas_id ),gas_id_t gas_id );
void         l1_sci_tick_wcdma( gas_id_t gas_id );
void         l1_sci_post_tick_wcdma( gas_id_t gas_id );
/* CR 349239*/
void         l1_sci_tick_sched_reconfim(gas_id_t gas_id);

uint8        l1_sc_get_tch_idle_frame(gas_id_t gas_id);
l1_sc_mode_T l1_sc_get_curr_mode(gas_id_t gas_id);

void         l1_sc_set_rat_change_active( boolean active, gas_id_t gas_id);
boolean      l1_sc_get_rat_change_active( gas_id_t gas_id );
void         l1_sc_irat_abort_g2w_reconfirm_gap( gas_id_t gas_id );
boolean      l1_sc_g2x_sdcch_inprogress(gas_id_t gas_id);

/*===========================================================================

FUNCTION l1_sc_wcdma_srch_active

DESCRIPTION
  Used by gdrivers to be able to identify when a W search is really being
  activated.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean l1_sc_wcdma_srch_active( gas_id_t gas_id );

/*===========================================================================
FUNCTION l1_sc_ncell_pch_low_monitor_active

DESCRIPTION
  Tell the drivers alyer if we are in the low monitor state for asynch
  interrupt sleep handling
===========================================================================*/
extern boolean l1_sc_ncell_pch_low_monitor_active( gas_id_t gas_id );

void         l1_sc_wcdma_receive_id_in_tbf( void ( *done )( void*, gas_id_t gas_id ), gas_id_t gas_id );
void         l1_sci_tick_wcdma_id_in_tbf( uint32 available_frames, gas_id_t gas_id );
void         l1_sc_wcdma_abort_id_tbf(gas_id_t gas_id);
boolean      l1_sc_wcdma_start_id_in_tbf(gas_id_t gas_id);

#ifdef  FEATURE_GSM_TO_LTE
boolean l1_sc_g2l_timed_search_active( gas_id_t gas_id );
#endif /* FEATURE_GSM_TO_LTE */

#ifdef FEATURE_G2X_TUNEAWAY
boolean l1_sc_wcdma_id_searching_in_tbf(gas_id_t gas_id);
void l1_print_sc_wcdma_id_searching_state(gas_id_t gas_id);
#endif /* FEATURE_G2X_TUNEAWAY */
boolean      l1_sci_wcdma_paging_block_tick( gas_id_t gas_id );
#ifdef FEATURE_GPRS_GBTA
void         l1_sci_paging_block_gbta_tick( gas_id_t gas_id );
#endif /* FEATURE_GPRS_GBTA */
void         l1_scdrv_wcdma_idle_meas_ind( gas_id_t gas_id );
boolean      l1_sci_is_wcdma_ready( uint32 gap_to_next_activity, gas_id_t gas_id );
void         l1_sci_wcdma_abort_reconfirm( gas_id_t gas_id );

void         l1_sci_monscan_do_idle( uint8 num_meas, uint8 meas_per_frame, gas_id_t gas_id );
void         l1_sci_monscan_do_ded( uint8 num_meas, uint8 meas_per_frame, boolean monscan_ded_tch_mode, gas_id_t gas_id );
void         l1_sci_monscan_go( gas_id_t gas_id );

void         l1_sc_set_paging_multiframes( word paging_multiframes, gas_id_t gas_id );
void         l1_sc_set_ba_list( inter_task_BCCH_allocation_T* ba_list, byte ncc_permitted, gas_id_t gas_id );
void         l1_sc_set_sync( uint32 frame_decrease, int16 qbit_decrease, gas_id_t gas_id );
void         l1_sc_ba_cell_set_sync( l1_sc_bs_T *bs, uint32 frame_decrease, int16 qbit_decrease, gas_id_t gas_id );
void         l1_sc_balist_gprs_set_sync( boolean ts_reconfig );
void         l1_scdrv_mon_ind( gas_id_t gas_id );
void         gl1_set_ncell_block_start( boolean value, gas_id_t gas_id );
boolean      gl1_is_ncell_block_start( gas_id_t gas_id );
void         gl1_set_pwr_meas_block_start( boolean value, gas_id_t gas_id );
boolean      gl1_is_pwr_meas_block_start( gas_id_t gas_id );
boolean      l1_sci_is_op_in_progress( gas_id_t gas_id );

void         l1_running_avg_init( running_avg_struct *ra_ptr, dBx16_T *buf, uint8 buf_len );
void         l1_running_avg_set_avg_len( running_avg_struct *ra_ptr, uint8 avg_len, gas_id_t gas_id );
void         l1_running_avg_add_meas( running_avg_struct *ra_ptr, dBx16_T meas, gas_id_t gas_id );
dBx16_T      l1_running_avg_get_avg( running_avg_struct *ra_ptr );
uint8        l1_running_avg_get_num_meas( running_avg_struct *ra_ptr );
void         l1_running_avg_clear_meas( running_avg_struct *ra_ptr );
int32        l1_running_avg_get_total ( running_avg_struct *ra_ptr );
boolean      l1_running_avg_is_full( running_avg_struct *ra_ptr );
void         l1_running_avg_stuff_buf( running_avg_struct *ra_ptr, dBx16_T power, gas_id_t gas_id );
dBx16_T      l1_running_avg_get_last_meas( running_avg_struct *ra_ptr );
void         l1_running_avg_sanity_check_meas_buf( running_avg_struct *ra_ptr, gas_id_t gas_id );

boolean      l1_sc_is_sce_running(gas_id_t gas_id);
#ifdef FEATURE_IDLE_DRX_SCALING
void         l1_running_avg_clear_all_meas(gas_id_t gas_id);
boolean      l1_check_is_sc_in_idle(gas_id_t gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
void         l1_sci_monscan_do_rxlev_trans( uint8 num_meas, uint8 meas_per_frame, gas_id_t gas_id );
void         l1_sc_monscan_abort_gprs( gas_id_t gas_id );
void         gpl1_gprs_update_bcch( ARFCN_T                               arfcn,
                                    grr_l1_surround_cell_si_requested_t   si_requested,
                                    normal_extended_t                     si13_normal_or_ext,
                                    gas_id_t gas_id );
void         gpl1_gprs_update_bcch_stop( boolean send_ind, gas_id_t gas_id );

#ifdef FEATURE_GPRS_GBTA
boolean l1_sc_check_idle_mon_allowed(gas_id_t gas_id);
boolean l1_sci_meas_in_progress( gas_id_t gas_id );
boolean l1_sc_get_monscan_active( gas_id_t gas_id );
#endif /* FEATURE_GPRS_GBTA */

boolean l1_sci_is_op_in_progress_gbta(gas_id_t gas_id );

#ifdef FEATURE_GSM_TDS
boolean l1_sc_tds_irat_in_idle_frame(gas_id_t gas_id);
void l1_sc_tds_irat_in_idle_frame_clear(gas_id_t gas_id);
#endif
#if ( defined ( FEATURE_DUAL_SIM ) || defined( FEATURE_TRIPLE_SIM ) ) \
    && defined(FEATURE_WCDMA)
/*===========================================================================

FUNCTION l1_sci_trm_blocked_inc_wcdma_page_cycle_tick

DESCRIPTION
  Used to increment the page_cycle_tick, if the lock wasn't granted in dual sim
  builds. This will ensure that the scheduling remains on track.
===========================================================================*/
void l1_sci_trm_blocked_inc_wcdma_page_cycle_tick( gas_id_t gas_id );
#endif /* ( FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM ) && FEATURE_WCDMA */

void    l1_sc_specific_irat_freq_update(mph_specific_irat_freq_update_req_T *pMsg, gas_id_t gas_id);
void    l1_sc_forced_meas_expiry(gas_id_t gas_id);

#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST
void    l1_sc_reduced_ncell_list_status(
            boolean *reduced_ncell_list_stored_in_l1,
            boolean *reduced_ncell_list_mode_active,
            boolean *reduced_ncell_list_allowed,
            gas_id_t gas_id );
void    l1_sc_update_sorted_ba_list( boolean include_serving, gas_id_t gas_id );
void    l1_sc_config_reduced_ncell_list_rr( boolean reduced_ncell_list_en, gas_id_t gas_id );
void    l1_sc_set_bmask_reduced_ncell_list( uint8 reduced_ncell_bmask, gas_id_t gas_id );
void    l1_sc_clear_bmask_reduced_ncell_list( uint8 reduced_ncell_bmask, gas_id_t gas_id );
void    l1_sci_set_bmask_reduced_ncell_list( uint8 reduced_ncell_bmask, gas_id_t gas_id );
void    l1_sci_clear_bmask_reduced_ncell_list( uint8 reduced_ncell_bmask, gas_id_t gas_id );
#endif /* #ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST */

#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST_DEBUG
void    l1_sc_balist_print_running_avg( gas_id_t gas_id );
#endif

#endif /* L1_SC_INT_H */

