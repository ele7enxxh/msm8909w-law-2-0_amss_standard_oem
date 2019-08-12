/*!
  @file
  lte_as.h

  @brief
  This file contains definitions for the Common type and constants required
  by different modules within LTE Access Stratum(AS)

*/

/*=============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_as.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
04/24/13   kp      Enable TM8 for L-TDD & disable FGI-9 for FDD by default
                   it can be enabled only through EFS.
04/10/13   kp      Added code to enable FGI-30(TDD--FDD HO) for NI 5.x 
12/01/11   st      Added a new RNTI type in the enum to match with ML1 for 
                   eMBMS 
10/25/11   kp      Added code to indicate FGI bit -27 for SRVCC feature
13/10/11   anm     eMBMS monitor support
08/31/11   isalman Don't use q6 headers when compiling for UMPLT.
07/18/11   mm      Added defintions for eMBMS
05/08/11   vatsac  Added LTE_BW_UNKNOWN to lte_bandwidth_e
04/19/11   ar      Renamed FEATURE_NIKEL_REDUCED_HARQS to 
                   FEATURE_LTE_NIKEL_REDUCED_HARQS
04/15/11   ar      Enabled FEATURE_NIKEL_REDUCED_HARQS
03/31/11   mm      Enabled ROHC profiles 1 and 2
01/06/11   sureshs Renamed FGI bit Multi User Mimo to PDSCH TM 5 per Sep spec
12/14/10   np      (Conditionally) enable e1xCSFB (11, 24) and PSHO (8, 22) FGI bits
09/20/10   wshaw   move wdog info to lte_common.h
09/17/10   wshaw   add watchdog support for LTE tasks
06/03/10   mm      Enabled long DRX and intra freq measurments for SON/ANR
05/25/10   kparikh Reverted the FEATURIZATION changes present in version 11
05/25/10   kparikh Reduced the number of SRBs in the code for the LTE PLT builds
03/23/10   sureshs Moved UE category to lte.h from lte_as.h
01/22/10   sureshs Changed UE category to 3
01/20/10   sureshs Added feature group indicators for Dec release
01/04/09   svk     Added lte_uarfcn_t
12/11/09   vatsac  Including header file lte_cfg.h for LTE AS customizations
11/05/09   sureshs Aligned feature group indicators with Sep release
10/23/09   sureshs Changed some capability defines for 2.0 release
10/23/09   sureshs Added some capability defines for eventual June release work
08/13/09   bn      Added API for MAC indicates to ML1 when failed sending PHR
08/03/09   mm      Added definition for lte_phy_cell_id_t. Added message
                   structure definition for LTE_RRC_HANDOVER_COMPLETED_IND
07/28/09   bn      Put back the changes for lte common indication
07/09/09   bn      Included lte_ind.h and added in MAC contention ind structure
06/02/09   sureshs Added capability defines for eventual March release work
06/02/09   sureshs Changed UE category to 2 in capabilities
04/17/09   dd      Moved lte_cipher_algo_e and lte_integrity_algo_e to lte.h
04/10/09   mm      Added LTE_OTA_IE_INFINITY_UINT32 for uint32 types
04/09/09   jyw     Added lte_ml1_get_curr_sfn
03/17/09   sureshs Removed LTE_UL_TX_DIVERSITY_SUPPORTED
03/06/09   sureshs Added LTE_MAX_NUMBER_ROHC_CONTEXT_SESSIONS
03/06/09   bn      Added in defined for invalid RA index mask
03/06/09   sureshs Added LTE_UL_TX_ANTENNA_SELECTION_SUPPORTED
01/30/09   sureshs Added defines for LTE capabilities
01/20/09   sah     Add back in the fixed rnti values for P-RNTI and SI-RNTI
11/14/08   sm      Added lte_rb_cfg_idx_t and LTE_MAX_RB_CFG_IDX
10/28/08   mm      Added a new bandwidth value to lte_bandwidth_e enum
10/25/08   sm      Added Macro's LTE_LIKELY_TRUE and LTE_UNLIKELY_TRUE for
                   branch prediction
10/14/08   sureshs Changes for using new LTE band enums
10/03/08   sureshs Added more LTE bands
08/04/08   jyw     Removed "1xerrno.h" and added common types from L1 side.
07/30/08   sm      initial version
=============================================================================*/

