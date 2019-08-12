#ifndef GPL1_GPRS_SERVING_CELL_H
#define GPL1_GPRS_SERVING_CELL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GPRS/EGPRS  SERVING CELL MEASUREMENTS
                       H E A D E R   F I L E

DESCRIPTION
   GPRS/EGPRS Seving cell measurements header file.


Copyright (c) 2004-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gpl1_gprs_serving_cell.h#1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
26/03/12   jj       CR344695 rename feature name FEATURE_GSM_GPRS_MSC34 to FEATURE_GSM_GPRS_MSC33 
17/08/09   ap       CR190902 BER support needed for PS in 2G
26/01/09   og       Ensure rxqual calculations ignore CS4 radio blocks. Resolves
                    CR170214.
24/11/08   og       FEATURE_GSM_GPRS_MSC33 GPL1 changes.
05/11/07   og       Add NV item 1027 support to EGPRS BEP.
24/09/07   og       Handle the error fatal case where EGPRS DL TBF is released
                    but queued nodes still exist.
06/07/07   og       Delay the EPDAN REQ generation so it occurs four
                    tdma frames before transmission. Resolves CR122066.
12/08/05   ws       Updated for initial DTM release
31/05/05   og       Inform the egprs qual meas module of the broadcast bep period
                    directly.
16/05/05   og       Additional mean bep reporting changes.
11/05/05   og       Take TFI of the data block into consideration when
                    sliding the bler window.
09/05/05   og       Addition of BLER sliding window reference to mean bep.
20/04/05   og       Addition of filtered bep params to the msg metrics log packet.
19/04/05   og       Changes to egprs bep report generation.
14/04/05   og       correction to GMSK mean bep quantisation values.
07/04/05   og       Use npl1 mean/cv bep.
30/3/05    og       Turn on egprs bep debug printout.
17/1/05    og       Extra interference/bep message debug added.
09/12/04   og       Fix to tx power control by tracking c value on ccch.
14/10/04   og       Fixes to the handling of inteference measurements while
                    in egprs packet transfer.
30/06/04    og      Updates following integration using simulator.
23/06/04    og      Egprs Qual measurements handling.
                    Initial version created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* None */
#include "geran_variation.h"
#include "gpl1_gprs_log.h" 


/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*========================== #defines =====================================*/

/* GPRS related definitions */
#define PC_MEAS_CHAN_ON_BCCH  FALSE
#define PC_MEAS_CHAN_ON_PDCH  TRUE

/* Receive level buffers */
#define NUM_RSSI_BUFFERS  2
#define BUFFER_1  0
#define BUFFER_2  1

/* Burst Ids */
#define L1_BURST_1 0
#define L1_BURST_2 1
#define L1_BURST_3 2
#define L1_BURST_4 3

/*  Task event bit map defines  */
#define PROCESS_C_VALUE         0x01
#define PROCESS_RXQUAL_SIGN_VAR 0x02
#define QUALITY_REPORT_TIME     0x04
#define RLA_P_REPORT_TIME       0x08

/*  Maximum number of pending quality reports.  */
#ifdef  FEATURE_GSM_GPRS_MSC33

/* This is 24 * (5/4) which the ratio of 5 ts to 4 ts */
#define MAX_NUM_QUAL_RPRTS      (30)

#else

#define MAX_NUM_QUAL_RPRTS      24

#endif

/*  Quality report sent to MAC within two blocks of sending to the network  */
#define TWO_BLOCKS_BEFORE_QR    8

#define BCCH_PWR_CNTRL_TIMER    5
#define GPL1_INVALID_RXLEV      0xff

#define GPRS_SC_RLA_P_COUNT_TRANSFER       168
#define GPRS_SC_RLA_P_COUNT_TRANSFER_CONT  20
#define GPRS_SC_RLA_P_COUNT_IDLE           5
#define GPRS_SC_RLA_P_COUNT_IDLE_CONT      1

#define L1_GPRS_SERV_CELL_MEAS_BUF_LEN  170

/* Length of the filter in 52 IDLE mode */
#define GPRS_SERV_CELL_IDLE_52_AVG_LEN  5

/* Length of the filter in TRANSFER mode
 * 5 second running average start-up
 */
