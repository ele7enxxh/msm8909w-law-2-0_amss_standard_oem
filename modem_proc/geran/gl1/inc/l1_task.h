#ifndef L1_TASK_H
#define L1_TASK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L1   T A S K   P R O C E S S I N G
                       H E A D E R   F I L E

GENERAL DESCRIPTION
   This header file contains declarations relevant to L1 task processing.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_task.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $


when       who      what, where, why
--------   -------- ---------------------------------------------
09/10/15   df       CR921075 FR23269 M2M devices: Handle RR->GL1 MPH_UPDATE_IDLE_DRX_IND
29/05/14   mko      CR639716 GCF TC 21.1 failure, Limit the changes of CR592893 to idle mode only
03/11/14   sb       CR749906 Correctly featurise the sending of IRAT measurements
18/09/14   am       CR725978 Enable rach_ready_to_send for GPLT
21/08/14   dv       CR711417 Ensure DR mode is enabled when camping on cell 
06/08/14   df       CR536059 Allow Asynchronous sleep for all idle activities
29/07/14   ap       CR695234: Changes for GL1's New Rach interface
17/06/14   cgc      CR680138 expose gl1_handle_message_router_msgs()
10/07/14   js       CR593074- ROTA/QBTA Feature implementation
10/07/14   ka       CR628921 Use LTE earfcn type in gl1 to allow move to 32bit earfcn's
04/07/14   cs       CR688003 GRM functions moved to gpl1_grm_intf.c
30/05/14   ws       CR657674 Data + MMS Phase 1
25/06/14   cs       Remove FEATURE_ASYMMETRIC_GERAN_BANDS
25/06/14   cs       Mainline FEATURE_GSM_DIME_TRM
17/06/14   cs/ch    CR651795 TRM Updated changes for DR-DSDS
29/05/14   cs/pa    CR660831 Use new trm_status definitions specific to GL1
12/05/14   ssh      CR661514 Additional protection around panic reset handling [workaround for buffer overflow]
08/05/14   sp       CR655478:Handle Unsupported Bands on WTRs while RANE during Powerscan.
28/04/14   dv       CR655739 ensure GFW does not sleep before receiving STOP CNF from G2X RATS
16/04/14   cgc      CR641709 Check TDSCDMA_IRAT_INIT_CNF response for failure and reset TDS irat state
09/04/14   am       CR639289 Report SNR in MPH_FCCH_SCH_DECODE_IND to GRR
02/04/14   cs       CR536059 Allow Asynchronous sleep for all idle activities
14/03/14   am       CR629668 Update trm_status in single sim mode also
28/04/14   dv       CR655739 ensure GFW does not sleep before receiving STOP CNF from G2X RATS
04/04/14   cah      CR643764 VSTMR multi-sim restruct and FEATURE_MSIM_VSTMR removal
24/01/14   cah      CR598552: enable sleep for DSDS GSTMR+VSTMR
23/01/14   cgc      CR606502 rework CR589523 add l1_sc_set_skip_g2x_pkt_txfr()
07/01/14   cgc      CR516981 propagation from 2.x, add l1_sc_wcdma_set_mode_from_init()
02/11/12   jj       CR402579 Support of Tx Diversity in GERAN.
01/10/13   sk       CR552259 CR sync from 1.0/1.1
06/09/13   sjw      CR467545 - Pass updated RXLEV to GRR in FCCH_SCH_DECODE_IND
02/09/13   nm       CR465537 propagation GL1/GRR Introduce MPH_RA_ABORT_REQ and MPH_RA_ABORT_CNF for DSDS
                    to resolve state transition in RR/L1.
