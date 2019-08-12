/*!
  @file rfdevice_lte_type_defs.h

  @brief
  Provides type definitions needed by the rfdevice_interface.
 
  @details
  This file contains the type definitions used by the rfdevice interface
  functions.

*/

/*============================================================================== 
   
  Copyright (c) 2009 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
   
==============================================================================*/ 

/*==============================================================================
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_lte_type_defs.h#1 $ 
   
when       who     what, where, why 
--------   ---     -------------------------------------------------------------
02/24/15   smi     Added structure rfdevice_lte_lif_type
01/19/16   ry      Updated the rfdevice_lte_get_rx_path_info_type API
09/25/15   npi     Interface to check if band support on a device is over ICI
09/15/15   smi     Added WTR2965 support
07/27/15   bm      Renamed the structure rfdevice_lte_single_rb_data_type
                   Also added default_4fmod_script member to this structure
05/19/15   bm      Added support for single_rb_data_type structure
03/11/14   aks     Support for threshold based notches
12/16/14   aks     Updated exit and get_tune_script APIs to accept an array 
                   through which they can enforce retune of an active Rx chain
12/04/14   dps     Added support for NV input to RxRSB calibration
11/04/14   aks     Updates to LTE CB API for Spur Mitigation
08/26/14   aks     Added support for Scouting API to indicate if retune of an active cell
                   is required when a new cell is added   
08/01/14   ndb     Added support for extended EARFCN for LTE
07/07/14   aks     Updated tune_to_chan to accept Call Back API from tech driver 
                   for LTE CA Spur Mitigation
4/29/14    xsh     Modify the fbrx rsb cal API to ensure calibrated rsb cal can pass back
04/25/14   jf      Add FBRX and LDO timing
03/20/14   aks     Added support for Double Sided Notches
03/20/14   aks     Removed unused LTE Rx timing enums
03/12/14   npi     Check for Rx intraband CA support
02/02/14   aks     Increased RFDEVICE_LTE_MAX_NOTCHES from 3 to 6
11/25/13    pv     Support for ODD Algo for ET.
10/25/13   aka     LTE FB path RSB cal
09/26/13   kab     Fix for using stg_is_bad_fuse value from actual STG involved in RSB Cal
09/18/13   php     Pick LO Div ratio based Type 1 Spur present
08/20/13   aka     Added FB path STG APIs
07/24/13   gvn     RSB Cal updates on online mode, iqmc update and IRAT
07/19/13   php     Add TxLO Cal support to get info from ag
07/03/13   php     Add EFS support to save and load dcoc value
06/26/13   gvn     Write RSB Cal params to NV and other RSB cal updates
06/12/13   shb     Added API to get ET delay compensation
05/03/13   gvn     Added STG support for RSB Cal
04/16/13   shb     Added API to return if tx band uses saw or sawless port
04/12/13   php     Split Tx ON and Tx EN Low timings
03/26/13   vss     Add support for send intraband ca status to rx tune function 
03/08/13   shb     Implemented tune_to_default_port_fp for ET FB capture support 
                   on SV cards
03/04/13   php     Use new ENUMs to get timing from WTR ag
02/20/13   php     Added support for Tx EN and Rx EN events
01/10/13   pl      Added API definition to set RX Common
12/15/12   pv      Intra band CA flag for RF Cal.
12/13/12   pv      Rx disable API update with band param.
12/13/12   pv      Support for new structure for LNA Gainword.
11/30/12   pv      Support for Gain Offsets in CA mode.
11/30/12   pv      Support for RSB Coeff for FTM mode.
11/30/12   pv      Support for RSB Coeff API.
11/29/12   av      Using proper featurization and removing featurization 
                    that is not needed
11/20/12   gvn     Featurize LTE for Triton 
11/15/12   pv      Support for Off chip LNA GW selection.
11/15/12   pv      Get Rx Gain table setting API.
11/13/12   php     Change ENUM naming for Tx timing parameters
10/31/12   vss     Added func_ptr for retrieving rx and tx timing info to Rx and Tx
                   func_ptr table 
10/03/12   shb     Added lte_rx_build_meas_script_func_ptr_type to RX 
                   function pointer table
09/13/12   vss     Add support to pass bus info for get_tx_char_tbl 
09/12/12   pv      Added parameter to tx_set_band_port
09/12/12   vss     Add support to pass bus info for get_rx_gain_state_tbl  
08/16/12   shb     Added rfdevice_lte_trx_device_type
08/16/12   shb     Renamed rfdevice_enum_type to rfdevice_id_enum_type
08/15/12   shb     Fixed compiler errors
08/15/12   shb     Moved all LTE data structs from rfdevice_lte_interface.h to 
                   rfdevice_lte_type_defs.h 
06/18/12   aca     LTE DAC Cal update- LTE specific Iref programming
05/22/12   aca     AFC clean up- unified definition between FW, ML1 and RFSW 
05/11/12   php      Add TX_ENABLE/DISABLE script generation 
04/21/12   vss     Added band fields to the get_rx_gain tbl and set_rx_gain data types
04/04/12   aca     AFC cleanup- moved out MDSP function from driver to MC
02/17/12   vss     Add HDET Therm V2 support to the device
02/09/12   aca     Tx PLL error correction
01/16/12   sbm     Added rfdev_dvt_data for npler dvt script support
11/16/11   tnt     Support for query PDET compensation values for different BW settings 
11/15/11   cri     Support for TX PLL adjust
11/08/11   tnt     Expand rfdevice_lte_rx_rsb_offset_type to include LTE BW para
11/02/11   whc     Update LTE RTR Autogen RSB and MSM layer to interpret them
09/25/11   vb      Support for DAC cal using different Irefs
09/14/11   can     Merge MDM9K tips to NikeL.
09/07/11   jyu     Added support to query TxDAC through RFDEVICE intf 
09/06/11   aca     Removed Spectral Inversion hardcoding
08/29/11   aca     Removed hardcoded RX Gain address, number of gain states
08/26/11   whc     Cal V3 LTE optimize HDET reading  
08/15/11   pl      merge to Genesis PLF 118
06/29/11   cri     Move HDET struct from device to mdsp
06/28/11   can     LTE FTM TXAGC mode support.
02/10/11   vss     Remove band members from tx_char_tbl structures.
10/23/10   jyu     Add L2L interfreq support 
07/01/10   pl      Remove FW dependency from public MDSP API
07/01/10   mkv     Adding support LTE LPM HDET Mode.   
06/29/10   pl      Use MDSP layer to abstract FW from RF IRAT module
06/03/10   qma     Warning fixes
05/26/10   qma     Added RSB Cal support
05/19/10   qma     LTE IM2 DAC write support 
05/17/10   pl      Define structure for LTE IRAT support   
05/03/10   can     HDET support optimization.
04/04/09    zt     Initial version.
==============================================================================*/ 

#ifndef RFDEVICE_LTE_TYPE_DEFS_H
#define RFDEVICE_LTE_TYPE_DEFS_H

/*============================================================================== 
 
                                 INCLUDE FILES
 
==============================================================================*/
#include "rfcom.h" 
#include "rfdevice_meas_type_defs.h"
#include "rfdevice_cmn_type_defs.h"
#include "rf_buffer_intf.h"

#define RFDEVICE_LTE_MAX_LNA_STATES 6

#if defined FEATURE_LTE || !defined FEATURE_TRITON_MODEM
#include "intf_ul_common_type.h"
#include "rflte_ext_mc.h"

/*============================================================================== 
 
                               MACROS AND TYPEDEFS 
 
==============================================================================*/

/*----------------------------------------------------------------------------*/
typedef enum 
{
  LTE_HDET_HPM = 0,
  LTE_HDET_LPM = 1,
  LTE_HDET_MAX
}rfdevice_lte_hdet_power_mode_type;
/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 bw; 
  rfcom_lte_band_type band; 
  uint32 lo_freq; 
  uint64 *adc_out_freq_ptr; 
}
rfdevice_lte_adc_clk_data_type;


/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 pa_state;
  uint8 char_entry;
} 
rfdevice_lte_set_tx_char_tbl_entry_type;


/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 pa_state;
  uint32 tx_char_tbl_reg_address;
  const uint32 *tx_char_tbl_reg_values;
  uint8 bus;
} 
rfdevice_lte_get_tx_char_tbl_type;


/*----------------------------------------------------------------------------*/

/*! 
  @brief
  Structure to store the nominal gain state and the default gain offsets for
  each gain state. Offsets are calculated from the nominal gain
 
*/

