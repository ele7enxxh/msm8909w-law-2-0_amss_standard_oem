#ifndef TDSHSLOG_H
#define TDSHSLOG_H

/*==========================================================================
             TDSCDMA L1 HSDPA configuration header file

DESCRIPTION
  This file contains definition, declaration required for HSDPA configuration
  and maintainance code.

  Copyright (c) 2010 - 2011 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdshslog.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/22/13   zl      fix log packet 0xD033
09/08/11   owen    clean-up to comply with requirement from tools team
25/04/11   owen    to meet TDS requirement 
10/08/10   owen    Inherited this file from WCDMA code

===========================================================================*/

#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"


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

/* Information type identifier */
//#define TDSHSLOG_CFG_LOG_DATA_CHANNEL_INFO_TYPE        1
//#define TDSHSLOG_CFG_LOG_CONTROL_CHANNEL_INFO_TYPE     2
//#define TDSHSLOG_CFG_LOG_PDSCH_MIDAMBLE_INFO_TYPE      3
//#define TDSHSLOG_CFG_LOG_SCCH_SICH_MIDAMBLE_INFO_TYPE  4

// maimum size of HS-PDSCH midamble configuration message size
//#define TDSHSCFG_PDSCH_MIDAMBLE_MSG_LEN_W32 1
//#define TDSHSCFG_SCCH_SICH_MIDAMBLE_MSG_LEN_W32 3

#define TDSHSLOG_DECODE_STATUS_LOG_MAX_SAMPLE     12

/* definition for EHS and decode status  */

#define TDSHSLOG_DECODE_STATUS_EHS_NSDU_BMSK      0x1F
#define TDSHSLOG_DECODE_STATUS_EHS_NSDU_SHFT      0

#define TDSHSLOG_DECODE_STATUS_EHS_SI_BMSK      0x60
#define TDSHSLOG_DECODE_STATUS_EHS_SI_SHFT      5

#define TDSHSLOG_DECODE_STATUS_EHS_TSN_BMSK     0x1F80
#define TDSHSLOG_DECODE_STATUS_EHS_TSN_SHFT     7

#define TDSHSLOG_DECODE_STATUS_EHS_QID_BMSK     0xE000
#define TDSHSLOG_DECODE_STATUS_EHS_QID_SHFT     13

/* for td-scdma */
  /* Bit 0 - Mac-ehs enabled
             0: No
             1: Yes
     Bit 1 - 64QAM configured
             0: 64QAM is not configured 
             1: 64QAM is configured 
     Bit 4:2 (Max number of HARQ processes configured -1)
             0 = 1 HARD process configured; 
             7 = 8 HARQ processes configured. 
     Bit 5: Transport Block alignment type
             0: non-octet aligned (legacy design)
             1: octet aligned (used for MAC-ehs)
     Bit 15:6 - UE category
     */
#define TDSHSLOG_DECODE_STATUS_INFO_MAC_EHS_BMSK 0x01
#define TDSHSLOG_DECODE_STATUS_INFO_MAC_EHS_SHFT 0

#define TDSHSLOG_DECODE_STATUS_INFO_64QAM_DL_BMSK 0x02
#define TDSHSLOG_DECODE_STATUS_INFO_64QAM_DL_SHFT 1

#define TDSHSLOG_DECODE_STATUS_INFO_MAX_HARQ_BMSK 0x1C
#define TDSHSLOG_DECODE_STATUS_INFO_MAX_HARQ_SHFT 2

#define TDSHSLOG_DECODE_STATUS_INFO_TBSIZE_TBL_BMSK 0x20
#define TDSHSLOG_DECODE_STATUS_INFO_TBSIZE_TBL_SHFT 3

#define TDSHSLOG_DECODE_STATUS_INFO_UE_CAT_BMSK 0xFFC0
#define TDSHSLOG_DECODE_STATUS_INFO_UE_CAT_SHFT 6

/* end */

