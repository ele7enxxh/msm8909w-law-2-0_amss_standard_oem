#ifndef L1LOG_V_H
#define L1LOG_V_H

/*===========================================================================
                             D L D E M L O G . H

DESCRIPTION
This file contains or includes files that contain Layer 1 demod log packet
structure definitions, prototypes for Layer 1 logging functions, any
constant definitions that are needed, and any external variable declarations
needed for Layer 1 logging.


    Copyright (c) 2001 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldemlog_v.h    $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dldemlog_v.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
05/20/16    sl      Changes for 0x4186 log packet version11
01/05/15    pr      Changes for 0x4186 for EcIo conversion table usage.
11/24/14    mit     0x4186 log pkt changes to add L1 State after APEX and QXDM is ready
11/05/14    mit     Backing out 0x4186 log pkt changes as APEX and
                    QXDM have not released these changes officially 
10/30/14    mit     0x4186 log pkt changes to add L1 State
06/26/14    hk      DL Features Mainline
05/21/14    sl      Changes for 0x4186 log packet version8
05/23/13    geg     WSW decoupling (except for component of QCHAT)
04/11/13    pr      Added logging changes for 0x4186 for debug purposes.
04/02/13    pr      Mainlined FEATURE_WCDMA_L1_TA_INFO_LOG_CODE_4186
08/23/12    pr      0x4186 logpacket Changes for STMR CTRS
06/25/12    pr      Added logging for sixTapEn in 0x4186
02/01/12    gnk     Feature cleanup.
01/30/12    vs      Feature cleanup.
04/24/11    geg     initial file
===========================================================================*/

#include "wcdma_variation.h"
#include "srchset.h"
#include "dltri.h"
#include "log_codes_wcdma_int.h"
#include "log_codes.h"
#include "dlphch.h"
#include "dltriparm.h" 
#include "dldemlog.h"

/*--------------------------------------------------------------------------
                 LOG PACKET: WCDMA_FINGER_TA_INFO_LOG_PACKET

This structure is use to log finger information.  There is room to log
all 8 fingers but if fewer are logged in a given packet, usually due to
the fact that not all 8 are enabled, then they will be packed into the
first K entries (K the number of fingers logged).
--------------------------------------------------------------------------*/
/* the log code for the finger TA packet */
#ifndef WCDMA_FINGER_TA_INFO_LOG_PACKET
  #define WCDMA_FINGER_TA_INFO_LOG_PACKET WCDMA_FINGER_TA_INFO_0x4186_LOG_PACKET 
#else
#error redefinition of LOG command code: WCDMA_FINGER_TA_INFO_LOG_PACKET
#endif

/* sampling and commit rates for the finger information */
#define WCDMA_FING_LOG_PKT_SAMPLE_RATE 50 // sampled once every 50 frames
#define WCDMA_FING_LOG_PKT_COMMIT_RATE 50 // committed once every 50 frames

#if defined(WCDMA_TA_INFO_LOG_CODE_4202) || defined(WCDMA_TA_INFO_LOG_CODE_4006)

/* Define finger HS info fields */

/* HS cell index info */
#define WCDMA_FING_HS_INFO_HS_CELL_IDX_BMSK 0x07
#define WCDMA_FING_HS_INFO_HS_CELL_IDX_SHFT 0

/* HS enabled */
#define WCDMA_FING_HS_INFO_HS_ENABLED_BMSK 0x10
#define WCDMA_FING_HS_INFO_HS_ENABLED_SHFT 4

/* HS refence finger */
#define WCDMA_FING_HS_INFO_HS_REF_FING_BMSK 0x20
#define WCDMA_FING_HS_INFO_HS_REF_FING_SHFT 5

