#ifndef TDSDLLOG_H
#define TDSDLLOG_H

/*===========================================================================
                     T D S D L L O G . H

DESCRIPTION
  This header file describes the Layer 1 DownLink log packet definitions.

Copyright (c) 2001-2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdllog.h#1 $ 
 
when        who     what, where, why
--------    ---     --------------------------------------------------------

05/19/2011  sk    initial cut for TD-SCDMA (R4) L1 downlink    
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "tdscdma_variation.h"
#include "log.h"
#include "log_codes.h"
#include "log_codes_tdscdma.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/*! \brief Downlink - Max number of Transport Format  */
#define TDSDLLOG_MAX_TF                      64
/*! \brief Downlink - Max number of Transport channel */
#define TDSDLLOG_MAX_TRCH                    8
/*! \brief Downlink - Max number of CCTrCH */
#define TDSDLLOG_MAX_CCTRCH                  3
/*! \brief Downlink - Max number of Transport Format Combination */
#define TDSDLLOG_MAX_TFC                     128
/*! \brief Downlink - Max number of Common Physical Chaannel */
#define TDSDLLOG_MAX_COMMON_PHYCH            3
/*! \brief Downlink - Max number of DPCH time slots */
#define TDSDLLOG_MAX_DPCH_TS                 6  

/* LOG_TDSCDMA_BASE_C has been defined in log_codes.h */

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_TRCH_CFG_LOG_PACKET
                 (LOG CODE: 0xD010)
 
This log packet will contain the configuration information 
of current active TD-SCDMA L1 DL TRransport CHannels. 
This log record is generated whenever each channel is setup 
or reconfigured. For each CCTRCH channel the information in 
this packet is padded sequentially
--------------------------------------------------------------------------*/
/*! \brief TDSCDMA L1 DownLink TrCH cfg 
 * 64 bits total
 */
typedef struct
{
  /*! \brief Validity of this TrCH : 0 - FALSE, 1 - TRUE */
  uint32  trchValid:1;   
  /*! \brief Transport channel ID, Integer number 1 to 32 from RRC  */     
  uint32  trchId:6;           
  /*! \brief TrCh type : 0 - BCH, 1 - PCH, 2 - FACH, 3 - DCH */
  uint32  trchType:2;
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
  /*! \brief Rate-matching attribute of the channel; number from 1 to 256*/       
  uint32  rmAttr:9;   
  /*! \brief Reserved */
  uint32  :6;
//32 bits
      
  /*! \brief Number of transport formats per channel; number from 1 to 64  */
  uint32  numTf:7;  
  /*! \brief TF info base in the TF info table */          
  uint32  tfInfoBase:7;       
  /*! \brief Reserved */
  uint32  :18;
//32 bits
} tdsdllog_trch_cfg_type;

/*! \brief TDSCDMA L1 DownLink CCTrCH cfg 
 * 32 + (64 * 8) = 544 bits total
 */
typedef struct
{
  /*! \brief CCTRCH ID number from RRC  */
  uint32  cctrchId:4;  
  /*! \brief Number of TrCH configutations, max number is 8 */   
  uint32  numTrch:4;      
  /*! \brief Reserved */
  uint32  :24;
//32 bits

  /*! \brief TrCH configutations, actual number determined by numTrch */
  tdsdllog_trch_cfg_type dlTrchCfg [TDSDLLOG_MAX_TRCH];

} tdsdllog_cctrch_cfg_type;

/*! \brief TDSCDMA L1 DownLink Transport Format info 
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
//32 bits

} tdsdllog_tf_Info_type;

/*! \brief        LOG PACKET: TDSCDMA_L1_DL_TRCH_CFG_LOG_PACKET
                 (LOG CODE: 0xD010)
 
This log packet is for internal use only! 
 
This log packet will contain the configuration information 
of current active TD-SCDMA L1 DL TRransport CHannels. 
This log record is generated whenever each channel is setup 
or reconfigured. For each CCTRCH channel the information in 
this packet is padded sequentially
 
MIN Size: 4 bytes, MAX Size: 464 bytes
32 + (32*64) + 32 + (544 * 3) = 3744 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;       
    /*! \brief number of Transport Format Info table */
  uint32 num_tf: 8;
  /*! \brief Number of CCTRCH config info table */
  uint32  numCctrch:4;       
  /*! \brief Reserved */
  uint32  :12;
//32 bits

  /*! \brief Transport Format Info table,actual number determined by num_tf */
  tdsdllog_tf_Info_type dlTfInfoTbl [TDSDLLOG_MAX_TF];
  // 32*64 = 2048bits = 256 bytes

  /*! \brief CCTRCH config info table, actual number determined by numCctrch */
  tdsdllog_cctrch_cfg_type dlCctrchCfg [TDSDLLOG_MAX_CCTRCH];
  // 544 * 3 = 1632 bit = 204 butes in worst case

} tdsdllog_trch_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSL1LOG_DL_TRCH_CFG_PKT_LEN(num_tf,num_cctrch)  ( \
    FPOS(tdsdllog_trch_cfg_pkt_type, dlTfInfoTbl)+  \
    ( num_tf * sizeof(tdsdllog_tf_Info_type))+\
    ( num_cctrch * sizeof(tdsdllog_cctrch_cfg_type)))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_TFC_CFG_LOG_PACKET
                 (LOG CODE: 0xD011)
                     
TD-SCDMA L1 DL Transport  Format Combination (LOG_CODE 0x4115/TBD)
This log packet will contain the TD-SCDMA DL transport format combinations. 
This record is generated upon new channel set up (re-used from WCDMA ICD). 
 
NOTE:	The TF_IDX is an 8-element array inside the TFC information array. 
Unused portions of TF_IDX are always set to zero.

Together with L1_DL_TRCH_CFG_LOG, this packet is used to determine the 
current received channel rate. The field TFI_IDX in this packet points 
to a current (specific) TFC for each transport channel. That TFC can 
then be looked up in with L1_DL_TRCH_CFG_LOG to find out the predefined rate. 
--------------------------------------------------------------------------*/
/*! \brief TDSCDMA L1 DownLink TFC info 
 * 32 + 64 bits total = 96 bytes
 */
typedef struct
{
  /*! \brief This TFC valid or not : 0 - Not valid, 1 - Valid */
  uint8  tfcValid;   
  /*! \brief Reserved */                   
  uint8  reserved[3];
// 32 bits

  /*! \brief TF index for TrCHs in this TFC : 
   *  1st byte - transport format index for the 1st transport channel
   *  2nd byte - transport format index for the 2nd transport channel
   *     *  8th byte - transport format index for the 8th transport channel
   *  Note. A transport format index: 0 to 63
   */
  uint8  tfIdx [TDSDLLOG_MAX_TRCH];      
// 64 bits 

} tdsdllog_tfc_cfg_type;

/*! \brief TDSCDMA L1 DownLink CCTrCH TFC info 
 * 32 bits total = 4 bytes
 */
typedef struct
{
  /*! \brief CCTrCH ID; channel ID from 0 to 5  */
  uint32  cctrchID:4;  
  /*! \brief Number of TFC integer 1-128 */    
  uint32  numTfc:8;  
  /*! \brief TFC info base in the TFC info table */      
  uint32  tfcInfoBase:8;   
  /*! \brief Reserved */
  uint32  :12;
//32 bits

}tdsdllog_cctrch_tfc_cfg_type;

/*! \brief        LOG PACKET: TDSCDMA_L1_DL_TFC_CFG_LOG_PACKET
                 (LOG CODE: 0xD011)
 
This log packet is for internal use only! 

TD-SCDMA L1 DL Transport  Format Combination (LOG_CODE 0x4115/TBD)
This log packet will contain the TD-SCDMA DL transport format combinations. 
This record is generated upon new channel set up (re-used from WCDMA ICD). 
 
NOTE:	The TF_IDX is an 8-element array inside the TFC information array. 
Unused portions of TF_IDX are always set to zero.

Together with L1_DL_TRCH_CFG_LOG, this packet is used to determine the 
current received channel rate. The field TFI_IDX in this packet points 
to a current (specific) TFC for each transport channel. That TFC can 
then be looked up in with L1_DL_TRCH_CFG_LOG to find out the predefined rate. 
 
MIN Size: 4 bytes, MAX Size: 1552 bytes
32+ 12288 + 32 + 96 = 12448 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;       
  /*! \brief Actual number of tfc */
  uint32  num_tfc:16; 
  /*! \brief Actual number of CCTrCH */
  uint32  numCctrch:4;  
  /*! \brief Reserved */
  uint32  :4;
//32 bits

  /*! \brief TFC info table, actual number determined by num_tfc */
  tdsdllog_tfc_cfg_type dlTfcCfg [TDSDLLOG_MAX_TFC];
