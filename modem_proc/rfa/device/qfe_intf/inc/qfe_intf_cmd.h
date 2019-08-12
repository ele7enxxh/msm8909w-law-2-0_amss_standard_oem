#ifndef QFE_INTF_CMD_H
#define QFE_INTF_CMD_H

/*!
  @file
  qfe_intf_cmd.h

  @brief

  @details

*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.


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

===============================================================================*/

/*==============================================================================


                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/qfe_intf/inc/qfe_intf_cmd.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
10/04/12   vbh     Updated the rfe device type by removing qfe1320 (avoid redefinition)
09/28/12    kg     Clean up unused QFE1320 enum
06/21/12    vb     Added support for QFE1510
04/17/12   sn      Added support for QFE1320 therm read.
04/11/12   Saul    QFE. Initial support.
04/05/12   sn      QFE1320 code cleanup for IRAT
03/20/12   sn      Operation cleanup 
02/01/12   sn      Added new structure type for IRAT measurement
01/16/12   kg      Initial version.

============================================================================*/

#include "rfcom.h"
#include "rfdevice_wcdma_intf.h"
#include "fw_rf_irat_intf.h"
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device API command IDs for the ones going through the command dispatch. 
*/
/*----------------------------------------------------------------------------*/
typedef enum
{
  QFE_SET_THERM_ON,
  
  QFE_SET_THERM_OFF,
  
  QFE_GSM_GET_UPDATED_TXAGC_INFO,

  QFE_GSM_GET_PA_GAIN,

  QFE_GSM_GET_PA_CTL,

  QFE_WCDMA_GET_PA_CTL,

  QFE_CDMA_GET_PA_ONOFF_CTL,

  QFE_GET_PHYSICAL_BUS
}qfe_cmd_enum_type;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device type
*/
typedef enum
{
  DEFAULT_QFE,
  QTUNER1510, 
  QFE_MAX_NUM
} qfe_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
   IRAT measurement interface structure
*/
/*----------------------------------------------------------------------------*/
typedef struct 
{
  rfdevice_wcdma_meas_param_type* rfmeas_param_ptr;
  uint16* sbi_counter_ptr;
  rfmeas_sbi_type* sbi_script_ptr;
  uint16 max_size;
}qfe_meas_cmd_type;


#endif /* RFDEVICE_INTF_CMD_H */
