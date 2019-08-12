#ifndef RFC_CARD_TDSCDMA_H
#define RFC_CARD_TDSCDMA_H

/*!
   @file
   rfc_card_tdscdma.h

   @brief
   This file contains tdscdma mode data definitions and prototypes.

   @details

*/

/*===========================================================================

Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rfc_card_tdscdma.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
04/02/15   nsh     Smartly handle Rx2Tx and Tx2Rx scenario for GRFC ASM
11/13/14   qzh     Add check of if RFC TDS is created before calling TDS init in rfm_init
11/02/14   qzh     Get PA related timing from RFC file instead of using default value for GRFC PA
09/03/14   rsr     Changes to return TX LUT index when querying TX LUT info.
07/17/14   ms      Sawless RX support
07/10/14   jyu     Asd bringup with rfc changes and device script
02/10/14   kb      [Merge][XPT] Add support for porper shutdown of WTR during XPT deconfig.
09/18/13   ms      Add api for getting filtered band
07/26/13   ms      AsDiv support
05/20/13   al      Added functions to support split band
04/30/13   kb      [XPT] Support to set up feedback path and device for EPT
12/21/12   jyu     Modified the get_rf_device_obj function to return double pointer  
12/04/12   ms      APT support
10/01/12   sb      Removed GRFC engine references in Dime code 
09/14/12   jyu     Added interface to return pa lut type information from RFC 
08/20/12   jyu     Added support to set/clean specific GRFC (immediate execution)
08/20/12   jyu     Added support to cleanup GRFCs   
08/02/12   nsh     interface change from "PATH" to "DEVICE"
08/02/12   jyu     Added support to query RFC for device ptr 
05/29/12   zg      Added definition of TDS_SIG_LIST_NOTIME_MAX_LENGTH 
11/09/11   zg      Added rfc_meas_tdscdma_generate_grfc_script. 
09/12/11   jhe     Removed rfc_config_band_data_type
08/15/11   zg      Initial version.
  
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfc_card.h"
#include "DDITlmm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                           Data Definitions
===========================================================================*/
/* Maximal length of signal lists without timing among all RFCs*/
#define TDS_SIG_LIST_NOTIME_MAX_LENGTH 30  

/* Enum for TDSCDMA RFC command dispatch*/
typedef enum
{
  RFC_TDSCDMA_SET_RX_GRFC_NO_TIME,
  RFC_TDSCDMA_SET_TX_GRFC_NO_TIME,
  RFC_TDSCDMA_CLEAN_RX_GRFC_NO_TIME,
  RFC_TDSCDMA_CLEAN_TX_GRFC_NO_TIME,
  RFC_TDSCDMA_MAX_CMD = 0xFFFF
} rfcs_tdscdma_cmd_type;

/* TDSCDMA function pins */
/* NOTE: Do not disturb the order, add the new function pins at the end, 
before the RFC_TDSCDMA_MAX_GRFC */

typedef enum
{
  RFC_TDSCDMA_PA_HB_EN_GRFC,
  RFC_TDSCDMA_PA_LB_EN_GRFC,
  RFC_TDSCDMA_RF_ON_RX_ON_GRFC,
  RFC_TDSCDMA_RF_ON_TX_ON_GRFC,
  RFC_TDSCDMA_TX_ACTIVE_GRFC,
  RFC_TDSCDMA_ANTENNA_SEL0_GRFC,
  RFC_TDSCDMA_ANTENNA_SEL1_GRFC,
  RFC_TDSCDMA_ANTENNA_SEL2_GRFC,
  RFC_TDSCDMA_ANTENNA_SEL3_GRFC,

  RFC_TDSCDMA_PA_RANGE0_GRFC,
  RFC_TDSCDMA_PA_RANGE1_GRFC,
  /* Add new ones above this line */
  RFC_TDSCDMA_MAX_GRFC
} rfc_tdscdma_grfc;

typedef struct
{
  uint32 grfc_num;
  uint32 mask;
  uint32 value;
  uint32 pol_mask;
  uint32 tristate_mask;
  uint32 timed_mask;
  uint32 time_ctl_addr;
  DALGpioSignalType gpio;
  uint32 gpio_mask;
  DALGpioSignalType grfc;
} rfc_tdscdma_grfc_attributes_type;