#define GPRS_SERV_CELL_TRANSFER_AVG_LEN  168

#define  F6_10_SHIFT_VALUE   10
#define  ONE_F6_10           1024U  /* 1 << 10     */

/* Field Value */
#define  GPRS_PL1_BEP_PERIOD2_INVALID    0xFF

/* EGPRS link_qual_meas_mode possible values */
#define  NO_MEASUREMENTS      0
#define  INTERF_MEASUREMENTS  1
#define  BEP_MEASUREMENTS     2
#define  BOTH_MEASUREMENTS    3
#define  INVALID_MEAS_MODE    4

/* Used for definning lookup table. */
#define NUM_MODULATION_SCHEMES    2

/* Quality measurements queueing system size
 * This value assumes that reporting is done as late
 * as possible and that multislot polling is off.
 */
#ifdef  FEATURE_GSM_GPRS_MSC33

/* This is 8 * (5/4) */
#define  GPL1_EGPRS_QUAL_MEAS_QUEUE_SIZE  (30)

#else

#define  GPL1_EGPRS_QUAL_MEAS_QUEUE_SIZE  (24)

#endif

/* Frame countdown value for when the queued node
 * is to be taken off the queue
 */
#define  UNLINK_NODE_FN   0

#define  TIMESLOTS_0_1_2_3    0xF0
#define  TIMESLOTS_4_5_6_7    0x0F

/* Values of the coefficient of variation of
 * error probability of a radio block in F6.10
 * format. (ETSI 05.08)
 */
#define  CV_BEP_0  1792  // 1.75 
#define  CV_BEP_1  1536  // 1.50
#define  CV_BEP_2  1280  // 1.25
#define  CV_BEP_3  1024  // 1.00
#define  CV_BEP_4   768  // 0.75
#define  CV_BEP_5   512  // 0.50
#define  CV_BEP_6   256  // 0.25
#define  CV_BEP_7     0  // 0.0

/*                     GMSK MODULATION
 *
 *       ETSI MEAN BEP      Value        Percentage    Log10( BEP)
 *                           in            value 
 *                       1/4294967295        % 
 *                          units
 */ 
#define  GMSK_MEAN_BEP_0  1078847007U    // 25.118      -0.6
#define  GMSK_MEAN_BEP_1   856958639U    // 19.952      -0.7
#define  GMSK_MEAN_BEP_2   680706443U    // 15.849      -0.8
#define  GMSK_MEAN_BEP_3   540704347U    // 12.589      -0.9
#define  GMSK_MEAN_BEP_4   429496730U    // 10.000      -1.0
#define  GMSK_MEAN_BEP_5   341161379U    //  7.943      -1.1
#define  GMSK_MEAN_BEP_6   270994116U    //  6.310      -1.2
#define  GMSK_MEAN_BEP_7   215258278U    //  5.012      -1.3
#define  GMSK_MEAN_BEP_8   170985728U    //  3.981      -1.4
#define  GMSK_MEAN_BEP_9   135818791U    //  3.162      -1.5
#define  GMSK_MEAN_BEP_10  107884701U    //  2.512      -1.6
#define  GMSK_MEAN_BEP_11   85695864U    //  1.995      -1.7
#define  GMSK_MEAN_BEP_12   68070644U    //  1.585      -1.8
#define  GMSK_MEAN_BEP_13   54070435U    //  1.259      -1.9
#define  GMSK_MEAN_BEP_14   42949673U    //  1.000      -2.0      
#define  GMSK_MEAN_BEP_15   34116138U    //  0.794      -2.1
#define  GMSK_MEAN_BEP_16   27099412U    //  0.631      -2.2
#define  GMSK_MEAN_BEP_17   21525828U    //  0.501      -2.3
#define  GMSK_MEAN_BEP_18   17098573U    //  0.398      -2.4
#define  GMSK_MEAN_BEP_19   13581879U    //  0.316      -2.5
#define  GMSK_MEAN_BEP_20   10788470U    //  0.251      -2.6
#define  GMSK_MEAN_BEP_21    8569586U    //  0.200      -2.7
#define  GMSK_MEAN_BEP_22    6807064U    //  0.158      -2.8
#define  GMSK_MEAN_BEP_23    5407043U    //  0.129      -2.9
#define  GMSK_MEAN_BEP_24    4294967U    //  0.100      -3.0
#define  GMSK_MEAN_BEP_25    3411614U    //  0.079      -3.1
#define  GMSK_MEAN_BEP_26    2709941U    //  0.063      -3.2
#define  GMSK_MEAN_BEP_27    2152583U    //  0.050      -3.3      
#define  GMSK_MEAN_BEP_28    1709857U    //  0.040      -3.4      
#define  GMSK_MEAN_BEP_29    1358188U    //  0.032      -3.5      
#define  GMSK_MEAN_BEP_30    1078847U    //  0.025      -3.6      
#define  GMSK_MEAN_BEP_31          0U    //  0.020    < -3.6