typedef struct
{
  /*! Number of Gain States */
  uint8 num_gain_states;

  /*! Nominal Gain- Gain for Gain State G0 */
  uint16 nominal_g0_gain;

  /*! Offsets for each Gain State calculated from G0 */
  int16* default_rx_gain_offset;
}
rfdevice_lte_get_rx_gain_settings_type;
                                               
/*----------------------------------------------------------------------------*/
#define RFDEVICE_LTE_MAX_NOTCHES RFDEVICE_MAX_NOTCHES

typedef struct
{
  uint64 other_lo_rx_hz;
  uint64 other_lo_tx_hz;
  rfcom_lte_bw_type other_bw;
  rfcom_lte_band_type other_band;
  boolean is_type1_spur_present;

  /* Number of notches */
  uint8 band_home_spur_count;
  uint8 band_other_spur_count;

  /* Notch Frequency list */
  int64 band_home_notch_freq_list_hz[RFDEVICE_LTE_MAX_NOTCHES];
  int64 band_other_notch_freq_list_hz[RFDEVICE_LTE_MAX_NOTCHES];

  /* Notch depth list */
  uint8 band_home_notch_depth_list[RFDEVICE_LTE_MAX_NOTCHES];
  uint8 band_other_notch_depth_list[RFDEVICE_LTE_MAX_NOTCHES];

  /* Notch Threshold list */
  uint8 band_home_notch_threshold_list[RFDEVICE_LTE_MAX_NOTCHES];
  uint8 band_other_notch_threshold_list[RFDEVICE_LTE_MAX_NOTCHES];

  uint32 home_lo_rx_hz;
}
rfdevice_lte_spur_mitigation_type;

/*----------------------------------------------------------------------------*/

typedef struct
{
   /*HoRxD frequency offset*/
   uint32 lif_offset_kz;
}
rfdevice_lte_lif_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 pa_state;
  uint32 gain_value;
} 
rfdevice_lte_set_tx_gain_value_type;


/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_band_type band; 
  rfcom_lte_bw_type bw;
  uint8 rx_gain_state;
  boolean intra_band_CA_status;
} 
rfdevice_lte_set_rx_gain_state_type;


/*----------------------------------------------------------------------------*/

/*! 
  @brief
  Structure to indicate which chain needs to be retuned when a new Cell
  is added or an existing cell is removed 
 
  */
typedef struct
{
  /*! Flag to indicate PRx Chain needs to be retuned */
  boolean prx_retune_reqd;

  /*! Flag to indicate DRx Chain needs to be retuned */
  boolean drx_retune_reqd;

  /*! Flag to indicate Tx Chain needs to be retuned */
  boolean tx_retune_reqd;
} 
rfdevice_lte_retune_reqd_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  ENUM used to indicate the event for which the WTR is queried
  if retune of an active chain is reuired
 
  */
typedef enum
{
  /*! An active Rx Chain is disabled */
  RFDEVICE_LTE_DISABLE_RX_CHAIN,

  /*! An active Tx Chain is disabled */
  RFDEVICE_LTE_DISABLE_TX_CHAIN,

  /*! Rx Chain is being tuned */
  RFDEVICE_LTE_TUNE_RX_CHAIN,

  /*! Tx Chain is being tuned */
  RFDEVICE_LTE_TUNE_TX_CHAIN,

  /* Invalid Event */
  RFDEVICE_LTE_INVALID_RETUNE_EVENT
} 
rfdevice_lte_retune_reqd_event_type;

/*----------------------------------------------------------------------------*/

/*! 
  @brief
  Callback function pointer from LTE tech layer. Would be invoked after
  spur detection during tune to execute all spur mitigation requirements
  e.g. programming notches/ restoring gain state
 
  @param retune_done
  Flag to indicate Rx Chain was retuned
 
  @param gain_settings
  Structure with updated gain offsets if chain was retuned
 
  @return
  Success/ Failure Flag
 
  */

typedef boolean (*rfdevice_lte_spur_mitigation_cb) 
(
  /*! Indicates which Cell Rx/Tx Chain belongs to */
  rflte_mc_cell_idx_type cell_idx,

  /*! RFM Device used to indicate Rx/Tx Chain */
  rfm_device_enum_type rfm_device,

  /*! Band to which Rx/Tx Chain was retuned to (if retune is required) */
  rfcom_lte_band_type rf_band,

  /*! BW to which Rx/Tx Chain was retuned to (if retune is required) */
  rfcom_lte_bw_type rfcom_bw,

  /*! Frequency to which Rx/Tx Chain was retuned to */
  uint32 freq_khz,

  /*! Intra Band CA FLag used to tune Rx Chain */
  boolean intra_band_CA_status,

  /*! Flag to indicate Rx/Tx Chain was retuned */
  boolean retune_done,

  /*! Enum to indicate why CB was invoked  */
  rfdevice_lte_retune_reqd_event_type retune_event,

  /*! Updated Gain Offsets if retune is required */
  rfdevice_lte_get_rx_gain_settings_type* gain_settings,

  /*! Script to be populated */
  rfdevice_meas_script_data_type* script_data_ptr
);

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_band_type band;
  rfcom_lte_bw_type bw;
  uint8 num_sbi_addresses;
  uint8 num_gain_states;
  const uint8 *rx_gain_register_address_tbl;
  const uint8 *rx_gain_state_tbl;
  uint8 bus;
} 
rfdevice_lte_get_rx_gain_state_tbl_type;

/* New Interface to be used going fwd from Dime */
/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_band_type band;
  rfcom_lte_bw_type bw;
  uint8 num_gain_states;
  uint8 rx_gain_register_address_tbl;
  uint8 rx_gain_state_tbl[RFDEVICE_LTE_MAX_LNA_STATES];
  uint8 bus;
  boolean validity_status;
  boolean intra_band_CA_status;
} 
rfdevice_lte_get_rx_gain_state_tbl_settings_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
    rfdevice_lte_hdet_power_mode_type hdet_power_mode;
    uint8 bw;
} rfdevice_lte_hdet_enable_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
    rfdevice_lte_hdet_power_mode_type hdet_power_mode;
    uint16 hdet_read;
} rfdevice_lte_hdet_settings_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfdevice_dac_iref_val iref_val;
  rfcom_lte_band_type band; 
} rfdevice_lte_dac_cal_iref_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 i_val;
  uint8 q_val;
} 
rfdevice_lte_im2_dac_codes_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_bw_type bw;
  rfcom_lte_band_type band;
  boolean spectral_inversion;
  int32 rsb_a;
  int32 rsb_b;
  boolean in_cal_state;
  boolean intraband_CA_status;
  uint16 therm_value; /* Raw therm value*/
  uint32 rx_freq_hz;
} 
rfdevice_lte_rx_rsb_offset_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_band_type band;
  rfcom_lte_bw_type bw;
  uint32 freq;
  rfdevice_meas_script_type *sbi_script_tbl;
  boolean gen_return_script;
}
rfdevice_lte_get_irat_meas_tbl_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_band_type from_band;
  rfcom_lte_bw_type from_bw;
  uint32 from_freq;
  rfcom_lte_band_type to_band;
  rfcom_lte_bw_type to_bw;
  uint32 to_freq;
  rfdevice_meas_script_type *sbi_script_tbl;
  boolean gen_return_script;
}
rfdevice_lte_get_l2l_tbl_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_band_type band;
  uint32 spectral_inversion;  
} 
rfdevice_lte_tx_spectral_inversion_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_band_type band;
  uint16 dac_setpoint;
} 
rfdevice_lte_get_tx_dac_type;
/*----------------------------------------------------------------------------*/

typedef struct
{
  uint8 tx_pll_int_msbs_addr;
  uint8 tx_pll_int_lsbs_addr;
  uint8 tx_pll_frac_msbs_addr;
  uint8 tx_pll_frac_mids_addr;
  uint8 tx_pll_frac_lsbs_addr;

  uint8 tx_pll_int_msbs_val;
  uint8 tx_pll_int_lsbs_val;
  uint8 tx_pll_frac_msbs_val;
  uint8 tx_pll_frac_mids_val;
  uint8 tx_pll_frac_lsbs_val;

  uint8 tx_pll_tuned_flag;
}
rfdevice_lte_tx_pll_data_type;


typedef struct
{
  uint8 bus;
  uint8 addr;
  uint8 val;
} rfdevice_lte_script_type;