typedef struct 
{
  int rx_alpha_qs;
  int ant_start_delta_qs;
  int ant_stop_delta_qs;
  int dc_cal_time_qs;
  int modem_delay_time_qs;
  int pll_settling_time_qs;
  int rx_mdsp_overhead;
  int mux_dly_freez_time_qs;
} rfc_tdscdma_rx_timing_info_type;


typedef struct
{
  uint16 pwr_lvl;
  uint16 *tx_ind;
} rfc_tdscdma_tx_pcl_ind_type;

typedef struct
{
  rfcom_tdscdma_band_type band;
  rfc_signal_type sig_type;
  rfc_logic_type override_logic;
} rfc_tdscdma_grfc_notime_control_type;

typedef struct
{
  boolean enable;
  uint16 smps_default;
  uint16 smps_idle;
  uint16 over_drv_dur;
} rfc_tdscdma_apt_cfg_type;


/*! @todo dchow:11/21/08: Sample implemented only.. needs definition */
/* ----------------------------------------------------------------------- */
/*!
   @brief
   TDSCDMA Mode core configuration data types.

   @details

*/
typedef struct
{
   rfc_tdscdma_rx_timing_info_type    *rx_timing;
   //rfc_tdscdma_rx_timing_info_type    *rx_mon_timing;
   boolean                            pwr_off_in_sleep;
   uint16                             rf_warmup_time;
   //uint16                             rf_sigma_delta_max_db;
   //uint8                              rfc_gsm_tx_ref_clock_mode;
   rfc_tdscdma_apt_cfg_type           apt;
} rfc_tdscdma_core_config_type;

/*! @todo dchow:11/21/08: Sample implemented only.. needs definition */
/* ----------------------------------------------------------------------- */
/*!
   @brief
   TDSCDMA Mode feature configuration data types.

   @details

*/
typedef struct
{
   int feature1;
   int feature2;
} rfc_tdscdma_feature_config_type;

/*This needs to be in 6x95 specific */
typedef struct
{
  int path;
  rfc_grfc_rfctl_info_type* grfc_info; 
  rfc_grfc_rfctl_info_type* rfctl_info;
} rfc_tdscdma_band_params_type;

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This data structure defines the settings associated with UMTS (i.e. both
  WCDMA and TDSCDMA) bands.

  @details
  This is a common card struct since it is used by both TDSCDMA and WCDMA modes.

*/
typedef struct
{
  /*! ptr to the band specific GPIO settings */
  rfc_set_gpio_type *band_gpio_info_ptr;

  /*! ptr to the band specific rf control signal settings */
  rfc_control_signal_info_type *rf_ctrl_sig_ptr;

  /*! ptr to band specific configuration parameters */
  rfc_tdscdma_band_params_type *band_params_ptr;

} rfc_tdscdma_band_config_type;

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

   /*! ptr to TDSCDMA specific core settings */
   rfc_tdscdma_core_config_type *rfc_core_config_ptr;

   /*! ptr to TDSCDMA specific feature settings */
   rfc_tdscdma_feature_config_type *rfc_feature_config_ptr;

   /*! ptr to card specfic TDSCDMA GPIO settings */
   rfc_gpio_config_type *rfc_gpio_config_ptr;

   /*! ptr to card specfic TDSCDMA Rx band settings */
   rfc_tdscdma_band_config_type *rfc_band_rx_config_ptr;

   /*! ptr to card specfic TDSCDMA Tx band settings */
   rfc_tdscdma_band_config_type *rfc_band_tx_config_ptr;

   /*! ptr to TDSCDMA specific power management function ptrs */
   rfc_pwr_mgnt_type *rfc_pwr_mgnt_ptr;

} rfc_tdscdma_attribute_type;

extern boolean prx_port_is_sawless;

/*===========================================================================
                           Prototypes
===========================================================================*/
/* ----------------------------------------------------------------------- */
/* function prototypes */


void rfc_tdscdma_mode_config( rfm_device_enum_type  device );

void *rfc_tdscdma_mode_config_get (rfm_device_enum_type  device, rfc_mode_config_data_enum_type config_data);

int32 rfc_tdscdma_command_dispatch( rfm_device_enum_type  device, int32 cmd, void *data );

void rfc_tdscdma_rx_band_config( rfm_device_enum_type  device, rfcom_tdscdma_band_type band );

