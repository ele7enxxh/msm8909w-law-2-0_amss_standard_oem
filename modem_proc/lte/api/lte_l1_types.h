/*!
  @file
  lte_l1_types.h

  @brief
  This header file defines the basic LTE L1 types. It is shared by entire
  Access Stratum and LL. It should be included in the lte_ml_fw.h(TBD) and
  lte_cphy_sap.h. It should be also included by LTE L2 and L3 software
  when the CPHY interfaces is in use.

  @detail
    This interface is generated from
    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_l1_types.h#1 $
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_l1_types.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/12/14   sb       CR656696: KW fixes -
                    Change value of LTE_L1_MAX_MTCH_PER_PMCH to 29 from 32
05/02/13   kp      Updated PUCCH format1bcs interface.
08/17/12   kp      Updated CA interface with latest spec changes
02/28/12   mm      Added a flag to indicate if p0_persistent is signaled
12/08/11   tmai    W2L time transfer interface change
09/06/11   anm     Added eMBMS related interface
11/23/10   sah     Add in more recovery cause codes.
09/01/10   mm      Added support for passing transmission mode 8
08/17/10   mm      Defined reason fields for OOS and RLF indications
06/22/10   mm      Added definitions for Release 9 mandatory features
05/10/10   nl      changed maximum power of UE power class to 23dBm  
05/04/10   anm     CDRX initial support
03/16/10   fhu     Replaced lte_l1_tdd_subframe_assign_e by 
                     lte_l1_tdd_ul_dl_cfg_index_e
03/15/10   fhu     Added a new type of lte_l1_tdd_subframe_assign_e which is 
                   supposed to replace lte_l1_tdd_subframe_assign_e later. 
08/21/09   mm      Added missing fields in SPS UL for March ASN.1 migration 
08/06/09   mm      Added and featurized the changes for March ASN.1 migration
08/03/09   mm      Moved lte_phy_cell_id_t definition to lte_as.h
07/02/09   ub      Corrected value of invalid time constant
05/28/09   mm      Added enum types for T310, n310 and n311
04/29/09   anm     Added fw hypothesis enum values for unknown pa and pb
04/08/09   ub      Added invalid and min rsrp values
04/01/09   gsc     Updates to some enums to add invalid value
03/13/09   ub      Updates after initial idle bringup on qtf
03/13/09   jyw     lte_l1_dl_tx_mode_e starts from 1 to match the LL config.
02/27/09   dk      Added in the types for PUCCH power control
02-26-09   anm     Added LTE_CPHY_NULL_SFN macro for sib sched req
02/26/09   sah     Remove unused codebook union for PLT autogen.
02/05/09   gsc     Fixed a typo in lte_l1_max_harq_tx_e
01/23/09   ub      Added time type and invalid value constants
07-05-2008 jyw     Created the file.
===========================================================================*/

#ifndef LTE_L1_TYPES_H
#define LTE_L1_TYPES_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

//#include "comdef.h"
#include <lte_as.h>

/*===========================================================================

                          GENERAL DEFINITIONS

===========================================================================*/
/*! @brief
  Used as a placeholder when the type is not known
*/
typedef uint32 lte_l1_TBD_t;

/*! @brief Time type for absolute system time since power up */
typedef uint64 lte_ml1_time_t;

/*! @brief Invalid value of time */
#define LTE_ML1_COMMON_INVALID_VALUE_TIME    ((uint64)(-1))

#define LTE_CPHY_NULL_SFN                    0xFFFF

/*! Maximum UE Tx power */
#define LTE_ML1_UE_MAX_OUTPUT_POWER          23

/*! Invalid value of RSRP */
#define LTE_ML1_INVALID_VALUE_RSRP           -255

/*! Invalid value of RSRP */
#define LTE_ML1_INVALID_VALUE_DL_PL_RSRP     0xFFFFFFFF

/*! Invalid value of RSRQ */
#define LTE_ML1_INVALID_VALUE_RSRQ           -255

/*! Minimum value of RSRP */
#define LTE_ML1_MIN_VALUE_RSRP               -127

/*! @brief Value of report_amt to be used if number of reports to send is infinity */
#define LTE_CPHY_MEAS_REPORT_AMT_INFINITY    0xFF

/*! Maximum number of SCells sent in RRC config req message */
#define LTE_CPHY_MAX_S_CELL   4

/*! Maximum number of Serving cells one Pcell + upto 4Scells*/
#define LTE_CPHY_MAX_SERVING_CELL   5

/*! Maxumum value for n1_pucch_an_list_size */
#define MAX_N1_PUCCH_AN_LIST_SIZE 4

/*! @brief Maximum PMCH per MBSFN area */
#define LTE_L1_MAX_PMCH_PER_MBSFN_AREA          15

/*! @brief Maximum MTCH Logical channels per PMCH */
#define LTE_L1_MAX_MTCH_PER_PMCH                29

/*! @brief Maximum possible subframes with PMCH in a frame */
#define LTE_L1_MAX_PMCH_SUBF_PER_FRAME          6 //TDD = 5, FDD = 6

/*! @brief MTCH not scheduled field value in MSI */
#define LTE_L1_MSI_MTCH_NOT_SCHEDULE_VAL        2047

/*! @breif Invalid value for frame offset */
#define LTE_ML1_FRAME_OFFSET_INVALID            0xFFFFFFFF 

/*<! @brief  Maximum number of bands to update 
            got from EFS ("update_band_range")*/
#define LTE_L1_MAX_BANDS_TO_UPDATE    6

/*! @brief MeasSubframe Pattern max size */
#define LTE_L1_MAX_MEAS_SUBFM_PATTERN_SIZE      3

/*! LTE RTC ticks per millisecond */
#define LTE_L1_RTC_TICKS_PER_MS                 30720

/* Max num bands in bs_bw_supported */
#define LTE_L1_MAX_BS_BW_SUPPORTED 10

/*! ID for PCC */
#define LTE_CPHY_PCC_ID            0

/*! STAG-ID for PCC */
#define LTE_CPHY_PCC_STAG_ID       0

/*! Maximum STAG ID */
#define LTE_CPHY_MAX_STAG_ID       3

/*! Maximum number of CCs supported with configured uplink */
#define LTE_CPHY_MAX_UL_CC         2
/*! @brief
    Enumeration defines action for MBMSFN areas physical channel
*/
typedef enum {
  /*! No actions */
  LTE_CPHY_MBSFN_AREA_NONE = 0,
  /*! Monitor on all potential occasions unless specific scheduling info available */
  LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS = 1,
  /*! Monitor only at modifucation boundaries occasion */
  LTE_CPHY_MBSFN_AREA_MONITOR_ONLY_MODIFICATION_BOUNDARY = 2,
  /*! Don't monitor the PMCH */
  LTE_CPHY_MBSFN_AREA_NO_MONITOR = 3
} lte_cphy_mbsfn_area_monitor_pattern_e;

/*! @brief
    Enumeration defines action of configuration
    LTE_L1_CONFIG_NO_ACTION = No action = No params passed
    LTE_L1_DECONFIG = Erase/Remove/deconfig from L1
    LTE_L1_CONFIG = Update/Reconfig/NewConfig
*/
typedef enum {
  LTE_L1_CONFIG_NO_ACTION = 0,
  LTE_L1_DECONFIG = 1,
  LTE_L1_CONFIG = 2
} lte_l1_config_action_e;

/*===========================================================================

          RF CHANNEL, BANDWIDTH, ANTENNA, BAND SCAN RELATED TYPES

===========================================================================*/
/*! @brief
  Enumeraion of the down link transmission mode
  Defined in lte_LL1_dl_pdsch_params_struct
  from Espresso UE LL-ML API Specification
  Defined in 36.213 Table 7.1-1
*/
typedef enum
{
  /*----------------------------------------------------------------------------
                            Values signalled over the air
   ---------------------------------------------------------------------------*/
  LTE_L1_DL_TM1 = 1,
  LTE_L1_DL_TM2 = 2,
  LTE_L1_DL_TM3 = 3,
  LTE_L1_DL_TM4 = 4,
  LTE_L1_DL_TM5 = 5,
  LTE_L1_DL_TM6 = 6,
  LTE_L1_DL_TM7 = 7,
  LTE_L1_DL_TM8_REL9 = 8,
  LTE_L1_DL_TM9_REL10 = 9,
  LTE_L1_DL_TM10_REL11 = 10,
  LTE_L1_DL_TRANSMISSIONMODE_TOTAL,

  /*----------------------------------------------------------------------------
                            Translated values for L1
   ---------------------------------------------------------------------------*/
  /*! Invalid mode */
  LTE_L1_DL_TX_MODE_INVALID = 0,

  /*! Single antenna port */
  LTE_L1_DL_TX_MODE_SINGLE_ANT_PORT0 = LTE_L1_DL_TM1,

  /*! Transmit diversity */
  LTE_L1_DL_TX_MODE_TD_RANK1 = LTE_L1_DL_TM2,

  /*! Open-loop spatial multiplexing */
  LTE_L1_DL_TX_MODE_OL_SM = LTE_L1_DL_TM3,

  /*! Close-loop spatial multiplexing */
  LTE_L1_DL_TX_MODE_CL_SM = LTE_L1_DL_TM4,

  /*! Mulit-user MIMO */
  LTE_L1_DL_TX_MODE_MU_MIMO = LTE_L1_DL_TM5,

  /*! Close-loop rank1 precoding */
  LTE_L1_DL_TX_MODE_CL_RANK1_PC = LTE_L1_DL_TM6,

  /*! Single-antenna port(5), single layer beam forming */
  LTE_L1_DL_TX_MODE_SINGLE_ANT_PORT5 = LTE_L1_DL_TM7,

  /*! Two-antenna port(7) (8), beam forming */
  LTE_L1_DL_TX_MODE_TWO_ANT_PORT7_PORT8 = LTE_L1_DL_TM8_REL9,   

  /*! Antenna ports 7 to 14, single user beam forming with up to 8x8 MIMO */
  LTE_L1_DL_TX_MODE_UP_TO_EIGHT_LAYER_BEAMFORMING = LTE_L1_DL_TM9_REL10,

  /*! COMP */
  LTE_L1_DL_TX_MODE_COMP = LTE_L1_DL_TM10_REL11,

  LTE_L1_DL_TX_MAX_NUM_MODES = LTE_L1_DL_TRANSMISSIONMODE_TOTAL
} lte_l1_dl_tx_mode_e;

/*! @brief
    Enumeration indicates whether UE transmit antenna selection control is 
    closed-loop or open-loop. (ref: 36.331, 6.3.2) 
    
*/
typedef enum {
  LTE_L1_TX_ANTENNA_SELECTION_CLOSED_LOOP,
  LTE_L1_TX_ANTENNA_SELECTION_OPEN_LOOP
} lte_l1_tx_antenna_selection_e;


/*! @brief
    Represents the number of cell specific antenna ports where an1 corresponds 
    to 1, an2 to 2 antenna ports etc. (ref: 36.331, 6.3.2) 
    see 36.211, 6.2.1. A UE in IDLE mode acquires the information about the 
    number of transmit antenna ports according to TS 36.212, 5.3.1.1.
*/
typedef enum {
  LTE_L1_ANTENNA_INFO_PORTS_COUNT_1 = 1,
  LTE_L1_ANTENNA_INFO_PORTS_COUNT_2 = 2,
  LTE_L1_ANTENNA_INFO_PORTS_COUNT_4 = 4,
  LTE_L1_ANTENNA_INFO_PORTS_COUNT_8 = 8
} lte_l1_antenna_info_ports_count_e;


/*===========================================================================

                    SYSTEM INFORMATION TYPES

===========================================================================*/
/*! @brief
  SFN type used in the cell timming; only the lower 10 bits are valid
  from 0 ... 1023.
  Defined in 36.331 MasterInformationBlock
*/
typedef uint16 lte_l1_sfn_t;


/*! @brief
  Subframe number type used in the cell timming from 0 ... 9.
  Defined in 36.211 Section 4.1 and 4.2
*/
typedef uint8  lte_l1_subfn_t;


/*! @brief
  Frame reference time type derived from RTC cnt used in the searched
  cell timming
*/
typedef uint64 lte_l1_frame_ref_time_t;


