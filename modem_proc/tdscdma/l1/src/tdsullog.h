#ifndef TDSULLOG_H
#define TDSULLOG_H

/*===========================================================================
                     T D S U L L O G . H

DESCRIPTION
  This header file describes the Layer 1 UpLink log packet definitions.

Copyright (c) 2001-2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsullog.h#1 $ 
$DateTime: 2016/12/13 07:58:19 $ 
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
05/26/11    sk     Initial cut for TD-SCDMA (R4) L1 uplink 
05/26/11    hzhou  Remove all old log packet definition for UMTS
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "log.h"
#include "log_codes.h"
#include "log_codes_tdscdma.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/*! \brief Uplink - Max number of TransportFormat */
#define TDSULLOG_MAX_TF                    32
/*! \brief Uplink - Max number of Transport Channel */
#define TDSULLOG_MAX_TRCH                  8
/*! \brief Uplink - Max number of CCTrCH*/
#define TDSULLOG_MAX_CCTRCH                1
/*! \brief Uplink - Max number of Transport Format Combination */
#define TDSULLOG_MAX_TFC                   64
/*! \brief Uplink - Max number of Prach Channel code */
#define TDSULLOG_MAX_NUM_PRACH_CHAN_CODE   4
/*! \brief Uplink - Max number of Prach and Fpach pairs */
#define TDSULLOG_MAX_PRACH_FRACH_PAIRS     8
/*! \brief Uplink - Max number of ASC */
#define TDSULLOG_MAX_NUM_ASC               8
/*! \brief Uplink - Max number of DPCH Channel Code */
#define TDSULLOG_MAX_NUM_DPCH_CHAN_CODE    2
/*! \brief Uplink - Max number of DPCH time slots*/
#define TDSULLOG_MAX_DPCH_TS               5
/*! \brief Uplink - Max number of DPCH physical channels: 2 chan codes * 5 timeslots */
#define TDSULLOG_MAX_DPCH_PHY_CHAN         10

/* LOG_TDSCDMA_BASE_C has been defined in log_codes.h */

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_UL_DCH_CFG_LOG_PACKET
                 (LOG CODE: 0xD020)
                     
This log packet will contain the TD-SCDMA UL DCH configuration information. 
 
This log record is generated whenever each channel is setup or reconfigured.

--------------------------------------------------------------------------*/

/*! \brief TDSCDMA L1 UpLink DCH cfg 
 * 64 bits total
 */
typedef struct
{
  /*! \brief Transport channel ID, Integer number 0 to 31 */
  uint32  trchId:5;            
  /*! \brief TTI format :
   *  0 - 5 (for TDS RACH)
   *  1 - 10 
   *  2 - 20
   *  3 - 40
   *  4 - 80 
   */          
  uint32  tti:3;              
  /*! \brief Number of CRC bits:
   *  0 - 0 
   *  1 - 8 
   *  2 - 12
   *  3 - 16
   *  4 - 24
   */
  uint32  crcLen:3;  
  /*! \brief Coding rate and type:
   *  0 - 1/2 and convolutional 
   *  1 - 1/3 and convolutional
   *  2 - 1/3 and turbo
   *  3 - No coding 
   */         
  uint32  codeRate:2;  
  /*! \brief Rate-matching attribute of the channel; number from 1 to 256 */
  uint32  rmAttr:9;
  /*! \brief Number of transport formats per channel; number from 1 to 32  */           
  uint32  numTf:6;           
  /*! \brief Reserved */
  uint32  :4;
// 32 bits
       
  /*! \brief TF info base index in the TF info table */
  uint32  tfInfoBase:6;      
  /*! \brief Reserved */
  uint32  :26;
// 32 bits
} tdsullog_dch_cfg_type;

/*! \brief TDSCDMA L1 UpLink Transport Format info 
 * 32 bits total
 */
typedef struct
{
  /*! \brief Transport block size in bits, max 8960  */
  uint32  tbSize:16; 
  /*! \brief Number of transport blocks, max 32   */        
  uint32  numTb:6;           
  /*! \brief Reserved */
  uint32  :10;
// 32 bits
} tdsullog_tf_Info_type;


/*! \brief        LOG PACKET: TDSCDMA_L1_UL_DCH_CFG_LOG_PACKET
                 (LOG CODE: 0xD020)
 
This log packet is for internal use only! 
 
This log packet will contain the TD-SCDMA UL DCH configuration information. 
This log record is generated whenever each channel is setup or reconfigured. 

MIN Size: 4 bytes, MAX Size: 196 bytes
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;         
  /*! \brief Corresponding CCTRCH ID number */
  uint32  cctrchId:4;   
  /*! \brief Number of current transport channels, max number is 8 */     
  uint32  numTrch:4;
  /*! \brief number of Transport Format Info table, max number is 32 */
  uint32  numTf:8;           
  /*! \brief Reserved */
  uint32  :8;
// 32 bits
    
  /*! \brief Transport Format Info table, actual number determined by numTf */
  tdsullog_tf_Info_type ulTfInfoTbl [TDSULLOG_MAX_TF];
  // 32*32 = 1024bits = 128 bytes

  /*! \brief DCH config type, actual number determined by numTrch */
  tdsullog_dch_cfg_type ulTchCfg [TDSULLOG_MAX_TRCH];
  //64 * 8 = 512 bits = 64 bytes

} tdsullog_dch_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_DCH_CFG_PKT_LEN(num_tf, num_trch)  ( \
    FPOS(tdsullog_dch_cfg_pkt_type, ulTfInfoTbl)+  \
    ((num_tf) * sizeof(tdsullog_tf_Info_type))+  \
    ((num_trch) * sizeof(tdsullog_dch_cfg_type)))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_UL_DCH_GAIN_FACTOR_CFG_LOG_PACKET
                 (LOG CODE: 0xD021)
                     
This log packet will contain the TD-SCDMA UL DCH Gain Factor configuration 
information. 
 
This log record is generated whenever each channel is setup or reconfigured.

--------------------------------------------------------------------------*/

/*! \brief TDSCDMA L1 Uplink Gain Factor info for each TFC 
 * 32 bits total
 */
