#ifndef LOG_CODES_WCDMA_INT_H
#define LOG_CODES_WCDMA_INT_H

/*===========================================================================

                         Log Code Definitions

General Description
  This file contains log code definitions for WCDMA.

Copyright (c) 2003-2010 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/log_codes_wcdma_int.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/16   nr      Adding log packet for standalone HRB db
05/12/16   vs      Adding log packet for measurement related logging
05/09/16   nr      Adding log packet for hanging RB logging 0x4804
06/23/15   aa      Changes for new log packet 0x41CE
06/09/15   aa      Replaced RLC UM/AM/TM config log packets with new packets
01/14/15   aa      Changes for new ULC log packet(0x41C5)
01/05/15   cc      New energy to EcIo formula for interF step1 0x41C6 and PN 0x417A
01/04/15   gm      0x41c1 log packet changes
11/10/14   sl      Code changes for new f/w log packet 0x41C4 V1
11/06/14   aa      Replaced 0x4321 log packet with enhanced version 0x41C2
08/12/14   vn      Added log:0x41BC to accomodate Narrow Band Rejection results.
06/20/14   ymu     Enhancement of the log packet 0x4307 to 0x435D
06/09/14   db      RRC Logging Changes for CMAC and CRLC
05/05/14   rs      FR-3672 CTCH log packet changes. 
03/19/14   dp      Add support for tgpsi activity logging
01/23/14   gsk     Adding support for ngbr srch opt log pkt
01/07/14   as      New log packet WL1TRM_TUNEAWAY_LOG_PKT for tuneaway report logging.
12/17/13   yw      Change R-combo log code to 0x435C for boost feature
11/15/13   hdk     Added log:0x41BA to accomodate FW changes
10/18/13   as      Continuing port of FEATURE_WCDMA_MODEM_COEXISTENCE_SW
08/07/13   kc      Log packet code to reduce F3s logged for MAC UL ciphering parameters
07/23/13   vs      Defined the log codes 0x41B4 and 0x41B5.
05/23/13   geg     WSW decoupling (except for component of QCHAT)
05/22/13   oh      Adding DC-HSUPA framework support
04/09/13   vs      New log pkt 0x41b1 wcdma_statistics_log_pkt
09/06/12   sr      New log pkt 0x4198 WCDMA_ULTPC_DEBUG_PKT
03/27/13   vn      New log packet for frequency scan.
02/28/13   sn      New log packet for CSG finger printing information
02/19/13   zr      Defining 0x41AC log packet to add dual carrier support and replace 0x4110
02/19/13   ar      Define 0x41AE (csg asf_meas_req) and 0x41AF (csg cell_reselection) log packets
09/24/12   zr      Defining 0x41AC log packet to add dual carrier support and replace 0x4110
09/23/12   pv      Added DRX_PICH_LOG_PKT.
09/06/12   sr      New log pkt 0x4198 WCDMA_ULTPC_DEBUG_PKT
12/03/12   pr      Added changes for 0x4778 Log Packet.
08/23/12   mrg     RLC UM logging change
08/22/12   kc      New log packet 0x4314 for HS RACH MAC timer info logging
09/24/12   zr      Defining 0x41AC log packet to add dual carrier support and replace 0x4110
08/14/12   zr      Adding log IDs to reduce F3 logging from searcher
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/02/12   cc      New log packet 0x417A for W2W CM PN search results
06/18/12   mrg     L2 - MAC-I and HS-RACH code changes
06/08/12   gv      Feature HS RACH Initial code check in
05/03/12   vsr     FET/DED/ED-V2 Feature checkin
05/9/12    vs      Added the support for the new hs-scch log packet(0x423f).
03/28/12   hk      Added support for FEATURE_WCDMA_HS_FACH_DRX
03/14/12   zr      Added PN bypass peak info log packet
03/06/12   vsr     New RxD CTRL log packet (0x430D)
01/17/12   vsr     Added ARD log packet (0x430C)
06/17/11   sa      Added log pkt code 0x417A for all cells PN search. 
04/29/11   geg     Moved #define WCDMA_AGC_0x4176_LOG_PACKET back to log_codes_wcdma_int.h
04/25/11   geg     Moved #define WCDMA_AGC_0x4176_LOG_PACKET to log_codes_wcdma.h from log_codes_wcdma_int.h
04/06/11   mr      Added support for absolute priority cell Reselection log packet (0x4027)
03/16/11   amj     Added log code for CPC DRX CFG log packet.
01/17/11   sp      Added support for UL_RACH_DEBUG_LOG_PKT (0x4025)
01/10/11   sp      Adding back log codes (0x4180, 0x4181, 0x4182) as they are
                   needed for online targets
01/07/11   sp      Added log code for new UL RACH FW Log Pkt (0x4026)
                   Removed unused rach log codes (0x4180, 0x4181, 0x4182)
11/15/10   asm     Added log code for new db10 AGC log packet
10/15/10   ks      Added log code for new power control log packet
09/27/10   asm     Added Per-slot TxAGC logging support
09/17/10   ms      Added support for HS_SCCH_DECODE_STATUS_LOG_PKT
09/20/10   sp      Added support for R99 UL Data dump Log packet 0x417F
09/19/10    av     HS_FACH and Enhanced 0x4220 log changes
09/02/10   gnk     Checking in the changes for UL open loop MTD log packet
08/27/10   vsr     EWC Debug log code changed from 0x4233 to 0x423E
08/16/10    dm     Added definition for WL1_DL_DRXCTRL_LOG_PKT
07/15/10   vsr     Updated Reg EQ log packet code to 0x4238
06/30/10   vsr     Updated EQ logging for Dual Carrier
05/03/10   rvs     Add log code for new revision of CME_DBG_SCHIC_PKT, 0x418A.
04/02/10   scm     Add WCDMA_XO_LOG_PKT (0x4776).
03/16/10    ms     Created separate definitions for different log codes for
                   HS_SCCH_STAT and HS_CQI_STAT
03/11/10    rc     Added definition WCDMA_SCHIC_DBG_LOG_PKT.
03/10/10   gnk     Added back the missing CPC DTX feature log packet codes
02/09/10    sv     Added definitions for different NASTT and PN packets.
11/23/09    nd     Added definition WCDMA_AGC_0x4176_LOG_PACKET
11/23/09    nd     Added definition SRCH_STEP1_0x4177_LOG_PACKET
                   changed SRCH_PN_LOG_PACKET to 0x4179
11/21/09    ms     Changed HS_SCCH_STAT_LOG_PKT to 0x421F
                   Changed HS_CQI_STAT_LOG_PKT to 0x422E
09/28/09   sup     Put together all the WCDMA log code definitions

===========================================================================*/

