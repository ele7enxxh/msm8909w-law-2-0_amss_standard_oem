#ifndef MCALTDS_DEC_H
#define MCALTDS_DEC_H

/*============================================================================*/
/** @file
 * This module has definition and declaration related to MCAL TDSCDMA decoder
 * module APIs
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/mcaltds_dec.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/29/12    owen    Change FEATURE_HSDPA_MAC_EHS to FEATURE_TDSCDMA_HSDPA_MAC_EHS
12/02/10    cyx     Remove tdsmvsif.h to clean up l2 interface
11/09/10    ow      Make changes in order to comapatible with MCAL


===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsuecomdef.h"
#include "dsm.h"
#include "rex.h"
#include "tdsl1std.h"
#include "tdsl1sapcommon.h"
#include "tfw_const.h"

#ifdef FEATURE_TDSCDMA_DAL_INT_CONTROLLER
#include "DALSys.h"
#include "DDIInterruptController.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#endif

#include "tfw_sw_intf_msg.h"

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
#include "tdsdllog.h"
#include "tdshslog.h"
#endif


/* -------------------------------------- */
/* PUBLIC API section of mcaltds_dec.h  */
/* -------------------------------------- */



/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/** Macro to translate CRC enum to its absolute value */
#define MCALTDS_DEC_CRC_LENGTH_VAL(crc_enum) mcaltds_dec_crc_length_val_table[(crc_enum)]

/** Macro to translate CRC enum to its absolute value */
#define MCALTDS_DEC_TRCH_REF_TYPE_BF_VAL(trch_ref_type) mcaltds_dec_trch_ref_type_bf_val_table[(trch_ref_type)]

/** Macro to translate CRC enum to its absolute value */
#define MCALTDS_DEC_MAX_FW_TABLE_INDEX 3


/** Number of decoder related HSDPA information */
#define MCALTDS_DEC_HS_INFO_COUNT 2

/** Number of MAC-d PDU size information per queue */
//#define MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT 8
#define MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT TDSL1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT

/** Number of MAC-hs queue for a HSDPA information table */
#define MCALTDS_DEC_HS_MAC_HS_Q_MAX_COUNT 8

/* The final number for this depends on what kind of network configurations we end up
 * seeing in practice. Being very conservative, I would not expect more than 1 Q for SRB and
 * max 3 for data (1 I/B, 1 streaming and 1 conv bearer). Further, I would not expect all of
 * these to be configured with max window - and finally chances that they all stall at the max
 * window at the same time is very small. For that reason I would budget for 32 x 4 elements,
 * instead of 32 x 16, which I still consider very conservative.
 */
#define MCALTDS_DEC_MAX_AVG_HS_Q 4

/* TSN window size used at MAC */
#define MCALTDS_DEC_TSN_WIN_SIZE 32

#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT
/* 3GPP 25.321 Spec allows 70 MAC-d PDUs per MAC-hs transport block */
//#define MCALTDS_DEC_MAX_DL_RLC_PDU 70
#define MCALTDS_DEC_MAX_DL_RLC_PDU TDSL1_DEC_MAX_DL_RLC_PDU

/* Maximum LC ID per Rel-7 Spec 25.321 */
//#define MCALTDS_DEC_MAX_LC_ID 16
#define MCALTDS_DEC_MAX_LC_ID  TDSL1_DEC_MAX_LC_ID
#endif /* FEATURE_TDSCDMA_DL_DATA_PATH_OPT */


#define MCALTDS_DEC_MAX_HS_FR_PER_10MS_TTI TDSL1_DEC_MAX_HS_FR_PER_10MS_TTI

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS

#define MCALTDS_DEC_INVALID_Q_ID 8

/* Spec 25.321 Max possible Reordering SDUs per HS TrBlk */
//#define MCALTDS_DEC_HS_REORDER_SDU_MAX_COUNT 26
#define MCALTDS_DEC_HS_REORDER_SDU_MAX_COUNT TDSL1_DEC_HS_REORDER_SDU_MAX_COUNT

/* Macro for MAC to use to obtain DOB extension buffer size.
 * This defines the number of bytes available in DecOB for
 * concatenating the partial PDUs to perform deciphering.
 * Max possible RLC PDU size is 1504 bytes. For MIMO scenario,
 * there can be 10 * 1504 bytes every 10ms to concatenate, which
 * is equal to 15040 bytes. Set 16000 bytes to be on safer side.
 * Do not exceed 16000 bytes since copy engine has limitation of
 * 8K bytes per transfer and we are programming two copy
 * repeat tasks. */
#define MCALTDS_DEC_DOB_EXTN_BUF_SIZE 16000

/* The maximum number of reordering pdu in one transport block, currently 3 per
 * 3GPP 25.321 Mac spec. */
//#define MCALTDS_DEC_MAX_REORD_PDU_PER_HS_FRAME 3
#define MCALTDS_DEC_MAX_REORD_PDU_PER_HS_FRAME TDSL1_DEC_MAX_REORD_PDU_PER_HS_FRAME

/* The maximum number of partial pdu per reordering pdu, currently 2 per
 * 3GPP 25.321 Mac spec. */
//#define MCALTDS_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU 2
#define MCALTDS_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU TDSL1_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU

/* We can have 2 complete pdus for each reordering queue in the previous TTI,
 * total = 3 * 2 * 5 for Non-MIMO
 * total = 3 * 2 * 10 for MIMO */
/*#define MCALTDS_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI \
            (MCALTDS_DEC_MAX_REORD_PDU_PER_HS_FRAME * \
             MCALTDS_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU * \
             MCALTDS_DEC_MAX_HS_FR_PER_10MS_TTI) */

#define MCALTDS_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI TDSL1_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI

/** This macro indicates the total number of TSNs for which
 * complete PDUs were formed from previous TTIs partial PDUs. L1
 * retrieves a buffer from the Free Q, populates it and enqueues
 * into a Q to MAC. This macro is triple-buffered since
 * if MAC-hs task does not get a chance to run for three 10ms
 * frames, even then the information can be held in the
 * queue. */
#define MCALTDS_DEC_TOTAL_MAX_COMPLETE_PDU_FROM_PREV_TTI (MCALTDS_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI*3)

#define MCALTDS_DEC_MAX_TSN_INFO_BUF ((MCALTDS_DEC_TSN_WIN_SIZE * MCALTDS_DEC_MAX_AVG_HS_Q) + \
                                         MCALTDS_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI)


/* 3GPP 25.321 Spec allows 26 RLC pdus per MAC-ehs transport block */
#define MCALWCDMA_DEC_MAX_DL_RLC_PDU_REL7 26

/** Based on MAX IP Pkt size of 1500 bytes and RLC PDU size
 *  of 40 bytes, N/W can split up a PDU into 38 partial PDUs.
 *  MAC sends concatenation information to L1 once every
 *  10ms, so the number of maximum partial PDUs across all
 *  reordering PDUs can be computed based on the the max number
 *  of reordering PDU per HS TTI and the number of HS TTI per
 *  10ms. */
#define MCALTDS_DEC_MAX_RLC_PDU_SEGMENTATION 38
#define MCALTDS_DEC_MAX_RLC_PDU_SEGMENTATION_PER_10MS_TTI \
                  (MCALTDS_DEC_MAX_RLC_PDU_SEGMENTATION * \
                   MCALTDS_DEC_MAX_REORD_PDU_PER_HS_TTI * \
                   MCALTDS_DEC_MAX_HS_FR_PER_10MS_TTI)

/** For Scatter/Gather Data Mover command, there would be
 *  multiple source and destination descriptors. Each
 *  descriptor is made of:
 *  Word0 - Address Word (Absolute Address in memory)
 *  Word1 - Bit 31:16 Reserved
 *          Bit 15:0  Length in bytes to move */
/** There are 2 words per descriptor and there can be x number
 *  of segments per PDU. WR_PTR is also part of this
 *  descriptor list. */
/* Only for partial pdus. Worst case we have to move 38 blocks to form a
 * complete pdu, which could happen for all three queues at the same time and
 * across multiple HS TTIs, or total = (38 * 3 * 5) + 1 descriptor for the WR_PTR
 * address. */
#define MCALTDS_DEC_MAX_DMOV_SOURCE_DESCRIPTOR_LEN_PARTIAL \
              ((MCALTDS_DEC_MAX_RLC_PDU_SEGMENTATION * \
                MCALTDS_DEC_MAX_REORD_PDU_PER_HS_TTI * \
                MCALTDS_DEC_MAX_HS_FR_PER_10MS_TTI) + 1)

//#define MCALTDS_DEC_MAX_DMOV_SOURCE_DESCRIPTOR_LEN_MOVE_IN 100
#define MCALTDS_DEC_MAX_DMOV_SOURCE_DESCRIPTOR_LEN_MOVE_IN TDSL1_DEC_MAX_DMOV_SOURCE_DESCRIPTOR_LEN_MOVE_IN

/** Since partial PDUs will be concatenated in the DOB
 *  extension buffer, there is only one destination address
 *  that needs to be specified + 1 descriptor for the WR_PTR
 *  address. */
//#define MCALTDS_DEC_MAX_DMOV_DESTINATION_DESCRIPTOR_LEN_MOVE_IN 4
#define MCALTDS_DEC_MAX_DMOV_DESTINATION_DESCRIPTOR_LEN_MOVE_IN TDSL1_DEC_MAX_DMOV_DESTINATION_DESCRIPTOR_LEN_MOVE_IN

#else /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */

#define MCALTDS_DEC_MAX_TSN_INFO_BUF (MCALTDS_DEC_TSN_WIN_SIZE * MCALTDS_DEC_MAX_AVG_HS_Q)

#endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */


/* Total number of CCTrCh table in MCAL DEC module.
** If support multi cctrch, when cctrch is recfg,
** this value should be double numbei of cctrch.*/
#define MCALTDS_DEC_TOTAL_CCTRCH_TABLES 6
#define MCALTDS_DEC_FW_SCCPCH_TABLE_INDEX 0
#define MCALTDS_DEC_FW_DPCH_TABLE_INDEX 1
/* MCALTDS_DEC_TOTAL_CCTRCH_TABLES - 4 (BCH) */
#define MCALTDS_DEC_TOTAL_CCTRCH_TABLES_NON_BCH 13

/** Indicate invalid MCAL WCDMA decoder CCTrCh Id value */
#define MCALTDS_DEC_INVALID_CCTRCH_ID_VAL 0xFF

/** Maximum number of DOB handles */
#define MCALTDS_DEC_MAX_DOB_HANDLES 128

/** Number of W32 to be read in transport block header information */
//#define MCALTDS_DEC_TB_HDR_SIZE_W32 3
#define MCALTDS_DEC_TB_HDR_SIZE_W32 TDSL1_DEC_TB_HDR_SIZE_W32
/** Number of word 32 data reads required for HS-DSCH MAC-d PDU */
#define MCALTDS_DEC_HS_MAC_D_HDR_SIZE_W32 1

/** This is the maximum number of transport block set for which the transport
 * block headers can be reported by L1 to L2 for decoding
 * Note that size defined is based on the maximum amout that can be
 * reported by mDSP. SW has to process that and should have that much capacity.
 * This is not as per UE capability specified in standard */
//#define MCALTDS_DEC_MAX_TB_SET_HDR 32
//#else
//#define MCALTDS_DEC_MAX_TB_SET_HDR 16
//#endif
#define MCALTDS_DEC_MAX_TB_SET_HDR TDSL1_DEC_MAX_TB_SET_HDR

/* Total number of TrCh table in mDSP */
#define MCALTDS_DEC_TOTAL_TRCHT_PER_CCTRCH 8

/** 32 R99 Max TB ina frame for all TTI ending at this frame (No double buffering)
 * Max 70 PDU per sub frame Number of sub frames to cover are 5 (in 10 ms) +
 * 2 (To cover interrupt latency
 * Total transport block (MAC-d PDUs) will be 32 + 7*70 = 524 */
#define MCALTDS_DEC_MAX_TB 524

/** @name DOB status field valid bit mask indicator */
/**@{*/
/** Indicate CRC valid */
#define MCALTDS_DEC_DOB_STATUS_CRC_RESULT_VALID_BMSK 0x01
/** Indicate path energy matric valid */
#define MCALTDS_DEC_DOB_STATUS_EGY_MATRIC_VALID_BMSK 0x02
/** Indicate symbol error valid */
#define MCALTDS_DEC_DOB_STATUS_SYM_ERR_VALID_BMSK    0x04
/** Indicate quality bits valid */
#define MCALTDS_DEC_DOB_STATUS_QUAL_BITS_VALID_BMSK  0x08
/**@}*/

/** TBset header strctuture member access macro. It takes care of any
 * featurization and provide access to appropriate member based on
 * featurization */
#ifdef __GNUC__

#define MCALTDS_DEC_TB_SET_HDR(hdr_struct_ptr,member) \
  hdr_struct_ptr->info.non_hs.member

#else /* __GNUC__ */

#define MCALTDS_DEC_TB_SET_HDR(hdr_struct_ptr,member) \
  hdr_struct_ptr->info.non_hs.##member

#endif /* __GNUC__ */

/** Number of TPC reject logged per TPC index */
#define MCALTDS_DEC_LOG_MAX_TPC_INFO 6

/** Number of bytes in cipheting key stream */
#define MCALTDS_DEC_NUM_BYTES_CIPHER_KEY 16 /* 128 bit key */