#ifndef LTE_AS_H
#define LTE_AS_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <comdef.h>
#include <lte_ind_msg.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*! @brief
    This Macro provides the compiler with branch prediction information.

    Calling LTE_LIKELY_TRUE(expr) informs the compiler that it is expected that
    expr == 1 <NOTE: expr has to be equal to 1 not greater than 1>.

    Calling LTE_UNLIKELY_TRUE(expr) informs the compiler that it is expected
    that expr is going to be 0, hence dont perform prefetching of instructions
    inside.

    @usage
    Use "if (LTE_LIKELY_TRUE(a==b))" {} if expression '(a==b)' is expected to be
    TRUE most of the time else use "if (LTE_UNLIKELY(a==b)"
*/
#if defined(__GNUC__) && !defined(_lint)
  #define LTE_LIKELY_TRUE(x) __builtin_expect((x), 1)
  #define LTE_UNLIKELY_TRUE(x) __builtin_expect((x), 0)
#else
  #define LTE_LIKELY_TRUE(x) (x)
  #define LTE_UNLIKELY_TRUE(x) (x)
#endif /* ! (__GNUC__) */


/*! @brief LTE 3GPP Release supported by the UE
*/
#define LTE_3GPP_REL 8
/*! @brief LTE 3GPP Release Month
*/
#define LTE_3GPP_REL_MONTH 9
/*! @brief LTE 3GPP Release Year
*/
#define LTE_3GPP_REL_YEAR 2008

/*! @brief Maximum Number of Radio Bearers, this only includes Unicast RBs
*/
#define LTE_MAX_RB 32

/*! @brief Maximum Number of Logical Channels, this only includes Unicast LCs
*/
#define LTE_MAX_LC 32

/*!< @brief Maximim Number of MBSFN Areas */

#define LTE_EMBMS_MAX_MBSFN_AREA 8




/*!< @brief Default MCCH RB ID */
#define LTE_EMBMS_MCCH_RB_ID 35

/*! @brief macro for Min RB id of MTCH
*/
#define LTE_MIN_MTCH_RB_ID (LTE_EMBMS_MCCH_RB_ID + 1)

/*! @brief macro for Max RB id of MTCH
*/
#define LTE_MAX_MTCH_RB_ID (LTE_MIN_MTCH_RB_ID + (LTE_EMBMS_MAX_ACTIVE_SESSIONS - 1))

/*! @brief Maximum Number of Multicast Logical Channels
  For now, assume 32 MRBs (32 MTCHs)
*/
#define LTE_MAX_ACTIVE_MRB 32

/*! @brief Maximum Number of de prioritized frequency
*/
#define LTE_MAX_NUM_DEPRI_FREQ 64

/*! @brief Maximum Number of Multicast Logical Channels
  For now, assume 8 MTCHs
*/
#define LTE_MAX_EMBMS_LC LTE_MAX_ACTIVE_MRB

/*! @brief Maximum Number of active Signalling Radio Bearers supported in UE
    TBD: Need to confirm (shall we consider only AM/UM SRBs here?)
*/
#define LTE_MAX_ACTIVE_SRB 2

/*! @brief Maximum Number of active Data Radio Bearers supported in UE
    TBD: Need to change the value of this macro as per decision in 3gpp
*/
#define LTE_MAX_ACTIVE_DRB 8

/*! @brief Maximum Number of Active Radio Bearers (SRB + DRB)
*/
#define LTE_MAX_ACTIVE_RB (LTE_MAX_ACTIVE_SRB + LTE_MAX_ACTIVE_DRB)

/*! @brief Maximum Number of Active Logical Channels (over SRB and DRB)
    Note: Logical channels have one to one mapping with radio bearers
*/
#define LTE_MAX_ACTIVE_LC LTE_MAX_ACTIVE_RB

/*! @brief Maximum Number of Radio Bearer Configuration Index for defining
    array for direct hashing
*/
#define LTE_MAX_RB_CFG_IDX (LTE_MAX_RB + LTE_MAX_ACTIVE_SRB + 1)

/*! @brief Maximum Number of RLC PDUs that can be received in
    a given transport block (MAC PDU)
*/
#define LTE_MAX_RLC_PDU_PER_TR_BLK 10

/*! @brief Define invalid RA ID
*/
#define LTE_INVALID_RA_ID            0xFF

/*! @brief Define invalid RA ID MASK
*/
#define LTE_INVALID_RA_ID_MASK       0xFF

/*! @brief used to indicate that OTA IE has infinity value.  used for uint16
*/
#define LTE_OTA_IE_INFINITY_UINT16  0xFFFF

