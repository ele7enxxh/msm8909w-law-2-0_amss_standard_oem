#ifndef RFM_ANTENNA_TYPES_H
#define RFM_ANTENNA_TYPES_H

/*!
  @file
  Definitions of types pertaining to RF antennas.

  @details
  This file exports the definitions of types that are used in the RFM
  interface relating to RF antenna parameters.
*/

/*===========================================================================

Copyright (c) 2009, 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_antenna_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/11   aro     Added MAX number of antenna
02/20/09   sar     Initial Release.

============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*!
  @brief
  A listing of all potential RF antennas in the system.

  @details
  Not all instances of the RF driver will support every antenna
  enumerated below.  Some antennas may only support Rx, some may
  support both Rx and Tx.
*/
typedef enum
{
  RFM_ANTENNA_0,
  RFM_ANTENNA_1,
  RFM_ANTENNA_2,
  RFM_ANTENNA_3,
  RFM_ANTENNA_MAX
} rfm_antenna_enum_type;

/*!
  @brief
  Relative quality difference between two antennas
*/
typedef enum
{
  RFM_ANT_QLTY_UNKNOWN, /*!< The two antennas cannot be compared */
  RFM_ANT_QLTY_IDENTICAL, /*!< The two antennas have the same performance */
  RFM_ANT_QLTY_BETTER, /*!< The first antenna passed is better than the second antenna passed */
  RFM_ANT_QLTY_WORSE /*!< The first antenna passed is worse than the second antenna passed */
} rfm_antenna_diff_type;

#ifdef __cplusplus
}
#endif

#endif /* RFM_ANTENNA_TYPES_H */


