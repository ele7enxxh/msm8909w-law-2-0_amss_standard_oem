#ifndef TDSL1_STD_H
#define TDSL1_STD_H
/*============================================================================
                     T D S L 1 S T D _ H
DESCRIPTION
  This file defines constants and enumerations based on TD-SCDMA specifications

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     --------------------------------------------------------
12/02/11    cdf     Add uplink standard definition
11/02/11    yongh   Create for save all of parameters from standard.
===========================================================================*/

/**-------------------------------------------------------------------------
 ** Include Files
 ** -----------------------------------------------------------------------*/







/**=========================================================================
 **
 **                              Common Part
 **
 **==========================================================================*/


/**------------------------------------------------------------------------
 ** Macro 
 ** -----------------------------------------------------------------------*/
 
/* the maximum number of transport channels. This is the same for both uplink and downlink */
#define TDSL1_MAX_NUM_TRCH                            8

/*ts0,ts1~ts6*/
#define TDSL1_MAX_NUM_TS                              7 

/* The maximum number of channelisation code */
#define TDSL1_MAX_NUM_CHAN_CODE_PER_SLOT              16

/* The number of possible midamble shift */
#define TDSL1_MAX_NUM_MIDAMBLE_SHIFT                  16

/* DPCH desired received power in dBm. 
 * The valid range is [-120, -58] by step of 1 */
#define TDSL1_DESIRED_POWER_MIN                       (-120)
#define TDSL1_DESIRED_POWER_MAX                       (-58)

#define TDSL1_PCCPCH_TX_POWER_MIN                     6
#define TDSL1_PCCPCH_TX_POWER_MAX                     43

/* Puncture Limit.
 * The range is [0.4, 1] in steps of 0.04.
 * Zoom out 100 and use [40, 100] in steps of 4 to avoid floating point calculation
 */
#define TDSL1_PUNCT_LIMIT_MIN                         40
#define TDSL1_PUNCT_LIMIT_MAX                         100

#define TDSL1_DPCH_REPEAT_PRD_MIN                     1
#define TDSL1_DPCH_REPEAT_PRD_MAX                     64


/* defines the maximum number of uplink physical channels per timeslot */
#define TDSL1_MAX_NUM_UL_CODE_PER_SLOT                2

/* defines the maximum number of downlink physical channels per timeslot */
#define TDSL1_MAX_NUM_DL_CODE_PER_SLOT                16


/* See 25.222/4.2.2.2 for these definitions */
#define TDSL1_MAX_BLOCK_SIZE_CONV                     504
#define TDSL1_MAX_BLOCK_SIZE_TURBO                    5114
#define TDSL1_MIN_BLOCK_SIZE_TURBO                    40

/* Number of prime numbers in list for Turbo interleaving parameters */
#define TDSL1_TRUBO_INTLV_PRIME_LIST_SIZE             52

#define TDSL1_REPEAT_PRD_1                            1
#define TDSL1_REPEAT_PRD_2                            2
#define TDSL1_REPEAT_PRD_4                            4
#define TDSL1_REPEAT_PRD_8                            8
#define TDSL1_REPEAT_PRD_16                           16
#define TDSL1_REPEAT_PRD_32                           32
#define TDSL1_REPEAT_PRD_64                           64

/* TFCI code word bits, in range (4,8,16,32, default value is 0)*/
#define TDSL1_TFCI_CODING_BITS_0                      0
#define TDSL1_TFCI_CODING_BITS_4                      4
#define TDSL1_TFCI_CODING_BITS_8                      8
#define TDSL1_TFCI_CODING_BITS_16                     16
#define TDSL1_TFCI_CODING_BITS_32                     32

/* static rate matching attribute */
#define TDSL1_MIN_RATE_MATCH_ATTRIB                   1
#define TDSL1_MAX_RATE_MATCH_ATTRIB                   256

/* maximum number of additional SS&TPC symbols */
#define TDSL1_DPCH_NUM_ADDITIONAL_TPC_SS_CODE         15