typedef struct
{
  /*! \brief TFC ID */
  uint32  tfci:7; 
  /*! \brief Gain Factor calculation method : 
   *  0 - computed 
   *  1 - signaled
   *  2 - None
   */           
  uint32  gfAlgo:2;          
  /*! \brief Gain factor beta_D.
   *  Bit 15 - 6: integer part
   *  Bit 5- 0: fraction part
   */
  uint32  betaD:16;       
  /*! \brief Referenced TFCI  for computed BetaD. Mandatory for computed, optional for signalled.  */    
  uint32  refTfci:7;         
// 32 bits

} tdsullog_tfc_gf_cfg_type;


/*! \brief        LOG PACKET: TDSCDMA_L1_UL_DCH_GAIN_FACTOR_CFG_LOG_PACKET
                 (LOG CODE: 0xD021)
 
  This log packet is for internal use only! 

  This log packet will contain the TD-SCDMA UL DCH Gain Factor configuration 
  information. 
  This log record is generated whenever each channel is setup or reconfigured.
 
  MIN Size: 4 bytes, MAX Size: 260 bytes
  32 + 2048 = 2080 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;         
  /*! \brief Reserved */
  uint32  :12;
  /*! \brief Corresponding CCTRCH ID number */
  uint32  cctrchID:4;   
  /*! \brief Actual Number of transport format combination; number from 1 to 64; */     
  uint32  numTfc:8;          
//32 bits

  /*! \brief TFC table, actual number determined numTfc */
  tdsullog_tfc_gf_cfg_type ulTfcGfCfg [TDSULLOG_MAX_TFC];
  //32 * 64 = 2048 bits = 256 bytes in worst case

} tdsullog_dch_gain_factor_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_DCH_GAIN_FACTOR_CFG_PKT_LEN(num_tfc)  ( \
    FPOS(tdsullog_dch_gain_factor_cfg_pkt_type, ulTfcGfCfg)+  \
    ( num_tfc * sizeof(tdsullog_tfc_gf_cfg_type)))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_UL_TFC_CFG_LOG_PACKET
                 (LOG CODE: 0xD022)
                     
This log packet will contain the TD-SCDMA UL DCH transport format combinations. 
 
This record is generated upon new channel set up.
 
NOTE	The TF_IDX is an 8-element array inside the TFC information array. 
      Unused portions of TF_IDX are always set to zero.

Together with L1_UL_DCH_CFG_LOG, this packet is used to determine the 
current received channel rate. The field TFI_IDX in this packet points 
to a current (specific) TFC for each transport channel. That TFC can then 
be looked up in with L1_UL_TRCH_CFG_LOG to find out the predefined rate.

--------------------------------------------------------------------------*/

/*! \brief TDSCDMA L1 Uplink TFC cfg 
 * 32 + 64 bits total = 96 bytes
 */
typedef struct
{
  /*! \brief This TFC valid or not : For each TFC
   *  0 - Not valid
   *  1 - Valid
   */
  uint8  tfcValid;                       
  /*! \brief Reserved */
  uint8  reserved[3];
// 32 bits

  /*! \brief TF index all the TrCh in this TFC : 
   *  1st byte - transport format index for the 1st transport channel
   *  2nd byte - transport format index for the 2nd transport channel
   *     *  8th byte - transport format index for the 8th transport channel
   *  Note. A transport format index: 0 to 31
   */
  uint8  tfIdx [TDSULLOG_MAX_TRCH];      
// 64 bits    

} tdsullog_tfc_cfg_type;

/*! \brief        LOG PACKET: TDSCDMA_L1_UL_TFC_CFG_LOG_PACKET
                 (LOG CODE: 0xD022)
 
This log packet is for internal use only! 
 
This log packet will contain the TD-SCDMA UL DCH transport format combinations. 
This record is generated upon new channel set up.
 
NOTE	The TF_IDX is an 8-element array inside the TFC information array. 
      Unused portions of TF_IDX are always set to zero.

Together with L1_UL_DCH_CFG_LOG, this packet is used to determine the 
current received channel rate. The field TFI_IDX in this packet points 
to a current (specific) TFC for each transport channel. That TFC can then 
be looked up in with L1_UL_TRCH_CFG_LOG to find out the predefined rate.
 
MIN Size: 4 bytes, MAX Size: 772 bytes
32 + (96 * 64) = 6176 bits 

*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;         
  /*! \brief Reserved */
  uint32  :12;
  /*! \brief Corresponding CCTRCH ID number */
  uint32  cctrchID:4;   
  /*! \brief Actual Number of TFC  in TFC information array; integer 1 to 64 */     
  uint32  numTfc:8;          
//32 bits

  /*! \brief TDSCDMA L1 Uplink TFC cfg, actual number determined by numTfc */
  tdsullog_tfc_cfg_type ulTfcCfg [TDSULLOG_MAX_TFC];
  // 96 * 64 = 6144 bits = 768 bytes in worst case

} tdsullog_tfc_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_TFC_CFG_PKT_LEN(num_tfc)  ( \
    FPOS(tdsullog_tfc_cfg_pkt_type, ulTfcCfg)+  \
    ( num_tfc * sizeof(tdsullog_tfc_cfg_type)))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_UL_DPCH_CFG_LOG_PACKET
                 (LOG CODE: 0xD023)
 
This log packet will contain the TD-SCDMA UL dedicated physical channels 
configuration information received over dedicated signaling. 
 
This log record is generated whenever a new dedicated physical channel is 
setup or reconfigured.
 
--------------------------------------------------------------------------*/

/*! \brief TDSCDMA L1 Uplink DPCH TimeSlot info  
 * 64 bits total
 */
