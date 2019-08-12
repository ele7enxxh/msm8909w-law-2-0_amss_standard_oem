#ifndef RFC_CDMA_H
#define RFC_CDMA_H
/*!
  @file
  rfc_cdma.h

  @brief
  <b>This is the RFC interface exposed to the MC layer.</b> This module 
  contains all CDMA-specific implementations to support RFC, and is 
  platform independent. 

  @addtogroup RF_RFC_CDMA_MC_INTF
  @{

*/

/*==============================================================================

  Copyright (c) 2008 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 
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
  written permission of Qualcomm Technologies Incorporated.d.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rfc_cdma.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/12/14   spa     Change tx_nv_path to be per band per logical device
11/06/14   spa     Change rx_nv_path to be per band per logical device
09/18/14   rsr     Changes to return TX LUT index when querying TX LUT info.
08/25/14   spa     Add num_grfc pointer as arg to generate_tx_on_off_grfc_script 
07/17/14   spa     Added api rfc_cdma_update_alt_rx_path
05/06/14   sty     deleted unused rfc_cdma_get_tx_device_cfg_info
04/11/14   APU     Enabled AsDIV
04/08/14   yb      Added support for coupler
08/26/13   shb     Updates for new C++ transceiver device interface
07/18/13   zhw     Add AsDiv Support for setting GRFC switches
06/14/13   spa     Use CDMA wrapper object for antenna tuner
05/29/13   spa     Added delay param to APIs for getting GRFC scripts
05/02/13   vb      Tuner CA changes 
04/15/13   APU     Corrected a typo. 
03/21/13   yzw     Add device driver support to query RFFE device information {mfg_id, prd_id, rev_id} 
03/13/13   sn      Added XPT functions
02/05/13   bmg     Added antenna tuner pointer to CDMA RFC
01/15/13   cd      Add support for CDMA ASM aggregator functionality
01/03/13   aro     Interface to query slave modem chain
12/27/12   shb     Updates for different physical rx_device per band
12/19/12   zhw     CDMA HDET support 
12/11/12   hdz     Enabled timing support for rfc_cdma_generate_tx_on_off_grfc_script
12/06/12   APU     Replaced LM chains with Modem chains.
11/30/12   adk     Dime RFC updates to get hdet device instance
11/19/12   adk     Dime RFC updates to get tuner device instance
11/05/12   zhw     Support for PA On/Off GRFC script in RF On/Off event
10/29/12   zhw     Support for query device_config from RFC
10/23/12   zhw     Support for getting Tx Template ID based on RFC query
10/16/12   zhw     RF On/Off grfc scripting using overwritten rfc signal
10/02/12   spa     Added Tx device info to logical device params
10/01/12   sb      Removed GRFC engine references in Dime code
08/21/12   APU     Added support to get modem_chain from rfc_device_info
08/25/12   spa     Changed pa and asm to arrays in logical device params 
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/09/12   spa     Renamed grfc_buf_ptr to script_buf_ptr 
07/06/12   spa     grfc_script APIs now accept void pointer to script 
                   buffer, removed is_irat flag 
05/14/12   zw      Removed unused APIs
05/09/12   zw      Added GRFC scripting generation APIs
04/16/12   Saul    QFE. Fix strip/pack errors.
04/14/12   sty     Added grfc_script pointer to calls to RFC on enable/disable 
04/13/12   Saul    QFE. Renamed rfqfe_cdma_X to qfe_cdma_X
04/11/12   Saul    QFE. Initial support.
03/26/12   sty     Deleted redundant prototype rfc_cdma_rx_gpio_grfc_config
03/08/12   cd      API to obtain GRFC information from RFC
02/17/12   sty     Deleted rfc_cdma_get_rx_recommendations()
01/23/12   aro     Added RFC interface to query enhanced RF WU time
01/03/12   aro     Renamed the TxLM and RxLM chain parameter to make it
                   explicit that it is used only for FTM
01/03/12   aro     Added TxLM DAC to logical device parameter
01/03/12   aro     Added TxLM Chain specific to 1x and HDR in logical
                   device param
12/29/11   aro     Added RxLM ADC and 1x/HDR specific RxLM chain in logical
                   device param
11/29/11   sty     Added paired_device in rfc_cdma_logical_device_params_type
                   Added prototype for rfc_cdma_get_rx_recommendations()
09/12/11   sty     Added rfc_cdma_is_tunable_on_device
09/11/11   aro     Merged CDMA device interface to delete
                   rfdevice_1x_interface.h
08/05/11   sty     Deleted deprecated data types and APIs
07/29/11   Saul    IRAT update. Skip setting GPIOs during IRAT.
07/23/11   Saul    Converted rx wake up APIs to use GRFC generate script.
07/20/11   sty     Changed return type of rfc_cdma_get_tx_lut_table_name()
                   Deleted lut_table_enum_for_pa_state[]
07/11/11   cd      Separated GPIO/GRFC config functions for sleep and wakeup
07/07/11   sty     Added lut_table_enum_for_pa_state in 
                   rfc_cdma_logical_device_params_type
                   Added prototype for rfc_cdma_get_tx_lut_table_name
06/29/11   cd      - New card interfaces to configure GPIO/GRFC signals using 
                   autogen settings
                   - Cleaned up path enum definitions
06/07/11   shb     Renamed rf_cdma_rxdev_type to rfdevice_cdma_rx_type
05/31/11   aro     Added logical device internal parameter
05/25/11   aro     Added RFC Path enum
05/21/11   aro     Added RxLM and TxLM Chain info to logical device parameter
                   for FTM usage
05/21/11   aro     Changed RFC interface to handle device parameter
05/21/11   aro     Added interface to return Logical Device parameter
05/19/11   bmg     Added rfc_cdma_logical_device_params_type to provide logical
                   to physical device mapping parameters to CDMA MC.
05/04/11   sty     Added GPIO info in Rx list and GRFC engine info in Tx
04/28/11   sty     Changed signature of manage_vregs()
03/04/11   aro     Renamed RFC config structure name 
02/18/11   sty     Deleted rfc_cdma_get_tx_path
02/17/11   sty     Renamed RFC_CARD_1X_H to RFC_CDMA_H
01/25/11   sty     Added rfc_cdma_get_warmup_time
01/10/11   sty     Deleted unused variables/struct
                   Rearranged some APIs
01/10/11   sty     (1) Deleted rfc_cdma_get_mode_config_data and 
                       rfc_cdma_config_band_data
                   (2) Added struct rfc_cdma_rx_list_type and 
                       rfc_cdma_tx_list_type
                   (3) Changed return type in rfc_cdma_get_rx_band_config and
                       rfc_cdma_get_tx_band_config
01/08/11   aro     Compiler Warning Fix
01/08/11   aro     Changed @ingroup to @addtogroup
01/06/11   sty     Minor update to comments
01/06/11   sty     Deleted redundant header files. Doxygen updates
01/06/11   sty     Deleted unused APIs and variables
                   Deleted rfc_config_vregs - which is now part of 
                   rfc_manage_vregs()
                   Doxygen comments
01/05/11   sty     Added manage and config VREGs
01/05/11   sty     Added doxygen comments, cleanup
01/04/11   sty     Changed all *_1xhdr_* to *_cdma_*
01/04/11   sty     Added C++ protection
12/20/10   sty     Replaced rf_path_enum_type with rfm_path_enum_type
12/20/10   sty     Cleanup
10/22/10   Saul    Added entries for BC15(AWS)
07/29/10   aro     Common GRFC Design Implementation for 1xHDR
06/28/10   aro     Added rfc_1x_band_params_type
04/15/10   adk     Fixed compilation errors
04/02/10   adk     Merged branch scmm_mdm9k_merge_final
03/16/10   Saul    Added entries for BC6
03/05/10   Vish    Merged from src: 01/21/10 - 01/26/10
01/26/10   sr      Added copyright year
01/21/10   sr      Changed rfc_1x_core_config_type, code cleanup
01/24/10   sr      Added rf1x_rx_warmup_time_in_us
01/21/10   aro     Moved Therm Comp Table to 1xHDR common file
01/21/10   sr      Changes needed to support new RFC structures
01/20/10   sr      Changes needed to support new RFC structures in 1x
08/25/09   aro     RFC 1x Design Update
                   [1] Moved HDET Therm Compensation into Band Specific data str
                   [2] Moved RC3, RRI, and CW Comp into 1x core config data str
                   [3] Created Generic functions to query each of these
                       Compensation Values. No more RF Card specific functions
                       are required for this purpose
                   [4] Changed the function/variable name to have "1xhdr"
                       instead of just "1x"
                   [5] Doxygen Documentation Added
07/08/09   av      Cleanup and added port configuration (RTR) data  in RFC
11/21/08   dyc     Update design.
11/04/08   dyc     Initial version.

==============================================================================*/
#include "rfa_variation.h"
#ifdef FEATURE_CDMA1X
#ifdef __cplusplus
extern "C"
{
#endif

#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rfm_cdma_band_types.h"
#include "rfc_card.h"
#include "rfc_common.h"
#include "rfdevice_cdma_interface.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_mdsp_types.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"

#include "rf_cdma_pa.h"
#include "rf_cdma_asm.h"
#include "rf_cdma_atuner.h"
#include "rfcommon_atuner_intf.h"

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_manager.h"
#endif /* FEATURE_RF_ASDIV */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Band Subclass Bit Id - this info is internal to RFC 
*/
#define RF_SC_0 ((uint32)1<<0)
#define RF_SC_1 ((uint32)1<<1)
#define RF_SC_2 ((uint32)1<<2)
#define RF_SC_3 ((uint32)1<<3)
#define RF_SC_4 ((uint32)1<<4)
#define RF_SC_5 ((uint32)1<<5)
#define RF_SC_6 ((uint32)1<<6)
#define RF_SC_7 ((uint32)1<<7)
#define RF_SC_8 ((uint32)1<<8)
#define RF_SC_9 ((uint32)1<<9)
#define RF_SC_10 ((uint32)1<<10)
#define RF_SC_11 ((uint32)1<<11)
#define RF_SC_12 ((uint32)1<<12)
#define RF_SC_13 ((uint32)1<<13)
#define RF_SC_14 ((uint32)1<<14)
#define RF_SC_15 ((uint32)1<<15)

/*----------------------------------------------------------------------------*/
/*!
   @brief
  Enumeration of RFC CDMA Paths. This should be used internally to RFC.
*/
typedef rf_path_enum_type rfc_cdma_path_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   1x PA switch points for access probes only.

   @details
   This data structure is 1x specific and contains hard-coded PA switch
   points for 1x and HDR Access Mode. The mobile will use it to
   overwirte the previous configuration from NVs.
*/
typedef struct 
{
  uint8 pa_rise[3];  /*!< for pa_rise value */
  uint8 pa_fall[3];  /*!< for pa_fall value */
} rfc_cdma_access_mode_pa_switchpoints_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA Band-specific Parameters
  
  @details
  This will hold band specific Rx and Tx paramaters suchas RTR port number for
  each band.
*/
typedef struct
{
  int rx_path; /*!< Receive Path Parameter */
  int tx_path; /*!< Transmit Path Parameter */
  rfc_cdma_access_mode_pa_switchpoints_type* access_switchpoints;
  /*!< PA switch points */
} rfc_cdma_band_params_type;
/* ----------------------------------------------------------------------- */

/*!
  @brief
  Data structure containing RFC device parameters specific to a given band
  and chain combination.
*/
typedef struct rfc_cdma_rfc_device_info_type_s
{
  /*!
    @brief
    Specifies the sub-classes supported
  */
  rfc_device_info_type* rfc_device_info [RFM_CDMA_MAX_BAND] ;

} rfc_cdma_rfc_device_info_type ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Duplicate 1xHDR Band Parameters Typedef
  
  @details
  Introduced to help Merge Transition
*/
typedef rfc_cdma_band_params_type rfc_1x_band_params_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Data structure that contains all the data that needs to be retrieved from
   the RFC for Tx 

   @details
   This data structure is intended to be a comprehensive list of all Tx data 
   that needs to be passed from RFC.
   This data structure will need to be updated for any additional RFC info
   that may be needed in future.
 
*/
typedef struct
{
  rfc_cdma_band_params_type *band_params_ptr;
  /*!< band and core data ptr */

  rfc_control_signal_info_type *rf_ctrl_sig_ptr;
  /*!< ptr to the band specific rf control signal settings */

} rfc_cdma_tx_list_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure containing all of the platform-dependent parameters to
  configure the proper hardware devices.
*/
typedef struct rfc_cdma_logical_device_params_s
{
  /*!
    @brief
    Pointer to the RF chipset receiver device that should be used for
    this logical device.
  */
  rfdevice_trx_cdma_rx* rx_device[RFM_CDMA_MAX_BAND];

  /*!
    @brief
    CDMA RF NV path that should be used for this logical device per band.
  */
  rf_cdma_nv_path_type rx_nv_path[RFM_CDMA_MAX_BAND];

  /*!
    @brief
    CDMA firmware path that should be used for this logical device.
  */
  rf_cdma_mdsp_chain_enum_type rx_mdsp_path;

  /*!
    @brief
    CDMA Rx Link Manager Chain that should be used for this logical device in
    1x mode. This will be used in FTM Mode.
  */
  rxlm_chain_type ftm_rxlm_chain_onex;

  /*!
    @brief
    CDMA Rx Link Manager Chain that should be used for this logical device in
    HDR mode. This will be used in FTM Mode.
  */
  rxlm_chain_type ftm_rxlm_chain_hdr;

  /*!
    @brief
    CDMA Rx Link Manager ADC that should be used for this logical device.
  */
  rxlm_adc_type rxlm_adc;

  /*!
    @brief
    Pointer to the RF chipset transmitter device that should be used for
    this logical device.

    @details
    If this logical device does not support transmitting, this pointer
    will be NULL.
  */
  rfdevice_trx_cdma_tx* tx_device[RFM_CDMA_MAX_BAND] ;

  /*!
    @brief
    CDMA RF NV path that should be used for this logical device.
  */
  rf_cdma_nv_path_type tx_nv_path[RFM_CDMA_MAX_BAND];

  /*!
    @brief
    CDMA firmware path that should be used for this logical device.
  */
  rf_cdma_mdsp_chain_enum_type tx_mdsp_path;

  /*!
    @brief
    CDMA Tx Link Manager Chain that should be used for this logical device
    in 1x mode. This will be used in FTM Mode.
  */
  txlm_chain_type ftm_txlm_chain_onex;

  /*!
    @brief
    CDMA Tx Link Manager Chain that should be used for this logical device
    in HDR mode. This will be used in FTM Mode.
  */
  txlm_chain_type ftm_txlm_chain_hdr;

  /*!
    @brief
    CDMA Tx Link Manager DAC that should be used for this logical device.
  */
  txlm_dac_type txlm_dac;
  
  /*!
    @brief  
    This variable tells which logical device has been paired with the current  
    device.  
    For ex: device-0 and device-1 are paired with each other 
            device-2 and device-4 are paired with each other.  
    This variable is used since the device mappings can change based on the 
    RTR-RFC being used
  */
  rfm_device_enum_type paired_device;

  /*!
    @brief
    Holds the pointer to any custom PA
  */
  cdma_pa* pa[RFM_CDMA_MAX_BAND] ;

  /*!
    @brief
    Holds the pointer to any custom ASM
  */
  rf_cdma_asm* sm [RFM_CDMA_MAX_BAND];

  /*!
    @brief
    Holds the pointer to any QPOET device (per band)
  */
  void* qpoet_obj [RFM_CDMA_MAX_BAND];

  /*!
    @brief
    Holds the pointer to any QTUNER device (per band)
  */
  #ifdef FEATURE_RF_HAS_QTUNER
  void* qtuner_obj [RFM_CDMA_MAX_BAND];
  #endif

  /*!
    @brief
    Table of pointers for the antenna tuner managers for this device.
   
    @details
    Each band may have a different antenna tuner, so this is stored as
    a table of pointers.
  */
  rf_cdma_atuner* ant_tuner[RFM_CDMA_MAX_BAND];

  /*!
    @brief
    Holds the pointer to any HDET device (per band)
  */
  void* hdet_obj [RFM_CDMA_MAX_BAND];

  /*!
    @brief
    Holds the pointer to any COUPLER device (per band)
  */
  void* coupler_obj [RFM_CDMA_MAX_BAND];

  /*!
    @brief
    Holds the rfc device info
  */
  rfc_cdma_rfc_device_info_type rfc_dev_info ;

  /*! 
    @brief
    Holds RFC Tx device Information
  */
  rfc_cdma_rfc_device_info_type rfc_tx_device_info ;

} rfc_cdma_logical_device_params_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure containing RFC internal platform-dependent 
  parameters to configure the proper hardware devices. These information are
  not exposed to CORE/MC
*/
typedef struct rfc_cdma_logical_device_internal_params_s
{

  /*!
    @brief
    CDMA RFC Rx Chain that should be used for this logical device. This
    will be used only within RFC.
  */
  rfc_cdma_path_type rfc_rx_path;

  /*!
    @brief
    CDMA RFC Tx Chain that should be used for this logical device. This
    will be used only within RFC.
  */
  rfc_cdma_path_type rfc_tx_path;

  /*!
    @brief
    Path number to be used to configure Vreg for this logical device. This will
    be used only within RFC.
  */
  rf_path_enum_type vreg_path_num;

  /*!
    @brief
    RF Chip number to which this logical device is associated. This
    will be used only within RFC.
  */
  rfc_rf_chip_number rf_chip_num;
  
} rfc_cdma_logical_device_internal_params_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure containing RFC internal platform-dependent parameters to
  configure the proper hardware devices. This information is not exposed to
  CORE/MC.
*/
typedef struct rfc_cdma_sub_band_class_info_type_s
{
  /*!
    @brief
    Specifies the sub-classes supported
  */
  uint32 sub_classes_supported[RFM_CDMA_MAX_BAND];

} rfc_cdma_sub_band_class_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure containing RFC internal platform-dependent parameters to
  configure the proper hardware devices. This information is not exposed to
  CORE/MC.
*/
typedef struct rfc_cdma_tx_lut_to_use_type_s
{
  /*!
    @brief
    Specifies the TX LUT table to be used 
  */
  rfdevice_cdma_lut_type tx_lut_type[RFM_CDMA_MAX_BAND][RFM_PA_GAIN_STATE_NUM];

} rfc_cdma_lut_to_use_for_pa_state_type;

/* ----------------------------------------------------------------------- */
boolean 
rfc_cdma_manage_vregs
(
  rfm_device_enum_type device, 
  rfm_mode_enum_type mode, 
  rfm_cdma_band_class_type band,
  rfm_path_state state
);

/*----------------------------------------------------------------------------*/
int32
rfc_cdma_get_warmup_time
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_get_enhanced_warmup_time
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode,
  void * const rf_wu_data
);

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_get_tx_lut_table_name
(
  rfm_mode_enum_type const mode,
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type const band,
  rfm_pa_gain_state_type const pa_state,
  int* table_name_ptr,
  rfdevice_cdma_lut_type* table_type_ptr
);