// 96 * 128 = 12288 bits = 1536 bytes
  
  /*! \brief TFC config per CCTrCH ,actual number determined by numCctrch*/
  tdsdllog_cctrch_tfc_cfg_type dlCctrchTfcCfg [TDSDLLOG_MAX_CCTRCH];
  // 32 * 3 = 96 bits = 12 bytes

} tdsdllog_tfc_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSL1LOG_DL_TFC_CFG_PKT_LEN(num_tfc,numCctrch)  ( \
    FPOS(tdsdllog_tfc_cfg_pkt_type, dlTfcCfg)+  \
    ( num_tfc * sizeof(tdsdllog_tfc_cfg_type))+\
    ( numCctrch * sizeof(tdsdllog_cctrch_tfc_cfg_type)))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_COMMON_PHYCHAN_CFG_LOG_PACKET
                 (LOG CODE: 0xD012)
                     
This log packet will contain the TD-SCDMA DL physical channel configuration 
information. 
This log record is generated whenever each channel is setup or reconfigured.

--------------------------------------------------------------------------*/
/*! \brief TDSCDMA L1 DownLink PCCPCH info 
 * 64 bits total
 */
typedef struct
{
// union pccpch

  /*! \brief CCTRCH ID number from RRC */          
  uint32  cctrchID:4;  
  /*! \brief UTRA Absolute Radio Frequency Number */          
  uint32  uarfcn:16; 
  /*! \brief Cell Parameter ID (0..127)*/            
  uint32  cpi:8;                 
  /*! \brief PCCPCH dection method :
   *  0 - No TTI Detection needed
   *  1 - Phase based TTI detection
   *  2 - Blind TTI detection
   *  3 - Not used
   */
  uint32  pccpchDetetionType:2;  
  /*! \brief TSTD enabled or not : 
   *  0 - TSTD Not enabled
   *  1 - TSTD enabled
   */
  uint32  tstdInd:1;             
  /*! \brief Reserved */
  uint32  :1;
  // 32 bits
} tdsdllog_pccpch_cfg_type;

/*! \brief TDSCDMA L1 DownLink SCCPCH info 
 * 128 bits total
 */
typedef struct
{
// union sccpch

  /*! \brief CCTRCH ID number from RRC */          
  uint32  cctrchID:4;  
  /*! \brief UTRA Absolute Radio Frequency Number */          
  uint32  uarfcn:16; 
  /*! \brief Cell Parameter ID (0..127) */            
  uint32  cpi:8;   
  /*! \brief Second interleaving mode : 
   *  0 - Frame based interleaving
   *  1 - Slot based interleaving
   */
  uint32  secondIntlvMode:1;   
  /*! \brief Reserved */
  uint32  :3;                
// 32 bits
  
  /*! \brief The number of bits for the TFCI bits code word : Valid value: 4,8,16,32 for QPSK configuration.  */
  uint32  tfciLen:6; 
  /*! \brief Puncturing limit,  40 to 100  step by 4. (0.40..1.0 by step of 0.04) * 100 */            
  uint32  punctLim:7;   
  /*! \brief TDM repetition period. Valid value:  1, 2,4,8,16,32,64. */         
  uint32  repPrd:7;   
  /*! \brief TDM repetition length. Valid value: 1 to  63.*/             
  uint32  repLen:6;       
  /*! \brief TDM repetition Offset: SFN mod Repetition period. Valid value: 0 - Repetition period -1. */
  uint32  repOffset:6;           
// 32 bits

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
  /*! \brief Timeslot number. Valid value: 0,2,3,4,5,6. */      
  uint32  tsNum:3;   
  /*! \brief Amount of SS and TPC bits send in this timeslot. 
   *  0 - 0 SS symbol
   *  1 - 1 SS symbol
   *  2 - 16/SF SS symbols
   */             
  uint32  ssTpcSymbols:2;      
  /*! \brief The number of additional codes in this timeslot that carry TPC and SS symbols. Valid value: 1 to 15. */   
  uint32  addiTpcSsSymbols:4;     
  /*! \brief Reserved */
  uint32  :12;
// 32 bits

  /*! \brief Code channel bitmap for SCCPCH. Bit 0..15 for code 0..15 */
  uint32  codeChBitmap:16;       
  /*! \brief Reserved */
  uint32  :16;
  // 32 bits
} tdsdllog_sccpch_cfg_type;

/*! \brief TDSCDMA L1 DownLink PICH info 
 * 160 bits total
 */
typedef struct
{  //union PICH

  /*! \brief UTRA Absolute Radio Frequency Number */         
  uint32  uarfcn:16; 
  /*! \brief Cell Parameter ID (0..127) */            
  uint32  cpi:8;  
  /*! \brief Time slot used by paging indication channel. Valid value: 0,2,3,4,5,6 */               
  uint32  tsNum:3;   
  /*! \brief The power transmitted on the PICH minus power of PCCPCH Tx Power.
   *  0 - 15 represent -10 to +5.
   */
  uint32  powerOffset:4;               
  /*! \brief Reserved */
  uint32  :1;
// 32 bits

  /*! \brief TDM repetition period. Valid value:  1, 2,4,8,16,32,64. */        
  uint32  repPrd:7; 
  /*! \brief TDM repetition length. Valid value: 1 to  63.*/               
  uint32  repLen:6;                
  /*! \brief TDM repetition Offset: SFN mod Repetition period. Valid value: 0 - Repetition period -1. */
  uint32  repOffset:6;             
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
  /*! \brief Length of one paging indicator in bits : 
   *  0 - 4 bits
   *  1 - 8 bits
   *  2 - 16 bits
   */      
  uint32  piLen:2; 
// 32 bits

  /*! \brief Number of frames between the last frame carrying PICH for this Paging Occasion and the first frame carrying paging messages for this Paging Occasion.
   *  0 - 2 frames
   *  1 - 4 frames
   *  2 - 8 frames
   */              
  uint32  pichPchGap:2; 
  /*! \brief NNumber of PCH paging groups (Npch). Valid value: 1 to 8 */         
  uint32  numPagingGroup:4;  
  /*! \brief Reserved */ 
  uint32  :2;
  /*! \brief Code channel bitmap for PICH. Bit 0..15 for code 0..15 */
  uint32  codeChBitmap:16; 
  /*! \brief q = PI mod NPI where NPI  is determined by LPI . q has Range (0..88-1).
   *  Each paging indicator Pq (where Pq, q = 0, ..., NPI-1, Pq   {0, 1}) in one radio frame is mapped to
   *  the bits {s2LPI*q+1,...,s2LPI*(q+1)} where LPI  is length of paging indicator.
   *  qPrimeValue (q') is defined as the index of current paging indicator's first bit in the SFNprime subframe:
   *  q' = 2 *LPI * q+1,  Range (1..349).
   *  q' has Range(1..173) in 1st subframe,
   *  q' has Range (177 ..349) in 2nd subframe.
   *  Page Indicator (PI):
   *  PI = DRX_index mod Np
   *  where DRX_index = IMSI div 8192 and NP= NPICH * NPI 
   *  Integer (0..351).
   */       
  uint32  qVal:7; 
  /*! \brief Reserved */ 
  uint32  :1;
// 32 bits
  
  /*! \brief IMSI divided by K, as per 8.3 in 25.304, where K = number of SCCPCH that have a PCH mapped. 
   *  This element is used to calculate paging occasion and paging message receiving occasion.
   *  Paging Occasion= {(IMSI div K) mod (DRX_ cycle_length div PBP)} * PBP + frame_offset+ i * DRX_cycle_length
   *  where i = 0,1,2as long as the resulting SFN is below its maximum value 4095.
   *  This value is calculated by RRC with i = 0.
   *
   *  Paging Message Receiving Occasion =Paging Occasion + NPICH + NGAP + {(DRX_index mod Np) mod NPCH } *2
   *  where NPICH is number of PICH frames.
   *
   *  This value is calculated by RRC with Paging Occasion when i=0.
   */    
  uint32  imsiDivPchCountL;   
// 32 bits

  uint32  imsiDivPchCountH;  
// 32 bits

  /*! \brief IMSI divided by 8192, as per 8.3 in 25.304.This element is used to calculate Paging Indicator value.
   *  Page Indicator (PI): PI = DRX_index mod Np
   *  where DRX_index = IMSI div 8192 and NP= NPICH * NPI 
   *
   *  Each paging indicator Pq (where Pq, q = 0, ..., NPI-1, Pq   {0, 1}) in one radio frame is
   *  mapped to the bits {s2LPI*q+1,...,s2LPI*(q+1)} where LPI is length of paging indicator,
   *  and q = PI mod NPI where NPI  is determined by LPI .
   */
  uint32  imsiDiv8192L; 
// 32 bits
  uint32  imsiDiv8192H; 
// 32 bits

} tdsdllog_pich_cfg_type;

/*! \brief TDSCDMA L1 DownLink Common physical Ch info 
 * ### bits total
 */
typedef union 
{ 
  /*! \brief PCCPCH */        
  tdsdllog_pccpch_cfg_type  pccpch;
  /*! \brief SCCPCH */    
  tdsdllog_sccpch_cfg_type  sccpch;   
  /*! \brief PICH */ 
  tdsdllog_pich_cfg_type    pich;      
}tdsdllog_phych_cfg_type;

/*! \brief TDSCDMA L1 DownLink Common physical Ch info 
 * ### bits total
 */