/*                     8PSK MODULATION
 *
 *       ETSI MEAN BEP      Value        Percentage    Log10( BEP)
 *                           in            value 
 *                       1/4294967295        % 
 *                          units
 */ 
#define  PSK8_MEAN_BEP_0  1078847007U    // 25.119      -0.60
#define  PSK8_MEAN_BEP_1   983920165U    // 22.909      -0.64
#define  PSK8_MEAN_BEP_2   897345855U    // 20.893      -0.68
#define  PSK8_MEAN_BEP_3   818389147U    // 19.055      -0.72
#define  PSK8_MEAN_BEP_4   746379773U    // 17.378      -0.76
#define  PSK8_MEAN_BEP_5   680706443U    // 15.849      -0.80
#define  PSK8_MEAN_BEP_6   620811654U    // 14.454      -0.84
#define  PSK8_MEAN_BEP_7   566186958U    // 13.183      -0.88
#define  PSK8_MEAN_BEP_8   516368643U    // 12.023      -0.92
#define  PSK8_MEAN_BEP_9   470933799U    // 10.965      -0.96
#define  PSK8_MEAN_BEP_10  429496730U    // 10.000      -1.00
#define  PSK8_MEAN_BEP_11  391705673U    //  9.120      -1.04
#define  PSK8_MEAN_BEP_12  357239819U    //  8.318      -1.08
#define  PSK8_MEAN_BEP_13  325806588U    //  7.586      -1.12
#define  PSK8_MEAN_BEP_14  297139139U    //  6.918      -1.16
#define  PSK8_MEAN_BEP_15  270994116U    //  6.310      -1.20
#define  PSK8_MEAN_BEP_16  187482122U    //  4.365      -1.36
#define  PSK8_MEAN_BEP_17  129705939U    //  3.020      -1.52
#define  PSK8_MEAN_BEP_18   89734586U    //  2.089      -1.68
#define  PSK8_MEAN_BEP_19   62081165U    //  1.445      -1.84
#define  PSK8_MEAN_BEP_20   42949673U    //  1.000      -2.00
#define  PSK8_MEAN_BEP_21   29713914U    //  0.692      -2.16
#define  PSK8_MEAN_BEP_22   20557006U    //  0.479      -2.32
#define  PSK8_MEAN_BEP_23   14221973U    //  0.331      -2.48
#define  PSK8_MEAN_BEP_24    9839202U    //  0.230      -2.64
#define  PSK8_MEAN_BEP_25    6807064U    //  0.158      -2.80
#define  PSK8_MEAN_BEP_26    4709338U    //  0.110      -2.96
#define  PSK8_MEAN_BEP_27    3258066U    //  0.076      -3.12
#define  PSK8_MEAN_BEP_28    2254031U    //  0.052      -3.28
#define  PSK8_MEAN_BEP_29    1559408U    //  0.036      -3.44
#define  PSK8_MEAN_BEP_30    1078847U    //  0.025      -3.60
#define  PSK8_MEAN_BEP_31          0U    //  0.020    < -3.60

/* BEP PERIOD2 Values in fixed point F6.10 format */
#define  BEP_PERIOD2_FIELD_VALUE_0   1024  // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_1   819   // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_2   665   // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_3   512   // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_4   409   // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_5   307   // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_6   256   // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_7   204   // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_8   153   // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_9   102   // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_10  81    // BEP_PERIOD
#define  BEP_PERIOD2_FIELD_VALUE_11  66
#define  BEP_PERIOD2_FIELD_VALUE_12  51
#define  BEP_PERIOD2_FIELD_VALUE_13  40
#define  BEP_PERIOD2_FIELD_VALUE_14  30

