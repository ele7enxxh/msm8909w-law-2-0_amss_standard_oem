#ifndef MCALWCDMA_DEC_H
#define MCALWCDMA_DEC_H

/*============================================================================*/
/** @file  
 * This module has definition and declaration related to MCAL WCDMA decoder
 * module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_dec.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/16/15    ap      Initial check-in for FR 30856
02/24/15    ap      Initial check-in for WL1 QSH framework 
10/17/14    ac      Add discard_l2_pdu field in rlc header info to avoid wrongly 
                    deciphered PDU being processed
09/16/14    pkj     Optimization: un-used structure/variable removal and type change
05/05/14    jhl     Updated MCALWCDMA_DEC_HS_REORDER_SDU_MAX_COUNT for 3C/4C 
01/10/14    jhl     moved FEATURE_WCDMA_HS_FACH out of FEATURE_HSDPA_MAC_EHS
12/16/13    ac      Add LVA non-first traceback and skip decode flags
12/10/13    bs      Improve SW-FW handshake of TDEC clock usage
11/14/13    vs      Increasing the TSN Buf count for 3C to 450.
11/14/13    pj      Wrapped HWIO access with WL1_HWIO macro
10/30/13    bs      Extended TSN support added for 3C
04/04/13    vvv     Added support to send decode health to Layer2
03/14/13    rc      Added prototype for mcalwcdma_dec_get_zsb_word 
03/05/13    jhl     Cleanup F8 related code 
10/22/12    vs      Removed the critical section dob_read_crit_sect
10/15/12    jd      Memory Optimization Changes
08/16/12    vsr     MBMS CCTRCH cleanup
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/07/12    vs      MBMS feature cleanup.
05/03/12    vsr     FET/DED/ED-V2 Feature checkin
03/22/12    vs      Change for Max harq processes.
03/16/12    ms      Added support for FEATURE_WCDMA_HS_FACH
01/27/12    vs      Nikel feature cleanup.
12/02/11    ms      Added prototype for mcalwcdma_decdata_power_up_init
11/23/11    ms      Added profiling of DMA from DOB and Decipher ISR
11/14/11    ms      A2 Power Collapse changes
10/03/11    ms      Moved extern definition to header file 
09/26/11    stk     Added prototype for mcalwcdma_decdata_hw_init() to be used in wl1m.c
09/17/11    mc      Changes to get TrCh enable state from WHL1
09/15/11    ms      Added prototype for mcalwcdma_dec_get_turbo_coding_configured() , 
                    mcalwcdma_dec_get_conv_coding_configured() and mcalwcdma_dec_get_turbo_coding_enabled().
08/24/11    ms      Re-write the DL cipher keys to HW after DEMBACK comes
                    out of power collapse
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
02/02/11    vs      Added support for querying convolutionally coded TrCh 
                    within configured CCTrCh 
08/23/10    mc      Changed max number of times decode_done_ise can be skipped
08/18/10    ms      Added one more profile point within Decode Done ISR.
06/21/10    ms      Fixed compiler warnings
06/18/10    ms      Removed unused variables f8_start and f8_end
04/23/10    ms      Removed redundant declaration of mcalwcdma_decdata_init
04/20/10    ms      Added prototype for mcalwcdma_dec_query_energy_metric
04/09/10    ms      Updated type specifier for DRMB size to uint32
04/09/10    ms      Added support for dynamic memory allocation from Modem Heap.
02/05/10    ms      Taxis/Genesis code merge
12/29/09    ms      Bumped up MCALWCDMA_DEC_MAX_TSN_INFO_BUF to 300 for now.
11/12/09    ms      Added missing / at the end of comment line
10/26/09    ms      Indicate to MAC whether the HS blocks being delivered
                    belong to current TTI or previous TTI or both.
10/24/09    ms      PP/A2 featurization changes
10/13/09    sup     Fixed compilation error
09/02/09    ms      Added support for TSN tracing.
07/09/09    ms      Fixed variable name as mcalwcdma_dec_cfn_adjust_for_tti.
07/06/09    ms      Added prototype for dec_cfn_adjust_for_tti.
06/03/09    mr      Skip TFCI logging after CCTrCh enable
05/29/09    mc      Overwrite TFCI fail with CRC fail in loopback mode
05/12/09    ms      Restricted DOB extension buffer size to 16000 bytes.
04/24/09    mr      Fixed Compiler Warnings
04/09/09    ms      Code cleanup.
04/02/09    ms      Added macro DEC_DOB_RD_INTLOCK.
03/30/09    rm      Added new featurization for FEATURE_CMI.
03/26/09    ms      Removed #include "p2_umts_test.h"
03/23/09    ms      Increased DOB extension buffer size to 16384 bytes to
                    account for Max RLC PDU size of 1504 bytes * 10 HS blocks
                    per radio frame for MIMO.
                    Added macro MCALWCDMA_DEC_TOTAL_MAX_COMPLETE_PDU_FROM_PREV_TTI.
12/04/08    ms      Increased DOB extension buffer size to 4096 again.
09/23/08    mc      Added support for sending TB size to MAC in hdr_rd
                    Changed L1_CIPHER_ALGO_UEA0 to L1_CIPH_ALG_NO_ENCRYPTION
11/11/08    ms      Reduced DOB extension buffer size to 2048 to prevent
                    concatenation of too many PDUs in one shot.
10/15/08    ms      Added prototype for mcalwcdma_dec_deregister_cd_client.
10/03/08    hk      Bringing in 7k Mailine fixes
10/03/08    ms      Temporarily increased DOB extension buffer space macro
                    to 4096 to avoid piling up of TSNs due to low DOB 
                    Extension buffer space.
09/06/08    ms      Increased number of move out descriptors from 100 to 1500.
06/20/08    ms      Replaced ciphering_control with ciphering_algo.
                    Added support for Snow3G deciphering algorithm.
06/19/08    ms      Replaced FEATURE_WCDMA_REL7_L2_IMPROVEMENT with
                    FEATURE_HSDPA_MAC_EHS.
06/18/08    ms      Removed all occurences of FEATURE_WCDMA_HSPA_DEC.
06/10/08    ms      Support for MAC-ehs.
                    Function and variable names are changed.
07/31/07    mc      Fix MBMS related feature definitions
05/15/07    ms      MBMS Logging support.
04/05/07    mc      MBMS TrCh enable/disable support
03/30/07    mc      Changed interface for MBMS info table update
03/03/07    mc      Sync up for MBMS phase2A integration
12/12/06    mc      Synced up for MBMS demo - 3042 baseline
11/29/06    mc      Branched for MBMS demo.
09/28/06    mc      Added to //depot/asic/msmshared/umts/wcdma/l1/mcal/
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "uecomdef.h"
#include "dsm.h"
#include "mcalwcdma_common.h"

#include "rex.h"
#include "l1rrcif.h"
#if defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC)
#include "wl1_qsh_ext.h"
#endif /* defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC) */

#ifndef FEATURE_CMI

/* -------------------------------------- */
/* PUBLIC API section of mcalwcdma_dec.h  */
/* -------------------------------------- */

/** Enum type definition for TTI enum type */
typedef enum {
  L1_TTI_10MS, /**< 10 ms TTI */
  L1_TTI_20MS, /**< 20 ms TTI */
  L1_TTI_40MS, /**< 40 ms TTI */
  L1_TTI_80MS, /**< 80 ms TTI */
  L1_NUM_TTI   /**< Total number oif TTI types */
} mcalwcdma_dec_tti_enum_type;

#else

/* Public portion of the API exists in wcdmamvsif.h */
#include "wcdmamvsif.h"

#endif /* FEATURE_CMI */

#include "l1msm.h"

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* TrCh ref type bit field */
#define MCALWCDMA_DEC_TRCH_REFTYPE_SBCH_BF 0x01 /**< TrCh reference type S_BCH */
#define MCALWCDMA_DEC_TRCH_REFTYPE_NBCH_BF 0x02 /**< TrCh reference type N_BCH */
#define MCALWCDMA_DEC_TRCH_REFTYPE_DCH_BF  0x04 /**< TrCh reference type DCH */
#define MCALWCDMA_DEC_TRCH_REFTYPE_DSCH_BF 0x08 /**< TrCh reference type DSCH (Obsolete) */
#define MCALWCDMA_DEC_TRCH_REFTYPE_PCH_BF  0x10 /**< TrCh reference type PCH */
#define MCALWCDMA_DEC_TRCH_REFTYPE_FACH_BF 0x20 /**< TrCh reference type FACH */

