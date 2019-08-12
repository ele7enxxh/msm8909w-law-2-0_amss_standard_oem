#ifndef RFC_CARD_WCDMA_H
#define RFC_CARD_WCDMA_H
/*!
  @file rfc_card_wcdma.h

  @brief
  This file contains WCDMA mode data definitions and prototypes.

  @details

*/

/*===========================================================================

Copyright (c) 2008 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfc_card_wcdma.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
09/04/15   ak      Changes to put the GRFCs to init state in DRX sleep
09/18/14   rsr     Changes to return TX LUT index when querying TX LUT info.
04/17/14   ac      asd bringup with rfc changes and device script
07/17/13   aro     Replaced with antenna position enum
07/26/13   vs      Add RF interface to query DBDC bands supported
07/15/13   aro/kr  Added initial support for WCDMA ASDiv
07/09/13   kcj     Check that device init succeeded during rfwcdma_mc_init.
04/11/13   kai     Match apt_config_enum with WFW SMPS mode enum
03/12/13   sn      XPT dual WTR fix for feedback path
11/26/12   tks     Changed the rfc_wcdma_get_device api from single pointer to 
                   double pointer 
11/14/12   dw      Add Dime APT support
10/11/12   dw      SVVZ bringup updates
10/01/12   sb      Removed GRFC engine references in Dime code 
09/18/12   ac      rfc to take device as parameter
08/12/12   vbh     Added wcdma_get_device()
07/17/12   shb     Added rfc_wcdma_init()
05/24/12   kai     Remove structure rfc_wcdma_band_supported_type
04/04/12   swb     Add multi-band support for HSPA APT 
03/01/01   dw      Add support for HSPA APT
03/16/12   swb     Add npler LUT configuration to rfc_wcdma_core_config_type
02/15/12   sar     Removed RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE.
02/02/12   kg      Fix bug in rfc_wcdma_qpa_status_enum_type 
01/01/12   kg      rfc support for wcdma with qpa 
11/03/11   ac      rfc tx_disable
10/31/11   ac      rfc wakeup
10/05/11   ac/sr   rfc sleep
09/30/11   dw      Remove rf_card_wcdma_smps_pa_bias_apt_type
09/29/11   ac      EU bringup for BC3
09/06/11   dw      Enable APT
08/23/11   ac      moving to tech specific command dispatch
08/08/11   dw      RFC cleanup for WCDMA
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
04/22/11   aak    Changing rfc_path_state to rfm_path_state
04/11/11   dw     Change to support new power management for Nikel
04/06/11   ad      Port APT changes from MDM9k
04/01/11   bmg     Added extern "C" for C++ safety.
03/22/11   dw      Add RFC_WCDMA_GET_CORE_CONFIG in command dispatch
01/27/11   dw      WCDMA RFC cleanup
01/07/11   dw      WCDMA New RFC design integration 
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
07/21/10   tnt     Moving IRAT code to common RFC
07/08/09   av      Cleanup and added port configuration (RTR) data  in RFC
03/09/09   clk     Code cleanup
11/21/08   dyc     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfc_card.h"
#include "nv_items.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                           Data Definitions
===========================================================================*/

/* Enum for WCDMA RFC command dispatch*/
typedef enum
{
  RFC_WCDMA_IS_DUAL_CARRIER_SUPPORTED,
  RFC_WCDMA_GET_CORE_CONFIG,
  RFC_WCDMA_GET_APT_CONFIG,
  RFC_WCDMA_IS_LP_TX_LUT_SUPPORTED,
  RFC_WCDMA_IS_BAND_SUPPORTED,
  RFC_WCDMA_IS_QPA_SUPPORTED,
  RFC_WCDMA_GET_HSPA_APT_CONFIG,
  RFC_WCDMA_MAX_CMD = 0xFFFF
} rfcs_wcdma_cmd_type;