#define  GPRS_PL1_FORGET_FACTOR_INVALID  0xFFFF
#define  NUM_MEAN_BEP_VALUES  32

/* Used to provide extra debug for inteference/bep measurements */
#define DEBUG_GPL1_INTERF_MEAS  (0)
#define DEBUG_GPL1_EGPRS_BEP    (1)

/* Used to turn on/off mean bep reporting optimisations
 * Do not change this, this is now controlled via
 * nv items.
 */
#define EGPL1_OPTIMISE_MEAN_BEP  (1)

/*========================= typedefs ======================================*/

/* Enums */
typedef enum
{
  NO_PTCCH,
  UL_PTCCH,
  DL_PTCCH,
  UL_DL_PTCCH
} ta_meas_in_prog_E;

typedef enum
{
  SCM_INIT,
  SCM_RUNNING
#ifdef FEATURE_GSM_DTM
  ,SCM_DTM_RUNNING
#endif
} state_E;

/* Primary data structure typedefs */
/* Gprs Related typedefs */
typedef struct
{
  uint32        value;
  uint16        count;
} rx_qual_T;

typedef struct
{
  uint8    ptcch_count;
  uint8    idle_count;
  uint8    measurement_count;
  boolean  valid_ind;
  int32    ss_chn;
  int32    meas_1;
} interference_T;

typedef struct
{
  uint8    last_burst_received;
  uint8    num_on_bcch;
  int16    accum_block_rssi;
  int16    burst_ss[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ];
  boolean  burst_on_bcch[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ];
  uint8    num_bursts;
} block_rssi_t;

typedef struct
{
  uint8    tn;
  boolean  valid;
} lowest_tn_T;

/* EGPRS Related typedefs */
typedef struct
{
    uint16  mean_bep_tn;
    uint16  cv_bep_tn;
    uint16  num_blocks;
    uint16  r;         /* ETSI 05.08 filter reliability parameter */
} gpl1_egprs_qual_meas_T;

typedef struct
{
    uint16                bep[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ]; 
    gl1_defs_coding_type  coding_type;
    boolean               decode_error;

#if  EGPL1_OPTIMISE_MEAN_BEP
    boolean               first_payload_error;
    boolean               second_payload_error;
#endif
} gpl1_egprs_decode_metrics_T;

typedef struct
{
    gl1_defs_tn_type  timeslot;
    uint8             rrbp;
    uint8             es_p;
    uint16            bsn1;
    uint32            rrbp_fn;
} gpl1_egprs_qual_meas_data_T;

#if  EGPL1_OPTIMISE_MEAN_BEP
typedef struct
{
    gl1_defs_coding_type  coding_scheme;
    boolean               data_crc_fail;
} gpl1_egprs_bler_window_element_T;
#endif


/* Secondary data structure typedefs */

/* Queue typedef and associated macros */
typedef struct gpl1_egprs_qual_meas_queue_T
{
    /* Num frames till node is taken off the queue */
    uint8                                fn_countdown;
    /* Information holding part */
    gpl1_egprs_qual_meas_data_T          data;
    /* Pointer to next element in queue */
    struct gpl1_egprs_qual_meas_queue_T *next_ptr;
} gpl1_egprs_qual_meas_queue_T;

typedef struct
{
uint32 acc_8psk_r;
uint32 acc_mean_8psk_bep_tn;
uint32 num_8psk_blocks;
uint32 acc_gmsk_r;
uint32 acc_mean_gmsk_bep_tn;
uint32 num_gmsk_blocks;
}egprs_mean_bep_store_T;

typedef struct
{
egprs_mean_bep_store_T egprs_mean_bep_store[2];
uint8 active_buffer;
}egprs_rxqual_calc_T;

