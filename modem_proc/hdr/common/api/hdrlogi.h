#ifndef HDR_LOGI_H
#define HDR_LOGI_H
/*===========================================================================

                       HDR Logging Packets Header File

General Description
  This file contains declarations for HDR packets to be logged.

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                             Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrlogi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/05/14   vlc     Added EVDO physical layer power info log packet (0x1878) 
05/08/14   rmv     Updated FL Packet Header log packet to include packet
                   cancelled status and Preamble Detection type 
02/27/14   arm     Fixed missing sDTX fl_active member in DTX log packet.                   
09/25/13   arm     Consolidated Flexconn, sDTX, tDTX and nDTX into one log
                   packet and removed FW specific values.
08/28/13   wsh     Updated ASDiv NV and logging
06/26/13   wsh     Added Selective TxD feature
03/13/13   vlc/arm Added new parameters to sleep info packet. 
11/27/12   smd     Supported log packets for FlexConn and DTX.
11/24/12   ukl     Fix Klocwork issues
07/27/12   ssm     Added fields p_max_neq and in_funnel_mode in 
                   hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_info_type struct.
05/25/12   wsh     Moved ARD logic into it's own state machine 
03/27/12   vlc     Moved HDRLOG_MC_PILOT_SETS_VER to 2. 
03/22/12   arm     Keep only one definition of HDRLOG_VAR_SIZEOF. 
03/08/12   vlc/kss Added gRICE info to EQ logging.
                   Changed demod_decision to eq_status.
02/11/12   smd     Added new fields in DTX and FlexConn log packets.
09/09/11   kss     Added gRICE info to EQ logging.
09/13/11   smd     SU APU cleanup: removed RLP log packets.
04/14/10   pba     Updated Ovhd Cache Packet log summary.
03/25/10   pxu     Removed FEATURE_HDR_EQ feature checks. 
12/14/09   lyl     Moved num_reacq_pilots_srched to end of enh_sleep_info pkt.
10/15/09   etv     Added defines for invalid channel number and band class.
11/01/09   etv     Fixed Klocwork errors.
09/15/09   kss     Use new PACK() macro for Q6 compatibility.
05/15/09   wsh     Include needed headers to support diag split
04/07/09   lyl     Added pilot sets v3 log packet for ofs condition fltr eng.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI-
02/27/09   spn     Fixed Lint Error
02/24/09   wsh     CMI: updated included headers due to header split
02/18/09   wsh     Refactored hdrlogi to remove ext-AU definitions
01/26/09   lyl     Logged the number of reacq search pilots in enh_sleep_info.
01/23/09   wsh     Updated OvhdCacheLog to include 128b SectID & QPCH fields
12/11/08   etv     Removed MACIndex from FL Packet header info log packet.
11/19/08   rmg     Change from data team - increase MRLP log buf size.
11/11/08   grl     Updated EnhSleepInfo for version 1 support.
11/10/08   lyl     Fixed pilot sets log packet (version 2 0x108B)
10/07/08   lyl     Added the phase info in MC finger data log packet.
09/08/08   lyl     Added ofs_cond_eng in MC pilot sets log packet
09/08/08   lyl     Add pilot sets log packet (version 3).
07/08/08   wsh     Log ovhd cache when it is updated.
06/17/08   rmg     Added new fields to PA headroom log packet.
06/16/08   etv     Added timestamp to FL/RL phy layer log packets.
06/15/08   wsh     Reserved log code for OVHD cache entry log packet
06/11/08   cc/etv  Updated MCSUFwdStats to support enhanced RevB rates.
05/23/08   kss     New version of MC RL Gains (BetaP, global Tx max/min).
05/21/08   spn     Updated the MRLP Partial Log Packets from a uint8 timeref 
                   to a uint16 timeref
05/05/08   wsh     Added PCP log packet support
05/01/08   etv     Resolved log code conflict with PCP log packet.
04/29/08   etv     Reserved log code for PB PilotDetection log packet.
03/08/08   etv     Increased the size of BASPSINR from 16 to 32 bits.
03/31/08   etv     Updated MC Power log packet definition.
03/11/08   rmg     Added dropping rank logging in hdrm log packet.
03/04/08   jyw     Updated the multiple carrier pilot sets logging.
02/26/08   jyw     Added multiple carrier pilot sets logging.
02/12/08   rmg     Added MC_PA_HDRM_AND_CARRIER_MGMT log packet.
02/04/08   etv     Obsoleted RTC & FTC Phy pkt logging and created a new log
                   for logging RL & FL physical layer(0,A & B) packet dump 
01/16/08   rmg     Added reference_subaset field in MC RAB Info log packet.
01/14/08   etv     Replaced bit-packet struct with uint16 to simplify copy.
01/08/08   etv     Made EqBestC2I a 32-bit field.
12/19/07   etv     Made updates to MC Airlinksummary log packet definition.
01/03/08   etv     Fixed log code allocation conflict for Equalized data log.
12/21/07   sb      Added support for Multi Carrier Equalizer Data log packet
                   Added support for Multi Carrier Power log packet.
12/13/07   cc      Moved pilot_strength_ps to per channel structure in RMAC4
                   T2P Resource packet.
12/13/07   cc      Added RMAC4 2nd iteration information.
12/11/07   etv     Reserved log codes for MC EQ data and PB log packets.
12/03/07   etv     Added support for the following MC log packets.
                   1. Multi-carrier AirLinkSummary log packet
                   2. Multi-carrier DRC-ARQ-DSC Buffer log packet
                   3. Multi-carrier Equalizer C2I buffer log packet
                   4. Multi-carrier Finger Data log packet
                   5. Multi-carrier Searcher Data log packet.
10/11/07   grl     Added rev A sleep info packet.
11/08/07   etv     Added support for MC SUP fwd stats log packet.
11/07/07   rmg     Added support for MC_RL_GAINS log packet.
11/01/07   rmg     Added support for MC_RL_PKT_STATUS log packet.
11/01/07   etv     Reserved log code for EQ C2I buffer log packet. 
10/30/07   etv     Reserved log code for MC RL Packet Status log packet
10/18/07   rmg     Added support for MC_RL_METRICS log packet.
10/16/07   cc      Renamed normal_hdr_inactive to hdr_interrupted; changed
                   qrab from uint8 to int8.
10/12/07   cc      Renamed num_rlp_pkts_in_pkt to num_app_pkts_in_pkt in 
                   LOG_HDR_RMAC_PKT_TIME_INFO_C log packet.
10/09/07   rmg     Added support for LOG_HDR_MC_RL_ARQ_STATUS_C and 
                   LOG_HDR_MC_RAB_UPDATE_INFO_C log packets.
10/08/07   spn     Added MMPA Rev Stats log definition
10/05/07   da      Added MMPA log type definitions
09/27/07   grl     Added rev V sleep info pkt id.
09/25/07   cc      Added RMAC4 log packets definitions.
09/25/07   etv     Reserved log codes for Rev-B Priority 2 & 3 log pkts.
08/24/07   sb      Added Multi-carrier fl pkt header log packet definition.
09/11/07   etv     Reserved Log codes for Rev-B priority 1 log packets.
04/02/07   hrk     Added Enhanced ACMAC and Access Probe Logging support.
03/28/07   rmg     Fixed invalid values on HDR debug screen
03/05/07   sju     Add MMPA to hdrlog_hdr_app_enum_type
11/20/06   ac      Support for enhanced MRLP logging
11/13/06   etv     Reserved Log codes for upcoming Log packets.
11/13/06   rmg     Added rand_num in HDR_REVA_RMAC3_PKT_INFO
11/06/06   mpa     Allocated new enh MRLP log packets.
10/10/06   hal     Added REVA_RUP_TC_LIST log definition
09/05/06   etv     Added support for LOG_HDR_SUBTYPE_STREAM_CONFIG_C log pkt.
09/01/06   spn     Updated the MRLP Log Packet, to reflect the sequence 
                   length, and the RLP ID Length.
08/03/06   kss     Corrected type of pa_headroom field in RL Gains packet.
08/03/06   yll     Use different app_subtype for EMPA
06/13/06   etv     Included CRC and sequence to fl_pkt_header_info log pkt.
04/24/06   hal     Minor change to REVA_PCP_PKT_STATS log definition
                   (header union changed from uint32 to uint8[4])
01/20/06   hal     Minor change to REVA_PCP_PKT_STATS log definition
                   (queue_type changed from enum to uint8)
12/04/05   hal     Added REVA_PCP_PKT_STATS log definition
12/02/05   sq      Added new re-acq result enum
11/14/05   kss     Minor change to RL_PKT_STATUS log.
11/01/05   kss     Added RL_PKT_STATUS packet; added REVA to RMAC3 log names..
10/27/05   ljl     Changed name of RAB Update Info log packet
10/18/05   kss     Added RL_ARQ_STATUS packet.
10/18/05   sq      Added RMAC3 Packet Info logging
10/18/05   etv     Code review comment fixes for MUP user stats log pkt.
10/13/05   ljl     Updated Rev RABUpdateInfo log packet definition
10/12/05   ds      Added DRC-ARQ-DSC log packet definition, move FL_PHY code
10/12/05   etv     Added support for MUP User statistics summary log packet.
10/10/05   jyw     Added Rev A pkt header log packet definition.
10/04/05   ljl     Added Rev A RAB related log packets
09/26/05   kss     Revised definition of LOG_HDR_REVA_RL_GAINS_C.
10/03/05   sr      Added RTCMAC3 T2P resource log packet (Rev A) 
09/16/05   kss     Added Rev A RL metrics and gains packets, reserved codes.
07/25/05   etv     Added support for Multi-user packet Fwd. Stats summary.
07/11/25   ds      Added Search Type2 log packet with Equalizer fields
07/05/05   ds      Defined structures for LOG_HDR_MDSP_DEBUG_BUFFER_C 
06/20/05   sk      Added version number fields on statistics log packets
06/14/05   ds      Added version number fields to equalizer log packets
06/10/05   etv     Removed feature "FEATURE_HDR_REVA_L1" and changed #define 
                   LOG_HDR_REVA_SU_FW_STATS_C to LOG_HDR_REVA_SU_FWD_STATS_C
06/03/05   ds      Added two equalizer related log packet definitions
05/31/05   sk      Defined structures to log MRLP statistics and MRLP data
05/17/05   etv     Redefined LOG_HDR_REVA_SU_FW_STATS_C with per-DRC bad CRC.
05/16/05   ds      Reserved log codes for equalizer and mdsp debug log packets
04/13/05   pba     Removed re-definition of time_type already defined in 
                   "time_svc.h", and instead included time_svc.h
04/04/05   pba     Removed Lint errors
03/16/05   etv     Added definition for LOG_HDR_REVA_SU_FW_STATS_C
                   Reserved log codes for single/multi-user fwd_stats_summary.
02/22/05   dna     Reserved codes for new message log packets for CMML parser
01/11/05   mpa     Added new range of reserved log codes (629-704).
01/04/05   etv     Added definitions of new log packets for FTCMAC and RTCMAC.
11/29/04   grl     Added support for connected state stat logging.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
09/09/04   mpa     Added packet app type logging support in signalling
06/26/04   kss     Fix bug in HDR_MCS_PHY_CHAN_METRICS definition.
06/07/04   kss     Removed HDR_BCMCS featurization, fixed typo in log name.
05/03/04   ds      Added a new log packet (debug_display)
05/05/04   kss     Added HDR BCMCS packet definitions.
03/31/04   mpa     Added new reserved log codes range
11/11/03   sq      Added 3 new packets( acq_info, reacq_info, sleep_info)
07/21/03   mpa     Merged in 6500 baseline
02/10/03   mpa     Changes max pilots in NSET from 20 to 40.
10/29/02   aaj     Added support for two frames logging in drc-arq log packet
           mpa     Log full uati in HDR_SESSION_ATTEMPT instead of uati024
                   Added new HDR_RCV_ATI_LIST, HDR_FWD_STATS_SUMMARY,
           kss/mpa HDR_AC_MAC_CAPSULE, HDR_CC_MAC_PACKET packets.
           sq      Added more searcher logging info related to window centering.
08/01/02   mpa     Added is_hdris890 field to signalling packets.
04/04/02   mpa     Added hybrid_mode field to HDR_STATE_INFO.
02/22/02   kss     Added drcLockPeriod field to HDR_REV_PCTRL.
12/11/02   mpa     Added ant0_rssi and ant1_rssi fields to the FINGER pkt.
12/10/02   mpa     Modified FINGER DATA to accommodate 12 fingers instead of 8
12/05/01   mpa     Removed LOG_HDR_RATE_IDX_MAP_C definition (obsolete).
                   Modified REV PCTRL definition slightly.
11/30/01   mpa     Added num_records field to REV PCTRL packet definition
11/28/01   mpa     Added HDR REV PCTRL packet definition
11/14/01   mpa     Moved SAM definition to samlog.h (Requested new log ID)
                   Removed LOG_HDR_SRCH_STATUS_C and HDR_GEN_TA_C (obsolete)
                   Expanded HDR_DRC_ARQ_BUFFER_C.
11/08/01   mpa     Changed packing order of HDR SAM. Added enum types for QCAT
11/01/01   dna     HDR signaling message log header is 11 bytes, not 12
10/30/01   mpa     Merged rev 1.20 and 1.21 (rev 1.21 based on rev 1.19)
10/29/01   aaj     Added definition for RAB packet size
10/23/01   mpa     Added new HDR DEBUG HEX DUMP packet definition.
                   Modified both fwd and rev snapshot packets.
                   Deleted AT CONFIG log packet definition.
09/20/01   rao     Added Partial RLP Frame logging support
09/14/01   kss     Corrected the HDR FTC_RATE_SLOT_COUNT type name.
09/13/01   kss     Corrected the HDR POWER packet definition.
08/28/01   mpa     Corrected the HDR HRES TA packet definition.
08/23/01   sh      Removed sector id and added uati024 for session attempt
07/24/01   aaj     Filled in more packet definitions after 1.0 Alpha release
08/19/01   sh      Updated connection attempt and session attempt log packet
08/07/01   aaj     Changes to HDR log packets
06/06/01   aaj     REDEFINED ALL PACKET TYPES FOR NEW HDR LOG DEFINITIONS
02/28/01   aaj     Added RX diversity and RX1_AGC to search status packet
02/09/01   aaj     Added new HDR RX STATS packet definition
02/08/01   aaj     Added new HDR SRCH status packet definition
02/07/01   dna     Added HDRLOG_NONE for use as default log link type.
02/06/01   aaj     Added HDR GEN TA log packet definition
01/26/01   dna     LOG_HDR_AC_MSG_C is log ID 111, not 113
12/19/00   kss     Fixed type in FTC rate count packet.
12/13/00   aaj     Added bunch of other srch packet defs. Modified a few
11/28/00   aaj     Added Finger/Searcher/TA log packet types
11/28/00   kss     Added several packets that were not yet defined.
11/27/00   kss     Created.  Includes HDR packets only.
===========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "log_dmss.h"
#include "log.h"
#include "hdraddr.h"

#include "time_svc.h"

#include "hdrlogoem.h"
#include "hdrcom_api.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/
#define HDRLOG_VAR_SIZEOF( main_type, sub_type, sub_cnt ) \
           ( sizeof( main_type ) + (sub_cnt) * sizeof( sub_type ) )

#define HDRLOG_VAR_SIZEOF_2( main_type, sub_type, sub_cnt, \
           sub_sub_type, sub_sub_cnt )                     \
           ( sizeof( main_type ) +                         \
           ( (sub_cnt-1) * sizeof( sub_type ) ) +          \
           ( (sub_cnt) * (sub_sub_cnt-1) * sizeof( sub_sub_type ) ) )
           
/* EJECT */
/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

#define LOG_HDR_BASE2_C LOG_HDR_RESERVED_CODES_BASE_2_C
#define LOG_HDR_BASE3_C LOG_HDR_RESERVED_CODES_BASE_3_C

/*----------------------------------------------*/
/* LOG_HDR_BASE_C=93 (93-143 HDR reserved logs) */
/*----------------------------------------------*/

/* LOG_HDR_SEARCH_DATA_C moved to 44 */
#define LOG_HDR_HANDOFF_STATE_C        1 + LOG_HDR_BASE_C
#define LOG_HDR_ASP_C                  2 + LOG_HDR_BASE_C
#define LOG_HDR_RL_PKT_SUMMARY_C       3 + LOG_HDR_BASE_C
/* LOG_DRC_ARQ_BUFFER_C moved to 47 */
#define LOG_HDR_RTC_RATE_CNT_C         5 + LOG_HDR_BASE_C
#define LOG_HDR_REV_PCTRL_C            6 + LOG_HDR_BASE_C
/* LOG_HDR_FINGER_DATA_C moved to 45 */
#define LOG_HDR_DEBUG_HEX_DUMP_C       8 + LOG_HDR_BASE_C
#define LOG_HDR_ARQ_EFF_RCV_RATE_C     9 + LOG_HDR_BASE_C
/* LOG_HDR_FWD_RATE_SLOT_CNT_C obsolete */
#define LOG_HDR_LINK_SUMMARY_C        11 + LOG_HDR_BASE_C
#define LOG_HDR_POWER_C               12 + LOG_HDR_BASE_C
#define LOG_HDR_FWD_PKT_SNAPSHOT_C    13 + LOG_HDR_BASE_C
/* LOG_HDR_SESSION_ATTEMPT_C moved to 43 */
#define LOG_HDR_ACCESS_ATTEMPT_C      15 + LOG_HDR_BASE_C
#define LOG_HDR_RAB_BUFFER_C          16 + LOG_HDR_BASE_C
#define LOG_HDR_CONN_ATTEMPT_C        17 + LOG_HDR_BASE_C
#define LOG_HDR_PAGE_MSG_C            18 + LOG_HDR_BASE_C
#define LOG_HDR_CONN_RELEASE_C        20 + LOG_HDR_BASE_C
#define LOG_HDR_RAB_INFO_C            21 + LOG_HDR_BASE_C
#define LOG_HDR_HRES_TA_C             22 + LOG_HDR_BASE_C
#define LOG_HDR_REV_PKT_SNAPSHOT_C    23 + LOG_HDR_BASE_C

#define LOG_HDR_SIG_AC_MSG_C          25 + LOG_HDR_BASE_C
#define LOG_HDR_SIG_RTC_MSG_C         26 + LOG_HDR_BASE_C
#define LOG_HDR_SIG_CC_DIR_MSG_C      27 + LOG_HDR_BASE_C
#define LOG_HDR_SIG_FTC_MSG_C         28 + LOG_HDR_BASE_C
/* LOG_HDR_PILOT_SETS_C moved to 46 */
#define LOG_HDR_TCA_INFO_C            30 + LOG_HDR_BASE_C
#define LOG_HDR_SIG_CC_BCAST_MSG_C    31 + LOG_HDR_BASE_C

#define LOG_HDR_STATE_INFO_C          33 + LOG_HDR_BASE_C
/* LOG_HDR_FWD_RATE_STATS_C obsolete */
#define LOG_HDR_SECTOR_INFO_C         35 + LOG_HDR_BASE_C

#define LOG_HDR_FWD_STATS_SUMMARY_C   39 + LOG_HDR_BASE_C
#define LOG_HDR_AC_MAC_CAPSULE_C      40 + LOG_HDR_BASE_C
#define LOG_HDR_CC_MAC_PACKET_C       41 + LOG_HDR_BASE_C
#define LOG_HDR_RCV_ATI_LIST_C        42 + LOG_HDR_BASE_C
#define LOG_HDR_SESSION_ATTEMPT_C     43 + LOG_HDR_BASE_C
#define LOG_HDR_SEARCH_DATA_TYPE1_C   44 + LOG_HDR_BASE_C
#define LOG_HDR_FINGER_DATA_C         45 + LOG_HDR_BASE_C
#define LOG_HDR_PILOT_SETS_V2_C       46 + LOG_HDR_BASE_C
#define LOG_HDR_DRC_ARQ_BUFFER_C      47 + LOG_HDR_BASE_C
#define LOG_HDR_ACQ_INFO_C            48 + LOG_HDR_BASE_C
#define LOG_HDR_REACQ_INFO_C          49 + LOG_HDR_BASE_C
#define LOG_HDR_SLEEP_INFO_C          50 + LOG_HDR_BASE_C

/* MAX HDR log code, LOG_HDR_LAST_C = 50 + LOG_HDR_BASE_C */

/*-------------------------------------------------*/
/* LOG_HDR_BASE2_C=383 (383-403 HDR reserved logs) */
/*-------------------------------------------------*/

#define LOG_HDR_MCS_PKT_RATE_METRICS_C    0 + LOG_HDR_BASE2_C
#define LOG_HDR_MCS_PHY_CHAN_METRICS_C    1 + LOG_HDR_BASE2_C
#define LOG_HDR_MCS_ECB_STATUS_C          2 + LOG_HDR_BASE2_C
#define LOG_HDR_MCS_LOG_CHAN_METRICS_C    3 + LOG_HDR_BASE2_C
#define LOG_HDR_DEBUG_DISPLAY_C           4 + LOG_HDR_BASE2_C                 
                
/* Connection related log packets */          
#define LOG_HDR_TUNE_AWAY_INFO_C         5 + LOG_HDR_BASE2_C
#define LOG_HDR_TUNE_AWAY_RRI_INFO_C     6 + LOG_HDR_BASE2_C
#define LOG_HDR_CONN_TERM_STATS_C        7 + LOG_HDR_BASE2_C
#define LOG_HDR_CONN_STATE_PS_STATS_C    8 + LOG_HDR_BASE2_C
#define LOG_HDR_CONN_STATE_TX_STATS_C    9 + LOG_HDR_BASE2_C
#define LOG_HDR_CONN_STATE_RMAC_STATS_C 10 + LOG_HDR_BASE2_C
#define LOG_HDR_CONN_STATE_SRCH_STATS_C 11 + LOG_HDR_BASE2_C
                
/* **Obsolete** Forward traffic Channel MAC packet log code */
#define LOG_HDR_FTC_PHY_LAYER_PKT_C     12 + LOG_HDR_BASE2_C

/* **Obsolete** Reverse traffic Channel MAC packet log code */
#define LOG_HDR_RTC_PHY_LAYER_PKT_C     13 + LOG_HDR_BASE2_C 

/* New message log packets that include subtype */
#define LOG_HDR_SIG2_AC_MSG_C           14 + LOG_HDR_BASE2_C
#define LOG_HDR_SIG2_RTC_MSG_C          15 + LOG_HDR_BASE2_C
#define LOG_HDR_SIG2_CC_DIR_MSG_C       16 + LOG_HDR_BASE2_C
#define LOG_HDR_SIG2_FTC_MSG_C          17 + LOG_HDR_BASE2_C
#define LOG_HDR_SIG2_CC_BCAST_MSG_C     18 + LOG_HDR_BASE2_C

/* Forward Statistics Summary log packet for Revision-A Single-user packets */
#define LOG_HDR_REVA_SU_FWD_STATS_C     19 + LOG_HDR_BASE2_C

/* Forward Statistics Summary log packet for Revision-A Multi-user packets */
#define LOG_HDR_REVA_MU_FWD_STATS_C     20 + LOG_HDR_BASE2_C 

/* MAX HDR log code, LOG_HDR_LAST_2_C = 20 + LOG_HDR_BASE2_C */

/*-------------------------------------------------*/
/* LOG_HDR_BASE3_C=629 (629-704 HDR reserved logs) */
/*-------------------------------------------------*/

/* #define LOG_HDR_xxx_C                   0 + LOG_HDR_BASE3_C */

/* HDR Equalizer related log packets */
#define LOG_HDR_EQUALIZER_DATA_C           4 + LOG_HDR_BASE3_C
#define LOG_HDR_EQUALIZER_CI_BUFFER_C      5 + LOG_HDR_BASE3_C

/* MDSP debug logging related log packet */
#define LOG_HDR_MDSP_DEBUG_BUFFER_C        6 + LOG_HDR_BASE3_C

/* Revised search log with equalizer fields */
#define LOG_HDR_SEARCH_DATA_TYPE2_C        7 + LOG_HDR_BASE3_C

/* Rev A Reverse Link Gains */
#define LOG_HDR_REVA_RL_GAINS_C            8 + LOG_HDR_BASE3_C

/* Rev A Reverse Link Packet Metrics */
#define LOG_HDR_REVA_RL_PKT_METRICS_C      9 + LOG_HDR_BASE3_C

/* RTCMAC Subtype 3 T2P Resource related log packet */
#define LOG_HDR_REVA_RMAC3_T2P_RESOURCE_C 10 + LOG_HDR_BASE3_C

/* RTCMAC Subtype 3 Packet Determination log packet */
#define LOG_HDR_REVA_RMAC3_PKT_INFO_C     11 + LOG_HDR_BASE3_C

/* RAB update info log packets */
#define LOG_HDR_REVA_RAB_UPDATE_INFO_C    12 + LOG_HDR_BASE3_C

/* HDR DRC-ARQ-DSC log code */
#define LOG_HDR_DRC_ARQ_DSC_BUFFER_C      13 + LOG_HDR_BASE3_C

/* Multi-user Forward User Statistics Summary Log packet */
#define LOG_HDR_REVA_MUP_USER_STATS_C     14 + LOG_HDR_BASE3_C

/* Forward link pkt header info log packet */
#define LOG_HDR_FL_PHY_PKT_HEADER_C       15 + LOG_HDR_BASE3_C

/* Rev A Reverse Link ARQ Status */
#define LOG_HDR_REVA_RL_ARQ_STATUS_C      16 + LOG_HDR_BASE3_C

/* Rev A Reverse Link Packet Status */
#define LOG_HDR_REVA_RL_PKT_STATUS_C      17 + LOG_HDR_BASE3_C

/* Reserve codes for upcoming Rev A log packets */
#define LOG_HDR_REVA_RESERVED_09_C        18 + LOG_HDR_BASE3_C
#define LOG_HDR_REVA_RESERVED_10_C        19 + LOG_HDR_BASE3_C

/* Rev A PCP Packet Statistics */
#define LOG_HDR_REVA_PCP_PKT_STATS_C      20 + LOG_HDR_BASE3_C

/* Protocol subtypes and Stream Configuration for each personality */
#define LOG_HDR_SUBTYPE_STREAM_CONFIG_C   21 + LOG_HDR_BASE3_C 

/* Rev A RUP Trigger Code List */
#define LOG_HDR_REVA_RUP_TC_LIST_C        22 + LOG_HDR_BASE3_C

/* PB Multicast Packet Rate Metrics */
#define LOG_HDR_PB_MCS_PKT_RATE_METRICS_C 27 + LOG_HDR_BASE3_C

/* Reserved for upcoming RevA log packets */
#define LOG_HDR_PB_MCS_TIMING_SYNC_ALGO_C 28 + LOG_HDR_BASE3_C

#define LOG_HDR_ENH_ACCESS_ATTEMPT_C      29 + LOG_HDR_BASE3_C
#define LOG_HDR_ACCESS_PROBE_C            30 + LOG_HDR_BASE3_C

/* Multi-carrier  Forward link pkt header info log packet */
#define LOG_HDR_MC_FL_PHY_PKT_HEADER_C    31 + LOG_HDR_BASE3_C

/* Multi-carrier Finger data log packet */
#define LOG_HDR_MC_FINGER_DATA_C          32 + LOG_HDR_BASE3_C

/* Multi-carrier Air Link Summary log packet */
#define LOG_HDR_MC_LINK_SUMMARY_C         33 + LOG_HDR_BASE3_C

/* Multi-carrier Searcher Data log packet */
#define LOG_HDR_MC_SEARCH_DATA_C          34 + LOG_HDR_BASE3_C 

/* Multi-carrier DRC_ARQ_DSC log packet */
#define LOG_HDR_MC_DRC_ARQ_DSC_BUFFER_C   35 + LOG_HDR_BASE3_C

/* RTCMAC Subtype 4 T2P Resource related log packet */
#define LOG_HDR_REVB_RMAC4_T2P_RESOURCE_C 36 + LOG_HDR_BASE3_C

/* RTCMAC Subtype 4 Packet Determination log packet */
#define LOG_HDR_REVB_RMAC4_PKT_INFO_C     37 + LOG_HDR_BASE3_C

/* RTCMAC Packet Build/Load Time log packet */
#define LOG_HDR_RMAC_PKT_TIME_INFO_C      38 + LOG_HDR_BASE3_C

/* Multi-carrier Power log packet */
#define LOG_HDR_MC_POWER_C                39 + LOG_HDR_BASE3_C

/* Multi-carrier RL Gains log packet */
#define LOG_HDR_MC_RL_GAINS_C             40 + LOG_HDR_BASE3_C 

/* Multi-carrier RL ARQ Status log packet */
#define LOG_HDR_MC_RL_ARQ_STATUS_C        41 + LOG_HDR_BASE3_C

/* Multi-carrier RL Metrics log packet */
#define LOG_HDR_MC_RL_METRICS_C           42 + LOG_HDR_BASE3_C

/* Multi-carrier RAB Update Information log packet */
#define LOG_HDR_MC_RAB_UPDATE_INFO_C      43 + LOG_HDR_BASE3_C

/* Multi-carrier Pilot Sets Log packet */
#define LOG_HDR_MC_PILOT_SETS_C           44 + LOG_HDR_BASE3_C

/* Multi-carrier Single-user packet Forward Statistics log packet */
#define LOG_HDR_MC_SUP_FWD_STATS_C        45 + LOG_HDR_BASE3_C

/* Multi-carrier Multi-user packet Forward Statistics log packet */
#define LOG_HDR_MC_MUP_FWD_STATS_C        46 + LOG_HDR_BASE3_C

/* Revision A Sleep Info log packet supporting enhanced idle protocol. */
#define LOG_HDR_ENH_SLEEP_INFO_C          47 + LOG_HDR_BASE3_C

/* Multicarrier RL Packet Status log packet */
#define LOG_HDR_MC_RL_PKT_STATUS_C        53 + LOG_HDR_BASE3_C

/* Multicarrier Equalizer CI Buffer log packet */
#define LOG_HDR_MC_EQUALIZER_CI_BUFFER_C  54 + LOG_HDR_BASE3_C

#define LOG_HDR_PB_MCS_NOISE_VAR_EST_C    55 + LOG_HDR_BASE3_C

/* Multicarrier equalizer data log packet */
#define LOG_HDR_MC_EQUALIZER_DATA_C       56  + LOG_HDR_BASE3_C

#define LOG_HDR_PB_MCS_CHAN_EST_ALGO_C    57 + LOG_HDR_BASE3_C

/* Multi-carrier PA Headroom and Carrier Management log packet */
#define LOG_HDR_MC_PA_HDRM_AND_CARRIER_MGMT_C      \
                                          58 + LOG_HDR_BASE3_C

/* Forward link physical layer frame dump log packet */
#define LOG_HDR_FL_PHY_LAYER_PKT_C        59 + LOG_HDR_BASE3_C

/* Reverse link physical layer frame dump log code */
#define LOG_HDR_RL_PHY_LAYER_PKT_C        60 + LOG_HDR_BASE3_C 

#define LOG_HDR_MC_MUP_USER_STATS_C       61 + LOG_HDR_BASE3_C

