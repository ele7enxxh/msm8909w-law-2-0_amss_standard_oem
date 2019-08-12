#ifndef ULLOG_H
#define ULLOG_H

/*===========================================================================
                              U L L O G . H

DESCRIPTION
  This header file describes the UL CcTrCh and Physical Channel log packet
  definitions

Copyright (c) 2000 - 2005, 2007 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/ullog.h_v   1.13   29 May 2002 11:37:40   msatyana  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/ullog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
11/19/14    vr      Deprecating and removing 0x4312 log packet code.
06/19/14    ash     Backed out code for 0x410E Log Pkt
05/23/13    geg     WSW decoupling (except for component of QCHAT)
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/19/12    vr      HSRACH code update
06/08/12    gv      Feature HS RACH Initial code check in
02/01/12    raj     Nikel Feature cleanup.
02/01/12    gnk     Nikel Feature cleanup.
01/27/12    vs      Nikel feature cleanup.
10/20/11    gnk     Removed MTD related code for NikeL
04/28/11    asm     Mainlined FEATURE_WCDMA_TXAGC_DB10 on offline targets
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
03/30/11    gnk     Mainlined several features
02/23/11    asm     Cleanup of unused, mainlined or irrelevant features in offline UL code
01/17/11    sp      Added support for UL_RACH_DEBUG_LOG_PKT (0x4025)
01/17/11    sp      Featurized UL RACH FW Log Pkt (0x4026) under FEATURE_WCDMA_GENESIS_SW_FW
01/07/11    sp      Added support for new UL RACH FW Log Pkt (0x4026)
                    Removed support for 0x4180, 0x4181, 0x4182
11/19/10    sp      Optimized R99 UL Data Dump Log Pkt implementation
11/19/10    gnk     Modifying the log packet to print dBm values in 0x4192
10/27/10    gnk     Incorporating review comments for MTD
09/20/10    sp      Added R99 UL Data Dump Log packet
09/15/10    gnk     Fixes for UL open loop MTD
09/02/10    gnk     Checking in the changes for UL open loop MTD feature
11/18/09    gv      Use db10 instead of db12 units for computation
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
02/17/09    ka      Moved UL_TRCH_CODING_LOG_BUFF_SIZE macro definition.
02/13/09    ka      Fixed concurrency issue in processing UL_TRCH_CODING_LOG_PKT
10/03/08    hk      Bringing in 7k Mailine fixes
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
10/15/07    gnk     Added Prach log packets & F3 msgs temporarily
03/16/07    am      4312 logpkt changes
08/23/05    asm     Modifications for new common UL TFC PL log packet 0x4163
                    Also some optimizations to UL TFC PL implementation
05/10/05    yiz     Change Saber UL TFC PL log code for APEX support
                    Add Raven UL TFC PL log packet support
10/05/04    yiz     Add UL_DPCH_PWR_INFO_LOG_PKT.
09/30/04    yiz     Add UL_TFC_PWR_ELMN_LOG_PKT for UL TFC PL.
08/02/04    yiz     Add UL_RACH_RAM_DUMP_LOG_PKT for RACH debugging.
12/08/03    yiz     Add rf_tx_pwr_dbm field in RACH params log packet.
07/24/02    sk      Removed FEATURE_WCDMA_AI_SUM_LOG.
05/29/02    m       Added support for LOD by introducing DPCH and PRACH
                    structures to save snapshot of DPCH and PRACH log packets
04/23/02    yiz     Appended last_ai_sum field to RACH_PROC_LOG under
                    FEATURE_WCDMA_AI_SUM_LOG.
                    Renamed access_slot_no to subchan_no in UL RACH PROC log.
09/17/01    yiz     Fixed definition of log codes due to LOG_WCDMA_BASE_C
                    defined as 0x4000 instead of expected 0x4100.
09/14/01    sk/yiz  Exchanged fields sf and num_trchs per ICD in 0x410E.
                    Changed size of sf_min from 16 bits to 8 bits in 0x410D.
                    Changed pre_rm_bits size to 16 bits,SF to 8 bits (0x410E).
                    Changed init_tx_pwr from uint8 to int8 (in 0x410C).
                    Added tfci (in 0x410E).
09/12/01    sk/yiz  Added init_tx_pwr field in UL_PHYCHAN_DPCH_LOG_PKT.
07/20/01    sk      Added UL Log header declaration.
06/12/01    sk      Re-organized UL DPCH and RACH status log packets.
04/23/01    sk      Added number of TrCHs to UL_TRCH_CODING_LOG_PKT
04/23/01    sk      Fixed mismatches in log packet names.
04/20/01    sk      Updated log code to use LOG_WCDMA_BASE_C.
04/19/01    sk      Fixed errors in RACH procedure parameters.
04/04/01    sk      Created this file
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "customer.h"
#include "qw.h"
#include "comdef.h"
#include "log.h"
#include "log_codes.h"
#include "l1rrcif.h"
#include "l1const.h"
#include "eullog.h"
#include "log_codes_wcdma_int.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define L1_LOG_SAMPLES_MAX  8  /* 1 * 8 samples is max Tx message data size */