typedef struct
{
  uint16 txen_ltefdd;
  uint16 rfon_ltefdd;
  uint16 txen_ltetdd;
  uint16 rfon_ltetdd;
  rfcom_lte_band_type band;
  uint8 num_writes;
  rfdevice_lte_script_type script[10]; 
} rfdevice_lte_tx_enable_disable_data_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_band_type band;
  rfcom_lte_earfcn_type tx_channel;
  uint8 tx_rgi;
  uint8 search_resolution;
} 
rfdevice_lte_get_txlo_cal_param_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  int8 pdet_4kHz_filter_comp_val_db10;
  int8 pdet_11kHz_filter_comp_val_db10;
  int8 pdet_38kHz_filter_comp_val_db10;
} rfdevice_lte_pdet_filter_comp_type;

/*!
  @brief
  if enable then use the give index in this strucure with npler table

  @details
  enable_index_override = enable/disable index override for npler table
  npler_index= index for the enpler table.
*/
typedef struct rfdev_dvt_data
{
 boolean enable_index_override;
 uint8 npler_index;
}rfdevice_dvt_data_declaration;

/*----------------------------------------------------------------------------*/
typedef struct
{
  /*! Frequency error seen by FW where +ve error means current freq > nominal freq */
  int32 rx_freq_error_in_hz;
  /*! Nominal Rx Channel */
  rfcom_lte_earfcn_type rf_chan_rx;
  /*! Nominal Tx Channel */
  rfcom_lte_earfcn_type rf_chan_tx;
  /* PLL tuned flag to alert FW */
  uint8 tx_pll_tuned_flag;
  /*! Script to correct the error */
  lte_LL1_ul_txpll_script_array_t* txpll_script_buf_ptr;
}
rfdevice_lte_tx_pll_script_data_type;
/*----------------------------------------------------------------------------*/

typedef enum
{
  RFDEVICE_LTE_HDET_START_SETTINGS,
  RFDEVICE_LTE_HDET_LO_PWR_HI_BW_SETTINGS,
  RFDEVICE_LTE_HDET_LO_PWR_MID_BW_SETTINGS,
  RFDEVICE_LTE_HDET_LO_PWR_LO_BW_SETTINGS,
  RFDEVICE_LTE_HDET_HI_PWR_HI_BW_SETTINGS,
  RFDEVICE_LTE_HDET_HI_PWR_MID_BW_SETTINGS,
  RFDEVICE_LTE_HDET_HI_PWR_LO_BW_SETTINGS,
  RFDEVICE_LTE_HDET_CONVERSION_SETTINGS,
  RFDEVICE_LTE_HDET_READ_SETTINGS,
  RFDEVICE_LTE_THERM_INIT,
  RFDEVICE_LTE_THERM_CONV_DISABLE,
  RFDEVICE_LTE_THERM_READ,
  RFDEVICE_LTE_HDET_LO_PWR_LOW_BW_ODD_DELAY_A_SETTINGS,
  RFDEVICE_LTE_HDET_HI_PWR_LOW_BW_ODD_DELAY_A_SETTINGS,
  RFDEVICE_LTE_HDET_LO_PWR_LOW_BW_ODD_DELAY_B_SETTINGS,
  RFDEVICE_LTE_HDET_HI_PWR_LOW_BW_ODD_DELAY_B_SETTINGS,
  RFDEVICE_LTE_HDET_READ_ODD_SETTINGS,
  RFDEVICE_LTE_HDET_DISABLE_ODD_SETTINGS,
  RFDEVICE_LTE_HDET_SETTINGS_NUM,
  RFDEVICE_LTE_HDET_SETTINGS_INVALID,
} rfdevice_lte_hdet_therm_settings_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 ssbi_buf_size;
  rfdevice_lte_hdet_therm_settings_type hdet_settings_type;
  rfdevice_ssbi_write_type* hdet_ssbi_buf; 
  uint16 hkadc_value;
}rfdevice_lte_hdet_therm_data_type;

typedef struct
{
  rfdevice_lte_hdet_therm_data_type lte_hdet_therm_settings;
  uint16 *pdet_result;
  uint16 script2_timing;
  uint16 script3_timing;
} rfdevice_lte_et_hdet_data_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 iref;
  rfcom_lte_band_type band;
} 
rfdevice_lte_program_iref_type;

/*----------------------------------------------------------------------------*/

typedef struct
{
  /*!  */
  union 
  {
    struct
    {
      uint8 addr0;
      uint8 addr1;
      uint8 addr2;
      uint8 addr3;
    };
   uint32 addrs;
  };
  /*! Nominal Rx Channel */
  uint8 mask_index;
  /*! Nominal Tx Channel */
  boolean enable_flag;
  /*! Script to correct the error */
  uint8 and_mask;
  uint8 or_mask;
}
rfdevice_lte_tx_low_gain_data_type;

/*----------------------------------------------------------------------------*/
typedef enum
{
 RFDEVICE_LTE_TIMING_TX_ON_UL_SF_AGC = 0,
 RFDEVICE_LTE_TIMING_TX_ON_UL_SF_TXPLL_AFC,
 RFDEVICE_LTE_TIMING_TX_ON_PRACH_OR_SRS_ONLY_AGC,
 RFDEVICE_LTE_TIMING_TX_CONT_UL_SF_AGC,
 RFDEVICE_LTE_TIMING_TX_CONT_UL_SF_TXPLL_AFC,
 RFDEVICE_LTE_TIMING_TX_CONT_POST_UE_SRS_TXPLL_AFC,
 RFDEVICE_LTE_TIMING_TX_CONT_POST_UE_SRS_AGC,
 RFDEVICE_LTE_TIMING_FDD_TX_TX_EN_HIGH,
 RFDEVICE_LTE_TIMING_FDD_TX_TX_ON_HIGH,
 RFDEVICE_LTE_TIMING_FDD_TX_TX_ON_TX_EN_LOW, /* Deprecated ENUM - Do not Use */
 RFDEVICE_LTE_TIMING_TDD_TX_TX_EN_TX_ON_HIGH,
 RFDEVICE_LTE_TIMING_TDD_TX_TX_EN_TX_ON_LOW,
 RFDEVICE_LTE_TIMING_FDD_TX_TX_ON_LOW, 
 RFDEVICE_LTE_TIMING_FDD_TX_TX_EN_LOW, 
 RFDEVICE_LTE_TIMING_FBRX_LDO_EN_OFFSET_TO_RGI,
 RFDEVICE_LTE_TIMING_TX_OFF_UL_SF_RGI0,
 RFDEVICE_LTE_TIMING_TX_CELL_SRS_UL_SF_RGI0,
 RFDEVICE_LTE_TIMING_TX_MAX_NUM,
 RFDEVICE_LTE_TIMING_TX_INVALID = RFDEVICE_LTE_TIMING_TX_MAX_NUM
}
rfdevice_lte_tx_timing_enum_type;

/*----------------------------------------------------------------------------*/
typedef enum
{
 RFDEVICE_LTE_TIMING_RX_RX_ON_DL_SF_AGC,
 RFDEVICE_LTE_TIMING_RX_CONT_DL_SF_AGC,
 RFDEVICE_LTE_TIMING_TDD_RX_RX_ON_HIGH,
 RFDEVICE_LTE_TIMING_TDD_RX_RX_ON_LOW,
 RFDEVICE_LTE_TIMING_TDD_RX_RX_EN_HIGH,
 RFDEVICE_LTE_TIMING_TDD_RX_RX_EN_LOW,
 RFDEVICE_LTE_TIMING_FDD_RX_RX_ON_HIGH,
 RFDEVICE_LTE_TIMING_FDD_RX_RX_ON_LOW,
 RFDEVICE_LTE_TIMING_RX_MAX_NUM,
 RFDEVICE_LTE_TIMING_RX_INVALID = RFDEVICE_LTE_TIMING_RX_MAX_NUM
}
rfdevice_lte_rx_timing_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold intra CA capability info
*/
typedef struct
{
  /*! Rx Port split CA capability indicator */
  boolean split_ca_supported;
  /*! Rx Port maximum BW supported */
  rfcom_lte_bw_type max_bw_supported;  
} 
rfdevice_lte_get_rx_intra_ca_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold inter CA capability info
*/
typedef struct
{
  /*! CA capability over inter-chip interface indicator */
  boolean ici_supported;
  /*!RXAGC freeze indicator for the given band combo */
  boolean pcc_reconfig_required;
  /*! Rx Port maximum BW supported */
  rfcom_lte_bw_type max_bw_supported;
}
rfdevice_lte_get_rx_inter_ca_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold path info
*/
typedef struct
{
  /*! Is path over inter-chip interface? */
  boolean is_ici_path;  
}
rfdevice_lte_get_rx_path_info_type;

