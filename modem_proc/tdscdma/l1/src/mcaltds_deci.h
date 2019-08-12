#ifndef MCALTDS_DECI_H
#define MCALTDS_DECI_H

/*============================================================================*/
/** @file  
 * This module contains definition declaration for MCAL (modem core abstraction
 * layer) WCDMA decoder module. This header filer is only intended to be included
 * in MCAL WCDMA decoder related modules for internal use only.
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/mcaltds_deci.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/29/12    owen    Change FEATURE_HSDPA_MAC_EHS to FEATURE_TDSCDMA_HSDPA_MAC_EHS 

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "tdscdma_variation.h"
#include "comdef.h"
#include "queue.h"
#include "mcaltds_dec.h"
#include "tdsl1std.h"
#include "tdsl1macif.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_IDX_INVALID 0xFF

/* Max cctrch table config table from RRC during one time configuration. */
#define MCALTDS_DEC_MAX_CCTRCH_CONFIG_TABLE 3
/* Transport channel table info */    
/* ---------------------------- */

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_FIRST_R4_DRM_BUF_IDX 0

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_LAST_R4_DRM_BUF_IDX 4

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_BCH_DRM_BUF_IDX 0

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_PCH_FACH_DRM_BUF_IDX 1

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_DCH_DRM_BUF_IDX 1

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_LAST_DCH_DRM_BUF_IDX 3

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_FIRST_DRM_BUF_START_ADDR 0

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_INVALID_DRM_BUF_ADDR 0xFFFF

/* Invalid index. Used as general purpose indicator */
#define MCALTDS_DEC_INVALID_DRM_BUF_DIR 0xFF


/*      TF table info      */    
/* ----------------------- */

/* Total number of TF table in mDSP */
#define MCALTDS_DEC_TOTAL_TFT_PER_CCTRCH 64
/* Maximum items in the TF pool */
#define MCALTDS_MAX_NUM_TF_POOL   TDSL1_DL_MAX_TF


/* Minimum DSM item size in bytes */
#define MCALTDS_DEC_MIN_DSM_ITEM_SIZE 48

/* Maximum possible DSM item chain pointers.
   The 4 is added to the TB size bytes. 1 for any fractions output
   of a byte after dividing by 8 and 3 for possible 3 extra word if
   there is non word32 align number */
#define MCALTDS_DEC_MAX_DSM_ITEM_CHAIN_DEPTH ((((TDSL1_DL_MAX_TB_BIT_SIZE)/8) + 4)/(MCALTDS_DEC_MIN_DSM_ITEM_SIZE))
 
/* Number of bits required for BCH block
 * PCCPCH 246 bits + 16 bit CRC + 8 Tail bits = 270
 */
#define MCALTDS_DEC_BCH_BLOCK_SIZE_IN_BITS 270

/* DRM buffer size required for BCH code block */
#define MCALTDS_DEC_DRM_BUF_SIZE_BCH_BLOCK MCALTDS_DEC_DRM_BUF_SEGMENT_SIZE(MCALTDS_DEC_BCH_BLOCK_SIZE_IN_BITS)

/* DRM buffer size required for non BCH code block */
#define MCALTDS_DEC_NON_BCH_DRM_BUF_START_ADDR (MCALTDS_DEC_FIRST_DRM_BUF_START_ADDR + MCALTDS_DEC_DRM_BUF_SIZE_BCH_BLOCK)
//#define MCALTDS_DEC_NON_BCH_DRM_BUF_START_ADDR 14500 // in line, from Hari


/* DRM buffer strorage direction macro */
#define MCALTDS_DEC_DRM_BUF_ST_DIR_INC 1
#define MCALTDS_DEC_DRM_BUF_ST_DIR_DEC 0

/* Total DRM buffer size */

//#define MCALTDS_DEC_DRM_BUFER_SIZE WFW_DEMOD_DRMB_MEM_SIZE
//#define TDSDEC_DRM_BUF_SYMBOLS_PER_WORD WFW_DEMOD_DRMB_NUM_BANKS



// added by Owen for TDS SW, based on FW document
/* Total number of 6-bit words in Taxis hardware = 536232 
 * (3217392 /6)
 * Each word contains 21 symbols. So total number of DRMB
 * locations = 536232 / 21 = 25534 */
/* Whether the total space can be allocated is unknown due 
 * to HW bug */
#define MCALTDS_DEC_DRM_BUFER_SIZE 14800  // in line, from Hari on Nov 10
#define MCALTDS_DEC_DRM_BUF_SYMBOLS_PER_WORD 21

#define MCALTDS_DEC_DRM_BUF_SYMBOLS_PER_LINE 21

#define MCALTDS_DEC_DRM_BUF_SEGMENT_SIZE(size)  CEIL((size * 1), MCALTDS_DEC_DRM_BUF_SYMBOLS_PER_LINE)