/* Value for CRC length for each CRC enum type */
#define MCALWCDMA_DEC_CRC_LENGTH_0_VAL     0  /**< CRC length 0 size */
#define MCALWCDMA_DEC_CRC_LENGTH_8_VAL     8  /**< CRC length 8 size */
#define MCALWCDMA_DEC_CRC_LENGTH_12_VAL    12 /**< CRC length 12 size */
#define MCALWCDMA_DEC_CRC_LENGTH_16_VAL    16 /**< CRC length 16 size */
#define MCALWCDMA_DEC_CRC_LENGTH_24_VAL    24 /**< CRC length 24 size */

/* PCCPCH TF info */
#define MCALWCDMA_DEC_BCH_NUM_TB          1
#define MCALWCDMA_DEC_BCH_TB_SIZE_BITS    246

/** Macro to translate CRC enum to its absolute value */
#define MCALWCDMA_DEC_CRC_LENGTH_VAL(crc_enum) mcalwcdma_dec_crc_length_val_table[(crc_enum)]

/* Values for TTI in ms for each enum type */
#define L1_TTI_10MS_VAL 10 /**< TTI value for 10 ms */
#define L1_TTI_20MS_VAL 20 /**< TTI value for 20 ms */
#define L1_TTI_40MS_VAL 40 /**< TTI value for 40 ms */
#define L1_TTI_80MS_VAL 80 /**< TTI value for 80 ms */

/** Macro to translate TTI enum to its absolute value */
#define MCALWCDMA_DEC_TTI_VAL(tti_enum) mcalwcdma_dec_tti_val_table[(tti_enum)]


/** Number of decoder related HSDPA information */
#define MCALWCDMA_DEC_HS_INFO_COUNT 2

 /** Number of MAC-hs queue for a HSDPA information table */
 #define MCALWCDMA_DEC_HS_MAC_HS_Q_MAX_COUNT 8

/* The final number for this depends on what kind of network configurations we end up 
 * seeing in practice. Being very conservative, I would not expect more than 1 Q for SRB and 
 * max 3 for data (1 I/B, 1 streaming and 1 conv bearer). Further, I would not expect all of 
 * these to be configured with max window - and finally chances that they all stall at the max 
 * window at the same time is very small. For that reason I would budget for 32 x 4 elements,
 * instead of 32 x 16, which I still consider very conservative.
 */
#define MCALWCDMA_DEC_MAX_AVG_HS_Q 4

/* TSN window size used at MAC */
#define MCALWCDMA_DEC_TSN_WIN_SIZE 32

/* 3GPP 25.321 Spec allows 70 MAC-d PDUs per MAC-hs transport block */
#define MCALWCDMA_DEC_MAX_DL_RLC_PDU 70

/* Maximum LC ID per Rel-7 Spec 25.321 */
#define MCALWCDMA_DEC_MAX_LC_ID 16

#ifdef FEATURE_MIMO
#define MCALWCDMA_DEC_MAX_HS_FR_PER_10MS_TTI 10
#else
#define MCALWCDMA_DEC_MAX_HS_FR_PER_10MS_TTI 5
#endif /* FEATURE_MIMO */

#ifdef FEATURE_HSDPA_MAC_EHS

#define MCALWCDMA_DEC_INVALID_Q_ID 8

#if defined FEATURE_WCDMA_3C_HSDPA
/* Max possible Reordering SDUs per HS TrBlk for 3C/4C */
#define MCALWCDMA_DEC_HS_REORDER_SDU_MAX_COUNT 44
#else
/* Max possible Reordering SDUs per HS TrBlk for SC/DC */
#define MCALWCDMA_DEC_HS_REORDER_SDU_MAX_COUNT 26
#endif


/* Macro for MAC to use to obtain DOB extension buffer size.
 * This defines the number of bytes available in DecOB for 
 * concatenating the partial PDUs to perform deciphering. 
 * Max possible RLC PDU size is 1504 bytes. For MIMO scenario,
 * there can be 10 * 1504 bytes every 10ms to concatenate, which
 * is equal to 15040 bytes. Set 16000 bytes to be on safer side.
 * Do not exceed 16000 bytes since copy engine has limitation of
 * 8K bytes per transfer and we are programming two copy
 * repeat tasks. */
#define MCALWCDMA_DEC_DOB_EXTN_BUF_SIZE 16000

/* The maximum number of reordering pdu in one transport block, currently 3 per 
 * 3GPP 25.321 Mac spec. */
#define MCALWCDMA_DEC_MAX_REORD_PDU_PER_HS_FRAME 3

/* The maximum number of partial pdu per reordering pdu, currently 2 per
 * 3GPP 25.321 Mac spec. */
#define MCALWCDMA_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU 2

/* We can have 2 complete pdus for each reordering queue in the previous TTI,
 * total = 3 * 2 * 5 for Non-MIMO
 * total = 3 * 2 * 10 for MIMO */
#define MCALWCDMA_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI \
            (MCALWCDMA_DEC_MAX_REORD_PDU_PER_HS_FRAME * \
             MCALWCDMA_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU * \
             MCALWCDMA_DEC_MAX_HS_FR_PER_10MS_TTI)

/** This macro indicates the total number of TSNs for which
 * complete PDUs were formed from previous TTIs partial PDUs. L1
 * retrieves a buffer from the Free Q, populates it and enqueues
 * into a Q to MAC. This macro is triple-buffered since
 * if MAC-hs task does not get a chance to run for three 10ms
 * frames, even then the information can be held in the
 * queue. */
#define MCALWCDMA_DEC_TOTAL_MAX_COMPLETE_PDU_FROM_PREV_TTI (MCALWCDMA_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI*3)


#if defined FEATURE_WCDMA_3C_HSDPA
#define MCALWCDMA_DEC_MAX_TSN_INFO_BUF 450
#elif defined FEATURE_WCDMA_DC_HSDPA
/* This count is to accommodate RLC not getting a chance to 
   run very frequently at peak data rates. */
#define MCALWCDMA_DEC_MAX_TSN_INFO_BUF 300
#else
#define MCALWCDMA_DEC_MAX_TSN_INFO_BUF ((MCALWCDMA_DEC_TSN_WIN_SIZE * MCALWCDMA_DEC_MAX_AVG_HS_Q) + \
                                         MCALWCDMA_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI)
#endif

/* 3GPP 25.321 Spec allows 26 RLC pdus per MAC-ehs transport block */
#define MCALWCDMA_DEC_MAX_DL_RLC_PDU_REL7 26

/** Based on MAX IP Pkt size of 1500 bytes and RLC PDU size
 *  of 40 bytes, N/W can split up a PDU into 38 partial PDUs.
 *  MAC sends concatenation information to L1 once every
 *  10ms, so the number of maximum partial PDUs across all
 *  reordering PDUs can be computed based on the the max number
 *  of reordering PDU per HS TTI and the number of HS TTI per
 *  10ms. */
#define MCALWCDMA_DEC_MAX_RLC_PDU_SEGMENTATION 38
#define MCALWCDMA_DEC_MAX_RLC_PDU_SEGMENTATION_PER_10MS_TTI \
                  (MCALWCDMA_DEC_MAX_RLC_PDU_SEGMENTATION * \
                   MCALWCDMA_DEC_MAX_REORD_PDU_PER_HS_TTI * \
                   MCALWCDMA_DEC_MAX_HS_FR_PER_10MS_TTI)

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
#define MCALWCDMA_DEC_MAX_DMOV_SOURCE_DESCRIPTOR_LEN_PARTIAL \
              ((MCALWCDMA_DEC_MAX_RLC_PDU_SEGMENTATION * \
                MCALWCDMA_DEC_MAX_REORD_PDU_PER_HS_TTI * \
                MCALWCDMA_DEC_MAX_HS_FR_PER_10MS_TTI) + 1)

#define MCALWCDMA_DEC_MAX_DMOV_SOURCE_DESCRIPTOR_LEN_MOVE_IN 100

/** Since partial PDUs will be concatenated in the DOB
 *  extension buffer, there is only one destination address
 *  that needs to be specified + 1 descriptor for the WR_PTR
 *  address. */
#define MCALWCDMA_DEC_MAX_DMOV_DESTINATION_DESCRIPTOR_LEN_MOVE_IN 4

#else /* FEATURE_HSDPA_MAC_EHS */

#define MCALWCDMA_DEC_MAX_TSN_INFO_BUF (MCALWCDMA_DEC_TSN_WIN_SIZE * MCALWCDMA_DEC_MAX_AVG_HS_Q)

#endif /* FEATURE_HSDPA_MAC_EHS */


