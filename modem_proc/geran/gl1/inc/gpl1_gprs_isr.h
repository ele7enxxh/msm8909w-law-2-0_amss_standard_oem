#ifndef GPL1_GPRS_ISR_H
#define GPL1_GPRS_ISR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L1  G P R S  I S R   P R O C E S S I N G
                       H E A D E R   F I L E

DESCRIPTION
   GPRS Layer 1 ISR Context Processing


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gpl1_gprs_isr.h#1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
23/05/16   db       CR921073 Fix a limitation with PCH in transfer
03/10/14   aga      CR732455 Changes required to tune away framework for TH 1.0 single sim build 
12/09/14   nm       CR723570 GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
13/08/14   ggu      CR687349:DSDS: GL1 should guarentee at least one frame eariler before start check TBF suspension for G2W TA case
23/07/14   pjr      CR682764 TP IR Feature - implement IR mem Full handling from GFW to GRLC
29/07/14   ap       CR695234: Changes for GL1's New Rach interface
20/06/14   pjr      DATA+MMS feature - USF utilisation calculation
30/05/14   ws       CR657674 Data + MMS Phase 1
03/04/14   ab       CR 639723 : Notify PS - to register ARFCN/Freq Id when starttime has elapsed when TRM lock is not acquired Single Shot
06/03/14   br       CR626780, RACE condition between TRANSFER G sub and IDLE G sub, issuing SCH in G2x gap.
15/08/13   sk       CR524039 GBTA bringup changes
29/04/13   ws       CR481359 enable GTA support
15/02/13   ws       CR 453155 - Correct missing gas id in gprs_mac_l1_acc_func()
26/03/12   jj       CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33
18/06/10   nt       CR:243318 ensure that SCE isn't stuck upon TBF suspend abort
14/06/10   ab       CR: 235571 - 3GPP: Radio blocks for channel quality reporting
02/12/09   ws       Added FEATURE_GSM_GPRS_AGC_UPDATE for GPRS/EGPRS AGC
                    Improvements
19/05/09   nt       CR178773: Added check when scheduling scell bcch that it occurs
                    after TBF resumption, if TBF is currently suspended.
28/01/09   og       Do not apply the Enhanced Ext TBF criterea in single block
                    TBF. Resolves CR170320.
24/11/08   og       FEATURE_GSM_GPRS_MSC34 GPL1 changes.
20/08/08   og       CR155903 Open SRB loopback as late as possible - TC21.8 / 21.9 BER fix.
02/06/07   cs       Fix compile issue if EDTM not defined
26/01/07   og       Implement UL ctrl msg deletion in L1.
01/12/06   og       Rewind back the fn counter by one when dropping from DTM mode
                    to transfer mode and cs ts < TS2.
08/11/06   nt       CR94409:- Added Prach protection against cell going bad when waiting to
                    send the initial PRACH.
25/08/06   og       EDTM addition of the mac_ph_cs_connect_ind message and
                    typedef.
31/07/06   av       CR85810 NB AB TSC, and TA fix.
23/11/05   og       Update tx call back routines to provide more information.
15/11/05   og       Fix to USF granularity handling, resolves CR 82339, 82341.
04/11/05   og       Adjustements to start time checks, resolves CR 79207.
28/10/05   nt       Added gpl1_dtm_complete_abort_procedure function prototype
16/09/05   og       Send a sysinfo read failure when transitioning from transfer
                    back to idle.
26/08/05   og       Removal of DEBUG_TRANSFER compiler switch and associated
                    code.
25/08/05   og       Tick the drx timers independent of L1 mode.
19/08/05   og       Removal of downlink block FIFO.
12/08/05   ws       Updated for initial DTM release
08/08/05   og       Restructuring PL1 before addition of DTM.
04/08/05   og       Merge change 225343 from the raven branch.
23/06/05   og       Code changes to accomodate the uplink tbf data block count
                    log packet.
02/06/05   og       Removal of header-header includes.
27/04/05   og       Allow PBCCH reading in pkt transfer on timeslots lower than
                    the camp timeslot.
06/04/05   og       Fix the implementation/use of the prach tx callback function.
17/3/05    og       Updates to srb loopback handling following integration
                    on raven target.
7/03/05    og       Allow BCCH read attempts during packet transfer even when
                    TBF is suspended.