/* Macro for clearing deciphering done interrupt flag */
#define MCALTDS_DEC_CLEAR_DECIPHER_DONE_INT() \
 HWIO_OUT(BACKUP_INT_CLEAR, HWIO_FMSK(BACKUP_INT_CLEAR,DECIPH_DONE_INT_CLR));

#define MCALTDS_DEC_QUERY_DECIPHER_DONE_INT_STATUS() \
 (HWIO_IN(BACKUP_INT_STATUS) & HWIO_FMSK(BACKUP_INT_STATUS,DECIPH_DONE_INT_STATUS))

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/** MCAL WCDMA decoder CCTrCh id type that is returned for CCrCh setup request */
typedef uint8 mcaltds_dec_cctrch_id;

/** MCAL WCDMA decoder DOB status handle type definition */
typedef uint32 mcaltds_dec_dob_status_handle_type;

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */
typedef enum
{
  TDSL1_DL_CCTRCH_SETUP,
  TDSL1_DL_CCTRCH_UPDATE,
  TDSL1_DL_CCTRCH_NONE_OPERATION
} mcaltds_dec_cctrch_operation_enum_type;

#if 0
/** This enum defines the coding type of the TrCh in a CCTrCh. */
typedef enum
{
  TDSL1_DL_CONV_1_BY_2_CODING, /**< Convolutional coding with rate 1/2 */
  TDSL1_DL_CONV_1_BY_3_CODING, /**< Convolutional coding with rate 1/3 */
  TDSL1_DL_TURBO_CODING,       /**< Turbo coding. Rate is always 1/3 */
  TDSL1_DL_UNCODED             /**< Uncoded data */
} mcaltds_dec_coding_enum_type;
#endif

typedef tdsl1_dl_cctrchcfg_coding_enum_type mcaltds_dec_coding_enum_type;


#if 0
/** This enum defines the reference type of the TrCh */
typedef enum
{
  TDSL1_DL_TRCH_SBCH_TYPE,    /**< Serving cell BCH transport channel type */
  TDSL1_DL_TRCH_NBCH_TYPE,    /**< Neighbor cell BCH transport channel type */
  TDSL1_DL_TRCH_DCH_TYPE,     /**< Dedicated channel transport channel type */
  TDSL1_DL_TRCH_DSCH_TYPE,    /**< Should never be used, only for MAC/RRC compatibility */
  TDSL1_DL_TRCH_PCH_TYPE,     /**< Paging channel transport channel type */
  TDSL1_DL_TRCH_FACH_TYPE,    /**< Fast access chanel transport channel type */
  TDSL1_DL_TRCH_HS_DSCH_TYPE, /**< HSDPA channel transport channel type */
#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
  TDSL1_DL_TRCH_EHS_DSCH_TYPE, /**< Enhanced HSDPA channel transport channel type */
#endif
  TDSL1_DL_NUM_TRCH_REF_TYPE, /**< Total number of transport channels types */
  TDSL1_DL_TRCH_INVALID       /* Needed by tdsrrcllcoc.c */
} mcaltds_dec_trch_ref_type_enum_type;
#endif

typedef tdsl1_dl_trch_ref_type_enum_type mcaltds_dec_trch_ref_type_enum_type;

typedef uecomdef_umts_cipher_algo_e_type mcaltds_dec_ciphering_algo_enum_type;


#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
/* L1 will interpret the segmentation index type and then assign a segmentation type
to each MAC-ehs PDU within the reordering pdu.

OTA_Reordering_Segmentation_Type  MAC-ehs_segmentation_type

01                            The first mac-ehs sdu will be of type partial end and the remaining
                              mac-ehs pdus will be having complete pdu as segmentation type
10                            The last mac-ehs sdu will be of type partial start and the remaining
                              mac-ehs pdus will be having complete pdu as segmentation type
11                            If there is only 1 mac-ehs sdu in the reordering pdu then it will be of
                              type partial middle, start or end. If there is 2 or more then
                              the first one will be partial end and the last one will be partial start
00                            All mac-ehs sdus are complete pdus

*/
#if 0
typedef enum
{
  TDS_COMPLETE_PDU,
  TDS_PARTIAL_FIRST,
  TDS_PARTIAL_MIDDLE,
  TDS_PARTIAL_LAST
} mcaltds_dec_rlc_pdu_segmentation_type;
#endif
#endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */
typedef tdsl1_dec_rlc_pdu_segmentation_type mcaltds_dec_rlc_pdu_segmentation_type;

/* call back function pointer type for header read and data read */
typedef void (*mcal_header_data_reading_cb_fn)(void);

/*----------------------------------------------------------------------------
   Enum indicated by L1 to MAC along with HS data inidcation in order to make
   decision for processing the current, prev or both TTIs
 -----------------------------------------------------------------------------*/
/* HSDPA Action Variable */

#if 0
typedef enum
{
  TDS_MACDL_HS_CURR_TTI,
  TDS_MACDL_HS_PREV_TTI,
  TDS_MACDL_HS_CURR_PREV_TTI,
  TDS_MACDL_HS_NONE
} mcaltds_dec_mac_hs_tti_type;
#endif

typedef tdsl1_dl_hs_tti_enum_type mcaltds_dec_mac_hs_tti_type;

#if 0
/** Transparant mode deciphering states for various transport block */
typedef enum
{
  TDSL1_TB_TM_CIPHER_NOT_APPLICABLE, /**< Block not ciphering in TM mode */
  TDSL1_TB_TM_CIPHER_START,          /**< First block iciphered in TM mode */
  TDSL1_TB_TM_CIPHER_CONT,           /**< Continue ciphering key stream generation */
  TDSL1_TB_TM_CIPHER_STOP,           /**< Last block iciphered in TM mode */
  TDSL1_TB_TM_CIPHER_SINGLE          /**< Single block in TM mode so don't */
} mcaltds_dec_tb_tm_cipher_state_enum_type;
#endif

typedef tdsl1_tb_tm_cipher_state_enum_type mcaltds_dec_tb_tm_cipher_state_enum_type;

/* Structure type declaration */
/* -------------------------- */

/* Structures for CCTrCh setup informtion */

/* This enum defines the PDSCH spreading factor type. The enum values are
   same as in the RRC IEs and denotes the spreading factor values */
typedef enum
{
  TDSL1_DL_SF_1,
  TDSL1_DL_SF_2,
  TDSL1_DL_SF_4,     /* Spreading factor is 4 */
  TDSL1_DL_SF_8,     /* Spreading factor is 8 */
  TDSL1_DL_SF_16,    /* Spreading factor is 16 */
} mcaltds_dec_sf_enum_type;

#if 0
/** This structure defines transport format information of TF in a
 * transport channel  */
typedef struct
{
  uint8  num_tb;   /**< Number of transport block in TBS. */
  uint16 tb_size;  /**< Transport block size. */
} mcaltds_dec_tf_info_struct_type;
#endif

typedef tdsl1_dl_cctrchcfg_tf_info_struct_type mcaltds_dec_tf_info_struct_type;

#if 0
/** This structure defines the transport channel information */
typedef struct
{
  /** Transport channel reference type */
  mcaltds_dec_trch_ref_type_enum_type ref_type;

  /** Transport channel Id. This is for backward compatibility of structure
   * and used by WHL1. NOT USED IN MCAL SOFTWARE */
  uint8 trch_id;


  /** Rate matching attribute. */
  uint16 rate_match_attrib;

  /** Coding type of transport chanel */
  mcaltds_dec_coding_enum_type coding_type;

  /** CRC length (enum or number of bits). */
  tdsl1_crc_length_enum_type crc_length;

  /** TTI duration of transport channel */
  tdsl1_tti_enum_type tti_value;

  /** DCH BLER Quality Value. If not valid or not available, set it to -127.
   * Valid range -63 to 0. For value x real BLER quality is 10^(x/10).
   * This is for backward compatibility of structure and used by WHL1.
   * NOT USED IN MCAL SOFTWARE */
  int8 bler_quality_val;

  /** Loop back mode for this TrCh. Only Applicable if TrCh ref type is DCH */
  tdsl1_loop_back_mode_type_enum_type loopback_mode;

  /* If set to TRUE, this flag indicates that this TrCh is mapped to
     Non-TM bearer.  Else if FALSE it is mapped to TM bearers.
     This flag is only applicable for TDSL1_LOOPBACK_MODE_1 */
  boolean                       non_tm_rb;

  /** Number of transport format. */
  uint8 num_tf;

  /** Pointer to the array of transport format. */
  mcaltds_dec_tf_info_struct_type *tf_info_array[TDSL1_DL_MAX_TF];
} mcaltds_dec_trch_info_struct_type;
#endif

typedef tdsl1_dl_cctrchcfg_trch_info_struct_type mcaltds_dec_trch_info_struct_type;

/** This structure defines CCTrCh information parameters for each transport
 * channel in it and all transport format of each transport channel */
#if 0
typedef struct
{
  /** CCTrCh Id for this CCTrCh information. This is for backward compatibility
   * of structure and used by WHL1. NOT USED IN MCAL SOFTWARE */
  uint8 cctrch_id;

  /** TrCh ref type bit field indicating the TrCh ref type present in this CCTrCh */
  uint8 trch_ref_type_bf;

  /** TrCh ref type enable bit field indicating the TrCh ref type data will be
   * passed up to MAC. If the bit for a given TrCh reference type is 0 TBset is
   * dropped after it is received. This is for backward compatibility of
   * structure and used by WHL1. NOT USED IN MCAL SOFTWARE */
  uint8 trch_enable_bf;

  /** Number of transport channel. */
  uint8 num_trch;

  /** Pointer to the TrCh info array. */
  mcaltds_dec_trch_info_struct_type *trch_info_array[TDSL1_DL_MAX_TRCH];
} mcaltds_dec_cctrch_info_struct_type;
#endif
typedef tdsl1_dl_cctrchcfg_cctrch_info_struct_type mcaltds_dec_cctrch_info_struct_type;


#if 0
/** This structure defines the CTFC or TFC entry information members */
typedef struct
{
  /** Is this TFC is valid or not */
  boolean valid_ctfc;

  /** pointer to transport format index of each TrCh in CCTrCh array */
  uint8 *tfi_table;
} mcaltds_dec_ctfc_entry_info_struct_type;
#endif

typedef tdsl1_dl_cctrchcfg_ctfc_entry_info_struct_type  mcaltds_dec_ctfc_entry_info_struct_type;

#if 0
/** This structure defines transport format confinition set information */
typedef struct
{
  /*Record the number of ctfc entry.*/
  uint16 num_ctfc_entry;

  /** Pointer to the array of TFC information */
  mcaltds_dec_ctfc_entry_info_struct_type *ctfc_entry_array[TDSL1_DL_MAX_TFC];
} mcaltds_dec_ctfc_info_struct_type;
#endif

typedef tdsl1_dl_cctrchcfg_ctfc_info_struct_type mcaltds_dec_ctfc_info_struct_type;

/** Strcuture for DOB status information */
typedef struct
{
  /** Valid bit mask for various information below. */
  uint8 valid_bit_mask;

  /** Coding type of transport chanel */
  mcaltds_dec_coding_enum_type coding_type;

  boolean zsb;          /**< zero state bit */
  uint16 energy_metric; /**< energy metric of the TB set */
  uint16 sym_err_count; /**< symbol error count for TB set */
  /** CRC error bits of each TB. MSB is last block received in this TBset */
  uint32 crc_bits;
  uint16 quality_bits;  /**< quality bits for TB set */
} mcaltds_dec_dob_status_type;


/** This structure defines information for HSDPA MAC-d PDU size */
typedef struct
{
  /** indicate this Size index Id is valid */
  boolean valid;
  /** MAC-d PDU size for this SID */
  uint16 size;
} mcaltds_dec_hs_mac_d_pdu_sz_info_struct_type;

/** This structure defines information for HSDPA Q information for its MAC-d PDI size */
typedef struct
{
  /** indicate this Q Id is valid */
  boolean valid;

  /** indicate Size info for each SID (Size identifier)
   * SID goes with index of this array */
  mcaltds_dec_hs_mac_d_pdu_sz_info_struct_type
  mac_d_pdu_sz_info[MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT];

} mcaltds_dec_hs_mac_d_pdu_sz_q_info_struct_type;

/** This structure defines the information for HSDPA information table for
 * getting MAC-d PDU out of MAC-HS block */
typedef struct
{
  /** MAC-d queue information for this HS table */
  mcaltds_dec_hs_mac_d_pdu_sz_q_info_struct_type
  mac_d_pdu_sz_q_info[MCALTDS_DEC_HS_MAC_HS_Q_MAX_COUNT];
} mcaltds_dec_hs_info_struct_type;


/* Structures for Header indication to L2 */

#if 0
/** This structure defines the ciphering parameters for a transport block
 * header information of a transport block */
typedef struct
{
  /** Ciphering enabled or not for this
   * transport block. */
  boolean ciphering_control;
  /** Ciphering algorithm specified by L2 for this
   * transport block. */
  mcaltds_dec_ciphering_algo_enum_type ciphering_algo;

  /** Ciphering bit offset returned by L2 of each transport block. */
  uint8 ciphering_bit_offset;

  /** Ciphering key id returned by L2 of this transport block. */
  uint8 ciphering_key_id;

  /** Ciphering count (CFN/SFN) returned by L2 of this transport block. */
  uint32 ciphering_count;

  /** Ciphering radio bearer id returned by L2 for this transport block. */
  uint8 ciphering_radio_bearerid;
} mcaltds_dec_ciphering_param_struct_type;
#endif