/* RevB PCP log packet */
#define LOG_HDR_REVB_PCP_PKT_STATS_C      62 + LOG_HDR_BASE3_C

/* Platinum Broadcast Pilot Detection Algorithm log packet */
#define LOG_HDR_PB_MCS_PDET_ALGO_C        63 + LOG_HDR_BASE3_C

/* Overhead Cache Summery log packet */
#define LOG_HDR_OVHD_CACHE_SUMMARY_C      64 + LOG_HDR_BASE3_C

/* Pilot Sets V3 log packet */
#define LOG_HDR_PILOT_SETS_C              65 + LOG_HDR_BASE3_C

/* ARD event log packet */
#define LOG_HDR_ARD_EVENT_C               68 + LOG_HDR_BASE3_C

/*  69 is taken by TxD log packet */
#define LOG_HDR_STXD_EVENT_C              69 + LOG_HDR_BASE3_C

/* FlexConn log packet for RevA */
#define LOG_HDR_REVA_FLEXCONN_INFO_C      70 + LOG_HDR_BASE3_C

/* DTX log packet for RevA */
#define LOG_HDR_REVA_DTX_INFO_C           71 + LOG_HDR_BASE3_C

/* FlexConn log packet for RevB */
#define LOG_HDR_MC_FLEXCONN_INFO_C        72 + LOG_HDR_BASE3_C

/* DTX log packet for RevB */
#define LOG_HDR_MC_DTX_INFO_C             73 + LOG_HDR_BASE3_C
/* MAX HDR log code, LOG_HDR_LAST_3_C = 75 + LOG_HDR_BASE3_C */

/*---------------------------------------------------------------------------
                            HDR LOG TYPES
 The log types definitions listed below are in the order of the log codes.
 ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                          LOG_HDR_SEARCH_DATA_TYPE1/TYPE2_C
  ---------------------------------------------------------------------------*/

#define HDR_LOG_MAX_SEARCH_PILOTS   6
/* Maximum active set pilots that can be logged in one packet */

#define HDR_BYTES_IN_SYSTEM_TIME    5
/* number of bytes used for system time in the log packet     */

#define HDR_SLOTS_PER_ONE_ROLL      16
/* Slots in each PN roll */

#define HDR_LOG_MAX_CARRIERS        3
/* Maximum number of carriers */

typedef PACK(struct)
{
  PACK(struct)
  {
    uint16   pn       : 9;        /* Bit8 :0  :: PN Offset */
    uint16            : 1;        /* Bit9     :: Reserved */
    uint16   ant_sel  : 2;        /* Bit11:10 :: Antenna Select */
    uint16   pilot_set: 3;        /* Bit14:12 :: Pilot Set Type */
    uint16            : 1;        /* Bit15    :: Reserved */
  } pilot_pn;                     /* pilot pn related information */
  
  uint16  win_center_x2;     
    /* Position of window center in chipx2 units    */

  uint16  earliest_pos_x2;   
    /* Position of the earliest peak in chipx2 units */

  PACK(struct)
  {
    sint15  peak_pos[HDR_LOG_MAX_SEARCH_PILOTS];         
      /* MSTR compensated position of peaks in chipx2 units */

    uint16  peak_eng[HDR_LOG_MAX_SEARCH_PILOTS];         
      /* Energy of peaks */
  }peak_info;

  PACK(struct)
  {
    uint8   peaks_used;          /* Peaks used for center of mass computation */
    int8    peak_eng_thresh;     /* Energy threshold for peaks considered
                                     for center of mass computation */
    int16   com_posx2;           /* Center of mass RTC offset in chipx2 units */

  } com_info;                    /* Center of mass related information */
} log_hdr_search_data_type;

LOG_RECORD_DEFINE(LOG_HDR_SEARCH_DATA_TYPE2_C)
  uint8  version;
    /* The version number of the packet. */

  uint8    srch_state;         /* searcher state */
    /* 0 : Start, 1 : Acq, 2 : Sync, 3 : Idle, 4 : connected */

  uint8    srch_substate;      /* searcher substate. Value depends on state */

  uint16   srch_win;           /* search window size in chipx1 */

  PACK(struct)
  {
    uint16 chan_no      :   11;     /* Bit10:0  Channel Number    */
    uint16 band_class   :    5;     /* Bit15:11 Band Class        */
  }cdma_freq;


  PACK(struct)
  {
    uint32 mstrx8       :   18;     /* Bit17:0 MSTR in chipx8 units */
    uint32 reserved     :   14;     /* Bit31:18 Reserved            */
  } srch_info;

  uint8    num_pilots;         /* Number of pilots */

  PACK(struct)
  {
    uint8   flag        :    1; /* Flag to indicate if equalizer is on */
    uint8   reserved    :    7; /* reserved */
  } eq;

  uint8    best_asp_idx;       /* best active set pilot index */
  log_hdr_search_data_type pilots[1];
    /* this is a variable length field with num_pilots occurences */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_FL_PHY_PKT_HEADER_C
  ---------------------------------------------------------------------------*/

#define LOG_HDR_FL_PHY_PKT_HEADER_VERSION         3

typedef PACK(struct)
{
  uint16     hstr;
    /* half slot time reference */

  uint8      data_len;
    /* length of data portion of the PHY packet, in 32-bit multiples */

  uint8      drc_value;
    /* DRC value for the current incoming packet */

  uint8      pkt_type;
    /*  0x00  Single-user Traffic Channel Packet
     *  0x08  Multi-user Traffic Channel Packet decoded
     *        at Mac index MUP_PREAMBLE_BASE
     *  0x09  Multi-user Traffic Channel Packet decoded
     *        at Mac index MUP_PREAMBLE_BASE+1 
     *  0x0A  Multi-user Traffic Channel Packet decoded
     *        at Mac index MUP_PREAMBLE_BASE+2 
     *  0x0B  Multi-user Traffic Channel Packet decoded
     *        at Mac index MUP_PREAMBLE_BASE+3
     *  0x0C  Multi-user Traffic Channel Packet decoded
     *        at Mac index MUP_PREAMBLE_BASE+4 
     *  0x10  Release-0 (std.) Control Channel Packet
     *  0x11  Rev-A (short) Control channel packet
     *  0x12  Rev-A Control channel packet received at 
     *        user-defined Short packet MAC index
     *  0x18  Broadcast
     *  0x19  Enhanced BroadCast 
     *  0x1A  Platinum Broadcast */    

  uint8     seq_num;
    /* Sequence number for re-ordering packet */

  PACK(struct)
  {
    uint16      slots_to_decode   : 6;
      /* Number of slots taken to decode */

    uint16      crc_status        : 2; 
      /* Indicates whether CRC status
       * It could be one of the following:
       * 0x00  CRC Failure
       * 0x01  CRC Pass
       * 0x02  CRC False Pass 
       * 0x03  Packet Cancelled */

    uint16      pd_type           : 2; 
      /* Indicates Preamble Detection Type
       * 0x00 Normal Preamble Detection
       * 0x01 CC Forced Preamble Detection 
       * 0x02 TC Forced Preamble Detection */

    uint16      reserved_bits     : 6;
      /* Bits 15:9 */
  } bit_fields;

} log_hdr_fl_pkt_header_info_type;


LOG_RECORD_DEFINE(LOG_HDR_FL_PHY_PKT_HEADER_C)      

  uint8  version;
    /* The version number of the packet. */

  uint8  rec_cnt;
    /* Number of pkt record in the log pkt */
  
  log_hdr_fl_pkt_header_info_type fl_pkt_header_info[1];
    /* The array to hold the pkt header records */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_HANDOFF_STATE_C
---------------------------------------------------------------------------*/

typedef PACK(struct)
{
  uint16     pilot_pn;
  uint8      drc_cover;
  uint8      mac_index;
  uint8      rpc_index;
  uint8      pilot_strength;      /* pilot strength in 0.5dB. [0,63] */

} log_hdr_handoff_state_type;

LOG_RECORD_DEFINE(LOG_HDR_HANDOFF_STATE_C)

  uint8    srch_state;    /* 0: st, 1: acq, 2: sync, 3:idle, 4:Tc */
  uint8    aset_cnt;      /* aset_cnt number of records of the following */

  log_hdr_handoff_state_type aset[1];
    /* Additional memory will be allocated for (aset_cnt-1) number
       of fields */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_ASP_C
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(LOG_HDR_ASP_C)

  uint8  aset_cnt;

  uint16 best_pn;
  uint8  best_drc_cover;
  uint8  best_rpc_index;
  uint16 best_sinr;

  uint16 serving_pn;
  uint8  serving_drc_cover_index;
  uint8  serving_rpc_index;
  uint16 serving_sinr;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_REVA_RAB_UPDATE_INFO_C
---------------------------------------------------------------------------*/

#define HDRLOG_ASET_MAX_SIZE              6
  /* The number of ASP */

#define HDRLOG_RAB_UPDATE_INFO_SUBFRAMES  8
  /* Number of subframes in a RAB update info log packet */

#define HDRLOG_RAB_UPDATE_INFO_VERSION    0
  /* Version of RABUpdateInfo log packet */

/* QRAB value used in log: 1: loaded, 0: unloaded */
typedef enum
{
  HDRLOG_QRAB_LOADED     = 1,
  HDRLOG_QRAB_UNLOADED   = 0
} hdrlog_qrab_enum_type;

typedef PACK(struct)
{
  uint8      subframe_n;
    /* The subframe being logged */
  
  uint8      serving_asp;
    /* Serving sector */
  
  uint16     pilot_strength[ HDRLOG_ASET_MAX_SIZE ];
    /* Pilot strenght per ASP */

  int16      frab;
    /* FRAB soft decision max at subframe */

  int16      slot_frab_per_asp[ HDRLOG_ASET_MAX_SIZE ];
    /* SlotFRAB per ASP */

  PACK(struct)  
  {
    uint16    qrab             : 1;
      /* QRAB hard decision */

    uint16    qrab_ps          : 1;
      /* QRABps hard decision */

    uint16    drc_lock_per_asp : 6;
      /* DRCLock per ASP */

    uint16    slot_qrab_per_asp: 6;
      /* SlotQRAB per ASP */

    uint16    reserved_bits    : 2;
      /* Reserved bits */

  } bit_info;

} log_hdr_rab_update_info_type;

LOG_RECORD_DEFINE(LOG_HDR_REVA_RAB_UPDATE_INFO_C)
  
  uint8                         version;
    /* The version number of the packet. */
  
  uint8                         count;
    /* Number of subframes in the log packet */

  log_hdr_rab_update_info_type  
    rab_update_info[ HDRLOG_RAB_UPDATE_INFO_SUBFRAMES ];
    /* Log for each sub frame */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_RL_PKT_SUMMARY_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_RL_PKT_SUMMARY_C)
  uint8   rri_limit;
  uint8   curr_rate;
  uint8   comb_busy_bit;
  uint32  rand_var;
  uint32  trans_probability;
  uint8   pa_max;
  uint8   pred_rri;
  uint8   tx_rri;
  uint8   num_pad_bytes;
LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_DRC_ARQ_BUFFER_C
---------------------------------------------------------------------------*/
#define HDRLOG_DRC_ARQ_FRAMES_PER_PKT      2

typedef PACK(struct)
{
  /* This packet has been packed in the same format as the data
     read from the DSP so that DMSS does not have to waste precious
     cycles in unpacking and repacking the data */

  uint16 drc [HDR_SLOTS_PER_ONE_ROLL];
  /* drc is bit-packed in the same way as mDSP register to avoid 
     un-necessary unpacking and re-packing overhead.
        Bit2 :Bit0  :: DRC cover
        Bit7 :Bit3  :: Reserved
        Bit11:Bit8  :: DRC value
        Bit12       :: ACK/NACK
        Bit13       :: ACK channel enabled
        Bit15:Bit14 :: Reserved
  */

  uint8      slot_count;

  /* ------------ */
  /*   DRC Lock   */
  /* ------------ */

  uint16     drc_lock;
    /* -------------------------------------
       Bit 0 - DRC lock for ASP1
       Bit 1 - DRC lock valid bit for ASP1
       Bit 2 - DRC lock for ASP2
       Bit 3 - DRC lock valid bit for ASP2
       ...
       Bit 10 - DRC lock for ASP6
       Bit 11 - DRC lock valid bit for ASP6
       ------------------------------------
    */

  uint8      drc_lock_update;
    /* Last slot in which the DRC lock was updated */

  /* ------------- */
  /* ASP RL and FL */
  /* ------------- */

  PACK(struct)
  {
    uint8    asp_rl : 3; /* Bit2 :Bit0 :: RL ASP */
    uint8    asp_fl : 3; /* Bit5 :Bit3 :: FL ASP */
    uint8           : 2; /* Bit7 :Bit6 :: Reserved */

  } best_asp_index[ HDR_SLOTS_PER_ONE_ROLL ];

}log_hdr_drc_arq_buffer_type;

LOG_RECORD_DEFINE(LOG_HDR_DRC_ARQ_BUFFER_C)

log_hdr_drc_arq_buffer_type drc_arq[HDRLOG_DRC_ARQ_FRAMES_PER_PKT];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_RTC_RATE_CNT_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_RTC_RATE_CNT_C)
  PACK(struct)
  {
    uint32 rate9600;
    uint32 rate19200;
    uint32 rate38400;
    uint32 rate76800;
    uint32 rate153600;
  } rates;
LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_AT_CONFIG_C
---------------------------------------------------------------------------*/
// Implemented as a diag packet (refer to hdrlog.{h,c})

/*---------------------------------------------------------------------------
                           LOG_HDR_REV_PCTRL_C
---------------------------------------------------------------------------*/
#define HDR_MAX_FRAMES_PER_REV_PCTRL_PKT 10

typedef PACK(struct)
{
  int16    rx_agc;              // Receive signal strength
  int16    tx_total;            // Tx total power
  int16    tx_adjust;           // Tx closed loop adjustment value
  int16    rpc_history;         // RPC decisions for last frame
  int16    tx_pilot;            // Tx pilot power

  int16    drc_channel_gain;    // Current DRC channel gain
  int16    ack_channel_gain;    // Current ACK channel gain
  int16    data_channel_gain;   // Current DATA channel gain
  int16    pilot_channel_gain;  // Current pilot channel gain

  PACK(struct)
  {
    uint8  max_tx_pwr_rri : 4;  // Maximum recommended RRI value based on
    uint8  drc_lock_per   : 4;  // DRC Lock period
  } packed_info;

} log_hdr_rev_pctrl_type;

LOG_RECORD_DEFINE(LOG_HDR_REV_PCTRL_C)

  uint8 num_records;
    /* Number of frames logged in this packet (max 10) */

  log_hdr_rev_pctrl_type rev_pctrl_data[ HDR_MAX_FRAMES_PER_REV_PCTRL_PKT ];
    /* Variable length packet */

LOG_RECORD_END
/*---------------------------------------------------------------------------
                          LOG_HDR_FINGER_DATA_C
---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  uint16   pilot_pn;        /* pn offset the finger is demodulating */
  uint32   posx8;           /* rtc offset position of fing in chipx8 */
  uint16   rssi;            /* C/I equivalent of the finger */
                            /* combined for both antennaes  */

  PACK(struct)
  {
    uint8    locked     : 1;  /* locked flag 1: locked, 0: unlocked */
    uint8    antenna    : 1;  /* 0 : antenna 0, 1 : Antenna1 */
    uint8    diversity  : 1;  /* 0/1 : diversity disabled/enabled for fing*/
    uint8    fing_index : 4;  /* index of the finger [0-11] */
  } fing_info;

  PACK(struct)
  {
    uint8    rpc        : 3;  /* Reverse Power Control combiner cell index */
    uint8    asp        : 3;  /* Active set pilot to which fing belongs */
  } fing_sect;

  /* meaningful when diversity is enabled */
  uint16     ant0_rssi;       /* C/I equivalent of the finger for antenna 0 */
  uint16     ant1_rssi;       /* C/I equivalent of the finger for antenna 1 */

} log_hdr_finger_data_type;

LOG_RECORD_DEFINE(LOG_HDR_FINGER_DATA_C)
  uint8    srch_state;      /* State of the hdrsrch task */

  uint32   mstrx8;          /* Mobile Station time reference */
  uint16   mstr_err_x8;     /* MSTR w.r.t. to earliest arriving finger */
  uint16   mstr_pn;         /* Reference pilot PN */

  uint8     num_fings;      /* number of fingers [0-11] */

  log_hdr_finger_data_type fing_data[1];
    /* num_fings entries of the above struct */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_DEBUG_HEX_DUMP_C
---------------------------------------------------------------------------*/
typedef uint8 hdrlog_debug_hex_dump_type;

LOG_RECORD_DEFINE(LOG_HDR_DEBUG_HEX_DUMP_C)

  uint16 pkt_length;

  /* variable length - for debugging only (use HEX dump) */
  hdrlog_debug_hex_dump_type  data[1];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_ARQ_EFF_RCV_RATE_C
---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  PACK(struct)
  {
    uint8 dec_rate  : 4;    /* Decoded rate of the packet */
    uint8 dec_slots : 4;    /* number of slots required to decode pkt */
  } dec_info;

  PACK(struct)
  {
    uint16  hstr : 9;       /* HSTR when the preamble of this pkt was found */
    uint16  pad  : 6;       /* zero padding */
    uint16  crc  : 1;       /* whether or not CRC passed */
  } pkt_info;

} log_hdr_arq_eff_rcv_rate_type;

LOG_RECORD_DEFINE(LOG_HDR_ARQ_EFF_RCV_RATE_C)

  uint8   num_pkts;          /* Number of packets included below */

  log_hdr_arq_eff_rcv_rate_type arq_info[1];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                LOG_HDR_FTC_RATE_SLOT_COUNT_C (Obsolete)
                     Merged in FWD_STATS_SUMMARY
---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------
                            LOG_HDR_LINK_SUMMARY_C
---------------------------------------------------------------------------*/
#define HDRLOG_BUFFER_SIZE 16
#define HDRLOG_ASP_NUM      6
#define HDRLOG_FRAMES_PER_PKT 2

typedef PACK(struct)
{
  uint8  drc_buffer[8];
  uint16 basp_sinr[16];

  PACK(struct)
  {
    uint8 best_asp_pred : 4;   /* best ASP predicted index */
    uint8               : 2;   /* zero padding */
    uint8 pa_state      : 1;   /* pa state */
    uint8 rxed_flag     : 1;   /* packet rxed boolean */
  } packed_field;

  uint16 aspfilt_sinr[6];
  uint16 pn_asp[6];
  int16  rpc[6];
  uint16 per_inst;
  uint16 per_seq;
  uint16 hstr;
  uint8  rpc_index[6];
} hdrlog_link_summary_type;

LOG_RECORD_DEFINE(LOG_HDR_LINK_SUMMARY_C)

  hdrlog_link_summary_type link_summary[HDRLOG_FRAMES_PER_PKT];

LOG_RECORD_END


/*---------------------------------------------------------------------------
                              LOG_HDR_POWER_C
---------------------------------------------------------------------------*/

#define HDRLOG_AT_TYPE_MSM5500 2

typedef PACK(struct)
{
  PACK(struct)
  {
    uint8  at_type      : 4;
    uint8  ratchet_mode : 1;
    uint8  pa_state     : 1;
    uint8               : 2;   /* zero padding */
  } power_info;

  int16  tx_open_loop_power;
  int16  tx_closed_loop_adjust;
  int16  tx_pilot_power;
  int16  tx_total_power;
  int16  rx_agc0;
  int16  rx_agc1;
} hdrlog_power_type;

LOG_RECORD_DEFINE(LOG_HDR_POWER_C)

  hdrlog_power_type power[HDRLOG_FRAMES_PER_PKT];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_FWD_PKT_SNAPSHOT_C
---------------------------------------------------------------------------*/
typedef enum
{
  HDR_FWD_SNAPSHOT_CONTROL_CHANNEL = 0,
  HDR_FWD_SNAPSHOT_TRAFFIC_CHANNEL
} hdrlog_fwd_snapshot_channel_enum_type;

typedef uint8 hdrlog_fwd_pkt_snapshot_type;

LOG_RECORD_DEFINE(LOG_HDR_FWD_PKT_SNAPSHOT_C)

  uint8 pkt_type;   /* hdrlog_fwd_snapshot_channel_enum_type */
  uint8 pkt_length;

  /* variable length - for debugging only (use HEX dump) */
  hdrlog_fwd_pkt_snapshot_type  data[1];
LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_SESSION_ATTEMPT_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_SESSION_ATTEMPT_C)

  uint8  trans_id;
  uint8  result;
  uint32 rati;
  uint16 duration;
  uint32 success_count;
  uint32 failure_count;
  uint32 attempts_count;
  uint16 pn;
  uint8  color_code;
  hdraddr_type full_uati;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                            LOG_HDR_ACCESS_ATTEMPT_C
---------------------------------------------------------------------------*/
/* Note that this is a variable length record, with the length depending
** on the number of probe sequences performed.
*/
#define HDRLOG_AC_MAX_PROBE_CNT  16

typedef enum
{
  HDRLOG_AMAC_ACACK_NOT_RECEIVED = 0,
  HDRLOG_AMAC_ACACK_RECEIVED,
  HDRLOG_AMAC_TCA_MSG_RECEIVED,
  HDRLOG_AMAC_PROBE_INTERRUPTED
} hdrlog_access_result_enum_type;

LOG_RECORD_DEFINE(LOG_HDR_ACCESS_ATTEMPT_C)

  PACK(struct)
  {
    uint8  transaction_id;
    uint8  probe_num_step;
    uint8  max_probe_sequence;
    uint8  probe_sequence_count;
    uint8  result;               /* hdrlog_access_result_enum_type */
    uint16 duration;
    uint32 success_count;
    uint32 failure_count;
    uint32 attempts_count;
    uint16 pn;
    uint16 sector_id_lsw;        /* lower 16 bits of sector id */
    uint8  sector_id_usw;        /* upper 8 bits of sector id */
    uint8  color_code;

    uint8  probe_count[HDRLOG_AC_MAX_PROBE_CNT];

  } acmac_log;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                            LOG_HDR_ENH_ACCESS_ATTEMPT_C
---------------------------------------------------------------------------*/
/* Note that this is a variable length record, with the length depending
** on the number of probe sequences performed.
*/

typedef PACK(struct)
  {
    uint8  transaction_id;
    uint8  probe_num_step;
    uint8  max_probe_sequence;
    uint8  probe_sequence_count;
    uint8  result;               /* hdrlog_access_result_enum_type */
    uint16 duration;
    uint32 success_count;
    uint32 failure_count;
    uint32 attempts_count;
    uint16 pn;
    uint16 sector_id_lsw;        /* lower 16 bits of sector id */
    uint8  sector_id_usw;        /* upper 8 bits of sector id */
    uint8  color_code;
    uint8  probe_count[HDRLOG_AC_MAX_PROBE_CNT];

  } acmac_rev0_log_type;

LOG_RECORD_DEFINE(LOG_HDR_ENH_ACCESS_ATTEMPT_C)

  PACK(struct)
  {
    uint8  version;
    uint8  max_pn_delay;           /* Max PN Randomization Delay */
    uint8  enh_access_params_inc;
    int16  probe_pwr_adjust;
    uint8  preamble_length_slots;  /* preamble length in Rev A. mode */
    uint8  access_offset;          /* access offset specified by AN */
    uint8  sector_access_max_rate; /* max sector access rate */
    uint8  payload_size;           /* size of data sent in access probe */
    acmac_rev0_log_type rev0_log;

  } enh_acmac_log;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                            LOG_HDR_ACCESS_PROBE_C
---------------------------------------------------------------------------*/
/* Note that this is a variable length record, with the length depending
** on the number of frames within a probe.
*/

#define HDRLOG_AC_MAX_FRAME_CNT  16

typedef PACK(struct)
  {
    int32  pilot_power;       /* pilot power for this frame */
    int32  total_power;       /* total power for this frame */
    uint8  pa_headroom;       /* PA headroom available during this frame */
    uint8  frame_data_size;   /* size of data in this frame */
    uint8  frame_rate;        /* rate at which frame was transmitted */

  } log_frame_info_type;

LOG_RECORD_DEFINE(LOG_HDR_ACCESS_PROBE_C)

  PACK(struct)
  {
    uint8  version;
    uint16 reserved;
    uint8  probe_seq_num;     /* number of sequence this probe was part of */
    uint8  probe_num;         /* probe number within a sequence */
    int32  preamble_pwr;      /* preamble power for this access probe */
    uint8  frame_count;       /* number of frames within this access probe */
    boolean probe_delayed;

    log_frame_info_type frame_info[HDRLOG_AC_MAX_FRAME_CNT]; /* frame info */

  } probe_log;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_RAB_BUFFER_C
---------------------------------------------------------------------------*/
#define HDR_RAB_BUFFER_LEN 6

LOG_RECORD_DEFINE(LOG_HDR_RAB_BUFFER_C)

  uint16  rab[HDR_RAB_BUFFER_LEN];
    /* contains the reverse activity bits for the past 16 slots for
       each ASP. Bit15 to Bit0 in each word represent the RAB received
       for that ASP in the past 16 slots. There are 6 entries of this
       word corresponding to 6 ASPs */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_CONN_ATTEMPT_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_CONN_ATTEMPT_C)

  uint8  trans_id;
  uint8  msg_seq;
  uint8  result;
  uint16 duration;
  uint32 success_count;
  uint32 failure_count;
  uint32 attempts_count;
  uint16 pn;

  uint16 sector_id_lsw;        /* lower 16 bits of sector id */
  uint8  sector_id_usw;        /* upper 8 bits of sector id */
  uint8  color_code;
  uint8  num_ho;               /* number of handoffs */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_PAGE_MSG_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_PAGE_MSG_C)

  uint16 hstr;
  uint32 attempts_count;
  uint16 pn;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_CONN_RELEASE_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_CONN_RELEASE_C)

  uint8  reason;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_RAB_INFO_C
---------------------------------------------------------------------------*/

typedef PACK(struct)
{
  uint8     rab_len;          /* Number of slots over which RAB is sent */
  uint8     rab_offset;       /* Frame offset for the RAB bits */
  uint16    pn_offset;        /* The PN sector sending the RABit */

}log_hdr_rab_info_type;

LOG_RECORD_DEFINE(LOG_HDR_RAB_INFO_C)

  uint8     pilot_cnt;
    /* Count of active set pilots */

  log_hdr_rab_info_type   rab_info[1];
    /* Variable length field. There are 'pilot_cnt' number of entries of this
       field */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_HRES_TA_C
---------------------------------------------------------------------------*/
#define HDR_LOG_MAX_HRES_TA_LEN  128

LOG_RECORD_DEFINE(LOG_HDR_HRES_TA_C)

  uint16     ta_start_x2;     /* start pos of trace in chipx2 */
  uint8      antenna;         /* 00: ant0, 01: ant1, 1x: both */
  uint8      ta_win_size;     /* Search window in x1. max 64 */
  uint8      num_samples;
  uint16     pk_eng[HDR_LOG_MAX_HRES_TA_LEN];     /* pilot eng at x2 positions */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_REV_PKT_SNAPSHOT_C
---------------------------------------------------------------------------*/
typedef enum
{
  HDR_REV_SNAPSHOT_ACCESS_CHANNEL = 0,
  HDR_REV_SNAPSHOT_TRAFFIC_CHANNEL
} hdrlog_rev_snapshot_channel_enum_type;

typedef uint8 hdrlog_rev_pkt_snapshot_type;

LOG_RECORD_DEFINE(LOG_HDR_REV_PKT_SNAPSHOT_C)

  uint8 pkt_type;    /* hdrlog_rev_snapshot_channel_enum_type  */
  uint8 pkt_length;

  /* variable length - for debugging only (HEX dump) */
  hdrlog_rev_pkt_snapshot_type  data[1];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_RATE_IDX_MAP_C
---------------------------------------------------------------------------*/
/* Second record to be logged after the ATConfiguration record
** Obsolete: No longer necessary.
*/

/*---------------------------------------------------------------------------
                           LOG_HDR_SIG_AC_MSG_C
---------------------------------------------------------------------------*/

#define HDRLOG_MAX_MSG_BYTES 1024

/* number of bytes in msg log pkt other than the variable message body */
#define HDRLOG_MSG_BUF_FIXED_SIZE 11
            
typedef enum
{
  HDRLOG_APP_DEFAULT   = 0, /* Default Packet App Bound to AN/SN   */
  HDRLOG_APP_MULTIFLOW = 1, /* Multiflow Packet App Bound to AN/SN */
  HDRLOG_APP_CSNA      = 2, /* Circuit Service Notification Application */
  HDRLOG_APP_ENH_MULTIFLOW       = 3, /* Enhanced Multiflow Packet App */
  HDRLOG_APP_MULTILINK_MULTIFLOW = 4  /* MultiLink Multiflow Packet App */
    /* Multimode Capability Discovery Application can be identified 
     * by its protocol ID, so no need for an app_subtype */

  /* MAX = 7 */
} hdrlog_hdr_app_enum_type;

LOG_RECORD_DEFINE(LOG_HDR_SIG_AC_MSG_C)

  uint8          band;      /* sys_band_class_e_type */
  uint16         chan_num;  /* sys_channel_num_type */
  uint16         pilot_pn;  /* sys_pilot_pn_type */
  uint16         hstr;      /* hdrdec_half_slot_time_type */

  PACK(struct)
  {
    uint8   reliable         : 1;
    uint8   fragmented       : 1;
    uint8   ack_seq_no_valid : 1;
    uint8   seq_no_valid     : 1;
    uint8   is_hdris890      : 1;
    uint8   app_type         : 3; /* Not a flag, but used to determine the app 
                                     type for a given stream 
                                     (except for the test app (IS890)) */
  } flags;

  uint8          seq_no;
  uint8          ack_seq_no;
  boolean        scc_flag;
  uint8          msg[ HDRLOG_MAX_MSG_BYTES ];
    /* Actual access channel message bits. */
    /* starts with uint8 prot_type, uint8 msg_id, then the message body */

LOG_RECORD_END

/* Message log types for all channels have the same structure */
typedef LOG_HDR_SIG_AC_MSG_C_type log_hdr_msg_type;

/*---------------------------------------------------------------------------
                     LOG_HDR_SIG_RTC_MSG_C
---------------------------------------------------------------------------*/

typedef LOG_HDR_SIG_AC_MSG_C_type LOG_HDR_SIG_RTC_MSG_C_type;

/*---------------------------------------------------------------------------
                     LOG_HDR_SIG_FTC_MSG_C
---------------------------------------------------------------------------*/

typedef LOG_HDR_SIG_AC_MSG_C_type LOG_HDR_SIG_FTC_MSG_C_type;

/*---------------------------------------------------------------------------
                     LOG_HDR_SIG_CC_DIR_MSG_C
---------------------------------------------------------------------------*/

typedef LOG_HDR_SIG_AC_MSG_C_type LOG_HDR_SIG_CC_DIR_MSG_C_type;

/*---------------------------------------------------------------------------
                          LOG_HDR_PILOT_SETS_C ( V3 )
---------------------------------------------------------------------------*/

#define HDR_LOG_ASET_CNT   6
#define HDR_LOG_CSET_CNT   6
#define HDR_LOG_NSET_CNT   40