13/1/05    og       MS timebase adjustements confined to radio block gap.
06/12/04   ws       Updates for TEST mode B for GPRS
13/12/04   og       Removal of single block flag from gprs change timeslot function.
2/11/04    og       Addition of egprs srb test mode capability.
28/10/04   ws        Lint tidy up
25/10/04   og       Re-arrangement of private/public functions.
18/10/04   dlh      Added gpl1_gprs_setup_RESELECTION protos.
13/10/04   kf       Changes for time slot slamming in transfer.
12/10/04   ws       Added prototype for gpl1_gprs_reset_pacch_fifo()
26/08/04   kf       Added gpl1_gprs_change_timeslot.
30/07/04   ws       Added burst_num to pdtch metrics callback prototype
23/06/04   og       Egprs Qual measurements handling.
05/05/04   kf       Changes for ts slamming.
13.05/04   rm       Removed ta_meas_in_prog_T declaration.
28/04/04   rm       Moved PTCCH and RRBP parameter declarations to their modules
20/04/04   dlh      Lint clean.
19/03/04   ws       Added support for bad snr detection on PTCCH/D decodes
18/03/04   dlh      Added support for nc non drx timer
13/02/04   ws       Added prototype for gpl1_gprs_calc_ptcch_tn_clash()
27/01/04   kf       added ul_power_band to gprs_pl1_serving_cell_meas_T
23/01/04   kf       added prev_highest_tn to gprs_pl1_serving_cell_meas_T.
26/11/03   ws       Lint cleanup
06/11/03   gfr      Support for quad-band.
01/10/03   ws       Added gl1_msg_pdch.h for gl1_msg_retrieve_ta()
26/09/03   kf       Fix for power control on pdch
25/09/03   dlh      52 sleep mode additions.
09/09/03   dlh      Removed pl1_52_idle_wait as no longer needed.
29/08/03   dlh      Removed and/or modified packet paging variables.
12/08/03   ws       Moved various prototypes from gpl1_gprs_dynamic.c
23/07/03   dlh      Added rrbp_52_idle flag.
23/07/03   ws       Added prototype for gpl1_gprs_send_acq_psi_rej()
21/07/03   dlh      Added flags for sleep mode/ extended paging etc.
09/07/03   pjr      Modified rla_p average values.
08/07/03   pjr      Modified value of GPRS_SERV_CELL_IDLE_52_AVG_LEN
19/06/03   pjr      Modified gprs_serving_cell_meas_T for using running average.
16/06/03   pjr      Added UL_DL_PTCCH to ta_meas_in_prog enum.
30/05/03   ag       Added gpl1_gprs_read_serving_psi prototype.
15/05/03   pjr      Added rla_p_count_idle to gprs_pl1_serving_cell_meas_T to allow
                    the timing of idle mode measurement reports sent to GRR to vary.
14/05/03   pjr      Added uint8 quality_report_count to gprs_pl1_serving_cell_meas_T
                    Increased size of MAX_NUM_QUAL_RPRTS to 24.
14/05/03   dlh      Added various extern DRX sleep flag and variables.
14/05/03   dlh      Added extern flag for handling the change of TN on the fly.
05/12/03   dp       Changes for BSIC transfer with timeslot reconfiguration
02/05/03   ws       Updates for pbcch handler
30/04/03   pjr      Added reconf_in_prog parameter to gprs_pl1_serving_cell_meas_T
14/03/03   pjr      Modified RLA_P average count to account for different averages
                    in IDLE and TRANSFER
06/03/03   kf       changed gpl1_gprs_idle_mode_ISR from void to boolean
05/04/03   DLH      Added gprs_leaving_52_idle_mode param to 52_idle proto.
04/03/03   pjr      Added abort_prach_once to prach_params_T
28/02/03   pjr      Added rrbp_pending and abort_fn to prach_params_T
                    Added mode parameter to L1_packet_access_ISR
27/02/03   DLH      Added gpl1_gprs_get_tc func proto.
27/02/03   pjr      Added define GPRS_SERV_CELL_RLA_P_COUNT
14/02/03   DLH      Added prototype for gprs_copy_paging_data.
14/02/03   DLH      Remove gpl1_gprs_store_psi1_repeat_period
14/02/03   pjr      Modified serving cell measurement structure.
13/02/93   ws       Added prototype for configuring AGC
05/02/03   ws       Added TEMP_DELAY_OS and changed NPL1_FN_OFFSET to 1 frame
16/01/03   pjr      Added suspend_tbf boolean to serving cell measurement structure.
08/01/03   pjr      Added dual band defines to serving cell measurement structure.
06/12/02   pjr      Modified serving cell measurement structure.
25/11/02   pjr      Added serving cell measurement defines.
08/11/02   pjr      Added quality_report_time_index to serving cell measurements.
06/11/02   ws       Updated gpl1_gprs_send_l1_mac_tbf_rel_confirm prototype to
                    accept release_T
04/11/02   pjr      Merge Neighbour cell measurements under feature switch
01/11/02   pjr      Added serving cell measurements under feature switch
12/09/02    pjr     Removed pending_rel structure to prach_params_T
19/08/02    pjr     Added function prototype gpl1_gprs_send_l1_mac_tbf_rel_confirm
08/27/02    DLH     Added gprs_l1_non_drx_flag
19/08/02    pjr     Removed arfcn from the structure prach_params_T
8/08/02     ws      Added Prototype for Single block
01/08/02    ws      Corrected discarded ptcch calback prototype
15/07/02    ws      Corrected error with parameter type for cont. ta  in
                    gpl1_gprs_config_ptcch()
