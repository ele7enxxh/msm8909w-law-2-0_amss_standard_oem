#ifndef RFM_COMMON_H
#define RFM_COMMON_H

/*!
  @file
  SVDO interfaces common to 1x and HDR public interface.

  @details
  This file exports the definitions and declartions that are used by
  clients of both the 1x and HDR SVDO interfaces.
*/

/*===========================================================================

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_common.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/11/13   sar     Updates for APQ, GNSS only target.
08/07/12   ndb     Change antenna tuner settings api to be customizable 
05/22/12   aca     AFC clean up- unified definition between FW, ML1 and RFSW
02/09/12   aca     Tx PLL error correction
11/29/11   bmg     Added rfm_is_band_chan_supported_v2()
09/28/11   sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and
                   FEATURE_RF_WCDMA_LM_RFM_INTERFACE.
09/12/11   sty     Deprecated rfm_is_tunable_on_antenna()
09/02/11   bmg     Added rfm_get_supported_bw() support
04/20/11   dw      Temporarily add back lm buffer indexes
04/05/11   sar     Removed conditional #def's to comply with CMI.
01/05/10    dw     RFM interface change for RxLM
12/14/10   aro     Removed Conflicting prototypes
10/02/90   zg      Moved rf_is_device_valid() to an internal header.
10/01/09   zg      Added rf_is_device_valid() for SVDO enabled build.
09/02/09   bmg     SVDO fixes
09/01/09   bmg     Provided SVDO and non-SVDO version of common APIs
07/10/09   Saul    Made changes to enable RF async API calls and use of RF task.
02/20/09   sar     Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "sys.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "sys_v.h"
#endif
#include "rfm_device_types.h"
#include "rfm_antenna_types.h"
#include "rfm_async_types.h"
#include "rfm_mode_types.h"
#include "rfcom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TXPLL_SCRIPT_ENTRIES 5

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


typedef struct 
{
  rf_path_enum_type path;
  uint16 channel;
  uint8 band;
  void *atuner_data_ptr;
} rfm_ant_tuner_setting_type;

/*!
  @brief
  Structure for partial band support in rfm layer

  @details
  This structure is the same as the one in RFC layer. The aim is to create a separation 
   between RFM layer from RFC layer
  */
typedef struct
{
    uint8 band;
    boolean partial_band_enabled;
}rfm_partial_band_info_type;



/*!
  @brief
  Receiver and transmitter baseband bandwidth, in Hz
*/
typedef int32 rfm_band_width_type;

rfm_band_width_type
rfm_get_supported_bw
(
  const rfm_device_enum_type device,
  const rfm_mode_enum_type rf_mode
);

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
boolean rfm_channel_config_is_valid
(
  const rfm_device_enum_type device,   // new type for the device
  const uint32 num_band_chan,
  const sys_channel_type* const band_chan_array,
  const boolean lo_retune_allowed
);
boolean rfm_channel_config_is_valid_legacy
(
  const rfcom_device_enum_type device,  // legacy type for the device
  const uint32 num_band_chan,
  const sys_channel_type* const band_chan_array,
  const boolean lo_retune_allowed
);
#endif

uint8
rfm_get_num_antenna
(
  void
);

uint8
rfm_get_num_devices
(
  void
);

rfm_antenna_diff_type
rfm_get_antenna_diff
(
  const rfm_device_enum_type device,
  const rfm_antenna_enum_type ant1,
  const rfm_antenna_enum_type ant2,
  const sys_channel_type band_chan
);

boolean
rfm_is_band_chan_supported_v2
(
  rfm_mode_enum_type rf_mode, 
  sys_band_class_e_type band, 
  word chan
);

uint32
rfm_enter_mode
(
  const rfm_device_enum_type device,
  const rfm_mode_enum_type rf_mode,
  void * const user_data_ptr,
  rfm_cb_handler_type cb_handler
  ,uint32 rxlm_buf_idx
);

#ifdef __cplusplus
}
#endif

#endif /* RFM_COMMON_H */
