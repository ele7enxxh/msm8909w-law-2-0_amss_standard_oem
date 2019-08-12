#ifndef DIAG_WCDMA_CMD_H
#define DIAG_WCDMA_CMD_H
/*==========================================================================

      Diagnostic Services Packet Processing Command Code Defintions

Description
  This file contains packet id definitions for the serial interface to
  the dmss.  

Copyright (c) 1993 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History
                            
$PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagpkt_wcdma_cmd.h_v   1.4   22 Feb 2002 16:37:26   yiz  $                            
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/common/inc/wcdmadiag.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---    -----------------------------------------------------------
09/09/15   vn     Addng QXDM support to attenuate WCDMA signal
01/23/15   gv     Putting back the RACH stats code (incorrectly removed in code cleanup)
10/01/14   gv     Code cleanup. Removal of stale and unused code. 
07/01/14   hk     Header file inclusion cleanup
06/26/14   hk     DL Features Mainline
05/01/14   gsk    Changes for Version-2 of qxdm command to crash UE on unexpected search results
11/09/13   vs     CQI override changes for 3C.
07/03/13   gsk    Adding Qxdm support to crash UE on unexpected search results
07/02/13   ks     Add ability to freeze cm measurements through qxdm
10/24/12   scm    Fix unused var and duplicate define compiler warnings.
09/19/12   dp     Finished mainlining FEATURE_WCDMA_QICE_CONTROLLER
05/22/12   pr     Dime Bringup Changes
01/30/12   scm    Mainline FEATURE_L1_AUTO_BLER_MEAS.
                  Obsolete FEATURE_WCDMA_L1_MANUAL_CQI_UPD.
01/29/12   ms     Added QXDM command to set debug flag for DL datapath f3s
11/04/11   zr     Adding command to enable power profiling via QXDM
10/14/11   rl     Made changes for WCDMA Security Setting independent of NV Items.
04/20/11   vsr    Added support to call RxD APIs through QXDM command 
02/18/11   sb      Added code to support Network Meas parameters through QXDM command.
02/11/11   su     Added code to support setting UE HSDPA category 
                  through QXDM command.
11/03/10   grk    Added support to set RLC/MAC debug variables through QXDM command.
05/03/10   rvs    Add params to WCDMA_QICE_ORIDE_CONFIG_PARAMS_F.
04/22/10   rvs    Fixed KW errors 
03/08/09   mc/rgn Added DIAG command for DC-HSDPA
12/16/09   rvs    Add the WCDMA_QICE_DEBUG_ENABLE_F command params.
09/24/09   rvs    Modify QICE SW oride command params.
07/15/09   rvs    Added QICE oride commands.
05/13/09   scm    Merge in mods from 8650 branch.
03/11/09   rm     Included wenc.h because enc.h was replaced/renamed as wenc.h
06/25/08   kps    Added support for diag commands WCDMA_DIAG_WRITE_FBIBER_THRESH_F 
                  and WCDMA_DIAG_READ_FBIBER_THRESH_F
02/08/08   gnk    Added QXDM support for R99 interface
01/16/07   stk    Modified the include path to fix compiler error in 7601 build.
12/13/07   gnk    Reset RACH stats counter on overflow after printing out stats
11/30/07   mc     Added support for dumping TBset data.
10/08/07   mc     Added support for enabling MBMS debug mode
09/10/07   yh     Added support for WPLT cmds being sent through DIAG.
08/28/07   gnk    Added, renamed and modified some RACH statistic variables
08/20/07   kps    Expose dl_num_times_sfn_mismatch for diag
07/12/07   kps    Minor fix
06/13/07   kps    Fix WPLT CRM Compilation issue.
05/25/07   kps    Add support to output RACH Transmission Preamble Count.
04/24/07   kp     Added ability to output RRC counters,Namely Intra freq  cell reselection 
                  statistics, WCDMA to GSM cell reselection statistics, OOS related statistics
                  & RACH transmission related statistics.
04/20/07   kps    Display/reset reacq counters in wcdma stat functions.
                  "Paging stats" is now renamed to "wcdma stats".
04/12/07   kps    Add ability to output Paging counters.
03/28/07   scm    Featurize file with FEATURE_WCDMA to help non-WCDMA builds.
03/07/07   nd     changes for PSC scanner srch under FEATURE_WCDMA_L1_PSC_SCANNER
02/02/07   oh     Enhanced interface to control various f3 messages for EUL.
11/10/06   am     Added code for enabling/disabling Pwr restrction throu QXDM
08/11/06   ttl    Support CPU based flow control.
03/21/06   mg     Add support for Command code 22 (Sample Server data logging)
07/20/05   mc     Add support for Command code 21 (Manual CQI offset updation)
07/07/04   scm    Add debug ability to freeze and slam VCTCXO.
07/28/03   scm    Support for query of various DRX reacquisition statistics.
04/22/03   gs     Added support for Command Code 08 (BLER meas rate change).
09/17/02   gsc    Added support for Command Code 15 (wcdma_additional_status).
02/22/02   abp    Update WCDMA verno packet per Serial ICD revC.
11/02/01   yiz    Added declarations to implement Call Originate and Call End.
09/25/01   abp    Modified rsp packet for WCDMA_DIAG_VERNO_F
09/14/01   abp    Defined req and rsp packets for WCDMA_STATUS_F
04/30/01   cpe    Created the file
===========================================================================*/

