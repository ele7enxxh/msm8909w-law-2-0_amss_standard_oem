/*!
  @file
  rflte_earfcn_util.h

  @brief
  Utility functions for operating on LTE EARFCNs.
*/

/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rflte_earfcn_util.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/12/16   bmg     Added rflte_util_get_rx_band_range()
12/22/15   bmg     Created
===========================================================================*/

#ifndef RFLTE_EARFCN_UTIL_H
#define RFLTE_EARFCN_UTIL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rflte_earfcn_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
typedef uint32 rflte_freq_khz_type;

/*----------------------------------------------------------------------------*/
rflte_freq_khz_type
rflte_util_get_freq_from_earfcn(
  rfcom_lte_earfcn_type earfcn
);

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type
rflte_util_get_earfcn_from_freq(
  rflte_freq_khz_type freq_khz,
  rfcom_lte_band_type band_hint
);

/*----------------------------------------------------------------------------*/
uint32
rflte_util_get_integer_band_from_earfcn(
  rfcom_lte_earfcn_type earfcn
);

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type
rflte_util_get_rx_earfcn_from_tx_earfcn(
  rfcom_lte_earfcn_type tx_earfcn
);

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type
rflte_util_get_tx_earfcn_from_rx_earfcn(
  rfcom_lte_earfcn_type rx_earfcn
);

/*----------------------------------------------------------------------------*/
boolean
rflte_util_get_tx_band_range(
  const uint32 integer_band,
  rflte_freq_khz_type* low_freq_khz,
  rflte_freq_khz_type* high_freq_khz
);

/*----------------------------------------------------------------------------*/
boolean
rflte_util_get_rx_band_range(
  const uint32 integer_band,
  rflte_freq_khz_type* low_freq_khz,
  rflte_freq_khz_type* high_freq_khz
);

/*-----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type
rflte_util_get_min_dl_earfcn(
  const uint32 integer_band
);

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type
rflte_util_get_absolute_rx_earfcn_from_offset(
  const uint16 offset_earfcn,
  const int integer_band
);

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type
rflte_util_get_absolute_tx_earfcn_from_offset(
  const uint16 offset_earfcn,
  const int integer_band
);

/*----------------------------------------------------------------------------*/
boolean
rflte_util_is_valid_earfcn(
  rfcom_lte_earfcn_type earfcn
);

/*----------------------------------------------------------------------------*/
boolean
rflte_util_is_valid_earfcn_for_band(
  rfcom_lte_earfcn_type earfcn,
  const int integer_band
);

/*----------------------------------------------------------------------------*/
boolean
rflte_util_is_valid_tx_earfcn_for_band(
  rfcom_lte_earfcn_type earfcn,
  const int integer_band
);

#ifdef __cplusplus
}
#endif

#endif /* RFLTE_EARFCN_UTIL_H */

