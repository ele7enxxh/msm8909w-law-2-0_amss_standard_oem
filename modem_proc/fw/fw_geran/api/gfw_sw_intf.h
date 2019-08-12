/*!
  @file
  gfw_sw_intf.h

  @brief
  Declaration of global host IF data structures

  @details
  This file contains data type declarations of the interface to the host processor

*/
/*===========================================================================

  Copyright (c) 2008-10 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_geran/api/gfw_sw_intf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef GFW_SW_INTF_H
#define GFW_SW_INTF_H

#include "modem_fw_memmap.h"
#include "gfw_sw_intf_common.h"
#include "gfw_sw_rf_intf.h"

#define CACHE_LINE_SZ       32


#define GFW_INTERFACE_VERSION "$Revision: #1 $\0"


//This is the number of cycles after a ftame tick we should not schedule ACQ or SCH
#define FRAME_TICK_SETUP_MARGIN_QS 625

#define GFW_DEBUG_BUFF_SIZE 1024

#define GFW_SCH_BURST_METRIC_LEN 10
#define GFW_SCH_MESSAGE_LEN 7

#define MAX_RX_DATA_PER_FRAME  19          /* for MDSP_TCH_F144         */


/* decoded DTM RX message packet */
#define MAX_RX_DTM_DATA_PER_FRAME  MAX_RX_GPRS_DATA_PER_FRAME

/* maximum non-sbi register writes */
#define MDSP_MAX_NONSBI_TRANSACTIONS         26

/*.t2g uses a system where there may be 6 monitors oustanding */
#define GFW_X2G_PWR_MON_BUFFERING   (6)

/* Structure of tx buffer in shared memory - not used except for calculating
   buffer sizes in mdsp_mem.c.  Max data length is max of GPRS and GSM */
#define MAX_TX_GSM_DATA_PER_FRAME 68

#define MDSP_RX_HDR_SIZE        8 /* this is changed from 6 to 8 because hrateSumC1SumC2, numSoftDec added to hdr */
#define MDSP_RX_EGPRS_HDR_SIZE  3
#define GFW_PD_TABLE_QUANT 1


/****************************************************************************
                   mDSP   DEFINITIONS  and  ENUMERATIONS
 ***************************************************************************/

/* We have five tx data buffers - 4 PDTCH + 1 PTCCH/SACCH */
#define MDSP_DTM_NUM_TX_DATA_BUFFERS 5

/* 13 Buffers, 5 ping-pong for PDTCH, 1 ping-pong for ASYNC RX and 1 for PTCCH/SACCH */
#define MDSP_DTM_NUM_RX_DATA_BUFFERS  (PINGPONG*(MDSP_MAX_DL_TS + 1) + 1)


/* AM/AM and AM/PM tables for the polar RF path */
#define MDSP_AM_AM_TABLE_ENTRIES  256
#define MDSP_AM_PM_TABLE_ENTRIES  256


/* There is one word for number of spurs and four */
/* int32 words for the spurs making 9 words total */
#define MDSP_TOTAL_SPUR_WORDS  9


/* Defines for messages lengths */
#define MDSP_CS1_WORDS    12
#define MDSP_CS2_WORDS    17
#define MDSP_CS3_WORDS    20
#define MDSP_CS4_WORDS    27
#define MDSP_PTCCHD_WORDS  MDSP_CS1_WORDS
#define MDSP_PTCCHU_WORDS  1
#define MDSP_ACCESS_WORDS  1

#define MDSP_MCS1_WORDS   12
#define MDSP_MCS2_WORDS   15
#define MDSP_MCS3_WORDS   19
#define MDSP_MCS4_WORDS   23
#define MDSP_MCS5_WORDS   29
#define MDSP_MCS6_WORDS   38
#define MDSP_MCS7_WORDS   29
#define MDSP_MCS8_WORDS   35
#define MDSP_MCS9_WORDS   38
#define MAX_RX_GPRS_DATA_PER_FRAME  (MDSP_MCS9_WORDS + 1) /* +1 for CRC */
#define MAX_TX_GPRS_DATA_PER_FRAME  MDSP_MCS9_WORDS
/* We use this method to assign the MAX of the GSM/GPRS Tx data for LINT */
#define MAX_TX_DTM_DATA_PER_FRAME MAX_TX_GSM_DATA_PER_FRAME

/* Data sizes for GSM in 16 bit words */
#define SCH_DATA_PER_FRAME      3
#define BCCH_DATA_PER_FRAME    12
#define FACCH_DATA_PER_FRAME   12


#define MDSP_RX_PTCCH_BUF_SIZ      ((5 + MDSP_PTCCHD_WORDS) * 2)
#define MDSP_TX_PTCCH_BUF_SIZ      ((2 + MDSP_PTCCHU_WORDS) * 2)


#define MDSP_HARD_DECISIONS_GMSK    8
#define MDSP_HARD_DECISIONS_8PSK   22
#define MDSP_MAX_HARD_DECISIONS    MDSP_HARD_DECISIONS_8PSK

/* Count of extra words when EGPRS is defined - async RX does not have these */
#define MDSP_EXTRA_EGPRS_BURST_METRICS  (MDSP_MAX_HARD_DECISIONS + 4)

#define GFW_X2G_CLEANUP_OVERHEAD      70 //!This is the overhead that L1 should add to the cleanup time to allow FW to finish at the end of a gap.

#define GFW_SHARED_MEM_START     FW_SMEM_GERAN_ADDR
#define GFW_SHARED_MEM_SIZE      FW_SMEM_GERAN_SIZE

#define MAX_TX_CAL_CMD_DMA_DATA_LENGTH    64

//! Maximum number of IQ samples: length(active burst) + maxSearchWidth + h3Delay
#define MAX_NUM_IQ_SAMPLES (4 * (256))

//! Type for version
#define PINGPONG 2
#define TRIPPLEBUF 3

// Extra guard period in GSM symbols required at the end of SYNC_RX & ASYNC_RX bursts in DSDS
#define GFW_CXM_RX_EXTRA_GUARD_PERIOD  12

typedef enum GfwTargetType
{
  GFW_TARGET_GENESIS = 0,
  GFW_TARGET_NIKEL,
  GFW_TARGET_DIME,
  GFW_TARGET_TRITON,
  GFW_TARGET_BOLT,
  GFW_TARGET_UKKNOWN
} GfwTargetType;

//!
typedef struct
{
  uint16 branch;
  uint16 major;
  uint16 minor;
  uint16 patch;
}GfwVersionNumType;

//!
typedef struct
{
  GfwTargetType target;
  GfwVersionNumType number;
  char topFwVersion[25];
}GfwVersionType;


//!Type for IQ Sample Debug buffer.


//!Type indicating burst number after wakeup
typedef enum
{
   GFW_DEL_WAKEUP_BURST_FIRST = 0, //Indicating no delayed wake up more for code readability
   GFW_DEL_WAKEUP_BURST_SECOND,    //Wakeup on 2nd frame
   GFW_DEL_WAKEUP_BURST_THIRD,     //Wakeup on 3rd frame
   GFW_DEL_WAKEUP_BURST_FOURTH     //Wakeup on 4th frame
}GfwRxDelWkupFno;

typedef struct
{
  union
  {
    uint8 controlWord;
    struct
    {
      uint8 enableRxd     :1; //!< Enable Rx Diversity
      uint8 enableDrxOnly :1; //!< Use only Drx - for debug/FTM mode
      uint8 enablePrxOnly :1; //!< Use only Prx - for debug/FTM mode
      uint8 unused        :5;

    };
  };
}GfwRxdControlType;

//!Type indicating desense channel type
typedef enum
{
  NOT_DESENSE_CHAN     = 0, // No desense channel handling required
  DESENSE_CHAN         = 1, // Current channel is a desense channel
  DESENSE_CHAN_PLUS_1  = 2, // Current channel is desense channel + 1
  DESENSE_CHAN_MINUS_1 = 3, // Current channel is desense channel - 1
} GfwRfDesenseChanType;


/*---------------------------------------------------------------------------

           Data structures and definements for FCCH detection

  -------------------------------------------------------------------------*/

//! Number of FCCH results in fcchResultBuff
#define MAX_FCCH_RES 8
//! Number of SCH results in schResultBuff
#define MAX_SCH_RES 3

//!
typedef enum GfwChanType
{
  GFW_TCH_FS         = 0,
  GFW_TCH_EFS        = 1,
  GFW_TCH_F96        = 2,
  GFW_TCH_F144       = 3,
  GFW_SACCH          = 4,
  GFW_SDCCH          = 4,
  GFW_PCH            = 4,
  GFW_FACCH          = 5,
  GFW_SCH            = 6,
  GFW_RACH           = 7,
  GFW_PRACH          = 7,
  GFW_PDTCHU_CS1     = 8,
  GFW_PDTCHU_CS2     = 9,
  GFW_PDTCHU_CS3     = 10,
  GFW_PDTCHU_CS4     = 11,
  GFW_PDTCHD         = 12,
  GFW_PACCHU         = 13,
  GFW_PRACH_EXT      = 14,
  GFW_PACCHU_EXT     = 15,
  GFW_PTCCHU         = 16,
  GFW_PTCCHU_EXT     = 17,
  GFW_F96_AND_FACCH  = 18,
  GFW_F144_AND_FACCH = 19,
  GFW_TCH_HS         = 20,
  GFW_TCH_AFS        = 21,
  GFW_TCH_AHS        = 22,
  GFW_TCH_WFS        = 23,
  GFW_FACCH_WFS      = 24,
  GFW_PTCCHD         = 28,
  GFW_BCCH           = 29,
  GFW_NBCCH          = 29,
  GFW_FACCH_HS       = 30,
  GFW_FACCH_AFS      = 31,
  GFW_FACCH_AHS      = 32,
  GFW_PDTCHU_MCS1    = 33,
  GFW_PDTCHU_MCS2    = 34,
  GFW_PDTCHU_MCS3    = 35,
  GFW_PDTCHU_MCS4    = 36,
  GFW_PDTCHU_MCS5    = 37,
  GFW_PDTCHU_MCS6    = 38,
  GFW_PDTCHU_MCS7    = 39,
  GFW_PDTCHU_MCS8    = 40,
  GFW_PDTCHU_MCS9    = 41,
  GFW_INVALID_CHAN_TYPE = 42
} GfwChanType;