/* Log header type declaration */
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
} ul_trch_log_hdr_struct_type;


/* Log header type declaration */
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
} ul_log_hdr_struct_type;





/* ---------------------------------------- */
/* UL RACH Debug Log Packet                               */
/* ---------------------------------------- */

#define MAX_RACH_DEBUG_SAMPLES  10

/* Bit Masks for multi-field values */
#define PREAMBLE_PWR_STEP_BMSK  0x0F
#define ASC_NUM_SHIFT_BMSK      4

#define AICH_TT_BMSK            0x01
#define MAX_PREAMBLE_SHIFT_BMSK 0x01

#define RACH_MSG_LEN_BMSK       0x0F
#define AICH_STATUS_SHIFT_BMSK  0x04

#define SIG_USED_BMSK           0x0F
#define LAST_SUBCHAN_SHIFT_BMSK 0x04

#define EAI_DECODE_STATUS_BMSK        0x0F
#define EAI_DECODE_STATUS_SHIFT_BMSK  0x04
#define HSRACH_STATUS_BMSK            0x0F

#define TRCH_TYPE_BMSK                0x01
#define E_TTI_BMSK                    0x01
#define E_TTI_SHIFT_BMSK              0x01
#define INIT_SERV_GRANT_SHIFT_BMSK    0x02

#define NUM_COMM_EDCH_RSRC_SHIFT_BMSK 0x01
#define EAI_CONFIGURED_BMSK           0x01

#define EAI_CONFIGD_BMSK              0x01
#define EAI_ACCESS_SLOT_BMSK          0x0F
#define EAI_ACCESS_SLOT_SHIFT_BMSK    0x01

/* RACH config params */
typedef PACKED struct PACKED_POST
{  
  uint16    avail_sig_mask;     /* 16 bit mask of available signatures (0..15)*/
  uint16    avail_sub_chan;     /* 16 bit mask of available subchannels */
  uint8     asc_num_and_preambl_pwrstep;   /* Access Service Class: bits 4-7
                                            * Preamble power ramp step: bits 0-3*/
  uint8     max_preamble_tx_and_aich_tt;   /* Max number of preambles (1..64):bits 1-7 
                                            * Aich timing: bit 0 */
  int8      cpich_tx_pwr;       /* PrCPICH Tx Pwr */
  int8      ul_if;              /* UL Interference */
  int16     cpich_rscp_pwr;     /* CPICH RSCP Pwr */
  int8      rach_init_tx_pwr;   /* Preamble Initial Tx Pwr */
  int8      const_val;          /* Const used in calc of init preamble pwr */
  uint8     first_sub_chan;     /* start subchannel num */
  uint8     TFCI;
  uint16    rach_cfg_sfn;    /* SFN when rach is config */
  uint16    btf_delay_value;
  #ifdef FEATURE_WCDMA_HS_RACH
  int8     pwroffset_pp_m;  /* Power offset between preamble and message */
  int8     max_tx_power;
  #endif /* FEATURE_WCDMA_HS_RACH */
}ul_rach_debug_cfg_info_struct_type;

/* RACH status params */
typedef PACKED struct PACKED_POST
{
  uint8     aich_stat_and_msg_len; /* RACH Message Length - 10 or 20 ms (bits 0-3) 
                                    * AICH status (bits 4-7): NO_ACK(0)/ACK(1)/NAK(2)/ABORT(3) (bits 4-7)*/ 
  uint8     num_preambles;       /* Number of preambles transmitted */
  int8      rach_last_tx_pwr;    /* Tx Pwr of Last Transmitted Preamble */
  int8      rach_msg_tx_pwr;     /* Tx pwr at which rach msg is transmitted */
  int16     ec_io;
  uint8     ref_fing;
  uint8     last_subchan_and_sig_used; /* Signature of the last transmitted preamble: 0-15 
                                        * Subchanel/access slot number of the 
                                        * last transmitted preamble: 0-15 */
  uint16    serv_cell_psc;        /* psc of primary serving cell */
  uint16    sfn_at_rach_done;     /* SFN when last preamble was transmitted */
  uint32    aiRssi;
  int32     last_ai_sum;           /* soft demodulated value of last AI */
  int32     aich_timing_err;      /* Timing Error on AICH */
  int32     tx_timing_err;        /* Timing Error on RACH */
  int16     xo_freq_error;
}ul_rach_debug_status_info_struct_type;

