#ifndef RFM_DEVICE_TYPES_H
#define RFM_DEVICE_TYPES_H

/*!
  @file
  Definitions of types pertaining to RF devices.

  @details
  This file exports the definitions of types that are used in the RFM
  interface relating to RF device parameters.
*/

/*===========================================================================

Copyright (c) 2009, 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_device_types.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/12/14   kab     Add support for RFM_DEVICE_5 and remove placeholders
10/03/12   dbz     Fix out of order RFM device definitions 
07/26/12   vrb     Updated RFM Device definitions
11/23/11   swb     Map deprecated RFCOM_RECEIVER_DIV to device 1 (instead of DIV placeholder)
10/20/11   adk     Added support for SV diversity.
04/07/11   aro     Documentation Update
04/07/11   sar     Relocated for CMI and removed conditional #def's. 
02/03/10   aro     Documentation Update
10/05/09   bmg     Added RFM_INVALID_DEVICE
09/02/09   bmg     SVDO fixes
02/20/09   sar     Initial Release.

============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  A listing of all potential RF devices in the system.

  @details
  Not all instances of the RF driver will support every device
  enumerated below.  Some devices may only support Rx, some may
  support both Rx and Tx.
*/
typedef enum
{
  /*! RF Device 0 */
  RFM_DEVICE_0,

  /*! RF Device 1 */
  RFM_DEVICE_1,

  /*! RF Device 2 */
  RFM_DEVICE_2,

  /*! RF Device 3 */
  RFM_DEVICE_3,

  /*! RF Device 4 */
  RFM_DEVICE_4,

  /*! RF Device 5 */
  RFM_DEVICE_5,

  /*! Max WAN devices */
  RFM_MAX_WAN_DEVICES,


  /*! RF GPS Device */
  RFM_DEVICE_GPS,

  /*! @deprecated This should not be used to size arrays, but is provided
      for legacy support.  */
  RFM_MAX_DEVICES,


  /*! Can be used to indicate an invalid RFM device.  */
  RFM_INVALID_DEVICE,

  /*! Wild card to indicate ALL RFM Devices*/
  RFM_ALL_DEVICES = 0xFFFF

} rfm_device_enum_type;

/*----------------------------------------------------------------------------*/
/*! Deprecate : Device Type */
typedef rfm_device_enum_type rfcom_device_enum_type;

/*----------------------------------------------------------------------------*/
/*! Deprecate : Device Type */
#define RFCOM_TRANSCEIVER_0 RFM_DEVICE_0

/*----------------------------------------------------------------------------*/
/*! Deprecate : Device Type */
#define RFCOM_RECEIVER_1    RFM_DEVICE_1

/*----------------------------------------------------------------------------*/
/*! Deprecate : Device Type */
#define RFCOM_RECEIVER_2    RFM_DEVICE_2

/*----------------------------------------------------------------------------*/
/*! Deprecate : Device Type */
#define RFCOM_RECEIVER_3    RFM_DEVICE_3

/*----------------------------------------------------------------------------*/
/*! Deprecate : Device Type */
#define RFCOM_RECEIVER_4    RFM_DEVICE_4

/*----------------------------------------------------------------------------*/
/*! Deprecate : Device Type */
#define RFCOM_RECEIVER_5    RFM_DEVICE_5

/*----------------------------------------------------------------------------*/
/*! Deprecate : Device Type */
#define RFCOM_RECEIVER_DIV  RFM_DEVICE_1

/*----------------------------------------------------------------------------*/
/*! Deprecate : Device Type */
#define RFCOM_MAX_DEVICES   RFM_MAX_DEVICES

#ifdef __cplusplus
}
#endif

#endif /* RFM_DEVICE_TYPES_H */