//!
typedef enum GfwMdspCmdType
{
  //leave zero empty, as suring debug it is nice to see a value
//burst receive/transmitt commands
  GFW_CMD_OFFSET                          = 0x100,  //!<commands start from an offset of
  //!< 0x100 to make it easy to identify
  //!< commands in the command buffer.
  GFW_GSM_TX_CMD                          = 0x101,
  GFW_SYNC_RX_CMD                         = 0x102,
  GFW_SCH_CMD                             = 0x103,
  GFW_START_ACQ_CMD                       = 0x104,
  GFW_STOP_ACQ_CMD                        = 0x105,
  GFW_GPRS_PWR_MSR_CMD                    = 0x106,

  // LTE/UMTS -> GERAN IRAT (Compressed mode) commands
  GFW_X2G_STARTUP_CMD                     = 0x107,
  GFW_X2G_CLEANUP_CMD                     = 0x108,
  GFW_X2G_PWR_MSR_CMD                     = 0x109,

  // GERAN -> LTE/UMTS IRAT reselection commands
  GFW_G2X_STARTUP_CMD                     = 0x10A,
  GFW_G2X_CLEANUP_CMD                     = 0x10B,

  //configuration commands
  GFW_CODEC_SET_CONFIGURE_CMD             = 0x10C,
  GFW_CLEAR_SACCH_XCCH_DEINTERLEAVER_CMD  = 0x10D,
  GFW_CONFIGURE_IR_WINDOW_CMD             = 0x10E,
  GFW_GENERIC_CONFIG_CMD                  = 0x10F,
  GFW_FLUSH_ESAIC_IIR_FILTER_CMD          = 0x110,
  GFW_NON_SBI_REGISTER_WRITE_CMD          = 0x111,
  //GFW_PEEK_CMD                            = 0x112,
  GFW_START_CONTINUOUS_TX_CMD             = 0x113,
  GFW_STOP_CONTINUOUS_TX_CMD              = 0x114,
  //GFW_PEEK_PA_LUT_CMD                     = 0x115,
  GFW_LOOPBACK_CMD                        = 0x116,
  GFW_CIPHER_CONTROL_CMD                  = 0x117,
  GFW_TX_CAL_CMD                          = 0x118,
  GFW_SET_APP_MODE_CMD                    = 0x119,
  GFW_EXIT_CMD                            = 0x11A,
  GFW_CONFIGURE_TX                        = 0x11B,
  GFW_AFC_UPDATE_CMD                      = 0x11C,
  GFW_ENTER_MODE_CMD                      = 0x11D,
  GFW_SET_TX_BAND_CMD                     = 0x11E,
  GFW_X2G_PRELOAD_CMD                     = 0x11F,
  GFW_ASYNC_RX_CMD                        = 0x120,
  GFW_IP2_CAL_CMD                         = 0x121,
  GFW_INVALID_CMD_TYPE                    = 0x122
} GfwMdspCmdType;

// Schedule result bit definition
#define  TYPE1_ERROR_BIT     (1 << 0)
      // Sync Rx was knocked out if host scheduled a TX in same TS
#define  TTB_TRB_ERROR_BIT   (1 << 1)
      // Sync Rx was knocked out if Trb/Ttb condition not meet
#define  BURST_SUM_ERROR_BIT (1 << 2)
      // Sync Rx was knocked out if burst sum exceeds
#define  TTA_ERROR_BIT       (1 << 3)
      // Sync Rx was knocked out if tta not meet when sched pwr msr
#define  ACQ_KNOCK_RX_BIT    (1 << 4)
      // Sync Rx was knocked out by ACQ
#define  RX_KNOCK_TX_BIT     (1 << 5)
      // Tx was knocked out as there is a conflict with Rx on other SUB
#define  HOST_KNOCK_RX_BIT   (1 << 6)
      // Sync Rx was knocked out by HOST
#define  TX_KNOCK_RX_BIT     (1 << 7)
      // Rx was knocked by Tx on other sub
#define  TX_KNOCK_TX_BIT     (1 << 8)
      // Tx was knocked by Tx on other sub
#define  RX_KNOCK_RX_BIT     (1 << 9)
      // Rx was knocked by Rx on other sub
#define  RX_KNOCK_MON_BIT    (1 << 10)
      // Mon was knocked by Rx on other sub
#define  TX_KNOCK_MON_BIT    (1 << 11)
      // Mon was knocked by Tx on other sub
#define  HOST_ERROR_3        (1 << 12)
      // Host try to schedule a host pwr msr on an existing pwr msr
#define  HOST_ERROR_2        (1 << 13)
      // Host try to schedule a Tx on a existing pwr msr burst
#define  HOST_ERROR_1        (1 << 14)
      // Host try to schedule a sync rx on existing burst
#define  RX_SCHEDULED_BIT    (1 << 15)
      // Rx is scheduled
#define  SCH_SCHEDULED_BIT   (1 << 16)
      // SCH is scheduled
#define  EARLY_DEC_SUCESS_CANCEL_RX (1<<25)
      //Cancel SyncRx for this case as we had early decode success 
#define  BLANK_LAST_BURST_CANCEL_RX (1<<26)
      //Cancel SyncRx for last burst as first three bursts got blanked
#define  ASYNC_KNOCK_RX (1<<27)
      //burst not received because RF returns error
#define  RF_NOT_TUNED (1<<28) 

/*! @brief Action that needs to be taken due to possible
  Antenna Switch Diversity conflict
  */
typedef enum
{
  /*! No Action Taken - Default Type */
  GFW_CXM_ASDIV_ACT_NONE = 0,
  /*! Register with aux type as asdiv */
  GFW_CXM_ASDIV_ACT_REG,
  /*! Blank the burst due possible asdiv conflict */
  GFW_CXM_ASDIV_ACT_BLANK
} gfw_cxm_asdiv_act_type_e;

//!
typedef struct
{
  union
  {
    uint32 reg;                        /* abstract register id */
    uint32 registerType;
  };
  union
  {
    int16  start_delta;                /* start delta          */
    int16  startDelta;
    uint16 data_h;                     /* ms uint16 of data      */
    uint16 dataHigh;
  };
  union
  {
    int16  stop_delta;                 /* stop delta           */
    int16  stopDelta;
    uint16 data_l;                     /* ls uint16 of data      */
    uint16 dataLow;

  };
}GfwGrfcItem;

//!
typedef GfwGrfcItem  mdsp_nonsbi_timed_ctl_unit;
typedef GfwGrfcItem  mdsp_nonsbi_ctl_unit;


/*! NON sbi buffer defines
   This structure defines the layout of SBI buffers used by SW/FW.

*/
typedef struct
{
  uint16 numberTransactions;
  GfwGrfcItem grfc[MDSP_MAX_NONSBI_TRANSACTIONS];
}GfwGrfcBuffer;


//!
typedef struct
{
  uint16 tsc             :3;
  uint16 dummy           :5;
  uint16 chanDesignation :8;
} GfwSyncRxCmdResWord0;

//!
typedef struct
{
  uint16 fcchAfcFlag  :1;
  uint16 rotatorFlag   :1;
  uint16 xoFlag :1;
  uint16 dummy :13;
} GfwStartAcqCmdAfcWord;


//! @brief start_acq_cmd buffer structure
typedef struct
{
  GfwMdspCmdType cmdType;
  uint16 timeOffset;
  uint16 startIndexMsw;
  uint16 startIndexLsw;
  uint16 schDecodeFlag;
  GfwStartAcqCmdAfcWord fcchAfcFlag;
  int32 phaseIncrement;
  boolean enableAeqOnSch;
  uint32  rfBufIndex;
  boolean logIq;
  uint8 iqBufferIndex; // iq sample buffer index, 0 or 1
  boolean enablePartToneDet;
  uint32  cxmPriority;
  uint32  cxmDesenseId;
  uint16  arfcn;
  boolean doEnhancedFCCH;
  boolean doJdetReading;  
  uint32  rfSeqNr;  
} GfwStartAcqCmdBufStruct;


//! @brief stop_acq_cmd buffer structure
typedef struct
{
  GfwMdspCmdType cmdType;
  uint16 timeOffset;
  uint32 rfBufIndex;
} GfwStopAcqCmdBufStruct;


typedef struct
{
  unsigned int decodeFlag :1;
  unsigned int            :2;
  unsigned int tsc        :5;
  unsigned int swidth     :8;

} GfwSchCmdTscWord;


//! Rx Diversity Log Packet
typedef struct
{
  uint16 len;           //!< Packet length inclusive of len */
  uint16 seqNum;        //!< Sequence number for the current sync rx cmd
  uint32 rssiPRx;       //!< RSSI for PRx
  uint32 rssiDRx;       //!< RSSI for DRx
  int32  rssiPRxdBm;    //!< RSSI for PRx computed in FW in dBm
  int32  rssiDRxdBm;    //!< RSSI for DRx computed in FW in dBm 
  int16  gainPRxQ4;     //!< Gain adjustment for PRx
  int16  gainDRxQ4;     //!< Gain adjustment for DRx
  int16  dcIPRx;        //!< DC I for PRx
  int16  dcQPRx;        //!< DC Q for PRx
  int16  dcIDRx;        //!< DC I for DRx
  int16  dcQDRx;        //!< DC Q for DRx

  union
  {
    uint16 val;

    struct
    {
      uint16 modDet       :2; //!< Detected modulation scheme
      uint16 aciIdxPRx    :2; //!< ACI index for PRx
      uint16 aciIdxDRx    :2; //!< ACI index for DRx
      uint16 rxdOn        :1; //!< Flag to indicate RxD was enabled
      uint16 rxdSelStream :2; //!< Selected stream for RxD - 0:comb 1:pri 2:sec
      uint16 rxdCtrl      :3; //!< RxD Control
      uint16              :4; //!< unused bits
    };     
  }rxdMisc;

  int16  freqOffsetPRx;
  int16  timingOffsetPRx;
  uint16 snrPRxQ8;

  uint32 rxdCorr;
  uint32 filtRxdCorr;

  uint32 unused1;
  uint32 unused2;
  uint32 unused3;
  uint32 unused4;
} GfwRxdLogPacket;

//! AEQ Phase 2A Log packet for 0x508F
typedef struct
{
  boolean bAeqPh2aOn;
  int8    normShiftAmtPreAci;
  int8    normShiftAmtPostRXF;
  int16   LsDcH0;
  int16   LsDcH1;
  int16   LsDcH2;
  int16   LsDcH3;
} GfwAeqPh2aLogPacket;

