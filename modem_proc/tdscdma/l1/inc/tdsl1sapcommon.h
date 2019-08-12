#ifndef TDSL1_SAP_COMMON_H
#define TDSL1_SAP_COMMON_H
/*===========================================================================
                     T D S L 1 _ S A P _ C O M M O N . H

DESCRIPTION

This file defines types that are common to the control and data service
access point interfaces.                        

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  
Copyright (c) 2000 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1sapcommon.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/28/11    tds     file created.
                    
===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsl1std.h"
#include "tdsuecomdef.h"
#include "dsm.h"
#ifdef FEATURE_MCAL_TDSCDMA
//#include "mcaltds_dec.h"
#endif

/* -------------------------------------- */
/* PUBLIC API section of tdsl1sapcommon.h        */
/* -------------------------------------- */

#ifdef FEATURE_TDSCDMA_HSUPA
typedef enum
{
  TDSL1_E_TTI_2MS,
  TDSL1_E_TTI_10MS,
  TDSL1_NUM_E_TTI
} tdsl1_e_tti_enum_type;
#endif /* FEATURE_TDSCDMA_HSUPA */

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*--------------------------------------------------------------------------
                 TRANSPORT BLOCK SIZE and RAM LIMITATIONS
--------------------------------------------------------------------------*/
#define TDSL1_MAX_UL_NUM_TRBLK            24
#define TDSL1_MAX_UL_TB_SIZE              5120
#define TDSL1_MAX_ERAM_A_BUFFER_SIZE      14080 /* 440 words * 32 bits per word */

/*===========================================================================
FUNCTION  tdsl1_get_max_ul_num_trblk

DESCRIPTION
  This function is called by MAC to get the upper bound on the number of 
  transport blocks that can be transmitted in a TTI.

DEPENDENCIES
  TDSL1_MAX_UL_NUM_TRBLK must be defined accurately for every new MSM.

RETURN VALUE
  The value of TDSL1_MAX_UL_NUM_TRBLK

SIDE EFFECTS
  None
===========================================================================*/
INLINE uint16 tdsl1_get_max_ul_num_trblk( void )
{
  return TDSL1_MAX_UL_NUM_TRBLK;
}

/*===========================================================================
FUNCTION  tdsl1_get_max_ul_tb_size

DESCRIPTION
  This function is called by MAC to get the upper bound on the size of each 
  transport block that can be transmitted in a TTI.

DEPENDENCIES
  TDSL1_MAX_UL_TB_SIZE must be defined accurately for every new MSM.

RETURN VALUE
  The value of TDSL1_MAX_UL_TB_SIZE (in bits)

SIDE EFFECTS
  None
===========================================================================*/
INLINE uint16 tdsl1_get_max_ul_tb_size( void )
{
  return TDSL1_MAX_UL_TB_SIZE;
}


/*===========================================================================
FUNCTION  tdsl1_get_max_ul_data_size

DESCRIPTION
  This function is called by MAC to get the upper bound on the total number
  of data bits that can be transmitted in a TTI.

DEPENDENCIES
  TDSL1_MAX_ERAM_A_BUFFER_SIZE must be defined accurately for every new MSM.

RETURN VALUE
  The value of TDSL1_MAX_ERAM_A_BUFFER_SIZE (in bits)

SIDE EFFECTS
  None
===========================================================================*/
INLINE uint16 tdsl1_get_max_ul_data_size( void )
{
  return TDSL1_MAX_ERAM_A_BUFFER_SIZE;
}


/*+ Legacy for TDS UL/DL dev, to be deleted later ******************************/
/* 10.3.6.33 Compressed mode measurement purpose (TGMP) */
typedef enum
{
  TDSL1_CM_TGMP_NONE,
  TDSL1_CM_TGMP_WCDMA_INTERF,
  TDSL1_CM_TGMP_GSM_RSSI_SCAN,
  TDSL1_CM_TGMP_GSM_BSIC_IDENTIFY,
  TDSL1_CM_TGMP_GSM_BSIC_RECONF,
  TDSL1_CM_NUM_TGMP_TYPE
} tdsl1_cm_tg_meas_purpose_enum_type;
/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/

/*--------------------------------------------------------------------------
                 SPREADING FACTOR ENUMS AND VALUES
--------------------------------------------------------------------------*/