/*! @brief used to indicate that OTA IE has infinity value.  used for uint32
*/
#define LTE_OTA_IE_INFINITY_UINT32  0xFFFFFFFF

/*! Default value for p_max */
#define LTE_DEFAULT_PMAX           (23)

/*! @brief unused marcro
*/
#define LTE_UNUSED(x)   NOTUSED(x)

/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/
/*! @brief Radio Bearer Mode
*/
typedef enum
{
  LTE_RB_RLC_MODE_INVALID, /*!< For initialization usage */
  LTE_RB_RLC_MODE_AM,      /*!< Acknowledgement mode */
  LTE_RB_RLC_MODE_UM,      /*!< Un-acknowledgement mode */
  LTE_RB_RLC_MODE_TM       /*!< Transparent mode */
} lte_rb_rlc_mode_e;

/*! @brief Radio Bearer type
*/
typedef enum
{
  LTE_RB_TYPE_INVALID, /*!< For initialization usage */
  LTE_RB_TYPE_SRB,     /*!< Unicast Signalling Radio Bearer */
  LTE_RB_TYPE_DRB,     /*!< Unicast Data Radio Bearer */
  LTE_RB_TYPE_MTCH     /*!< Multicast Data Radio Bearer */
} lte_rb_type_e;



/*=============================================================================

                   EXTERNAL STRUCTURE/UNION TYPES

=============================================================================*/



/*=============================================================================

                   DATA TYPES

=============================================================================*/
/*! @brief
  Physical cell ID type Range: 0..503
  Defined in 36.331 Section 6.3.4

  @see LTE_L1_MAX_PHY_CELL_ID for the maximum valid cell id value
  @see LTE_L1_NO_CELL_ID to denote that the cell id is not present
*/
typedef uint16 lte_phy_cell_id_t;

/*! @brief This defines the radio bearer configuration index type
*/
typedef uint8 lte_rb_cfg_idx_t;

/*! @brief This defines the Radio Bearer identity as described in 36.331 (RRC)
*/
typedef uint8 lte_rb_id_t;

/*! @brief This defines the Logical Channel identity as described in 36.331(RRC)
*/
typedef uint8 lte_lc_id_t;

/*! @brief This defines the MBSFN area identity as described in 36.331(RRC)
*/
typedef uint8 lte_mbsfn_area_id_t;

/*! @brief This defines the PMCH identity as described in 36.331(RRC)
*/
typedef uint8 lte_pmch_id_t;

/*===========================================================================

                   RF CHANNEL, BANDWIDTH RELATED TYPES

===========================================================================*/
/*! @ brief
    Number of LTE supported bandwidths
    Defined in 36.101 Section 5.4.2.1
*/
#define  LTE_MAX_NUM_BANDWIDTHS  6

/*! @brief
  LTE support bandwidths
  Defined in 36.101 Section 5.4.2.1
*/
typedef enum
{
  /*! 1.4MHz bandwidth */
  LTE_BW_NRB_6 = 6,
  /*! 3MHz bandwidth */
  LTE_BW_NRB_15 = 15,
  /*! 5MHz bandwidth */
  LTE_BW_NRB_25 = 25,
  /*! 10MHz bandwidth */
  LTE_BW_NRB_50 = 50,
  /*! 15MHz bandwidth */
  LTE_BW_NRB_75 = 75,
  /*! 20MHz bandwidth */
  LTE_BW_NRB_100 = 100,
  /*! Minimum supported bandwidth */
  LTE_BW_MIN = LTE_BW_NRB_6,
  /*! Maximum supported bandwidth */
  LTE_BW_MAX = LTE_BW_NRB_100,
  /*! Unknown Bandwidth */
  LTE_BW_UNKNOWN
} lte_bandwidth_e;

/*! @brief
  LTE support bandwidths as indices
  Defined in 36.101 Section 5.4.2.1
*/
typedef enum
{
  /*! 1.4MHz bandwidth */
  LTE_BW_IDX_NRB_6 = 0,
  /*! 3MHz bandwidth */
  LTE_BW_IDX_NRB_15,
  /*! 5MHz bandwidth */
  LTE_BW_IDX_NRB_25,
  /*! 10MHz bandwidth */
  LTE_BW_IDX_NRB_50,
  /*! 15MHz bandwidth */
  LTE_BW_IDX_NRB_75,
  /*! 20MHz bandwidth */
  LTE_BW_IDX_NRB_100,
  /*! Minimum supported bandwidth */
  LTE_BW_IDX_MIN = LTE_BW_IDX_NRB_6,
  /*! Maximum supported bandwidth */
  LTE_BW_IDX_MAX = LTE_BW_IDX_NRB_100
} lte_bandwidth_idx_e;