//! DFT ACI Log packet for 0x508F
typedef struct
{
  boolean bEnableCx4;
  uint16  bMap;
  uint16  bMapPruned;
  uint32  aciDftMetricsLinear[4];
  uint32  parMetricsLinear[4];
  int32   aciDftMetricsDb[4];
  uint32  variance;
  boolean bSkipFwDec7;
  boolean bColouredNoise;
  uint8   aciIndex;
  uint32  kEnergy[4];
  boolean isCompensated[4]; 
  uint8   gainState;
} GfwDftAciLogPacket;

//! AEQ log packet for GMSK
typedef struct
{
  uint16 preMimoSnr;

  struct
  {
     int8 syms;
     int8 qtrSyms;
  } timing;

  uint16 unused2;

  struct
  {
    int16 rssi    : 8; /// burst metrics has this
    int16 zeroSd  : 8;
  } postMimoCompleteSvanr;

  union
  {
    int16 val;
    struct
    {
      uint16 numIter           : 2;
      uint16 schMode           : 1;
      uint16 schCrc            : 1;
      int16  estFreqDiv100     : 4;
      uint16 chosenStream      : 1;
	  uint16 spurRemoval       : 1;
      uint16 unused            : 2;
      uint16 aeq4Enabled       : 1;
      uint16 hybAEQRsseAct     : 1;
      uint16 freqSearchEnabled : 1;
      uint16 aeqEnabled        : 1;
    };
  } reservedSaicWord0;

  struct
  {
    uint16 esaicDelta   : 4;
    uint16 convCorrPeak : 4;
    uint16 corrPeak     : 4;
    uint16 searchWidth  : 4;
  } preMimoTscSnr0;

  struct
  {
    uint16 iirNotCopied : 1;
    uint16 iirFlushed   : 1;
    uint16 iirDisabled  : 1;
    uint16 hybridEnabled: 1;
    uint16 convCirRef   : 4;
    uint16 eMDon        : 1;
    uint16 eMDforced    : 1;
    uint16 freqHypIdx   : 3;
    uint16 modulation   : 1;
    uint16 unused       : 2;
  } postMimoTscSnr0;

  union
  {
      uint16 val;
      struct
      {
         uint16 vamosAqpskSNR     : 8;
         uint16 vamosAqpskDetected: 1;
         uint16 vamosTscIdx       : 4;
         uint16 unused            : 2;
         uint16 vamosSupport      : 1;
      };
  } reservedSaicWord1;

  
  struct
  {
  	//log packet for hybridAEQ
	struct
	{  
     	uint16 RSSEburstSnr; //snr used in the comparison AEQ/RRSE   Q(12,4)
     	uint16 AEQburstSnr;  //snr used in the comparison AEQ/RRSE. this SNR refers to the first iteration Q(12,4)
	 }hybridAEQword;
	 
     uint16 unused1;
     uint16 unused2;
     uint16 unused3;
     uint16 unused4;
  }reservedSaicWord2;
  

} GfwAeqGmskLogPacket;

//!< List of all the possible AEQ log packets (GMSK, 8PSK, ...)
typedef union
{
  GfwAeqGmskLogPacket aeqGmskLogPacket;

} GfwAeqLogPacket;

//! VAMOS log structure
typedef struct
{
   union
  {
    uint32 word0;
      struct
      {
         uint32 vamosAqpskSNR     : 8;
         uint32 vamosAqpskDetected: 1;
         uint32 vamosTscIdx       : 4;
         uint32 vamosSupport      : 2;           
         uint32 skipVss           : 1;
         uint32 paired            : 1;
         uint32 switchAciFilt     : 1;      
         uint32 SCPIR             : 8;
         uint32 unused            : 6;
      };
  } vamosMetWord0;

  uint32 s2SnrQ8;
  uint32 chEnS1;
  uint32 chEnS2;
  int64  s2Cir[5];

} GfwVamosLogType;

//! Structure to receive metrics from mDSP
typedef struct
{
  uint16        len;           //!< Packet length inclusive of len
  uint16        seq_num;
  uint16        pdch;          //!< Only set for GPRS bursts
  uint16        rssi_h;
  uint16        rssi_l;
  int16         dc_offset_i;
  int16         dc_offset_q;
  int16         freq_offset;
  int16         timing_offset;
  uint16        snr;
  uint16        jdetReading;

  GfwAeqLogPacket aeqLogPacket;
  GfwRxdLogPacket rxdLogPacket;

} GfwCommonBurstMetricsStruct;

/* RSB estimation Type*/
typedef struct
{
  int32 accISqr; //t1
  int32 accQSqr; //t2
  int32 accIQ;   //t3
} GfwRsbEstimation;

/* WBDC/WBEE Est Type*/
typedef struct
{
  uint32 wbPwrEstRaw; // Raw WBEE estimation
  uint32 wbPwrEst; // Wbdc compensated WBEE estimation
  int32  wbdcEstI; // Wbdc Estimation I Coeff
  int32  wbdcEstQ; // Wbdc Estimation Q Coeff
} GfwWbeeWbdcEst;

/* Lif Burst Metrics */
typedef struct
{
  GfwRsbEstimation rsbEstimation;
  GfwWbeeWbdcEst   wbeeWbdcEstPRx;
  GfwWbeeWbdcEst   wbeeWbdcEstDRx;
  int32            curLifFreqKhz;
  int32            nextLifFreqKhz;
  int32            rsbImgAciMetric;
  boolean          aci400Detected;
} GfwLifBurstMetricsStruct;

/* decoded RX message packet */
typedef struct
{
  uint16   len;
  uint16   seq_num;
  uint16   control[2];
  uint16   rx_qual_hard;
  uint16   rx_qual_soft;
  uint16   data_1[MAX_RX_DATA_PER_FRAME];
  uint32   reason;
} GfwSchDecodeDataStruct;


//! PhaseIncrement and Channel ID control word in schCmd
typedef struct
{
  uint16 chanId      : 6;
  uint16             : 4;
  uint16 usePhaseInc : 1;
} GfwSchCmdCtrlWord;


//! sch_cmd buffer structure
typedef struct
{
  GfwMdspCmdType cmdType;
  GfwSchCmdCtrlWord ctrlWord;
  GfwSchCmdTscWord sw_tsc_word;
  uint8 burstMetricsBufIndex;
  uint8 schDecodeDataBufIndex;
  uint16 time_offset;
  int32 phaseIncrement;
  boolean enableAeqOnSch;
  GfwRxdControlType rxdControl;
  uint32  rfBufIndex;
  boolean logIq;
  uint8 iqBufferIndex; // iq sample buffer index, 0 or 1
  uint32  cxmPriority;
  uint32  cxmDesenseId;
  uint16  arfcn;
  boolean doJdetReading; 
  uint32  rfSeqNr;  
} GfwSchCmdBufStruct;


//!  Structures used within gsmSyncRxCmd buffer structure
typedef union
{
  struct
  {
    uint16 decodeFlag   :1;
    uint16 bSbDecNullPage :1;
    uint16 enableEPD    :1;  //this flag has to be populated by L1 according to the page sharing detection 
    uint16 aciDetect    :1;
    uint16 forceFacchCs1:1;
    uint16 earlyDecode  :1;
    uint16 lookForFacch :1;
    uint16 usePhaseInc  :1;
    uint16 cmiPhase     :1;
    uint16 firstBurst   :1;
    uint16 lookForRfacch :1;
    uint16 rFacchPingPong :1; //RFACCH2_0
    uint16 lookForRsacch :1;
    uint16 prioritySchedule  :1; // 1=schedule; 0=blank
    uint16 amrPd        :1;   // Enable AMR_PD for each sync rx burst from GL1
    uint16 dtxDetect    :1;   // Do DTX detection for this block
  };
  uint16 ctrlField;

} GfwSyncRxSlotCtrlField;

//!
typedef struct
{
  uint16       tsNum    :3;
  uint16                :5;
  GfwChanType  chanType :8;


} GfwChanTsField;

//! Structures used within gsmSyncRxCmd buffer structure
typedef struct
{
  GfwChanType chanType;
  uint8       usePhaseIncrement;
} GfwAsyncRxChannelField;

//! Structures used within gsmSyncRxCmd buffer structure
typedef struct
{
  uint16      searchWidth;
  boolean     decodeFlag;
} GfwAsyncRxControlField;

//! SAIC control word structure (part of the gsm_sync_rx_cmd)

typedef union
{
    struct
{
  uint16 enableSaic     :1; //!< Enable SAIC bit --NEEDS CLEAN-UP: THIS IS OBSOLETE on this target
  uint16 shortSaic      :1; //!< Enable short SAIC --NEEDS CLEAN-UP: THIS IS OBSOLETE on this target
  uint16 gainRangeFlag  :1; //!< --NEEDS CLEAN-UP: NOT USED ANYWHERE
  uint16 suspendIir     :1; //!< eSAIC flag to disable IIR
  uint16 enableESaic    :1; //!< Enable eSAIC --NEEDS CLEAN-UP: THIS IS OBSOLETE on this target
  uint16 enableAeq      :1; //!< Enable AEQ Phase I  (GMSK)
  uint16 enableAeq2     :1; //!< Enable AEQ Phase II (8PSK)
  uint16 enableAeq2a    :1; //!< Enable AEQ Phase IIA (8PSK Adaptive)
  uint16 enableAeq4     :1; //!< Enable AEQ Phase 4 (Voice)
  uint16 vamosTscSet    :1; //!< VAMOS TSC set indicator
  uint16 vamosSACCH     :1; //!< VAMOS SACCH Indicator
  uint16 spurChanFlag   :2; //!< spur channel type
  uint16                :3; //!< UNUSED BITS
};
  uint16 ctrlWord;

} GfwSaicCtrlWordStruct;


typedef union
{
  //! Decoded Control Word1 Structure
  struct
  {
    boolean facch          :1;   //!< FACCH Bit
    boolean rsacchStoreStatus :1;//!< R-SACCH flag to indicate whether CKs stored for recombination
    boolean ratScchDecode  :1;   //!< ratScchDecode Bit
    boolean ratScchMarker  :1;   //!<  Bit 4
    boolean sidFirst       :1;   //!<  Bit 5
    uint16 hrVoicingModeOrSidParams :2; //!< 6th bit sid update 7th bit onset /6th and 7th hr voicing mode
    boolean inhibit        :1;   //!< Bit 8
    boolean rfacchCombStatus :1; //!< Flag to indicate whether R-FACCH recombination has been done
    boolean rfacchStoreStatus :1;//!< R-FACCH flag to indicate whether CKs stored for recombination
    uint16 rsacchCombStatus :1;  //!< Flag to indicate whether R-SACCH recombination has been done
    boolean phase          :1;   //!< Bit 12
    uint16 cmi             :2;   //!< Bits 13 and 14
    uint16 cmc             :2;   //!< Bits 15 and 16 bits
  };

  uint16 ctrlWord1;

} GfwDecodedCtrlWord1Struct;