/* max number of radio links, corresponds to maxRL */
#define TDSL1_MAX_RL 6   


/* max downlink timeslot number */
//#define TDSL1_DL_MAX_NUM_TS        6 /*ts0, ts2~ts6*/

/* UL SF is valid upto SF=256 only */
#define TDSL1_UL_NUM_SF   ((uint8)TDSL1_NUM_SF - 1)

/* Values for speading factor for each enum type */
#define TDSL1_SF_1_VAL    1
#define TDSL1_SF_2_VAL    2
#define TDSL1_SF_4_VAL    4
#define TDSL1_SF_8_VAL    8
#define TDSL1_SF_16_VAL   16
/*+ Legacy for TDS UL/DL dev, to be deleted later ******************************/ 
#define TDSL1_SF_32_VAL   32
#define TDSL1_SF_64_VAL   64
#define TDSL1_SF_128_VAL  128
#define TDSL1_SF_256_VAL  256
#define TDSL1_SF_512_VAL  512
/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/ 

extern uint16 tdsl1_sf_val_table[TDSL1_NUM_SF];

#define TDSL1_SF_VAL(sf_enum) tdsl1_sf_val_table[(sf_enum)]

/*--------------------------------------------------------------------------
                 TRANSMIT TIME INTERVAL ENUMS AND VALUES
--------------------------------------------------------------------------*/
/* zhouya add for 10.3.6.88 end */

/* Values for TTI in ms for each enum type */
#define TDSL1_TTI_5MS_VAL 5
#define TDSL1_TTI_10MS_VAL 10
#define TDSL1_TTI_20MS_VAL 20
#define TDSL1_TTI_40MS_VAL 40
#define TDSL1_TTI_80MS_VAL 80

extern uint16 tdsl1_tti_val_table[TDSL1_NUM_TTI];

#define TDSL1_TTI_VAL(tti_enum) (tdsl1_tti_val_table[(tti_enum)])


/* Yongh add for midamble K value */
#define  TDSL1_MIDAMBLE_CONFIG_2_VAL 2
#define  TDSL1_MIDAMBLE_CONFIG_4_VAL 4
#define  TDSL1_MIDAMBLE_CONFIG_6_VAL 6
#define  TDSL1_MIDAMBLE_CONFIG_8_VAL 8
#define  TDSL1_MIDAMBLE_CONFIG_10_VAL 10
#define  TDSL1_MIDAMBLE_CONFIG_12_VAL 12
#define  TDSL1_MIDAMBLE_CONFIG_14_VAL 14
#define  TDSL1_MIDAMBLE_CONFIG_16_VAL 16

extern uint8 tdsl1_midamble_config_val_table[TDSL1_NUM_MIDAMBLE_CONFIG];

#define TDSL1_MIDAMBLE_CONFIG_VAL(midamble_config_enum) (tdsl1_midamble_config_val_table[(midamble_config_enum)])


#ifdef FEATURE_TDSCDMA_DL_DATA_PATH_OPT
/* 3GPP 25.321 Spec allows 70 MAC-d PDUs per MAC-hs transport block */
#define TDSL1_DEC_MAX_DL_RLC_PDU 70

#define TDSL1_DEC_MAX_LC_ID 16
#endif /* FEATURE_TDSCDMA_DL_DATA_PATH_OPT */

#define TDSL1_DEC_MAX_HS_FR_PER_10MS_TTI 5

/* Spec 25.321 Max possible Reordering SDUs per HS TrBlk */
#define TDSL1_DEC_HS_REORDER_SDU_MAX_COUNT 26

/* The maximum number of reordering pdu in one transport block, currently 3 per 
 * 3GPP 25.321 Mac spec. */
#define TDSL1_DEC_MAX_REORD_PDU_PER_HS_FRAME 3

/* The maximum number of partial pdu per reordering pdu, currently 2 per
 * 3GPP 25.321 Mac spec. */
#define TDSL1_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU 2

/* We can have 2 complete pdus for each reordering queue in the previous TTI,
 * total = 3 * 2 * 5 for Non-MIMO
 * total = 3 * 2 * 10 for MIMO */
#define TDSL1_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI \
            (TDSL1_DEC_MAX_REORD_PDU_PER_HS_FRAME * \
             TDSL1_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU * \
             TDSL1_DEC_MAX_HS_FR_PER_10MS_TTI)