09/07/02    pjr     Added pending_rel structure to prach_params_T
18/06/02    pjr     Added prach_config_once flag to prach_params_T
27/05/02    ws      Added (re)config protoypes for up and downlink channels
10/04/02    pjr     Added prach_data_valid to the structure type prach_params_T
01/12/01    ws      Initial version created

===========================================================================*/

/* Avoid including header files */

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*========================== #defines =====================================*/

/* Enable the handling of USF granularity of 4 blocks */

/* defines the FN offset from PL1 to NPL1 to schedule RX or TX messages */
#define NPL1_FN_OFFSET  1
#define TEMP_DELAY_OS   4

/* Defines the assignment start time modulo value */
#define  PL1_FN_MODULO42432  42432U

#define PAYLOAD_TYPE_BITMASK  0xC0
#define PAYLOAD_TYPE_SHIFT    6
/*
 * Check's the payload type in a received downlink mac control block
 * which apears in top two MSB's of octet 1
 */
#define CHECK_PAYLOAD_TYPE(x) ( ( (*x) & PAYLOAD_TYPE_BITMASK ) >> PAYLOAD_TYPE_SHIFT )

/*
 * Check's the TFI in the incomming RLC data block against the tfi received
 * in the assignment
 * TFI is in bits 2 to 6 of octet 2
 */

#define CHECK_RLC_TFI(msg_in, tfi) ( ((msg_in[1] & 0x3E)>>1) != tfi? FALSE : TRUE)

/*
 * Check's the TFI in the incomming RLC control block against the tfi received
 * in the control message with payload type = 2 and Check if AC bit (LSB) is
 * present to ascertain if TFI value is available in next octet.
 * Also additional check that it is a control block for Downkink TBF D bit = 1
 * TFI is in bits 2 to 6 of octet 2
 */
#define CHECK_RLC_TFI_IN_CTRL_BLK(msg_in, tfi) (( ( ((msg_in[0]  & 0xC0) >> 6 ) == 2)       && \
                                                  ( ((msg_in[1]  & 0x01))       == 1)       && \
                                                  ( ((msg_in[2]  & 0x01))       == 1)       && \
                                                  ( (((msg_in[2] & 0x3E)) >> 1) == tfi) ) ? TRUE : FALSE)


/*  Definitions for PRACH functionality */
#define PRACH_INHIBIT_COUNT     6
#define DL_UL_START_SET_INVALID 0xff
#define PRACH_DELAY_INVALID     0xff

/* Gamma values as per 5.08 */
#define GAMMA_GSM900            (39 * 16)
#define GAMMA_GSM1800           (36 * 16)

#include "geran_variation.h"
#define GAMMA_GSM1900           (36 * 16)
#define GAMMA_GSM850            (39 * 16)

#define L1_DATA_BLOCK               0x0
#define L1_CTRL_BLOCK               0x1
#define L1_CTRL_BLOCK_PLUS_OPTIONS  0x2
#define L1_RESERVED_BLOCK           0x3

#define GL1_ACCESS_CONFIRM_TIMEOUT 0xC8 /* 200 milliseconds */

#if defined ( FEATURE_GSM_DTM ) && defined ( FEATURE_GSM_EDTM )
/* This is used to enable extra debug for the ul pacch fifo
 * deletion.
 */
#define  DEBUG_GPL1_PACCH_FIFO_DELETION  (0)
#endif

/*========================= typedefs ======================================*/

/* Enums */
typedef enum
{
  NORMAL_PRACH_TX,
  NO_PRACH_DATA,
  HALT_PRACH
} gl1_gprs_prach_status_e;

typedef enum
{
  PRACH_NORMAL_NO_RRBP,
  PRACH_SUSPEND_RRBP_PENDING,
  PRACH_SUSPEND_RRBP_SENT,
  PRACH_SUSPEND_RESCHED_PRACH
}prach_suspended_e;

enum /* gprs mframe blocks */
{
  B0,
  B1,
  B2,
  B3,
  B4,
  B5,
  B6,
  B7,
  B8,
  B9,
  B10,
  B11
};

typedef enum
{
    B1_BS_PBCCH_BLKS,
    B6_BS_PBCCH_BLKS,
    B3_BS_PBCCH_BLKS,
    B9_BS_PBCCH_BLKS
} pbcch_blks_e;

typedef enum
{
  GPRS_NORMAL_PAGING,
  GPRS_EXTENDED_PAGING,
  GPRS_PAGING_REORGANIZATION,
  GPRS_SAME_AS_BEFORE
}gprs_page_mode_e;