typedef struct 
{ 
  /*! \brief Common physical channel type : Channel type:
   *  0 - PCCPCH
   *  1 - SCCPCH
   *  2 - PICH
   */
  uint32  phyChType:8;  
  uint32  :24; 
  //32 bits
  
  tdsdllog_phych_cfg_type phych;
     
}tdsdllog_common_phych_cfg_type;


/*! \brief        LOG PACKET: TDSCDMA_L1_DL_COMMON_PHYCHAN_CFG_LOG_PACKET
                 (LOG CODE: 0xD012)
 
This log packet is for internal use only! 

This log packet will contain the TD-SCDMA DL physical channel configuration 
information. 
This log record is generated whenever each channel is setup or reconfigured.
 
 MIN Size: 4 bytes, MAX Size: 76 bytes
 32 + (160 * 3) = 512 bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;        
  /*! \brief Reserved */
  uint32  :20;
  /*! \brief Actual number of physical channels */
  uint32  numPhyCh:4;       
//32 bits

  /*! \brief Common physical channel config, actual number determined by numPhyCh*/
  tdsdllog_common_phych_cfg_type dlCommonPhyChCfg [TDSDLLOG_MAX_COMMON_PHYCH];
  // 160 * 3  = 480 bits
} tdsdllog_common_phych_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSL1LOG_DL_COMMON_PHYCH_CFG_PKT_LEN(num_phych)  ( \
    FPOS(tdsdllog_common_phych_cfg_pkt_type, dlCommonPhyChCfg)+  \
    ( num_phych * sizeof(tdsdllog_common_phych_cfg_type)))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_DPCH_CFG_LOG_PACKET
                 (LOG CODE: 0xD013)
                     
This log packet will contain the TD-SCDMA DL physical channel configuration 
information received over dedicated signaling. 
 
This log record is generated whenever a new dedicated physical channel is 
setup or reconfigured

--------------------------------------------------------------------------*/
/*! \brief TDSCDMA L1 DownLink DPCH Time Slot cfg info 
 * 64 bits total
 */
typedef struct
{
  /*! \brief Timeslot number. Valid value: 0,2,3,4,5,6. */
  uint32  tsNum:3; 
  /*! \brief The TFCI existence indication.
   *  0 - not existed
   *  1 - existed
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
  /*! \brief Modulation type:
   *  0 - QPSK
   *  1 - 8PSK 
   *  2 - 16QAM
   */        
  uint32  modulationtype:2;  
  /*! \brief Amount of SS and TPC bits send in this timeslot. 
   *  0 - 0 SS symbol
   *  1 - 1 SS symbol
   *  2 - 16/SF SS symbols
   */       
  uint32  ssTpcSymbols:2;     
  /*! \brief The number of additional codes in this timeslot that carry TPC and SS symbols. Valid value: 1 to 15. */    
  uint32  addiTpcSsSymbols:4;     
  /*! \brief Reserved */
  uint32  :9;
//32 bits

  /*! \brief  Code channel bitmap for DPCH.
   *  -	Bit 0..15 for code 0..15.
   * -	If all bits = 0, SF=1 and one code
   */
  uint32  codeChBitmap:16;        
  /*! \brief Reserved */
  uint32  :16;
  //32 bits

} tdsdllog_dpch_ts_info_type;


/*! \brief        LOG PACKET: TDSCDMA_L1_DL_DPCH_CFG_LOG_PACKET
                 (LOG CODE: 0xD013)
 
This log packet is for internal use only! 

This log packet will contain the TD-SCDMA DL physical channel configuration 
information received over dedicated signaling. 
 
This log record is generated whenever a new dedicated physical channel is 
setup or reconfigured
 
MIN Size: 12 bytes, MAX Size: 60 bytes
The worst case packet size for this log is 60 bytes.  
96 + (6*64) = 480bits 
*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;   
  /*! \brief CCTRCH ID number from RRC  */      
  uint32  cctrchID:4;   
  /*! \brief UTRA Absolute Radio Frequency Number */     
  uint32  uarfcn:16;         
  /*! \brief Reserved */
  uint32  :4;                
//32 bits

  /*! \brief Cell Parameter ID (0..127)*/
  uint32  cpi:8;    
  /*! \brief Activation time : start of the physical channel existence in CFN number. Valid value: 0 to 255 */         
  uint32  actTime:8;  
  /*! \brief Second interleaving mode : 
   *  0 - Frame based interleaving
   *  1 - Slot based interleaving
   */       
  uint32  secondIntlvMode:1; 
  /*! \brief The number of bits for the TFCI bits code word. Valid value: 4,8,16,32 for QPSK configuration. */
  uint32  tfciLen:6;  
  /*! \brief Puncturing limit,  40 to 100  step by 4. (0.40..1.0 by step of 0.04) * 100. */       
  uint32  punctLim:7;        
  /*! \brief Reserved */
  uint32  :2;
// 32 bits

  /*! \brief TDM repetition period. Valid value:  1, 2,4,8,16,32,64. */ 
  uint32  repPrd:7;   
  /*! \brief TDM repetition length. Valid value: 1 to  63.*/             
  uint32  repLen:6;                
  /*! \brief TDM repetition Offset: SFN mod Repetition period. Valid value: 0 - Repetition period -1. */
  uint32  repOffset:6;  
  /*! \brief Default DPCH offset value (DOFF) in number of frames. Valid value: 0 to7. */
  uint32  doff:3; 
  /*! \brief TSTD information
   *  0 - TSTD Not enabled
   *  1 - TSTD enabled
   */           
  uint32  tstdInd:1;         
  /*! \brief Timing indication
   *  0 - timing initialised
   *  1 - timing maintained
   */
  uint32  timingInd:1; 
  /*! \brief TPC Step size.
   *  0 - 1 dB
   *  1 - 2 dB
   *  2 - 3 dB
   */      
  uint32  tpcStepSize:2;    
  /*! \brief Actual number of timeslots used in DL */ 
  uint32  numTs:3;           
  /*! \brief Reserved */
  uint32  :3;
// 32 bits

  /*! \brief DPCH time slot info,actual number determined by numTs */
  tdsdllog_dpch_ts_info_type dlDpchTsInfo [TDSDLLOG_MAX_DPCH_TS];

} tdsdllog_dpch_cfg_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSL1LOG_DL_DPCH_CFG_PKT_LEN(num_dpch_ts)  ( \
    FPOS(tdsdllog_dpch_cfg_pkt_type, dlDpchTsInfo)+  \
    ( num_dpch_ts * sizeof(tdsdllog_dpch_ts_info_type)))

#ifdef FEATURE_TDSCDMA_CELL_FACH_MEAS_OCCASION
/*--------------------------------------------------------------------------
        LOG PACKET: TDSCDMA_L1_DL_FMO_CFG_LOG_PACKET
                 (LOG CODE: 0xD014)
                     
This packet holds the FACH measurement occasion information. This record is 
generated whenever UE obtains FACH measurement occasion information in FACH 
state. 
 
This packet can be formed immediately, thus use the log_alloc() and log_commit. 
Do not need to init log header if it is dynamically allocated with log_alloc. 
 --------------------------------------------------------------------------*/

/*! \brief        LOG PACKET: TDSCDMA_L1_DL_FMO_CFG_LOG_PACKET
                  (LOG CODE: 0xD014)
 
This log packet is for internal use only! 
 
This packet holds the FACH measurement occasion information. This record is 
generated whenever UE obtains FACH measurement occasion information in FACH 
state. 
 
This packet can be formed immediately, thus use the log_alloc() and log_commit. 
Do not need to init log header if it is dynamically allocated with log_alloc. 
 
MIN Size: 8 bytes, MAX Size: 8 bytes
The worst case packet size for this log is 8 bytes.  
32+32=64 bits
*/

typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;
  /*! \brief Reserved */
  uint32  :24;
//32 bits

  /*! \brief FACH measurement occasion parameters are valid or not :
   *  1 - Valid
   *  0 - Invalid
   */
  uint32 isInfoValid:1;
  /*! \brief FACH Measurement occasion cycle length coefficient: Integer(1..12) */
  uint32 K_value:4;
  /*! \brief C_RNTI, 0 ~ 65535d */
  uint32 C_RNTI:16;
  /*! \brief Longest FACH TTI in integer (1, 2, 4, 8) of TDS frames   */
  uint32 N_TTI:4;
  /*! \brief Reserved */
  uint32  :7;
//32 bits
} tdsl1log_dl_fmo_config_pkt_type;

#define TDSL1LOG_DL_FMO_CFG_PKT_LEN() \
  (sizeof(tdsl1log_dl_fmo_config_pkt_type))
#endif

/*--------------------------------------------------------------------------
        LOG PACKET: TDSCDMA_L1_DL_BLER_LOG_PACKET
                 (LOG CODE: 0xD015)
                     
This packet holds the DL bler measurement information. This record is 
generated every 2000ms to report CRC bler and counter of special burst. 
 
This packet can be formed immediately, thus use the log_alloc() and log_commit. 
Do not need to init log header if it is dynamically allocated with log_alloc. 
 --------------------------------------------------------------------------*/
 