#define TDSL1_DEC_MAX_DMOV_SOURCE_DESCRIPTOR_LEN_MOVE_IN 100

#define TDSL1_DEC_MAX_DMOV_DESTINATION_DESCRIPTOR_LEN_MOVE_IN 4

/** Number of W32 to be read in transport block header information */
#define TDSL1_DEC_TB_HDR_SIZE_W32 3

/** This is the maximum number of transport block set for which the transport
 * block headers can be reported by L1 to L2 for decoding
 * Note that size defined is based on the maximum amout that can be
 * reported by mDSP. SW has to process that and should have that much capacity.
 * This is not as per UE capability specified in standard */
#define TDSL1_DEC_MAX_TB_SET_HDR 32

/*--------------------------------------------------------------------------
                         CRC ENUMS AND VALUES
--------------------------------------------------------------------------*/

/* Value for CRC length for each CRC enum type */
#define TDSL1_CRC_LENGTH_0_VAL     0
#define TDSL1_CRC_LENGTH_8_VAL     8
#define TDSL1_CRC_LENGTH_12_VAL    12
#define TDSL1_CRC_LENGTH_16_VAL    16
#define TDSL1_CRC_LENGTH_24_VAL    24

extern uint8 tdsl1_crc_length_val_table[TDSL1_NUM_CRC_LENGTH];

#define TDSL1_CRC_LENGTH_VAL(crc_len_enum) \
  tdsl1_crc_length_val_table[(crc_len_enum)]
  

/*--------------------------------------------------------------------------
                            ENUM: DL_REF_PHYCHAN_ENUM_TYPE
                            
This enumerates the various downlink physical channel reference id's                            
--------------------------------------------------------------------------*/
typedef enum {
  TDSL1_DL_PHYCHAN_PCCPCH_S,
  TDSL1_DL_PHYCHAN_PCCPCH_N,
  TDSL1_DL_PHYCHAN_SCCPCH0,
  TDSL1_DL_PHYCHAN_SCCPCH1,
  TDSL1_DL_PHYCHAN_PICH,
#if 0
  TDSL1_DL_PHYCHAN_FPACH,
  TDSL1_DL_PHYCHAN_AICH,
#endif 
  TDSL1_DL_PHYCHAN_DPCH,
/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/ 
  TDSL1_DL_PHYCHAN_PDSCH,
#if 0
#endif
  TDSL1_NUM_DL_PHYCHAN,
  TDSL1_DL_PHYCHAN_NOCHAN
} tdsl1_dl_phychan_enum_type;


#define TDSL1_DL_NUM_PCCPCH 2
#define TDSL1_DL_NUM_SCCPCH 2

/*--------------------------------------------------------------------------
Values for DL physical channel status offset field in CPHY_SETUP_CNF 
packet structure
--------------------------------------------------------------------------*/
#define TDSL1_DL_PHYCHAN_PCCPCH_S_OFFSET 0
#define TDSL1_DL_PHYCHAN_PCCPCH_N_OFFSET 4
#define TDSL1_DL_PHYCHAN_SCCPCH0_OFFSET  8
#define TDSL1_DL_PHYCHAN_SCCPCH1_OFFSET  12
#define TDSL1_DL_PHYCHAN_DPCH_OFFSET     16
#define TDSL1_DL_PHYCHAN_PDSCH_OFFSET    20


#if 0
/* This enum defines the reference type of the TrCh */
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_trch_ref_type_enum_type tdsl1_dl_trch_ref_type_enum_type;
#else
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
} tdsl1_dl_trch_ref_type_enum_type;
#endif
#endif

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
} tdsl1_dl_trch_ref_type_enum_type;


/* defines for the TrCh ref type bit field */
#define TDSL1_DL_TRCH_REFTYPE_SBCH_BF 0x01
#define TDSL1_DL_TRCH_REFTYPE_NBCH_BF 0x02
#define TDSL1_DL_TRCH_REFTYPE_DCH_BF  0x04
/*+ Legacy for TDS UL/DL dev, to be deleted later ******************************/
#define TDSL1_DL_TRCH_REFTYPE_DSCH_BF 0x08
/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/
#define TDSL1_DL_TRCH_REFTYPE_PCH_BF  0x10
#define TDSL1_DL_TRCH_REFTYPE_FACH_BF 0x20