typedef enum
{
  HDRLOG_PILOT_EMPTYSET,             /* HDR Sector NULL type */
  HDRLOG_PILOT_RSET,                 /* HDR Remaining Set sector type */
  HDRLOG_PILOT_NSET,                 /* HDR Neighbor Set sector type */
  HDRLOG_PILOT_CSET,                 /* HDR Candidate Set sector type */
  HDRLOG_PILOT_ASET,                 /* HDR Active Set sector type */
  HDRLOG_PILOT_MAX_NUM_SETS          /* Maximum number of set types */
} log_hdr_pilot_sets_enum_type;

typedef PACK(struct)
{
  uint16  pilot_pn;          /* pilot PN offset */
  uint16  total_eng;         /* total filtered energy of this pilot */
  uint16  ofs_cond_eng;      /* filtered energy for OFS condition */

  /* field 1 */
  PACK(struct)
  {
    uint16 asp_index : 4;     /* ASP index for the ASET */
    uint16 rpc_index : 4;     /* RPC index for the ASET */
    uint16 drc_cover : 3;     /* DRC Cover  */
    uint16 reserved  : 5;     /* Reserved  */
  } index_info;

  /* field 2 */
  PACK(struct)
  {
    uint16  index                : 8;     /* Bit7:0 Mac Index       */
    uint16  droptimer_expired    : 1;     /* Bit8   Drop Timer Expired Flag  */
    uint16  droptimer_active     : 1;     /* Bit9   Drop Timer Active Flag  */
  } mac_info;

  /* field 3 */
  uint16   win_center_x2;

} log_hdr_pilot_sets_aset_type;

typedef PACK(struct)
{
  uint16  pilot_pn;                  /* pilot PN offset */
  uint16  total_eng;                 /* total filtered energy of this pilot */
  uint16  ofs_cond_eng;              /* filtered energy for OFS condition */

  /* field 1 */
  PACK(struct)
  {
      uint16 chan_no     :   11;     /* CSET channel number      */
      uint16 band_class  :    5;     /* CSET band class          */
  } cset_freq;                       /* CDMA Frequency           */

  /* field 2 */
  PACK(struct)
  {
    uint16  reserved             : 8;     /* Bit7:0 Reserved       */
    uint16  droptimer_expired    : 1;     /* Bit8   Drop Timer Expired Flag  */
    uint16  droptimer_active     : 1;     /* Bit9   Drop Timer Active Flag  */
  } cset_info;

  /* field 3 */
  uint16  win_center_x2;

} log_hdr_pilot_sets_cset_type;

typedef PACK(struct)
{
  uint16  pilot_pn;          /* pilot PN offset */
  uint16  total_eng;         /* total filtered energy of this pilot */
  uint16  ofs_cond_eng;      /* filtered energy for OFS condition */

  /* field 1 */
  PACK(struct)
  {
    uint16 chan_no     :   11;       /* NSET channel number      */
    uint16 band_class  :    5;       /* NSET band class          */
  } nset_freq;                       /* CDMA Frequency           */

  /* field 2 */
  uint16  win;               /* NSET window can be upto 512 chips */

  /* field 3 */
  PACK(struct)
  {
    uint16  offset     :   3;            /* Bit2:0  Neighbor search window offset */
    uint16  reserved   :   5;            /* Bit7:3  Reserved        */
    uint16  age        :   8;            /* Bit15:8 Age             */
  }win_info;

} log_hdr_pilot_sets_nset_type;

typedef PACK(union)
{
  log_hdr_pilot_sets_aset_type aset;
  log_hdr_pilot_sets_cset_type cset;
  log_hdr_pilot_sets_nset_type nset;
} log_hdr_pilot_sets_union_type;

LOG_RECORD_DEFINE(LOG_HDR_PILOT_SETS_C)

  uint8   version;                /* The version number of the log packet */
  uint8   pn_inc;                 /* PN increment in chipx64 units */
  uint8   aset_cnt;               /* total number of pilots */
  uint8   aset_win;               /* ASET window size in chipx1 */

  PACK(struct)
  {
    uint16   chan_no    : 11;       /* Bit10 :0  :: ASET Channel Number */
    uint16   band_class :  5;       /* Bit15:11  :: Band Class */
  } aset_channel;

  uint8   srch_state;             /* Denotes the state of the HDR searcher task */
  uint8   cset_cnt;               /* total number of pilots */
  uint8   cset_win;               /* CSET window size in chipx1 */
  uint8   nset_cnt;               /* total number of pilots */
  uint16  rset_win;               /* rset search window */

  log_hdr_pilot_sets_union_type set_info[1];
    /* one entry of the pilot set */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_PILOT_SETS_V2_C
---------------------------------------------------------------------------*/

typedef PACK(struct)
{
  uint16  pilot_pn;          /* pilot PN offset */
  uint16  total_eng;         /* total filtered energy of this pilot */

  /* field 1 */
  PACK(struct)
  {
    uint16 asp_index : 4;     /* ASP index for the ASET */
    uint16 rpc_index : 4;     /* RPC index for the ASET */
    uint16 drc_cover : 3;     /* DRC Cover  */
    uint16 reserved  : 5;     /* Reserved  */
  } index_info;

  /* field 2 */
  PACK(struct)
  {
    uint16  index                : 8;     /* Bit7:0 Mac Index       */
    uint16  droptimer_expired    : 1;     /* Bit8   Drop Timer Expired Flag  */
    uint16  droptimer_active     : 1;     /* Bit9   Drop Timer Active Flag  */
  } mac_info;

  /* field 3 */
  uint16   win_center_x2;

} log_hdr_pilot_sets_v2_aset_type;

typedef PACK(struct)
{
  uint16  pilot_pn;                  /* pilot PN offset */
  uint16  total_eng;                 /* total filtered energy of this pilot */

  /* field 1 */
  PACK(struct)
  {
      uint16 chan_no     :   11;     /* CSET channel number      */
      uint16 band_class  :    5;     /* CSET band class          */
  } cset_freq;                       /* CDMA Frequency           */

  /* field 2 */
  PACK(struct)
  {
    uint16  reserved             : 8;     /* Bit7:0 Reserved       */
    uint16  droptimer_expired    : 1;     /* Bit8   Drop Timer Expired Flag  */
    uint16  droptimer_active     : 1;     /* Bit9   Drop Timer Active Flag  */
  } cset_info;

  /* field 3 */
  uint16  win_center_x2;

} log_hdr_pilot_sets_v2_cset_type;

typedef PACK(struct)
{
  uint16  pilot_pn;          /* pilot PN offset */
  uint16  total_eng;         /* total filtered energy of this pilot */

  /* field 1 */
  PACK(struct)
  {
    uint16 chan_no     :   11;       /* NSET channel number      */
    uint16 band_class  :    5;       /* NSET band class          */
  } nset_freq;                       /* CDMA Frequency           */

  /* field 2 */
  uint16  win;               /* NSET window can be upto 512 chips */

  /* field 3 */
  PACK(struct)
  {
    uint16  offset     :   3;            /* Bit2:0  Neighbor search window offset */
    uint16  reserved   :   5;            /* Bit7:3  Reserved        */
    uint16  age        :   8;            /* Bit15:8 Age             */
  }win_info;

} log_hdr_pilot_sets_v2_nset_type;

typedef PACK(union)
{
  log_hdr_pilot_sets_v2_aset_type aset;
  log_hdr_pilot_sets_v2_cset_type cset;
  log_hdr_pilot_sets_v2_nset_type nset;
} log_hdr_pilot_sets_v2_union_type;

LOG_RECORD_DEFINE(LOG_HDR_PILOT_SETS_V2_C)

  uint8   pn_inc;                 /* PN increment in chipx64 units */
  uint8   aset_cnt;               /* total number of pilots */
  uint8   aset_win;               /* ASET window size in chipx1 */

  PACK(struct)
  {
    uint16   chan_no    : 11;       /* Bit10 :0  :: ASET Channel Number */
    uint16   band_class :  5;       /* Bit15:11  :: Band Class */
  } aset_channel;

  uint8   srch_state;             /* Denotes the state of the HDR searcher task */
  uint8   cset_cnt;               /* total number of pilots */
  uint8   cset_win;               /* CSET window size in chipx1 */
  uint8   nset_cnt;               /* total number of pilots */
  uint16  rset_win;               /* rset search window */

  log_hdr_pilot_sets_v2_union_type set_info[1];
    /* one entry of the pilot set */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_TCA_INFO_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_TCA_INFO_C)
  uint8   sho_cell_cnt;
  uint8   drc_len;
  int8    drc_gain;
  int8    ack_gain;
  uint8   frame_offset;
LOG_RECORD_END

/*---------------------------------------------------------------------------
                     LOG_HDR_SIG_CC_BCAST_MSG_C
---------------------------------------------------------------------------*/

typedef LOG_HDR_SIG_AC_MSG_C_type LOG_HDR_SIG_CC_BCAST_MSG_C_type;

/*---------------------------------------------------------------------------
                          LOG_HDR_STATE_INFO_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_STATE_INFO_C)

  uint8    at_state;
  uint8    session_state;
  uint8    almp_state;
  uint8    init_state;
  uint8    idle_state;
  uint8    conn_state;
  uint8    rup_state;
  uint8    ovhd_state;
  boolean  hybrid_mode;   /* hybrid mode flag TRUE=ON, FALSE=OFF */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_FWD_RATE_STAT_C (Obsolete)
                          Merged in FWD_STATS_SUMMARY
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                          LOG_HDR_SECTOR_INFO_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_SECTOR_INFO_C)

  uint8   sector_id[16];
  uint8   subnet_mask;
  uint8   color_code;
  uint16  pn_offset;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_FWD_STATS_SUMMARY_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_FWD_STATS_SUMMARY_C)

  PACK(struct)
  {
    uint32 cc38400_good;
    uint32 cc38400_bad;

    uint32 cc76800_good;
    uint32 cc76800_bad;

    uint32 tc38400_good;
    uint32 tc38400_bad;

    uint32 tc76800_good;
    uint32 tc76800_bad;

    uint32 tc153600_good;
    uint32 tc153600_bad;

    uint32 tc307200_short_good;
    uint32 tc307200_short_bad;

    uint32 tc307200_long_good;
    uint32 tc307200_long_bad;

    uint32 tc614400_short_good;
    uint32 tc614400_short_bad;

    uint32 tc614400_long_good;
    uint32 tc614400_long_bad;

    uint32 tc921600_good;
    uint32 tc921600_bad;

    uint32 tc1228800_short_good;
    uint32 tc1228800_short_bad;

    uint32 tc1228800_long_good;
    uint32 tc1228800_long_bad;

    uint32 tc1843200_good;
    uint32 tc1843200_bad;

    uint32 tc2457600_good;
    uint32 tc2457600_bad;

  } rate_stats;

  PACK(struct)
  {
    uint32    rate38400[16];
    uint32    rate76800[8];
    uint32    rate153600[4];
    uint32    rate307200short[2];
    uint32    rate307200long[4];
    uint32    rate614400short;
    uint32    rate614400long[2];
    uint32    rate921600[2];
    uint32    rate1228800short;
    uint32    rate1228800long[2];
    uint32    rate1843200;
    uint32    rate2457600;
  } tc_rate_slt_cnt;

  PACK(struct)
  {
    uint32    rate38400[16];
    uint32    rate76800[8];
  } cc_rate_slt_cnt;

  uint32 seq_num;

LOG_RECORD_END /* LOG_HDR_FWD_STATS_SUMMARY_C */

/*---------------------------------------------------------------------------
                           LOG_HDR_AC_MAC_CAPSULE
---------------------------------------------------------------------------*/
typedef uint8 hdrlog_ac_mac_capsule_type;

LOG_RECORD_DEFINE(LOG_HDR_AC_MAC_CAPSULE_C)
  uint16 pkt_length;

  /* variable length - pkt_length instances of this field */
  hdrlog_ac_mac_capsule_type  data[1];

LOG_RECORD_END /* LOG_HDR_AC_MAC_CAPSULE_C */

/*---------------------------------------------------------------------------
                           LOG_HDR_CC_MAC_PACKET
---------------------------------------------------------------------------*/
typedef uint8 hdrlog_cc_mac_packet_type;

LOG_RECORD_DEFINE(LOG_HDR_CC_MAC_PACKET_C)
  uint16 pkt_length;

  /* variable length - pkt_length instance of this field */
  hdrlog_cc_mac_packet_type  data[1];

LOG_RECORD_END /* LOG_HDR_CC_MAC_PACKET_C */

/*---------------------------------------------------------------------------
                           LOG_HDR_RCV_ATI_LIST_C
---------------------------------------------------------------------------*/
typedef uint32 hdrlog_rcv_ati_list_type;

LOG_RECORD_DEFINE(LOG_HDR_RCV_ATI_LIST_C)

  /* number of UATI records to log */
  uint8 num_records;

  /* Variable lgth packet - there are 'num_records' instances of this field */
  hdrlog_rcv_ati_list_type  uati_value[1];

LOG_RECORD_END /* LOG_HDR_RCV_ATI_LIST_C */

/*---------------------------------------------------------------------------
                           LOG_HDR_ACQ_INFO_C
---------------------------------------------------------------------------*/

/* This packet is logged at the following HDRSRCH state transitions:
  ACQ             ->  INACTIVE
  SYNC            ->  INACTIVE
  SYNC            ->  IDLE                                                  */

typedef enum
{
  HDRLOG_ACQ_SUCCESS,             /* ACQ and SYNC success               */
  HDRLOG_ACQ_MICRO_FAILURE,       /* ACQ micro srch failure             */
  HDRLOG_ACQ_PILOT_FAILURE,       /* ACQ pilot predetect/detect failure */
  HDRLOG_ACQ_FLL_FAILURE,         /* ACQ FLL lock failure               */
  HDRLOG_ACQ_SLAM_FAILURE,        /* ACQ slam failure                   */
  HDRLOG_ACQ_SYNC_FAILURE,        /* ACQ Sync failure                   */

    /* The above 6 enums will be reported to DIAG as acquisition results. 
       The enums below are for internal usage. They are used as an indicator
       about at what stage that log info is to be recorded              */

  HDRLOG_ACQ_SLAM_SUCCESS,        /* Slamming successful, entering sync */
  HDRLOG_ACQ_NULL                /* Initial NULL state                 */

} log_hdr_acq_result_enum_type;

typedef PACK(struct)
{
  PACK(struct)
  {
    uint16   chan_no    : 11;       /* Bit10 :0  :: Channel Number  */
    uint16   band_class :  5;       /* Bit15:11  :: Band Class      */
  } channel;

  uint8     result;                 /* 0: Acquisition success
                                       1: Micro srch failure
                                       2: Pilot predetect/detect failure
                                       3: FLL failure
                                       4: Slam failure
                                       5: Sync failure
                                       Use log_hdr_acq_result_enum_type */

  PACK(struct)
  {
    uint8   mode        : 4;        /* Search mode                  */
    uint8   stage       : 4;        /* Search stage                 */
  } srch_cmd;

  uint16    total_time_no_1x_tuneaway_ms;  
    /* Excluding time during 1x tuneaway, in millisecond */
  uint16    total_time_1x_tuneaway_ms;     
    /* Including time during 1x tuneaway, in millisecond */
  uint16    pilot_acq_time_ms;             
    /* Excluding time during 1x tuneaway, in millisecond */
  uint16    fll_lock_time_ms;              
    /* FLL lock acquisition time, in millisecond         */
  uint16    slam_time_ms;                  
    /* Slam time, in millisecond                         */
  uint8     rf_warmup_tune_time_ms;        
    /* RF warmup and tune time, in millisecond           */
  uint8     coset;
    /* Coset for successful acquisition,                 */

  uint8     system_time[HDR_BYTES_IN_SYSTEM_TIME];             
    /* RAW system time from SYNC message */
  uint16    pn_offset;
    /* PN from SYNC message               */

  uint16    fing_rssi_sync;         /* Finger RSSI in sync state          */
  int16     agc0_sync;              /* AGC of Rx Chain 0 in Sync state    */
  int16     agc1_sync;              /* AGC of Rx Chain 1 in Sync state    */
  
  uint16    attempt_count;          /* Cumulative attempt count from reset*/
  uint16    attempt_count_success;  /* Successful acq attempt count       */
  uint16    attempt_count_failure;  /* Failed acq attempt count           */
  
  uint32    total_time_ms;
    /* Cumulative acq time from reset, excluding 1x tune-away and sync,
       in millisecond                                                     */
  uint32    total_failure_time_ms;
    /* Cumulative acq failure time from reset, excluding 1x tune-away,
       in millisecond                                                     */
}
hdrsrchlog_acq_info_struct_subtype;
  /* This structure is the payload of ACQ INFO log packet   */

LOG_RECORD_DEFINE(LOG_HDR_ACQ_INFO_C)

  hdrsrchlog_acq_info_struct_subtype  acq_info;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_REACQ_INFO_C
---------------------------------------------------------------------------*/

/* This packet is logged at the following HDRSRCH state transitions:
  IDLE REACQ      ->  INACTIVE
  IDLE REACQ      ->  IDLE                                          */

typedef enum
{
  HDRLOG_REACQ_OLD_PILOT,         /* REACQ on old pilot             */
  HDRLOG_REACQ_RAHO,              /* RAHO                           */
  HDRLOG_REACQ_LOST,              /* REACQ lost after sleep         */
  HDRLOG_REACQ_LOST_DSP_NOT_READY,  /* REACQ lost, no dsp resource    */
  HDRLOG_REACQ_NULL              /* Initial NULL state                 */
} log_hdr_reacq_result_enum_type;

 typedef PACK(struct)
{
  uint32    sleep_cycle_count;      /* Incremental count from reset       */

  uint16    sleep_duration_ms;      
    /* sleep time before this wakeup, in millisecond      */

  uint8     rf_warmup_tune_time_ms; 
    /* RF warmup and tune time, in millisecond            */
  
  uint8     tune_to_scc_start_hs;       
    /* Time from tune-done to SCC start, in half-slot unit                */

  uint8     srch_count;           
    /* Number of searches before success or declaring system loss         */

  int8      slew_x2;              /* Slew of the pilot                    */

  uint8     result;               /* 0: Re-acq on old pilot
                                     1: RAHO
                                     2: lost acq after sleep            
                                     Use log_hdr_reacq_result_enum_type */

  uint8     last_srch_state;      /* Srch state before waking up (3a/3e)  */
  
  uint16    pilot_energy;         
    /* Raw pilot energy. Will be the new pilot energy after RAHO        */
  uint16    pilot_pn;
    /* Pilot PN offset. Will be the new pilot pn offset after RAHO      */
  uint16    old_pn_energy;        
    /* Old pilot energy in case of RAHO             */
  uint16    old_pn;               
    /* Old pilot PN offset in case of no RAHO       */

  uint16    failure_count;        /* Cumulative count of failed reacq     */

}
hdrsrchlog_reacq_info_struct_subtype;
  /* This structure is the payload of REACQ INFO log packet   */

LOG_RECORD_DEFINE(LOG_HDR_REACQ_INFO_C)

  hdrsrchlog_reacq_info_struct_subtype  reacq_info;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_SLEEP_INFO_C
---------------------------------------------------------------------------*/

/* This packet is logged at the following HDRSRCH state transitions:
  IDLE            ->  SLEEP
  IDLE OFS        ->  SLEEP      
  IDLE            ->  INACTIVE
  IDLE            ->  TRAFFIC
  IDLE monitor    ->  IDLE access                                       */

typedef enum
{
  HDRLOG_CC_GOOD_CRC,              /* CRC pass                          */
  HDRLOG_CC_PREAMBLE_LOST,         /* Preamble detection failed         */
  HDRLOG_CC_BAD_CRC,               /* CRC fails                         */
  HDRLOG_CC_FALSE_CRC,             /* False CRC pass                    */
  HDRLOG_CC_REACQ_FAIL,            /* Reacq failed                      */
  HDRLOG_CC_SUBSYNC_PREAMBLE_LOST, /* Subsync preamble detection failed */
  HDRLOG_CC_NULL                   /* Initial NULL state                */
}
hdrlog_cc_status_enum_type;

typedef enum
{
  HDRLOG_OFS_NO_OFS,      /* No Off-frequency search                  */
  HDRLOG_OFS_NO_HO,       /* Off-frequency search, but no hand-off    */
  HDRLOG_OFS_HO,          /* Off-frequency search and hand-off        */
  HDRLOG_OFS_NULL         /* Initial NULL state                       */
}
hdrlog_ofs_status_enum_type;

typedef PACK(struct)
{
  uint32    sleep_cycle_count;    /* Incremental count from reset       */

  uint16    awake_time_ms;           
    /* Duration from wakeup ISR, till packet is logged, in millisecond  */

  uint16    scc_start_to_ccdec_hs;
    /* Time from start of SCC, till decoder ISR receives the CC packet, 
       in half slot. Assume that SCC starts at HSTR 0                   */

  uint16    ccdec_to_sleep_hs;
    /* Time from last decoded CC packet to entering sleep, in half slot,
       only valid when the "next_srch_state" is SLEEP state             */

  uint16    dead_time_hs;            
    /* Time from fing assignment to SCC start, in half slot             */
  
  uint8     slot_count;          /* Number of slots to decode CC        */
  uint8     cc_rate;             /* Contrl channel rate                 */
  uint8     cc_status;           /* 0: Good CRC
                                    1: Missed CC preamble
                                    2: Bad CRC
                                    3: False CRC pass                   */

  uint8     next_srch_state;      /* Next srch state                    */

  uint16    pilot_pn;             /* ASET PN before entering sleep      */
  uint16    pilot_energy;         /* ASET pilot energy before sleep     */

  uint8     handoff_count;        /* Number of handoffs during wakeup   */
  
  uint8     ofs_status;           /* 0: no OFS
                                     1: OFS, no handoff
                                     2: Off freq handoff                */
  PACK(struct)
  {
    uint16   chan_no    : 11;     /* Bit10 :0  :: Channel Number */
    uint16   band       :  5;     /* Bit15:11  :: Band Class */
  } ofs_channel;

  uint16    ofs_pn;               /* OFS PN offset                      */
  uint16    ofs_pn_energy;        /* OFS PN energy                      */

  uint8     hdr_sci;              /* HDR slot cycle index               */

  uint16    cc_mer_count;         /* Cumulative CC MER count from reset */

  uint16    reacq_loss_count;     
    /* Cumulative count of acq losses after sleep from reset            */

  uint16    cmac_expiry_count;    
    /* Cumulative count of CMAC timer expiry from reset                 */

  uint32    iho_count;            
    /* Cumulative count of idle hand-off from reset                     */

  uint32    ofs_count;            
    /* Cumulative count of off freq searches from reset                 */
  
  uint16    ofs_handoff_count;    
    /* Cumulative count of OFS hand-off from reset                      */
  
  uint8     chan_hash_count;      
    /* Cumulative count of idle channl hashing from reset               */
  
}
hdrsrchlog_sleep_info_struct_subtype;
  /* This structure is the payload of SLEEP INFO log packet   */

LOG_RECORD_DEFINE(LOG_HDR_SLEEP_INFO_C)

  hdrsrchlog_sleep_info_struct_subtype  sleep_info;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_ENH_SLEEP_INFO_C
---------------------------------------------------------------------------*/
#define HDR_LOG_ENH_SLEEP_INFO_VER          3
  /* Version number of the Enh Sleep Info log packet. */

typedef PACK(struct)
{
  uint8     version;              
    /* The version number of the packet.  */

  uint32    sleep_cycle_count;    /* Incremental count from reset       */

  uint16    awake_time_ms;           
    /* Duration from wakeup ISR, till packet is logged, in millisecond  */

  uint16    cc_start_to_ccdec_hs;
    /* Time from start of CC, till decoder ISR receives the CC packet, 
       in half slot.                                                    */

  uint16    ccdec_to_sleep_hs;
    /* Time from last decoded CC packet to entering sleep, in half slot,
       only valid when the "next_srch_state" is SLEEP state             */

  int16     dead_time_hs;            
    /* Time from fing assignment to SCC start, in half slot. Can be 
       positive or negative.                                            */
  
  uint8     slot_count;          /* Number of slots to decode CC        */
  uint8     cc_rate;             /* Contrl channel rate                 */
  uint8     cc_status;           /* 0: Good CRC
                                    1: Missed CC preamble
                                    2: Bad CRC
                                    3: False CRC pass                   */

  uint8     next_srch_state;      /* Next srch state                    */

  uint16    pilot_pn;             /* ASET PN before entering sleep      */
  uint16    pilot_energy;         /* ASET pilot energy before sleep     */

  uint8     handoff_count;        /* Number of handoffs during wakeup   */
  
  uint8     ofs_status;           /* 0: no OFS
                                     1: OFS, no handoff
                                     2: Off freq handoff                */
  PACK(struct)
  {
    uint16   chan_no    : 11;     /* Bit10 :0  :: Channel Number */
    uint16   band       :  5;     /* Bit15:11  :: Band Class */
  } ofs_channel;

  uint16    ofs_pn;               /* OFS PN offset                      */
  uint16    ofs_pn_energy;        /* OFS PN energy                      */

  uint8     hdr_sci;              /* HDR slot cycle index               */

  uint32    cc_mer_count;         /* Cumulative CC MER count from reset */

  uint32    reacq_loss_count;     
    /* Cumulative count of reacq failures after sleep from reset        */

  uint16    cmac_expiry_count;    
    /* Cumulative count of CMAC timer expiry from reset                 */

  uint32    iho_count;            
    /* Cumulative count of idle hand-off from reset                     */

  uint32    ofs_count;            
    /* Cumulative count of off freq searches from reset                 */
  
  uint16    ofs_handoff_count;    
    /* Cumulative count of OFS hand-off from reset                      */
  
  uint8     chan_hash_count;      
    /* Cumulative count of idle channl hashing from reset               */
  
  uint32    cc_mer_denom;         
    /* Cumulative CC MER denom from reset */

  uint32    cur_warmup_time_us;
    /* Current determined warmup time based on WUTA. */

  boolean   force_preamble_used;
    /* Decode attempt due to force preamble detection. */

  uint32    force_preamble_count;
    /* Number of times force preamble detection caused a decode to be 
       attempted. */

  uint32    force_preamble_failed_count;
    /* Number of times force preamble detection caused a decode to be attempted
       without success. */

  uint32    total_time_out_of_service_ms;
    /* Total time out of service in ms. */

  uint32    est_out_of_service_ccmer;
    /* An estimate of what the CC MER is including time spent out of service 
       due to system loss. Add this value to the cc_mer_count and the 
       cc_mer_denom to get the adjusted ccmer result. */

  uint32    intra_scc_fpd_cnt;
    /* The cumulative number of times force preamble detection caused a decode
       to be attempted during intra-SCC wakeups. */

  uint32    intra_scc_fpd_failure_cnt;
    /* The cumulative number of times force preamble detection caused a decode
       to be attempted without success during intra-SCC wakeups. */

  uint32    intra_scc_reacq_failure_cnt;
    /* Cumulative reacquisition failure count from reset for intra SCC 
       wakeups. */

  uint8     ccmac_pkt_number;
    /* The number of the first packet in the Control Channel Capsule 
       decoded in this wakeup.  If the wakeup is for the first packet of the
       SCC it is 1.  If it is the second packet of the SCC it is 2, and so on.
       If it is the subsync packet, it should always be 1. If the wakeup was
       for the quick paging capsule it is a 0. */

  uint8     ccmac_pkt_within_wakeup_cnt;
    /* The number of CCMAC packets decoded in this wakeup (for when intra SCC
       sleep was not performed). */

  uint32    first_cc_failure_cnt;
    /* The cumulative number of first packets which were not decoded 
       successfully. */

  uint32    cont_cc_from_sleep_cnt;
    /* The cumulative number of continuation CCMAC packets in which a decode
       was attempted upon waking up from intra SCC sleep. */

  uint32    cont_cc_from_awake_cnt;
    /* The cumulative number of continuation CCMAC packets in which a decode
       was attempted when the AT was awake from a previous wakeup (for when
       intra SCC sleep was not performed). */

  uint32    cont_cc_from_sleep_failure_cnt;
    /* The cumulative number of continuation CCMAC packets which were not 
       decoded successfully upon waking up from sleep. */

  uint32    cont_cc_from_awake_failure_cnt;
    /* The cumulative number of continuation CCMAC packets which were not
       decoded successfully when the AT was awake from a previous wakeup
       (for when intra SCC sleep was not performed). */

  uint32    subsync_mer_cnt;         
    /* Cumulative subsynchronous packet CC mer demod failures from reset.
       This number excludes missed preamble cases. */

  uint32    subsync_mer_denom;         
    /* Cumulative sub synchronous packet CC MER denom from reset. */

  uint8     num_reacq_pilots_srched;
    /* Number of pilots searched in reacq; Note: it's for 1st reacq srch only */

  uint16    num_conflict_within_time_period;
    /* Number of times HDR wakes up within certain amount of time of 1x waking up */

  uint16    num_conflict_adjustment_triggered;
    /* Number of instances where HDR conflict adjustment is triggered */

  int32     slack_us;
    /* Slack time from RF warmup */

  uint32    actual_rf_warmup_us;
    /* actual RF warm up time, calculated from start of RF warmup to 
       when slack time starts (done with RF warmup) */

  boolean   rxdiv_enable;
    /* Whether or not RX diversity chain is enabled */

  uint8     rxdiv_enable_reason;
    /* Reason why RX diversity chain is enabled
        Bit 0: syncc_cc_missed
        Bit 1: prev_psinr_avg is less than threshold
        Bit 2: prev_sleep_time is higher than threshold
        Bit 3: reacq_failed 
    */
    

}
hdrsrchlog_enh_sleep_info_struct_subtype;
  /* This structure is the payload of SLEEP INFO log packet   */

LOG_RECORD_DEFINE(LOG_HDR_ENH_SLEEP_INFO_C)

  hdrsrchlog_enh_sleep_info_struct_subtype  sleep_info;
    /* New rev A sleep info information. */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                        LOG_HDR_MCS_PHY_CHAN_METRICS
---------------------------------------------------------------------------*/
/*
  Broadcast Physical Channel metrics.
  Provides CRC pass/fail information about broadcast packets on a per 
  interlace-multiplex basis. 
  There is no notion about which flow these broadcast packets belong to
  in this log packet.
*/

typedef PACK(struct)
{

   /* Number of Broadcast MAC packets received in this interlace-multiplex 
   pair  with Good CRC.
   */
   uint32 mac_crc_pass;
   
   /* Number of Broadcast MAC packets received in this interlace-multiplex 
   pair  with BAD CRC.
   */
   uint32 mac_crc_fail;

} hdrlogbc_multiplex_metrics_type;

LOG_RECORD_DEFINE(LOG_HDR_MCS_PHY_CHAN_METRICS_C)

   /* Number of multiplexes configured (4,8, or 16) */
   uint8 multiplex_count;   

   /* Bitmask to indicate which Interlace-Multiplex pairs are currenly
      being monitored.  (There are muliplex_count * 4 valid bits, which are
      right-justified in this field.
   */
   uint8 monitored_im_pairs[8];

   /* Interlace_multiplex metrics ( total of multiplex_count * 4 records )
   */
   hdrlogbc_multiplex_metrics_type mux_metrics[1];