//! @ Decoded Control Word2 Structure
typedef struct
{
  boolean crc2           :1; //!< CRC for Message 2
  boolean crc1           :1; //!< CRC for Message 1
  boolean Ir1            :1; //!< IR Flag for Data 1
  uint16  CCS            :4; //!< Bits 4 to 7 for CCS
  boolean crcH           :1; //!< Header CRC
  boolean Ir2            :1; //!< IR Flag for Data 2
  boolean Ir1W           :1; //!< IR Write Flag
  boolean Ir2WOrFireCode :1; //!< BIT 11 to indicate Ir2W or Firecode
  boolean Ir1DatabaseFull :1; //!< BIT 12 = true if CRC failed, database is full, so cannot store Ck (payload1)
  boolean Ir2DatabaseFull :1; //!< BIT 13 = true if CRC failed, database is full, so cannot store Ck (payload2)
  uint16 pdch            :3; //!< PDCH Number: Bits 14 to 16
} GfwDecodedCtrlWord2Struct;

//! eSACCH Log Structure
typedef struct
{
  int32   corrSI5;    //!< Correlation result for SI5
  int32   corrSI6;    //!< Correlation result for SI6
  int32   corrSI5ter; //!< Correlation result for SI5ter
  int32   corrSI5bis; //!< Correlation result for SI5bis
  int32   corrMI;     //!< Correlation result for MI
  boolean bCorrPassed;//!< Flag to indicate correlation passed or failed
  uint8   octet3;     //!< Octet3 of decoded SACCH message
  uint8   octet7;     //!< Octet7 of decoded SACCH message
  uint8   bestSnrIdx; //!< Index for the burst in the block with best SNR

  union
  {
    uint8 dummy;
    struct
    {
      uint16 si5    : 1; //!< SI5 found
      uint16 si6    : 1; //!< SI6 found
      uint16 si5bis : 1; //!< SI5bis found
      uint16 si5ter : 1; //!< SI5ter found
      uint16 MI     : 1; //!< MI found
      uint16 SMS    : 1; //!< SMS found
      uint16 missed : 1; //!< SI missed - unknown pattern
      uint16 unused : 1;
    };
  } esacchDetPattern;

} GfwEsacchLogType;

//! Enhanced EDGE decoder logging
//! message log packet
typedef union
{  
  uint32 enhEdgeDecMsgLog;
  struct
  {
    uint32 algDecUsed       :1; // Flag to indicate algebraic decoder was used
    uint32 algDecCrc        :1; // Algebraic decoder CRC
    uint32 algDecType       :1; // 0: low SNR alg decode - 1:high SNR alg. decode
    uint32 mcs              :4; // mcs for pattern selection
    uint32 puncPattern      :2; // P1/P2/P3 for pattern selection
    uint32 highSnrThresh    :1; // 1 if above threshold for pattern selection
	uint32 freeLocations    :9; // Number of free locaitons in the database
	uint32 bsn			    :11;// bsn number
    uint32 unused           :2;
  };
} GfwEnhancedEdgeMsgLogPacket;

//! header log packet
typedef union
{ 
  uint32 enhEdgeDecHdrLog;
  struct 
  {
    uint32 earlyHdrDecPass     :1; // Flag to indicate that early hdr dec. passed
    uint32 mcsGroupNum         :2; // 0: mcs1-4, 1: mcs5-6 or 2: mcs7-9
    uint32 modScheme           :1; // voted modulation type during the early decode
    uint32 earlyHdrNumBursts   :2; // number of bursts used, e.g. 2 or 3
    uint32 earlyHdrTfiMatched  :1; // Flag to indicate if the Tfi matched or not
    uint32 earlyPydNumBursts   :2; // number of bursts for payload decode
    uint32 earlyPydDecPass     :1; // Flag to indicate that early payload dec. passed
    uint32 unused              :22;
  }; 
} GfwEnhancedEdgeHdrLogPacket;


//! R-FACCH Log Structure
typedef struct
{     
  uint8  storeStatus     : 1;
  uint8  combStatus      : 1; 
  uint8  clearStatus     : 1;
  uint8  crcStatus       : 1;
  uint8  buffId          : 1;  
  uint32 snrStored;
  uint32 snrCurrent;
  uint16 rFacchFNL1;
  uint16 rFacchFNFW;
} GfwRfacchLogType;



//! Enhanced Edge Log Packet ( header + (message1, message2) )
typedef struct
{

  GfwEnhancedEdgeHdrLogPacket logPktHdr;  
  GfwEnhancedEdgeMsgLogPacket logPktMsg[2];

} GfwEnhancedEdgeLogPacket;

//! AEQ Phase 4 log info for each burst
typedef struct
{
   uint16   burstSnrQ8;           // Burst SNR for current burst
   uint32   numUpdatedBits  : 6;  // Number of bits updated in S for the current burst
   int32    snrImprovement  : 2;  // 1: SNR improvement, -1: SNR degradation, 0: same SNR
   uint32   frameNumber;          // Frame number
} GfwAeq4BurstInfo;

//! AEQ Phase 4 log info for each iteration
typedef struct
{
   GfwAeq4BurstInfo burstInfo[4];         // Information for each burst of the current iteration
   uint16           rxQual;               // RXQUAL for current iteration
   uint16           numUpdatedBursts : 3; // Number of bursts with modified S
   uint16           crc              : 1; // CRC for current iteration
   uint16           replacedMsg      : 1; // the re-decoded msg has replaced the original
   uint16                            : 3;
} GfwAeq4IterInfo;

//! AEQ Phase 4 log packet
typedef struct
{
   uint16   enabled      : 1;   // TRUE if AEQ Phase 4 is enabled
   uint16   validChan    : 1;   // TRUE if channel is supported
   uint16   numIter1     : 2;   // N1 iterations
   uint16   numIter2     : 2;   // N2 iterations
   uint16   crc0         : 1;   // CRC of first decode
   uint16                : 1;
   uint16   numDecodes   : 2;   // Number of decode attemps
   uint16   chanType     : 6;   // Channel type
   uint16   amrCodecRate : 4;   // AMR codec rate
   uint16                : 4;
   uint16   rxQual0;            // RXQUAL of first decode
   uint16   burstSnr0Q8[4];     // Burst SNR's from first equalization
   GfwAeq4IterInfo iterInfo[2]; 
} GfwAeq4LogPacket;


//! EPD correlation data struct 
typedef struct{
   uint8   maxCorrIndex;
   int16   maxCorr;
   int16   corrThresh;
}GfwEpdCorrDataStruct;
//! EPD log packet struct 
typedef struct{
   struct
   {
      uint8 bDoFullDecode :1;
      uint8 bValidPage    :1;
      uint8 bValidHeader  :1;
      uint8 bAeq          :1;
      uint8 unused        :4;
   };
   uint16  burstSnr;
   GfwEpdCorrDataStruct headerCorrData[3];
   GfwEpdCorrDataStruct xmsiCorrData[3];
}GfwEpdLogPacket;

//! DFT based SPUR handling dedicated log packet
typedef struct
{
   int32  Fspur_rough[4];   
   int32  Fspur_fine[4];
   int16  A[4];
   int16  phi[4];
   int16  Nbar_rough[4];
   int16  Nbar_fine[4];
   int16  a[4];
   uint16 kEnergy[4];

} GfwDftSpurMetricsStruct;

//! DFT based ACI sensing dedicated log packet
typedef struct
{  
  union
  {
    uint32 value;
    struct
    {
      uint32 bEnableCx4     :1;
      uint32 bColouredNoise :1;
      uint32 bSkipFwDec7    :1;
      uint32 bPerform40taps :1;
      uint32 bMap           :4;
      uint32 bMapPruned     :4;
      uint32 aciIndex       :4;
      uint32                :4;
      uint32 isComp_m400    :1;
      uint32 isComp_m200    :1;
      uint32 isComp_p200    :1;
      uint32 isComp_p400    :1;
      uint32 unused         :8;      
    };
  };

  int16  parMetricsDb[4];
  int16  aciDftMetricsDb_C_over_I[4];
  uint16 varianceDb;  

  GfwDftSpurMetricsStruct gfwDftSpurMetrics;
   
} GfwDftSpurAciMetricsStruct;

//! Decoded Data Structure
typedef struct
{
  uint16   len;    //!<
  uint16   seqNum; //!<
  GfwDecodedCtrlWord1Struct gfwDecodedCtrlWord1; //!<
  GfwDecodedCtrlWord2Struct gfwDecodedCtrlWord2; //!<
  int16    hardRxQual;      //!<
  int16    softRxQual;      //!<
  uint16   hrateSumC1SumC2; //!<
  uint16   numSoftDec;      //!<
  uint16   data1[MAX_RX_DTM_DATA_PER_FRAME]; //!<

  uint16   egprs_hdr[MDSP_RX_EGPRS_HDR_SIZE];//!<
  uint16   data2[MAX_RX_DTM_DATA_PER_FRAME]; //!<
  uint16   reserved[5];                      //!<
  GfwEsacchLogType esacchLogBuff;            //!<
  GfwEnhancedEdgeLogPacket enhEdgeLogPkt;    //!<
  GfwAeq4LogPacket aeq4LogPacket;
  boolean bVamosPaired;
  GfwEpdLogPacket epdLogPacket;
  GfwRfacchLogType rfacchLogBuff;  
} GfwDecodedDataStruct;

//! Sync RX Control Field Structure
typedef struct
{
  uint16        :4;    //!<
  uint16 genCsprng :1; //!<
  uint16 tsc    :3;    //!<
  uint16 uncWin :8;    //!<
  uint16 arfcn;
} GfwSyncRxCtrlField;

//! Sync RX slot Structure
typedef struct
{
  uint8 decodedDataBufferIndex;
  GfwSyncRxSlotCtrlField    controlField;   /* burst control fields */
  GfwChanTsField            chanTsField;   /* channel type and timeslot */
  uint16                    offset;         /* offset to center of burst */
  GfwSaicCtrlWordStruct     saicControl;    /* SAIC control information */
  GfwRxdControlType         rxdControl;     /* Rx Diversity control information*/
  boolean                   logIq;
  boolean                   logNewIq;
  uint32                    cxmPriority;
  uint32                    cxmDesenseId;
  gfw_cxm_asdiv_act_type_e  cxmActAsDiv;
} GfwSyncRxSlotStruct;