/*! @brief
    LTE support bandwidths as bits in a bitmask
    Defined in 36.101 Section 5.4.2.1
*/
#define  LTE_BW_BMASK_NRB_6    (1 << (uint32) LTE_BW_IDX_NRB_6)
#define  LTE_BW_BMASK_NRB_15   (1 << (uint32) LTE_BW_IDX_NRB_15)
#define  LTE_BW_BMASK_NRB_25   (1 << (uint32) LTE_BW_IDX_NRB_25)
#define  LTE_BW_BMASK_NRB_50   (1 << (uint32) LTE_BW_IDX_NRB_50)
#define  LTE_BW_BMASK_NRB_75   (1 << (uint32) LTE_BW_IDX_NRB_75)
#define  LTE_BW_BMASK_NRB_100  (1 << (uint32) LTE_BW_IDX_NRB_100)

/*! @brief 
    LTE supported bandwidths in terms of frequency as bits in a bitmask
    Defined in 36.101 Section 5.4.2.1
*/
#define  LTE_BW_BMASK_1_4_MHZ  (1 << (uint32) LTE_BW_IDX_NRB_6)
#define  LTE_BW_BMASK_3_MHZ    (1 << (uint32) LTE_BW_IDX_NRB_15)
#define  LTE_BW_BMASK_5_MHZ    (1 << (uint32) LTE_BW_IDX_NRB_25)
#define  LTE_BW_BMASK_10_MHZ   (1 << (uint32) LTE_BW_IDX_NRB_50)
#define  LTE_BW_BMASK_15_MHZ   (1 << (uint32) LTE_BW_IDX_NRB_75)
#define  LTE_BW_BMASK_20_MHZ   (1 << (uint32) LTE_BW_IDX_NRB_100)


/*! @brief
  Describing the absolue cell's frequency Range: 0..262143
  Defined in 36.1.1 Section 5.4.4
  03/11/14: Extend to 32 bit, Range: 0..4294967295
*/
typedef uint32 lte_earfcn_t;

/*! @brief
  Describing the absolue cell's frequency Range: 0..65535
  Used to covert to/from efs/mem format
*/
typedef uint16 lte_efs_earfcn_t;

/*! @brief
  Describing the absolue cell's frequency Range: 1..64
  Used to covert to/from efs/mem format
*/
typedef uint8 lte_efs_band_enum_t;

/*! @brief
  Describing the absolue cell's band Range: 1..256
  Used to covert to/from efs/mem format
*/
typedef uint16 lte_efs_extnd_band_enum_t;

/*! @brief
  UTRA frequency Range: 0..16383
  @TODO remove once better home is found for such #defines
*/

typedef uint16 lte_uarfcn_t;

/*===========================================================================

                    LTE SYSTEM INFORMATION TYPES

===========================================================================*/
/*! @brief
  Enumeration of supported RATs
  Defined in 36.331 Section 6.3.4
*/
typedef enum
{
  /*! LTE type */
  LTE_RAT_EUTRAN = 0,
  /*! WCDMA type */
  LTE_RAT_UTRAN,
  /*! GSM type */
  LTE_RAT_GERAN,
  /*! 1X RTT type */
  LTE_RAT_CDMA1XRTT,
  /*! HRPD type */
  LTE_RAT_CDMAHRPD,
  /*! TDSCDMA type */
  LTE_RAT_UTRAN_TDD,
  /*! MAX number of RATs supported */
  LTE_MAX_NUM_RATS,
  /*! Indicates no RAT */
  LTE_RAT_NONE
} lte_rat_e;


/*===========================================================================

                    RNTI TYPES

===========================================================================*/

/*!
  @defgroup rntis

  Describes the ranges and values for RNTIs from 36.321 Section 7.1

  @{
  */
#define LTE_SI_RNTI_VAL 0xFFFF  
#define LTE_P_RNTI_VAL 0xFFFE   // conflict with  lte_pdcch_rnti_e (lte_ml1_gm_ul.h)
#define LTE_M_RNTI_VAL 0xFFFD

