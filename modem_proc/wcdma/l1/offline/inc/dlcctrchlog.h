#ifndef DLCCTRCHLOG_H
#define DLCCTRCHLOG_H

/*===========================================================================
                     D L C C T R C H L O G . H

DESCRIPTION
  This header file describes the CcTrCh log packet definitions

Copyright (c) 2001-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dlcctrchlog.h_v   1.9.1.0   18 Jun 2002 17:54:56   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dlcctrchlog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
11/13/14    kr      Corrt JO featurization 
11/10/14    sl      Code changes for new f/w log packet 0x41C4 V1
10/23/14    sl      Changes for 0x0423 Log Packet version 7
06/26/14    hk      DL Features Mainline
06/06/14    pr      Enhancement for x4023 log pkt v6
03/25/14    srk     0x4023 log packet support for HS only TPC featue.
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
12/27/13    pr      Added enhancement to 0x4778 Log Packet
09/19/13    sad     Added support for secondary carrier logging for 0x4198
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
05/23/13    geg     WSW decoupling (except for component of QCHAT)
09/06/12    sr      New log pkt 0x4198 WCDMA_ULTPC_DEBUG_PKT
04/01/13    hk      Mainlined FEATURE_WCDMA_EFDPCH
12/14/12    sad     Added NUM_TPC_GROUPS macro
06/29/12    sr      Created new version of 0x4023
12/03/12    pr      Added changes for 0x4778 Log Packet.
01/26/12    vs      Mainlining FEATURE_L1_LOGGING_ENABLED & FEATURE_L1_POWER_CTR_PKT_WINDUP_MODE
                    Deleting FEATURE_TRUNCATED_CRC_LOG_PKT & FEATURE_LOG_TPC_ACCUM
01/19/12    zr      Mainlined FEATURE_MCAL_WCDMA 
06/23/11    dm      Added enhanced power control log packet(0x4023) support
04/28/11    sj      upmerge from offline/main
04/26/11    ks      Added ILPC (0x4774) logging support
03/14/11    ms      Mainlined FEATURE_6275_COMPILE
03/11/11    dp      Ported CPC DRX changes to offline Architecture
05/13/10    ks      Max number of fingers restrictred to 12 for CLTD logging
04/06/10    ks      Added Enhanced FDPCH support
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
05/08/09    sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m.h, wsrch.h, wsrchi.h, wsrchlog.h
10/21/08    mc      Set CRC packet num samples only for old DL.
10/03/08    hk      Bringing in 7k Mailine fixes
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
02/21/07    ms      Added F-DPCH logging support.
01/10/07    vr      Revised CLTD log packet contents for MAP-AV algorithm (0x401F)
09/28/06    mc      Featurized decoder log packets under the 
                    feature FEATURE_MCAL_WCDMA.
06/01/06    mc      Featurized the usage of new log code 0x4217 using mDSP
                    exported define of 8 bit TF Id field change
06/01/06    mc      Temporary version to support new log code 0x4217.
03/16/06    asm     Corrected Featurization, Indentation and date.
03/16/06    asm     Added support for logging TPC bits from multiple combiners
08/31/05    yiz     Cleanup power control log pkt featurization
08/23/05    vb      Added tpc field to cltd logging struct
06/22/05    sk      Added bit-field macros for windup and windown for
                    FEATURE_L1_POWER_CTR_PKT_WINDUP_MODE
05/16/05    vb      Addition of field cltd_override_hist to cltd log pkt
02/22/05    vb      Added CLTD log packet
12/28/04    gs      Changed log code of DL_TBSET_STATUS_PKT to Base+0x20C
                    when feature for mDSP 6275 interface is defined
03/25/04    scm     Change DL_TRCH_CM_BLER_LOG_PKT from 0x4775 to 0x4013.
03/12/04    gs      Added max_tfci_less_than_64 to TFCI decode log packet
                    Changed the fields in TFCI decode log packet to uint16 to
                    make it replica of what is in mDSP.
11/23/03    asr     Removed accidental definition of FEATURE_SPIKE_MITIGATION from this file.
11/21/03    yus     Add FEATURE_SPIKE_MITIGATION.
02/27/03    gs      Defined CM BLER log packet under feature
                    FEATURE_CM_BLER_LOGGING
02/04/03    yus     Changed power ctr log code to 0x4010.
                    Added the support for number of rejected TPC bits in power
                    control logpacket.
12/08/02    gs      Changed log code for power control log packet from WCDMA
                    LOG BASE + 0x730 to 0x001
09/30/02    gs      Added the support for TPC valid bit due to compressed mode
                    in power control log packet under feature
                    FEATURE_WCDMA_L1_PWR_CTRL_PKT_W_CM_INFO.
                    
07/31/02    gs      1. Changed the power control log packet structure member
                       tx_gain_adj to tx_accum
                    2. Defined the feature FEATURE_WCDMA_L1_PWR_CTRL_PKT_NEW
                       under which power control log packet code is changed
                       to new one and it has addition structure member
                       tx_rate_adj_in
                       
06/18/02    gs      1. Added the definition for L1 TPC accumulation log packet

02/28/02    gs      1. Added the DL TBset status log packet definition

02/04/02    gs      1. Added the slot format in Power control packet.
                    2. Changed the tx  gain adj from unsigned to signed.
                    
09/27/01    gs      1. Changed the BLER log packet definition

09/17/01    gs      1. Corrected the log codes.

09/07/01    gs      1. Merged the 1.4.1.0 for power control log packet.
                    2. Added the header definition for bler log packet.
                    
08/24/01    gs      Added the L1 power control log packet definition.
06/07/01    gs      Optimized the log packet for their size

04/20/01    gs      1. Added the DL TrCh BLER log packet
                    2. Changed the log code macro definition to use log code base.
                    
04/19/01    gs      Added the include file l1rrcif.h
01/17/01    gs      Created this file                    
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "qw.h"
#include "log.h"
#include "l1rrcif.h"
#include "wdec.h"
#include "dltri.h"
#include "log_codes_wcdma_int.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define L1_PWR_CTRL_NUM_SAM 50
#define L1_FDPCH_DEBUG_NUM_SAM 25
#define L1_MULTI_COMB_TPC_NUM_SAM 50
#define L1_CLTD_NUM_SAM     10
#define L1_NUM_TPC_COMBINERS   6
#ifndef L1_CLTD_PKT
#define L1_CLTD_PKT L1_CLTD_0x401F_LOG_PKT
#define NUM_AV_COMBINING_GROUPS 12
#endif

/* Max number of fingers for CLTD logging */
#define MAX_NUM_FINGERS_CLTD_LOGGING 12 