typedef enum
{
  UPDATE_ABORT_FLAG,
  UPDATE_BUFFER,
  RESET_BUFFER

} gpl1_defer_mode_t;

/* Structure Tyepdefs */

typedef struct
{
    uint8   block_num;
    uint32  start_frame_num;
    uint8   number_of_frames;    
    boolean g2x_notGTA; /*TRUE means G2W/G2T TA*/
}tbf_suspend_T;

typedef struct
{
    struct
    {
    boolean  usf;
    } block[12];

}l1_prach_metrics_T;

typedef struct
{
    gl1_defs_tn_type          tn;
    int16                     tx_sig_strngth;
    int16                     rx_sig_strngth;
    gl1_defs_coding_type      coding_scheme;
    uint8                     bsic;
    uint8                     pccch_tsc;
    uint8                     ab_tsc; /* Access Burst TSC */
    boolean                   usf_enable;
    uint16                    current_delay;
    uint16                    next_delay;
    uint16                    prach_frame_to_tx;
    uint8                     prach_frame_offset;
    uint16                    data_contents;
    uint8                     prach_rrbp_fn;
    uint8                     bs_pbcch_blks;
    boolean                   tx_access_burst;
    boolean                   waiting_for_usf;
    boolean                   usf;
    boolean                   usf_block;
    boolean                   prach_indicator;
    boolean                   prach_indicator_transmitted;
    prach_suspended_e         prach_suspended;
    gl1_gprs_prach_status_e   prach_data_valid;
    boolean                   prach_config_once;
    boolean                   rrbp_pending;
    uint32                    abort_fn;
    boolean                   abort_prach_once;
} prach_params_T;

/* Store for accessing vars shared between packet transfer and DTM modes */

typedef struct
{
  boolean                                      usf_detected;
  boolean                                      enable_tx;
  boolean                                      first_rx;
  boolean                                      defer_abort_flag;
  rlc_ul_status_T                              deferred_get_buff;
  boolean                                      abort_callbacks;
  uint8                                        num_rx_tn;
  gl1_defs_rx_pkt_data_type                    gpl1_gprs_dyn_dl_data[MAX_NUM_DL_MSGS]; /* buffer for storing DL PACCH PDTCH data */

#ifdef  FEATURE_GSM_GPRS_MSC34

  /* buffer for storing DL metrics */
  gl1_defs_rx_pkt_metrics_type                 gpl1_gprs_dyn_dl_metrics[PL1_MULTISLOT_RX][GL1_DEFS_FRAMES_IN_BLK_PERIOD];

  /* holds ptr's to metrics, log and data buffer for rx's */
  gl1_defs_rx_pkt_mem_type                     rlc_mac_dl_info[PL1_MULTISLOT_RX];

#else

  gl1_defs_rx_pkt_metrics_type                 gpl1_gprs_dyn_dl_metrics[4][4]; /* buffer for storing DL metrics */
  gl1_defs_rx_pkt_mem_type                     rlc_mac_dl_info[4]; /* holds ptr's to metrics, log and data buffer for rx's */

#endif

#ifdef FEATURE_GSM_DTM
  gl1_msg_afc_tt_mode                          gpl1_afc_tt_mode;
#endif


boolean                                        start_usf_gran_count;
uint8                                          usf_gran_count;

  /* Holds blanking information used by RLC to control data flow */
  l1_usf_utilisation_info_t                    usf_utilisation_stats;
  /* Calculate the blanking average, this is used by gl1 to request RR to reduce MSC */
  uint32                                       usf_utilisation_blanking_average;

} gpl1_ps_store_T;

typedef struct
{
    uint8  num_dl_pdch;
    uint8  lowest_dl_timeslot;
    uint8  highest_dl_timeslot;
} downlink_timeslot_info_t;

#ifdef  FEATURE_GSM_DTM
#ifdef  FEATURE_GSM_EDTM

/* This is used to allow the deleting of particular PACCH FIFO
 * radio blocks on a particular frame.
 */
typedef struct
{
  boolean                      valid;
  mac_l1_delete_ul_ctrl_msg_t  msg;

} q_mac_l1_delete_ul_ctrl_msg_t;

#endif
#endif

/*========================= Public Data declarations ======================*/

extern gl1_msg_tx_pkt_cb_type  prach_callbacks;
extern prach_params_T         *l1_prach_ptr;


extern uint8           gprs_pbcch_rx_in_progress;
extern uint8           gprs_pccch_rx_in_progress;
extern uint8           idle_monitors_in_progress;

extern boolean         psi1_flag;

extern boolean         ext_page_blk_flag;
extern boolean         read_extended_block;

extern boolean         gprs_sleep_timer;
extern boolean         calc_gap;
/* this array is used to convert nc_non_DRX_period to frame ticks */
extern uint16          nc_non_DRX_period[8];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/****************** Packet Idle mode Related functions *********************/

