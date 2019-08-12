#ifndef RFDEVICE_TYPE_DEFS_H
#define RFDEVICE_TYPE_DEFS_H

/*!
   @file rfdevice_type_defs.h

   @brief
   It provides type definitions needed by the rfdevice_interface.

   @details
  This file contains the type definitions used by the rfdevice interface
  functions.
*/

/*===========================================================================

Copyright (c) 2008, 2010-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_type_defs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/15/15   smi     Expanded rfdevice_trx_phy_path_enum_type upto 8 chains
05/19/15   dps     Added support for power logging using rfdevice_wtr_config_bitmask
05/12/15   sd      Added enum to distinguish between PRx and Drx operation modes 
04/15/15   zg      Added support for 2 wtr voting
01/19/15   zg      Fixed a bit-overflowing bug in PA LUT mapping
11/25/14   vv      Added support for rfc based lna to system gain mapping
04/29/14   tks     Added support for 8 PA states
01/09/14   vv      Added new enum to enable config AG settings for TP130 PAs
09/18/13   vs      Added new enums and structure for PA and ASM register dybamic update
08/22/13  shb/sr   Added rxtx and phy_path enums for transceiver
03/16/12   lp      Featurize data types requiring mdsp interface
08/30/12    pv     Added new cmn type definitions for RFC based PA state  LUT mapping
08/17/12   shb     Deleted rfdevice_sbi_burst_cmd_type
04/01/12   lcl     Removed empty mdsp interface header file  
03/17/12   jfc     Remove deprecated mdsp_intf_v.h include
01/05/12   shb     Updated RSB A, B coeff to be unsigned
12/05/11   vb      Updated rfdevice_hdet_mdsp_config_data_type
10/12/11   sb      Enabled RSB correction  
09/14/11   vrb     Remove unused rfdevice_rsb_coef_type
09/08/11   vrb     Get RSB and Rx spec inversion requirement from Device Intf
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
05/04/11   shb     Moved TX LUT API arg capsule typedefs to
                   rfdevice_1x_interface.h as it needs CDMA specific headers
04/20/11   jhe     Add hdet mdsp config typedef
03/11/11   sty     DOxygen updates 
03/10/11   sty     Changed member name of rfdevice_lut_data_type
08/24/10   aro/sty Added seperate votes for Rx0 and Rx1 in 
                   rfdevice_mode_enum_type
05/10/10   dw      Added rfdevice_multi_carrier_type
04/01/10   adk     Merged branch scmm_mdm9k_merge_final
03/10/10   wen     Added Physical ID 
02/10/10   vb      Added struct rtr8600_lut_data_type 
01/26/10   xw      Added rfdevice_mode_enum_type 
01/26/10   sar     Lint fixes.
01/22/10   sar     Fixed compiler error.
01/21/10   xw      Lint error fixes  
01/19/10   aro     Added SBI Bus Config Feature
12/02/09   dyc     Featurize for C2K only build.
11/10/09   dbz     Moved device specifc typedefs into RTR library 
11/09/09   dbz     Changed to generic RTR naming convention
10/13/09   dbz     Added support for GNSS ELNA Mode 2/Device Mode overhaul
07/04/09   sr      changed the gsm band type 
06/26/09   dbz     Merged Gen8 engineering codebase into source
03/13/09   sr      removed the gsm specific types
03/10/09   gy      Updated GNSS enum definitions.
10/14/08   sr      Initial version.

============================================================================*/

#include "rfa_variation.h"
#include "rfcom.h" 

#define RFDEVICE_RX_PATH_NUM  5
#define RFDEVICE_TX_PATH_NUM  1

/* Needed for RFC based PA LUT Mapping */
#define RFDEVICE_PA_LUT_MAPPING_VALID 0x10000000
#define RFDEVICE_PA_LUT_MAPPING_INVALID 0x00000000
#define RFDEVICE_PA_STATE_0_BSHFT 0
#define RFDEVICE_PA_STATE_1_BSHFT 3
#define RFDEVICE_PA_STATE_2_BSHFT 6
#define RFDEVICE_PA_STATE_3_BSHFT 9
#define RFDEVICE_PA_STATE_4_BSHFT 12
#define RFDEVICE_PA_STATE_5_BSHFT 15
#define RFDEVICE_PA_STATE_6_BSHFT 18
#define RFDEVICE_PA_STATE_7_BSHFT 21

#define RFDEVICE_MAX_PA_STATES 8
#define RFDEVICE_BITS_PER_PA_STATE 3