/* Total number of CCTrCh table in mDSP */
#define MCALWCDMA_DEC_TOTAL_CCTRCH_TABLES 6
/* MCALWCDMA_DEC_TOTAL_CCTRCH_TABLES - 4 (BCH) */
#define MCALWCDMA_DEC_TOTAL_CCTRCH_TABLES_NON_BCH 2

/** Indicate invalid MCAL WCDMA decoder CCTrCh Id value */
#define MCALWCDMA_DEC_INVALID_CCTRCH_ID_VAL 0xFF

/** Maximum number of DOB handles */
#define MCALWCDMA_DEC_MAX_DOB_HANDLES (WFW_DBACK_NON_HS_DECOB_STATUS_FIFO_SIZE+WFW_DBACK_HS_DECOB_STATUS_FIFO_SIZE)

/** Number of W32 to be read in transport block header information */
#define MCALWCDMA_DEC_TB_HDR_SIZE_W32 3
/** Number of word 32 data reads required for HS-DSCH MAC-d PDU */
#ifdef FEATURE_WCDMA_HS_FACH
/* Need two Word32 so that UE-ID is also read and given to MAC */
#define MCALWCDMA_DEC_HS_MAC_D_HDR_SIZE_W32 2
#else
#define MCALWCDMA_DEC_HS_MAC_D_HDR_SIZE_W32 1
#endif

/** This is the maximum number of transport block set for which the transport
 * block headers can be reported by L1 to L2 for decoding
 * Note that size defined is based on the maximum amout that can be
 * reported by mDSP. SW has to process that and should have that much capacity.
 * This is not as per UE capability specified in standard */
#define MCALWCDMA_DEC_MAX_TB_SET_HDR 32

/* Total number of TrCh table in mDSP */
#define MCALWCDMA_DEC_TOTAL_TRCHT_PER_CCTRCH 12

/** 32 R99 Max TB ina frame for all TTI ending at this frame (No double buffering)
 * Max 70 PDU per sub frame Number of sub frames to cover are 5 (in 10 ms) +
 * 2 (To cover interrupt latency
 * Total transport block (MAC-d PDUs) will be 32 + 7*70 = 524 */
#define MCALWCDMA_DEC_MAX_TB 524

/** @name DOB status field valid bit mask indicator */
/**@{*/ 
/** Indicate CRC valid */
#define MCALWCDMA_DEC_DOB_STATUS_CRC_RESULT_VALID_BMSK 0x01
 /** Indicate path energy matric valid */
#define MCALWCDMA_DEC_DOB_STATUS_EGY_MATRIC_VALID_BMSK 0x02
/** Indicate symbol error valid */
#define MCALWCDMA_DEC_DOB_STATUS_SYM_ERR_VALID_BMSK    0x04
 /** Indicate quality bits valid */
#define MCALWCDMA_DEC_DOB_STATUS_QUAL_BITS_VALID_BMSK  0x08
/**@}*/

/** TBset header strctuture member access macro. It takes care of any
 * featurization and provide access to appropriate member based on
 * featurization */
#ifdef __GNUC__

 #define MCALWCDMA_DEC_TB_SET_HDR(hdr_struct_ptr,member) \
  hdr_struct_ptr->info.non_hs.member

#else /* __GNUC__ */

 #define MCALWCDMA_DEC_TB_SET_HDR(hdr_struct_ptr,member) \
  hdr_struct_ptr->info.non_hs.##member

#endif /* __GNUC__ */

/** Number of TPC reject logged per TPC index */
#define MCALWCDMA_DEC_LOG_MAX_TPC_INFO 6

/** Number of bytes in cipheting key stream */
#define MCALWCDMA_DEC_NUM_BYTES_CIPHER_KEY 16 /* 128 bit key */


/* Macro for clearing deciphering done interrupt flag */
#define DEC_CLEAR_DECIPHER_DONE_INT() \
 WL1_HWIO_OUT(BACKUP_INT_CLEAR, WL1_HWIO_FMSK(BACKUP_INT_CLEAR, DECIPH_DONE_INT_CLR));

#define DEC_QUERY_DECIPHER_DONE_INT_STATUS() \
 (WL1_HWIO_IN(BACKUP_INT_STATUS) & WL1_HWIO_FMSK(BACKUP_INT_STATUS, DECIPH_DONE_INT_STATUS))

/* Macro indicating how many Decode Done ISRs can be skipped
   before timing out, waiting for A2 DL PHY transaction to complete 
   There are 32 DOB entries so even if 20 entries are skipped, SW 
   should be able to catch up with FW */
#define MCALWCDMA_DEC_DECODE_DONE_ISR_MAX_SKIP_CNT 20

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/** MCAL WCDMA decoder CCTrCh id type that is returned for CCrCh setup request */
typedef uint8 mcalwcdma_dec_cctrch_id;

/** MCAL WCDMA decoder DOB status handle type definition */
typedef uint32 mcalwcdma_dec_dob_status_handle_type;

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */

#ifdef FEATURE_HSDPA_MAC_EHS
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
typedef enum
{
  COMPLETE_PDU,
  PARTIAL_FIRST,
  PARTIAL_MIDDLE,
  PARTIAL_LAST
} mcalwcdma_dec_rlc_pdu_segmentation_type;
#endif /* FEATURE_HSDPA_MAC_EHS */

#ifdef FEATURE_WCDMA_HS_FACH
/* This enum defines the type of H-RNTI used
   to decode the HS TrBlk */
typedef enum
{
  L1_DL_BCCH_H_RNTI_TYPE,
  L1_DL_COMMON_H_RNTI_TYPE,
  L1_DL_DEDICATED_H_RNTI_TYPE,
  L1_DL_BLIND_DECODING_TYPE
} mcalwcdma_dec_e_fach_h_rnti_type;
#endif /* FEATURE_WCDMA_HS_FACH */


/*----------------------------------------------------------------------------
   Enum indicated by L1 to MAC along with HS data inidcation in order to make 
   decision for processing the current, prev or both TTIs
 -----------------------------------------------------------------------------*/
/* HSDPA Action Variable */
typedef enum
{
  MACDL_HS_CURR_TTI,
  MACDL_HS_PREV_TTI,
  MACDL_HS_CURR_PREV_TTI,
  MACDL_HS_NONE
} mcalwcdma_dec_mac_hs_tti_type;





/** Transparant mode deciphering states for various transport block */
typedef enum
{
  L1_TB_TM_CIPHER_NOT_APPLICABLE, /**< Block not ciphering in TM mode */
  L1_TB_TM_CIPHER_START,          /**< First block iciphered in TM mode */
  L1_TB_TM_CIPHER_CONT,           /**< Continue ciphering key stream generation */
  L1_TB_TM_CIPHER_STOP,           /**< Last block iciphered in TM mode */
  L1_TB_TM_CIPHER_SINGLE          /**< Single block in TM mode so don't */
} mcalwcdma_dec_tb_tm_cipher_state_enum_type;

typedef enum 
{
  DL_EVEN_CFN_PCCPCH,
  DL_ODD_CFN_PCCPCH,
  DL_BOTH_CFN_PCCPCH
} dl_cctrch_cfn_type ;


/** Strcuture for DOB status information */ 
typedef struct
{
  uint32 crc_bits;

  uint16 energy_metric; /**< energy metric of the TB set */
  uint16 sym_err_count; /**< symbol error count for TB set */
  /** CRC error bits of each TB. MSB is last block received in this TBset */

  uint16 quality_bits;  /**< quality bits for TB set */
  /** Valid bit mask for various information below. */
  uint8 valid_bit_mask;

  /** Coding type of transport chanel */
  mcalwcdma_dec_coding_enum_type coding_type;

  boolean zsb;          /**< zero state bit */
  uint32 zsb_word;
} mcalwcdma_dec_dob_status_type;


/** This structure defines information for HSDPA MAC-d PDU size */ 
typedef struct
{
  /** indicate this Size index Id is valid */
  boolean valid;
  /** MAC-d PDU size for this SID */
  uint16 size;
} mcalwcdma_dec_hs_mac_d_pdu_sz_info_struct_type;

/** This structure defines information for HSDPA Q information for its MAC-d PDI size */ 
typedef struct
{
  /** indicate this Q Id is valid */
  boolean valid;

  /** indicate Size info for each SID (Size identifier)
   * SID goes with index of this array */
  mcalwcdma_dec_hs_mac_d_pdu_sz_info_struct_type
    mac_d_pdu_sz_info[MCALWCDMA_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT];

} mcalwcdma_dec_hs_mac_d_pdu_sz_q_info_struct_type;