#ifdef FEATURE_WCDMA_HS_RACH
/* RACH status params */
typedef PACKED struct PACKED_POST
{
  uint8  eaistatus_hsrachstatus;
  uint8  eai_edch_rsrc_indx;
  uint8  eai_signature;
  uint8  num_common_edchrsrc_and_eaicfg;
  int16  init_dpch_pwr;  
}ul_hsrach_log_info_struct_type;
#endif /* FEATURE_WCDMA_HS_RACH */

/* RACH Debug Log Sample Struct  */
typedef PACKED struct PACKED_POST
{
  ul_rach_debug_cfg_info_struct_type rach_cfg_info;        /* RACH Configuration Summary */
  ul_rach_debug_status_info_struct_type rach_status_info;  /* RACH Final Status Summary */
}ul_rach_debug_log_info_sample_struct_type;

LOG_RECORD_DEFINE(UL_RACH_DEBUG_LOG_PKT)
  /* version number of this log packet */
  uint8 version;
  #ifdef FEATURE_WCDMA_HS_RACH
  uint8 num_rach_debug_samples;
  uint16 num_hsrach_debug_samples;
  #else
  uint32 num_rach_debug_samples;
  #endif
  ul_rach_debug_log_info_sample_struct_type rach_debug_info_sample [MAX_RACH_DEBUG_SAMPLES];
  #ifdef FEATURE_WCDMA_HS_RACH
  ul_hsrach_log_info_struct_type hsrach_debug_info_sample[MAX_RACH_DEBUG_SAMPLES];
  #endif
LOG_RECORD_END

#ifdef FEATURE_WCDMA_HS_RACH
/* Transport Channel Type, EUL TTI, INITIAL Serving GRANT
 * Preamble to Msg Poweroffset, Preamble Step Size,
 * Max Num Common EDCH Resources, EAI Configured (TRUE/FALSE)
 * Additional EDCH Backoff Timer
 * Max CCCH Allocation Timer
 * Max AGCH Collision Resolution Timer
 * EDCH Tx Continuation Backoff Timer
 * Avail Sig Mask, Avail Subchan Mask,
 * Max number of preambles, Max Tx Pwr (MTPL)
*/
typedef PACKED struct PACKED_POST
{
   uint8  trch_type_and_etti_and_init_serv_gra;
   uint8  preamble_step_size;
   uint8  max_pream_num;
   uint8  aich_timing;
   int8   pwr_offset_ppe;
   int8   max_tx_pwr;  
   uint8  num_comm_edch_rsrc_and_eaicfg;
   uint8  addtnl_edch_backoff_timer;
   uint8  maxCCCHAllocation_timer;
   uint8  maxAGCHCollResolution_timer;
   uint8  edchTxContBackoff_timer;
   uint16 avail_sig_mask;
   uint16 avail_subchan_mask;
} ul_hsrach_config_log_info_struct_type;

/* UL HSRACH configuration log packet.*/
LOG_RECORD_DEFINE(UL_HSRACH_CONFIG_LOG_PKT)
  uint8 version;
  ul_hsrach_config_log_info_struct_type ul_hsrach_config_log_info;
LOG_RECORD_END

#define UL_HSRACH_CONFIG_LOG_PKT_SIZE() (sizeof(UL_HSRACH_CONFIG_LOG_PKT_type))

/* Common EDCH Rsrc Index,
 * HICH OVSF code 
 * HICH Signature
 * RGCH Signature, UL slot format
 * S_Offset
 * UL Scrambling Code
 */
typedef PACKED struct PACKED_POST
{
   uint8 comm_edch_rsrc_idx;
   uint8 s_offset;
   uint8 rgch_enable;
   uint8 hich_sig_idx;
   uint8 rgch_sig_idx;
   uint8 fdpch_ovsf_code;
   uint8 hich_ovsf_code;
   uint8 ul_scr_code_type;
   uint32 ul_scr_code;
} ul_hsrach_alloc_edch_rsrc_log_info_struct_type;

/* UL RACH Allocated EDCH Resource log packet.
   Log Packet Id: 0x4195 */