#define WCDMA_FING_HS_LOG_INFO_SET_FIELD(buf, field, value) \
  (buf) = (((buf) & (~WCDMA_FING_HS_INFO_##field##_BMSK)) | \
           (((value) << WCDMA_FING_HS_INFO_##field##_SHFT) & WCDMA_FING_HS_INFO_##field##_BMSK))

#endif /* defined(WCDMA_TA_INFO_LOG_CODE_4202) || defined(WCDMA_TA_INFO_LOG_CODE_4006) */

/* Finger Pair Assign Mode */
#define WCDMA_FING_RXD_INFO_PAIR_MODE_BMSK  0x03
#define WCDMA_FING_RXD_INFO_PAIR_MODE_SHFT  0

/* Paired Finger Index */
#define WCDMA_FING_RXD_INFO_PAIR_FING_IDX_BMSK  0x3C
#define WCDMA_FING_RXD_INFO_PAIR_FING_IDX_SHFT  2

/* Finger Antenna */
#define WCDMA_FING_RXD_INFO_ANT_IDX_BMSK  0x40
#define WCDMA_FING_RXD_INFO_ANT_IDX_SHFT  6

#define WCDMA_FING_RXD_LOG_INFO_SET_FIELD(buf, field, value) \
  (buf) = (((buf) & (~WCDMA_FING_RXD_INFO_##field##_BMSK)) | \
           (((value) << WCDMA_FING_RXD_INFO_##field##_SHFT) & WCDMA_FING_RXD_INFO_##field##_BMSK))

/* Finger ttl stepsize */
#define WCDMA_FING_TTLSTEPSIZE_INFO_BMSK  0x10
#define WCDMA_FING_TTLSTEPSIZE_INFO_SHFT  4

#define WCDMA_FING_LOG_INFO_SET_FIELD(buf, field, value) \
  (buf) = (((buf) & (~WCDMA_FING_##field##_BMSK)) | \
           (((value) << WCDMA_FING_##field##_SHFT) & WCDMA_FING_##field##_BMSK))

/* Finger EUL Cell RLS Index */
#define WCDMA_FING_EUL_INFO_RG_COMB_INDEX_BMSK  0x38
#define WCDMA_FING_EUL_INFO_RG_COMB_INDEX_SHFT     3

/* Finger EUL Serving Cell Indicator */
#define WCDMA_FING_EUL_INFO_SERV_CELL_BMSK      0x40
#define WCDMA_FING_EUL_INFO_SERV_CELL_SHFT         6

/* Finger EUL Serving Cell Ref Finger Indicator */
#define WCDMA_FING_EUL_INFO_REF_FING_BMSK       0x10
#define WCDMA_FING_EUL_INFO_REF_FING_SHFT          4

/* Finger EUL Active Indicator */
#define WCDMA_FING_EUL_INFO_ACTIVE_BMSK         0x80
#define WCDMA_FING_EUL_INFO_ACTIVE_SHFT            7

#define WCDMA_FING_EUL_LOG_INFO_SET_FIELD(buf, field, value) \
  (buf) = (((buf) & (~WCDMA_FING_EUL_INFO_##field##_BMSK)) | \
           (((value) << WCDMA_FING_EUL_INFO_##field##_SHFT) & WCDMA_FING_EUL_INFO_##field##_BMSK))

/* The shift needed for EDL channels to fill in the finger log information*/
#define EDL_FING_LOG_CHAN_INFO_AGCH_SHIFT 14
#define EDL_FING_LOG_CHAN_INFO_RGCH_SHIFT 15
#define EDL_FING_LOG_CHAN_INFO_HICH_SHIFT 16
/* The shift needed for EDL channels to fill in the finger log information*/

/* The shift needed to retreive active HWCH info to populate RL TTO*/
#define DL_PHCH_HWCH_DPCH_FDPCH_SHIFT 0x280
#define DL_PHCH_HWCH_OTHER_TTO_NEEDED_SHIFT 0x504
/* The shift needed to retreive active HWCH info to populate RL TTO*/

/*--------------------------------------------------------------------------
                      LOG_PACKET: WCDMA TEMPORAL ANALYSIS (NEW)
--------------------------------------------------------------------------*/
/* Cell information struture to be embedded in TA packet */
typedef PACKED struct PACKED_POST {

  /*0/1/2 - 1C/2C/3C resp*/
  uint8 carr_Idx;

  /* Cell Index (0..10)*/
  uint8 cell_idx;

  /* scrambling code 
     BIT [0..3] Secondary Scrambling code
     BIT [4..12] Primary Scrambling code
  */
  uint16 scr_code;

  /* In 256 cx1 units */
  uint8 tto;
	  
  /* Service bit mask
     BIT 0: HSDPA Service on this Cell 
     BIT 1: HSUPA Service on this Cell
     BIT 2: R99 ASET Service on this Cell
     BIT 3: NBR Service on this Cell
     BIT 4: MBMS Service on this Cell
     BIT 5: MIMO Service on this Cell
     BIT [5..7] Reserved
  */
  uint8 svc_mask;

  /*
    BIT 0..2 TPC Index
    BIT 3..5 EUL RG Index
    BIT 6 EUL Serving Cell
    BIT 7 MBMS Serving Cell
    BIT 8 R99 ASET Reference Cell
    BIT 9 Primary CPICH phase reference enabled
    BIT 10 Secondary CPICH phase reference enabled
    BIT [11..12]  TX diversity Info
                0 - No diversity pilot (CPICH)
                1- Diversity pilot available (CPICH)
                2 - MIMO S-CPICH enabled
                3- Reserved
    BIT 13 TD detection pending
    BIT [14..15] Reserved
  */
  uint16 index_ref;

  /*
    BIT [0..1] EQ Index
              0 - 
              1 -
              2 -
              3 - Invalid
    BIT 2 SCH IC enabled 
    BIT 3QICE enabled
    BIT 4 LEQ enabled
    BIT [5..7]: Reserved
  */
  uint8 eq_info;

  /* if EQ is disabled : 0xFFFF */
  uint16 eq_window_pos;

  /*Primary CPICH based Reference Finger*/
  uint8 pri_ref_fing;
  /*Secondary CPICH based Reference Finger*/
  uint8 sec_ref_fing;
  /*Cell reference position from Searcher*/
  uint32 ref_pos;
  /*The OVSF code of the secondary CPICH.*/
  uint8 sec_cpich_ovsf;
  /*All Configured Physical Channels that are active on this Cell*/
  uint32 active_physical_channels;
  /*
    BIT [0..18]: chipx8 value
    BIT [19..30]: Frame Number
    BIT 31: Reserved
  */
  uint32 sfn_wrc_diff;

}per_cell_info_struct_type;

/* Path information struture to be embedded in TA packet */
typedef PACKED struct PACKED_POST {

  /* The energy of the path in searcher output units.  See the ICD for a
     conversion formula */
  uint32 eng;
  /* The PN position of the path, [0..38400*8 - 1] */
  uint32 pos;
  /* Index of the Cell valid range [0..7]*/
  uint8 cell_idx;
  /* Index of the finger assigned to the path. Valid range [0,23].  A value of 0xFF indicates no finger.*/
  uint8 fing;
  /* Coherent integration length, N, in units of chips.*/
  uint16 nc;
  /* Non-coherent integration length, M, in units of coherent integration length.*/
  uint8 nn;
  /*
    BIT 0: 1 if the path is a virtual path, 0 otherwise.
    BIT 1: Primary Ant
    BIT 2: Div Ant
    BIT 3: STTD
    BIT [4..7]: Reserved
  */
  uint8 ant_and_virtual_path_info;

}per_path_info_struct_type;

/* Finger information struture to be embedded in TA packet */
typedef PACKED struct PACKED_POST {

  /* Index of the Cell valid range [0..7]*/
  uint8 cell_idx;
  /*Finger ID.  Valid range [0, 15].*/

  uint8 fing;
  /*  Finger lock status bitmask.  
     BIT 0 - Lock detects state.
     BIT 1 - Time tracking lock.
     BIT 2 - Combiner lock.
     BIT 3 - Power lock.
     BIT 4 - ttl stepSize
     BIT [4..7] :Reserved
  */  
  uint8 lock;

  /*Finger PN position.  Valid range [0, 38400*32-1].*/
  uint32 fing_pos_cx32;

  /* The CPICH energy of the non-diversity arm of a finger given in linear units.
     This is read directly from the firmware.  See the ICD for a conversion
     formula to get to dB EcIo */  
  uint16 cp_eng;

  /* The CPICH energy of the diversity arm of a finger given in linear units.
     This is read directly from the firmware.  See the ICD for a conversion
     formula to get to dB EcIo 
  */
  uint16 txdiv_cp_eng;

  /* the sum of the RSSI from the non-diversity and diversity arms of the
     CPICH saturated at 0 
  */
  uint16 tot_cp_eng;

  /* the value of the finger rotator */
  int16 rot;

  /* Rx Diversity information for this finger (pair mode, paired finger, antenna) */
  uint8 fing_rxd_info;
    
  uint16 apfAlpha;
    
  uint8 cellRscType;    

  uint8 vrtCellBpgOffset;
    
}per_finger_info_struct_type;

/* Timing information struture to be embedded in TA packet */
typedef PACKED struct PACKED_POST {

  /*
  BIT [0..23] - Reference Counter Adjust Value
  BIT [24..32] - Reserved
  */
  uint32 ref_count_adjust;

  /* Accumulated TCXO adjust value */
  int16 tcxo_adj_accum;

  /* Filtered average frequency error over all fingers */
  int16 freq_common;

  /* 
  BIT [0..18] - TX System Count Value in chipx8
  BIT [19..21] - Frame number within TTI
  BIT [22..31] - Reserved
  */
  uint32 tx_system_count;

  /*
  BIT [0..18] - Frame Reference Counter Value in chipx8
  BIT [19..30] - Frame number
  BIT 32 - Reserved
  */
  uint32 wrc;

  /*
  BIT [0..1] - The STMR counter that is aligned to RX timeline
  BIT [2..7]: Reserved
  */
  uint8 aligned_stmr_counter;

  /*
  BIT[ 0:15] - Combiner Counter Value in chipx1
  BIT [16:23] - CFN Count
  BIT [24:28] - Reference Finger Index
  Num Counters are 4. We limit logging to
  two counters. Other two are unused.
  */
  uint32 stmr_counter[2];

  uint32 vrtPnPos;

  /* Tx timing drift (in cx8) tracked by WFW */
  int16 txHwBpgDrift;
}timing_info_struct_type;

LOG_RECORD_DEFINE(WCDMA_FINGER_TA_INFO_LOG_PACKET)
  /* Version number of this log packet.*/
  uint8 version_num;
  /* L1 state.*/
  uint8 l1_state;
  /*Ecio Conversion Table version. 0 is the default value for pre JO targets*/
  uint8 ecio_conv_tbl_ver;
  /* Number of active cells valid range [0..10]*/
  uint8 num_cell;
  /* Array of Cell structures Length can be 0 to 10, in units of cell structures.*/
  per_cell_info_struct_type cell[DL_MAX_NUM_CELLS];
  /* Number of active set paths.  Valid range [0, 35]. */
  uint8 num_paths;
  /* Array of path structures.  Length can be 0 to 35, in units of path structures.*/
  per_path_info_struct_type paths[DL_TRI_MAX_PATHS];
  /*Number of fingers.  Valid range [0, 23].*/
  uint8 num_fing;
  /* Array of finger structures.  Length can be 0 to 23, in units of finger structures.*/
  per_finger_info_struct_type fing_log[MSMHW_MAX_NUM_FINGERS];
  /* Timing Structure*/
  timing_info_struct_type tx_timing;
LOG_RECORD_END



/*--------------------------------------------------------------------------
                      LOG_PACKET: CELL STATUS DEBUG PACKET
--------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  /* 
  BIT 0: Enable flag for LEQ receiver
  BIT 1: Enable flag for QICE receiver
  BIT 2:  Enable flag for SCH interference cancellation for this cell
  BIT [3..4] : Receiver Info
      0 - Rake
      1 - LEQ
      2 - QICE
      3 - Reserved
  BIT [5..6]  TX diversity Info
              0 - No diversity pilot (CPICH)
              1- Diversity pilot available (CPICH)
              2 - MIMO S-CPICH enabled
              3- Reserved

  */
  uint8 cell_config;

  /* CPICH Ec/Io for primary */
  uint32 primary_info;

  /* CPICH Ec/Io for diversity */
  uint32 div_info;

  /* Nt/Io */
  uint32 ntio_info;
}cell_info_struct_type;

LOG_RECORD_DEFINE(WCDMA_CELL_STATUS_DEBUG_LOG_PACKET)
  /*
  Enabled Cell bit mask.
  BIT 0: 1 if cell 0 is enabled, Otherwise 0
  BIT 1: 1 if cell 1 is enabled, Otherwise 0
  BIT 2: 1 if cell 2 is enabled, Otherwise 0
  BIT 3: 1 if cell 3 is enabled, Otherwise 0
  BIT 4: 1 if cell 4 is enabled, Otherwise 0
  BIT 5: 1 if cell 5 is enabled, Otherwise 0
  BIT 6: 1 if cell 6 is enabled, Otherwise 0
  BIT 7: 1 if cell 7 is enabled, Otherwise 0
  */
  uint8 enabled_cells;  
  /*
  BIT [0..2] : HS Cell Index
  BIT[3..7] : Reserved
             0xFF - No HS Serving Cell
  */
  uint8 hs_cell_idx;

  /* Hs enabled cell info*/
  cell_info_struct_type hs_cell_info[1];

  /* All enabled cell infor */
  cell_info_struct_type cell_info[8];
LOG_RECORD_END

/*rxd finger stats log record definition*/
/*This 5 comes from prxagc(2 bytes) + srxagc(2 bytes) + number of fing
 pairs(1 byte)*/
#define WCDMA_RXD_COMMON_FING_STATS_SIZE 5

typedef PACKED struct PACKED_POST
{
    /*Which antenna the finger belongs to*/
    uint8 antenna_id;
    /*which finger is this from among the 12*/
    uint8 fing_id;
    /*which finger is this paired with from among the 12*/
    uint8 paired_fing_id;
    /*lock mode*/
    uint8 lock_status;
    /*Which cell does this belong to*/
    uint16 psc;
    /* primary pilot filter I value this includes the tx divesity pilot filter i value*/
    int16 pf_i_val;
    /* primary pilot filter Q value this includes the tx divesity pilot filter q value*/
    int16 pf_q_val; 
}rxd_finger_iq_rec_struct_type;


typedef PACKED struct PACKED_POST
{
    /*rxagc 0 value*/
    int16 prim_rxagc;
    /* rx agc1 value*/
    int16 div_rxagc;
    /*number of fingers being sent in this log*/
    uint8 num_fings;
    /*array that holds the rssi stats for all the fingers that are paired*/
    rxd_finger_iq_rec_struct_type rxd_finger_iq_stats[MSMHW_MAX_NUM_FINGERS];
   
}wcdma_rxd_ant_radiation_stats_struct_type;

LOG_RECORD_DEFINE(WCDMA_RXD_ANT_RADIATION_STATS_LOG)

wcdma_rxd_ant_radiation_stats_struct_type wcdma_rxd_ant_radiation_stats;  

LOG_RECORD_END

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

#endif /* L1LOG_V_H */