#define LTE_RA_RNTI_MIN_FDD 0x1
#define LTE_RA_RNTI_MAX_FDD 0xA

#define LTE_RA_RNTI_MIN_TDD 0x1
#define LTE_RA_RNTI_MAX_TDD 0x3C

#define LTE_COM_RNTI_POOL_MIN_FDD 0x1
#define LTE_COM_RNTI_POOL_MAX_FDD 0xFFF3

#define LTE_COM_RNTI_POOL_MIN_TDD 0x1
#define LTE_COM_RNTI_POOL_MAX_TDD 0xFFF3

#define LTE_RNTI_RESERVED_MIN 0xFFF4
#define LTE_RNTI_RESERVED_MAX 0xFFFD

#define LTE_MAX_RNTI 0xFFFF

/* @} */ /* end group rntis */

/* LTE capabilities begin */
/* LTE_UE_CATEGORY moved to lte.h */
#define LTE_SON_RACH_RPT_R9_SUPPORTED                        TRUE

#define LTE_TX_ANTENNA_SELECTION_SUPPORTED_TDD               TRUE
#define LTE_TX_ANTENNA_SELECTION_SUPPORTED_FDD               FALSE
#define LTE_ROHC_PROFILE_0x0001_SUPPORTED                    TRUE
#define LTE_ROHC_PROFILE_0x0002_SUPPORTED                    TRUE
#define LTE_ROHC_PROFILE_0x0003_SUPPORTED                    FALSE
#define LTE_ROHC_PROFILE_0x0004_SUPPORTED                    FALSE
#define LTE_ROHC_PROFILE_0x0006_SUPPORTED                    FALSE
#define LTE_ROHC_PROFILE_0x0101_SUPPORTED                    FALSE
#define LTE_ROHC_PROFILE_0x0102_SUPPORTED                    FALSE
#define LTE_ROHC_PROFILE_0x0103_SUPPORTED                    FALSE
#define LTE_ROHC_PROFILE_0x0104_SUPPORTED                    FALSE
/* valid values as defined in 36.331 section 6.3.6
    cs2, cs4, cs8, cs12, cs16, cs24, cs32, cs48, cs64, cs128, cs256, cs512, 
    cs1024, cs16384
    cs2 corresponds with 2 (context sessions), cs4 corresponds with 4 and so on
, setting it to the default 16 */
#define LTE_MAX_NUMBER_ROHC_CONTEXT_SESSIONS                 0x10
#define LTE_SPECIFIC_REF_SIGS_SUPPORTED                      FALSE
#define LTE_HALF_DUPLEX                                      FALSE
#define LTE_INTER_FREQ_MEAS_GAPS_NEEDED                      TRUE
#define LTE_INTER_FREQ_MEAS_GAPS_NOT_NEEDED                  FALSE    
#define LTE_IRAT_MEAS_GAPS_NEEDED                            TRUE
#define LTE_FEICIC_DCCH_SIB1_SUPPORTED                                 TRUE
#define LTE_FEICIC_CRS_SUPPORTED                                 TRUE
#define LTE_TDD_SPECIAL_SUBFRAME_SUPPORTED                   TRUE

/* Rel8 feature group indicators */
/* bit 1 */
#define LTE_INTRA_SUBFRAME_FREQ_HOPPING                      TRUE
#define LTE_DCI_FORMAT_3A                                    TRUE
/* #define LTE_MULTI_USER_MIMO                                  FALSE */
/* Renamed per Sept 2010 36.331 */
#define LTE_PDSCH_TM_5                                       FALSE
#define LTE_APERIODIC_CQI_REP_WITHOUT_PMI                    TRUE
#define LTE_APERIODIC_CQI_REP_WITH_MULT_PMI                  TRUE
/* bit 2 */
#define LTE_SIMUL_CQI_ACK_NACK                               TRUE
#define LTE_ABS_TPC_CMD                                      TRUE
#define LTE_RES_ALLOC_TYPE_1                                 TRUE
#define LTE_PERIODIC_CQI_REP_WITHOUT_PMI                     TRUE
#define LTE_PERIODIC_CQI_REP_WITH_SINGLE_PMI                 TRUE
/* bit 3 */
#define LTE_SEMI_PERSISTENT_SCHEDULING                       TRUE
#define LTE_TTI_BUNDLING                                     TRUE
#define LTE_5BIT_RLC_UM_SN                                   TRUE
#define LTE_7BIT_PDCP_SN                                     TRUE