08/05/13   cgc      CR453758 add FEATURE_CM_G2L search functionality FR-3009
24/06/13   cgc      CR504088 add L1_send_MPH_UE_MODE_CHANGE_CNF() support for SGLTE
15/07/13   sp       CR508123: Update panic reset to use multi-sim data space
21/06/13   cs       Major TSTS Triton Syncup
05/06/13   dv       CR496269 Ensure no G2X active in Idle before running l1_dual_sim_trm_manager
06/06/13   cs       TSTS Updates
29/04/13   cja      CR481603 Do not shutdown G2L on DEINIT conf if INIT conf already sent.
06/04/13   sv       CR462075: Corrected the logic when ignoring GRR commands in GTW PLMN cases.
04/03/13   ws       CR457871 - Crash in l1_acch_nv_flag_initialise() due to missing gas_id
19/11/12   cja      CR414098 W clock changes Dime
17/08/12   ip       CR384421 Disable GSM sleep during CCO activity
15/05/11   jj       CR357981 Abort aquicition earlier if panic reset triggered
28/03/12   pa       CR 346062 Workaround for G2T Abort.
10/02/12   ws       CR 336194 Enable TD-SCDMA IRAT for GERAN
06/12/11   cja      CR321466 In dedicated ID search send RR update if cell not found
17/11/11   cja      CR318115 Turn off MCPM W nbr during TCH/TRANS if no W
14/11/11   cja      CR318816 Call g2X shutdown in panic reset
09/09/11   cja      CR303806  Ensure G2X disable on suspend and inactive
03/02/11   ky       266163:Reverted CR261247 fix. Added three frame delay before deactivating the
                    FW during the panic reset procedure to allow fw to process all the commands
                    and ready for deactivation. Also removed sleep during the inactivation procedure
29/11/10   og       Adding G2L support.
24/03/10   ws       CR 230032 - Replaced RR message types with RR union in l1_ip_message_T
29/05/09   ps       Zero Initialise memory relocation to Heap for selected data structure
15/01/09   nt       CR167218 - changes to restore old rot and pdm values after HO fails
12/01/08   gj       CR165617 - mainline FEATURE_GSM_GPRS_ENHANCED_MEASUREMENT_REPORTING feature.
09/10/08   ap       CR 158238 - Complete L1 support to RR for Frequency Redefinition
                    Interface.
27/06/08   nt       Removal of redundant types mph_part_channel_release_req_T,
                    mph_alternate_bcch_monitor_req_T.
26/04/07   ws       CR 114617 - Added L1_send_MPH_FCCH_SCH_DECODE_IND for UOOS
22/11/06   og       Reverse the order of stopping SCE and setting the dedicated
                    stop main command. Resolves CR105367.
04/05/06   nt       CR66476 and CR92528 - Cell ID tick is now updated in transfer
                    mode using the id_tick that gets ticked on the IDLE frames of the
                    PDTCH. Function l1_sc_wcdma_tick_idle_id_in_tbf() is called to do this
                    The TBF suspend for ID is also scheduled off the idle ID Tick
                    to keep IDLE and TRANSFER mode cell ID's correctly spaced at 25secs.
05/04/06   nt       Added gl1_save_current_cell_data for CR90366 - Support for
                    STOP_GSM_MODE_REQ under DTM. Also added support for
                    L1_HOST_TEST message layer signalling