/** This structure defines the information for HSDPA information table for
 * getting MAC-d PDU out of MAC-HS block */
typedef struct
{
  /** MAC-d queue information for this HS table */
  mcalwcdma_dec_hs_mac_d_pdu_sz_q_info_struct_type
    mac_d_pdu_sz_q_info[MCALWCDMA_DEC_HS_MAC_HS_Q_MAX_COUNT];
} mcalwcdma_dec_hs_info_struct_type;


/* Structures for Header indication to L2 */

/** This structure defines the ciphering parameters for a transport block
 * header information of a transport block */
typedef struct
{
  /** Ciphering enabled or not for this 
   * transport block. */
  boolean ciphering_control;
  /** Ciphering algorithm specified by L2 for this
   * transport block. */
  mcalwcdma_dec_ciphering_algo_enum_type ciphering_algo;
  
  /** Ciphering bit offset returned by L2 of each transport block. */
  uint8 ciphering_bit_offset;
  
  /** Ciphering key id returned by L2 of this transport block. */
  uint8 ciphering_key_id;
  
  /** Ciphering count (CFN/SFN) returned by L2 of this transport block. */
  uint32 ciphering_count;
  
  /** Ciphering radio bearer id returned by L2 for this transport block. */
  uint8 ciphering_radio_bearerid;
} mcalwcdma_dec_ciphering_param_struct_type;

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

  /* Flag to discard the DL PDUs received while the RESET ACK was being processed */
  boolean discard_l2_pdu;
  
  /** TB index of this TB */
  uint8 tb_idx; 
  
  /** Logical channel ID for MAC to fill up in Rel5-Rel6 case,
   *  irrespective of C/T mux. */
  uint8 lc_id;

  /** RLC Id corresponding to lc_id - to be filled by MAC 
   * This is needed to pass to RLC because RLC doesnt have notion of
   * lc_id. It only understands rlc_id */
  uint8 rlc_id;

  /** Bit offset from the starting transport block. The offset specified in
   * in the number of bits. The L2 function that decode the header, reports
   * back the offset to read the transport block data in this strcuture member */
  uint8 num_bit_offset_trblk;
  
  /** ciphering parameters for this transport block */
  mcalwcdma_dec_ciphering_param_struct_type ciphering_param;
  
  /** Pointer to the transport block header */
  uint8 tb_header[MCALWCDMA_DEC_TB_HDR_SIZE_W32 * 4];
} mcalwcdma_dec_tb_hdr_struct_type;


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
} mcalwcdma_dec_iovec_type;

/* Each complete pdu will reside in one dsm item. The dsm item is large enough to hold
multiple complete pdus. rlc_hdr_data is the demulitplexed header portion from the rlc pdu
these are separate so that the data can be contiguous and can be shipped to higher layers
avoiding a dsm copy and a new dsm alloc */

typedef struct
{
  mcalwcdma_dec_iovec_type iovec; /* IOVEC for this complete PDU */

  uint32 rlc_hdr_data; /* RLC Header contents */

  /* For rel-7 LC_ID is known to L1 while doing the header read event. For rel-6 the MAC will
  pass the LC_ID information at the end of the header read event. DSM items are allocated
  per lc_id basis hence L1 needs to be aware of the lc_id for rel6 too.
  This is NW configured Logical Channel ID <1..15> and not internal RLC ID */
  uint8 rlc_id; /* Logical Channel ID */
} complete_pdu_type;

#ifdef FEATURE_HSDPA_MAC_EHS
typedef struct
{
  mcalwcdma_dec_iovec_type iovec; /* IOVEC for this partial PDU */

  /* L1 uses this variable to indicate to MAC whether
     RLC PDU header is received completely or not. */
  boolean complete_rlc_hdr_rcvd;
  
  uint32 rlc_hdr_data; /* RLC Header contents */
  
  /* This indicates if the demultiplexed PDU is a partial first, middle or last*/
  mcalwcdma_dec_rlc_pdu_segmentation_type segmentation_info; /* Segmentation indication */

  uint8 lc_id; /* Logical Channel ID to which this pdu belongs*/
} partial_pdu_type;
#endif

/** Enum type definition for TSN trace enum type */
typedef enum {
  DL_TSN_FREE_Q = 1, /**< TSN is in Dec TSN info free Q */
  DL_TSN_L1_INT_Q,   /**< TSN is in L1 Internal Q */
  DL_TSN_INFO_Q,     /**< TSN is in Dec TSN info Q */
  DL_TSN_MAC_INT,    /**< TSN is in MAC internal structures */
  DL_TSN_RLC_Q,      /**< TSN is in RLC TSN info Q */
  DL_TSN_RLC         /**< TSN is in RLC */
} mcalwcdma_dec_tsn_trace_enum_type;

/** If a TSN structure passed to MAC has both complete
 *  and partial pdu info, then RLC should not release the
 *  structure. MAC will have to free this structure when the
 *  partial PDUs are taken care of. */
typedef struct
{
  q_link_type link; /**< Link to manage this item on the queue */
  uint8 index;      /**< index of this element in array */

  /* Indicates which module is holding the TSN items */
  mcalwcdma_dec_tsn_trace_enum_type tracer;

  /** UE can receive Complete PDU, Complete PDU and Partial Start
   *  of another PDU in that order. But this structure
   *  organization destroys the original order. Seems ok from MAC
   *  point of view as of now, the order is not important. */

  /* TSN decoded as part of the Header Read event 
     14 bits for >= 3C and 6 bits for < 3C */
  uint16 tsn;

  /* Queue ID decoded as part of the Header Read event */
  uint8 queue_id;

  /** TB Id. This is set by MAC during Header read event. Its
   * passed back to MAC in data indication */
  uint16 tb_id;

  /** Flag for MAC usage */
  boolean received;

 #ifdef FEATURE_WCDMA_HS_FACH
  /* Enum to indicate H-RNTI used to decode this block */
  mcalwcdma_dec_e_fach_h_rnti_type h_rnti_type;
 #endif

  /* While freeing this structure (either MAC/RLC), set this field to 0.
   * L1 will set them to zero also, during Q init. */
  uint8 num_complete_pdus;
  
 #ifdef FEATURE_HSDPA_MAC_EHS
  /* While freeing this structure (either MAC/RLC), set this field to 0.
   * L1 will set them to zero also, during Q init. */
  uint8 num_partial_pdus;
 #endif

  complete_pdu_type complete_pdu[MCALWCDMA_DEC_MAX_DL_RLC_PDU];

 #ifdef FEATURE_HSDPA_MAC_EHS
  /*UE_MAX_PARTIAL_PDU_PER_REORDER_PDU=2 since in a reordering pdu either the
  first mac-ehs sdu could be a partial one and/or the last mac-ehs sdu in the
  reordering pdu could be a partial one*/
  partial_pdu_type partial_pdu[MCALWCDMA_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU];
 #endif
} mcalwcdma_dec_tsn_info_type;

/** This structure defines the DSM information per logical
 *  channel. */
typedef struct
{
  /** DSM item being populated */
  dsm_item_type *current_dsm_ptr;

  /** Offset in the DSM ptr where the next TB data can be
   *  populated (units of bytes). */
  uint16 offset;
} mcalwcdma_dec_dsm_info_type;

#ifdef FEATURE_HSDPA_MAC_EHS

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
  mcalwcdma_dec_tsn_info_type *tsn_info_buf;

  /** ciphering parameters for this RLC PDU */
  mcalwcdma_dec_ciphering_param_struct_type ciphering_param;
} mcalwcdma_dec_partial_rlc_pdu_hdr_struct_type;

/** This structure defines the information for all RLC PDUs that can
 *  be constructed out of multiple partial PDUs. This structure
 *  also contains the information required to get the data
 *  from the hardware. */
typedef struct
{
  /** Number of complete RLC PDUs that can be formed */
  uint16 num_complete_pdus;

  /* for each complete RLC PDU that is being formed */
  mcalwcdma_dec_partial_rlc_pdu_hdr_struct_type rlc_pdu_hdr_info[MCALWCDMA_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI];
} mcalwcdma_dec_rlc_pdu_concatenate_info_struct_type;