/* bit 4 */
#define LTE_SHORT_DRX_CYCLE                                  TRUE
/* bit 5 */
#define LTE_LONG_DRX_CYCLE                                   TRUE
#define LTE_DRX_CMD_MAC_CTRL_ELEM                            TRUE

/* bit 6 */
#define LTE_PRIORITIZED_BIT_RATE                             TRUE
/* bit 7 */
#define LTE_RLC_UM                                           TRUE
/* bit 8 (see custlte.h) */
#define LTE_EUTRA_CONN_TO_UTRA_CELL_DCH_PS_HO \
        LTE_EUTRA_CONN_TO_UTRA_CELL_DCH_PS_HO_DEFINED
/* bit 9 */
#define LTE_EUTRA_CONN_TO_GERAN_GSM_DED_HO                   TRUE

/* bit 10 */
#define LTE_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO \
        LTE_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_DEFINED

#define LTE_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_WITH_NACC \
        LTE_EUTRA_CONN_TO_GERAN_IDLE_BY_CCO_WITH_NACC_DEFINED

/* bit 11 (see custlte.h) */
#define LTE_EUTRA_CONN_TO_1XRTT_CS_ACTIVE_HO \
        LTE_EUTRA_CONN_TO_1XRTT_CS_ACTIVE_HO_DEFINED
/* bit 12 */
#define LTE_EUTRA_CONN_TO_HRPD_ACTIVE_HO \
        LTE_EUTRA_CONN_TO_HRPD_ACTIVE_HO_DEFINED
/* bit 13 */
#define LTE_INTER_FREQ_HO                                    TRUE
/* bit 14 */
#define LTE_MEAS_EVENT_A4                                    TRUE
#define LTE_MEAS_EVENT_A5                                    TRUE

/* bit 15 (see custlte.h) */
#define LTE_MEAS_EVENT_B1               LTE_MEAS_EVENT_B1_DEFINED

/* bit 16 */
#define LTE_MEAS_PERIODIC_REP_NON_ANR                        TRUE
/* bit 17 - Can be set to TRUE only if bit 5 is set to TRUE */
#define LTE_MEAS_PERIODIC_REP_SON_ANR                        TRUE
#define LTE_MEAS_ANR_INTRA_FREQ_REP                          TRUE
/* bit 18 */
#define LTE_MEAS_ANR_INTER_FREQ_REP                          TRUE
/* bit 19 */
#define LTE_MEAS_ANR_INTER_RAT_REP                           TRUE

/* bit 20 */
#define LTE_5X_AM_DRB_3X_UM_DRB                              TRUE
#define LTE_8X_AM_DRB                                        TRUE
/* bit 21 */
#define LTE_PREDEFINED_INTRA_SUBFRAME_FREQ_HOPPING           TRUE
#define LTE_PREDEFINED_INTER_SUBFRAME_FREQ_HOPPING           TRUE

/* bit 22 (see custlte.h) */
#define LTE_MEAS_UTRAN_REP_IN_EUTRA_CONN \
        LTE_MEAS_UTRAN_REP_IN_EUTRA_CONN_DEFINED

/* bit 23 (see custlte.h) */
#define LTE_MEAS_GERAN_REP_IN_EUTRA_CONN \
        LTE_MEAS_GERAN_REP_IN_EUTRA_CONN_DEFINED
/* bit 24 */
#define LTE_MEAS_1X_REP_IN_EUTRA_CONN \
        LTE_MEAS_1X_REP_IN_EUTRA_CONN_DEFINED
/* bit 25 */
#define LTE_MEAS_INTER_FREQ_REP_IN_EUTRA_CONN                TRUE
/* bit 26 (see custlte.h)  */
#define LTE_MEAS_HRPD_REP_IN_EUTRA_CONN \
        LTE_MEAS_HRPD_REP_IN_EUTRA_CONN_DEFINED
/* bit 27 */
#define LTE_EUTRA_CONN_TO_UTRA_CELL_DCH_CS_HO                TRUE

/* bit 31 */
#define LTE_MULTI_BAND_INFO_LIST                             TRUE

/* Rel9 feature group indicators */
/* bit 33 */
#define LTE_MEAS_UTRAN_ANR_REP                               TRUE
/* bit 34 */
#define LTE_MEAS_GERAN_ANR_REP                               TRUE
/* bit 35 */
#define LTE_MEAS_1X_ANR_REP                                  TRUE
/* bit 36 */ 
#define LTE_MEAS_HRPD_ANR_REP                                TRUE

