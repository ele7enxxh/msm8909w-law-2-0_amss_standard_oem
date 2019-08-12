/*!
  @file
  rfm_lte_earfcn.h

  @brief
  Utility functions for working with LTE EARFCNs, available for use by
  external modules.
*/
/*===========================================================================

  Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rfm_lte_earfcn.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/15/15   bmg     Initial version

===========================================================================*/

#ifndef RFM_LTE_EARFCN_H
#define RFM_LTE_EARFCN_H

#include "rflte_earfcn_types.h"

#define RFM_LTE_INVALID_EARFCN 0xFFFFFFFF

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type
rfm_lte_get_tx_earfcn_from_rx_earfcn(
  rfcom_lte_earfcn_type rx_earfcn
);

#ifdef __cplusplus
}
#endif

#endif /* RFM_LTE_EARFCN_H */