typedef struct
{
  /* For rel-7 LC_ID is known to L1 while doing the header read event. For rel-6 the MAC will
  pass the LC_ID information at the end of the header read event. DSM items are allocated
  per lc_id basis hence L1 needs to be awar of the lc_id for rel6 too*/
  uint8 lc_id;        /* Logical Channel ID */
  uint16 tsn;          /* Transmission seq number 14 bits for >= 3C and 6 bits for < 3C */
  uint16 length;      /* Length of MAC e-HS SDU in bytes*/
  mcalwcdma_dec_rlc_pdu_segmentation_type segmentation_info; /* Complete or Partial PDU */
} mcalwcdma_dec_mac_ehs_sdu_hdr_type;

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
  mcalwcdma_dec_tsn_info_type *tsn_info_buf;
} mcalwcdma_dec_concatenated_pdu_tsn_type;

extern mcalwcdma_dec_dsm_info_type partial_pdu_dsm_info[MCALWCDMA_DEC_MAX_LC_ID];

/** This array stores the TSNs for which complete PDUs were
 * formed from previous TTIs partial PDUs. L1 retrieves a buffer
 * from the Free Q, populates it and enqueues into a Q to MAC.
 * This data structure is triple-buffered since if MAC-hs
 * task does not get a chance to run for three 10ms frames, even
 * then the information can be held in the queue. */
extern mcalwcdma_dec_concatenated_pdu_tsn_type 
  mcalwcdma_dec_concatenated_pdu_tsn_info[MCALWCDMA_DEC_TOTAL_MAX_COMPLETE_PDU_FROM_PREV_TTI];

/* Q of TSN value structures */
/* L1 pulls structures out of Free Q, fills it and enqueues them in Q */
/* MAC uses the structures in Q and puts them back in Free Q */
extern q_type dec_concatenated_pdu_tsn_q;
extern q_type dec_concatenated_pdu_tsn_free_q;
#endif /* FEATURE_HSDPA_MAC_EHS */

/* DL cipher information */
extern l1_cipher_key_info_struct_type dec_cipher_info;

/** This structure defines the transport block header information for a
 * transport block set (or a transport channel) */
typedef struct
{
  /** Number of transport blocks in this transport block set. */
  uint8 num_tb;
  
  /** CCTrCh Ids for the transport block set. This is set by WHL1 */
  uint8 cctrch_id;
  
  /** TrCh reference type for the transport block set */
  mcalwcdma_dec_trch_ref_type_enum_type trch_ref_type;
  

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
      uint8 num_blk[MCALWCDMA_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT];
      /** SID value for each k */
      uint8 sid_val[MCALWCDMA_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT];
    } hs;

   #ifdef FEATURE_HSDPA_MAC_EHS
    /** E-HS channel information */
    struct
    {
      uint16 sub_frame_num; /* Starting Sub frame number when TTI is received */
      uint8 num_reorder_sdu; /* Number of Reordering SDUs in this HS TTI */
     #ifdef FEATURE_WCDMA_HS_FACH
      /* Enum to indicate H-RNTI used to decode this block */
      mcalwcdma_dec_e_fach_h_rnti_type h_rnti_type;
     #endif
      /* LC-ID, Length, TSN and Segmentation Type of each MAC-eHS SDU in this HS TTI */
      mcalwcdma_dec_mac_ehs_sdu_hdr_type mac_ehs_sdu_hdr[MCALWCDMA_DEC_HS_REORDER_SDU_MAX_COUNT];
    } ehs;
   #endif /* FEATURE_HSDPA_MAC_EHS */
  } info;


  /** indicate that TBs in this TB set are in transparent mode */
  boolean tb_in_tm_mode;
  /** Pointer to the structure array of information of all
    transport block header in this TB set */
  mcalwcdma_dec_tb_hdr_struct_type *tb_hdr_info;
} mcalwcdma_dec_tb_set_hdr_struct_type;

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
  mcalwcdma_dec_tb_set_hdr_struct_type tb_set_hdr[MCALWCDMA_DEC_MAX_TB_SET_HDR];

 #ifdef FEATURE_HSDPA_MAC_EHS
  /** Information for all RLC PDUs that can be constructed out of
   *  multiple partial PDUs. */
  mcalwcdma_dec_rlc_pdu_concatenate_info_struct_type rlc_pdu_concatenate_info;
 #endif
} mcalwcdma_dec_tb_hdr_decode_struct_type;

/* Structures for Data indication to L2 */

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
} mcalwcdma_dec_tb_data_in_tbset_struct_type;

/** This structure defines the data and status element for a transport block set */
typedef struct
{
  /** TrCh reference type for the transport block set */
  mcalwcdma_dec_trch_ref_type_enum_type trch_ref_type;

  /** Number of transport blocks in this transport block set. */
  uint8 num_tb;
  
  /** Confidence in the Decode result - True: Good, False: Bad */
  boolean decode_result;
  
  /** transport block size for the transport block of this trblk set */
  uint16 trblk_size;
  
  /** points to the array of the TB info structure */
  mcalwcdma_dec_tb_data_in_tbset_struct_type *tb_data_info;
} mcalwcdma_dec_tb_set_data_struct_type;

/** This structure defines the transport block information that is used to get
 * the transport block headers decoded and update the transport block
 * information to the L2 buffers */
typedef struct
{
  /** Number of transport block set for which the data and status is to be
     transferred to L2. 
     If the feature FEATURE_WCDMA_DL_DATA_PATH_OPT is defined, field is valid
     only for R99 data. If only HS data is present, it will be set to zero.*/
  uint8 num_tb_set;
  
  /** Data and status for all transport blocks sets */
  mcalwcdma_dec_tb_set_data_struct_type tb_set_data[MCALWCDMA_DEC_MAX_TB_SET_HDR];
} mcalwcdma_dec_tb_data_struct_type;


/* This array stores the TSN information.
 * L1 retrieves a buffer from the Free Q, populates it and enqueues into a Q to MAC.
 * The contents of this buffer will not be copied locally in MAC or RLC,
 * All data in this buffer is reused by MAC & RLC */
#ifdef FEATURE_WCDMA_L1_ZI_OPT
extern mcalwcdma_dec_tsn_info_type *mcalwcdma_dec_tsn_info_buf[MCALWCDMA_DEC_MAX_TSN_INFO_BUF];
#else
extern mcalwcdma_dec_tsn_info_type mcalwcdma_dec_tsn_info_buf[MCALWCDMA_DEC_MAX_TSN_INFO_BUF];
#endif

/* Q of TSN info structures */
/* L1 pulls structures out of Free Q, fills it and enqueues them in Q */
/* MAC/RLC use the structures in Q and puts them back in Free Q */
extern q_type dec_tsn_info_q;
extern q_type dec_tsn_info_free_q;


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
  uint16 num_reject_tpc[MCALWCDMA_DEC_LOG_MAX_TPC_INFO];
} mcalwcdma_dec_power_control_info_struct_type;

#ifdef FEATURE_DL_PATH_PROFILING
typedef struct
{
  uint32 time_trigger;
  uint32 time_cb_return;
  uint32 num_src_des;
  uint32 num_dest_des;
  uint32 bits_transfer;
  uint32 num_p2c;
  uint32 total_len_p2c;
} dec_dmov_cb_type;

typedef struct
{
  uint32 cfn;
  uint32 cx1;
  uint32 rd_idx;
  uint32 wr_idx;
  uint32 start_addr;
  uint32 hs_trch_type;
  uint32 fw_nonhs_counter;
  uint32 sw_nonhs_counter;
  uint32 fw_hs_counter;
  uint32 sw_hs_counter;
  uint32 timetick;
} dec_decode_done_isr_type;

typedef struct
{
  uint32 decode_start;
  uint32 decode_step2;
  uint32 decode_mid;
  uint32 decode_end;
} dec_decode_done_isr_start_end_type;

typedef struct
{
  uint32 trigger_ts;
  uint32 cb_ts;
  uint32 idx;
  uint8 num_ptoc;
} dec_gather_and_decipher_struct_type;

typedef struct
{
  uint8  num_non_hs_dob_handles;
  uint8  num_hs_dob_handles;
  uint8  num_partial;
  uint8  num_complete;
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
} dec_hdr_rd_isr_struct_type;

typedef struct
{
  uint32 cfn;
  uint32 cx1;
  uint32 timetick;
} dec_data_rd_struct_type;

typedef struct
{
  uint32 dma_done;
  uint32 tsn_enqueue_done;
  uint32 l2_ind_done;
} dec_dma_from_dob_struct_type;

extern dec_hdr_rd_isr_struct_type dec_hdr_rd_isr[];
extern dec_data_rd_struct_type dec_data_rd[];

extern uint8 dec_hdr_rd_idx;
extern uint8 dec_data_rd_idx;
#endif


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Flag indicating if target logging CFN is beyond combiner enable CFN*/
 extern boolean mcalwcdma_dec_skip_tfci_logging[MCALWCDMA_DEC_TOTAL_CCTRCH_TABLES]; 