/* Rel10 feature group indicators */
/* bit 102 aperiodic SRS*/
#define LTE_APERIODIC_SRS                                    TRUE

/* bit 103 */
#define LTE_PDSCH_TM9_MAX_4_ANT                                  FALSE

/* bit 104 */
#define LTE_PDSCH_TM9_MAX_8_ANT                                  FALSE

/* bit 105 */
#define LTE_PERIODIC_CQI_PMI_TM9_4_ANT                         FALSE

/* bit 106 */
#define LTE_PERIODIC_CQI_PMI_TM9_8_ANT                         FALSE

/* bit 107 */
#define LTE_APERIODIC_CQI_PMI_TM9_4_ANT                        FALSE

/* bit 108 */
#define LTE_APERIODIC_CQI_PMI_TM9_8_ANT                        FALSE

/* bit 109 */
#define LTE_PERIODIC_CQI_PMI_TM9_8_ANT_MODE_1_1                    FALSE

/* bit 110 */
#define LTE_PERIODIC_CQI_PMI_TM9_8_ANT_MODE_1_2                    FALSE

/* Rel10 feature group indicators */
/* bit 111 */
#define LTE_MEAS_EVENT_A6                                    TRUE

/* bit 112 */
#define LTE_X2L_PSHO_WITH_CA_FEATURE_ENABLE                  TRUE

/* bit 114 */
#define LTE_MEAS_REPORT_CPICH_RSCP_AND_ECNO                  TRUE

/* bit 115 */
#define LTE_EICIC_MEAS_SUBFRAME_RESTRICTION                  FALSE

/* bit 30 (see custlte.h)*/
#define LTE_HANDOVER_BETWEEN_FDD_AND_TDD \
          LTE_HANDOVER_BETWEEN_FDD_AND_TDD_DEFINED
/* LTE capabilities end */

/*UE supports enhanced dual layer (PDSCH transmission mode 8) for TDD */
#define LTE_PDSCH_TM_8_L_TDD                                 TRUE

/*UE supports enhanced dual layer (PDSCH transmission mode 8) for FDD */
#define LTE_PDSCH_TM_8_L_FDD                                 FALSE

#define LTE_PDCP_SN_EXTENSION_R11      FALSE

#define LTE_SUPPORT_ROHCCONTEXTCONTINUE_R11 TRUE

#define LTE_CRS_INTERFHANDL_R11 \
           LTE_FEICIC_CRS_SUPPORTED

#define LTE_SS_CCH_INTERFHANDL_R11 \
           LTE_FEICIC_DCCH_SIB1_SUPPORTED

#define LTE_TDD_SPECIAL_SUBFRAME_R11 \
            LTE_TDD_SPECIAL_SUBFRAME_SUPPORTED
  
#define LTE_DL_COMP_1CSIPROC_R11 FALSE

#define LTE_DL_COMP_MULTICSIPROC_R11 FALSE

#define LTE_EPDCCH_R11                 FALSE
#define LTE_MULTIACK_CSIREPORTING_R11  FALSE
#define LTE_TXDIV_PUCCH1B_CHSELECT_R11 FALSE
#define LTE_UL_COMP_R11                FALSE
#define LTE_MULTIPLETIMINGADVANCE_R11  FALSE
#define LTE_SIMULTANEOUSRX_TX_R11      FALSE
#define LTE_SUPPORTEDCSI_PROC_R11      FALSE
#define LTE_RSRQMEASWIDEBAND_R11       FALSE
#define LTE_UE_RX_TXTIMEDIFFMEASUREMENTSFORMDT_R11 FALSE
#define LTE_CDMA2000_NWSHARING_R11     FALSE
#define LTE_INDEVICECOEXIND_R11        FALSE
#define LTE_POWERPREFIND_R11           FALSE
#define LTE_STANDALONEGNSS_LOCATION_R10 TRUE
#define LTE_SIMULTANEOUSACKNACKANDCQI_FORMAT3_R11 FALSE
#define LTE_IDC_CONFIG_R11              FALSE
#define LTE_DRX_CONFIG_V1130            FALSE
#define LTE_OBTAINLOCATIONCONFIG_R11    FALSE
#define LTE_LOCATIONINFO_R10            FALSE