/*! @brief
  Cell system time
  Used in LTE_LL_SYS_SF_IND defined in Espresso UE ML-LL interface
*/
typedef struct
{
  /*! System frame number */
  lte_l1_sfn_t    sfn;

  /*! Subframe number from 0 .. 9. */
  lte_l1_subfn_t  subfn;
} lte_l1_cell_systime_s;

/*! @brief
  Band Frequency range Structure got from 
  EFS ("update_band_range")*/
typedef struct 
{
  /* Band index */
  uint16       band_id;
  /* Min earfcn supported */
  uint16       start_freq;
  /* Max earfcn supported */
  uint16       end_freq;
} lte_l1_band_freq_range_s;

/*! @brief
  Band Frequency range Structure got from 
  EFS ("update_band_range")*/
typedef struct 
{
  /* Band index */
  lte_efs_extnd_band_enum_t band_id;
  /* Min earfcn supported */
  lte_earfcn_t start_freq;
  /* Max earfcn supported */
  lte_earfcn_t end_freq;
} lte_l1_band_extnd_freq_range_s;


/*! @brief
  Band Updated info structure got from  
  EFS ("update_band_range")*/
typedef struct 
{
  /* Number of bands */
  uint16                          num_bands;
  /* Band freq info structure */
  lte_l1_band_freq_range_s  lte_band_range[LTE_L1_MAX_BANDS_TO_UPDATE];
} lte_l1_bands_updated_info_s;

/*! @brief
  Band Updated info structure got from  
  EFS ("update_band_range")*/
typedef struct 
{
  /* Number of bands */
  uint16                          num_bands;
  /* Band freq info structure */
  lte_l1_band_extnd_freq_range_s  lte_band_range[LTE_L1_MAX_BANDS_TO_UPDATE];
} lte_l1_extnd_bands_updated_info_s;

/*! @brief 
   Struct for storing bandwidths supported for band scan */
typedef struct
{
  uint8   num_bands;

  /* Band */
  uint8   band[LTE_L1_MAX_BS_BW_SUPPORTED];

  /* earfcn within the band user wants UE to camp on */
  uint8   bw_supported[LTE_L1_MAX_BS_BW_SUPPORTED];
} lte_l1_bs_bw_supported_s;

/*! @brief
  Minimum system frame number.
*/
#define LTE_L1_MIN_FRAME_NUM 0
/*! @brief
  Maximum system frame number.
*/
#define LTE_L1_MAX_FRAME_NUM 1023

/*! @brief Invalid value of sub-FN */
#define LTE_ML1_COMMON_INVALID_VALUE_SUB_FN  0xFF

/*===========================================================================

                    CELL RELATED TYPES

===========================================================================*/
/*! @brief
  Miximum number of the LTE physical cell id .
  Defined in 36.331 Section 6.3.3
*/
#define LTE_L1_MAX_PHY_CELL_ID 503

/*! @brief
  Cell Id value to be used when that the cell id is not present
  */
#define LTE_L1_NO_CELL_ID      0xFFFF


/*! @brief
  Physical cell ID and range
*/
typedef struct
{
  /*! Lowest PCI in the range */
  lte_phy_cell_id_t start_pci;

  /*! Highest PCI in the range (for single PCI, equals start_pci) */
  lte_phy_cell_id_t end_pci;

} lte_phy_cell_id_and_range_s;


/*! @brief
  LTE cell duplexing modes
  Defined in 36.201 Section 4.2.1
*/
typedef enum
{
  /*! Frequency division Duplex */
  LTE_L1_CELL_DUPLEX_MODE_FDD = 0,

  /*! Time Division Duplex */
  LTE_L1_CELL_DUPLEX_MODE_TDD,

  /*! Half Duplex */
  LTE_L1_CELL_DUPLEX_MODE_HALF,

  /*! Maximum number of duplex modes */
  LTE_L1_MAX_NUM_CELL_DUPLEX_MODES
} lte_l1_cell_duplex_e;


/*! @brief
  Supported frame structures enum
  Defined in 36.201 Section 4.2.1
*/
typedef enum
{
  /*! Frame structure 1 (generic frame structure) */
  LTE_L1_FRAME_STRUCTURE_FS1 = 0,

  /*! Frame structure 2 (alternative frame structure) */
  LTE_L1_FRAME_STRUCTURE_FS2,

  /*! Maximum number of frame structure for LTE cells */
  LTE_L1_FRAME_MAX_NUM_STRUCTURES
} lte_l1_frame_struct_e;


/*! @brief
  Supported cyclix prefix modes
  Defined in 36.211 Table 6.12-1 for normal and extended mode.
  TBD: fill in the spec reference for the extended_msbfn mode.
*/
typedef enum
{
  /*! Normal CP */
  LTE_L1_CP_MODE_NORMAL = 0,

  /*! Extended CP */
  LTE_L1_CP_MODE_EXTENDED,

  /*! Long CP for MBSFN
     @todo Do we need list the CP_MODE_EXTENDED MBSFN in this enum? */
  LTE_L1_CP_MODE_EXTENDED_MBSFN,

  LTE_L1_MAX_NUM_CP_MODES
} lte_l1_cp_e;


/*! @brief
  Cell DL information used in the CPHY interface
  Defined in TBD
*/
typedef struct
{
  /*! Downlink center frequency */
  lte_earfcn_t     earfcn;

  /*! Downlink bandwidth  */
  lte_bandwidth_e  dl_bandwidth;
} lte_l1_dl_cell_info_s;


/*! @brief
  Cell ul information used in the CPHY interface
  Defined in TBD
*/
typedef struct
{
  /*! Uplink center frequency */
  lte_earfcn_t        earfcn;

  /*! Uplink bandwidth */
  lte_bandwidth_e     ul_bandwidth;

  /*! TBD */
  lte_l1_TBD_t        additional_spectrum_emission;
} lte_l1_ul_cell_info_s;

/*! @brief
  Cell information used in the CPHY interface
  Defined in TBD
*/
typedef struct
{
  /*! Cell physical identity */
  lte_phy_cell_id_t       phy_cell_id;

  /*! DL cell info */
  lte_l1_dl_cell_info_s   dl_cell_info;

  /*! UL cell info */
  lte_l1_ul_cell_info_s   ul_cell_info;
} lte_l1_cell_info_param_s;


/*! @brief
  PHICH duration in terms of OFDM symbols.
  Defined in PHICH
*/
typedef enum
{
  /*! Normal PHICH duration */
  LTE_L1_PHICH_DURATION_NORMAL = 0,
  /*! Extended PHICH duration */
  LTE_L1_PHICH_DURATION_EXTENDED,
  /*! Max number of PHICH duration modes */
  LTE_L1_PHICH_MAX_NUM_DURATION_MODES,
  /*! Minimum supported PHICH duration */
  LTE_L1_PHICH_DURATION_MIN = LTE_L1_PHICH_DURATION_NORMAL,
  /*! Maximum supported PHICH duration */
  LTE_L1_PHICH_DURATION_MAX = LTE_L1_PHICH_DURATION_EXTENDED
} lte_l1_phich_duration_e;

/*! @brief 
    Indicates DL/UL subframe configuration where sa0 point to Configuration 0,
    sa1 to Configuration 1 etc. as specified in the 36.211, table 4.2.2.
    (ref: 36.331, 6.3.2)
*/
typedef enum {
  LTE_L1_TDD_SUBFRAME_ASSIGN_0 = 0,
  LTE_L1_TDD_SUBFRAME_ASSIGN_1,
  LTE_L1_TDD_SUBFRAME_ASSIGN_2,
  LTE_L1_TDD_SUBFRAME_ASSIGN_3,
  LTE_L1_TDD_SUBFRAME_ASSIGN_4,
  LTE_L1_TDD_SUBFRAME_ASSIGN_5,
  LTE_L1_TDD_SUBFRAME_ASSIGN_6,
  LTE_L1_TDD_SUBFRAME_ASSIGN_MAX  /* Invalid value */
} lte_l1_tdd_subframe_assign_e;


/*! @brief 
    Indicates TDD DL/UL configurations where LTE_L1_TDD_UL_DL_CFG_0 points to Configuration 0,
    LTE_L1_TDD_UL_DL_CFG_1 points to Configuration 1,  etc. as specified in the 36.211, table 4.2.2.
    (ref: 36.331, 6.3.2)
 
*/
typedef enum
{
  LTE_L1_TDD_UL_DL_CFG_0 = 0,
  LTE_L1_TDD_UL_DL_CFG_1 = 1,
  LTE_L1_TDD_UL_DL_CFG_2 = 2,
  LTE_L1_TDD_UL_DL_CFG_3 = 3,
  LTE_L1_TDD_UL_DL_CFG_4 = 4,
  LTE_L1_TDD_UL_DL_CFG_5 = 5,
  LTE_L1_TDD_UL_DL_CFG_6 = 6,
  LTE_L1_TDD_UL_DL_CFG_UNKNOWN_WITH_UNKNOWN_10MS_FRAME = 7,
  LTE_L1_TDD_UL_DL_CFG_UNKONWN_WITH_KNOWN_10MS_FRAME   = 8 ,
  LTE_L1_TDD_UL_DL_CFG_NUM
} lte_l1_tdd_ul_dl_cfg_index_e;

typedef enum
{
	LTE_L1_FDD_DL_REFERENCE_CFG = 0,
  LTE_L1_TDD_DL_REFERENCE_CFG_0,
  LTE_L1_TDD_DL_REFERENCE_CFG_1,
  LTE_L1_TDD_DL_REFERENCE_CFG_2,
  LTE_L1_TDD_DL_REFERENCE_CFG_3,
  LTE_L1_TDD_DL_REFERENCE_CFG_4,
  LTE_L1_TDD_DL_REFERENCE_CFG_5,  
  LTE_L1_TDD_DL_REFERENCE_CFG_6,    
  LTE_L1_MAX_DL_REFERENCE_CFG
} lte_l1_dl_reference_cfg_index_e;

typedef enum
{
	LTE_L1_FDD_UL_REFERENCE_CFG = 0,
  LTE_L1_TDD_UL_REFERENCE_CFG_0,
  LTE_L1_TDD_UL_REFERENCE_CFG_1,
  LTE_L1_TDD_UL_REFERENCE_CFG_2,
  LTE_L1_TDD_UL_REFERENCE_CFG_3,
  LTE_L1_TDD_UL_REFERENCE_CFG_4,
  LTE_L1_TDD_UL_REFERENCE_CFG_5,  
  LTE_L1_TDD_UL_REFERENCE_CFG_6,    
  LTE_L1_MAX_UL_REFERENCE_CFG
} lte_l1_ul_reference_cfg_index_e;

/*! @brief
    Indicates DL/UL subframe configuration where ssp0 point to Configuration 0,
    ssp1 to Configuration 1 etc. as specified in the 36.211, table 4.2.1.
    (ref: 36.331, 6.3.2)
*/
typedef enum {
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_0 = 0,
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_1,
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_2,
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_3,
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_4,
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_5,
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_6,
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_7,
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_8,
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_9, /* Rel-11 SSF9 */
  LTE_L1_TDD_SPECIAL_SUBFRAME_PATTERN_MAX /* Invalid value */
} lte_l1_tdd_special_subframe_pattern_e;

/*! @brief
  PHICH resource settings
  Defined in TBD
*/
typedef enum
{
  LTE_L1_PHICH_RESOURCE_ONE_SIXTH = 0,
  LTE_L1_PHICH_RESOURCE_ONE_HALF,
  LTE_L1_PHICH_RESOURCE_ONE,
  LTE_L1_PHICH_RESOURCE_TWO,
  LTE_L1_PHICH_MAX_NUM_RESOURCE_MODES,
  /*! Minimum supported PHICH resource */
  LTE_L1_PHICH_RESOURCE_MIN = LTE_L1_PHICH_RESOURCE_ONE_SIXTH,
  /*! Maximum supported PHICH resource */
  LTE_L1_PHICH_RESOURCE_MAX = LTE_L1_PHICH_RESOURCE_TWO
} lte_l1_phich_resource_e;


/*! @brief
  PHICH  configuration info
  Defined in TBD
*/
typedef struct
{
  /*! The phich duration configures lower limit on the size of the control
      region signaled by the PCFICH. */
  lte_l1_phich_duration_e  phich_duration;

  /*! Parameter for determining the number of PHICH groups */
  lte_l1_phich_resource_e  phich_resource;
} lte_l1_phich_cfg_param_s;