boolean  gpl1_gprs_idle_mode_ISR( boolean  gprs_leaving_52_idle_mode );

void     gpl1_gprs_set_psi1_flag( boolean psi1_flag );

uint8    gpl1_gprs_get_current_tc( uint8 offset );
uint8    gpl1_gprs_get_tc( uint32 );

void     gpl1_gprs_del_psi_from_list( uint8 *psi_tc_list,
                                      uint8  i );

void     gpl1_gprs_schedule_serving_psi1( uint8  timeslot );
void     gpl1_gprs_read_serving_psi( void );
void     gpl1_gprs_read_pbcch( void );

void     gpl1_gprs_send_acq_psi_rej( void );
void     gpl1_gprs_send_psi1_decode_fail_ind( void );

void     gpl1_gprs_non_drx_mode( void );

                          /* PBCCH data / metrics call back functions */
void     gpl1_gprs_rx_pbcch_callback( gl1_defs_rx_pkt_data_type *msg_data,
                                      boolean                    valid );

void     gpl1_gprs_pbcch_metrics_callback( gl1_defs_rx_pkt_metrics_type *md[GL1_DEFS_MAX_DL_MSGS],
                                           uint8                         num_msgs );

                          /* PCCCH data / metrics call back functions */
void     gpl1_gprs_rx_pccch_callback( gl1_defs_rx_pkt_data_type *data,
                                      boolean                    valid );

void     gpl1_gprs_pccch_metrics_callback( gl1_defs_rx_pkt_metrics_type *md[GL1_DEFS_MAX_DL_MSGS],
                                           uint8                         num_msgs,
                                           uint8                         burst_num );

extern boolean gpl1_gprs_pbcch_get_psi1_flag( void );
extern void gpl1_gprs_pbcch_set_psi1_flag(boolean psi1_flag );
/****************** Packet Access mode Related functions *********************/

boolean  L1_packet_access_ISR( l1_mode_command_T  mode );

void     l1_prach_tx_callback( gl1_msg_tx_scheduled_data_type  msgs_txd[ GL1_DEFS_MAX_UL_MSGS ],
                               uint8                           num_msg_txd );

void     l1_prach_discard_callback( gl1_msg_tx_discarded_data_type  msgs_discarded[ GL1_DEFS_MAX_UL_MSGS ],
                                    uint8                           num_msg_txd );

void     l1_prach_metrics_callback( gl1_defs_rx_pkt_metrics_type *md[],
                                    uint8                         num_msgs,
                                    uint8                         burst_num );

void     prach_pbcch_data( gl1_defs_rx_pkt_msg_type *msg_data );
void     prach_pbcch_pccch_data( gl1_defs_rx_pkt_data_type  *msg_data,
                                 boolean                     valid );
prach_params_T*        gl1_ms_switch_prach_params_idle_if_data(gas_id_t gas_id);
/****************** Packet Transfer mode Related functions *********************/

boolean  gpl1_gprs_control_transfer( l1_mode_command_T  mode, gas_id_t gas_id );

void gpl1_gprs_notify_start(transfer_data_T *transfer_data_ptr, gas_id_t gas_id);

void gpl1_gprs_handle_tbf_complete(transfer_data_T *transfer_data_ptr, gas_id_t gas_id);

boolean gpl1_gprs_handle_reconfig( transfer_data_T*   transfer_data_ptr,
                                       l1_mode_command_T  mode,
                                       uint8 l1_state, gas_id_t gas_id );
const gpl1_ps_store_T *  gpl1_read_xfer_isr_store(gas_id_t gas_id);


void gpl1_gprs_update_usf_det( boolean  usf_detected, gas_id_t gas_id);

void gpl1_gprs_update_first_rx( boolean  first_rx, gas_id_t gas_id );

void gpl1_gprs_update_enable_tx( boolean enable_tx, gas_id_t gas_id );

void gpl1_gprs_update_deferred_buff(rlc_ul_status_T * rlc_buf,
                                    boolean abort_flag,
                                    gpl1_defer_mode_t mode,
                                    gas_id_t gas_id);

void gpl1_gprs_update_num_rx_tn( uint8 num_rx_tn, gas_id_t gas_id );

void gpl1_gprs_update_abort_callbacks( boolean  abort_callbacks, gas_id_t gas_id );

void gpl1_gprs_init_metrics_cb_store( gas_id_t gas_id );

void gpl1_gprs_init_data_cb_store(uint8 index, gl1_defs_rx_pkt_data_type *buff_ptr, gas_id_t gas_id);

void gpl1_gprs_init_dl_fifo(gas_id_t gas_id);

#ifdef FEATURE_GSM_COEX_SW_CXM
/*===========================================================================

FUNCTION gpl1_get_ul_slammed_tn

DESCRIPTION
  get the corresponding slammed ul slot

DEPENDENCIES

RETURN VALUE
===========================================================================*/
uint8 gpl1_get_ul_slammed_tn(uint8 tn_index, gas_id_t gas_id);
#endif


