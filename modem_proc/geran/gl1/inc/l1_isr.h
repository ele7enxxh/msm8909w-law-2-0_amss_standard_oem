#ifndef L1_ISR_H
#define L1_ISR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L1   I S R   P R O C E S S I N G
                       H E A D E R   F I L E

DESCRIPTION
   Layer 1 ISR Context Processing


Copyright (c) 2001-2015 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_isr.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ----------------------------------------------------------
24/11/15   mko      CR924073: FR23269 M2M, Allow sc_start and block the Ncell measurements from sci_go; Clear UPDATE_IDLE_DRX_IND data during the reselection
05/11/15   df       CR923258 Avoid SCE crash while aborting idle and receiving MPH_UPDATE_IDLE_DRX_IND  
09/10/15   akh      CR912421 FN/QS not correct for active ttg, sleep ttg resp delayed by 1s
09/10/15   df       CR921075 FR23269 M2M devices: Handle RR->GL1 MPH_UPDATE_IDLE_DRX_IND
05/06/15   cs       CR808438 Ensure any pending TT requests are sent before cell
                    information is updated
23/02/15   br       CR794803:UE not moving to a new  CS channel after recieving MAC_L1_DTM_CHANNEL_ASSIGN_REQ  which has cs reconfiguration.
10/02/15   am       CR793750 Correcting compilation warning
30/01/15   am       CR785921 Do TRM reserve & request for bcch readings in cell reselection state
06/01/14   am       CR390109 Improved DSDS cell reselection
17/12/14   am       CR770270 Ensure CBCH is not scheduled if it is being from frame manager
12/09/14   nm       CR723570 Port GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
21/07/14   am       CR690871 Restart rach gaurd timer on reception on assignment
29/07/14   ap       CR695234: Changes for GL1's New Rach interface
07/01/14   dg       CR593574 GL1 to send Block Quality Ind to RR for the paging
                    blocks which belong to its group in RACH mode
29/11/13   js       CR574402 : COEX : VBATT : Accurate prediction of N+1 frame Tx power for CS (TCH and SDCCH)
27/11/13   cja/npt  CR555678 Speed up clock during power scan for RF processing load
04/11/13   mc       CR556742 G+G COEX : Enhancements for monitor positioning and improved SDCCH/FACCH conflict detection
14/10/13   cja      CR555678 Speed up clock during power scan for RF processing load
15/08/13   sk       CR524039 GBTA bringup changes
01/08/13   sk       CR521009 Ensure the global flag rr_send_stop_gsm_mode_req is data spaced
13/03/13   mc       CR 459487 : COEX Support - GERAN arbitrator framework
01/03/12   ws       CR 458470 - Fix referencing of tx_lev resolves erratic txlev
                    power in dedicated mode
08/11/12   ab       CR 418358 Increase RACH GUARD Timer based on starttime
19/06/12   jj       CR367081 Ignoring RR messages once received MPH_STOP_GSM_MODE_REQ
                    until next state change
13/07/10   cs       Updates for CB_DRX support
05/01/10   cs       CR 218984 GCF 13.3.4.1 failure on high bands TX lev wrongly
                    reported
28/08/08   cs       Implement the Fast Rx Sweep functionality for FTM
22/04/08   ap       CR#144832 Ensure L1 drops back to IDLE when the single block
                    start time elapses for 52 multiframe.
21/11/07   og       Ensure L1 drops back to IDLE when the single block start
                    time elapses. Resolves CR115529.
14/08/06   ws       Allow dedicated to be timeslot slammed to lowest received tn
                    and also to place the CS timeslot on TN 2
08/24/05   ws       Changed prototype for MPH_DTM_CHANNEL_RELEASE_IND to
                    include cs_released and ps_released flags
