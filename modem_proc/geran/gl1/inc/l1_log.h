#ifndef L1_LOG_H
#define L1_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            L1 Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging L1 packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_log.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
24/03/15   nm       CR810361 Add debug crash if idle monitors stays more than 1 for mor ethan 20 frames
26/01/15   jk       CR786615: Fixed Logging issue 
24/03/14   dg       CR626860 - New log packets by adding timeslot information
19/11/14   br       CR763774 GL1 logging improvement to ease debugging
27/10/14   msa      CR740964: [DSDS LWG+G][MPSS.JO.1.0] Request to implement subscription identity for GSM Events - GERAN changes
19/08/14   pjr      CR711506 Support for AMR Partial Decode and DTX detection FR19667
17/09/14   pjr      CR725173 Added Chain ID and action type to CXM log packets
09/09/14   jk       CR697308:GL1 changes to support HLin LLin sawless feature
14/04/14   pjr      CR641577 Replaced FN with Seq Number in cxm log table
11/07/14   pjr      CR 692177 Added support for RFACCH 2.0
06/02/14   cgc      CR598057 Increase logging buffer size when o/p are also logged
11/18/13   pjr      CR563534 Fix incorrect channel type shown in CXM logs packets
24/09/13   sp       CR 449651 - Define new logpacket for Burst metrics.
27/09/13   sp       CR551580: Add NV logging of outgoing L1 messages
17/09/13   pjr      CR538893 Changes to enable CXM logging
02/07/13   pjr      CR507542 added data type and function for cxm logging
13/03/12   ws       CR 330862 - Change dedicated mode F3 level's and add sacch burst
                    metrics and compact message metrics log packets.
14/12/09   ws       Merged PACKED_POST changes from MUSTANG to resolve packed
                    struct alignment issues with loging
04/21/05   sv       Add "panic reset" error code when logging to NV
02/03/05   bm       Added support to log WCDMA SCE activities
01/26/05   sv       Add support for Frame Manager Logging.
01/24/05   pp       Changes to support AMR RATSCCH request/response event
                    logging. Changed prototype for l1_log_transmit_burst_metrics()
01/05/05   dp       Merged dedicated timeline changes for audio optimizations
                    to mainline
07/15/04   gfr      Added NCELL BCCH/SCH logging (moved from NPL1).
06/22/04   sv       Added L1 NV logging mechanism on Error Fatals.
01/28/04   gw       Changes to support reselection optimizations.
11/05/03   gfr      Support for quad-band.
05/07/03   gsc      Added AMR logging support
01/24/03   atm      Logging cleanup (Added TXLEV/TA Logging)
12/04/02   ws       Updated GPRS burst metrics log packet def's
11/25/02   mk       Updated GPRS baseline.
11/25/02   ws       Added gprs agc log packets
11/13/02   mk       Updated GPRS baseline.
09/15/02   ATM      Added support for AGC info subcommand
07/18/02   ATM      Removed l1_log_fcch/sch functions -- moved to HW level.
07/11/02   ATM      Added NCELL acq events.
05/08/02   ATM      Split Rx_Metrics into Message/Burst packets
05/02/02   JC       Changed prototype for l1_log_power_scan()
04/30/02   ATM      Enhanced l1_log_power_scan function
04/19/02   ATM      Split recieve metrics into BURST/MESSAGE pieces
04/11/02   ATM      Adding LOG_GSM_RECEIVE_METRICS_C packet
03/29/02   ATM      Added NCELL packets
02/12/02   atm      Added LOG_ON_DEMAND support
01/21/02   atm      Initial version.
===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gl1_msg.h"
#include "gl1_msg_g.h" // for gl1_msg_rx_rpt & gl1_msg_chan_type enum
#include "l1_fm.h"
#include "l1i.h"
#include "gsmdiag.h"
#include "gsmdiag_v.h"

#include "geran_eng_mode_info.h"   /* duplicate defn eng_mode_sys_band_T */

#define DEBUG_SCE_RX_LOGGING
#define L1_MSG_TRAFFIC_LOGGING
#define FRAME_MANAGER_LOGGING
#define FEATURE_GL1_VAR_LOGGING

enum {
    L1_LOG_NCELL_ACQ_START,
    L1_LOG_NCELL_ACQ_SUCCESS,
    L1_LOG_NCELL_ACQ_FAIL_FCCH,
    L1_LOG_NCELL_ACQ_FAIL_SCH,
    L1_LOG_NCELL_ACQ_RECONF_LOST
};