/* @brief
  Parameter struct for L1M Acquisition Request
  Defined in 36.331 MIB format
*/
typedef struct
{
  /*! Number of Tx antennas detected */
  uint8                      num_tx_antennas;

  /*! Downlink bandwidth */
  lte_bandwidth_e            dl_bandwidth;

  /*! PHICH configuration */
  lte_l1_phich_cfg_param_s  phich_info;
} lte_l1_mib_info_param_s;

/*! @brief Invalid value of frequency is of type uint32*/
#define LTE_ML1_COMMON_INVALID_VALUE_FREQ    0xFFFFFFFF

/*! @brief Invalid value of cell id */
#define LTE_ML1_COMMON_INVALID_VALUE_PCI     0xFFFF

/*===========================================================================

             DL/UL PHYSICAL CHANNEL CONFIGURATION TYPES

===========================================================================*/
/*
  DL channel configuration mask used in the LL channel config interface
  Used in Espresso UE ML-LL interface lte_LL1_dl_ch_config_req_struct
*/
/*! @brief DL PCFICH config mask */
#define LTE_L1_DL_CH_PCFICH_CONFIG_MASK 0

/*! @brief DL PHICH config mask */
#define LTE_L1_DL_CH_PHICH_CONFIG_MASK  2

/*! @brief DL PDCCH config mask */
#define LTE_L1_DL_CH_PDCCH_CONFIG_MASK  4

/*! @brief DL PDSCH config mask */
#define LTE_L1_DL_CH_PDSCH_CONFIG_MASK  8


/*
  UL channel configuration mask used in the LL channel config interface
  Used in Espresso UE ML-LL interface lte_LL1_ul_ch_config_req_struct
*/
/*! @brief UL PRACH config mask */
#define LTE_L1_UL_CH_PRACH_CONFIG_MASK  0

/*! @brief UL PUSCH config mask */
#define LTE_L1_DL_CH_PUSCH_CONFIG_MASK  2

/*! @brief UL PUCCH config mask */
#define LTE_L1_DL_CH_PUCCH_CONFIG_MASK  4


/*! @brief
  Physical channel enumeration.
  Defined in 36.201 Section 4.2.2
*/
typedef enum
{
  LTE_L1_DL_CH_PCFICH = 0,
  LTE_L1_DL_CH_PHICH,
  LTE_L1_DL_CH_PDCCH,
  LTE_L1_DL_CH_PDSCH,
  LTE_L1_UL_CH_PRACH,
  LTE_L1_UL_CH_PUSCH,
  LTE_L1_UL_CH_PUCCH,
  LTE_L1_UL_CH_PBCH,
  LTE_L1_MAX_NUM_CHS,
  LTE_L1_UL_CH_INVALID = LTE_L1_MAX_NUM_CHS
} lte_l1_phy_chan_e;


/*! @brief
  Cyclic Delay Diversity mode
  Defined in 36.211 Section 6.3.4.2.1
*/
typedef enum
{
  LTE_L1_CDD_NO_DELAY = 0,
  LTE_L1_CDD_SMALL_DELAY,
  LTE_L1_CDD_LARGE_DELAY
} lte_l1_cdd_delay_e;


/*! @brief
  UL slot hopping mode
  Defined in 36.211 Section 5.3.4
*/
typedef enum
{
  /*! Subframe hoppping mode */
  LTE_L1_INTER_SUB_FRAME_HOPPING = 0,

  /*! Slot hopping mode */
  LTE_L1_INTER_INTRA_SUB_FRAME_HOPPING,

  /*! Maximum num of hopping mode*/
  LTE_L1_MAX_NUM_HOPPING_MODES
} lte_l1_slot_hopping_e;


/*! @brief
  Traffic Power control step
  Defined in TBD
*/
typedef enum
{
  /*! TBD  */
  LTE_L1_TPC_SETUP_ENUM_TBD_ENTRY = 0,
} lte_l1_tpc_step_e;



/*! @brief
    DCI format 3 or 3A used for TPC Index.
*/
typedef enum
{
  /* Index of N when DCI format 3 is used.*/
  LTE_L1_TPC_DCI_FORMAT3 = 0,

  /* Index of M when DCI format 3A is used. */
  LTE_L1_TPC_DCI_FORMAT3A = 1
} lte_l1_tpc_dci_format_e;

/*! The index of the UE within the TPC-PDCCH for uplink power control */
typedef struct
{
  /* DCI format */
  lte_l1_tpc_dci_format_e dci_format;

  /* TPC index - Range is [1-15] for DCI format 3 and [1-31] for format 3A*/
  uint8 index_value;
}lte_l1_tpc_index_t;

/******************************************************************************
                            UL Cyclic Prefix Length
*******************************************************************************/
/*! @brief
    UL Cyclic Prefix Length.  len1 corresponds to normal cyclic prefix and
    len2 corresponds to extended cyclic prefix.
*/
typedef enum
{
  LTE_CPHY_UL_CYCLIC_PREFIX_LENGTH_1,
  LTE_CPHY_UL_CYCLIC_PREFIX_LENGTH_2,
  LTE_CPHY_UL_CYCLIC_PREFIX_LENGTH_MAX /* Invalid Value */
}lte_cphy_ul_cyclic_prefix_length_e;
/*! @brief
    Alpha enum (ref: 36.331, 6.3.2)
    where al0 corresponds to 0, al04 corresponds to value 0.4, al05 to 0.5, al06
    to 0.6, al07 to 0.7, al08 to 0.8, al09 to 0.9 and al1 corresponds to 1
*/
typedef enum {
  LTE_L1_UL_POWER_CTRL_ALPHA_0= 0,
  LTE_L1_UL_POWER_CTRL_ALPHA_04,
  LTE_L1_UL_POWER_CTRL_ALPHA_05,
  LTE_L1_UL_POWER_CTRL_ALPHA_06,
  LTE_L1_UL_POWER_CTRL_ALPHA_07,
  LTE_L1_UL_POWER_CTRL_ALPHA_08,
  LTE_L1_UL_POWER_CTRL_ALPHA_09,
  LTE_L1_UL_POWER_CTRL_ALPHA_1
} lte_l1_ul_power_ctrl_alpha_e;

/*! @brief
    PUCCH Format enum
*/
typedef enum {
  LTE_L1_PUCCH_FORMAT_1=0,
  LTE_L1_PUCCH_FORMAT_1A,
  LTE_L1_PUCCH_FORMAT_1B,
  LTE_L1_PUCCH_FORMAT_2,
  LTE_L1_PUCCH_FORMAT_2A,
  LTE_L1_PUCCH_FORMAT_2B,
  LTE_L1_PUCCH_FORMAT_1BCS,
  LTE_L1_PUCCH_FORMAT_3,
  LTE_L1_PUCCH_NUM_FORMATS,
  LTE_L1_PUCCH_FORMAT_INVALID = LTE_L1_PUCCH_NUM_FORMATS
} lte_l1_pucch_format_e;

/*! @brief
    DCI Format enum
*/
typedef enum
{
  LTE_L1_DCI_0 = 0,
  LTE_L1_DCI_1,
  LTE_L1_DCI_1A,
  LTE_L1_DCI_1B,
  LTE_L1_DCI_1C,
  LTE_L1_DCI_1D,
  LTE_L1_DCI_2,
  LTE_L1_DCI_2A,
  LTE_L1_DCI_3,
  LTE_L1_DCI_3A,
  LTE_L1_DCI_2B,
  LTE_L1_DCI_2C,
  LTE_L1_DCI_NUM_FORMATS,
  LTE_L1_DCI_INVALID = LTE_L1_DCI_NUM_FORMATS
} lte_l1_dci_format_e;

typedef enum {
  LTE_L1_DELTA_F_PUCCH_FORMAT1_N2 = -2,
  LTE_L1_DELTA_F_PUCCH_FORMAT1_0 = 0,
  LTE_L1_DELTA_F_PUCCH_FORMAT1_2 = 2
}lte_l1_delta_f_pucch_format1_e;

typedef enum {
  LTE_L1_DELTA_F_PUCCH_FORMAT1B_1 = 1,
  LTE_L1_DELTA_F_PUCCH_FORMAT1B_3 = 3,
  LTE_L1_DELTA_F_PUCCH_FORMAT1B_5 = 5
}lte_l1_delta_f_pucch_format1b_e;

/*! @brief
    deltaF-PUCCH-Format1bCS (ref: 36.331, 6.3.2)
    Parameter:   for the PUCCH formats 1bcs with channel selection.
    See TS 36.213 [23, 5.1.2] where deltaF1 corresponds to 1 dB, deltaF2 corresponds to 2 dB and so on.
*/
typedef enum {
  LTE_L1_DELTA_F_PUCCH_FORMAT1BCS_1 = 1,
  LTE_L1_DELTA_F_PUCCH_FORMAT1BCS_2 = 2
}lte_l1_delta_f_pucch_format1bcs_e;

/*! @brief
    deltaF-PUCCH-Format3 (ref: 36.331, 6.3.2)
    Parameter:   for the PUCCH format 3 with channel selection.
    See TS 36.213 [23, 5.1.2] where deltaF1 corresponds to 1 dB, deltaF2 corresponds to 2 dB and so on.
*/
typedef enum {
  LTE_L1_DELTA_F_PUCCH_FORMAT3_N1 = -1,
  LTE_L1_DELTA_F_PUCCH_FORMAT3_0 = 0,
  LTE_L1_DELTA_F_PUCCH_FORMAT3_1 = 1,
  LTE_L1_DELTA_F_PUCCH_FORMAT3_2 = 2,
  LTE_L1_DELTA_F_PUCCH_FORMAT3_3 = 3,
  LTE_L1_DELTA_F_PUCCH_FORMAT3_4 = 4,
  LTE_L1_DELTA_F_PUCCH_FORMAT3_5 = 5,
  LTE_L1_DELTA_F_PUCCH_FORMAT3_6 = 6
}lte_l1_delta_f_pucch_format3_e;

typedef enum {
  LTE_L1_DELTA_F_PUCCH_FORMAT2_N2 = -2,
  LTE_L1_DELTA_F_PUCCH_FORMAT2_0 = 0,
  LTE_L1_DELTA_F_PUCCH_FORMAT2_1 = 1,
  LTE_L1_DELTA_F_PUCCH_FORMAT2_2 = 2
}lte_l1_delta_f_pucch_format2_e;


typedef enum {
  LTE_L1_DELTA_F_PUCCH_FORMAT2A_N2 = -2,
  LTE_L1_DELTA_F_PUCCH_FORMAT2A_0 = 0,
  LTE_L1_DELTA_F_PUCCH_FORMAT2A_2 = 2
}lte_l1_delta_f_pucch_format2a_e;

typedef enum {
  LTE_L1_DELTA_F_PUCCH_FORMAT2B_N2 = -2,
  LTE_L1_DELTA_F_PUCCH_FORMAT2B_0 = 0,
  LTE_L1_DELTA_F_PUCCH_FORMAT2B_2 = 2
}lte_l1_delta_f_pucch_format2b_e;

/*! @brief
    en0 corresponds to value 0 corresponding to state "disabled". en1
    corresponds to value 1.25 corresponding to "enabled"
    (ref: [36.213, 5.1.1.1.],  [36.331, 6.3.2])
*/
typedef enum {
  LTE_L1_UL_POWER_CTRL_DELTA_MCS_ENABLE_0 = 0,
  LTE_L1_UL_POWER_CTRL_DELTA_MCS_ENABLE_1
} lte_l1_ul_power_ctrl_delta_mcs_enable_e;

/*! @brief
    Filter Coefficient enum
*/
typedef enum {
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_0 = 0,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_1,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_2,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_3,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_4,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_5,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_6,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_7,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_8,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_9,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_11,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_13,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_15,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_17,
  LTE_L1_UL_POWER_CTRL_FILTER_COEFFICIENT_19
} lte_l1_ul_power_ctrl_filter_coefficient_e;

/*! @brief
  P0 PUSCH setup parameters
  Defined in TBD
*/
typedef struct
{
  /*! P0 value for PUSCH for persistent scheduling range TBD. */
  int16 persistent;

  /*! P0 value for PUSCH for non-persistent scheduling range TBD.*/
  int16 non_persistent;
} lte_l1_p0_pusch_s;