#define NUM_SLOTS   15
#define NUM_TPC_GROUPS   6

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Header definition since the header definition is defined in log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} l1_log_hdr_struct_type;

typedef l1_log_hdr_struct_type dl_trch_log_hdr_struct_type;
typedef l1_log_hdr_struct_type dl_trch_bler_log_hdr_struct_type;
typedef l1_log_hdr_struct_type l1_pwr_ctrl_log_hdr_struct_type;
typedef l1_log_hdr_struct_type l1_pwr_ctrl_sync_log_hdr_struct_type;
typedef l1_log_hdr_struct_type l1_ultpc_debug_log_hdr_struct_type;
typedef l1_log_hdr_struct_type l1_fdpch_debug_log_hdr_struct_type;
typedef l1_log_hdr_struct_type l1_multi_comb_tpc_log_hdr_struct_type;
typedef l1_log_hdr_struct_type l1_cltd_log_hdr_struct_type;
typedef l1_log_hdr_struct_type dec_tbset_status_log_hdr_struct_type;
typedef l1_log_hdr_struct_type dl_dp_metrics_log_hdr_struct_type;

#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
typedef l1_log_hdr_struct_type wcdma_timeline_profiling_log_hdr_struct_type;
#endif /* FEATURE_WCDMA_JOLOKIA_MODEM */

#ifdef FEATURE_WCDMA_CPC_DRX
typedef l1_log_hdr_struct_type wl1_dl_drxctrl_log_hdr_struct_type;
#endif