extern uint8 tdsl1_dl_trch_ref_type_bf_val[TDSL1_DL_NUM_TRCH_REF_TYPE];

#define tdsl1_dl_trch_ref_type_bf_val(ref_type) tdsl1_dl_trch_ref_type_bf_val[(ref_type)]

/* Utility macro to check upper boundary value for a variable
** Valid value of val should be less than bound
*/
#define TDSL1_CHECK_UPPER_BOUND(val, bound) \
{ \
  if(!(val <= bound)) \
  { \
    ERR_FATAL("Val %d Exceeds boundary value %d", val, bound, 0); \
  } \
}


/*--------------------------------------------------------------------------
Values of the downlink transport channel status fields in the 
CPHY_SETUP_CNF packet structure
--------------------------------------------------------------------------*/
#define TDSL1_DL_TRCH_SBCH_TYPE_OFFSET 0
#define TDSL1_DL_TRCH_NBCH_TYPE_OFFSET 4
#define TDSL1_DL_TRCH_PCH_TYPE_OFFSET  8
#define TDSL1_DL_TRCH_FACH_TYPE_OFFSET 12
#define TDSL1_DL_TRCH_DCH_TYPE_OFFSET  16
/*+ Legacy for TDS UL/DL dev, to be deleted later ******************************/
#define TDSL1_DL_TRCH_DSCH_TYPE_OFFSET 20
/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/

/*--------------------------------------------------------------------------
Values of the uplink transport channel status fields in the 
CPHY_SETUP_CNF packet structure
--------------------------------------------------------------------------*/
#define TDSL1_UL_TRCH_RACH_OFFSET 0
#define TDSL1_UL_TRCH_DCH_OFFSET  4


#define TDSL1_UL_PHYCHAN_DPCH_OFFSET 4
#define TDSL1_UL_PHYCHAN_PRACH_OFFSET 0

/* Gain Factor calculation methods */
#define DEFAULT_BATAD_NO_PWROFFSETINFO_EXIST 7

/*--------------------------------------------------------------------------
           Externs for CPHY_SETUP_CNF status offset lookup arrays
--------------------------------------------------------------------------*/
extern uint8 tdsl1_cphy_setup_cnf_dl_phychan_offset[TDSL1_NUM_DL_PHYCHAN];
extern uint8 tdsl1_cphy_setup_cnf_dl_trch_offset[TDSL1_DL_NUM_TRCH_REF_TYPE];
extern uint8 tdsl1_cphy_setup_cnf_ul_phychan_offset[TDSL1_UL_NUM_PHYCHAN];
extern uint8 tdsl1_cphy_setup_cnf_ul_trch_offset[TDSL1_UL_NUM_TRCH];



/*--------------------------------------------------------------------------
  Constants indicating how many slot formats for a given physical channel
--------------------------------------------------------------------------*/
#define TDSL1_MAX_SCCPCH_SLOT_FORMAT 18
#define TDSL1_MAX_DPCH_SLOT_FORMAT 17
#define TDSL1_MAX_PDSCH_SLOT_FORMAT 7

/*--------------------------------------------------------------------------
                            CIPHERING_KEY_REQ
--------------------------------------------------------------------------*/

/* defines for the ciphering key updates */
#define TDSL1_NUM_BYTES_CIPHER_KEY  16
#define TDSL1_MAX_CIPHER_KEY        4
#define TDSL1_MAX_CIPHER_KEY_ID_VAL 3

#define TDSL1_UL_CIPHER_KEY_REQ 0x01
#define TDSL1_DL_CIPHER_KEY_REQ 0x02

/* This enum defines the ciphering algorithm type */
//typedef mcaltds_dec_ciphering_algo_enum_type tdsl1_cipher_alg_enum_type;
typedef uecomdef_umts_cipher_algo_e_type tdsl1_cipher_alg_enum_type;

/* This structure defines the structure for the ciphering key information */
typedef struct
{
     /* key Id of the ciphering key in dl */
     uint8 key_id;

     /* ciphering algorithm */
     tdsl1_cipher_alg_enum_type algorithm;
     
     /* ciphering key bit stream.
        MSB byte 0 is CK_bit0. LSB last byte is CK_bit127 */
     uint8 key_val[TDSL1_NUM_BYTES_CIPHER_KEY];
} tdsl1_cipher_key_struct_type       ;