typedef struct
{
  /*! Enable/disable field */
  boolean enable;
  /*! DeltaF format 3 Parameter */
  lte_l1_delta_f_pucch_format3_e format3;
  /*! DeltaF format 1bcs Parameter */
  lte_l1_delta_f_pucch_format1bcs_e format1bcs;
}lte_l1_delta_f_list_r10_pucch_s;


typedef struct
{
  lte_l1_delta_f_pucch_format1_e format1;
  lte_l1_delta_f_pucch_format1b_e format1b;
  lte_l1_delta_f_pucch_format2_e format2;
  lte_l1_delta_f_pucch_format2a_e format2a;
  lte_l1_delta_f_pucch_format2b_e format2b;
  /*! UL power ctrl common release 10 parameters */
  lte_l1_delta_f_list_r10_pucch_s rel10_delta_f_param;
}lte_l1_delta_f_list_pucch_s;

/*! @brief
    Group PUSCH assignment (ref: 36.331, 6.3.2)
*/
typedef uint8  lte_cphy_pusch_grp_pusch_assign_t;

/*! @brief
    PUCCH config common delta shift (ref: 36.331, 6.3.2)
    ds1 corresponds to value 1 ds2 to 2 etc.
*/
typedef enum {
  LTE_L1_PUCCH_DELTA_SHIFT_1 = 1,
  LTE_L1_PUCCH_DELTA_SHIFT_2 = 2,
  LTE_L1_PUCCH_DELTA_SHIFT_3 = 3
} lte_l1_pucch_delta_shift_e;

/*! @brief The maximum number of HARQ transmissions */
typedef enum
{
  LTE_L1_n1 = 1,
  LTE_L1_n2 = 2,
  LTE_L1_n3 = 3,
  LTE_L1_n4 = 4,
  LTE_L1_n5 = 5,
  LTE_L1_n6 = 6,
  LTE_L1_n7 = 7,
  LTE_L1_n8 = 8,
  LTE_L1_n10 = 10,
  LTE_L1_n12 = 12,
  LTE_L1_n16 = 16,
  LTE_L1_n20 = 20,
  LTE_L1_n24 = 24,
  LTE_L1_n28 = 28
} lte_l1_max_harq_tx_e;

/*! @brief The semi-persistant scheduling intervals */
typedef enum
{
  LTE_L1_SPS_INTERVAL_SF10 = 10,
  LTE_L1_SPS_INTERVAL_SF20 = 20,
  LTE_L1_SPS_INTERVAL_SF30 = 30,
  LTE_L1_SPS_INTERVAL_SF32 = 32,
  LTE_L1_SPS_INTERVAL_SF40 = 40,
  LTE_L1_SPS_INTERVAL_SF60 = 60,
  LTE_L1_SPS_INTERVAL_SF64 = 64,
  LTE_L1_SPS_INTERVAL_SF80 = 80,
  LTE_L1_SPS_INTERVAL_SF120 = 120,
  LTE_L1_SPS_INTERVAL_SF128 = 128,
  LTE_L1_SPS_INTERVAL_SF160 = 160,
  LTE_L1_SPS_INTERVAL_SF320 = 320,
  LTE_L1_SPS_INTERVAL_SF640 = 640,
  LTE_L1_SPS_INTERVAL_NONE = 0
} lte_l1_sps_interval_e;

typedef enum 
{
  LTE_L1_PUSCH_N_SB_1=1,
  LTE_L1_PUSCH_N_SB_2=2,
  LTE_L1_PUSCH_N_SB_3=3,
  LTE_L1_PUSCH_N_SB_4=4
}lte_l1_pusch_n_sb_e;

/*! @brief
    p0_nominal value for PUCCH Range: -127..-96. unit dBm (ref: 36.331, 6.3.2)
*/
typedef int8 lte_l1_ul_power_ctrl_p0_nom_pucch_t;

/*! @brief
    p0_nominal value for PUCCH Range: -8..7(ref: 36.331, 6.3.2)
*/
typedef int8 lte_l1_ul_power_ctrl_p0_ue_t;


/*! @brief
    pSRS offset: 0..15(ref: [36.331, 6.3.2], [36.213, 5.1.3.1.])
    For Set1, the actual parameter value is pSRS-Offset value - 3. For Set2 the
    actual parameter value is -10.5 + 1.5*pSRS-Offset value
*/
typedef uint8 lte_l1_ul_power_ctrl_psrs_offset_t;

/*! @brief
    pSRS offsetAp: 0..15(ref: [36.331, 6.3.2], [36.213, 5.1.3.1.])
    For Set1, the actual parameter value is pSRS-Offset value - 3. For Set2 the
    actual parameter value is -10.5 + 1.5*pSRS-Offset value
*/
typedef uint8 lte_l1_ul_power_ctrl_psrs_offsetAp_t;


/*! @brief
    SRS sub frame config enum (ref: 36.331, 6.3.2)
    sc0 corresponds to value 0, sc1 to 1 etc.
*/
typedef enum {
  LTE_L1_SRS_SUBFRAME_0 = 0,
  LTE_L1_SRS_SUBFRAME_1,
  LTE_L1_SRS_SUBFRAME_2,
  LTE_L1_SRS_SUBFRAME_3,
  LTE_L1_SRS_SUBFRAME_4,
  LTE_L1_SRS_SUBFRAME_5,
  LTE_L1_SRS_SUBFRAME_6,
  LTE_L1_SRS_SUBFRAME_7,
  LTE_L1_SRS_SUBFRAME_8,
  LTE_L1_SRS_SUBFRAME_9,
  LTE_L1_SRS_SUBFRAME_10,
  LTE_L1_SRS_SUBFRAME_11,
  LTE_L1_SRS_SUBFRAME_12,
  LTE_L1_SRS_SUBFRAME_13,
  LTE_L1_SRS_SUBFRAME_14,
  LTE_L1_SRS_SUBFRAME_15
} lte_l1_srs_subframe_e;

/*! @brief
    Tells whether frequency hopping is enabled or disabled
    (ref: [36.213, 8.2],  [36.331, 6.3.2])
    REMOVE: AFTER LL removes (currently only LL is using it)
*/
typedef enum {
  LTE_L1_SRS_FREQ_HOP_DISABLED = 0,
  LTE_L1_SRS_FREQ_HOP_ENABLED
} lte_l1_srs_freq_hop_enable_e;

typedef enum {

  LTE_L1_SRS_HOPPING_BW_0,
  LTE_L1_SRS_HOPPING_BW_1,
  LTE_L1_SRS_HOPPING_BW_2,
  LTE_L1_SRS_HOPPING_BW_3,
}lte_l1_srs_hopping_bw_e;


/*! @brief
    Provides information (n-SRS) used to calculate cyclic shift for the SRS
    transmission from the UE. where cs0 corresponds to 0 etc.
    CS for SRS = (2*pi*n-SRS)/8
    (ref: [36.211, 5.5.3.1],  [36.331, 6.3.2])
*/
typedef enum {
  LTE_L1_SRS_CYCLIC_SHIFT_0 = 0,
  LTE_L1_SRS_CYCLIC_SHIFT_1,
  LTE_L1_SRS_CYCLIC_SHIFT_2,
  LTE_L1_SRS_CYCLIC_SHIFT_3,
  LTE_L1_SRS_CYCLIC_SHIFT_4,
  LTE_L1_SRS_CYCLIC_SHIFT_5,
  LTE_L1_SRS_CYCLIC_SHIFT_6,
  LTE_L1_SRS_CYCLIC_SHIFT_7
} lte_l1_srs_cyclic_shift_e;

/*! @brief
    Parameter: SRS Bandwidth see TS 36.211 [21, 5.5.3.2: table 5.5.3.2-1].
*/
typedef enum {
  LTE_L1_SRS_BW0 = 0,
  LTE_L1_SRS_BW1,
  LTE_L1_SRS_BW2,
  LTE_L1_SRS_BW3,
  LTE_L1_SRS_BW4,
  LTE_L1_SRS_BW5,
  LTE_L1_SRS_BW6,
  LTE_L1_SRS_BW7
} lte_l1_srs_bw_e;


typedef enum
{
  LTE_L1_DSR_TRANS_MAX_N4 = 4,
  LTE_L1_DSR_TRANS_MAX_N8 = 8,
  LTE_L1_DSR_TRANS_MAX_N16 = 16,
  LTE_L1_DSR_TRANS_MAX_N32 = 32,
  LTE_L1_DSR_TRANS_MAX_N64 = 64
}lte_l1_dsr_trans_max_e ;


/*! @brief
  The CQI format Indicator 
*/
typedef enum
{
  LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND,
  LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND
}lte_l1_cqi_format_indicator_e;

/*! @brief
    Parameter: reporting mode. Value rm12 corresponds to Mode 1-2, rm20
    corresponds to Mode 2-0, rm22 corresponds to Mode 2-2 etc. PUSCH reporting
    modes are described in TS 36.213 [23, 7.2.1].
*/
typedef enum
{
  LTE_L1_CQI_MODE_APERIODIC_RM12 = 0,
  LTE_L1_CQI_MODE_APERIODIC_RM20,
  LTE_L1_CQI_MODE_APERIODIC_RM22,
  LTE_L1_CQI_MODE_APERIODIC_RM30,
  LTE_L1_CQI_MODE_APERIODIC_RM31,
  PUSCH_CSF_MAX_REPORTING_MODES,
} lte_l1_cqi_reporting_mode_aperiodic_e;


/*! @brief
  csi-ConfigIndex
  E-UTRAN configures csi-ConfigIndex only for PCell and only if 
  csi-SubframePatternConfig is configured. The UE shall release csi-ConfigIndex 
  if csi-SubframePatternConfig is released. CSI config Index 
*/

typedef struct 
{
   /*! CSI config index enable flag */
  boolean enable;
   /*! cqi-pmi-ConfigIndex2-r10 	  	 INTEGER   (0 .  . 1023 ) */
  uint8 cqi_pmi_config_index2;
  /*! RI Config index2 enable flag */
  boolean enable_ri_cfg_index2;
  /*! ri-ConfigIndex2-r10 	  	  	  	 INTEGER   (0 .  . 1023 ) */
  uint8 ri_cfg_index2;  

}lte_l1_csi_cfg_idx_s;

/*! @brief Periodic PUCCH CQI reporting types added for CSF schedule message. 
    Types are semi-statically configured through higher layers 36.213 Tab. 7.2.2-1.
*/
typedef enum
{
   PUCCH_CSF_REPORT_TYPE_1 = 1,  ///< Type 1: SB CQI
   PUCCH_CSF_REPORT_TYPE_2 = 2,  ///< Type 2: WB CQI, PMI
   PUCCH_CSF_REPORT_TYPE_3 = 3,  ///< Type 3: RI
   PUCCH_CSF_REPORT_TYPE_4 = 4,  ///< Type 4: WB CQI
   PUCCH_CSF_REPORT_TYPE_5 = 5,  ///< Type 5 report: RI and WB PMI_1
   PUCCH_CSF_REPORT_TYPE_6 = 6,  ///< Type 5 report: RI and WB PMI_1
   PUCCH_CSF_REPORT_TYPE_1A = 7, ///< type 1A: SB CQI and PMI_2
   PUCCH_CSF_REPORT_TYPE_2A = 8, ///< type 2A: WB PMI
   PUCCH_CSF_REPORT_TYPE_2B = 9, ///< type 2B: WB CQI, WB PMI_1, differential WB CQI_2 if RI>1
   PUCCH_CSF_REPORT_TYPE_2C = 10,///< type 2C: WB CQI, WB PMI_1, WB PMI_2, differential WB CQI_2 if RI>1
   PUCCH_CSF_INVALID_REPORT_TYPE
}lte_LL1_pucch_csf_report_type_enum;

/*! @brief
  CQI reporting Periodic
*/

