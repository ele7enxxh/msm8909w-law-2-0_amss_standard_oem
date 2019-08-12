#ifndef RFM_MODE_TYPES_H
#define RFM_MODE_TYPES_H

/*!
  @file
  Common types used in RFM APIs that deal with the mode of the
  RF diver.

  @details
  This file exports the definitions of types that are used in the RFM
  interface relating to RF mode control.
*/

/*===========================================================================

Copyright (c) 2009-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_mode_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/13/15   vs      Add mode for second WCDMA for W+W
08/21/14   cd      Update RFM_ALL_MODES to 0x7FFFFFFF to make it max signed 
                   32-bit value
07/06/13   qzh     Update Non-signaling R4&HSUPA code with new TL1-RFSW interface and function partition
05/30/13   tws     Add EGSM_MODE_3
05/07/13   aak     Move EGSM_MODE_2 above RFM_NUM_MODES
04/30/13   aak     Change GSM_2 mode enum to 12
04/12/13   aak     Adding a mode for the second GSM chain in DSDA mode
07/26/12   vrb     Added RFM_ALL_MODES enum to indicate all modes 
10/11/11   gy      Added GPS Demod states to rfm_path_state
07/11/11   hy      Including TD-SCDMA support.
07/07/11   aak     Change to rfm_path_state 
05/17/11   aak     Moving rfm_path_state to this file
02/17/11   aro     Added Invalid Mode Enum
03/25/09   bmg     Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*!
  @brief
  Operating modes of the radio devices.

  @details
  This enumeration lists the modes that may be accepted by the
  rfm_enter_mode() function.
*/
typedef enum
{
  RFM_PARKED_MODE,   /*!< Parked/sleep mode */
  RFM_IMT_MODE,      /*!< WCDMA @todo depricate this enum @deprecated  */
  RFM_EGSM_MODE,     /*!< GSM @todo depricate this enum @deprecated  */
  RFM_1X_MODE,       /*!< 1X Mode */
  RFM_GPS_MODE,      /*!< GPS Mode @todo depricate this enum @deprecated  */
  RFM_1XEVDO_MODE,   /*!< HDR Rev 0, A, and B modes */
  RFM_SVDO_MODE,     /*!< Mode to support simultaneous 1x and HDR transmit on a single device. */
  RFM_GNSS_MODE,     /*!< Global navigation systems mode. */
  RFM_LTE_MODE,      /*!< UMTS Long Term Evolution mode. */
  RFM_LTE_FTM_MODE,  /*!< UMTS Long Term Evolution FTM Mode */
  RFM_TDSCDMA_MODE,  /*!< TDSCDA mode */
  RFM_TDSCDMA_MODE_FTM=14,/*!< TDSCDMA FTM NSmode */
  RFM_EGSM_MODE_2,   /*!< Second GSM mode for DSDA case for RF-MCPM interface */
  RFM_EGSM_MODE_3,   /*!< Third GSM mode for TSTS/TSDA case for RF-MCPM interface */
  RFM_WLAN_MODE,     /*!< WLAN Mode */
  RFM_IMT_MODE_2,    /*!< Second WCDMA in W+W mode*/
  RFM_NUM_MODES,     /*!< Total number of supported Modes */
  RFM_INVALID_MODE = RFM_NUM_MODES, /*!< Invalid RFM Mode  */
  RFM_ALL_MODES = 0x7FFFFFFF /*!< Wildcard to indicate all Modes  */
} rfm_mode_enum_type;

typedef rfm_mode_enum_type rfcom_mode_enum_type;

#define RFCOM_PARKED_MODE  RFM_PARKED_MODE
#define RFCOM_WCDMA_MODE   RFM_IMT_MODE
#define RFCOM_TDSCDMA_MODE RFM_TDSCDMA_MODE
#define RFCOM_GSM_MODE     RFM_EGSM_MODE
#define RFCOM_1X_MODE      RFM_1X_MODE 
#define RFCOM_GPS_MODE     RFM_GPS_MODE
#define RFCOM_1XEVDO_MODE  RFM_1XEVDO_MODE
#define RFCOM_GNSS_MODE    RFM_GNSS_MODE
#define RFCOM_LTE_MODE     RFM_LTE_MODE
#define RFCOM_LTE_FTM_MODE RFM_LTE_FTM_MODE
#define RFCOM_WLAN_MODE    RFM_WLAN_MODE
#define RFCOM_NUM_MODES    RFM_NUM_MODES

/*!
  @brief
  A listing of all states a path can be in.
*/
typedef enum
{
  RF_PATH_SLEEP_STATE,
  RF_PATH_RX_STATE,
  RF_PATH_RXTX_STATE,
  RF_PATH_DEMOD_NORM_STATE,
  RF_PATH_DEMOD_HIGH_STATE,
  RF_PATH_MAX_STATE,
} rfm_path_state;

#ifdef __cplusplus
}
#endif

#endif /* RFM_MODE_TYPES_H */