typedef tdsl1_dl_ciphering_param_struct_type mcaltds_dec_ciphering_param_struct_type;

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

typedef tdsl1_dl_tb_hdr_struct_type mcaltds_dec_tb_hdr_struct_type;

#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT

#if 0
/* IOVEC definiton:
 * An IOVEC is a data structure that contains pointer to a DSM item,
 * offset within the DSM item in bytes and the length of the PDU
 * in bytes.
 */
typedef struct
{
  dsm_item_type *ptr; /* Pointer to DSM item */
  uint16 offset; /* Offset in bytes from the beginning of data
                    section of the DSM item */
  uint16 length; /* Length in bytes of the PDU */
} mcaltds_dec_iovec_type;
#endif
typedef tdsl1_dec_iovec_type mcaltds_dec_iovec_type;

/* Each complete pdu will reside in one dsm item. The dsm item is large enough to hold
multiple complete pdus. rlc_hdr_data is the demulitplexed header portion from the rlc pdu
these are separate so that the data can be contiguous and can be shipped to higher layers
avoiding a dsm copy and a new dsm alloc */

#if 0
typedef struct
{
  mcaltds_dec_iovec_type iovec; /* IOVEC for this complete PDU */

  uint32 rlc_hdr_data; /* RLC Header contents */

  /* For rel-7 LC_ID is known to L1 while doing the header read event. For rel-6 the MAC will
  pass the LC_ID information at the end of the header read event. DSM items are allocated
  per lc_id basis hence L1 needs to be aware of the lc_id for rel6 too.
  This is NW configured Logical Channel ID <1..15> and not internal RLC ID */
  uint8 rlc_id; /* Logical Channel ID */
} tds_complete_pdu_type;

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
typedef struct
{
  mcaltds_dec_iovec_type iovec; /* IOVEC for this partial PDU */

  /* L1 uses this variable to indicate to MAC whether
     RLC PDU header is received completely or not. */
  boolean complete_rlc_hdr_rcvd;

  uint32 rlc_hdr_data; /* RLC Header contents */

  /* This indicates if the demultiplexed PDU is a partial first, middle or last*/
  mcaltds_dec_rlc_pdu_segmentation_type segmentation_info; /* Segmentation indication */

  uint8 lc_id; /* Logical Channel ID to which this pdu belongs*/
} tds_partial_pdu_type;
#endif
#endif

#if 0
/** Enum type definition for TSN trace enum type */
typedef enum
{
  TDSDL_TSN_FREE_Q = 1, /**< TSN is in Dec TSN info free Q */
  TDSDL_TSN_L1_INT_Q,   /**< TSN is in L1 Internal Q */
  TDSDL_TSN_INFO_Q,     /**< TSN is in Dec TSN info Q */
  TDSDL_TSN_MAC_INT,    /**< TSN is in MAC internal structures */
  TDSDL_TSN_RLC_Q,      /**< TSN is in RLC TSN info Q */
  TDSDL_TSN_RLC         /**< TSN is in RLC */
} mcaltds_dec_tsn_trace_enum_type;
#endif
typedef tdsl1_dec_tsn_trace_enum_type mcaltds_dec_tsn_trace_enum_type;

/** If a TSN structure passed to MAC has both complete
 *  and partial pdu info, then RLC should not release the
 *  structure. MAC will have to free this structure when the
 *  partial PDUs are taken care of. */
#if 0
typedef struct
{
  q_link_type link; /**< Link to manage this item on the queue */
  uint8 index;      /**< index of this element in array */

  /* Indicates which module is holding the TSN items */
  mcaltds_dec_tsn_trace_enum_type tracer;

  /** UE can receive Complete PDU, Complete PDU and Partial Start
   *  of another PDU in that order. But this structure
   *  organization destroys the original order. Seems ok from MAC
   *  point of view as of now, the order is not important. */

  /* TSN decoded as part of the Header Read event */
  uint8 tsn;

  /* Queue ID decoded as part of the Header Read event */
  uint8 queue_id;

  /** TB Id. This is set by MAC during Header read event. Its
   * passed back to MAC in data indication */
  uint16 tb_id;

  /** Flag for MAC usage */
  boolean received;

  /* While freeing this structure (either MAC/RLC), set this field to 0.
   * L1 will set them to zero also, during Q init. */
  uint8 num_complete_pdus;

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
  /* While freeing this structure (either MAC/RLC), set this field to 0.
   * L1 will set them to zero also, during Q init. */
  uint8 num_partial_pdus;
#endif

  tds_complete_pdu_type TDS_COMPLETE_PDU[MCALTDS_DEC_MAX_DL_RLC_PDU];

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
  /*UE_MAX_PARTIAL_PDU_PER_REORDER_PDU=2 since in a reordering pdu either the
  first mac-ehs sdu could be a partial one and/or the last mac-ehs sdu in the
  reordering pdu could be a partial one*/
  tds_partial_pdu_type partial_pdu[MCALTDS_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU];
#endif
} mcaltds_dec_tsn_info_type;
#endif
typedef tdsl1_dec_tsn_info_type mcaltds_dec_tsn_info_type;

/** This structure defines the DSM information per logical
 *  channel. */
typedef struct
{
  /** DSM item being populated */
  dsm_item_type *current_dsm_ptr;

  /** Offset in the DSM ptr where the next TB data can be
   *  populated (units of bytes). */
  uint16 offset;
} mcaltds_dec_dsm_info_type;

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
#if 0
/* This structure identifies the number of partial PDU segements which forms
a complete PDU. These segments will belong to different TSN's and resides in the
reassembly Q in MAC. Once MAC finds that a complete PDU can be formed, this
is populated and given as part of the header read event along with
deciphering information for complete PDUs already residing in the DOB*/

/** This structure provides the location of various segments of
 *   this RLC PDU and also contains deciphering information
 *   from RLC, if necessary. */
typedef struct
{
  /* For rel-7 LC_ID is known to L1 while doing the header read event. For rel-6 the MAC will
  pass the LC_ID information at the end of the header read event. DSM items are allocated
  per lc_id basis hence L1 needs to be aware of the lc_id for rel6 too*/
  uint8 lc_id;

  /** RLC Id corresponding to lc_id - to be filled by MAC
   * This is needed to pass to RLC because RLC doesnt have notion of
   * lc_id. It only understands rlc_id */
  uint8 rlc_id;

  /* Number of Partial PDUs that complete this RLC PDU */
  uint16 num_partial_pdus;
  /* Length in bytes of the complete RLC PDU. Populated
   * by MAC. This is required by L1 in order to prepare
   * the move-out task. If this field is not present,
   * L1 has to again loop through all the source descriptor
   * list entries and compute the length of each complete PDU */
  uint16 length;
  /* TSN info structure corresponding to first segment of the
   * RLC PDU. */
  mcaltds_dec_tsn_info_type *tsn_info_buf;

  /** ciphering parameters for this RLC PDU */
  mcaltds_dec_ciphering_param_struct_type ciphering_param;
} mcaltds_dec_partial_rlc_pdu_hdr_struct_type;
#endif

typedef tdsl1_dl_partial_rlc_pdu_hdr_struct_type mcaltds_dec_partial_rlc_pdu_hdr_struct_type;

#if 0
/** This structure defines the information for all RLC PDUs that can
 *  be constructed out of multiple partial PDUs. This structure
 *  also contains the information required to get the data
 *  from the hardware. */
typedef struct
{
  /** Number of complete RLC PDUs that can be formed */
  uint16 num_complete_pdus;

  /* for each complete RLC PDU that is being formed */
  mcaltds_dec_partial_rlc_pdu_hdr_struct_type rlc_pdu_hdr_info[MCALTDS_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI];
} mcaltds_dec_rlc_pdu_concatenate_info_struct_type;
#endif
typedef tdsl1_dec_rlc_pdu_concatenate_info_struct_type mcaltds_dec_rlc_pdu_concatenate_info_struct_type;

#if 0
typedef struct
{
  /* For rel-7 LC_ID is known to L1 while doing the header read event. For rel-6 the MAC will
  pass the LC_ID information at the end of the header read event. DSM items are allocated
  per lc_id basis hence L1 needs to be awar of the lc_id for rel6 too*/
  uint8 lc_id;        /* Logical Channel ID */
  uint8 tsn;          /* Transmission seq number 6 bits */
  uint16 length;      /* Length of MAC e-HS SDU in bytes*/
  mcaltds_dec_rlc_pdu_segmentation_type segmentation_info; /* Complete or Partial PDU */
} mcaltds_dec_mac_ehs_sdu_hdr_type;
#endif
typedef  tdsl1_dec_mac_ehs_sdu_hdr_type mcaltds_dec_mac_ehs_sdu_hdr_type;

#if 0
/** Every PDU formed out of partial PDUs from previous TTIs
 *  will have a corresponding TSN provided by MAC. The list
 *  of TSNs corresponding to each complete PDU will be queued
 *  to MAC as part of a queue separate from the queue for
 *  passing current TTIs TSN info structures. */
typedef struct
{
  q_link_type link; /**< Link to manage this item on the queue */
  uint8 index;      /**< index of this element in array */

  /** TSN corresponding to first segment of a complete PDU */
  mcaltds_dec_tsn_info_type *tsn_info_buf;
} mcaltds_dec_concatenated_pdu_tsn_type;
#endif

typedef tdsl1_dec_concatenated_pdu_tsn_type mcaltds_dec_concatenated_pdu_tsn_type;

extern mcaltds_dec_dsm_info_type mcaltds_dec_partial_pdu_dsm_info[MCALTDS_DEC_MAX_LC_ID];

/** This array stores the TSNs for which complete PDUs were
 * formed from previous TTIs partial PDUs. L1 retrieves a buffer
 * from the Free Q, populates it and enqueues into a Q to MAC.
 * This data structure is triple-buffered since if MAC-hs
 * task does not get a chance to run for three 10ms frames, even
 * then the information can be held in the queue. */
extern mcaltds_dec_concatenated_pdu_tsn_type
mcaltds_dec_concatenated_pdu_tsn_info[MCALTDS_DEC_TOTAL_MAX_COMPLETE_PDU_FROM_PREV_TTI];

/* Q of TSN value structures */
/* L1 pulls structures out of Free Q, fills it and enqueues them in Q */
/* MAC uses the structures in Q and puts them back in Free Q */
extern q_type mcaltds_dec_concatenated_pdu_tsn_q;
extern q_type mcaltds_dec_concatenated_pdu_tsn_free_q;

#endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */

#endif /* FEATURE_TDSCDMA_DL_DATA_PATH_OPT */


/** This structure defines the transport block header information for a
 * transport block set (or a transport channel) */
#if 0
typedef struct
{
  /** Number of transport blocks in this transport block set. */
  uint8 num_tb;

  /** CCTrCh Ids for the transport block set. This is set by WHL1 */
  uint8 cctrch_id;

  /** TrCh reference type for the transport block set */
  mcaltds_dec_trch_ref_type_enum_type trch_ref_type;


  /** HS and non-HS channel information */
  union
  {
    /** R99 non-HS channel information */
    struct
    {
      uint8 cfn; /**< CFN for this transport block set */
      uint8 trch_id; /**< TrCh Ids for the transport block set. Set by WHL1 */
      uint8 tfi; /**< TF index for this TBset or TrCh */
      uint8 num_total_tb; /**< Total number of TB in the TF of this TBset */
      uint16 tb_num_bits; /* Number of bits in this TB */
    } non_hs;

    /** HS channel information */
    struct
    {
      uint16 sub_frame_num; /**< Starting Sub frame number when TTI is received */
      uint8 version_flag; /**< version flag 1 bit */
      uint8 queue_id; /**< Queue Id 3 bits */
      uint8 tsn; /**< Transmission seq number 6 bits */
      uint8 num_sid_in_hdr; /**< k of SIDk,Nk pair in MAC HS header up to 8 */
      /** Nk value for each k */
      uint8 num_blk[MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT];
      /** SID value for each k */
      uint8 sid_val[MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT];
    } hs;

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
    /** E-HS channel information */
    struct
    {
      uint16 sub_frame_num; /* Starting Sub frame number when TTI is received */
      uint8 num_reorder_sdu; /* Number of Reordering SDUs in this HS TTI */
      /* LC-ID, Length, TSN and Segmentation Type of each MAC-eHS SDU in this HS TTI */
      mcaltds_dec_mac_ehs_sdu_hdr_type mac_ehs_sdu_hdr[MCALTDS_DEC_HS_REORDER_SDU_MAX_COUNT];
    } ehs;
#endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */
  } info;


  /** indicate that TBs in this TB set are in transparent mode */
  boolean tb_in_tm_mode;
  /** Pointer to the structure array of information of all
    transport block header in this TB set */
  mcaltds_dec_tb_hdr_struct_type *tb_hdr_info;
} mcaltds_dec_tb_set_hdr_struct_type;
#endif

typedef tdsl1_dl_tb_set_hdr_struct_type mcaltds_dec_tb_set_hdr_struct_type;