// Below is a subset of gl1_defs_metrics_rpt
typedef PACKED struct PACKED_POST
{
    uint32              FN;
    word                arfcn;
    uint32              rssi;
    dBx16_T             pwr_dBm_x16;
    int16               dc_offset_i;
    int16               dc_offset_q;
    int16               freq_offset;
    int16               timing_offset;
    uint16              snr;
    byte                gain_state;
} l1_log_burst_metrics_T;
extern l1_log_burst_metrics_T l1_log_gsm_agc_info;

typedef PACKED struct PACKED_POST
{
    uint32              FN;
    word                arfcn;
    uint32              rssi;
    dBx16_T             pwr_dBm_x16;
    int16               dc_offset_i;
    int16               dc_offset_q;
    int16               freq_offset;
    int16               timing_offset;
    uint16              snr;
    byte                gain_state;
    byte                aci_filter_used;
    uint32              reliabFactorQ16;
    boolean             aqpsk_detected;
    byte                timeslot;
    uint16              jdet_reading;
    uint32              WBEE;
    boolean             HLinLLinState;
} l1_log_burst_metrics_v2_T;

typedef PACKED struct PACKED_POST
{
    uint32              FN;
    word                arfcn;
    dBx16_T             pwr_dBm_x16;
    int32               dc_offset;
    int16               freq_offset;
    int16               timing_offset;
    uint16              snr;
    byte                gain_state;
} l1_log_sacch_burst_metrics_T;

typedef PACKED struct PACKED_POST
{
    uint32              FN;
    word                arfcn;
    int16              txlev;
} l1_log_transmit_burst_metrics_T;

typedef PACKED struct PACKED_POST
{
    uint32              FN;
    word                arfcn;
    int16              txlev;
    byte                TN;
} l1_log_transmit_burst_metrics_v1_T;

#ifdef FEATURE_GSM_AMR
typedef PACKED struct PACKED_POST
{
  byte old_state;
  byte new_state;
}l1_log_amr_state_change_T;

typedef PACKED struct PACKED_POST
{
  byte old_phase;
  byte new_phase;
}l1_log_amr_cmi_phase_change_T;

typedef PACKED struct PACKED_POST
{
  byte dl_acs;
  byte dl_icm;
  byte thresh_tbl[3];
  byte hyst_tbl[3];
}l1_log_amr_req_act_timer_expiry_T;

typedef PACKED struct PACKED_POST
{
  uint32 ustmr_start;
  uint32 ustmr_end;
  uint32 cxm_freq_id;
  uint16 gstmr_start;
  uint16 gstmr_end;
  uint16 arfcn;
  uint16 cxm_priority;
  uint16 power;
  uint16 tx_desense_back_off;
  uint16 tx_vbatt_back_off;
  uint8  channel_type;  
  uint8  timeslot      : 4;
  uint8  activity_type : 2;
  uint8  cxm_status    : 2; 
  uint8  channel_id; 
  uint8  action; 
} l1_cxm_result_info_T;

typedef struct
{
  byte ul_acs;
  byte ul_icm;
}l1_log_amr_ack_act_timer_expiry_T;

typedef enum
{
  L1_LOG_AMR_CONFIG_REQ_FORMAT_1,
  L1_LOG_AMR_CONFIG_REQ_FORMAT_2,
  L1_LOG_AMR_THRESH_REQ
}l1_log_amr_ratscch_req_T;

#endif /* FEATURE_GSM_AMR */

#define L1_STATE_LOG_LEN 32

typedef struct
{
   uint32 FN;
   uint8  state;
   uint8  sub_state;
   uint8  main_command;
   uint8  command;
   uint8  start_of_isr;
} l1_state_log_T;


#ifdef DEBUG_SCE_RX_LOGGING
#define SCE_RX_LOG_LENGTH 20

typedef enum {
   EVE_NONE,
   EVE_INIT,
   EVE_START_RX,
   EVE_DONE_ISR,
   EVE_DONE_TASK,
   EVE_ABORT,
   EVE_ABORT_ISR,
   EVE_ABORT_CB,
   EVE_WCDMA_INIT,
   EVE_WCDMA_START_RCV,
   EVE_WCDMA_DONE_CB,
   EVE_WCDMA_DONE_PROCESS,
   EVE_WCDMA_ABORT,
   EVE_WCDMA_ABORT_CB
} rx_event_T;

typedef struct {
   uint32      FN;
   boolean     in_ISR;
   rx_event_T  event;
   uint32      data1;
   uint32      data2;
   uint32      data3;
} rx_log_element_T;

