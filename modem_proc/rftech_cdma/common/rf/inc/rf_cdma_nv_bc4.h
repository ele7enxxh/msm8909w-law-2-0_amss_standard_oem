#ifndef RF_CDMA_NV_BC4_H
#define RF_CDMA_NV_BC4_H

/*!
  @file
  rf_cdma_nv_bc4.h

  @details
  This file exports the definitions and declartions necessary inorder to
  interface with the RF 1X NV Manager for Band Class 4
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_nv_bc4.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/14/12   cd      Remove unused old NV function
03/22/12   id      Added support for converting Tx freq to chan 
09/20/11   cd      Initial Release.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_nv.h"
                                    
/*----------------------------------------------------------------------------*/
rf_cdma_cal_nv_items_map_type*
rf_cdma_nv_get_bc4_calibration_table
(
  rf_cdma_nv_path_type nv_path
);

/*----------------------------------------------------------------------------*/  
rf_cdma_static_nv_items_map_type*
rf_cdma_nv_get_bc4_static_nv_table
(
  rf_cdma_nv_path_type nv_path
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_nv_bc4_convert_chan_to_tx_freq
(
  uint16 channel
);

/*----------------------------------------------------------------------------*/
uint16
rf_cdma_nv_bc4_convert_tx_freq_to_chan
(
  uint32 freq
);

#endif /* RF_CDMA_NV_BC4_H */