08/12/05   ws       Added initial FEATURE_GSM_DTM support
03/11/05   sv       Mainlined FEATURE_GSM_CB
02/11/04   bm       Support for Background WCDMA PLMN Scan
08/26/04   sv       Lint cleanup
08/09/04   gsc      Added function prototypes for sending CB_MISSING_BLOCK_IND
                    and CB_SKIP_CNF to CB task.
05/12/04   gw       Added prototype for L1_get_bcch_list().
03/18/04   gsc      Lint clean up (Dedicated Mode).
02/25/04   gw       Lint clean up.
11/05/03   gfr      Support for quad-band.
07/29/03   gw       Removed #define of DEBUG_L1_STATE_LOG.
07/16/03   bk       defined DEBUG_L1_STATE_LOG. This is used to capture information
                    when stuck in Idle abort
07/11/03   gfr      Lint cleanup.
06/23/03   gfr      Compiler warning cleanup.
29/05/03   kf       ncell pbcch read changse
05/12/03   ATM      Moved Frame Number management entirely inside GTMRS
05/07/03   gsc      Added GSM AMR support
02/25/03   dp       Moved L1_control_half_rate prototype to this file.
11/15/02   mk       Updated GPRS baseline with release 2.0 functionality.
11/14/02   ws       Modified prototype change to L1_IDLE_MODE
10/16/02   dp       Changed L1_control_full_rate interface to pass in freq
                    redef flag
10/01/02   ATM      Moved central get and set FN routines to l1_utils
09/03/02   dp       Cleaned up first_cch_block
08/13/02   dp       Added prototypes for parallel decode cnf functions
06/27/02   dp       Added prototype for L1_get_sch
05/30/02   pjr/mk   Added PL1 GPRS code hooks under switch FEATURE_GSM_GPRS_L1.
05/30/02   dp       Declared l1_tskisr_blk as volatile to suppress compiler
                    optimizations
04/30/02   gw       Added prototype for function L1_send_sysinfo_PH_DATA_IND.
03/20/02   dp       Added functions to support power control in steps of 2dB
02/20/02   dp       Removed handover_in_progress from L1_control_SDCCH4/8
                    interfaces
02/12/02   thh      Removed l1_determine_rach() function prototype.  This
                    function is no longer externalized, especially here.
                    Updated copyright date.
02/07/02   gw       Removed unused SCE variables.
12/07/01   mk       Changed prototype for L1_control_full_rate().
09/27/01   gw       Removed unused protos and variables.
09/25/01   mk       General SCE related cleanup.
08/31/01   JC       Changed rach_ARFCN type and general cleanup.
08/17/01   mk/gw    Added Ncell support, featurized via MONITOR_DEBUG.
05/16/01   JC       Initial revision. Changed type of txcch_ARFCN.

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

#include "l1i.h"     /* ISRTIM_CMD_BLK */
#include "gtmrs.h"
#include "gtmrs_g.h"   /* Frame Number Management */
#include "cbl1if.h"
#include "geran_eng_mode_info.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
/* Constants for synthesiser control */
#define RX  0
#define TX  1
#define MON 2

/* Types for cch_read  */

#define L1_CCH_NO_OF_INSTANCES 2

typedef byte cch_command_T;
enum
{
   L1_CCH_INIT,
   L1_CCH_START,
   L1_CCH_RUN
};

typedef byte cch_state_T;

enum
{
   L1_CCH_NULL,
   L1_CCH_BEGIN,
   L1_CCH_1,
   L1_CCH_2,
   L1_CCH_3,
   L1_CCH_4,
   L1_CCH_WAIT,
   L1_CCH_DECODED,
   L1_CCH_STOP,
   L1_CCH_NCELL_DELAY
};

typedef byte l1_mode_command_T;

enum
{
   L1_MODE_RUN,
   L1_MODE_STOP
};


/* Globals  */
/* The globals are defined in m10505 */

extern const byte               ded_sdcch_8_table [102] [2 ] [2];
extern const byte               ded_sdcch_4_table [102] [4 ] [2];
extern boolean                  isr_initialised;
extern boolean                  dtm_assign_cs_present[NUM_GERAN_DATA_SPACES];