typedef struct
{

  /*! Reporting Periodic enable flag */
  boolean enable;

  /*! PUCCH resource Index. Range: 0-767 */
  uint16 pucch_resource_index;

  /*! CQI/PMI Periodicity and Offset Configuration Index.  Range: 0-511*/
  uint16 cqi_pmi_cfg_index;

  /* PUCCH CQI Feedback Type, see TS 36.213 [23, table 7.2.2-1]. Depending on 
     transmissionMode, reporting mode is implicitly given from the table. */
  lte_l1_cqi_format_indicator_e format_indicator;

  /* Paramater K - Used only if CQI format indicator is set to 
     LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND.  Range: 1-4 */
  uint8 subband_cqi_k;

  /* RI cfg index enable / disable flag */
  boolean ri_cfg_index_enable;

  /* Parameter RI cfg index */
  uint16 ri_cfg_index;

  /*! Parameter: Simultaneous-AN-and-CQI. see TS 36.213 [23, 10.1] TRUE indicates 
      that simultaneous transmission of ACK/NACK and CQI is allowed. */
  boolean sim_ack_nak_cqi;

  /*! csi-ConfigIndex only for PCell and only if csi-SubframePatternConfig is configured. 
      The UE shall release csi-ConfigIndex if csi-SubframePatternConfig is 
      released. CSI config Index */
  lte_l1_csi_cfg_idx_s csi_cfg_idx;

}lte_l1_cqi_reporting_periodic_s;

/*! @brief
  The CQI Report mode
*/
typedef enum
{
  LTE_L1_CQI_SUB_MODE_1 ,
  LTE_L1_CQI_SUB_MODE_2
} lte_l1_csi_report_mode_e;

/*! @brief
  The CQI periodicty factor 
*/
typedef enum
{
  LTE_L1_CQI_PERIOD_FACT_N2 ,
  LTE_L1_CQI_PERIOD_FACT_N4
} lte_l1_csi_periodicity_factor_e;

/*! @brief
  The CQI config index action 
*/
typedef enum
{
  LTE_L1_CQI_CONFIG_INDEX_SETUP ,
  LTE_L1_CQI_CONFIG_INDEX_RELEASE
} lte_l1_csi_config_index_action_e;

  /*! @brief
  aperiodicCSI-Trigger as part of CQI-ReportAperiodic r10
  */
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;
  /*! indicates for which serving cell(s) the aperiodic CSI report is triggered when one or more SCells are configured. 
  trigger1 corresponds to the CSI request field 10 and trigger2 corresponds to the CSI request field 11, see TS 36.213 [23, table 7.2.1-1A]. 
  The leftmost bit, bit 0 in the bit string corresponds to the cell with ServCellIndex=0 and bit 1 in the bit string corresponds to the cell with 
  ServCellIndex=1 etc.Bit 0 is Most Significant Bit(Left Most bit) of the BITSTRING*/
  uint8 trigger1;
  uint8 trigger2;
}lte_l1_cqi_aperiodic_rel10_param_s;

/*! @brief
  csi-ConfigIndex
  E-UTRAN configures csi-ConfigIndex only for PCell and only if 
  csi-SubframePatternConfig is configured. The UE shall release csi-ConfigIndex 
  if csi-SubframePatternConfig is released. CSI config Index 
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;
  
  /*cqi_pmi_config_Index2     INTEGER   (0 .  . 1023 )  ,  */
  uint16 cqi_pmi_config_Index2;
  /*valid only when csi_config_index_action = SETUP, ri_config_index2    INTEGER   (0 .  . 1023 )  ,      */
  boolean ri_config_index2_presnt;
  /*RI Config Index IRI, see TS 36.213 [23, 7.2.2-1B]*/
  uint16 ri_config_index2;

}lte_l1_csi_cfg_index_rel10_param_s;


/*! @brief
   CQI-Reportperiodic r10
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;

  /*! mL1 to look at pucch_resource_indexp1 only if pucch_resource_indexp1_present is TRUE*/
  boolean pucch_resource_indexp1_present;
  /*! PUCCH resource Index. Range: (0 .  . 1184 ) */
  uint16 pucch_resource_indexp1;

  /*csi_reportmode is valid only if format_indicator = LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND*/
  boolean csi_reportmode_valid;
  lte_l1_csi_report_mode_e csi_reportmode;

  /* Paramater periodicityFactor - Used only if CQI format indicator is set to 
     LTE_L1_CQI_FORMAT_INDICATOR_PERIODIC_SUBBAND.  Range: n2,n4 */
  lte_l1_csi_periodicity_factor_e subband_cqi_periodicity_factor;

  /*! csi-ConfigIndex-r10 */
  lte_l1_csi_cfg_index_rel10_param_s csi_cfg_index;
}lte_l1_cqi_periodic_rel10_param_s;

/*! @brief
  MeasSubframePattern type
*/
typedef enum
{
  LTE_L1_MEAS_SUBFM_PATTERN_FDD ,
  LTE_L1_MEAS_SUBFM_PATTERN_TDD
} lte_l1_meas_subfm_pattern_type_e;


/*! @brief
   MeasSubframePattern-r10
*/
typedef struct
{
  /*! Indicate which of TDD or FDD cfg valid */
  lte_l1_meas_subfm_pattern_type_e type;

  /*! The IE MeasSubframePattern is used to specify time domain measurement
      resource restriction. The first/leftmost bit corresponds to the 
      subframe #0 of the radio frame satisfying SFN mod x = 0, where SFN is that 
      of PCell and x is the size of the bit string divided by 10. "1" denotes 
      that the corresponding subframe is used for measurement.
      FDD pattern is 40 bits.
      TDD could be 20, 70 or 60 bits.
  */
  uint8 subfm_num_bits;
  uint32 subfm_pattern[LTE_L1_MAX_MEAS_SUBFM_PATTERN_SIZE];

  /*add TDD part when we will add CA support for TDD*/

}lte_l1_meas_subfm_pattern_param_s;


/*! @brief
   csi-SubframePatternConfig-r10
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;
  
  lte_l1_meas_subfm_pattern_param_s csi_meas_subfm_set1;
  
  lte_l1_meas_subfm_pattern_param_s csi_meas_subfm_set2;

}lte_l1_csi_subfm_pattern_cfg_param_s;


/*! @brief
   measurement subframe pattern used for RRM and RLM
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean enable;
  
  lte_l1_meas_subfm_pattern_param_s meas_subfm;
  
}lte_l1_subfm_pattern_cfg_param_s;


/*! @brief
  CQI reporting Release 10 parameters
*/
typedef struct
{
  /*!CQI-ReportAperiodic -r10 */
  lte_l1_cqi_aperiodic_rel10_param_s cqi_aperiodic_cfg;

  /*!CQI-Reportperiodic -r10 */
  lte_l1_cqi_periodic_rel10_param_s cqi_periodic_cfg;
  
  /*!csi-SubframePatternConfig-r10, needed only for on enhanced-ICIC  feature */
  lte_l1_csi_subfm_pattern_cfg_param_s csi_subfm_pattern_cfg;

}lte_l1_cqi_rel10_param_s;


/*! @brief
  CQI reporting Release 9 parameters
*/

typedef struct
{

  /*! CQI Mask enable flag */
  boolean cqi_mask_enable;

  /*! PMI_RI report configured flag */
  boolean pmi_ri_report_configured;

}lte_l1_cqi_rel9_param_s;


/*! @brief
  PDSCH assignment type
  Defined in TBD
*/
typedef enum
{
  LTE_L1_DL_PDSCH_DYNAMIC_ASSIGN = 0,
  LTE_L1_DL_PDSCH_PERSISTENT_ASSIGN
} lte_l1_dl_pdsch_assignment_e;


/*! @brief
  PHICH decoder result enum
  Defined in TBD
*/
typedef enum
{
  LTE_L1_PHICH_RESULT_NACK = 0,    
  LTE_L1_PHICH_RESULT_ACK,             // same as before

  LTE_L1_PHICH_RESULT_NACK1_NACK0 = 0, //  i.e. 0 0
  LTE_L1_PHICH_RESULT_NACK1_ACK0,      //       0 1
  LTE_L1_PHICH_RESULT_ACK1_NACK0,      //       1 0
  LTE_L1_PHICH_RESULT_ACK1_ACK0,       //       1 1     

} lte_l1_phich_result_e;

/*! @brief
  PDCCH header type.
  Defined in 3.2.3.2 pdcch header from Espresso UE ML-LL interface
  TBD: to discuss with LL folks to include the detail pdcch header struct
  in the this file.
*/
typedef uint64 lte_l1_pdcch_header_t;


/*! @brief
  PDCCH header type.
  Defined in 3.2.3.3 PDCCH payload from Espresso UE ML-LL interface
  TBD: to discuss with LL folks to include the detail pdcch header struct
*/
typedef uint64 lte_l1_pdcch_payload_t;


/*! @brief
  PDSCH result
  Defined in 3.2.3.4 LTE_LL1_DL_PDSCH_RESULT_IND of Espresso UE ML-LL interface
  TBD: to discuss with LL folks to include the detail PDSCH result struct
*/
typedef struct
{
  /*! subframe index within a radio frame [0..9] */
  lte_l1_subfn_t subfn;
} lte_l1_pdsch_result_s;


/*! @brief
  PDSCH deocder header strcut
  Defined in 3.2.3.5 pdsch_decoder_data header from Espresso UE ML-LL interface
  TBD: to discuss with LL folks to include the detail struct
*/
typedef struct
{
  /*! SFN index tagged by the DDE*/
  lte_l1_sfn_t sfn_idx;
} lte_l1_pdsch_decoder_header_s;

/*! @brief
    P_B offset (ref: 36.213, 5.2) between Type A and Type B
    PDSCH resource elements.
    P_B indicates the ratio of PDSCH-to-RS EPRE in symbols with
    and without reference symbols for 1, 2 or 4 cell specific
    antenna ports.
*/
typedef enum {
  LTE_L1_PB0 = 0,
  LTE_L1_PB1,
  LTE_L1_PB2,
  LTE_L1_PB3,
  /* Used when PB unknown*/
  LTE_L1_PB_FW_HYPOTHESIS,
  LTE_L1_MAX_NUM_PB
} lte_l1_pb_e;

/*! @brief
    The offset between PDSCH and RS EPRE. (ref:36.213, 5.2). P_A provides
    information about the exact power setting of the PDSCH transmission.
    dB_n6 corresponds to -6 dB, dB_3 corresponds to 3 dB etc.
*/
typedef enum {
  LTE_L1_PA_DB_N6 = 0,
  LTE_L1_PA_DB_N4DOT77,
  LTE_L1_PA_DB_N3,
  LTE_L1_PA_DB_N1DOT77,
  LTE_L1_PA_DB_0,
  LTE_L1_PA_DB_1,
  LTE_L1_PA_DB_2,
  LTE_L1_PA_DB_3,
  /* Used when PA unknown*/
  LTE_L1_PA_FW_HYPOTHESIS,
  LTE_L1_MAX_NUM_PA
} lte_l1_pa_e;

/*! @brief
    Parameter: PERIODIC_PHR_TIMER [36.321]. Value in number of sub-frames. Value
    sf10 corresponds to 10 subframes, sf20 corresponds to 20 subframes and so on.
*/
typedef enum {
  LTE_L1_PERIODIC_PHR_TIMER_SF10 = 10,
  LTE_L1_PERIODIC_PHR_TIMER_SF20 = 20,
  LTE_L1_PERIODIC_PHR_TIMER_SF50 = 50,
  LTE_L1_PERIODIC_PHR_TIMER_SF100 = 100,
  LTE_L1_PERIODIC_PHR_TIMER_SF200 = 200,
  LTE_L1_PERIODIC_PHR_TIMER_SF500 = 500,
  LTE_L1_PERIODIC_PHR_TIMER_SF1000 = 1000,
  LTE_L1_PERIODIC_PHR_TIMER_INFINITE = 0
} lte_l1_periodic_phr_timer_e;

/*! @brief
    Parameter: PROHIBIT PHR TIMER [36.321]. Value in number of sub-frames. Value
    sf10 corresponds to 10 subframes, sf20 corresponds to 20 subframes and so on.
*/
typedef enum {
  LTE_L1_PROHOBIT_PHR_TIMER_SF0 = 0,
  LTE_L1_PROHOBIT_PHR_TIMER_SF10 = 10,
  LTE_L1_PROHOBIT_PHR_TIMER_SF20 = 20,
  LTE_L1_PROHOBIT_PHR_TIMER_SF50 = 50,
  LTE_L1_PROHOBIT_PHR_TIMER_SF100 = 100,
  LTE_L1_PROHOBIT_PHR_TIMER_SF200 = 200,
  LTE_L1_PROHIBIT_PHR_TIMER_SF500 = 500,
  LTE_L1_PROHOBIT_PHR_TIMER_SF1000 = 1000
} lte_l1_prohibit_phr_timer_e;