/* This structure defines the structure for all the the ciphering key
   information */
typedef struct
{
     /* number of ciphering keys */
     uint8 num_keys;

     /* ciphering key info */
     tdsl1_cipher_key_struct_type        key_info[TDSL1_MAX_CIPHER_KEY];
} tdsl1_cipher_key_info_struct_type;

/* This structure defines the command packet for the TDSL1_CPHY_CIPHERING_KEY_REQ */
typedef struct
{
     /* ciphering request mask
        Bit 0: UL ciphering param
        Bit 1: DL ciphering param */
     uint8 req_mask;

     /* UL ciphering key info */
     tdsl1_cipher_key_info_struct_type ul_cipher_info;

     /* DL ciphering key info */
     tdsl1_cipher_key_info_struct_type dl_cipher_info;
} tdsl1_ciphering_key_req_type;


#if 0
#ifdef FEATURE_MCAL_TDSCDMA
#define TDSL1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT  MCALTDS_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT
#else
#define TDSL1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT 8
#endif
#endif
#define TDSL1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT 8


/** This enum defines the coding type of the TrCh in a CCTrCh. */
typedef enum
{
  TDSL1_DL_CONV_1_BY_2_CODING, /**< Convolutional coding with rate 1/2 */
  TDSL1_DL_CONV_1_BY_3_CODING, /**< Convolutional coding with rate 1/3 */
  TDSL1_DL_TURBO_CODING,       /**< Turbo coding. Rate is always 1/3 */
  TDSL1_DL_UNCODED             /**< Uncoded data */
} tdsl1_dl_cctrchcfg_coding_enum_type;

//#ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
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
  TDS_COMPLETE_PDU,
  TDS_PARTIAL_FIRST,
  TDS_PARTIAL_MIDDLE,
  TDS_PARTIAL_LAST
} tdsl1_dec_rlc_pdu_segmentation_type;
//#endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */


/*----------------------------------------------------------------------------
   Enum indicated by L1 to MAC along with HS data inidcation in order to make 
   decision for processing the current, prev or both TTIs
 -----------------------------------------------------------------------------*/
/* HSDPA Action Variable */
typedef enum
{
  TDS_MACDL_HS_CURR_TTI,
  TDS_MACDL_HS_PREV_TTI,
  TDS_MACDL_HS_CURR_PREV_TTI,
  TDS_MACDL_HS_NONE
} tdsl1_dl_hs_tti_enum_type;

/** Transparant mode deciphering states for various transport block */
typedef enum
{
  TDSL1_TB_TM_CIPHER_NOT_APPLICABLE, /**< Block not ciphering in TM mode */
  TDSL1_TB_TM_CIPHER_START,          /**< First block iciphered in TM mode */
  TDSL1_TB_TM_CIPHER_CONT,           /**< Continue ciphering key stream generation */
  TDSL1_TB_TM_CIPHER_STOP,           /**< Last block iciphered in TM mode */
  TDSL1_TB_TM_CIPHER_SINGLE          /**< Single block in TM mode so don't */
} tdsl1_tb_tm_cipher_state_enum_type;


/** This structure defines transport format information of TF in a
 * transport channel  */
typedef struct
{
  uint8  num_tb;   /**< Number of transport block in TBS. */
  uint16 tb_size;  /**< Transport block size. */
} tdsl1_dl_cctrchcfg_tf_info_struct_type;


/** This structure defines the transport channel information */
typedef struct
{
  /** Transport channel reference type */
  tdsl1_dl_trch_ref_type_enum_type ref_type;

  /** Transport channel Id. This is for backward compatibility of structure
   * and used by WHL1. NOT USED IN MCAL SOFTWARE */
  uint8 trch_id;


  /** Rate matching attribute. */
  uint16 rate_match_attrib;

  /** Coding type of transport chanel */
  tdsl1_dl_cctrchcfg_coding_enum_type coding_type;

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
  tdsl1_dl_cctrchcfg_tf_info_struct_type *tf_info_array[TDSL1_DL_MAX_TF];
} tdsl1_dl_cctrchcfg_trch_info_struct_type;