typedef struct
{
  /*! \brief Timeslot number. Valid value: 1,2,3,4,5 */
  uint32  tsNum:3; 
  /*! \brief TFCI existence indication : 
   *  1 - TFCI existed
   *  0 - TFCI not existed
   */               
  uint32  tfciExistence:1;        
  /*! \brief midamble alloc mode :
   *  0 -  Default
   *  1 - common
   *  2 - UE specific
   */       
  uint32  midambleAllocMode:2;   
  /*! \brief Midable configuration (K): Integer (2, 4, 6, 8, 10, 12, 14, 16)*/ 
  uint32  midambleCfg:5;    
  /*! \brief Midable shift: Integer (0..15) */      
  uint32  midambleShift:4; 
  /*! \brief Modulation type : 
   *  0 - QPSK
   *  1 - 8 PSK (not supported)
   */
  uint32  modulationtype:1; 
  /*! \brief Amount of SS and TPC bits send in this timeslot. 
   *  0 - 0 SS symbol
   *  1 - 1 SS symbol
   *  2 - 16/SF SS symbols
   */      
  uint32  ssTpcSymbols:2;       
  /*! \brief The number of additional codes in this timeslot that carry TPC and SS symbols. Valid value: 0, 1. */  
  uint32  addiTpcSsSymbols:4;   
  /*! \brief The number of code channels. Valid value: 1, 2  */  
  uint32  numCodeCh:2;            
  /*! \brief Reserved */
  uint32  :8; 
// 32 bits

  /*! \brief Channelisation code */
  uint8  chanCode [TDSULLOG_MAX_NUM_DPCH_CHAN_CODE];
  /*! \brief Spreading factor. Valid value: 1,2,4,8,16. */
  uint8  sf [TDSULLOG_MAX_NUM_DPCH_CHAN_CODE];
// 32 bits

} tdsullog_dpch_ts_info_type;

/*! \brief        LOG PACKET: TDSCDMA_L1_UL_DPCH_CFG_LOG_PACKET
                 (LOG CODE: 0xD023)
 
This log packet is for internal use only! 
 
This log packet will contain the TD-SCDMA UL dedicated physical channels 
configuration information received over dedicated signaling. 
 
This log record is generated whenever a new dedicated physical channel is 
setup or reconfigured.
 
MIN Size: 28 bytes, MAX Size: 136 bytes
224 + 320 = 544 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8; 
  /*! \brief Corresponding CCTRCH ID number */        
  uint32  cctrchID:4;              
  /*! \brief UTRA Absolute Radio Frequency Number */
  uint32  uarfcn:16;               
  /*! \brief TPC step size. Valid value: 1,2,3.  */        
  uint32  tpcStepSize:2;    
  /*! \brief UL Timing advance control enable flg. 
   *  1-UL timing advance control enabled;
   *  0 - not enabled 
   */
  uint32  timingAdvCtrlEnabled:1;  
  /*! \brief Second interleaving mode.
   *  0 - Frame based interleaving
   *  1 - Slot based interleaving
   */             
  uint32  secondIntlvMode:1;  
// 32 bits

  /*! \brief Cell Parameter ID, Integer(0..127) */
  uint8   cpi;                   
  /*! \brief Maximum allowed transmit power in dBm . Enum: -50 to 33 */
  int8    maxTxPwr;   
  /*! \brief UL target SIR in dBm. Enum: -12058 */           
  int8    prxDpchDes;    
  /*! \brief PCCPCH tx power in dBm. Valid value: 0, 6-43 */       
  uint8   pccpchTxPwr;           
// 32 bits

  /*! \brief Uplink synchronisation step size. Valid value: 1..8.*/
  uint32  syncStepSize:4;          
  /*! \brief Uplink synchronisation frequency.  Valid value: 1..8.  */
  uint32  syncFreq:4;              
  /*! \brief UUL sync parameter presence indication.
   *  0 - not existed
   *  1 - existed
   */
  uint32  syncParamPresent:1;   
  /*! \brief Bitmask to indicate what UL sync codes are available. */   
  uint32  syncCodeMask:8;          
  /*! \brief UL sync code power ramp step in dB when there's no ACK. Valid value: 0,1,2,3 (dB).  */
  uint32  pwrRampStep:2;           
  /*! \brief Maximum UL sync code transmission. Valid value: 1,2,4,8 */
  uint32  maxSyncUlTx:4;           
  /*! \brief UpPCH shift. Valid value: 0..127 */
  uint32  uppchShift:7;        
  /*! \brief Reserved */
  uint32  :2;
// 32 bits

  /*! \brief Desired UpPCH received power in dBm. Enum: -120..-58 */
  int8    prxUppchDes;           

  /*! \brief FPACH timeslot number (0..6) */
  uint8   fpachTsNum;
  /*! \brief FPACH Channelisation code */
  uint8   fpachChanCode;
  /*! \brief FPACH Spreading factor. Valid value: 1,2,4,8,16. */
  uint8   fpachSf;
// 32 bits

  /*! \brief FPACH midamble alloc mode :
   *  0 -  Default
   *  1 - common
   *  2 - UE specific
   */       
  uint8   fpachMidambleAllocMode;   
  /*! \brief FPACH Midable configuration (K): Integer (2, 4, 6, 8, 10, 12, 14, 16)*/ 
  uint8   fpachMidambleCfg;    
  /*! \brief FPACH Midable shift: Integer (0..15) */      
  uint8   fpachMidambleShift; 
  /* ! \briefWAIT TIME. The number of sub-frames to wait for FPACH ACK, following
   *   the sub-frame in which the SYNC_UL is transmitted. (1..4) 
   */
  uint8   wt;
// 32 bits

  /*! \brief Activation time : The frame number in CFN at which the operation/changes 
   *  caused by the related message shall take effect. Values between 0 and 255.
   */
  uint32  actTime:8; 
  /*! \brief The number of bits for the TFCI bits code word. Valid value: 4,8,16,32 for QPSK configuration. */              
  uint32  tfciLen:8;    
  /*! \brief Puncturing limit,  40 to 100  step by 4. (0.40..1.0 by step of 0.04) * 100 */     
  uint32  punctLim:8;  

  uint32  :8;
// 32 bits

  /*! \brief TDM repetition period. Valid value:  1, 2,4,8,16,32,64. */ 
  uint32  repPrd:7;   
  /*! \brief TDM repetition length. Valid value:  1 to 63.*/             
  uint32  repLen:6;                
  /*! \brief TDM repetition Offset: SFN mod Repetition period. Valid value: 0 - Repetition period -1. */
  uint32  repOffset:6;  
  /*! \brief Dynamic SF used or not : 
   *  1 - dynamic SF used;
   *  0 - not used
   */
  uint32  dynamicSfUse:1;     
  /*! \brief Actual Number of timeslots used */     
  uint32  numTs:3;                 
  /*! \brief Reserved */
  uint32  :9;