#include "log_codes_wcdma.h" //for LOG_WCDMA_BASE_C and LOG_WCDMA_LAST_C 


/* ========================   LAYER 1 PACKETS   ======================== */


#define L1_CM_GSM_MEAS_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x000)
#define WCDMA_FINGER_TA_INFO_0x4003_LOG_PACKET           (LOG_WCDMA_BASE_C + 0x003)
#define DRX_LOG_PKT                                      (LOG_WCDMA_BASE_C + 0x004)
#define SRCH_CELL_RESEL_RANK_LOG_PKT                     (LOG_WCDMA_BASE_C + 0x005)
#define WCDMA_FINGER_TA_INFO_0x4006_LOG_PACKET           (LOG_WCDMA_BASE_C + 0x006)
#define WCDMA_RXD_ANT_RADIATION_STATS_LOG                (LOG_WCDMA_BASE_C + 0x008)
#define L1_MULTI_COMB_TPC_PKT                            (LOG_WCDMA_BASE_C + 0x011)
#define WCDMA_SAMPSRV_LOG_PACKET                         (LOG_WCDMA_BASE_C + 0x012)
#define DL_TRCH_CM_BLER_LOG_PKT                          (LOG_WCDMA_BASE_C + 0x013)
#define WCDMA_AGC_0x4015_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x015)
#define WCDMA_FINGER_TA_INFO_0x4016_LOG_PACKET           (LOG_WCDMA_BASE_C + 0x016)
#define SRCH_LIST_0x4017_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x017)
#define SRCH_STEP1_0x4018_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x018)
#define SRCH_STEP2_0x4019_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x019)
#define SRCH_STEP3_0x401A_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x01A)
#define L1_RXD_MEAS_LOG_PKT                              (LOG_WCDMA_BASE_C + 0x01B)
#define SRCH_LIST_0x401C_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x01C)
#define L1_FDPCH_DEBUG_PKT                               (LOG_WCDMA_BASE_C + 0x01D)
#define SRCH_PSC_SCAN_PILOT_INFO_LOG_PKT                 (LOG_WCDMA_BASE_C + 0x01E)
#define L1_CLTD_0x401F_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x01F)
#define MCALWCDMA_DEMOD_FINGER_PKT                       (LOG_WCDMA_BASE_C + 0x020)
#define MCALWCDMA_DEMOD_STATUS_DUMP_PKT                  (LOG_WCDMA_BASE_C + 0x021)
#define L1_BLER_FILTER_PKT                               (LOG_WCDMA_BASE_C + 0x022)
#define WCDMA_POWER_CTRL_SYNC_LOG_PKT                    (LOG_WCDMA_BASE_C + 0x023)
#define UL_RACH_DEBUG_LOG_PKT                            (LOG_WCDMA_BASE_C + 0x025)
#define UL_FW_RACH_DEBUG_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x026)
#define SRCH_ABS_PRIO_CELL_RESEL_RANK_LOG_PKT            (LOG_WCDMA_BASE_C + 0x027)
#define WCDMA_AGC_0x4031_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x031)
#define UL_PER_SLOT_TXAGC_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x035)
#define WCDMA_AGC_0x4105_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x105)
#define UL_RACH_PROC_0x4106_LOG_PKT                      (LOG_WCDMA_BASE_C + 0x106)
#define DL_CCTRCH_0x4108_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x108)
#define UL_CCTRCH_LOG_PKT                                (LOG_WCDMA_BASE_C + 0x109)
#define WCDMA_COMMON_DL_PHYCHAN_LOG_PACKET               (LOG_WCDMA_BASE_C + 0x10A)
#define WCDMA_DED_DL_PHYCHAN_LOG_PACKET                  (LOG_WCDMA_BASE_C + 0x10b)
#define UL_PHYCHAN_DPCH_LOG_PKT                          (LOG_WCDMA_BASE_C + 0x10C)
#define UL_PHYCHAN_PRACH_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x10D)
#define UL_TRCH_CODING_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x10E)
#define ASET_LOG_PACKET                                  (LOG_WCDMA_BASE_C + 0x110)
#define NSET_LOG_PACKET                                  (LOG_WCDMA_BASE_C + 0x111)
#define DL_TRCH_DEBUG_0x4112_LOG_PKT                     (LOG_WCDMA_BASE_C + 0x112)
#define DL_TBSET_STATUS_0x4113_PKT                       (LOG_WCDMA_BASE_C + 0x113)
#define DL_TFC_0x4115_LOG_PKT                            (LOG_WCDMA_BASE_C + 0x115)
#define DL_TRCH_BLER_LOG_PKT                             (LOG_WCDMA_BASE_C + 0x116)
#define L1_TPC_ACCUM_LOG_PKT                             (LOG_WCDMA_BASE_C + 0x12D)
#define MDSP_CMD_LOG_PACKET                              (LOG_WCDMA_BASE_C + 0x136)
#define SRCH_LIST_0x413F_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x13F)
#define SRCH_STEP1_0x414C_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x14C)
#define SRCH_STEP2_0x414D_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x14D)
#define SRCH_STEP3_0x414E_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x14E)
#define SRCH_LIST_0x414F_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x14F)
#define L1_WCDMA_INTERF_STEP1_0x4155_SRCH_LOG_PKT        (LOG_WCDMA_BASE_C + 0x155)
#define L1_WCDMA_INTERF_LIST_0x4156_SRCH_LOG_PKT         (LOG_WCDMA_BASE_C + 0x156)
#define UL_RACH_PROC_0x4160_LOG_PKT                      (LOG_WCDMA_BASE_C + 0x160)
#define UL_TFC_PWR_ELMN_LOG_PKT                          (LOG_WCDMA_BASE_C + 0x163)
#define WCDMA_AGC_0x4165_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x165)
#define WCDMA_AGC_0x4176_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x176) 
#define UL_DPCH_PWR_INFO_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x166)
#define L1_CLTD_0x4167_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x167)
#define WCDMA_COMBINER_TIMING_LOG_PACKET                 (LOG_WCDMA_BASE_C + 0x16B)
#define SRCH_LIST_0x4170_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x170)
#define DL_CCTRCH_0x4171_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x171)
#define DL_TRCH_DEBUG_0x4172_LOG_PKT                     (LOG_WCDMA_BASE_C + 0x172)
#define DL_TFC_0x4173_LOG_PKT                            (LOG_WCDMA_BASE_C + 0x173)
#define L1_WCDMA_INTERF_STEP1_0x4174_SRCH_LOG_PKT        (LOG_WCDMA_BASE_C + 0x174)
#define L1_WCDMA_INTERF_LIST_0x4175_SRCH_LOG_PKT         (LOG_WCDMA_BASE_C + 0x175)
#define SRCH_STEP1_0x4177_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x177)
#define SRCH_NASTT_0x4178_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x178) 
#define SRCH_PN_0x4179_LOG_PACKET                        (LOG_WCDMA_BASE_C + 0x179)
#define L1_WCDMA_INTERF_LIST_0x417A_SRCH_LOG_PKT         (LOG_WCDMA_BASE_C + 0x17A)
#define UL_DATA_DUMP_LOG_PKT	                       	 (LOG_WCDMA_BASE_C + 0x17F)
#define UL_RACH_PREAMB_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x180)
#define UL_RACH_TIMING_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x181)
#define TX_CONFIG_LOG_PKT                                (LOG_WCDMA_BASE_C + 0x182)
#define SRCH_STEP1_0x4183_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x183)
#define SRCH_NASTT_0x4184_LOG_PACKET                     (LOG_WCDMA_BASE_C + 0x184)
#define SRCH_PN_0x4185_LOG_PACKET                        (LOG_WCDMA_BASE_C + 0x185)
#define WCDMA_FINGER_TA_INFO_0x4186_LOG_PACKET           (LOG_WCDMA_BASE_C + 0x186) 
#define WCDMA_CELL_STATUS_DEBUG_LOG_PACKET               (LOG_WCDMA_BASE_C + 0x187)  
#define CME_DBG_EQ_RECFG_PKT                             (LOG_WCDMA_BASE_C + 0x188)
#define CME_SFC_DBG_QICE_PKT                             (LOG_WCDMA_BASE_C + 0x189)
#define CME_DBG_SCHIC_0x418A_PKT                         (LOG_WCDMA_BASE_C + 0x18A)
#define CME_FFINV_DBG_QICE_PKT                           (LOG_WCDMA_BASE_C + 0x190)
#define CME_DBG_QSET_UPDATE_PKT                          (LOG_WCDMA_BASE_C + 0x191)
#define UL_MTD_LOG_PKT                                   (LOG_WCDMA_BASE_C + 0x192)
#define WL1_DEMBACK_EXT_LOG_PKT                          (LOG_WCDMA_BASE_C + 0x193)
#define WCDMA_ULTPC_DEBUG_PKT                            (LOG_WCDMA_BASE_C + 0x198)
#define DRX_PICH_LOG_PKT                                 (LOG_WCDMA_BASE_C + 0x1AA)
#define WCDMA_PERIODIC_STAT_0x41B1_LOG_PKT               (LOG_WCDMA_BASE_C + 0x1B1)
#define WCDMA_ANT_SWITCH_CONFIG_0x41B4_LOG_PKT           (LOG_WCDMA_BASE_C + 0x1B4)
#define WCDMA_ANT_SWITCH_METRICS_0x41B5_LOG_PKT          (LOG_WCDMA_BASE_C + 0x1B5)
#define L1_WCDMA_INTERF_STEP1_0x41C6_SRCH_LOG_PKT        (LOG_WCDMA_BASE_C + 0x1C6)
#define HS_CFG_0x4200_LOG_PKT                            (LOG_WCDMA_BASE_C + 0x200)
#define HS_UL_BETA_TABLE_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x201)
#define WCDMA_FINGER_TA_INFO_0x4202_LOG_PACKET           (LOG_WCDMA_BASE_C + 0x202)
#define HS_DEMOD_CTRL_TABLE_LOG_PKT                      (LOG_WCDMA_BASE_C + 0x203)
#define HS_MOD_CTRL_TABLE_LOG_PKT                        (LOG_WCDMA_BASE_C + 0x204)
#define HS_DECODE_STATUS_0x4206_LOG_PKT                  (LOG_WCDMA_BASE_C + 0x206)
#define HS_SCCH_STAT_0x4207_LOG_PKT                      (LOG_WCDMA_BASE_C + 0x207)
#define HS_DSCH_HARQ_STAT_0x4208_LOG_PKT                 (LOG_WCDMA_BASE_C + 0x208)
#define HS_CQI_STAT_0x4209_LOG_PKT                       (LOG_WCDMA_BASE_C + 0x209)
#define HS_UL_DPCCH_INFO_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x20A)
#define HS_UL_DPCCH_INFO_DBG_0x420B_LOG_PKT              (LOG_WCDMA_BASE_C + 0x20B)
#define DL_TBSET_STATUS_0x420C_PKT                       (LOG_WCDMA_BASE_C + 0x20C)
#define HS_PESS_CQI_0x420D_LOG_PKT                       (LOG_WCDMA_BASE_C + 0x20D)
#define HS_AMP_ENG_EST_0x420E_LOG_PKT                    (LOG_WCDMA_BASE_C + 0x20E)
#define HS_PESS_CQI_0x420F_LOG_PKT                       (LOG_WCDMA_BASE_C + 0x20F)
#define HS_SCCH_0x423F_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x23F)
#define HS_MAC_HS_HDR_LOG_PKT                            (LOG_WCDMA_BASE_C + 0x211)
#define HS_DECODE_STATUS_0x4214_LOG_PKT                  (LOG_WCDMA_BASE_C + 0x214)
#define EQ_DBG_PKT_0x4215                                (LOG_WCDMA_BASE_C + 0x215)
#define EQ_RECFG_PKT                                     (LOG_WCDMA_BASE_C + 0x216)
#define DL_TBSET_STATUS_0x4217_PKT                       (LOG_WCDMA_BASE_C + 0x217)
#define HS_AMP_ENG_EST_0x4218_LOG_PKT                    (LOG_WCDMA_BASE_C + 0x218)
#define HS_AMP_ENG_EST_0x4219_LOG_PKT                    (LOG_WCDMA_BASE_C + 0x219)
#define DL_TBSET_STATUS_0x421A_PKT                       (LOG_WCDMA_BASE_C + 0x21A)
#define DL_TBSET_STATUS_0x421B_PKT                       (LOG_WCDMA_BASE_C + 0x21B)
#define DL_TBSET_STATUS_0x41BA_PKT                       (LOG_WCDMA_BASE_C + 0x1BA)
#define HS_UL_DPCCH_INFO_DBG_0x421C_LOG_PKT              (LOG_WCDMA_BASE_C + 0x21C)
#define HS_AMP_ENG_EST_0x421D_LOG_PKT                    (LOG_WCDMA_BASE_C + 0x21D)
#define HS_SCCH_STAT_0x421F_LOG_PKT                      (LOG_WCDMA_BASE_C + 0x21F)
#define HS_DSCH_HARQ_STAT_0x4221_LOG_PKT                 (LOG_WCDMA_BASE_C + 0x221)
#define HS_DECODE_STATUS_0x4222_LOG_PKT                  (LOG_WCDMA_BASE_C + 0x222)
#define HS_MIMO_CH_ANALYSIS_LOG_PKT                      (LOG_WCDMA_BASE_C + 0x223)
#define HS_MIMO_DEMOD_LOG_PKT                            (LOG_WCDMA_BASE_C + 0x224)
#define HS_CFG_0x4225_LOG_PKT                            (LOG_WCDMA_BASE_C + 0x225)
#define CME_DBG_EQ_PKT                                   (LOG_WCDMA_BASE_C + 0x226)
#define CME_DBG_SCHIC_0x4227_PKT                         (LOG_WCDMA_BASE_C + 0x227)
#define EUL_DTX_CONFIG_RECONFIG_LOG_PKT                  (LOG_WCDMA_BASE_C + 0x22B)
#define CPC_DTX_FW_STATE_MACHINE_LOG_PKT                 (LOG_WCDMA_BASE_C + 0x22C)
#define DL_IMC_METRIC_LOG_PKT                            (LOG_WCDMA_BASE_C + 0x22D)
#define HS_CQI_STAT_0x422E_LOG_PKT                       (LOG_WCDMA_BASE_C + 0x22E)
#define MBMS_CFG_LOG_PKT                                 (LOG_WCDMA_BASE_C + 0x230)
#define MBMS_MICH_LOG_PKT                                (LOG_WCDMA_BASE_C + 0x231)
#define EQ_POSEIDON_0x4233_PKT                           (LOG_WCDMA_BASE_C + 0x233)
#define DL_DEBUG_TBSET_PKT                               (LOG_WCDMA_BASE_C + 0x234)
#define WCDMA_SCHIC_DBG_LOG_PKT                          (LOG_WCDMA_BASE_C + 0x237)
#define CME_DC_REG_EQ_0x4238_PKT                         (LOG_WCDMA_BASE_C + 0x238)
#define WL1_DL_DRXCTRL_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x23A)
#define WL1_DL_DRXCTRL_CFG_LOG_PKT                       (LOG_WCDMA_BASE_C + 0x23B)
#define CME_DC_DBG_EQ_0x423E_PKT                         (LOG_WCDMA_BASE_C + 0x23E)
#define WCDMA_EUL_DL_DECODE_INFO_LOG_PACKET              (LOG_WCDMA_BASE_C + 0x302)
#define EUL_R_COMBO_TABLE_INFO_LOG_PKT                   (LOG_WCDMA_BASE_C + 0x35C)
#define EUL_UL_EDPCH_INFO_0x4306_LOG_PKT                 (LOG_WCDMA_BASE_C + 0x306)
#define EUL_UL_ETFC_RESTRICT_INFO_LOG_PKT                (LOG_WCDMA_BASE_C + 0x35D)
#define WCDMA_FACH_DRX_STATE_MACHINE_LOG_PKT             (LOG_WCDMA_BASE_C + 0x30A)
#define WCDMA_FACH_DRX_CONFIG_LOG_PKT                    (LOG_WCDMA_BASE_C + 0x30B)
#define RXD_ARD_LOG_PKT                                  (LOG_WCDMA_BASE_C + 0x30C)
#define L1_RXD_CTRL_LOG_PKT                              (LOG_WCDMA_BASE_C + 0x30D)
#define EUL_DPCH_LOG_PKT                                 (LOG_WCDMA_BASE_C + 0x312)