extern l1_serving_cell_meas_T   l1_serving_cell_meas[];

extern byte l1_get_current_timeslot( gas_id_t gas_id);
extern void l1_set_current_timeslot(byte current_timeslot,gas_id_t gas_id );

/* Globals for the Routine L1_read_CCH (m10510.c) */
extern  byte    cch_TSC_1;
extern  byte    cch_TSC_2;
extern  idle_data_T *idle_data_ptr_1;
extern  idle_data_T *idle_data_ptr_2;

extern boolean l1_get_cch_ciphering_flag( gas_id_t gas_id );
extern void l1_set_cch_ciphering_flag(boolean cch_ciphering_flag ,gas_id_t gas_id);
extern  boolean cch_ciphering_flag;
extern  byte  no_of_messages;

/* Globals for L1_access_isr */

extern void l1_set_rach_read_all_AGCH(boolean rach_read_all_AGCH,gas_id_t gas_id);

extern boolean rr_send_stop_gsm_mode_req[] ;


extern const byte        fill_frame[N201_MAX];


#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
extern uint32 l1_get_ind_timer_value(gas_id_t gas_id);
extern uint32 l1_get_ind_timer_FN(gas_id_t gas_id);
#endif
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
extern void l1_rach_timer_start( gas_id_t gas_id );
extern void l1_rach_timer_deactivate( gas_id_t gas_id );
#endif /* ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM ) */
/* Keep track of how long we are in random access mode in case RR has abandoned us */
#define L1_MAX_RANDOM_ACCESS_MODE_FRAMES 6500  /* 30 seconds in RA mode */
/* Additional guard time if (start time more than 30 sec */
#define L1_RACH_ADDITIONAL_GUARD_MARGIN  1300 /* 6 secs */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef FEATURE_IDLE_DRX_SCALING
boolean l1_check_RR_idle_mode_meas_enabled(gas_id_t gas_id);
void l1_set_L1_ready_for_sc_start(l1_sc_mode_T mode, gas_id_t gas_id);
void l1_clear_L1_ready_for_sc_start(gas_id_t gas_id);
void l1_set_L1_ready_for_go(boolean L1_ready_for_sc_go, gas_id_t gas_id);
void l1_clear_drx_ind_data( gas_id_t gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
extern uint8 gl1_get_num_idle_subs( void );
void    l1_tdma_1_ISR( sys_modem_as_id_e_type as_id);
boolean L1_get_bcch( uint32 *frames_until_active_ptr, gas_id_t gas_id );
uint32 L1_gap_to_next_bcch( uint32 bcch_mask, uint32 ext_bcch_mask, gas_id_t gas_id );
boolean L1_get_bcch_list( void );
void    L1_get_sch ( gas_id_t gas_id );
boolean L1_idle_mode_ISR( boolean leaving_idle_mode, gas_id_t gas_id );
boolean L1_control_paging_mode(boolean      own_paging_block,
                               byte         *message,
                               idle_data_T  *idle_data_ptr, gas_id_t gas_id );
boolean L1_control_full_rate ( dedicated_data_T       *ded_dtr_ptr,
                               channel_information_T  *chnl1_info_ptr,
                               boolean                 apply_freq_redef, gas_id_t gas_id);
void    L1_process_EQU_data( byte channel_type, byte  meas_type);
boolean L1_access_ISR(l1_mode_command_T  mode, gas_id_t gas_id);

void         L1_control_random_access(idle_data_T *data);
extern void  l1_access_isr_set_mac_sb_failure( gas_id_t gas_id );

boolean L1_send_random_burst(void);
boolean L1_determine_time(word Mod42432, word starting_time);
boolean L1_control_dedicated(gas_id_t gas_id);
boolean L1_determine_start_time ( void );
void    L1_change_channel(  dedicated_data_T       *ded_dtr_ptr,
                            channel_information_T  *chnl1_info_ptr, boolean dtm_assignment, gas_id_t gas_id);
void    L1_change_timeslot( byte new_ts, gas_id_t gas_id );

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER
byte gpl1_read_page_mode(byte *message,gas_id_t gas_id);
#endif /* FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

extern void gpl1_stop_sc_tick_trans( gas_id_t gas_id );
extern void gpl1_start_sc_tick_trans( gas_id_t gas_id );

#ifdef FEATURE_GSM_DTM
void    L1_dtm_change_timeslot( byte new_ts, channel_information_T  *chnl1_info_ptr, gas_id_t gas_id );
#endif /* End of FEATURE_GSM_DTM */
boolean L1_control_SDCCH_4( dedicated_data_T      *ded_data,
                            channel_information_T *chnl_1_data, gas_id_t gas_id
                          );
boolean L1_control_SDCCH_8( dedicated_data_T      *ded_data,
                            channel_information_T *chnl_1_data, gas_id_t gas_id
                           );
void    L1_send_dedicated_report( gas_id_t gas_id);
void    L1_power_scan( gas_id_t gas_id );
void    l1_pscan_mcpm_change( boolean pscan_start, gas_id_t gas_id );

word    L1_set_meas_gain( word  *rxlev, byte *no_of_meas, byte   slot_type);

void    L1_process_signal_measurement(uint16 *rxlev_sum,  /* power level */
                                      int32 sys_gain,      /* system gain */
                                      uint8 *no_of_measurements,
                                      uint8 *no_of_attempts);

int32   L1_convert_eq_pwr(int32 pwr_meas);
int32   L1_get_next_scan_freq ( uint8 command, Frequency_List_T  *frq_list_ptr);

void    L1_first_fcb_req( void );
boolean L1_set_AFC_FCB( void);
word    L1_calc_first_timer_corr(word latch_offset, byte hysterisis );
void    L1_set_rx_gain(word total_of_meas_uptonow, byte no_of_meas_uptonow);
void    L1_set_surrounding_rx_gain(byte expected_power);
void    L1_sch_req ( void );

boolean L1_initial_acq( gas_id_t gas_id );

word    L1_calc_timer_corr (word latch_offset, byte hysterisis );
void    L1_send_FCSE_to_DSP(void);
void    L1_process_eq_power(word  power,
                                byte  meas_type);
boolean L1_get_CCH_message( boolean    use_filler_message,
                            l2_channel_type_T channel_type,
                            dedicated_data_T  *ded_data,
                            byte              *op_buffer_ptr,
                            uint16            *data_length,
                            gas_id_t           gas_id);

void L1_free_CCH_message(l2_channel_type_T channel_type, dedicated_data_T  *ded_data, gas_id_t gas_id);
void L1_get_CCH_filler_message( l2_channel_type_T channel_type,
                                byte              *op_buffer_ptr,
                                uint16            *num_octets_copied_ptr );
boolean L1_CCH_message_pending( l2_channel_type_T channel_type, gas_id_t gas_id );

void    L1_set_synthesiser ( uint16 channel_no, uint8 timeslot_type );
void    L1_initialise_isr( gas_id_t gas_id);
void    L1_IdleMinusTwoAct ( void );
void    L1_IdleMinusOneAct ( void );
void    L1_IdleAct (void);
void    L1_IdleSCHAct (channel_information_T *chnl_dta );
void    L1_scan_init(void);
void    L1_set_tx (int  ta);
void    L1_set_rach_tx (void);
void    L1_send_PH_DATA_IND(boolean  good_data,
                            uint16   channel_type,
                            boolean  own_paging_block,
                            ARFCN_T  ARFCN,
                            byte    *data_ptr,
                            byte     data_length, gas_id_t gas_id);

void L1_send_sysinfo_PH_DATA_IND( boolean  good_data,
                                  uint16   channel_type,
                                  boolean  own_paging_block,
                                  ARFCN_T  ARFCN,
                                  uint8    TC,
                                  byte    *data_ptr,
                                  byte     data_length, gas_id_t gas_id);

void    L1_send_PH_RANDOM_ACCESS_CNF(uint16 ra_content, word sent_FNmod42432, gas_id_t gas_id);

void    L1_send_ISR_SCH_CNF (boolean  success, byte *sch_dta_ptr, gas_id_t gas_id);
void    L1_send_ISR_DECODE_BCCH_LIST_SCH_CNF(boolean  success,
                                             uint16   qbit_lag,
                                             uint32   fn_lag,
                                             uint8    bsic, gas_id_t gas_id );

void    L1_send_ISR_FIRST_FCB_CNF (boolean  success, gas_id_t gas_id);
void    L1_send_ISR_SCAN_CNF (gas_id_t gas_id);
void    L1_set_AFC ( gas_id_t gas_id ) ;
boolean L1_control_half_rate( dedicated_data_T       *ded_dtr_ptr,
                              channel_information_T  *chnl1_info_ptr,
                              boolean                 apply_freq_redef, gas_id_t gas_id);
void    L1_terminate_dedicated( void);
void    L1_transfer_new_data ( gas_id_t gas_id );
void    L1_send_MPH_BLOCK_QUALITY_IND(byte   chnl_type,
                                        gl1_block_quality_t  good_frame, gas_id_t gas_id);
void    L1_correct_timing ( void );
void    L1isr_send_PH_READY_TO_SEND_IND (byte    channel_type, gas_id_t gas_id);
void    L1isr_send_MPH_IMMED_ASSIGN_CNF (byte    error, gas_id_t gas_id);
void    L1isr_send_MPH_CHANNEL_RELEASE_CNF (byte    error, gas_id_t gas_id);
void    L1isr_send_MPH_SET_CIPHERING_CNF (byte    error, gas_id_t gas_id);
void    L1isr_send_PH_CONNECT_IND(channel_type_T chnl_type, gas_id_t gas_id);
void    L1isr_send_MPH_CHANNEL_ASSIGN_CONF(byte    error_code, gas_id_t gas_id);
#ifdef FEATURE_GSM_DTM
void L1isr_send_MPH_DTM_CHANNEL_RELEASE_IND (byte error, boolean ps_release, boolean cs_release, gas_id_t gas_id);
void L1isr_send_MPH_DTM_CHANNEL_ASSIGN_CNF (gas_id_t gas_id);
#endif /*FEATURE_GSM_DTM*/
void    L1_notify_start (channel_type_T     physical_channel_type,
                         l2_channel_type_T  logical_channel_type,
                         byte               message_type,
                         byte               error_code, gas_id_t gas_id );

void    L1_store_ta_txlev ( dedicated_data_T *ded_dtr_ptr, byte new_ta, byte new_txlev,
                            gas_id_t gas_id);

void    L1_precompute_txlev( int16 ordered_txlev, int16 current_txlev,
                             byte rep_period_length, ARFCN_T arfcn,
                             gas_id_t gas_id);

void    L1_retrieve_txlev ( uint8 block_start, uint8 burst_spacing, int16 block_tx_power_index[],gas_id_t gas_id);
void    L1_map_txlev_to_power_index( int16 tx_lev[], int16 power_index[], uint8 num_values );
byte    L1_change_ta( byte demanded_ta, byte ta_in_use,gas_id_t gas_id );
void    L1_set_reporting_period(uint16 reporting_period_end_minus_1, uint16 reporting_period, gas_id_t gas_id);
int16   L1_get_txlev_for_FN(uint32 fn, gas_id_t gas_id);

void    L1isr_send_L1_REP_PERIOD_IND(gas_id_t gas_id);
void    L1_handle_nt_downlink ( dedicated_data_T       *ded_dtr_ptr );
void    L1_handle_nt_uplink ( dedicated_data_T       *ded_dtr_ptr );
dword   L1_change_FN ( dword FN_lag, gas_id_t gas_id );
void    L1isr_send_MPH_RECONNECT_CHANNEL_CNF(gas_id_t gas_id);
boolean L1_more_abs_to_send(dedicated_data_T *ded_d_ptr, byte  DCCH_type, gas_id_t gas_id);
void    L1_send_ISR_HANDOVER_CNF (gas_id_t gas_id);
boolean L1_send_access_burst(void);
uint8   L1isr_get_jump_to_sdcch(gas_id_t gas_id);
void L1_restart_frames_in_random_access_mode( gas_id_t gas_id );
boolean L1_background_HPLMN_search_in_progress( gas_id_t gas_id );

#ifdef FEATURE_GSM_AMR
boolean  L1_control_amr_fr ( dedicated_data_T       *ded_dtr_ptr,
           channel_information_T  *chnl1_info_ptr,
           boolean                 apply_freq_redef, gas_id_t gas_id );


boolean  L1_control_amr_hr ( dedicated_data_T       *ded_dtr_ptr,
           channel_information_T  *chnl1_info_ptr,
           boolean                apply_freq_redef, gas_id_t gas_id );

#endif /* FEATURE_GSM_AMR */


void L1_send_CB_MISSING_BLOCK_IND( uint32 fn, gas_id_t gas_id );

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
extern void L1_send_CB_SCHED_EXPIRY_IND( cb_channel_ind_type chan, gas_id_t gas_id );

extern void L1_send_CB_SCHEDULING_CNF( uint32 *fn_list, gas_id_t gas_id );
#endif /*FEATURE_GSM_CB_DRX_SUPPORTED */

extern boolean L1_get_bcch_list_fcch_sch_bcch( gas_id_t gas_id );
extern void L1_bcch_list_acq_bcch_resume (  gas_id_t gas_id );
extern void L1_bcch_list_acq_bcch_suspend ( void* dummy, gas_id_t gas_id );
boolean is_ccch_rx_block( idle_data_T       *idle_data_ptr,
                                 uint8             *own_paging_block,
                                 gl1_msg_chan_type *chan_type,
                                 gas_id_t gas_id );

#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
extern boolean l1_get_gsm_stop_req_in_queue( gas_id_t gas_id );
extern void l1_set_gsm_stop_req_in_queue(boolean gsm_stop_req_in_queue, gas_id_t gas_id);
#endif /* FEATURE_WTOG_BACKGROUND_PLMN_SEARCH */

extern void gl1_wait_for_dpc( gas_id_t gas_id );

void L1_send_CB_SKIP_CNF( uint32 *fn_list, gas_id_t gas_id );

void l1_clear_cbch_scheduling( gas_id_t gas_id );

/*===========================================================================

FUNCTION  l1_get_tch_chan_mode

DESCRIPTION
  This function returns tch data channel mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
eng_mode_chan_type_t l1_get_tch_chan_mode(gas_id_t gas_id);


/*===========================================================================

FUNCTION  l1_indicate_g2w_ho

DESCRIPTION
  This function sets the g2w indicator flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void l1_indicate_g2w_ho( boolean g2w_flag, gas_id_t gas_id );

/*===========================================================================

FUNCTION  l1_check_for_g2w_ho

DESCRIPTION
  This function reads g2w indicator flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean l1_check_for_g2w_ho(gas_id_t gas_id);

#ifndef FEATURE_GL1_GPLT
/*===========================================================================

FUNCTION  l1_isr_run_gps_tick_handler

DESCRIPTION
  Run gps tick handler if FN/QS can be read properly

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void l1_isr_run_gps_tick_handler( gas_id_t gas_id );
#endif /* FEATURE_GL1_GPLT */

#endif /* L1_ISR_H */