//! IQ LOG Debug Buffer Structure
//! @brief GERAN FW RxFE driver supported burst types.
typedef enum GfwRxFrontEndBurstType
{
   GFW_RXFE_BURST_NORMAL,
   GFW_RXFE_BURST_ACQ ,
   GFW_RXFE_BURST_PWR_MSR,
   GFW_RXFE_BURST_SCH,
   GFW_RXFE_BURST_LN_PWR_MSR,
   GFW_RXFE_BURST_STOP_ACQ,       //this is not a burst as such.
   GFW_RXFE_BURST_PWR_SCAN,
   GFW_RXFE_BURST_IP2_CAL,
   GFW_RXFE_BURST_TYPE_MAX
} GfwRxFrontEndBurstType;

typedef struct
{
  /* Number of int16 elements present in the buffer ('numSamples' excluded and seq num excluded) */
  uint16 seqNum;   //!< Sequence Number
  int16 numSamples;//!< Number of samples to be logged per burst
  int16 samples[MAX_NUM_IQ_SAMPLES]; //!< Buffer to store the IQ samples
  GfwRxFrontEndBurstType logType;
  uint8 slotNumber;
} GfwIqSamplesBuffer;


#define GFW_MAX_BURSTS_PER_FRAME  MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS

typedef  struct
{
     GfwIqSamplesBuffer buffer[PINGPONG][GFW_MAX_BURSTS_PER_FRAME];
} GfwIqLoggingMemoryRegion;

//! Burst Metrics Packet Structure
typedef struct
{
  uint16        len;           //!< Packet length inclusive of len */
  uint16        seqNum;
  uint16        pdch;          //!< Only set for GPRS bursts */
  uint16        rssiMsw;
  uint16        rssiLsw;
  int16         dcI;
  int16         dcQ;
  int16         freqOffset;
  int16         timingOffset;
  uint16        snr;
  uint16        cOverI;
  uint16        aci_pwr_0Hz;
  uint16        aci_pwr_p200kHz;
  uint16        aci_pwr_n200kHz;
  struct
  {
    uint16  modDet    :2;
    uint16  aciIndex  :2;
    uint16  brstCancel:1;   
    uint16  unused    :11;     
  };
  uint32 reliabFactorQ16;
  /* SAIC MIMO metrics additions. */
  uint16        pre_mimo_trained_complete_snr;
  uint16        post_mimo_trained_complete_snr;
  uint16        misc_flags;
  uint16        hard_decisions[MDSP_MAX_HARD_DECISIONS];

  GfwLifBurstMetricsStruct gfwLifBurstMetricsStruct;

  GfwAeqLogPacket aeqLogPacket;
  GfwRxdLogPacket rxdLogPacket;
  GfwVamosLogType vamosLogPacket;
  GfwDftSpurAciMetricsStruct gfwDftSpurAciMetrics;
  boolean scheduleTimeError; //!< If set to one, this means there was an error meeting the time deadlline to schedule this burst.
  uint16 jdetReading;
} GfwBurstMetricsStruct;

//! Structure of Burst Scheduler Results  - used with gprs_sync_rx cmd
typedef struct
{
  uint16                    len;          //!< len of packet in words
  uint16                    seqNum;       //!< seq num issued in command
  uint32                    reason[8];    //!< bit 15= True for successful schedule
                                          //!<       = FALSE otherwise,
                                          //!< failure code in bits 0-7
} GfwSchedulerResultStruct;

//! Structure of TX burst Scheduler Results  - used with gsm_tx cmd
typedef struct
{
  uint16                    len;          //!< len of packet in words
  uint16                    seqNum;       //!< seq num issued in command
  uint16                    reason[10];   //!< bit 15= True for successful schedule
                                          //!<       = FALSE otherwise,
                                          //!< failure code in bits 0-7
} GfwSchedulerTxResultStruct;


/*! @brief FW Coexistence manager Activity type defined in gfw , sw wont have these enum 
  this should be done to export these enums to sw 
  */
typedef enum
{
  /*!ACTIVITY_NONE is for initialization only*/
  GFW_CXM_ACTIVITY_NONE         = 0,
  /*! Receive activity*/
  GFW_CXM_ACTIVITY_RX           = 1,
  /*! Transmit Activity*/
  GFW_CXM_ACTIVITY_TX           = GFW_CXM_ACTIVITY_RX << 1,
  /*! For internal use only*/
  GFW_CXM_ACTIVITY_LAST_VAL      = GFW_CXM_ACTIVITY_TX,
} gfw_cxm_act_type_e;

/*! @brief FW Coexistence manager conflict check outcomes defined in gfw*/
typedef enum
{
  /*! This is returned when  there is no activity overlap among RATs.Caller can 
    proceed with what it is doing*/
  GFW_CXM_NO_CONFLICT    = 0,
  /*! This is returned when there is activity overlap among RATs and caller RAT 
    has higher priority and no action is needed*/
  GFW_CXM_PROCEED        = 1,
  /*! This is returned when there is activity overlap among RATs and caller RAT 
    has lower priority than competing RAT over the full/partial period of the
    query interval.Caller RAT may have to do Rx or Tx blanking or none based on
    the type of de-sense action.*/
  GFW_CXM_YIELD          = 2,
  /* This is used when the interval is only registered and there is no query
     associated (eg ACQ, SCH). */
  GFW_REGISTRATION       = 3,
} gfw_cxm_conflict_status_e;

/* to indicate the type of registration*/
typedef enum GfwCxmRegType
{
  GFW_CXM_REG_TYPE_NORMAL,
  GFW_CXM_REG_TYPE_ACQ,
  GFW_CXM_REG_TYPE_GAP
} GfwCxmRegType;

typedef struct
{
  uint32                   startGstmrQs  : 16;                        //!<startGstmr time for the query (in QS)
  uint32                   endGstmrQs    : 16;                         //!<endGstmr time for the query (in QS)
  uint32                   startUstmr    : 27;                       //!<startUstmr time for the query
  uint32                   activity_type : 2;  //!<activity type
  uint32                   startGstmrFn  : 2;
  uint32                   endUstmr      : 27;                         //!<endUstmr time for the query
  uint32                   cxmStatus     : 2;  //!<cxm conflict status
  uint32                   endGstmrFn    : 2;
  uint32                   freq_id;  
  uint16                   priority;                               //!<priority of the activity
  uint16                   arfcn;              //!<ARFCN
  GfwChanType              chanType;           //!<Channel type
  uint16                   seqNum;             //!< seqNum of the command, only used by GL1 to retrieve specific information associated with this burst
  GfwCxmRegType            gfwCxmRegType;      // to indicate the type of registrations
  uint8                    channel_id;
  uint8                    action;
} GfwCxmResultStruct;

typedef struct
{
  uint16                                numEntries;     //!<semaphore, number of entries
  uint32                                gsmFrameNo; //!<current frame number
  GfwCxmResultStruct  cxmResult[10];  //!<cxm info
} GfwCxmLogPacketBuff;

//! Structure of decoded usfs - used with gprs_sync_rx cmd
typedef struct
{
  uint16                    len;         //!< len of packet in words
  uint16                    seqNum;      //!< seq num issued in command
  uint16                    usf[8];      //!< bit 15= True for valid usf
                                        //!<    0-2 = usf value
} GfwUsfBufStruct;

//!
typedef struct
{
  GfwBurstMetricsStruct gfwBurstMetricsBuffers[MDSP_MAX_DL_TS];
} GfwMetricsStruct;


//!  gsm_sync_rx_cmd Buffer Structure
typedef struct
{
  GfwMdspCmdType            cmd;             /* command code   */
  GfwSyncRxCtrlField        controlField;    /* message control field */
  uint8                     metricsBufIndex;
  GfwIqSamplesBuffer        *iqSampleBuffer; /* iq sample buffer */
  GfwIqSamplesBuffer        *iqSampleBufferDrx; /* iq sample buffer */
  uint8                     schedulerResultBufIndex;   /* burst scheduler results buffers */
  int32                     phaseIncrement;/* rotator value for the burst */
  uint16                    numSlots;      /* Number of receive slots */
  GfwSyncRxSlotStruct       slotInfo[MDSP_MAX_DL_TS];   /* Slot control info for mDSP. */
  uint16                    rfSpurBuffer;  /* rf spur buffer */
  uint16                    pdchvsUSFH[8];
  uint32                    rfBufIndex;
  uint8                     iqBufferIndex;  // iq sample buffer index, 0 or 1
  GfwRxDelWkupFno           gfwDelWkupFrNo; // In case of delayed wakeup indicating the burst number of wakeup
  int16                     rxLeveldB;        // Expected signal level in dB
  uint16                    rFacchFN; //RFACCH2_0 frame check  
  boolean                   doJdetReading;
  uint32                    rfSeqNr;  
} GfwGsmSyncRxCmd;

//! DTM TX EGPRS Control Word
typedef struct
{
  uint16              :11;
  uint16 ps1          :2; //!< 12th and 13th bits
  uint16 ps2          :2; //!< 14th and 15th bits
  uint16              :1;
} GfwDtmTxEgprsCtrlWord;

//! DTM TX EGPRS Structure
typedef struct
{
  uint16 len;
  GfwDtmTxEgprsCtrlWord controlField;
  uint16 hdr[3]; 
  uint16 data_1[MAX_TX_DTM_DATA_PER_FRAME];
  uint16 data_2[MAX_TX_DTM_DATA_PER_FRAME];
} GfwDtmTxEgprsStruct;

//!
typedef union
{
  //! AMR Control Word
  struct
  {
    uint16 sidFirstInhibit   :1; /* 1st bit */
    uint16 sidUpdateInhibit  :1; /* 2nd bit */
    uint16 onset             :1; /* 3rd bit */
    uint16 phase             :1; /* 4th bit */
    uint16 cmi               :2; /* 5,6th bits */
    uint16 cmc               :2; /* 7,8th bits */
    uint16 codecRate         :4; /* 9,10,11,12 bits */
    uint16 sidFirst          :1; /* 13th bit */
    uint16 sidUpdate         :1; /* 14th bit */
    uint16 ratscch           :1; /* 15th bit */
    uint16 XXX               :1; /* 16th bit */
  };

  uint16 flag;

} GfwAmrCtrlWord;