void gpl1_gprs_set_usf_gran_info( uint8    usf_gran_count,
                                  boolean  start_usf_gran_count,
                                  gas_id_t gas_id );


#define GPL1_RESET_DEFER_BUFF( gas_id)  gpl1_gprs_update_deferred_buff(NULL,FALSE,RESET_BUFFER, gas_id)
#define GPL1_SET_ABORT_FLAG( gas_id ) gpl1_gprs_update_deferred_buff(NULL,TRUE,UPDATE_ABORT_FLAG, gas_id)
#define GPL1_CLEAR_ABORT_FLAG( gas_id ) gpl1_gprs_update_deferred_buff(NULL,FALSE,UPDATE_ABORT_FLAG, gas_id)

#define GPL1_DISABLE_TX( gas_id )  gpl1_gprs_update_enable_tx(FALSE, gas_id)
#define GPL1_ENABLE_TX( gas_id )   gpl1_gprs_update_enable_tx(TRUE, gas_id)

#define GPL1_RESET_USF_DETECT( gas_id )  gpl1_gprs_update_usf_det(FALSE, gas_id)
#define GPL1_SET_USF_DETECT( gas_id )  gpl1_gprs_update_usf_det(TRUE, gas_id)
#define GPL1_RESET_ABORT_CB( gas_id )  gpl1_gprs_update_abort_callbacks(FALSE, gas_id)
#define GPL1_SET_ABORT_CB( gas_id )  gpl1_gprs_update_abort_callbacks(TRUE, gas_id)

/* Functions common to transfer and dtm */

/* Used to provide lowest/highest timeslot information resulting from
 * the received assignment/release message.
 */
void  gpl1_calc_dyn_dl_ts_info( uint8                       downlink_timeslot_alloc,
                                const uint8                 usf_value[ GL1_DEFS_SLOTS_IN_FRAME ],
                                downlink_timeslot_info_t*   downlink_ts_info_ptr,
                                gl1_defs_rx_pkt_alloc_type  alloc_type[ GL1_DEFS_MAX_ASSIGNED_DL_TS ],
                                gas_id_t gas_id );

/* Function checks if there is a release pending */
boolean gpl1_gprs_awaiting_release( gas_id_t gas_id );

#ifdef FEATURE_GPRS_GBTA
boolean gpl1_gprs_leaving_transfer( gas_id_t gas_id );
boolean gpl1_gprs_reconfig_progress( uint32 frames_required,gas_id_t gas_id );
#endif /* FEATURE_GPRS_GBTA */


/* This function is called in the transfer init state to setup
 * incremental redundancy i.e. configure NPL1.
 * Sets the rlc mode in the transfer data structure.
 */
void  gpl1_init_egprs_incr_redundancy    ( transfer_data_T *transfer_data_ptr,
                                                  uint32           FN ,
                                                  gas_id_t        gas_id);

/* This function is called in the transfer reconfig state to setup
 * incremental redundancy i.e. configure NPL1.
 * Updates the rlc mode in the transfer data structure.
 */
void  gpl1_reconfig_egprs_incr_redundancy( transfer_data_T *transfer_data_ptr,
                                                  uint32           FN,
                                                  gas_id_t        gas_id);


/* Methods used to gather uplink tbf data/dummy
 * block statistics.
 */
void  gpl1_reset_uplink_tbf_blk_count( gas_id_t gas_id );
void  gpl1_increment_uplink_tbf_data_blk_count( gas_id_t gas_id );
void  gpl1_increment_uplink_tbf_dummy_blk_count( gas_id_t gas_id );

       /**************** DTM mode Related functions ****************/
#ifdef FEATURE_GSM_DTM

l1_dtm_mode_T gpl1_dtm_control(  gas_id_t gas_id );
l1_dtm_mode_T gpl1_dtm_control_cs_ps(dtm_data_T   *dtm_data, boolean apply_freq_redef, gas_id_t gas_id);
boolean gpl1_dtm_complete_abort_procedure( l1_dtm_mode_T abort_status, gas_id_t gas_id );
void gpl1_dtm_invalidate_mac_l1_release_from_message_store( gas_id_t gas_id );

#ifdef  FEATURE_GSM_EDTM

extern void  gpl1_decrement_fn_on_next_tbase_change( gas_id_t gas_id );

/* This function searches through the pacch fifo and
 * removes any pending messages.
 * @@WARNING: This is to be called only radio block
 * boundaries.
 */
void  gpl1_pacch_fifo_delete_msg( uint8  csn_msg_type, gas_id_t gas_id );

/* This function queues the mac l1 delete ul ctrl msg */
extern void  gpl1_queue_mac_l1_delete_ul_ctrl_msg( uint8  message_type, gas_id_t gas_id);