void sce_rx_log_event( rx_event_T event,
                              uint32     data1,
                              uint32     data2,
                              uint32     data3,
                              gas_id_t gas_id );
#endif /* DEBUG_SCE_RX_LOGGING */

#ifdef L1_MSG_TRAFFIC_LOGGING
#define L1_MSG_LOG_LEN 64
#define L1_MSG_RR_L1_LOG_LEN  32 
typedef struct { 
   uint32 FN;
     uint16 msg_set;
     uint16 msg_ID;
   boolean incoming_message;
} l1_msg_traffic_log_T;
#endif

#ifdef FRAME_MANAGER_LOGGING
#define L1_FM_LOG_LEN 50

typedef struct {
   uint32 FN;
     l1_fm_priority_T fm_task;
     l1_fm_state_T fm_state;
   uint32 fm_frames_reqd;
     fm_event_T  event;
} fm_log_element_T;
#endif

#ifdef FEATURE_GL1_VAR_LOGGING

#define L1_VAR_LOG_LEN 10

typedef struct {
     uint32 FN;
     uint8  rcv:1;
     uint32 linenum;
     l1_fname_T filename;
} l1_rcv_log_T;

typedef struct {
     uint32 FN;
     uint8  mon_scan:1;
     uint32 linenum;
     l1_fname_T  filename;
} l1_monscan_log_T;

typedef struct {
     uint32 FN;
     uint8  wsrch_id:1;
     uint32 linenum;
     l1_fname_T  filename;
} l1_wsrch_id_log_T;

typedef struct {
     uint32 FN;
     uint8  wsrch:1;
     uint32 linenum;
     l1_fname_T  filename;
} l1_wsrch_log_T;

typedef struct {
     uint32 FN;
     uint8  idle_monitors:2;
     uint32 linenum;
     l1_fname_T  filename ;
} l1_idle_moniters_log_T;

#endif

typedef struct {
     l1_state_log_T l1_state_log_buf[L1_STATE_LOG_LEN];
     uint8 l1_state_log_buf_index;
     l1_panic_reset_type_T error_code;
#ifdef DEBUG_SCE_RX_LOGGING
     rx_log_element_T sce_rx_log_buf[SCE_RX_LOG_LENGTH];
     uint8 sce_rx_log_buf_index;
#endif
#ifdef L1_MSG_TRAFFIC_LOGGING
     l1_msg_traffic_log_T l1_RR_log_msg[L1_MSG_LOG_LEN];
     uint8 l1_RR_log_msg_index;
     l1_msg_traffic_log_T l1_log_msg[L1_MSG_LOG_LEN];
     uint8 l1_log_msg_index;
#endif
#ifdef FRAME_MANAGER_LOGGING
     fm_log_element_T l1_fm_log_buf[L1_FM_LOG_LEN];
     uint8 l1_fm_log_index;
#endif

#ifdef FEATURE_GL1_VAR_LOGGING
     l1_rcv_log_T l1_rcv_log_buf[L1_VAR_LOG_LEN];
     uint8 l1_rcv_log_index;
     
     l1_monscan_log_T l1_monscan_log_buf[L1_VAR_LOG_LEN];
     uint8 l1_monscan_log_index;
     
     l1_wsrch_id_log_T l1_wsrch_id_log_buf[L1_VAR_LOG_LEN];
     uint8 l1_wsrch_id_log_index;
     
     l1_wsrch_log_T l1_wsrch_log_buf[L1_VAR_LOG_LEN];
     uint8 l1_wsrch_log_index;
     
     l1_idle_moniters_log_T l1_monitors_log_buf[L1_VAR_LOG_LEN];
     uint8 l1_monitors_msg_index;
#endif

} l1_NVLog_struct_T;

// used for logging L1 debug data into NVM
extern l1_NVLog_struct_T l1_nv_log[];

#ifdef FEATURE_GL1_VAR_LOGGING
#define L1_LOG_VAR(status,fname, var_id,gas_id)      l1_var_log(status,__LINE__,fname,var_id,gas_id);
#else
#define L1_LOG_VAR(status,fname, var_id,gas_id)      ((void)(0));
#endif

#define CXM_LOG_TABLE_SIZE           30
#define CXM_LOG_UNKNOWN_VALUE        -1
/* Special "channels" definitions */
#define CXM_LOG_MONITOR              60
#define CXM_LOG_INTERFERENCE_MEAS    61
#define CXM_LOG_ACQUISITION          62
#define CXM_LOG_QBTA_GAP             63
#define CXM_LOG_INVALID              70