/* maximum power ramp step */
#define TDSL1_PWR_RAMP_STEP_MAX                       3

#define TDSL1_MAX_BETAD                               15

/**------------------------------------------------------------------------
 ** Enum 
 ** -----------------------------------------------------------------------*/

typedef enum
{
  TDSL1_TRUBO_NUM_ROW_INDEX_PA,   /* 20 rows */
  TDSL1_TRUBO_NUM_ROW_INDEX_PB,   /* 20 rows */
  TDSL1_TRUBO_NUM_ROW_INDEX_PC,   /* 10 rows */
  TDSL1_TRUBO_NUM_ROW_INDEX_PD    /* 05 rows */
} tdsl1_trubo_num_row_index_enum_type;

/** This enum defines various loopback moes */
typedef enum
{
  TDSL1_LOOPBACK_NA,        /* No loopback is required */
  TDSL1_LOOPBACK_MODE_1,    /* loopback mode 1 */
  TDSL1_LOOPBACK_MODE_2     /* loopback mode 2 */
} tdsl1_loop_back_mode_type_enum_type;

/* enum type definition for spreading factor */
typedef enum 
{
  TDSL1_SF_1,
  TDSL1_SF_2,
  TDSL1_SF_4,
  TDSL1_SF_8,
  TDSL1_SF_16,
/*+ Legacy for TDS UL/DL dev, to be deleted later ******************************/  
  TDSL1_SF_32,
  TDSL1_SF_64,
  TDSL1_SF_128,
  TDSL1_SF_256,
  TDSL1_SF_512,
/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/
  TDSL1_NUM_SF
} tdsl1_sf_enum_type;

/* Enum type definition for TTI enum type */
typedef enum 
{ 
  TDSL1_TTI_5MS,
  TDSL1_TTI_10MS,
  TDSL1_TTI_20MS,
  TDSL1_TTI_40MS,
  TDSL1_TTI_80MS,
  TDSL1_NUM_TTI
} tdsl1_tti_enum_type;

/* Time Slot number enum type*/
typedef enum
{
  TDSL1_TS_NUM_0 = 0,
  TDSL1_TS_NUM_1,
  TDSL1_TS_NUM_2,
  TDSL1_TS_NUM_3,
  TDSL1_TS_NUM_4,
  TDSL1_TS_NUM_5,
  TDSL1_TS_NUM_6 = 0x06,
} tdsl1_ts_num_enum_type;

typedef enum 
{
  TDSL1_DEFAULT_MIDAMBLE,
  TDSL1_COMMON_MIDAMBLE,
  TDSL1_UE_SPECIFIC_MIDAMBLE, 
  TDSL1_INVALID_MIDAMBLE
}tdsl1_midamble_alloc_mode_enum_type;

typedef enum 
{
  TDSL1_MIDAMBLE_CONFIG_2,
  TDSL1_MIDAMBLE_CONFIG_4,
  TDSL1_MIDAMBLE_CONFIG_6,
  TDSL1_MIDAMBLE_CONFIG_8,
  TDSL1_MIDAMBLE_CONFIG_10,
  TDSL1_MIDAMBLE_CONFIG_12,
  TDSL1_MIDAMBLE_CONFIG_14,
  TDSL1_MIDAMBLE_CONFIG_16,
  TDSL1_NUM_MIDAMBLE_CONFIG
}tdsl1_midamble_config_enum_type;

typedef enum 
{
  TDSL1_MOD_QPSK,
  TDSL1_MOD_8PSK,
  TDSL1_MOD_16QAM,
  TDSL1_MOD_64QAM
} tdsl1_mod_enum_type;

/** This enum defines the coding type of the TrCh in a CCTrCh. */
typedef enum
{
  TDSL1_UNCODED,             /* Uncoded data */
  TDSL1_CONV_1_BY_2_CODING,  /* Convolutional coding with rate 1/2 */
  TDSL1_CONV_1_BY_3_CODING,  /* Convolutional coding with rate 1/3 */
  TDSL1_TURBO_CODING,        /* Turbo coding. Rate is always 1/3 */
  TDSL1_NUM_CODING_TYPE
} tdsl1_coding_enum_type;