/*----------------------------------------------------------------------------*/



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Forward declaration of device structure types to use in function pointers
  definitions
*/
typedef struct rfdev_lte_rx rfdevice_lte_rx_type;
typedef struct rfdev_lte_tx rfdevice_lte_tx_type;

typedef struct
{
  boolean write_to_hw_flag;
  rf_buffer_intf* script;
}rfdevice_lte_script_data_type;

/*!
   @brief
   It places the RF Device to parked state.

   @details
   The RTR device is initialized with values independent of Mode.
   This function must be called at power up and at anytime the
   device needs to be placed in its parked state.

   @param 
   band The LTE band to be initialized to

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver

   @retval None.
   
   @pre
   SBI clock regime must be enabled.
   
   @post
   RTR will be in its parked state.

   @todo 
   This might need to be merged to the common rf_device_interface.h
   as it is common for all Modes.
*/
typedef void (*lte_rx_init_func_ptr_type)
                     ( const rfdevice_lte_rx_type* rx_device,
                       rfcom_lte_band_type band,
                       rfdevice_lte_script_data_type* script_data_ptr,
                       rfm_device_enum_type rfm_device );

typedef void (*lte_tx_init_func_ptr_type)
                     ( const rfdevice_lte_tx_type* tx_device, 
                       rfcom_lte_band_type band,
                       rfdevice_lte_script_data_type* script_data_ptr,
                       rfm_device_enum_type rfm_device  );



/*!
   @brief
   It configure the RF Device for the requested mode of operation.

   @details
   It configure the RF device chain for the specified mode or technology.

   @param path
   RF Path of the device to be initialized.
   

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver

   @retval None.
   
   @pre
   SBI clock regime must be enabled.
   
   @post
   RTR will be in its default state.
*/
typedef void (*lte_rx_set_mode_func_ptr_type)
                            ( const rfdevice_lte_rx_type* rx_device, 
                              rfcom_lte_band_type rf_band,
                              rfdevice_lte_script_data_type* script_data_ptr,
                              rfm_device_enum_type rfm_device  );
typedef void (*lte_tx_set_mode_func_ptr_type)
                            ( const rfdevice_lte_tx_type* tx_device, 
                              rfcom_lte_band_type rf_band,
                              rfdevice_lte_script_data_type* script_data_ptr,
                              rfm_device_enum_type rfm_device  );


/*!
   @brief
   Programs the RTR for Rx or Tx mode of operation in a specific band.

   @details
   RTR device is configured to receive or transmit in a specific band.
   Places the RTR into either Rx mode if we are enabling the Rx.
   OR into Tx mode if we are enabling the Tx.
   The concept of device driver is device oriented not chip oriented. 
   So there will be rxdevice and txdevice not chip device instances. 
   In RTR device, we have RX and TX device independently. 

   @param path : RF Path of the device to be enabled.

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver
   
   @retval None
   
   @pre
   
   @post
   Rf Device will be placed either Rx or Tx or RxTx mode of operation
   depending on the use of RTR.
*/
typedef void (*lte_rx_config_band_func_ptr_type) 
                                  ( const rfdevice_lte_rx_type* rx_device,
                                    rfcom_lte_band_type rf_band,
                                    rfdevice_lte_script_data_type* script_data_ptr,
                                    rfm_device_enum_type rfm_device  );
/*!
   @brief
   Programs the RTR for Rx or Tx mode of operation in a specific band.

   @details
   RTR device is configured to receive or transmit in a specific band.
   Places the RTR into either Rx mode if we are enabling the Rx.
   OR into Tx mode if we are enabling the Tx.
   The concept of device driver is device oriented not chip oriented. 
   So there will be rxdevice and txdevice not chip device instances. 
   In RTR device, we have RX and TX device independently. 

   @param path : RF Path of the device to be enabled.

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver
   
   @retval None
   
   @pre
   
   @post
   Rf Device will be placed either Rx or Tx or RxTx mode of operation
   depending on the use of RTR.
*/
typedef void (*lte_tx_config_band_func_ptr_type) 
                                  ( const rfdevice_lte_tx_type* tx_device,
                                    rfcom_lte_band_type rf_band,
                                    rfdevice_lte_script_data_type* script_data_ptr,
                                    rfm_device_enum_type rfm_device  );

/*!
   @brief
   Program the PLL based on the band and frequency.

   @details
   Programs the RF synthesizer (PLL) as per the band and channel.

   @param path : RF Path of the device for which PLL to be programmed.
   @param band : Specifies the RF band the PLL is to be loaded for
   @param chan : Specifies the RF uplink channel within the specified RF band

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver

   @retval None.
   
   @pre
   RF Device init(), enter_mode(), and config_band() must have been called 
   prior to calling this function.
   
   @post
   RF synthesizer will be tuned for the requested band and channle.
*/
typedef void (*lte_rx_tune_to_chan_func_ptr_type)
                                  ( const rfdevice_lte_rx_type* rx_device,
                                    rfcom_lte_band_type rf_band, 
                                    uint32 freq,
                                    rfdevice_lte_script_data_type* script_data_ptr,
                                    rfm_device_enum_type rfm_device,
                                    boolean intra_band_CA_status,
                                    rfdevice_lte_spur_mitigation_type *spur_mitigation_params );

/*!
   @brief
   Program the PLL based on the band and frequency.

   @details
   Programs the RF synthesizer (PLL) as per the band and channel.

   @param path : RF Path of the device for which PLL to be programmed.
   @param band : Specifies the RF band the PLL is to be loaded for
   @param chan : Specifies the RF uplink channel within the specified RF band

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver
   
   @retval None.
   
   @pre
   RF Device init(), enter_mode(), and config_band() must have been called 
   prior to calling this function.
   
   @post
   RF synthesizer will be tuned for the requested band and channle.
*/
typedef void (*lte_tx_tune_to_chan_func_ptr_type)
                                  ( const rfdevice_lte_tx_type* tx_device,
                                    rfcom_lte_band_type rf_band, 
                                    uint32 freq, 
                                    int32 rx_freq_error,
                                    rfdevice_dpd_enum_type dpd_state,
                                    rfdevice_lte_script_data_type* script_data_ptr, boolean ftm_mode,
                                    rfm_device_enum_type rfm_device );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query API to indicate if a particular LTE band uses SAW or SAWless port on
  the device

  @param tx_device
  "this" pointer to tx device whose port info is queried
 
  @param rfm_device
  Logical device for which port info is queried
 
  @param band
  LTE rfcom band whose port info is needed
 
  @retval RFDEVICE_TX_INVALID_PORT
  Either the band is invalid or not configured in this device yet
 
  @retval RFDEVICE_TX_SAWLESS_PORT
  Band does not use Saw port in device
 
  @retval RFDEVICE_TX_SAW_PORT
  Band uses port with saw on this device
