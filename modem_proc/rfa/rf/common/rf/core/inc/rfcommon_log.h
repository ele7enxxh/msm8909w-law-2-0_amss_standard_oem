#ifndef RFCOMMON_LOG_H
#define RFCOMMON_LOG_H

/*!
   @file
   rfcommon_log.h

   @brief
   This file contains headers for RF log packets.
 
   @addtogroup RF_LOG_DEFN_COMMON
   @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_log.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/27/12   spa     Moved log packet header to RF header
03/26/12   spa     Initial version

==============================================================================*/

#include "comdef.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*!
  @name RF Header

  @brief
  Definitions for RF Header
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Shared header structure for all RF log packets.

  @details
  This structure is the header for the RF log packet, and is a shared structure 
  used by all RF log packets it is followed by subpackets with their respective 
  headers. */ 

typedef PACK(struct)
{
  
  /*! Log packet header, this contains information about the length of log 
  record, the log code and the timestamp associated with the packet */ 
  log_hdr_type log_header;

  /*! The version of the log packet. Set to one for RF log packets. All other 
      values are reserved for future field configurations */ 
  uint8 pkt_version;

  /*! Number of sub-packets in the RF Log packet*/
  uint8 num_sub_pkt;

  /*! Reserved */
  uint16 hdr_reserved;

} rfcommon_log_rf_header_type;

/*! @} */

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* RFCOMMON_LOG_H */