/** This structure defines CCTrCh information parameters for each transport
 * channel in it and all transport format of each transport channel */
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
  tdsl1_dl_cctrchcfg_trch_info_struct_type *trch_info_array[TDSL1_DL_MAX_TRCH]; 
} tdsl1_dl_cctrchcfg_cctrch_info_struct_type;

typedef tdsl1_dl_cctrchcfg_cctrch_info_struct_type tdsl1dlcctrchcfg_cctrch_info_struct_type;


/** This structure defines the CTFC or TFC entry information members */
typedef struct
{
  /** Is this TFC is valid or not */
  boolean valid_ctfc;

  /** pointer to transport format index of each TrCh in CCTrCh array */
  uint8 *tfi_table;
} tdsl1_dl_cctrchcfg_ctfc_entry_info_struct_type;

typedef tdsl1_dl_cctrchcfg_ctfc_entry_info_struct_type tdsl1dlcctrchcfg_dl_ctfc_info_struct_type;

/** This structure defines transport format confinition set information */
typedef struct
{
  /*Record the number of ctfc entry.*/
  uint16 num_ctfc_entry; 
  
  /** Pointer to the array of TFC information */
  tdsl1_dl_cctrchcfg_ctfc_entry_info_struct_type *ctfc_entry_array[TDSL1_DL_MAX_TFC];
} tdsl1_dl_cctrchcfg_ctfc_info_struct_type;


/* Enumerated type to determine ciphering enabled and ciphering
 * algorithm:
 * This is a 2-bit value -
 * bit 0 indicates ciphering algorithm (0 - Kasumi, 1-Snow3G)
 * bit 1 indicates ciphering enabled (1) or not (0)
 */
/*typedef enum
{
 TDSL1_CIPHERING_ALGO_NONE   = 0,
 TDSL1_CIPHERING_ALGO_KASUMI = 2,
 TDSL1_CIPHERING_ALGO_SNOW3G = 3
} tdsl1_ciphering_algo_enum_type;
*/
typedef uecomdef_umts_cipher_algo_e_type tdsl1_ciphering_algo_enum_type;
/** This structure defines the ciphering parameters for a transport block
 * header information of a transport block */
typedef struct
{
  /** Ciphering enabled or not for this 
   * transport block. */
  boolean ciphering_control;
  /** Ciphering algorithm specified by L2 for this
   * transport block. */
  tdsl1_ciphering_algo_enum_type ciphering_algo;
  
  /** Ciphering bit offset returned by L2 of each transport block. */
  uint8 ciphering_bit_offset;
  
  /** Ciphering key id returned by L2 of this transport block. */
  uint8 ciphering_key_id;
  
  /** Ciphering count (CFN/SFN) returned by L2 of this transport block. */
  uint32 ciphering_count;
  
  /** Ciphering radio bearer id returned by L2 for this transport block. */
  uint8 ciphering_radio_bearerid;
} tdsl1_dl_ciphering_param_struct_type;

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
  tdsl1_dl_ciphering_param_struct_type ciphering_param;
  
  /** Pointer to the transport block header */
  uint8 tb_header[TDSL1_DEC_TB_HDR_SIZE_W32 * 4];
} tdsl1_dl_tb_hdr_struct_type;


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
} tdsl1_dec_iovec_type;



typedef struct
{
  tdsl1_dec_iovec_type iovec; /* IOVEC for this complete PDU */

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
  tdsl1_dec_iovec_type iovec; /* IOVEC for this partial PDU */

  /* L1 uses this variable to indicate to MAC whether
     RLC PDU header is received completely or not. */
  boolean complete_rlc_hdr_rcvd;
  
  uint32 rlc_hdr_data; /* RLC Header contents */
  
  /* This indicates if the demultiplexed PDU is a partial first, middle or last*/
  tdsl1_dec_rlc_pdu_segmentation_type segmentation_info; /* Segmentation indication */

  uint8 lc_id; /* Logical Channel ID to which this pdu belongs*/
} tds_partial_pdu_type;
#endif

/** Enum type definition for TSN trace enum type */
typedef enum {
  TDSDL_TSN_FREE_Q = 1, /**< TSN is in Dec TSN info free Q */
  TDSDL_TSN_L1_INT_Q,   /**< TSN is in L1 Internal Q */
  TDSDL_TSN_INFO_Q,     /**< TSN is in Dec TSN info Q */
  TDSDL_TSN_MAC_INT,    /**< TSN is in MAC internal structures */
  TDSDL_TSN_RLC_Q,      /**< TSN is in RLC TSN info Q */
  TDSDL_TSN_RLC         /**< TSN is in RLC */
} tdsl1_dec_tsn_trace_enum_type;




