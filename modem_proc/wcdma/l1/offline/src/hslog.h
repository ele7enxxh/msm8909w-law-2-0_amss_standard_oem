#ifndef HSLOG_H
#define HSLOG_H

/*==========================================================================
             WCDMA L1 HSDPA configuration header file

DESCRIPTION
  This file contains definition, declaration required for HSDPA configuration
  and maintainance code.

  Copyright (c) 2004 - 2014 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/hslog.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/23/15   sr      Corrected the QSH featurization.
02/24/15   ap      Initial check-in for WL1 QSH framework 
10/29/14   qj      Add cmbpg info due to FR22000
10/08/14   jhl     Mainlined FEATURE_WCDMA_MDSP_INTERFACE_REV_6280 
09/16/14   pkj     Optimization: HARQ macro name changed
09/08/14   hdk     Adding parameters to HS config log pkt (0x4225)
07/30/14   jhl     Removed unused variable and fixed compiler warnings
07/21/14   hdk     Maintaining local ul hs call info to avoid dead lock due to polling 
                   of hs config during logging
07/18/14   vs      Adding support for HS multisim logging.
06/27/14   rc/jhl HS ASAP Start optimization and recfg removal    
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/17/14   ac      Force byte-alignment on dsch_info_mac_hs_st and reord_pdu_info_st
01/07/14   hdk     Increasing size of 0x4225 log packet.
01/06/14   hdk     Enhancing 0x4225 log packet for 3C.
12/19/13   hdk     Enhancing 0x423F log packet to send out only amount of data that is written.
11/26/13   hdk     Chaning CQI type field in 0x421C log.
11/22/13   hdk     0x421D log pkt enhanced for 3C logging.
11/22/13   hdk     0x423F enhanced for 3C logging.
11/19/13   hdk     Corrected #HARQ processes for HS DSCH HARQ STAT LOG PKT to 8 from 16.
11/16/13   hk      Disabled HS debug logging
11/14/13   hdk     0x420F enhanced for 3C logging.
11/12/13   hdk     0x421C HS UL DPCCH info debug log pckt support for 3C.
11/12/13   hdk     0x4222 HS Deocode log pkt support for 3C.
08/30/13   vs      HS_AMP_ENG_INFO_NUM_W8_PER_ENTRY increased to 50 for 0x421d log packet.
05/21/13   sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
05/23/13   geg     WSW decoupling (except for component of QCHAT)
02/22/13   jhl     Fixed HSDPA configuration log packet
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/9/12    vs      Added the support for the new HS-SCCH log packet(0x423f).
04/14/12   ms      Moved BCCH indicator to SCCH info field in 0x4222 
03/22/12   vs      MIMO related changes.
03/16/12   ms      Added support for FEATURE_WCDMA_HS_FACH
01/28/12   vs      Nikel feature cleanup.
04/28/11   sj      upmerge from offline/main
04/15/11   vs      Added 0x421d V6 support
03/28/11   ms      Fixed HS-SCCH order logging in 0x4222 log packet for
                   DC activate/deactivate
03/14/11   ms      Mainlined FEATURE_WCDMA_MDSP_INTERFACE_REV_7200
03/04/11   ms      Fixed the logging of RW field in MIMO Demod log 
                   packet (0x4224) v2
02/22/11   ms      Added support for 0x4222 v9
10/05/10   ms      Added A&E log packet v4 + MIMO demod log packet v2
06/22/10   mc      Fixed MIMO config log packet
06/08/10   ms      Added support for HSPA_PLUS_DC_CFG event version 2
06/03/10   ms      Added support to log DC-HSDPA events
06/01/10   mc      Corrected call configuration in HS decode status logging
03/31/10   mr      Added 0x4221 Log packer V4
03/16/10   ms      Moved HS_SCCH_STAT_LOG_PKT and HS_CQI_STAT_LOG_PKT
                   from log_codes_wcdma_int_v.h to hslog.h
02/05/10   ms      Taxis/Genesis code merge
12/14/09   mr      Fixed 0x421C log packet
11/25/09   ms      Changed HS_AMP_ENG_INFO_NUM_W8_PER_ENTRY to 25.
11/20/09   ms      Updated UE category field in log pkt to support Cat24
11/14/09   ms      Added A&E log packet support for DC.
11/13/09   ms      Added HS Decode Status Ver6 log packet support for DC.
09/22/09   sup     Removed log code assignments and placed them in 
                   log_codes_wcdma_int_v.h
07/15/09   rvs     Mainlined the FEATURE_WCDMA_HS_DECODE_LOG_CODE_V4 feature.
07/10/09   vc      Added support for 0x420F log packet and increased A&E
                   sample bitwidth for 0x421D.
04/29/09   vc      corrected the SEQ11_REAL_PART1 bit mask.
04/24/09   mr     Fixed Compiler Warnings
04/09/09   ms      Code cleanup.
03/13/09   vc      Fixed the incorrect bitmask fo mimo demod log packet.
12/04/08   vc      Added macro definitions for single and dual streams.
11/05/08   vc      Fixed the incorrect bit mask for seq imag field in 0x4224 log packet.
10/28/08   mc      Reduce number of samples for MIMO log packet
10/22/08   vc      Added support for 0x4222 v4.
10/01/08   mc      Added support for MIMO UL.
09/25/08   vc      Added Framework for 0x4223 MIMO chan analysis packet.
09/24/08   vc      Added support for 0x4224 MIMO demod log packet.
09/05/08   vc      Added support for 0x421D A&D log packet.
08/26/08   vc      Added mimo specific log support to 0x4221.
08/15/08   vc      Added mimo specific log support to 0x4222 log packet.
07/21/08   ms      Logging changes
06/18/08   ms      Removed occurences of FEATURE_WCDMA_HSPA_DEC,
                   FEATURE_HS_LOG_DECODE_STATUS_W_DATA and
                   FEATURE_WCDMA_HS_STATUS_LOG_PKT_VER2.
06/10/08   ms      Added PACKED_POST attribute for structures to make it
                   compatible for GNU compiler.
                   Updated log codes.
04/30/07   ms      Added HS Amplitude Est log packet as 0x4219 under feature
                   FEATURE_WCDMA_HSDPA_LLR_LOG_PKT_V3.
02/16/07   ms      Added support for Harq Preamble/Postamble.
09/28/06   mc      Added MCAL decoder support under feature 
                   FEATURE_MCAL_WCDMA
08/14/06   yiz     Mainline HSUPA code
08/02/06   mc      HS decode status log packet is logged based on variable
                   exported from mDSP or EQ feature being defined.
06/12/06   mc      Added log code for HS LLR log packet V2.
04/20/06   mc      Added support to log a subframe as MISSED if there has 
                   been a EQ->Rake switch.
04/13/06   mc      Removed incorrect comment leading to compilation error.
04/12/06   mc      Added support for the EQ log packets
04/11/06   bd      Mainlined the file from .../l1_msm6275/hslog.h#12
03/30/06   ms      Added UE Cat info (2 bits) along with TBS in 0x4206 for 
                   MSM6275.
11/23/05   mc      Added support for HS amplitude and energy estimation 
                   log packet. 
08/25/05   mc      Added packet definition for pessimistic CQI logs.
06/09/05   mc      Modified the HS demod and mod control packets to support
                   FEATURE_HSDPA_PLUS_CM logging.
04/01/05   mc      Splitted HS_UL_BETA_TABLE_LOG_PKT and added a type field.
                   DTX/ACK/NACK/CQI will be logged independently.
03/03/05   gs      Removed hardcoded feature flag
                   FEATURE_HS_LOG_DECODE_STATUS_W_DATA
01/12/05   gs      Updated definition of MAC-hs header log packet to change
                   type of num_mac_hdr to uint16 from uint8
12/28/04   gs      Added log code definitions for Demod, Mod control table and
                   MAC-hs header log packet.
                   Included header file dechs.h
12/27/04   gs      Changed HS_UL_DPCCH_INFO_LOG_MAX_SAMPLE to 100
11/10/04   gsc/rj  Changed HS_DECODE_STATUS_LOG_MAX_SAMPLE to 100.
10/11/04   gs      Created this module

===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "log.h"
#include "l1dlcctrchcfg.h"
#include "hscfg.h"
#include "dechs.h"
#include "log_codes_wcdma_int.h"
/* Temporary include for MAC-hs log packet till we create MCAL HS module */
#include "mcalwcdma_deci.h"
#include "l1rrcif.h"
#if defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC)
#include "wl1_qsh_ext.h"
#include "l1utils.h"
#endif /* defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC) */


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
 /* enable debug F3's for 3C logging changes */
#define WCDMA_HSLOG_DEBUG (0)

/* Maximum number of Carriers */
#ifdef FEATURE_WCDMA_DC_HSDPA
#define L1_DL_NUM_MAX_CARRIERS WFW_DEMOD_NUM_MAX_CARRIERS
#else
#define L1_DL_NUM_MAX_CARRIERS 1
#endif

/*convert bit position to number used for generating bit masks*/
#define BIT_POS_2_NUM(BIT_POS) (1<<BIT_POS)

/* --------------------- */
/* LOG Packet definition */
/* --------------------- */

#ifndef HS_CFG_LOG_PKT
#define HS_CFG_LOG_PKT HS_CFG_0x4225_LOG_PKT
#else
#error "Redefinition of the log command code: HS_CFG_LOG_PKT"
#endif

#ifndef HS_DECODE_STATUS_LOG_PKT
#define HS_DECODE_STATUS_LOG_PKT HS_DECODE_STATUS_0x4222_LOG_PKT
#else
#error "Redefinition of the log command code: HS_DECODE_STATUS_LOG_PKT"
#endif

#ifndef HS_SCCH_STAT_LOG_PKT
#define HS_SCCH_STAT_LOG_PKT HS_SCCH_STAT_0x421F_LOG_PKT
#else
#error "Redefinition of the log command code: HS_SCCH_STAT_LOG_PKT"
#endif

#ifndef HS_DSCH_HARQ_STAT_LOG_PKT
#define HS_DSCH_HARQ_STAT_LOG_PKT HS_DSCH_HARQ_STAT_0x4221_LOG_PKT
#else
#error "Redefinition of the log command code: HS_DSCH_HARQ_STAT_LOG_PKT"
#endif

#ifndef HS_UL_DPCCH_INFO_DBG_LOG_PKT
#define HS_UL_DPCCH_INFO_DBG_LOG_PKT HS_UL_DPCCH_INFO_DBG_0x421C_LOG_PKT
#else
#error "Redefinition of the log command code: HS_UL_DPCCH_INFO_DBG_LOG_PKT"
#endif

#ifndef HS_CQI_STAT_LOG_PKT
#define HS_CQI_STAT_LOG_PKT HS_CQI_STAT_0x422E_LOG_PKT
#else
#error "Redefinition of the log command code: HS_CQI_STAT_LOG_PKT"
#endif

#ifndef HS_PESS_CQI_LOG_PKT
#define HS_PESS_CQI_LOG_PKT HS_PESS_CQI_0x420F_LOG_PKT
#else
#error "Redefinition of the log command code: HS_PESS_CQI_LOG_PKT"
#endif

#ifndef HS_SCCH_LOG_PKT
#define HS_SCCH_LOG_PKT HS_SCCH_0x423F_LOG_PKT
#else
#error "Redefinition of the log command code: HS_SCCH_LOG_PKT"
#endif