typedef struct
{
  /*! \brief Common  channel type : Channel type:
   *  0 - SBCH
   *  1 - NBCH
   *  2 - DCH
   *  3 - DSCH
   *  4 - PCH
   *  5 - FACH
   */
  uint32 chType:4;  
  /*! \brief ID of trch table infor */
  uint32 trch_id: 8;
  /*! \brief Number of special burst */
  uint32  numofSB:8;
  /*! \brief Number of punchTTI */
  uint32  numofPunchTTI:8;
  /*! \brief Reserved */
  uint32  :4;
//32bits

  /*! \brief Number of crc received in calculate window */   
  uint32 crc_rece ;
  /*! \brief Number of error crc received in calculate window */  
  uint32 crc_err;
  /*! \brief Number of crc received in calculate window when TTI is desensed */
  uint32  desensed_crc_rece;
  /*! \brief Number of crc error in calculate window when TTI is desensed */
  uint32  desensed_crc_err;  
} tdsdllog_trch_bler_info_type;

typedef struct
{
  /*! \brief Number of trch config info table */
  uint32  numTrch:4;   
  /*! \brief Reserved */
  uint32  :28;
//32 bits
  
  /*! \brief TrCH configutations, actual number determined by numTrch */
  tdsdllog_trch_bler_info_type      dltrch_bler_info[TDSDLLOG_MAX_TRCH];
//32 bits * 3 = 96 bits  
} tdsdllog_cctrch_bler_type;


typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;
    /*! \brief Number of CCTRCH config info table */
  uint32  numCctrch:4;   
  /*! \brief Number of frames about calculate window */
  uint32 calc_win_size: 8;
  /*! \brief Reserved */
  uint32  :12;
//32 bits
  /*! \brief CCTRCH config info table, actual number determined by numCctrch */
  tdsdllog_cctrch_bler_type dlCctrch_bler_info [TDSDLLOG_MAX_CCTRCH];

} tdsl1log_dl_bler_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSCDMA_L1_DL_BLER_LOG_PACKET_LEN(num_cctrch)  ( \
    FPOS(tdsl1log_dl_bler_pkt_type, dlCctrch_bler_info)+  \
    ( num_cctrch * sizeof(tdsdllog_cctrch_bler_type)))

#define TDSDLLOG_PCCPCH_CFG_TYPE 0
#define TDSDLLOG_SCCPCH_CFG_TYPE 1
#define TDSDLLOG_PICH_CFG_TYPE 2

#define TDSDLLOG_DPCH_ADD 0
#define TDSDLLOG_DPCH_CFG 1

/*+ yongh add log package for cfg cmd of FW and decoded result.+*/
/*! \brief Downlink - Max number of cctrch*/
#define TDSDLLOG_FW_MAX_CCTRCH_NUM 3  
#define TDSDLLOG_FW_MAX_DOB_STATE_NETRY_NUM  80 //64 -> 80, buffering for 20ms(16), assume L1M should finish submit logpkt within 20ms
#define TDSDLLOG_FW_CCTRCH_UPDATA_TABLE0_FLAG 1
#define TDSDLLOG_FW_CCTRCH_UPDATA_TABLE1_FLAG 2
#define TDSDLLOG_FW_CCTRCH_UPDATA_TABLE2_FLAG 4

#define TDSDLLOG_FW_MAX_TS_NUM 6

typedef enum
{
  TDSDLLOG_PHYCHAN_DPCH,
  TDSDLLOG_PHYCHAN_PCCPCH,
  TDSDLLOG_PHYCHAN_SCCPCH,
  TDSDLLOG_PHYCHAN_INVALID
}tdsdllog_dl_phychan_enum_type;
/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FW_CCTRCH_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD01D)
                     
This log packet will contain the TD-SCDMA DL cctrch configuration command 
information between SW and FW. 
 
This log record is generated whenever a new downlink cctrch is 
setup or reconfigured

--------------------------------------------------------------------------*/
/*! \brief TDSCDMA L1 DownLink cctrch configuration command information between SW and FW. 
 * 64 bits total
 */
typedef struct
{
  /* word 0 */
  /*! \brief fw cctrch table index, used to indicate which table is configured by SW.*/ 
  uint32 fwCctrchIndex:2;
  /*! \brief DRMB storage direction for DRM. 0: symbols written in descending 
   *   addr order , 1: symbols written in ascending addr order.
   */
  uint32  drmbStorageDir : 1;
  /*! \brief Reserved */
  uint32  :5;
  /*! \brief SW CCTrCH index used in DOB, Valid range : ?? Note that
  this is only used in DOB for SW Table ID purpose. FW does not modify*/
  uint32  swCcTrChIndex : 8;
  /*! \brief DRMB start address for this CCTRCH, in unit of line (128 bytes)*/
  uint32  drmbStartAddr : 16;
  
  /* word 1 */
  /*! \brief Number of TrCH in this CCTrCH .Valid range [1..8]*/
  uint32  numTrCH : 4;
  /*! \brief Reserved */
  uint32  : 4;
  /*! \brief TrCH Info pool base index for this CCTrCH. TrCH pool index for this 
   *   CCTrCH starts from trchInfoBase to trchInfoBase+(numTrCH-1)
   *   Valid range [0..(TFW_TRCH_INFO_POOL_SIZE-numTrCH ].
   */
  uint32  trchInfoBase : 4;
  /*! \brief Reserved */
  uint32  : 4;
  /*! \brief Total number of TF for this CCTrCH. Valid range [1..64] */
  uint32  totalNumTF : 6;
  /*! \brief Reserved */
  uint32  : 2;
  /*! \brief TF Info pool base index for this CCTrCH. TF pool index for this 
   *   CCTrCH starts from tfInfoBase (relative to the beginning of the whole TF
   *   pool table) to tfInfoBase+ totalNumTF-1. Valid range
   *   [0..(TFW_TF_INFO_POOL_SIZE - totalNumTF)].
   */
  uint32  tfInfoBase : 8;

  /* word 2 */
  /*! \brief number of TFC for this CCTrCH. Valid range [1..128] */
  uint32  numTFC : 7;
  /*! \brief Reserved */
  uint32  : 9;
  /*! \brief TFC Info pool base index for this CCTrCH. TFC pool index for this 
   *  CCTrCH starts from tfcInfoBase (relative to the whole TFC pool table)
   *  to tfcInfoBase+ numTFC-1. Valid range [0..(TFW_TFC_INFO_POOL_SIZE - numTFC)]
   */
  uint32  tfcInfoBase : 16;

  /* word 3, TTI Order */
  /*! \brief TrCH number within a CCTrCT according to TTI decending order. 
   *  valid range [0..7] && less than numTrCH 
   */
  uint32  tti1 : 4;
  uint32  tti2 : 4;
  uint32  tti3 : 4;
  uint32  tti4 : 4;
  uint32  tti5 : 4;
  uint32  tti6 : 4;
  uint32  tti7 : 4;
  uint32  tti8 : 4;

}tdsdllog_fw_cctrch_cfg_info_type;

typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;

  /*! \brief Time when the configuration should be applied, in unit of subframe */
  uint32  actionTime : 16;

  /*! \brief Valid values 0: no update, 1: update */
  uint32  numCctrch : 2;
  /*! \brief Reserved */
  uint32  : 6;  

  /*! \brief CCTrCH table config for demback. For DPCH, up to 3 CCTrCH. The array 
   *   index shall match the cctrchid used in DL DPCH channel config. For SCCPCH, array
   *   index 0 shall be used always since there is only 1 CCTrCH 
   */
  tdsdllog_fw_cctrch_cfg_info_type  ccTrCh[TDSDLLOG_FW_MAX_CCTRCH_NUM];

}tdsdllog_fw_cctrch_cfg_cmd_pkt_type;

#define TDSL1LOG_FW_CCTRCH_CFG_CMD_PKT_LEN(num_cctrch) \
  FPOS(tdsdllog_fw_cctrch_cfg_cmd_pkt_type, ccTrCh)+  \
  (num_cctrch*sizeof(tdsdllog_fw_cctrch_cfg_info_type))
  
/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FW_MIDAMBLE_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD017)
                     
This log packet will contain the TD-SCDMA DL midamble configuration command 
information between SW and FW for each time slot allocated by network. 
 
This log record is generated whenever a new dedicated physical channel is 
setup or reconfigured

--------------------------------------------------------------------------*/
/*! \brief downlink midamble configuration information between SW and FW 
 *  for each time slot allocated by network.
 */ 
typedef struct
{
  /*! \brief Midamble allocation mode. When this slot is not assigned, set all
   *  fields to 0 
   *  Valid value : 
   *    TFW_MIDAMBLE_ALLOC_DEFAULT
   *    TFW_MIDAMBLE_ALLOC_COMMON
   *    TFW_MIDAMBLE_ALLOC_UE_SPECIFIC
   */
  uint16 allocMode : 2;
  /*! \brief Midamble Config Number. Valid : [2,4,6,8,10,12,14,16]*/
  uint16 configK : 5;
  /*! \brief Reserved */
  uint16 : 1;
  /*! \brief Midamble shift for UE Specific allocation. Valid : 0..(configK-1)*/
  uint16 shift : 8;
} tdsdllog_fw_midamble_cfg_info_type ;

typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;   
  /*! \brief Reserved */
  uint32  :24;                

  /*! \brief Midamble configuration. Up to 6 DL slots. When a slot is not used,
   *   the corresponding configuration shall be set to 0 to indicate "un-used".
   */
  tdsdllog_fw_midamble_cfg_info_type   midambleCfg[6];
}tdsdllog_fw_midamble_cfg_cmd_pkt_type;

#define TDSL1LOG_FW_MIDAMBLE_CFG_CMD_PKT_LEN() \
  (sizeof(tdsdllog_fw_midamble_cfg_cmd_pkt_type))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FW_PICH_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD018)
                     
This log packet will contain the TD-SCDMA DL PICH physical channel configuration 
information between SW and FW. 
 
This log record is generated whenever a new dedicated physical channel is 
setup or reconfigured

--------------------------------------------------------------------------*/
/*! \brief downlink PICH physical channel configuration information between SW and FW.*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;   
  /*! \brief  Action Type.  
   *  PICH decode is done only once per cmd. FW will report the result regardless 
   *  of detection of indicator. When command with enable is sent after 1st enable, 
   *  SW still needs to send the whole set of parameters though they are the same as
   *  before. SW needs to send disable cmd when PICH is to be torn down after paging 
   *  indicator is detected 
   *  Valid : 
   *    \li TFW_CMD_ACTION_DISABLE
   *    \li TFW_CMD_ACTION_ENABLE
   */
  uint32  actionType : 8;
  /*! \brief Time slot used by PICH. Valid values (0,2,3,4,5,6) */
  uint32  timeSlot : 8;
  /*! \brief Length (in bit) of one paging indicator, valid values 4, 8, 16 */
  uint32  pagingIndLen : 8;

  /* Word 1 */
  /*! \brief starting bit lcoation within subframe for paging indicator , 0..87 */
  uint32  qValue : 8;
  /*! \brief Reserved */
  uint32  :8;                
  /*! \brief Time (in subframe) when PICH indicator should be searched */
  uint32  actionTime:16;         

  /* Word 2 */
  /*! \brief SF16 code channels for PICH. Bit 0..15 for code 1..16*/
  uint32  phyChanBitMap:16;
  /*! \brief Reserved */
  uint32  :16; 
}tdsdllog_fw_pich_cfg_cmd_pkt_type;

#define TDSL1LOG_FW_PICH_CFG_CMD_PKT_LEN() \
  (sizeof(tdsdllog_fw_pich_cfg_cmd_pkt_type))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FW_PCCPCH_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD019)
                     
This log packet will contain the TD-SCDMA DL PCCPCH physical channel 
configuration command information between SW and FW. 
 
This log record is generated whenever a new PCCPCH physical channel is 
setup or drop.

--------------------------------------------------------------------------*/
/*! \brief DL PCCPCH physical channel configuration information between SW and FW*/
typedef struct 
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8; 
  /*! \brief Action Type. 
   *  Valid : 
   *   \li TFW_CMD_ACTION_DISABLE
   *   \li TFW_CMD_ACTION_ENABLE
   *   \li TFW_CMD_ACTION_RECONFIG
   */
  uint32  actionType : 2;
  /*! \brief TSTD Enable. 
   *  Valid : 
   *   \li TFW_CMD_TSTD_DISABLE
   *   \li TFW_CMD_TSTD_ENABLE
   */
  uint32  tstdEnable : 1;
  /*! \brief PCCPCH operation mode. 
   *  valid range. [0..2]
   *   \li 0: normal
   *   \li 1: Phase based TTI detection (not compatible with reconfig)
   *   \li 2: Blind TTI detection (not compatible with reconfig)
   */
  uint32  ttiDetectMode : 2;
  /*! \brief Reserved */
  uint32  :3;
  /*! \brief SW internal CCTrCH index used in DOB, Valid range : ??. Note that
   *   this is only used in DOB for SW table ID purpose. FW does not modify
   */
  uint32  swCcTrChIndex : 8;
  /*! \brief Reserved */
  uint32  :8;

  /* Word 1 */
  /*! \brief Time (in subframe) when the configuration should be applied. It is
   *   assumed that TTI will start at the subframe of action time in normal mode. 
   */
  uint32  actionTime : 16; 
  /*! \brief Channel present repetition period. Valid only when ttiDetectMode 
   *   above is "normal". Valid values: 1, 2,4,8,16,32,64 frames. 1 is used to indicate
   *   all frames are on 
   */
  uint32  chanOnRepPeriod : 8;
  /*! \brief DRMB storage direction for DRM. 0: symbols written in descending 
   *   addr order , 1: symbols written in ascending addr order. valid only if
   *   ttiDetectMode is normal mode. 
   */
  uint32  drmbStorageDir : 1;
  /*! \brief Reserved */
  uint32  :7;

  /* Word 2 */
  /*! \brief DRMB start address for PCCPCH, in unit of line (128 bytes). Valid
   *   only if ttiDetectMode is normal mode.
   */
  uint32  drmbStartAddr : 16;
  /*! \brief Deep search freq bin step size
   *  @verbatim 
   *     Resolution: 1 Hz 
   *     Recommeded systems configuration: 400
   *  @endverbatim 
   */ 
  uint32 deepSrchFreqStepSize:8;
  /*! \brief Number of deep search sided freq bins
   *  @verbatim 
   *     Total freq bins = 2*deepSrchNumSidedFreqBins + 1
   *     Recommeded systems configuration for init acq: 5
   *  @endverbatim 
   */ 
  uint32 deepSrchNumSidedFreqBins:8;

  /* Word 3 and 4 */
  /*! \brief Channel Present on-off bit mask to be applied at action time.  
   *   Bit[0..(chanOnRepPeriod-1)] with each bit indicates if Physical channel is present in
   *   the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
   *   shall be set to 0 when chanOnRepPeriod is less than 64.
   */
  uint64  chanPresentMask;
} tdsdllog_fw_pccpch_cfg_cmd_pkt_type ;

#define TDSL1LOG_FW_PCCPCH_CFG_CMD_PKT_LEN() \
  (sizeof(tdsdllog_fw_pccpch_cfg_cmd_pkt_type))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FW_SCCPCH_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD01A)
                     
This log packet will contain the TD-SCDMA DL SCCPCH physical channel 
configuration command information between SW and FW. 
 
This log record is generated whenever a new SCCPCH physical channel is 
setup or drop.

--------------------------------------------------------------------------*/
typedef struct
{
  /*! \brief  Slot format for this code channel. 
   * Valid range [0..25]. 
   *     0: invalid format, unused entry should be initialized with 0
   *     [1..25]: 1st to 25th entry in slot format table  
   */ 
  uint8 slotFormatIdx : 5;
  /*! \brief CCTrCH that this code channel is assigned to, valid : SCCPCH, DPCH 
   *  for DPCH, valid [0..2], for SCCPCH valid [0], Note the cctrch with index
   *  0 is mapped to use tfw_ccTrCh_table_t index 0, similarly 1 is
   *  mapped to use with tfw_ccTrCh_table_t index 1, 2 is mapped to use with
   *  tfw_ccTrCh_table_t index 2.
   */
  uint8 ccTrchIndex : 3;
} tdsdllog_phychan_format_info_struct_type;

/** DL Per Slot Physical Channel Info. */
typedef struct
{
  /*Word 0~3*/
  /*! \brief  Slot format for each of the 16 code channel in this time slot.
   *  Entry 0 corresponds to the lowest code channel in the slot 
   */
  tdsdllog_phychan_format_info_struct_type  phyInfo[16];

  /*word 4*/
  /*! \brief  Total number of code channels that carry TPC.
   * = 1+ Ntpc if a Time slot carries TPC, else = 0. Valid [0..16] 
   */
  uint32  numChanWithTPC:8;
  /*! \brief Reserved*/
  uint32  :24;  
} tdsdllog_slotformat_info_struct_type ;