#include "wcdma_variation.h"
#include "diagpkt.h"
#include "customer.h"
#include "rfm.h"

/* Wrap the file with this feature to help non-WCDMA builds. */
#ifdef FEATURE_WCDMA
#include "wenc.h"   /* renamed enc.h to wenc.h */
#include "wfw_sw_intf.h"

#include "mm.h"

/* Spec limits downlink tr block to 800 bytes so 2*800 + 20 should suffice */
#define MAX_DATA_LEN  200 /*For now */
/*#define MAX_DATA_LEN  1620 */

/* Max num of diag commands for wcdma subsystem. Can be modfied later */
#define DIAG_WCDMA_MAX_F 16

/* Max num of digits in dialed number for WCDMA call origination packet */
#define WCDMA_DIAG_MAX_WCDMA_DIGITS 32

#define WCDMA_DIAG_VERNO_F                 0
#define WCDMA_AL1_DATA_F                   1
#define WCDMA_DIAG_BLER_RATE_CHANGE_F      8
#define WCDMA_DIAG_ORIG_F                 12
#define WCDMA_DIAG_END_F                  13
#define WCDMA_STATUS_F                    14
#define WCDMA_ADDITIONAL_STATUS_F         15
#define WCDMA_DIAG_REACQ_SLEW_STATS_F     17
#define WCDMA_DIAG_REACQ_SRCH_STATS_F     18
#define WCDMA_DIAG_REACQ_CLEAR_STATS_F    19
#define WCDMA_DIAG_SET_TRK_LO_PDM_F       20
#define WCDMA_MANUAL_CQI_UPD_CONFIG_F     21
#define WCDMA_DIAG_SAMP_SERV_CONFIG_F     22
#define WCDMA_RLC_FLOW_CONTROL_CONFIG_F   23
#define WCDMA_DIAG_EUL_UL_VAR_UPD_F       24
#define WCDMA_L1_PSC_SCANNER_CONFIG_F     25
#define WCDMA_DIAG_WCDMA_STATS_F          26
#define WCDMA_DIAG_WCDMA_CLEAR_STATS_F    27
#define WCDMA_PLT_DIAG_CMDS               28
#define WCDMA_MBMS_DEBUG_F                29
#define WCDMA_DEC_TBSET_DUMP_F            30
#define WCDMA_DIAG_UL_VAR_UPD_F           31
#define WCDMA_DIAG_WRITE_FBIBER_THRESH_F  32
#define WCDMA_DIAG_READ_FBIBER_THRESH_F   33
#define WCDMA_MIMO_PARAMS_F               34
#define WCDMA_CQI_OVERIDE_PARAMS_F        35

#define WCDMA_QICE_ORIDE_TABLE_PARAMS_F   36
#define WCDMA_QICE_ORIDE_CONFIG_PARAMS_F  37
#define WCDMA_QICE_ORIDE_FHT_PARAMS_F     38
#define WCDMA_QICE_ORIDE_FW_PARAMS_F      39
#define WCDMA_QICE_ORIDE_LOG_PARAMS_F     40
#define WCDMA_QICE_RESET_F                41
#define WCDMA_CME_ORIDE_PARAMS_F          42
#define WCDMA_QICE_DEBUG_ENABLE_F         43

