#ifndef RFM_CDMA_CARRIER_TYPES_H
#define RFM_CDMA_CARRIER_TYPES_H

/*!
  @file
  rfm_cdma_carrier_types.h
 
  @brief
  Definitions of types pertaining to CDMA Carrier
 
  @details
  This file exports the definitions of types that are used in the RFM
  interface relating to CDMA Carrier.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_cdma_carrier_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/13/11   aro     Added MC support for HDR Wakeup Rx Interface
01/18/11   aro     [1] Added Number of carrier
                   [2] Added Carrier Mask Structure
12/20/10   aro     Initial version

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Identifies individual carriers of a multi-carrier CDMA radio.
*/
typedef enum
{

  RFM_CDMA_CARRIER_0,
  /*!< Carrier 0 */

  RFM_CDMA_CARRIER_1,
  /*!< Carrier 1 */

  RFM_CDMA_CARRIER_2,
  /*!< Carrier 2 */

  RFM_CDMA_CARRIER_CUMULATIVE,
  /*!< Composite of all active carriers taken together */

  RFM_CDMA_CARRIER_NUM = RFM_CDMA_CARRIER_CUMULATIVE,
  /*!< Total number of Carriers available */
  
  RFM_HDR_CARRIER_ID__0 = RFM_CDMA_CARRIER_0,
  /*!< Carrier 0 */

  RFM_HDR_CARRIER_ID__1 = RFM_CDMA_CARRIER_1,
  /*!< Carrier 1 */

  RFM_HDR_CARRIER_ID__2 = RFM_CDMA_CARRIER_2,
  /*!< Carrier 2 */

  RFM_HDR_CARRIER_ID__CUMULATIVE = RFM_CDMA_CARRIER_CUMULATIVE
  /*!< Composite of all active carriers taken together */

} rfm_cdma_carrier_type;

/*----------------------------------------------------------------------------*/
/*! Typedef to support Legacy Enum */
typedef rfm_cdma_carrier_type rfm_hdr_carrier_id_type;

/*----------------------------------------------------------------------------*/
/*! Structure indicating which carrier is enabled. Index 0 refers to Carrier
0 and so on */
typedef struct
{

  boolean enable[RFM_CDMA_CARRIER_NUM]; /*!< Flag for active carrier */

} rfm_cdma_carrier_en_mask_type;

/*----------------------------------------------------------------------------*/
/*! Maximum number of carrier supported by CDMA */
#define RFM_CDMA_MAX_CARRIER RFM_CDMA_CARRIER_CUMULATIVE

#ifdef __cplusplus
}
#endif

#endif /* RFM_CDMA_CARRIER_TYPES_H */