typedef enum 
{
  TDSL1_SS_TPC_SYMBOL_0,
  TDSL1_SS_TPC_SYMBOL_1,
  TDSL1_SS_TPC_SYMBOL_16_DIV_SF,
  TDSL1_NUM_TPC_SYMBOL_CFG
} tdsl1_ss_tpc_symb_enum_type;

typedef enum 
{
  TDSL1_SEC_INTLV_FRAME_BASED,
  TDSL1_SEC_INTLV_SLOT_BASED,
  TDSL1_SEC_INVALID_SEC_INTERLEAVING_MODE
} tdsl1_sec_intlv_mode_enum_type;

typedef enum 
{
  TDSL1_PICH_REPEAT_4_2,
  TDSL1_PICH_REPEAT_8_2,
  TDSL1_PICH_REPEAT_8_4,
  TDSL1_PICH_REPEAT_16_2,
  TDSL1_PICH_REPEAT_16_4,
  TDSL1_PICH_REPEAT_32_2,
  TDSL1_PICH_REPEAT_32_4,
  TDSL1_PICH_REPEAT_64_2,
  TDSL1_PICH_REPEAT_64_4,
  TDSL1_PICH_REPEAT_INVALID
} tdsl1_pich_repeat_prd_len_enum_type;

typedef enum 
{
  TDSL1_TPC_STEP_SIZE_0DB, /* If TPC Step Size is absent in OTA message */
  TDSL1_TPC_STEP_SIZE_1DB,
  TDSL1_TPC_STEP_SIZE_2DB,
  TDSL1_TPC_STEP_SIZE_3DB,
  TDSL1_NUM_TPC_STEP_SIZE
} tdsl1_tpc_step_size_enum_type;

/* This enum defines the CRC length for TrCh in a CCTrCh. The enum values
   are kept same as used in RRC IEs and denotes the number of CRC bits. */
typedef enum
{
  TDSL1_CRC_LENGTH_0,
  TDSL1_CRC_LENGTH_8,
  TDSL1_CRC_LENGTH_12,
  TDSL1_CRC_LENGTH_16,
  TDSL1_CRC_LENGTH_24,
  TDSL1_NUM_CRC_LENGTH
} tdsl1_crc_length_enum_type;


/* This enum defines the rate matching type */
 typedef enum
{
  TDSL1_NO_RM,      /* No rate matching to be used */
  TDSL1_RM_REPEAT,  /* Repetition used for rate matching */
  TDSL1_RM_PUNCT    /* Puncturing used for rate matching */
} tdsl1_rm_type_enum_type;








/**=========================================================================
 **
 **                                Uplink Part
 **
 **==========================================================================*/


/**------------------------------------------------------------------------
 ** Macro 
 ** -----------------------------------------------------------------------*/

/* maximal number of transport blocks allowed for uplink */
#define TDSL1_UL_MAX_NUM_TB                           24

/* maximum transport block size */
#define TDSL1_UL_MAX_TB_SIZE                          5120


/* Uplink synchronisation step size to be used for the adjustment of the
 * transmission timing. 
 */
#define TDSL1_UL_DPCH_SS_STEP_SIZE_MIN                1
#define TDSL1_UL_DPCH_SS_STEP_SIZE_MAX                8
#define TDSL1_UL_DPCH_SS_STEP_SIZE_DEFAULT            1


/* Uplink synchronisation frequency to be used for the adjustment of the
 * transmission timing. 
 */
#define TDSL1_UL_DPCH_SS_STEP_FREQ_MIN                1
#define TDSL1_UL_DPCH_SS_STEP_FREQ_MAX                8
#define TDSL1_UL_DPCH_SS_STEP_FREQ_DEFAULT            1