/* The serving cell measurement structure */
typedef struct
{
  /* Neighbour/Serving cell receive level average */
  running_avg_struct  rla_p_buffer;
  uint8               rla_p_count;
  uint8               rla_p_count_idle;
  int16               rla_p_isr_avg;
  int16               rla_p_reported_value;
  dBx16_T             reported_rxlev;
  int16               serv_cell_int;
  uint8               rla_p_count_transfer;

  /* Pkt Transfer C value calculations on BCCH
   * Used when pc mean chan == FALSE.
   */
  uint8               c_value_burst_count;
  int16               power_cntrl_reported_value;
  int32               power_cntrl_samples;
  int16               power_cntrl_avg;
  uint8               power_cntrl_count;

  /* Pkt Idle/Transfer C value calculations on PDCH
   * Used in transfer when pc meas chan == TRUE.
   */
  int8                weighted_pb;
  int16               ssblockn;
  block_rssi_t       *rssi_buffer_ptr;
  block_rssi_t        block_rssi[ NUM_RSSI_BUFFERS ];

  int32               sign_var;

  uint8               N_DRX;
  uint8               task_event;
  int16               cn;
  uint32              c_index;
  int32               cn_1;
  rx_qual_T           rx_qual;

  /* GPRS Measurement report generation */
  uint32              qrt;
  uint32              quality_report_time[MAX_NUM_QUAL_RPRTS];
  uint8               quality_report_count;
  uint8               quality_report_time_index;

  boolean             cal_c_value_51_idle;
  /* Tx power calculations */
  uint8               pmax;
  uint8               alpha;
  sys_band_T          ul_power_band;
  uint8               gamma_tn[ GL1_DEFS_SLOTS_IN_FRAME ];
  uint32              pch[ GL1_DEFS_SLOTS_IN_FRAME ];
  uint16              gamma_gsm;
  boolean             gprs_LB_MS_TXPWR_MAX_CCH_valid;
  uint8               lb_pmax;
  /* Interference Measurements */
  boolean             suspended_tbf;
  boolean             idle_frame_free;
  ta_meas_in_prog_E   ta_meas_in_prog;
  interference_T      int_array[GL1_DEFS_SLOTS_IN_FRAME];
  boolean             tx_in_progress[ GL1_DEFS_SLOTS_IN_FRAME ];
  gl1_defs_int_meas_result_type  int_results_info;
  boolean             idle_int_freq_array_valid;

  boolean             egprs_interf_meas_required;

  /* Used for EGPRS link quality reporting */
  uint8  link_qual_meas_mode;
  uint8  interf_meas_ts_bitmap;

  /* pbcch/bcch bep period forgetting factor */
  uint16                  broadcast_e;

  /* pacch(individual) or pbcch/bcch forgetting factor in use */
  uint16                  e;

  gpl1_egprs_qual_meas_T  qual_meas[ NUM_MODULATION_SCHEMES ]
                                   [ GL1_DEFS_SLOTS_IN_FRAME ];
/* Store used for calculating rxqual when in EGPRS */
  egprs_rxqual_calc_T egprs_rxqual_calc;


  lowest_tn_T        lowest_tn;
  uint8              prev_lowest_tn;
  uint8              prev_highest_tn;
  uint8              highest_tn;

  state_E            state;
} gprs_pl1_serving_cell_meas_T;