#if 0
/** This structure defines the information of all transport block headers
 * information for all transport block sets for which the headers need to be
 * decoded and information is required to get the data from the hardware. */
typedef struct
{
  /** Number of transport block set for which the TrBlk header is to be
   * decoded for all transport blocks in them */
  uint8 num_tb_set;

  /** transport block set information for all the transport block headers
   * in each transport block set */
  mcaltds_dec_tb_set_hdr_struct_type tb_set_hdr[MCALTDS_DEC_MAX_TB_SET_HDR];

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
  /** Information for all RLC PDUs that can be constructed out of
   *  multiple partial PDUs. */
  mcaltds_dec_rlc_pdu_concatenate_info_struct_type rlc_pdu_concatenate_info;
#endif
} mcaltds_dec_tb_hdr_decode_struct_type;
#endif

typedef tdsl1_dl_tb_hdr_decode_struct_type mcaltds_dec_tb_hdr_decode_struct_type;


/* Structures for Data indication to L2 */

#if 0
/** This structure defines the data element for a transport block in a
 * transport block set */
typedef struct
{
  /** TB Id. This is set and maintained by MAC */
  uint16  tb_id;

  /** RLC PDU size of the data in this TB in bits. In case of loopback mode 2
   * for the DCH TrCh the data size will include the CRC length also */
  uint16 data_size;

  /** data buffer size is bytes used to accomodate the RLC PDU bits */
  uint16 buffer_size_bytes;

  /** DSM item to store payload */
  dsm_item_type *tb_data_buffer;
} mcaltds_dec_tb_data_in_tbset_struct_type;
#endif
typedef  tdsl1_dl_tb_data_in_tbset_struct_type mcaltds_dec_tb_data_in_tbset_struct_type;

#if 0
/** This structure defines the data and status element for a transport block set */
typedef struct
{
  /** TrCh reference type for the transport block set */
  mcaltds_dec_trch_ref_type_enum_type trch_ref_type;

  /** Number of transport blocks in this transport block set. */
  uint8 num_tb;

  /** transport block size for the transport block of this trblk set */
  uint16 trblk_size;

  /** points to the array of the TB info structure */
  mcaltds_dec_tb_data_in_tbset_struct_type *tb_data_info;
} mcaltds_dec_tb_set_data_struct_type;
#endif

typedef tdsl1_dl_tb_set_data_struct_type mcaltds_dec_tb_set_data_struct_type;


#if 0
/** This structure defines the transport block information that is used to get
 * the transport block headers decoded and update the transport block
 * information to the L2 buffers */
typedef struct
{
  /** Number of transport block set for which the data and status is to be
     transferred to L2.
     If the feature FEATURE_TDSCDMA_DL_DATA_PATH_OPT is defined, field is valid
     only for R99 data. If only HS data is present, it will be set to zero.*/
  uint8 num_tb_set;

  /** Data and status for all transport blocks sets */
  mcaltds_dec_tb_set_data_struct_type tb_set_data[MCALTDS_DEC_MAX_TB_SET_HDR];
} mcaltds_dec_tb_data_struct_type;
#endif

typedef tdsl1_dl_tb_data_struct_type mcaltds_dec_tb_data_struct_type;

#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT

/* This array stores the TSN information.
 * L1 retrieves a buffer from the Free Q, populates it and enqueues into a Q to MAC.
 * The contents of this buffer will not be copied locally in MAC or RLC,
 * All data in this buffer is reused by MAC & RLC */

extern mcaltds_dec_tsn_info_type mcaltds_dec_tsn_info_buf[MCALTDS_DEC_MAX_TSN_INFO_BUF];

/* Q of TSN info structures */
/* L1 pulls structures out of Free Q, fills it and enqueues them in Q */
/* MAC/RLC use the structures in Q and puts them back in Free Q */
extern q_type mcaltds_dec_tsn_info_q;
extern q_type mcaltds_dec_tsn_info_free_q;

#endif /* FEATURE_TDSCDMA_DL_DATA_PATH_OPT */

#if 0
/** Power control information structure */
typedef struct
{
  /** @name Variables for DPCCH SIR measurement
   * @{ */
  uint16 wt_nt_io;
  uint16 slot_pilot_energy;
  uint16 wt_pilot_amp;
  uint32 wt_traffic_amp;
  /** @} */

  /** Forward (DL) power control decision history */
  uint16 fpc_decision_hist;
  /** reverse (UL) decision history */
  uint16 rtpc_decision_history;
  /** indicate the FPC dicision valid for each slot */
  uint16 tpc_decision_valid;
  /** Number of rejected TPC bits per TPC combiner */
  uint16 num_reject_tpc[MCALTDS_DEC_LOG_MAX_TPC_INFO];
} mcaltds_dec_power_control_info_struct_type;
#endif
#ifdef FEATURE_DL_PATH_PROFILING
#if 0
typedef struct
{
  uint32 time_trigger;
  uint32 time_cb_return;
  uint32 num_src_des;
  uint32 num_dest_des;
  uint32 bits_transfer;
  uint32 num_p2c;
  uint32 total_len_p2c;
} tdsdec_dmov_cb_type;
#endif
typedef struct
{
  uint32 rxsfm;
  uint32 wtsfm;
  uint32 nonhs_rd_idx;
  uint32 nonhs_wr_idx;
  uint32 hs_rd_idx;
  uint32 hs_wr_idx;
  uint32 start_addr;
  uint32 hs_trch_type;
  uint32 fw_nonhs_counter;
  uint32 sw_nonhs_counter;
  uint32 fw_hs_counter;
  uint32 sw_hs_counter;
  uint32 timetick;
} mcaltds_dec_decode_done_isr_type;

typedef struct
{
  uint32 decode_start;
  uint32 decode_mid;
  uint32 decode_end;
} mcaltds_dec_decode_done_isr_start_end_type;

typedef struct
{
  uint32 trigger_ts;
  uint32 cb_ts;
  uint32 idx;
  uint8 num_ptoc;
} mcaltds_dec_gather_and_decipher_struct_type;

typedef struct
{
  uint32 num_non_hs_dob_handles;
  uint32 num_hs_dob_handles;
  uint32 num_partial;
  uint32 num_complete;
  uint32 num_p_to_c;
  uint32 num_bytes_p_to_c;
  uint32 hdr_rd_start;
  uint32 hdr_rd_step2;
  uint32 hdr_rd_end;
  uint32 l2_start;
  uint32 l2_end;
  uint32 p2_start;
  uint32 p2_end;
  uint32 cur_tbset_proc_start;
  uint32 prev_tbset_proc_start;
  uint32 dmov_trig_start;
} mcaltds_dec_hdr_rd_isr_struct_type;

typedef struct
{
  uint32 cfn;
  uint32 cx1;
  uint32 timetick;
  uint32 cause;
  uint8 this_tick;
  uint8 scheduled_tick;
  uint8 accum_dob_count;
} mcaltds_dec_skip_hdr_struct_type;

typedef struct
{
  uint32 cfn;
  uint32 cx1;
  uint32 timetick;
  uint8 skipped;
  uint8 this_tick;
  uint8 scheduled_tick;
} mcaltds_dec_data_rd_struct_type;

/*
extern mcaltds_dec_hdr_rd_isr_struct_type tdsl1dec_hdr_rd_isr[];
extern mcaltds_dec_skip_hdr_struct_type tdsdec_skip_hdr[];
extern mcaltds_dec_data_rd_struct_type tdsdec_data_rd[];

extern uint32 tdsl1dec_hdr_rd_idx;
extern uint32 tdsdec_skip_hdr_idx;
extern uint32 tdsdec_data_rd_idx;
*/
#endif

/* DRM buffer storage locations are their parameters */
/* Number of DRM segments for R4 type CCTrCh
 * index 0 is only used for BCH channel.
 * index 1 is used for FACH or PCH.
 * index 1 ~ index 3 are used for DCH with multi cctrch.
 */
#define MCALTDS_DEC_CCTRCH_R4_TYPE_NUM_DRM_INFO 4


/* Number of DRM segments for HS type CCTrCh */
#define MCALTDS_DEC_CCTRCH_HS_TYPE_NUM_DRM_INFO 1

/* Number of areas in which DRM buffer is divided */
#define MCALTDS_DEC_NUM_DRM_BUF_AREA  \
   (MCALTDS_DEC_CCTRCH_R4_TYPE_NUM_DRM_INFO + \
    MCALTDS_DEC_CCTRCH_HS_TYPE_NUM_DRM_INFO)

/* Maximum items in the CTFC pool */
#define MCALTDS_MAX_NUM_TFCI_POOL   128


/* CCTrCh reference type enum type */
typedef enum
{
  MCALTDS_DEC_CCTRCH_REF_TYPE_BCH,      /* CCTrCh with BCH type TrCh */
  MCALTDS_DEC_CCTRCH_REF_TYPE_PCH_FACH, /* CCTrCh with PCH and/or FACH type TrCh */
  MCALTDS_DEC_CCTRCH_REF_TYPE_DCH,      /* CCTrCh with DCH type TrCh */
  MCALTDS_DEC_CCTRCH_REF_TYPE_INVALID   /* Used when can't generate CCTrCh ref type */
} mcaltds_dec_cctrch_ref_type_enum_type;


/* This structure defines turbo parameters that are required by
   turbo decoder. */
typedef struct
{
  /* Interleaver length. This is derived directly from the code block size. */
  uint16 interleaver_length;

  /* Number of maximum iteration. The possible values are 6 to 8 depending
    on the code rate. */
  uint8 max_iterations;

  /* Row index for the interleaving pattern. The possible values are B00 to
    B11 depending number of rows. This value indicates the pattern index
    used in inter-row permutations */
  uint8 row_index;

  /* Number of columns. The possible values are 5 to 257 depending number of
    rows. The value in this variable is one less than the actual number
    of columns. */
  uint16 num_columns;

  /* Row and column product. The product is between the actual number of
    column and rows and not as in variable row_index and num_columns */
  uint16 row_column_product;

  /* Prime number that is used to decide the number of column value. The
    possible values of the prime number  are 7 to 257. */
  uint16 prime_num;

  /* Prime number index corresponding to the prime number. The prime number
    index starts from 0 for prime number 7, 1 for 11 and so on. */
  uint8 prime_num_index;

  /* Last column used by last code block data in turbo interleaver table. */
  uint16 last_column;

  /* Last row used by last code block data in turbo interleaver table. */
  uint8 last_row;

  /* Last row bit exchange condition for the intra and inter row
    permutations. This is set to true if the code block size is equal to
    product of number of rows and column and number of column is one less
    than prime number. */
  boolean last_row_bits_exchange;
} mcaltds_dec_turbo_param_struct_type;

/* DRM Buffer information structure */
typedef struct
{
  /* Availability status */
  boolean available;

  /* DRM buffer storage direction */
  uint8 drm_buf_dir;

  /* DRM buffer start address */
  uint16 drm_buffer_start_addr;

  /* DRM buffer size */
  uint16 drm_buf_size;

  /* DRM buffer size in bits */
  uint16 drm_buf_size_bits;

  /* DRMB ID to CCTrCh mapping */
  uint8 cctrch_idx;

  /* CcTrCh reference type that is using DRM buffer area */
  mcaltds_dec_cctrch_ref_type_enum_type cctrch_ref;
} mcaltds_dec_drm_buffer_info_struct_type;


typedef struct
{
  /* This array stores the turbo parameters of all TF of all TrCh in the CCTrCh.
     undet setup */
  mcaltds_dec_turbo_param_struct_type *turbo_params[TDSL1_DL_MAX_TRCH];

  /* -----------------------------------------------------------------------
       ** Local Data Declarations
  ** ----------------------------------------------------------------------- */

  /* SW-FW Interface structures, mcaltds_decs.non_hs_intf */
  tfw_nonHS_dob_status_fifo_t *non_hs_intf;

  /* DRM buffer information, mcaltds_decs.drm_buffer_info */
  mcaltds_dec_drm_buffer_info_struct_type
  drm_buffer_info[MCALTDS_DEC_NUM_DRM_BUF_AREA];

  /* CCTrCh headers for all CCTrCh, mcaltds_dec_header */
  //mcaltds_dec_header_info_struct_type dec_header[MCALTDS_DEC_TOTAL_CCTRCH_TABLES];

  /* Free CCTrCh index table and num entries in the table */
//LOCAL uint8 mcaltds_dec_free_cctrch_index_table[MCALTDS_DEC_TOTAL_CCTRCH_TABLES];
//LOCAL uint8 mcaltds_dec_num_free_cctrch;

  /* CCTrCh setup related variables */
  /**********************************/

  /* TFCI Information : This variables stores if the TFCI is valid or not */
  /* boolean mcaltds_decs.rm_db_valid_ctfc[MCALTDS_DEC_TOTAL_CCTRCH_TABLES][TDSDEC_TOTAL_CTFC_ENTRIES_PER_TABLE]; */
  boolean rm_db_valid_ctfc[MCALTDS_DEC_TOTAL_CCTRCH_TABLES][TDSL1_DL_MAX_TFC];

  /* TFCI pool */
  uint16 tfci_pool[MCALTDS_MAX_NUM_TFCI_POOL];
  uint16 first_free_tfci_pool_index;
  uint16 last_free_tfci_pool_index;
  uint16 num_tfci_in_use;

  /* TF pool related variables */
  uint8 first_free_tf_pool_index;
  uint8 last_free_tf_pool_index;
  uint16 num_tf_in_use;

  /* Pool Direction Flag */
  uint8 trch_info_pool_ping_pong_flag;
  uint8 tf_info_pool_ping_pong_flag;
  uint8 tfc_info_pool_ping_pong_flag;

  /* Trch Update Bit Mask. Valid Bit[0..7] with each bit for each TrCH in TrCH info pool.
   Bit[0] for TrCH 0, Bit[1] for TrCh 1, and etc. Valid value [0,1], 0: no update, 1: update.
   Note that update will be effective immediately */
  uint8 update_bit_mask;

  /* TRCH Decode Enable or Disable Control. Valid [0,1], 0: disable, 1: enable */
  uint8 enable_bit_mask;

  /* TURBO Enable or Disable */
  boolean turbo_on;

} mcaltds_dec_struct_type;

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED

typedef struct _mcaltds_decdata_global_var_type
{
  /*decode done evt handler update*/
  volatile uint16 nohs_logpkt_wt_idx;
  /*for l1m task submit logpkt use */
  volatile uint16 nohs_logpkt_rd_idx;

  /*for task read use, and reset to 0*/
  volatile uint16 nohs_logpkt_tobe_rd_cnt; /*unfinished read count*/

} mcaltds_decdata_global_var_type;

#endif

/** This structure defines the decoder output buffer status FIFO entry structure.
 * It has all the information that mDSP status FIFO has. In addition to that it
 * has got information about the transport block that is collected from the TF
 * information using the mDSP TF Id */
typedef struct
{
  q_link_type link; /**< Link to manage this item on the queue */
  uint16 index;      /**< index of this element in array */

  boolean valid;               /**< information is valid or not              */

  uint32 trch_base_addr;

  uint16 trch_start_addr;      /**< Transport channel start address in DOB   */


  boolean hs_trch_type; /**< Indicate if TBset is of HS type or not */

  /** This is HS or non -HS info based on hs_trch_type */
  union
  {
    /** non-HS (R99) TBset information */
    struct
    {
      /* use 32 bit to record wall time counter.
       * chipX8cnt: 16 bits, 0-15 (LSB)
       * subFrmCnt: 13 bits, 16-28
       */
      uint32  stmr_wall_time_cnt;
      boolean tfci_decode_pass; /**< TFCI decode pass indicator */
      boolean sb_detected;       /**<special burst detected indicator>*/
      boolean puncturedInd;     /**<brief Partial TTI Punctured Indication From Slot Suspenstion. 0: No partial puncture 1: Partial puncture>*/
#ifdef FEATURE_TDSCDMA_PCH_FET
      boolean fetInd;
#endif
#ifdef FEATURE_TDSCDMA_PCH_SIR_FET
      uint8	 fet_tti_sfm;	  /**<brief FET decode isr happend in 10ms(1), 15ms(2), 20ms(3)>*/
#endif
#ifdef FEATURE_TDSCDMA_LVA_DECODE
      /*! \brief Lva indication. 1: LVA can be ued for this TRCH. 0: LVA is not used */
      boolean  lva_ind;
      /*! \brief Indication: Lva decode done and write back to Dob*/
      boolean  lva_wbk_ind;

#endif

      boolean deSensed;           /**< brief Partial TTI has been de-sensed. 0: No de-sensed 1: Partial de-sensed */
      boolean last_trch_ind; /**< Indication of last TrCH decode in a CCTrCH to use this config ID after a
                                               reconfiguration or disable. 1 : yes, 0: no.  */
      uint8 mdsp_cctrch_id;     /**< mDSP CCTrCh Id (mDSP CCTrCh table index) */
      uint8 mdsp_trch_id;       /**< mDSP TrCh Id (mDSP TrCh table index) */
      uint8 trch_code_type;     /**< mDSP CCTrCh Id (mDSP CCTrCh table index) */
      boolean mdsp_tfi_error;         /**< mDSP TF Id error indicator> */
      uint8 mdsp_tf_id;         /**< mDSP TF Id (mDSP TF table index), raw TF index relative within a cctrch */
      uint8 tf_id; /* redundant with the above. TF id relative for TrCH */
      uint8 cfn_value;          /**< CFN value reported in status FIFO */
      uint8 decoded_tfci;          /**< CFN value reported in status FIFO */
      uint8  num_tb;            /**< Number of transport block */
      uint16 tb_size_bits;      /**< Transport block size in number of bits */
      /** Word 32 align size of TrBlk. Hardware stores the data for TrBlks
       * start at uint32 align addr */
      uint16 tb_32bit_align_size_word32;
      /** Indicate don't check CRC pass/fail and get data for block of this TrCh */
      boolean override_crc;
      mcaltds_dec_dob_status_type dob_status; /**< DOB status information */
    } non_hs;

    /** HS TBset information */
    struct
    {
      uint8 hs_table_id;        /**< HSDPA table index Id */
      uint8 harq_id;            /**< HARQ process Id */
      boolean new_data;         /**< indicate new data bit state */
      boolean crc_pass;         /**< indicate PDSCH blocks CRC pass/fail status */
      uint16 global_sub_fn;     /**< HSDPA global sub frame number */
      uint16 hs_tb_size;        /**< HS transport block size */
      uint16 hs_hdr_size;       /**< MAC-hs header size */
      uint8  num_total_tb;      /**< Total block of all sizes */
      uint16 mac_d_pdu_sz[MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT]; /**< PDU size info */
      uint8 dob_fifo_idx;
    } hs;

  } tbset_info;

  /** This points to the array of CRC data bits */
  uint32 *crc_data_bits_ptr;
} mcaltds_dec_dob_status_fifo_struct_type;

#ifdef FEATURE_TDSCDMA_SW_F8_DECIPHER

typedef struct
{
  tdsl1_ciphering_algo_enum_type ciphering_algo;
  uint8 *key_ptr;
  uint8 dir;
  uint16 bearer_id;
  uint16 ciphering_offset;
  uint32 c_count;
  uint8 *data;
  int32 length;
} mcaltds_dec_sw_decipher_type;

#endif


#define FEATURE_TDSCDMA_HSDPA_DEBUG

/* number of word 32 in deciphering parameter FIFO entry */
#define MCALTDS_DEC_NUM_WORD32_DECIP_PARAM 3

#define MCALTDS_DEC_NUM_DECIPH_TASKS 128