#define MCALTDS_DEC_DRM_BUF_UNUSED              0xFFFF

 /* DRM buffer start address for HARQ process memory allocation */
 /* Starting address for HARQ in DRMB has to be an even address for Taxis, hence subtracting '2' after the end of
  * the PCCPCH DRMB segment. 1 DRMB location between the PCCPCH segment and the HSDPA segment is wasted. */
 //#define MCALTDS_DEC_DRM_BUF_START_ADDRESS_HARQ_MEM ((MCALTDS_DEC_DRM_BUFER_SIZE) - (4*(3 * MCALTDS_DEC_DRM_BUF_SIZE_BCH_BLOCK)) - 2)
 //#define MCALTDS_DEC_DRM_BUF_START_ADDRESS_HARQ_MEM    300  // in line, from Hari on Nov 10
 #define MCALTDS_DEC_DRM_BUF_START_ADDRESS_HARQ_MEM    1600  // in line, from Hari on Nov 10
 #define MCALTDS_DEC_DRM_BUF_HARQ_MEM_ST_DIR MCALTDS_DEC_DRM_BUF_ST_DIR_INC  // changed by owen on Nov 10, 2011

#ifndef TEST_FRAMEWORK
#define IMEM_L1_BASE  0x58004000
#else
#error code not present
#endif


/* There is always only one R99 CcTrCh active at any given time. It use DRM
   buffer parameter info at index 0 */
#define MCALTDS_DEC_CCTRCH_R4_TYPE_DRM_INFO_IDX 0

/* DRM bank for HSDPA */
//#define MCALTDS_DEC_CCTRCH_HS_TYPE_DRM_INFO_IDX   6


/* TURBO BETA SELECT types */

#define MCALTDS_DEC_TURBO_BETA_SELECT_NO_LUT 0
#define MCALTDS_DEC_TURBO_BETA_SELECT_3      1
#define MCALTDS_DEC_TURBO_BETA_SELECT_6      2
#define MCALTDS_DEC_TURBO_BETA_SELECT_9      3
#define MCALTDS_DEC_TURBO_BETA_SELECT_12     4

#ifdef FEATURE_TDSCDMA_OFFLINE_MODEM
#define MCALTDS_DEC_MAX_ITERATIONS 8
#else
#define MCALTDS_DEC_MAX_ITERATIONS 6
#endif

/* CCTrCh TrCh TF Bit field masks, shifts etc. */
/* ------------------------------------------- */

/* CCTrCh table headers */
/* -------------------- */

/* Maximum decoder output buffer status FIFO entries */
#define MCALTDS_DEC_MAX_NON_HS_STATUS_FIFO_COUNT  TFW_NON_HS_DECOB_STATUS_FIFO_SIZE
#define MCALTDS_DEC_MAX_HS_STATUS_FIFO_COUNT      TFW_HS_DECOB_STATUS_FIFO_SIZE

#define MCALTDS_DEC_MAX_STATUS_FIFO_COUNT MCALTDS_DEC_MAX_NON_HS_STATUS_FIFO_COUNT + \
                                  MCALTDS_DEC_MAX_HS_STATUS_FIFO_COUNT

/* Number of word 16 in a status FIFO entry */
#define MCALTDS_DEC_NUM_WORD16_PER_FIFO_ENTRY 4

/* Number of MAC-d PDUs size per Q received in one HS sub frame */
//#define MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT 8
#define MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT TDSL1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT

/* Number of word 32 in a TB status */
#define MCALTDS_DEC_NUM_WORD32_PER_TB_SET_STATUS_CONV_CODING  4
#define MCALTDS_DEC_NUM_WORD32_PER_TB_SET_STATUS_TURBO_CODING 1



#define MCALTDS_DEC_NUM_DECIPH_TASK_PRG_PER_TRIGGER (MCALTDS_DEC_NUM_DECIPH_TASKS/2)

/* DOB HW related macros */
/* --------------------- */
#define MCALTDS_DEC_CODE_TYPE_CONV_1_2 TFW_DBACK_CODE_TYPE_CONV_1_2
#define MCALTDS_DEC_CODE_TYPE_CONV_1_3 TFW_DBACK_CODE_TYPE_CONV_1_3 
#define MCALTDS_DEC_CODE_TYPE_TURBO TFW_DBACK_CODE_TYPE_TURBO
#define MCALTDS_DEC_CODE_TYPE_UNCODED TFW_DBACK_CODE_TYPE_UNCODED
#define MCALTDS_DEC_MAX_DOB_SIZE 286
#define MCALTDS_DEC_DOB_DATA_UNIT 4

#define MCALTDS_DEC_DOB_DATA_CONVERT_LITTLE_ENDIAN(data,temp_data) \
    temp_data = ((data<< 24) |((data & 0xFF00) << 8)|((data & 0xFF0000) >> 8)|((data & 0xFF000000) >> 24));\
    data = temp_data

#define MCALTDS_DEC_DOB_RD_DATA(dob_addr, dob_offset)\
  in_dword(dob_addr+ ((dob_offset) * MCALTDS_DEC_DOB_DATA_UNIT))
 
/* DOB buffer size */



/* Burst read address constants */
/* ---------------------------- */

/* DOB status item bit mask shifts */
/* ------------------------------- */

/* for viterbi decoder */
#define MCALTDS_DEC_DOB_ST_VD_TB_CRC_BITS_BMSK 0xFFFFFFFF
#define MCALTDS_DEC_DOB_ST_VD_TB_CRC_BITS_REG  0
#define MCALTDS_DEC_DOB_ST_VD_TB_CRC_BITS_SHFT 0

#define MCALTDS_DEC_DOB_ST_VD_TRCH_EGY_MAT_BMSK 0x0000FFFF
#define MCALTDS_DEC_DOB_ST_VD_TRCH_EGY_MAT_REG  1
#define MCALTDS_DEC_DOB_ST_VD_TRCH_EGY_MAT_SHFT 0