12/14/05   sv       Support the changed prototype for MPH_DECODE_BCCH_LIST_CNF.
11/07/05   sv       Add support for FEATURE_WTOG_BACKGROUND_PLMN_SEARCH.
20/09/05   nt       made function gl1_determine_l1_vocoder_control public under DTM
09/01/05   ws       Linted and code cleanup for FEATURE_GSM_DTM
08/12/05   ws       Added initial FEATURE_GSM_DTM support
06/20/05   ws       Chanded include definition from gMacSigL1.h to gmacsigl1.h
04/21/05   sv       Add support for L1 "Panic Reset"
03/15/05   dp       Added support for enhanced measurement reporting
02/11/04   bm       Support for Background WCDMA PLMN Scan
10/01/04   bm       Enhanced Parallel BCCH decode support
09/24/04   bm       Support for Split up the L1 deactivation into TASK and ISR
06/11/04   dp       Changed CCCH_UNI_DWN_REQ interface to take the BSIC instead
06/04/04   sv       Lint Cleanup.
04/28/04   gfr      Removed L1_ACTIVITY_MONITOR and enabled the ISR watchdog.
03/17/04   dlh      Added L1_send_MPH_START_IDLE_MODE_CNF proto.
02/25/04   gw       Lint clean up.
01/09/04   gw       Merged in support for new SI reading API from branch.
11/05/03   gfr      Support for quad-band.
07/11/03   gfr      Lint cleanup.
05/02/03   gw       Merged in power transfer changes from branch.
04/02/03   gw       Changes to reflect modifications in monscan module.
10/14/02   ATM      FEATURE_MULTIMODE_GSM is mainlined - removed flag per jchao
10/10/02   mk       Merged GPRS release 1 phase1 code to baseline.
08/13/02   dp       Interface changes for parallel acq/BCCH decodes
08/12/02   gw       Changed RR-L1 i/f for pscan as part of cell selection /
                    re-selection optimizations.
06/27/02   dp       Added prototype for L1_send_MPH_RELECTION_CNF
05/30/02   dp       Declared l1_isrtsk_blk as volatile to suppress compiler
                    optimizations
05/30/02   pjr/mk   Added PL1 GPRS code hooks under switch FEATURE_GSM_GPRS_L1.
                    Disabled L1 ISR activity monitor.
05/16/02   mk       Added L1 ISR activity monitor using T100 timer callback.
05/10/02   jc       Add support for FEATURE_MULTIMODE_GSM
02/20/02   ATM      Merge GS_GSM and GS functions together
02/01/02   JC/GW    Added changes to support AGC.
09/05/01   JC       Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Alphabetical Order */
#include "geran_variation.h"
#include "cmd.h"
#include "gl1_sys_algo.h"
#include "l1i.h"
#include "l2_l1.h"
#include "l2_l1_g.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "sys_type.h"

#include "gmacsigl1.h"
#include "gmacsigl1_g.h" /* definitions of messages from MAC */
#include "gpl1_gprs_if.h" /* l1_transfer_data_T */

#include "geran_dual_sim.h"

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
#include "wmgr.h"
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

#ifdef L1_HOST_TEST
#error code not present
#endif

/* Enable the ISR watchdog */


/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/


typedef union {
   /* include all RR->L1 messages from rr_l1.h */
   rr_l1_messages_u                  rr_l1_message;
   ph_data_ind_T                     ph_data_ind;
   ph_empty_frame_req_T              ph_empty_frame_req;
   ph_random_access_req_T            ph_random_access_req;
   /* mac signals */
   mac_l1_freq_config_t              mac_l1_freq_config;
   mac_l1_fixed_tbf_config_t         mac_l1_fixed_tbf_config;
   mac_l1_dummy_ctrl_block_t         mac_l1_dummy_ctrl_block;
   mac_ph_data_req_t                 mac_ph_data_req;
#ifdef FEATURE_GSM_DTM
   mac_l1_dtm_channel_assign_req_t   mac_l1_dtm_channel_assign_req;
#endif /*FEATURE_GSM_DTM*/
}l1_ip_message_T;

typedef struct {
  q_link_type      link;
  l1_ip_message_T  msg;
} l1_queue_message_T;

typedef enum{
  DONT_PANIC,
  RR_TRIGGERED_PANIC,
  L1_TRIGGERED_PANIC,
  L1_TRIGGERED_PANIC_AFTER_RR
} panic_reset_T;

/*
 * L1 task Globals
 */
extern boolean panic_reset_occured[] ;
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
/*This is the generic pointer that MUST be used within the task within the Dual Sim modules */
extern volatile ISRTIM_CMD_BLK   l1_tsk_buffer[];
#endif

extern  Frequency_List_T *l1_Frequency_List_Store[];
extern BCCH_List_T               l1_BCCH_List[];
extern l1_dcch_buffer_T          l1_dcch_s0_buffer[];
extern l1_sacch_buffer_T         l1_sacch_s0_buffer[];
extern const byte                rev_byte[16];
extern idle_data_T               l1_idle_data_store[];