// 32 bits

  /*! \brief Uplink DPCH TimeSlot info, actual number determined by numTs */
  tdsullog_dpch_ts_info_type ulDpchTsInfo [TDSULLOG_MAX_DPCH_TS];
  // 64 * 5 = 320 bits

} tdsullog_dpch_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_DPCH_CFG_PKT_LEN(num_dpch_ts)  ( \
    FPOS(tdsullog_dpch_cfg_pkt_type, ulDpchTsInfo)+  \
    ( num_dpch_ts * sizeof(tdsullog_dpch_ts_info_type)))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_UL_RACH_CFG_LOG_PACKET
                 (LOG CODE: 0xD024)
                     
This log packet will contain the TD-SCDMA UL RACH configuration information. 

This log record is generated whenever each channel is setup or reconfigured.

--------------------------------------------------------------------------*/

/*! \brief        LOG PACKET: TDSCDMA_L1_UL_RACH_CFG_LOG_PACKET
                 (LOG CODE: 0xD024)
 
This log packet is for internal use only! 
                     
This log packet will contain the TD-SCDMA UL RACH configuration information. 
This log record is generated whenever each channel is setup or reconfigured.
 
MIN Size: 8 bytes, MAX Size: 8 bytes
64 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;    
  /*! \brief Corresponding CCTRCH ID number */     
  uint32  cctrchId:4;    
  /*! \brief Transport channel ID, Integer number 0 to 31 */    
  uint32  trchId:5;          
  /*! \brief TTI format :
   *  0 - 5 (for TDS RACH)
   *  1 - 10 
   *  2 - 20
   *  3 - 40
   *  4 - 80 
   */          
  uint32  tti:3;              
  /*! \brief Number of CRC bits:
   *  0 - 0 
   *  1 - 8 
   *  2 - 12
   *  3 - 16
   *  4 - 24
   */
  uint32  crcLen:3;  
  /*! \brief Coding rate and type:
   *  0 - 1/2 and convolutional 
   *  1 - 1/3 and convolutional
   *  2 - 1/3 and turbo
   *  3 - No coding 
   */         
  uint32  codeRate:2;  
  /*! \brief Reserved */
  uint32  :7;
// 32 bits

  /*! \brief Rate-matching attribute of the channel; number from 1 to 256 */
  uint32  rmAttr:9; 
  /*! \brief Transport block size in bits, max 8960 */          
  uint32  tbSize:16;     
  /*! \brief Number of transport blocks, max 32 */      
  uint32  numTb:6;           
  /*! \brief Reserved */
  uint32  :1;  
  //32 bits

} tdsullog_rach_cfg_pkt_type;


/* Macro to determine the sizes of the log packet */
#define TDSULLOG_RACH_CFG_PKT_LEN  ( \
    sizeof(tdsullog_rach_cfg_pkt_type))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_UL_PRACH_FPACH_CFG_LOG_PACKET
                 (LOG CODE: 0xD025)
                     
This log packet holds the description of the PRACH and FPACH channel parameters. 
This record is generated when UE attempts a PRACH transmission.

--------------------------------------------------------------------------*/
/*! \brief TDSCDMA L1 Uplink Prach cfg info
 * 96 bits total
 */
typedef struct
{
  /*! \brief PRACH timeslot number: Integer (1,  5) */
  uint32  tsNum:3;                
  /*! \brief midamble alloc mode :
   *  0 -  Default
   *  1 - common
   *  2 - UE specific
   */       
  uint32  midambleAllocMode:2;   
  /*! \brief Midable configuration (K): Integer (2, 4, 6, 8, 10, 12, 14, 16)*/ 
  uint32  midambleCfg:5;    
  /*! \brief Midable shift: Integer (0..15) */      
  uint32  midambleShift:4;   
  /*! \brief Number of PRACH channelisation code: Integer (1,  4) */
  uint32  numChanCode:3;          
  /*! \brief Reserved */
  uint32  :15;
// 32 bits

  /*! \brief PRACH channelisation code: Integer (1,  16) */
  uint8  chanCode [TDSULLOG_MAX_NUM_PRACH_CHAN_CODE];
  /*! \brief PRACH spreading factor : Integer (4, 8, 16) */
  uint8  sf [TDSULLOG_MAX_NUM_PRACH_CHAN_CODE];
//64 bits

} tdsullog_prach_cfg_type;

/*! \brief TDSCDMA L1 Fpach cfg info 
 * 32 bits total 
 */
typedef struct
{
  /*! \brief FPACH timeslot number: Integer (0, 2,  6) */
  uint32  tsNum:3;                
  /*! \brief midamble alloc mode :
   *  0 -  Default
   *  1 - common
   *  2 - UE specific
   */       
  uint32  midambleAllocMode:2;   
  /*! \brief Midable configuration (K): Integer (2, 4, 6, 8, 10, 12, 14, 16)*/ 
  uint32  midambleCfg:5;    
  /*! \brief Midable shift: Integer (0..15) */      
  uint32  midambleShift:4;   
  /*! \brief Maximum number of subframes to wait : Integer (1,  4)*/
  uint32  wt:3; 
  /*! \brief Channel code ID: Integer (1,  16) */                  
  uint32  chanCode:5;  
  /*! \brief PRACH spreading factor: Integer (4, 8, 16) */           
  uint32  sf:5;                   
  /*! \brief Reserved */
  uint32  :5;
// 32 bits

} tdsullog_fpach_cfg_type;

/*! \brief        LOG PACKET: TDSCDMA_L1_UL_PRACH_FPACH_CFG_LOG_PACKET
                 (LOG CODE: 0xD025)
 
This log packet is for internal use only! 
                     
This log packet holds the description of the PRACH and FPACH channel parameters. 
This record is generated when UE attempts a PRACH transmission.
 
MIN Size: 8 bytes, MAX Size: 136 bytes
32 + 32 + 256 + 768 = 1088 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;       
  /*! \brief Corresponding CCTRCH ID number */  
  uint32  cctrchID:4;    
  /*! \brief UTRA Absolute Radio Frequency Number */         
  uint32  uarfcn:16;              
  /*! \brief Reserved */
  uint32  :4;