#ifdef FEATURE_WCDMA_DC_HSDPA
#define WCDMA_DIAG_DC_HS_SETUP            44
#endif
#define WCDMA_DIAG_RLC_DBG_F              45
#define WCDMA_DIAG_MAC_DBG_F              46

#ifdef FEATURE_WCDMA_SET_HS_CAT
#define WCDMA_DIAG_RRC_HS_CAT             47
#endif
#define WCDMA_DIAG_QUERY_MEAS_F           48
#define WCDMA_RXD_API_ORIDE_F             49
#ifdef FEATURE_WCDMA_SECURITY_SETTINGS
/*Integrity Enable/Disable*/
#define WCDMA_DIAG_INTEGRITY_ENABLE_F      50

/*Ciphering Enable/Disable*/
#define WCDMA_DIAG_CIPHERING_ENABLE_F      51

/*Fake security Enable/Disable*/
#define WCDMA_DIAG_FAKE_SECURITY_ENABLE_F  52
#endif
#define WCDMA_GPIO_PWR_PROF_F             53
#define WCDMA_DIAG_DATAPATH_DBG_F         54
#define WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F       55

#define WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F         56

#define WCDMA_ATTENUATE_SIGNAL_F         57


#define WCDMA_FW_DEBUG_CMD_F              100

#define WCDMA_DIAG_IMSI_SIZE              (sizeof(imsi_data_T))

/* Paging statistics structure. */
typedef PACKED struct{
  uint32 rrcpg1_num_pages;
  uint32 rrcpg1_num_pages_matched;
  uint32 drx_pich_demod_error;
  uint32 drx_cycle_len;
  uint32 num_drx_cycles;
} page_stats_type;

/* Data Structure to store the intra frequency reselection statistics */
typedef PACKED struct
{
  /* Intra frequency reselection success count since last power up */
  uint32 intra_freq_resel_success_cnt;
  /* Intra frequency reselection failure count due to N-BCCH set up failue since last power up */
  uint32 intra_freq_nbcch_setup_failure_cnt;
  /* Intra frequency reselection failure count due to SIB wait time out since last power up */
  uint32 intra_freq_sib_wait_timeout_cnt;
  /* Intra frequency reselection failure count due to transition failures since last power up */
  uint32 intra_freq_transition_fail_cnt;
  /* Intra frequency reselection failure count due to other causes since last power up */
  uint32 intra_freq_failed_other_causes_cnt;
}intra_freq_resel_stats_type;

/* Counters and defines to maintain statistics on reacquisition slew values
** during DRX operation.  Values can be queried by the QXDM.  All slews are
** in units of chipx8. */
typedef PACKED struct PACKED_POST
{
  uint32 srchzz_reacq_slew_small_cnt;    /* (-20...20)                   */
  uint32 srchzz_reacq_slew_med_cnt;      /* (-256...-21) or (21...256)   */
  uint32 srchzz_reacq_slew_large_cnt;    /* (-768...-257) or (257...768) */
  uint32 srchzz_reacq_slew_abnormal_cnt; /* (< -768) or (> 768)          */
} srchzz_reacq_slew_stats_type;

/* Counters to maintain statistics on successful and failed reacquisition
** searches during DRX operation.  Values can be queried by the QXDM. */
typedef PACKED struct PACKED_POST
{
  uint32 srchzz_reacq_list_success_cnt;
  uint32 srchzz_reacq_list_failure_cnt;
  uint32 srchzz_reacq_123_success_cnt;
  uint32 srchzz_reacq_123_failure_cnt;
} srchzz_reacq_srch_stats_type;

/* Data Structure to store the WTOG reselection statistics */
typedef PACKED struct
{
  /* WTOG cell reselection count since last power up */
  uint32 wtog_resel_cnt;
  /* WTOG cell reselection failure count since last power up */
  uint32 wtog_resel_failure_cnt;
}wtog_resel_stats_type;