LOG_RECORD_DEFINE(UL_HSRACH_ALLOC_EDCH_RSRC_LOG_PKT)
 uint8 version;
 ul_hsrach_alloc_edch_rsrc_log_info_struct_type ul_hsrach_alloc_edch_rsrc_log_info;
LOG_RECORD_END

#define UL_HSRACH_ALLOC_EDCH_RSRC_LOG_PKT_SIZE() (sizeof(UL_HSRACH_ALLOC_EDCH_RSRC_LOG_PKT_type))

/* S_Offset, EAI decode Access Slot
 * Tau FDPCH (bpgs)
 * STMR time at AICH decode
 * STMR time at activation of FDPCH, UL DPCH, EAGCH, EHICH, ERGCH
 * Offset of AGCH, RGCH, HICH from FDPCH in BPGs
 * Offset of DPCCH from REF time (PCCPCH)
*/
typedef PACKED struct PACKED_POST
{
   uint8 s_offset;
   uint8 eai_access_slot;
   uint8 tau_fdpch;   
   uint8 agch_to_fdpch_offset;
   uint8 hich_to_fdpch_offset;
   uint8 rgch_to_fdpch_offset; 
   uint16 dpcch_to_reftime_offset;
   uint32 aichdecode_stmr_time;
   uint32 fdpch_act_stmr_time;
   uint32 ul_dpcch_act_stmr_time;
   uint32 eagch_act_stmr_time;
   uint32 ehich_act_stmr_time;
   uint32 ergch_act_stmr_time;
} ul_hsrach_chan_timing_log_info_struct_type;

/* UL RACH Allocated EDCH Resource log packet.
   Log Packet Id: 0x4196 */
LOG_RECORD_DEFINE(UL_HSRACH_CHAN_TIMING_LOG_PKT)
  uint8 version;
  ul_hsrach_chan_timing_log_info_struct_type ul_hsrach_chan_timing_log_info;
LOG_RECORD_END

#define UL_HSRACH_CHAN_TIMING_LOG_PKT_SIZE() (sizeof(UL_HSRACH_CHAN_TIMING_LOG_PKT_type))

/* ---------------------------------------- */
/*  HSRACH Preamble Status Log Packet       */
/* ---------------------------------------- */

typedef PACKED struct PACKED_POST
{
  uint8  eul_tti;                         /* RACH Message Length - 10 or 20 ms */
  uint8  num_preambles;                   /* Total number of preambles transmitted during the procedure */
  uint8  aich_status;                     /* AICH status: NO_ACK(0)/ACK(1)/NAK(2) */
  uint8  last_ai_signature;               /* Signature of the last transmitted preamble: 0-15 */
  uint8  last_preamble_subchan_no;        /* Subchanel/access slot number of the last transmitted preamble: 0-15 */
  int8  last_preamble_pwr;
  uint8  eai_access_slot_and_eai_configd; /* last power setting of access preamble or message in MDSP */
  uint8 eai_signature;
  uint8 eai_status_and_hsrach_status;
  uint16 eai_edch_rsrc_indx;
  int8 init_dpch_pwr;
  uint32 sfn;                             /* SFN when last preamble was transmitted */
  uint32 last_ai_sum;                     /* soft demodulated value of last AI, reserved as Qualcomm internal */

}ul_hsrach_preamble_status_log_info_struct_type;

/* UL RACH Procedure Parameters log packet.
   Log Packet Id: 0x4197 */
LOG_RECORD_DEFINE(UL_HSRACH_PREAMBLE_STATUS_LOG_PKT)
  uint8 version;
  ul_hsrach_preamble_status_log_info_struct_type ul_hsrach_preamble_status_log_info;
LOG_RECORD_END

#define UL_HSRACH_PREAMBLE_STATUS_LOG_PKT_SIZE() (sizeof(UL_HSRACH_PREAMBLE_STATUS_LOG_PKT_type))

#endif /* FEATURE_WCDMA_HS_RACH */

/* ---------------------------------------- */
/* UL RACH Parameters log packet definition */
/* ---------------------------------------- */

#ifndef UL_RACH_PROC_LOG_PKT

#define UL_RACH_PROC_LOG_PKT UL_RACH_PROC_0x4160_LOG_PKT

#else
#error "Redefinition of the log command code: UL_RACH_PROC_LOG_PKT (0x4106)"
#endif