/*! \brief DL SCCPCH physical channel configuration information between SW and FW.*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8; 
  /*! \brief PCH_FET enable: 1; disable: 0*/
  uint32 fetEn: 1; 
  /*! \brief Offset in 5ms from 10 ms mark of a TTI that the 1st FET decode shall take place.
     *   Only valid if fetEn is set.
     *   Valid value, for 20 ms TTI [0,1], 0: 1st decode takes place at 10 ms into a TTI,
     *                             1: 1st decode takes place at 15 ms into a TTI,
     *         for 40 ms TTI [0;;5] 0: 1st decode takes place at 10 ms into a TTI,
     *                             1: 1st decode takes place at 15 ms into a TTI,
     *                             2: 1st decode takes place at 20 ms into a TTI,
     *                             3: 1st decode takes place at 25 ms into a TTI,
     *                             4: 1st decode takes place at 30 ms into a TTI,
     *                             5: 1st decode takes place at 35 ms into a TTI */
  uint32  fetDecOffset : 3;
   /*! \brief Reserved*/
  uint32  :20;    
   
  /*Word 1*/
  /*! \brief Action Type. 
   *   Valid : 
   *     TFW_CMD_ACTION_DISABLE, 
   *     TFW_CMD_ACTION_ENABLE,
   *     TFW_CMD_ACTION_RECONFIG 
   */
  uint32  actionType : 2;
  /*! \brief 2nd Interleave mode
   *   valid:  
   *     0: frame based, 
   *     1: time slot based 
   */
  uint32  secondIntlvMode : 1;
  /*! \brief CCTrCH Index associated with this SCCPCH, valid values: [0].
   *   The same CCTrCH is also mapped to use with tfw_ccTrCh_table_t index 0.
   */
  uint32  cctrchIndex : 3;

  /*! \brief insert Dob For Zero TB, pch is one, other is 0 */
  uint32 insertDobForZeroTB : 1;
  
  /*! \brief Reserved*/
  uint32  :1;
  /*! \brief Time slot used by SCCPCH. Valid values (0,2,3,4,5,6) */
  uint32  timeSlot : 8;
  /*! \brief Channel Present repetition period. Valid regardless of TDM mode. 
   *   Valid values: 
   *     1,2,4,8,16,32,64 frames. 
   *     1 is used to indicate all frames are on.
   */
  uint32  chanOnRepPeriod : 8;
  /*! \brief SFN number within TTI at the action time when config is applied  Valid :0..255.*/
  uint32  ttiSfn : 8;
 
  /*Word 2*/
  /*! \brief Time (in subframe) when the configuration should be applied */
  uint32  actionTime : 16;
  /*! \brief how many channel code is used.*/
  uint32 phyChanBitMap:16;
  
  /* Word 3 & 4 */
  /*! \brief Channel Present on-off bit mask to be applied at action time.  
   *   Bit[0..(chanOnRepPeriod-1)] with each bit indicates if Physical channel is present in
   *   the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
   *   should be set to 0  when chanOnRepPeriod is less than 64. But they will be ignored by FW.
   */
  uint64  chanPresentMask;

  /*Word 4~10*/
  /*! \brief slot information*/
  tdsdllog_slotformat_info_struct_type  slotInfo;
} tdsdllog_fw_sccpch_cfg_cmd_pkt_type ;

#define TDSL1LOG_FW_SCCPCH_CFG_CMD_PKT_LEN() \
  (sizeof(tdsdllog_fw_sccpch_cfg_cmd_pkt_type))

/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FW_DPCH_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD01B)
                     
This log packet will contain the TD-SCDMA DL DPCH physical channel 
configuration command information between SW and FW. 
 
This log record is generated whenever a new DPCH physical channel is 
setup , drop or reconfigured

--------------------------------------------------------------------------*/
/*! \brief DL DPCH physical channel configuration command
 *   information between SW and FW. 
 */
typedef struct
{
  /* Word 0 */
  /*! \brief Action Type. 
   *   Valid value : 
   *     TFW_CMD_ACTION_DISABLE, 
   *     TFW_CMD_ACTION_ENABLE, 
   *     TFW_CMD_ACTION_RECONFIG 
   */
  uint32  actionType : 2;
  /*! \brief CCTrCH Index associated with this DPCH, valid values: [0..2] 
   *   Note that dpch with cctrchIndex 0 is mapped to use tfw_ccTrCh_table_t index 0, 
   *   similarly, 1 is mapped to use with tfw_ccTrCh_table_t index 1 and 2 is 
   *   mapped to use with tfw_ccTrCh_table_t index 2.
   */
  uint32  cctrchIndex : 3;
  /*! \brief 2nd Interleave mode , valid:  0: frame based, 1: time slot based */
  uint32  secondIntlvMode : 1;
  /*! \brief Reserved*/
  uint32 : 2; /* used to be TDM mode bit. No longer used in FW */
  /*! \brief CFN number at the action time when config is applied  Valid :0..255*/
  uint32  cfn : 8;
  /*! \brief Channel Present repetition period. Valid regardless of TDM mode. 
   *   Valid values: 
   *     1, 2,4,8,16,32,64 frames. 
   *     1 is used when all frames are on.
   */
  uint32  chanOnRepPeriod : 8;
  /*! \brief Reserved */
  uint32  : 8;  

   /* Word 3 to 5 */
  /*! \brief 16 bit SF16 code channels mask per DL slot. */
  uint16  phyChanBitMap[6];

  /* Word 1 & 2 */
  /*! \brief Channel Present on-off bit mask to be applied at action time.  
   *   Bit[0..(chanOnRepPeriod-1)] with each bit indicates if Physical channel is present in
   *   the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
   *   should be set to 0  when chanOnRepPeriod is less than 64. But they will be ignored by FW.
   */
  uint64  chanPresentMask;
} tdsdllog_fw_dpch_cfg_info_struct_type ;


typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8; 
  /*! \brief Time when the configuration should be applied, in unit of subframe */
  uint32  actionTime : 16;
  /*! \brief Valid : TFW_CMD_TSTD_DISABLE & TFW_CMD_TSTD_ENABLE */
  uint32  tstdEnable : 1;
  /*! \brief Valid : lva algorithm used for this cctrch */
  uint32  lvaEnable  : 1;
  /*! \brief up to 3 DPCH. Valid values [1,2,3]*/
  uint32  numDPCH : 2;
  /*! \brief Reserved */
  uint32  : 4;

  /* Word 32 to 50 */
  /*! \brief dpch detail information */  
  tdsdllog_fw_dpch_cfg_info_struct_type  dpch[TDSDLLOG_FW_MAX_CCTRCH_NUM];

  /*1~31*/
  /*! \brief slot information*/
  tdsdllog_slotformat_info_struct_type slotInfo[6]; 
} tdsdllog_fw_dpch_cfg_cmd_pkt_type ;

#define TDSL1LOG_FW_DPCH_CFG_CMD_PKT_LEN(num_dpch) \
  (FPOS(tdsdllog_fw_dpch_cfg_cmd_pkt_type, dpch)+  \
   num_dpch*sizeof(tdsdllog_fw_dpch_cfg_info_struct_type)+ \
   6*sizeof(tdsdllog_slotformat_info_struct_type))
/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FW_RX_TIME_SYNC_CFG_CMD_LOG_PACKET
                 (LOG CODE: 0xD01C)
                     
This log packet will contain the TD-SCDMA DL RX time sync configuration 
command information between SW and FW. 
 
This log record is generated whenever L1SW gets system time and finish slamming 
operation, L1SW notify RX timing information to FW by this command.

--------------------------------------------------------------------------*/
/*! \brief DL RX time sync configuration command information between SW and FW.*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8; 
  /*! \brief Reserved */
  uint32  : 24;
  
  /* Word 1 */
  /*! \brief Offset between RX Time counter and Wall Time counter 
   *   at begin of TTI, relevent only for blind & S1/S2 PCCPCH.
   */
  uint32  offsetCnt;

  /* Word 2 */
  /*! \brief Indication if freqOffset is valid. Usually set after PCCPCH 
   *   blind detection. = 1 if freqOffset is valid. = 0, if invalid.
   */
  uint32  validOffset:16;
  /*! \brief Winning hypotheses frequency offset in 1 Hz to be applied to
   *   working frequency in serving cell. Valid only if validOffset== 1.
   */
  uint32   freqOffset:16;
} tdsdllog_fw_rx_time_sync_cfg_cmd_pkt_type ;

#define TDSL1LOG_FW_RX_TIME_SYNC_CFG_CMD_PKT_LEN() \
  (sizeof(tdsdllog_fw_rx_time_sync_cfg_cmd_pkt_type))


/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FW_PICH_DECODE_RESULT_LOG_PACKET 
                 (LOG CODE: 0xD01E)
                     
This log packet will contain the TD-SCDMA DL PICH physical channel decoded  
result information received from FW. 
 
This log record is generated whenever PICH decode done ISR is triggerred.
--------------------------------------------------------------------------*/
/*! \brief DL PICH physical channel decoded result information received from FW.*/
typedef struct
{
  /* Word 0 */
  /*! \brief Log header */
  log_hdr_type header;
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;
  /*! \brief qvalue that is sent to FW for PICH decode.*/
  uint32  qvalue:8;
  /*! \brief subframe number */
  uint32  subSfn:13;
  /*! \brief Valid : pich decode result. */
  uint32  piVal:1;
  /*! \brief Indication if PICH goes through decode. 1: PICH is decode. 0: PICH is not decoded */
  uint32 decoded:1;
  /*! \brief Reserved */
  uint32  : 1;
  uint32 actionTime:16;
  /*! \brief Reserved */
  uint32 : 16;
  /*metric value*/
  int32 metric;   //use a int32 value, no need to reserver other bit for PICH metric value

} tdsdllog_pich_decode_result_cmd_pkt_type;

#define TDSL1LOG_PICH_DECODE_RESULT_CMD_PKT_LEN() \
  (sizeof(tdsdllog_pich_decode_result_cmd_pkt_type))
#if 0
/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FPACH_DECODING_RESULT_LOG_PACKET   
                 (LOG CODE: 0xD013)
                     
This log packet will contain the TD-SCDMA DL FPACH physical channel decoded 
result information received from FW. 
 