/* Needed for RFC based LNA gain to system gain mapping*/
#define RFDEVICE_RX_GAIN_STATE_MAPPING_INVALID 0x00000000
#define RFDEVICE_RX_TOTAL_GAIN_STATES_BSHIFT 0
#define RFDEVICE_RX_GAIN_STATE_0_BSHFT 4
#define RFDEVICE_RX_GAIN_STATE_1_BSHFT 8
#define RFDEVICE_RX_GAIN_STATE_2_BSHFT 12
#define RFDEVICE_RX_GAIN_STATE_3_BSHFT 16
#define RFDEVICE_RX_GAIN_STATE_4_BSHFT 20
#define RFDEVICE_RX_GAIN_STATE_5_BSHFT 24

#define RFDEVICE_MAX_RX_GAIN_STATES 6
#define RFDEVICE_BITS_PER_RX_GAIN_STATE 4

#ifdef FEATURE_GSM
#include "mdsp_intf.h"
#include "wfw_mod_intf.h"
#endif /* FEATURE_GSM*/

/*!
  Enum to distinguish between RX and TX devices
*/
typedef enum
{
  RFDEVICE_RX,
  RFDEVICE_TX,
  RFDEVICE_RXTX_MAX,
  RFDEVICE_RXTX_INVALID = RFDEVICE_RXTX_MAX,
} rfdevice_rxtx_enum_type;

/*!
   Enumerates physical paths in a transceiver
*/
typedef enum
{
  RFDEVICE_TRX_PATH_0 = 0,
  RFDEVICE_TRX_PATH_1,
  RFDEVICE_TRX_PATH_2,
  RFDEVICE_TRX_PATH_3,
  RFDEVICE_TRX_PATH_4,
  RFDEVICE_TRX_PATH_5,
  RFDEVICE_TRX_PATH_6,
  RFDEVICE_TRX_PATH_7,
  RFDEVICE_TRX_MAX_PATHS,
  RFDEVICE_TRX_INVALID_PATH = RFDEVICE_TRX_MAX_PATHS, 
} rfdevice_trx_phy_path_enum_type;

/*!
   Enumerates physical vco paths in a receiver
*/
typedef enum
{
  WTR_RX_VCO0 = 0,
  WTR_RX_VCO1 = 1,
  WTR_RX_MAX_VCO_PATH ,
  WTR_RX_INVALID_VCO_PATH = WTR_RX_MAX_VCO_PATH
} rfdevice_rx_vco_path_enum_type;

/*! 
  Enum to identify if modem chain associated with a given
  Rx device is used for PRx/DRx operation 
*/
typedef enum
{
  RFDEVICE_PRX_OPER_MODE,
  RFDEVICE_DRX_OPER_MODE,
  RFDEVICE_DRX2_OPER_MODE,
  RFDEVICE_DRX3_OPER_MODE,
  RFDEVICE_MAX_RX_OPER_MODE,
  RFDEVICE_INVALID_OPER_MODE = RFDEVICE_MAX_RX_OPER_MODE
} rfdevice_rx_mode_type;

typedef struct
{
  uint8 i;
  uint8 q;
} rfdevice_iq_dac_type;

typedef struct
{
  rfcom_multi_carrier_hspa_id_type num_chan;
  rfcom_wcdma_band_type band;
} rfdevice_multi_carrier_type;

typedef struct
{
  rfcom_wcdma_band_type band;
  uint8 lut_idx;
  uint8 pa_range;
} rfdevice_lut_idx_pa_range_type;

typedef struct
{
  uint16 arfcn;
  int8 extcal;
  rfcom_gsm_band_type band;
} rfdevice_tx_burst_cal_type;

/* RF Tx PDM limit type */
typedef struct 
{
  uint16 min_pdm;
  uint16 max_pdm;
}  rftx_pdm_limit_type;

typedef struct
{
  uint8 data;
  uint8 addr;
  uint8 slave_id;
  uint8 ctrl_fifo;
} rfrx_non_timed_sbi_type;


typedef enum
{
  RTR_GNSS_FUNCTION1 = 0,
  RTR_GNSS_FUNCTION2,
  RTR_GNSS_FUNCTION3,
  RTR_GNSS_FUNCTION4,
  RTR_GNSS_FUNCTION5,
  RTR_GNSS_FUNCTION6,
  RTR_GNSS_FUNCTION7,
  RTR_GNSS_FUNCTION_MAX
} rtr_gnss_function_type;

/* This type specifies the available baseband gain step settings.  Note:
 * not all GPS devices support this configurability 
 */
 