#ifndef HS_AMP_ENG_EST_LOG_PKT
#define HS_AMP_ENG_EST_LOG_PKT HS_AMP_ENG_EST_0x421D_LOG_PKT
#else
#error "Redefinition of the log command code: HS_AMP_ENG_EST_LOG_PKT"
#endif


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
} hsdpa_log_hdr_struct_type;

/* this macro sets field in variable that has multiplexed field within
   same variable */
#define HS_LOG_SET_FIELD(var, field, val) \
  var &= (~(HS_LOG_##field##_BMSK)); \
  var |= (((val) << (HS_LOG_##field##_SHFT)) & (HS_LOG_##field##_BMSK))

/* HSDPA configuration event definition */
/* -------------------------------------*/

/* bit field and mask of various mutiplexed information */

#define HS_LOG_CARR_INFO_BMSK 0x18
#define HS_LOG_CARR_INFO_SHFT 3

#define HS_LOG_CARR_INFO1_BMSK 0x3
#define HS_LOG_CARR_INFO1_SHFT 0

#define HS_CFG_EVT_MAC_TYPE_BMSK 0x10
#define HS_CFG_EVT_MAC_TYPE_SHFT 4

#define HS_CFG_EVT_64QAM_STATUS_BMSK 0x1
#define HS_CFG_EVT_64QAM_STATUS_SHFT 0

#define HS_CFG_EVT_TBS_TABLE_BMSK 0x2
#define HS_CFG_EVT_TBS_TABLE_SHFT 1

#define HS_CFG_EVT_MIMO_ACTION_BMSK 0xC
#define HS_CFG_EVT_MIMO_ACTION_SHFT 2

#define HS_CFG_EVT_PCPICH_TD_BMSK 0x10
#define HS_CFG_EVT_PCPICH_TD_SHFT 4

#define HS_CFG_EVT_HS_TD_BMSK 0x60
#define HS_CFG_EVT_HS_TD_SHFT 5

#define HS_CFG_EVT_MIMO_N_M_BMSK 0xF
#define HS_CFG_EVT_MIMO_N_M_SHFT 0

#define HS_CFG_EVT_MIMO_PILOT_BMSK 0x10
#define HS_CFG_EVT_MIMO_PILOT_SHFT 4

#define HS_CFG_EVT_MIMO_SCPICH_CODE_BMSK 0x1FE0
#define HS_CFG_EVT_MIMO_SCPICH_CODE_SHFT 5

/* Values for DC action */

#define HS_CFG_EVT_DC_ACTION_NOOP    0
#define HS_CFG_EVT_DC_ACTION_DISBLE  1
#define HS_CFG_EVT_DC_ACTION_ENABLE  2
#define HS_CFG_EVT_DC_ACTION_RECFG   3
#define HS_CFG_EVT_DC_ACTION_REPOINT 4

/* this macro sets field in variable that has multiplexed field within
   same variable */
#define HS_CFG_EVT_SET_FIELD(var, field, val) \
  var &= (~(HS_CFG_EVT_##field##_BMSK)); \
  var |= (((val) << (HS_CFG_EVT_##field##_SHFT)) & (HS_CFG_EVT_##field##_BMSK))


/* HSDPA configuration log packet definition */
/* ----------------------------------------- */

/* bit mask for various information changes */
#define HS_CFG_LOG_DSCH_INFO_CHG_BMSK  0x01
#define HS_CFG_LOG_SCCH_INFO_CHG_BMSK  0x02
#define HS_CFG_LOG_SRV_RL_CHG_BMSK     0x04
#define HS_CFG_LOG_DPCCH_INFO_CHG_BMSK 0x08
#define HS_CFG_LOG_MIMO_INFO_CHG_BMSK  0x10
#define HS_CFG_LOG_HARQ_PMBL_MODE_BMSK 0x80
#define HS_CFG_LOG_HARQ_PMBL_MODE_SHFT 7

/* this macro sets field in variable that has multiplexed field within
   same variable */
#define HS_CFG_LOG_SET_FIELD(var, field, val) \
  var &= (~(HS_CFG_LOG_##field##_BMSK)); \
  var |= (((val) << (HS_CFG_LOG_##field##_SHFT)) & (HS_CFG_LOG_##field##_BMSK))

/* Maximum different type of infomration in this log packet */
#define HS_CFG_LOG_MAX_INFO_TYPE 6

/* Information type identifier */
#define HS_CFG_LOG_HSDPA_TABLE_INFO_TYPE    1
#define HS_CFG_LOG_DL_CFG_INFO_TYPE         2
#define HS_CFG_LOG_UL_CFG_INFO_TYPE         4
#define HS_CFG_LOG_C1_HSDPA_TABLE_INFO_TYPE 5
#define HS_CFG_LOG_C1_HSDPA_CFG_INFO_PARAM  6
#define HS_CFG_LOG_C2_HSDPA_TABLE_INFO_TYPE 7
#define HS_CFG_LOG_C2_HSDPA_CFG_INFO_PARAM  8
#define HS_CFG_LOG_C3_HSDPA_TABLE_INFO_TYPE 9
#define HS_CFG_LOG_C3_HSDPA_CFG_INFO_PARAM  10


/* bit field and mask of various mutiplexed information */

#define HS_LOG_CFG_ACTION_TYPE_BMSK 0x03
#define HS_LOG_CFG_ACTION_TYPE_SHFT 0
                   
#define HS_LOG_CFG_CONFIG_DB_BMSK 0x7C
#define HS_LOG_CFG_CONFIG_DB_SHFT 2
                   
#define HS_LOG_CFG_UL_CONFIG_DB_BMSK 0x04
#define HS_LOG_CFG_UL_CONFIG_DB_SHFT 2

#define HS_LOG_CFG_REF_FING_BMSK 0x0F
#define HS_LOG_CFG_REF_FING_SHFT 0
                   
#define HS_LOG_CFG_HS_CELL_IDX_BMSK 0x70
#define HS_LOG_CFG_HS_CELL_IDX_SHFT 4

#define HS_LOG_CFG_CQI_REP_FACTOR_BMSK 0x0F
#define HS_LOG_CFG_CQI_REP_FACTOR_SHFT 0

#define HS_LOG_CFG_ACK_NACK_REP_FACTOR_BMSK 0xF0
#define HS_LOG_CFG_ACK_NACK_REP_FACTOR_SHFT 4

#define HS_LOG_CFG_MIMO_CQI_RATIO_BMSK 0x0F
#define HS_LOG_CFG_MIMO_CQI_RATIO_SHFT 0

#define HS_LOG_CFG_MIMO_SCPICH_EN_BMSK 0x10
#define HS_LOG_CFG_MIMO_SCPICH_EN_SHFT 4

#define HS_LOG_CFG_MIMO_PILOT_CH_CODE_BMSK 0x1FE0
#define HS_LOG_CFG_MIMO_PILOT_CH_CODE_SHFT 5


/* Maximum size of different type of informations */
#define HS_CFG_HSDPA_INFO_TABLE_LOG_INFO_SZ sizeof(hs_cfg_hsdpa_info_table_log_struct_type)
#define HS_CFG_DL_CHAN_CFG_LOG_INFO_SZ MAX(sizeof(hs_cfg_dl_chan_en_recfg_log_struct_type), \
                                           sizeof(hs_cfg_dl_chan_dis_log_struct_type))
#define HS_CFG_UL_CHAN_CFG_LOG_INFO_SZ MAX(sizeof(hs_cfg_ul_chan_en_recfg_log_struct_type), \
                                           sizeof(hs_cfg_ul_chan_dis_log_struct_type))
#define HS_CFG_FING_CFG_LOG_INFO_SZ sizeof(hs_cfg_fing_cfg_log_info_struct_type)
#define HS_CFG_MIMO_CFG_LOG_INFO_SZ sizeof(hs_cfg_mimo_cfg_log_info_struct_type)

#define HS_CFG_DC_HSPA_PARAM_INFO_SZ  sizeof(hs_cfg_dc_hspa_param_info_struct_type)

/* Maximum size of HSDPA configuration information buffer */
#define HS_CFG_LOG_INFO_MAX_SZ (HS_CFG_HSDPA_INFO_TABLE_LOG_INFO_SZ + \
                                HS_CFG_DL_CHAN_CFG_LOG_INFO_SZ + \
                                HS_CFG_UL_CHAN_CFG_LOG_INFO_SZ + \
                                HS_CFG_MIMO_CFG_LOG_INFO_SZ + \
                                HS_CFG_LOG_MAX_INFO_TYPE + \
                               ((HS_CFG_HSDPA_INFO_TABLE_LOG_INFO_SZ + \
                                HS_CFG_DC_HSPA_PARAM_INFO_SZ)*\
                                (L1_HSDPA_MAX_CARRIERS-1)))

/* HSDPA information table log packet information */
typedef PACKED struct PACKED_POST
{
  /* This is image of HSDPA table uploaded to mDSP for this configuration */
  uint32 hsdpa_info_table_buf[HS_INFO_TABLE_LEN_W32];
} hs_cfg_hsdpa_info_table_log_struct_type;

/* HSDPA DL channel configuration for action type enable and reslam */
typedef PACKED struct PACKED_POST
{
  /* Action type and configuration database for DL channel configuration */
  uint8 action_cfg_db;
  /* HS serving RL reference finger and HS cell index for that finger */
  uint8 ref_fing_hs_cell_idx;
  /* Primary scrmabling code for refernce finger */
  uint16 ref_fing_psc;
  /* DPCH comblner HW channel number */
  uint8 dpch_hw_chan_num;
  /* DL DPCH to HS-SCCH start offset in units of ticks (256 Cx1 per tick) */
  uint8 dpch_scch_offset;
  /* offset of enable HS-SCCH relative to CPICH frame boundary (in bpg) */
  uint8 hs_offset;
  /* Starting global frame number */
  uint16 start_global_sub_fn;
  /* Last global frame number (only applicable to reslam) */
  uint16 final_global_sub_fn;
  /* DL DPCH CFN at which DL-HS channel will become active */
  uint8 dl_dpch_action_cfn;
  /* Frame tick based on DL DPCH timeline at which command was sent in previous
     frame from dl_dpch_action_cfn */
  uint8 dl_dpch_tick_cmd_sent;
} hs_cfg_dl_chan_en_recfg_log_struct_type;

/* HSDPA DL channel configuration for action type disable */
typedef PACKED struct PACKED_POST
{
  /* Action type and configuration database for DL channel configuration */
  uint8 action_cfg_db;
  /* Last global frame number */
  uint16 final_global_sub_fn;
  /* DL DPCH CFN at which DL-HS channel will stop */
  uint8 dl_dpch_action_cfn;
  /* Frame tick based on DL DPCH timeline at which command was sent in previous
     frame from dl_dpch_action_cfn */
  uint8 dl_dpch_tick_cmd_sent;
} hs_cfg_dl_chan_dis_log_struct_type;

/* HSDPA UL channel configuration for action type enable and reslam */
typedef PACKED struct PACKED_POST
{
  /* Action type and configuration database for UL channel configuration */
  uint8 action_cfg_db;
  /* CQI adjustment before quantization */
  uint16 cqi_adjustment;
  /* CQI feed back cycle in units of sub frames */
  uint8 cqi_feedback_cycle;
  /* CQI repetition factor and ACK/NACK repetition factor */
  uint8 repetition_factor;
  /* UL DPCH to HS-DPCCH start offset in units of ticks (256 Cx1 per tick) */
  uint8 hs_dpcch_offset;
  /* Starting global frame number */
  uint16 start_global_sub_fn;
  /* Last global frame number (only applicable to reslam) */
  uint16 final_global_sub_fn;
  /* UL DPCH CFN at which DL-HS channel will become active */
  uint8 ul_dpch_action_cfn;
  /* Frame tick based on UL DPCH timeline at which command was sent in previous
     frame from ul_dpch_action_cfn */
  uint8 ul_dpch_tick_cmd_sent;
} hs_cfg_ul_chan_en_recfg_log_struct_type;

/* HSDPA UL channel configuration for action type disable */
typedef PACKED struct PACKED_POST
{
  /* Action type and configuration database for UL channel configuration */
  uint8 action_cfg_db;
  /* Last global frame number */
  uint16 final_global_sub_fn;
  /* UL DPCH CFN at which DL-HS channel will stop */
  uint8 ul_dpch_action_cfn;
  /* Frame tick based on UL DPCH timeline at which command was sent in previous
     frame from ul_dpch_action_cfn */
  uint8 ul_dpch_tick_cmd_sent;
} hs_cfg_ul_chan_dis_log_struct_type;

/* HS MIMO configuration information */
typedef PACKED struct PACKED_POST
{
  /* Bit 3:0    MIMO N_cqi_typeA/M_cqi_ratio (1/2, 2/3, 3/4, 4/5, 5/6, 6/7, 7/8, 8/9, 9/10, 1/1)
   * Bit 4      MIMO S-CPICH Enabled (0 or 1)
   * Bit 12:5   MIMO pilot channelization code (0..255)
   * Bit 15:13  Reserved
   */
  uint16 mimo_config;
} hs_cfg_mimo_cfg_log_info_struct_type;

/* DC-HSPA Parameters */
typedef PACKED struct PACKED_POST
{
  /* Bit 0  [0- Disable, 1-Enable]
     Bit 1  [0- no TD, 1-TD]
     Bit 7:2 [Reserved]
  */
  uint8  dc_state               :1; /*Bit 0: DC state*/
  uint8  sttd_enable            :1; /*Bit 1: STTD state*/
  uint8  wait_on_scch_ord       :1; /*Bits2: Wait on SCCH order.*/
  uint8  reserved               :5; /*Bit 3:7: reserved*/
  
  uint16 scr_code;
  uint16 freq;
  uint8  cell_db_idx;
  uint8  hs_info_table_idx;
  uint8  mpo;
} hs_cfg_dc_hspa_param_info_struct_type;

LOG_RECORD_DEFINE(HS_CFG_LOG_PKT)
  /* Version field */
  uint8 version;
  /* indicate change in configuration of various type of channels */
  uint16 change_in_cfg;
  /* HS configuration information buffer */
  uint8 info[HS_CFG_LOG_INFO_MAX_SZ];
LOG_RECORD_END

/* ------------------------------------- */
/* HS DL demod control table log packet  */
/* ------------------------------------- */

#define HS_DEMOD_CTRL_NUM_BUF   2
#define HS_DEMOD_CTRL_TABLE_LOG_MAX_SUB_FR 500
#define HS_DEMOD_CTRL_TABLE_LOG_BUF_LEN_W32 \
  ((HS_DEMOD_CTRL_TABLE_LOG_MAX_SUB_FR & 0x1F) ? \
    ((HS_DEMOD_CTRL_TABLE_LOG_MAX_SUB_FR >> 5) + 1) : \
    (HS_DEMOD_CTRL_TABLE_LOG_MAX_SUB_FR >> 5))

LOG_RECORD_DEFINE(HS_DEMOD_CTRL_TABLE_LOG_PKT)
  /* starting gloabel sub frame number for this log accumulation */
  uint16 starting_global_sub_fn;
  /* Number of sub frames */
  uint16 num_sub_frames;
  /* demod control table info buffer */
  uint32 demod_ctrl_table_info[HS_DEMOD_CTRL_TABLE_LOG_BUF_LEN_W32];
LOG_RECORD_END

/* ----------------------------------- */
/* HS UL mod control table log packet  */
/* ----------------------------------- */

#define HS_MOD_CTRL_NUM_BUF   2
#define HS_MOD_CTRL_TABLE_LOG_MAX_SUB_FR 500
#define HS_MOD_CTRL_TABLE_LOG_NUM_SUB_FR_IN_W32  10
#define HS_MOD_CTRL_TABLE_LOG_BUF_LEN_W32 \
  ((HS_MOD_CTRL_TABLE_LOG_MAX_SUB_FR % HS_MOD_CTRL_TABLE_LOG_NUM_SUB_FR_IN_W32) ? \
    ((HS_MOD_CTRL_TABLE_LOG_MAX_SUB_FR / HS_MOD_CTRL_TABLE_LOG_NUM_SUB_FR_IN_W32) + 1) : \
    (HS_MOD_CTRL_TABLE_LOG_MAX_SUB_FR / HS_MOD_CTRL_TABLE_LOG_NUM_SUB_FR_IN_W32))

LOG_RECORD_DEFINE(HS_MOD_CTRL_TABLE_LOG_PKT)
  /* starting gloabel sub frame number for this log accumulation */
  uint16 starting_global_sub_fn;
  /* Number of sub frames */
  uint16 num_sub_frames;
  /* demod control table info buffer */
  uint32 mod_ctrl_table_info[HS_MOD_CTRL_TABLE_LOG_BUF_LEN_W32];
LOG_RECORD_END

/* --------------------------- */
/* HS decode status log packet */
/* --------------------------- */
/*Macro for invalid Stop/Final GSFN for logging*/
#define INVALID_STOP_GSFN (0xFFFF)

#ifdef FEATURE_WCDMA_HS_FACH
#define HS_LOG_DECODE_STATUS_STATE_IND_VAL_DCH  0
#define HS_LOG_DECODE_STATUS_STATE_IND_VAL_FACH 1
#define HS_LOG_DECODE_STATUS_STATE_IND_VAL_PCH  2
#endif

#define HS_LOG_DECODE_STATUS_DSCH_STATUS_CRC_FAIL 0
#define HS_LOG_DECODE_STATUS_DSCH_STATUS_CRC_PASS 1
#define HS_LOG_DECODE_STATUS_DSCH_STATUS_DTX      2

#define HS_DECODE_STATUS_DSCH_STATUS_NACK 0
#define HS_DECODE_STATUS_DSCH_STATUS_ACK  1
#define HS_DECODE_STATUS_DSCH_STATUS_AUTO_ACK  2

/* Number of HS decode status buffer */
#define HS_DECODE_STATUS_NUM_BUF 2

#define HS_DECODE_STATUS_LOG_MAX_SAMPLE 25


/* For HS decode status TB data bit logging, sub frame for frame N-2
   are logged at sub frame N, so there is 3 frame buffer, it will have
   3x5= 15 sub frames. Keep data buffered for 16 sub frames indexed with
   subframe mod 16 */
#define HS_DECODE_DSCH_TB_DATA_BIT_BUF_LEN 16

#define HS_LOG_MAX_NUM_SAMPLES (L1_HSDPA_MAX_CARRIERS*HS_DECODE_STATUS_LOG_MAX_SAMPLE)
typedef PACKED struct PACKED_POST
{
  uint32 extflag_2    :1;
  uint32 num_pdu2     :7;
  uint32 sid_2        :3;
  uint32 extflag_1    :1;
  uint32 num_pdu1     :7;
  uint32 sid_1        :3;
  uint32 tsn          :6;
  uint32 qid          :3;
  uint32 ver_flag     :1;
  
}dsch_info_mac_hs_st;

typedef PACKED struct PACKED_POST
{
  uint32              :8;
  uint32 num_sdu      :5;
  uint32 si           :2;  
  uint32 tsn          :14;  
  uint32 qid          :3;
 }reord_pdu_info_st;

typedef PACKED struct PACKED_POST
{
  uint8 num_reorder_sdu;
  reord_pdu_info_st reorder_pdu_info[MCALWCDMA_DEC_HS_REORDER_PDU_MAX_COUNT];
}dsch_info_mac_ehs_st;

typedef PACKED struct PACKED_POST
{
  /*Code Offset*/
  uint32 code_offset  :4;  // bit  0:3
  /*Number of Codes */
  uint32 num_code     :4;  // bit  4:7
  /*Modulation Scheme
      0 QPSK
      1 16 QAM
      2 64QAM
      3 Reserved
  */
  uint32 mod_scheme   :2;  // bit  8:9
  /*HARQ Process ID*/
  uint32 harq_id      :3;  // Bit  10:12
  /*XRV*/
  uint32 xrv          :3;  // bit  13:15
  /*NDI
    1: New transmission and  
    0: Retransmission
   */
  uint32 ndi          :1;  // bit  16
  /*Reserved*/
  uint32              :15; // bit  17:31 
} scch_info_ord_dis_st;

/*if SCCH order */
typedef PACKED struct PACKED_POST
{
  /*SCCH order type*/
  uint32 scch_ord_t   :3;  // bit 0:2
  /*SCCH order*/
  uint32 scch_ord     :3;  // bit 3:5
  /*Reserved*/
  uint32              :26; // bit 6:31  
} scch_info_ord_en_st;


typedef PACKED struct PACKED_POST
{ /*Unquantized CQI*/
  uint32 rake_cqi         :10;  // Bit 0:9
  uint32 qice_cqi         :10;  // Bit 10:19
  /*SCCH Attempted (1=Attempted, 0=Not attempted)*/
  uint32 scch_attempt     :1;
  /*SCCH Valid (1=TRUE, 0=FALSE)*/
  uint32 scch_valid       :1;
  /*DSCH status(0 = CRC fail, 1 = CRC pass)
  If HS-SCCH is not valid mark it as DTx*/
  uint32 dsch_status      :1;
  /*EQ enabled*/
  uint32 eq_en            :1;
  /* Q-ICE enabled*/
  uint32 qice_en          :1;
  /*(0: Rake, 1:  QICE)*/
  uint32 demfnt           :1;
  /*Receive Diversity enabled on Rake Receiver*/
  uint32 rxd_ena_on_rake  :1;
  /*Enable mask for TD early termination.*/
  uint32 td_early_term_en :1;
  /*SCCH info(The below info is valid only if SCCH is valid):*/
  /*1 = TRUE, HS-SCCH is an order, 
    0 = FALSE, HS-SCCH is not an order*/
  uint32 hs_scch_ord      :1;
  uint32 scchidx          :2;
  uint32 bcch_indic       :1;
  /*---------------1stWORD------------------*/
  PACKED union PACKED_POST
  {
    /*if SCCH not order */
    scch_info_ord_dis_st scch_info_ord_dis;
    
    /*if SCCH order */
    scch_info_ord_en_st scch_info_ord_en;
    
  };
  /*---------------2ndWORD------------------*/
  
  /*Firmware interpreted TBS*/
  uint16 fw_tbsize;
  
  /*Turbo Decoder early termination 
  on valid if TDEarlyTermEn flag is set*/
  PACKED struct PACKED_POST
  { 
    /*Number of full iterations 
    for the last code word in the subframe. */
    uint16 itr            :4; // bit 0:3
    /*Min Log Likelihood Ratio for the last codeword 
    in the subframe*/
    uint16 min_llr        :12; // bit 4:15
  }td_early_term_info;
  /*---------------3rdWORD------------------*/
  PACKED union PACKED_POST
  {
    dsch_info_mac_hs_st   dsch_info_mac_hs;
    dsch_info_mac_ehs_st  dsch_info_mac_ehs;  
  };
  /*---------------4thWORD------------------*/
  /* DL Desense presense on all bpgs(30) of 3 slots */
  uint32 dl_desense_qta_bpgs  :30; //bit 0:29
  /* 1 means Desense present, 0 means QTA present */
  uint32 desense_flag         :1;
  /* RxD enabled on QICE Receiver */
  uint32 rxd_ena_on_qice      :1;
  /*---------------5thWORD------------------*/
  /*CM gap presence in 30bpgs of a subframe*/
  uint32 cm_Gap_bpg           : 30;  /* Bits 29-0 */
  uint32                      : 2;   /* Bit 30-31, Reserved */
  /*---------------6thWORD------------------*/
}hs_info_per_sf_st;

LOG_RECORD_DEFINE(HS_DECODE_STATUS_LOG_PKT)
  uint8  ver;
  uint8  num_samp       :5;  //Bit 0:4
  /* Multi-Sim Configuration i.e SS/DSDS/DSDA/TSTS */
  uint8  multi_sim_conf :3;  //Bit 5:7
  
  uint16 strt_sf        :12; //Bit 0:11
  /*State Indicator 
      0 = CELL_DCH 
      1 = CELL_FACH*/
  uint16 wl1_state      :1; //Bit 12
  /*Mac-ehs enabled (1: Yes, 0: No)*/
  uint16 mac_hs_ehs     :1; //Bit 13
  
  /* num of carriers
      00: 1C
      01: 2C
      10: 3C
      11: 4C */
  uint16 num_car        :2; //Bit 14:15
  
  
  PACKED union PACKED_POST
  {
    uint16 cmn_info_raw;
    struct
    { /*0 -> 1 HARQ Process ... 7-> 8 HARQ Process*/
      uint16 num_harq                  :3; // Bit 0:2     
      
      /*Primary Carrier: C0*/
      /*Transport Block Size table used 
              0: non-octet aligned 
              1: octet aligned*/
      uint16 c0_tb_size_octet_align   :1; // Bit 3
      /*1: 64QAM Configured
        0: 64QAM not configured*/
      uint16 c0_64qam_config          :1; // Bit 4
      
      /*Secondary Carrier: C1*/
      uint16 c1_tb_size_octet_align   :1; // Bit 5
      uint16 c1_64qam_config          :1; // Bit 6
      
      /*Tertiary Carrier: C2*/
      uint16 c2_tb_size_octet_align   :1; // Bit 7
      uint16 c2_64qam_config          :1; // Bit 8
      
      /*Quaternary Carrier: C3*/
      uint16 c3_tb_size_octet_align   :1; // Bit 9
      uint16 c3_64qam_config          :1; // Bit 10
      
      /*UE category*/
      uint16 ue_cat                   :5; // Bit 11:15
    };
    
  }cmn_info;
  
  hs_info_per_sf_st hs_info_per_sf[HS_LOG_MAX_NUM_SAMPLES];
LOG_RECORD_END

/* ----------------------------- */
/* HS-SCCH statistics log packet */
/* ----------------------------- */

#define HS_STAT_LOG_MAX_SAMPLE 1024

LOG_RECORD_DEFINE(HS_SCCH_STAT_LOG_PKT)
  /* Version field */
  uint8 version;
  /*Num of carriers*/
  uint8 num_carr;
  /* Number of sub frames */
  uint16 num_sub_frames[L1_HSDPA_MAX_CARRIERS];
  /* num SCCH decode attempted */
  uint16 num_scch_decode_attempted[L1_HSDPA_MAX_CARRIERS];
  /* number of SCCH valid */
  uint16 num_scch_valid[L1_HSDPA_MAX_CARRIERS];
LOG_RECORD_END

/* ---------------------------------- */
/* HS-DSCH HARQ statistics log packet */
/* ---------------------------------- */
typedef PACKED struct PACKED_POST
{
  /* HARQ process Id */
  uint8 proc_id;
  /* tital number of bits received */
  uint32 num_rece_bits;
  /* number of new transmission */
  uint16 num_new_tx;
  /* number of block errors. No ACK received at all */
  uint16 num_blk_errors;
  /* number of NACKs after first (new) transmission */
  uint16 num_1_nack;
  /* number of NACKs after 2 transmissions */
  uint16 num_2_nack;
  /* number of NACKs after 3 transmissions */
  uint16 num_3_nack;
  /* number of NACKs after 4 transmissions */
  uint16 num_4_nack;
} hs_dsch_harq_proc_stat_log_struct_type;

LOG_RECORD_DEFINE(HS_DSCH_HARQ_STAT_LOG_PKT)
 /* Version field */
  uint8 version;
  /* Number of sub frames */
  uint16 num_sub_frames     :11;
  /*Num of carriers*/
  uint16 num_carr           :2;
  /*reserved*/
  uint16                    :3; 
  /* number of HARQ process on each carrier */
  uint16 num_harq_proc_c0   :4;
  uint16 num_harq_proc_c1   :4;
  uint16 num_harq_proc_c2   :4;
  uint16 num_harq_proc_c3   :4;
  
  /* proc stats */
  hs_dsch_harq_proc_stat_log_struct_type 
  proc_stat[L1_HSDPA_MAX_CARRIERS][L1_DL_HS_DSCH_HARQ_MAX_PROC];
LOG_RECORD_END

/* ---------------------------- */
/* HS CQI statistics log packet */
/* ---------------------------- */

#define HS_CQI_STAT_MAX_CQI_IDX 30

typedef PACKED struct PACKED_POST
{
  /* number of ACKs for this CQI index */
  uint16 num_ack;
  /* number of NACKs for this CQI index */
  uint16 num_nack;
  /* number of DTXs for this CQI index */
  uint16 num_dtx;
} hs_cqi_idx_stat_log_struct_type;

LOG_RECORD_DEFINE(HS_CQI_STAT_LOG_PKT)  
  /* Version field */
  uint8 version;
  /* Number of sub frames */
  uint16 num_sub_frames :11;
  uint16 num_carr       :2;
  uint16                :3; /*reserved*/
  /* CQI index stats for indicies from 0 to 30 */
  hs_cqi_idx_stat_log_struct_type cqi_idx_stat[L1_HSDPA_MAX_CARRIERS][HS_CQI_STAT_MAX_CQI_IDX + 1]; 
LOG_RECORD_END



/* This structure defines HS status that is required for HS downlink channels
   stat generation */
typedef struct
{
  /* hs Decode status for each carrier*/
  /*Num of configured carriers*/
  uint8 num_carr; 
  /* Quantized CQI from n-3 frame where n is rest of info */
  uint8 quan_cqi[L1_HSDPA_MAX_CARRIERS]; 
  /* indicate if SCCH demod was even attempted or not */
  boolean scch_demod_attempted[L1_HSDPA_MAX_CARRIERS];
  /* TRUE indicate SCCH was demodulated and CRC pass FALSE otherwise */
  boolean scch_valid[L1_HSDPA_MAX_CARRIERS];
  /* DSCH CRC pass/fail status */
  boolean dsch_status[L1_HSDPA_MAX_CARRIERS];
  /* New transmission of HS DSCH block */
  boolean new_tx[L1_HSDPA_MAX_CARRIERS];
  /* HARQ process Id primary*/
  uint8 harq_proc_id[L1_HSDPA_MAX_CARRIERS];
  /* transport block size of this DSCH block */
  uint16 tb_size[L1_HSDPA_MAX_CARRIERS];

} dec_hs_decode_status_struct_type;


/* This structure defines HS-DSCH HARQ process stats */
typedef struct
{
  /* indicate that ACK has been received for this flag. If ACK is received then
     this process Id is supposed to be done for stat generation. It is not considered
     again untill new transmission is received.
     Set to TRUE for initialization */
  boolean ack_received;
  /* number of NACKs received. Init it to 0 when new transmission is received */
  uint8 num_nack_rece;
} hs_dsch_harq_proc_stat_struct_type;

/* ---------------------------------- */
/* HS UL DPCCH information log packet */
/* ---------------------------------- */

/* Max number of sample in DPCCH information log packet */
#define HS_UL_DPCCH_INFO_LOG_MAX_SAMPLE 100

#define HS_UL_CQI_DTX   0x3FF

/* DPCH to DPCCH offset in header of 0x421C */
#define HS_LOG_DPCCH_HSDPCCH_OFFSET_BMSK 0xF800
#define HS_LOG_DPCCH_HSDPCCH_OFFSET_SHFT 11

/* Fields in HS DPCCH status */
/* For DC HSPA: dummy */
#define HS_LOG_DPCCH_STATUS_CQI_TYPE_BMSK 0x03
#define HS_LOG_DPCCH_STATUS_CQI_TYPE_SHFT 0

/* Raw CQI Txed on UL */

/* MIMO Type A */
#define HS_LOG_DPCCH_STATUS_CQI_TX_BMSK 0x3FC
#define HS_LOG_DPCCH_STATUS_CQI_TX_SHFT 2

/* MIMO Type B */
#define HS_LOG_DPCCH_STATUS_CQI_MIMO_TYPE_B_BMSK 0x07C
#define HS_LOG_DPCCH_STATUS_CQI_MIMO_TYPE_B_SHFT 2

/* PCI */
#define HS_LOG_DPCCH_STATUS_PCI_TX_BMSK 0xC00
#define HS_LOG_DPCCH_STATUS_PCI_TX_SHFT 10

/* DC HSPA */
#define HS_LOG_DPCCH_STATUS_CQI_DC_HSPA_BMSK 0x0F80
#define HS_LOG_DPCCH_STATUS_CQI_DC_HSPA_SHFT 7

/* Legacy/ non MIMO */
#define HS_LOG_DPCCH_STATUS_CQI_LEGACY_BMSK 0X07C
#define HS_LOG_DPCCH_STATUS_CQI_LEGACY_SHFT 2

/* ACK NACK status 
   0: ACK, 1: NACK, 2: ACK_ACK, 3: ACK_NACK, 4: NACK_ACK, 5: NACK_NACK
   6: PRE, 7: POST, 8: DTX */
#define HS_LOG_DPCCH_STATUS_ACK_NACK_DTX_BMSK 0xF000
#define HS_LOG_DPCCH_STATUS_ACK_NACK_DTX_SHFT 12

#define HS_UL_DPCCH_LOG_INFO_SZ_PER_SAMPLE 1

LOG_RECORD_DEFINE(HS_UL_DPCCH_INFO_LOG_PKT)
  /* number of samples in this log packet */
  uint8 num_samples;
  /* starting gloabel sub frame number for this log accumulation */
  uint16 starting_global_sub_fn;
  /* ACK/NACK/CQI status info */
  uint8 ack_nack_cqi_status[HS_UL_DPCCH_INFO_LOG_MAX_SAMPLE];
LOG_RECORD_END

/* ---------------------------------------- */
/* HS UL DPCCH debug information log packet */
/* ---------------------------------------- */
/* Number of HS decode status buffer */
#define ENCHS_LOG_NUM_BUF 2

/* Tx AGC and PA range fields at start of ACK/CQI */

#define HS_UL_DPCCH_DBG_LOG_INFO_SZ_PER_SAMPLE \
  sizeof(hs_ul_dpcch_debug_info_log_struct_type)

typedef PACKED struct PACKED_POST
{
  /* ACK/NACK/CQI status info */
  /* Bitmask indicating which serving 
        cells' CQI are reported this subframe*/
  uint32 hs_cqi_report         :1; /*Bit 0*/
  uint32 hs_cqi_valid_bmsk     :4; /*Bit 1:4*/
  /* CQI per carrier
        Takes a value of 0 to 30*/
  uint32 cqi_carr1             :5; /*Bit 5:9*/
  uint32 cqi_carr2             :5; /*Bit 10:14*/
  uint32 cqi_carr3             :5; /*Bit 15:19*/
  uint32 cqi_carr4             :5; /*Bit 20:24*/
  /* CQI type
     1: Rake CQI (Print “R”)
     2: QICE CQI (Print “Q”)
     3: Override   (Print “V”)
     Else ‘*’ */
  uint32 cqi_type              :2; /*Bit 25:26*/
  /* This bitmask specifies which secondary carriers are active.
       Bit27 – 1st secondary carrier active
       Bit28 – 2nd secondary carrier active
       Bit29 – 3rd secondary carrier active */
  uint32 hs_sec_carr_act_bmsk  :3; /*Bit 27:29*/
  /*Reserved*/
  uint32                       :2; /*Bit 30:31*/
  /*---------32bits---------------*/
  /*      ackNackStatus         Meaning
        ----------------------------
              1 -> DTX (print “D”)
              2 -> ACK (print “A”)
              3 -> NACK (print “N”)
              4-> CM (print “C”)
              5-> Preamble (print “R”)
              6-> Postamble (print “S”)
              All others -> (print “*”)  */
  uint16  ack_nack_carr1        :3;
  uint16  ack_nack_carr2        :3;
  uint16  ack_nack_carr3        :3;
  uint16  ack_nack_carr4        :3;
  uint16                        :4; /*Reserved*/
  /*----------16bits---------------*/
  /* A_HS programmed by the FW during the ACK slot. 
       BIT 0:5
       Valid range 0..38*/
  /* Bd/Bc for ACK, Bit 6:11
       Valid range: {0/15, 1/15, …14/15, 15/15, 15/14, 15/13, …15/1}*/
  uint16 beta_ack_slot         :12;
  /*Reserved*/
  uint16                       :4; /*bit 12:15*/
  /*---------16bits---------------*/
  /* A_HS programmed by the FW during the CQI slot. 
        BIT 0:5
       Valid range 0..38*/
  /* Bd/Bc for CQI Bit 6:11
       Valid range: {0/15, 1/15, …14/15, 15/15, 15/14, 15/13, …15/1}*/
  
  uint16 beta_cqi_slot         :12; 
  /*Reserved*/
  uint16                       :4; /*bit 12:15*/
  /*---------16bits---------------*/
  /* logging the Tx Agc Value(quantity after the limiter) 
     programmed in the first BPG of an ACK slot*/
  uint16 tx_agc_ack            :10; /*bit 0:9*/
  /* logging the PA RANGE in the first BPG of an ACK slot */
  uint16 tx_pa_ack             :2;  /*bit 10:11*/
  /*Reserved*/
  uint16                       :4;  /*bit 12:15*/
  /*---------16bits---------------*/
  /* logging the Tx Agc Value(quantity after the limiter) 
     programmed in the first BPG of an CQI slot*/
  uint16 tx_agc_cqi            :10; /*bit 0:9*/
  /* logging the PA RANGE in the first BPG of an CQI slot */
  uint16 tx_pa_cqi             :2;  /*bit 10:11*/
  /*Reserved*/
  uint16                       :4;  /*bit 12:15*/
  /* tx Blanking */
  uint32 tx_blanking           :30;  /*bit  0:29 */
  /* Reserved */
  uint32                       :2;
  
  
  /*---------16bits---------------*/
} hs_ul_dpcch_debug_info_log_struct_type;

LOG_RECORD_DEFINE(HS_UL_DPCCH_INFO_DBG_LOG_PKT)
  /* Packet version num */
  uint8 version_num              :5;
  /* Multi-Sim Configuration.SS/DSDS/DSDA/TSTS */
  uint8 multi_sim_conf           :3;
  /* number of samples in this log packet */
  uint8 num_samples;
  /* starting global sub frame number for this log accumulation */
  uint16 starting_global_sub_fn  :11; /*Bit 0:10*/
  /* BPGs from DPCCH to first  HSDPCCH subframe*/
  uint16 dpcch_offset            :5; /*Bit 11:15*/
  /* Number of configured carriers
       0 : only primary carrier configured
       1 : primary and secondary carrier configured
       2 : primary, secondary and tertiary carrier configured
       3 : 4 carriers configured */
  uint8 config_carr              :2; /*bit 0:1*/
  /*HS DPCCH slot format*/
  uint8 hsdpcch_slot_format      :1; /*bit 2*/
  uint8                          :5; /*bit 3:7 Reserved*/
  
  /* HS UL DPCCH debug log info */
  hs_ul_dpcch_debug_info_log_struct_type info[HS_UL_DPCCH_INFO_LOG_MAX_SAMPLE];
LOG_RECORD_END

/* ------------------------ */
/* MAC HS header log packet */
/* ------------------------ */
/* Number of log packet buffer for MAC hs header logging */
#define DEC_HS_NUM_MAC_HS_HDR_LOG_PKT_BUF 2
#define HS_NUM_MAX_MAC_HS_HDR_SAMPLES 90
#define HS_MAC_HS_HDR_LOG_BUG_MAX_LEN (HS_NUM_MAX_MAC_HS_HDR_SAMPLES * \
                                       sizeof(hs_mac_hdr_log_info_struct_type))


/* MAC-hs header info */
typedef PACKED struct PACKED_POST
{
  /* Global Subframe Number of MAC-hs headers */
  uint16 global_sub_fn;
  /* MAC header payload buffer
     Bit 0 is of MAC HS header is MSB of first W32 of MAC HS header buffer.
     End of MAC HS header is determined by F flag = 1.
     Maximum 8 N,SID,F fields are allowed. */
  uint32 mac_hdr_buffer[DEC_HS_DOB_W32_READ_FOR_MAC_HS_HEADER_DECODE];
} hs_mac_hdr_log_info_struct_type;

LOG_RECORD_DEFINE(HS_MAC_HS_HDR_LOG_PKT)
  /* Number of MAC headers */
  uint16 num_mac_hdr;

  uint8 mac_hdr_buffer[HS_MAC_HS_HDR_LOG_BUG_MAX_LEN];
LOG_RECORD_END

/* ------------------------ */
/* Pessimistic CQI log packet */
/* ------------------------ */

/* Number of HS LLR log buffer */
#define HS_AMP_ENG_LOG_NUM_BUF 2
#define HS_MIMO_LOG_NUM_BUF 2
#define HS_PESS_CQI_LOG_NUM_BUF 2
/* Number of HS SCCH log buffer */
#define HS_SCCH_LOG_NUM_BUF 2

#define HS_NUM_MAX_PESS_CQI_SAMPLES  (50)
#define HS_PESS_CQI_SAMPLE_SIZE      (sizeof(hs_pess_cqi_info_struct_type))

#define HS_PESS_CQI_LOG_MAX_BUF_SIZE (sizeof(hs_pess_cqi_info_struct_type) * \
                                           HS_NUM_MAX_PESS_CQI_SAMPLES)

typedef PACKED struct PACKED_POST
{
  /*CPICH SNR for non-MIMO, debug parameters for MIMO */
  uint32 cpich_snr_rake :10,
         cpich_snr_eq   :10,
         cpich_snr_qice :10;
  /*reserved*/
  uint32                :2;
    
  /* Phase noise rejection counter for all receivers */
  uint16 pnr_cnt_rake :5,
         pnr_cnt_eq   :5,
         pnr_cnt_qice :5;
  /*reserved*/
  uint16              :1; 
  
  /* IS bpg count */
  /* This is common to all receivers
  Bit 0..4: unsigned number of BPG 
              with undo AGC set per subframe
  Bit 5..9: unsigned bitmask of 6bpg groups 
              within a subframe where intermittent 
              scheduling took effect.*/
  uint16 num_undo_agc_bpgs  :5;
  uint16 int_schd_rej_bmsk  :5;
  /*Reserved*/
  uint16                    :6;
  
} hs_pess_cqi_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 num_carr;
  hs_pess_cqi_struct_type pess_cqi_info[L1_HSDPA_MAX_CARRIERS];
} hs_pess_cqi_info_struct_type;

LOG_RECORD_DEFINE(HS_PESS_CQI_LOG_PKT)
  /*Version*/
  uint8 version;
  /* start GSFN 
       0:14 SFN
       15: reserved
    */
  uint16 starting_global_sub_fn;
  /* Number of samples in this log packet */
  uint8 num_samples;
  /* pessimistic CQI samples */
  uint8 pess_cqi_buff[HS_PESS_CQI_LOG_MAX_BUF_SIZE];
LOG_RECORD_END

/* -------------------------------------------------*/
/* Sturctures for HS-SCCH Decode Status log packet*/
/* -------------------------------------------------*/
#define HSDPA_NUM_SCCH_MONITERED (4)
#define HS_NUM_MAX_SCCH_SAMPLES  (50)
#define MAX_SCCH_LOG_BUFF_SIZE   (sizeof(hs_scch_info_struct_type)\
                                  * HS_NUM_MAX_SCCH_SAMPLES)

typedef PACKED struct PACKED_POST
{
  /* SCCH Amplitudes valuse for SCCH slot 1, 2 and 3 */
  uint32 amp_slot1 :10; /* Bits 9 - 0 */
  uint32 amp_slot2 :10; /* Bits 19-10 */
  uint32 amp_slot3 :10; /* Bits 29-20 */
  uint32           :2 ; /* Bits 31-30  reserved*/

  /* Error count from SCCH Part1 VD decoding, rounded to 4 bits */
  uint8 err_cnt_pt1  :4;  /* Bits 3-0 */
  /* Error count from SCCH Part2 VD decoding, rounded to 4 bits */
  uint8 err_cnt_pt2 :4;  /* Bits 7-4 */
  
} hs_scch_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 num_carr;
  hs_scch_struct_type scch_info[L1_HSDPA_MAX_CARRIERS][HSDPA_NUM_SCCH_MONITERED];
} hs_scch_info_struct_type;


LOG_RECORD_DEFINE(HS_SCCH_LOG_PKT)
  /*Version*/
  uint8 version;
  /* start GSFN */
  uint16 starting_global_sub_fn;
  /* Number of samples in this log packet */
  uint8 num_samples;
  /* HS-SCCH samples */
  uint8 scch_info[MAX_SCCH_LOG_BUFF_SIZE];
LOG_RECORD_END

/* -------------------------------------------------*/
/* HSDPA Amplitude and Energy estimation log packet */
/* -------------------------------------------------*/
#define HS_NUM_MAX_HS_AMP_ENG_SAMPLES    (75)
#define MAX_AMP_ENG_MEAS_PER_SAMPLE      (5)

/*size of demod_info_type * Num of carrier + 2 Bytes for SF number and Num carr */
#define SIZE_OF_AMP_ENG_LOG_PER_SAMPLE(NUM_CARR)   (sizeof(demod_info_type)*NUM_CARR \
                                                    + sizeof(uint16))
#define MAX_LLR_BUFFER_SIZE (sizeof(amp_eng_info_type)* HS_NUM_MAX_HS_AMP_ENG_SAMPLES)

typedef PACKED struct PACKED_POST
{
  /*Amplitude estimate 
   (averaged across multiple symbols and codes) 
   in 13Q4 format*/
  uint32 amp_est :13;
  /*Energy estimate 
    (averaged across multiple symbols and codes)
    in 19Q0 format*/
  uint32 eng_est :19;
}amp_eng_type;

typedef PACKED struct PACKED_POST
{
  /*Demfront mode:
      0: Equalizer 
      1: Rake
      2: QICE
  */
  uint8 demfrnt_mode     :2;
  /*HSDPA Modulation scheme. 
      0: QPSK
      1: 16QAM
      2: 64QAM
     */
  uint8 hsdpa_modulation :2;
  /*New Transmission indicator bit(s)
      0: Old transmission
      1: New transmission
    */
  uint8 new_trans        :1;
  /*TD scale*/
  uint8 td_scale         :3;

  /*Amplitude and engergy per carrier*/
  amp_eng_type amp_eng_per_carr[MAX_AMP_ENG_MEAS_PER_SAMPLE];
  
}demod_info_type;
typedef PACKED struct PACKED_POST
{
  uint16 sf_number :11;
  uint16 num_carr  :2;
  uint16           :3; /*reserved*/
  demod_info_type demod_info_per_carr[L1_HSDPA_MAX_CARRIERS]; 
}amp_eng_info_type;

LOG_RECORD_DEFINE(HS_AMP_ENG_EST_LOG_PKT)
    /*Version*/
  uint8 version;
    /* Number of samples in this packet*/
  uint8 num_samples;
  /*Sample Period (T)*/
  uint8 sample_period  :2;
  /*number of samples per sub frame*/
  uint8 num_samples_sf :3;
  /*reserved*/
  uint8                :3; 
  
  /* amplitude n energy samples */
  uint8 llr_buffer[MAX_LLR_BUFFER_SIZE];

LOG_RECORD_END
/* ----------------------------- */
/* HS-MIMO channel analysis log packet */

/* ----------------------------- */

#define HS_MIMO_CHAN_DS_RATE_BMSK 0x0000FFFF
#define HS_MIMO_CHAN_DS_RATE_SHFT 0

#define HS_MIMO_CHAN_SS_RATE_BMSK 0x7FFF0000
#define HS_MIMO_CHAN_SS_RATE_SHFT 16

#define HS_MIMO_CHAN_DS_SS_PREF_BMSK 0x80000000
#define HS_MIMO_CHAN_DS_SS_PREF_SHFT 31

#define HS_MIMO_CHAN_SET_VAL(buf, field, val) \
  (buf) &= (~HS_MIMO_CHAN_##field##_BMSK); \
  (buf) |= (((val) << HS_MIMO_CHAN_##field##_SHFT) & \
            (HS_MIMO_CHAN_##field##_BMSK))

#define HS_MIMO_LOG_MAX_SAMPLE 25
#define HS_MIMO_CHAN_LOG_MAX_SAMPLE 25
#define HS_MIMO_CH_ANALYSIS_SAMPLE_SIZE sizeof(hs_mimo_ch_analysis_struct_type)
#define HS_MIMO_CH_ANALYSIS_LOG_MAX_BUF_SIZE  ((sizeof(hs_mimo_ch_analysis_struct_type)) * \
                                           HS_MIMO_CHAN_LOG_MAX_SAMPLE)


typedef PACKED struct PACKED_POST
{
  uint16 H00_real;
  uint16 H00_imag;
  uint16 H01_real;
  uint16 H01_imag;
  uint16 H10_real;
  uint16 H10_imag;
  uint16 H11_real;
  uint16 H11_imag;
  uint32 Rnn00_real;
  uint32 Rnn01_real;
  uint32 Rnn01_imag;
  uint32 Rnn11_real;
  uint32 supported_rate;
}hs_mimo_ch_analysis_struct_type;


LOG_RECORD_DEFINE(HS_MIMO_CH_ANALYSIS_LOG_PKT)
  /* Version field*/
  uint8 version;
  /* Number of samples*/
  uint8 num_samples;
  /*starting sub frame number and sample period
    Bit 0:10 sub frame number
    Bit 11:15 sample period
  */

  uint16 starting_sub_fn_and_sp;
  /* */

  uint8 info[HS_MIMO_CH_ANALYSIS_LOG_MAX_BUF_SIZE];
LOG_RECORD_END

/* ----------------------------- */
/* HS-MIMO demod log packet */
/* ----------------------------- */

typedef PACKED struct PACKED_POST
{
  uint32 seqx0_part1;
  uint16 seqx0_part2;
} seq_x_type;

typedef PACKED struct PACKED_POST
{
  uint32 seqy0_part1;
  uint16 seqy0_part2;
} seq_y_type;

typedef PACKED struct PACKED_POST
{
  uint32 rw_parts_part1;
  uint16 rw_parts_part2;
  uint8 rw_parts_part3;
} rw_type;

typedef PACKED struct PACKED_POST
{
  uint8 mimo_control_info;
  uint16 H00_real;
  uint16 H00_imag;
  uint16 H01_real;
  uint16 H01_imag;
  uint16 H10_real;
  uint16 H10_imag;
  uint16 H11_real;
  uint16 H11_imag;
  uint16 Ryy00_real;
  uint16 Ryy01_real;
  uint16 Ryy01_imag;
  uint16 Ryy11_real;

  uint32 seq0001_part1;
  uint16 seq0001_part2;
  uint32 seq1011_part1;
  uint16 seq1011_part2;  

  uint8 seq_no_of_roundbits;

  seq_x_type seq_x;
  uint8 seq_no_of_roundbits_rowx;

  seq_y_type seq_y;
  uint8 seq_no_of_roundbits_rowy;

  rw_type rw[4];
} hs_mimo_demod_struct_type;

#define HS_MIMO_DEMOD_SAMPLE_SIZE sizeof(hs_mimo_demod_struct_type)
#define HS_MIMO_DEMOD_LOG_MAX_BUF_SIZE  ((sizeof(hs_mimo_demod_struct_type)) * \
                                           HS_MIMO_LOG_MAX_SAMPLE)

#define HS_LOG_MIMO_DEMOD_CTRL_PDSCH_BMSK           0x1
#define HS_LOG_MIMO_DEMOD_CTRL_PDSCH_SHFT           0

#define HS_LOG_MIMO_DEMOD_CTRL_SIC_DONE_2ND_DECODE_BMSK        0x2
#define HS_LOG_MIMO_DEMOD_CTRL_SIC_DONE_2ND_DECODE_SHFT        1

#define HS_LOG_MIMO_DEMOD_CTRL_SIC_DONE_3RD_DECODE_BMSK        0x4
#define HS_LOG_MIMO_DEMOD_CTRL_SIC_DONE_3RD_DECODE_SHFT        2

#define HS_LOG_MIMO_DEMOD_CTRL_ID_BMSK              0x8
#define HS_LOG_MIMO_DEMOD_CTRL_ID_SHFT              3

#define HS_LOG_MIMO_DEMOD_GENIE_CORRELATION_BMSK              0x10
#define HS_LOG_MIMO_DEMOD_GENIE_CORRELATION_SHFT              4

/* SEQ 1 */
#define HS_LOG_MIMO_DEMOD_SEQ00_REAL_BMSK           0xFFF
#define HS_LOG_MIMO_DEMOD_SEQ00_REAL_SHFT           0

#define HS_LOG_MIMO_DEMOD_SEQ00_IMAG_BMSK           0xFFF000
#define HS_LOG_MIMO_DEMOD_SEQ00_IMAG_SHFT           12

#define HS_LOG_MIMO_DEMOD_SEQ01_REAL_PART1_BMSK     0xFF000000
#define HS_LOG_MIMO_DEMOD_SEQ01_REAL_PART1_SHFT     24

#define HS_LOG_MIMO_DEMOD_SEQ01_REAL_PART2_BMSK     0xF
#define HS_LOG_MIMO_DEMOD_SEQ01_REAL_PART2_SHFT     0

#define HS_LOG_MIMO_DEMOD_SEQ01_IMAG_BMSK           0xFFF0
#define HS_LOG_MIMO_DEMOD_SEQ01_IMAG_SHFT           4

/* SEQ 2*/
#define HS_LOG_MIMO_DEMOD_SEQ10_REAL_BMSK           0xFFF
#define HS_LOG_MIMO_DEMOD_SEQ10_REAL_SHFT           0

#define HS_LOG_MIMO_DEMOD_SEQ10_IMAG_BMSK           0xFFF000
#define HS_LOG_MIMO_DEMOD_SEQ10_IMAG_SHFT           12

#define HS_LOG_MIMO_DEMOD_SEQ11_REAL_PART1_BMSK     0xFF000000
#define HS_LOG_MIMO_DEMOD_SEQ11_REAL_PART1_SHFT     24

#define HS_LOG_MIMO_DEMOD_SEQ11_REAL_PART2_BMSK     0xF
#define HS_LOG_MIMO_DEMOD_SEQ11_REAL_PART2_SHFT     0

#define HS_LOG_MIMO_DEMOD_SEQ11_IMAG_BMSK           0xFFF0
#define HS_LOG_MIMO_DEMOD_SEQ11_IMAG_SHFT           4

/* round bits */
#define HS_LOG_MIMO_DEMOD_RB_ROW0_BMSK              0xF
#define HS_LOG_MIMO_DEMOD_RB_ROW0_SHFT              0

#define HS_LOG_MIMO_DEMOD_RB_ROW1_BMSK              0xF0
#define HS_LOG_MIMO_DEMOD_RB_ROW1_SHFT              4

/* SEQ x */
#define HS_LOG_MIMO_DEMOD_SEQx0_REAL_BMSK           0xFFF
#define HS_LOG_MIMO_DEMOD_SEQx0_REAL_SHFT           0

#define HS_LOG_MIMO_DEMOD_SEQx0_IMAG_BMSK           0xFFF000
#define HS_LOG_MIMO_DEMOD_SEQx0_IMAG_SHFT           12

#define HS_LOG_MIMO_DEMOD_SEQx1_REAL_PART1_BMSK     0xFF000000
#define HS_LOG_MIMO_DEMOD_SEQx1_REAL_PART1_SHFT     24

#define HS_LOG_MIMO_DEMOD_SEQx1_REAL_PART2_BMSK     0xF
#define HS_LOG_MIMO_DEMOD_SEQx1_REAL_PART2_SHFT     0

#define HS_LOG_MIMO_DEMOD_SEQx1_IMAG_BMSK           0xFFF0
#define HS_LOG_MIMO_DEMOD_SEQx1_IMAG_SHFT           4

/* round bits */
#define HS_LOG_MIMO_DEMOD_RB_ROWx_BMSK              0xF
#define HS_LOG_MIMO_DEMOD_RB_ROWx_SHFT              0

/* SEQ y */
#define HS_LOG_MIMO_DEMOD_SEQy0_REAL_BMSK           0xFFF
#define HS_LOG_MIMO_DEMOD_SEQy0_REAL_SHFT           0

#define HS_LOG_MIMO_DEMOD_SEQy0_IMAG_BMSK           0xFFF000
#define HS_LOG_MIMO_DEMOD_SEQy0_IMAG_SHFT           12

#define HS_LOG_MIMO_DEMOD_SEQy1_REAL_PART1_BMSK     0xFF000000
#define HS_LOG_MIMO_DEMOD_SEQy1_REAL_PART1_SHFT     24

#define HS_LOG_MIMO_DEMOD_SEQy1_REAL_PART2_BMSK     0xF
#define HS_LOG_MIMO_DEMOD_SEQy1_REAL_PART2_SHFT     0

#define HS_LOG_MIMO_DEMOD_SEQy1_IMAG_BMSK           0xFFF0
#define HS_LOG_MIMO_DEMOD_SEQy1_IMAG_SHFT           4

/* round bits */
#define HS_LOG_MIMO_DEMOD_RB_ROWy_BMSK              0xF
#define HS_LOG_MIMO_DEMOD_RB_ROWy_SHFT              0

/* RW */
#define HS_LOG_MIMO_DEMOD_RW00_REAL_BMSK            0x1FFF
#define HS_LOG_MIMO_DEMOD_RW00_REAL_SHFT             0

#define HS_LOG_MIMO_DEMOD_RW00_IMAG_BMSK            0x3FFE000
#define HS_LOG_MIMO_DEMOD_RW00_IMAG_SHFT             13

#define HS_LOG_MIMO_DEMOD_RW01REALPT1_BMSK          0xFC000000
#define HS_LOG_MIMO_DEMOD_RW01REALPT1_SHFT           26

#define HS_LOG_MIMO_DEMOD_RW01REALPT2_BMSK          0x7F
#define HS_LOG_MIMO_DEMOD_RW01REALPT2_SHFT            0

#define HS_LOG_MIMO_DEMOD_RW01IMAGPT1_BMSK          0xFF80
#define HS_LOG_MIMO_DEMOD_RW01IMAGPT1_SHFT            7

#define HS_LOG_MIMO_DEMOD_RW01IMAGPT2_BMSK          0xF
#define HS_LOG_MIMO_DEMOD_RW01IMAGPT2_SHFT            0

#define HS_LOG_MIMO_DEMOD_SET_VAL(buf, field, val) \
  (buf) &= (~HS_LOG_MIMO_DEMOD_##field##_BMSK); \
  (buf) |= (((val) << HS_LOG_MIMO_DEMOD_##field##_SHFT) & \
            (HS_LOG_MIMO_DEMOD_##field##_BMSK))


LOG_RECORD_DEFINE (HS_MIMO_DEMOD_LOG_PKT)
  /* Version field*/
  uint8 version;
  /* Number of samples*/
  uint8 num_samples;
  /*starting sub frame number and sample period
    Bit 0:10 sub frame number
    Bit 11:15 sample period
  */
  uint16 starting_sub_fn_and_sp;
  /* */
  /* Bit 0:2 ; integer value from 0 to 6db*/
  uint8 Scpich_Pcpich_Power_Imbalance;
  uint8 info[HS_MIMO_DEMOD_LOG_MAX_BUF_SIZE];
LOG_RECORD_END

/* This enum defines various actions for HS stat generation */
typedef enum 
{
  HS_STAT_GEN_NOOP,
  HS_STAT_GEN_START,
  HS_STAT_GEN_RESTART,
  HS_STAT_GEN_STOP
} hs_stat_gen_action_enum_type;

/* This enum defines various actions for HS UL DPCCH logging */
typedef enum 
{
  ENCHS_LOG_NOOP,
  ENCHS_LOG_START,
  ENCHS_LOG_RESTART,
  ENCHS_LOG_STOP
} enchs_log_action_enum_type;

/*used by dpcch hs logging, to have a local ul hs call config*/
typedef struct
{
  /*dpcch offset */
  uint8 dpcch_offset;
  /*num of configured carriers*/
  uint8 num_config_carriers;
}enc_hs_call_info_struct_type;

#ifdef FEATURE_QSH_EVENT_METRIC
/* Used for QSH metric collection - maps from WL1_QSH_* to
   hs_metric_db_struct_type array index*/
typedef enum
{
  HS_METRIC_CQI = 0,
  HS_METRIC_BLER,
  HS_METRIC_TBSIZE,

  HS_METRIC_COUNT
} hs_metric_enum_type;

/* Generic data for each metric */
typedef struct
{
  boolean enable;
  uint32 total;
  uint32 sample_count;
} hs_metric_db_struct_type;

#endif /* FEATURE_QSH_EVENT_METRIC */

/* -----------------------------------------------------------------------
** extern Declarations
** ----------------------------------------------------------------------- */
/* stores the start CFN for the HS combiner */
extern uint8 hs_cm_log_dl_start_cfn;
/* Holds the DL HS stop subframe number */
extern uint16 hs_cm_log_dl_stop_sub_fr_num;
/* stores the start CFN for the HS UL */
extern uint8 hs_cm_log_ul_start_cfn;
/* Holds the UL HS stop subframe number */
extern uint16 hs_cm_log_ul_stop_sub_fr_num;
/* variable to hold the HS+CM operation type */
extern enchs_log_action_enum_type hs_cm_log_ul_action;
/* variable to hold the HS+CM operation type */
extern hs_stat_gen_action_enum_type hs_cm_log_dl_action;
/*********************APIs exposed by HSlog.c******************************/

/* -------------------------------------------------------- */
/* Following functions handle HS statistics and its logging */
/* -------------------------------------------------------- */

/*===========================================================================
FUNCTION dec_hs_set_pess_cqi_accum_active

DESCRIPTION
  This function initialize the value of the global dec_hs_set_pess_cqi_accum_active for 
  HS pessimistic CQI logging

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Variables get inited to their initial values
===========================================================================*/

extern void dec_hs_set_pess_cqi_accum_active(boolean accum_status);
/*===========================================================================
FUNCTION dec_hs_stat_init

DESCRIPTION
  This function initializes the variables required for generation and
  accumulation of HS decode status and stats. It initialize the following
  variables

  hs_stat_accum_active is set to FALSE to indicate inactive
  hs_stats_gen_action is set to NOOP
  All HSDPA status log buffers are set to available

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All above mentioned variables in descriptions are inited
===========================================================================*/

extern void dec_hs_stat_init(void);


/*===========================================================================
FUNCTION dec_hs_stat_do_action

DESCRIPTION
  This function changes HSDPA decode status and stat generation mode. Before
  it commit action provided function argument, it validates if that action
  can be accepted. Current action should be always NOOP that is reset at
  start and after every action completion
  
  When stat generation is inactive, only action that is accepted is START.
  When stat generation is active it can be RESTARTED or STOPPED
  
  There are 2 buffer for accumulating HS decode status. These are used when
  a existing accumulation need to be submitted for logging. It happens when
  accumulation is done or there is reconfiguration and previous configuration
  need to be flushed.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on action accept validation

SIDE EFFECTS
  None
===========================================================================*/
extern boolean dec_hs_stat_do_action(
  /* Action for HS decode statu accumulation and stat generation */
  hs_stat_gen_action_enum_type action,
  /* Sub frame number at which to start or restart */
  uint16 start_sub_fn,
  /* start CFN to start or restart accumulation/stat generation */
  uint16 start_cfn,
  /* mDSP info table index from where to read the information */
  hs_dl_channel_per_carrier_struct_type  *info_table_ptr,
  /* number of carries, used to access idx array*/
  uint8  num_carr,
  /* Last sub frame number after which information is submitted for logging
     then either restarted ot stopped based on action */
  uint16 final_sub_fn);

/*===========================================================================
FUNCTION dec_hs_prep_log_action

DESCRIPTION
  HS logging start: provide valid info table ptr and Final SFN = 0xFFFF
  HS logging stop: provide valid SFN to stop 
DEPENDENCIES
  Hs config tables.

RETURN VALUE
  void

SIDE EFFECTS
  Max one start and stop action in 1 Radio Frame (10msec)
===========================================================================*/


extern void dec_hs_prep_log_action(
  
  /* mDSP pointer to info table idx array, from where to read the information */
  hs_dl_channel_per_carrier_struct_type  *info_table_ptr,
  /*number of carries, used to access idx array*/
  uint8  num_carr,
  /* Last sub frame number after which information is submitted for logging
   then either restarted ot stopped based on action */
  uint16 final_sub_fn);

/*===========================================================================
FUNCTION dec_hs_decode_update_stats

DESCRIPTION
  This function is called periodically every frame to act on new action
  information, if set and do periodic decode status accumulation and stat
  generation.
  Flush flag is used to force flush of LLR logging when HS channel is stopped.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_decode_update_stats(boolean force_flush);

/*===========================================================================
FUNCTION dec_hs_log_pess_cqi

DESCRIPTION
  This function checks the log code in the diag server and if enabled then
  it collects the next sample and fills it in the log buffer. Once the log 
  buffer is full it flushes the log buffer. When the log buffer is empty and
  the first sample has to be logged, it initializes all the buffers and state
  variables.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  dec_hs_pess_cqi_first_sample is changed to FALSE if it was TRUE after 
  initializing the log buffers and state variables.
  If the command was to log next sample, num_samples is incremented.
===========================================================================*/

extern void dec_hs_log_pess_cqi(void);

/*===========================================================================
FUNCTION dec_hs_flush_pess_cqi_log_pkt

DESCRIPTION
  This function computes the length of the log buffer being submitted and 
  then posts a local command to L1 to this log packet. It does a 
  sanity testing to see that the log packet is not empty before submitting.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Flag indicating log packet is submitted as well as that the next sample
  will be the first sample is made TRUE.
===========================================================================*/

extern void dec_hs_flush_pess_cqi_log_pkt(void);



/*===========================================================================
FUNCTION dec_hs_scch_submit_log_buffer

DESCRIPTION
  This function is called to post HS scch submit command. If the 
  buffer is not ready to be submitted to diag the function returns without
  doing anything. This can so happen because multiple logs are instructed
  to be submitted to diag by the same L1 command.
  It checks for the log code in diag and submits the packet.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Flag indicating the log buffer is submitted for flushing is reset.
===========================================================================*/

extern void dec_hs_scch_submit_log_buffer(void);

/*===========================================================================
FUNCTION dec_hs_pess_cqi_submit_log_buffer

DESCRIPTION
  This function is called to post HS pessimistic CQI submit command. If the 
  buffer is not ready to be submitted to diag the function returns without
  doing anything. This can so happen because multiple logs are instructed
  to be submitted to diag by the same L1 command.
  It checks for the log code in diag and submits the packet.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Flag indicating the log buffer is submitted for flushing is reset.
===========================================================================*/

extern void dec_hs_pess_cqi_submit_log_buffer(void);

/*===========================================================================
FUNCTION dec_hs_decode_status_send_log_submit_cmd

DESCRIPTION
  This function is called to post HS log submit command. There is only one HS
  log submit command. HS cfg module calls all accumlated logging entities. Each
  entity is reponsible for maintaining log submit pending information. It keeps
  this information for HS decode status log submit pending information
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_decode_status_send_log_submit_cmd(
  /* HS decode status log buffer index to submit */
  uint8 decode_status_info_buf_idx);

/*===========================================================================
FUNCTION dec_hs_decode_status_submit_log_buffer

DESCRIPTION
  This function is called from hscfg module in response of HS log submit
  command. It checks for hs decode status log submit flag. It is set to TRUE,
  it submit log buffer pointed by lof buffer index to submit.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_decode_status_submit_log_buffer(void);


/*===========================================================================
FUNCTION dec_hs_quantize_cqi

DESCRIPTION
  This function quantize CQI from lowest value of 1 to max value of 30.
  Unquantized value is 10 bit number in 6.4 format (10Q4).
  
DEPENDENCIES
  None

RETURN VALUE
  6 Bit unquantized value form 1 to 30

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 dec_hs_quantize_cqi(
  /* Unquantized CQI value */
  uint16 unquan_cqi);

/*===========================================================================
FUNCTION dec_hs_init_stats

DESCRIPTION
  This function initialize stats information.
  
  It is called when ever HS stat generation is started at START or RESTART
  action OR
  when ever stats have been generated using max required number of samples
  
DEPENDENCIES
  None

RETURN VALUE
  5 Bit unquantized value form 1 to 30

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_init_stats(void);

/*===========================================================================
FUNCTION dec_hs_gen_stats

DESCRIPTION
  This function generate various stats for HS downlink channels.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_gen_stats(
  /* HS decode status structure pointer */
  dec_hs_decode_status_struct_type *decode_status);

/*===========================================================================
FUNCTION dec_hs_submit_stats

DESCRIPTION
  This function submits the HS downlink channel stats to logging. This is
  done when HS stat gen/accumulation action is STOP or RESTART or when stats
  have been generated using max number of samples.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_submit_stats(void);


/*===========================================================================
FUNCTION dec_hs_mac_hs_hdr_send_log_submit_cmd

DESCRIPTION
  This function is called to post MAC-hs header log submit command. There is
  only one HS log submit command. HS cfg module calls all accumlated logging
  entities. Each entity is reponsible for maintaining log submit pending
  information. It keeps this information for HS decode status log submit
  pending information
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_mac_hs_hdr_send_log_submit_cmd(
  /* HS decode status log buffer index to submit */
  uint8 mac_hs_hdr_info_buf_idx);

/*===========================================================================
FUNCTION dec_hs_mac_hs_hdr_submit_log_buffer

DESCRIPTION
  This function is called from hscfg module in response of HS log submit
  command. It checks for MAC-hs header log submit flag. It is set to TRUE,
  it submit log buffer pointed by log buffer index to submit.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_mac_hs_hdr_submit_log_buffer(void);

/*===========================================================================
FUNCTION dec_hs_flush_mac_hs_log_pkt

DESCRIPTION
  This function is called from hscfg module to flush MAC hs header log packet
  if any accumulation is pending. It is called when there is switch in DL
  HS config at any reconfig or stop action.
  Note that variables accessed by this functions are shared with function
  that reads TB header read. However both functions are event on Rx timeline
  so there for they are mutually exclusive.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_flush_mac_hs_log_pkt(void);

/*===========================================================================
FUNCTION dec_hs_llr_log_init

DESCRIPTION
  This function initializes all the variables and states associated with HS
  Amplitude and energy Estimation log packet. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All state variables initialized to the default values.
===========================================================================*/

extern void dec_hs_llr_log_init(void);

/*===========================================================================
FUNCTION dec_hs_llr_update_log

DESCRIPTION
  This function is responsible reading A&E log data from FW and 
  populating SW log packet.
  
DEPENDENCIES
  force_flush flag is used for flush the log packet till the last 
  idx populated by FW and stopping.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_llr_update_log(boolean force_flush);

/*===========================================================================
FUNCTION dec_hs_llr_send_log_submit_cmd

DESCRIPTION
  This function makes a request to post a local command to submit the 
  currently accumulated log packet to diag. Before it does so, it checks if 
  a previous log packet that was submitted to diag has been  serviced or not.
  If not, it drops the current packet and proceeds further.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_llr_send_log_submit_cmd(
  /* HS decode status log buffer index to submit */
  uint8 hs_llr_log_idx);

/*===========================================================================
FUNCTION dec_hs_llr_submit_log_buffer

DESCRIPTION
  This function is called from hscfg module in response of HS log submit
  command. It checks for HS AMP and ENG estimation log submit flag. 
  If it is set to TRUE, it submit log buffer pointed by log buffer index 
  to submit.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dec_hs_llr_submit_log_buffer(void);


/*===========================================================================
FUNCTION hslog_enchs_init

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void hslog_enchs_init();


#ifdef FEATURE_WCDMA_MIMO
/*===========================================================================
FUNCTION dec_hs_mimo_demod_submit_log_buffer

DESCRIPTION
  This function is called from hscfg module in response of HS log submit
  command. It checks for MIMO demod log submit flag. 
  If it is set to TRUE, it submit log buffer pointed by log buffer index 
  to submit.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_mimo_demod_submit_log_buffer(void);

/*===========================================================================
FUNCTION dec_hs_mimo_chan_submit_log_buffer

DESCRIPTION
  This function is called from hscfg module in response of HS log submit
  command. It checks for MIMO chan log submit flag. 
  If it is set to TRUE, it submit log buffer pointed by log buffer index 
  to submit.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_mimo_chan_submit_log_buffer(void);

#endif

/*===========================================================================
FUNCTION dec_hs_register_hs_scch_stats_cb

DESCRIPTION
  Registers a function called when HS stats are generated.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_register_hs_scch_stats_cb(DEC_HS_CME_CME_CB_FUNC_TYPE cme_cb);

/*===========================================================================
FUNCTION dec_hs_deregister_hs_scch_stats_cb

DESCRIPTION
  Deregisters the function called when HS stats are generated.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_deregister_hs_scch_stats_cb(void);
/* ---------------------------------------------------------- */
/* Following functions handle UL HS DPCCH information logging */
/* ---------------------------------------------------------- */

/*===========================================================================
FUNCTION enchs_logging_init

DESCRIPTION
  This function initializes the variables required for accumulation 
  and logging of HS UL logging. It initialize the following variables
  
  enchs_log_accum_active is set to FALSE to indicate inactive
  enchs_log_action is set to NOOP
  All HSDPA UL log buffers are set to available
  Log to submit request id set to FALSE
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All above mentioned variables in descriptions are inited
===========================================================================*/

extern void enchs_logging_init(void);

/*===========================================================================
FUNCTION enchs_log_do_action

DESCRIPTION
  This function changes HSDPA UL logging mode. Before it commit action
  provided function argument, it validates if that action can be accepted.
  Current action should be always NOOP that is reset at start and after every
  action completion
  
  When logging is inactive, only action that is accepted is START.
  When logging is active it can be RESTARTED or STOPPED
  
  There are 2 buffer for accumulating HS UL logging. These are used when
  a existing accumulation need to be submitted for logging. It happens when
  accumulation is done or there is reconfiguration and previous configuration
  need to be flushed.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on action accept validation

SIDE EFFECTS
  None
===========================================================================*/

extern boolean enchs_log_do_action(
  /* Action for HS UL accumulation and logging */
  enchs_log_action_enum_type action,
  /* Sub frame number at which to start or restart */
  uint16 start_sub_fn,
  /* start CFN to start or restart accumulation/logging */
  uint16 start_cfn,
  /* mDSP info table index from where to read the information */
  uint8  info_table_index,
  /* Last sub frame number after which information is submitted for logging
     then either restarted ot stopped based on action */
  uint16 final_sub_fn,
  /*dpcch offset */
  uint8 dpcch_offset,
  /*num of configured carriers*/
  uint8 num_config_carriers);

/*===========================================================================
FUNCTION enchs_do_logging

DESCRIPTION
  This function is called periodically every frame to act on new action
  information, if set and do periodic log information accumulation.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void enchs_do_logging(void);

/*===========================================================================
FUNCTION enchs_dpcch_flush_log_pkt

DESCRIPTION
  This function flushes the DPCCH log packets if the UL timeline
  events are disabled and the HSDPA logging is still active. This can happen
  in the DCH to DCH transition. If even before the CFN is reached at which
  logging has to be stopped, the UL event timeline is suspended because of HHO
  measurements or any such conditions, this function will gracefully flush
  the packets and suspend logging.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void enchs_dpcch_flush_log_pkt(void);

/*===========================================================================
FUNCTION enchs_dpcch_send_log_submit_cmd

DESCRIPTION
  This function is called to post HS log submit command. There is only one HS
  log submit command. HS cfg module calls all accumlated logging entities. Each
  entity is reponsible for maintaining log submit pending information. It keeps
  this information for HS UL DPCCH log submit pending information
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void enchs_dpcch_send_log_submit_cmd(
  /* HS decode status log buffer index to submit */
  uint8 ul_hs_info_buf_idx);

/*===========================================================================
FUNCTION enchs_submit_log_buffer

DESCRIPTION
  This function is called from hscfg module in response of HS log submit
  command. It checks for HS UL DPCCH log submit flag. It is set to TRUE,
  it submit log buffer pointed by log buffer index to submit.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void enchs_submit_log_buffer(void);

/*===========================================================================
FUNCTION hs_cm_mod_ctrl_submit_log_buffer

DESCRIPTION
  This function is submits the HS+CM UL control log buffer. After successful
  submission, it releases the resources held up by the log submit operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The log buffer availability status as well as the submit pending flags are
  cleared upon successful log submit.
===========================================================================*/

extern void hs_cm_mod_ctrl_submit_log_buffer();

/*===========================================================================
FUNCTION mcalwcdma_dec_hs_mac_hs_hdr_logging_init

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_dec_hs_mac_hs_hdr_logging_init();

/*===========================================================================
FUNCTION dec_hs_invalid_logging_stop_gsfn

DESCRIPTION
  invalidates stop gsfn values for logging. used when HS is torn down.
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void dec_hs_invalid_logging_stop_gsfn();

/*===========================================================================
FUNCTION dec_hs_set_pess_cqi_action

DESCRIPTION
  This function sets stat generation mode.
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on action accept validation

SIDE EFFECTS
  None
===========================================================================*/
extern boolean dec_hs_set_pess_cqi_action(hs_stat_gen_action_enum_type action);

/*===========================================================================
FUNCTION dec_hs_get_pess_cqi_action

DESCRIPTION
  This function returns the action that HS Pess CQI state machine needs
  to perform next.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern hs_stat_gen_action_enum_type dec_hs_get_pess_cqi_action(void);

/*===========================================================================
FUNCTION dec_hs_pess_cqi_log_init
 
DESCRIPTION This function initializes the log states for pess cqi 
            log packet
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_pess_cqi_log_init(void);

/*===========================================================================
FUNCTION dec_hs_scch_log_init
 
DESCRIPTION This function initializes the scch log init flag and final sub frame.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dec_hs_scch_log_init(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wl1_datapath_dump_cb

DESCRIPTION
  This function dumps crash dump info into the
  wl1_datapath_mini_debug_info_struct_type structure 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_datapath_dump_cb(void* addr_ptr);
#endif /* FEATURE_QSH_DUMP */

#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
FUNCTION wl1_hdspa_metric_cb

DESCRIPTION
  This function performs the requested QSH action

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean wl1_hdspa_metric_cb(wl1_qsh_metric_e metric_id, uint8* addr_ptr, wl1_qsh_metric_action_enum_type action);
#endif /* FEATURE_QSH_EVENT_METRIC */

#endif /* HSLOG_H */