/* The minimum number of uplink timeslots */
#define TDSL1_UL_MIN_NUM_TS                           1
/* The maximum number of uplink timeslots */
#define TDSL1_UL_MAX_NUM_TS                           5

/* uplink timeslot number, ts1~ts5 */
#define TDSL1_UL_MIN_TS_NUM                           1
#define TDSL1_UL_MAX_TS_NUM                           5

/* Min Tx Power in dBm */
#define TDSL1_UL_MAX_TX_PWR_MIN                       (-50)
/* Max Tx Power in dBm */
#define TDSL1_UL_MAX_TX_PWR_MAX                       33

/* For DPCH initial setup/recfg, SB shall be programmed continuously at most 
 * 160ms following detection of in sync. 
 */
#define TDSL1_UL_160MS_SB                             16
/* For DTX, only one frame SB shall be programmed */
#define TDSL1_UL_10MS_SB                              1

/* maximum number of access service classes */
#define TDSL1_UL_ASC_MAX                              8

/* UpPCH shifting */
#define TDSL1_UL_UPPCH_SHIFT_MAX                      127

/* sub-channel size */
#define TDSL1_UL_SUB_CHAN_SIZE_MAX                    8

/* wait time */
#define TDSL1_UL_WT_MIN                               1
#define TDSL1_UL_WT_MAX                               4

/* Maximum UL sync code transmission */
#define TDSL1_UL_MAX_SYNC_CODE_TX                     8

#define TDSL1_UL_PRACH_CODE_MIN                       1
#define TDSL1_UL_PRACH_CODE_MAX                       4

/* The number of PRACH-FPACH pairs */
#define TDSL1_UL_PRACH_FPACH_MIN                      1
#define TDSL1_UL_PRACH_FPACH_MAX                      8

/* Maximum #TFCs allowed - implementation specific */
#define TDSL1_UL_TFC_MAX                              64

/* Reference TFC range */
#define TDSL1_UL_REF_TFC_MAX                          4

/* zhouya add for 10.3.6.88 begin */
#define  TDSL1_UL_CHAN_CODE_1                         1
#define  TDSL1_UL_CHAN_CODE_2                         2
#define  TDSL1_UL_CHAN_CODE_3                         3
#define  TDSL1_UL_CHAN_CODE_4                         4
#define  TDSL1_UL_CHAN_CODE_5                         5
#define  TDSL1_UL_CHAN_CODE_6                         6
#define  TDSL1_UL_CHAN_CODE_7                         7
#define  TDSL1_UL_CHAN_CODE_8                         8
#define  TDSL1_UL_CHAN_CODE_9                         9
#define  TDSL1_UL_CHAN_CODE_10                        10
#define  TDSL1_UL_CHAN_CODE_11                        11
#define  TDSL1_UL_CHAN_CODE_12                        12
#define  TDSL1_UL_CHAN_CODE_13                        13
#define  TDSL1_UL_CHAN_CODE_14                        14
#define  TDSL1_UL_CHAN_CODE_15                        15
#define  TDSL1_UL_CHAN_CODE_16                        16
/* zhouya add for 10.3.6.88 end */


/**------------------------------------------------------------------------
 ** Enum 
 ** -----------------------------------------------------------------------*/

/* This enum defines the uplink transport channel types */
typedef enum 
{
  TDSL1_UL_TRCH_RACH,
  TDSL1_UL_TRCH_DCH,
  TDSL1_UL_NUM_TRCH,
  TDSL1_UL_TRCH_NONE
} tdsl1_ul_trch_enum_type;

/* Uplink physical channels */
typedef enum 
{
  TDSL1_UL_PHYCHAN_DPCH,
  TDSL1_UL_PHYCHAN_PRACH,
  TDSL1_UL_NUM_PHYCHAN,
  TDSL1_UL_PHYCHAN_NONE
} tdsl1_ul_phychan_enum_type;