/* RACH preamble access attempt information */
typedef PACKED struct PACKED_POST
{
  /* RACH Message Length - 10 or 20 ms */
  uint8  msg_len;
  /* Total number of preambles transmitted during the procedure */
  uint8  num_preambles;
  /* AICH status: NO_ACK(0)/ACK(1)/NAK(2) */
  uint8  aich_stat;
  /* Signature of the last transmitted preamble: 0-15 */
  uint8  signature;
  /* Subchanel/access slot number of the last transmitted preamble: 0-15 */
  uint8  subchan_no;
  /* last power setting of access preamble or message in MDSP */
  int16  last_pwr;
  /* SFN when last preamble was transmitted */
  uint32 sfn;
  /* AICH TT */
  uint8  aich_tt;
  /* soft demodulated value of last AI, reserved as Qualcomm internal */
  uint32 last_ai_sum;

  /* RF TX power in dBm */
  int8   rf_tx_pwr_dbm;
 
} ul_log_rach_proc_info_struct_type;


/* UL RACH parameters log packet.
   Log Packet Id: 0x4106 */
LOG_RECORD_DEFINE(UL_RACH_PROC_LOG_PKT)
  /* RACH procedure summary */
  ul_log_rach_proc_info_struct_type rach_proc_log_info;
LOG_RECORD_END

#define UL_RACH_PROC_LOG_PKT_SIZE() (sizeof(UL_RACH_PROC_LOG_PKT_type))

/* ---------------------------------------- */
/* UL TRCH Info log packet definition */
/* ---------------------------------------- */

typedef PACKED struct PACKED_POST
{
  /* TrCH Id: 1 - 8 (allows for fewer than 8 TrCHs to be present) */
  uint8  trch_id;
  /* TrCH type: DCH(1) / RACH(0) */
  uint8  trch_type;
  /* Coding rate (Conv 1/2 - 1, Conv 1/3 - 2, Turbo - 3) */
  uint8  coding_rate;
  /* Num CRC bits (0 - 0, 8 - 1, 12 - 2, 16 - 3, 24 - 4) */
  uint8  num_crc;
  /* TTI format (10 - 0, 20 - 1, 40 - 2, 80 - 3) */
  uint8  tti;
  /* RM attribute: 1 - 256 */
  uint16 rm_attr;
} ul_log_semi_static_parms_struct_type;

typedef PACKED struct PACKED_POST
{
  /* Id of this TFC */
  uint8 tfc_id;
  /* Control channel gain */
  uint8 betaC;
  /* Data channel gain */
  uint8 betaD;
  /* Indicate of the GF is signalled or calculated */
  uint8 sig_flg;
} ul_log_gf_struct_type;

/* UL CCTRCH Info log packet.
   Log Packet Id: 0x4109 */
LOG_RECORD_DEFINE(UL_CCTRCH_LOG_PKT)
  /* Number of TrCHs (1-8) */
  uint8 num_trch;
  /* Semi-static parameters info */
  ul_log_semi_static_parms_struct_type semi_static_parms_log_info[L1_UL_TRCH_MAX];
  /* Total number of transport formats (1-64) */
  uint8 num_tf;
  /* gain factors info */
  ul_log_gf_struct_type gf_log_info[L1_UL_TFC_MAX];
LOG_RECORD_END

/* ---------------------------------------- */
/* UL PHYCHAN Info log packet definition */
/* ---------------------------------------- */
/* Uplink DPCH Power Control Info */
typedef PACKED struct PACKED_POST {
  /* DPCCH power offset in dB (-164 .. -6 in steps of 2) */
  int16  dpcch_pwr_offset;
  /* Power Control Preamble length in slots {0, 15} */
  uint8  pcp_len;
  /* Power Control Algorithm {1,2} */
  uint8  pca;
  /* TPC step size {1,2} in dB - optional field, valid only if PCA is 1 */
  uint8  tpc_step_size;
  /* TFCI present flag (TRUE 1, FALSE 0) */
  uint8  tfci_present;
  /* # FBI bits in DPCCH {0, 1,2} - valid only if num_dpdch = 1 */
  uint8  fbi_len;
  /* uplink puncturing limit (.4..1, in steps of .04) * 100 */
  uint8  punct_lim;
  /* Initial transmission power in dBm */
  int8   init_tx_pwr;
  /* DPCCH slot format 0..5 */
  uint8  dpcch_slot_fmt;
} ul_log_dpcch_info_struct_type;

/* UL DPCH Info */
typedef PACKED struct PACKED_POST {
  /* DPCH power control information */
  ul_log_dpcch_info_struct_type dpch_pwr_ctrl_info;
  /* short (0) or long (1) SC codes */
  uint8  scrambling_code_type;
  /*  Scrambling Code value (0..777215) */
  uint32 scrambling_code;
  /* Lowest allowed spreading factor */
  /* SF = 4,8,16,32,64,128,256 maps to 0,1,2,3,4,5,6 */
  uint8  sf_min;
} ul_log_dpch_cfg_struct_type;