LOG_RECORD_END



/*---------------------------------------------------------------------------
                        LOG_HDR_MCS_LOG_CHAN_METRICS
---------------------------------------------------------------------------*/

/*
  Provides information about broadcast packets on a per logical index basis. 
  Information logged includes MAC Layer (including ECB) statistics,
  Broadcast Framing Layer statistics, etc.
*/

typedef PACK(struct)
{
   /* Number of packets received at the MAC layer with good CRC*/
   uint32 packets_crc_passed;
   
   /* Number of packets received at the MAC layer with bad CRC */
   uint32 packets_crc_failed;
   
   /* Number of packets missing at the MAC layer */
   uint32 packets_missed;

   /* Number of ECB's that have no errors, i.e, no bad CRC 
   MAC packets or no missing packets */
   uint32 ecb_no_erasures;

   /* Number of ECB's that had errors, but were corrected
   using RS Decoding */
   uint32 ecb_decode_pass;
   
   /* Number of ECB's that had too many errors to attempt
      RS decoding */
   uint32 ecb_decode_abort;

   /* Number of MAC packets delivered after RS decoding 
   */
   uint32 packets_delivered;  
   
   /* Total number of IS-856 Security Layer packets received.
   Note this is NOT the broadcast security protocol packets*/
   uint32 security_layer_packets;

   uint32 reserved1;
   uint32 reserved2;

} hdrlogbc_mac_metrics_type;


/*
This structure provides statistics about packets processed at
the Broadcast Security Layer Protocol 
*/
typedef PACK(struct)
{
   /* Number of Broadcast Security Protocol packets received*/
   uint32 packet_count;
   
   /* Total Number of Broadcast Security Protocol bytes received*/
   uint32 byte_count;

   uint32 reserved1;
   uint32 reserved2;
   
} hdrlogbc_sec_metrics_type;


/*
This structure provides statistics about packets processed at
the Broadcast Framing Layer Protocol 
*/
typedef PACK(struct)
{
   /* Number of Framing Layer packets ( fragments) received */
   uint32 packet_count;

   /* Total Number of Framing Layer (fragment) bytes received */
   uint32 byte_count;
   
   /* Number of Deframed Higher Layer packets delivered to upper layer */
   uint32 packets_delivered;
   
   /* Number of Deframed Higher Layer packets dropped (ie, bad CRC ) */
   uint32 packets_dropped;

   /* Number of bytes delivered */
   uint32 bytes_delivered;
   
   uint32 reserved1;
   uint32 reserved2;
   
} hdrlogbc_framing_metrics_type;

/* Contains all metrics for each logical channel. 
*/
typedef PACK(struct)
{
  /* Logical channel id (arbitrarily chosen by AT) */
  uint8  chan_id;

  /* Flow ID associated with this logical channel. Note that there
     may be other flows associated with this channel also.*/
  uint32 flow_id;

  /* HDR MCS MAC statistics */
  hdrlogbc_mac_metrics_type     mac;

  /* HDR MCS Security statistics */
  hdrlogbc_sec_metrics_type     sec;

  /* HDR MCS Framing statistics */
  hdrlogbc_framing_metrics_type framing;

} hdrlogbc_log_chan_metrics_type;


LOG_RECORD_DEFINE(LOG_HDR_MCS_LOG_CHAN_METRICS_C)

  /* total number of logical channel records in this packet */
  uint8  logical_chan_count;

  hdrlogbc_log_chan_metrics_type metrics[1]; 
LOG_RECORD_END





/*---------------------------------------------------------------------------
                          LOG_HDR_MCS_PKT_RATE_METRICS
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_HDR_MCS_PKT_RATE_METRICS_C)

  /* Good/Bad CRC statistics for all control channel and broadcast 
     channel rates */
     
  PACK(struct)
  {
    uint32 cc38400_good;
    uint32 cc38400_bad;

    uint32 cc76800_good;
    uint32 cc76800_bad;
  } cc_rate_stats;

  PACK(struct)
  {

    uint32 bc38400_good;
    uint32 bc38400_bad;

    uint32 bc76800_good;
    uint32 bc76800_bad;

    uint32 bc153600_good;
    uint32 bc153600_bad;

    uint32 bc204800_good;
    uint32 bc204800_bad;

    uint32 bc307200_short_good;
    uint32 bc307200_short_bad;

    uint32 bc307200_long_good;
    uint32 bc307200_long_bad;

    uint32 bc409600_good;
    uint32 bc409600_bad;

    uint32 bc614400_short_good;
    uint32 bc614400_short_bad;

    uint32 bc614400_long_good;
    uint32 bc614400_long_bad;

    uint32 bc921600_good;
    uint32 bc921600_bad;

    uint32 bc1228800_short_good;
    uint32 bc1228800_short_bad;

    uint32 bc1228800_long_good;
    uint32 bc1228800_long_bad;

    uint32 bc1843200_good;
    uint32 bc1843200_bad;

    uint32 bc2457600_good;
    uint32 bc2457600_bad;

  } mcs_rate_stats;

  /* Statistics on the number of slots to decode each 
     broadcast rate packet received */
  PACK(struct)
  {
    uint32    rate38400[16];
    uint32    rate76800[8];
    uint32    rate153600[4];
    uint32    rate204800[3];
    uint32    rate307200short[2];
    uint32    rate307200long[4];
    uint32    rate409600[3];
    uint32    rate614400short;
    uint32    rate614400long[2];
    uint32    rate921600[2];
    uint32    rate1228800short;
    uint32    rate1228800long[2];
    uint32    rate1843200;
    uint32    rate2457600;
  } mcs_rate_slt_cnt;

LOG_RECORD_END /* _C */


/*---------------------------------------------------------------------------
                          LOG_HDR_MCS_ECB_STATUS
---------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(LOG_HDR_MCS_ECB_STATUS_C)

  /* Logical index this ECB is assigned */
  uint8    channel_id;

  /* Flow ID associated with this logical channel. Note that there
     may be other flows associated with this channel also.*/
  uint32   flow_id;

  /* Outer Code value (0-6) */
  uint8    outer_code;

  /* Column of ECB */
  uint8    columns_per_ecb;

  /* Whether this ECB contained a security layer packet
     (only valid when column is 0) */
  boolean  sec_pkt_included;

  uint32   reserved1;

  /* There can be a maximum of 32 of the following */
  PACK(struct)
  {
  
    /* Number of data erasures in this column */
    uint8    data_erasures;

    /* Total number of erasures (including parity) */
    uint8    total_erasures;
    
  } column[1];

LOG_RECORD_END /* LOG_HDR_MCS_ECB_STATUS_C */




/*---------------------------------------------------------------------------
                          LOG_HDR_DEBUG_DISPLAY_C
  ---------------------------------------------------------------------------*/

/* This packet is logged every 990ms                                       */
typedef PACK(struct)
{

  PACK(struct)
  {
    uint16 chan_num          : 11;  /* Bit10:0  Channel Number            */
    uint16 band_class        : 5 ;  /* Bit15:11 Band Class                */

  } hdr_freq;

  int8 rx_agc0;                     /* Receive signal strength chain0     */
  int8 rx_agc1;                     /* Receive signal strength chain1     */
  int8 tx_agc;                      /* Connected state Tx signal strength */


  uint16 serving_pn;                /* PN Offset of serving ASET pilot    */
  uint16 aset_pn[6];                /* ASET pilot PN offsets              */
  uint8  sleep_mode;                /* HDR Sleep mode
                                       Possible values- 0 : SCC, 1 : CCC,
                                       2 : Long, 3 : Until Reacq. handoff,
                                       4 : Rel A Idle                     */

  PACK(struct)
  {
    uint8 rel0_sci           : 3;   /* Bit2:0 Release 0 Slot Cycle Index
                                       Possible values:
                                       0 : 5.12s,  1 : 10.24s,
                                       2 : 20.48s, 3 : 40.96s,
                                       4 : Rel. A SCI                     */
    uint8 relA_sci           : 5;   /* Bit7:3 Release A Slot Cycle Index  */

  } sci;

  uint8 srch_state;                 /* HDR Searcher state                 */
  int8 rx_div;                      /* HDR Receive Diversity    
                                         0 : Rx0 only, 1 : Rx1 only       */

  uint8 prot_state;                 /* HDR Protocol state                 
                                        0 : Inactive, 1 : Acquistion        
                                        2 : Sync,     3 : Idle,
                                        4 : Access,   5 : Connected       */

  uint8 hdr_session_state;          /* HDR session state                  
                                       0 : Inactive, 1 :AMP Setup State
                                       2 : AT initiated, 3 : AN initiated
                                       4 : Open state                     */

  PACK(struct)
  {
    uint32 uati24            : 24;  /* UATI                               */
    uint32 color_code        : 8;   /* Color code for subnet              */
  } uati_info;

  uint16  stream_config[HDRLOG_NUM_STREAMS]; 
                                    /* Stream configuration Release 0     */ 

}
hdrlog_debug_display_logging_struct_subtype;

LOG_RECORD_DEFINE(LOG_HDR_DEBUG_DISPLAY_C)

  hdrlog_debug_display_logging_struct_subtype  debug_display;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_TUNE_AWAY_INFO_C
  ---------------------------------------------------------------------------*/

/* This struct describes the running total information for HDR tune aways. */
typedef PACK(struct)
{
  uint32 hdrtc_tune_away_count;
    /* Number of times the AT has tuned away from HDR. */

  uint32 hdrtc_tune_back_count;
    /* Number of times the AT has tuned back without terminating the 
     *  HDR connection. 
     */

  uint32 cum_hdrtc_suspend_time;
    /* Amount of time spent suspended from HDR in ms. */

  uint32 cum_hdrtc_time;
    /* Amount of time spent tuned to HDR in ms. */

  uint32 cum_hdrtc_tune_away_time;
    /* Amount of time spent away from HDR including time to tune back to 
       HDR in ms. */

} hdrsrchlog_tune_away_stats_struct;

/* This enum describes which unit is tuning the AT away from HDR. */
typedef enum
{
  HDRSRCHLOG_GPS_TUNEAWAY,
    /* This is a GPS Tuneaway from HDR. */

  HDRSRCHLOG_1X_TUNEAWAY,
    /* This is a 1x Tuneaway from HDR. */

  HDRSRCHLOG_UNKNOWN_TUNEAWAY,
    /* We don't know who caused us to tune away. */

} hdrsrchlog_tune_away_reason_type;

/* This struct describes the details of a particular tune away. */
typedef PACK(struct)
{
  time_type hdrtc_starttc_time_stamp;
    /* Timestamp when traffic started before the last tune away. */

  time_type hdrtc_tune_away_time_stamp;
    /* Timestamp when traffic ended so reciever could be used elsewhere. */

  time_type hdrtc_tune_back_time_stamp;
    /* Timestamp when HDR got the reciever back after a tune away. */

  time_type hdrtc_resume_time_stamp;
    /* Timestamp when HDR finished tuning the reciever to the HDR chan. */

  hdrsrchlog_tune_away_reason_type hdrtc_tune_away_reason;
    /* The reason why we tuned away from HDR (who used the reciever) */

  boolean performed_ofs;
    /* Whether we performed an HDR off freq. search. */

  time_type hdrtc_ofs_time_stamp;
    /* Timestamp when the OFS started if applicable. */

} hdrsrchlog_tune_away_instance_info_type;

/* This packet is logged for information about HDR tune aways. */
typedef PACK(struct) 
{
  uint8 version;
    /* The version number of the packet. */
  
  hdrsrchlog_tune_away_instance_info_type instance;
    /* Details on the last tune away. */
  
  hdrsrchlog_tune_away_stats_struct ta_stats_since_reset;
    /* Tune away information since last reset of phone or data. */

  hdrsrchlog_tune_away_stats_struct ta_stats_since_call;
    /* Tune away information since start of last call. */  

} hdrsrchlog_tune_away_info_struct_subtype;

LOG_RECORD_DEFINE(LOG_HDR_TUNE_AWAY_INFO_C)

  hdrsrchlog_tune_away_info_struct_subtype tune_away_info;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_TUNE_AWAY_RRI_INFO_C
  ---------------------------------------------------------------------------*/

/* This struct describes RRI information about a particular tune away. */
typedef PACK(struct)
{
  uint32 cum_hdrtc_tune_away_rri;
   /* Accumulated RRI values before the tune away */
 
  uint32 cum_hdrtc_tune_back_rri;
    /* Accumulated RRI values after the tune away */

  uint32 total_rri_count;
    /* Total number of RRIs in the accumulater. */

} hdrrmaclog_rri_stats_struct;

/* This packet is logged periodically to communicate RRI info regarding HDR
 *  tune aways. 
 */
typedef PACK(struct) 
{
  uint8 version;
    /* The version number of the packet. */
  
  uint8 hdrtc_tune_away_tx_rri;
    /* RRI value before the tune away */

  uint8 hdrtc_tune_back_tx_rri;
    /* RRI value after the tune away */

  hdrrmaclog_rri_stats_struct since_reset;
    /* RRI information since last reset of phone or data. */

  hdrrmaclog_rri_stats_struct since_call;
    /* RRI information since start of last call. */

} hdrrmaclog_tune_away_rri_info_struct_subtype;

LOG_RECORD_DEFINE(LOG_HDR_TUNE_AWAY_RRI_INFO_C)

  hdrrmaclog_tune_away_rri_info_struct_subtype rri_info;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_CONN_TERM_STATS_C
  ---------------------------------------------------------------------------*/

/* The different kind of connection terminations we are tracking. */
typedef enum
{
  HDRPLOG_FTCMAC_SUPERVISION_FAILURE_COUNT,
    /* Number of Forward Traffic Channel MAC supervision timeouts. */

  HDRPLOG_CCMAC_TRAFFIC_SUPERVISION_FAILURE_COUNT,
    /* Number of Control Channel MAC supervision timeouts during traffic. */

  HDRPLOG_CCMAC_TRAFFIC_SUSPENDED_SUPERVISION_FAILURE_COUNT,
    /* Number of Control Channel MAC supervision timeouts during suspended 
       mode. */

  HDRPLOG_CCMAC_IDLE_SUPERVISION_FAILURE_COUNT,
    /* Number of Control Channel MAC supervision timeouts during idle. */

  HDRPLOG_DRC_SUPERVISION_FAILURE_COUNT,
    /* Number of DRC supervision timeouts. */

  HDRPLOG_TCA_CONNECTION_SETUP_FAILURE_COUNT,
    /* Number of times we could not setup the traffic channel. */

  HDRPLOG_RTCACK_CONNECTION_SETUP_FAILURE_COUNT,
    /* Number of times the RTCACK was never received during connection 
       setup. */

  HDRPLOG_QC_TRAFFIC_SUPERVISION_FAILURE_COUNT,
    /* Number of QC supervision timeouts during traffic. */

  HDRPLOG_QC_IDLE_SUPERVISION_FAILURE_COUNT,
    /* Number of QC supervision timeouts during idle. */

  HDRPLOG_SP_TRAFFIC_SUPERVISION_FAILURE_COUNT,
    /* Number of SP supervision timeouts during traffic. */

  HDRPLOG_SP_IDLE_SUPERVISION_FAILURE_COUNT,
    /* Number of SP supervision timeouts during idle. */

  HDRPLOG_AP_TRAFFIC_SUPERVISION_FAILURE_COUNT,
    /* Number of AP supervision timeouts during traffic. */

  HDRPLOG_AP_IDLE_SUPERVISION_FAILURE_COUNT,
    /* Number of AP supervision timeouts during idle. */

  HDRPLOG_CONNECTION_CLOSE_RECEIVED_COUNT,
    /* Number of connection closes received from AN. */

  HDRPLOG_CONNECTION_CLOSE_SENT_COUNT,
    /* Number of connection closes sent by AT. */

  HDRPLOG_1X_HANDDOWN_COUNT,
    /* Number of times HDR was ended and the call was handed down to 1x. */

  HDRPLOG_1X_HANDDOWN_IGNORED_COUNT,
    /* Number of times asked to handdown to 1x but didn't since we didn't 
       have a data connection active. */

  HDRPLOG_NUM_CONN_TERM_STATS_COUNTS
    /* Number of connection termination statistic counters. */

} hdrplog_conn_term_stats_count_type;

/* This packet communicates information about the last call termination. */
typedef PACK(struct) 
{
  uint8 version;
    /* The version number of the packet. */
  
  time_type start_time_of_last_call;
    /* Timestamp of the start of the last call. */

  time_type end_time_of_last_call;
    /* Timestamp of the end of the last call. */

  uint8 last_call_release_reason;
    /* Protocol level call release reason. */

  uint32 traffic_time_since_reset;
    /* Total connected time since last reset including tune away time. */

  uint32 traffic_time_since_call;
    /* Total connected time for the last call including tune away time. */

  uint16 total_call_terms;
    /* Total number of call terminations. */

  uint16 since_reset_counts[HDRPLOG_NUM_CONN_TERM_STATS_COUNTS];
    /* Number of each type of call termination since the last reset. */

} hdrplog_conn_term_stats_struct_subtype;

LOG_RECORD_DEFINE(LOG_HDR_CONN_TERM_STATS_C)

  hdrplog_conn_term_stats_struct_subtype conn_term_stats;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_CONN_STATE_PS_STATS_C
  ---------------------------------------------------------------------------*/

/* The connected state protocol stack events we keep track of. */
typedef enum
{
  HDRPLOG_RUP_MESSAGE_COUNT,
    /* Number of Route Update Protocol messages received. */

  HDRPLOG_TCA_MESSAGE_COUNT,
    /* Number of Traffic Channel Assignment messages received. */

  HDRPLOG_RESET_REPORT_MESSAGE_COUNT,
    /* Number of reset report messages received. */

  HDRPLOG_NEIGHBOR_LIST_MESSAGE_COUNT,
    /* Number of neighbor list messages received. */

  HDRPLOG_TCC_MESSAGE_COUNT,
    /* Number of Traffic Channel Complete messages received. */

  HDRPLOG_TOTAL_USERS_COUNT,
    /* Number of total users in the quick config messages. */

  HDRPLOG_USERS_MESSAGES_COUNT,
    /* Number of messages containing user information. */

  HDRPLOG_TCAM_PILOT_COUNT,
    /* Number of pilots in the Traffic Channel Assignment Messages. */

  HDRPLOG_RUP_WITH_ZERO_KEEP_COUNT,
    /* Number of RUPs with no keep=1 fields. */

  HDRPLOG_RUP_WITH_OFS_NEIGHBORS_COUNT,
    /* Number of RUPs with off frequency search neighbors in them. */

  HDRPLOG_HARD_HANDOFF_COUNT,
    /* Number of hard handoffs performed. */

  HDRPLOG_UATI_ASSIGNMENT_COUNT,
    /* Number of UATI Assignment Messages recieved. */

  HDRPLOG_RUP_PILOT_COUNT,
    /* Number of pilots in the Route Update Protocol messages. */

  HDRPLOG_NUM_CONN_STATE_STATS_COUNTS
    /* Number of protocol stack connected state stats collected. */

} hdrplog_conn_state_stats_count_type;

/* The connected state info related to the protocol stack. */
typedef PACK(struct) 
{
  uint8 version;
    /* The version number of the packet. */

  uint32 since_reset_counts[HDRPLOG_NUM_CONN_STATE_STATS_COUNTS];
    /* The connected state statistics from the last reset. */

  uint32 since_call_counts[HDRPLOG_NUM_CONN_STATE_STATS_COUNTS];
    /* The connected state statistics from the start of the last call. */

} hdrplog_conn_state_stats_struct_subtype;

LOG_RECORD_DEFINE(LOG_HDR_CONN_STATE_PS_STATS_C)

  hdrplog_conn_state_stats_struct_subtype conn_state_ps_stats;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_CONN_STATE_RMAC_STATS_C
  ---------------------------------------------------------------------------*/

/* The connected state RMAC events we keep track of. */
typedef enum
{
  HDRRMACLOG_DRC_SUPERVISION_ENTRY_COUNT,
    /* Number of times we started a DRC supervision timer. */

  HDRRMACLOG_DRC_SUPERVISION_EXIT_COUNT,
    /* Number of times we stopped a DRC supervision timer. */

  HDRRMACLOG_RLSI_ENTRY_COUNT,
    /* Number of times we entered the silence interval. */

  HDRRMACLOG_RLSI_EXIT_COUNT,
    /* Number of times we exited the silence interval. */

  HDRRMACLOG_TOTAL_DRC_SUPERVISION_TIME,
    /* Total amount of time spent in DRC supervision. */

  HDRRMACLOG_TOTAL_RLSI_TIME,
    /* Total amount of time spent in the silence interval. */

  HDRRMACLOG_ACCUMULATED_BROADCAST_RRL_VALUES,
    /* Accumulated broadcast reverse rate level values. */

  HDRRMACLOG_BROADCAST_RRL_MEASUREMENT_COUNT,
    /* Total number of broadcast RRL values accumulated. */

  HDRRMACLOG_ACCUMULATED_UNICAST_RRL_VALUES,
    /* Accumulated unicast reverse rate level values. */

  HDRRMACLOG_UNICAST_RRL_MEASUREMENT_COUNT,
    /* Total number of unicast RRL values accumulated. */

  HDRRMACLOG_RRI_AT_PA_LIMIT_COUNT,
    /* Total number of times the RRI value was at the PA limit. */

  HDRRMACLOG_ENCODER_WINDOW_MISSED_COUNT,
    /* Number of times we missed the encoder window during a tx. */

  HDRRMACLOG_ENCODER_WINDOW_MISSED_DROPPED_BYTES_COUNT,
    /* Number of bytes not sent since we missed the encoder window during a 
       tx. */

  HDRRMACLOG_RAB_COUNT,
    /* Number of RAB bits counted. */

  HDRRMACLOG_RTC_FRAMES_COUNT,
    /* Number of Reverse Traffic Channel frames counted. */

  HDRRMACLOG_RATE_INERTIA_FRAMES_COUNT,
    /* Number of frames affected by rate inertia. */

  HDRRMACLOG_NUM_CONN_STATE_STATS_COUNTS
    /* The number of connected stat stats gathered for the RMAC layer. */

} hdrrmaclog_conn_state_stats_count_type;

/* The connected state statistics from the RMAC layer. */
typedef PACK(struct) 
{
  uint8 version;
    /* The version number of the packet. */

  uint32 since_reset_counts[HDRRMACLOG_NUM_CONN_STATE_STATS_COUNTS];
    /* The connected state statistics from the last reset. */

  uint32 since_call_counts[HDRRMACLOG_NUM_CONN_STATE_STATS_COUNTS];
    /* The connected state statistics from the start of the last call. */

} hdrrmaclog_conn_state_stats_struct_subtype;

LOG_RECORD_DEFINE(LOG_HDR_CONN_STATE_RMAC_STATS_C)

  hdrrmaclog_conn_state_stats_struct_subtype conn_state_rmac_stats;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_CONN_STATE_SRCH_STATS_C
  ---------------------------------------------------------------------------*/

/* The connected state search statistics we keep track of. */
typedef enum
{
  HDRSRCHLOG_SERVING_SECT_HANDOFF_COUNT = 0,
    /* The number of soft handoffs made. */

  HDRSRCHLOG_ASET_PILOT_ADD_EVENT_COUNT,
    /* The number of pilots added to the active set. */

  HDRSRCHLOG_ASET_PILOT_DROP_EVENT_COUNT,
    /* The number of pilots dropped from the active set. */

  HDRSRCHLOG_RSET2CSET_PILOT_ADD_EVENT_COUNT,
    /* The number of rset pilots moved to the candidate set. */

  HDRSRCHLOG_NSET2RSET_PILOT_DROP_EVENT_COUNT,
    /* The number of neighbor set pilots moved to the rset. */

  HDRSRCHLOG_DROP_TMR_START_EVENT_COUNT,
    /* The number of times the drop timer was started. */

  HDRSRCHLOG_DROP_TMR_CANCEL_EVENT_COUNT,
    /* The number of times the drop timer was canceled. */

  HDRSRCHLOG_DROP_TMR_EXPIRED_EVENT_COUNT,
    /* The number of times the drop timer expired. */

  HDRSRCHLOG_OFS_COUNT = 8,
    /* The number of off frequency searches. */

  HDRSRCHLOG_ASET_PILOT_COUNT = 9,
    /* The number of active set pilots counted every triage. */

  HDRSRCHLOG_CSET_PILOT_COUNT = 10,
    /* The number of candidate set pilots counted every triage. */

  HDRSRCHLOG_NSET_PILOT_COUNT = 11,
    /* The number of neighbor set pilots counted every triage. */

  HDRSRCHLOG_ASSIGNED_FINGERS_COUNT = 12,
    /* The number of assigned fingers counted every triage. */

  HDRSRCHLOG_LOCKED_FINGERS_COUNT = 13,
    /* The number of locked fingers counted every triage. */

  HDRSRCHLOG_TRIAGE_CYCLE_COUNT = 14,
    /* The number of triage cycles that occurred. */

  HDRSRCHLOG_NUM_CONN_STATE_STATS_COUNTS = 15
    /* The number of connected state stats for the searcher. */

} hdrsrchlog_conn_state_stats_count_type;

/*  HDRSRCHLOG_CELL_DIVERSITY_COUNT, */
/*  HDRSRCHLOG_SUBNET_DIVERSITY_COUNT, */

/* The connected state info related to the searcher. */
typedef PACK(struct) 
{
  uint8 version;
    /* The version number of the packet. */
  
  uint32 since_reset_counts[HDRSRCHLOG_NUM_CONN_STATE_STATS_COUNTS];
    /* The connected state statistics from the last reset. */

  uint32 since_call_counts[HDRSRCHLOG_NUM_CONN_STATE_STATS_COUNTS];
    /* The connected state statistics from the start of the last call. */

  uint8 curr_rssi;
    /* The RSSI value at the time the packet is logged. */

  uint8 curr_dli;
    /* The data level indicator at the time the packet is logged. */

} hdrsrchlog_conn_state_stats_struct_subtype;

LOG_RECORD_DEFINE(LOG_HDR_CONN_STATE_SRCH_STATS_C)

  hdrsrchlog_conn_state_stats_struct_subtype conn_state_srch_stats;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_CONN_STATE_TX_INFO_C
  ---------------------------------------------------------------------------*/

/* The connected state TX related statistics we keep track of. */
typedef enum
{
  HDRSLP_1ST_ATTEMPT_SUCCESS_COUNT,
    /* HDR signaling packet that was sent successfully on the first attempt. */

  HDRSLP_2ND_ATTEMPT_SUCCESS_COUNT,
    /* HDR signaling packet that was sent successfully on the second attempt. */

  HDRSLP_3RD_ATTEMPT_SUCCESS_COUNT,
    /* HDR signaling packet that was sent successfully on the third attempt. */

  HDRSLP_TRANSMIT_FAILURE_COUNT,
    /* HDR signaling packet that failed after three send attempts. */

  HDRSLP_NUM_CONN_STATE_STATS_COUNTS
    /* Total number of possible TX related statistics. */

} hdrslp_log_conn_state_stats_count_type;

/* The TX related connected state info. */
typedef PACK(struct) 
{
  uint8 version;
    /* The version number of the packet. */
  
  uint32 since_reset_counts[HDRSLP_NUM_CONN_STATE_STATS_COUNTS];
    /* The connected state statistics from the last reset. */

  uint32 since_call_counts[HDRSLP_NUM_CONN_STATE_STATS_COUNTS];
    /* The connected state statistics from the start of the last call. */

} hdrslp_log_conn_state_stats_struct_subtype;

LOG_RECORD_DEFINE(LOG_HDR_CONN_STATE_TX_STATS_C)

  hdrslp_log_conn_state_stats_struct_subtype conn_state_tx_stats;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_REVA_SU_FWD_STATS_C
  ---------------------------------------------------------------------------*/

/* Rev-A Traffic channel CRC statistics type.
   A Rev-A packet sent at DRC x could have multiple transmission formats with
   unique packet size for each transmission format. There is one  
   good count for each of these packet size(transmission format)/DRC 
   combination. Each member in this struct good_drcx_<packet_size> denotes 
   good count for DRC "x" and transmission format with packet size 
   "packet_size". The bad counts bad_drcx are recorded per-DRC only since there
   is no way of finding the size of CRC failed packet */
typedef PACK(struct)
{
  uint32 good_drc0_128bits;   /* 1.  DRC 0 - [128, 16, 1024] */
  uint32 good_drc0_256bits;   /* 2.  DRC 0 - [256, 16, 1024] */
  uint32 good_drc0_512bits;   /* 3.  DRC 0 - [512, 16, 1024] */
  uint32 good_drc0_1024bits;  /* 4.  DRC 0 - [1024, 16, 1024] */
  uint32 bad_drc0;            /* Number of CRC failed DRC 0 pkts */

  uint32 good_drc1_128bits;   /* 5.  DRC 1 - [128, 16, 1024] */
  uint32 good_drc1_256bits;   /* 6.  DRC 1 - [256, 16, 1024] */
  uint32 good_drc1_512bits;   /* 7.  DRC 1 - [512, 16, 1024] */
  uint32 good_drc1_1024bits;  /* 8.  DRC 1 - [1024, 16, 1024] */
  uint32 bad_drc1;            /* Number of CRC failed DRC 1 pkts */

  uint32 good_drc2_128bits;   /* 9.  DRC 2 - [128, 8, 512] */
  uint32 good_drc2_256bits;   /* 10. DRC 2 - [256, 8, 512] */
  uint32 good_drc2_512bits;   /* 11. DRC 2 - [512, 8, 512] */
  uint32 good_drc2_1024bits;  /* 12. DRC 2 - [1024, 8, 512] */
  uint32 bad_drc2;            /* Number of CRC failed DRC 2 pkts */

  uint32 good_drc3_128bits;   /* 13. DRC 3 - [128, 4, 256] */
  uint32 good_drc3_256bits;   /* 14. DRC 3 - [256, 4, 256] */
  uint32 good_drc3_512bits;   /* 15. DRC 3 - [512, 4, 256] */
  uint32 good_drc3_1024bits;  /* 16. DRC 3 - [1024, 4, 256] */
  uint32 bad_drc3;            /* Number of CRC failed DRC 3 pkts */

  uint32 good_drc4_128bits;   /* 17. DRC 4 - [128, 2, 128] */
  uint32 good_drc4_256bits;   /* 18. DRC 4 - [256, 2, 128] */
  uint32 good_drc4_512bits;   /* 19. DRC 4 - [512, 2, 128] */
  uint32 good_drc4_1024bits;  /* 20. DRC 4 - [1024, 2, 128] */
  uint32 bad_drc4;            /* Number of CRC failed DRC 4 pkts */

  uint32 good_drc5_512bits;   /* 21. DRC 5 - [512, 4, 128] */
  uint32 good_drc5_1024bits;  /* 22. DRC 5 - [1024, 4, 128] */
  uint32 good_drc5_2048bits;  /* 23. DRC 5 - [2048, 4, 128] */
  uint32 bad_drc5;            /* Number of CRC failed DRC 5 pkts */

  uint32 good_drc6_128bits;   /* 24. DRC 6 - [128, 1, 64] */
  uint32 good_drc6_256bits;   /* 25. DRC 6 - [256, 1, 64] */
  uint32 good_drc6_512bits;   /* 26. DRC 6 - [512, 1, 64] */
  uint32 good_drc6_1024bits;  /* 27. DRC 6 - [1024, 1, 64] */
  uint32 bad_drc6;            /* Number of CRC failed DRC 6 pkts */

  uint32 good_drc7_512bits;   /* 28. DRC 7 - [512, 2, 64] */
  uint32 good_drc7_1024bits;  /* 29. DRC 7 - [1024, 2, 64] */
  uint32 good_drc7_2048bits;  /* 30. DRC 7 - [2048, 2, 64] */
  uint32 bad_drc7;            /* Number of CRC failed DRC 7 pkts */

  uint32 good_drc8_1024bits;  /* 31. DRC 8 - [1024, 2, 64] */
  uint32 good_drc8_3072bits;  /* 32. DRC 8 - [3072, 2, 64] */
  uint32 bad_drc8;            /* Number of CRC failed DRC 8 pkts */

  uint32 good_drc9_512bits;   /* 33. DRC 9 - [512, 1, 64] */
  uint32 good_drc9_1024bits;  /* 34. DRC 9 - [1024, 1, 64] */
  uint32 good_drc9_2048bits;  /* 35. DRC 9 - [2048, 1, 64] */
  uint32 bad_drc9;            /* Number of CRC failed DRC 9 pkts */
  
  uint32 good_drc10_4096bits; /* 36. DRC 10 - [4096, 2, 64] */
  uint32 bad_drc10;           /* Number of CRC failed DRC 10 pkts */

  uint32 good_drc11_1024bits; /* 37. DRC 11 - [1024, 1, 64] */
  uint32 good_drc11_3072bits; /* 38. DRC 11 - [3072, 1, 64] */
  uint32 bad_drc11;           /* Number of CRC failed DRC 11 pkts */

  uint32 good_drc12_4096bits; /* 39. DRC 12 - [4096, 1, 64] */
  uint32 bad_drc12;           /* Number of CRC failed DRC 12 pkts */

  uint32 good_drc13_5120bits; /* 40. DRC 13 - [5120, 2, 64] */
  uint32 bad_drc13;           /* Number of CRC failed DRC 13 pkts */
  
  uint32 good_drc14_5120bits; /* 41. DRC14 - [5120, 1, 64] */
  uint32 bad_drc14;           /* Number of CRC failed DRC 14 pkts */

} hdrlog_rev_a_tc_crc_stats_type;