#define TDSHSLOG_DECODE_STATUS_LOG_MAX_BUF_SIZE (sizeof(tdshslog_decode_status_log_pkt_type) * \
                                              TDSHSLOG_DECODE_STATUS_LOG_MAX_SAMPLE)

#define TDSHSLOG_DECODE_STATUS_LOG_INFO_SIZE (sizeof(tdshslog_decode_status_log_pkt_type))
//#define TDSHSLOG_DECODE_STATUS_LOG_INFO_SIZE 4

#define TDSHSLOG_DECODE_STATUS_LOG_BUF_SIZE_MAC_HS (sizeof(tdshslog_mac_hs_status_log_struct_type))
#define TDSHSLOG_DECODE_STATUS_LOG_BUF_SIZE_MAC_EHS (sizeof(tdshslog_mac_ehs_status_log_struct_type))
    
/* DSCH status value to log */
#define TDSHSLOG_DECODE_STATUS_DSCH_STATUS_CRC_FAIL 0
#define TDSHSLOG_DECODE_STATUS_DSCH_STATUS_CRC_PASS 1
#define TDSHSLOG_DECODE_STATUS_DSCH_STATUS_DTX      2

/* HS-SCCH statistics*/
#define TDSHSLOG_STAT_LOG_MAX_SAMPLE 512

#define TDSHSLOG_DSCH_HARQ_STAT_INFO_BMSK   0x1800
#define TDSHSLOG_DSCH_HARQ_STAT_INFO_SHFT   11

#define TDSHSLOG_HS_DSCH_HARQ_MAX_PROC      8

/* MAC-hs header */
#define TDSHSLOG_NUM_MAX_MAC_HS_HDR_SAMPLES 90
#define TDSHSLOG_MAC_HS_HDR_LOG_BUG_MAX_LEN (TDSHSLOG_NUM_MAX_MAC_HS_HDR_SAMPLES * \
                                       sizeof(tdsl1_hs_mac_hdr_log_pkt_type))
#define TDSHSLOG_HS_MAC_HS_HDR_LEN_MAX_IN_WORDS     4 // words
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* this macro sets field in variable that has multiplexed field within
   same variable */