/* UL DPCH Phychan Info log packet.
   Log Packet Id: 0x410C */
LOG_RECORD_DEFINE(UL_PHYCHAN_DPCH_LOG_PKT)
  /* Maximum allowed Tx power in dBm (-50..33) */
  int8  max_tx_pwr;
  ul_log_dpch_cfg_struct_type  ul_dpch_log_info;
LOG_RECORD_END

#define UL_PHYCHAN_DPCH_LOG_PKT_SIZE() (sizeof(UL_PHYCHAN_DPCH_LOG_PKT_type))

#ifdef FEATURE_L1_LOG_ON_DEMAND
typedef struct
{
  /* snapshot of common dl trCH setup parameters */
  UL_PHYCHAN_DPCH_LOG_PKT_type    channel_log;

  /* flag to determine snapshot has atleast one valid value */
  boolean                         valid;
} ul_phychan_dpch_log_on_demand_struct_type;
#endif

/* Uplink PRACH configuration */
typedef PACKED struct PACKED_POST {
  /* 16 bit mask of available signatures (0..15) */
  uint16   avail_sig_mask;
  /* 12 bit mask of available sub-channels (0..11) */
  uint16   avail_sub_chan_mask;
  /* PRACH minimum spreading factor for use in transmission */
  uint8   sf_min;
  /* scrambling code number (PRACH SC type is always long) 0..15 */
  uint8    sc_num;
  /* uplink puncturing limit (.4..1, in steps of .04) * 100 */
  uint8    punct_lim;
  /* TrCH id (1..32) to be used with PRACH xmit (dynamic ?) */
  uint8    trch_id;
  /* PRACH preamble power ramp step when no ACK (P0 1..8 in dB) */
  int8     pwr_offset_P0;
  /* Maximum number of preamble transmissions allowed (1..64) */
  uint8    max_preamble_tx;
} ul_log_prach_cfg_struct_type;

/* UL PRACH Phychan Info log packet.
   Log Packet Id: 0x410D */
LOG_RECORD_DEFINE(UL_PHYCHAN_PRACH_LOG_PKT)
  /* Maximum allowed Tx power in dBm (-50..33) */
  int16  max_tx_pwr;
  ul_log_prach_cfg_struct_type  ul_prach_log_info;
LOG_RECORD_END

#define UL_PHYCHAN_PRACH_LOG_PKT_SIZE() (sizeof(UL_PHYCHAN_PRACH_LOG_PKT_type))

#ifdef FEATURE_L1_LOG_ON_DEMAND
typedef struct
{
  /* snapshot of common dl trCH setup parameters */
  UL_PHYCHAN_PRACH_LOG_PKT_type   channel_log;

  /* flag to determine snapshot has atleast one valid value */
  boolean                         valid;
} ul_phychan_prach_log_on_demand_struct_type;
#endif

/* Double buffer size for coding log pkt */
#define UL_TRCH_CODING_LOG_BUFF_SIZE 2

/* Coding parameters per TrCH in a TFC */
typedef PACKED struct PACKED_POST
{
  /* TrCh Id */
  uint8   trch_id;
  /* Parameters that change only with TFCs (logged every 10ms for now) */
  /* Nx,j - pre-rate matched bits for TrCH x in TF j */
  uint16  pre_rm_bits;
  /* Eplus */
  uint32  eplus;
  /* Eminus */
  uint32  eminus;

  /* Parameters that change every 10 ms */
  /* Eini*/
  uint32  eini;
  /* # bits to be punctured or repeated */
  int32   delta_Nij;
} ul_log_trch_rm_parms_struct_type;

typedef PACKED struct PACKED_POST
{
  /* TFCI Value */
  uint8   tfci;
  /* # TrCHs */
  uint8   num_trchs;
  /* Spreading Factor (changes with TFCI) */
  uint8   sf;
  /* Rate Matching parameters (for upto 8 TrCHs) */
  ul_log_trch_rm_parms_struct_type  ul_log_rm_info[L1_UL_TRCH_MAX];
} ul_log_trch_coding_parms_struct_type;

#define  UL_TFC_PWR_ELMN_TFCI_BMSK              0x3F  /* bits 0-5, decimal 63 */
#define  UL_TFC_PWR_ELMN_AGC_ACCUM_FREEZE_BMSK  0x40  /* bit 6 */
#define  UL_TFC_PWR_ELMN_CM_STATUS_BMSK         0x80  /* bit 7 */