/*----------------------------------------------------------------------------*/
const rfc_cdma_logical_device_params_type*
rfc_cdma_get_logical_device_params
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_is_tunable_on_device
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
int32
rfc_cdma_get_grfc_engine_num
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfc_signal_type sig_type
);

/*----------------------------------------------------------------------------*/
uint32
rfc_cdma_get_sub_class_for_band
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type rf_band
);

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_generate_rx_wakeup_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  rf_buffer_intf *script_buf_ptr,
  int16 delay
);

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_generate_rx_sleep_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  rf_buffer_intf *script_buf_ptr,
  int16 delay
);

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_generate_tx_wakeup_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  rf_buffer_intf *script_buf_ptr,
  int16 delay
);

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_generate_tx_sleep_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  rf_buffer_intf *script_buf_ptr,
  int16 delay
);

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_generate_tx_on_off_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  boolean tx_on,
  rf_buffer_intf *script_buf_ptr,
  int16 delay,
  int32 *num_grfc
);

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_generate_pa_on_off_grfc_script
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band, 
  boolean pa_on,
  rf_buffer_intf *script_buf_ptr,
  int16 delay
);

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_generate_asdiv_xsw_script
( 
   rfcommon_asdiv_position_type position,
   rf_buffer_intf *asdiv_script_ptr,
   void* cb_data
);
#endif /* FEATURE_RF_ASDIV */