#define WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET                (LOG_WCDMA_BASE_C + 0x340)
#define EUL_UL_CHAN_CFG_INFO_LOG_PKT                     (LOG_WCDMA_BASE_C + 0x341)
#define EUL_UL_EDPCH_INFO_0x4305_LOG_PKT                 (LOG_WCDMA_BASE_C + 0x342)
#define EUL_UL_RATE_MATCHING_INFO_LOG_PKT                (LOG_WCDMA_BASE_C + 0x343)
#define EUL_COMBINED_L1_MAC_LOG_PKT                      (LOG_WCDMA_BASE_C + 0x344)
#define EUL_L1_MAC_STATS_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x345)
#define UPLINK_TX_RF_DEBUG_LOG_PKT                       (LOG_WCDMA_BASE_C + 0x346)
#define WCDMA_EUL_DL_HICH_RESULTS_INFO_LOG_PACKET        (LOG_WCDMA_BASE_C + 0x347)
#define WCDMA_EUL_DL_RGCH_RESULTS_INFO_LOG_PACKET        (LOG_WCDMA_BASE_C + 0x348)
#define WCDMA_EUL_DL_AGCH_RESULTS_INFO_LOG_PACKET        (LOG_WCDMA_BASE_C + 0x349)
#define WCDMA_MULTI_CARR_EUL_POWER_ALLOC_LOG_PACKET      (LOG_WCDMA_BASE_C + 0x35A)
#define WCDMA_MULTI_CARR_EUL_MPR_LOG_PACKET              (LOG_WCDMA_BASE_C + 0x35B)