/* Data Structure to store the OOS related statistics */
typedef PACKED struct
{
  /* Number of times OOS is triggered in idle mode since last power up */
  uint32 oos_idle_cnt;
  /* Number of times OOS is triggered in connected mode since last power up */
  uint32 oos_connected_cnt;
}oos_stats_type;

/* Data Structure to store RACH Transmission RRC Retry related statistics */
typedef PACKED struct
{
  /*Count which captures number of times RRC is successful in receiving RRC 
    procedure level response message from N/W for any RRC message sent on RACH. */
  uint32 succful_rrc_tran_retry_cnt ;
  /*Count which captures number of times RRC Retry Failed due L2 ACK IND 
    with cause failure in sending RRC message on RACH transport channel */
  uint32 rrc_retry_fail_due_to_l2_ack_ind;
  /*Counter is incremented when RRC exhausts with max time re-transmission
    before receiving RRC procedure level response message on RACH */
  uint32 rrc_retry_fail_due_to_others;
}rach_transmission_stats_type;

/* Data Structure to store RACH Transmission Preamble related statistics */
typedef PACKED struct
{
  /* Indicates the total number of ACKs received for 0-8 preambles or above. */
  uint32   rach_ack_prmbl_cnt[PREAMBLE_CNT_MAX_BINS];
  /* Indicates the total number RACH procedures resulting in No_ACK */
  uint32   rach_noack_cnt;
  /* Indicates the maximum number of preambles attempted in NAK case */
  uint8    rach_nak_prmbl_max_cnt;
  /* Indicates the maximum number of preambles attempted in RACH procedure. */
  uint8    rach_max_prmbl_cnt;
  /* Indicates the total number of RACH attempts that the UE made */
  uint32   rach_total_attempts;
  /* Indicates the maximum Tx power used for RACH procedure in ACK or NACK case. */
  int16   rach_prmbl_detected_max_pwr_dB;
  /* Indicates the average Tx power used for RACH procedure in ACK or NACK case. */
  int16   rach_detected_last_prmbl_avg_pwr_dB;
  /* Indicates the average Tx power used for RACH procedure in NoACK case. */
  int16   rach_noack_last_prmbl_avg_pwr_dB;
  /*Indicates the maximum Tx power used for RACH procedure in NoACK case. */
  int16   rach_noack_prmbl_max_pwr_dB;
}rach_transmission_preamble_stats_type;


/************************************************************************** 
    PACKET DEFINITIONS
**************************************************************************/

/* WCDMA version  Diag Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_VERNO_F)
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_VERNO_F)
 /* command code */
//-LD  uint16  cmd_code;
  /* WCDMA class mark */
  //uint8 class_mark;
  /* Rev number of WCDMA standard  4 MSBs are used to set
     to represent year of release 0 - 1999  1 - 2000 etc
     4 LSBs will be set to the month */
  //uint8 g3gpp_rev;
  /* Release num, from 3 up */
  //uint8 g3gpp_rel;
  /* Vocoder DSP version num, 0 ...0xffff */
  uint16  vdsp_ver;
  /* MDSP version number(first part) 16 bits */
  uint16  mdsp_ver_rom;
  /* MDSP version number(second part) 16 bits */
  uint16  mdsp_ver_ram;
  /* Indicates if DRX is used for cell updating */
  //uint8 drx_index;
  /* MSM version ( first part) */
  //uint8 msm_ver_maj;
  /* MSM version ( second part) */
  //uint8 msm_ver_min;
  //uint8 model_num;
  /* WCDMA model number */
DIAGPKT_RSP_END


/* WCDMA Diag L1 BLER meas rate change packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_BLER_RATE_CHANGE_F)
 /* BLER meas duration in ms */
 uint16 bler_meas_duration_ms;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_BLER_RATE_CHANGE_F)
DIAGPKT_RSP_END


/* WCDMA Diag L1 DRX reacquisition slew statistics packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_REACQ_SLEW_STATS_F)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_REACQ_SLEW_STATS_F)
  /* Counts of the various slew values observed during DRX reacquisition. */
  srchzz_reacq_slew_stats_type srchzz_reacq_slew_stats;
DIAGPKT_RSP_END