typedef PACKED struct PACKED_POST {
  /* ALL fields are for previous OTA frame */
  uint8  ul_tx_info;
  uint16  max_supportable_rateAdj[SLOTS_PER_FRAME];
} ul_tfc_pwr_elmn_sample_struct_type; /* 31 bytes per sample */

#define UL_TFC_PWR_ELMN_SAMPLE_MAX 50
LOG_RECORD_DEFINE(UL_TFC_PWR_ELMN_LOG_PKT)
  uint8 num_samples;
  uint8 start_cfn;
  ul_tfc_pwr_elmn_sample_struct_type ul_tfc_pwr_elmn_samples[UL_TFC_PWR_ELMN_SAMPLE_MAX];
LOG_RECORD_END


#define UL_DPCH_PWR_INFO_TFCI_BMSK              0x3F  /* bits 0-5, decimal 63 */
#define UL_DPCH_PWR_INFO_CM_STATUS_BMSK         0x80  /* bit 7 */
#define UL_DPCH_PWR_INFO_BC_SHIFT_MSK           4     /* bits 4-7 */
#define UL_DPCH_PWR_INFO_BD_BMSK                0x0F  /* bits 0-3 */

typedef PACKED struct PACKED_POST {
  /* UL_DPCH_PWR_INFO_TFCI_BMSK, UL_DPCH_PWR_INFO_CM_STATUS_BMSK */
  uint8  ul_tfci_and_cm_status;

  /* UL_DPCH_PWR_INFO_BC_BMSK, UL_DPCH_PWR_INFO_BD_BMSK */
  uint8  gain_factors;

  uint8  cm_tx_pwr_delta;
} ul_dpch_pwr_info_sample_struct_type; /* 3 bytes per sample */

#define UL_DPCH_PWR_INFO_SAMPLE_MAX 50   /* 10ms per sample, 500ms accum */
LOG_RECORD_DEFINE(UL_DPCH_PWR_INFO_LOG_PKT)
  uint8 num_samples;
  uint8 start_cfn;   /* CFN for first frame in packet */
  ul_dpch_pwr_info_sample_struct_type ul_dpch_pwr_info_samples[UL_DPCH_PWR_INFO_SAMPLE_MAX];
LOG_RECORD_END

/* A 32-bit buffer is used to construct the log packet.The log packet size is
 * dynamic in nature. The structure of the log packet will look as below:

           --------------------------------------------
<LogHdr>   | code   |length |     Time Stamp          |
            --------------------------------------------
<LogHdr>   |num_trch|asc_num|       TFCI              |
           --------------------------------------------
<Trch1Hdr> |trch_id  |ntblks|   Trblksize             |
           --------------------------------------------
<Trblk1Hdr>|McHd_Size|RB_id |     Data_len            |
           --------------------------------------------
<Trblk1Hdr>|offset   |ckey  |      CipherAlgo         |
           --------------------------------------------
           |                Count                     |
           --------------------------------------------
           |                MAC_Header                |
           --------------------------------------------
           |                RAW DATA                  |
           --------------------------------------------
           |                     .                    |
            -------------------------------------------

 * A 32-bit buffer is dynamically allocated to the worst case size possible 
 * size, UL_DATA_LOG_MAX_BUF_SIZE_WORDS. 

 * The log packet definition will only have <LogHdr> fields. The remaining fields
 * are populated in the 32-bit buffer based on the number of TrCh and number of 
 * TrBlk per TrCh.

 * Before submitting the log packet to diag, the log packet pointer is pointed to 
 * the head of the buffer and the total size of the log packet computed dynamically is 
 * updated */

/* MAX Size of the Log Packet in 32-bit words. A 32-bit array buffer is used to pack the contents of the
 * log packet including all headers and data. So, 180 words buffer will lead to a max of
 * 720 byte log packet. */
#define UL_DATA_LOG_MAX_BUF_SIZE_WORDS 180

#define UL_DATA_LOG_MAX_TRBLK_SIZE_BYTES 42
#define UL_DATA_LOG_MAX_NUM_TRBLKS 12

#define UL_DATA_LOG_TRCHID_BMSK              0x3F  /* bits 0-5 */
#define UL_DATA_LOG_TTI_SHIFT_MASK           6     /* bits 6-7 */

#define UL_DATA_LOG_TFCI_BMSK                0x00FF
#define UL_DATA_LOG_CFN_SHIFT_BMSK           8