#define EUL_IRQ_FLOW_CTRL_INFO_LOG_PKT                   (LOG_WCDMA_BASE_C + 0x333)
#define UL_RACH_RAM_DUMP_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x70A)
#define SRCH_FACH_MEAS_OKSN_INFO_LOG_PKT                 (LOG_WCDMA_BASE_C + 0x721)
#define SRCH_FACH_INTER_RAT_MEAS_LOG_PKT                 (LOG_WCDMA_BASE_C + 0x722)
#define DL_COMPACT_CRC_PKT                               (LOG_WCDMA_BASE_C + 0x760)
#define DL_TFCI_0x4770_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x770)
#define EQ_DBG_PKT_0x4771                                (LOG_WCDMA_BASE_C + 0x771)
#define MBMS_TFCI_LOG_PKT                                (LOG_WCDMA_BASE_C + 0x772)
#define DL_TFCI_0x4773_LOG_PKT                           (LOG_WCDMA_BASE_C + 0x773)
#define HS_SCCH_DECODE_STATUS_LOG_PKT                    (LOG_WCDMA_BASE_C + 0x23F)
#define WCDMA_XO_LOG_PKT                                 (LOG_WCDMA_BASE_C + 0x776)
#define HS_SCCH_DEBUG_LOG_PKT                            (LOG_WCDMA_BASE_C + 0x777)
#define SRCH_ALL_CELLS_PN_0x417A_LOG_PACKET              (LOG_WCDMA_BASE_C + 0x17A)
#define SRCH_PN_BYPASS_INFO_LOG_PACKET                   (LOG_WCDMA_BASE_C + 0x17B)
#define UL_HSRACH_CONFIG_LOG_PKT                         (LOG_WCDMA_BASE_C + 0x194)
#define UL_HSRACH_ALLOC_EDCH_RSRC_LOG_PKT                (LOG_WCDMA_BASE_C + 0x195)
#define UL_HSRACH_CHAN_TIMING_LOG_PKT                    (LOG_WCDMA_BASE_C + 0x196)
#define UL_HSRACH_PREAMBLE_STATUS_LOG_PKT                (LOG_WCDMA_BASE_C + 0x197)
#define SEARCH_HIST_INFO_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x19E)
#define NASTT_HIST_INFO_LOG_PACKET                       (LOG_WCDMA_BASE_C + 0x19F)
#define NASTT_DETECT_HIST_INFO_LOG_PACKET                (LOG_WCDMA_BASE_C + 0x1A0)
#define CELL_HIST_INFO_LOG_PACKET                        (LOG_WCDMA_BASE_C + 0x1A1)
#define ASET_FILT_HIST_INFO_LOG_PACKET                   (LOG_WCDMA_BASE_C + 0x1A2)
#define SIRT_FILT_HIST_INFO_LOG_PACKET                   (LOG_WCDMA_BASE_C + 0x1A3)
#define FILT_ECIO_HIST_INFO_LOG_PACKET                   (LOG_WCDMA_BASE_C + 0x1A4)
#define RXD_FILT_HIST_INFO_LOG_PACKET                    (LOG_WCDMA_BASE_C + 0x1A5)
#define ASET_LOG_PACKET_V2                               (LOG_WCDMA_BASE_C + 0x1AC)
#define SRCH_CSG_ASF_MEAS_REQ_LOG_PKT                    (LOG_WCDMA_BASE_C + 0x1AE)
#define SRCH_CSG_CELL_RESEL_LOG_PKT                      (LOG_WCDMA_BASE_C + 0x1AF)
#define L1_MULTI_COMB_TPC_DC_PKT                         (LOG_WCDMA_BASE_C + 0x1B2)
#define WCDMA_SLOT_LEVEL_ILPC_LOG_PACKET                 (LOG_WCDMA_BASE_C + 0x778)
#define SRCH_FREQ_SCAN_LOG_PACKET                        (LOG_WCDMA_BASE_C + 0x1B0)
#define WCDMA_CXM_LOG_PACKET                             (LOG_WCDMA_BASE_C + 0x1B6)
#define WL1TRM_TUNEAWAY_LOG_PKT                          (LOG_WCDMA_BASE_C + 0x1B7)
#define SRCH_NGBR_SRCH_OPT_LOG_PKT                       (LOG_WCDMA_BASE_C + 0x1B3)
#define L1_CM_TGPSI_ACTIVITY_LOG_PKT                     (LOG_WCDMA_BASE_C + 0x1BB)
#define WCDMA_DRX_CTCH_ENH_LOG_PKT                       (LOG_WCDMA_BASE_C + 0x1B9)
#define WSRCH_NB_REJ_LOG_PACKET                          (LOG_WCDMA_BASE_C + 0x1BC)
#define WRM_MSG_LOG_PKT                                  (LOG_WCDMA_BASE_C + 0x1C1)
#define WCDMA_TIMELINE_CHECKS_LOG_PACKET                 (LOG_WCDMA_BASE_C + 0x1C4)