typedef enum
{
  GNSS_BB_GAIN_STEP_MINUS_15_DB = 0,
  GNSS_BB_GAIN_STEP_MINUS_12_DB,
  GNSS_BB_GAIN_STEP_MINUS_9_DB,
  GNSS_BB_GAIN_STEP_MINUS_6_DB,
  GNSS_BB_GAIN_STEP_MINUS_3_DB,
  GNSS_BB_GAIN_STEP_0_DB,
  GNSS_BB_GAIN_STEP_3_DB,
  GNSS_BB_GAIN_STEP_6_DB
} gnss_bb_gain_step_type;

/* This type specifies the availabe dc dac mode settings.  */

typedef enum
{
  GNSS_DC_DAC_MODE_0_P_5X = 0,   /* 0.5x */
  GNSS_DC_DAC_MODE_1_P_0X,       /* 1.0x */
  GNSS_DC_DAC_MODE_1_P_5X,       /* 1.5x */
  GNSS_DC_DAC_MODE_2_P_0X        /* 2.0x */
} gnss_dc_dac_mode_type;

typedef enum
{
  GNSS_LINEARITY_MODE_LOW = 0,   /* GPS Receiver in low linearity mode */
  GNSS_LINEARITY_MODE_HIGH,      /* GPS Receiver in high linearity mode */
  GNSS_LINEARITY_MODE_MAX
} gnss_linearity_mode_type;

typedef enum
{
  GNSS_60p53_KHZ,               /* Intermediate Frequency set to 60.53 khz */
  GNSS_0p53_KHZ,                /* Intermediate Frequency set to 0.53 hz */
  GNSS_LIF_MAX
} gnss_set_IF_frequency_type;

typedef enum
{
  DEVICE_GNSS_MODE_1=1,
  DEVICE_GNSS_MODE_2,
  DEVICE_GNSS_MODE_3,
  DEVICE_GNSS_MODE_4,
  DEVICE_GNSS_MODE_5,
  DEVICE_GNSS_MODE_6,
  DEVICE_GNSS_MODE_MAX,
} device_gnss_mode;

typedef enum
{
  RFDEVICE_PARKED_MODE,
  RFDEVICE_1XHDR_PATH0_MODE,
  RFDEVICE_1XHDR_PATH1_MODE,
  RFDEVICE_WCDMA_MODE,
  RFDEVICE_GSM_MODE,
  RFDEVICE_GNSS_MODE,
  RFDEVICE_LTE_MODE,
  RFDEVICE_MODE_MAX
} rfdevice_mode_enum_type;


typedef struct
{
    boolean rx_spec_inv;
    int32 rsb_a_coef;
    int32 rsb_b_coef;
}rfdevice_rsb_coef_type;

/*============================================================================*/
/*!
  @name RF Device Bus Information types

  @brief
  This section contains all definitions for SBI Bus Information Type
  corresponding to RF Devices available.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  SBI Bus information

  @details
  This structure holds the SBI bus numbers for RTR register banks and BBRx 
  register bank. Each bus might be assigned to different Register banks (Rx or
  Tx or other) depending on the Chipset. If the RF Card has more than one 
  chipset, then all the bus numbers must listed here.
 
  This structure is used commonly between all RF Chipstes; and the number of
  buses used by each Chipset might be different. Thus when variable of this 
  type is defined and populated, the bus number should be populated starting 
  from rtr_bus_a; and the order of populating the Bus numbers in this structure 
  should match the requirement from the init API of the RTR Driver.
*/
typedef struct
{

  uint8 tx_bus;
  /*!< SBI Bus 1 Mapping for Tx Register Bank. This is used for SBI Read/Write
  Function by RTR Driver */

  uint8 rx_bus;
  /*!< SBI Bus 2 Mapping for Rx Register Bank.This is used for SBI Read/Write
  Function by RTR Driver */

  uint8 phy_tx_bus;
  /*!< SBI Bus 1 Mapping for RTR8600 Tx Register Bank */

  uint8 phy_rx_bus;
  /*!< SBI Bus 2 Mapping for RTR8600 Rx Register Bank */

} rfdevice_bus_info_type;


/*! \} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This structure holds the SBI information for HDET enabling
  and HDET conversation from device driver.
*/
#ifdef FEATURE_GSM
typedef struct
{
  uint8  num_sbi[WFW_TX_AGC_HDET_NUM_SBI_GROUP]; 
  uint32 sbi_data[WFW_TX_AGC_HDET_NUM_SBI_GROUP][WFW_TX_AGC_HDET_MAX_SBI_PER_GROUP]; 
}rfdevice_hdet_mdsp_config_data_type;
#endif /* FEATURE_GSM*/

typedef struct
{
  int32 rsb_phase_x1000;
  int32 rsb_gain_x1000;
}rfdevice_rsb_offsets_x1000_type;

