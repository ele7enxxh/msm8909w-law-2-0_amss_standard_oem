#ifndef GPL1_LOG_H
#define GPL1_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            L1 Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging GPL1 packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---     ----------------------------------------------------------
11/11/14   ws      CR702510 Add support for gprs message metrics E log packet
09/09/14   jk      CR697308:GL1 changes to support HLin LLin sawless feature
24/10/13   jj      CR 497390 add LOG_GPRS_RECEIVE_BURST_METRICS_E_C
24/09/13   sp      CR449651 - Define new logpackets for Burst metrics
24/09/13   sp      CR482765 - New Log Packet - EDGE Enhanced Info.
06/05/10   dv      CR237642 - Add ASRB funtionality
14/12/09   ws      Merged PACKED_POST changes from MUSTANG to resolve packed
                   struct alignment issues with loging
01/12/08   cs      Add new include gl1_hw_debug.h
29/04/08   cs      Add the TSC and channel to the IQ log data
14/01/08   cs      Allow for separate IQ logging and saving
17/09/07   agv     Added I/Q logging to code base.
24/07/06   zyu     Added SRB hard decisions logging.
22/11/05   og      Changes to logging interfaces.
23/06/05   og      Code changes to accomodate the uplink tbf data block count
                   log packet.
18/04/05   og      Update usf_dl_tfi_info type to include filtered bep params.
28/10/04   ws      Lint tidy up
28/10/04   ws      Added EGPRS logging for message and burst metrics
28/04/04   rm      Modified input parameters for gpl1_gprs_timing_advance 
23/04/04   ws      Corrected Lint Error's and Warnings
01/04/03   ws	     Fixed compile warnings by adding l1_log.h
01/12/03   ws      Lint Cleanup
19/11/03   gfr     Update for quad-band
08/09/03   ws      Added prototypes for ptcch/d log packets
04/09/03   ws      Updates for gpl1_log_gprs_air_interface_summary packet
07/07/03   ws      Added message metrics v2 log packet
01/01/03   rm      Initial version.
===========================================================================*/

#include "geran_variation.h"
#include "comdef.h"  // for types 
#include "gl1_msg.h"
#include "gl1_msg_g.h" // for gl1_msg_rx_rpt & gl1_msg_chan_type enum 
#include "gpl1_gprs_if.h"
#include "l1_log.h"
#include "gl1_hw.h"
#include "gl1_hw_g.h"
#include "gl1_hw_debug.h"


typedef struct
{
  uint8   gprs_channel;
  dBx16_T  pr_a;
  dBx16_T  pr_b;
  dBx16_T  pr_c;
  dBx16_T  pr_d;
  dBx16_T  p0;
  dBx16_T  pb;
  uint8  tfi_usf;
}gprs_agc_info_t;

typedef PACKED struct PACKED_POST
{
    uint32              FN;
    byte                TN;
    word                arfcn;
    uint32              rssi;           
    dBx16_T             pwr_dBm_x16;
    uint16              dc_offset_i;
    uint16              dc_offset_q;
    uint16              freq_offset;
    uint16              timing_offset;
    byte                usf;
    uint16              snr;
    byte                gain_state;
    byte                mod_detected;
    byte                filter_used;
    uint16              aci_pwr_n200_khz;
    uint16              aci_pwr_0_hz;
    uint16              aci_pwr_p200_khz;
} gpl1_log_gprs_burst_metrics_T;

typedef PACKED struct PACKED_POST
{
    uint32              FN;
    byte                TN;
    word                arfcn;
    uint32              rssi;           
    dBx16_T             pwr_dBm_x16;
    uint16              dc_offset_i;
    uint16              dc_offset_q;
    uint16              freq_offset;
    uint16              timing_offset;
    byte                usf;
    uint16              snr;
    byte                gain_state;
    byte                mod_detected;
    byte                filter_used;
    uint16              aci_pwr_n200_khz;
    uint16              aci_pwr_0_hz;
    uint16              aci_pwr_p200_khz;
    uint32              reliabFactorQ16;
    uint16              jdet_reading;
    uint32              WBEE;
    boolean             HLinLLinState;
} gpl1_log_gprs_burst_metrics_ver2_T;


/* USF and DL_TFI information
 * Includes Filtered bep parameters
 * mean bep, cv bep and r for iteration
 * n.
 */
typedef struct
{
  byte               usf_match;
  byte               dl_tfi_match;
  uint16             mean_bep_tn;
  uint16             cv_bep_tn;
  uint16             r;
  gl1_defs_usf_type  usf;
} usf_dl_tfi_info_t;

/* Used to provide uplink throughput
 * statistics.
 */
typedef struct
{
  uint32  number_rlc_data_blks;
  uint32  number_dummy_ctrl_blks;
} ul_data_blk_count_t;


/* LOG INITIALIZATION -- for log on demand */
void gpl1_log_init(void);

/* LOG PACKETS */
/* NOTE -- use a boolean return value if it can be logged on demand */