// 32 bits

  /*! \brief Cell Parameter ID */
  uint8   cpi;        
  /*! \brief Maximum allowed uplink Tx Power: Enum (-50,  33) dBm */         
  int8    maxAllowTxPwr; 
  /*! \brief Actual number of PRACH-FPACH pairs: Integer (1,  8) */      
  uint8   numPrachFpachPairs;  
  /*! \brief Reserved */
  uint8   reserved;
// 32 bits

  /*! \brief Fpach configuration, actual number determined by numPrachFpachPairs */
  tdsullog_fpach_cfg_type ulFpachCfg [TDSULLOG_MAX_PRACH_FRACH_PAIRS];
  // 32 * 8 = 256 bits = 32 bytes in worst case

  /*! \brief Prach configuration , actual number determined by numPrachFpachPairs*/
  tdsullog_prach_cfg_type ulPrachCfg [TDSULLOG_MAX_PRACH_FRACH_PAIRS];
  // 96 * 8 = 768 bits = 96 bytes in worst case

} tdsullog_prach_fpach_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_PRACH_FPACH_CFG_PKT_LEN(num_prach_fpach_pairs)  ( \
    FPOS(tdsullog_prach_fpach_cfg_pkt_type, ulFpachCfg)+ \
    ( num_prach_fpach_pairs * sizeof(tdsullog_fpach_cfg_type)) + \
    ( num_prach_fpach_pairs * sizeof(tdsullog_prach_cfg_type)))


/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_UL_RANDOM_ACCESS_CFG_LOG_PACKET
                 (LOG CODE: 0xD026)
                     
This log packet holds the description of the TD-SCDMA Random Access 
configuration informatino. 
 
This record is generated when UE attempts a PRACH transmission.

--------------------------------------------------------------------------*/
/*! \brief TDSCDMA L1 Uplink ASC cfg info
 * 32 bits total
 */
typedef struct
{
  /*! \brief Indicate which UL sync codes are available */
  uint32  syncUlCodeMask:8;  
  /*! \brief Sub-channel size : Integer (1, 2, 4, 8) */
  uint32  subChanSize:4;
  /*! \brief Indicate which sub-channels are available */     
  uint32  subChanMask:8;     
  /*! \brief Reserved */
  uint32  :12;
// 32 bits

} tdsullog_asc_cfg_type;


/*! \brief        LOG PACKET: TDSCDMA_L1_UL_RANDOM_ACCESS_CFG_LOG_PACKET
                 (LOG CODE: 0xD026)
 
This log packet is for internal use only! 
                     
This log packet holds the description of the TD-SCDMA Random Access 
configuration information. 
 
This record is generated when UE attempts a PRACH transmission.
 
MIN Size: 8 bytes, MAX Size: 40 bytes
32 + 32 + 256 = 320 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;  
  /*! \brief Corresponding CCTRCH ID number */       
  uint32  cctrchID:4;          
  /*! \brief SYNC-UL code bitmap: Each bit represents a SYNC_UL code */      
  uint32  syncUlcodeBitmap:8;  
  /*! \brief SYNC-UL code power ramp step: Integer (0,  3) in dB */
  uint32  syncUlCodePwrRampStep:2;  
  /*! \brief Maximum SYNC-UL transmission time: Integer (1, 2, 4, 8) */
  uint32  maxSyncUlTxTime:4;  
  /*! \brief Reserved */
  uint32  :6;
// 32 bits

  /*! \brief Desired UpPCH received power: Enum (-120,  -58) dBm in step of 1dB */     
  int8    desUppchRcvPwr;         
  /*! \brief UpPCH shiftingInteger (0,  127) */           
  uint8   uppchShift; 
  /*! \brief PCCPCH Tx Power, for computing pathloss: Integer (6,  43) in dBm */      
  uint8   pccpchTxPwr; 
  /*! \brief Number of ASCInteger (1,  8) */            
  uint8   numAsc;                 
// 32 bits

  /*! \brief  ASC cfg info, actual number determined by numAsc */
  tdsullog_asc_cfg_type ulAscCfg [TDSULLOG_MAX_NUM_ASC];
  // 32 * 8 = 256 bits = 32 bytes in worst case

} tdsullog_Random_Access_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_RANDOM_ACCESS_CFG_PKT_LEN(num_asc)  ( \
    FPOS(tdsullog_Random_Access_cfg_pkt_type, ulAscCfg)+  \
    ( num_asc * sizeof(tdsullog_asc_cfg_type)))

/* + defangc@2011-10-10 */

/*! \brief       LOG PACKET: TDSCDMA_L1_UL_FW_UPPCH_FPACH_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD027)

This log packet is for internal use only! 

This log packet holds the description of the UPPCH/FPACH configuration information
that sent to FW for one random access attempt.

This record is generated when UE attempts a SYNC_UL code transmission.

MIN Size: 16 bytes, MAX Size: 16 bytes
32 + 32 + 32 +32= 128 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;  
  /*! \brief Activate mode:
   *  0 - None 
   *  1 - PRACH
   *  2 - HHO resync
   *  3 - ERUCCH 
   */
  uint32  act_mode:2;
  uint32  :6;

  /*! \brief Current sub-frame number (0..8191) */
  uint32  cur_subfrm:16;

  /* Word 1 */
  /*! \brief Valid value : 
   *  \li TFW_CMD_ACTION_DISABLE : random access procedure aborted
   *  \li TFW_CMD_ACTION_ENABLE : send UL sync and mointor FPACH
   */
  uint32  actionType:1;
  /*! \brief Time slot used by FPACH, valid value: [0,2,3,4,5,6] */
  uint32  fpachTimeSlot:6;
  uint32  :1;
  /*! \brief Maximum number of sub-frames to wait for the network 
   *  acknowledgement to a sent signature. Valid[1..4]
   */
  uint32  wt:3;
  /*! \brief 16SF for the physical channel of fpach, valid: [1..16]*/
  uint32  fpachCode:5;
  /*! \brief Sequence number for this random access procedure. Valid range [0..255] */
  uint32  txSeqNum:8;
  /*! \brief UL Sync Code. Valid range [0..255] */
  uint32  ulSyncCode:8;

  /* Word 2 */
  /*! \brief Sub-frame number to start transmission on UpPCH. Valid [0..8191] */
  uint16  ulSyncTxSubFrm;
  /*! \brief Flag indicating whether or not to use the timeAdv */
  uint16  useInitialTimeAdv;  
  /*! \brief Initial timing advance for UpPCH in Cx8. Default 48*8 Cx8.*/
  int16    timeAdv;
  /*! \brief UL Sync TX Power including step and pathloss compensation, if any.
   *  Valid <TBD> [-50dBm..33dBm] in 1/16 dBm step
   */
  int16   ulSyncTxPwr;
} tdsullog_fw_uppch_fpach_cfg_cmd_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_FW_UPPCH_FPACH_CFG_CMD_PKT_LEN ( \
    sizeof(tdsullog_fw_uppch_fpach_cfg_cmd_pkt_type))