/* Rev-A traffic channel early termination slot count type.
   Similar to CRC count we need one member for each of DRC/transmission format
   (packet size) combination. However, we need an array of size nominal
   transmit duration associated with each transmission format to record 
   the number of packets terminated early at each slot position. Each 
   struct member drc<x>_<pkt_size>[y] denotes an array of length "y" for early
   termination stats of packets of size "pkt_size" and DRC "x" */
typedef PACK(struct)
{
  uint32 drc0_128bits[16];    /* 1.  DRC 0 - [128, 16, 1024] */
  uint32 drc0_256bits[16];    /* 2.  DRC 0 - [256, 16, 1024] */
  uint32 drc0_512bits[16];    /* 3.  DRC 0 - [512, 16, 1024] */
  uint32 drc0_1024bits[16];   /* 4.  DRC 0 - [1024, 16, 1024] */

  uint32 drc1_128bits[16];    /* 5.  DRC 1 - [128, 16, 1024] */
  uint32 drc1_256bits[16];    /* 6.  DRC 1 - [256, 16, 1024] */
  uint32 drc1_512bits[16];    /* 7.  DRC 1 - [512, 16, 1024] */
  uint32 drc1_1024bits[16];   /* 8.  DRC 1 - [1024, 16, 1024] */

  uint32 drc2_128bits[8];     /* 9.  DRC 2 - [128, 8, 512] */
  uint32 drc2_256bits[8];     /* 10. DRC 2 - [256, 8, 512] */
  uint32 drc2_512bits[8];     /* 11. DRC 2 - [512, 8, 512] */
  uint32 drc2_1024bits[8];    /* 12. DRC 2 - [1024, 8, 512] */

  uint32 drc3_128bits[4];     /* 13. DRC 3 - [128, 4, 256] */
  uint32 drc3_256bits[4];     /* 14. DRC 3 - [256, 4, 256] */
  uint32 drc3_512bits[4];     /* 15. DRC 3 - [512, 4, 256] */
  uint32 drc3_1024bits[4];    /* 16. DRC 3 - [1024, 4, 1024] */

  uint32 drc4_128bits[2];     /* 17. DRC 4 - [128, 2, 128] */
  uint32 drc4_256bits[2];     /* 18. DRC 4 - [256, 2, 128] */
  uint32 drc4_512bits[2];     /* 19. DRC 4 - [512, 2, 128] */
  uint32 drc4_1024bits[2];    /* 20. DRC 4 - [1024, 2, 128] */

  uint32 drc5_512bits[4];     /* 21. DRC 5 - [512, 4, 128] */
  uint32 drc5_1024bits[4];    /* 22. DRC 5 - [1024, 4, 128] */
  uint32 drc5_2048bits[4];    /* 23. DRC 5 - [2048, 4, 128] */

  uint32 drc6_128bits[1];     /* 24. DRC 6 - [128, 1, 64] */
  uint32 drc6_256bits[1];     /* 25. DRC 6 - [256, 1, 64] */
  uint32 drc6_512bits[1];     /* 26. DRC 6 - [512, 1, 64] */
  uint32 drc6_1024bits[1];    /* 27. DRC 6 - [1024, 1, 64] */
  
  uint32 drc7_512bits[2];     /* 28. DRC 7 - [512, 2, 64] */
  uint32 drc7_1024bits[2];    /* 29. DRC 7 - [1024, 2, 64] */
  uint32 drc7_2048bits[2];    /* 30. DRC 7 - [2048, 2, 64] */

  uint32 drc8_1024bits[2];    /* 31. DRC 8 - [1024, 2, 64] */
  uint32 drc8_3072bits[2];    /* 32. DRC 8 - [3072, 2, 64] */

  uint32 drc9_512bits[1];     /* 33. DRC 9 - [512, 1, 64] */
  uint32 drc9_1024bits[1];    /* 34. DRC 9 - [1024, 1, 64] */
  uint32 drc9_2048bits[1];    /* 35. DRC 9 - [2048, 1, 64] */

  uint32 drc10_4096bits[2];   /* 36. DRC 10 - [4096, 2, 64] */

  uint32 drc11_1024bits[1];   /* 37. DRC 11 - [1024, 1, 64] */
  uint32 drc11_3072bits[1];   /* 38. DRC 11 - [3072, 1, 64] */

  uint32 drc12_4096bits[1];   /* 39. DRC 12 - [4096, 1, 64] */

  uint32 drc13_5120bits[2];   /* 40. DRC 13 - [5120, 2, 64] */
  
  uint32 drc14_5120bits[1];   /* 41. DRC 14 - [5120, 1, 64] */

} hdrlog_rev_a_tc_early_decode_type;


/* Rev-A control channel standard packet CRC statistics type. 
   The Rev-A control channel has five transmission formats. The transmission
   formats [1024,16,1024] and [1024,8,512] correspond to drcs 38.4 and 76.8 of
   release 0. These are standard control channel packets. */
typedef PACK(struct)
{
  uint32 good_drc1;
    /* Number of CRC passed CC std packet 38.4kbps - [1024,16,1024] */

  uint32 bad_drc1;
    /* Number of CRC failed DRC 1 CC std packet 38.4kbps - [1024,16,1024] */

  uint32 good_drc2;
    /* Number of CRC passed DRC 2 CC std packet 76.8kbps - [1024,8,512] */

  uint32 bad_drc2;
    /* Number of CRC failed DRC 2 CC std packet 76.8kbps - [1024,8,512] */

} hdrlog_cc_crc_stats_type;


/* Rev-A control channel short packet CRC statistics type.
   In addition to the above defined standard packets, 3 new short packet 
   transmission formats have been introduced in Rev-A namely [128,4,1024],
   [256,4,1024] and [512,4,1024]. CRC stats for these short packets. */
typedef PACK(struct)
{
  uint32 good_pktsize_128bits;
    /* CRC stats for CC short packet - 19.2kbps [128,4,1024] */

  uint32 good_pktsize_256bits;
    /* CRC stats for CC short packet - 38.4kbps [256,4,1024] */

  uint32 good_pktsize_512bits;
    /* CRC stats for CC short packet - 76.8kbps [512,4,1024] */

  uint32 bad_short_cc;
    /* Number of CRC failed short CC packet */

} hdrlog_rev_a_short_cc_crc_stats_type;


/* Rev-A control channel standard packet early decode stats type */
typedef PACK(struct)
{
  uint32 drc1[16];
    /* Early decode stats for CC standard packet 38.4kbps - [1024,16,1024] */

  uint32 drc2[8];
    /* Early decode stats for CC standard packet 76.8kbps - [1024,8,512] */

} hdrlog_cc_early_decode_type;


/* Rev-A control channel short packet early decode stats type. */
typedef PACK(struct)
{
  uint32 pktsize_128bits[4];
    /* Early decode stats for CC short packet 19.2kbps - [128,4,1024] */

  uint32 pktsize_256bits[4];
    /* Early decode stats for CC short packet 38.4kbps - [256,4,1024] */

  uint32 pktsize_512bits[4];
    /* Early decode stats for CC short packet 76.8kbps - [512,4,1024] */

} hdrlog_rev_a_short_cc_early_decode_type;


/* Log packet for Rev-A Single-user packet Forward statistics summary. */
LOG_RECORD_DEFINE ( LOG_HDR_REVA_SU_FWD_STATS_C )
  
  uint32 log_seq_num;
    /* log packet sequence number */

  hdrlog_rev_a_tc_crc_stats_type    tc_crc;
    /* Traffic channel CRC stats */

  hdrlog_rev_a_tc_early_decode_type tc_early_decode;
    /* Traffic channel early decode stats */

  hdrlog_rev_a_short_cc_crc_stats_type    cc_short_crc;
    /* Control channel CRC stats for short packets */
  
  hdrlog_cc_crc_stats_type cc_std_crc;
    /* Control channel CRC stats for standard sizes */

  hdrlog_rev_a_short_cc_early_decode_type cc_short_early_decode;
    /* Control channel early decode stats for short packets */

  hdrlog_cc_early_decode_type cc_std_early_decode;
    /* Control channel early decode stats for standard packets */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_REVA_MU_FWD_STATS_C
  ---------------------------------------------------------------------------*/

/* Rev-A Multi-user Packet CRC stats type.
   Right now there is no way of finding out the requested DRC for an MUP.
   Hence, MUP statistics are maintained per transmission format irrespective 
   of the requested DRC. */
typedef PACK(struct)
{

/*---------------------------------------------------------------------------
 * AT decodes FL packets based on Preamble which is covered with MACIndex of
 * the user. Preambles for all transmission formats look identical and when
 * a preamble is detected while decoding at a particular DRC, AT cannot know 
 * which tx format of that particular DRC was scheduled by AN. Hence at the 
 * end of every slot, AT tries to decode all supported Tx.formats of that DRC.
 * When the CRC, passes, it knows the Tx. Format scheduled by AN. However, 
 * when the CRC fails, the "Data Length" field of decoder header denotes the
 * SHORTEST PACKET TYPE supported because it is the last hypothesis tried 
 * before a CRC failure is declared. Since there is no way of knowing the 
 * Tx. Format scheduled by AN when the CRC fails, only per-DRC CRC failed 
 * statistics are recorded. 
 --------------------------------------------------------------------------*/
  uint32 good_cnt_drc3_128;
    /* Number of good of DRC 3 [128,4,256] short Multi-user packets */

  uint32 good_cnt_drc3_256;
    /* Number of good DRC 3 [256,4,256] short Multi-user packets */

  uint32 good_cnt_drc3_512;
    /* Number of good DRC 3 [512,4,256] short Multi-user packets */

  uint32 good_cnt_drc3_1024;
    /* Number of good DRC 3 [1024,4,256] Multi-user packets  */
  uint32 bad_cnt_drc3;
    /* Number of bad DRC 3 [1024,4,256] Multi-user packets */

  uint32 good_cnt_drc5_2048;
    /* Number of good DRC 5 [2048,4,128] Multi-user packets */
  uint32 bad_cnt_drc5;
    /* Number of bad DRC 5 [2048,4,128] Multi-user packets */

  uint32 good_cnt_drc8_3072;
    /* Number of good DRC 8 [3072,2,64] Multi-user packets */
  uint32 bad_cnt_drc8;
    /* Number of bad DRC 8 [3072,2,64] Multi-user packets */

  uint32 good_cnt_drc10_4096;
    /* Number of good DRC 10 [4096,2,64] Multi-user packets */
  uint32 bad_cnt_drc10;
    /* Number of bad DRC 10 [4096,2,64] Multi-user packets */
  
  uint32 good_cnt_drc13_5120;
    /* Number of good DRC 13 [5120,2,64] Multi-user packets */
  uint32 bad_cnt_drc13;
    /* Number of bad DRC 13 [5120,2,64] Multi-user packets */

} hdrlog_mup_crc_stats_type;


/* Rev-A Multi-user packet early decode statistics */
typedef PACK(struct)
{
  uint32 size_128[4];
    /* Received DRC 3 [128,4,256] */

  uint32 size_256[4];
    /* Received DRC 3 [256,4,256] */

  uint32 size_512[4];
    /* Received DRC 3 [512,4,256]*/

  uint32 size_1024[4];
    /* Received DRC 3 [1024,4,256] */

  uint32 size_2048[4];
    /* Received DRC 5 [2048,4,128] */

  uint32 size_3072[2];
    /* Received DRC 8 [3072,2,64] */

  uint32 size_4096[2];
    /* Received DRC 10 [4096,2,64] */

  uint32 size_5120[2];
    /* Received DRC 13 [5120,2,64] */

} hdrlog_mup_early_decode_type;

/* Log packet for Rev-A Multi-user Packet statistics */
LOG_RECORD_DEFINE ( LOG_HDR_REVA_MU_FWD_STATS_C )

  uint32 log_seq_num;
    /* log sequence number */

  hdrlog_mup_crc_stats_type crc;
    /* CRC statistics */

  hdrlog_mup_early_decode_type early_decode;
    /* early decode statistics */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_EQUALIZER_DATA_C
---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  PACK(struct)
  {
    uint16    antenna    : 1;  /* 0 : antenna 0, 1 : Antenna1 */
    uint16    diversity  : 1;  /* 0/1 : diversity disabled/enabled for eq*/
    uint16    eq_index   : 4;  /* index of the equalizer [0-11] */
    uint16    lms_step   : 3;  /* LMS step size */
    uint16    filt_length: 5;  /* filter length */
    uint16    reserved   : 2;  /* reserved */
  } eq_info;

  PACK(struct)
  {
    uint16     enable              : 2;
      /* Decision whether multipass1 is enabled */
    uint16     multipass_c2i_used  : 2; 
      /* Multipass C/I used in Rake/Eq  decision */
    uint16     multipass1_lms_step : 3; 
      /* Multipass1 LMS step */
    uint16     multipass2_lms_step : 3; 
      /* Multipass2 LMS step */
    uint16     reserved            : 6; 
      /* Reserved */
  } eq_multipass_info;

  uint32   c2i;                /* Instantaneous equalizer C/I */

} log_hdr_eq_data_type;

LOG_RECORD_DEFINE(LOG_HDR_EQUALIZER_DATA_C)
  uint8    version;            /* The version number of the packet */

  uint8    srch_state;         /* HDR Searcher state */

  uint32   mstrx8;             /* Mobile Station time reference */

  uint16   mstr_err_x8;        /* MSTR w.r.t. to earliest arriving finger */
  
  uint8    mode;               /* Equalizer/Rake/best of demodulation mode */

  uint32   com_posx8;          /* Center of mass rtc of best ASP 
                                  in chipx8 units */
  
  PACK(struct)
  {
    uint8    num_eq   : 4;     /* Number of equalizers enabled */
    uint8    flag     : 1;     /* Equalizer on/off flag */
    uint8    reserved : 3;     /* reserved */
  } eq;

  uint16 serving_pn;           /* PN offset of the best asp that the 
                                  equalizer is demodulating */

  PACK(struct)
  {
    uint8  best_asp_idx : 4;   /* Best ASP index */
    uint8  reserved     : 4;   /* reserved */
  } asp;

  log_hdr_eq_data_type eq_data[1];
    /* entries of the above struct */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_EQUALIZER_CI_BUFFER_C
---------------------------------------------------------------------------*/
#define HDR_SLOTS_PER_FRAME              16
#define HDRLOG_EQ_CI_FRAMES_PER_PKT      2

typedef PACK(struct)
{
  uint32 eq_status[HDR_SLOTS_PER_FRAME];
    /* Demod decision as to whether Rake/Equalizer/best selection
       was used  in each slot */

  uint16 eq_best_ci[HDR_SLOTS_PER_FRAME];
    /* C/I of the best equalizer in each slot */

  uint16 eq_alt_rake_ci[HDR_SLOTS_PER_FRAME];
    /* Alternate Rake C/I in each slot */

  uint8  slot_count;
    /* Slot Count */

} log_hdr_eq_ci_buffer_type;

LOG_RECORD_DEFINE(LOG_HDR_EQUALIZER_CI_BUFFER_C)
  uint8  version;
    /* The version number of the packet. */

  log_hdr_eq_ci_buffer_type eq_data[1];
    
LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_MDSP_DEBUG_BUFFER_C
---------------------------------------------------------------------------*/
#define MDSP_CHUNK_SIZE    64
  /* Size of MDSP chunk in words */

typedef uint16 hdrlog_mdsp_debug_buffer_type;
  /* Data type for debug log data read from the MDSP  */

LOG_RECORD_DEFINE(LOG_HDR_MDSP_DEBUG_BUFFER_C)

hdrlog_mdsp_debug_buffer_type mdsp_data[1];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_REVA_RL_GAINS_C
---------------------------------------------------------------------------*/

#define HDRLOG_REVA_RL_GAINS_VER           0
  /* Packet version number */

#define HDRLOG_MAX_SUBFRAMES_PER_RL_GAINS_PKT 8
  /* Version number for packet */

typedef PACK(struct)      /* 28 byte structure */
{

  int16 tx_open_loop;
    /* Tx Open Loop */

  int16 tx_total;
    /* Total Tx AGC */

  int16 tx_pilot;
    /* Tx Pilot power */

  int16 tx_adjust;
    /* Closed Loop Adjust */

} log_hdr_tx_agc_struct_type;

typedef PACK(struct)      /* 28 byte structure */
{
  uint8    subframe_n;
    /* The subframe being logged */

  PACK(struct)
  {
    uint16    rri : 4;
      /* RRI being sent in current subframe */

    uint16    sp_n : 2;
      /* subpacket number of current packet in transit */

    uint16    transmission_mode : 1;
      /* Transmission mode: 0 == HiCap, 1 == LoLat */

    uint16    pa_enabled : 1;
      /* 1 == PA is enabled, 0 == PA not enabled */

    uint16    rpc : 2;
      /* RPC bit received for this subframe. 0 = Hold, 1 = Up, 2 = Down. */

    uint16    tx_min_power : 1;
      /* Indicates whether Tx AGC is at max power */

    uint16    tx_max_power : 1;
      /* Indicates whether Tx AGC is at max power */

    uint16    tx_throttle : 1;
      /* Indicates whether Tx RL gains are being throttled */

    uint16    reserved : 3;
      /* Reserved bits */

  } rl_info;
  
  uint16 pilot_hs0;
    /* Linear pilot power in halfslot 0 of subframe */

  uint16 ack_hs0;
    /* Linear ACK channel power in halfslot 0 of subframe */
  
  uint16 pilot_hs1;
    /* Linear pilot power in halfslot 1 of subframe */

  uint16 rri_hs1;
    /* Linear RRI channel power in halfslot 1 of subframe */

  uint16 dsc_hs1;
    /* Linear DSC channel power in halfslot 1 of subframe */

  uint16 drc_hs1;
    /* Linear DRC channel power in halfslot 1 of subframe */

  uint16 data_hs1;
    /* Linear data channel power in halfslot 1 of subframe */

  uint16 aux_hs1;
    /* Linear auxiliary pilot gain in halfslot 1 of subframe */

  log_hdr_tx_agc_struct_type tx_agc;
    /* Tx AGC info */

  int16 pa_headroom;
    /* PA headroom (dB/4) */

} log_hdr_reva_rl_gains_type;

    
LOG_RECORD_DEFINE(LOG_HDR_REVA_RL_GAINS_C)

  uint8  version;
    /* The version number of the packet. */

  uint8  subframe_count;
    /* The number of subframes logged in this packet */

  log_hdr_reva_rl_gains_type reva_rl_subframe_gains
    [HDRLOG_MAX_SUBFRAMES_PER_RL_GAINS_PKT];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                        LOG_HDR_REVA_RL_PKT_METRICS_C
---------------------------------------------------------------------------*/

#define HDRLOG_REVA_RL_PKT_METRICS_VER    1
  /* Packet version number */

#define HDRLOG_MAX_RL_SUBPACKETS       4
#define HDRLOG_NUM_RL_PAYLOADS        12
#define HDRLOG_NUM_RL_TX_MODES         2
#define HDRLOG_NUM_RL_INTERLACES       3

/* RL rate logging metrics. Packet size is 1740 bytes + log header. */
typedef PACK(struct)
{
  PACK(struct)
  {
    PACK(struct)
    {
      PACK(struct)
      { 
        uint32 acks[HDRLOG_MAX_RL_SUBPACKETS];
        uint32 parq_naks;
        uint32 total_sent;
      } rri[HDRLOG_NUM_RL_PAYLOADS]; 

    } tm[HDRLOG_NUM_RL_TX_MODES];

    uint32 unexpected_parq_naks;

  } interlace[HDRLOG_NUM_RL_INTERLACES];

} log_hdr_reva_rl_pkt_metrics_type;


LOG_RECORD_DEFINE(LOG_HDR_REVA_RL_PKT_METRICS_C)

  uint8  version;
    /* The version number of the packet. */

  uint32 log_seq_num;
    /* log sequence number */

  log_hdr_reva_rl_pkt_metrics_type rl_pkt_metrics;

LOG_RECORD_END

/*---------------------------------------------------------------------------
                        LOG_HDR_REVA_RMAC3_T2P_RESOURCE_C
---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  uint32 tx_t2p;
    /* T2P of packet currently being transmitted over the air */

  uint8  pkt_id;
    /* Packet ID of the packet currently being transmitted over the air */

  uint8  sub_frame_count;
    /* current sub-frame count */

  uint8  pkt_size_index;
    /* Packet size of the packet currently being transmitted over the air */

  uint16 sum_payload;
    /* Aggregate allotted payload size of eligible flows in current packet */

  uint8  normal_hdr;
    /* TRUE if we have not tuned away and RLSI is not in effect */

  int32  pilot_strength_ps;
    /* Value of PilotStrength(pilot_strength) function */

  int16  frab;
    /* Filtered Reverse Activity Bit (from mDSP */

} hdrlog_rmac3_at_wide_info_type;

typedef PACK(struct)
{
  uint16 flow_allotment;
    /* Actual size (in bytes) of this flow */

  uint32 t2p_outflow;
    /* Actual T2P resource withdrawn from this flow's bucket in proportion to
     * its allotment
     */

  int32  bucket_level;
    /* Current bucket level */

  int8   qrab;
    /* Flow specific QRAB, 1: loaded, -1: unloaded */

  uint8  mac_flow_id;
    /* Mac flow ID */

  int32  delta_t2p_inflow;
    /* T2P Inflow increment or decrement for this flow */

  uint32 t2p_inflow;
    /* Average T2P resource added to bucket for this flow */

  uint8  tt2p_hold;
    /* Lifetime in subframes of the T2P allocation from Grant message */

  uint32 bucket_level_sat;
    /* Saturation level of bucket */

  int32  bucket_factor;
    /* Value of BucketFactor( T2PInflow, FRAB ) function */

} hdrlog_rmac3_flow_info_type;

LOG_RECORD_DEFINE(LOG_HDR_REVA_RMAC3_T2P_RESOURCE_C)

  uint8                          version;
    /* The version number of the packet. */

  hdrlog_rmac3_at_wide_info_type at_wide_info;
    /* common fileds applicable for the AT */

  uint8                          num_flows;
    /* Number of Active flows */

  hdrlog_rmac3_flow_info_type    flow_info[1];
    /* variable length record; there will be num_flows records of this type */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                       LOG_HDR_REVA_RMAC3_PKT_INFO_C
---------------------------------------------------------------------------*/
#define HDRLOG_REVA_RMAC3_PKT_INFO_VER    1
  /* Packet version number */

typedef PACK(struct)
{
  uint8  sub_frame_count;
    /* Count of sub-frames so far */

  uint16 cand_ps_set;
    /* final candidate packet size set */

  int16 rand_num;
    /* random number - used in t2p_dither_term calculation */

  PACK(struct)
  {
    uint8   final_ps_index     : 4;
      /* final packet size decided by RTC MAC */
    uint8   tm                 : 1;
      /* transmission mode ( HC/LL ) */
    uint8   reserved           : 3;
  } ps_info;
 
  uint16 tx_t2p_max;
    /* maximum transmitt T2P */

  uint16 round_down_quantum; /* TODO  */
    /* smallest unit of data allotted to a flow in the round-down algorithm */

} hdrlog_rmac3_pkt_at_wide_info_type;

typedef PACK(struct)
{
  uint32    initial_queue_size;
    /* initial queue size queried from PCP */

  uint32    pot_t2p_outflow;
    /* potential queue outflow  */

  uint32    q_outflow;
    /* maximum number of bytes allowed to be deducted from bucket */

  uint32    suggested_flow_allocation;
    /* suggested flow allocation as decided by RTC MAC algorithm  */

  uint32    actual_flow_allocation;
    /* actual flow allocation as decided by PCP  */
    
  PACK(struct)
  {
    uint8   mac_flow_id        : 4; 
      /* mac flow id  */
    uint8   non_empty_flow_set : 1; 
      /* whether this flow belongs to non-empty flow set  */
    uint8   eligible_flow_set  : 1;
      /* whether this flow belongs to eligible flow set  */
    uint8   reserved           : 2;

  } flow_set_info;
  
} hdrlog_rmac3_pkt_flow_info_type;

LOG_RECORD_DEFINE(LOG_HDR_REVA_RMAC3_PKT_INFO_C)

  uint8                                   version;
    /* The version number of the packet */

  hdrlog_rmac3_pkt_at_wide_info_type      at_wide_info;
    /* common fileds applicable for the AT */

  uint8                                   num_flows;
    /* Number of Active flows */

  hdrlog_rmac3_pkt_flow_info_type         flow_info[1];
    /* variable length record; there will be num_flows records of this type */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                           LOG_HDR_DRC_ARQ_DSC_BUFFER_C
---------------------------------------------------------------------------*/
#define HDRLOG_DRC_ARQ_DSC_FRAMES_PER_PKT      2
  /* Number of DRC-ARQ-DSC frames in one log packet */

typedef PACK(struct)
{
  /* This packet has been packed in the same format as the data
     read from the DSP so that DMSS does not have to waste precious
     cycles in unpacking and repacking the data */

  uint16 drc_arq_dsc [HDR_SLOTS_PER_ONE_ROLL];
  /* drc_arq_dsc is bit-packed in the same way as mDSP register
     to avoid un-necessary unpacking and re-packing overhead.
        Bit2 :Bit0  :: DRC cover
        Bit3        :: Reserved
        Bit6 :Bit4  :: DSC value
        Bit7        :: Multiuser ACK/NACK
        Bit11:Bit8  :: DRC value transmitted
        Bit12       :: ACK/NACK
        Bit13       :: ACK channel enabled
        Bit14       :: Redundant ACK/NACK
        Bit15       :: Reserved
  */

  uint8      slot_count;

  /* ------------ */
  /*   DRC Lock   */
  /* ------------ */

  uint16     drc_lock;
    /* -------------------------------------
       Bit 0 - DRC lock for ASP1
       Bit 1 - DRC lock valid bit for ASP1
       Bit 2 - DRC lock for ASP2
       Bit 3 - DRC lock valid bit for ASP2
       ...
       Bit 10 - DRC lock for ASP6
       Bit 11 - DRC lock valid bit for ASP6
       ------------------------------------
    */

  uint8      drc_lock_update;
    /* Last slot in which the DRC lock was updated */

  /* ------------- */
  /* ASP RL and FL */
  /* ------------- */

  PACK(struct)
  {
    uint8    asp_rl : 3; /* Bit2 :Bit0 :: RL ASP */
    uint8    asp_fl : 3; /* Bit5 :Bit3 :: FL ASP */
    uint8           : 2; /* Bit7 :Bit6 :: Reserved */

  } best_asp_index[ HDR_SLOTS_PER_ONE_ROLL ];
  
  /* ----------------------- */
  /* DRC and DSC gain boosts */
  /* ----------------------- */
  /* Following gain boosts contain bit-field data.
     Bit(n) : 1 - gain boost applied in slot(n)
              0 - gain boost not applied in slot(n)
  */
    uint16    drc_gain_boost; /* DRC Gain boost on RL. */
    uint16    dsc_gain_boost; /* DSC Gain boost on RL */
    
} log_hdr_drc_arq_dsc_buffer_type;

LOG_RECORD_DEFINE(LOG_HDR_DRC_ARQ_DSC_BUFFER_C)
   uint8  version;
    /* The version number of the packet. */

   log_hdr_drc_arq_dsc_buffer_type 
     drc_arq_dsc_data[HDRLOG_DRC_ARQ_DSC_FRAMES_PER_PKT];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                      LOG_HDR_REVA_MUP_USER_STATS_C
---------------------------------------------------------------------------*/

/* Maximum number of security layer packets in a single Multi-user packet */
#define HDRLOG_MAX_NUM_SEC_PKTS_IN_MUP            8

#define HDRLOG_REVA_MUP_USER_STATS_LOG_PKT_VER    0

/* This struct defines the per TxFormat User Statistics for a Multi-user 
   packet. The log packet would have once instance for each of the Multi-user
   transmission formats */
typedef PACK(struct)
{
  uint32 good_crc[HDRLOG_MAX_NUM_SEC_PKTS_IN_MUP];
    /* Per Tx Format Good CRC count for each category of MUP with different
       number of security layer packets in it. */

  uint32 bytes_for_user;
    /* Number of bytes destined for this user in the given Tx format */

}hdrlog_per_tx_format_mu_user_stats_type;

typedef PACK(struct)
{

  hdrlog_per_tx_format_mu_user_stats_type tx_128;
    /* User statistics for MU tx format [128,4,256] */

  hdrlog_per_tx_format_mu_user_stats_type tx_256;
    /* User statistics for MU tx format [256,4,256] */

  hdrlog_per_tx_format_mu_user_stats_type tx_512;
    /* User statistics for MU tx format [512,4,256] */

  hdrlog_per_tx_format_mu_user_stats_type tx_1024;
    /* User statistics for MU tx format [1024,4,256] */

  hdrlog_per_tx_format_mu_user_stats_type tx_2048;
    /* User statistics for MU tx format [2048,4,128] */

  hdrlog_per_tx_format_mu_user_stats_type tx_3072;
    /* User statistics for MU tx format [3072,2,64] */

  hdrlog_per_tx_format_mu_user_stats_type tx_4096;
    /* User statistics for MU tx format [4096,2,64] */

  hdrlog_per_tx_format_mu_user_stats_type tx_5120;
    /* User statistics for MU tx format [5120,2,64] */

} hdrlog_mup_user_stats_summ_type;