/* From 36.321 Section 7.1

  ----------------------------------------
  | RNTI TYPE   |         Value           |
  ----------------------------------------
  |             |   FDD     |    TDD      |
  ----------------------------------------
  |  RA-RNTI    |  0x0-0x9  |  0x0-0x3B   |
  ----------------------------------------
  |  C-RNTI,    |  0x000A-  |  0x003C-    |
  |  SPS-RNTI,  |  0xFFF2   |  0xFFF2     |
  |  Temp C-RNTI|           |             |
  ----------------------------------------
  |  Reserved   |    0xFFF3-0xFFFC        |
  ----------------------------------------
  |  P-RNTI     |       0xFFFE            |
  ----------------------------------------
  |  SI-RNTI    |       0xFFFF            |
  -----------------------------------------
*/
/*! @brief
  The type enum for RNTIs.
  Defined in TBD
*/
typedef uint16 lte_rnti_val_t;


/*! @brief
  This is an enumeration of all RNTI types. Note this matches to the
  firmware rnti_enable_mask by corresponding bit shift.
  Defined in 36.321 Section 7.1
*/
typedef enum
{
  /*! RNTI to decode dedicated UE data in connected mode */
  LTE_C_RNTI_TYPE = 0,
  /*! RNTI to activate Semi-Persistent scheduling */
  LTE_SPS_RNTI_TYPE,
  /*! RNTI used to identify paging information */
  LTE_P_RNTI_TYPE,
  /*! RNTI associated with PDCCH carrying RACH response */
  LTE_RA_RNTI_TYPE,
  /*! Temporary RNTI to decode MSG 4 of the RACH procedure */
  LTE_TEMP_RNTI_TYPE,
  /*! RNTI to identify System Information */
  LTE_SI_RNTI_TYPE,
  /*! RNTI used to notify the UE of UL power control information */
  LTE_TPC_RNTI_TYPE,
  /*! This is just to match the LL1 enum 
   *  this is the equivalent of LTE_LL1_TPC_PUSCH_RNTI
   */
  LTE_TPC_PUCCH_RNTI,
  /*! For cell specific MBMS on PDSCH in either connected state or idle state*/
  LTE_MBMS_RNTI_TYPE,
  /*! Max number of RNTI types */
  LTE_MAX_NUM_RNTI_TYPES
} lte_rnti_type_e;


/*! @brief
  This type capture the RNTI information
*/
typedef struct
{
  /*! The RNTI value */
  lte_rnti_val_t   rnti_val;
  /*! The RNTI type */
  lte_rnti_type_e  rnti_type;
} lte_rnti_info_s;

/*! @brief
  The system/sub-frame number type
*/
typedef struct
{
  uint16 sub_fn:4;
  uint16 sys_fn:10;
  uint16 xxx:2;
} lte_sfn_s;

/*! @brief MAC RACH contention result indication.
  MAC UL sends to L1,RRC and MAC DL
*/
typedef struct
{
  msgr_hdr_struct_type   hdr;     /*!< Message header */
  /* Message payload */
  boolean                status;  /*!< contention status */
  lte_sfn_s              sfn;     /*!< system time when MSG4 is decoded */
} lte_mac_contention_result_ind_msg_s;

/*! @brief MAC send this indication to ML1 to notify ML1 that MAC
  does not have enough grant to include PHR.
*/
typedef struct
{
  msgr_hdr_struct_type   hdr;     /*!< Message header */
  /* Message payload */
} lte_mac_send_phr_failed_ind_msg_s;

/*! @brief Data structure for Handover Completed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_phy_cell_id_t phy_cell_id;  /*!< Physical cell id */
  lte_earfcn_t dl_freq;  /*!< DL Frequency */
  lte_earfcn_t ul_freq;  /*!< UL Frequency */
  lte_bandwidth_e dl_bandwidth; /*!< DL Bandwidth */
  lte_bandwidth_e ul_bandwidth; /*!< UL Bandwidth */
} lte_rrc_handover_completed_ind_s;

/*! @brief Data structure for Config Completed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_config_completed_ind_s;



/*===========================================================================

  FUNCTION:  LTE_ML1_GET_CURR_SFN

===========================================================================*/
/*!
    @brief
    The function returns the current sfn in the format of lte_sfn_s

    @details
    Get the sfn from the ML1 scheduler interface.

    @return
    The current sfn and subframe number of the current serving cell.
*/
/*=========================================================================*/
extern lte_sfn_s lte_ml1_get_curr_sfn( void );
#endif /* LTE_AS_H */