/* filler polarity */
typedef enum {
  /* pad code segments with 1s */
  TDSL1_UL_RF_EQ_FILLER_0,
  /* pad code segments with 0s */
  TDSL1_UL_RF_EQ_FILLER_1,
  TDSL1_UL_NUM_FILLER_TYPE
} tdsl1_ul_filler_polarity_enum_type;

typedef enum {
  TDSL1_UL_GF_NONE,
  TDSL1_UL_GF_CALCULATED,
  TDSL1_UL_GF_SIGNALLED
} tdsl1_ul_gain_factor_calc_enum_type;






/**=========================================================================
 **
 **                              Downlink Part
 **
 **==========================================================================*/


/**------------------------------------------------------------------------
 ** Macro 
 ** -----------------------------------------------------------------------*/

#define TDSL1_DL_MAX_TRBLK_PER_TRCH                   32

/*TDSL1_DL_MAX_TB_BIT_SIZE*/
#define TDSL1_DL_MAX_TB_BIT_SIZE                      6400

/* max downlink timeslot number, ts0, ts2~ts6 */
#define TDSL1_DL_MAX_NUM_TS                           6 

#define TDSL1_DL_MAX_SCCPCH                           1

#define TDSL1_DL_MAX_REPETITION_PERIOD                64

#include "tdscdma_variation.h"
#define TDSL1_DL_MAX_DPCH                             1

#define TDSL1_DL_CCTRCH_ID_COUNT                      6

#define TDSL1_DL_MIN_TRCH_ID                          1

#define TDSL1_DL_MAX_TRCH_ID                          32

/* The following parameters are based on 5.2.2 TS 25.306 V9.5.0 */
/** Max transport channels in one CCTrCh 
#define MCALTDS_DEC_MAX_TRCH                                         8 */
#define TDSL1_DL_MAX_TRCH                             8

/** Maximum number of transport format per transport channel,
 *  32 from 11.4 TS 25.331 v9.50
#define MCALTDS_DEC_MAX_TF_PER_TRCH                              64 */
#define TDSL1_DL_MAX_TF                               64

/** Total number of CTFC entries per table in mDSP,  */
/* previously TDSDEC_TOTAL_CTFC_ENTRIES_PER_TABLE */
#define TDSL1_DL_MAX_TFC                              128

/* Chipx8 per sub-frame */
#define TDSL1_CX8_PER_SUB_FRAME                       51200
/* Chipx1 per sub-frame */
#define TDSL1_CX1_PER_SUB_FRAME                       6400
/* Chipx8 per ms */
#define TDSL1_CX8_PER_MS                              10240
/* Chipx1 per ms */
#define TDSL1_CX1_PER_MS                              1280

/* 5 ms per sub-frame*/
#define TDSL1_MSEC_PER_SUB_FRAME                      5

/* 5000 us per sub-frame*/
#define TDSL1_USEC_PER_SUB_FRAME                      5000

#define TDSL1_MAX_SUB_FRAME_NUM                       8192
#define TDSL1_MAX_FRAME_NUM                           4096
#define TDSL1_INVALID_SUB_FN                          0xFFFF
#define TDSL1_MAX_CFN_COUNT                           256

#define TDSL1_INVALID_FREQ                            0xFFFF
#define TDSL1_INVALID_FREQ_ERR                        0x7FFFFFFF


/** Max Simultaneous CCTRCH **/
#define TDSL1_DL_MAX_CCTRCH                           3

/**------------------------------------------------------------------------
 ** Enum 
 ** -----------------------------------------------------------------------*/
#if 0
typedef enum //xuqiangz: standard?
{
  TDSL1_DL_TRCH_SBCH_TYPE,
  /* TrCh is for neighbor BCH  */
  TDSL1_DL_TRCH_NBCH_TYPE,          
  TDSL1_DL_TRCH_DCH_TYPE,
  TDSL1_DL_TRCH_PCH_TYPE,
  TDSL1_DL_TRCH_FACH_TYPE,
  TDSL1_DL_TRCH_HS_DSCH_TYPE,
  TDSL1_DL_NUM_TRCH_REF_TYPE,
  TDSL1_DL_TRCH_INVALID
} tdsl1_dl_trch_ref_type_enum_type;