/*========================= Public Data declarations ======================*/

 extern  gprs_pl1_serving_cell_meas_T  gprs_serv_cell_meas[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Methods to access/change the GPRS measurements in the scell
 * measurements structure.
 */
                    /* Interference measurements */

extern void     gpl1_egprs_scell_interf_meas_config( uint8  link_qual_meas_mode , gas_id_t gas_id);
extern uint8  gpl1_egprs_meas_scale( const uint32  *log_scale_ptr,
                                     uint32         target_measurement );

extern void     gpl1_gprs_scell_disable_int_meas( gl1_defs_tn_type  ul_timeslot, gas_id_t gas_id );

extern void     gpl1_gprs_serv_cell_int_cb( gl1_defs_int_meas_result_type *int_meas_results, gas_id_t gas_id );

                    /* Circuit switched Idle metrics/data callback
                     * Used to run c value calculation and consequent tx
                     * power updating on the ccch logical channel.
                     */
extern void     gpl1_scell_cs_idle_metrics_cb( uint8    burst_number,
                                               dBx16_T  rssi, gas_id_t gas_id );

extern void     gpl1_scell_cs_idle_data_cb( boolean  decode_ok , gas_id_t gas_id );

                    /* Packet Idle metrics / data callback */
extern void     gpl1_scell_pkt_idle_metrics_cb( uint8    burst_number,
                                                dBx16_T  rssi, gas_id_t gas_id );

extern void     gpl1_scell_pkt_idle_data_cb( boolean  decode_ok, gas_id_t gas_id );

                    /* Packet Access metrics / data callback */
extern void     gpl1_scell_pkt_access_metrics_cb( uint8    burst_number,
                                                  dBx16_T  rssi, gas_id_t gas_id );

extern void     gpl1_scell_pkt_access_data_cb( boolean  decode_ok, gas_id_t gas_id );

                    /* Packet Transfer metrics / data callback */
extern void     gpl1_scell_pkt_transfer_metrics_cb( uint16   arfcn_number,
                                                    uint8    burst_number,
                                                    dBx16_T  rssi,
                                                    gas_id_t gas_id );

extern void     gpl1_scell_pkt_transfer_data_cb( gl1_defs_tn_type  timeslot,
                                                 boolean           decode_ok,
                                                 uint16                rx_qual,
                                                 gl1_defs_coding_type  coding_scheme
                                                 , gas_id_t gas_id );

                    /* C value running average filter */
extern int16    gpl1_gprs_divide_ff( int16   input_value,
                                     int32  *prev_xn,
                                     uint16  ff );

extern boolean  gpl1_gprs_serving_cell_meas( l1_mode_command_T  mode, gas_id_t gas_id );

extern void     gpl1_gprs_build_quality_report( gas_id_t gas_id );

/* Methods to access/change the EGPRS quality measurements in the
 * scell measurements structure.
 */
extern void  gpl1_egprs_scell_qual_meas_broadcast_config( uint8  bep_period, gas_id_t gas_id );

extern void  gpl1_egprs_scell_qual_meas_pacch_config( uint8  link_qual_meas_mode,
	                                                  uint8  new_dl_timeslot_alloc,
	                                                  uint8  bep_period2, gas_id_t gas_id );

extern void  gpl1_egprs_scell_qual_meas_nv_item_config( boolean  gcf_item_set, gas_id_t gas_id );

/* Method to provide the logging modules with bep statistical parameters */
extern uint16  egpl1_calc_block_statistical_params( const uint16  bep[ GL1_DEFS_FRAMES_IN_BLK_PERIOD ],
                                                    uint16       *mean_bep_ptr,
                                                    gas_id_t      gas_id );

extern void  gpl1_egprs_serv_cell_data_cb( gl1_defs_tn_type                   timeslot,
                                           const gpl1_egprs_decode_metrics_T *block_decode_metrics_ptr,
                                           boolean                            tfi_match,
                                           usf_dl_tfi_info_t*                 usf_dl_tfi_info_ptr,
                                           gas_id_t                           gas_id );

extern void  gpl1_egprs_serv_cell_qual_meas_report( egprs_chn_qual_rpt_t *qual_report_msg_ptr,
                                                    uint8                 dl_timeslot_alloc, gas_id_t gas_id );

extern void  gpl1_egprs_serv_cell_qual_meas_reset( gas_id_t gas_id );

/* Methods to enable queueing, searching and clearing of potential
 * quality reports
 */
extern void  gpl1_egprs_create_qual_meas_node( gl1_defs_tn_type  timeslot,
                                               uint8             rrbp,
											   uint8             es_p,
                                               uint16            bsn1,
                                               gas_id_t gas_id );

extern void  gpl1_egprs_search_qual_meas_queue( uint8  dl_timeslot_alloc, gas_id_t gas_id );

extern void  gpl1_egprs_empty_qual_meas_queue( gas_id_t gas_id );

extern dBx16_T*   gpl1_gprs_get_serv_cell_meas_buf( gas_id_t gas_id );
extern gpl1_egprs_qual_meas_queue_T*   gpl1_gprs_get_serv_cell_head_ptr( gas_id_t gas_id );

#endif /* GPL1_GPRS_SERVING_CELL_H */