//!
typedef struct
{
  uint16 len;
  GfwAmrCtrlWord controlField;
  uint16 data[MAX_TX_DTM_DATA_PER_FRAME];
} GfwDtmTxGsmGprsStruct;

//!
typedef union
{
  uint64                  dummyForAlignment;
  GfwDtmTxEgprsStruct     egprs;
  GfwDtmTxGsmGprsStruct   gsm_gprs;
} GfwDtmTxStruct;

//!
typedef struct
{
  uint16 channelType  :8;
  uint16 E            :1;
  uint16 vamosTscSet  :1;
  uint16 XXX          :2;
  uint16 PDCH         :3;
  uint16 M            :1;
} GfwGsmTxMsgCtrlWord;

//!
typedef struct
{
  GfwGsmTxMsgCtrlWord controlField;

  GfwDtmTxStruct       *dataBuffer; //!< used when encode flag is set
} GfwGsmTxMsgStruct;

//!
typedef struct
{
  uint16                   timeslot;             //!<timeslot number of the burst
  uint16                   offset;               //!<offset to start of burst
  uint32                   cxmPriority;
  uint32                   cxmDesenseId;
  gfw_cxm_asdiv_act_type_e cxmActAsDiv;
  uint16                   arfcn;
  int16                    powerdBm;
  uint16                   prioritySchedule  :1; //!< DSDA: 1=schedule; 0=blank
} GfwGsmTxSlotStruct;

typedef struct
{
  uint16             :1; //!< 1st bit
  uint16 useRotator  :1; //!< 2nd bit

  uint16 loopBack    :1; //!< 3rd bit

  uint16 abTsc       :2; //!< 4,5th bits
  uint16 nbTsc       :3; //!< 6,7,8 bits
  uint16 bsic        :6; //!< 9-14 bits
  uint16             :2;
} GfwGsmTxCmdCtrlWord;

//! gsm_tx_cmd buffer structure.
typedef struct
{
   GfwMdspCmdType      cmd;            // command code.
   GfwGsmTxCmdCtrlWord controlField;   // TSC
   uint16              num_msgs;
   GfwGsmTxMsgStruct   msgInfo[MDSP_MAX_UL_TS];
   uint16              num_slots;
   GfwGsmTxSlotStruct  slotInfo[MDSP_MAX_UL_TS];
   int32               f_hz_q6;
   uint32              inv_f_dl_lo;
   uint32              txlmBufIndex;   // index for txlm 
   uint32              rfBufIndex;
   uint8               schedulerTxResultBufIndex; /* Result buf index */
   boolean             ratmode;   //FALSE in G+G mode True in X+G mode 
   int32               ppm;
   uint32              rfSeqNr;   
} GfwGsmTxCmd;

//! Power measure scheduling type
typedef enum
{
    GFW_PWR_MSR_HOST_SCHEDULED = 0,
    GFW_PWR_MSR_DSP_SCHEDULED,
    GFW_PWR_MSR_SCHEDULING_TYPE_INVALID
} GfwPwrMsrSchedlingType;

//! Power measure control fields
typedef struct
{
    uint8 burstSize;
    GfwPwrMsrSchedlingType type;
    boolean combine;
} GfwPwrMsrCmdCtrlWord;


//! GPRS Power Measure command substruct
typedef struct
{
  uint16  burstNo;               /* Burst number              */
  uint16  offset;                /* Offset: SOB in QS         */
  uint32  rfBufIndex;
  boolean logIq;
  uint32  cxmPriority;
  uint32  cxmDesenseId;
  uint16  arfcn;                //!< arfcn
  boolean skip;                              //!
  boolean doJdetReading;
  boolean debugCrash;  
  uint16  prioritySchedule  :1; //!< DSDA: 1=schedule; 0=blank
  uint32  rfSeqNr;  
} GfwGprsPwrMeasStruct;


//! Structure to receive power measure metrics from mDSP in GPRS mode
typedef  struct
{
  uint16        length;
  uint16        seqNum;
  uint16        timeslots;
  boolean       readyMask[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];
  uint32        rssi[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];
  boolean       scheduleTimeError;
  uint32        reason[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];
  uint16        jdetReading[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];  
} GfwPwrMsrResultBuff;

//! Interval where GFW should try scheduling power measure
typedef struct  
{
    int16 start; // start timing offset (qs). To be useful, must be within [FTSM, FTSM + 5000)
    int16 stop;  // stop timing offset (qs). To be useful, must be be within [FTSM, FTSM + 5000) and >= than start
} GfwPwrMsrAllowInterval;

//! GPRS Power Measure command struct
typedef struct
{
  GfwMdspCmdType        cmdType;               /* Command Type                   */
  GfwPwrMsrCmdCtrlWord  controlField;          /* GFW or host sched length       */
  GfwPwrMsrAllowInterval allowInterval[2];      /* For GFW scheduled monitor only */
  uint8                 pwrMsrResultBuffIndex; /* Power measure metrics buffer   */
  uint16                nBursts;               /* Number of measurements         */
  uint8                 iqBufferIndex;         /* IQ sample buffer index, 0 or 1 */
  GfwGprsPwrMeasStruct  ctrl[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];   /* Structure for the bursts  */
} GfwGsmPwrMsrCmd;


//! TX data mode for various mdsp transmit commands
typedef enum  GfwTxDataModeType
{
  GFW_TX_DATA_FROM_BUFFER = 0,
  GFW_TX_DATA_PSEUDO_RANDOM = 1,
  GFW_TX_DATA_TONE = 2,
  GFW_FORCEWORD_TX_DATA_MODE_TYPE = 3
} GfwTxDataModeType;

//!
typedef struct
{
  GfwTxDataModeType txDataModeType     :2;
  boolean           modulation         :1;/* 1 for GMSK and 0 for 8PSK */
  boolean           useRotator         :1;/* use rotator or not */
  uint16            XXX                :12;
} GfwStartTxCtrlWord;

//! Tx transmit command struct for phonet
typedef struct
{
  GfwMdspCmdType      cmd;                /* command code   */
  GfwStartTxCtrlWord  controlField;       /* tx data mode */
  uint8               dtmTxBufIndex;
  uint16              offset;             /* time offset to start of burst */
  uint32              rfBufIndex;
  uint32              rfSeqNr;
} GfwStartTestTxCmd;

//! Stop Tx continous transmit command for phoneT
typedef struct
{
  GfwMdspCmdType  cmd;                /* command code   */
  uint16          offset;             /* time offset to stop the burst */
  uint32          rfBufIndex;
  uint32          rfSeqNr;  
} GfwStopTestTxCmd;

//! GFW ciphering types
#define GFW_CIPHERING_A5_1 0x0
#define GFW_CIPHERING_A5_2 0x1
#define GFW_CIPHERING_A5_3 0x2
#define GFW_CIPHERING_A5_4 0x3


//! Structure for enabling/disabling ciphering
typedef struct
{
  GfwMdspCmdType        cmd;                //!< command code
  boolean               downlink;           //!< 1 = enable ciphering, 0 = disable
  boolean               uplink;             //!< 1 = enable ciphering, 0 = disable
  uint32                fn;                 //!< FN for ciphering
} GfwCipherControlCmd;

//! nonSBI register write command struct
typedef struct
{
  GfwMdspCmdType        cmd;                /* command code   */
  uint8  grfcBuffIndex;

} GfwNonSbiRegisterWriteCmd;


//! GFW ciphering types
typedef enum  GfwLoopBackType
{
  GFW_LOOPBACK_STOP = 0x0,
  GFW_LOOPBACK_A = 0x1,
  GFW_LOOPBACK_B = 0x2,
  GFW_LOOPBACK_C = 0x3
} GfwLoopBackType;


//! loopback command struct
typedef struct
{
  GfwMdspCmdType        cmd;             //!< command code
  GfwLoopBackType       type;            //!< loopback type
} GfwLoopBackCmd;

//!
typedef struct
{
  GfwMdspCmdType  cmd;            //!< command code
  uint16    reserved;
  uint8     amrCm[4];
} GfwDlCodecSetConfigureCmd;

typedef enum GfwConfigureIRCfgType
{
  MDSP_CFG_IR_WINDOW_CLEAR = 0,
  MDSP_CFG_IR_WINDOW_SET = 1,
  MDSP_CFG_IR_RELEASE = 2,
} GfwConfigureIRCfgType;

//! Configure IR window
typedef struct
{
  GfwMdspCmdType cmd;
  GfwConfigureIRCfgType  config;  //!< Set active window or clear BSN range
  uint16 startBsn;
  uint16 endBbsn;
} GfwConfigureIrWindowCmd;


//! Flush ESAIC IIR filter command
typedef struct
{
   GfwMdspCmdType cmd;
   union
   {
      uint16 dummy;
      struct
      {
         uint16 flushEsacch      : 1; // 1 for inter-cell HO success, 0 for inter-cell HO failed
         uint16 restoreErsacch   : 1; // 0 for inter-cell HO success, 1 for inter-cell HO failed
         uint16 unused           : 14;
      };
   };

} GfwFlushEsaicIIRFilter;


typedef struct
{
  uint32 gfwDebug;
  uint32 gfwDiag1;
  uint32 gfwDiag2;
  uint32 gfwDiag3;
  uint32 gfwDiag4;
  uint16 vamosSupport;
  uint16 cmCleanupAlpha;
  uint16 x2gRxfeConfigAlpha;
  uint16 x2gStartAlpha;
  boolean useIdleModeAlpha;
}GfwRxSharedVariableType;

typedef enum GfwMultislotClass
{
    GFW_MS_CLASS_12 = 0,
    GFW_MS_CLASS_33
} GfwMultislotClass;

typedef struct
{
  uint8 cOverIthresh;
  uint16 cOverISigma;
  boolean aeqEnableSchFlag;
  uint8 cipherMode;
  uint32 cipherKey[4];
  uint8  mediaAccMode;
  boolean edgeModeFlag;
  boolean doIRFlag;
  uint16 tfiForIR;
  uint16 sbdTmsi[2];
  uint16 sbdPtmsi[2];
  uint16 sbdImsi[5];
  GfwMultislotClass msc;

} GfwEdgeSharedVariableType;

typedef struct
{
  GfwRxSharedVariableType     rx;
  GfwEdgeSharedVariableType   edge;
}GfwSharedVariableType;