/* This is the CFN adjustment from the CFN of the last frame
   of the TTI to reach at first CFN of the TTI */
extern const uint8 mcalwcdma_dec_cfn_adjust_for_tti[L1_NUM_TTI];

/* CRC length value corresponding to CRC enum type */
extern const uint8 mcalwcdma_dec_crc_length_val_table[L1_NUM_CRC_LENGTH];

/* TTI value corresponding to TTI enum type */
extern const uint16 mcalwcdma_dec_tti_val_table[L1_NUM_TTI];

/* Mutex for DOB read functionality */

/* Debug var for DL datapath F3s */
extern uint16 datapath_f3_debug;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*============================================================================*/
/**
 * This function initializes the variables related to decoder module
 */
/*============================================================================*/

extern void mcalwcdma_dec_init(void);

extern void mcalwcdma_dec_idle_init(void);

/*===========================================================================
FUNCTION mcalwcdma_decdata_power_up_init

DESCRIPTION
  This function initializes the variables related to decoder module once 
  upon UE power-up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcalwcdma_decdata_power_up_init(void);

/*===========================================================================
FUNCTION mcalwcdma_decdata_init

DESCRIPTION
  This function initializes the variables related to decoder module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcalwcdma_decdata_init(void);

extern void mcalwcdma_decdata_idle_init(void);

#ifdef FEATURE_WCDMA_L1_ZI_OPT
/*===========================================================================
FUNCTION mcalwcdma_decdata_deinit

DESCRIPTION
  This function de-initializes the variables related to decoder module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcalwcdma_decdata_deinit(void);
#endif

/*===========================================================================

FUNCTION mcalwcdma_dec_get_turbo_coding_configured

DESCRIPTION
   This function returns TRUE if only convolutional coding is used on all the
   TrCh across all active CCTrCh. 

DEPENDENCIES
   None.

RETURN VALUE
   TRUE is only convolutional coding used for all R99 TrCh
   FALSE otherwise

SIDE EFFECTS
   None.

===========================================================================*/

extern boolean mcalwcdma_dec_get_turbo_coding_configured (void);

/*===========================================================================

FUNCTION mcalwcdma_dec_get_conv_coding_configured

DESCRIPTION
   This function returns TRUE if convolutional coding is used on atleast one
   TrCh across all active CCTrCh.

DEPENDENCIES
   None.

RETURN VALUE
   TRUE is convolutional coding used for at least one R99 TrCh
   FALSE otherwise

SIDE EFFECTS
   None.

===========================================================================*/

extern boolean mcalwcdma_dec_get_conv_coding_configured (void);

/*===========================================================================

FUNCTION mcalwcdma_dec_get_turbo_coding_enabled

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

extern boolean mcalwcdma_dec_get_turbo_coding_enabled (void);

/*===========================================================================
FUNCTION mcalwcdma_dec_get_bch_drmb_size

DESCRIPTION
  This function returns the DRMB size needed for PCCPCH CCTrCh.

DEPENDENCIES
  None

RETURN VALUE
  DRMB size needed for PCCPCH.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 mcalwcdma_dec_get_bch_drmb_size(void);

extern void mcalwcdma_dec_deregister_cd_client(void);

/*============================================================================*/
/**
 * This function validate CCTrCh setup parameters for their valiidity. It do
 * all range checks other things that bound mDSP and ARM interface.
 *
 * @return TRUE or FALSE based on validation success.
 * indicator
 */
/*============================================================================*/

extern boolean mcalwcdma_dec_validate_cctrch_setup(
  /** CCTrCh info structure pointer */
  mcalwcdma_dec_cctrch_info_struct_type *cctrch_info,
  /** TFC information for this CCTrCh */
  mcalwcdma_dec_ctfc_info_struct_type *ctfc_info);

/*============================================================================*/
/**
 * This function setup CCTrCh table in mDSP. It computes rate maching parameters,
 * coding parameters, selects CCTrCh table index in mDSP to be setup with these
 * parameters and uploads the contents to mDSP.
 * If there is associated TFC table with CcTrCh. It finds free TFC table index
 * and upload TFC information in that table.
 *
 * @return MCAL WCDMA decoder CCTrCh Id for the CcTrCh setup. If there is failure
 * to setup CCTrCh, it returns MCALWCDMA_DEC_INVALID_CCTRCH_ID_VAL as error
 * indicator
 *
 * @see MCALWCDMA_DEC_INVALID_CCTRCH_ID_VAL
 */
/*============================================================================*/

extern void mcalwcdma_dec_setup_pccpch_table_mdsp(
  /** MCAL WCDMA decoder CCTrCh Id to be setup for PCCPCH */
  mcalwcdma_dec_cctrch_id cctrch_idx,
  /* Even, odd or both CFN to be enabled/disabled */
  dl_cctrch_cfn_type cfn_type,
  /* Enable/disable state */
  boolean state);

extern mcalwcdma_dec_cctrch_id mcalwcdma_dec_cctrch_setup(
  /** number of bits that can be in the physical channel(s) that are
   * associated with this CCTrCh */
  uint32 physical_channel_bits,
  /** CCTrCh info structure pointer */
  mcalwcdma_dec_cctrch_info_struct_type *cctrch_info,
  /** TFC information for this CCTrCh */
  mcalwcdma_dec_ctfc_info_struct_type *ctfc_info
  );

/*============================================================================*/
/**
 * This function frees CCTrCh table Id and mark correspondig CCTrCh table index
 * and DRM buffer area available to use.
 */
/*============================================================================*/

extern void mcalwcdma_dec_cctrch_free(
  /** MCAL WCDMA decoder CCTrCh Id to be freed */
  mcalwcdma_dec_cctrch_id cctrch_id);

extern boolean mcalwcdma_dec_enable_disable_trch(
  /* CCTrCh table index */
  uint8 cctrch_idx,
  /* TrCh index in this CCTrCh */
  uint8 trch_idx,
  /* New TrCh state, 0: disable, 1: enable */
  boolean state);


/*============================================================================*/
/**
 * This function saves HSDPA channel decode related information. Information
 * is copied into internal structures and is marked valid.
 */
/*============================================================================*/

extern void mcalwcdma_dec_hs_update_hs_info(
  /** HSDPA info Id to update information */
  uint8 dec_hs_info_id,
  /** HADPA decode related information structure pointer */
  mcalwcdma_dec_hs_info_struct_type *dec_hs_info);

/*============================================================================*/
/**
 * This function frees HSDPA table decode info and mark correspondig is invalid
 */
/*============================================================================*/