LOG_RECORD_DEFINE(LOG_HDR_REVA_MUP_USER_STATS_C)

  uint8 version;
    /* The version number of the log packet */

  uint32 log_seq_num;
    /* Sequence number of the log packet */

  hdrlog_mup_user_stats_summ_type mup_user_stats;
    /* MUP User statistics summary */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_REVA_RL_ARQ_STATUS_C
---------------------------------------------------------------------------*/

#define HDRLOG_REVA_RL_ARQ_STATUS_VER    0
  /* Version number of this packet */

#define HDRLOG_MAX_SUBFRAMES_PER_RL_ARQ_PKT 8
  /* Max number of subframes to log per packet */

typedef PACK(struct)      /* 5 byte structure */
{
  uint8    subframe_n;
    /* The subframe being logged */

  uint8    packet_id;
    /* The id of the packet sent this subframe */

  PACK(struct)
  {
    uint8     packet_status : 5;
      /* Packet Status report */

    uint8     parq : 1;
      /* P-ARQ Ack or Nack */

    uint8     harq : 2;
      /* H-ARQ Ack, Nack, or neither */
  } arq_status;

  PACK(struct)
  {
    uint16    rri : 4;
      /* RRI being sent in current subframe */

    uint16    sp_n : 2;
      /* subpacket number of current packet in transit */

    uint16    interlace : 2;
      /* interlace */

    uint16    transmission_mode : 1;
      /* Transmission mode: 0 == HiCap, 1 == LoLat */

    uint16    aux_pilot_enabled : 1;
      /* whether aux pilot is enabled */

    uint16    reserved : 6;
      /* Reserved bits */

  } packet_info;
  
} log_hdr_reva_rl_arq_status_type;

    
LOG_RECORD_DEFINE(LOG_HDR_REVA_RL_ARQ_STATUS_C)

  uint8  version;
    /* The version number of the packet. */

  uint8  subframe_count;
    /* The number of subframes logged in this packet */

  log_hdr_reva_rl_arq_status_type rl_arq_status
    [HDRLOG_MAX_SUBFRAMES_PER_RL_ARQ_PKT];

LOG_RECORD_END


/*---------------------------------------------------------------------------
                       LOG_HDR_REVA_RL_PKT_STATUS_C
---------------------------------------------------------------------------*/

#define HDRLOG_REVA_RL_PKT_STATUS_VER    0
  /* Version number of this packet */

#define HDRLOG_MAX_ENTRIES_PER_RL_PKT_STATUS_PKT 8
  /* Max number of subframes to log per packet */

typedef PACK(struct)      /* 21 byte structure */
{
  uint8    packet_id;
    /* The id of the packet */

  uint16   data_bytes_sent;
    /* Number of non-padding bytes */

  PACK(struct)
  {
    uint16    rri : 4;
      /* PacketSizeIndex (RRI) of packet */

    uint16    subpackets : 2;
      /* Number of subpackets transmitted */

    uint16    interlace : 2;
      /* interlace */

    uint16    transmission_mode : 1;
      /* Transmission mode: 0 == HiCap, 1 == LoLat */

    uint16    aux_pilot_enabled : 1;
      /* Whether aux pilot was enabled for this packet */

    uint16    parq_nak_received : 1;
      /* Whether entire packet was NAK'd */

    uint16    reserved : 5;
      /* Reserved bits */

  } packet_info;

  uint8 subframe_t2p[HDRLOG_MAX_RL_SUBPACKETS];
    /* T2P ratio for each subframe (db/4) */

  int8  rri_gain[HDRLOG_MAX_RL_SUBPACKETS];
    /* RRI gain for each subframe (dB/4) */

} log_hdr_reva_rl_pkt_status_type;

    
LOG_RECORD_DEFINE(LOG_HDR_REVA_RL_PKT_STATUS_C)

  uint8  version;
    /* The version number of the packet. */

  uint8  packet_count;
    /* The number of subframes logged in this packet */

  log_hdr_reva_rl_pkt_status_type rl_pkt_status
    [HDRLOG_MAX_ENTRIES_PER_RL_PKT_STATUS_PKT];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_REVA_PCP_PKT_STATS_C 
---------------------------------------------------------------------------*/

#define HDRLOG_REVA_PCP_PKT_STATS_LOG_PKT_VER         0
#define HDRLOG_REVA_PCP_PKT_STATS_MAX_PACKET_RECORDS 10

/* Queue type enumeration of queues with logging support */
typedef enum
{
  HDRLOG_PCP_PKT_STATS_RLP_NEW_Q,
  HDRLOG_PCP_PKT_STATS_RLP_RETX_Q,
  HDRLOG_PCP_PKT_STATS_RLP_FLUSH_Q,
  HDRLOG_PCP_PKT_STATS_SLP_NEW_Q,
  HDRLOG_PCP_PKT_STATS_SLP_RETX_Q,
  HDRLOG_PCP_PKT_STATS_TEST_APP_Q

} hdrlog_reva_pcp_pkt_stats_queue_type_enum;

/* Packet record type */
typedef PACK(struct)
{
  /* 22 bit RLP sequence number or SLP header */
  PACK(union) 
  {
    uint8 rlp_seqno[4];
    uint8 slp_header[4];
  } header;

  /* Packet size including any PCP headers */
  uint16 packet_size;

} hdrlog_reva_pcp_pkt_stats_packet_record_type;

/* Queue record type */
typedef PACK(struct)
{
  /* Queue type of the current queue record */
  uint8 queue_type;

  /* The MAC flow for which this queue is registered */
  uint8 mac_flow_id;

  /* The stream on which the MAC flow transmits */
  uint8 stream_id;

  /* RLP treats the substream id as the RLP flow id */
  uint8 substream_id;

  /* The number of pending bytes on this queue */
  uint16 pending_size;

  /* The number of packets from this queue included in this 
     connection layer packet */
  uint8 num_packets;

  /* The corresponding packet record for each packet included 
     in the connection layer packet */
  hdrlog_reva_pcp_pkt_stats_packet_record_type packets
    [HDRLOG_REVA_PCP_PKT_STATS_MAX_PACKET_RECORDS];
  
} hdrlog_reva_pcp_pkt_stats_queue_record_type;

LOG_RECORD_DEFINE( LOG_HDR_REVA_PCP_PKT_STATS_C )

  /* Version number of the PCP Packet Statistics log packet */
  uint8 version;

  /* Log number mod 256 */
  uint8 log_record_number;

  /* Flagged if the previous connection layer packet of data was canceled */
  boolean previous_packet_canceled;

  /* Flagged if the current connection layer packet contains more packets 
     than the max loggable packet count */
  boolean packet_overflowed;

  /* Flagged TRUE if the current connection layer packet
     is format a; FALSE if the packet is format b */
  boolean format_a_packet;

  /* Maximum packet size alloted by RMAC for this
     connection layer packet */
  uint16 max_packet_size;

  /* Number of queues logged for this connection layer packet */
  uint8 num_queues;

  /* Queue record for each queue with data transmitted in the current
     connection layer packet (variable length field depending on num_queues) */
  hdrlog_reva_pcp_pkt_stats_queue_record_type queues[1];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_REVB_PCP_PKT_STATS_C 
---------------------------------------------------------------------------*/

typedef PACK(struct)
{
  uint16 total_size;
  uint8 header_bytes[4];

} hdrlog_revb_pcp_pkt_stats_queue_data_type;

typedef PACK(struct)
{
  uint8 mac_flow_id;

  hdrlog_revb_pcp_pkt_stats_queue_data_type re_tx_q;
  hdrlog_revb_pcp_pkt_stats_queue_data_type new_tx_q;

} hdrlog_revb_pcp_pkt_stats_flow_data_type;

typedef PACK(struct)
{
  uint8 carrier_id;
  boolean prev_sf_canceled;
  boolean is_fmt_b;

  uint8 num_flows;
  hdrlog_revb_pcp_pkt_stats_flow_data_type tx_flows[1];

} hdrlog_revb_pcp_pkt_stats_carrier_data_type;  

typedef PACK(struct)
{
  uint8 combo_id;
  uint8 num_carriers;
  hdrlog_revb_pcp_pkt_stats_carrier_data_type carriers[1];

} hdrlog_revb_pcp_pkt_stats_combo_data_type;

LOG_RECORD_DEFINE(LOG_HDR_REVB_PCP_PKT_STATS_C)

  uint8 version;
  uint8 log_record_number;
  uint8 sub_frame_count;
  uint8 prev_chosen_combo_id;

  uint8 num_combos;
  hdrlog_revb_pcp_pkt_stats_combo_data_type combos[1];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                     LOG_HDR_SUBTYPE_STREAM_CONFIG_C  
---------------------------------------------------------------------------*/

/* Total number of protocols */
#define HDRLOG_MAX_PROTOCOL_COUNT       HDRHAI_PROTOCOL_TYPE_ENUM_MAX

/* Total number of streams */
#define HDRLOG_MAX_STREAM_COUNT         4

/* Protocol subtype and stream-Apptype mapping information */
typedef PACK(struct)
{
  /* Index of the personality */
  uint8 personality_index;

  /* Subtype of each protocol in the personality */
  uint16 prot_sub[HDRLOG_MAX_PROTOCOL_COUNT];

  /* Apptype mapped to each stream */
  uint16 app_type[HDRLOG_MAX_STREAM_COUNT];

} hdrlog_st_stream_cfg_type;

    
LOG_RECORD_DEFINE(LOG_HDR_SUBTYPE_STREAM_CONFIG_C)

  uint8  version;
    /* The version number of the packet. */

  uint16 current_personality;
    /* AT's current personality index */

  uint8 personality_count;
    /* Number of Personalities */

  hdrlog_st_stream_cfg_type pers[1];
    /* Protocol-subtypes and stream-Apptype mapping information for all the 
       personalities */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_REVA_RUP_TC_LIST_C
---------------------------------------------------------------------------*/
#define HDRLOG_REVA_RUP_TC_LIST_LOG_PKT_VER         0

typedef PACK(struct)
{
  /* The subnet ID of the current sector */
  hdraddr_type subnet_id;

  /* The timestamp (in units of 1.28s) when the trigger code was received */
  uint32 timestamp;

  /* The trigger code given for this sector */
  uint16 trigger_code;

  /* If the RouteUpdate sent in response to the trigger code was ACKED */
  boolean rup_was_acked;

  /* The subnet mask of the current sector */
  uint8 subnet_mask;

} hdrlog_reva_rup_tc_list_entry_type;

LOG_RECORD_DEFINE( LOG_HDR_REVA_RUP_TC_LIST_C )

  /* Log packet version number */
  uint8 version;

  /* Negotiated list size */
  uint8 list_size;

  /* Number of list entries */
  uint8 num_entries;

  /* RouteUpdateTriggerCode list entries */
  hdrlog_reva_rup_tc_list_entry_type entries[1];

LOG_RECORD_END


/*==========================================================================

              MULTI-CARRIER LOG PACKETS DEFINITIONS AND TYPEDEF's 

This section contains common multi-carrier constants and type definitions 
used in Rev B logging module.

==========================================================================*/

/* Maximum number of slots in each frame */
#define HDRLOG_NUM_SLOTS_IN_FRAME       16

/* Maximum number of slots in each sub-frame */
#define HDRLOG_NUM_SLOTS_IN_SUBFRAME    4

/* Number of ASP in each frame */
#define HDRLOG_NUM_ASP                  6

/* Number of slots logged by DSP at a time for slot logging */
#define HDRLOG_NUM_SLOTS_LOGGED_DSP     4

/* Invalid Channel Number */
#define HDRLOG_INVALID_CHANNEL_NUM      0x7FF
/* Invalid band class */
#define HDRLOG_INVALID_BAND_CLASS       0x1F

/* Data type to read slot count to log buffer  */
typedef uint16 hdrlog_slot_time_ref_type;  

typedef PACK(struct)
{
  uint8      channel_id             :4;
    /* Demodulator/RL Carrier Id, Bits 3:0 */

  uint8      reserved_bits          :4;
    /* Reserved bits in carrier id, Bits 7:4 */

  uint16     channel_no             :11;
    /* Channel number, Bits: 10:0 */

  uint16     band_class             :5;
    /* System band class, Bits: 15:11 */

} hdrlog_mc_carrier_id_type;


/*-----------------------------------------------------------------------------
                       LOG_HDR_MC_FL_PHY_PKT_HEADER_C
-----------------------------------------------------------------------------*/

#define LOG_HDR_MC_FL_PHY_PKT_HEADER_VERSION         1

typedef PACK(struct)
{
  hdrlog_slot_time_ref_type     str;
    /* Slot time reference of current incoming packet */

  uint8      data_len;
    /* Packet length in 32 bit multiples */

  uint8      drc_value;
    /* DRC value for the current incoming packet */

  uint8      seq_num;
    /* Sequence number for reordering packet */

  uint8      link_id;
    /* Serving sector link id */

  PACK(struct)
  {
    uint16      slots_to_decode     : 6;
      /* Bits 5:0 Number of slots taken to decode */

    uint16      crc_status          : 2; 
      /* Bits 7:6 Indicates whether the packet has passed or failed CRC.
       * It could be one of the following:
       * 0x00  CRC Failure
       * 0x01  CRC Pass
       * 0x02  CRC False Pass
       * 0x03  Packet Cancelled */

    uint16      crc_is_24_bits      : 1;
      /* Bit 8 Indicates whether CRC is 24 bits or 16bits */

    uint16      pkt_type            :5;
      /*  Bits 13:9 The pkt_type is defined 
       *  0x00  Single-user Traffic Channel Packet
       *  0x08  Multi-user Traffic Channel Packet decoded
       *        at Mac index MUP_PREAMBLE_BASE
       *  0x09  Multi-user Traffic Channel Packet decoded
       *        at Mac index MUP_PREAMBLE_BASE+1 
       *  0x0A  Multi-user Traffic Channel Packet decoded
       *        at Mac index MUP_PREAMBLE_BASE+2 
       *  0x0B  Multi-user Traffic Channel Packet decoded
       *        at Mac index MUP_PREAMBLE_BASE+3
       *  0x0C  Multi-user Traffic Channel Packet decoded
       *        at Mac index MUP_PREAMBLE_BASE+4 
       *  0x10  Release-0 (std.) Control Channel Packet
       *  0x11  Rev-A (short) Control channel packet
       *  0x12  Rev-A Control channel packet received at 
       *        user-defined Short packet MAC index
       *  0x18  Broadcast
       *  0x19  Enhanced BroadCast 
       *  0x1A  Platinum Broadcast */    

    uint16      pd_type             :2;
      /* Bits 15:14 Preamble Detection Type
       * 0x00 Normal Preamble Detection
       * 0x01 CC Forced Preamble Detection 
       * 0x02 TC Forced Preamble Detection */

  } bit_fields;

} hdrlog_mc_fl_pkt_header_info_type;

typedef PACK(struct)
{
      hdrlog_mc_carrier_id_type  carrier_id;
    /* Carrier id of incoming packet that consists of 
     * channel id,channel no and band class */ 

  uint8  rec_cnt;
    /* Number of packet records in log packet */

  hdrlog_mc_fl_pkt_header_info_type fl_pkt_header_info[1];
    /* The array to hold the pkt header records */

} hdrlog_mc_fl_pkt_header_per_carrier_info_type;


LOG_RECORD_DEFINE(LOG_HDR_MC_FL_PHY_PKT_HEADER_C)      

  uint8  version;
    /* The version number of the packet. */

  uint8  carrier_cnt;
    /* The number of carriers */

  hdrlog_mc_fl_pkt_header_per_carrier_info_type 
    per_carrier_pkt_header_info[1];
    /* The array to hold the per carrier records */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                        LOG_HDR_REVB_RMAC4_T2P_RESOURCE_C
---------------------------------------------------------------------------*/
#define HDRLOG_REVB_RMAC4_T2P_RESOURCE_VER  1
  /* Packet version number */

/* Identical to hdrlog_rmac3_flow_info_type */
typedef PACK(struct)
{
  uint16 flow_allotment;
    /* Actual size (in bytes) of this flow */

  uint32 t2p_outflow;
    /* Actual T2P resource withdrawn from this flow's bucket in proportion to
     * its allotment
     */

  int32  bucket_level;
    /* Current bucket level */

  int8   qrab;
    /* Flow specific QRAB, 1: loaded, -1: unloaded */

  uint8  mac_flow_id;
    /* Mac flow ID */

  int32  delta_t2p_inflow;
    /* T2P Inflow increment or decrement for this flow */

  uint32 t2p_inflow;
    /* Average T2P resource added to bucket for this flow */

  uint32 bucket_level_sat;
    /* Saturation level of bucket */

  int32  bucket_factor;
    /* Value of BucketFactor( T2PInflow, FRAB ) function */

  uint8  tt2p_hold;
    /* Lifetime in subframes of the T2P allocation from Grant message */

} hdrlog_rmac4_flow_info_type;

/* Per Channel Log Packet Structure Definition */
typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Carrier ID and Channel Record */

  uint8  pkt_id;
    /* Packet ID of the packet currently being transmitted over the air */

  uint8  hdr_interrupted;
    /* 
     * 0    - Normal HDR mode is active
     * 0x01 - Normal HDR is inactive due to no Finger Lock
     * 0x02 - Normal HDR is inactive due to channel is in DRC Supervision 
     * 0x04 - Normal HDR is inactive due to RLSI
     * 0x08 - Normal HDR is inactive due to RLSI is imminent
     * 0x10 - Normal HDR is inactive due to 1x tune-away is imminent
     * 0x20 - Normal HDR is inactive due to zero max T2P allocation
     * 0x40 & 0x80  - reserved values.
     */

  uint8  pkt_size_index;
    /* Packet size of the packet currently being transmitted over the air */

  uint32 tx_t2p;
    /* T2P of packet currently being transmitted over the air */

  int32                          pilot_strength_ps;
    /* Value of PilotStrength(pilot_strength) function */

  uint16 sum_payload;
    /* Aggregate allotted payload size of eligible flows in current packet */

  int16  frab;
    /* Filtered Reverse Activity Bit (from mDSP */

  hdrlog_rmac4_flow_info_type flow_info[1];
    /* 
     * Variable length record; one per MAC flow; there will be num_flows records
     * of this type.
     */
} hdrlog_rmac4_chan_wide_info_type;

LOG_RECORD_DEFINE(LOG_HDR_REVB_RMAC4_T2P_RESOURCE_C)

  uint8                          version;
    /* The version number of the packet. */

  uint8                          sub_frame_count;
    /* current sub-frame count */

  uint8                          num_carriers;
    /* Number of Open Channels */

  uint8                          num_flows;
    /* Number of Active Flows */

  hdrlog_rmac4_chan_wide_info_type carrier_info[1];
    /* 
     * Variable length record; one per Open Channel; there will be num_carriers
     * records of this type.
     */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_REVB_RMAC4_PKT_INFO_C
---------------------------------------------------------------------------*/
#define HDRLOG_REVB_RMAC4_PKT_INFO_VER  0
  /* Packet version number */

/* Identical to hdrlog_rmac3_pkt_flow_info_type */
typedef PACK(struct)
{
  uint32    initial_queue_size;
    /* initial queue size queried from PCP */

  uint32    pot_t2p_outflow;
    /* potential queue outflow  */

  uint32    q_outflow;
    /* maximum number of bytes allowed to be deducted from bucket */

  uint32    suggested_flow_allocation;
    /* suggested flow allocation as decided by RTC MAC algorithm  */

  uint32    actual_flow_allocation;
    /* actual flow allocation as decided by PCP  */
    
  PACK(struct)
  {
    uint8   mac_flow_id        : 4; 
      /* mac flow id, Bits: 3:0  */
    uint8   non_empty_flow_set : 1; 
      /* whether this flow belongs to non-empty flow set, Bit: 4  */
    uint8   eligible_flow_set  : 1;
      /* whether this flow belongs to eligible flow set, Bit: 5  */
    uint8   reserved           : 2;
      /* Bits: 7:6*/
  } flow_set_info;
  
} hdrlog_rmac4_pkt_flow_info_type;

/* Per Channel Packet Info log packet structure definition */
typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Carrier ID and Channel Record */

  PACK(struct)
  {
    uint8   rtcmac      : 4;
      /* Bits: 3:0 */
    uint8   caput       : 4;
      /* Bits: 7:4 */
  } priority;
    /* Channel Priorities */

  uint16 cand_ps_set;
    /* final candidate packet size set */

  int16 rand_num;
    /* random number - used in t2p_dither_term calculation */

  uint16 tx_t2p_max;
    /* maximum transmitt T2P */

  uint16 round_down_quantum;
    /* smallest unit of data allotted to a flow in the round-down algorithm */

  PACK(struct)
  {
    uint8   final_ps_index     : 4;
      /* final packet size decided by RTC MAC, Bits: 3:0 */
    uint8   tm                 : 1;
      /* transmission mode ( HC/LL ), Bit: 4 */
    uint8   reserved           : 3;
      /* Bits: 7:5 */
  } ps_info;
 
  hdrlog_rmac4_pkt_flow_info_type flow_info[1];
    /* 
     * Variable length record; one per flow; there will be num_flows
     * records of this type.
     */

} hdrlog_rmac4_pkt_chan_wide_info_type;

/* Per Channel Packet Info log packet structure definition */
typedef PACK(struct)
{
  uint8   mac_flow_id;
    /* MAC Flow ID */

  uint16  sum_transmitted_data;
    /* Total number of octets transmitted over all assigned reverse channels */

  uint32  data_bucket_level;
    /* DataBucketLevel, in unit of octets */

} hdrlog_rmac4_pkt_data_flow_info_type;

LOG_RECORD_DEFINE(LOG_HDR_REVB_RMAC4_PKT_INFO_C)

  uint8                          version;
    /* The version number of the packet */

  uint8                          sub_frame_count;
    /* current sub-frame count */

  uint8                          num_carriers;
    /* Number of Open Channels */

  uint8                          num_flows;
    /* Number of Active Flows */

  uint8                          combo_id;
    /* chosen combo ID */

  PACK(struct)
  {
    uint8   slp_carrier_id          : 4;
      /* RL ID of the SLP carrier, Bits: 3:0 */

    uint8   two_iterations_executed : 1;
      /* 
       * TRUE if 2nd iteration of PS determination is execuedt;
       * FALSE otherwise. Bit: 4.
       */

    uint8   reserved                : 3;
      /* Bits: 7:5 */
  } at_ps_info;
    /* AT-wide packet status information */

  hdrlog_rmac4_pkt_data_flow_info_type    flow_info[1];
    /* 
     * Variable length record; one per flow; there will be num_flows
     * records of this type.
     */

  hdrlog_rmac4_pkt_chan_wide_info_type    carrier_info[1];
    /* 
     * Variable length record; one per Open Channel; there will be num_carriers
     * records of this type.
     */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                     LOG_HDR_RMAC_PKT_TIME_INFO_C
---------------------------------------------------------------------------*/
#define HDRLOG_RMAC_PKT_TIME_INFO_VER   0
  /* Packet version number */

/* Definitions used in the "prev_pkt_status" field */
#define HDRLOG_PREV_PKT_ACCEPTED  0
#define HDRLOG_PREV_PKT_CANCELED  1
#define HDRLOG_PREV_PKT_IDLE      2

/* Per channel Packet Time Info log packet structure definition */
typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Carrier ID and Channel Record */

  PACK(struct)
  {
    uint8   num_mac_flows_in_pkt:   4;
      /* Number of MAC flows contained in this packet, Bits: 3:0 */
    uint8   is_format_b:            1;
      /* Bit: 4 */
    uint8   prev_pkt_status:        2;
      /* Accepted, Canceled, or Idle Packet, Bits: 6:5 */
    uint8   reserved_bit1:          1;
      /* Bit: 7 */

    uint8   num_app_pkts_in_pkt:    8;
      /* Number of Application Packets contained in this packet */

    uint16  pkt_size_index:         4;
      /* Bits: 3:0 */
    uint16  data_bytes_len:         11;
      /* Bits: 14:4*/
    uint16  reserved_bit2:          1;
      /* Bit: 15 */
  } pkt_info;

} hdrlog_rmac_pkt_time_chan_wide_info_type;

/* Definitions used in "enc_status" field */
#define HDRLOG_ENC_STATUS_NORMAL      0
#define HDRLOG_ENC_STATUS_SW_TOO_LATE 1

LOG_RECORD_DEFINE(LOG_HDR_RMAC_PKT_TIME_INFO_C)

  uint8                          version;
    /* The version number of the packet */

  uint8                          sub_frame_count;
    /* current sub-frame count */

  uint8                          num_carriers;
    /* Number of Open Channels */

  uint8                          enc_status;
    /*
     * Encoder status - normal.
     *                - SW Too Late.
     */

  uint16                         frame_build_time;
    /* 
     * Time from rmacXdrv_get_prev_tx_status till rmacXdrv_send_packet.
     * In unit of chips.
     */

  uint16                         frame_load_time;
    /* Time spent in rmacXdrv_send_packet.  In unit of chips. */

  uint16                         rpt_pkt_status_time;
    /* Time spent in pcp_report_prev_subframe_status.  In unit of chips. */

  uint16                         get_pkt_info_time;
    /* Time spent in pcp_get_packet_info.  In unit of chips. */

  uint16                         get_pkt_time;
    /* Time spend in pcp_get_packet.  In unit of chips. */

  hdrlog_rmac_pkt_time_chan_wide_info_type  carrier_info[1];
    /* 
     * Variable length record; one per Open Channel; there will be num_carriers
     * records of this type.
     */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                        LOG_HDR_MC_RL_ARQ_STATUS_C
---------------------------------------------------------------------------*/

#define HDRLOG_MC_RL_ARQ_STATUS_VER              0
  /* Version number of this packet */

#define HDRLOG_MAX_SUBFRAMES_PER_REVB_RL_ARQ_PKT 8
  /* Max number of subframes to log per packet */

/* Per subframe RL ARQ status log packet structure definition */
typedef PACK(struct)      /* 5 byte structure */
{
  uint8    subframe_n;
    /* The subframe being logged */

  uint8    packet_id;
    /* The id of the packet sent this subframe */

  PACK(struct)
  {
    uint8     packet_status : 5;
      /* Packet Status report */

    uint8     parq : 1;
      /* P-ARQ Ack or Nack */

    uint8     harq : 2;
      /* H-ARQ Ack, Nack, or neither */
  } arq_status;

  PACK(struct)
  {
    uint16    rri : 4;
      /* RRI being sent in current subframe */

    uint16    sp_n : 2;
      /* subpacket number of current packet in transit */

    uint16    interlace : 2;
      /* interlace */

    uint16    transmission_mode : 1;
      /* Transmission mode: 0 == HiCap, 1 == LoLat */

    uint16    aux_pilot_enabled : 1;
      /* whether aux pilot is enabled */

    uint16    reserved : 6;
      /* Reserved bits */

  } packet_info;
  
} hdrlog_rl_arq_status_subframe_wide_info_type;

/* Per channel RL ARQ status log packet structure definition */
typedef PACK(struct)      /* 8 byte structure */
{
  hdrlog_mc_carrier_id_type                    carrier_id;
    /* Carrier ID and Channel Record */

  hdrlog_rl_arq_status_subframe_wide_info_type subframe_info[1];
    /* Variable length record; one per subframe; there will be subframe_count
       records of this type. */

} hdrlog_rl_arq_status_chan_wide_info_type;

    
LOG_RECORD_DEFINE(LOG_HDR_MC_RL_ARQ_STATUS_C)

  uint8                                    version;
    /* The version number of the packet. */

  uint8                                    subframe_count;
    /* The number of subframes logged in this packet */

  uint8                                    num_carriers;
    /* Number of open channels */

  hdrlog_rl_arq_status_chan_wide_info_type rl_arq_status[1];
    /* Variable length record; one per carrier; there will be num_carriers
       records of this type. */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                         LOG_HDR_MC_RAB_UPDATE_INFO_C
---------------------------------------------------------------------------*/

#define HDRLOG_MC_RAB_UPDATE_INFO_VER                0
  /* Version of RABUpdateInfo log packet */

#define HDRLOG_MAX_SUBFRAMES_PER_REVB_RAB_UPDATE_PKT 8
  /* Max number of subframes to log per packet */

#define HDRLOG_SUBASET_MAX_SIZE                      6
  /* Max number of subactive sets */

typedef PACK(struct)  /* 30 byte structure */
{
  uint8      subframe_n;
    /* The subframe being logged */
  
  uint8      serving_asp;
    /* Serving sector */
  
  uint16     pilot_strength[ HDRLOG_SUBASET_MAX_SIZE ];
    /* Pilot strenght per ASP */

  int16      frab;
    /* FRAB soft decision max at subframe */

  int16      slot_frab_per_asp[ HDRLOG_SUBASET_MAX_SIZE ];
    /* SlotFRAB per ASP */

  PACK(struct) 
  {
    uint16    qrab             : 1;
      /* QRAB hard decision */

    uint16    qrab_ps          : 1;
      /* QRABps hard decision */

    uint16    drc_lock_per_asp : 6;
      /* DRCLock per ASP */

    uint16    slot_qrab_per_asp: 6;
      /* SlotQRAB per ASP */

    uint16    reference_subaset: 1;
      /* Flag to indicate reference subactive set */

    uint16    reserved_bit     : 1;
      /* Reserved bit */

  } bit_info;

} hdrlog_rab_update_subframe_wide_info_type;

/* Per channel RL ARQ status log packet structure definition */
typedef PACK(struct)      /* 33 byte structure */
{
  hdrlog_mc_carrier_id_type                 carrier_id;
    /* Carrier ID and Channel Record */

  hdrlog_rab_update_subframe_wide_info_type subframe_info[1];
    /* Variable length record; one per subframe; there will be subframe_count
       records of this type. */

} hdrlog_rab_update_chan_wide_info_type;

    
LOG_RECORD_DEFINE(LOG_HDR_MC_RAB_UPDATE_INFO_C)

  uint8                                 version;
    /* The version number of the packet. */

  uint8                                 subframe_count;
    /* The number of subframes logged in this packet */

  uint8                                 num_carriers;
    /* Number of open channels */

  hdrlog_rab_update_chan_wide_info_type rab_update_info[1];
    /* Variable length record; one per carrier; there will be num_carriers
       records of this type. */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_MC_RL_METRICS_C
---------------------------------------------------------------------------*/

#define HDRLOG_MC_RL_METRICS_VER          0
  /* Packet version number */

#define HDRLOG_MC_MAX_RL_SUBPACKETS       4
#define HDRLOG_MC_NUM_RL_PAYLOADS        12
#define HDRLOG_MC_NUM_RL_TX_MODES         2
#define HDRLOG_MC_NUM_RL_INTERLACES       3