typedef enum
{
  RFC_WCDMA_APT_NOT_SUPPORTED,
  RFC_WCDMA_APT_TX_PDM_SUPPORTED,
  RFC_WCDMA_APT_GPIO_SUPPORTED,
  RFC_WCDMA_XPT_QPOET_SUPPORTED
} rfc_wcdma_apt_config_enum_type;

typedef enum
{
  RFC_WCDMA_QPA_NOT_SUPPORTED,
  RFC_WCDMA_QPA_IS_SUPPORTED,
  RFC_WCDMA_QPA_MAX
} rfc_wcdma_qpa_status_enum_type;

/* ----------------------------------------------------------------------- */
/*!
  @brief
  WCDMA Mode core configuration data types.

  @details

*/
typedef struct
{
  uint16 warmup_time;
  uint16 tune_time;
  boolean pwr_on_wakeup;
  uint16 cgagc_settling_time;
} rfc_wcdma_core_config_type;

/* ----------------------------------------------------------------------- */
/*!
  @brief
  HSPA APT config data type to support no Cal change (use exisiting
  first sweep data)

  @details

*/
typedef struct
{
  /* Flag to support HSPA APT using existing first sweep data*/
  boolean hspa_apt_use_canned_data;
  /* PA state*/
  uint8 pa_index;
  /* start power in dBm 10*/
  int16 start_power;
  /* Stop power in dBm 10*/
  int16 stop_power;
} rfc_wcdma_hspa_apt_config_type;

/*!
  @brief
  HSPA APT band config data type

  @details

*/
typedef struct
{
  /* APT config data */
  rfc_wcdma_hspa_apt_config_type rfc_wcdma_hspa_config_data;
  /* The current band */
  rfcom_wcdma_band_type band;
} rfc_wcdma_hspa_apt_config_band_type;

/*! @todo dchow:11/21/08: Sample implemented only.. needs definition */
/* ----------------------------------------------------------------------- */
/*!
  @brief
  WCDMA Mode feature configuration data types.

  @details

*/
typedef struct
{
  int sample1;
} rfc_wcdma_feature_config_type;

/*@todo: anupamav: This needs to be in 6x95 specific */
typedef struct
{
  int rx_path;
  int tx_path;

} rfc_wcdma_band_params_type;

typedef struct
{
  rfcom_wcdma_band_type band;
  boolean dc_is_supported;
} rfc_wcdma_dc_supported_type;

typedef struct
{
  rfcom_wcdma_band_type band;
  rfc_wcdma_apt_config_enum_type apt_config;
  uint16 default_smps_val;
} rfc_wcdma_apt_supported_type;

typedef struct
{
  rfcom_wcdma_band_type band;
  rfc_wcdma_qpa_status_enum_type qpa_status;
} rfc_wcdma_qpa_config_type;

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This data structure defines the settings associated with UMTS (i.e. both
  WCDMA and GSM) bands.

  @details
  This is a common card struct since it is used by both GSM and WCDMA modes.

*/
typedef struct
{
  /*! ptr to the band specific GPIO settings */
  rfc_set_gpio_type *band_gpio_info_ptr;

  /*! ptr to the band specific rf control signal settings */
  rfc_control_signal_info_type *rf_ctrl_sig_ptr;

  /*! ptr to band specific configuration parameters */
  rfc_wcdma_band_params_type *band_params_ptr;

  /*! ptr to band specific SMPS PA characterization tables */
  rfc_smps_pa_characterization_table_type  *smps_pa_characterization_table_ptr;

} rfc_wcdma_band_config_type;

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This generic data structure defines the RF configuration attributes on
  a per mode and band basis.

  @details
  The core, feature and gpio configuration pointers will be updated
  with a mode change.  There will be a rfc_mode_attribute structure assigned
  on a path basis.

  The rx and tx band configuration pointers will be updated with a band
  change.

  Pointers not used in a mode or band should be assigned to NULL.