typedef enum
{
   RFDEVICE_PA_ON,
   RFDEVICE_PA_OFF,
   RFDEVICE_PA_RANGE,
   RFDEVICE_PA_BIAS,
   RFDEVICE_PA_CONFIG,
   RFDEVICE_PA_MAX_CMD
}rfdevice_pa_mod_reg_enum_type;

typedef enum
{
   RFDEVICE_ASM_ON,
   RFDEVICE_ASM_OFF,
   RFDEVICE_ASM_MAX_REG
}rfdevice_asm_mod_reg_enum_type;

typedef struct
{
   uint8 max_num_reg;
   uint8 *reg_addr;
   int16 *reg_data;
}rfdevice_mod_reg_info_type;

typedef struct
{
   rfcom_mode_enum_type mode;
   rfcom_band_type_u band;
   uint8 gain_range;
   rfdevice_pa_mod_reg_enum_type reg_type;
}rfdevice_pa_mod_cfg_type;

typedef struct
{
   rfcom_mode_enum_type mode;
   rfcom_band_type_u band;
   rfdevice_rxtx_enum_type txrx;
   rfdevice_asm_mod_reg_enum_type reg_type;
}rfdevice_asm_mod_cfg_type;

typedef enum
{
  RFDEVICE_FBRX_PATH_DEDICATED_IQ,
  RFDEVICE_FBRX_PATH_INTERNAL_ADC,
  RFDEVICE_FBRX_PATH_DRX_IQ,
  RFDEVICE_FBRX_PATH_INVALID
}rfdevice_fbrx_path_enum_type;

/* Needed for device power logging */
typedef enum
{
  RFDEVICE_WTR_CFG_TECH_CDMA,
  RFDEVICE_WTR_CFG_TECH_WCDMA,
  RFDEVICE_WTR_CFG_TECH_TDSCDMA,
  RFDEVICE_WTR_CFG_TECH_GSM,
  RFDEVICE_WTR_CFG_TECH_LTEFDD,
  RFDEVICE_WTR_CFG_TECH_LTETDD,
  RFDEVICE_WTR_CFG_TECH_INVALID
}rfdevice_wtr_cfg_tech_enum_type;

typedef enum
{
  RFDEVICE_WTR_CFG_MODE_PRXLGY1,
  RFDEVICE_WTR_CFG_MODE_DRXLGY1,
  RFDEVICE_WTR_CFG_MODE_PRXLGY2,
  RFDEVICE_WTR_CFG_MODE_DRXLGY2,
  RFDEVICE_WTR_CFG_MODE_PRXSCA,
  RFDEVICE_WTR_CFG_MODE_DRXSCA,
  RFDEVICE_WTR_CFG_MODE_TX,
  RFDEVICE_WTR_CFG_MODE_INVALID
}rfdevice_wtr_cfg_mode_enum_type;

typedef enum 
{
  RFDEVICE_WTR_CFG_LUT_HP, 
  RFDEVICE_WTR_CFG_LUT_MP, 
  RFDEVICE_WTR_CFG_LUT_LP,
  RFDEVICE_WTR_CFG_NO_LUT, 
  RFDEVICE_WTR_CFG_LUT_INVALID 
}rfdevice_wtr_cfg_lut_pwr_enum_type;

typedef enum
{
  RFDEVICE_WTR_CFG_CHIP_WTR3925,
  RFDEVICE_WTR_CFG_CHIP_WTR4905,
  RFDEVICE_WTR_CFG_CHIP_INVALID
}rfdevice_wtr_cfg_chip_enum_type;

#define RFDEVICE_WTR_CFG_TECH_BSHFT 0
#define RFDEVICE_WTR_CFG_TECH_BIT_SIZE 3

#define RFDEVICE_WTR_CFG_BAND_BSHFT 3
#define RFDEVICE_WTR_CFG_BAND_BIT_SIZE 6

#define RFDEVICE_WTR_CFG_BW_BSHFT 9
#define RFDEVICE_WTR_CFG_BW_BIT_SIZE 4

#define RFDEVICE_WTR_CFG_MODE_BSHFT 13
#define RFDEVICE_WTR_CFG_MODE_BIT_SIZE 3

#define RFDEVICE_WTR_CFG_LUT_BSHFT 16
#define RFDEVICE_WTR_CFG_LUT_BIT_SIZE 2

#define RFDEVICE_WTR_CFG_CHIP_BSHFT 18
#define RFDEVICE_WTR_CFG_CHIP_BIT_SIZE 2

typedef uint32 rfdevice_wtr_config_bitmask_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Callback function provided by common core manager 
*/
typedef boolean (*handle_pwr_cb_type)
(
  /* Pointer to the input data if there is any */
  void* data_ptr
);

#endif /* RFDEVICE_TYPE_DEFS_H */