/* UL DATA DUMP Log Packet Header */
LOG_RECORD_DEFINE(UL_DATA_DUMP_LOG_PKT)
  uint8 num_trch;
  uint8 asc_num;
  /* bits 0-7 to store TFCI; bits 8-15 for CFN */
  uint16 CFN_and_TFCI;
LOG_RECORD_END

/* Transport Channel Header */
typedef struct {
  /* bits 0-5 store trchid and bits 6-7 used to store tti
   * TTI information: 0 => 10ms; 1 => 20ms; 2 => 40ms; 3 => 80ms*/
  uint8  trchid_and_tti;
  uint8  ntblks;
  uint16 trblk_size;
}ul_erama_trch_struct_type;

/* Transport Block Header-
 * The sizes of the variables are chosen such that the total size
 * of the header is on 32-bit boundary.
 * */
typedef struct {
  uint8  mac_hdr_size;
  uint8  radio_bearer_id;
  uint16 data_len;
  uint8  offset;
  uint8  ciphering_key_id;
  /* ciphering_on & cipher_algo: 0-N, 1-Kasumi, 2-Snow3G */
  uint16 cipher_info;
  uint32 count_c;
}ul_erama_trblk_struct_type;

typedef enum
{
 CIPHERING_ALGO_NONE   = 0,
 CIPHERING_ALGO_KASUMI = 1,
 CIPHERING_ALGO_SNOW3G = 2
} ul_log_ciphering_algo_enum_type;


/***************************************************************************/
/*                                 RACH DEBUGGING LOG CODE                                                                   */
/***************************************************************************/

/* This log packet logs the RACH debug information from Firmare which
 * include config, status parameters and also last two preamble information.
 * This packet will be generated after every RACH procedure */

#define RACH_FW_DEBUG_MAX_NUM_PREAMBLES 2

/* Structure to record RACH Cfg and Status Params from Firmware */
typedef PACKED struct PACKED_POST{
  /* Tx STMR time at timing slam */
  uint32 stmr_init_tx_time;
  /* Tx Stmr time before last preamble */
 uint32 stmr_tx_time;
  /* Ref Fing Pos */
  uint32 ref_fing_pos;
  /* PN count of last preamble */
  int32 aich_pn_cnt;
  /* Time status dump */
  uint32 tx_time_status_dump;
  /* (TC)XO Current Frame Adj Accum */
  int32 tc_xo_adj_accum;
  /* Signed UE Reference Count */
  int32 ref_cnt_adj;
  /* (TC)XO inner loop freq common */
  int16 xo_freq_common;
  /* Tx time tracking accumulation */
  int16 tx_trk_accum;
  /* AI ACK THRESH */
  int16 ai_ack_thresh;
  /* AI NACK THRES */
  int16 ai_nack_thresh;
  /* Tx PN Pos */
  uint32 tx_pn_position;
  /* DL Ref chan Offset */
  int8 dl_ref_offset;
  /* Tx Frame Count Cx8*/
  uint8 tx_tti;
  /* XO Freeze Flag */
  int16 xo_freeze;
  #ifdef FEATURE_WCDMA_HS_RACH
  uint16 eai_decoding_metric;
  uint8 eai_succpncnt;
  #endif
}ul_rach_fw_params_info_struct_type;

/* Structure to record RACH preambles from Firmware */
typedef PACKED struct PACKED_POST{
  int32 ai_sum;        /* AI SUM */
  uint32 ai_rssi;      /* AI RSSI */
  uint16 hw_preamble;
  int16 tx_agc_dbm;
  uint32 aich_demod_begun;
  uint32 aich_demod_fing_lock;
  uint8 ai_result;      /* RACH STATUS */
  uint8 pa_range;
}ul_rach_fw_preamble_info_struct_type;

LOG_RECORD_DEFINE(UL_FW_RACH_DEBUG_LOG_PKT)
  uint8   version;
  uint8  num_preambles;
  ul_rach_fw_params_info_struct_type    ul_rach_params_info;
  ul_rach_fw_preamble_info_struct_type  ul_rach_preamb_info[RACH_FW_DEBUG_MAX_NUM_PREAMBLES];
LOG_RECORD_END

 /*=======================================================================
 EUL DPCH log packet (Internal) 
 The sample of this packet is collected every 10ms. This packet is delivered
 after collecting 20 samples or upon reconfiguration of either DPCH, or
 HS-DPCCH, or E-DPCH
=======================================================================*/

/* No.of EUL_DPCH_LOG_PKT_SAMPLES*/
#define HS_OFFSET_WHEN_HS_NOT_CFG  31

#endif //ULLOG_H