/* DL TrCh BLER info log packet related definitions */
/* ------------------------------------------------ */

typedef PACKED struct PACKED_POST
{
  uint8 trch_id;   /* TrCh Id of the TrCh */
  uint32 crc_rece; /* Number of TrBlk/CRC received */
  uint32 crc_err;  /* Number of TrBlk with CRC check failed */
} dl_trch_log_bler_info_struct_type;

LOG_RECORD_DEFINE(DL_TRCH_BLER_LOG_PKT)
  uint8 num_trch; /* number of the TrCh reporting BLER stats */
  uint16 calc_win_size; /* BLER win size in frames (10 ms) */
  dl_trch_log_bler_info_struct_type trch_log_bler_info[L1_MAX_TRCH];
LOG_RECORD_END

/* L1 forward and reverse power control packet definition */
/* ------------------------------------------------------ */
/* MSB or Bit-15 is windup and Bit-14 is winddown*/
#define PC_LOG_WINDUP_MODE 0x8000
#define PC_LOG_WINDOWN_MODE 0x4000

typedef PACKED struct PACKED_POST
{
  /* Power control variable related to D/L */
  /* ------------------------------------- */

  /* variables required for DPCCH SIR measurement */
  uint16 wt_nt_io;
    uint32 wt_traffic_amp;
  
    /* DL outer loop set point */
    uint16 outerloop_set_pt;

    /* Forward (DL) power control decision history */
    uint16 fpc_decision_hist;
  
    /* Power control variable related to U/L */
    /* ------------------------------------- */
    
    /* Tx Accum */
    int16 tx_accum;
    /* reverse (UL) decision history */
    uint16 rtpc_decision_history;
  
    int16 tx_rate_adj_in;
  
    /* indicate the FPC dicision valid for each slot */
    uint16 tpc_decision_valid;
  
    /*Indicates the RTPC decision validity for each slot based on 0/1/DSDx/Unattempted*/
    uint32 rtpc_decision_valid;

    /* Number of rejected TPC bits for combiner 0 and 1 */
    uint8 num_reject_tpc_01;
    /* Number of rejected TPC bits for combiner 2 and 3 */
    uint8 num_reject_tpc_23;
    /* Number of rejected TPC bits for combiner 4 and 5 */
    uint8 num_reject_tpc_45;

    /* Tr blks received */
    uint8 num_trblk_recvd;
  
    /* Tr blks in error */
    uint8 num_trblk_err;
  
    /* accumulated dpcch energy */
    int32 avg_dpch_eng;
  
    /*Bmsk representing internal FW params*/
    uint8 fw_internal_pc_bmsk;

    /*
    All the below fields are packed in one uint8
    [7:4] bits Reserved
    [3:3] (bit #3) for DL in_sync decision
    [2:2] (bit #2) for DL out_sync decision
    [1:1] (bit #1) to indicate PA ON/OFF
    [0:0] (bit #0) - Reserved
    */
    uint8 sltfmrt_isync_osync_paon;

   /*
    All the below fields are packed in one uint8
    [6:7] (bit #6,7) reserved for future use
    [4:5] (bit #4,5) to indicate HS Only TPC mode
    [0:3] (bit #0,1,2,3) - number of FDPCH active slots
    */
    uint8 fdpchactiveslots_hstpcmode;

  } l1_power_ctrl_sync_log_info_struct_type;
  
  LOG_RECORD_DEFINE(WCDMA_POWER_CTRL_SYNC_LOG_PKT)
    uint8 version; 
    uint8 num_sample;
    uint8 start_cfn;
    /* Linear value of Q_IN */
    int16 q_in_ph1;
    int16 q_in_ph2;
    /* Linear value of Q_OUT */
    int16 q_out;
    uint16 slot_pilot_energy;
    uint16 wt_pilot_amp;
    uint8 num_carriers;

    /*
    [5-7] Reserved
    [4:1] bits for Slot Format
    [0:0] (bit #0) for FDPCH Enabled
           1 - Yes 0 - No 
    */
    uint8 sfmt_fdpch_enabled;
    l1_power_ctrl_sync_log_info_struct_type power_ctrl_sync_log_info[WL1_MAX_UL_CARR_IDX][L1_PWR_CTRL_NUM_SAM];
  LOG_RECORD_END

