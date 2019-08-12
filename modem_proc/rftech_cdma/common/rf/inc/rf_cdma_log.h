#ifndef RF_CDMA_LOG_H
#define RF_CDMA_LOG_H

/*!
   @file
   rf_cdma_log.h

   @brief
   This file contains structures for RF log sub-packets specific to CDMA.
 
   @addtogroup RF_LOG_DEFN_CDMA
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_log.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/18/12   spa     Removed rf_cdma_log_is_enabled 
04/09/12   spa     Added prototype for  rf_cdma_log_is_enabled()
04/09/12   spa     Moved subpackets to 1x and HDR specific log header files
03/26/12   spa     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfcommon_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*!
  @name CDMA Sub Packet Header

  @brief
  Definitions for CDMA sub-packet header
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Header structure for RF log sub-packets.

  @details
  This structure is the header for any RF sub-packet, contains information about 
  the payload that follows
*/
typedef PACK(struct)
{
  /*! Sub-packet ID, a unique sub-packet identifier*/ 
  uint8 pkt_id;

  /*! Sub-packet version number, if the version is unknown to the parsing tool 
      the sub-packet will be skipped */ 
  uint8 pkt_ver;

  /*! Sub-packet size, size in bytes of the sub-packet, including the header. If 
      the version of the sub-packet is unknown this size can be used to skip the 
      subpacket and decode the next one */ 
  uint16 pkt_size;

} rf_cdma_log_sub_pkt_header_type;

/*! @} */

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* RF_CDMA_LOG_H */