/*----------------------------------------------------------------------------*/
uint32
rfc_cdma_get_tx_modem_chain
(
  rfm_device_enum_type device ,
  rfm_cdma_band_class_type band 
) ;

/*----------------------------------------------------------------------------*/
uint32
rfc_cdma_get_slave_tx_modem_chain
(
  rfm_device_enum_type device ,
  rfm_cdma_band_class_type band 
) ;

/*----------------------------------------------------------------------------*/
uint32
rfc_cdma_get_rx_modem_chain
(
  rfm_device_enum_type device ,
  rfm_cdma_band_class_type band 
) ;

boolean 
rfc_cdma_setup_fb_path 
(
  rfm_device_enum_type device_fb, 
  rfm_cdma_band_class_type band, 
  boolean et_enable
);

boolean
rfc_cdma_setup_fb_device 
(
  rfm_device_enum_type device, 
  rfm_device_enum_type device_fb,
  rfm_cdma_band_class_type band, 
  uint32 tx_chan,
  rfdevice_rxtx_common_class**  fb_device_ptr    
);

/*----------------------------------------------------------------------------*/
boolean 
rfc_cdma_get_rffe_device_info
(
	rfm_device_enum_type rfm_dev, 
	rfm_cdma_band_class_type band, 
	rfdevice_type_enum_type rffe_device_type,
	uint16 *mfg_id,
	uint16 *prd_id,
	uint16 *rev_id
) ;

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_is_rx_device
(
  rfm_device_enum_type device  
) ;

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_is_tx_device
(
  rfm_device_enum_type device  
) ;

/*----------------------------------------------------------------------------*/
boolean
rfc_cdma_update_alt_rx_path
( 
  rfm_device_enum_type device,
  rf_card_band_type curr_band, 
  uint8 alt_path
);


/*----------------------------------------------------------------------------*/
uint8
rfc_cdma_get_current_rx_path
(
   rfm_device_enum_type        device ,
   rfm_cdma_band_class_type    band 
) ;

/*----------------------------------------------------------------------------*/
rfdevice_rxtx_common_class *
rfc_cdma_get_cmn_rxtx_device
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type cdma_band, 
  uint8 alt_path
);

#endif /* FEATURE_CDMA1X */

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* RFC_CDMA_H */