/* Multiple-combiner Power Control Log Packet for PCA2 */
/* L1 reverse power control packet definition */
/* ------------------------------------------------------ */
typedef PACKED struct PACKED_POST
{
  /* indicate the FPC dicision valid for each slot */
  uint16 tpc_decision_valid;

  /* Reverse (UL) power control decision history */
  uint32 tpc_decision_hist[L1_NUM_TPC_COMBINERS];
  uint32 tpc_overall_decision_hist;
} l1_multi_comb_tpc_log_info_struct_type;

LOG_RECORD_DEFINE(L1_MULTI_COMB_TPC_DC_PKT)
  uint8 version;
  uint8 num_sample;
  uint8 start_cfn;
  uint8 num_carriers;
  l1_multi_comb_tpc_log_info_struct_type multi_comb_tpc_log_info[WL1_MAX_UL_CARR_IDX][L1_MULTI_COMB_TPC_NUM_SAM];
LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  /* F-DPCH enabled or disabled on each finger */
  uint16 fdpch_fing_en;
  /* STTD enabled or disabled on each finger */
  uint16 fdpch_fing_sttd_en;
  /* FPC decode decisions valid or not for each finger */
  uint16 fpc_fdpch_valid;
  /* TPC combiner values */
  uint16 tpc_combs[L1_NUM_TPC_COMBINERS];
} wl1_fdpch_fing_debug_log_packet_info_struct_type;

LOG_RECORD_DEFINE(L1_FDPCH_DEBUG_PKT)
  uint8 num_sample;
  uint8 start_cfn;
  /* fingFDPCHEnState   *
   * fingFDPCHSttdState *
   * fpcFDPCHValid      *
   * tpcCombs[0]        *
   * tpcCombs[1]        *
   * tpcCombs[2]        *
   * tpcCombs[3]        *
   * tpcCombs[4]        *
   * tpcCombs[5]        */
  wl1_fdpch_fing_debug_log_packet_info_struct_type fdpch_debug_log_info[L1_FDPCH_DEBUG_NUM_SAM];
LOG_RECORD_END

/* L1 CLTD packet definition */
/* ------------------------------------------------------ */

typedef PACKED struct PACKED_POST
{
  /* CLTD init mode */
  uint16 pa_disable;
  uint16 cltd_init_counter;
  uint16 cltd_init_state_timer;

  /* CLTD override mode */
  uint16 cltd_override_hist;
  uint16 cltd_override_counter;

  /* FBI Tx bits */
  uint16 fbi_decision_hist;
  uint16 fbi_decision_valid;
  uint16 assigned_fing_bmsk;

  /* Non-AV w1 weights */
  uint16 w1i_nonAV_hist[MAX_NUM_FINGERS_CLTD_LOGGING];
  uint16 w1q_nonAV_hist[MAX_NUM_FINGERS_CLTD_LOGGING];

  /* AV w1 weights */
  uint16 w1i_AV_hist[MAX_NUM_FINGERS_CLTD_LOGGING];
  uint16 w1q_AV_hist[MAX_NUM_FINGERS_CLTD_LOGGING];

  /* To log tpc index per fing */
  uint16 tpc[MAX_NUM_FINGERS_CLTD_LOGGING];
} l1_cltd_log_info_struct_type;

LOG_RECORD_DEFINE(L1_CLTD_PKT)
  uint8 num_sample;
  uint8 start_cfn;
  l1_cltd_log_info_struct_type cltd_log_info[L1_CLTD_NUM_SAM];

  /* Metrics for the CLTD MAP Antenna Verification algorithm */
  uint16 pdlq[NUM_AV_COMBINING_GROUPS]; /* Downlink channel quality metric */
  uint16 pjtq[NUM_AV_COMBINING_GROUPS]; /* Joint Uplink and Downlink channel quality metric */
  uint16 omega[NUM_AV_COMBINING_GROUPS]; /* Amplitude (A) metric */
  int16 threshold[NUM_AV_COMBINING_GROUPS]; /* Threshold metric */