/*! \brief       LOG PACKET: TDSCDMA_L1_UL_FW_PRACH_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD028)

This log packet is for internal use only! 

This log packet holds the description of the PRACH configuration information
that sent to FW for RACH transmission.

This record is generated when UE sends RACH data to network after get FPACH ACK.

MIN Size: 12 bytes, MAX Size: 12 bytes
32 + 32 + 32 = 96 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;  

  /*! \brief Message TTI length in subframe. Valid [1,2,4] for 5,10,20ms. value 4 (20ms)
   *  is only valid for PRACH and not valid for E-RUCCH 
   */
  uint32  msgTTIlen:4;
  /*! \brief Time slot used by PRACH, valid value: [1,2,3,4,5] */
  uint32  timeSlot:4;
  /*! \brief Absolute Midamble Shift to be used for PRACH. 
   *  Valid [0, 128/k, 2*(128/k), .., (k-1)*(128/k) ], where K is midamble 
   *  configuration [2,4,6,8,10,12,14,16] 
   */
  uint32  midambleShift:8; 

  /*! \brief 16SF for the physical channel of PRACH, valid: [1..16]*/
  uint32  chanCode:8;

  /* Word 1 */
  /*! \brief Spreading factor for PRACH or E-RUCCH. 
   *  Valid value : [4,8,16] for PRACH, [8,16] for E-RUCCH  
   */
  uint32  sf:5;
  uint32  :11;

  /*! \brief Sub-frame number to start transmission on PRACH. Valid [0..8191] */
  uint32  txSubFrm:16;

  /* Word 2 */
  /*! \brief Starting position of the UpPCH from FPACH in 1/8 chip. 
   *  Valid[0..8191] 
   */
  uint16  upPchPos;

  /*! \brief PRACH TX power includes BetaD and compensation for pathloss, if any. 
   *  Valid <TBD> [-50dBm .. 33dBm] in 1/16 dBm step 
   */
  int16   prachTxPwr;
} tdsullog_fw_prach_cfg_cmd_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_FW_PRACH_CFG_CMD_PKT_LEN ( \
    sizeof(tdsullog_fw_prach_cfg_cmd_pkt_type))


/*! \brief DPCH per physical channel configuration
 * 64 bits total
 */
typedef struct
{
  /* Word 0 */
  /*! \brief Timeslot id. Valid [0,1,2,3,4] for TS1~TS5 *
    * 0 - TS1
    * 1 - TS2
    * 2 - TS3
    * 3 - TS4
    * 4 - TS5
    */
  uint32  ts_id:3;
  
  /*! \brief OVSF Code. Valid [1..sf], sf=1,2,4,8,16 */
  uint32  chanCode:5;
  /*! \brief Spreading factor for DPCH. Valid [1,2,4,8,16] */
  uint32  sf:5;
  /*! \brief Source CCTrCH Index, need to be consistent with CCTrCH number in
   *  TX encode. Valid [0,1], 0: 1st CCTrCH, 1: 2nd CCTrCH 
   */
  uint32  cctrchId:3;

  uint32  :16;

  /* Word 1 */
  /*! \brief Number of SS TPC bits in an OVSF Code Channel. Valid [0,2,4,8,16,32] */
  uint32  numSsTpc:6;
  /*! \brief Number of TFCI bits in an OVSF Code Channel. Valid [0,2,4,8,16,32] */
  uint32  numTFCI:6;
  /*! \brief Absolute Midamble Shift to be used for DPCH
   *  Valid [0, 128/k, 2*(128/k), .., (k-1)*(128/k) ], where K is midamble 
   *  configuration [2,4,6,8,10,12,14,16] 
   */
  uint32  midambleShift:8;
  uint32  :12;
} tdsullog_dpch_per_phy_config ;

/*! \brief       LOG PACKET: TDSCDMA_L1_UL_FW_DPCH_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD029)

This log packet is for internal use only! 

This log packet holds the description of the DPCH configuration information
that sent to FW for Tx, SB or DTX.

This record is generated when UE sends DPCH config command to FW.

MIN Size: 12 bytes, MAX Size: 92 bytes
32 + 32 + 32 + 64*10 = 736 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;  

  /*! \brief Current CFN (0..255)  */
  uint32  cur_cfn:8;

  /*! \brief Current sub-frame number (0..8191) */
  uint32  cur_subfrm:16;

  /* Word 1 */
  /*! \brief UL CCTrCH DTX indication. Valid only when "valid" = TFW_UL_CCTRCH_VALID
   *  \li 1: This CCTrCH is DTXed 
   *  \li 0 : This CCTrCH is not DTXed 
   */
  uint32  dtx:1;
  /*! \brief UL Special Burst indication. Valid only when "valid" = TFW_UL_CCTRCH_VALID
   *  \li 1 : This CCTrCH contains special burst
   *  \li 0 : This CCTrCH does not constain special burst 
   */
  uint32  sb:1;
  uint32  :14;
  
  /*! \brief Uncoded TFCI for this CCTrCH. Valid Range [0..1023]. =0 for Special
   *  Burst or DTX 
   */
  uint32  uncodedTFCI: 16;

  /* Word 2 */
  /*! \brief DPCH TX power including BetaD gain (from TFC) and pathloss 
   *  compensation, if any, but excluding Gama gain (from SF) per CCTrCH.
   *  Valid <TBD> [-50dBm..33dBm] in 1/16 dBm step 
   */
  int16   dpchTxPwr; 

  /*! \brief The number of physical channels that are used */
  uint16  num_phy_chan_used;

  /*! \brief The DPCH configuration per physical channels, actual number determined by num_phy_chan_used */
  tdsullog_dpch_per_phy_config  dpch_per_phy_config[TDSULLOG_MAX_DPCH_PHY_CHAN];
} tdsullog_fw_dpch_cfg_cmd_pkt_type;