typedef enum //xuqiangz: standard?
{
  TDSL1_DL_PHYCHAN_PCCPCH_S,
  TDSL1_DL_PHYCHAN_PCCPCH_N,
  TDSL1_DL_PHYCHAN_SCCPCH0,
  TDSL1_DL_PHYCHAN_SCCPCH1,
  TDSL1_DL_PHYCHAN_FPACH,
  TDSL1_DL_PHYCHAN_PICH,
  TDSL1_DL_PHYCHAN_DPCH,
  TDSL1_DL_PHYCHAN_PDSCH,
   /*TDSL1_NUM_DL_PHYCHAN*/
  TDSL1_DL_NUM_PHYCHAN, 
  TDSL1_DL_PHYCHAN_NOCHAN
} tdsl1_dl_phychan_enum_type;
#endif
/*tdsl1_dl_ch_code_enum_type*/
typedef enum 
{
  TDSL1_DL_SF16_CH_CODE_1,
  TDSL1_DL_SF16_CH_CODE_2,
  TDSL1_DL_SF16_CH_CODE_3,
  TDSL1_DL_SF16_CH_CODE_4,  
  TDSL1_DL_SF16_CH_CODE_5,
  TDSL1_DL_SF16_CH_CODE_6,
  TDSL1_DL_SF16_CH_CODE_7,
  TDSL1_DL_SF16_CH_CODE_8,
  TDSL1_DL_SF16_CH_CODE_9,
  TDSL1_DL_SF16_CH_CODE_10,
  TDSL1_DL_SF16_CH_CODE_11,
  TDSL1_DL_SF16_CH_CODE_12,
  TDSL1_DL_SF16_CH_CODE_13,
  TDSL1_DL_SF16_CH_CODE_14,
  TDSL1_DL_SF16_CH_CODE_15,
  TDSL1_DL_SF16_CH_CODE_16,
  TDSL1_DL_SF1_CH_CODE,
  TDSL1_DL_INVALID_CH_CODE = 0xFF
}tdsl1_dl_ch_code_enum_type;

typedef enum 
{
  TDSL1_DL_PI_LEMGTH_4,
  TDSL1_DL_PI_LEMGTH_8,
  TDSL1_DL_PI_LEMGTH_16,
  TDSL1_DL_INVALID_PI_LEMGTH
}tdsl1_dl_pi_length_enum_type;

typedef enum 
{
  TDSL1_DL_PICH_NUM_GAP_2,
  TDSL1_DL_PICH_NUM_GAP_4,
  TDSL1_DL_PICH_NUM_GAP_8,
  TDSL1_DL_INVALID_PICH_NUM_GAP
}tdsl1_dl_pich_num_gap_enum_type;

/* 25.306 Section 5 UE radio access capability, Table 5.1c */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT1  11264  /* For Cat 1  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT2  22528  /* For Cat 2  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT3  33792  /* For Cat 3  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT4  22528  /* For Cat 4  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT5  45056  /* For Cat 5  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT6  67584  /* For Cat 6  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT7  33792  /* For Cat 7  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT8  67584  /* For Cat 8  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT9  101376 /* For Cat 9  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT10 45056  /* For Cat 10 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT11 90112  /* For Cat 11 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT12 135168 /* For Cat 12 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT13 56320  /* For Cat 13 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT14 112640 /* For Cat 14  */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT15 168960 /* For Cat 15 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT16 50688  /* For Cat 16 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT17 101376 /* For Cat 17 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT18 152064 /* For Cat 18 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT19 67584  /* For Cat 19 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT20 135168 /* For Cat 20 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT21 202752 /* For Cat 21 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT22 84480  /* For Cat 22 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT23 168960 /* For Cat 23 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT24 253440 /* For Cat 24 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT25 152064 /* For Cat 25 */ // For non MIMO only
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT26 202752 /* For Cat 26 */ // For non MIMO only
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT27 253440 /* For Cat 27 */ // For non MIMO only
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT28 304128 /* For Cat 28 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT29 405504 /* For Cat 29 */
#define TDSL1_HS_PDSCH_VIR_IR_BUF_SZ_CAT30 506880 /* For Cat 30 */