/* RL rate logging metrics */
typedef PACK(struct)  /* 1743 byte structure */
{
  hdrlog_mc_carrier_id_type                 carrier_id;
    /* Carrier ID and Channel Record */

  PACK(struct)
  {
    PACK(struct)
    {
      PACK(struct)
      { 
        uint32 acks[HDRLOG_MC_MAX_RL_SUBPACKETS];
          /* Number of received/interpreted HARQ ACKs */

        uint32 parq_naks;
          /* Number of received PARQ NAKs */

        uint32 total_sent;
          /* Total RL packets accepted by the encoder */

      } rri[HDRLOG_MC_NUM_RL_PAYLOADS]; 

    } tm[HDRLOG_MC_NUM_RL_TX_MODES];

    uint32 unexpected_parq_naks;
      /* Total number of unexpected PARQ NAKs */

  } interlace[HDRLOG_MC_NUM_RL_INTERLACES];

} hdrlog_rl_metrics_chan_wide_info_type;


LOG_RECORD_DEFINE(LOG_HDR_MC_RL_METRICS_C)

  uint8                                 version;
    /* The version number of the packet */

  uint32                                log_seq_num;
    /* Log sequence number */

  uint8                                 num_carriers;
    /* Number of open channels */

  hdrlog_rl_metrics_chan_wide_info_type mc_rl_pkt_metrics;
    /* Per-carrier RL metrics */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                       LOG_HDR_MC_RL_PKT_STATUS_C
---------------------------------------------------------------------------*/

/* Version number of this packet */
#define HDRLOG_MC_RL_PKT_STATUS_VER                  0
  
/* Maximum number of packets to log per log packet */
#define HDRLOG_MAX_ENTRIES_PER_MC_RL_PKT_STATUS_PKT  12

typedef PACK(struct)      /* 13 byte structure */
{
  uint8    packet_id;
    /* The id of the packet */

  uint16   data_bytes_sent;
    /* Number of non-padding bytes */

  PACK(struct)
  {
    uint16    rri : 4;
      /* Bits 3:0 PacketSizeIndex (RRI) of packet */

    uint16    subpackets : 2;
      /* Bits 5:4 Number of subpackets transmitted */

    uint16    interlace : 2;
      /* Bits 7:6 Interlace */

    uint16    transmission_mode : 1;
      /* Bit 8 Transmission mode: 0 == HiCap, 1 == LoLat */

    uint16    aux_pilot_enabled : 1;
      /* Bit 9 Whether aux pilot was enabled for this packet */

    uint16    parq_nak_received : 1;
      /* Bit 10 Whether entire packet was NAK'd */

    uint16    reserved : 5;
      /* Bits 15:11 Reserved bits */

  } packet_info;

  uint8 subframe_t2p[HDRLOG_MAX_RL_SUBPACKETS];
    /* T2P ratio for each subframe (db/4) */

  int8  rri_gain[HDRLOG_MAX_RL_SUBPACKETS];
    /* RRI gain for each subframe (dB/4) */

} hdrlog_rl_pkt_status_packet_wide_info_type;

typedef PACK(struct)      /* 17 byte structure */
{
  hdrlog_mc_carrier_id_type  carrier_id;
    /* Carrier id of incoming packet that consists of 
     * channel id,channel no and band class */

  uint8  packet_count;
    /* The number of packets logged in this packet */

  hdrlog_rl_pkt_status_packet_wide_info_type pkt_info[1];
    /* The array to hold the pkt header records */

} hdrlog_rl_pkt_status_chan_wide_info_type;


LOG_RECORD_DEFINE(LOG_HDR_MC_RL_PKT_STATUS_C)      

  uint8 version;
    /* The version number of the packet. */  

  uint8 num_carriers;
    /* Number of logged channels */

  hdrlog_rl_pkt_status_chan_wide_info_type rl_pkt_status[1];
    /* The array to hold the per carrier records */
  
LOG_RECORD_END


/*---------------------------------------------------------------------------
                            LOG_HDR_MC_RL_GAINS_C
---------------------------------------------------------------------------*/

#define HDRLOG_MC_RL_GAINS_VER                   1
  /* Packet version number */

#define HDRLOG_MAX_SUBFRAMES_PER_MC_RL_GAINS_PKT 8
  /* Version number for packet */

typedef PACK(struct)      /* 8 byte structure */
{

  int16 tx_open_loop;
    /* Tx Open Loop */

  int16 tx_total;
    /* Total Tx AGC */

  int16 tx_pilot;
    /* Tx Pilot power */

  int16 tx_adjust;
    /* Closed Loop Adjust */

} hdrlog_tx_agc_struct_type;


typedef PACK(struct)      /* 27 byte structure */
{
  uint8    subframe_n;
    /* The subframe being logged */

  PACK(struct)
  {
    uint16    rri : 4;
      /* RRI being sent in current subframe */

    uint16    sp_n : 2;
      /* subpacket number of current packet in transit */

    uint16    transmission_mode : 1;
      /* Transmission mode: 
         0 == HiCap,
         1 == LoLat */

    uint16    carrier_enabled : 1;
      /* 1 == Carrier enabled,
         0 == Carrier disabled */

    uint16    rpc : 2;
      /* RPC bit received for this subframe. 
         0 == Hold,
         1 == Up,
         2 == Down. */

    uint16    reserved : 6;
      /* Reserved bits */

  } rl_info;
  
  uint16 pilot_hs0;
    /* Linear pilot power in halfslot 0 of subframe */

  uint16 ack_hs0;
    /* Linear ACK channel power in halfslot 0 of subframe */
  
  uint16 pilot_hs1;
    /* Linear pilot power in halfslot 1 of subframe */

  uint16 rri_hs1;
    /* Linear RRI channel power in halfslot 1 of subframe */

  uint16 dsc_hs1;
    /* Linear DSC channel power in halfslot 1 of subframe */

  uint16 drc_hs1;
    /* Linear DRC channel power in halfslot 1 of subframe */

  uint16 data_hs1;
    /* Linear data channel power in halfslot 1 of subframe */

  uint16 aux_hs1;
    /* Linear auxiliary pilot gain in halfslot 1 of subframe */

  uint16 betap_hs1;
    /* BetaP gain value for this channel. Sampled in halfslot 1. */

  hdrlog_tx_agc_struct_type tx_agc;
    /* Tx AGC info */

} hdrlog_rl_gains_per_chan_subframe_wide_info_type;


/* Per channel RL Gains log packet structure definition */
typedef PACK(struct)  /* 30 byte structure */
{
  hdrlog_mc_carrier_id_type                         carrier_id;
    /* Carrier ID and Channel Record */

  hdrlog_rl_gains_per_chan_subframe_wide_info_type  subframe_info[1];
    /* Variable length record; one per subframe; there will be subframe_count
       records of this type. */

} hdrlog_rl_gains_chan_wide_info_type;


typedef PACK(struct)  /* 7 byte structure */
{
  uint8    subframe_n;
    /* The subframe being logged */

  PACK(struct)
  {

    uint8    pa_enabled   : 1;
      /* Power amplifier enabled flag */

    uint8    tx_throttle  : 1;
      /* Indicates whether Tx RL gains are being throttled */

    uint8    tx_min_power : 1;
      /* Indicates whether Tx AGC is at min power */

    uint8    tx_max_power : 1;
      /* Indicates whether Tx AGC is at max power */

    uint8    reserved     : 4;

  } rl_common_info;

  uint8 backoff;
    /* Backoff used */

  int16 total_tx_power;
    /* Total transmit power over all carriers */

  int16 max_avail_tx_power;
    /* Max available transmit power; Threshold of current Tx power 
       to start throttling the DRC/ACK and ratcheting power up command */

} hdrlog_rl_gains_pwr_info_type;


LOG_RECORD_DEFINE(LOG_HDR_MC_RL_GAINS_C)

  uint8  version;
    /* The version number of the packet. */

  uint8  subframe_count;
    /* The number of subframes logged in this packet */

  uint8  num_carriers;
    /* Number of open channels */

  hdrlog_rl_gains_pwr_info_type        power_info[1];
    /* Per-subframe power information. 
       Variable length record; one per subframe; there will be subframe_count
       records of this type. */

  hdrlog_rl_gains_chan_wide_info_type  rl_gains_info[1];
    /* Per-carrier rl gain.
       Variable length record; one per carrier; there will be num_carriers
       records of this type. */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                   LOG_HDR_MC_PA_HDRM_AND_CARRIER_MGMT_C
---------------------------------------------------------------------------*/

/* Version number of this packet */
#define HDRLOG_MC_PA_HDRM_AND_CARRIER_MGMT_VER       3
  
/* Maximum number of subframes per log packet */
#define HDRLOG_MAX_SUBFRAMES_PER_MC_PA_HDRM_CM_PKT   8

#define HDRLOG_MAX_NUM_RL_CARRIERS                   3

typedef PACK(struct)      /* 26 byte structure */
{
  PACK(struct)
  {
    uint16  channel_id     : 4;
      /* RL Carrier ID */

    uint16  carrier_state  : 6;
      /* Carrier State */

    uint16  data_allowed   : 1;
      /* Data allowed flag.
         FALSE => data transmission disabled due to one of the following:
         DRC supervision timeout, carrier in inactive, to-be-deactivated or
         to-be-dropped states. */

    uint16  is_slp_carrier : 1;
      /* SLP carrier flag.
         TRUE  => the carrier is the current SLP carrier
         FALSE => otherwise */

    uint16 reserved        : 4;
      /* Reserved bits */

  } bit_info;

  int16  inst_txp;
    /* Instantaneous transmit pilot power read from DSP */

  int16  peak_txp;
    /* Peak transmit pilot power read from DSP */

  int16  filt_txp;
    /* Filtered transmit pilot power read from DSP */

  int16  pwr_margin;
    /* Power margin read from DSP */

  uint32  filt_pwr_margin;
    /* Filtered power margin in Q22.10 linear units */

  uint32 filt_rab_limited_p_data;
    /* Filtered RAB limited power */

  uint32 txt2p_max_ps;
    /* TxT2PMax(PS) */

  uint32 txt2p_max;
    /* Headroom */

} hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_chan_wide_info_type;

typedef PACK(struct)      /* 7 byte structure */
{
  uint8    subframe_n;
    /* The subframe being logged */

  int16    p_max;
    /* Total available Tx power */

  int16    p_max_neq;
    /* max transmit power when AT in funnel mode */

  boolean  in_funnel_mode;
    /* AT funnel mode state */

  uint8    carrier_config;
    /* Current carrier configuration */

  uint8    target_carrier_config;
    /* Target carrier configuration */

  PACK(struct)
  {
    /* RTCMAC prioritized carriers */
    uint16 rtcmac_pri_carrier0  : 2;
    uint16 rtcmac_pri_carrier1  : 2;
    uint16 rtcmac_pri_carrier2  : 2;

    /* Overall prioritized carriers */
    uint16 overall_pri_carrier0 : 2;
    uint16 overall_pri_carrier1 : 2;
    uint16 overall_pri_carrier2 : 2;

    uint16 pa_hdrm_run_decision : 1;
      /* PA headroom run decision */

    uint16 cm_run_decision      : 1;
      /* Carrier management run decision */

    uint16 slp_has_data         : 1;
      /* Flag to indicate that the current SLP carrier has 
         data in queue and so cannot be switched */

    uint16 reserved             : 1;
      /* Reserved bits */

  } priority_and_run_info;

} hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_info_type;

typedef PACK(struct)      /* 28 byte structure */
{
  hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_info_type
    sf_info;

  hdrlog_rl_hdrm_and_carrier_mgmt_per_sf_chan_wide_info_type 
    carrier_info[1];
    /* The array to hold the per carrier records */

} hdrlog_rl_hdrm_and_carrier_mgmt_sf_wide_info_type;


typedef PACK(struct)      /* 4 byte structure */
{
  hdrlog_mc_carrier_id_type  carrier_id;
    /* Carrier id, channel number and band class information of
       all carriers */

  PACK(struct)
  {
    uint8  dropping_rank  : 6;
      /* Carrier dropping rank */

    uint8  reserved       : 2;
      /* Reserved bits */

  } ch_info;

} hdrlog_rl_hdrm_and_carrier_mgmt_chan_wide_info_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_PA_HDRM_AND_CARRIER_MGMT_C)

  uint8 version;
    /* The version number of the packet. */  

  uint8  subframe_count;
    /* The number of subframes logged in this packet */

  uint8  num_carriers;
    /* Number of open channels */

  hdrlog_rl_hdrm_and_carrier_mgmt_chan_wide_info_type 
    rl_hdrm_cm_chan_info[HDRLOG_MAX_NUM_RL_CARRIERS];
    /* The array to hold the per carrier records */

  hdrlog_rl_hdrm_and_carrier_mgmt_sf_wide_info_type 
    rl_hdrm_cm_sf_info[1];
    /* The array to hold the per subframe records */
  
LOG_RECORD_END


/*---------------------------------------------------------------------------
                          LOG_HDR_MC_SUP_FWD_STATS_C
---------------------------------------------------------------------------*/

#ifdef FEATURE_HDR_REVB_ENH_RATES

/* Version number of the log packet */
#define HDRLOG_MC_SUP_FWD_STATS_VERSION           1

#else

/* Version number of the log packet */
#define HDRLOG_MC_SUP_FWD_STATS_VERSION           0

#endif /* FEATURE_HDR_REVB_ENH_RATES */

#ifdef FEATURE_HDR_REVB_ENH_RATES
typedef PACK(struct)
{
  uint32 good_drc16_1024bits; /* 42. DRC 16 - [1024, 4, 64] */
  uint32 good_drc16_2048bits; /* 43. DRC 16 - [2048, 4, 64] */
  uint32 good_drc16_3072bits; /* 44. DRC 16 - [3072, 4, 64] */
  uint32 bad_drc16;           /* Number of CRC failed DRC 16 pkts */

  uint32 good_drc17_1024bits; /* 45. DRC 17 - [1024, 4, 64] */
  uint32 good_drc17_2048bits; /* 46. DRC 17 - [2048, 4, 64] */
  uint32 good_drc17_4096bits; /* 47. DRC 17 - [4096, 4, 64] */
  uint32 bad_drc17;           /* Number of CRC failed DRC 17 pkts */

  uint32 good_drc18_1024bits; /* 48. DRC 18 - [1024, 4, 64] */
  uint32 good_drc18_2048bits; /* 49. DRC 18 - [2048, 4, 64] */
  uint32 good_drc18_5120bits; /* 50. DRC 18 - [5120, 4, 64] */
  uint32 bad_drc18;           /* Number of CRC failed DRC 18 pkts */

  uint32 good_drc19_2048bits; /* 51. DRC 19 - [2048, 4, 64] */
  uint32 good_drc19_6144bits; /* 52. DRC 19 - [6144, 4, 64] */
  uint32 bad_drc19;           /* Number of CRC failed DRC 19 pkts */

  uint32 good_drc20_1024bits; /* 53. DRC 20 - [1024, 4, 64] */
  uint32 good_drc20_7168bits; /* 54. DRC 20 - [7168, 4, 64] */
  uint32 bad_drc20;           /* Number of CRC failed DRC 20 pkts */

  uint32 good_drc21_8192bits; /* 55. DRC 21 - [8192, 4, 64] */
  uint32 bad_drc21;           /* Number of CRC failed DRC 21 pkts */

  uint32 good_drc22_2048bits; /* 56. DRC 22 - [2048, 2, 64] */
  uint32 good_drc22_6144bits; /* 57. DRC 22 - [6144, 2, 64] */
  uint32 bad_drc22;           /* Number of CRC failed DRC 22 pkts */

  uint32 good_drc23_1024bits; /* 58. DRC 23 - [1024, 2, 64] */
  uint32 good_drc23_7168bits; /* 59. DRC 23 - [7168, 2, 64] */
  uint32 bad_drc23;           /* Number of CRC failed DRC 23 pkts */

  uint32 good_drc24_8192bits; /* 60. DRC 24 - [8192, 2, 64] */
  uint32 bad_drc24;           /* Number of CRC failed DRC 24 pkts */

  uint32 good_drc25_2048bits; /* 61. DRC 25 - [2048, 1, 64] */
  uint32 good_drc25_6144bits; /* 62. DRC 25 - [6144, 1, 64] */
  uint32 bad_drc25;           /* Number of CRC failed DRC 25 pkts */

  uint32 good_drc26_1024bits; /* 63. DRC 26 - [1024, 1, 64] */
  uint32 good_drc26_7168bits; /* 64. DRC 26 - [7168, 1, 64] */
  uint32 bad_drc26;           /* Number of CRC failed DRC 26 pkts */

  uint32 good_drc27_8192bits; /* 65. DRC 27 - [8192, 1, 64] */
  uint32 bad_drc27;           /* Number of CRC failed DRC 27 pkts */

} hdrlog_rev_b_enhanced_tc_rates_crc_stats_type;

typedef PACK(struct)
{
  uint32 drc16_1024bits[4];   /* 42. DRC 16 - [1024, 4, 64] */
  uint32 drc16_2048bits[4];   /* 43. DRC 16 - [2048, 4, 64] */ 
  uint32 drc16_3072bits[4];   /* 44. DRC 16 - [3072, 4, 64] */

  uint32 drc17_1024bits[4];   /* 45. DRC 17 - [1024, 4, 64] */
  uint32 drc17_2048bits[4];   /* 46. DRC 17 - [2048, 4, 64] */
  uint32 drc17_4096bits[4];   /* 47. DRC 17 - [4096, 4, 64] */

  uint32 drc18_1024bits[4];   /* 48. DRC 18 - [1024, 4, 64] */
  uint32 drc18_2048bits[4];   /* 49. DRC 18 - [2048, 4, 64] */
  uint32 drc18_5120bits[4];   /* 50. DRC 18 - [5120, 4, 64] */

  uint32 drc19_2048bits[4];   /* 51. DRC 19 - [2048, 4, 64] */
  uint32 drc19_6144bits[4];   /* 52. DRC 19 - [6144, 4, 64] */

  uint32 drc20_1024bits[4];   /* 53. DRC 20 - [1024, 4, 64] */
  uint32 drc20_7168bits[4];   /* 54. DRC 20 - [7168, 4, 64] */

  uint32 drc21_8192bits[4];   /* 55. DRC 21 - [8192, 4, 64] */

  uint32 drc22_2048bits[2];   /* 56. DRC 22 - [2048, 2, 64] */
  uint32 drc22_6144bits[2];   /* 57. DRC 22 - [6144, 2, 64] */

  uint32 drc23_1024bits[2];   /* 58. DRC 23 - [1024, 2, 64] */
  uint32 drc23_7168bits[2];   /* 59. DRC 23 - [7168, 2, 64] */

  uint32 drc24_8192bits[2];   /* 60. DRC 24 - [8192, 2, 64] */

  uint32 drc25_2048bits[1];   /* 61. DRC 25 - [2048, 1, 64] */
  uint32 drc25_6144bits[1];   /* 62. DRC 25 - [6144, 1, 64] */

  uint32 drc26_1024bits[1];   /* 63. DRC 26 - [1024, 1, 64] */
  uint32 drc26_7168bits[1];   /* 64. DRC 26 - [7168, 1, 64] */

  uint32 drc27_8192bits[1];   /* 65. DRC 27 - [8192, 1, 64] */

} hdrlog_rev_b_enhanced_tc_rates_early_decode_type;

#endif /* FEATURE_HDR_REVB_ENH_RATES */

typedef PACK(struct)
{
  hdrlog_rev_a_short_cc_crc_stats_type short_crc;
    /* Control channel CRC stats for short packets */

  hdrlog_cc_crc_stats_type std_crc;
    /* Control channel CRC stats for standard sizes */

  hdrlog_rev_a_short_cc_early_decode_type short_early_decode;
    /* Control channel early decode stats for short packets */

  hdrlog_cc_early_decode_type std_early_decode;
    /* Control channel early decode stats for standard packets */

} hdrlog_cc_stats_type;

typedef PACK(struct)
{
  hdrlog_rev_a_tc_crc_stats_type crc;
    /* Traffic Channel CRC Statistics */

#ifdef FEATURE_HDR_REVB_ENH_RATES
  hdrlog_rev_b_enhanced_tc_rates_crc_stats_type enh_crc;
    /* Traffic Channel CRC statistics for Enhanced Rev-B DRCs */
#endif /* FEATURE_HDR_REVB_ENH_RATES */

  hdrlog_rev_a_tc_early_decode_type early_decode;
    /* Traffic Channel early decode statistics */

#ifdef FEATURE_HDR_REVB_ENH_RATES
  hdrlog_rev_b_enhanced_tc_rates_early_decode_type enh_early_decode;
    /* Traffic Channel early decode statistics for Enhanced Rev-B DRCs */
#endif /* FEATURE_HDR_REVB_ENH_RATES */

} hdrlog_tc_stats_type;

/* Per-carrier forward statistics type */
typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Carrier ID */

  hdrlog_tc_stats_type tc;
    /* Traffic Channel Statistics */

  boolean cc_stats_included;
    /* Does this log packet include control channel statistics */

  /* The following field in this struct is optional. It is included
     if cc_stats_included field is set to true and skipped otherwise */

  hdrlog_cc_stats_type cc;
    /* Control channel stats */

} hdrlog_perc_fwd_stats_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_SUP_FWD_STATS_C)

  uint8 version;
    /* The version number of the packet */

  uint32 log_seq_num;
    /* Log packet sequence number */

  uint8 num_carriers;
    /* Number of carriers containing the traffic channel statistics. Please
       note that the fwd statistics for each carrier is logged in separate
       log packet. This field is only for informational purposes  */

  hdrlog_perc_fwd_stats_type perc_fwd_stats;
    /* Forward channel statistics (Both tc & cc) for one carrier */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_MC_FINGER_DATA_C
---------------------------------------------------------------------------*/

/* Packet version number */
#define HDRLOG_MC_FINGER_DATA_VER   1  

typedef PACK(struct)
{
  uint16   pilot_pn; 
    /* PN offset the finger is demodulating */

  uint32   rtc_pos_x8; 
    /* RTC offset position of fing in chipx8 */

  uint16   rssi;            
    /* C/I equivalent of the finger combined for both antennaes */

  PACK(struct)
  {
    uint8    locked         : 1;  
      /* Bit 0 :: Locked state of this finger 1: locked, 0: unlocked */

    uint8    antenna        : 1;  
      /* Bit 1 :: Antenna selection 0 : antenna 0, 1 : Antenna1 */

    uint8    diversity      : 1;  
      /* Bit 2 :: Indicates whether diversity is enabled for AT
       * 0: diversity disabled 1:diversity enabled */

    uint8    reserved_bits  : 1;  
      /* Bit 3 :: Reserved bit */

    uint8    fing_index     : 4;  
      /* Bits 7:4 :: Index of the demodulator finger [0-11] */

  } fing_info;

  PACK(struct)
  {
    uint8    rpc_idx        : 3;  
      /* Bits 2:0 :: Reverse Power Control combiner cell index */

    uint8    reserved_bits1 : 1;   
      /* Bits 3 :: Reserved Bits */

    uint8    asp_idx        : 3;  
      /* Bits 6:4 :: Active set pilot to which finger belongs */

    uint8    reserved_bits2 : 1;   
      /* Bits 7 :: Reserved Bits */

  } fing_sect;

  /* Meaningful when diversity is enabled */
  uint16     ant0_rssi;       
    /* C/I equivalent of the finger for antenna 0 */

  uint16     ant1_rssi;       
    /* C/I equivalent of the finger for antenna 1 */

  int32      ant0_phase;
    /* Phase info of the finger for antenna 0 */

  int32      ant1_phase;
    /* Phase info of the finger for antenna 1 */
} hdrlog_per_finger_data_type;

typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Channel id */ 

  uint8    sub_aset_id;
    /* Id of active set pilot */

  uint8    num_fingers; 
    /* Number of fingers [0-7] */

  hdrlog_per_finger_data_type finger_data[1];
    /* The array to hold the finger records */

} hdrlog_per_carrier_finger_data_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_FINGER_DATA_C)

  uint8    version;          
    /* The version number of the packet. */

  uint8    srch_state;       
    /* State of the hdrsrch task */

  uint32   mstr_x8;          
    /* Mobile Station time reference */

  uint16   mstr_err_x8;      
    /* MSTR with respect to the earliest arriving finger */

  uint16   mstr_pn;          
    /* Pilot PN of the finger, which the MSTR is tracking typically
     * the earliest arriving finger */
  
  uint8    num_carriers;
    /* Number of carriers [0-2] in the log packet */
  
  hdrlog_per_carrier_finger_data_type  perc_finger_data[1];
    /* This is a variable length array to hold the per carrier records */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_MC_SEARCH_DATA_C
---------------------------------------------------------------------------*/

/* Maximum active set pilots that can be logged in one packet */
#define HDR_LOG_MC_MAX_SEARCH_PILOTS    6

/* Packet version number */
#define HDRLOG_MC_SEARCH_DATA_VER   0  

/* Per-pilot Search Data Type */
typedef PACK(struct)
{
  /* Pilot pn related information */
  PACK(struct)
  {
    uint16   pn            : 9;
      /* Bits 8:0 :: PN Offset */

    uint16   reserved_bit  : 1;
      /* Bit 9 :: Reserved */
     
    uint16   ant_sel       : 2;
      /* Bits 11:10 :: Antenna Select */

    uint16   pilot_set     : 3;
      /* Bits 14:12 :: Pilot Set Type */

    uint16   reserved_bit1 : 1;
      /* Bit 15 :: Reserved */

  } pilot_pn;                          
  
  uint16  win_center_x2;
    /* Position of window center in chipx2 units */

  uint16  earliest_pos_x2;
    /* Position of the earliest peak in chipx2 units */

  PACK(struct)
  {
    int16   peak_pos[HDR_LOG_MC_MAX_SEARCH_PILOTS];
      /* MSTR compensated position of peaks in chipx2 units */

    uint16  peak_eng[HDR_LOG_MC_MAX_SEARCH_PILOTS];         
      /* Energy of peaks */

  } peak_info;

  /* Center of mass related information */
  PACK(struct)
  {
    uint8   peaks_used;
      /* Peaks used for center of mass computation */

    int8    peak_eng_thresh;
      /* Energy threshold for peaks considered for center of mass 
       * computation */

    int16   com_posx2;
      /* Center of mass RTC offset in chipx2 units */

  } com_info;

} hdrlog_per_pilot_search_data_type;

/* Per-carrier Search data type */
typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Forward link channel ID */  

  uint8 best_asp_idx;       
    /* Best active set pilot index */

  uint8 num_pilots;         
    /* Number of pilots per frame */

  hdrlog_per_pilot_search_data_type  pilots[1];
    /* This is a variable length field with num_pilots occurences */

} hdrlog_mc_per_carrier_search_data_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_SEARCH_DATA_C)

  uint8    version;
    /* The version number of the packet. */

  uint16  slot_count;
    /* Slot count of log packet */

  uint8   time_to_search;
    /* Time to complete search(in units of slow clocks)*/

  uint8    srch_state;
    /* Searcher state{0 : Start, 1 : Acq, 2 : Sync, 3 : Idle, 4 : connected} */

  uint8    srch_substate;
    /* Searcher substate. Value depends on state */

  uint16   srch_win;
    /* Search window size in chipx1 */

  PACK(struct)
  {
    uint32 mstrx8       :   18;
      /* Bits 17:0 :: MSTR in chipx8 units */

    uint32 reserved     :   14;
      /* Bits 31:18 :: Reserved Bits */

  } srch_info;  

  PACK(struct)
  {
    uint8   enabled        :    1;
      /* Bit 0 :: Flag to indicate if equalizer is on */

    uint8   reserved       :    7;
      /* Bits 7:1 :: Reserved Bits */
  } eq;

  uint8   num_carriers;
    /* Number of carriers in a log packet */
  
  hdrlog_mc_per_carrier_search_data_type carriers[1];
    /* This is a variable length field with num_carriers occurences */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                           LOG_HDR_MC_DRC_ARQ_DSC_BUFFER_C
---------------------------------------------------------------------------*/

#define HDRLOG_MC_DRC_ARQ_DSC_BUFFER_VER    0
  /* Version number of the multicarrier DRC-ARQ-DSC buffer log packet */

typedef PACK(struct)
{
  /* Please note that the fields in the log packet are bit-packed in a 
     similar manner to mDSP logging buffer. So copying the rl & asp field 
     directly is more efficient */

  uint16 rl;      
    /* RL field that packs several reverse link related fields specified 
       below
    PACK(struct)
    {
      uint16 drc_cover    : 3;  Bit2 : Bit0  :: DRC Cover
      uint16 reserved1    : 1;  Bit3         :: Reserved
      uint16 dsc_value    : 3;  Bit6 : Bit4  :: DSC Value
      uint16 mup_ack_nack : 1;  Bit7         :: Multi-user ACK/NAK
      uint16 reserevd2    : 2;  Bit9 : Bit8  :: Reserved
      uint16 dsc_boost    : 1;  Bit10        :: DSC Gain Boost
      uint16 drc_boost    : 1;  Bit11        :: DRC Gain Boost
      uint16 ack_nack     : 1;  Bit12        :: ACK/NACK
      uint16 ack_enabled  : 1;  Bit13        :: Ack Channel enabled
      uint16 forced_ack   : 1;  Bit14        :: Forced ACK
      uint16 reserved3    : 1;  Bit15        :: Reserved
    } rl; */

  uint8 trans_drc; /* Transmitted DRC value */

  uint8 asp;
  /* 
    ASP field that packs demod ASP and next ASP
    PACK(struct)
    {
      uint8 next      : 3; Bit2 : Bit0 :: ASP on which we are req. service
      uint8 reserved1 : 1; Bit3        :: Reserved
      uint8 demod     : 3; Bit6 : Bit4 :: ASP used for Demodulation
      uint8 reserved2 : 1; Bit7        :: Reserved
    } asp; */

} hdrlog_per_slot_channel_info_type;

typedef PACK(struct)
{
  /* This packet has been packed in the same format as the data
     read from the DSP so that DMSS does not have to waste precious
     cycles in unpacking and repacking the data */

  hdrlog_mc_carrier_id_type carrier_id;
    /* Carrier id of each carrier */

  uint16 pn;
    /* PN value of each carrier */

  hdrlog_per_slot_channel_info_type slot_info [HDRLOG_NUM_SLOTS_IN_FRAME];
    /* Per-slot information for every slot in a frame */

  uint16 drc_lock;
    /* -------------------------------------
       Bit 0 - DRC lock for ASP1
       Bit 1 - DRC lock valid bit for ASP1
       Bit 2 - DRC lock for ASP2
       Bit 3 - DRC lock valid bit for ASP2
       ...
       Bit 10 - DRC lock for ASP6
       Bit 11 - DRC lock valid bit for ASP6
       ------------------------------------
    */

  uint8 drc_lock_update;
    /* Last slot in which the DRC lock was updated */

} hdrlog_per_carrier_drc_arq_dsc_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_DRC_ARQ_DSC_BUFFER_C)

  uint8 version;
    /* The version number of the packet. */

  uint8 num_carriers;
    /* Number of carriers per log packet */

  uint8 slot_count;
    /* Slot count of first slot in the log buffer */

  hdrlog_per_carrier_drc_arq_dsc_type per_carrier[1];
    /* This is a variable length field with num_carriers occurences */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                           LOG_HDR_MC_LINK_SUMMARY_C