This log record is generated whenever FPACH deocded done ISR is triggerred.
--------------------------------------------------------------------------*/
/*! \brief DL FPACH physical channel decoded result information received from FW.*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;
  /*! \brief FPACH Status. Valid [TFW_UL_SYNC_NO_ACK, TFW_UL_SYNC_ACK, 
  TFW_UL_SYNC_ABORT] */
  uint32   status:8;
  /*! \brief Received Signature Reference Number. Valid only if status is ACK*/
  uint32   sigRefNo:8;
  /** Sequence number for this random access procedure. Valid range [0..255] */
  uint32   txSeqNum:8;

  /* Word 1 */
  /** UL Sync Code. Valid range [0..255] */
  uint32   ulSyncCode:8;
  /** Sub-frame number to start transmission on UpPCH. Valid [0..8191] */
  uint32  ulSyncTxSubFrm:16;
  /*! \brief Received Relative Sub-Frame Number. Valid only if status is ACK */
  uint32   relSubFrm:8;

  /* Word 2 */
  /*! \brief Received Transmit Power Level Command for RACH message. 
   *   Valid only if status is ACK
   */
  uint32   prxPRACH:8;
  /*! \brief Received starting position of the UpPCH. Valid [0..8191] in 1/8 
   *   chip. Valid only if status is ACK.
   */
  uint32  upPchPos:16;
  /*! \brief Reserved */
  uint32  : 8;  
}tdsdllog_fpach_decode_result_cmd_pkt_type;

#define TDSL1LOG_FPACH_DECODE_RESULT_CMD_PKT_LEN() \
  (sizeof(tdsdllog_fpach_decode_result_cmd_pkt_type))
#endif
/*--------------------------------------------------------------------------
                 LOG PACKET: TDSCDMA_L1_DL_FW_DPCH_PCCPCH_SCCPCH_DECODE_RESULT_LOG_PACKET    
                 (LOG CODE: 0xD01F)
                     
This log packet will contain the TD-SCDMA DL DPCH, SCCPCH or PCCPCH physical channel 
decoded result information received from FW. 
 
This log record is generated whenever decoded done ISR is triggerred.
--------------------------------------------------------------------------*/
typedef struct
{
  /* Word 0*/
  /*! \brief dob status fifo read index , Valid [0..127] */
  uint32 FifoRdIdx:7;
  /*! \brief dob status fifo write index , Valid [0..127] */    
  uint32 FifoWrIdx:7;
  /*! \brief End of TTI CFN for a TrCH, Valid [0..255] */
  uint32  cfn : 8; 
  /*! \brief channel type, 
   *  0 -  DPCH
   *  1 -  PCCPCH
   *  2 -  SCCPCH; 
   */
  uint32  channelType:2;  
  /*! \brief SW CCTrCH index this TrCH belongs to . Valid range ??*/                         
  uint32  cctrchIndex : 3;  
  /*! \brief Arriving index of TrCH in CCTrCH, Valid [0..7]  */           
  uint32 trChIdx:3;
  /*! \brief 
   *  0 -  FALSE, 
   *  1 -  TRUE 
   */        
  uint32 tfciDecPass : 1; 
  /*! \brief Special burst detection, 
   *  0 -  FALSE 
   *  1 -  TRUE 
   */              
  uint32 sbDetect : 1;


  /* Word 1*/  
   /*! \brief CRC error bits of each TB. MSB is last block received in this TBset */
  uint32 crcBits;  

  /* Word 2*/
   /*! \brief indicate don't check CRC pass/fail and get data for block of this TrCh
   *  0 -  FALSE 
   *  1 -  TRUE 
   */
  uint32 overrideCrc:1; 
  /*! \brief TFI error for a Trch, To be ignored in pccpch or if sbDetect is set 
   * 0 -  no Error 
   * 1 -  error
   */              
  uint32 tfiError : 1;

  /*! \ brief Partial TTI has been de-sensed. 0: No de-sensed 1: Partial de-sensed */
  uint32 deSensed: 1; 
  /*! \brief Partial TTI Punctured Indication From Slot Suspenstion. 0: No partial puncture 1: Partial puncture>*/
  uint32 puncturedInd: 1;     
  
  /*! \brief PCH_FET enable: 1; disable: 0*/
  uint32 fetInd: 1;

  
  /*! \brief Lva indication. 1: LVA can be ued for this TRCH. 0: LVA is not used */
  uint32  lva_ind: 1;
  /*! \brief Indication: Lva decode done and write back to Dob*/
  uint32  lva_wbk_ind: 1; 

  /*! \brief Reserved */
  uint32  : 1;
  /*! \brief TF index for this TrCH within a CCTrCH. TF index does not 
   *   reset to 0 when going across different TrCHs witin a CCTrCH. Valid [0..63].
   */    
  uint32  tfIdx : 8;  
    /*! \brief the number of tb.*/  
  uint32 numTB:8;
  /*! \brief coding type of transport chanel */
  uint32 codingType:2;
  /*! \brief zero state bit.*/
  uint32 zsb:1;    
  /*! \brief Indication of last TrCH decode in a CCTrCH to use this config ID after a
   *   reconfiguration or disable. 1 : yes, 0: no. When there is no reconfig or enable cmd
   *   it shall be 0. Note that if reconfig or disable cmd comes before maximum TTI
   *   of TrcH within a CCTrCH, there maybe no TrcH decode.
   */
  uint32  lastTrChInd : 1; 
    /*! \brief Reserved */
  uint32  : 4;    

  /* Word 3*/   
   /*! \brief TB size in bit, */
  uint32 trBlkSize : 16;   
  /*! \brief TrCH start addr in DOB */
  uint32 trChStartAddr : 16;      

  /* Word 4*/
  /*! \brief The extra information word. Depending on channels, the meaning is
   *   different. For PCCPCCH, it will give STMR wall timer count at TTI. Not used
   *   for other channels.
   */
  uint32  stmrWallTimeCnt;

  /* Word 5*/  
  /*! \brief symbol error count for TB set.*/  
  uint32 symErrCount:16; 
  /*! \brief quality bits for TB set.*/
  uint32 qualityBits:16; 

  /* Word 6*/
  /*! \brief energy metric of the TB set.*/
  uint32 energyMetric:16; 
  /*! \brief Reserved */
  uint32  : 16;


}tdsdllog_dpch_pccpch_sccpch_decode_result_dob_struct_type;


/*! \brief DL DPCH, SCCPCH or PCCPCH physical channel decoded result information received from FW.*/
typedef struct
{
  /*! \brief Log header */
  log_hdr_type header;

  /* Word 0 */
  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;

  /*! \brief Number of dob status info table */
  uint32 NumDobStatus: 8;

  /*! \brief Reserved */
  uint32  : 16;    

  /* size 24 bytes */
  /*! \brief CCTRCH config info table, actual number determined by numCctrch */
  tdsdllog_dpch_pccpch_sccpch_decode_result_dob_struct_type dob_status[TDSDLLOG_FW_MAX_DOB_STATE_NETRY_NUM];


}tdsdllog_dpch_pccpch_sccpch_decode_result_cmd_pkt_type;

#define TDSL1LOG_DPCH_PCCPCH_SCCPCH_DECODE_RESULT_CMD_PKT_LEN(NumDobStatus) \
  FPOS(tdsdllog_dpch_pccpch_sccpch_decode_result_cmd_pkt_type, dob_status)+  \
  (NumDobStatus*sizeof(tdsdllog_dpch_pccpch_sccpch_decode_result_dob_struct_type))


/*- yongh add log package for cfg cmd of FW and decoded result.-*/


//OLPC log packet structure definition added by meilinw
typedef struct{
  /*! \brief TrCH Id from higher layer,*/
  uint32 trch_id : 3;
  /*! \brief targett BLER value from RRC, Enum(-6.3:0.1:0), exponent of 10, here enum 0 ~ 6.3; enum63~0.0*/
  uint32 bler_target : 6;
  /*! \brief tti value Enum(5, 10, 20, 40, 80)*/
  uint32 tti_val : 3;
  /*! \brief max number of TB among all the TF,  Integer(0,1,2,бн,32) Max number of TrBlks */
  uint32 max_num_tblk : 6;
  /*! \brief Researve,*/
  uint32 : 14;
  
  /*! \brief Ceiling of SIR target*/
  int32 sir_target_ceiling;
  /*! \brief Floor of SIR target*/
  int32 sir_target_floor;
}tdsdllog_olpc_trch_config_type;

/*! \brief        LOG PACKET: TDSDLLOG_OLPC_CONFIG_LOG_PKT

                 (LOG CODE: 0xD060)

 

This log packet is for internal use only! 

 

This log packet will contain the TD-SCDMA UE side OLPC (outer loop power control). 
configuration message for DL DCH.
 

MIN Size: 16 bytes, MAX Size: 112 bytes

*/


typedef struct{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;
    /*! \brief cctrch index, Integer(0,..,3) (default only 1 CCTrCH, i.e., CCTrCH0)*/
  uint32  cctrch_idx:2;    
  /*! \brief Number Transport channel */
  uint32 num_of_trch: 3;
  /*! \brief Reserved */
  uint32  :19;
//32 bits
  /*! \brief olpc transport channel configuartion information */
  tdsdllog_olpc_trch_config_type  olpc_trch_config_info[TDSDLLOG_MAX_TRCH];
}tdsdllog_olpc_config_log_pkt_type;