/* IRAT status flags */
boolean l1_sc_get_wcdma_update_list_irat_active(gas_id_t gas_id);
void    l1_sc_set_wcdma_irat_active(boolean value, gas_id_t gas_id);
void    l1_sc_set_wcdma_mcpm_irat_active(boolean value, gas_id_t gas_id);
boolean l1_sc_check_irat_active(gas_id_t gas_id);

/* Only need 1 flag for L or T */
void    l1_sc_set_skip_g2x_pkt_txfr(boolean value, gas_id_t gas_id);
void    l1_sc_ts_changed(gas_id_t gas_id);

#ifdef  FEATURE_GSM_TO_LTE
boolean l1_sc_get_lte_irat_active(gas_id_t gas_id);
#endif

#ifdef FEATURE_GSM_TDS
  /* Make this a global flag so that we know TD cells are
   * present in ncell list. Assumption is that only G & T
   * are active
   */
boolean l1_sc_get_tds_irat_active(gas_id_t gas_id);
void    l1_sc_set_tds_irat_active(boolean value, gas_id_t gas_id);
void    l1_sc_set_tds_issue_cleanup(boolean value, gas_id_t gas_id);
boolean l1_sc_get_tds_issue_cleanup(gas_id_t gas_id);
void    l1_sc_set_tds_state(byte value,gas_id_t gas_id);
byte    l1_sc_get_tds_state(gas_id_t gas_id);
extern boolean l1_rcvd_non_drx_ind;
#endif

#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
extern void l1_set_wtog_bplmn_task_state(background_HPLMN_activity_type state, gas_id_t gas_id);
extern void l1_set_wtog_bplmn_task_substate(l1_wtog_bplmn_sub_state_T sub_state, gas_id_t gas_id);
extern background_HPLMN_activity_type l1_get_wtog_bplmn_task_state(gas_id_t gas_id);
extern l1_wtog_bplmn_sub_state_T l1_get_wtog_bplmn_task_substate(gas_id_t gas_id);
#endif

#ifdef DEBUG
extern char  debug_string[500];
#endif

extern transfer_data_T           l1_transfer_data[];
extern mac_ul_ctrl_buffer_T      gpl1_gprs_mac_ul_buffer[];

#ifdef FEATURE_GSM_DTM
extern dtm_data_T                gpl1_dtm_data[];
#endif /* FEATURE_GSM_DTM */
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
extern void l1_reset_fw_loaded_and_ready_flag(void);
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
extern boolean l1_get_background_wcdma_plmn_scan_active( gas_id_t gas_id );
extern void l1_set_background_wcdma_plmn_scan_active(boolean background_wcdma_plmn_scan_active, gas_id_t gas_id);
#endif