LOG_RECORD_END

/* L1 TPC accumulation log packet */
/* ------------------------------ */


LOG_RECORD_DEFINE(DL_TRCH_CM_BLER_LOG_PKT)
  uint8 num_trch; /* number of the TrCh reporting BLER stats */
  uint16 calc_win_size; /* BLER win size in frames (10 ms) */
  dl_trch_log_bler_info_struct_type trch_log_cm_bler_info[L1_MAX_TRCH];
LOG_RECORD_END


typedef PACKED struct
{
  int16 scaled_TPC_per_group[NUM_TPC_GROUPS];
  int16 scaled_DP_per_TPC_group[NUM_TPC_GROUPS];
  int16 scaled_NtIo_per_TPC_group[NUM_TPC_GROUPS];
  int16 mdsp_sir_A_val;
  int16 mdsp_sir_B_val;
  int16 mdsp_sir_tgt_val;
  int16 mdsp_fpc_dec_val;
  int16 dp_power;
  int16 dp_noise;
  int16 data_power;
  int16 data_noise;
  uint8 data_bits;
  uint8 tpc_cmd;
}l1_dp_metric_log_struct_type;

LOG_RECORD_DEFINE(WCDMA_SLOT_LEVEL_ILPC_LOG_PACKET)
  uint8 version;
  uint8 start_cfn;
  uint8 num_carriers;
  uint16 null_slots;
  uint16 sid_slots;
  uint16 full_slots;
  uint16 dcch_slots;
  uint8 dtch_class1_decision;
  l1_dp_metric_log_struct_type info[WL1_MAX_UL_CARR_IDX][NUM_SLOTS];
LOG_RECORD_END

/* WCDMA_ULTPC_DEBUG_PKT 0x4198 */
typedef PACKED struct PACKED_POST
{
  /* DL DPCCH slot format used to calculate the DPCCH SIR */
  uint8 slot_fmt;
  /* DP Eng of this TPC group */
   uint32 DP_Eng_Per_Frame[DL_TRI_MAX_RL_SETS];
  /* NtIo Eng of this TPC group*/
  uint32 NtIo_Per_Frame[DL_TRI_MAX_RL_SETS];
  /* Up rejection threshold of this TPC group */
  int16 UTHR_Per_Frame[DL_TRI_MAX_RL_SETS];
  /* Down rejection threshold of this TPC group */
  int16 DTHR_Per_Frame[DL_TRI_MAX_RL_SETS];
} l1_wcdma_ultpc_debug_log_info_struct_type;

  LOG_RECORD_DEFINE(WCDMA_ULTPC_DEBUG_PKT)
    uint8 version; 
    uint8 num_sample;
    uint8 start_cfn;
    uint8 num_carriers;
    l1_wcdma_ultpc_debug_log_info_struct_type ultpc_debug_log_info[WL1_MAX_UL_CARR_IDX][L1_PWR_CTRL_NUM_SAM];
  LOG_RECORD_END

#ifdef FEATURE_WCDMA_JOLOKIA_MODEM

/* Log packet to capture WFW time-line violations*/
typedef PACKED struct PACKED_POST
{
   /* Number of time-line violations per event */
   uint8 count_per_event[WCDMA_NUM_TIMELINE_CHECKS];
   
} wcdma_timeline_violation_debug_log_info_struct_type;
  
LOG_RECORD_DEFINE(WCDMA_TIMELINE_CHECKS_LOG_PACKET)
   uint8 version; 
   uint8 approx_cfn;

   /*  Approx WSTMR interpretation
   [0-10] - Unknown
   [11-18] - BPG 
   [19 -30] - Frame	
   [31] - Unknown
   */
   uint32 approx_wstmr;
   uint32 approx_ustmr;
   uint32 approx_pcycles;
   wcdma_timeline_violation_debug_log_info_struct_type wcdma_timeline_debug_log_info;
LOG_RECORD_END

#endif /* FEATURE_WCDMA_JOLOKIA_MODEM */
#endif