void *rfc_tdscdma_rx_band_config_get(rfm_device_enum_type  device, rfcom_tdscdma_band_type band,
                                 rfc_band_config_data_enum_type config_data);

void rfc_tdscdma_tx_band_config( rfm_device_enum_type  device, rfcom_tdscdma_band_type band );

void *rfc_tdscdma_tx_band_config_get(rfm_device_enum_type  device, rfcom_tdscdma_band_type band,
                                 rfc_band_config_data_enum_type config_data);

void** rfc_tdscdma_get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                      rfcom_tdscdma_band_type band, rfdevice_type_enum_type dev_type);

void rfc_tdscdma_sleep(rfm_device_enum_type  device);

void rfc_tdscdma_wakeup(rfm_device_enum_type  device);

void rfc_tdscdma_get_band_pa_lut_type(rfm_device_enum_type device, rfcom_tdscdma_band_type band, uint32* rf_asic_band_pa_lut_map);

void rfc_tdscdma_manage_vregs
( 
  rfm_device_enum_type  device, 
  rfcom_tdscdma_band_type band, 
  rfm_path_state state
);

void rfc_meas_tdscdma_generate_grfc_script
(
  uint32 band, 
  rfc_grfc_device_type *device, 
  rfc_grfc_script_type *grfc_buf_ptr,
  boolean is_irat 
);

boolean rfc_tdscdma_setup_fb_path (rfm_device_enum_type device_fb, rfcom_tdscdma_band_type band, boolean xpt_enable);

boolean rfc_tdscdma_setup_fb_device(rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_tdscdma_band_type band, uint16 tx_chan, boolean xpt_enable);

void rfc_tdscdma_get_rx_band(boolean *split_band, uint16 rx_chan,
                         rfcom_tdscdma_band_type *rfc_band);

void rfc_tdscdma_get_tx_band(boolean *split_band, uint16 tx_chan, 
                         rfcom_tdscdma_band_type *rfc_band);

void rfc_tdscdma_get_filtered_band(rfcom_tdscdma_band_type band, rfc_coex_filter_status_type filter_status, rfcom_tdscdma_band_type *rfc_band);

void rfc_tdscdma_get_split_band_earfcns(rfcom_tdscdma_band_type split_band,
                                    uint16 *low_earfcn,
                                    uint16 *high_earfcn);


void* 
rfc_tdscdma_get_asd_xsw_config
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band,
  uint32 position
);

#ifdef FEATURE_RF_ASDIV  
boolean
rfc_tdscdma_get_asdiv_xsw_script
( 
  uint32 position,
  rf_buffer_intf *asdiv_script_ptr,
  void* cb_data
);
#endif

rfdevice_rxtx_common_class* rfc_tdscdma_get_common_rxtx_device
(
  rfm_device_enum_type rfm_device, 
  rfcom_tdscdma_band_type band,
  uint32 alt_path
);

boolean rfc_tdscdma_grfc_pa_existed(void);

boolean rfc_tdscdma_is_rfc_tds_created(void);

boolean rfc_tdscdma_tx_rx_ant_sel_control(rfm_device_enum_type prx_device,
                                               rfm_device_enum_type drx_device,
                                               rfm_device_enum_type tx_device,              
                                               rfcom_tdscdma_band_type band,
                                               boolean drx_enable,
                                               rf_buffer_intf * rf_buffer);


boolean rfc_tdscdma_rx_tx_ant_sel_control(rfm_device_enum_type prx_device,
                                               rfm_device_enum_type drx_device,
                                               rfm_device_enum_type tx_device,              
                                               rfcom_tdscdma_band_type band,
                                               boolean drx_enable,
                                               rf_buffer_intf * rf_buffer);


rfc_sig_cfg_type * rfc_tdscdma_combine_two_signal_list(rfc_sig_cfg_type * rf_signals_a,
	                                                   rfc_sig_cfg_type * rf_signals_b);

rfc_sig_cfg_type * rfc_tdscdma_get_unique_signal_from_seta_to_setb(rfc_sig_cfg_type * rf_signals_a,
	                                                               rfc_sig_cfg_type * rf_signals_b);





#ifdef __cplusplus
}
#endif

#endif /* RFC_CARD_TDSCDMA_H */