/* This is a global structure containing all the global variables */
typedef struct
{
  /* global variable */
  mcaltds_dec_struct_type mcaltds_decs;

  /* CCTrCH Table index to be allocated 0 ~ 7. */
  uint8 current_cctrch_table_idx;

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
  tdsdllog_trch_cfg_pkt_type mcaltds_dec_trch_cfg_log_pkt;

  tdsdllog_tfc_cfg_pkt_type mcaltds_dec_tfc_cfg_log_pkt;
#endif

  /*Yongh define this datastructure to dump all of configuration command*/
  tfw_dl_ccTrCh_config_cmd_t mcaltds_cctrch_cfg_msg;

#ifdef FEATURE_TDSCDMA_HSDPA_DEBUG
  uint32 mcaltds_dec_hsDataRecvCnt;

  uint32 mcaltds_dec_hsDataCRCpassCnt;
#endif

  /* vaiable used by workaround for A2 stuck */
  uint8 mcaltds_dec_a2_waiting_cnt;

  uint8 mcaltds_dec_a2_stuck6times_cnt;

  /* Flag for initializing shared data queues */
  boolean mcaltds_dec_init_dec_shared_data_queue;


#ifdef FEATURE_TDSCDMA_A2_HW_DL
  /* Variables for P-to-C profiling */
  mcaltds_dec_gather_and_decipher_struct_type mcaltds_dec_gather_and_decipher[128];

  uint32 mcaltds_dec_gather_and_decipher_idx;
#endif /* FEATURE_TDSCDMA_A2_HW_DL */

#ifdef FEATURE_TDSCDMA_DAL_INT_CONTROLLER
  DalDeviceHandle *mcaltds_dec_f8_doneHandle;
#endif

  /* Any P-to-C operations to be done or not */
  boolean mcaltds_dec_concat_needed;

#ifdef FEATURE_TDSCDMA_A2_HW_DL
  uint32 mcaltds_dec_tdstotal_p_to_c_complete;
#endif

  uint32 mcaltds_dec_transfer_cnt;

  uint32 mcaltds_dec_total_num_p_to_c; /* num times P-to-C had to be formed */

  UWord32 mcaltds_dec_tfwInfoUpdateFlagCounter[TFW_INFO_UPDATE_FLAGS];

  uint8 mcaltds_hs_status_fifo_wr_idx;

  /* Indices for source and destination move-out descriptors */
  uint32 mcaltds_dec_index_src_desc;

  uint32 mcaltds_dec_index_dst_desc;

  /* Number of times DM transfer has been invoked since init */
  uint32 mcaltds_dec_cd_call_cnt;

  /* Debug var for DL datapath F3s
  	 Bit 0 => Decode Done ISR
  	 Bit 1 => Header Read ISR
  	 Bit 2 => P2/A2 programming - Current TTI path
  	 Bit 3 => P2/A2 programming - P to C path
  	 Bit 4 => P2/A2 programming - Deciphering
  	 Bit 5 => R99 path deciphering
  	 Bit 6 => DMOV/A2 Done ISR
  	 Bit 7 => Crash upon DSM exhaustion
   */
  uint16 mcaltds_dec_datapath_f3_debug;

  boolean mcaltds_dec_non_dtx_mode_en;

  uint16 mcaltds_dec_debug_tb_size;

  uint32 mcaltds_dec_debug_buf[6];

  uint32 mcaltds_dec_debug_tsn;

  uint32 mcaltds_dec_debug_reord_pdu_idx;

  /* Enable HS data transfer on WPLT as well. Retain this
     variable for some time so that it can be set
     through Trace if something goes wrong. */
  uint8 mcaltds_dec_skip_hs_data_transfer;

#ifdef FEATURE_TDSCDMA_PLT_MAC
  boolean mcaltds_dec_crash_if_invalid_pdu;
#endif

  uint16 mcaltds_dec_num_total_hs_deciph_task_prep;

  /* Source address in DOB for Data Mover operation */
  uint32 mcaltds_dec_ehs_dm_addr;

  /* DSM information about the accumulated complete PDUs */
  mcaltds_dec_dsm_info_type mcaltds_dec_compl_pdu_dsm_info[MCALTDS_DEC_MAX_LC_ID];

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
  tdshslog_decode_status_log_pkt_type mcaltds_dec_this_pdsch_logging;

  tdsdllog_dpch_pccpch_sccpch_decode_result_cmd_pkt_type mcaltds_dec_decode_result_log_pkt;

  uint8 mcaltds_dec_decode_done_cfn;

  uint8 mcaltds_dec_decode_done_dob_status_cnt;

#ifndef FEATURE_TDSCDMA_PCH_FET
  boolean mcaltds_pch_suspend_done_after_decoding_isr;
#endif

  /* struct defined in mcaltds_dec.h
   * define the global struct to wrap all the global var*/
  mcaltds_decdata_global_var_type mcaltds_dec_global_var;
#endif

#ifdef FEATURE_TDSCDMA_A2_HW_DL
  /* Global flag to indicate HS block has been received from F/W */
  boolean mcaltds_dec_hs_tb_rcvd;
#endif

#ifdef FEATURE_TDSCDMA_QTF_A2_SIM
  uint32 mcaltds_dec_sim_eram_dob_star_addr;
#endif

#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT

  /* boolean to indicate HS TBSet present */
  boolean mcaltds_dec_hs_tbset_rcvd;
  /* biggest tb size in bytes for each logical channel */
  uint16 mcaltds_dec_max_tb_size_bytes[MCALTDS_DEC_MAX_LC_ID];
  /* biggest p-to-c size in bytes for each logical channel */
  uint16 mcaltds_dec_max_tb_size_bytes_ptoc[MCALTDS_DEC_MAX_LC_ID];
#endif /* FEATURE_TDSCDMA_DL_DATA_PATH_OPT */

  /* Decoder header/date reading call back function */
  mcal_header_data_reading_cb_fn mcal_decdata_header_read_cb_func;

  mcal_header_data_reading_cb_fn mcal_decdata_data_read_cb_func;

  /* TBset decode done handling related variables */
  /************************************************/

  /* This indicates the result of last read mDSP DOB status FIFO. This is
     intended to indicate if the mDSP status FIFO structure in L1 was available
     to store the result when a valid mDSP status FIFO entry in mDSP was found.
     Applicable when tdsdec_last_read_mdsp_fifo_valid is TRUE */
  boolean mcaltds_dec_last_read_mdsp_fifo_result;
  /* This saves the mDSP fifo info structure array index of the last read
     status FIFO. Valid only when  tdsdec_last_read_mdsp_fifo_valid and
     mcaltds_dec_last_read_mdsp_fifo_result is TRUE */
  uint16 mcaltds_dec_last_read_fifo_info_idx;
  /* This saves the last read mDSP status FIFO info. This the copy of mDSP status
     FIFO info stored in SW info structure is it is available. This can be used to
     do some cleanup if NO storage space is available. Only primary mDSP status
     FIFO info is stored. These are what is given by mDSP for each status FIFO */
  mcaltds_dec_dob_status_fifo_struct_type mcaltds_dec_last_rece_mdsp_status_fifo_info;

  /* This variable indicate the number if DOB status FIFO received in last 10 ms.
     It is reset every TB Hdr read event. This is useful to see if there are too
     many DOB status FIFO received from mDSP and S/W may go out of DOB status
     FIFO buffers */
  uint16 mcaltds_dec_num_dob_status_fifo_rece;

  /* This array stores the status fifo information. This information is refered
     to by all other function to get the starting address of the DOB. This
     information is also used by header and data reading function to get the
     transport format Id to get the transport block size and number of transport
     blocks */
// mcaltds_dec_dob_status_fifo_struct_type mcaltds_dec_dob_status_fifo[MCALTDS_DEC_MAX_STATUS_FIFO_COUNT];

  /* Q for DOB status FIFO */
//q_type mcaltds_dec_dob_status_fifo_free_q;

  /* Decob config register shadow */
  uint32 mcaltds_dec_decob_cfg_reg_shadow;

#if defined(FEATURE_TDSCDMA_PLT)
  //rumi3_test_case_type mcaltds_dec_rumi3_test_case;
#endif

  /* HS DOB FIFO management variables */
  /* -------------------------------- */
  /* Latest read index after decode done ISR */
  uint16 mcaltds_dec_hs_saved_rd_fifo_idx;

  /* Start and end DOB indices for current TTIs header read */
  uint16 mcaltds_dec_hs_start_process_fifo_idx;

  uint16 mcaltds_dec_hs_end_process_fifo_idx;

  /* TBset header read related variables */
  /***************************************/

  /* This array stores the TrBlk header CRC data bits all the TrBlk that are received
     within TTIs of all TrChs that ends with in 10 ms interval */
  uint32 mcaltds_dec_tb_crc_data[MCALTDS_DEC_MAX_TB];

  /* TBset data read related variables */
  /*************************************/

  /* The array of the TB data information in the TB set */
  mcaltds_dec_tb_data_in_tbset_struct_type mcaltds_dec_dl_tb_data_in_tbset_info[MCALTDS_DEC_MAX_TB];

  /* Deciphering task trigger state machine variable */
  /* ----------------------------------------------- */

  /* Deciphering active state machine variables */
  boolean mcaltds_dec_decipher_in_progress;
  /* number of tasks active in current deciphering process */
  uint8 mcaltds_dec_decipher_num_tasks_active;
  /* Header position where deciphering tasks starts */
  uint8 mcaltds_dec_decipher_task_head;
  /* number of bits that are being deciphered */
  uint32 mcaltds_dec_num_bits_being_deciphered;
  /* Number of TBsets complete when this deciphering is complete */
  uint8 mcaltds_dec_decipher_num_tbset_will_be_done;
  /* number of TB complete in TBset for which deciphering may be in progress */
  uint8 mcaltds_dec_decipher_num_tb_will_be_done;

  /* Deciphering task prep state machine variable */
  /* -------------------------------------------- */

  /* number of actual deciphered bits till now. This must be reset at the
     begining of transport block data read function from where TM deciphering
     is done block by block */
  uint16 mcaltds_dec_num_tm_deciphered_bits;
  uint16 mcaltds_dec_num_tm_leftover_mask_bits;
  uint8 mcaltds_dec_next_tb_idx_to_decipher;

  /* number of transport block set (TrCh) scanned for deciphering */
  uint8 mcaltds_dec_num_tbset_scaned_for_deciph;
  /* number of TB scanned for deciphering in current TBset */
  uint8 mcaltds_dec_num_tb_scaned_for_deciph;
  /* next TB index to prepare for deciphering */
  uint8 mcaltds_dec_next_tb_idx_to_scan_for_deciph;
  /* Number of tasks prepared for deciphering */
  uint8 mcaltds_dec_num_deciph_task_prep;
  /* number if bits to decipher in all programmed tasks */
  uint32 mcaltds_dec_num_bits_deciph_prep;
  /* Header position where deciphering task prep starts */
  uint8 mcaltds_dec_decipher_task_tail;

  /* deciphering parameter FIFO entry buffer. Though not required but will
     provide visibility for debugging */
  uint32 mcaltds_dec_deciph_task_param_buf[MCALTDS_DEC_NUM_DECIPH_TASKS][MCALTDS_DEC_NUM_WORD32_DECIP_PARAM];


  /* Background data transfer state machine variables */
  /* ------------------------------------------------ */

  /* Indicate backgound data transfer is waiting for current active
     deciphering to be finished */
  boolean mcaltds_dec_data_xfer_waiting_for_decipher_done;
  /* number of transport block set (TrCh) done for deciphering */
  uint8 mcaltds_dec_num_tbset_done_for_deciph;
  /* number of TB done for deciphering in current TBset */
  uint8 mcaltds_dec_num_tb_done_for_deciph;

  /* number of transport block set (TrCh) done for deciphering */
  uint8 mcaltds_dec_num_tbset_done_for_bg_data_xfer;
  /* number of TB done for deciphering in current TBset */
  uint8 mcaltds_dec_num_tb_done_for_bg_data_xfer;
  /* number of bits done in background data transfer */
  uint32 mcaltds_dec_num_bits_bg_xfered_done;
  /* number of total TB (or MAC-d PDU read) */
  uint16 mcaltds_dec_num_total_tb_read;


  /* HS-DSCH MAC-d PDU deciphering related variables */

  /* Deciphering start address of current MAC-d PDU */
  uint16 mcaltds_dec_hs_tb_deciph_start_addr;
  /* Deciphering start bit position at start address */
  uint16 mcaltds_dec_hs_deciph_start_bit_pos;
  uint16 mcaltds_dec_ehs_dm_move_bit_pos;
  uint16 mcaltds_dec_hs_dm_move_bit_pos;
  /* Length with in MAC-d PDU size to decipher. This length is one less
     than actual length */
  uint16 mcaltds_dec_hs_tb_deciph_len;
  /* Offset of first bit to be deciphered of current MAC-d PDU from start of
     HS-DSCH block */
  uint16 mcaltds_dec_hs_blk_deciph_first_bit_offset;

  /* Size indication information element indexof current block */
  uint8 mcaltds_dec_hs_cur_sid_scaned_deciph;
  uint8 mcaltds_dec_ehs_cur_sdu_scaned_deciph;
  /* Number of blocks in current size indicator processed */
  uint8 mcaltds_dec_hs_num_blks_scaned_deciph;
  /* offset to current MAC-d PDU start from start of HS-DSCH block */
  uint16 mcaltds_dec_hs_cur_blk_start_offset_deciph;


  /* HS-DSCH MAC-d SDU data transfer related variables */

  /* DOB start address to read data from */
  uint16 mcaltds_dec_hs_tb_bg_xfer_start_addr;
  /* Starting offset to read first bit at DOB start address */
  uint8  mcaltds_dec_hs_bg_xfer_start_offset;
  /* MAC-d SDU length */
  uint16 mcaltds_dec_hs_tb_bg_xfer_len;

  /* Size indication information element indexof current block */
  uint8 mcaltds_dec_hs_cur_sid_bg_xfer;
  /* Number of blocks in current size indicator processed */
  uint8 mcaltds_dec_hs_num_blks_bg_xfer;
  /* offset to current MAC-d PDU start from start of HS-DSCH block */
  uint16 mcaltds_dec_hs_cur_blk_start_offset_bg_xfer;

#ifdef FEATURE_TDSCDMA_TBSET_DEBUG_DUMP
  /* TBset debug data dump */
  /* --------------------- */

  /* The list of CCTrCh index and TrCh idx that needs to be dumped */
  boolean mcaltds_dec_debug_dump_list[MCALTDS_DEC_TOTAL_CCTRCH_TABLES][MCALTDS_DEC_TOTAL_TRCHT_PER_CCTRCH];
  /* Flag to check if data dump is enabled by DIAG */
  boolean mcaltds_dec_debug_data_dump_enabled;
  /* Number of TB sizes to be dumped */
  uint8   mcaltds_dec_debug_dump_num_tbsize;
  /* TB size list */
  uint16  mcaltds_dec_debug_dump_tbsize_list[TDSDL_MAX_NUM_TBSIZE_DUMP];
#endif

#ifdef FEATURE_TDSCDMA_SW_F8_DECIPHER

  tdsl1_cipher_key_info_struct_type mcaltds_dec_cipher_info;

  /*wrap the ciphering param local variable */
  mcaltds_dec_sw_decipher_type mcaltds_dec_decipher_param; //make the param global as

  /*buffer to store the tm mode */
  uint8 mcaltds_dec_tm_key[TDSL1_DL_MAX_TB_BIT_SIZE >> 3];

#endif

  tfw_nonHS_dob_status_t *mcaltds_dec_non_hs_dob_status_fifo_ptr;

  /* holds the pointer to the status FIFO entry structure currently being
     read from the mDSP */
  mcaltds_dec_dob_status_fifo_struct_type *mcaltds_dec_cur_mdsp_status_fifo;

  /* Pointer to HS mDSP Status FIFO */
//WfwDembackHsDecobStatusFifoTableStruct *mcaltds_dec_hs_dob_status_fifo_ptr;
  tfw_hs_dob_status_t *mcaltds_dec_hs_dob_status_fifo_ptr;

  uint32 *mcaltds_dec_debug_data_buffer_ptr;

  /* Pointers to DOB FIFO SW-FW Interface structures */
  tfw_nonHS_dob_status_fifo_t *mcaltds_dec_non_hs_dob_status_fifo;
  tfw_hs_dob_status_fifo_t *mcaltds_dec_hs_dob_status_fifo;
  uint8 mcaltds_dec_bplmn_crc_err_cnt;
} mcaltds_dec_global_struct_type;




//WfwDembackNonHsDecobStatusFifoTableStruct *mcaltds_dec_non_hs_dob_status_fifo_ptr;
/* temporary keep the old data structure here for compiling */



/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

extern mcaltds_dec_global_struct_type *mcaltds_dec_global;

/* Flag indicating if target logging CFN is beyond combiner enable CFN*/

/* This is the CFN adjustment from the CFN of the last frame
   of the TTI to reach at first CFN of the TTI */
extern const uint8 mcaltds_dec_cfn_adjust_for_tti[TDSL1_NUM_TTI];

/* CRC length value corresponding to CRC enum type */
extern const uint8 mcaltds_dec_crc_length_val_table[TDSL1_NUM_CRC_LENGTH];

#define MCALTDS_DEC_DOB_RD_INTLOCK()  REX_ISR_LOCK(&mcaltds_dec_dob_read_crit_sect)
#define MCALTDS_DEC_DOB_RD_INTFREE()  REX_ISR_UNLOCK(&mcaltds_dec_dob_read_crit_sect)

#define MCALTDS_DEC_A2_PC_INTLOCK()  REX_ISR_LOCK(&mcaltds_dec_a2_pc_crit_sect)
#define MCALTDS_DEC_A2_PC_INTFREE()  REX_ISR_UNLOCK(&mcaltds_dec_a2_pc_crit_sect)


#ifdef FEATURE_TDSCDMA_PCH_FET
#define MCALTDS_DEC_PCH_FET_ISR_MAX_CNT 3
#define MCALTDS_DEC_PCH_FET_TTI_OFFSET_NUM 3 /*min 20ms tti, 4 subframe, offset 0(10ms) 1(15ms) 2*/

typedef struct
{
  uint8 isr_cnt;          /* count the dec isr when feten*/
#ifdef FEATURE_TDSCDMA_PCH_SIR_FET
  uint8 fet_tti_sfm;
#endif
  boolean immed_drop_crc_pass; /*in dec isr when crc pass set the immed drop  */
  uint32 isr_pass_statics[MCALTDS_DEC_PCH_FET_TTI_OFFSET_NUM];
  uint32 pch_dec_count;   /*total pch decode count*/
} mcaltds_dec_pch_fet_info_struct_type;
#endif
/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*============================================================================*/
/**
 * This function initializes the variables related to decoder module
 */
/*============================================================================*/

extern void mcaltds_dec_init(void);

extern void mcaltds_dec_idle_init(void);

/*===========================================================================
FUNCTION mcaltds_decdata_init

DESCRIPTION
  This function initializes the variables related to decoder module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcaltds_decdata_init(void);

extern void mcaltds_decdata_idle_init(void);

/*============================================================================*/
/**
 * This function validate CCTrCh setup parameters for their valiidity. It do
 * all range checks other things that bound mDSP and ARM interface.
 *
 * @return TRUE or FALSE based on validation success.
 * indicator
 */
/*============================================================================*/

extern boolean mcaltds_dec_validate_cctrch_setup(
  /** number of CCTrCHs */
  uint8 num_cctrch,
  /** CCTrCh info structure pointer */
  mcaltds_dec_cctrch_info_struct_type cctrch_array[],
  /** TFC information for this CCTrCh */
  mcaltds_dec_ctfc_info_struct_type ctfc_array[]);

/*============================================================================*/
/**
 * This function setup CCTrCh table in mDSP. It computes rate maching parameters,
 * coding parameters, selects CCTrCh table index in mDSP to be setup with these
 * parameters and uploads the contents to mDSP.
 * If there is associated TFC table with CcTrCh. It finds free TFC table index
 * and upload TFC information in that table.
 *
 * @return MCAL WCDMA decoder CCTrCh Id for the CcTrCh setup. If there is failure
 * to setup CCTrCh, it returns MCALTDS_DEC_INVALID_CCTRCH_ID_VAL as error
 * indicator
 *
 * @see MCALTDS_DEC_INVALID_CCTRCH_ID_VAL
 */