#define TDSHSLOG_SET_FIELD(var, field, val) \
  var &= (~(TDSHSLOG_##field##_BMSK)); \
  var |= (((val) << (TDSHSLOG_##field##_SHFT)) & (TDSHSLOG_##field##_BMSK))

/* this macro sets field in variable that has multiplexed field within
   same variable */
#define TDSHSLOG_CFG_EVT_SET_FIELD(var, field, val) \
  var &= (~(TDSHSLOG_CFG_EVT_##field##_BMSK)); \
  var |= (((val) << (TDSHSLOG_CFG_EVT_##field##_SHFT)) & (TDSHSLOG_CFG_EVT_##field##_BMSK))

/* this macro sets field in variable that has multiplexed field within
   same variable */
#define TDSHSLOG_DECODE_STATUS_SET_VAL(buf, field, val) \
  (buf) &= (~TDSHSLOG_DECODE_STATUS_##field##_BMSK); \
  (buf) |= (((val) << TDSHSLOG_DECODE_STATUS_##field##_SHFT) & \
            (TDSHSLOG_DECODE_STATUS_##field##_BMSK))

/* this macro sets field in variable that has multiplexed field within
   same variable */
#define TDSHSLOG_DSCH_HARQ_STAT_SET_VAL(buf, field, val) \
  (buf) &= (~TDSHSLOG_DSCH_HARQ_STAT_##field##_BMSK); \
  (buf) |= (((val) << TDSHSLOG_DSCH_HARQ_STAT_##field##_SHFT) & \
            (TDSHSLOG_DSCH_HARQ_STAT_##field##_BMSK))
/*-------------------------------------------------------------------------

			  LOG PACKET: TDSCDMA_L1_HSDPA_CFG_LOG_PACKET
				(LOG CODE: 0xD030)			

This structure is used to log HSDPA channel configuration information. 
 
This Log is only generated after TDS L1 send channel configuration message to 
FW. The log packet try to record most of things which is sent to FW.
--------------------------------------------------------------------------*/

/*! \brief HSDPA Harq process DRMB Info.
 *  
 * 32 bits total 	
 */
typedef struct
{
  /*! \brief DRMB start address for this harq process, in unit of line (128 bit)*/
  uint32  harqDrmbStartAddr : 16;
  /*! \brief DRMB size for this harq process, in unit of line (128 bit)*/
  uint32  harqDrmbSize : 16;      
} tds_hs_drmb_struct_type ;

/*! \brief HSDPA data channel configruation information
 *  
 * 288 bits total   	
 */
typedef struct
{
  /* Word 0 */
  /*! \brief Number of harq prcesses in this configuration. Valid : 1..8 */
  uint32  numHarqProcess : 4;
  /*! \brief DRMB storage direction for DRM2. 0: symbols written in descending 
    addr order , 1: symbols written in ascending addr order */
  uint32  drmbStorageDir : 1;
  /*! \brief HS configuration database index used in DECOB */
  uint32  dbIdx : 1;
  uint32  :2; 
  /*! \brief for TB size LUT. 0: bit aligned tables, 1: octet aligned tables*/
  uint32  tfriTableType : 1;
  /*! \brief UE category for TB size LUT. valid range: [1..15] when 
    tfriTableType == 0, [1..23] when tfriTableType == 1 */
  uint32  ueCategory : 5;
  uint32  :18;
   
  /* Word 1 to 8 */
  /*! \brief up to 8 HARQ processes */
  tds_hs_drmb_struct_type   drmb[8];
} tdshslog_cfg_data_channel_struct_type ;


/** HS-SCCH and HS-SICH information  
 *  
 * 32 bits total 
 */
typedef struct
{
  /*! \brief Time slot used by HS-SCCH, valid value: [0,2,3,4,5,6] */
  uint32  scchTimeSlot : 3;
  /*! \brief 16SF for the 1st physical channel of HS-SCCH, valid: [1..15]*/
  uint32  scchCode1 : 5;
  /*! \brief 16SF for the 2nd physical channel of HS-SCCH, valid: [2:16] && 
    scchCode2 > scchCode1 */
  uint32  scchCode2 : 5;
  /*! \brief Time slot used by HS-SICH, valid value: [0,2,3,4,5,6] */
  uint32  sichTimeSlot : 3;
  /*! \brief Absolute Midamble Shift to be used for HS-SICH. 
  Valid [0, 128/k, 2*(128/k), .., (k-1)*(128/k) ], where K is midamble 
  configuration [2,4,6,8,10,12,14,16] */
  uint32  midambleShift : 8;
  /*! \brief 16SF for physical channel of HS-SICH, valid: [1..16]*/
  uint32  sichCode1 :5;
  uint32  :3;

} tds_hs_ctrl_chan_struct_type ;

/** HS-SICH Power Control Configuration.  
 *  
 * 64 bits total 
 */
typedef struct
{
  /*! \brief Power difference between Ack and Nack (in 1 dB), valid : -7..8 dB */
  int16   ackNakPwrOffset;      
  /*! \brief For open loop HS-SICH power control, valid : -120..-58 dB, in 1 dB*/
  int16   prxHSSich;           
  /*! \brief Gap in frame for pathloss compensation decision, valid: 0..255 */
  uint16  maxPwrCtrlGap;

  /*! \brief if 1, UE perform compensation for pathloss when HS-SICH 
    transmission gap is less than maxPwrCtrlGap. Valid : [0,1]*/
  uint8  compEnable;
  /*! \brief HS-SICH closed loop TPC step size. Valid : 1,2,3 (dB) */
  uint8  tpcStep;

} tds_hssich_pwrctrl_struct_type;

/*! \brief HSDPA Control Channel Configuration Structure.
 *  
 * 224 bits total 	
 */
typedef struct
{
  uint32  irFlush : 1;
  uint32  : 7;
  /*! \brief Number of HS control channel pairs. Valid values : 1..4*/
  uint32  numCtrlChannel : 3;       
  uint32  : 5;
  /*! \brief H-RNTI for this UE */ 
  uint32  hrnti : 16;

  /*! \brief Up to 4 pairs of HS-SCCH/HS-SICH control channel info */
  tfw_hs_ctrlChan_info_t  info[4];

  /*! \brief Power control information for HS-SICH */
  tfw_hssich_pwrCtrl_config_t   hssichPwrConfig;

} tdshslog_cfg_control_channel_struct_type ;

/*! \brief HSDPA pdsch midable configuration and scch/sich miamble configuration   
 * 
 * 16 bits total 	
 */
typedef struct
{
  /** Midamble allocation mode. When this slot is not assigned, set all
      fields to 0 */
  uint16 allocMode : 2;
  /*! \brief Midamble Config Number. Valid : [2,4,6,8,10,12,14,16]*/
  uint16 configK : 5;
  uint16 : 1;
  /*! \brief Midamble shift for UE Specific allocation. Valid : 0..(configK-1)*/
  uint16 shift : 8;
} tdshslog_midamble_config_struct_type;


/*! \brief TDSCDMA HSDPA Configuration Log Packet (LOG CODE: 0xD030):
 * This Log contains all Channel configuration information, including: 
 * Data channel, control channel, and HS-PDSCH midamble config 
 * !!! This data structure for this log packet must align with FW/SW interface
 *    
 * 528 bits total 	 
 */
typedef struct
{
  /* Log header */
  log_hdr_type header;

  /*! \brief Version  */
  uint32  version:8;
  /*! \brief Activation time  */
  uint32 act_frame:8;
  /*! \brief reserved */
  uint32  reserved:16;

  /*! \brief HS configuration for data channel */  
  tdshslog_cfg_data_channel_struct_type data_ch_cfg;
  /*! \brief HS configuration for control channel */  
  tdshslog_cfg_control_channel_struct_type control_ch_cfg;
  /*! \brief HS configuration for HS-PDSCH midamble */ 
  tdshslog_midamble_config_struct_type pdsch_midamble;

} tdsl1_hsdpa_cfg_log_pkt_type;

#define TDSL1LOG_HSDPA_CFG_DATA_CHANN_STRUCT_LEN(num_drmb)  ( \
    FPOS(tdshslog_cfg_data_channel_struct_type, drmb)+  \
    ( num_drmb * sizeof(tds_hs_drmb_struct_type)))

#define TDSL1LOG_HSDPA_CFG_CTRL_CHANN_STRUCT_LEN(num_scch)  ( \
    FPOS(tdshslog_cfg_control_channel_struct_type, info)+  \
    ( num_scch * sizeof(tds_hs_ctrl_chan_struct_type)))
  
/* Macro to determine the sizes of the log packet */
#define TDSL1LOG_HSDPA_CFG_LOG_PKT_LEN(num_drmb, num_scch)  ( \
    sizeof(log_hdr_type)+ \
    sizeof(uint32) + \
    TDSL1LOG_HSDPA_CFG_DATA_CHANN_STRUCT_LEN(num_drmb) + \
    TDSL1LOG_HSDPA_CFG_CTRL_CHANN_STRUCT_LEN(num_scch)+\
    sizeof(tdshslog_midamble_config_struct_type))

/*-------------------------------------------------------------------------

		LOG PACKET: TDSCDMA_L1_HSDPA_DECODE_STATUS_LOG_PACKET
				(LOG CODE: 0xD031)			

This structure is used to log HSDPA data decoding status: HS-PDSCH & HS-SCCH. 
 
This Log is only generated after UE begins to decode HS-SCCH and HS-PDSCH;   
the results of HS-SCCH is from FW.
--------------------------------------------------------------------------*/

/*! \brief HS-SCCH decoding result data structure
 * 
 * 192 bits total 	
 */

typedef struct
{
  /*! \brief Starting System frame number,  range is 0 to 4095 */
  uint32 arriving_sys_frame_no : 16;
  /*! \brief subframe number */
  uint32 arriving_sub_frame_no : 16;

  /*! \brief CRC pass or fail, 0:1 */
  uint32 crc_pass : 16;
  /*! \brief HS-SCCH cyclic sequence number */
  uint32 hcsn : 3;
  /*! \brief new data indication */
  uint32 new_data : 1;
  /*! \brief redundant version */
  uint32 redundant_ver : 3;
  /*! \brief HARQ process ID; 0:7*/
  uint32 harq_id : 3;
  /*! \brief TB size index, 64 different TB sizes */
  uint32 tbs_index :6;

  /*! \brief modulation scheme for HS-PDSCH, 1=16QAM, 0=QPSK */
  uint32 mod_scheme : 1;
  /*! \brief time slots are scheduled for HS-PDSCH */
  uint32 ts_hs_pdsch : 5;
  /*! \brief start code index for HS-PDSCH, 0:15*/
  uint32 start_code_index : 4;
  /*! \brief end code index  for HS-PDSCH,, 0:15*/
  uint32 end_code_index : 4;
  /*! \brief HS-SCCH type; not used; 0- type 1; 1 - 15 reserved */
  uint32 type : 4;
  /*! \brief HS-SCCH index 0:3 */
  uint32 index : 2;
  uint32  : 12;

 /*! \brief  TPC command for HS-SICH transmissio*/
  uint32 tpc :16; // temporally
  /*! \brief  Uplink timing shiftting command for HS-SICH transmission*/
  uint32 ss : 16; // temporally

  /* internal info */
  /*! \brief LLR Energy metric */
  uint32 energy_metric :8;
  /*! \brief Yamamoto Metric, only for conv code */
  uint32 yama_metric :8;
  /*! \brief Symbol error rate, only for conv code  */
  uint32 symb_err_rate :8;
  /*! \brief reserved */
  uint32 :8;

  /*! \brief Additional FW information from HS-SCCH decode when CRC fails . */
  uint32 extra_info : 5;
  uint32 : 27;

} tdshslog_scch_decode_status_log_struct_type;

/*! \brief HS-PDSCH decoding result data structure
 * 
 * 64 bits total 	
 */

typedef struct
{

  /*! \brief Starting System frame number,  range is 0 to 4095 */
  uint32 arriving_sys_frame_no : 16;
  /*! \brief subframe number */
  uint32 arriving_sub_frame_no : 16;

  /*! \brief CRC pass or fail, 0:1*/
  uint32 crc_pass : 1 ;
  /*! \brief HARQ process ID; 0:7*/
  uint32 harq_id : 3 ;
  /*! \brief new data indication */
  uint32 new_data : 1;
  /*! \brief TB size, from DOB status */
  uint32 trblk_size : 14;
  /*! \brief retransmission number
     0x0 - First transmissions
 	 0x1 - Second transmission
      ...
     0x7 - 8th transmission */
  uint32 num_retrans : 3;
  /*! \brief reserved */
  uint32 :10;

}tdshslog_pdsch_decode_status_log_struct_type;  
   
/*! \brief TDSCDMA DECODE Status Union Type: 
 *  For HS-SCCH and HS-PDSCH decoding results
 *  
 *  256 bits in total
 *  
 */    
typedef union
{
  /*! \brief Log data buffer for HS-SCCH  */
  tdshslog_scch_decode_status_log_struct_type scch;

  /*! \brief Log data buffer for HS-PDSCCH  */
  tdshslog_pdsch_decode_status_log_struct_type pdsch;

}tdshslog_decode_status_log_pkt_union_type;

/*! \brief TDSCDMA L1 DownLink Common physical Ch info 
 * ### bits total
 */
typedef struct 
{ 
  /*! \brief physical channel type : Channel type:
   *  0 - HS-SCCH
   *  1 - HS-PDSCH
   */
  uint32  phyChType:8;  
  uint32  :24; 
  //32 bits
  
  tdshslog_decode_status_log_pkt_union_type phych;
     
}tdshslog_decode_status_log_pkt_type;

/*! \brief TDSCDMA DECODE Status Log Packet (LOG CODE: 0xD031): 
 *  For HS-SCCH and HS-PDSCH decoding results
 *  
 *  6464 bits in total 
 *  
 */   

typedef struct
{
  /* Log header */
  log_hdr_type header;

  /*! \brief Version  */
  uint32  version:8;
  /*! \brief Serving Cell Parameter ID, Range is 0 to 127 */
  uint32 cell_id : 8;
  /*! \brief reserved */
  uint32  reserved:16;

  /*! \brief Number of samples*/
  uint32  num_samples :8;

  /* Bit 0 - Mac-ehs enabled
             0: No
             1: Yes
     Bit 1 - Transport Block Size Table Used
             0: 
             1: 
     Bit 2 - 64QAM configured
             0: 64QAM is not configured 
             1: 64QAM is configured 
     Bit 5:3 (Max number of HARQ processes configured -1)
             0 = 1 HARD process configured; 
             7 = 8 HARQ processes configured. 
     Bit 6: Transport Block alignment type
             0: non-octet aligned (legacy design)
             1: octet aligned (used for MAC-ehs)
     Bit 15:7 - UE category
  */
  /*! \brief misc information for decoding */
  uint32 info : 16;
  /*! \brief reserved */
  uint32 : 8;

  /* log data buffer for HS-SCCH or HS-PDSCH*/
  tdshslog_decode_status_log_pkt_type decode_status[TDSHSLOG_DECODE_STATUS_LOG_MAX_SAMPLE];
  
}tdsl1_hsdpa_decode_status_log_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSL1LOG_HSDPA_DECODE_STATUS_PKT_LEN(num_sample)  ( \
    FPOS(tdsl1_hsdpa_decode_status_log_pkt_type, decode_status)+  \
    ( num_sample * sizeof(tdshslog_decode_status_log_pkt_type)))


/*-------------------------------------------------------------------------

		LOG PACKET: TDSCDMA_L1_HSDPA_HSSCCH_STAT_LOG_PACKET
				(LOG CODE: 0xD032)			

This structure is used to log HSDPA HS-SCCH decoding statistics results.  

--------------------------------------------------------------------------*/

/*! \brief TDSCDMA HS-SCCH STAT Log Packet (LOG CODE: 0xD032): 
 *  
 *  96 bits in total 
 *  
 */   

typedef struct
{
  /* Log header */
  log_hdr_type header;

  /*! \brief Version  */
  uint32  version:8;
  /*! \brief Serving Cell Parameter ID, Range is 0 to 127 */
  uint32 cell_id : 8;
  /*! \brief starting system frame number, Range is 0 to 4096 */
  uint32 start_sys_frame_no : 16;

  /*! \brief Number of sub frames */
  uint32 num_sub_frames : 16;
  /*! \brief number of SCCH valid */
  uint32 num_scch_valid : 16;

  /*! \brief number of SCCH invalid */
  uint32 num_scch_invalid : 16;
  uint32 : 16;

} tdsl1_hs_scch_stat_log_pkt_type;


/*-------------------------------------------------------------------------

		LOG PACKET: TDSCDMA_L1_HSDPA_HARQ_STAT_LOG_PACKET
				(LOG CODE: 0xD033)			

This structure is used to log HSDPA HS-SCCH decoding statistics results.  

--------------------------------------------------------------------------*/

/*! \brief TDSCDMA HARQ PROC stat data structure (LOG CODE: 0xD033): 
 *  
 *  160 bits in total 
 *  
 */
typedef struct 
{
  /*! \brief HARQ process Id */
  uint32 proc_id : 4;
  /*! \brief BLER of New transmissions */
  uint32 bler_newTx_percent :16;
  /*! \brief reserved */
  uint32 rsvd: 12;
  // 4 bytes
  /*! \brief number of new transmission */
  uint32 num_newtx :12;
  /*! \brief number of Retransmission */
  uint32 num_retx :12;
  /*! \brief number of block errors for new transmission. No ACK received at all. 
           This is the number of lost PDUs after maximal number of retransmission. 
    */
  uint32 num_failed_newtx :8;
  //8 bytes
  /*! \brief number of block errors for Re-transmission. No ACK received at all: 
           */
  uint32 num_failed_retx :8;
  /*! \brief number of NACKs after first (new) transmission */
  uint32 num_nack_1st_Tx :8;
  /*! \brief number of NACKs after 2 transmissions */
  uint32 num_nack_2nd_Tx : 8;
  /*! \brief number of NACKs after 3 transmissions */
  uint32 num_nack_3rd_Tx :8;
  // 12 bytes
  /*! \brief number of NACKs after 4 transmissions */
  uint32 num_nack_4th_Tx :8;
  /*! \brief number of ACKs for New transmissions */
  uint32 num_ack_newTx :8;
  /*! \brief number of ACKs for the first Retransmissions */
  uint32 num_ack_1st_Rtx :8;
  /*! \brief number of ACKs for the second Retransmissions */
  uint32 num_ack_2nd_Rtx :8;
  //16 bytes

  /*! \brief number of ACKs for the 3rd Retransmissions */
  uint32 num_ack_3rd_Rtx :8;
  /*! \brief number of ACKs for the 4th and remaining Retransmissions */
  uint32 num_ack_res_Rtx :8;
  /* */
  uint32 padding: 16;
  // 20 bytes
} tdshslog_dsch_harq_proc_stat_log_struct_type;


/*! \brief TDSCDMA HS-DSCH HARQ STAT Log Packet: 
 *  
 *  MAX: 20 bytes * 8 + 36 bytes = 196 bytes
 *  MIN: 20 bytes *1 + 36 bytes = 56 bytes
 */   

typedef struct
{
  /* Log header */
  log_hdr_type header;

  /*! \brief Version  */
  uint32  version:8;
  /*! \brief Serving Cell Parameter ID, Range is 0 to 127 */
  uint32 cell_id : 8;
  /*! \brief reserved */
  uint32  reserved:16;
   // 4 bytes
  /*! \brief Number of sub frames */
  uint32 num_sub_frames :14;
  /*! \brief starting system frame number, Range is 0 to 4096 */
  uint32 start_sys_frame_no : 14;
  /*! \brief number of HARQ process */
  uint32 num_harq_proc :4;
  // 8 bytes
  /*! \brief Avg Bit rate in kbps */
  uint32 bitrate_avg_kbps :32;
  // 12 bytes
  /*! \brief BLER of Transmissions every 2 seconds*/
  //uint32 bler_in_2secs_percent :8;
  /*! \brief Bit rate in kbps for 2 seconds */
  uint32 bitrate_in_2secs_kbps : 32;
  //16 bytes
  /*! \brief BLER of New transmissions */
  uint32 bler_overall_percent :16;  
  /*! \brief New tx ACK for all HARQ processes in about 2 seconds */
  uint32 num_new_ack_2sec: 16;
  //20 bytes
  /*! \brief Total tx ACK for all HARQ processes in about 2 seconds */
  uint32 num_tot_ack_2sec: 16;
  /*! \brief New tx NACK for all HARQ processes in about 2 seconds */
  uint32 num_1st_nack_2sec: 16;
  //24 bytes
  /*! \brief Remaining Tx NACK for all HARQ processes in about 2 seconds: 
              NACK after max num of retransmission */
  uint32 num_res_nack_2sec: 16;
  /*! \brief Total tx NACK for all HARQ processes in about 2 seconds: nack1+nack2+nack3+... */
  uint32 num_tot_nack_2sec: 16;
  //28 bytes

  /*! \brief newtx_bler_2sec = num_1st_nack_2sec/(num_new_ack_2sec + num_1st_nack_2sec) */
  uint32 newtx_bler_2sec: 16;
  /*! \brief res_bler_2sec = num_res_nack_2sec/(num_tot_ack_2sec + num_res_nack_2sec) */
  uint32 res_bler_2sec: 16;
  //32 bytes
  /*! \brief tot_bler_2sec = num_tot_nack_2sec/(num_tot_ack_2sec + num_tot_nack_2sec) */
  uint32 tot_bler_2sec: 16;
  uint32 padding: 16;
  //36 bytes
  /*! \brief HARQ proc stats */
  tdshslog_dsch_harq_proc_stat_log_struct_type 
          proc_stat[TDSHSLOG_HS_DSCH_HARQ_MAX_PROC];
  // 20 bytes * 8 + 36 bytes = 196 bytes. 
}tdsl1_dsch_harq_stat_log_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSL1LOG_HSDPA_HARQ_STATUS_PKT_LEN(num_harqs)  ( \
    FPOS(tdsl1_dsch_harq_stat_log_pkt_type, proc_stat)+  \
    ( num_harqs * sizeof(tdshslog_dsch_harq_proc_stat_log_struct_type)))
/*-------------------------------------------------------------------------

		LOG PACKET: TDSCDMA_L1_HSDPA_MAC_HS_HDR_LOG_PACKET
				(LOG CODE: 0xD034)			

This structure is used to log HSDPA HS-SCCH decoding statistics results.  

--------------------------------------------------------------------------*/

/*! \brief TDSCDMA MAC HS header logging packet (LOG CODE: 0xD034): 
 *  
 *  
 *  176 bits in total 
 *  
 */ 
typedef struct
{
  /* Log header */
  log_hdr_type header;

  /*! \brief Version  */
  uint32  version:8;
  /*! \brief Global subframe number, Range is 0 to 8192 */
  uint32 global_sub_fn : 16;
  /*! \brief reserved */
  uint32  reserved : 8;

  /*! \brief MAC header payload buffer */
  uint32 mac_hdr_buffer[TDSHSLOG_HS_MAC_HS_HDR_LEN_MAX_IN_WORDS];

  /*! \brie Number of MAC headers */
  uint16 num_mac_hdr;

} tdsl1_hs_mac_hdr_log_pkt_type;

/* Macro to determine the sizes of the log packet */
#define TDSL1LOG_HS_MAC_HDR_PKT_LEN(num_hdrs)  ( \
    FPOS(tdsl1_hs_mac_hdr_log_pkt_type, mac_hdr_buffer)+  \
    ( num_hdrs * sizeof(uint32)))

/*-------------------------------------------------------------------------

		LOG PACKET: TDSCDMA_L1_HSDPA_OL_CQI_LOG_PACKET
				(LOG CODE: 0xD035)			

This structure is used to log HSDPA outer-loop CQI informaion.  

--------------------------------------------------------------------------*/

/*! \brief TDSCDMA HSDPA Outer-Loop CQI logging packet (LOG CODE: 0xD035): 
 *  
 *  
 *  64 bits in total 
 *  
 */ 
typedef struct
{
  /* Log header */
  log_hdr_type header;

  /*! \brief Version  */
  uint32  version:8;
  /*! \brief Global subframe number, Range is 0 to 8192 */
  uint32 global_system_frame_no : 16;
  /*! \brief reserved */
  uint32  reserved : 8;

  /*! \brief SE Ret flag */
  uint16 bSERest: 1;
  /*! \brief Calculated BLER for the window (i.e., 100) */
  uint16 bler_calc:15;

  /*! \brief SIR(X1024) adjustment value */
  int16 sirAdj;

} tdsl1_hs_ol_cqi_log_pkt_type;


#endif /* TDSHSLOG_H */