/*! @brief
    Parameter: DL PathlossChange [36.321]. Value in dB. Value dB1 corresponds to
    1 dB, dB3 corresponds to 3 dB and so on.
*/
typedef enum {
  LTE_L1_DL_PATHLOSS_DB_1 = 1,
  LTE_L1_DL_PATHLOSS_DB_3 = 3,
  LTE_L1_DL_PATHLOSS_DB_6 = 6,
  LTE_L1_DL_PATHLOSS_DB_INFINITE
} lte_l1_dl_pathloss_change_e;

/*! @brief
  SPS configuration for Downlink
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean valid;

  /*! Flag to enable\disable SPS config for DL */
  boolean enable;

  /*! The downlink semi-persistant scheduling interval */
  lte_l1_sps_interval_e sps_interval;

  /*! Number of configured SPS processes.  Range: 1-8*/
  uint8 num_conf_sps_processes;

  /*! Parameter: N1 PUCCH.  see TS 36.213, [23, 10.1]*/
  uint8 n1_pucch_an_list_size;
  uint16 n1_pucch_an_persistent[MAX_N1_PUCCH_AN_LIST_SIZE]; 

}lte_l1_sps_cfg_dl_s;

/*! @brief
  SPS configuration for Uplink
*/
typedef struct
{
  /*! Indicates if field within this structure are valid or not */
  boolean valid;

  /*! Flag to enable\disable SPS config for UL */
  boolean enable;

  /*! The uplink semi-persistant scheduling interval */
  lte_l1_sps_interval_e sps_interval;

  /*! Flag to enable\disable two-intervals-SPS for uplink */
  boolean two_intervals_config;

  /*! Flag to indicate if p0_persistent is signaled or not; The flag applies
    to p0_nominal_pusch_persistent and p0_ue_pusch_persistent fields below.
    If set to TRUE, ML1 should apply the values signaled in these fields;
    If set to FALSE, ML1 should apply the values as follows, per 36.331 sec 6.3.2.
    p0-NominalPUSCH for p0-NominalPUSCH-Persistent
    p0-UE-PUSCH for p0-UE-PUSCH-Persistent. */
  boolean p0_persistent_present;

  /*! See TS 36.213, 5.1.1.1, unit dBm step 1. This field is applicable for 
      persistent scheduling, only. Range -126-24 */
  int16 p0_nominal_pusch_persistent;

  /*! See TS 36.213, 5.1.1.1, unit dB. This field is applicable for persistent 
      scheduling, only.  Range -8-7 */
  lte_l1_ul_power_ctrl_p0_ue_t p0_ue_pusch_persistent;


}lte_l1_sps_cfg_ul_s;

/*===========================================================================

                          SEARCH RELATED TYPES

===========================================================================*/
/*! @brief
  Search coherent accumulation length
  Defined 3.4.1.2  LTE_LL1_SRCH_REQ_INIT_ACQ from Espresso UE ML-LL interface
*/
typedef enum
{
  LTE_L1_SRCH_COH_ACC_LEN_64 = 0,
  LTE_L1_SRCH_COH_ACC_LEN_32,
  LTE_L1_SRCH_COH_ACC_LEN_16
} lte_l1_srch_coh_acc_len_e;


/*===========================================================================

                    CELL SELECTION/RESELECTION/HANDOVER TYPES

===========================================================================*/
/*! @brief
  Handover info parameteters
  Defined in 36.331 Section 6.3.4 MobilityControlInformation
  TODO fill in this structure
*/
typedef struct
{
  lte_l1_TBD_t tbd_params;
} lte_l1_handover_info_param_s;

/*! @brief
  Cell selection parameters for Release 9
*/
typedef struct
{
  /*! Minimum required quality level in the cell in dB, Range (-34 .. -3),
    default value LTE_CPHY_INT8_NEG_INFINITY when not signaled */
  int8 q_qualmin;

  /*! Offset to the signaled q_qualmin, Range (1 .. 8), default value 0 
    when not signaled */
  uint8 q_qualmin_offset;

} lte_l1_cell_select_info_rel9_s;

/*===========================================================================

                    DRX PARAMTER TYPES

===========================================================================*/

/*! NB parameter used to specify the number of paging groups
  36.331 Section 6.3.2
 */
typedef enum
{
  LTE_L1_NB_FOURT,
  LTE_L1_NB_TWOT,
  LTE_L1_NB_ONET,
  LTE_L1_NB_HALFT,
  LTE_L1_NB_QUARTERT,
  LTE_L1_NB_ONEEIGHTT,
  LTE_L1_NB_ONSIXTEENTHT,
  LTE_L1_NB_ONETHIRTYSECONDT
} lte_l1_nb_e;

/*! The ue id to be used for the paging interval */
typedef uint16 lte_l1_drx_ue_id_t;

/*! Minimum Rx level to camp on cell (-70..-22) * 2 */
typedef int32 lte_l1_q_rxlevmin_t;

/*! Range: 1..8 */
typedef uint32 lte_l1_q_rxlevmin_offset_t;

/*! Reselection Threshold Range: 0..63 for C DMA2000; 0..31 for all other RATs */
typedef uint8 lte_l1_resel_threshold_t;

/*! Invalid value for measurement search thresholds. */
#define LTE_CPHY_MEAS_SRCH_THRESHOLD_INVALID    0xFF

/*! Search Threshold Range: 0..31 */
typedef uint8 lte_l1_srch_meas_threshold_t;

/*! Resel priority value not provided by NW. */
#define LTE_CPHY_MEAS_RESEL_PRIORITY_NONE    0xFF

/*! Resel priority value when a freq is deprioritized. Priority should be treated as 
  lower than the network configured priority 0..7 */
#define LTE_CPHY_MEAS_RESEL_PRIORITY_MIN     -1

/*! Reselection Priority Range: -1..7.  
    LTE_CPHY_MEAS_RESEL_PRIORITY_NONE is used for serving freq when NW doesn't 
    provide priority info.

    LTE_CPHY_MEAS_RESEL_PRIORITY_MIN is used when a freq is deprioritized and its 
    priority should be treated as lower than the network configured priority 0..7 

    Non serving freqs and IRAT freqs are filtered out by RRC if NW doesn't provide
    cell resel priority info.
 */
typedef int16 lte_l1_resel_priority_t;

/*! Reselection Timeout Range: 0..7 */
typedef uint8 lte_l1_treselection_t;



/*===========================================================================

                   CONNECTED MODE DRX PARAMETER TYPES

===========================================================================*/

typedef enum
{
  LTE_L1_ON_DURATION_TIMER_PSF1 = 1,
  LTE_L1_ON_DURATION_TIMER_PSF2 = 2,
  LTE_L1_ON_DURATION_TIMER_PSF3 = 3,
  LTE_L1_ON_DURATION_TIMER_PSF4 = 4,
  LTE_L1_ON_DURATION_TIMER_PSF5 = 5,
  LTE_L1_ON_DURATION_TIMER_PSF6 = 6,
  LTE_L1_ON_DURATION_TIMER_PSF8 = 8,
  LTE_L1_ON_DURATION_TIMER_PSF10 = 10,
  LTE_L1_ON_DURATION_TIMER_PSF20 = 20,
  LTE_L1_ON_DURATION_TIMER_PSF30 = 30,
  LTE_L1_ON_DURATION_TIMER_PSF40 = 40,
  LTE_L1_ON_DURATION_TIMER_PSF50 = 50,
  LTE_L1_ON_DURATION_TIMER_PSF60 = 60,
  LTE_L1_ON_DURATION_TIMER_PSF80 = 80,
  LTE_L1_ON_DURATION_TIMER_PSF100 = 100,
  LTE_L1_ON_DURATION_TIMER_PSF200 = 200
} lte_l1_on_duration_timer_e;


typedef enum
{
  LTE_L1_DRX_INACTIVITY_TIMER_PSF0 = 0,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF1 = 1,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF2 = 2,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF3 = 3,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF4 = 4,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF5 = 5,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF6 = 6,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF8 = 8,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF10 = 10,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF20 = 20,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF30 = 30,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF40 = 40,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF50 = 50,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF60 = 60,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF80 = 80,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF100 = 100,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF200 = 200,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF300 = 300,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF500 = 500,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF750 = 750,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF1280 = 1280,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF1920 = 1920,
  LTE_L1_DRX_INACTIVITY_TIMER_PSF2560 = 2560
}lte_l1_drx_inactivity_timer_e;


typedef enum
{
  LTE_L1_DRX_RETX_TIMER_SF1 = 1,
  LTE_L1_DRX_RETX_TIMER_SF2 = 2,
  LTE_L1_DRX_RETX_TIMER_SF4 = 4,
  LTE_L1_DRX_RETX_TIMER_SF6 = 6,
  LTE_L1_DRX_RETX_TIMER_SF8 = 8,
  LTE_L1_DRX_RETX_TIMER_SF16 = 16,
  LTE_L1_DRX_RETX_TIMER_SF24 = 24,
  LTE_L1_DRX_RETX_TIMER_SF33 = 33
} lte_l1_drx_retx_timer_e;


typedef enum
{
  LTE_L1_DRX_LONG_CYCLE_SF10 = 10,
  LTE_L1_DRX_LONG_CYCLE_SF20 = 20,
  LTE_L1_DRX_LONG_CYCLE_SF32 = 32,
  LTE_L1_DRX_LONG_CYCLE_SF40 = 40,
  LTE_L1_DRX_LONG_CYCLE_SF64 = 64,
  LTE_L1_DRX_LONG_CYCLE_SF80 = 80,
  LTE_L1_DRX_LONG_CYCLE_SF128 = 128,
  LTE_L1_DRX_LONG_CYCLE_SF160 = 160,
  LTE_L1_DRX_LONG_CYCLE_SF256 = 256,
  LTE_L1_DRX_LONG_CYCLE_SF320 = 320,
  LTE_L1_DRX_LONG_CYCLE_SF512 = 512,
  LTE_L1_DRX_LONG_CYCLE_SF640 = 640,
  LTE_L1_DRX_LONG_CYCLE_SF1024 = 1024,
  LTE_L1_DRX_LONG_CYCLE_SF1280 = 1280,
  LTE_L1_DRX_LONG_CYCLE_SF2048 = 2048,
  LTE_L1_DRX_LONG_CYCLE_SF2560 = 2560
} lte_l1_drx_long_cycle_e;



typedef enum
{
  LTE_L1_DRX_SHORT_CYCLE_SF2 = 2,
  LTE_L1_DRX_SHORT_CYCLE_SF5 = 5,
  LTE_L1_DRX_SHORT_CYCLE_SF8 = 8,
  LTE_L1_DRX_SHORT_CYCLE_SF10 = 10,
  LTE_L1_DRX_SHORT_CYCLE_SF16 = 16,
  LTE_L1_DRX_SHORT_CYCLE_SF20 = 20,
  LTE_L1_DRX_SHORT_CYCLE_SF32 = 32,
  LTE_L1_DRX_SHORT_CYCLE_SF40 = 40,
  LTE_L1_DRX_SHORT_CYCLE_SF64 = 64,
  LTE_L1_DRX_SHORT_CYCLE_SF80 = 80,
  LTE_L1_DRX_SHORT_CYCLE_SF128 = 128,
  LTE_L1_DRX_SHORT_CYCLE_SF160 = 160,
  LTE_L1_DRX_SHORT_CYCLE_SF256 = 256,
  LTE_L1_DRX_SHORT_CYCLE_SF320 = 320,
  LTE_L1_DRX_SHORT_CYCLE_SF512 = 512,
  LTE_L1_DRX_SHORT_CYCLE_SF640 = 640
} lte_l1_drx_short_cycle_e;

typedef enum
{
  LTE_L1_ACK_NAK_REPETITION_FACTOR_N2,
  LTE_L1_ACK_NAK_REPETITION_FACTOR_N4,
  LTE_L1_ACK_NAK_REPETITION_FACTOR_N6
}lte_l1_ack_nak_repetition_factor_e;

typedef enum
{
  LTE_L1_TDD_ACK_NAK_FEEDBACK_MODE_BUNDLING,
  LTE_L1_TDD_ACK_NAK_FEEDBACK_MODE_MULTIPLEXING
}lte_l1_tdd_ack_nak_feedback_mode_e;