/*============================================================================*/
extern void mcaltds_dec_cctrch_setup(
  /** pointer to the array of CCTrCH index pointers */
  uint8 cctrch_idx[],
  /** number of CCTrCHs */
  uint8 num_cctrch,
  /** CCTrCh info structure pointer */
  mcaltds_dec_cctrch_info_struct_type cctrch_info[]
);
/*===========================================================================
FUNCTION mcaltds_dec_cctrch_update

DESCRIPTION
  This function setup CCTrCh table in mDSP. It computes rate maching parameters,
  coding parameters, selects CCTrCh table index in mDSP to be setup with these
  parameters and uploads the contents to mDSP.
  If there is associated TFC table with CcTrCh. It finds free TFC table index
  and upload TFC information in that table.

DEPENDENCIES
  None

RETURN VALUE
  MCAL WCDMA decoder CCTrCh Id for the CCTrCh setup. If there is failure
  to setup CCTrCh, it returns MCALTDS_DEC_INVALID_CCTRCH_ID_VAL as error
  indicator

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_dec_cctrch_update(
  /** number of CCTrCHs */
  uint8 num_cctrch,
  /** pointer to CCTrCH info structure array  */
  uint8 cctrch_index[]);

#ifdef FEATURE_TDSCDMA_MCPM_POWER_REPORTING
/*===========================================================================
FUNCTION mcaltds_dec_get_decoder_power_report

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 mcaltds_dec_get_decoder(void);
#endif

/*============================================================================*/
/**
 * This function frees CCTrCh table Id and mark correspondig CCTrCh table index
 * and DRM buffer area available to use.
 */
/*============================================================================*/

extern void mcaltds_dec_cctrch_free(
  /** MCAL WCDMA decoder CCTrCh Id to be freed */
  mcaltds_dec_cctrch_id cctrch_id);


/*============================================================================*/
/**
 * This function saves HSDPA channel decode related information. Information
 * is copied into internal structures and is marked valid.
 */
/*============================================================================*/

extern void mcaltds_dec_hs_update_hs_info(
  /** HSDPA info Id to update information */
  uint8 dec_hs_info_id,
  /** HADPA decode related information structure pointer */
  mcaltds_dec_hs_info_struct_type *dec_hs_info);

/*============================================================================*/
/**
 * This function frees HSDPA table decode info and mark correspondig is invalid
 */
/*============================================================================*/

extern void mcaltds_dec_hs_free_hs_info(
  /** MCAL WCDMA decoder HS info Id to be freed */
  uint8 dec_hs_info_id);


/*============================================================================*/
/** @name Accessor function for DOB handles contents
 * Following function provide access to various information in DOB handle */
/**@{*/
/*============================================================================*/

/*============================================================================*/
/**
 * Query if DOB handle if of HS type or not
 *
 * @return TRUE or FALSE. TRUE indicate DOB handle is of HS type
 */
/*============================================================================*/