/* ========================   LAYER 2 PACKETS   ======================== */

/* RLC packets */
#define WCDMA_UL_RLC_STATE_LOG_PACKET                    (LOG_WCDMA_BASE_C + 0x119) 
#define WCDMA_DL_RLC_STATE_LOG_PACKET                    (LOG_WCDMA_BASE_C + 0x11F)   
#define WCDMA_DL_RLC_AM_USR_PLANE_PDU_0x4134_LOG_PACKET  (LOG_WCDMA_BASE_C + 0x134)
#define WCDMA_DL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET         (LOG_WCDMA_BASE_C + 0x135)   
#define WCDMA_UL_RLC_V2_AM_STAT_LOG_PACKET               (LOG_WCDMA_BASE_C + 0x138)
#define WCDMA_UL_RLC_AM_USR_PLANE_PDU_0x413B_LOG_PACKET  (LOG_WCDMA_BASE_C + 0x13B)
#define WCDMA_UL_RLC_AM_SIG_PLANE_PDU_LOG_PACKET         (LOG_WCDMA_BASE_C + 0x13C)
#define WCDMA_UL_RLC_AM_NAK_PDU_LOG_PACKET               (LOG_WCDMA_BASE_C + 0x145)   
#define WCDMA_DL_RLC_AM_NAK_PDU_LOG_PACKET               (LOG_WCDMA_BASE_C + 0x146)   
#define WCDMA_DL_RLC_AM_STAT_LOG_PACKET                  (LOG_WCDMA_BASE_C + 0x14A)  
#define WCDMA_DL_RLC_AM_USR_PLANE_PDU_0x4154_LOG_PACKET  (LOG_WCDMA_BASE_C + 0x154)
#define WCDMA_UL_RLC_AM_USR_PLANE_PDU_0x415B_LOG_PACKET  (LOG_WCDMA_BASE_C + 0x15B)
#define WCDMA_UL_RLC_V1_CIPH_CFG_LOG_PACKET              (LOG_WCDMA_BASE_C + 0x161) 
#define WCDMA_DL_RLC_V1_CIPH_CFG_LOG_PACKET              (LOG_WCDMA_BASE_C + 0x162) 
#define WCDMA_DL_RLC_V1_PDU_CIPHER_LOG_PACKET            (LOG_WCDMA_BASE_C + 0x168)  
#define WCDMA_UL_RLC_V1_PDU_CIPHER_LOG_PACKET            (LOG_WCDMA_BASE_C + 0x169)
#define WCDMA_DL_FLEXIBLE_RLC_AM_PDU_LOG_PACKET_V1       (LOG_WCDMA_BASE_C + 0x18B)
#define WCDMA_RLC_UL_COMPRESSION_LOG_PACKET              (LOG_WCDMA_BASE_C + 0x1C5)
#define WCDMA_UL_RLC_TM_LOG_PACKET                       (LOG_WCDMA_BASE_C + 0x1C7)
#define WCDMA_UL_RLC_UM_LOG_PACKET                       (LOG_WCDMA_BASE_C + 0x1C8) 
#define WCDMA_DL_RLC_TM_LOG_PACKET                       (LOG_WCDMA_BASE_C + 0x1C9)
#define WCDMA_DL_RLC_UM_LOG_PACKET                       (LOG_WCDMA_BASE_C + 0x1CA)
#define WCDMA_UL_RLC_V1_AM_LOG_PACKET                    (LOG_WCDMA_BASE_C + 0x1CB)
#define WCDMA_DL_RLC_V1_AM_LOG_PACKET                    (LOG_WCDMA_BASE_C + 0x1CC)  
#define WCDMA_UL_RLC_UM_USR_PLANE_PDU_LOG_PACKET         (LOG_WCDMA_BASE_C + 0x331)
#define WCDMA_DL_RLC_UM_USR_PLANE_PDU_LOG_PACKET         (LOG_WCDMA_BASE_C + 0x332)
#define WCDMA_MAC_HS_UL_MAC_I_LOG_PACKET                 (LOG_WCDMA_BASE_C + 0x30E)
#define WCDMA_UL_FLEXIBLE_RLC_PDU_LOG_PACKET_V1          (LOG_WCDMA_BASE_C + 0x30F)


