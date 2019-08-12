#ifndef RFM_WCDMA_FTM_H
#define RFM_WCDMA_FTM_H

/*!
   @file
   rfm_wcdma_ftm.h

   @brief
   RF Driver's WCDMA interface file for FTM.
*/

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/mc/inc/rfm_wcdma_ftm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/06/14   aro     Support to override TempComp mode of operation
05/15/14   aro     Interface to override TxPlim override mode
02/04/14   vbh     Initial revision 
10/01/13   aro     Added interface to query debug flags
09/30/13   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfwcdma_data.h"
#include "rfm_device_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*----------------------------------------------------------------------------*/
/*! Structure to hold the rxagc information in FTM context */
typedef struct
{
  int16 rxagc_val;
  /*!< RxAGC value in units of dBm10 */

  uint8 lna_state;
  /*!< LNA Gain state */

  uint8 lna_word;
  /*!< LNA Gain word */

  boolean pll_lock_status;
  /*!< Flag to indicate the PLL lock status */

} rfm_wcdma_ftm_rxagc_info;
 /*!< AGC information holder per device*/

/*----------------------------------------------------------------------------*/
rfwcdma_data_debug_flags_type*
rfm_wcdma_ftm_get_debug_flags
(
  void
);

/*----------------------------------------------------------------------------*/

boolean
rfm_wcdma_ftm_get_rxagc_info
(
  uint8 device,
  uint8 multicarrier_idx,
  uint8 agc_unit,
  rfm_wcdma_ftm_rxagc_info *agc_now
);

/*----------------------------------------------------------------------------*/
boolean
rfm_wcdma_ftm_plim_override_mode
(
  rfm_device_enum_type device,
  boolean plim_enable
);

/*----------------------------------------------------------------------------*/
boolean
rfm_wcdma_ftm_temp_comp_override
(
  rfm_device_enum_type device,
  boolean enable_tempcomp
);


#ifdef __cplusplus
}
#endif

#endif /* RFM_WCDMA_FTM_H */