---------------------------------------------------------------------------*/
/* Version number of the log packet */
#define HDRLOG_MC_LINK_SUMMARY_VER        1

/* Per-slot link summary information */
typedef PACK(struct)
{

  uint8 pred_drc;
    /* Predicted DRC */

  uint32 basp_sinr;
    /* Best ASP SINR */

} hdrlog_per_slot_link_summary_type;

/* Per ASP Link Summary information */
typedef PACK(struct)
{
  uint16 asp_filt_sinr;
    /* ASP's Filtered SINR */

  uint16 pilot_pn_asp;
    /* ASP's Pilot PN */

  int16 rpc;
    /* ASP's Filtered RPC value */

  uint8 rpc_cell_idx;
    /* ASP's RPC Cell Index */

} hdrlog_per_asp_link_summary_type;

/* Per carrier link summary information */
typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Carrier ID */

  hdrlog_per_slot_link_summary_type per_slot[HDRLOG_NUM_SLOTS_IN_FRAME];
    /* Per-slot data for the current frame starting from "slot_count" 
       time reference */

  hdrlog_per_asp_link_summary_type per_asp[HDRLOG_NUM_ASP];
    /* Per ASP data */

  PACK(struct)
  {
    uint8 basp_pred               : 4;
      /* Best ASP Predicted */

    uint8 last_basp_changed_slot  : 4;
      /* Last Best ASP Changed slot */
  } basp_info;

  /* RCL Predictor outer-loop Adjust */
  int32 rcl_pred_adj;

  uint16 num_bad_pkts;
    /* Number of bad packets out of the last 16 packets received:
       Used to compute Instantaneous packet error rate; */

} hdrlog_per_carrier_link_summary_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_LINK_SUMMARY_C)

  uint8 version;
    /* The version number of the packet. */

  uint8 num_carriers;
    /* Number of carriers per log packet */

  uint8 str;
    /* Slot time reference of first slot in the log buffer */

  uint8 ref_sub_aset_locked_mode;
    /* Reference Sub-ASET in locked mode */

  hdrlog_per_carrier_link_summary_type per_carrier[1];
    /* This is a variable length field with num_carriers occurences */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                           LOG_HDR_MC_EQUALIZER_CI_BUFFER_C
---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_GRICE
#define HDRLOG_MC_EQUALIZER_CI_BUFFER_VERSION     3
#else
#define HDRLOG_MC_EQUALIZER_CI_BUFFER_VERSION     1
#endif

typedef PACK(struct)
{
  uint32 eq_status;
    /* Equalizer information bits */ 

  uint32 eq_best_ci;
    /* C/I of the best equalizer in each slot */

  uint16 eq_alt_rake_ci;
    /* Alternate Rake C/I in each slot */

#ifdef FEATURE_HDR_GRICE
  uint16 g_asp;
    /* ASPs for gRice tracking set (G-set) */
#endif

} hdrlog_per_slot_eq_ci_buffer_type;

typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Carrier ID */

  hdrlog_per_slot_eq_ci_buffer_type eq_data[HDRLOG_NUM_SLOTS_IN_FRAME];
    /* Per-slot equalizer data for this carrier */

} hdrlog_per_carrier_eq_ci_buffer_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_EQUALIZER_CI_BUFFER_C)

  uint8  version;
    /* The version number of the packet. */

  uint8 num_carriers;
    /* Number of Demod carriers */

  uint16 str;
    /* Slot time reference */

#ifdef FEATURE_HDR_GRICE
  uint16 eq_mode;
    /* Mode of eq - 1 -> gRICE enabled. */
#endif 

  hdrlog_per_carrier_eq_ci_buffer_type per_carrier[1];
 
LOG_RECORD_END

/*---------------------------------------------------------------------------
                              LOG_HDR_MC_POWER_C
---------------------------------------------------------------------------*/

/* Version number of this log packet */
#define HDRLOG_MC_POWER_VER    1

typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type  carrier_id;
    /* Carrier id of incoming packet that consists of 
     * channel id,channel no and band class */

  PACK(struct)
  {
    uint8  ratchet_mode     : 1;
      /* Bit 0  State of the TX power limiting 
       * 0 – Tx power not limited 1 – Tx power is limited */

    uint8  carrier_enabled  : 1;
      /* Is Tx enabled for this carrier? */

    uint8  reserved         : 6;   
      /* Bits 7:2  Zero padding */

  } power_info;

  int16   tx_open_loop_power;
    /* TX power determined by the open loop adjust mechanism */

  int16   tx_closed_loop_adjust;
    /* TX power determined by the closed loop adjust mechanism */

  int16   tx_pilot_power;
    /* Pilot power that is transmitted over the pilot channel */

  int16   tx_total_power;
    /* Total Tx power as determined by the entire Tx AGC mechanism */

  int16   rx_agc0;
    /* Total receive power as seen by Antenna0 */

  int16   rx_agc1;
    /* Total receive power as seen by the Antenna1 */

} hdrlog_mc_power_per_carrier_info_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_POWER_C)

  uint8   version;
    /* The version number of the packet. */  

  uint8   lna_state;
    /* LNA state */

  uint8   tx_enabled;
    /* Is Tx enabled? */

  uint16  hstr;
    /* Current half-slot reference count */

  int16   total_tx_power;    
    /* Total Tx power over all the carriers */

  uint8   num_carriers;
    /* Number of carriers in the log packet */

  hdrlog_mc_power_per_carrier_info_type  per_carrier_power_info[1];
    /* The array to hold the per carrier records */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_MC_EQUALIZER_DATA_C
---------------------------------------------------------------------------*/

#define HDRLOG_MC_EQUALIZER_DATA_VER    0

#define HDRLOG_MC_NUM_EQ_PASSES         5

typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type  carrier_id;
    /* Carrier id of incoming packet that consists of 
     * channel id,channel no and band class */

  PACK(struct)
  {
    uint8  best_asp_idx : 4; 
      /* Bits 3:0  Best ASP index */   

    uint8  mode         : 1;  
      /* Bit 4 Equalizer/Rake/best of demodulation mode
       * 0-Rake  1-Equalizer */

    uint8  reserved     : 3; 
      /* Bits 7:5  Reserved */

  } bit_fields;

  uint16  serving_pn;
    /* PN offset of the best asp that the equalizer is demodulating */

  uint32  com_posx8;
    /* Center of mass rtc of best ASP in chipx8 units */
    
  uint32  c2i[HDRLOG_MC_NUM_EQ_PASSES];  
    /* Instantaneous equalizer C/I */

} hdrlog_per_carrier_mc_eq_data_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_EQUALIZER_DATA_C)

  uint8    version; 
    /* The version number of the packet */

  uint8    srch_state; 
    /* HDR Searcher state */

  uint32   mstrx8;   
    /* Mobile Station time reference */

  uint16   mstr_err_x8;
    /* MSTR w.r.t. to earliest arriving finger */  
  
  PACK(struct)
  {
    uint8  eq_status       : 1; 
      /* Bits 0  Equalizer on/off flag  */

    uint8   reserved       : 3;    
      /* Bits 3:1  Reserved */

    uint8   num_eq         : 4;
      /* Bits 7:4  Number of equalizers enabled */      

  } eq;

  uint16   str;
    /* Slot time reference of the log packet */

  uint8    num_carriers;
    /* Number of carriers in the log packet */  

  hdrlog_per_carrier_mc_eq_data_type  per_carrier_eq_data[1];
    /* The array to hold the per carrier records */

LOG_RECORD_END

/*---------------------------------------------------------------------------
                          LOG_HDR_FL_PHY_LAYER_PKT_C               
---------------------------------------------------------------------------*/

#define HDRLOG_FL_PHY_LAYER_PKT_LOG_VERSION             1
  /* Version number of the log packet */

#define HDRLOG_MAX_FL_MAC_PKTS                          5
  /* Number of Forward link MAC packet that can be stored in a single log packet */

#define HDRLOG_MAX_FL_PKT_SIZE_BYTES                    640
  /* Size of the biggest FL phy packet in bytes */

/* Encapsulates a FTC Physical layer packet */
typedef PACK(struct)
{
  PACK(struct)
  {
    uint8 pkt_len_in_16bytes  : 7;
      /* Length of the FTC Phy layer packet data in units of 16-bytes */

    uint8 pkt_rev             : 1;
      /* Physical layer revision ( 0 -> Release 0 and 1 -> Rev-A/B) */

  } pkt_info;

  hdrlog_mc_carrier_id_type carrier_id;
    /* FL Carrier on which the FTC packet is received */

  uint8 data[HDRLOG_MAX_FL_PKT_SIZE_BYTES];
    /* Physical packet data */

} hdrlog_fl_phy_layer_pkt_type;

/* Definition of FTC Physical layer log packet */
LOG_RECORD_DEFINE ( LOG_HDR_FL_PHY_LAYER_PKT_C )

  uint8 version;
    /* Version number of the log packet */

  uint16 log_seq_num;
    /* Sequence number of log packet */

  uint16 hstr;
    /* Half slot time reference corresponding to Preamble detection */

  uint8 num_phy_layer_pkts;
    /* Number of FTC physical layer packets in the log pkt */

  hdrlog_fl_phy_layer_pkt_type phy_layer_pkts[1];  
    /* Array of forward link physical layer packets */

LOG_RECORD_END /* LOG_HDR_FL_PHY_LAYER_PKT_C */

/*---------------------------------------------------------------------------
                          LOG_HDR_RL_PHY_LAYER_PKT_C               
  ---------------------------------------------------------------------------*/

#define HDRLOG_MAX_RMAC_PKT_SIZE_BYTES                  1532

#define HDRLOG_RL_PHY_LAYER_PKT_LOG_VERSION             1
  /* Version number of the log packet */

#define HDRLOG_PHY_LAYER_REVISION_0                     0
#define HDRLOG_PHY_LAYER_REVISION_A                     1
#define HDRLOG_PHY_LAYER_REVISION_B                     2

/* Encapsulates a RTC Physical packet with its format*/
typedef PACK(struct)
{
  PACK(struct)
  {
    uint16 pkt_len_in_16bytes  : 7;
      /* Bits 0-6 Length of the RTC physical packet data in units of 16-bytes */

    uint16 reserved            : 7;
      /* Bits 7-13 Reserved bits */

    uint16 phy_rev             : 2;
      /* Bits 14-15 Physical layer revision 
         ( 0 -> Release 0, 1 -> Rev-A, and 2 -> Rev-B) */

  } pkt_info;

  hdrlog_mc_carrier_id_type carrier_id;
    /* RL Carrier ID */

  uint8 data[HDRLOG_MAX_RMAC_PKT_SIZE_BYTES];
    /* Physical channel packet data */

} hdrlog_rtc_phy_pkt_type;

/* Definition of an RTC MAC log packet */
LOG_RECORD_DEFINE ( LOG_HDR_RL_PHY_LAYER_PKT_C )

  uint8 version;
    /* Version number of the log packet */

  uint16 log_seq_num;
    /* log packet sequence number */

  uint8 subframe_num;
    /* mod 75 counter of subframes */

  hdrlog_rtc_phy_pkt_type phy_pkt;
    /* Only one RTC physical pkt in a log pkt */

LOG_RECORD_END /* LOG_HDR_RL_PHY_LAYER_PKT_C */

/*---------------------------------------------------------------------------
                          LOG_HDR_MC_MUP_FWD_STATS_C
---------------------------------------------------------------------------*/
#define HDRLOG_MC_MUP_FWD_STATS_LOG_VERSION             0

typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Carrier ID */

  hdrlog_mup_crc_stats_type crc;
    /* CRC statistics */

  hdrlog_mup_early_decode_type early_decode;
    /* early decode statistics */

} hdrlog_perc_mup_fwd_stats_type;

/* Log packet for MC Multi-user Packet statistics */
LOG_RECORD_DEFINE ( LOG_HDR_MC_MUP_FWD_STATS_C )

  uint8 version;
    /* Version number of the log packet */

  uint32 log_seq_num;
    /* log sequence number */

  uint8 num_carriers;
    /* Number of carriers logged */

  hdrlog_perc_mup_fwd_stats_type perc_mup_stats[1];
    /* NumCarriers Instances of MUP Fwd statistics */

LOG_RECORD_END /* LOG_HDR_MC_MUP_FWD_STATS_C */

/*---------------------------------------------------------------------------
                          LOG_HDR_MC_MUP_USER_STATS_C
---------------------------------------------------------------------------*/
#define HDRLOG_MC_MUP_USER_STATS_LOG_VERSION             0

typedef PACK(struct)
{
  hdrlog_mc_carrier_id_type carrier_id;
    /* Carrier ID */

  hdrlog_mup_user_stats_summ_type mup_user_stats;
    /* MUP User statistics summary */

} hdrlog_perc_mup_user_stats_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_MUP_USER_STATS_C)

  uint8 version;
    /* The version number of the log packet */

  uint32 log_seq_num;
    /* Sequence number of the log packet */

  uint8 num_carriers;
    /* Number of carriers */

  hdrlog_perc_mup_user_stats_type perc[1];
    /* NumCarriers instnaces of per-carrier MUP user stats */

LOG_RECORD_END


/*---------------------------------------------------------------------------
                         LOG_HDR_MC_PILOT_SETS_C          
---------------------------------------------------------------------------*/

#define HDRLOG_MC_PILOT_SETS_VER       2 
  /* Version number for the multi carrier pilot sets log */

typedef PACK(struct)
{
  /* Total 17 bytes per ASET pilot record */

  /* --- Aset Header field (7 bytes) --- */

  uint16  pilot_pn;
    /* Pilot PN offset */
  uint16  total_eng;
    /* Total filtered energy of this pilot */
  uint16  ofs_cond_eng;
    /* Filtered energy for OFS condition */
  uint8   pilot_gid;
    /* Pilot Group ID */

  /* --- Channel info field (2 bytes) --- */

  PACK(struct)
  {
    uint16 chan_no               : 11;
      /* Sector channel number */
    uint16 band_class            : 5;
      /* Sector band class */
  } chan_info;                    /* CDMA Frequency           */

  /* --- Index info field (4 bytes) --- */

  PACK(struct)
  {
    uint8  link_id               : 8;
      /* Link id */

    uint8  demod_index           : 2;
      /* Demod index for this subactive set */
    uint8  reportable            : 1;
      /* Is the sector reportable */
    uint8  subaset_index         : 2;
      /* TCA index */
    uint8  sched_tag             : 3;
      /* Scheduler tag assigned */

    uint8  asp_index             : 4;
      /* ASP index for the ASET sector */
    uint8  rpc_index             : 4;
      /* RPC index for the ASET */

    uint8  drc_cover             : 3;
      /* DRC Cover  */
    uint8  droptimer_expired     : 1;
      /* Drop Timer Expired Flag  */
    uint8  droptimer_active      : 1;
      /* Drop Timer Active Flag  */
    uint8  reserved              : 3;
      /* Reserved  */
  } index_info;

  /* --- MAC info field (4 bytes) --- */

  PACK(struct)
  {
    uint16  fl_mac_index         : 10;
      /* Forward link mac Index */
    uint8   dsc_value            : 3;
      /* DSC value */
    uint8   aux_drc_cover        : 3;
      /* AUX DRC cover */

    uint8   rab_mac_index        : 7;
      /* RAB mac index */
    uint16  rl_mac_index         : 9;
      /* RL mac index */
  } mac_info;

  /* --- Win center field (2 bytes) --- */

  uint16   win_center_x2;
    /* Search window center */

} log_hdr_mc_pilot_sets_aset_type;

typedef PACK(struct)
{
  /* Total 10 bytes */

  /* --- Header field (7 bytes) --- */

  uint16  pilot_pn;
    /* Pilot PN offset */
  uint16  total_eng;
    /* Total filtered energy of this pilot */
  uint16  ofs_cond_eng;
    /* Filtered energy for OFS condition */
  uint8   pilot_gid;
    /* Pilot Group ID */

  /* --- Channel info field (2 bytes) --- */

  PACK(struct)
  {
    uint16 chan_no             : 11;
      /* CSET channel number      */
    uint16 band_class          : 5;
      /* CSET band class          */
  } chan_info; 

  /* --- CSET info (1 bytes) --- */

  PACK(struct)
  {
    uint16  droptimer_expired    : 1;
      /* Bit8   Drop Timer Expired Flag  */
    uint16  droptimer_active     : 1;
      /* Bit9   Drop Timer Active Flag  */
    uint8   reserved             : 6;
      /* Reserved  */
  } cset_info;

  /* --- Win Center (2 bytes) -- */

  uint16  win_center_x2;

} log_hdr_mc_pilot_sets_cset_type;

typedef PACK(struct)
{
  /* Total 12 bytes */

  /* --- Header field (7 bytes) --- */

  uint16  pilot_pn;          
    /* Pilot PN offset */
  uint16  total_eng;
    /* Total filtered energy of this pilot */
  uint16  ofs_cond_eng;
    /* Filtered energy for OFS condition */
  uint8   pilot_gid;
    /* Pilot Group ID */

  /* --- Channel info field (2 bytes) --- */

  PACK(struct)
  {
    uint16 chan_no               : 11;
      /* NSET channel number      */
    uint16 band_class            : 5;
      /* NSET band class          */
  } chan_info;

  /* --- Search window offset and aging info field (3 bytes) --- */

  PACK(struct)
  {
    uint16  win_size;
      /* NSET window can be upto 512 chips */
    uint16  offset                : 3;
      /* Neighbor search window offset */
    uint16  reserved              : 5;
      /* Reserved */
  } win_info;

  /* --- neighbor set aging information (2 bytes) --- */

  uint16  age;
    /* Age */
} log_hdr_mc_pilot_sets_nset_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_PILOT_SETS_C)

  uint8    version; 
    /* The version number of the log packet */

  uint8    pn_inc; 
    /* The PN increments */

  uint8    srch_state;             
    /* Denotes the state of the HDR searcher task */

  uint8    aset_cnt;   
    /* Active set pilot number */

  uint8    aset_win;
    /* Active set pilot search window size in the unit of chipx1 */  
  
  uint16   slot_cnt;
    /* TBD */  

  uint8    cset_cnt;   
    /* Candidate set pilot number */

  uint8    cset_win;
    /* Candidate set pilot search window size in the unit of chipx1 */  

  uint8    nset_cnt;   
    /* Neighbor set pilot number */

  uint8    rset_win;
    /* Remaining set pilot search window size in the unit of chipx1 */  

  PACK(struct)
  {
    uint16   chan_no    : 11;
      /* Bit10 :0  :: RSET Channel Number */
    uint16   band_class :  5;
      /* Bit15:11  :: Band Class */
  } rset_channel;

  log_hdr_mc_pilot_sets_aset_type     aset_info[1];
    /* One entry of the ASET pilot */

  log_hdr_mc_pilot_sets_cset_type     cset_info[1];
    /* One entry of the CSET pilot */

  log_hdr_mc_pilot_sets_nset_type     nset_info[1];
    /* One entry of the NSET pilot */
LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_OVHD_CACHE_SUMMARY_C 
---------------------------------------------------------------------------*/
#define HDRLOG_OVHD_CACHE_SUMMARY_LOG_VERSION             2

typedef uint32 hdrlog_ovhd_cache_pointer_type;

typedef PACK(struct)
{
  uint16 sig;
    /* BCMCS overhead msg signiture */

} log_hdr_ovhd_cache_bc_entry_type;

typedef PACK(struct)
{
  uint16 sig;
    /* AP msg signiture */
} log_hdr_ovhd_cache_ap_entry_type;

typedef PACK(struct)
{
  uint8 index;
    /* index into hdrovhd cache table */

  uint8 config_change;
    /* config change field in QP msg */

  qword timestamp;
    /* time last QC was received on this sector */

  uint8 band;
    /* band on which SP msg was received */

  uint16 channel;
    /* channel on which SP msg was received */

  uint16 pilot_pn;
    /* pilot PN of the sector */

  uint8 color_code;
    /* Subnet color code */

  uint16 sector_sig;
    /* SP msg sigature */

  hdraddr_type sector_id;
    /* 128-bit sector ID */

  uint8 subnet_mask;
    /* subnet mask */

  uint8 qc_valid;
    /* QC cache is valid */

  uint8 ap_included;
    /* if AP msg entry is included */

  log_hdr_ovhd_cache_ap_entry_type ap[1];

  uint8 bc_state;
    /* BCMCS cache state:
         INVALID    : 0
         CURRENT    : 1
         NOT_PARSED : 2 
     */

  hdrlog_ovhd_cache_pointer_type bc_packed_msg;
    /* pointer to packed BCMCS overhead msg
       if bc_state != CURRENT, set to NULL */

  log_hdr_ovhd_cache_bc_entry_type bc[1];
    /* BCMCS entry. Included when bc_state == CURRENT */
} log_hdr_ovhd_cache_entry_type;

LOG_RECORD_DEFINE(LOG_HDR_OVHD_CACHE_SUMMARY_C)

  uint8    version; 
    /* The version number of the log packet */

  uint8    cur_cache_idx;
    /* Current cache index */

  uint8    num_cache_entry;
    /* Number of cache entries */

  log_hdr_ovhd_cache_entry_type cache[1];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_ARD_EVENT_C 
---------------------------------------------------------------------------*/
#define HDRLOG_ARD_EVENT_LOG_VERSION             1

typedef enum
{
  HDRLOG_ARD_EVENT_ENTER_TC,
  HDRLOG_ARD_EVENT_EXIT_TC,
  HDRLOG_ARD_EVENT_SUSPEND,
  HDRLOG_ARD_EVENT_RESUME,
  HDRLOG_ARD_EVENT_FL_ACTIVITY,
  HDRLOG_ARD_EVENT_FL_INACT_TIMER_EXP,
  HDRLOG_ARD_EVENT_DRC_CHANGED,  
  HDRLOG_ARD_EVENT_SET_DRC,

  HDRSRCHLOG_ARD_EVENT_ENUM_SIZE
} hdrlog_ard_event_enum_type;

typedef enum
{
  HDRLOG_ARD_STATE_INACTIVE_STATE,
  HDRLOG_ARD_FL_ACTIVE_STATE,
  HDRLOG_ARD_FL_INACTIVE_DRC_LOW_STATE,
  HDRLOG_ARD_FL_INACTIVE_DRC_HIGH_STATE,
  HDRLOG_ARD_SUSPENDED_STATE,

  HDRLOG_ARD_SAME_STATE = 0xFF
} hdrlog_ard_state_enum_type;

LOG_RECORD_DEFINE(LOG_HDR_ARD_EVENT_C)
  uint8    version; 
    /* The version number of the log packet */

  hdrlog_ard_event_enum_type    event;
    /* ARD event */

  hdrlog_ard_state_enum_type    new_state;
    /* New ARD state */

  uint8    time_left;
    /* Time left in inactivity timer */

  boolean  div_requested;
    /* Whether diversity has beenr requested */

  uint8    drc;
    /* Predicted DRC. value*/
LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_REVA_DTX_INFO_C 
---------------------------------------------------------------------------*/
#define HDRLOG_REVA_DTX_INFO_VERSION             3
  /* Version number for packet */

#define HDRLOG_MAX_SUBFRAMES_DTX_INFO_PKT        8
  /* max subframes in a log packet */

typedef PACK(struct) {

  boolean  in_tap_call;
    /* Whether AT is in TAP call */
  boolean  fc_enabled_sw;
    /* Whether flexConn is enabled in software */
  boolean  fc_monitor_started;
    /* Whehter SW has started to monitor conditions to enable/disable fc mode */ 
  boolean  fl_active_flexconn;
    /* Whether FL has been inactive for flexconn timer */
  boolean  fl_active_sdtx;
    /* Whether FL has been active for sdtx timer */
  boolean  rl_active;
    /* Whether RL is active */
  boolean  sdtx_enabled;
    /* Whether sDTX is enabled in software */
  boolean  tdtx_enabled;
    /* Whether tDTX is enabled in software */
  boolean  ndtx_enabled;
    /* Whether nDTX is enabled in software */
  boolean  dtx_monitor_started;
    /* Whehter SW has started to monitor conditions to
       enable/disable DTX mode */ 
  boolean  cxm_allows_dtx;
    /* In DSDA scenario if we are already blanking due to G interference, CXM
        Will not allow any other device only dtx */
  boolean  first_dsc_duty_cycle;
    /* Whether HDR is in first duty cycle */
  boolean  drc_boost;
    /* Whether drc channel is boosted */
  boolean is_repointing_msg_rcvd;
    /* Whether we received repointing msg from FW in this subframe */
  uint8    subframe_num;
    /* subframe number */
  uint16   curr_hstr;
    /* current hstr */
  uint8    mac_index_cnt;
    /* mac index count */
  int16    frab;
    /* frab: [-32768,32767] mapping to [-1,+1]  */
  uint8    dsc_length_in_slots;
    /* DSC length in slots */
  uint16   dtx_mode;
    /*  DTX mode programmed to FW */
  uint8    drc_length_in_slot;
    /* drc length */
  uint8    dtx_frame_offset_sw;
    /* frame offset for DTX */
} hdrlog_reva_dtx_info_type;

LOG_RECORD_DEFINE(LOG_HDR_REVA_DTX_INFO_C)
  uint8  version;
    /* The version number of the packet. */

  uint8  subframe_count;
    /* The number of subframes logged in this packet */

  hdrlog_reva_dtx_info_type   reva_subframe_dtx_info
    [HDRLOG_MAX_SUBFRAMES_DTX_INFO_PKT];

LOG_RECORD_END

/*---------------------------------------------------------------------------
                       LOG_HDR_MC_DTX_INFO_C 
---------------------------------------------------------------------------*/
#define HDRLOG_MC_DTX_INFO_VERSION             2

typedef PACK(struct) {

  uint8    carrier_id;
    /* Carrier ID: 0,1 or 2 */

  int16    frab;
    /* frab: [-32768,32767] mapping to [-1,+1]  */

} hdrlog_mc_dtx_info_chan_wide_type;

typedef PACK(struct) {

  boolean  in_tap_call;
    /* Whether AT is in TAP call */
  boolean  fc_enabled_sw;
    /* Whether flexConn is enabled in software */
  boolean  fc_monitor_started;
    /* Whehter SW has started to monitor conditions to enable/disable fc mode */ 
  boolean  fl_active_flexconn;
    /* Whether FL has been active for flexconn timer */
  boolean  fl_active_sdtx;
    /* Whether FL has been active for sdtx timer */
  boolean  rl_active;
    /* Whether RL is active */
  boolean  sdtx_enabled;
    /* Whether sDTX is enabled in software */
  boolean  tdtx_enabled;
    /* Whether tDTX is enabled in software */
  boolean  ndtx_enabled;
    /* Whether nDTX is enabled in software */
  boolean  dtx_monitor_started;
    /* Whehter SW has started to monitor conditions to
       enable/disable DTX mode */ 
  boolean  cxm_allows_dtx;
    /* In DSDA scenario if we are already blanking due to G interference, CXM
        Will not allow any other device only dtx */
  boolean  first_dsc_duty_cycle;
    /* Whether HDR is in first duty cycle */
  boolean  drc_boost;
    /* Whether drc channel is boosted */
  boolean is_repointing_msg_rcvd;
    /* Whether we received repointing msg from FW in this subframe */
  uint8    subframe_num;
    /* subframe number */
  uint16   curr_hstr;
    /* current hstr */
  uint8    mac_index_cnt;
    /* mac index count */
  uint8    dsc_length_in_slots;
    /* DSC length in slots */
  uint16   dtx_mode;
    /*  DTX mode programmed to FW */
  uint8    drc_length_in_slot;
    /* drc length */
  uint8    dtx_frame_offset_sw;
    /* frame offset for DTX */
  hdrlog_mc_dtx_info_chan_wide_type  dtx_info_per_chan[HDR_LOG_MAX_CARRIERS];

} hdrlog_mc_dtx_info_sf_wide_type;

LOG_RECORD_DEFINE(LOG_HDR_MC_DTX_INFO_C)
  uint8  version;
    /* The version number of the packet. */

  uint8                            subframe_count;
    /* The number of subframes logged in this packet */

  hdrlog_mc_dtx_info_sf_wide_type   dtx_info_per_sf[HDRLOG_MAX_SUBFRAMES_DTX_INFO_PKT];

LOG_RECORD_END

#ifdef FEATURE_HDR_SELECTIVE_TXD
typedef PACK(struct)
{
  uint8 type;
  uint8 decision_stage;
  uint16 hold_time;
  uint16 rx0;
  uint16 rx1;
  uint16 curr_sinr;
  uint16 prev_sinr;
  uint16 preswitch_sinr;
  uint16 curr_ratchet;
  uint16 prev_ratchet;
  uint16 preswitch_ratchet;
} log_hdr_stxd_timer_event_type;

typedef PACK(struct)
{
  uint16 rx0;
  uint16 rx1;
  
  uint16 sinr;
  uint16 ratchet;

  uint8 agc_cnt;
  uint8 ratchet_cnt;
} log_hdr_stxd_mdsp_event_type;

LOG_RECORD_DEFINE(LOG_HDR_STXD_EVENT_C)
  uint8 version;
  uint8 type;
  uint8 event;
  uint8 state;
  uint8 pending_state;
  uint8 test_mode;
  uint8 ant_switch;
  uint8 pending_switch;
  union {
    log_hdr_stxd_timer_event_type timer_info;
    log_hdr_stxd_mdsp_event_type mdsp_info;
  };
LOG_RECORD_END
#endif /* FEATURE_HDR_SELECTIVE_TXD */

#ifdef FEATURE_POWER_INFO_LOG_PACKET
/* -------------------------------------------------
   EVDO    EVDO physical layer power info     0x1878
   ------------------------------------------------- */

#define HDRLOG_PHYSICAL_LAYER_POWER_INFO_VERSION  1

LOG_RECORD_DEFINE(LOG_EVDO_PHYSICAL_LAYER_POWER_INFO_C)
  uint8      log_pkt_version;
  uint8      padding[7]; /* Ensure byte alignment of ustmr */
  uint64     ustmr;      /* Time when first sample in packet was collected */
  uint8      number_of_carriers; /* [1,2,3] */                                                                           
  uint8      aset_size_carrier0; /* 0-6 */                                                                               
  uint8      aset_size_carrier1; /* 0-6*/                                                                                
  uint8      aset_size_carrier2; /* 0-6*/                                                                                
  uint8      number_of_fingers;  /* 0-8*/                                                                                
  uint8      rxd;                /* TRUE if RxD is enabled. True if RxD is enabled at any point in the 26.66ms period.*/ 
  uint8      txpwr;              /* Latest non-zero Tx power used, in 0.5dBm steps. Max Tx power is 24dBm.*/             
  uint8      dtx;                /* Number of slots that Tx is disabled during the 26.66ms period.*/                     
  uint8      num_drx_slots;      /* Number of DRX slots (5/3ms) during the 26.66ms duration. */                          
LOG_RECORD_END
#endif /* FEATURE_POWER_INFO_LOG_PACKET */

#endif /* HDR_LOGI_H */