/* Activity Type */
#define CXM_LOG_ACTIVITY_NONE        0
#define CXM_LOG_ACTIVITY_RX          1
#define CXM_LOG_ACTIVITY_TX          2

typedef struct
{
  uint16 seq_number;
  uint16 arfcn;
  uint16 gstmr_center;
  uint16 tx_desense_back_off;
  uint16 vbatt_back_off;
  uint16 power;
  uint8  channel_type;
  uint8  tn;
  uint8  activity_type;
} cxm_log_element_T;

typedef struct
{
  cxm_log_element_T logs[CXM_LOG_TABLE_SIZE];
  uint16            write_index;
} cxm_log_table_T;

void l1_log_cxm_table_init(gas_id_t gas_id);
void l1_log_cxm_new_entry(uint16 seq_num, uint16 arfcn, uint8 channel_type,
                          int16 gstmr_center, int8 tn, 
                          uint16 tx_desense_back_off, uint16 vbatt_back_off, 
                          uint16 power, uint8 activity, gas_id_t gas_id);

void l1_log_cxm_get_entry(int8 index, uint16 *arfcn, 
                          uint8 *channel_type, uint8 *tn, 
                          uint16 *tx_desense_back_off, uint16 *vbatt_back_off, 
                          uint16 *power, gas_id_t gas_id);

/* LOG INITIALIZATION -- for log on demand */
void l1_log_init(void);

/* Convert ARFCN_T to a uint16 for logging */
uint16 arfcn_to_word ( ARFCN_T arfcn );

/* LOG PACKETS */
/* NOTE -- use a boolean return value if it can be logged on demand */
void    l1_log_power_scan             ( gas_id_t gas_id );
void    l1_log_agc_params             ( void );
void    l1_log_afc_params             ( void );
void l1_log_sacch_burst_metrics (gl1_msg_metrics_rpt *rpt_ptr[],gas_id_t gas_id );

void l1_log_transmit_burst_metrics ( gl1_msg_chan_type    channel,
                                     uint8 fn_offset,
                                     uint8 burst_spacing,
                                     uint8 num_bursts,
                                     ARFCN_T arfcn[4],
                                     int16 txlev[4],
                                     uint8  tsc,
                                     uint8  ta,
                                     gas_id_t gas_id);

void l1_log_transmit_burst_metrics_ver1( gl1_msg_chan_type    channel,
                                     uint8 fn_offset,
                                     uint8 burst_spacing,
                                     uint8 num_bursts,
                                     ARFCN_T arfcn[4],
                                     int16 txlev[4],
                                     uint8  tsc,
                                     uint8  ta,
                                     gas_id_t gas_id);

void    l1_log_message_metrics        ( gl1_msg_chan_type      channel,
                                        gl1_msg_decode_rpt    *rpt_ptr, gas_id_t gas_id );

void  l1_log_message_metrics_compact  ( gl1_msg_chan_type   channel,
                                        gl1_msg_decode_rpt *rpt_ptr, gas_id_t gas_id );

void l1_log_rfacch_metrics(gl1_msg_decode_rpt *rpt_ptr, gas_id_t gas_id );

#ifdef FEATURE_GSM_AMR
void    l1_log_message_metrics_amr    (gl1_msg_decode_rpt    *rpt_ptr,
               uint8                 codec_mode,
               uint16                amr_status,
               uint8                 dl_acs,
               uint8                 ul_acs,
               uint16                norm_c_over_i, gas_id_t gas_id);

void  l1_log_message_metrics_compact_amr  (gl1_msg_decode_rpt   *rpt_ptr,
                           uint8                codec_mode, gas_id_t gas_id);
#endif

void    l1_log_measurement_report     ( void );
void    l1_log_surrounding_cell_info  ( void );
boolean l1_log_paging_data            ( gas_id_t gas_id );
void    l1_log_calibration_values     ( void );
boolean l1_log_ncell_engine_mode      ( gas_id_t gas_id );
void    l1_log_ncell_ba_list          ( gas_id_t gas_id );
void    l1_log_ncell_acq ( byte mode, uint8 rank, ARFCN_T arfcn, dBx16_T rx_power, gas_id_t gas_id );
boolean l1_log_serving_cell_power     ( gas_id_t gas_id );
void    l1_log_tx_lev_and_ta ( byte tx_lev, byte ta, gas_id_t gas_id );

/* EVENTS */
void    l1_log_state                  ( gas_id_t gas_id );