/* -----------------------------------------------------------------------
** Constant / Define Declarations for MCAL - moved from MCALTDS_DEC.H
** ----------------------------------------------------------------------- */

/* TrCh ref type bit field */
#define TDSL1_DL_TRCH_REFTYPE_SBCH_BF 0x01 /**< TrCh reference type S_BCH, previously  MCALTDS_DEC_TRCH_REFTYPE_SBCH_BF */
#define TDSL1_DL_TRCH_REFTYPE_NBCH_BF 0x02 /**< TrCh reference type N_BCH, MCALTDS_DEC_TRCH_REFTYPE_NBCH_BF */
#define TDSL1_DL_TRCH_REFTYPE_DCH_BF  0x04 /**< TrCh reference type DCH, MCALTDS_DEC_TRCH_REFTYPE_DCH_BF */
#define TDSL1_DL_TRCH_REFTYPE_DSCH_BF 0x08 /**< TrCh reference type DSCH (Obsolete), MCALTDS_DEC_TRCH_REFTYPE_DSCH_BF */
#define TDSL1_DL_TRCH_REFTYPE_PCH_BF  0x10 /**< TrCh reference type PCH,  MCALTDS_DEC_TRCH_REFTYPE_PCH_BF */
#define TDSL1_DL_TRCH_REFTYPE_FACH_BF 0x20 /**< TrCh reference type FACH, MCALTDS_DEC_TRCH_REFTYPE_FACH_BF */

/* Value for CRC length for each CRC enum type */
#define TDSL1_CRC_LENGTH_0_VAL     0  /**< CRC length 0 size */
#define TDSL1_CRC_LENGTH_8_VAL     8  /**< CRC length 8 size */
#define TDSL1_CRC_LENGTH_12_VAL    12 /**< CRC length 12 size */
#define TDSL1_CRC_LENGTH_16_VAL    16 /**< CRC length 16 size */
#define TDSL1_CRC_LENGTH_24_VAL    24 /**< CRC length 24 size */

/* PCCPCH TF info */
#define TDSL1_BCH_NUM_TB           1
#define TDSL1_BCH_TB_SIZE_BITS     246

/* Values for TTI in ms for each enum type */
#define TDSL1_TTI_5MS_VAL          5 /**< TTI value for 5ms  */ 
#define TDSL1_TTI_10MS_VAL         10 /**< TTI value for 10 ms */
#define TDSL1_TTI_20MS_VAL         20 /**< TTI value for 20 ms */
#define TDSL1_TTI_40MS_VAL         40 /**< TTI value for 40 ms */
#define TDSL1_TTI_80MS_VAL         80 /**< TTI value for 80 ms */


/* UE need not support more than 16 reporting criteria in total. (25.133) */
#define TDSL1_QUALMEAS_MAX_QUALITY_MEAS 16
/* UE should not support more than 2 measurements per transport channel. (25.133) */
#define TDSL1_QUALMEAS_MAX_MEAS_PER_TRANSPORT_CHANNEL 2

/* We can get upto 32 transport channel in an MCM and can support two measurements
   per transport channel, so need to support 64 channels at any given time. But since
   currently we support 8 transport channels in DCH state, this is defined as 16 */

/* Maximum number of tranposrt channels in the transport channels database */
#define TDSL1_QUALMEAS_MAX_TRANSPORT_CHANNEL_INDEX 16

#define TDSL1_INTERNMEA_MAX_INTERNAL_MEAS 8


#endif /* TDSL1_STD_H */
