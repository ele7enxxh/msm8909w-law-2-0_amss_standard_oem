#ifndef RF_CDMA_NV_BC6_H
#define RF_CDMA_NV_BC6_H

/*!
  @file
  rf_cdma_nv_bc6.h

  @details
  This file exports the definitions and declartions necessary inorder to
  interface with the RF 1X NV Manager for Band Class 6
*/

/*==============================================================================

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_nv_bc6.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/14/12   cd      Remove unused old NV function
03/22/12   id      Added support for converting Tx freq to chan 
08/30/11   cd      Add loading of multi-linearizer NV using common functionality
08/09/11   cd      Cleaned up unused legacy LNA threshold parameters. Moved 
                   definitions to source file.
08/02/11   cd      Moved band-specific NV loading functions to common file in 
                   order to remove redundancy. Only chain-specific 
06/10/11   cd      Changed static NV items map to array of structs of size 
                   RF_CDMA_NV_PATH_NUM, so get API takes path as parameter
06/06/11   cd      Changed cal items map to array of structs, so get API takes 
                   path as parameter
06/02/11   cd      Added static NV items table 
05/24/11   cd      Added support for loading Tx and Rx NV items for SV path
04/21/11   cd      Removed old NV structure LNA offsets
03/22/11   bmg     Added inital NV loading for the new config structure.
02/25/11   bmg     Removed CDMA NV items that are no longer used or will be
                   replaced with new functionality.
02/24/11   aro     Renamed type name to rf_cdma_nv_band_items_type
02/01/11   aro     Added support for NV Commit
12/29/10   aro     Mainlined RF_HAS_POWER_MODE_LIB
12/29/10   aro     Added NV Table Accessor function
12/16/10   aro     Doxygen Update
12/14/10   aro     Removed unused NV functions 
12/09/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_nv.h"
                                    
/*----------------------------------------------------------------------------*/
rf_cdma_cal_nv_items_map_type*
rf_cdma_nv_get_bc6_calibration_table
(
  rf_cdma_nv_path_type nv_path
);

/*----------------------------------------------------------------------------*/  
rf_cdma_static_nv_items_map_type*
rf_cdma_nv_get_bc6_static_nv_table
(
  rf_cdma_nv_path_type nv_path
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_nv_bc6_convert_chan_to_tx_freq
(
  uint16 channel
);

/*----------------------------------------------------------------------------*/
uint16
rf_cdma_nv_bc6_convert_tx_freq_to_chan
(
  uint32 freq
);

#endif /* RF_CDMA_NV_BC6_H */

