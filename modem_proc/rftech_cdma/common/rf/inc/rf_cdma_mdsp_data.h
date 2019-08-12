#ifndef RF_CDMA_MDSP_DATA_H
#define RF_CDMA_MDSP_DATA_H
/*! 
  @file
  rf_cdma_mdsp_data.h
 
  @brief
  This file manages all the Data used by CDMA RF. This module is completely
  internal to this layer.
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_mdsp_data.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/16/12   sty     Initial Revision
 
==============================================================================*/

#include "comdef.h"
#include "rfcommon_locks.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Structure to hold data related to sequence number generation for 
    SW-FW communication */
 
typedef struct
{
  uint32 sequence_number;
  /*!< 
  @brief
  Most recent sequence number generated which was used in last message 
  sent to FW from FW side. 
   
  @details 
  Most recent sequence number generated which was used in last message 
  sent to FW from FW side. This is a 32bit number where 8 MSBs is used 
  to indicate the technology (copied from 8 MSBs from UMID). Remaning 24 LSBs 
  are used as common counter between all techs (1x and HDR).
  */ 

  rf_lock_data_type snum_lock;
  /*!< Semaphore lock to be used while accessing the sequence number */
} rf_cdma_mdsp_sum_data_type;

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_MDSP_DATA_H */