void l1_set_rach_ready_to_send( boolean ready_to_send, gas_id_t gas_id );

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
/*===========================================================================
FUNCTION l1_rach_timer_create

DESCRIPTION
  creates/defines the rach disable timer
===========================================================================*/
void l1_rach_timer_create( gas_id_t gas_id );
#endif /* ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM ) */
// For access to l1_state (in l1_task.c)
extern volatile ISRTIM_CMD_BLK l1_tsk_buffer[];
extern gs_queue_id_T l1_queues[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
void     L1_send_MPH_SERVING_DED_MEAS_IND(byte    Rxlev_full,
                                          byte    Rxlev_sub,
                                          byte    Rxlev_val,
                                          byte    Mean_BEP,
                                          byte    CV_BEP,
                                          byte    Num_Rcvd_Blocks,
                                          byte    Rxqual_full,
                                          byte    Rxqual_sub,
                                          boolean dtx_used, gas_id_t gas_id);
void     L1_handle_reset_mode(IMH_T *);
void     L1_handle_null_mode(IMH_T *);
void     L1_handle_find_bcch(IMH_T *);
void     L1_handle_idle_mode(IMH_T *, gas_id_t gas_id);
void     L1_handle_random_access_mode(IMH_T *);
void     L1_handle_sdcch_4(IMH_T *);
void     L1_handle_sdcch_8(IMH_T *);
void     L1_handle_dedicated_mode(IMH_T *);
void     L1_handle_tch(IMH_T *);

#ifdef FEATURE_IDLE_DRX_SCALING
void L1_handle_UPDATE_IDLE_DRX_IND(IMH_T *msg_header);
#endif /* FEATURE_IDLE_DRX_SCALING */

boolean  L1_select_next_ARFCN(word   *ARFCN_ptr, gas_id_t gas_id );
void     L1_send_ISRTIM_FIRST_FCB_REQ(int ARFCN_index );
void     L1_send_ISRTIM_SCH_REQ(int ARFCN_ptr );
void     L1_send_ISRTIM_ACQ_REQ(ARFCN_T ARFCN, gas_id_t gas_id );
void     L1_send_ISRTIM_ACQ_LIST_REQ(uint16 ARFCN_index, gas_id_t gas_id );
void     L1_send_ISRTIM_DECODE_BCCH_LIST_REQ(  gas_id_t gas_id );

void     L1_send_ISRTIM_SCAN_REQ( byte command, gas_id_t gas_id );
void     L1_send_ISRTIM_CCH_UNI_DWN_REQ( word           BSIC,
                                         ARFCN_T        ARFCN,
                                         uint32         ext_BCCH_mask,
                                         sys_algo_agc_T *agc_ptr, gas_id_t gas_id );

void     L1_start_idle_reporting_timer( void );
byte     L1_calculate_RA_first_delay( byte tx_integer );
word     L1_calculate_RA_delay(byte tx_integer, boolean BS_CCCH_SDCCH_COMB);

void     L1_send_MPH_RESET_IND( gas_id_t gas_id );
void     L1_send_MPH_RESET_CNF( gas_id_t gas_id );
void     L1_send_MPH_POWER_SCAN_CNF( boolean l1_suspending, gas_id_t gas_id );

void     L1_send_MPH_SELECT_SPECIFIC_BCCH_CNF(boolean found, byte *sch_block, gas_id_t gas_id);
void     L1_send_MPH_RESELECTION_CNF(boolean found, gas_id_t gas_id);

void     L1_send_MPH_DECODE_BCCH_LIST_CNF( boolean l1_suspending, gas_id_t gas_id );
void     L1_send_MPH_DECODE_BCCH_LIST_FAILURE_IND (ARFCN_T arfcn, rr_l1_decode_bcch_failure_T failure_cause, gas_id_t gas_id);
void     L1_send_MPH_FREQUENCY_REDEFINITION_IND( boolean channels_in_same_band, boolean starting_time_elapsed, gas_id_t gas_id );
void     L1_send_MPH_COEX_MSC_CHANGE_IND(uint8 ul_slot_restriction, uint8 dl_slot_restriction, gas_id_t gas_id);


void     L1_send_MPH_FCCH_SCH_DECODE_IND(ARFCN_T arfcn, byte bsic, byte rxlev, uint16 fcch_snr, gas_id_t gas_id);

void     L1_send_ISR_DEACT_CNF ( boolean deep_sleep, gas_id_t gas_id );


void     L1_send_MPH_CHANNEL_MODE_MODIFY_CNF(byte error, gas_id_t gas_id);
void     L1_send_MPH_HANDOVER_IND(byte error, gas_id_t gas_id);
void     L1_send_MPH_HANDOVER_CNF(dword ob_timing_diff, byte prev_chnl_tmng_adv, gas_id_t gas_id);
void     L1_send_MPH_STOP_HANDOVER_ACCESS_CNF(dword ob_timing_diff, byte  prev_chnl_tmng_adv, gas_id_t gas_id);

void     L1_send_MPH_IMMED_ASSIGN_CNF(byte error, gas_id_t gas_id);




/* Added for SGLTE support */

/*
** Bit fields for tracking active rats to allow MPH_UE_MODE_CHANGE_CNF to be sent.
*/

enum
{
    bMODE_NULL          = 0,
    bMODE_CNF_PENDING   = 1,
    bLTE_RAT_INACTIVE   = 2,
    bWCDMA_RAT_INACTIVE = 4,
    bTDS_RAT_INACTIVE   = 8,
    bMODE_FromInit      = 0x10   /* discriminate between  init() START_MODE_REQ and actual MPH_UE_MODE_CHANGE_REQ msg */

};

#if defined(FEATURE_GSM_TDS)
/*===========================================================================

FUNCTION l1_sc_reset_G2T_state

DESCRIPTION
  If TDSCDMA_IRAT_INIT_CMD fails need to be able to clear TDS irat state

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void     l1_sc_reset_G2T_state(gas_id_t gas_id );
#endif

void     l1_sc_wcdma_set_mode_from_init(boolean gsm_only, gas_id_t gas_id);

void     L1_send_MPH_UE_MODE_CHANGE_CNF (gas_id_t gas_id);

void     l1_sc_mode_cnf_pending(boolean SetPending , uint16 ratInactiveBit, gas_id_t gas_id);

boolean  l1_sc_g2x_shutdown_cnf_pending(gas_id_t gas_id);

void     L1_remove_oob_ba(inter_task_BCCH_allocation_T *baptr1,
                          inter_task_BCCH_allocation_T *baptr2 );

void     L1_handle_ENABLE_SLEEP_IND ( IMH_T  *msg_header );

boolean  L1_non_connected_state( gas_id_t gas_id );

extern void L1_handle_MPH_DSC_THRESHOLD_IND( IMH_T *msg_header );
extern void L1_enable_dual_receive(gas_id_t gas_id);

void     L1_handle_inactive_mode(IMH_T *);
void     L1_handle_MPH_START_GSM_MODE_REQ ( IMH_T  *msg_header );
void     L1_send_MPH_START_GSM_MODE_CNF ( gas_id_t gas_id );
void     L1isr_send_MPH_STOP_GSM_MODE_CNF ( gas_id_t gas_id );
void     gsm_l1_wait (rex_sigs_type mask, gas_id_t gas_id);

#ifdef FEATURE_GSM_DTM
void gl1_determine_l1_vocoder_control( gas_id_t gas_id );
void gpl1_gprs_handle_dtm_mode(IMH_T *, gas_id_t gas_id);
void gl1_save_current_cell_data(boolean store_afc_info, gas_id_t gas_id);
#endif

void     L1_send_MPH_START_IDLE_MODE_CNF(gas_id_t gas_id);
void     L1_send_MPH_ABORT_RA_CNF( gas_id_t gas_id ,gl1_mode_t l1_mode);


void l1_isr_watchdog_kick (gas_id_t gas_id);
void l1_isr_watchdog_activate (gas_id_t gas_id);
void l1_isr_watchdog_deactivate (gas_id_t gas_id);

#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
void L1_resume_req(gas_id_t gas_id);
void L1_setup_SUSPEND(gas_id_t gas_id);
void L1_handle_suspend_mode(IMH_T  *msg_header);
void L1_send_ISR_SUSPEND_CNF ( uint32 timer_value, gas_id_t gas_id );
#endif

#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
void L1_send_MPH_POWER_SCAN_FOR_BPLMN_CNF( boolean send_decode_bcch_list_req, gas_id_t gas_id );
void L1_send_MPH_START_GSM_MODE_FOR_BPLMN_CNF ( gas_id_t gas_id );
void L1_send_MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF ( gas_id_t gas_id );
void L1_send_MPH_SUSPEND_GSM_MODE_IND ( gas_id_t gas_id );
void L1_handle_WTOG_BPLMN_mode(IMH_T  *msg_header);
void L1_handle_MPH_START_GSM_MODE_FOR_BPLMN_REQ ( IMH_T  *msg_header );
void L1_handle_MPH_RESUME_GSM_MODE_FOR_BPLMN_REQ ( IMH_T  *msg_header );
#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
void L1_handle_MPH_NV_REFRESH_REQ(IMH_T *msg_header);
void L1_send_MPH_NV_REFRESH_CNF(gas_id_t gas_id);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
void L1_send_MPH_L1_DS_ABORT_IND(gas_id_t gas_id, rr_ds_abort_type_T abort_cause, uint32 timeout);
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

void l1_set_grm_status( gl1_trm_state_t trm_status, gas_id_t gas_id );

void L1_send_MPH_SURROUND_IRAT_MEAS_IND( rr_l1_irat_rat_e rat, const rr_l1_irat_measurements_u*  measurements_ptr, gas_id_t gas_id);
void L1_send_not_found_MPH_SURROUND_DED_ID_MEAS_IND( rr_l1_irat_rat_e rat, uint32 uearfcn, gas_id_t gas_id );
void l1_do_panic_reset_after_delay(gas_id_t gas_id);
boolean l1_check_for_panic_reset(gas_id_t gas_id);
uint32 l1_get_panic_reset_received_frame_no(gas_id_t gas_id);
void l1_set_panic_reset_received_frame_no(gas_id_t gas_id, uint32 frame_no);
void l1_reset_panic_reset_received_frame_no(gas_id_t gas_id);

/* shared functions between l1_sc_wcdma.c and sc_trans_mease.c*/
boolean l1_sc_wcdma_tick_idle_id_in_tbf( uint16, gas_id_t );

/* This function is used to allow clients to reset the
 * current_rr_l1_msg_id variable.
 */
extern void  l1_reset_rr_current_msg_id( gas_id_t gas_id );

extern void l1_sc_g2x_pending_shutdown(gas_id_t gas_id);

extern void l1_sc_mcpm_w_meas_stop( gas_id_t gas_id );
extern void init_l1_utils_data( gas_id_t gas_id );
extern void init_gpl1_gprs_ptcch_data( gas_id_t gas_id );
extern void init_local_idle_data( gas_id_t gas_id );
extern void init_l1_bcch_list_data( gas_id_t gas_id );
extern void init_l1_acq_data( gas_id_t gas_id );
extern void init_gpl1_gprs_serving_cell_data( gas_id_t gas_id );
extern void init_gpl1_gprs_single_data( gas_id_t gas_id );
extern void init_l1_isr_data( gas_id_t gas_id );
extern void init_l1_multi_sim( gas_id_t gas_id );
extern void init_sc_wcdma_data( gas_id_t gas_id );

extern void L1_sdcch_init_data_struct( gas_id_t gas_id );
extern void L1_tch_init_data_struct( gas_id_t gas_id );
extern void l1_tch_amr_init_data( gas_id_t gas_id );
extern rex_tcb_type* l1_task_tcb_read( gas_id_t gas_id );

        void    l1_acch_nv_flag_initialise      ( gas_id_t gas_id );
#define L1_GSM_ACCH_NV_FLAG_DISABLED            0x00
#define L1_GSM_ACCH_NV_FLAG_ENABLED             0x01
#define L1_GSM_ACCH_NV_FLAG_R_FACCH_ONLY        0x02
#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
void start_tx_div_antenna_alg ( boolean packet_trans ,gas_id_t gas_id);
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */
void antenna_swth_get_nv_item (void);

#if defined (FEATURE_GSM_TO_LTE) || defined (FEATURE_GSM_TDS)
/*
** exposed for dedicated suspend 
*/
void   gl1_handle_message_router_msgs( gas_id_t gas_id );
#endif

#if  defined(FEATURE_QBTA)
void gl1_read_nv_qbta_support (void);
boolean gl1_get_qbta_nv_support(void);
#endif

#endif /* L1_TASK_H */