#define MCALTDS_DEC_DOB_ST_VD_TRCH_SE_CNT_BMSK 0xFFFF0000
#define MCALTDS_DEC_DOB_ST_VD_TRCH_SE_CNT_REG  2
#define MCALTDS_DEC_DOB_ST_VD_TRCH_SE_CNT_SHFT 16

#define MCALTDS_DEC_DOB_ST_VD_CB_Q_BITS_BMSK 0x0000FFFF
#define MCALTDS_DEC_DOB_ST_VD_CB_Q_BITS_REG  2
#define MCALTDS_DEC_DOB_ST_VD_CB_Q_BITS_SHFT 0

/* Last Best Metric */
#define MCALTDS_DEC_DOB_ST_VD_TRCH_LBM_BMSK  0xFFFF0000
#define MCALTDS_DEC_DOB_ST_VD_TRCH_LBM_REG   3
#define MCALTDS_DEC_DOB_ST_VD_TRCH_LBM_SHFT  16

/* Last State Metric */
#define MCALTDS_DEC_DOB_ST_VD_TRCH_LSM_BMSK  0x0000FFFF
#define MCALTDS_DEC_DOB_ST_VD_TRCH_LSM_REG   3
#define MCALTDS_DEC_DOB_ST_VD_TRCH_LSM_SHFT  0

/* for turbo decoder */
#define MCALTDS_DEC_DOB_ST_TD_TB_CRC_BITS_BMSK 0xFFFFFFFF
#define MCALTDS_DEC_DOB_ST_TD_TB_CRC_BITS_REG  0
#define MCALTDS_DEC_DOB_ST_TD_TB_CRC_BITS_SHFT 0

/* for uncoded TrChs */
#define MCALTDS_DEC_DOB_ST_UNC_TB_CRC_BITS_BMSK 0xFFFFFFFF
#define MCALTDS_DEC_DOB_ST_UNC_TB_CRC_BITS_REG  0
#define MCALTDS_DEC_DOB_ST_UNC_TB_CRC_BITS_SHFT 0

#define MCALTDS_DEC_DOB_ST_UNC_TRCH_EGY_MAT_BMSK 0x0000FFFF
#define MCALTDS_DEC_DOB_ST_UNC_TRCH_EGY_MAT_REG  1
#define MCALTDS_DEC_DOB_ST_UNC_TRCH_EGY_MAT_SHFT 0