/** If a TSN structure passed to MAC has both complete
 *  and partial pdu info, then RLC should not release the
 *  structure. MAC will have to free this structure when the
 *  partial PDUs are taken care of. */
typedef struct
{
  q_link_type link; /**< Link to manage this item on the queue */
  uint8 index;      /**< index of this element in array */

  /* Indicates which module is holding the TSN items */
  tdsl1_dec_tsn_trace_enum_type tracer;

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

  tds_complete_pdu_type TDS_COMPLETE_PDU[TDSL1_DEC_MAX_DL_RLC_PDU];

 #ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
  /*UE_MAX_PARTIAL_PDU_PER_REORDER_PDU=2 since in a reordering pdu either the
  first mac-ehs sdu could be a partial one and/or the last mac-ehs sdu in the
  reordering pdu could be a partial one*/
  tds_partial_pdu_type partial_pdu[TDSL1_DEC_MAX_PARTIAL_PDU_PER_REORDER_PDU];
 #endif
} tdsl1_dec_tsn_info_type;


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
  tdsl1_dec_tsn_info_type *tsn_info_buf;

  /** ciphering parameters for this RLC PDU */
  tdsl1_dl_ciphering_param_struct_type ciphering_param;
} tdsl1_dl_partial_rlc_pdu_hdr_struct_type;



/** This structure defines the information for all RLC PDUs that can
 *  be constructed out of multiple partial PDUs. This structure
 *  also contains the information required to get the data
 *  from the hardware. */
typedef struct
{
  /** Number of complete RLC PDUs that can be formed */
  uint16 num_complete_pdus;

  /* for each complete RLC PDU that is being formed */
  tdsl1_dl_partial_rlc_pdu_hdr_struct_type rlc_pdu_hdr_info[TDSL1_DEC_MAX_COMPLETE_PDU_FROM_PREV_TTI];
} tdsl1_dec_rlc_pdu_concatenate_info_struct_type;

typedef struct
{
  /* For rel-7 LC_ID is known to L1 while doing the header read event. For rel-6 the MAC will
  pass the LC_ID information at the end of the header read event. DSM items are allocated
  per lc_id basis hence L1 needs to be awar of the lc_id for rel6 too*/
  uint8 lc_id;        /* Logical Channel ID */
  uint8 tsn;          /* Transmission seq number 6 bits */
  uint16 length;      /* Length of MAC e-HS SDU in bytes*/
  tdsl1_dec_rlc_pdu_segmentation_type segmentation_info; /* Complete or Partial PDU */
} tdsl1_dec_mac_ehs_sdu_hdr_type;


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
  tdsl1_dec_tsn_info_type *tsn_info_buf;
} tdsl1_dec_concatenated_pdu_tsn_type;



/** This structure defines the transport block header information for a
 * transport block set (or a transport channel) */
typedef struct
{
  /** Number of transport blocks in this transport block set. */
  uint8 num_tb;
  
  /** CCTrCh Ids for the transport block set. This is set by WHL1 */
  uint8 cctrch_id;
  
  /** TrCh reference type for the transport block set */
  tdsl1_dl_trch_ref_type_enum_type trch_ref_type;
  

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
      uint8 num_blk[TDSL1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT];
      /** SID value for each k */
      uint8 sid_val[TDSL1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT];
    } hs;

   #ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
    /** E-HS channel information */
    struct
    {
      uint16 sub_frame_num; /* Starting Sub frame number when TTI is received */
      uint8 num_reorder_sdu; /* Number of Reordering SDUs in this HS TTI */
      /* LC-ID, Length, TSN and Segmentation Type of each MAC-eHS SDU in this HS TTI */
      tdsl1_dec_mac_ehs_sdu_hdr_type mac_ehs_sdu_hdr[TDSL1_DEC_HS_REORDER_SDU_MAX_COUNT];
    } ehs;
   #endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS */
  } info;


  /** indicate that TBs in this TB set are in transparent mode */
  boolean tb_in_tm_mode;
  /** Pointer to the structure array of information of all
    transport block header in this TB set */
  tdsl1_dl_tb_hdr_struct_type *tb_hdr_info;
} tdsl1_dl_tb_set_hdr_struct_type;


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
  tdsl1_dl_tb_set_hdr_struct_type tb_set_hdr[TDSL1_DEC_MAX_TB_SET_HDR];

 #ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
  /** Information for all RLC PDUs that can be constructed out of
   *  multiple partial PDUs. */
  tdsl1_dec_rlc_pdu_concatenate_info_struct_type rlc_pdu_concatenate_info;
 #endif
} tdsl1_dl_tb_hdr_decode_struct_type;

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
} tdsl1_dl_tb_data_in_tbset_struct_type;

