/*!
  @file
  gfw_sw_intf_common.h

  @brief
  Definition of basic GFW/SW/RF interface constants and types

  @details
  This file contains basic constants and types which are used in other GERAN
  API headers. This file must not include other interface files. 

*/

#ifndef GFW_SW_INTF_COMMON_H
#define GFW_SW_INTF_COMMON_H
/*===========================================================================

  Copyright (c) 20012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_geran/api/gfw_sw_intf_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/* Maximum number of DL and UL timeslots per TDMA frame. */
#define MDSP_MAX_DL_TS  (5)
#define MDSP_MAX_UL_TS  (4)

/* Maximum number of standalone (i.e. issued without any Rx and Tx in the same
   frame) RSSI measurements per TDMA frame. The definition of
   MDSP_MAX_NON_STANDALONE_RSSI_MEASUREMENTS is in gfw_sw_rf_intf.h */
#define MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS (12)


/* end of file gfw_sw_intf_common.h */
#endif /* GFW_SW_INTF_COMMON_H */