/* WCDMA Diag L1 DRX reacquisition search statistics packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_REACQ_SRCH_STATS_F)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_REACQ_SRCH_STATS_F)
  /* Counts of successful and failed reacquisitions, both list and 1/2/3. */
  srchzz_reacq_srch_stats_type srchzz_reacq_srch_stats;
DIAGPKT_RSP_END

/* WCDMA Diag L1 DRX reacquisition clear all statistics packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_REACQ_CLEAR_STATS_F)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_REACQ_CLEAR_STATS_F)
DIAGPKT_RSP_END

/* WCDMA Diag L1 set trk_lo_adj PDM value
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_SET_TRK_LO_PDM_F)
  /* What value should we force the PDM to. */
  uint16 tcxo_pdm_val;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_SET_TRK_LO_PDM_F)
DIAGPKT_RSP_END

/* WCDMA Diag Call Origination Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_ORIG_F)
 /* DM shall set it to the number of valid digits in DIGITS field */
 uint8  num_digits;

 /* Dialed digits; ASCII chars; 0-9, #, * only */
 uint8  digits[WCDMA_DIAG_MAX_WCDMA_DIGITS];

 /* Rate of the vocoder: 0 ~ 7 for
    4.75, 5.15, 5.90, 6.70, 7.40, 7.95, 10.2, 12.2 kbps
  */
 uint8  amr_rate;
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_ORIG_F)
DIAGPKT_RSP_END

/* WCDMA Diag Call End Request Packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_END_F)
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_END_F)
DIAGPKT_RSP_END

/* Packet data exchanged between QXDM and Abstract l1 (AL1) running on SURF
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_AL1_DATA_F)
 /* length of the downlink data */
  uint16 data_len ; 

 /* downlink data as a char array */
 uint8  data[MAX_DATA_LEN] ; 
DIAGPKT_REQ_END

 /* The response packet is defined below.
 */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_AL1_DATA_F)
 /* length of the uplink data */
 uint16 data_len ;

 /* uplink data as a char array */
 /* uint8  data[MAX_DATA_LEN] ; */
 char  data[MAX_DATA_LEN] ;

DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    WCDMA TMC STATUS
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_STATUS_F) 
DIAGPKT_REQ_END

 /* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_STATUS_F)
 /* TMC state */
 uint8 tmc_state;
 
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    WCDMA ADDITIONAL STATUS
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_ADDITIONAL_STATUS_F) 
DIAGPKT_REQ_END

 /* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_ADDITIONAL_STATUS_F)
 /* International Mobile Equipment ID */
 uint8 imei[9];
 /* International Mobile Subscriber ID */
 uint8 imsi[WCDMA_DIAG_IMSI_SIZE];
 /*l1_state*/
 uint8 l1_state;
 
DIAGPKT_RSP_END


#ifdef FEATURE_WCDMA_MIMO
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_MIMO_PARAMS_F)
  uint32   mimo_param_type;
  uint32   mimo_param_val;
DIAGPKT_REQ_END

 /* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_MIMO_PARAMS_F)
   uint8  status;
DIAGPKT_RSP_END
#endif

#define WCDMA_CQI_TEST_PATTERN_LEN  32
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_CQI_OVERIDE_PARAMS_F)
  uint8   override;
  uint8   pattern_len;
  uint8   repetition_cycle_repeat_count;
  uint8   test_pattern;
DIAGPKT_REQ_END

 /* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_CQI_OVERIDE_PARAMS_F)
   uint8  status;
DIAGPKT_RSP_END

#ifdef FEATURE_WCDMA_SAMPLE_SERVER_DATA_LOGGING
#error code not present
#endif /* FEATURE_WCDMA_SAMPLE_SERVER_DATA_LOGGING */

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
/*---------------------------------------------------------------------------
    WCDMA ADDITIONAL STATUS
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_RLC_FLOW_CONTROL_CONFIG_F) //tliou
   uint16 ws_step_period;
   uint16 ws_step_stage;
   uint16 init_ws;
DIAGPKT_REQ_END

 /* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_RLC_FLOW_CONTROL_CONFIG_F)
   uint8  status;
DIAGPKT_RSP_END
#endif /* FEATURE_CPU_BASED_FLOW_CONTROL */