#define MCALTDS_DEC_DOB_TB_SET_STATUS_ITEM(buffer, item, coding_type) \
  (((buffer)[MCALTDS_DEC_DOB_ST_##coding_type##_##item##_REG] & \
    MCALTDS_DEC_DOB_ST_##coding_type##_##item##_BMSK) >> \
   MCALTDS_DEC_DOB_ST_##coding_type##_##item##_SHFT)
   
/* minumum TB energy matrix for TBset for Conv coded TrCh.
   If the egy matric value is lower than this CRC is forced to fail */
#define MCALTDS_DEC_MIN_TB_EGY_MATRIC_THRESH 256

/* --------------------------------------------------------------------------- */
/*                              TFCI decode statistics                         */
/* --------------------------------------------------------------------------- */
#if 0
/* Number of bytes in a single TFCI sample */
#define MDSP_NUM_BYTES_TFCI_ENTRY   8
/* There are two banks of TFCI: even and odd. Each bank has information for 
   10 CCTrCh. The details of the structure can be found in 
   mcaltds_dec_decoded_tfci_info_struct_type */
#define TDSMDSP_DECODED_TFCI_EVEN  TDSMDSP_DECODED_TFCI
/* To get to the odd address, add offset corresponding to 10 CCTrCh for even */
#define TDSMDSP_DECODED_TFCI_ODD   (TDSMDSP_DECODED_TFCI + \
   (MCALTDS_DEC_TOTAL_CCTRCH_TABLES_NON_BCH * TDSMDSP_NUM_BYTES_TFCI_ENTRY))
/* Each CCTrCh buffers TFCI info for 4 frames in mDSP */
#define MCALTDS_DEC_NUM_TFCI_BUFFER_PER_CCTRCH    4
#endif

/* DOB deciphering engine related macros */
/* ------------------------------------- */

#ifdef FEATURE_TDSCDMA_A2_HW_DL
#define MCALTDS_DEC_DOB_DECIP_COUNT_BMSK 0xFFFFFFFF
#define MCALTDS_DEC_DOB_DECIP_COUNT_REG  0
#define MCALTDS_DEC_DOB_DECIP_COUNT_SHFT 0 

#define MCALTDS_DEC_DOB_DECIP_DIR_BMSK 0x00000001
#define MCALTDS_DEC_DOB_DECIP_DIR_REG  1
#define MCALTDS_DEC_DOB_DECIP_DIR_SHFT 0

#define MCALTDS_DEC_DOB_DECIP_LEN_BMSK 0x7FFF0000
#define MCALTDS_DEC_DOB_DECIP_LEN_REG  1
#define MCALTDS_DEC_DOB_DECIP_LEN_SHFT 16 

#define MCALTDS_DEC_DOB_DECIP_BEARER_ID_BMSK 0x000001F0
#define MCALTDS_DEC_DOB_DECIP_BEARER_ID_REG  1
#define MCALTDS_DEC_DOB_DECIP_BEARER_ID_SHFT 4

#define MCALTDS_DEC_DOB_DECIP_KEY_ID_BMSK 0x0000000E
#define MCALTDS_DEC_DOB_DECIP_KEY_ID_REG  1
#define MCALTDS_DEC_DOB_DECIP_KEY_ID_SHFT 1

#define MCALTDS_DEC_DOB_DECIP_ALGO_BMSK 0x00000200
#define MCALTDS_DEC_DOB_DECIP_ALGO_REG  1
#define MCALTDS_DEC_DOB_DECIP_ALGO_SHFT 9

#define MCALTDS_DEC_DOB_DECIP_STOP_BIT_BMSK 0x80000000
#define MCALTDS_DEC_DOB_DECIP_STOP_BIT_REG  2
#define MCALTDS_DEC_DOB_DECIP_STOP_BIT_SHFT 31

#define MCALTDS_DEC_DOB_DECIP_INT_BIT_BMSK 0x40000000
#define MCALTDS_DEC_DOB_DECIP_INT_BIT_REG  2
#define MCALTDS_DEC_DOB_DECIP_INT_BIT_SHFT 30

#define MCALTDS_DEC_DOB_DECIP_CONTINUE_BIT_BMSK 0x20000000
#define MCALTDS_DEC_DOB_DECIP_CONTINUE_BIT_REG  2
#define MCALTDS_DEC_DOB_DECIP_CONTINUE_BIT_SHFT 29

#define MCALTDS_DEC_DOB_DECIP_START_ADDR_BMSK 0x003FFF00
#define MCALTDS_DEC_DOB_DECIP_START_ADDR_REG  2
#define MCALTDS_DEC_DOB_DECIP_START_ADDR_SHFT 8

#define MCALTDS_DEC_DOB_DECIP_OFFSET_BMSK 0x0000001F
#define MCALTDS_DEC_DOB_DECIP_OFFSET_REG  2
#define MCALTDS_DEC_DOB_DECIP_OFFSET_SHFT 0
#endif /* FEATURE_TDSCDMA_A2_HW_DL */

/*lint -e773 */
#define MCALTDS_DEC_SET_DOB_DECIP_PARAM(buffer, param, val) \
  /*lint -e737 */ \
  buffer[MCALTDS_DEC_DOB_DECIP_##param##_REG] |= \
    ((((uint32) (val)) <<MCALTDS_DEC_DOB_DECIP_##param##_SHFT) & MCALTDS_DEC_DOB_DECIP_##param##_BMSK) \
  /*lint +e737 */
/*lint +e773 */

#define MCALTDS_DEC_DOB_DECIPH_STATUS_TRBLK_NUM_BMSK 0x000000FF
#define MCALTDS_DEC_DOB_DECIPH_STATUS_TRBLK_NUM_SHFT 0

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
#define MCALTDS_DEC_NUM_TBSET_LOGPKT  2
#define MCALTDS_DEC_INVALID_INDEX     0xFF
/*for L1M submit log packet use*/
void mcaltds_decdata_decode_log_submit(void);
#endif


/* MAC-HS header fields */
/* -------------------- */

/* CRC length for HS-DSCH block */
#define MCALTDS_DEC_HS_DSCH_BLK_CRC_LEN 24

/* Number of bits in varioys fields of MAC-hs header */   
#define MCALTDS_DEC_HS_MAC_HS_HDR_VF_NUM_BITS         1 /* Version flag */
#define MCALTDS_DEC_HS_MAC_HS_HDR_QID_NUM_BITS        3 /* Queue Id */
#define MCALTDS_DEC_HS_MAC_HS_HDR_TSN_NUM_BITS        6 /* Transmission seq number */
#define MCALTDS_DEC_HS_MAC_HS_HDR_SID_NUM_BITS        3 /* MAC-d PDU size indicator */
#define MCALTDS_DEC_HS_MAC_HS_HDR_N_MACD_PDU_NUM_BITS 7 /* Number of MAC-d s*/
#define MCALTDS_DEC_HS_MAC_HS_HDR_F_FLAG_NUM_BITS     1 /* Final flag bit */

/* Maximum possible size of MAC-hs header */   
#define MCALTDS_DEC_HS_MAC_HS_HDR_LEN_MAX \
  ((MCALTDS_DEC_HS_MAC_HS_HDR_VF_NUM_BITS + \
    MCALTDS_DEC_HS_MAC_HS_HDR_QID_NUM_BITS + \
    MCALTDS_DEC_HS_MAC_HS_HDR_TSN_NUM_BITS) + \
   ((MCALTDS_DEC_HS_MAC_HS_HDR_SID_NUM_BITS + \
     MCALTDS_DEC_HS_MAC_HS_HDR_N_MACD_PDU_NUM_BITS + \
     MCALTDS_DEC_HS_MAC_HS_HDR_F_FLAG_NUM_BITS) * \
    MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT))

/* Number of word32 required to extract MAC-hs fields of maximum length */
/*lint -emacro(506,TDSDEC_HS_DOB_W32_READ_FOR_MAC_HS_HEADER_DECODE) Max number of W32 needed to read
  MAC-HS header is dependent on various field lengths of the header */ 
#define MCALTDS_DEC_HS_DOB_W32_READ_FOR_MAC_HS_HEADER_DECODE \
  ((MCALTDS_DEC_HS_MAC_HS_HDR_LEN_MAX & 0x1F) ? \
   ((MCALTDS_DEC_HS_MAC_HS_HDR_LEN_MAX >> 5) + 1) : \
   (MCALTDS_DEC_HS_MAC_HS_HDR_LEN_MAX >> 5))
    
/* Macro to extract MAC-hs fields */  
#define MCALTDS_DEC_HS_MAC_HDR_GET_FIELD(buf,offset_ptr,field) \
  EXTRACT_BITS_FROM_BUF(buf,offset_ptr, MCALTDS_DEC_HS_MAC_HS_HDR_##field##_NUM_BITS)

#define MCALTDS_DEC_HS_MAX_LOG_CHAN 16

/* Spec 25.321 Max possible Reordering PDUs per HS TrBlk */
#define MCALTDS_DEC_HS_REORDER_PDU_MAX_COUNT 3

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
/* Number of bits in various fields of MAC-ehs header */   
#define MCALTDS_DEC_HS_MAC_EHS_HDR_LC_ID_NUM_BITS         4 /* Logical Channel ID */
#define MCALTDS_DEC_HS_MAC_EHS_HDR_LEN_NUM_BITS          11 /* Length of reordering SDU */
#define MCALTDS_DEC_HS_MAC_EHS_HDR_TSN_NUM_BITS           6 /* Transmission seq number for reordering PDU */
#define MCALTDS_DEC_HS_MAC_EHS_HDR_SI_NUM_BITS            2 /* Segmentation indicator for reordering PDU */
#define MCALTDS_DEC_HS_MAC_EHS_HDR_F_FLAG_NUM_BITS        1 /* Final flag bit */

/* Maximum possible size of MAC-hs header */   
#define MCALTDS_DEC_HS_MAC_EHS_HDR_LEN_MAX \
  ((MCALTDS_DEC_HS_MAC_EHS_HDR_LC_ID_NUM_BITS + \
    MCALTDS_DEC_HS_MAC_EHS_HDR_LEN_NUM_BITS + \
    MCALTDS_DEC_HS_MAC_EHS_HDR_TSN_NUM_BITS + \
    MCALTDS_DEC_HS_MAC_EHS_HDR_SI_NUM_BITS + \
     MCALTDS_DEC_HS_MAC_EHS_HDR_F_FLAG_NUM_BITS) * \
    MCALTDS_DEC_HS_REORDER_SDU_MAX_COUNT)

/* Number of word32 required to extract MAC-ehs fields of maximum length */
/*lint -emacro(506,TDSDEC_HS_DOB_W32_READ_FOR_MAC_HS_HEADER_DECODE) Max number of W32 needed to read
  MAC-HS header is dependent on various field lengths of the header */ 
#define MCALTDS_DEC_HS_DOB_W32_READ_FOR_MAC_EHS_HEADER_DECODE \
  ((MCALTDS_DEC_HS_MAC_EHS_HDR_LEN_MAX & 0x1F) ? \
   ((MCALTDS_DEC_HS_MAC_EHS_HDR_LEN_MAX >> 5) + 1) : \
   (MCALTDS_DEC_HS_MAC_EHS_HDR_LEN_MAX >> 5))

/* Macro to extract MAC-ehs fields */  
#define MCALTDS_DEC_HS_MAC_EHS_HDR_GET_FIELD(buf,offset_ptr,field) \
  EXTRACT_BITS_FROM_BUF(buf,offset_ptr, MCALTDS_DEC_HS_MAC_EHS_HDR_##field##_NUM_BITS)

#endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */

#define MCALTDS_DEC_UPDATE_SRC_DESC(addr, len) \
  mcaltds_dec_src_descriptor_move_out[mcaltds_dec_global->mcaltds_dec_index_src_desc] = (addr); \
  mcaltds_dec_src_descriptor_move_out[mcaltds_dec_global->mcaltds_dec_index_src_desc + 1] = (len); \
  mcaltds_dec_global->mcaltds_dec_index_src_desc += 2;

#define MCALTDS_DEC_UPDATE_DST_DESC(addr, len) \
  mcaltds_dec_dst_descriptor_move_out[mcaltds_dec_global->mcaltds_dec_index_dst_desc] = (addr); \
  mcaltds_dec_dst_descriptor_move_out[mcaltds_dec_global->mcaltds_dec_index_dst_desc + 1] = (len); \
  mcaltds_dec_global->mcaltds_dec_index_dst_desc += 2;

/* Cache line size in bytes */
#define MCALTDS_DEC_CACHE_LINE_SIZE 32

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED

/* Number of log packet buffer for MAC hs header logging */
#define TDSHSCFG_TDSDECHS_NUM_MAC_HS_HDR_LOG_PKT_BUF 2

#endif

/* Indicate invalid HS info table */
#define MCALTDS_DEC_HS_INFO_TABLE_INVALID_VAL 0xFF



/* Deciphering algorithms */
#ifdef FEATURE_TDSCDMA_A2_HW_DL
#define MCALTDS_DEC_DECIPHER_ALGO_KASUMI 0
#define MCALTDS_DEC_DECIPHER_ALGO_SNOW3G 1
#else
#define MCALTDS_DEC_DECIPHER_ALGO_KASUMI 1
#define MCALTDS_DEC_DECIPHER_ALGO_SNOW3G 2
#endif



#define MCALTDS_DEC_INVALID_FW_TABLE_INDEX 255

#define MCALTDS_DEC_CCTRCH_INDEX_IS_VALID(cctrch_index) \
    ((cctrch_index>=0) && (cctrch_index<MCALTDS_DEC_TOTAL_CCTRCH_TABLES))
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */


/* This enum defines the endian-ness types of the DOB */
typedef enum
{
  /* litle endian mode used to read the word32 status data */
  MCALTDS_DEC_DOB_LITTLE_ENDIAN,
  /* big endian mode used for access the data octets as word32 reads */
  MCALTDS_DEC_DOB_BIG_ENDIAN
} mcaltds_dec_dob_endian_type_enum_type;

/* Type of data transfer mode */
typedef enum
{
  MCALTDS_DEC_DATA_XFER_MODE_BG_READ,
  MCALTDS_DEC_DATA_XFER_MODE_READ_W_IMME_TM_CIPHER
} mcaltds_dec_data_xfer_mode_eum_type;

/* Structure type declaration */
/* -------------------------- */



/* TF pool structure */
typedef struct
{
  /* PLEASE DO NOT CHANGE THE ORDER OF ELEMENTS
     IN THIS STRUCTURE. 
     The members are placed strategically for memory optimization */
  uint8  next;    /**< Link to the next node */
  uint8  num_tb;  /**< Number of transport block in TBS. */
  uint16 tb_size; /**< Transport block size. */
}mcaltds_dec_tf_pool_info_struct_type;

typedef struct
{
  uint8  num_tb;   /**< Number of transport block in TBS. */
  uint16 tb_size;  /**< Transport block size. */
} mcaltds_dec_cctrch_table_tf_info_struct_type;

typedef struct
{
  /*+yongh add this flag to FW determine whether TRCH is disable or enable.*/
  boolean trch_disable;
  /*+yongh add this flag to FW determine whether TRCH is disable or enable.*/
  
  /* Transport channel ID. Used only for logging */
  uint8 trch_id;

  /* Transport channel reference type */
  mcaltds_dec_trch_ref_type_enum_type trch_ref_type;
  
  /* Coding type of transport chanel */
  mcaltds_dec_coding_enum_type coding_type;

  /* CRC length in bits */
  uint8 crc_length;

  /* TTI duration of transport channel */
  tdsl1_tti_enum_type tti_value;

  /* DRMB size of TrCh in bits */
  uint16 drm_buffer_size;

  /* Loop back mode for this TrCh. Only Applicable if TrCh ref type is DCH */
  tdsl1_loop_back_mode_type_enum_type loopback_mode;

  /* Rate matching attribute. USed only for logging */
  uint16 rate_match_attrib;

  /* Number of TF in this TrCh */
  uint8 num_tf;

  /* TFT base in TF table */
  uint16 tft_base;

} mcaltds_dec_cctrch_table_trch_info_struct_type;

/* CCTrCh table information */
typedef struct
{
  /* CCTrCh table in use flag */
  boolean in_use;

  /* CCTrCh ID for which this index is being used */
  uint8 cctrch_id;
  
  /* CCTrCh Table index corresponding to the information contained here */
  uint8 table_index;

  /*Used to record table index from FW, it's value is 0, 1, 2.*/
  uint8 fw_table_index;
  
  /* CCTrCh reference type, indicate type of TrCh in that CCTrCh */
  mcaltds_dec_cctrch_ref_type_enum_type ref_type;

  /* DRM buffer segment index */
  uint8 drm_buf_area_id;

  /* Numner of TrCh in this CCTrCh */
  uint8 num_trch;

  /* TrCh info structure */
  mcaltds_dec_cctrch_table_trch_info_struct_type trch_info[MCALTDS_DEC_TOTAL_TRCHT_PER_CCTRCH];

  /* TrCH pool info base */
  uint8 trch_info_base;

  /* TF info structure */
  /* Total number of TF in this CCTrCh */
  uint8 num_tf;

  /* Index of the TF pool for each TF in CCTrCh */
  uint8 tf_pool_index[MCALTDS_DEC_TOTAL_TFT_PER_CCTRCH];

  /* used to record tf start addr in TF pool*/
  uint8 tf_info_base;

  /* indicate number of TFC in CTFC table */
  uint8 num_tfc;

  /* Head for the TFCI pool. The TFCI pool can be traversed from this index till index+num_tfc */
  uint16 tfci_pool_head;

  /* used to record tfc start addr in TFCI pool*/
  uint16 tfc_info_base;
} mcaltds_dec_cctrch_table_info_struct_type;

/* This structure defines code block parameters for a transport format. */
typedef struct
{
  /* Code block size after code block segmentation. */
  uint16 code_blk_size;
  
  /* Number of code blocks after the code block segmentation */
  uint16 num_code_blks;
  
  /* Number filler bits or padded bits after the code block segmentation. 
    These bits are always 0 and are added at the begining of code block
    or first segment. */
  uint16 num_filler_bits;
} mcaltds_dec_code_blk_param_struct_type;

  

/* This structure defines rate matching parameters that are required for
   de-rate marching. */
typedef struct
{
  /* Rate matched size per radio frame */
  uint16 rate_matched_size_per_radio_frame;
  
  /* De rate matching scale factor */
  uint16 drm_scale_factor;
  
  /* Optimized eini value for rate matching parameters */
  uint32 eini1[8];
  uint32 eini2[8];

  /* Optimized eplus/a value for rate matching parameters */
  uint32 eplus_over_a;
  
  /* Optimized eminus/a value for rate matching parameters */
  uint32 eminus_over_a;
  
  /* Indicates it the delta N per TTI is ODD or not */
  boolean delta_n_tti_odd;
  
  /* rate matching type used */
  tdsl1_rm_type_enum_type rm_type;
} mcaltds_dec_rm_param_struct_type;

/* This enum defines the TrCh types that the TBSet info represents */
typedef enum
{
  MCALTDS_DEC_TBSET_TRCH_TYPE_R99,
  MCALTDS_DEC_TBSET_TRCH_TYPE_HS,
  MCALTDS_DEC_TBSET_TRCH_TYPE_EHS
} mcaltds_dec_tbset_trch_type;



/** @name Macros for DOB status FIFO member access
 *  Now HS and non HS type depending on HSDPA feature defined.
 *  Following macros help them in easy access */
/**@{*/

#ifdef __GNUC__

 #define MCALTDS_DEC_DOB_STATUS_FIFO(dob_status_fifo,member) \
  dob_status_fifo.tbset_info.non_hs.member
 #define MCALTDS_DEC_DOB_STATUS_FIFO_PTR(dob_status_fifo,member) \
  dob_status_fifo->tbset_info.non_hs.member

#else /* __GNUC__ */

 #define MCALTDS_DEC_DOB_STATUS_FIFO(dob_status_fifo,member) \
  dob_status_fifo.tbset_info.non_hs.##member
 #define MCALTDS_DEC_DOB_STATUS_FIFO_PTR(dob_status_fifo,member) \
  dob_status_fifo->tbset_info.non_hs.##member

#endif /* __GNUC__ */

/**@}*/
#if 0
/* The data structure for storing decoded TFCI info as mirrored from mDSP */
typedef struct
{
  /* Decoded CFN */
  uint16 decoded_cfn;

  /* Max energies 1 and 2 */
  uint16 max_energy1;
  uint16 max_energy2;

  /* The decoded TFCI */
  uint16 decoded_tfci;
}mcaltds_dec_decoded_tfci_info_struct_type;
#endif

/* temporarily added by Owen */
//#define TFW_NON_HS_DECOB_STATUS_FIFO_SIZE   128
//#define TFW_HS_DECOB_STATUS_FIFO_SIZE       8

/* mcaltds_deci global data structure */
typedef struct
{
/* This array stores the status fifo information. This information is refered
   to by all other function to get the starting address of the DOB. This
   information is also used by header and data reading function to get the
   transport format Id to get the transport block size and number of transport
   blocks */
   mcaltds_dec_dob_status_fifo_struct_type mcaltds_dec_dob_status_fifo[MCALTDS_DEC_MAX_STATUS_FIFO_COUNT];

/* Q for DOB status FIFO */
   q_type mcaltds_dec_dob_status_fifo_free_q;

   uint32 mcaltds_dec_dl_hw_task_q_wr_ptr;

   mcaltds_dec_cctrch_table_info_struct_type mcaltds_dec_cctrch_info[MCALTDS_DEC_TOTAL_CCTRCH_TABLES];

   mcaltds_dec_tf_pool_info_struct_type mcaltds_dec_tf_pool[TDSL1_DL_MAX_TF];  

/* Indicate if decoder related HSDPA information is valid or not */
   boolean mcaltds_dec_hs_info_valid[MCALTDS_DEC_HS_INFO_COUNT];

/* Decoder related HSDPA information */
   mcaltds_dec_hs_info_struct_type mcaltds_dec_hs_info[MCALTDS_DEC_HS_INFO_COUNT];

} mcaltds_deci_global_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

extern mcaltds_deci_global_struct_type *mcaltds_deci_global;

/* This array stores the code block parameters of all TF of all TrCh in the
   CCTrCh under setup*/
//extern mcaltds_dec_code_blk_param_struct_type *mcaltds_dec_code_blk_param[MCALTDS_DEC_TOTAL_TRCHT_PER_CCTRCH];



#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT
/* This structure defines the HS transport block information passed to
   MAC and L2 during Data Read Event handling. */
//typedef  tdsl1_dec_hs_tsn_info_type mcaltds_dec_tsn_info_type;

//typedef tdsl1_dec_iovec_type mcaltds_dec_iovec_type;
#endif

#if 0
/** This structure defines the transport block header information for a
 * transport block */
typedef struct
{
  /** TB Id. This is set and maintained by MAC. Its passed back to MAC in
   * data indication */
  uint16  tb_id;
  
  /** Valid flag for this TB. If MAC finds any problem with this TB
   * it will set this TB valid flag to FALSE otherwise it will be TRUE */
  boolean valid;
  
  /** TB index of this TB */
  uint8 tb_idx; 
  
#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT
  /** Logical channel ID for MAC to fill up in Rel5-Rel6 case,
   *  irrespective of C/T mux. */
  uint8 lc_id;

  /** RLC Id corresponding to lc_id - to be filled by MAC 
   * This is needed to pass to RLC because RLC doesnt have notion of
   * lc_id. It only understands rlc_id */
  uint8 rlc_id;
#endif /* FEATURE_TDSCDMA_DL_DATA_PATH_OPT */

  /** Bit offset from the starting transport block. The offset specified in
   * in the number of bits. The L2 function that decode the header, reports
   * back the offset to read the transport block data in this strcuture member */
  uint8 num_bit_offset_trblk;
  
  /** ciphering parameters for this transport block */
  mcaltds_dec_ciphering_param_struct_type ciphering_param;
  
  /** Pointer to the transport block header */
  uint8 tb_header[MCALTDS_DEC_TB_HDR_SIZE_W32 * 4];
} mcaltds_dec_tb_hdr_struct_type;
#endif

//typedef  tdsl1_dl_tb_hdr_struct_type mcaltds_dec_tb_hdr_struct_type;

//typedef tdsl1_dec_mac_hs_tti_type mcaltds_dec_mac_hs_tti_type;

/* This array stores the turbo parameters of all TF of all TrCh in the CCTrCh.
   undet setup */
//extern mcaltds_dec_turbo_param_struct_type *mcaltds_dec_turbo_params[MCALTDS_DEC_TOTAL_TRCHT_PER_CCTRCH];


/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================
FUNCTION mcaltds_dec_do_data_transfer

DESCRIPTION
  This function does the data transfer of all blocks in header indication and
  read the data from DOB for them.
  
  It supports 2 modes of data transfer.
  1. Background data transfer: For this block must be unciphered or if ciphered
     deciphering must be done already. TM modeciphered blocks are not handled
     in this mode
  2. TM mode ciphered blocks are handled in tis mode. Blocks are deciphered
     immediately block by block before reading data out of DOB.

DEPENDENCIES
  Use following variable set outside the function
  dec_num_tbset_done_for_bg_data_xfer: Starts checking data transfer from this
    value.
  dec_num_tb_done_for_bg_data_xfer: Start checking for data transfer for this
    block in TBset
  dec_num_bits_being_deciphered: If deciphering is not in progress then
    it transfers atleast  this much amount before giving up control for
    deciphering done status check.

RETURN VALUE
  None

SIDE EFFECTS
  Set flag dec_data_xfer_waiting_for_decipher_done if encouters block with
  ciphering but not yet deciphered. Background data transfer stops with this
  condition
  Updates variables dec_num_tbset_done_for_bg_data_xfer and
  dec_num_tb_done_for_bg_data_xfer
==========================================================================*/

extern LOCAL void mcaltds_dec_do_data_transfer(
  /* pointer to the TB header decode info strcuture */
  mcaltds_dec_tb_hdr_decode_struct_type *tb_hdr_decode_info,
  /* Ptr to array of the DOB handles for these TB sets Hdr info */
  mcaltds_dec_dob_status_handle_type *dob_handles,
  /* Data transfer mode type */
  mcaltds_dec_data_xfer_mode_eum_type data_xfer_mode,
  /* Ptr to structure in which to store data */
  mcaltds_dec_tb_data_struct_type *dl_tb_data_info);

/*===========================================================================
FUNCTION tdsdec_hs_get_next_bg_xfer_offset

DESCRIPTION
  This function gets following parameter for current MAC-d PDU to transfer it
  out of DOB
  - DOB starting address where MAC-d SDU starts
  - Start bit position with start address
  - Length of MAC-d SDU to be transferred
  
  
DEPENDENCIES
  Init function mst have been called at the start of HS block before using
  this function.

RETURN VALUE
  None

SIDE EFFECTS
  All above mentioned variables in descriptions are modified
===========================================================================*/

extern boolean tdsdec_hs_get_next_bg_xfer_offset(
  /* DOB status FIFO information structure pointer */
  mcaltds_dec_dob_status_fifo_struct_type *mdsp_status_fifo,
  /* TB set header information structure pointer */
  mcaltds_dec_tb_set_hdr_struct_type    *tbset_hdr_info,
  /* MAC-d PDU block information structure pointer */
  mcaltds_dec_tb_hdr_struct_type *tb_hdr_info);

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
/*===========================================================================
FUNCTION mcaltds_dec_proc_log_submit

DESCRIPTION
  This function is called in response to the log service request by 
  the decoder module. It checks for the state of the log packets and if
  waiting to be submitted, it calls the appropriate routines to submit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/

extern void mcaltds_dec_proc_log_submit(void);
#endif

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
/*===========================================================================
FUNCTION        mcaltds_dec_submit_trch_cfg_logpkt

FILE NAME       

DESCRIPTION     

DEPENDENCIES                

RETURN VALUE   

SIDE EFFECTS   
 
===========================================================================*/
void mcaltds_dec_submit_trch_cfg_logpkt(      
      uint8 num_cctrch,
      /** pointer to CCTrCH info structure array  */
      mcaltds_dec_cctrch_info_struct_type cctrch_info[] );

/*===========================================================================
FUNCTION mcaltds_dec_log_trch_type_transform

DESCRIPTION
  This function set the trch type to log pkt

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
uint8 mcaltds_dec_log_trch_type_transform( mcaltds_dec_trch_ref_type_enum_type ref_type);

/*===========================================================================
FUNCTION mcaltds_dec_log_data_decode_done_result

DESCRIPTION
  This function updates the read index for HS in FW/SW shared memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
void mcaltds_dec_log_data_decode_done_result(mcaltds_dec_dob_status_fifo_struct_type * dob_status_fifo_ptr,uint8 mdsp_read_fifo_index);
#endif


#endif /* MCALWCDMA_DECI_H */