//! generic command struct
typedef struct
{
  GfwMdspCmdType         cmd;             //!< command code
  boolean rxSharedVarUpdated;
  boolean edgeSharedVarUpdated;
  uint32 PAD;

} GfwGenericCmd;


//!
typedef uint64 GfwTxCalDmaData64BitType;

//!
typedef struct
{
  GfwMdspCmdType            cmd;         //!< command code
  uint8                     buffIndex;
  uint16                    numOfSweeps; //!< This will tell how many valid grfcTxCalBuffers in the above array
  uint32                    startOffset;
  uint32                    stepDuration;
  GfwTxCalDmaData64BitType  *calDmaData;
  uint16                    dmaLength;
  boolean                   modulationType;
  boolean                   firstTxCalCmd;
  boolean                   lastTxCalCmd;
  int16                     modulatorStartTimeOffsetAdj;
  uint16                    calFrameNumber;
  uint8                     chainIndex;
} GfwTxCalCmd;


//! Structure to receive power measure metrics from mDSP in CM mode
typedef  struct
{
  uint16        length;
  uint16        seqNum;
  uint16        PAD; // for consistency with GfwPwrMsrResultBuff
  boolean       readyMask[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];
  uint32        rssi[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];
  boolean       scheduleTimeError;
  uint32        reason[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];    //!< bit 15= True for successful schedule
                                                                  //!<       = FALSE otherwise,
                                                                  //!< failure code in bits 0-7
  uint16        jdetReading[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];
}GfwX2GPwrMsrResultBuff;

//! X2G Power Measure command substruct
typedef struct
{
  uint16  offset;             /* Offset: SOB in QS         */
  uint32  rfBufIndex;
  boolean logIq;
  uint32  cxmPriority;
  uint32  cxmDesenseId;
  uint16  arfcn;
  boolean doJdetReading;
  boolean debugCrash;  
  uint32  rfSeqNr;  
} GfwX2GPwrMeasStruct;

//! X2G Power Measure command struct
typedef struct
{
  GfwMdspCmdType               cmdType;               /* Command Type              */
  uint16                       nBursts;               /* Number of measurements    */
  uint8                        metricsBufferIndex;       /* Power measure metrics buffer index*/
  GfwX2GPwrMeasStruct          ctrl[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];   /* Structure for the bursts  */
  uint8                        iqBufferIndex;                    // iq sample buffer index, 0 or 1
  boolean                      skipLastBurst; 
}GfwX2GPwrMsrCmd;

//! @brief GERAN FW Tune-away supports using CM design
//! //W2G CxM tune-away
typedef enum GfwX2GTuneAwayType
{
   NO_TUNE_AWAY,
   QTA_CM_CxM, 
   G2W_TA,
   T2G_CM_CxM,
   FULL_QBTA_CM_CxM  
} GfwTuneAwayType;

//! Compressed mode preload command data structures
typedef struct
{
  GfwMdspCmdType  cmd;                   /* command code   */
  uint16          preLoadTimeoffset;      /* time offset of preload CCS (in QS) */
  uint16          timeOffset;           /* time offset of start of Gap (in QS) */
  uint8           rfBufPreLoadIndex;      /* RF script index for IRAT CCS Preload */
  uint32          rxlmBufIndex;          /*index for rxlm*/
  boolean         allowExpiredOffset;
}GfwX2GPreLoadCmd;


//! Compressed mode command data structures
typedef struct
{
  GfwMdspCmdType      cmd;                   /* command code   */
  uint16              time_offset;           /* time offset of start of samples (in QS) */
  uint16              rfTune2g;              /* Specify whether to tune to G */
  uint32              rxlmBufIndex;          /*index for rxlm*/
  uint8               deviceID; 
  int                 rfBufStartIndex;       /* start index in the IRAT scripts array */
  int                 numRfEvents;           /* number of events in the array above */
//W2G CxM tune-away
  GfwTuneAwayType     isTuneAway;            /* X2G tune-away type */
  uint32              actPriority;           /* CxM priority for any activity */
  uint32              noActPriority;         /* CxM priority for no activity */
  int8                channel_id0;
}GfwX2GStartupCmd;

//!
typedef struct
{
  GfwMdspCmdType  cmd;                   /* command code   */
  uint16          time_offset;           /* time offset of start of samples (in QS) */
  uint16          rfTuneback;            /* Specify whether to do the tuneback to G*/
  uint32          rxlmBufIndex;          /*index for rxlm*/
  int             rfBufStartIndex;       /* start index in the IRAT scripts array */
  int             numRfEvents;           /* number of events in the array above */
}GfwX2GCleanupCmd;

//! GERAN - LTE reselection
typedef struct
{
  GfwMdspCmdType  cmd;                   /* command code   */
  uint16          time_offset;           /* time offset of start of samples (in QS) */
  uint16          rfTune2x;              /* Specify whether to tune to X */
  uint32          rxlmBufIndex;          /*index for rxlm*/
  int             rfBufStartIndex;       /* start index in the IRAT scripts array */
  int             numRfEvents;           /* number of events in the array above */
}GfwG2XStartupCmd;

//!
typedef struct
{
  GfwMdspCmdType  cmd;                   /* command code   */
  uint16          time_offset;           /* time offset of start of samples (in QS) */
  uint16          rfTuneback;            /* Specify whether to do the tuneback to X*/
  uint32          rxlmBufIndex;          /*index for rxlm*/
  int             rfBufStartIndex;       /* start index in the IRAT scripts array */
  int             numRfEvents;           /* number of events in the array above */
}GfwG2XCleanupCmd;


//! GFW ciphering types
typedef enum GfwAppModeType
{
  GFW_APP_MODE_ACTIVE,
  GFW_APP_MODE_MEASUREMENT,
  GFW_APP_MODE_IDLE,
  GFW_APP_MODE_MEASUREMENT_ACTIVE,
  GFW_APP_MODE_ACTIVE_GAP
} GfwAppModeType;


/*! @brief Asynchronous enter mode Command/Response messages

*/
typedef enum GfwEnterModeType
{
  GFW_ENTER_MODE_PRX = 0,
  GFW_ENTER_MODE_DRX,
  GFW_ENTER_MODE_COMBINED
} GfwEnterModeType;


//!Device mode
typedef enum GfwDeviceMode
{
  GFW_DEVICE_MODE_SS,
  GFW_DEVICE_MODE_DSDS,
  GFW_DEVICE_MODE_DSDA,
  GFW_DEVICE_MODE_TSTS
} GfwDeviceModeType;


//!
typedef struct
{
  GfwMdspCmdType  cmd;                   /* command code   */
  GfwAppModeType  mode;                  /* ACTIVE/MEASUREMENT */
  uint32  rxlmBufIndex;                  /*index for rxlm*/
  GfwDeviceModeType devMode;             /*Device Mode SS, DSDS and DSDA*/
  uint8 channel_id0;					 /* CxM channel	Id*/
 }GfwSetAppModeCmd;

typedef struct
{
  GfwMdspCmdType  cmd;                   /* command code   */
  uint32  txlmBufIndex;                  /*index for txlm*/
  uint8	  channel_id0;					 /* CxM channel	Id*/			
}GfwConfigureTxCmd;

typedef struct
{
  GfwMdspCmdType cmd;
  int32 f_hz_q6;
  uint32 inv_f_dl_lo;
} GfwAfcUpdateCmd;

typedef struct
{
  GfwMdspCmdType cmd;
  GfwAppModeType mode;   
  GfwEnterModeType enterMode;                /* ACTIVE/MEASUREMENT */
  uint32 rxlmBufIndex;  //
  uint32 rxlmDrxBufIndex;
  uint8  deviceID;  
} GfwEnterModeCmd;

typedef struct
{
  GfwMdspCmdType cmd;
  
} GfwExitModeCmd;

typedef GfwMdspCmdType GfwSetTxBandCmd;

#define MAX_IP2CAL_MEAS_STEPS 20
typedef struct
{
  uint32 rxlmBufIndex;                   
  uint32 txlmBufIndex;                  
  uint32 numIp2CalMeas;                              
  uint32 ip2CalStepSizeQS;
} GfwIp2CalInfoType;

typedef struct
{
  GfwMdspCmdType cmd;
  uint16 timeOffsetQs;           // Offset to start of IP2 cal burst in QS 
  GfwIp2CalInfoType ip2CalInfo;  // IP2 cal measurement info
} GfwIp2CalCmd;

//! Command Buffer Type Union
typedef union
{
  GfwMdspCmdType          cmdId;
  GfwStartAcqCmdBufStruct startAcqCmd;
  GfwStopAcqCmdBufStruct  stopAcqCmd;
  GfwSchCmdBufStruct      schCmd;
  GfwGsmSyncRxCmd         rxCmd;
  GfwGsmTxCmd             gsmTxCmd;
  GfwGsmPwrMsrCmd         pwrMsrCmd;
  GfwStartTestTxCmd       startTestTxCmd;
  GfwStopTestTxCmd        stopTestTxCmd;
  GfwCipherControlCmd     cipherControlCmd;
  GfwNonSbiRegisterWriteCmd nonSbiRegisterWriteCmd;
  GfwLoopBackCmd          loopBackCmd;
  GfwDlCodecSetConfigureCmd amrCodecCmd;
  GfwConfigureIrWindowCmd configureIrWindowCmd;
  GfwFlushEsaicIIRFilter  flushEsaicIIRFilter;
  GfwGenericCmd           genericCmd;
  GfwX2GPwrMsrCmd         x2gPwrMsrCmd;
  GfwX2GStartupCmd        x2gStartupCmd;
  GfwX2GCleanupCmd        x2gCleanupCmd;
  GfwG2XStartupCmd        g2xStartCmd;
  GfwG2XCleanupCmd        g2xCleanupCmd;
  GfwTxCalCmd             txCalCmd;
  GfwConfigureTxCmd       configureTxCmd;
  GfwAfcUpdateCmd         afcUpdateCmd;
  GfwSetAppModeCmd        setAppMode;
  GfwEnterModeCmd         enterModeCmd;
  GfwSetTxBandCmd         setTxBandCmd;
  GfwGsmSyncRxCmd         asyncRxCmd;
  GfwX2GPreLoadCmd        x2gPreLoadCmd;
  GfwIp2CalCmd            ip2CalCmd;
} GfwCmdBufType;

//!
typedef struct
{
  uint16        semaphoreAndNumCommands;
  uint16        cmdSeqNum;
  GfwCmdBufType cmd;
} GfwHostCmdBufType;