/* This function removes the mac l1 delete ul ctrl msg */
extern void  gpl1_remove_mac_l1_delete_ul_ctrl_msg( gas_id_t gas_id );

extern const q_mac_l1_delete_ul_ctrl_msg_t*  gpl1_read_mac_l1_delete_ul_ctrl_msg( gas_id_t gas_id );

#endif

#endif

extern void  gl1_egprs_open_srb_loopback( gas_id_t gas_id );

           /**************** Single Block Allocation ****************/

boolean  gpl1_gprs_control_single_blk( single_blk_information_T *single_blk_ptr,
                                       frequency_information_T  *freq_info_ptr,
                                       l1_mode_command_T         mode, gas_id_t gas_id );

                   /* Data/metrics call back functions */
void     sb_dl_data_cb( gl1_defs_rx_pkt_data_type  *msg_data,
                        boolean                    valid, gas_id_t gas_id );

void     sb_dl_metrics_cb( gl1_defs_rx_pkt_metrics_type *md[GL1_DEFS_MAX_DL_MSGS],
                           uint8                         num_msgs,
                           uint8                         burst_num,
                           gas_id_t gas_id );

                   /* Transmission failed/succeeded call back function */
void     gpl1_gprs_tx_single_pdtch_disc_cb( gl1_msg_tx_discarded_data_type  msgs_discarded[ GL1_DEFS_MAX_UL_MSGS ],
                                            uint8                           num_msg_txd );

void     gpl1_gprs_tx_single_pdtch_sched_cb( gl1_msg_tx_scheduled_data_type  msgs_txd[ GL1_DEFS_MAX_UL_MSGS ],
                                             uint8                           num_msg_txd, gas_id_t gas_id );

          /**************** Dynamic Allocation ****************/

boolean  gpl1_gprs_control_dynamic_tbf( dynamic_tbf_information_T *dynamic_tbf_ptr,
                                        frequency_information_T   *freq_info_ptr,
                                        l1_mode_command_T          mode, gas_id_t gas_id );

                  /* PDCH data/metrics call back functions */
void     gpl1_gprs_rx_dyn_data_cb( gl1_defs_rx_pkt_data_type *msg_data,
                                   boolean                    valid , gas_id_t gas_id);

void     gpl1_gprs_rx_dyn_metrics_cb( gl1_defs_rx_pkt_metrics_type *md[GL1_DEFS_MAX_DL_MSGS ],
                                      uint8                         num_msgs,
                                      uint8                         burst_num, gas_id_t gas_id );

                  /* PDCH/RRBP Transmission failed/succeeded callback function */
void     gpl1_gprs_tx_dyn_rrbp_disc_cb( gl1_msg_tx_discarded_data_type  msgs_discarded[ GL1_DEFS_MAX_UL_MSGS ],
                                        uint8                           num_msg_txd,
                                        gas_id_t                        gas_id );

void     gpl1_gprs_tx_dyn_rrbp_sched_cb( gl1_msg_tx_scheduled_data_type  msgs_txd[ GL1_DEFS_MAX_UL_MSGS ],
                                         uint8                           num_msg_txd,
                                         gas_id_t                        gas_id );

                  /* PDCH/RRBP USF utilisation callback function */
void     gpl1_gprs_tx_dyn_rrbp_usf_util_cb( gl1_msg_usf_utilisation_data_type usf_utilisation,
                                            gas_id_t                          gas_id);

                  /* PBCCH data/metrics  call back functions */
void     gpl1_pkt_transfer_pbcch_data_cb( gl1_defs_rx_pkt_data_type  *data_ptr,
                                          boolean                    valid_callback );

void     gpl1_pkt_transfer_pbcch_metrics_cb( gl1_defs_rx_pkt_metrics_type  *md[ GL1_DEFS_MAX_DL_MSGS ],
                                             uint8                         num_msgs );

              /* Packet transfer frame tick notification to NPL1 */
void     gpl1_gprs_transfer_send_ftn( uint8             *pacch_pdtch_ul_blk_start_cnt,
                                      uint8             *pacch_pdtch_dl_blk_start_cnt,
                                      l1_mode_command_T  mode, gas_id_t gas_id );
void     gpl1_gprs_config_agc( gas_id_t gas_id );

uint8*   gpl1_gprs_get_ul_data( gl1_defs_coding_type *coding_scheme );

void gpl1_gprs_get_arfcns(int16 start, uint16 increment, uint16 num_arfcns, ARFCN_T arfcns[], gas_id_t gas_id);


void     gpl1_gprs_reset_pacch_fifo(gas_id_t gas_id);
geran_pdu_priority_t  gpl1_gprs_get_egprs_ul_data( gl1_defs_coding_type     *coding_scheme,
                                      gl1_defs_puncturing_type  puncturing_scheme[2],
                                      uint8                    *msg_data[3],
                                      gas_id_t                  gas_id,
                                      boolean                   single_block_alloc );

            /* Packet transfer TBF suspension functions */