#ifdef FEATURE_GSM_AMR
void l1_log_ratscch_in_dtx ( gas_id_t gas_id );
void l1_log_facch_in_dtx ( gas_id_t gas_id );
void l1_log_facch_and_ratscch_collision ( gas_id_t gas_id);
void l1_log_facch_and_sid_update_collision ( gas_id_t gas_id );
void l1_log_ratscch_and_sid_update_collision ( gas_id_t gas_id );
void l1_log_amr_state_change (l1_log_amr_state_change_T *data, gas_id_t gas_id);
void l1_log_amr_cmi_phase_change(l1_log_amr_cmi_phase_change_T *data, gas_id_t gas_id);
void l1_log_amr_req_act_timer_expiry(l1_log_amr_req_act_timer_expiry_T *data, gas_id_t gas_id);
void l1_log_amr_ack_act_timer_expiry(l1_log_amr_ack_act_timer_expiry_T *data, gas_id_t gas_id );
void l1_log_amr_ratscch_req(l1_log_amr_ratscch_req_T *req, gas_id_t gas_id);
void l1_log_amr_ratscch_rsp( uint8 rsp, gas_id_t gas_id );
void l1_log_amr_cmc_turnaround_time ( uint16 elapsed_frames, gas_id_t gas_id );
#endif /* FEATURE_GSM_AMR */

void l1_log_serving_aux_meas ( gas_id_t gas_id );
void l1_log_ncell_aux_meas ( gas_id_t gas_id );
void l1_log_ncell_bcch_burst_metrics(gl1_msg_metrics_rpt *rpt[], gas_id_t gas_id);
void l1_log_ncell_bcch_msg_metrics (ccch_decode_rpt *rpt, gas_id_t gas_id);
void l1_log_ncell_sch_burst_metrics(gl1_msg_metrics_rpt *rpt, gas_id_t gas_id);
void l1_log_ncell_sch_msg_metrics (sch_decode_rpt *rpt, gas_id_t gas_id);

#ifdef L1_MSG_TRAFFIC_LOGGING
void l1_log_msg_traffic(uint8 msg_set, uint8 msg_ID, uint32 FN, boolean incoming_message, gas_id_t gas_id);
#endif

#ifdef FEATURE_GL1_VAR_LOGGING
void l1_var_log( uint8 var,uint32 linenum,l1_fname_T fname,uint16 var_id,gas_id_t gas_id);
void l1_rcv_log( uint8 recive_in_progress,uint32 linenum, l1_fname_T fname,gas_id_t gas_id);
void l1_monscan_log( uint8 monscan_in_progress,uint32 linenum,l1_fname_T fname, gas_id_t gas_id);
void l1_wsrch_id_log( uint8 wcdma_srch_id_progress,uint32 linenum,l1_fname_T fname, gas_id_t gas_id);
void l1_wsrch_log( uint8 wcdma_srch_in_progress ,uint32 linenum,l1_fname_T fname, gas_id_t gas_id);
void l1_idle_monitors_log( uint8 idle_moniters ,uint32 linenum,l1_fname_T fname, gas_id_t gas_id);
#endif

void l1_log_packet_init(gsmdiag_pkt_info_t *info_ptr, log_code_type pkt_id, unsigned int pkt_size, gas_id_t gas_id);

uint8 l1_log_get_TxPa_state( gas_id_t gas_id );   /* CMAPI feature for Tx pa gain state */

void l1_log_convert_txlev_to_pwr_dBm(eng_mode_sys_band_T band, uint8 txlev, uint8 *power_out);

void l1_log_signal_info(geran_eng_mode_signal_info_t *signal_info_ptr, gas_id_t gas_id);

#ifdef FEATURE_GSM_LOG_COEX
  void l1_log_coex_params(volatile GfwCxmLogPacketBuff* cxm_log_buf_ptr, gas_id_t gas_id);
#endif

#ifdef FEATURE_GSM_AMR_PD_DTX_DETECT
void l1_log_amr_pd_dtx_burst_metrics ( gl1_msg_chan_type      channel,
                                       gl1_defs_metrics_rpt  *rpt_ptr[], 
                                       gas_id_t               gas_id );

void l1_log_amr_pd_dtx_message_metrics ( gl1_msg_decode_rpt *rpt_ptr, 
                                         uint8               codec_mode, 
                                         gas_id_t            gas_id );
#endif /* FEATURE_GSM_AMR_PD_DTX_DETECT */
#endif /* L1_LOG_H */