/*! FCCH result structure
    This structure contains the FCCH detection results for one valid tone,
    including tone position, frequency and SNR estimates and value applied
    for AFC.
*/

typedef struct
{
  uint16 toneCount;          //!< Number of tone
  int32  toneStartPosition;  //!< Number of GSM at tone position
  int32  coarseFreqEstimate; //!< Result of coarse freq. estimation
  int32  fineFreqEstimate;   //!< Result of fine freq. estimation
  int32  afcFreqEst;         //!< Freq. correcton value applied to rotator or PDM
  uint16 snrEst;             //!< Result of SNR estimation
  boolean scheduleTimeError; //!< If set to one, this means there was an error meeting the time deadlline to schedule this burst.
  uint32  rssi;              //!< RSSI of the first ACQ block captured
  uint16  seqNum;
  uint16  jdetReading;  
} GfwFcchResultStruct;


/*! FCCH result buffer
    This buffer contains NUM_FCCH_RESULTS instances of the fcchResultStruct, read and write
    pointers to the structure containing the acutal tone results, a read counter and a write
    counter which are used for error checking.

*/

typedef struct
{
  GfwFcchResultStruct *fcchResultWrPtr;             //!< Write pointer used by mdsp
  uint16 mdspFcchResultWrCtr;                    //!< Number of FCCH results written by mdsp
  uint16 mdspFcchResultRdCtr;                    //!< Mumber of FCCH results read by host
  GfwFcchResultStruct fcchResults[MAX_FCCH_RES]; //!< Array containing tone data
} GfwFcchResultBuff;



/*! SCH result structure
    This structure contains the SCH results for one decoded SCH burst, used in
    combined acquisition.
*/

typedef struct
{
  uint16 schToneCount;       //!< Number of tone that triggered SCH decode
  uint16 schCrc;             //!< SCH CRC results
  uint16 schRxQual;          //!< SCH RX Qual measurement
  uint16 schBfi;
  uint16 schPacketLen;       //!< Number of elements in this buffer
  uint32 schData;            //!< Decoded SCH payload
  uint32 schStartPosition;   //!< Starting point of SCH bursts
  uint16 schCorrelationPeak; //!< Result of SNR estimation
  uint16 coarse_freq;        //!< Coarse frequency estimate
  uint16 schSnr;             //!< SCH SNR
  boolean scheduleTimeError; //!< If set to one, this means there was an error meeting the time deadlline to schedule this burst.
  uint16 schCombSNR;         //!< Result of SNR estimation for SCH combined acq.  
  uint16 jdetReading;
} GfwSchResultStruct;


/*! SCH result buffer
    This buffer contains NUM_SCH_RESULTS instances of the schResultStruct, read and write
    pointers to the structure containing the acutal tone results, a read counter and a write
    counter which are used for error checking.

*/

typedef struct
{
  GfwSchResultStruct *schResultWrPtr;           //!< Write pointer used by mdsp
  uint16 mdspSchResultWrCtr;                 //!< Number of FCCH results written by mdsp
  uint16 mdspSchResultRdCtr;                 //!< Mumber of FCCH results read by host
  GfwSchResultStruct GfwSchResults[MAX_SCH_RES];   //!< Array containing tone data
} GfwSchResultBuff;



/*! HostCmdBuffer
    Definition of structure for hostCommandBuffer
*/
typedef uint8 GfwHostCmdBuff[1024] ALIGN(32);


//---------------------------------------------------------------------------
//
//!           Data structures and definements for sleep
//
//  -------------------------------------------------------------------------

typedef enum GfwSleepCmdType
{
  GFW_SLEEP,
  GFW_WAKE,
  GFW_SLEEP_QUERY
} GfwSleepCmdType;

typedef enum GfwAsyncCmd
{
  GFW_NO_CMD,
  GFW_ASYNC_CMD
} GfwAsyncCmd;

//! an item in the sleep state struct is active/not sleeping
#define GFW_SLEEP_ITEM_ACTIVE 1
//! an item in the sleep state struct is inactive/sleeping
#define GFW_SLEEP_ITEM_INACTIVE 0

//!
typedef union
{
  struct
  {
    uint32 gfw                :1;   //!< this bit indicates whether GFW is sleeping.
                                    //!< if it is active, read the following bits to work out
                                    //!< what item was stopping it from sleeping.
    uint32                    :4;
    uint32 ccsPending         :1;
    uint32 asyncCmdPending    :1;
    uint32 genericTaskPending :1;
    uint32 rxRfTriggersPending:1;
    uint32 txRfTriggersPending:1;
    uint32 fwProcActive       :1;
    uint32 acqActive          :1;
    uint32 iratActive         :1;
    uint32 rxfeActive         :1;
    uint32 gstmrIsrActive     :1;
    uint32 pwrScanPending     :1;
  };
  uint16 gfwSleepState;
} GfwSleepStateStruct;

//!
typedef union
{
  struct
  {
    uint16 crashOnError       : 1;  // crash if CCS scripts status is error
    uint16 reserved           :15;
  };
  uint16 gfwAllActions;
} GfwActionOnCcsStatusStruct;

typedef enum GfwDspHaltError
{
  GFW_ERROR_RXFE_STUCK = (1<<0),
  GFW_ERROR_UNSPECIFIED =(1<<1)
} GfwDspHaltError;

//!
typedef struct
{
  uint16   gfwFTickCounter;
  int16   gfwReadyForCmds;
  GfwVersionType gfwVersion;
  GfwSleepStateStruct gfwSleepState ALIGN(CACHE_LINE_SZ);
  boolean nullPagePatternInUse;
  GfwDspHaltError  dspHalt;
  uint16 irVariableWindow;
}GfwReadVariablesStruct;
typedef struct 
{ 
  uint8 dtxEsaicMetric;        //!< Normalized ESAIC metric in Q8
  boolean bDtxCxmReg;          //!< CXM registration indication for DTX detection
  boolean bAmrPdCxmReg;        //!< CXM registration indication for AMR partial decode
  boolean bDtxRxCancel;        //!< RX burst cancellation indication for DTX detection
  boolean bAmrPdRxCancel;      //!< RX burst cancellation indication for AMR partial decode
  boolean bAmrPdInd;           //!< ??
  boolean bAmrPdDesense;       //!< Desense indication for AMR partial decode
  boolean bDtxIndDec;          //!< Indication whether DTX was detected for this block, valid at decode tick
  boolean bAmrPdIndDec;        //!< Indication whether AMR partial decode used for this block, valid at decode tick
  boolean bFwToL1DisableAmrPd; //!< Indication to disable AMR partial decode
} GfwDtxAmrPdMetrics;
typedef enum GfwNullPageType
{
  NULL_PAGE_AGILENT = 0,
  NULL_PAGE_ANRITSU,
  NULL_PAGE_INVALID
} GfwNullPageType;

typedef struct
{
  GfwFcchResultBuff           gfwFcchResultBuffer;
  GfwSchResultBuff            gfwSchResultBuffer;
  GfwDecodedDataStruct        gfwSyncRxDataBuffer[MDSP_DTM_NUM_RX_DATA_BUFFERS + TRIPPLEBUF];
  //GfwDecodedDataStruct        gfwAsyncRxDataBuffers[TRIPPLEBUF];  --combined with sync buffer, use last 3 entries.
  GfwMetricsStruct            gfwMetricsBuffers[TRIPPLEBUF + TRIPPLEBUF];
  //GfwMetricsStruct            gfwAsyncMetricBuffers[TRIPPLEBUF]; --combined with sync buffer, use last 3 entries.
  GfwPwrMsrResultBuff         gfwPwrMsrResultBuffers[TRIPPLEBUF];
  GfwX2GPwrMsrResultBuff      gfwX2GPwrMsrResultBuffers[GFW_X2G_PWR_MON_BUFFERING];
  GfwCommonBurstMetricsStruct gfwSchRxBurstMetricsBuffer[TRIPPLEBUF];
  GfwSchDecodeDataStruct      gfwSchDecodedDataBuffer[TRIPPLEBUF];
  GfwSchedulerResultStruct    gfwSchedulerResultBuffers[TRIPPLEBUF];
  GfwSchedulerTxResultStruct  gfwSchedulerTxResultBuffers[TRIPPLEBUF];
  GfwUsfBufStruct             gfwUsfResultBuffer;
  GfwNullPageType             sbdNullPageType;
  GfwCxmLogPacketBuff   	  gfwCxmLogBuffer[TRIPPLEBUF];
  GfwDtxAmrPdMetrics          gfwDtxAmrPdLogBuffer[TRIPPLEBUF];
} GfwHostMdspResultBuffs;

/*---------------------------------------------------------------------------

                Host Interface Structure for GSM operation

  -------------------------------------------------------------------------*/
typedef struct
{
  GfwHostCmdBuff              gfwHostCmdBuffer[PINGPONG];

  GfwHostMdspResultBuffs      gfwResultBuffers;

  GfwDtmTxStruct              gfwDtmTxBuffers[MDSP_DTM_NUM_TX_DATA_BUFFERS];

  GfwRfEventTable             rfEventTable;

  GfwGrfcBuffer               gfwNonSbiWriteBuffers[PINGPONG]; //was gfwGrfcReqBuffers[2];

  // read structure for MDSP DRIVERS. This structure is updated by FW and MDSP Drivers will read this structure
  GfwReadVariablesStruct      gfwReadVariablesStruct;

  GfwAsyncCmd                 gfwAsyncCmd ALIGN(CACHE_LINE_SZ);
  GfwTxCalDmaData64BitType    gfwDmaData[MAX_TX_CAL_CMD_DMA_DATA_LENGTH];

  GfwIqSamplesBuffer          gfwIqBuffer[PINGPONG];

  GfwActionOnCcsStatusStruct  gfwActionOnCcsStatus;
  boolean                     gfwHaltFirmware ALIGN(CACHE_LINE_SZ);
  uint8                       dummy[CACHE_LINE_SZ] ALIGN(CACHE_LINE_SZ);  //here to insure gfwHaltFirmware is one complete line
  //as it is cache invalidated in FW, and we do whole lines at a time.

  GfwSharedVariableType       gfwSharedVariables;
  char                        gfwInterfaceVersion[sizeof(GFW_INTERFACE_VERSION)] ALIGN(CACHE_LINE_SZ); //used to check that FW/SW interface is build with compatible versions


} GfwHostMdspInterfaceSegment;

#endif // GFW_SW_INTF_H