*/
typedef rfdevice_tx_saw_port_type (*lte_tx_get_port_func_ptr_type)
(
  const rfdevice_lte_tx_type* tx_device,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type band
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the ET delay compensation to be applied for a given LTE band and
  bandwidth

  @details
  ET delay compensation value depends on RC error, process, TX port being used
  and the bandwidth of LTE operation.
 
  This device API will return the delay comp based on these inputs
 
  @param tx_device
  "this" pointer to LTE Tx device of operation.
 
  @param rfm_device
  Logical rfm device of operation. A place holder - not used currently
 
  @param band
  LTE band of operation.
 
  @param bw
  LTE bandwidth being used. Delay comp depends on the bandwidth
 
  @param delay_comp_x10ns
  Delay compensation to be applied in 1/10th of a ns unit
 
  @retval TRUE
  Indicates the API was successful and a correct delay comp value has been
  returned
 
  @retval FALSE
  Indicates API failed due to bad parameters
*/
typedef boolean (*lte_tx_get_et_delay_comp_func_ptr_type)
(
  const rfdevice_lte_tx_type* tx_device,
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type band,
  rfcom_lte_bw_type bw,
  int16* delay_comp_x10ns
);

/*!
   @brief
   Disables the Rx or Tx mode of operation.

   @details
   Puts PRX, DRX, or TX to sleep.
   Reset the RX_ENABLE or TX_ENABLE bit for PRX and TX.
   
   @param path : RF Path of the device to be disabled.

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver

   @retval None
   
   @pre
   RF device init() must be called.
   
   @post
   Rf Device will be placed either Rx or Tx or Sleep mode of operation
   depending on the use of RTR.

   @todo
   Add support for DRX. 
   For implemenation details, we may need further investigations. 
   For WCDMA, the enable bit is set in the last comments in pll tune. 
   As we talked to Christian, we will have config_before_enable( ) 
   and config_after_enable( ). We can update the RTR device interface 
   and driver when we are clear about this implemenation and have the 
   SSBI spreadsheet with columns supporting config before enable 
   and after enable.
*/

typedef void (*lte_rx_disable_func_ptr_type) ( const rfdevice_lte_rx_type* rx_device,
                                               rfdevice_lte_script_data_type* script_data_ptr,
                                               rfm_device_enum_type rfm_device,
                                               rfcom_lte_band_type rf_band );

/*!
   @brief
   Disables the Rx or Tx mode of operation.

   @details
   Puts PRX, DRX, or TX to sleep.
   Reset the RX_ENABLE or TX_ENABLE bit for PRX and TX.
   
   @param path : RF Path of the device to be disabled.

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver
   
   @retval None
   
   @pre
   RF device init() must be called.
   
   @post
   Rf Device will be placed either Rx or Tx or Sleep mode of operation
   depending on the use of RTR.

   @todo
   Add support for DRX. 
   For implemenation details, we may need further investigations. 
   For WCDMA, the enable bit is set in the last comments in pll tune. 
   As we talked to Christian, we will have config_before_enable( ) 
   and config_after_enable( ). We can update the RTR device interface 
   and driver when we are clear about this implemenation and have the 
   SSBI spreadsheet with columns supporting config before enable 
   and after enable.
*/

typedef void (*lte_tx_disable_func_ptr_type) ( const rfdevice_lte_tx_type* tx_device,
                                               rfdevice_lte_script_data_type* script_data_ptr,
                                               rfm_device_enum_type rfm_device  );

/*!
   @brief
   Provides generic get and set functionality for the device specific
   parameters.

   @details
   It provides IOCTL kind of interface to get and set device specific
   parameters.

   @param path : RF Path of the device for which cmd to be performed.
   @param cmd  : Specifies the command to performed by the device.
   @param data : Input and/or Output parameter for the command.

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver

   @retval ret_val : generic return value which depends on the 'cmd' parameter.
   
   @pre
   It depends on the 'cmd' parameter. For some commands device specific init() 
   function need to be called. For some there may not be any pre-condition prior 
   to using this function.
   
   @post
   
*/
typedef int32 (*lte_rx_cmd_dispatch_func_ptr_type) 
                                        ( const rfdevice_lte_rx_type* rx_device,
                                          int cmd, 
                                          void *data,
                                          rfdevice_lte_script_data_type* script_data_ptr,
                                          rfm_device_enum_type rfm_device  );

/*!
   @brief
   Provides generic get and set functionality for the device specific
   parameters.

   @details
   It provides IOCTL kind of interface to get and set device specific
   parameters.

   @param path : RF Path of the device for which cmd to be performed.
   @param cmd  : Specifies the command to performed by the device.
   @param data : Input and/or Output parameter for the command.

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver

   
   @retval ret_val : generic return value which depends on the 'cmd' parameter.
   
   @pre
   It depends on the 'cmd' parameter. For some commands device specific init() 
   function need to be called. For some there may not be any pre-condition prior 
   to using this function.
   
   @post
   
*/
typedef int32 (*lte_tx_cmd_dispatch_func_ptr_type) 
                                        ( const rfdevice_lte_tx_type* tx_device,
                                          int cmd, 
                                          void *data, 
                                          rfdevice_lte_script_data_type* script_data_ptr,
                                          rfm_device_enum_type rfm_device  );

/*!
   @brief
   Set the BW for TX or RX of the RTR device.

   @details
   Config the RTR device for TX/RX BW settings. LTE only.

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver
   
*/
typedef void (*lte_rx_set_bw_func_ptr_type) 
                                ( const rfdevice_lte_rx_type* rx_device,
                                 rfcom_lte_bw_type bw,
                                 rfdevice_lte_script_data_type* script_data_ptr,
                                 rfm_device_enum_type rfm_device );

/*!
   @brief
   Set the BW for TX or RX of the RTR device.

   @details
   Config the RTR device for TX/RX BW settings. LTE only.

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver
   
*/
typedef void (*lte_tx_set_bw_func_ptr_type) 
                                (const rfdevice_lte_tx_type* tx_device,
                                 rfcom_lte_bw_type bw,
                                 rfdevice_lte_script_data_type* script_data_ptr,
                                 rfm_device_enum_type rfm_device );

/*!
   @brief
   This function wakes up the RTR from sleep.

   @details
   Wakeup PRx or DRx from sleep mode

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver
   
   @retval None
   
   @pre
   RF device init() must be called.

   @post
   Rf Device will not be in sleep mode.
*/

typedef void (*lte_rx_wakeup_func_ptr_type) ( const rfdevice_lte_rx_type* rx_device,
                                              rfdevice_lte_script_data_type* script_data_ptr,
                                              rfm_device_enum_type rfm_device  );


/*!
   @brief
   This function exits RTR from LTE mode

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver

   @details
   Exit LTE mode
   
   @retval None
   
   @post
   RF Device will not be in LTE mode.
*/

typedef void (*lte_rx_exit_func_ptr_type) ( const rfdevice_lte_rx_type* rx_device, 
                                            rfdevice_lte_script_data_type* script_data_ptr,
                                            rfm_device_enum_type rfm_device  );
/*!
   @brief
   This function sets the band and RTR/WTR port mapping.

   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver
 
   @param band
   Band for which the port is being set. It needs to be less than RFCOM_NUM_LTE_BANDS
 
   @param rx_band_port
   Actual port no. for the instance and the band. 

   @details
   sets the band and RTR/WTR port mapping.
   
   @retval None
   
   @post
   RF Device will be configured with correct band-port mapping for rx.
*/

typedef boolean (*lte_rx_set_port_func_ptr_type) ( const rfdevice_lte_rx_type* rx_device, 
                                                rfcom_lte_band_type band, uint8 rx_band_port,
                                                   rfm_device_enum_type rfm_device);
/*!
   @brief
   This function sets the band and RTR/WTR port mapping.

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver
 
   @param band
   Band for which the port is being set. It needs to be less than RFCOM_NUM_LTE_BANDS
 
   @param rx_band_port
   Actual port no. for the instance and the band. 

    @param tx_band_pa_lut_map
    Provides the PA state to LUT mapping per band.

   @details
   sets the band and RTR/WTR port mapping.
   
   @retval None
   
   @post
   RF Device will be configured with correct band-port mapping for tx.
*/ 
typedef boolean (*lte_tx_set_port_func_ptr_type) ( const rfdevice_lte_tx_type* tx_device, 
                                                   rfcom_lte_band_type band, uint8 tx_band_port,
                                                   uint32 tx_band_pa_lut_map ,
                                                   rfm_device_enum_type rfm_device );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Rx band specific information to device
  
  @details
  Does a version check to ensure it is valid. Then band_port enum to be
  used for Rx on given band is stored in device
  
  @param *rx_device
  "this" pointer to RX device that needs to be configured
 
  @param band
  LTE band whose data is being pushed
 
  @param data_array
  Array of int32 containing device Rx data for band
 
  @param array_size
  Size of array of int32 data to be pushed to device
*/
typedef boolean (*lte_rx_set_band_data_func_ptr_type) 
( 
  const rfdevice_lte_rx_type* rx_device,
  rfcom_lte_band_type band,
  int32* data_array,
  uint8 array_size,
  rfm_device_enum_type rfm_device
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Tx band specific information to device
  
  @details
  Does a version check to ensure it is valid. Then band_port enum to be
  used for Tx on given band is stored in device
  
  @param *tx_device
  "this" pointer to TX device that needs to be configured
 
  @param band
  LTE band whose data is being pushed
 
  @param data_array
  Array of int32 containing device Tx data for band
 
  @param array_size
  Size of array of int32 data to be pushed to device
*/
typedef boolean (*lte_tx_set_band_data_func_ptr_type) 
( 
  const rfdevice_lte_tx_type* tx_device,
  rfcom_lte_band_type band,
  int32* data_array,
  uint8 array_size,
  rfm_device_enum_type rfm_device
);

/*!
   @brief
   This function exits RTR from LTE mode

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver

   @details
   Exit LTE mode
   
   @retval None
   
   @post
   RF Device will not be in LTE mode.
*/

typedef void (*lte_tx_exit_func_ptr_type) ( const rfdevice_lte_tx_type* tx_device,
                                            rfdevice_lte_script_data_type* script_data_ptr,
                                            rfm_device_enum_type rfm_device  );

/*!
   @brief
   This function provides the BB filter response array from auto-gen for different BW.  

   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver

   @details
   Provide access to BB filter response array to upper layer.
   
   @retval
   *pointer to the array.
   
   @post
   LTE tech will use this value to compensate the Linearizer for a specific BW.
*/

typedef const int8* (*lte_tx_get_bbf_func_ptr_type) ( const rfdevice_lte_tx_type* tx_device,
                                                rfcom_lte_band_type current_lte_band,
                                                rfcom_lte_bw_type current_lte_bw,
                                                rfm_device_enum_type rfm_device);
/*!
   @brief
   This function provides the Tx timing info based on timing parameter  
 
   @details
   All timings are in Ts and relative to the LTE subframe boundary. If timing
   is negative, it means that it is before the sub frame boundary. If it is
   positive, it means that is after the sub frame boundary.
 
   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver
 
   @param rfm_device
   rfm device for which timing is required
 
   @param current_lte_band
   band for which timing is required
 
   @param tx_timing_type
   timing parameter to indicate what timing to retrieve
 
   @param timing_result
   store timing in the result; output parameter; this timing is
   valid only if status is TRUE
 
   @return
   Returns true if timing is found and no errors; false otherwise

*/
typedef boolean (*lte_tx_get_tx_timing_info_func_ptr_type) ( const rfdevice_lte_tx_type* tx_device,
                                                rfm_device_enum_type rfm_device,
                                                rfcom_lte_band_type current_lte_band,
                                                rfdevice_lte_tx_timing_enum_type tx_timing_type,
                                                int16 *timing_result);
/*!
   @brief
   This function provides the Rx timing info based on timing parameter  
 
   @details
   All timings are in Ts and relative to the LTE subframe boundary. If timing
   is negative, it means that it is before the sub frame boundary. If it is
   positive, it means that is after the sub frame boundary.
 
   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver
 
   @param rfm_device
   rfm device for which timing is required
 
   @param current_lte_band
   band for which timing is required
 
   @param rx_timing_type
   timing parameter to indicate what timing to retrieve
 
   @param timing_result
   store timing in the result; output parameter; this timing is
   valid only if status is TRUE
 
   @return
   Returns true if timing is found and no errors; false otherwise

*/
typedef boolean (*lte_rx_get_rx_timing_info_func_ptr_type) ( const rfdevice_lte_rx_type* rx_device,
                                                rfm_device_enum_type rfm_device,
                                                rfcom_lte_band_type current_lte_band,
                                                rfdevice_lte_rx_timing_enum_type rx_timing_type,
                                                int16 *timing_result);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Build LTE measurement script in rf_buffer_intf format

  @details
  Similar to RFDEVICE_GET_MEAS_START_SCRIPTS but builds script in
  rf_buffer_intf format. Checks various irat parameters and builds start or
  cleanup script based on arguments.
 
  This API builds the script only for the rx_device provided. So if PRx and DRx
  scripts need to be built this API needs to be called twice - first with PRx
  device and then with DRx device pointer
 
  @param rx_device
  "this" pointer to LTE RX device whose measurement script needs to be built.
 
  @param band
  LTE band whose script is requested
 
  @param freq
  RX tune frequence
 
  @param bw
  LTE receive bandwidth
 
  @param buf
  Pointer to rf_buffer_intf object that will be populated with LTE meas script
*/
typedef boolean (*lte_rx_build_meas_script_func_ptr_type)
(
   const rfdevice_lte_rx_type* rx_device,
   rfcom_lte_band_type band,
   uint32 freq,
   rfcom_lte_bw_type bw,
   rf_buffer_intf* buf,
   rfm_device_enum_type rfm_device
);

/*!

   @details
   All timings are in Ts and relative to the LTE subframe boundary. If timing
   is negative, it means that it is before the sub frame boundary. If it is
   positive, it means that is after the sub frame boundary.
 
   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver
 
   @param lna_gain_state_tbl
   This is the RF Device structure which store the info for band, bw, bus, addr,
   num of gain states available, settings. This structure is populated and sent back
   for MC/Core to program the SMEM.
   
   @param rfm_device
   rfm device for which timing is required
 
   @return
   Returns true if timing is found and no errors; false otherwise

*/
typedef void (*lte_rx_get_gain_table_func_ptr_type) ( const rfdevice_lte_rx_type* rx_device,
                                                      rfdevice_lte_get_rx_gain_state_tbl_settings_type *lna_gain_state_tbl,
                                                      rfm_device_enum_type rfm_device 
                                                    );

/*!
   @brief
   This function provides the Rx timing info based on timing parameter  
 
   @details
   All timings are in Ts and relative to the LTE subframe boundary. If timing
   is negative, it means that it is before the sub frame boundary. If it is
   positive, it means that is after the sub frame boundary.
 
   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver
 
   @param lna_gain_state_tbl
   This is the RF Device structure which store the info for band, bw, bus, addr,
   num of gain states available, settings. This structure is populated and sent back
   for MC/Core to program the SMEM.
 
   @param rfm_device
   rfm device for which timing is required
 
   @return
   Returns true if timing is found and no errors; false otherwise

*/
typedef boolean (*lte_rx_set_gain_table_func_ptr_type) ( const rfdevice_lte_rx_type* rx_device,
                                                         rfdevice_lte_set_rx_gain_state_type *lna_gain_state_tbl,
                                                         rfm_device_enum_type rfm_device 
                                                         );

/*!
   @brief
   This function calculates rsb NV params
 
   @details
   
 
   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver
 
   @param *rsb_data
   This is the RF Device structure which store the info for band, bw, spectral inversion,
   rsb_a and rsb_b coefficient. This structure is populated and sent back
   for MC/Core to program the MSM.
 
   @param rfm_device
   rfm device for which rsb data is desired to be calculated
 
   @return
   Returns true if timing is found and no errors; false otherwise

*/
typedef boolean (*lte_rx_get_rsb_coeff_func_ptr_type) ( const rfdevice_lte_rx_type* rx_device,
                                                        rfdevice_lte_rx_rsb_offset_type *rsb_data,
                                                        rfm_device_enum_type rfm_device,
                                                        rfdevice_lte_spur_mitigation_type *spur_mitigation_params
                                                       );

/*!
   @brief
   Program the common LTE setting

   @details
   Programs the common, non-band specific LTE settings


   @param *rx_device
   "this" pointer to Rx Device pertaining to the current instance of device 
   driver
   @param rf_band : Specifies the RF band 
   @param script_data_ptr : buffer point of which the setting script is to be returned
   @param rfm_device_enum_type : the logical device requested

 
   @retval None.
   
   @pre
   
   @post
*/
typedef void (*lte_rx_set_common_func_ptr_type)
                                  ( const rfdevice_lte_rx_type* rx_device,
                                    rfcom_lte_band_type rf_band, 
                                    rfdevice_lte_script_data_type* script_data_ptr,
                                    rfm_device_enum_type rfm_device  );


/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *rx_device
  
  @param band
 
  @param *data
 
  @param script_data_ptr
 
  @return
  NULL 
*/
typedef boolean (*lte_rx_config_stg_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rfdevice_lte_rx_type *rx_device,
  rfcom_lte_band_type band, 
  uint8 *stg_is_bad_fuse,
  void *data,
  rfdevice_lte_script_data_type* script_data_ptr
); 

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *rx_device
  
  @param band
 
  @param *channel
 
  @param *data_valid_status
 
  @param script_data_ptr
 
  @return
  NULL 
*/
typedef boolean (*lte_rx_get_stg_chan_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rfdevice_lte_rx_type *rx_device,
  rfcom_lte_band_type band, 
  rfcom_lte_earfcn_type *channel,
  boolean *data_valid_status, 
  rfdevice_lte_script_data_type* script_data_ptr
); 

/*----------------------------------------------------------------------------*/
/*!
  @brief: Disables stg
 
  @param rfm_dev
 
  @param *rx_device
 
  @param script_data_ptr
*/

typedef boolean (*lte_rx_disable_stg_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfdevice_lte_rx_type *rx_device,
  rfdevice_lte_script_data_type* script_data_ptr
);


/*----------------------------------------------------------------------------*/
/*!
  @brief: Get rsb nv params
 
  @param rfm_dev: RFM Device for which RSB params are desired
 
  @param *rx_device: "this" pointer to Rx Device pertaining to the current instance of device 
   driver
 
  @param rf_band: Band in use
 
  @param i_sq: I square calculated from RSB Func#1
 
  @param q_sq: Q square calculated from RSB Func#1
 
  @param i_q: I_Q calculated from RSB Func#1
 
  @param therm_data: Raw Therm read data
 
  @param *sin_theta_final: Parameter that will be returned by this function
 
  @param *cos_theta_final: Parameter that will be returned by this function
 
  @param *gain_inv_final: Parameter that will be returned by this function
*/
typedef boolean (*lte_rx_calc_rsb_nv_params_func_ptr_type)
( 
  rfm_device_enum_type rfm_dev,
  rfdevice_lte_rx_type *rx_device,
  rfcom_lte_band_type rf_band, 
  rfcom_lte_bw_type rx_bw,
  uint64   i_sq,
  uint64   q_sq,
  int64    i_q,
  uint16   therm_data, 
  int16*   sin_theta_final, 
  int16*   cos_theta_final, 
  int16*   gain_inv_final,
  uint8 stg_is_bad_fuse

);

/*----------------------------------------------------------------------------*/
/*!
  @brief: Get fbrx rsb nv params
 
  @param rfm_dev: RFM Device for which RSB params are desired
 
  @param *tx_device: "this" pointer to Tx Device pertaining to the current instance of device 
   driver, FB path is assocuated with that Tx device.
 
  @param rf_band: Band in use
 
   @param *rsb_data
  pointer to rsb data type, input I/Q, output the final mismathed sin_theta, cos_theta and gain_inv
  
*/
typedef boolean (*lte_tx_calc_rsb_nv_params_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfdevice_lte_tx_type *tx_device,
  rfcom_lte_band_type rf_band,
  rfcom_lte_bw_type tx_bw,
  rfdevice_rsb_cal_data_type *rsb_data
);


/*!
   @brief
   This function calculates rsb NV params
 
   @details
  
   @param *tx_device
   "this" pointer to Tx Device pertaining to the current instance of device 
   driver
 
   @param *rsb_data
   This is the RF Device structure which store the info for band, bw, spectral inversion,
   rsb_a and rsb_b coefficient. This structure is populated and sent back
   for MC/Core to program the MSM.
 
   @param rfm_device
   rfm device for which rsb data is desired to be calculated
 
   @return
   Returns true if timing is found and no errors; false otherwise

*/
typedef boolean (*lte_tx_get_fbrx_rsb_coeff_func_ptr_type) ( const rfdevice_lte_tx_type* tx_device,
                                                        rfdevice_lte_rx_rsb_offset_type *rsb_data,
                                                        rfm_device_enum_type rfm_device);

/*----------------------------------------------------------------------------*/
/*!
   @brief
  Helper function to save TxLO cal data for band on a particular device path
  to lte instance and EFS

  @details
  Used by TxLO leakage cal API to save calibration results. Data is saved in instance and
  entire structure is copied to efs file path initialized at bootup by RFC
 
  @param tx_device
  this pointer to tx device whose data is to be extracted 
 
  @param band
  LTE band whose data is being saved
 
  @param dcoc_value
  This is IQ value for which we have the smallest TxLO leakage
 
  @return
  Flag indicating success or failure of EFS write
*/

typedef boolean (*lte_tx_save_txlo_dcoc_value_func_ptr_type)
(
  rfdevice_lte_tx_type *tx_device,
  rfcom_lte_band_type band,
  uint32 dcoc_value
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get DCOC value for the band and device
   
  @param tx_device
  this pointer to tx device whose data is to be extracted 
 
  @param band
  enum variable which indicates the current band
 
  @param *dcoc_value
  This is the pointer to the IQ value for which we have the smallest TxLO leakage
 
  @param *nv_active
  This is the pointer to flag indicating whether this EFS is active
  
  @return status
  Return TRUE if returned data is valid else return FALSE 
*/
typedef boolean (*lte_tx_get_txlo_dcoc_value_func_ptr_type)
(
  rfdevice_lte_tx_type *tx_device,
  rfcom_lte_band_type band,
  uint32 *dcoc_value,
  boolean *nv_active
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *tx_device
  
  @param band
 
  @param *data
 
  @param script_data_ptr
 
  @return
  NULL 
*/
typedef boolean (*lte_tx_config_stg_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rfdevice_lte_tx_type *tx_device,
  rfcom_lte_band_type band, 
  uint8 *stg_is_bad_fuse,  
  void *data,
  rfdevice_lte_script_data_type* script_data_ptr
); 


/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *tx_device
  
  @param band
 
  @param *channel
 
  @param *data_valid_status
 
  @param script_data_ptr
 
  @return
  NULL 
*/
typedef boolean (*lte_tx_get_stg_chan_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rfdevice_lte_tx_type *tx_device,
  rfcom_lte_band_type band, 
  rfcom_lte_earfcn_type *channel,
  boolean *data_valid_status, 
  rfdevice_lte_script_data_type* script_data_ptr
); 

/*----------------------------------------------------------------------------*/
/*!
   @brief
   LTE RF Device Rx related interface function pointers.

   @details
   A RF Device independent interface will be provided to Main Control, Core and 
   Feature modules through these functions pointers. These function pointers
   will be initialized by the specific RF device whenever there is change in
   mode or band. This is for LTE RX for both FDD and TDD.
*/
typedef struct 
{ 
  lte_rx_init_func_ptr_type  init_fp;               /*!< to place the RTR RX into Parked state */
  lte_rx_set_mode_func_ptr_type set_mode_fp;        /* configure the RTR for the mode */
  lte_rx_config_band_func_ptr_type config_band_fp;  /*!< configure the RTR for a specific Rx band */
  lte_rx_tune_to_chan_func_ptr_type tune_to_chan_fp;/*!< Program the RX PLL for specific  
                                                    band and chan. This will be used
                                                    for both FDD and TDD. In LTE TDD, 
                                                    rx tune will map to RX or TX PLL
                                                    inside RTR driver, which is 
                                                    transparent to MC*/
  lte_rx_disable_func_ptr_type disable_fp;          /*!< disable Rx mode */
  lte_rx_wakeup_func_ptr_type wakeup_fp;            /*!< Rx wakeup from sleep */ 
  lte_rx_set_bw_func_ptr_type set_bw_fp;            /*!< set Rx bandwidth */
  lte_rx_exit_func_ptr_type exit_fp;                /*!< Exit LTE mode */
  lte_rx_set_port_func_ptr_type set_port_fp;        /*!< set band-port mapping */
  lte_rx_set_band_data_func_ptr_type set_band_data_fp; /*!< Push band data to device */
  lte_rx_cmd_dispatch_func_ptr_type cmd_dispatch_fp;/*!< generic IOCTL kind of  
                                                    function */ 
  lte_rx_build_meas_script_func_ptr_type build_meas_script_fp; /*!< Builds LTE meas script in 
                                                                    rf_buffer_intf format */
  lte_rx_get_rx_timing_info_func_ptr_type get_rx_timing_info_fp; /*!< Gets rx timing info based on
                                                                      device, band, and timing parameter */
  lte_rx_get_gain_table_func_ptr_type get_rx_gain_table_fp; /*!< Gets rx gain state settings based on 
                                                                 device, band, and timing parameter  */
  lte_rx_get_rsb_coeff_func_ptr_type get_rx_rsb_coeff_fp; /*!< Gets Rx RSB Coefficients from RF device */
  lte_rx_set_gain_table_func_ptr_type set_rx_gain_table_fp; /*!< Gets rx timing info based on
                                                              device, band, and timing parameter */
  lte_rx_set_common_func_ptr_type set_rx_common_fp; /*!< set only LTE common table settings */ 
  lte_rx_get_stg_chan_func_ptr_type get_stg_chan_fp; /*!< get stg chan*/
  lte_rx_config_stg_func_ptr_type config_stg_fp; /*!< config stg settings*/
  lte_rx_disable_stg_func_ptr_type disable_stg_fp; /*!< disable stg*/
  lte_rx_calc_rsb_nv_params_func_ptr_type calc_rsb_nv_params_fp;  /*!< Used to get rsb NV params */
}rfdevice_lte_rx_func_tbl_type;


/*!
   @brief
   LTE RF Device Tx related interface function pointers.

   @details
   A RF Device independent interface will ve provided to Main Control, Core and 
   Feature modules through these functions pointers. These function pointers
   will be initialized by the specific RF device whenever there is change in
   mode or band. This is for LTE TX for both FDD and TDD.
*/
typedef struct 
{
  lte_tx_init_func_ptr_type  init_fp;
  /*!< to place the RTR TX into Parked state */

  lte_tx_set_mode_func_ptr_type set_mode_fp;        
  /*!< configure the RTR for the mode */

  lte_tx_config_band_func_ptr_type config_band_fp;  
  /*!< configure the RTR for a specific Tx band */

  lte_tx_tune_to_chan_func_ptr_type tune_to_chan_fp;
  /*!< Program the TX PLL for specific band and chan. In TDD, MC will 
    not call this function */

  lte_tx_disable_func_ptr_type disable_fp;          
  /*!< disable Tx mode */

  lte_tx_set_bw_func_ptr_type set_bw_fp;            
  /*!< set Tx bandwidth */

  lte_tx_set_port_func_ptr_type set_port_fp;        
  /*!< set band-port mapping */

  lte_tx_set_band_data_func_ptr_type set_band_data_fp; 
  /*!< Push band data to device */

  lte_tx_cmd_dispatch_func_ptr_type cmd_dispatch_fp;
  /*!< generic IOCTL kind of  function */

  lte_tx_get_bbf_func_ptr_type get_bbf_fp;          
  /*!< function to query the BBF response matrix from device */

  lte_tx_get_tx_timing_info_func_ptr_type get_tx_timing_info_fp; 
  /*!< function to get the timing info based on rfm device, 
    band and timing parameter type */

  lte_tx_tune_to_chan_func_ptr_type tune_to_default_port_fp; 
  /*!< tune tx device to any port. Will be used for Feedback path calibration 
    where port is a dont care */

  lte_tx_get_port_func_ptr_type get_port_type_fp;
  /*!< Returns enum indicating whether the TX port used for the band on the 
    device is saw or sawless */

  lte_tx_get_et_delay_comp_func_ptr_type get_et_delay_comp_fp;
  /*!< Returns the ET delay compensation to be applied for a given LTE band and
  bandwidth */

  lte_tx_save_txlo_dcoc_value_func_ptr_type save_txlo_dcoc_value_fp;
  /*!< Saves dcoc value with least TxLO leakage into EFS file */

  lte_tx_get_txlo_dcoc_value_func_ptr_type get_txlo_dcoc_value_fp;
  /*!< Gets the dcoc value with least TxLO leakage from EFS file */

  lte_tx_get_stg_chan_func_ptr_type get_stg_chan_fp; /*!< get stg chan*/

  lte_tx_config_stg_func_ptr_type config_stg_fp; /*!< config stg settings*/

  lte_tx_calc_rsb_nv_params_func_ptr_type calc_rsb_nv_params_fp;
  /*!< Used to calculate and write FBRx RSB params */

  lte_tx_get_fbrx_rsb_coeff_func_ptr_type get_fbrx_rsb_coeff_fp;
  /*!< Gets FBRx RSB Coefficients from RF device */

}rfdevice_lte_tx_func_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF Device Rx related interface function pointers and state variables are
  defined in this typedef

  @details
  A RF Device independent interface will ve provided to Main Control, Core and
  Feature modules through these functions pointers. These function pointers
  will be initialized by the specific RF device whenever there is change in
  mode or band.
*/
typedef struct rfdev_lte_rx
{
  rf_lock_data_type *critical_sect;
  
  const rfdevice_id_enum_type device_id;    /*!< RF device being used for rx  */

  const rfdevice_lte_rx_func_tbl_type* const func_tbl;

  void const *data; /*!< Void pointer to device data which can be device specific */
}rfdevice_lte_rx_declaration;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF Device Tx interface function pointers and state variables are defined in 
  this typedef

  @details
  A RF Device independent interface will ve provided to Main Control, Core and
  Feature modules through these functions pointers. These function pointers
  will be initialized by the specific RF device whenever there is change in
  mode or band.
*/
typedef struct rfdev_lte_tx
{
  rf_lock_data_type *critical_sect;

  const rfdevice_id_enum_type device_id;  /*!< RF device being used for rx*/

  const rfdevice_lte_tx_func_tbl_type* const func_tbl;

  void const *data; /*!< Void pointer to device data which can be device specific */
}rfdevice_lte_tx_declaration;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure contains pointers to the various LTE devices in a transceiver device

  @details
  RF transceivers contains 2 LTE Receive paths: prx_device used as a primary receiver and
  drx_device used for Diversity and 1 Transmitter. This structure puts 
  pointers to these LTE devices together for a particular single transceiver 
  device.

  A structure of this type is stored in the common instance's tech_instance pointer
*/
typedef struct
{
  /*! Primary LTE receive device (on physical path 0) of WTR1605 */
  rfdevice_lte_rx_type* prx_device;

  /*! Secondary LTE receive device (on physical path 1) of WTR1605 - used
  for Diversity and SHDR operation */
  rfdevice_lte_rx_type* drx_device;

  /*! LTE transmit device (on physical path 0) of WTR1605 */
  rfdevice_lte_tx_type* tx_device;
} rfdevice_lte_trx_device_type;

/*! 
  @brief
  Calibration data stored in NV item after RSB cal per band, path, and bw
*/
typedef struct
{
  /*! Indicates whether the NV item has been calibrated successfully or not */
  boolean nv_active;

  /*! Sine theta estimate at room temp */
  int16 sin_theta;

  /*! Cosine theta estimate at room temp */
  int16 cos_theta;

  /*! Gain inv estimate at room temp */
  int16 gain_inv;

  /*! LTE Bandwidth used for Rx RSB calibration */
  rfcom_lte_bw_type cal_bw;

}rfdevice_lte_rx_rsb_cal_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure with the following elements:
  - Flag to indicate if Rx/Tx Chain needs to be retuned with parameters specified
  - Band, BW and Freq (KHz) to which Rx Chain is to be tuned to
 */

typedef struct
{  
  /*! RF Band */
  rfcom_lte_band_type band;

  /*! Frequency in KHz */
  uint32 freq_khz;

  /*! Bandwidth */
  rfcom_lte_bw_type bw;

  /* Flag to indicate if Rx/Tx chain needs to be retuned 
  with band - bw - freq_khz specified */
  boolean retune_flag;
}
rfdevice_lte_chain_info_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to indicate:
  - If a given cell is active or not
  - State info( parameters used for tuning) for Rx/Tx Chains
 
  */
typedef struct
{
  /*! Parameters with which PRx Chain was tuned with */  
  rfdevice_lte_chain_info_type prx_info; 

  /*! Parameters with which DRx Chain was tuned with */  
  rfdevice_lte_chain_info_type drx_info; 

  /* Flag to indicate if cell is active or not */
  boolean active_carrier;
}
rfdevice_lte_carrier_info_type;

/*---------------------------------------------------------------------------*/
/*!
  Captures Transceiver scripts needed to be programmed for 4F mod improvements
  in LTE (not all the WTR devices might have such requirements)
*/
typedef struct
{
  /*! Boolean to let the caller know if the single RB writes are 
    needed for the current WTR*/
  boolean is_4fmod_writes_valid;
 
  /*! Reference RB location to choose one of the lhs or rhs 
    single RB config scripts */
 uint8 ref_rb_location;
 
  /*! for the config with number of RB below this, use
  the LHS and/or RHS scripts */
  uint8 num_rb_threshold;
 
  /*! WXE trigger writes when number of RB <= num_rb_threshold
   and if these RBs location is on the right of reference RB */
  rf_buffer_intf* wtr_4fmod_script_rhs_of_ref;
 
  /*! WXE trigger writes when number of RB <= num_rb_threshold
   and if these RBs location is on the left of reference RB */
  rf_buffer_intf* wtr_4fmod_script_lhs_of_ref;
 
  /*! WXE trigger writes when number of RB > num_rb_threshold */
  rf_buffer_intf* default_4fmod_script;
 
 
} rfdevice_lte_4fmod_data_type;


/*----------------------------------------------------------------------------*/

extern rfdevice_dvt_data_declaration default_dvt_data_device;


/*! 
   @brief
   lte_rxdevice : provides interface to the RF Rx Device Driver
   lte_txDevice : provides interface to the RF Tx Device Driver
 */
extern const rfdevice_lte_rx_type *lte_rxdevice[RF_PATH_MAX];
extern const rfdevice_lte_tx_type *lte_txdevice[RF_PATH_MAX];

#endif /*FEATURE_LTE*/
#endif /* RFDEVICE_LTE_TYPE_DEFS_H */