/*===========================================================================

                    PRACH PARAMETER TYPES

===========================================================================*/
/*! @brief
  Defined in TBD
  TBD fill in this struct
*/
typedef struct
{
  lte_l1_TBD_t params_to_be_filled_in;
} lte_l1_rach_cfg_s;


/*===========================================================================

                     MBSFN related TYPES

===========================================================================*/

typedef enum
{
  LTE_L1_MBSFN_ALLOCATION_ONE = 0,
  LTE_L1_MBSFN_ALLOCATION_FOUR
} lte_l1_mbsfn_allocation_e;

typedef enum
{
  LTE_L1_MBSFN_ALLOCATION_PERIOD_1 = 0,
  LTE_L1_MBSFN_ALLOCATION_PERIOD_2,
  LTE_L1_MBSFN_ALLOCATION_PERIOD_4,
  LTE_L1_MBSFN_ALLOCATION_PERIOD_8,
  LTE_L1_MBSFN_ALLOCATION_PERIOD_16,
  LTE_L1_MBSFN_ALLOCATION_PERIOD_32
} lte_l1_mbsfn_allocation_period_e;

/*! @brief
    Enumeration defines non-mbsfn region length in MBSFN subframe
*/
typedef enum
{
  LTE_L1_NON_MBSFN_SYMBOL_ONE = 1,
  LTE_L1_NON_MBSFN_SYMBOL_TWO = 2
} lte_l1_non_mbsfn_region_length_e;

/*! @brief
    Enumeration defines MCCH repition period in radio frames
*/
typedef enum
{
  LTE_L1_MBSFN_MCCH_REP_PERIOD_32RF = 32,
  LTE_L1_MBSFN_MCCH_REP_PERIOD_64RF = 64,
  LTE_L1_MBSFN_MCCH_REP_PERIOD_128RF = 128,
  LTE_L1_MBSFN_MCCH_REP_PERIOD_256RF = 256
} lte_l1_mbsfn_mcch_rep_period_e;

/*! @brief
    Enumeration defines MCCH modification period in radio frames
*/
typedef enum
{
  LTE_L1_MBSFN_MCCH_MOD_PERIOD_512RF = 512,
  LTE_L1_MBSFN_MCCH_MOD_PERIOD_1024RF = 1024
} lte_l1_mbsfn_mcch_mod_period_e;

/*! @brief
    Enumeration defines signalling MCS: Indicates the Modulation and Coding Scheme (MCS)
    applicable for the subframes indicated by the field sf-AllocInfo and for the first
    subframe of each MCH scheduling period (which may contain the MCH scheduling
    information provided by MAC). Value n2 corresponds with the value 2 for parameter
    in TS 36.213 [23, Table 7.1.7.1-1], and so on. 
*/
typedef enum
{
  LTE_L1_MBSFN_SIGNALLING_MCS_N2  = 2,
  LTE_L1_MBSFN_SIGNALLING_MCS_N7  = 7,
  LTE_L1_MBSFN_SIGNALLING_MCS_N13 = 13,
  LTE_L1_MBSFN_SIGNALLING_MCS_N19 = 19
} lte_l1_mbsfn_signalling_mcs_e;

/*! @brief
    Enumeration defines mcch change notification coefficient.
    Actual change notification repetition period common for all MCCHs
    that are configured = shortest modification period/ notificationRepetitionCoeff.
    The 'shortest modificaton period' corresponds with the lowest value of
    mcch-ModificationPeriod of all MCCHs that are configured.
    Value n2 corresponds to coefficient 2, and so on.
*/
typedef enum
{
  LTE_L1_MBSFN_NOTIFICATION_COEFF_N2 = 2,
  LTE_L1_MBSFN_NOTIFICATION_COEFF_N4 = 4
} lte_l1_mbsfn_notification_coeff_e;

/*! @brief
    Enumeration defines actual change notification repetition period
    common for all MCCHs that are configured
    = shortest modification period/ notificationRepetitionCoeff.
    The 'shortest modificaton period' corresponds with the lowest value of
    mcch-ModificationPeriod of all MCCHs that are configured.
*/
typedef enum
{
  LTE_L1_MBSFN_NOTIFICATION_PERIOD_128RF = 128,
  LTE_L1_MBSFN_NOTIFICATION_PERIOD_256RF = 256,
  LTE_L1_MBSFN_NOTIFICATION_PERIOD_512RF = 512
} lte_l1_mbsfn_notification_period_e;

/*! @brief
    Enumeration defines MBSFN area common subframe allocation period in radio frames   
*/
typedef enum
{
  LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_4RF = 4,
  LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_8RF = 8,
  LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_16RF = 16,
  LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_32RF = 32,
  LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_64RF = 64,
  LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_128RF = 128,
  LTE_L1_MBSFN_COMMON_SF_ALLOC_PERIOD_256RF = 256
} lte_l1_mbsfn_common_sf_allocation_period_e;

/*! @brief
    Enumeration defines MCH scheduling period in radio frames   
*/
typedef enum
{
  LTE_L1_MBSFN_MCH_SCHDL_PERIOD_8RF = 8,
  LTE_L1_MBSFN_MCH_SCHDL_PERIOD_16RF = 16,
  LTE_L1_MBSFN_MCH_SCHDL_PERIOD_32RF = 32,
  LTE_L1_MBSFN_MCH_SCHDL_PERIOD_64RF = 64,
  LTE_L1_MBSFN_MCH_SCHDL_PERIOD_128RF = 128,
  LTE_L1_MBSFN_MCH_SCHDL_PERIOD_256RF = 256,
  LTE_L1_MBSFN_MCH_SCHDL_PERIOD_512RF = 512,
  LTE_L1_MBSFN_MCH_SCHDL_PERIOD_1024RF = 1024
} lte_l1_mbsfn_mch_schdl_period_e;

/*! @brief The mbsfn configuration: subframes that are reserved for MBSFN in 
    downlink
*/
typedef struct
{

  /*! The mbsfn allocation offset */
  uint8                            allocation_offset;
  /*! The mbsfn allocation periodicity */
  lte_l1_mbsfn_allocation_period_e allocation_period;

  /*! The number of mbsfn subframes allocated every repetition. */
  lte_l1_mbsfn_allocation_e        sfn_allocation;

  /*!
  Radio-frames that contain MBSFN subframes occur when equation SFN mod 
  radioFrameAllocationPeriod = radioFrameAllocationOffset is satisfied. 
  n1 denotes value 1, n2 denotes value 2, and so on. When fourFrames is used, 
  the equation defines the first radio frame referred to in the description 
  below. Values n1 and n2 are not applicable when fourFrames is used. 
 

   Bit mask identifying which subframes are mbsfn subframes 
   the lower 6 bits are used when there is a 1 subframe allocation,
   the lower 24 bits are used if there is a 4 frame allocation. 
   
   The bits are allocated as follows:

   case LTE_L1_MBSFN_ALLOCATION_ONE:

   FDD: The first/leftmost bit defines the MBSFN allocation for subframe #1,
   the second bit for #2, third bit for #3 , fourth bit for #6, fifth bit 
   for #7, sixth bit for #8.
   TDD: The first/leftmost bit defines the allocation for subframe #3, the 
   second bit for #4, third bit for #7, fourth bit for #8, fifth bit for 
   #9. Uplink subframes are not allocated. The last bit is not used.

   case LTE_L1_MBSFN_ALLOCATION_FOUR:

   FDD: Starting from the first radioframe and from the first/leftmost bit 
   in the bitmap, the allocation applies to subframes  #1, #2, #3 , #6, #7,
   and #8 in the sequence  of the four radio-frames.
   TDD: Starting from the first radioframe and from the first/leftmost bit 
   in the bitmap, the allocation applies to subframes  #3, #4, #7, #8, and 
   #9 in the sequence  of the four radio-frames. The last four bits are not
   used. Uplink subframes are not allocated. 

   */
  uint32 sfn_allocation_mask;


} lte_l1_mbsfn_allocation_s;

/*! @brief MBSFN area MCCH schedule config
*/
typedef struct
{
  /*! Defines the interval between transmissions of MCCH information,
      in radio frames, Value rf32 corresponds to 32 radio frames,
      rf64 corresponds to 64 radio frames and so on.
  */
  lte_l1_mbsfn_mcch_rep_period_e     rep_period;

  /*! MCCH offset (0 -10): Indicates, together with the mcch-RepetitionPeriod,
     the radio frames in which MCCH is scheduled i.e. MCCH is scheduled in
     radio frames for which: SFN mod mcch-RepetitionPeriod = mcch-Offset
  */
  uint8                              offset;

  /*! Defines periodically appearing boundaries, i.e. radio frames for which
      SFN mod mcch-ModificationPeriod = 0, The contents of different transmissions
      of MCCH information can only be different if there is at least one
      such boundary in-between them.
  */
  lte_l1_mbsfn_mcch_mod_period_e     mod_period;

  /*! Indicates the subframes of the radio frames indicated by the mcch-RepetitionPeriod
      and the mcch-Offset, that may carry MCCH. Value "1" indicates that the corresponding
      subframe is allocated.
      The following mapping applies:
      FDD: The first/ leftmost bit defines the allocation for subframe #1 of the radio frame
      indicated by mcch-RepetitionPeriod and mcch-Offset, the second bit for #2, the third bit for #3 ,
      the fourth bit for #6, the fifth bit for #7 and the sixth bit for #8.
 
      TDD: The first/leftmost bit defines the allocation for subframe #3 of the radio frame
      indicated by mcch-RepetitionPeriod and mcch-Offset, the second bit for #4, third bit for #7,
      fourth bit for #8, fifth bit for #9. Uplink subframes are not allocated. The last bit is not used.
  */
  uint8                              sf_alloc_info_bitmask;

  /*! Signalling MCS */
  lte_l1_mbsfn_signalling_mcs_e      mcs;         
  
} lte_l1_mbsfn_mcch_cfg_s;


/*! @brief MBSFN area PMCH schedule config ( PMCH allocation)
*/
typedef struct
{
  /* PMCH unique identifier: MBSFN area ID + Unique PMCH ID allocate by RRC */
  /*! PMCH ID:  Only used by higher layers TB-PMCH identification mapping [0 - 15]
 
      Unique PMCH id allocated by RRC and passed from ML1->LL1
      and LL1 can add it in meta data info to upper layers
      0 = Unknown/Reserved (used by ML1 during MCCH decode where PMCHs config is not known)
  */
  lte_pmch_id_t                   pmch_id;

  /*! sf alloc start [0, 1535]: Indicates the first subframe allocated to this (P)MCH within a period
      identified by field commonSF-AllocPeriod. The subframes allocated to (P)MCH corresponding
      with the nth entry in pmch-InfoList are the subsequent subframes starting from either the
      subframe identified by sf-AllocEnd of the (n-1)th listed (P)MCH or, for n=1, the first subframe,
      through the subframe identified by sf-AllocEnd of the nth listed (P)MCH.
      Value 0 corresponds with the first subframe defined by field commonSF-Alloc.
 
      sf_alloc_start <= sf_alloc_end
  */
  uint16                          sf_alloc_start;

  /*! sf alloc end [0, 1535]: Indicates the last subframe allocated to this (P)MCH within a period
      identified by field commonSF-AllocPeriod. The subframes allocated to (P)MCH corresponding
      with the nth entry in pmch-InfoList are the subsequent subframes starting from either the
      subframe identified by sf-AllocEnd of the (n-1)th listed (P)MCH or, for n=1, the first subframe,
      through the subframe identified by sf-AllocEnd of the nth listed (P)MCH.
      Value 0 corresponds with the first subframe defined by field commonSF-Alloc. 
  */
  uint16                          sf_alloc_end;
  /*! Data MCS [0, 28]: Indicates the value for parameter  in TS 36.213 [23, Table 7.1.7.1-1],
      which defines the Modulation and Coding Scheme (MCS) applicable for the subframes
      of this (P)MCH as indicated by the field commonSF-Alloc.
      The MCS does however neither apply to the subframes that may carry MCCH
      i.e. the subframes indicated by the field sf-AllocInfo within SystemInformationBlockType13
      nor for the first subframe of each MCH scheduling period
      (which may contain the MCH scheduling information provided by MAC).
  */
  uint8                            mcs;    
  
  /*! MCH scheduling period: Indicates the MCH scheduling period i.e. the periodicity used for
      providing MCH scheduling information at lower layers (MAC) applicable for an MCH.
      Value rf8 corresponds to 8 radio frames, rf16 corresponds to 16 radio frames and so on
      i.e. radio frames for which SFN mod mch-SchedlingPeriod = 0.
  */
  lte_l1_mbsfn_mch_schdl_period_e  schdl_period;     
  
} lte_l1_mbsfn_pmch_cfg_s;