/** This structure defines the data and status element for a transport block set */
typedef struct
{
  /** TrCh reference type for the transport block set */
  tdsl1_dl_trch_ref_type_enum_type trch_ref_type;

  /** Number of transport blocks in this transport block set. */
  uint8 num_tb;
  
  /** transport block size for the transport block of this trblk set */
  uint16 trblk_size;
  
  /** points to the array of the TB info structure */
  tdsl1_dl_tb_data_in_tbset_struct_type *tb_data_info;
} tdsl1_dl_tb_set_data_struct_type;

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
  tdsl1_dl_tb_set_data_struct_type tb_set_data[TDSL1_DEC_MAX_TB_SET_HDR];
} tdsl1_dl_tb_data_struct_type;


/* zhouya del for 10.3.6.21 begin */

typedef struct {
  /* bitmask indicating the absence/presence of optional parameters.  Use
     the above defined bit definitions */
  uint16 opt_parms_present_mask;

  /* 10.3.6.17 Primary CPICH usage for channel estimation */
  //tdsl1dlphycfg_ch_est_cpich_usage_enum_type use_pri_cpich;

/* shanyufei this para should delete, FDD only, 3.2.4 begin */
  /* 10.3.6.17 DPCH frame offset */
  uint16 tdsframe_offset;
/* shanyufei this para should delete, FDD only, 3.2.4 end*/

  /* 10.3.6.17 -> 10.3.6.63 -> 10.3.6.64
     secondary CPICH secondary scrambling code */
  uint16 sec_cpich_sec_scr_code;

  /* 10.3.6.17 -> 10.3.6.63 secondary CPICH channelisation code */
  uint16 sec_cpich_ch_code;

  /* how many DPCH multicode channels are being set up */
  uint16 num_dpch;

  /* array of per DPCH info structures, there will be num_dpch valid
     entries */
  struct
  {
    #define DL_DPCH_CODE_CH_SEC_SCR_CODE_PRESENT 0x01
    #define DL_DPCH_CODE_CH_SCR_CODE_CHG_PRESENT 0x02
    /* optional parameter presence mask */
    uint8 opt_parm_present_mask;

    /* 10.3.6.17 -> 10.3.6.64 DPCH Secondary Scrambling Code */
    uint16 sec_scr_code;

    /* 10.3.6.17 Code Number */
    uint16 code_number;

  } dl_chan_code[TDSL1_DL_MAX_DPCH];

  /* 10.3.6.17 TPC Combination Index */
  uint8 tpc_idx;

  /* 10.3.6.17 Closed loop timing adjustment mode */
  uint8 closed_loop_timing_adj_mode;

} tdsl1dl_dpch_info_per_rl_struct_type;
/* zhouya del for 10.3.6.21 begin */

typedef struct {
/* shanyufei del for 8.6.6.4 begin */
  /* primary scrambling code of the CPICH */
  uint16 pri_cpich_scr_code;
/* shanyufei del for 8.6.6.4 end */

//#ifdef PDSCH_SUPPORTED
//  l1_pdsch_sho_dch_info_struct_type pdsch_sho_info;
//  l1_pdsch_code_map_struct_type pdsch_code_map;
//#endif

  tdsl1dl_dpch_info_per_rl_struct_type dpch_per_rl;

} tdsl1dl_info_per_rl_struct_type;
/* zhouya del for 10.3.6.21 end */

/* Number of HS-SCCH/HS-SICh pairs */
#define TDSL1DLPHYCFG_HS_SCCH_SICH_NUM_MAX 4

#endif /* TDSL1_SAP_COMMON_H */