extern void mcalwcdma_dec_hs_free_hs_info(
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

extern boolean mcalwcdma_dec_query_handle_type_hs(
  /** DOB status handle to query for HS type */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get HS channel info Id for this DOB status handle. DOB status handle must be
 * checked for it to be HS type. No validation is done in this function.
 *
 * @return HS channel info Id for this channel that was returned at the time of
 * HS channel setup.
 */
/*============================================================================*/

extern uint8 mcalwcdma_dec_get_hs_info_id(
  /** DOB status handle for which to get HS info Id */
  mcalwcdma_dec_dob_status_handle_type handle);

extern uint16 mcalwcdma_dec_get_hs_subframe_num(
  mcalwcdma_dec_dob_status_handle_type handle);


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

extern boolean mcalwcdma_dec_trch_idx_exists(uint8 cctrch_idx, uint8 trch_idx);

extern uint8 mcalwcdma_dec_get_cctrch_info_id(
  /** DOB status handle for which to get CCTrCh info Id */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get transport channel information index in CCTrCh. 
 *
 * @return Transport channel information index in CCTrCh. This is same index
 * for the transport channel that was in CCTrCh setup info at the time of setup.
 */
/*============================================================================*/

extern uint8 mcalwcdma_dec_get_trch_index(
  /** DOB status handle for which to transport channel index */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get transport format information index of transport channel in CCTrCh. 
 *
 * @return Transport format information index of transport channel in CCTrCh.
 * This is same index that was in CCTrCh setup info at the time of setup.
 */
/*============================================================================*/

extern uint8 mcalwcdma_dec_get_tf_index(
  /** DOB status handle for which to transport format index */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Query TFCI decode status.
 *
 * @return TRUE or FALSE. TRUE indicate that TFCI decode passed.
 */
/*============================================================================*/

extern boolean mcalwcdma_dec_get_tfci_decode_status(
  /** DOB status handle for which to TFCI decode status is queried */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the starting CFN of decoded TTI
 */
/*============================================================================*/

extern uint32 mcalwcdma_dec_get_crc_result_bits(
  /** DOB status handle for which to get CRC pass/fail result bits */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the ZSB bit value
 */
/*============================================================================*/

extern boolean mcalwcdma_dec_get_zsb(
  /** DOB status handle for which to get ZSB bit value */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the ZSB word value
 */
/*============================================================================*/

extern uint32 mcalwcdma_dec_get_zsb_word(
                             mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the starting CFN of decoded TTI
 */
/*============================================================================*/
extern uint8 mcalwcdma_dec_get_decode_cfn(
  /** DOB status handle for which to get decoded CFN */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the value of FW DCCH detection result **
 */
/*============================================================================*/
extern uint8 mcalwcdma_dec_get_dcch_early_detection(
  /** DOB status handle for which to get decoded CFN */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the value of FW LVA skipped decode status **
 */
/*============================================================================*/
extern uint8 mcalwcdma_dec_get_skipped_decode_status(
  /** DOB status handle for which to get skip decode flag */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get the value of FW non first traceback status **
 */
/*============================================================================*/
extern uint8 mcalwcdma_dec_get_non_first_traceback_status(
  /** DOB status handle for which to get LVA non-first traceback flag */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Set the decode status value **
 */
/*============================================================================*/
extern void mcalwcdma_dec_set_decode_status(
                                            /* DOB status handle for which to TFCI decode status is queried */
                                            mcalwcdma_dec_dob_status_handle_type handle,
                                            /* Decode status value */
                                            boolean status);

/*============================================================================*/
/**
 * Set CRC override status for DOB handle. Used in case of loopback mode need
 * to pass CRC errored block
 */
/*============================================================================*/

extern void mcalwcdma_dec_set_crc_oride_status(
  /** DOB status handle for which to TFCI decode status is queried */
  mcalwcdma_dec_dob_status_handle_type handle,
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

extern boolean mcalwcdma_dec_query_crc_oride_status(
  /* DOB status handle for which to TFCI decode status is queried */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * Get Energy Metric value for DOB handle.
 *
 * @return Energy Metric value
 */
/*============================================================================*/

extern uint16 mcalwcdma_dec_query_energy_metric(
  /* DOB status handle for which Energy Metric is queried */
  mcalwcdma_dec_dob_status_handle_type handle);

/*===========================================================================
FUNCTION mcalwcdma_dec_overwrite_tfci_fail

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

extern void mcalwcdma_dec_overwrite_tfci_fail(
  /* DOB handle */
  mcalwcdma_dec_dob_status_handle_type handle,
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

extern mcalwcdma_dec_dob_status_type* mcalwcdma_dec_get_dob_status(
  /** Status FIFO index for which the DOB status is to be read */
  mcalwcdma_dec_dob_status_handle_type handle);

/**@}*/

/*============================================================================*/
/**
 * This function frees DOB handle in MCAL decoder database. After that access to
 * DOB handle information accessor function should not be done.
 */
/*============================================================================*/
 
extern void mcalwcdma_dec_free_dob_handle(
  /** DOB hadle to be freed */
  mcalwcdma_dec_dob_status_handle_type handle);

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

extern void mcalwcdma_dec_get_decode_trblk_header(
  /** Number of DOB handles for which header need to be read (input) */
  uint8 num_dob_handles,
  /** DOB handle array pointer for which to read the data (input) */
  mcalwcdma_dec_dob_status_handle_type *dob_handles_in,
  /** DOB handle array pointer corresponding to TBset header structure elements (output)
   * This aray pointer must have at least num_dob_handles elements */
  mcalwcdma_dec_dob_status_handle_type *dob_handles_out,
  /** pointer to the TB header decode info strcuture (output)
   * This aray pointer must have at least num_dob_handles elements */
  mcalwcdma_dec_tb_hdr_decode_struct_type *tb_hdr_decode_info);

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

extern void mcalwcdma_dec_get_trblk_data(
  /** pointer to the TB header decode info strcuture */
  mcalwcdma_dec_tb_hdr_decode_struct_type *tb_hdr_decode_info,
  /** Ptr to array of the DOB handles for these TB sets Hdr info */
  mcalwcdma_dec_dob_status_handle_type *dob_handles,
  /* Ptr to structure in which to store data */
  mcalwcdma_dec_tb_data_struct_type *dl_tb_data_info);
  

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

extern void mcalwcdma_dec_tbset_decode_done_handler(void);

/*============================================================================*/
/**
 * This function updates the ciphering key for downlink deciphering operation
 * to the ciphering key index locations requested.
 */
/*============================================================================*/

extern void mcalwcdma_dec_set_ciphering_key(
  /** ciphering key index (one of the 6 keys) to which the key is set */
  uint8 index,
  /** ciphering key info. Pointer to the array of 16 bytes. May not be W32
   * aligned */
  uint8 *ciphering_key_data);

/*============================================================================*/
/**
 * This function sets DL power control outer loop set point. Value passed is in
 * signed value in dB Q25 units. This is called every radio frame once WHL1 has
 * processed all received DOB handles receibed so far. Recommended place to do
 * that is in transport block header indication call back.
 *
 * @see mcalwcdma_dec_trblk_header_cb
 */
/*============================================================================*/

extern void mcalwcdma_dec_set_olpc_set_point(
  /** OLPC set point in dB Q25 units */
  int32 *val_db_q25);

/*============================================================================*/
/**
 * This function is used to get power control og packet related information.
 * It read values from mDSP and populates in structure pointer passed to this
 * function.
 */
/*============================================================================*/

extern void mcalwcdma_dec_get_power_ctrl_info(
  /** Power control information */
  mcalwcdma_dec_power_control_info_struct_type *power_control_info);

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

extern boolean mcalwcdma_dec_tbset_decode_done_cb(
  /* DOB status handle for this TB decode done */
  mcalwcdma_dec_dob_status_handle_type handle);

/*============================================================================*/
/**
 * This function in called at some pre-determined time where all accumulated DOB
 * handles are processed for header information. This is called before data
 * indication is given to WHL1. Data indication needs information from this
 * function. WHL1 processes DOB handles and come up with list of DOB handles for
 * which header information bits need to be read for its blocks from DOB.
 *
 * @see mcalwcdma_dec_get_decode_trblk_header
 * @see mcalwcdma_dec_trblk_data_cb
 */
/*============================================================================*/

extern boolean mcalwcdma_dec_trblk_header_cb(void);

/*============================================================================*/
/**
 * This function in called at some pre-determined time where all DOB handles that
 * are processed for header indication, needs data pulled out of DOB for delivery
 * to higher layers. It needs information that is generated after calling
 * mcalwcdma_dec_trblk_header_cb function in which WHL1 has got all ciphering and
 * data block offsets to read information from higher layer.
 *
 * @see mcalwcdma_dec_trblk_header_cb
 * @see mcalwcdma_dec_get_decode_trblk_header
 */
/*============================================================================*/

extern boolean mcalwcdma_dec_trblk_data_cb(void);

/*============================================================================*/
/**
 * This function reads the BCH data out of the DOB for the handle
 * passed in by WHL1. 

 * @return Raw BCH data
 */
/*============================================================================*/

extern uint32 mcalwcdma_dec_read_bch_sfn( 
   mcalwcdma_dec_dob_status_handle_type handle);

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))

/*============================================================================*/
/**
 * This function returns one free TSN entry from Free Queue.
 */
/*============================================================================*/
extern mcalwcdma_dec_tsn_info_type *dec_get_tsn_buffer(void);  
#endif

/**@}*/
/*============================================================================*/
/**
 * This function dumps HARQ statistics as observed from DOB status FIFO. This 
 * function is called every time the log packet accumulating HS DL data is 
 * flushed out. 
 */
/*============================================================================*/

extern void mcalwcdma_dec_hs_dump_stats_from_dob_status_fifo(void);

/*============================================================================*/
/**
 * This function resets HARQ statistics as observed from DOB status FIFO. Once
 * the DOB data statistics has been dumped, it resets the counters so that the 
 * next set of statistics can be accumulated.

 * @sideeffect All counters are reset to 0.
 */
/*============================================================================*/

extern void mcalwcdma_dec_hs_reset_stats(void);

/*============================================================================*/
/**
 * This function accumulates MAC-hs header log samples and submits to DIAG once
 * the packet is full. IT then creates a new packet for next accumulation.
 */
/*============================================================================*/

extern void mcalwcdma_dec_hs_update_mac_hs_header_logging(
  /* DOB status handle */
  mcalwcdma_dec_dob_status_handle_type handle,
  /* buffer for MAC-HS header data */
  uint32 *mac_hs_header_buf,
  uint16 offset);

#ifdef FEATURE_HSDPA_MAC_EHS
/*===========================================================================
FUNCTION mcalwcdma_dec_hs_update_mac_ehs_header_logging

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

extern void mcalwcdma_dec_hs_update_mac_ehs_header_logging(
  /* DOB status handle */
  mcalwcdma_dec_dob_status_handle_type handle,
  /* TB set header information structure pointer */
  mcalwcdma_dec_tb_set_hdr_struct_type *tbset_hdr_info,
  uint32 num_reorder_sdu_in_reord_pdu[],
  uint8 si_info[],
  uint16 tsn_info[],
  uint8 q_info[]
  );
#endif /* FEATURE_HSDPA_MAC_EHS */


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

extern void mcalwcdma_dec_decode_done_isr_handler_cb(void);

/*============================================================================*/
/**
 * This function posts a request to submit any partial accumulated
 * MCAL decoder log packets.
 */
/*============================================================================*/

extern void mcalwcdma_dec_submit_pending_logpkts(void);

/*===========================================================================
FUNCTION  mcalwcdma_dec_update_tfci_stat_logpkt

DESCRIPTION
  This funtion accumulates TFCI statistics log packet samples.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcalwcdma_dec_update_tfci_stat_logpkt(void);

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION  mcalwcdma_dec_process_cctrch_log_on_demand

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
void mcalwcdma_dec_process_cctrch_log_on_demand(void);
#endif

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION  mcalwcdma_dec_process_tfc_log_on_demand

DESCRIPTION
  This funtion submits TFC log information to QXDM

DEPENDENCIES
  CCTrCH database

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcalwcdma_dec_process_tfc_log_on_demand(void);
#endif


extern boolean mcalwcdma_dec_get_cctrch_enable_state(
  /* CCTrCh index for which enable/disable state is queried */
  uint8 cctrch_index);

extern void mcalwcdma_dec_free_drm_buffer(
  /* CCTrCh index for which DRM buffer has to be freed */
  uint8 cctrch_idx);

/*===========================================================================
FUNCTION mcalwcdma_dec_update_loopback_info

DESCRIPTION
  This function updates the loopback mode of a Trch in MCAL.
  
DEPENDENCIES
  None

RETURN VALUE
  None   

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_dec_update_loopback_info(
  /* CCTrCh index */
  uint8 cctrch_index, 
  /* TrCh index */
  uint8 trch_index,
  /* loopback mode */
  mcalwcdma_dec_loop_back_mode_type_enum_type loopback_mode);

/*===========================================================================
FUNCTION mcalwcdma_dec_write_hs_deciphering_parms

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

extern void mcalwcdma_dec_write_hs_deciphering_parms(
  /* ciphering algorithm */
  mcalwcdma_dec_ciphering_algo_enum_type ciphering_algo,
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
FUNCTION mcalwcdma_dec_write_trigger_dm_task

DESCRIPTION
  This function writes the parameters required for Trigger DM task to the 
  UMTS DL HW Task Q.

DEPENDENCIES
  None

RETURN VALUE
  Updated write pointer if SUCCESS
  0xFFFFFFFF if FAILURE

SIDE EFFECTS
  None
==========================================================================*/

extern void mcalwcdma_dec_write_trigger_dm_task(void);

/*===========================================================================
FUNCTION mcalwcdma_dec_free_status_fifo

DESCRIPTION
  This function frees the DOB status FIFO in MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcalwcdma_dec_free_status_fifo(
                                   /* status FIFO index in mDSP to be freed */
                                   uint16 fifo_index);

extern uint32 mcalwcdma_dec_get_hs_dob_fifo_idx(mcalwcdma_dec_dob_status_handle_type handle);

/*===========================================================================
FUNCTION dl_get_trch_enable_state

DESCRIPTION
  This function returns the TrCh enable state. It need the CCTrCh table index
  and TrCh table index.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on trch state

SIDE EFFECTS
  
===========================================================================*/

extern boolean dl_get_trch_enable_state(
  /* CCTrCh table index for which the enable state is to be set */
  uint8 cctrch_index,
  /* TrCh index */
  uint8 trch_index);

/*===========================================================================
FUNCTION mcalwcdma_dec_do_hs_data_transfer

DESCRIPTION
  This function programs Data Mover and P2 hardware to perform data read
  activity for HS blocks residing in DOB and also to perform partial
  pdu concatenation.

  - Checks to see if previous data transfer has completed. If it
    is still pending, then do not program a new transfer and wait
    until the previous one completes.
  - Checks to see if P2 UMTS TaskQ h/w read and write pointers
    are in sync, if not, err_fatal.
  - Calls mcalwcdma_dec_process_current_tti_hs_blocks routine
  - If there is a need to do partial pdu concatenation, calls
    mcalwcdma_dec_process_prev_tti_hs_blocks routine
  - If there is a need for any data transfer, calls
    mcalwcdma_dec_trigger_p2_processing and 
    mcalwcdma_dec_dsm_maintenance routines
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

extern void mcalwcdma_dec_do_hs_data_transfer(
                                      /* pointer to the TB header decode info strcuture */
                                      mcalwcdma_dec_tb_hdr_decode_struct_type *tb_hdr_decode_info,
                                      /* Ptr to array of the DOB handles for these TB sets Hdr info */
                                      mcalwcdma_dec_dob_status_handle_type *dob_handles);

extern uint16 mcalwcdma_dec_get_hs_harq_id(
                                   mcalwcdma_dec_dob_status_handle_type handle);

extern void mcalwcdma_dec_submit_tbset_stat_logpkt_cmd(void);

/*===========================================================================
FUNCTION mcalwcdma_dec_a2_umts_dl_init

DESCRIPTION
  This function initializes the A2 DL PHY module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_dec_a2_umts_dl_init(void);

/*===========================================================================
FUNCTION mcalwcdma_decdata_hw_init

DESCRIPTION
  This function initializes the HW registers related to decoder module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_decdata_hw_init(void);

/*===========================================================================
FUNCTION mcalwcdma_dec_get_a2_dl_phy_status

DESCRIPTION
  This function returns the status of A2 DL PHY module whether it is
  active or not.

DEPENDENCIES
  None

RETURN VALUE
  boolean - A2 DL PHY status

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_dec_get_a2_dl_phy_status(void);


/*===========================================================================
FUNCTION mcalwcdma_dec_get_num_coded_bits_per_tti

DESCRIPTION
  This function returns the total number of coded bits per TTI for a 
  set of (CCTrCh, TrCh, TF)

DEPENDENCIES
  None

RETURN VALUE
  Total number of coded bits per TTI

SIDE EFFECTS
  None
===========================================================================*/

extern uint32 mcalwcdma_dec_get_num_coded_bits_per_tti(
  /* CCTrCh index */
  uint8 cctrch_idx,
  /* TrCh index */
  uint8 trch_idx,
  /* TF index */
  uint8 tf_idx);

/*===========================================================================

FUNCTION wcdma_datapath_set_datapath_f3_dbg_var_through_qxdm

DESCRIPTION

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
ex. commands
send_data 75 4 49 0 1 <val>   - for datapath_f3_debug

<val> - 32 bit value of debug variable
for providing hex value append val with 0x

===========================================================================*/

extern void wcdma_datapath_set_datapath_f3_dbg_var_through_qxdm
(
  uint8 debug_var_type,
  uint16 debug_var_val
);

/*===========================================================================
FUNCTION mcalwcdma_dec_cctrch_dirty_bit_update_pre_cb

DESCRIPTION
  This function updates the Demback TrCh table (FW-SW intf) based on the
  TrCh state information available in SW.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcalwcdma_dec_cctrch_dirty_bit_update_pre_cb (void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================

FUNCTION wl1_decdata_debug_dump

DESCRIPTION 
    Dumps local variables into wl1_datapath_mini_debug_info_struct_type  

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/
void wl1_decdata_debug_dump(wl1_datapath_mini_debug_info_struct_type *dump_ptr);
#endif /* FEATURE_QSH_DUMP */

/*===========================================================================
FUNCTION  mcalwcdma_dec_get_trch_is_voice

DESCRIPTION
  Returns whether given TrCh ID carries voice

DEPENDENCIES
  None

RETURN VALUE
  TRUE is TrCh ID carries voice, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean mcalwcdma_dec_get_trch_is_voice(uint8 trch_id);

#endif /* MCALWCDMA_DEC_H */