void gpl1_log_gprs_burst_metrics_a ( gl1_msg_dl_chn_type channel,uint32 fn, const gl1_defs_rx_pkt_metrics_type *rpt_ptr, gas_id_t gas_id );
void gpl1_log_gprs_burst_metrics_b ( gl1_msg_dl_chn_type channel,uint32 fn, const gl1_defs_rx_pkt_metrics_type *rpt_ptr, gas_id_t gas_id );
void gpl1_log_gprs_burst_metrics_c ( gl1_msg_dl_chn_type channel,uint32 fn, const gl1_defs_rx_pkt_metrics_type *rpt_ptr, gas_id_t gas_id );
void gpl1_log_gprs_burst_metrics_d ( gl1_msg_dl_chn_type channel,uint32 fn, const gl1_defs_rx_pkt_metrics_type *rpt_ptr, gas_id_t gas_id );
void gpl1_log_gprs_burst_metrics_e ( gl1_msg_dl_chn_type channel,uint32 fn, const gl1_defs_rx_pkt_metrics_type *rpt_ptr, gas_id_t gas_id );

void gpl1_log_gprs_burst_metrics_a_ver2 ( gl1_msg_dl_chn_type channel,uint32 fn, const gl1_defs_rx_pkt_metrics_type *rpt_ptr, gas_id_t gas_id );
void gpl1_log_gprs_burst_metrics_b_ver2 ( gl1_msg_dl_chn_type channel,uint32 fn, const gl1_defs_rx_pkt_metrics_type *rpt_ptr, gas_id_t gas_id );
void gpl1_log_gprs_burst_metrics_c_ver2 ( gl1_msg_dl_chn_type channel,uint32 fn, const gl1_defs_rx_pkt_metrics_type *rpt_ptr, gas_id_t gas_id );
void gpl1_log_gprs_burst_metrics_d_ver2 ( gl1_msg_dl_chn_type channel,uint32 fn, const gl1_defs_rx_pkt_metrics_type *rpt_ptr, gas_id_t gas_id );

void gpl1_log_gprs_message_metrics_b ( gl1_msg_dl_chn_type   channel, uint32 fn,  const gl1_defs_rx_pkt_data_type *rpt_ptr, gas_id_t gas_id );
void gpl1_log_gprs_message_metrics_c ( gl1_msg_dl_chn_type   channel, uint32 fn,  const gl1_defs_rx_pkt_data_type *rpt_ptr, gas_id_t gas_id );
void gpl1_log_gprs_message_metrics_d ( gl1_msg_dl_chn_type   channel, uint32 fn,  const gl1_defs_rx_pkt_data_type *rpt_ptr, gas_id_t gas_id );

void gpl1_log_gprs_message_metrics_a_ver2 ( gl1_msg_dl_chn_type   channel, uint32 fn,  const gl1_defs_rx_pkt_data_type *rpt_ptr,
                                            const usf_dl_tfi_info_t         *usf_dl_tfi_info, gas_id_t gas_id );
void gpl1_log_gprs_message_metrics_b_ver2 ( gl1_msg_dl_chn_type   channel, uint32 fn,  const gl1_defs_rx_pkt_data_type *rpt_ptr,
                                            const usf_dl_tfi_info_t         *usf_dl_tfi_info, gas_id_t gas_id );
void gpl1_log_gprs_message_metrics_c_ver2 ( gl1_msg_dl_chn_type   channel, uint32 fn,  const gl1_defs_rx_pkt_data_type *rpt_ptr,
                                            const usf_dl_tfi_info_t         *usf_dl_tfi_info, gas_id_t gas_id );
void gpl1_log_gprs_message_metrics_d_ver2 ( gl1_msg_dl_chn_type   channel, uint32 fn,  const gl1_defs_rx_pkt_data_type *rpt_ptr,
                                            const usf_dl_tfi_info_t         *usf_dl_tfi_info, gas_id_t gas_id );
void gpl1_log_gprs_message_metrics_e_ver2 ( gl1_msg_dl_chn_type   channel, uint32 fn,  const gl1_defs_rx_pkt_data_type *rpt_ptr,
                                            const usf_dl_tfi_info_t         *usf_dl_tfi_info, gas_id_t gas_id );

void  gpl1_log_edge_message_metrics  ( gl1_msg_dl_chn_type channel, uint32 fn, byte tn,
                                       const GfwEnhancedEdgeLogPacket *edge_data, gas_id_t gas_id);
void gpl1_log_srb_hard_decisions  ( uint32 FN, uint16* HD_ptr, uint16 mcs, gas_id_t gas_id );

#ifdef FEATURE_GSM_LOG_IQ_SAMPLES
void gpl1_log_mdsp_debug_buffer( uint16 *MDSP_buffer_p, uint32 FN,
                                 uint16 TSC, uint16 channel, gas_id_t gas_id );
#endif /* FEATURE_GSM_LOG_IQ_SAMPLES */

void gpl1_log_gprs_air_interface_summary(boolean transfer_ind, gas_id_t gas_id);
void gpl1_log_agc (const sys_algo_agc_T *agc_ptr, const gprs_agc_info_t *gprs_agc_info, gas_id_t gas_id);

void gpl1_log_timing_advance ( const ta_description_T*  active_ta_params, uint8  ta_value, gas_id_t gas_id );

void gpl1_log_gprs_power_control(gl1_msg_ul_chn_type channel, gl1_defs_tn_type tn, dBx16_T tx_pwr, gl1_defs_coding_type cs, gas_id_t gas_id);
void gpl1_log_gprs_transfer_summary(gas_id_t gas_id);
void convert_txlev_to_pwr_dBm(sys_band_T band, uint8 txlev, uint8 *power_out);

/* Uplink tbf data/dummy block count */
void  gpl1_log_uplink_tbf_data_block_count( const ul_data_blk_count_t*  ul_data_blk_count_ptr, gas_id_t gas_id );


#endif /* GPL1_LOG_H */