#ifdef FEATURE_WCDMA_HSUPA
/*---------------------------------------------------------------------------
    WCDMA EUL UPLINK SUBSYSTEM VARIABLE UPDATE
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_EUL_UL_VAR_UPD_F)
  uint8  debug_enable_f3_message_type; /* Type of f3 message to be enabled from QXDM*/
  uint32 debug_var_val; /* bitmask to be used*/
DIAGPKT_REQ_END

/* Response packet for EUL UL VAR update request*/
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_EUL_UL_VAR_UPD_F)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END
#endif /*end of FEATURE_WCDMA_HSUPA*/

/*---------------------------------------------------------------------------
    WCDMA SKIP CM MEASUREMENTS VARIABLE UPDATE
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F)
  uint8 debug_var_val; /* enable/disable skip_cm_measurements */
DIAGPKT_REQ_END

/* Response packet for SKIP CM MEAS VAR update request*/
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END

#ifdef FEATURE_WCDMA_DC_HSDPA
/*---------------------------------------------------------------------------
    WCDMA DC HSDPA SETUP
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_DC_HS_SETUP)
  uint8   enable; /* enable/disable dc hs setup*/
  uint16  dc_psc; /* dc frequency*/
  uint16  dc_freq; /* dc psc */
DIAGPKT_REQ_END

/* Response packet for EUL UL VAR update request*/
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_DC_HS_SETUP)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END
#endif

/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_UL_VAR_UPD_F)
  uint8  debug_enable_f3_message_type; /* Type of f3 message to be enabled from QXDM*/
  uint32 debug_var_val; /* bitmask to be used*/
DIAGPKT_REQ_END

/* Response packet for UL VAR update request*/
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_UL_VAR_UPD_F)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END

#ifdef FEATURE_MBMS
#error code not present
#endif 

#ifdef FEATURE_WCDMA_TBSET_DEBUG_DUMP
#error code not present
#endif

#ifdef FEATURE_WCDMA_L1_PSC_SCANNER
/* WCDMA Diag PSC scanner On/OFF packet
 */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_L1_PSC_SCANNER_CONFIG_F)
/* search cmd to ON or OFF */
  uint8 mview_cmd;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_L1_PSC_SCANNER_CONFIG_F)
  /* status of the REQ cmd if it was a success or a failure */
  uint8  status;
  /* sending back the requested cmd in the response. */
  uint8  mview_cmd;
DIAGPKT_RSP_END
#endif  /* #ifdef FEATURE_WCDMA_L1_PSC_SCANNER */


/* WCDMA Diag wcdma statistics packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_WCDMA_STATS_F)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_WCDMA_STATS_F)
  /* Counts of the various slew values observed during DRX reacquisition. */
  srchzz_reacq_slew_stats_type      srchzz_reacq_slew_stats;
  /* Counts of successful and failed reacquisitions, both list and 1/2/3. */
  srchzz_reacq_srch_stats_type      srchzz_reacq_srch_stats;
  /* Number of times SFN mismatch is detected */
  uint32                            dl_num_times_sfn_mismatch;
  /* Various paging statistics */
  page_stats_type                   page_stats;
  /* Various Intra freq cell reselection statistics */
  intra_freq_resel_stats_type   intra_freq_resel_stats;
  /* Various WCDMA to GSM cell reselection statistics */
  wtog_resel_stats_type    wtog_resel_stats;
  /* Various OOS related statistics */
  oos_stats_type  oos_stats;
  /* Various RACH transmission related statistics */
  rach_transmission_stats_type rach_transmission_stats;
  /* Data Structure to store RACH Transmission Preamble related statistics */
  rach_transmission_preamble_stats_type   rach_transmission_preamble_stats;
DIAGPKT_RSP_END

/* WCDMA FW debug cmd packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_FW_DEBUG_CMD_F)
  uint32 debug_pkt[WFW_SW_DEBUG_CMD_NUM_W32];
DIAGPKT_REQ_END
/* WCDMA FW debug cmd packet response */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_FW_DEBUG_CMD_F)
  uint8  status;
  uint32 debug_pkt[WFW_SW_DEBUG_CMD_NUM_W32];
DIAGPKT_RSP_END