/*MAC packets */
#define WCDMA_MAC_UL_TVM_REPORT_LOG_PACKET               (LOG_WCDMA_BASE_C + 0x002)
#define WCDMA_UL_MAC_CFG_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x117)
#define WCDMA_DL_MAC_CFG_LOG_PACKET                      (LOG_WCDMA_BASE_C + 0x11d)
#define WCDMA_MAC_RACH_CFG_LOG_PACKET                    (LOG_WCDMA_BASE_C + 0x123)
#define WCDMA_MAC_UL_TFCS_CFG_LOG_PACKET                 (LOG_WCDMA_BASE_C + 0x124) 
#define WCDMA_MAC_HS_UL_CONFIG_LOG_PACKET_V1             (LOG_WCDMA_BASE_C + 0x1C2) 
#define WCDMA_MAC_UL_CIPHER_CONFIG_LOG_PACKET            (LOG_WCDMA_BASE_C + 0x1CE)
#define WCDMA_MAC_HS_CONFIG_LOG_PACKET                   (LOG_WCDMA_BASE_C + 0x210)
#define WCDMA_MAC_HS_STATUS_REPORT_LOG_PACKET            (LOG_WCDMA_BASE_C + 0x212)
#define WCDMA_MAC_HS_RESET_LOG_PACKET                    (LOG_WCDMA_BASE_C + 0x213)
#define WCDMA_MAC_HS_REASSEMBLY_LOG_PACKET               (LOG_WCDMA_BASE_C + 0x21E)
#define WCDMA_MAC_HS_CONFIG_LOG_PACKET_V2                (LOG_WCDMA_BASE_C + 0x220)
#define WCDMA_MAC_HS_UL_HEADER_STATUS_LOG_PACKET         (LOG_WCDMA_BASE_C + 0x322)
#define WCDMA_MAC_HS_UL_ETFCI_STATUS_LOG_PACKET          (LOG_WCDMA_BASE_C + 0x323)
#define WCDMA_MAC_HS_TRESET_EXPIRY_LOG_PACKET            (LOG_WCDMA_BASE_C + 0x23C)
#define WCDMA_MAC_HS_STATUS_REPORT_LOG_PACKET_V2         (LOG_WCDMA_BASE_C + 0x23D) //Change to new number for V2
#define WCDMA_HS_RACH_MAC_TIMERS_LOG_PACKET              (LOG_WCDMA_BASE_C + 0x314)
#define WCDMA_MAC_UL_CIPHER_LOG_PACKET                   (LOG_WCDMA_BASE_C + 0x1A6)