*/
typedef struct
{
  /*! RF mode ID to identify this structure type */
  rfcom_mode_enum_type mode_id;

  /*! ptr to card specfic WCDMA core settings */
  rfc_wcdma_core_config_type *rfc_core_config_ptr;

  /*! ptr to card specfic WCDMA feature settings */
  rfc_wcdma_feature_config_type *rfc_feature_config_ptr;

  /*! ptr to mode WCDMA GPIO/GRFC/RF_CONTROL settings */
  rfc_gpio_grfc_config_type *rfc_gpio_config_ptr;

  /*! ptr to card specfic WCDMA Rx band settings */
  rfc_wcdma_band_config_type *rfc_band_rx_config_ptr;

  /*! ptr to card specfic WCDMA Tx band settings */
  rfc_wcdma_band_config_type *rfc_band_tx_config_ptr;

  /*! ptr to the power management configuration function ptrs */
  rfc_pwr_mgnt_type *rfc_pwr_mgnt_ptr;

} rfc_wcdma_attribute_type;


/*===========================================================================
                           Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
void rfc_wcdma_rx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band
);
/* ----------------------------------------------------------------------- */
void *rfc_wcdma_get_rx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rfc_band_config_data_enum_type config_data
);
/* ----------------------------------------------------------------------- */
void rfc_wcdma_tx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band
);
/* ----------------------------------------------------------------------- */
void *rfc_wcdma_get_tx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rfc_band_config_data_enum_type config_data
);
/* ----------------------------------------------------------------------- */
void rfc_wcdma_init( void );

/* ----------------------------------------------------------------------- */
void rfc_wcdma_mode_config( rfm_device_enum_type device );

/* ----------------------------------------------------------------------- */
void *rfc_wcdma_get_mode_config_data
(
  rfm_device_enum_type device,
  rfc_mode_config_data_enum_type config_data
);

int32 rfc_wcdma_command_dispatch
( 
  rfm_device_enum_type device, 
  rfcs_wcdma_cmd_type cmd, 
  void *data
);

int32 rfc_wcdma_tx_disable(rfm_device_enum_type device,rfcom_wcdma_band_type band);

int32 rfc_wcdma_sleep
( 
  rfm_device_enum_type device 
);

int32 rfc_wcdma_wakeup(rfm_device_enum_type device);

void rfc_wcdma_manage_vregs
( 
  rfm_device_enum_type device, 
  rfcom_wcdma_band_type band, 
  rfm_path_state state
);


void** rfc_wcdma_get_device
( 
  rfm_device_enum_type rfm_dev, 
  rfc_rxtx_enum_type rx_tx,
  rfcom_wcdma_band_type band, 
  rfdevice_type_enum_type dev_type
);

void rfc_wcdma_get_grfc_info_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rfc_sig_cfg_type* rfc_signal,
  rfc_signal_type rfc_signal_type
);

boolean rfc_wcdma_setup_fb_path (rfm_device_enum_type device_fb, rfcom_wcdma_band_type band, boolean et_enable);

boolean rfc_wcdma_setup_fb_device (rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_wcdma_band_type band, uint16 tx_chan);

void* 
rfc_wcdma_get_asd_xsw_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint32 position
);

#ifdef FEATURE_RF_ASDIV  
boolean
rfc_wcdma_get_asdiv_xsw_script
( 
  uint32 position,
  rf_buffer_intf *asdiv_script_ptr,
  void* cb_data
);
#endif

boolean rfc_wcdma_get_device_init_complete(void);

void *rfc_wcdma_get_dbdc_band_support (void);

rfdevice_rxtx_common_class * rfc_wcdma_get_common_rxtx_device
(
  rfm_device_enum_type rfm_device, 
  rfcom_wcdma_band_type band, 
  uint32 alt_path
);


void rfc_wcdma_update_rx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band
);


#ifdef __cplusplus
}
#endif

#endif /* RFC_CARD_WCDMA_H */