void  gpl1_gprs_tbf_susp_range( uint32         multiframe_start,
                                uint16         frame_index,
                                tbf_suspend_T *ncell_frame_struct,
                                uint8          bcch_fn_mod52, gas_id_t gas_id );

boolean  gpl1_gprs_check_tbf_suspension( uint32  start_frame,
                                         uint8   no_of_frames,
                                         boolean check_ptcch, gas_id_t gas_id);

void     gpl1_gprs_init_tbf_suspension( gas_id_t gas_id );

void     gpl1_gprs_set_serving_psi1_tbf_suspension( uint32 psi1_fn );

uint32   gpl1_gprs_get_serving_psi1_fn( void );

boolean  gpl1_get_tbf_resumption_fn(uint32 *resumption_frame_nbr, gas_id_t gas_id);

             /* L1 --> MAC/RLC confirmation/indication message functions */
void  gpl1_send_mac_ph_random_access_conf( boolean  tx_pcr,
                                           uint32   prach_fn,
                                           boolean  prach_ind,
                                           boolean  bad_cell);

void  gpl1_gprs_send_l1_mac_single_block_sent( gas_id_t gas_id );

void  gpl1_gprs_send_rlc_ph_ready_to_send_ind( void );

void  gpl1_gprs_send_l1_mac_ph_connect_ind( uint16 tbf_type, gas_id_t gas_id );
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
void gpl1_gprs_send_l1_mac_ph_connect_ind_trm_failed(uint16 tbf_type, gas_id_t gas_id );
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

void  gpl1_gprs_send_mac_ph_data_ind( uint32  frame_number,
                                      uint8   timeslot,
                                      uint8  *control_data, gas_id_t gas_id );

void  gpl1_gprs_send_l1_mac_usf_detected( gas_id_t gas_id );

void  gpl1_gprs_send_l1_mac_tbf_rel_confirm( release_T tbf_type, gas_id_t gas_id );

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER
boolean gpl1_conflict_check_for_ccch (gas_id_t gas_id);
#ifdef FEATURE_DUAL_SIM
void gpl1_check_rel_resv_ccch_xfer(boolean rel_or_resv, gas_id_t gas_id);
#endif
#endif /* FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */


#ifdef  FEATURE_GSM_DTM
#ifdef  FEATURE_GSM_EDTM

void gpl1_send_mac_ph_cs_connect_ind( uint8 cs_timeslot,gas_id_t gas_id );

#endif
#endif

/*********************** Ncell Related functions *******************************/

void l1_sc_balist_gprs_set_sync_in_task( void *, gas_id_t gas_id );
void gpl1_gprs_change_timeslot ( byte new_ts,  gas_id_t gas_id );

/*  Neighbour Cell Measurements */
boolean  gpl1_gprs_measurements( l1_mode_command_T, gas_id_t gas_id );

void     gpl1_gprs_do_RESELECTION_GPRS( IMH_T *msg_header );

void     gpl1_gprs_setup_RESELECTION_GPRS_PBCCH( IMH_T *msg_header );

/**************************** Non-modal ****************************************/


               /* Ms Timebase information functions */
uint8  gpl1_gprs_ts_conv       ( uint8  un_slammed, gas_id_t gas_id ); // kf time slot slamming
uint8  gpl1_gprs_ts_conv_unslam( uint8  slammed, gas_id_t gas_id ); // kf time slot un slamming

void  gpl1_transfer_service_pending_sysinfo_reqs( gas_id_t gas_id );

/*===========================================================================

FUNCTION gpl1_reset_stop_sc_tick_trans

DESCRIPTION
  Resets the boolean variable stop_sc_tick_trans

DEPENDENCIES

RETURN VALUE
===========================================================================*/
void gpl1_reset_stop_sc_tick_trans( gas_id_t gas_id );

/*===========================================================================

FUNCTION gpl1_reset_stop_sc_tick_trans

DESCRIPTION
  Resets the boolean variable stop_sc_tick_trans

DEPENDENCIES

RETURN VALUE
===========================================================================*/
void gpl1_stop_sc_tick_trans( gas_id_t gas_id );
void gpl1_start_sc_tick_trans( gas_id_t gas_id );
boolean gpl1_get_stop_sc_tick_trans(gas_id_t gas_id );


/*===========================================================================

FUNCTION gpl1_get_egprs_ir_mem_status

DESCRIPTION
  This function returns the Incremental redundancy memory status.
 
  TRUE =  MEMORY FULL
  FALSE = MEMORY NOT FULL

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean gpl1_get_egprs_ir_mem_status(gas_id_t gas_id);

#endif /* L1_ISR_H */