extern boolean mcaltds_dec_query_handle_type_hs(
  /** DOB status handle to query for HS type */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get HS channel info Id for this DOB status handle. DOB status handle must be
 * checked for it to be HS type. No validation is done in this function.
 *
 * @return HS channel info Id for this channel that was returned at the time of
 * HS channel setup.
 */
/*============================================================================*/

extern uint8 mcaltds_dec_get_hs_info_id(
  /** DOB status handle for which to get HS info Id */
  mcaltds_dec_dob_status_handle_type handle);

extern uint16 mcaltds_dec_get_hs_subframe_num(
  mcaltds_dec_dob_status_handle_type handle);


/*============================================================================*/
/**
 * Get R99 channel info Id for this DOB status handle. DOB status handle must be
 * checked for it to be R99 channel type and not HS type. No validation is done
 * in this function.
 *
 * @return R99 channel CCTrCh info Id for this channel that was returned at the
 * time of CCTrCh setup.
 */
/*============================================================================*/

extern boolean mcaltds_dec_trch_idx_exists(uint8 cctrch_idx, uint8 trch_idx);

extern uint8 mcaltds_dec_get_cctrch_info_id(
  /** DOB status handle for which to get CCTrCh info Id */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get transport channel information index in CCTrCh.
 *
 * @return Transport channel information index in CCTrCh. This is same index
 * for the transport channel that was in CCTrCh setup info at the time of setup.
 */
/*============================================================================*/

extern uint8 mcaltds_dec_get_trch_index(
  /** DOB status handle for which to transport channel index */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get transport format information index of transport channel in CCTrCh.
 *
 * @return Transport format information index of transport channel in CCTrCh.
 * This is same index that was in CCTrCh setup info at the time of setup.
 */
/*============================================================================*/

extern uint8 mcaltds_dec_get_tf_index(
  /** DOB status handle for which to transport format index */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Query TFCI decode status.
 *
 * @return TRUE or FALSE. TRUE indicate that TFCI decode passed.
 */
/*============================================================================*/

extern boolean mcaltds_dec_get_tfci_decode_status(
  /** DOB status handle for which to TFCI decode status is queried */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Query Special Burst decode status.
 *
 * @return TRUE or FALSE. TRUE indicate that SB detected.
 */
/*============================================================================*/

extern boolean mcaltds_dec_get_sb_detected_status(
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Query puncturedInd status.
 *
 * @return TRUE or FALSE. TRUE indicate that current TTI is partial punctured.
 */
/*============================================================================*/

extern boolean mcaltds_dec_get_puncturedInd_status(
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the starting CFN of decoded TTI
 */
/*============================================================================*/

extern uint32 mcaltds_dec_get_crc_result_bits(
  /** DOB status handle for which to get CRC pass/fail result bits */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the ZSB bit value
 */
/*============================================================================*/

extern boolean mcaltds_dec_get_zsb(
  /** DOB status handle for which to get ZSB bit value */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the starting CFN of decoded TTI
 */
/*============================================================================*/
extern uint8 mcaltds_dec_get_decode_cfn(
  /** DOB status handle for which to get decoded CFN */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Set CRC override status for DOB handle. Used in case of loopback mode need
 * to pass CRC errored block
 */
/*============================================================================*/

extern void mcaltds_dec_set_crc_oride_status(
  /** DOB status handle for which to TFCI decode status is queried */
  mcaltds_dec_dob_status_handle_type handle,
  /** CRC override status value */
  boolean status);

/*============================================================================*/
/**
 * Get CRC override status for DOB handle. Used in case of loopback mode need
 * to pass CRC errored block
 *
 * @return CRC overriode status TRUE/FALSE
 */
/*============================================================================*/

extern boolean mcaltds_dec_query_crc_oride_status(
  /* DOB status handle for which to TFCI decode status is queried */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get Energy Metric value for DOB handle.
 *
 * @return Energy Metric value
 */
/*============================================================================*/

extern uint16 mcaltds_dec_query_energy_metric(
  /* DOB status handle for which Energy Metric is queried */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get stmr wall time counter value for DOB handle.
 *
 * @return stmr wall time counter value
 */
/*============================================================================*/
extern uint32 mcaltds_dec_get_stmr_wall_time_cnt(
  mcaltds_dec_dob_status_handle_type handle);

/*===========================================================================
FUNCTION mcaltds_dec_overwrite_tfci_fail

DESCRIPTION
  This function is called when UE is configured in loopback mode and encounters
  a TFCI decode error. In this case, the TFCI decode error is overwritten
  and treated as CRC failure with TF as the largest TF. This will help in
  looping back junk data to L2.
  If TFCI decode error was treated as such, no data is passed to L2.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  DOB status FIFO values changed
===========================================================================*/

extern void mcaltds_dec_overwrite_tfci_fail(
  /* DOB handle */
  mcaltds_dec_dob_status_handle_type handle,
  /* TF index to be set for this TBset */
  uint8 tf_index);

/*============================================================================*/
/**
 * This function returns pointer to DOB status information structure for the
 * DOB status handle passed in function argument. Since there is no DOB status
 * for HS channel, it will return NULL for that. A pointer returned remains
 * valid untill DOB status handle is freed. There is no check done for validity
 * of DOB status handle while returning information pointer
 *
 * @return DOB status information pointer in case of R9 channel. NULL otherwise
 */
/*============================================================================*/

extern mcaltds_dec_dob_status_type *mcaltds_dec_get_dob_status(
  /** Status FIFO index for which the DOB status is to be read */
  mcaltds_dec_dob_status_handle_type handle);

/**@}*/

/*============================================================================*/
/**
 * This function frees DOB handle in MCAL decoder database. After that access to
 * DOB handle information accessor function should not be done.
 */
/*============================================================================*/

extern void mcaltds_dec_free_dob_handle(
  /** DOB hadle to be freed */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * This function reads transport block header information. This function is
 * called in the interrupt context by the transport block header read heart
 * beat handler. This function gets the transport header information for all
 * transport blocks that have passed CRC or are in loopback mode that needs
 * CRC errored blocks to be passed up. Rest of the blocks are discarded.
 * This function is passed the pointer to the array of the DOB status handles
 * and number if handles in that array to be processed.
 *
 * Out of the function is to update TBset header info structure. Its pointer is
 * one of the argument. This structure is for higher layers that WHL1.
 */
/*============================================================================*/

extern void mcaltds_dec_get_decode_trblk_header(
  /** Number of DOB handles for which header need to be read (input) */
  uint8 num_dob_handles,
  /** DOB handle array pointer for which to read the data (input) */
  mcaltds_dec_dob_status_handle_type *dob_handles_in,
  /** DOB handle array pointer corresponding to TBset header structure elements (output)
   * This aray pointer must have at least num_dob_handles elements */
  mcaltds_dec_dob_status_handle_type *dob_handles_out,
  /** pointer to the TB header decode info strcuture (output)
   * This aray pointer must have at least num_dob_handles elements */
  mcaltds_dec_tb_hdr_decode_struct_type *tb_hdr_decode_info);

#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT
/*===========================================================================
FUNCTION mcaltds_dec_initiate_deciphering

DESCRIPTION
  This function does the following
  - Initialize deciphering related variables
  - Prepare max number of deciphering tasks
  - Trigger deciphering if deciphering is not in progress and deciphering
    task are already prepared and ready

DEPENDENCIES
  RLC Ciphering parameters are ready before this function is called

RETURN VALUE
  None

SIDE EFFECTS
  The first batch of deciphering is triggered at the end of header read event
===========================================================================*/

extern void mcaltds_dec_initiate_deciphering(
  /* pointer to the TB header decode info strcuture */
  mcaltds_dec_tb_hdr_decode_struct_type *tb_hdr_decode_info,
  /* Ptr to array of the DOB handles for these TB sets Hdr info */
  mcaltds_dec_dob_status_handle_type *dob_handles);
#endif /* defined(FEATURE_TDSCDMA_DL_DATA_PATH_OPT) && !defined(FEATURE_WCDMA_PP_HW_DL) */

/*============================================================================*/
/**
 * This function reads data from DOB for all blocks in current TBset header
 * information. It implements a priority based tasks for data transfer along
 * with deciphering. Order or priority is as following
 *
 * - Trigger deciphering if deciphering is not in progress and deciphering
 *   task are already prepared and ready
 * - Poll for deciphering done status if it is active
 * - Prepare and keep read next non TM mode ciphered block deciphering tasks
 *   if they are not yet ready and ciphered blocks are pending
 * - Transfer data in background when following conditions are met.
 *   - If ciphering is in progress it will not transfer data more than what
 *     is being deciphered.
 *   - If it hit block that is ciphered but not yet deciphered, data transfer
 *     aborts
 *
 * After non TM or unciphered data has been transferred then TM mode ciphered
 * data is deciphered and transferred from DOB block by block.
 */
/*============================================================================*/

extern void mcaltds_dec_get_trblk_data(
  /** pointer to the TB header decode info strcuture */
  mcaltds_dec_tb_hdr_decode_struct_type *tb_hdr_decode_info,
  /** Ptr to array of the DOB handles for these TB sets Hdr info */
  mcaltds_dec_dob_status_handle_type *dob_handles,
  /* Ptr to structure in which to store data */
  mcaltds_dec_tb_data_struct_type *dl_tb_data_info);


/*============================================================================*/
/**
 * This function is interrupt service routine for TBset decode done function.
 * It gets the head of the status FIFO in mDSP and get the status FIFO info
 * from that index. It repeat till it has got all the status FIFO. Though one
 * TBset decode done event corresponds to one status FIFO update for one TBset.
 * however there might be some more TBset decoded while the current index is
 * being read from status FIFO. This function tries to get the maximum possible
 * status FIFO entries.
 */
/*============================================================================*/

extern void mcaltds_dec_tbset_decode_done_handler(void);

/*============================================================================*/
/**
 * This function updates the ciphering key for downlink deciphering operation
 * to the ciphering key index locations requested.
 */
/*============================================================================*/
#ifdef FEATURE_TDSCDMA_SW_F8_DECIPHER

extern void mcaltds_dec_set_ciphering_key(tdsl1_cipher_key_info_struct_type *key_info_ptr );

#else
extern void mcaltds_dec_set_ciphering_key(
  /** ciphering key index (one of the 6 keys) to which the key is set */
  uint8 index,
  /** ciphering key info. Pointer to the array of 16 bytes. May not be W32
   * aligned */
  uint8 *ciphering_key_data);

#endif
/*============================================================================*/
/** @name External APIs for MCAL WCDMA decoder
 * Following APIs are expected to be imlpemented by user of MCAL WCDMA decoder
 * module and are called by MCAL WCDMA encoder at various instances as described
 * in each function prototype below */
/**@{*/
/*============================================================================*/

/*============================================================================*/
/**
 * This function in called in response to a TBset decoded. For every TBset
 * decoded, MCAL WCDMA decoder reads mDSP DOB status FIFO and set information
 * contents for this handle and calls this function.
 *
 * @return TRUE or FALSE. WHL1 may not be able to accept this handle and return
 * boolean. If it is FALSE, handle is immediately freed by MCAL WCDMA decoder.
 */
/*============================================================================*/

extern boolean mcaltds_dec_tbset_decode_done_cb(
  /* DOB status handle for this TB decode done */
  mcaltds_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * This function in called at some pre-determined time where all accumulated DOB
 * handles are processed for header information. This is called before data
 * indication is given to WHL1. Data indication needs information from this
 * function. WHL1 processes DOB handles and come up with list of DOB handles for
 * which header information bits need to be read for its blocks from DOB.
 *
 * @see mcaltds_dec_get_decode_trblk_header
 * @see mcaltds_dec_trblk_data_cb
 */
/*============================================================================*/

extern boolean mcaltds_dec_trblk_header_cb(void);

/*============================================================================*/
/**
 * This function in called at some pre-determined time where all DOB handles that
 * are processed for header indication, needs data pulled out of DOB for delivery
 * to higher layers. It needs information that is generated after calling
 * mcaltds_dec_trblk_header_cb function in which WHL1 has got all ciphering and
 * data block offsets to read information from higher layer.
 *
 * @see mcaltds_dec_trblk_header_cb
 * @see mcaltds_dec_get_decode_trblk_header
 */
/*============================================================================*/

extern boolean mcaltds_dec_trblk_data_cb(void);

/*============================================================================*/
/**
 * This function reads the BCH data out of the DOB for the handle
 * passed in by WHL1.

 * @return Raw BCH data
 */
/*============================================================================*/

extern uint32 mcaltds_dec_read_bch_sfn(
  mcaltds_dec_dob_status_handle_type handle);

/**@}*/
#ifdef FEATURE_HSDPA_L1_STATS
/*============================================================================*/
/**
 * This function dumps HARQ statistics as observed from DOB status FIFO. This
 * function is called every time the log packet accumulating HS DL data is
 * flushed out.
 */
/*============================================================================*/

extern void mcaltds_dec_hs_dump_stats_from_dob_status_fifo(void);

/*============================================================================*/
/**
 * This function resets HARQ statistics as observed from DOB status FIFO. Once
 * the DOB data statistics has been dumped, it resets the counters so that the
 * next set of statistics can be accumulated.

 * @sideeffect All counters are reset to 0.
 */
/*============================================================================*/

extern void mcaltds_dec_hs_reset_stats(void);
#endif

/*============================================================================*/
/**
 * This function accumulates MAC-hs header log samples and submits to DIAG once
 * the packet is full. IT then creates a new packet for next accumulation.
 */
/*============================================================================*/

extern void mcaltds_dec_hs_update_mac_hs_header_logging(
  /* DOB status handle */
  mcaltds_dec_dob_status_handle_type handle,
  /* buffer for MAC-HS header data */
  uint32 *mac_hs_header_buf,
  uint16 offset);

#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
/*===========================================================================
FUNCTION mcaltds_dec_hs_update_mac_ehs_header_logging

DESCRIPTION
  This function accumulates MAC-ehs header log samples and submits to DIAG once
  the packet is full. It then creates a new packet for next accumulation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcaltds_dec_hs_update_mac_ehs_header_logging(
  /* DOB status handle */
  mcaltds_dec_dob_status_handle_type handle,
  /* TB set header information structure pointer */
  mcaltds_dec_tb_set_hdr_struct_type *tbset_hdr_info,
  uint32 num_reorder_sdu_in_reord_pdu[],
  uint8 si_info[],
  uint8 tsn_info[],
  uint8 q_info[]
);
#endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */


/*============================================================================*/
/**
 * This function is interrupt service routine for TBset decode done function.
 * It gets the head of the status FIFO in mDSP and get the status FIFO info
 * from that index. It repeat till it has got all the status FIFO. It may be
 * noted that one interrupt corresponds to one status FIFO update. However
 * there might be some more TBset decoded while the current index is being
 * read from status FIFO. This function tries to get the maximum possible
 * status FIFO entries. It then calls the WHL1 decoder callback function and
 * passes a DOB handle for WHL1 to operate on.
 */
/*============================================================================*/

extern void mcaltds_dec_decode_done_isr_handler_cb(void);

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION  mcaltds_dec_process_cctrch_log_on_demand

DESCRIPTION
  This function collects all the items required for DL CCTrCH
  logging and submits the log packet to QXDM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcaltds_dec_process_cctrch_log_on_demand(void);
#endif

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION  mcaltds_dec_process_tfc_log_on_demand

DESCRIPTION
  This funtion submits TFC log information to QXDM

DEPENDENCIES
  CCTrCH database

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcaltds_dec_process_tfc_log_on_demand(void);
#endif


extern void mcaltds_dec_free_drm_buffer(
  /* CCTrCh index for which DRM buffer has to be freed */
  uint8 cctrch_idx);

/*===========================================================================
FUNCTION mcaltds_dec_update_loopback_info

DESCRIPTION
  This function updates the loopback mode of a Trch in MCAL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_dec_update_loopback_info(
  /* CCTrCh index */
  uint8 cctrch_index,
  /* TrCh index */
  uint8 trch_index,
  /* loopback mode */
  tdsl1_loop_back_mode_type_enum_type loopback_mode);

/*===========================================================================
FUNCTION mcaltds_dec_write_hs_deciphering_parms

DESCRIPTION
  This function writes the deciphering parameters to the UMTS DL HW Task Q
  to decipher HSDPA transport blocks.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SUCCESS
  FALSE if FAILURE

SIDE EFFECTS
  None
==========================================================================*/

extern void mcaltds_dec_write_hs_deciphering_parms(
  /* ciphering algorithm */
  mcaltds_dec_ciphering_algo_enum_type ciphering_algo,
  /** Ciphering key id */
  uint8 ciphering_key_id,
  /* Deciphering will continue in the next job or not */
  boolean will_continue,
  /* radio bearer id */
  uint8 bearer_id,
  /* Length in bits to decipher */
  uint32 length,
  /* DOB Address from where to start the ciphering */
  uint32 start_addr,
  /* Offset in bits to start deciphering */
  uint32 bit_offset,
  /** Ciphering count (CFN/SFN) */
  uint32 count_c);

/*===========================================================================
FUNCTION mcaltds_dec_free_status_fifo

DESCRIPTION
  This function frees the DOB status FIFO in MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcaltds_dec_free_status_fifo(
  /* status FIFO index in mDSP to be freed */
  uint16 fifo_index);

extern uint16 mcaltds_dec_get_hs_dob_fifo_idx(mcaltds_dec_dob_status_handle_type handle);

/*===========================================================================
FUNCTION mcaltds_dec_do_hs_data_transfer

DESCRIPTION
  This function programs Data Mover and P2 hardware to perform data read
  activity for HS blocks residing in DOB and also to perform partial
  pdu concatenation.

  - Checks to see if previous data transfer has completed. If it
    is still pending, then do not program a new transfer and wait
    until the previous one completes.
  - Checks to see if P2 UMTS TaskQ h/w read and write pointers
    are in sync, if not, err_fatal.
  - Calls mcaltds_dec_process_current_tti_hs_blocks routine
  - If there is a need to do partial pdu concatenation, calls
    mcaltds_dec_process_prev_tti_hs_blocks routine
  - If there is a need for any data transfer, calls
    mcaltds_dec_trigger_p2_processing and
    mcaltds_dec_dsm_maintenance routines
  - If there is no data transfer activity needed, it immediately
    updates hsDobStatFifoReadIndex in the FW-SW interface.

ASSUMPTIONS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcaltds_dec_do_hs_data_transfer(
  /* pointer to the TB header decode info strcuture */
  mcaltds_dec_tb_hdr_decode_struct_type *tb_hdr_decode_info,
  /* Ptr to array of the DOB handles for these TB sets Hdr info */
  mcaltds_dec_dob_status_handle_type *dob_handles);

extern uint16 mcaltds_dec_get_hs_harq_id(
  mcaltds_dec_dob_status_handle_type handle);

#ifdef FEATURE_TDSCDMA_A2_HW_DL
/*===========================================================================
FUNCTION mcaltds_dec_a2_dl_init

DESCRIPTION
  This function initializes the A2 DL PHY module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_dec_a2_dl_init(void);

/*===========================================================================
FUNCTION mcaltds_dec_a2_dl_deinit

DESCRIPTION
  This function de-initializes the A2 DL PHY module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_dec_a2_dl_deinit(void);

#endif /* FEATURE_TDSCDMA_A2_HW_DL */

/*===========================================================================
FUNCTION mcaltds_dl_register_dec_header_read_callback

DESCRIPTION
  This function is the call back function by MCAl to run the header read function
  in decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_dl_register_dec_header_read_callback(mcal_header_data_reading_cb_fn header_read_cb_func);

/*===========================================================================
FUNCTION mcaltds_dl_register_dec_data_read_callback

DESCRIPTION
  This function is the call back function by MCAl to run the header read function
  in decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_dl_register_dec_data_read_callback(mcal_header_data_reading_cb_fn data_read_cb_func);


/*===========================================================================
FUNCTION mcaltds_dec_update_hs_readindex

DESCRIPTION
  This function updates the read index for HS in FW/SW shared memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
extern void mcaltds_dec_update_hs_readindex(uint8 index);

/*===========================================================================
FUNCTION mcaltds_dec_get_fw_table_index

DESCRIPTION
  This function is called from MCAL DL module.
  It will indicate cctrch table index of demback in FW,
  so MCAL DL can configure this table index to demfront.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  DRMB address alignment.
===========================================================================*/
extern uint8  mcaltds_dec_get_fw_table_index(uint8 cctrch_index);

/*===========================================================================
FUNCTION mcaltds_dec_get_drm_buf_start_addr

DESCRIPTION
  This function get DRM buffer start addr of channel.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: set successfully.
  FALSE: set failure.

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 mcaltds_dec_get_drm_buf_start_addr(uint8 cctrch_index);

/*===========================================================================
FUNCTION mcaltds_dec_get_drm_buf_dir

DESCRIPTION
  This function get DRM buffer dir of channel.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: set successfully.
  FALSE: set failure.

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 mcaltds_dec_get_drm_buf_dir(uint8 cctrch_index);

/*===========================================================================

FUNCTION mcaltds_dec_get_turbo_coding_enabled

DESCRIPTION
   This function returns TRUE if there is at least one TURBO coding TrCh
   in enable state.

DEPENDENCIES
   None.

RETURN VALUE
   TRUE if at least one turbo coding TrCh is configured and in enable state
   FALSE otherwise

SIDE EFFECTS
   None.

===========================================================================*/
extern boolean mcaltds_dec_get_turbo_coding_enabled (void);
/*===========================================================================

FUNCTION mcaltds_dec_enable_disable_tdec

DESCRIPTION
   This function enables (through MCPMDRV) TDEC based on whether there is at
   least one TURBO coding TrCh in enable state.

DEPENDENCIES
   None.

RETURN VALUE
   TRUE if at least one turbo coding TrCh is configured and in enable state
   FALSE otherwise

SIDE EFFECTS
   None.

===========================================================================*/
extern boolean mcaltds_dec_enable_disable_tdec(void);


/*===========================================================================

FUNCTION mcaltds_dec_get_active_fw_table_idx

DESCRIPTION
   This function looks up the cuurent active cctrch's fw tabel idex, and return it

DEPENDENCIES
   None.

RETURN VALUE
   the cuurent active cctrch's fw table idex

SIDE EFFECTS
   None.

===========================================================================*/

extern uint8 mcaltds_dec_get_active_fw_table_idx(void);

extern void mcaltds_dec_alloc_dynamic_mem(void);

extern void mcaltds_dec_free_dynamic_mem(void);

#ifdef FEATURE_TDSCDMA_PCH_FET
/*===========================================================================
FUNCTION mcaltds_decdata_pch_fet_info_init

DESCRIPTION
  This function initializes the variables related to pch fet statistics

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_decdata_pch_fet_info_init(void);
/*===========================================================================
FUNCTION mcaltds_decdata_get_pch_fet_isr_cnt

DESCRIPTION
  This function return number of  isr in FET for one page

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 mcaltds_dec_get_pch_fet_isr_cnt(void);
#ifdef FEATURE_TDSCDMA_PCH_SIR_FET
/*===========================================================================
FUNCTION mcaltds_dec_get_pch_sir_fet_tti_sfm

DESCRIPTION
  This function return fet_tti_sfm

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

uint8 mcaltds_dec_get_pch_sir_fet_tti_sfm(void);
#endif
#endif

#endif /* MCALWCDMA_DEC_H */