/* WCDMA Diag clear wcdma statistics packet
 */

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_WCDMA_CLEAR_STATS_F)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_WCDMA_CLEAR_STATS_F)
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_WRITE_FBIBER_THRESH_F)
  uint16 fbiBER_threshold;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_WRITE_FBIBER_THRESH_F)
  uint8 status;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_READ_FBIBER_THRESH_F)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_READ_FBIBER_THRESH_F)
  uint8 status;
  uint16 fbiBER_threshold;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_QICE_ORIDE_TABLE_PARAMS_F)
  uint32 carrier;
  uint32 tanh_table[4];
  uint32 sqrt_table_1[7];
  uint32 sqrt_table_2[5];
  uint32 param_table[3];
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_QICE_ORIDE_TABLE_PARAMS_F)
  uint8  status;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_QICE_ORIDE_CONFIG_PARAMS_F)
  uint32 carrier;
  uint32 commit_margin_cx2;
  uint32 sfc_energy_thresh;
  uint16 sfc_hw_sum_alpha;
  uint16 sfc_ratio_alpha;
  uint16 sfc_ratio_thresh;
  uint8 mmse_weight_diag[4];
  uint8 mmse_weight_cross[4];
  uint8 noise_inflation;
  uint8 iterations;
  uint8 bank_switch_delay;
  uint8 sfc_bpg_per_dump;
  uint8 sfc_dump_per_ratio;
  uint8 sfc_ratio_per_classify;
  uint8 sf16_active_subframes;
  uint8 idle_windows;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_QICE_ORIDE_CONFIG_PARAMS_F)
  uint8  status;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_QICE_ORIDE_FHT_PARAMS_F)
  uint32 carrier;
  uint32 fht256[8];
  uint16 fht16;
  uint16 psc;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_QICE_ORIDE_FHT_PARAMS_F)
  uint8  status;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_QICE_ORIDE_FW_PARAMS_F)
  uint32 carrier;
  uint16 psc;
  uint16 misc_sf16_overrides;
  uint16 sf16_non_linear_override;
  uint16 sf16_non_linear_select;
  uint16 sf16_mode_override;
  uint16 sf16_mode_select;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_QICE_ORIDE_FW_PARAMS_F)
  uint8  status;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_QICE_ORIDE_LOG_PARAMS_F)
  uint32 carrier;
  uint8 hw_dump_per_log; 
  uint8 primary_ovsf;
  uint8 non_primary_ovsf;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_QICE_ORIDE_LOG_PARAMS_F)
  uint8  status;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_QICE_RESET_F)
  uint32 carrier;
  uint8 reset_bmsk;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_QICE_RESET_F)
  uint8  status;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_CME_ORIDE_PARAMS_F)
  uint32 carrier;
  uint8 max_num_qset_cells;
  uint8 qset_ecio_add_threshold;
  uint8 qset_ecio_drop_threshold;
  uint8 qset_drop_ctr_threshold;
  uint8 qset_metric_threshold;
  uint8 qset_primary_ctr_threshold;
  uint8 pset_leak;
  uint8 num_locked_fings_window_size;
  uint8 scch_success_rate_window_size;
  uint8 min_avg_num_locked_fings_no_rxd;
  uint8 min_avg_num_locked_fings_rxd;
  uint8 min_avg_scch_success_rate;
  int16 max_tx_agc_val;
  uint8 disable_qice_timer_init_val;
  uint8 enable_qice_timer_init_val;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_CME_ORIDE_PARAMS_F)
  uint8  status;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_QICE_DEBUG_ENABLE_F)
  uint32 carrier;
  uint8 enable;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_QICE_DEBUG_ENABLE_F)
  uint8  status;
DIAGPKT_RSP_END

#ifdef FEATURE_WCDMA_RX_DIVERSITY
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_RXD_API_ORIDE_F)
  uint8 api_identifier;
  uint32 input_arg_1;
  uint32 input_arg_2;
  uint32 input_arg_3;
  uint32 input_arg_4;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_RXD_API_ORIDE_F)
  uint8  status;
DIAGPKT_RSP_END