#define TDSDLLOG_OLPC_CONFIG_PKT_LEN(num_trch) ( \
    FPOS(tdsdllog_olpc_config_log_pkt_type, olpc_trch_config_info)+  \
    ( num_trch * sizeof(tdsdllog_olpc_trch_config_type)))


typedef struct{
  /*! \brief IIR filter BLER Record 0-40% IIR-filtered BLER (equivalent to step size selected) */
  uint32 iir_filter_bler : 6;
  /*! \brief Number of TrBlks transmitted integer(0,1,2,бн,32) */
  uint32 num_trblk_transmitted : 6;
  /*! \brief Number of TrBlks CRC failure integer(0,1,2,бн,32) */
  uint32 num_crc_failed : 6;
  /*! \brief Researve,*/
  uint32 : 14;
  
  /*! \brief updated SIR target dB*/
  int32 updated_sir_target;
}tdsdllog_olpc_trch_history_info_type;

/*! \brief        LOG PACKET: TDSDLLOG_OLPC_UPDATE_LOG_PKT

                 (LOG CODE: 0xD061)

 

This log packet is for internal use only! 

 

This log packet will contain the TD-SCDMA UE side OLPC (outer loop power control). 
 results for DL DCH updated each smallest TTI (among all the transport channels).
 

MIN Size: 20 bytes, MAX Size: 84 bytes

*/


typedef struct{
  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;
  /*! \brief cctrch index, Integer(0,..,3) (default only 1 CCTrCH, i.e., CCTrCH0)*/
  uint32  cctrch_idx:2;    
  /*! \brief current system frame number 0,бн4095*/
  uint32 current_sfn : 12;
  /*! \brief current Subframe number (0,1)*/
  uint32 current_subfn : 1;
  /*! \brief dormant or not(0, 1)*/
  uint32 dormant_indicator : 1;
  /*! \brief current olpc mode :0 иC Init 1 иC Acquisition 2 - Tracking*/
  uint32 current_olpc_mode : 2;
  /*! \brief Number of Transport channel */
  uint32 num_of_trch: 3;
  /*! \brief Reserved */
  uint32  : 3;
  /*! \brief current_sir_target ((1<<25)x dB) */
  int32 current_sir_target;

  /*! \brief olpc transport channel configuartion information */
  tdsdllog_olpc_trch_history_info_type  olpc_trch_history_record[TDSDLLOG_MAX_TRCH];
}tdsdllog_olpc_updated_log_pkt_type;

#define TDSDLLOG_OLPC_UPDATE_PKT_LEN(num_trch) ( \
    FPOS(tdsdllog_olpc_updated_log_pkt_type, olpc_trch_history_record)+  \
    ( num_trch * sizeof(tdsdllog_olpc_trch_history_info_type)))	
//end of definition of OLPC log packet by meilinw
/*--------------------------------------------------------------------------
        LOG PACKET: TDSCDMA_L1_DL_SYNC_DETEC_LOG_PACKET
                 (LOG CODE: 0xD062)
                     
This packet holds the DL sync detection information. This record is 
generated every 10ms to report sync detection process. 
 
This log record is generated whenever a new dedicated physical channel is 
setup or reconfigured

Size: 20 bytes
----------------------------------------------------------------------------*/
typedef struct {

  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;   
  /*! \brief Indication for sub frame number; range is 0 to 8192 */
  uint32  SubSfn:16; 
  /*! \brief Indication for TDM.
   *  0 -  OFF
   *  1 -  ON
   */      
  uint32  TdmInd:1;   
  /*! \brief Indication for  Special Burst.
   *  0 -  Data Burst
   *  1 -  Special Burst
   */  
  uint32  SbInd:1;
  /*! \brief Indication if DPCH established.
   *  0 -  FALSE
   *  1 -  TRUE
   */  
  uint32  DpchEstInd:1;
  /*! \brief Indication if sync detection phase 1.
   *  0 -  PHASE TWO
   *  1 -  PHASE ONE
   */  
  uint32  PhaseOneInd:1;
  /*! \brief Indication of current frame sync status.
   *  0 -  LL_SYNC_IN_SYNC_IND
   *  1 -  LL_SYNC_OUT_SYNC_IND
   *  2 -  LL_SYNC_NO_IND,
   *  3 -  LL_SYNC_ERR_IND
   */ 
  uint32 CurrentFrameStatus:2;

  /*! \brief Researve,*/
  uint32 : 2;
//32 bits

  /*! \brief the burst energy of current frame */
  int32 BurstEngCur;
//32 bits

  /*! \brief the DPCH average energy in 160ms window*/
  int32 DpchAveEngVal;
//32 bits


  /*! \brief Indication if CRC part in sync.
   *  0 -  FALSE
   *  1 -  TRUE
   */  
  uint32  CrcInSyncInd:1;
  /*! \brief Indication if crc part out of sync.
   *  0 -  FALSE
   *  1 -  TRUE
   */  
  uint32  CrcOutOfSyncInd:1;
  /*! \brief Indication if dpch part in sync.
   *  0 -  FALSE
   *  1 -  TRUE
   */  
  uint32  DpchInSyncInd:1;
  /*! \brief Indication if dpch part out of sync.
   *  0 -  FALSE
   *  1 -  TRUE
   */  
  uint32  DpchOutOfSyncInd:1;
  /*! \brief Indication if SB part in sync.
   *  0 -  FALSE
   *  1 -  TRUE
   */  
  uint32  SbInSyncInd:1;
  /*! \brief Indication if SB part out of sync.
   *  0 -  FALSE
   *  1 -  TRUE
   */  
  uint32  SbOutOfSyncInd:1;
  /*! \brief Indication Of the number that consecutive in sync indications have been received,max value is 1000(N312/N315) */
  uint32  InSyncCnt:10;  
  /*! \brief Indication Of the number that consecutive low level out of sync indications have been received,max value is 200(N313) */
  uint32  OutSyncCnt:8; 
  /*! \brief Researve,*/
  uint32 : 8;
//32 bits 

  /*! \brief Indication Of the number that T312 frame number, max value 15*100*/
  uint32  T312Cnt:11;  
  /*! \brief Indication Of the number that T313 frame number, max value 15*100*/
  uint32  T313Cnt:11; 
  /*! \brief Researve,*/
  uint32 : 10;

//32 bits
}tdsl1log_dl_sync_det_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSCDMA_L1_DL_SYNC_DETEC_LOG_PACKET_LEN() \
    (sizeof(tdsl1log_dl_sync_det_pkt_type))

/*--------------------------------------------------------------------------
        LOG PACKET: TDSCDMA_L1_INT_PCCPCH_LOG_PACKET
                 (LOG CODE: 0xD063)
                     
This packet holds the step3 for acq process. This record is 
generated every time for a ACQ process to report step3 result. 

Size: 28 bytes
----------------------------------------------------------------------------*/
typedef struct {

  /*! \brief Log header */
  log_hdr_type header;

  /*! \brief Log packet version; range is 0 to 255 */
  uint32  version:8;   
  /*! \brief UTRA Absolute Radio Frequency Number */          
  uint32  uarfcn:16; 
  /*! \brief Cell Parameter ID (0..127)*/            
  uint32  cpi:8; 
//32 bits 

  /*! \brief internal pccpch detection result : 
   *  0 - FAILURE
   *  1 - SUCCESS
   */
  uint32  status:1;
  /*! \brief PCCPCH dection method :
   *  0 - No TTI Detection needed
   *  1 - Shallow
   *  2 - Deep
   *  3 - Not used
   */
  uint32  pccpchDetetionType:2;  
  /*! \brief Indication for block num of deep search; range is 0 to 10. */
  uint32  succeedBlockNum:4;
  /*! \brief Researve,*/
  uint32 : 25;
//32 bits 

  /*! \brief Indication for freq offset in internal pccpch detection */
  uint32  freqError:16;
  /*! \brief Indication for sub frame number; range is 0 to 4096 */
  uint32  succeedSfn:16; 
//32 bits 

  /*! \brief Indication for energy of succeed sfn */
  uint32  energy ;
//32 bits

  /*! \brief Indication for wall time sub frame of succeed sfn get from FW */
  uint32  wtSubFrame:16 ;
  /*! \brief Indication for wall time CX8 of succeed sfn get from FW */
  uint32  wtCx8:16 ;
//32 bits

  /*! \brief Indication for sub frame which slam to rx time */
  uint32  slamRTSubFrame: 16 ;
  /*! \brief Indication for cx8 which slam to rx time */
  uint32  slamRTCx8: 16 ;
//32 bits

  /*! \brief Indication for sub frame which slam to rx time */
  uint32  slamWTSubFrame: 16 ;
  /*! \brief Indication for cx8 which slam to rx time */
  uint32  slamWTCx8: 16 ;
//32 bits 
}tdsl1log_dl_int_pccpch_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSCDMA_L1_INT_PCCPCH_LOG_PACKET_LEN() \
    (sizeof(tdsl1log_dl_int_pccpch_pkt_type))

#endif /* TDSDLLOG_H */