/*! @brief MSI MTCHs schedule info
*/
typedef struct
{
  /*! Whether UE is interested in monitoring this scheduled MTCH or not
      N/A action: LTE_CPHY_MBSFN_AREA_MONITOR_ONLY_MODIFICATION_BOUNDARY
  */
  lte_cphy_mbsfn_area_monitor_pattern_e  monitor_action;
  /*! UE interested MTCH/PMCH ordinal subframe info
      First MTCH in OTA MAC control element start ordinal subframe  = 1st
      Nth MTCH in OTA MAC control element start ordinal subframe =
      Scheduled previous MTCH (may be N-1) stop ordinal subframe
      Nth MTCH end ordinal subframe = MTCH stop field value of this MTCH
  */
  uint16                                 stop_ordinal_subfn;
  /*! Logical channel id */
  uint8                                  lcid;
    
} lte_l1_mbsfn_msi_ordinal_subfn_info_s;

/*! @brief MCH scheduling information (MSI)
*/
typedef struct
{
  /*! MBSFN area id: 0 - 255 */
  lte_mbsfn_area_id_t                       area_id;

  /*! PMCH ID:  Only used by higher layers TB-PMCH identification mapping [0 - 15]
 
      Unique PMCH id allocated by RRC and passed from ML1->LL1
      and LL1 can add it in meta data info to upper layers
      0 = Unknown/Reserved (used by ML1 during MCCH decode where PMCHs config is not known)
  */
  lte_pmch_id_t                             pmch_id;

  /*! MSI OTA SFN */
  lte_l1_sfn_t                              sfn;

  /*! MSI OTA Subfn */
  lte_l1_subfn_t                            subfn;

  /*! Network scheduled all MTCH/PMCH ordinal subframe info in order 
      First MTCH in OTA MAC control element start ordinal subframe  = 1st
      Nth MTCH in OTA MAC control element start ordinal subframe =
      Scheduled previous MTCH (may be N-1) stop ordinal subframe
  */
  lte_l1_mbsfn_msi_ordinal_subfn_info_s     monitor_pmch_info[LTE_L1_MAX_MTCH_PER_PMCH];
 
} lte_l1_mbsfn_msi_cfg_s;


/******************************************************************************
                      UE Timers and Constants Parameter Types
*******************************************************************************/

/*! @brief Enumeration of t310 values
*/
typedef enum
{
    LTE_CPHY_T310_0,      /*!<  0 */
    LTE_CPHY_T310_50,      /*!< 1 */
    LTE_CPHY_T310_100,      /*!< 2 */
    LTE_CPHY_T310_200,      /*!< 3 */
    LTE_CPHY_T310_500,      /*!< 4 */
    LTE_CPHY_T310_1000,      /*!< 5 */
    LTE_CPHY_T310_2000     /*!< 6 */

} lte_cphy_t310_e;

/*! @brief Enumeration of n310 values
*/
typedef enum
{
    LTE_CPHY_N310_1 = 1,      /*!<  1 */
    LTE_CPHY_N310_2 = 2,      /*!< 2 */
    LTE_CPHY_N310_3 = 3,      /*!< 3 */
    LTE_CPHY_N310_4 = 4,      /*!< 4 */
    LTE_CPHY_N310_6 = 6,      /*!< 6 */
    LTE_CPHY_N310_8 = 8,     /*!< 8 */
    LTE_CPHY_N310_10 = 10,    /*!< 10 */
    LTE_CPHY_N310_20 = 20   /*!< 20 */

} lte_cphy_n310_e;

/*! @brief Enumeration of n311 values
*/
typedef enum
{
    LTE_CPHY_N311_1 = 1,      /*!<  1 */
    LTE_CPHY_N311_2 = 2,      /*!< 2 */
    LTE_CPHY_N311_3 = 3,      /*!< 3 */
    LTE_CPHY_N311_4 = 4,      /*!< 4 */
    LTE_CPHY_N311_5 = 5,      /*!< 5 */
    LTE_CPHY_N311_6 = 6,      /*!< 6 */
    LTE_CPHY_N311_8 = 8,     /*!< 8 */
    LTE_CPHY_N311_10 = 10    /*!< 10 */

} lte_cphy_n311_e;

/*! @brief Enumeration of OOS reason
*/
typedef enum
{
    LTE_CPHY_OOS_REASON_NORMAL = 1,      /*!<  1 */
    LTE_CPHY_OOS_REASON_RESET = 2,      /*!< 2 */

    LTE_CPHY_OOS_REASON_TMR   = 3,       /*!< 3 */
    LTE_CPHY_OOS_REASON_RESET_1   = 4,       /*!< 4 */
    LTE_CPHY_OOS_REASON_RESET_2   = 5,       /*!< 5 */
    LTE_CPHY_OOS_REASON_RESET_3   = 6,       /*!< 6 */
    LTE_CPHY_OOS_REASON_RESET_4   = 7,       /*!< 7 */
    LTE_CPHY_OOS_REASON_RESET_5   = 8,       /*!< 8 */
    LTE_CPHY_OOS_REASON_RF_UNAVAILABLE = 9,   /*!< 9 */
    LTE_CPHY_OOS_REASON_RSRP_CAMPING = 10   /*!< 10 */
    /*If a new OOS indication is being added (non FW recovery indication related)
         then please update cause in lte_rrc_csp_sm_camped_oos_ind handler
         wherein RRC should ignore Non FW recovery indication in SUSPENDED state*/
} lte_l1_oos_reason_e;

/*! @brief Enumeration of RLF reason
*/
typedef enum
{
    LTE_CPHY_RLF_REASON_NORMAL = 1,      /*!<  1 */
    LTE_CPHY_RLF_REASON_RESET = 2,      /*!< 2 */
    LTE_CPHY_RLF_REASON_TMR = 3,        /*!< 3 */
    LTE_CPHY_RLF_REASON_RESET_1   = 4,       /*!< 4 */
    LTE_CPHY_RLF_REASON_RESET_2   = 5,       /*!< 5 */
    LTE_CPHY_RLF_REASON_RESET_3   = 6,       /*!< 6 */
    LTE_CPHY_RLF_REASON_RESET_4   = 7,       /*!< 7 */
    LTE_CPHY_RLF_REASON_RESET_5   = 8,       /*!< 8 */
    LTE_CPHY_RLF_REASON_RF_UNAVAILABLE   = 9,       /*!< 9 */
    LTE_CPHY_RLF_REASON_CDRX_COLLISION   = 10,       /*!< 10 */
    LTE_CPHY_RLF_REASON_MSG4_ACK_NOT_SENT   = 11       /*!< 11 */

} lte_l1_rlf_reason_e;

/*! @Brief GSM cell Cell ID structure */
typedef struct
{
  uint8 ncc;
  uint8 bcc;
} lte_l1_gsm_cell_id_s;

/*! ARFCN type used to specify a GSM frequency.  This should match
  with the type from the GSM interface, but has been placed here to remove
  the dependancy. */
typedef uint16 lte_l1_gsm_arfcn_t;

typedef enum  
{
  LTE_L1_GSM_FIRST_BAND,
  LTE_L1_GSM_BAND_EGSM_900 = LTE_L1_GSM_FIRST_BAND,
  LTE_L1_GSM_BAND_PGSM_900,
  LTE_L1_GSM_BAND_CELL_850,
  LTE_L1_GSM_BAND_DCS1800,
  LTE_L1_GSM_BAND_PCS1900,
  LTE_L1_GSM_NUM_BANDS
} lte_l1_gsm_band_e;

/*! RSSI measurement quantity type.  Quantity is in dBx16 or 1 unit 
    represents 1/16 of a dB
 */
typedef int32 lte_l1_gsm_rssi_t;

/*! GSM quarter symbol value */
typedef uint32 lte_l1_gsm_qs_type_t;

/*! Complete GSM ARFCN type which unambiguously identifies the GSM frequency
  by using a band indicator
  */
typedef struct
{
  /*! ARFCN number of the GSM cell */
  lte_l1_gsm_arfcn_t arfcn;
  /*! Band that the GSM cell resides on. */
  lte_l1_gsm_band_e band;

} lte_l1_gsm_complete_arfcn_s;


/*! @brief GSM frame number */
typedef struct
{
  /*! Tie breaking number used to simply handle wraparound cases.  Should
    be ignored in most cases and is used internally.
    */
  uint32 hyperframe:8;
  /*! The frame of the gfn */
  uint32 frame:24;
} lte_l1_gfn_s;

/*! @brief Quarter Symbol accurate GSM timing offset */
typedef struct
{
  /*! The GFN Frame/Superframe number */
  lte_l1_gfn_s gfn;
  /*! The quarter symbol offset in the frame */
  lte_l1_gsm_qs_type_t qs_offset;
} lte_l1_gsm_timing_s;

/*! @brief Transmission type enum */
typedef enum
{
  LTE_L1_TX_TYPE_NEW,
  LTE_L1_TX_TYPE_RETX,
  LTE_L1_TX_TYPE_DTX
} lte_l1_tx_type_e;

/*===========================================================================
                     Time Transfer related TYPES
===========================================================================*/
/*! WCDMA STMR structure */
typedef struct
{
  uint32 frame_count;
  uint32 chip_x8_offset;
} lte_l1_stmr_w_s;

/*! TDS STMR structure */
typedef struct
{
  uint16 subframe;
  uint16 cx8_offset;
} lte_l1_stmr_t_s;

/*! GSM STMR structure */
typedef struct
{
  uint32 frame;
  uint16 qs_offset;
  uint16 raw_13M;
  uint32 prescale; /* Deprecated */ 
} lte_l1_stmr_g_s;

/*! HDR STMR structure */
typedef struct
{
  uint32 cdma_cx8;
} lte_l1_stmr_h_s;

/*! CDMA 1x STMR structure */
typedef struct
{
  uint32 cdma_cx8;
} lte_l1_stmr_c_s;

/*! LTE STMR structure */
typedef struct
{
  uint64 subframe_ref_time; /*!< RTC value */
  uint32 o_stmr;            /*!< OSTMR value */
  uint32 o_stmr_phase;      /*!< OSTMR phase */
  uint32 snapshot_offset;   /*!< Offset from SF boundary */  
  uint32 sfn;               /*!< LTE system frame number */
  uint32 subfn;             /*!< LTE subframe number */
  uint32 ustmr;             /*!< XO count */
} lte_l1_stmr_l_s;

/*! Union of other RAT STMR types */
typedef union
{
  lte_l1_stmr_w_s w; /* WCDMA STMR */
  lte_l1_stmr_t_s t; /* TDS STMR */
  lte_l1_stmr_g_s g; /* GSM STMR */
  lte_l1_stmr_c_s c; /* CDMA 1x STMR */
  lte_l1_stmr_h_s h; /* HDR STMR */
} lte_l1_stmr_u;

/*! ORAT STMR snapshot */
typedef struct
{
  lte_rat_e rat;
  lte_l1_stmr_u stmr;
} lte_l1_orat_stmr_snapshot_s;

/*! Snapshot of STMR values for LTE and another RAT */
typedef struct
{
  lte_l1_stmr_l_s l; /*!< LTE STMR value */
  lte_l1_orat_stmr_snapshot_s orat; /*!< ORAT STMR value */
} lte_l1_lte_stmr_snapshot_s;

/*! @brief  CDRX Collision parameters (configured via EFS)
*/
typedef struct
{
  /*!<  CDRX Number of RLF Threshold configured via EFS (to be used in ML1)
             Count to indicate TRM Rejects during CDRX*/
  uint8 num_rlf_thresh;

  /*!<  CDRX Number of Reestabishment Threshold configured via EFS (to be used in RRC)
             Count to indicate Reestablishment attempts for 
               RLF due to TRM rejects during CDRX*/
  uint8 num_reest_thresh;
}lte_l1_cdrx_collision_params_s;


#endif /*  LTE_L1_TYPES_H */