/* Macro to determine the sizes of the log packet */
#define TDSULLOG_FW_DPCH_CFG_CMD_PKT_LEN(num_phy_chan_used) ( \
    FPOS(tdsullog_fw_dpch_cfg_cmd_pkt_type, dpch_per_phy_config)+ \
    ((num_phy_chan_used) * sizeof(tdsullog_dpch_per_phy_config)))


/*! \brief       LOG PACKET: TDSCDMA_L1_DL_FPACH_DECODE_RESULT_LOG_PACKET
                 (LOG CODE: 0xD015)

This log packet is for internal use only! 

This log packet holds the description of the FPACH decode result that sent by
FW via FPACH_DECODE_DONE_ISR.

This record is generated when FPACH_DECODE_DONE_ISR is triggered.

MIN Size: 16 bytes, MAX Size: 16 bytes
32 + 32 + 32 + 32 = 96 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;  

  /*! \brief Activate mode:
   *  0 - None 
   *  1 - PRACH
   *  2 - HHO resync
   *  3 - ERUCCH 
   */
  uint32  act_mode:2;

  uint32  :6;

  /*! \brief Current sub-frame number (0..8191) */
  uint32  cur_subfrm:16;

  /* Word 1 */
  /*! \brief FPACH Status. Valid [TFW_UL_SYNC_NO_ACK, TFW_UL_SYNC_ACK, TFW_UL_SYNC_ABORT] */
  uint32  status:8;

  /*! \brief Received Signature Reference Number. Valid only if status is ACK*/
  uint32  sigRefNo:8;

  /*! \brief Sequence number for this random access procedure. Valid range [0..255] */
  uint32  txSeqNum:8;

  /*! \brief UL Sync Code. Valid range [0..255] */
  uint32  ulSyncCode:8;

  /* Word 2 */
  /*! \brief Sub-frame number to start transmission on UpPCH. Valid [0..8191] */
  uint32  ulSyncTxSubFrm:16;

  /*! \brief Received Relative Sub-Frame Number. Valid only if status is ACK */
  uint32  relSubFrm:8;

  /*! \brief Received Transmit Power Level Command for RACH message. 
   *  Valid only if status is ACK
   */
  uint32  prxPRACH:8;

  /* Word 3 */
  /*! \brief Received starting position of the UpPCH. Valid [0..8191] in 1/8 
   *  chip. Valid only if status is ACK 
   */
  uint32  upPchPos:16;
  /*! \brief Energy Metric for fpach decoding*/
  uint32  energyMetric:16;
  /*! \brief errorCnt for fpach decoding*/
  uint32  errorCnt : 7;
  uint32  :25;
} tdsullog_fpach_decode_result_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_FPACH_DECODE_RESULT_PKT_LEN ( \
    sizeof(tdsullog_fpach_decode_result_pkt_type))

/*! \brief       LOG PACKET: TDSCDMA_L1_UL_FW_UpPCH_TX_PWR_TIMING_CFG_LOG_PACKET
                 (LOG CODE: 0xD06F)

This log packet is for internal use only! 

This log packet holds the description of the UpPCH Power and Timing Control parameters.

This record is generated when UE initial random access.

MIN Size: 5 bytes, MAX Size: 5 bytes
32 + 32 = 64 bits 
*/

typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;

  /*! \brief  
   * 0 - None 
   * 1 - PRACH 
   * 2 - HHO resync  
   * 3 - ERUCCH 
   */
  uint32  act_mode:4;

  /*! \brief Maximum Allowed Combined TX output power. 
   * [-50dBm..33dBm] in 1/16 dBm step. Note that FW only keeps one copy of maxTxPwr 
   * and will overwrite with the latest message with maxTxPwr
   */
  uint32  maxTxPwr:16;

  /*! \brief vacant bits for word align purpose */
  uint32 :4;

  /*Word 1*/
  /*! \brief Initial timing advance for UpPCH in Cx8. Default 48*8 Cx8 */
  uint16  timeAdv;
  
  /*! \brief Timing adjustment for UpPCH relative to DwPCH. 
   *  Valid [0..127] in 16 chip step  
   */
  uint8  nUpPCHShift;

  /*! \brief Flag indicating whether or not to use the timeAdv */
  uint8  useInitialTimeAdv;
}tdsullog_fw_uppch_tx_pwr_timing_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_FW_UpPCH_TX_PWR_TIMING_CFG_PKT_LEN ( \
    sizeof(tdsullog_fw_uppch_tx_pwr_timing_cfg_pkt_type))

/*! \brief       LOG PACKET: TDSCDMA_L1_UL_FW_DPCH_TX_PWR_TIMING_CFG_LOG_PACKET
                 (LOG CODE: 0xD070)

This log packet is for internal use only! 

This log packet holds the description of the DPCH Power and Timing Control parameters.

This record is generated when UE initial DPCH TX.

MIN Size: 12 bytes, MAX Size: 12 bytes
32 + 32 + 32= 96 bits 
*/

typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;

  /*! \brief Maximum Allowed Combined TX output power.
   * [-50dBm..33dBm] in 1/16 dBm step. Note that FW only keeps one copy of maxTxPwr
   * and will overwrite with the latest message with maxTxPwr 
   */
  uint32  maxTxPwr:16;

  /*! \brief Frequency of the adjustment of the uplink transmission timing.
   * Valid [1..8] in subframe
   */
  uint32  timeAdjStep:4;

  /*! \brief Frequency of the adjustment of the uplink transmission timing.
   * Valid [1..8] in subframe  
   */
  uint32  timeAdjFreqM:4;

  /*Word 1*/
  /*! \brief TPC Step Size. Valid [1,2,3] in dB */
  uint32  tpcStep:2;
  
  /*! \brief UL Timing Advance Control. 
   * Valid [TFW_UL_DPCH_TIME_ADV_CTRL_ENABLE, TFW_UL_DPCH_TIME_ADV_CTRL_DISABLE]. Note
   * that though it is an optional IE, SW should enable timeAdv 
   */
  uint32  timeAdvCtrl:1;

  /*! \brief Flag used to indicate that tpcAccInitVal is valid and shall be applied.
   * This is mainly used in HO failure recovery. Valid [0,1]. 1: use tpcAccInitVal,
   * 0: otherwise 
   */
  uint32  useTPCAccInit:1;

  /*! \brief Valid when useTPCAccInit = 1. In dB*/
  uint32 tpcAccInitVal:8;

  /*! \brief Valid when useInitialTimeAdv = TRUE. In 1/8 chip*/
  uint32 timeAdv:16;

  /*! \brief Flag used to indicate that timeAdv is valid and shall be applied.
   * This is mainly used in HO failure recovery. Valid [TRUE,FALSE]. TRUE: use timeAdv,
   * FALSE: otherwise
   */
  uint32 useInitialTimeAdv:1;

  /*! \brief Number of CCTrCH of DPCH type in this configuration. Valid [1,2]*/
  uint32 numCCTrCH:2;

  /*! \brief vacant bits for word align purpose*/
  uint32 :1;

  /*Word 2*/
  /*! \brief vacant bits for word align purpose*/
  uint32 numCCTrCHTimeSlotPairs:4;

  /*! \brief Number of UL time slots carrying CCTrCHs. Valid [0...5]*/
  uint32 numCCTrCHTimeSlots:3;

  /*! \brief UL DPCH tx power boost according to DL DPCH SNR ON/OFF*/
  uint32 enableTxPwrBoost:1;

  /*! \brief UL DPCH  tx power increase step*/
  uint32 txPwrBoostStep:8;

  /*! \brief snrDiffThres*/
  uint32 snrDiffThres:8;

  /*! \brief vacant bits for word align purpose*/
  uint32 :8;
}tdsullog_fw_dpch_tx_pwr_timing_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_FW_DPCH_TX_PWR_TIMING_CFG_PKT_LEN ( \
    sizeof(tdsullog_fw_dpch_tx_pwr_timing_cfg_pkt_type))

/*! \brief       LOG PACKET: TDSCDMA_L1_UL_ERUCCH_SI_DATA_LOG_PACKET
                 (LOG CODE: 0xD071)

This log packet is for internal use only! 

This log packet holds the description of the ERUCCH payload.

This record is generated when UE sends ERUCCH data to network after get FPACH ACK.

MIN Size: 5 bytes, MAX Size: 5 bytes
32 + 32 = 64 bits 
*/

typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;
  
  /*! \brief serving cell and neigbhor cells path loss information */
  uint32  snpl:5;

  /*! \brief UE Power Headroom */
  uint32  uph:5;

  /*! \brief Total E-DCHBuffer Status */
  uint32  tebs:5;

  /*! \brief Highest priority Logical channel Buffer Status */
  uint32  hlbs:4;
  
  /*! \brief Highest priority Logical channel ID (HLID) */
  uint32  hlid:4;
  
  /*! \brief vacant bits for word align purpose */
  uint32  :9;

  /*Word 1*/
  /*! \brief ERNTI */
  uint32 ernti:16;
  /*! \brief vacant bits for word align purpose */
  uint32 :16;
}tdsullog_erucch_si_data_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_ERUCCH_SI_DATA_PKT_LEN ( \
    sizeof(tdsullog_erucch_si_data_pkt_type))
    

/*! \brief       LOG PACKET: TDSCDMA_L1_UL_FW_ERUCCH_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD072)

This log packet is for internal use only! 

This log packet holds the description of the ERUCCH configuration information
that sent to FW for ERUCCH transmission.

This record is generated when UE sends RACH data to network after get FPACH ACK.

MIN Size: 12 bytes, MAX Size: 12 bytes
32 + 32 + 32 = 96 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;  

  /*! \brief Message TTI length in subframe. Valid [1,2,4] for 5,10,20ms. value 4 (20ms)
   *  is only valid for PRACH and not valid for E-RUCCH 
   */
  uint32  msgTTIlen:4;
  /*! \brief Time slot used by PRACH, valid value: [1,2,3,4,5] */
  uint32  timeSlot:4;
  /*! \brief Absolute Midamble Shift to be used for ERUCCH. 
   *  Valid [0, 128/k, 2*(128/k), .., (k-1)*(128/k) ], where K is midamble 
   *  configuration [2,4,6,8,10,12,14,16] 
   */
  uint32  midambleShift:8; 

  /*! \brief 16SF for the physical channel of PRACH, valid: [1..16]*/
  uint32  chanCode:8;

  /* Word 1 */
  /*! \brief Spreading factor for PRACH or E-RUCCH. 
   *  Valid value : [4,8,16] for PRACH, [8,16] for E-RUCCH  
   */
  uint32  sf:5;
  
  uint32  :11;
  
  /*! \brief Sub-frame number to start transmission on ERUCCH. Valid [0..8191] */
  uint32  txSubFrm:16;

  /* Word 2 */
  /*! \brief Starting position of the UpPCH from FPACH in 1/8 chip. 
   *  Valid[0..8191] 
   */
  uint16  upPchPos;

  /*! \brief ERUCCH TX power includes BetaD and compensation for pathloss, if any. 
   *  Valid <TBD> [-50dBm .. 33dBm] in 1/16 dBm step 
   */
  int16   erucchTxPwr;
} tdsullog_fw_erucch_cfg_cmd_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSULLOG_FW_ERUCCH_CFG_CMD_PKT_LEN ( \
    sizeof(tdsullog_fw_erucch_cfg_cmd_pkt_type))

#endif /* TDSULLOG_H */