/* ========================   LAYER 3 PACKETS   ======================== */

#define WCDMA_RRC_MODES_AND_STATES                       (LOG_WCDMA_BASE_C + 0x125)
#define WCDMA_RRC_PROTOCOL_ERRORS                        (LOG_WCDMA_BASE_C + 0x126)
#define WCDMA_CELL_ID                                    (LOG_WCDMA_BASE_C + 0x127)
#define WCDMA_UL_CHANNEL_MAPPING                         (LOG_WCDMA_BASE_C + 0x128)
#define WCDMA_DL_CHANNEL_MAPPING                         (LOG_WCDMA_BASE_C + 0x129)
#define WCDMA_FP_INFO                                    (LOG_WCDMA_BASE_C + 0x12a)
#define WCDMA_SIB                                        (LOG_WCDMA_BASE_C + 0x12b)
#define WCDMA_CMAC_DL_R99                                (LOG_WCDMA_BASE_C + 0x12C)
#define WCDMA_SIGNALLING_MESSAGE                         (LOG_WCDMA_BASE_C + 0x12f)
#define WCDMA_RLC_AM_LOGGING                             (LOG_WCDMA_BASE_C + 0x1BD)
#define WCDMA_CMAC_UL_R99_LOGGING                        (LOG_WCDMA_BASE_C + 0x1BE)
#define WCDMA_CMAC_UL_HS_LOGGING                         (LOG_WCDMA_BASE_C + 0x1BF)
#define WCDMA_CMAC_DL_HS_LOGGING                         (LOG_WCDMA_BASE_C + 0x1C0)
#define WCDMA_HANGING_RB_INFO                            (LOG_WCDMA_BASE_C + 0x804)
#define WCDMA_MEAS_LOGGING_INTRAFREQ                     (LOG_WCDMA_BASE_C + 0x805)
#define WCDMA_MEAS_LOGGING_INTERFREQ                     (LOG_WCDMA_BASE_C + 0x806)
#define WCDMA_MEAS_LOGGING_IRAT                          (LOG_WCDMA_BASE_C + 0x807)
#define WCDMA_HANGING_RB_MAPPING_INFO                    (LOG_WCDMA_BASE_C + 0x808)
/*============================ LOG CODE LIMIT CHECK ===================== */

/* 
   This is for a check in wcdmadiag.c to make sure the last code limit is not 
   crossed. This should be updated whenever someone uses a log code greater than 0x4777
   to avoid limit exceeded error fatal 
*/
#define LOG_WCDMA_INTERNAL_LAST_C                        (LOG_WCDMA_BASE_C + 0x777)   

#endif /* LOG_CODES_WCDMA_INT_H */