#endif

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_GPIO_PWR_PROF_F)
  uint8 enabled;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_GPIO_PWR_PROF_F)
  uint8 status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    WCDMA L2 SUBSYSTEM VARIABLE UPDATE
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_RLC_DBG_F)
  uint8  debug_var_type; /* Type of f3 message to be enabled from QXDM*/
  uint32 debug_var_val; /* bitmask to be used*/
DIAGPKT_REQ_END

/* Response packet for L2 VAR update request*/
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_RLC_DBG_F)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END

/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_MAC_DBG_F)
  uint8  debug_var_type; /* Type of f3 message to be enabled from QXDM*/
  uint32 debug_var_val; /* bitmask to be used*/
DIAGPKT_REQ_END

/* Response packet for L2 VAR update request*/
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_MAC_DBG_F)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END


/*---------------------------------------------------------------------------
    WCDMA RRC SUBSYSTEM VARIABLE UPDATE
---------------------------------------------------------------------------*/


#ifdef FEATURE_WCDMA_SET_HS_CAT
/*---------------------------------------------------------------------------
    WCDMA RRC HS CAT
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_RRC_HS_CAT)
  uint8   hs_cat; /* HSDPA CAT to be set*/
DIAGPKT_REQ_END

/* Response packet for RRC HS CAT */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_RRC_HS_CAT)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END
#endif

/* Request packet for measurement query */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_QUERY_MEAS_F)
DIAGPKT_REQ_END

/* Response packet for measurement query */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_QUERY_MEAS_F)
  /* Response status */
  uint8  valid;
  /* Version number of this response */
  uint8 version;
  int8 rscp;
  uint8 ec_io;
  uint16 scr_code;
  uint16 freq;
DIAGPKT_RSP_END

#ifdef FEATURE_WCDMA_SECURITY_SETTINGS
/*---------------------------------------------------------------------------
    WCDMA RRC Security
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_INTEGRITY_ENABLE_F)
  uint8   enable; /* Integrity to be set*/
DIAGPKT_REQ_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_INTEGRITY_ENABLE_F)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END
  
  /* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_CIPHERING_ENABLE_F)
  uint8   enable; /* Ciphering to be set*/
DIAGPKT_REQ_END

/* Response packet*/
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_CIPHERING_ENABLE_F)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END
  
  /* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_FAKE_SECURITY_ENABLE_F)
  uint8   enable; /* Fake security to be set*/
DIAGPKT_REQ_END

/* Response packet*/
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_FAKE_SECURITY_ENABLE_F)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END
#endif

/*---------------------------------------------------------------------------
    WCDMA L1 SUBSYSTEM VARIABLE UPDATE
---------------------------------------------------------------------------*/
/* Request packet */
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DIAG_DATAPATH_DBG_F)
  uint8  debug_var_type; /* Type of f3 message to be enabled from QXDM . nOTE: only one type of variable here though*/ 
  uint16 debug_var_val; /* bitmask to be used*/
DIAGPKT_REQ_END

/* Response packet for L2 VAR update request*/
DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DIAG_DATAPATH_DBG_F)
  /* Response status */
  uint8  status;
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
    WCDMA L1 SUBSYSTEM VARIABLE UPDATE
---------------------------------------------------------------------------*/

typedef struct {
  diagpkt_subsys_header_v2_type header; /* Sub System header */
  uint8   status;   /* Status from the wcdma_cmd_status_type */
  uint32  value;     /* Value retrieved by wcdma cmd */
} DIAG_SUBSYS_WCDMA_WCDMA_STATUS_rsp_type;

/*---------------------------------------------------------------------------
    WCDMA L1 SUBSYSTEM VARIABLE UPDATE
---------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F)
  uint8 l1m_state;
  uint8 bmask;
  uint8 carr_idx;
  uint8 type_of_search;
  uint8 thresh;
  uint16 freq;
  uint16 psc;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F)
  uint8 status;
DIAGPKT_RSP_END

  
DIAGPKT_SUBSYS_REQ_DEFINE(WCDMA, WCDMA_ATTENUATE_SIGNAL_F)
  uint8 attenuate_value;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCDMA, WCDMA_ATTENUATE_SIGNAL_F)
  uint8 status;
DIAGPKT_RSP_END

#endif  /* #ifdef FEATURE_WCDMA */

#endif  /* #ifndef DIAG_WCDMA_CMD_H */
