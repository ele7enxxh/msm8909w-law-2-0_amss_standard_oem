#ifndef RFM_CDMA_BAND_TYPES_H
#define RFM_CDMA_BAND_TYPES_H

/*!
  @file
  rfm_cdma_band_types.h
 
  @brief
  Band class definitions as specified by 3GPP2 C.S0057
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_cdma_band_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/26/11   shb     Added rfm_cdma_chan_type
02/22/11   bmg     Initial revision.

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*!
  @brief
  Band class definitions as specified by 3GPP2 C.S0057.
*/
typedef enum rfm_band_class_e
{
  RFM_CDMA_BC0  = 0,  /*!< 800 MHz Band                             */
  RFM_CDMA_BC1  = 1,  /*!< 1900 MHz Band                            */
  RFM_CDMA_BC2  = 2,  /*!< TACS Band                                */
  RFM_CDMA_BC3  = 3,  /*!< JTACS Band                               */
  RFM_CDMA_BC4  = 4,  /*!< Korean PCS Band                          */
  RFM_CDMA_BC5  = 5,  /*!< 450 MHz Band                             */
  RFM_CDMA_BC6  = 6,  /*!< 2 GHz Band                               */
  RFM_CDMA_BC7  = 7,  /*!< Upper 700 MHz Band                       */
  RFM_CDMA_BC8  = 8,  /*!< 1800 MHz Band                            */
  RFM_CDMA_BC9  = 9,  /*!< 900 MHz Band                             */
  RFM_CDMA_BC10 = 10, /*!< Secondary 800 MHz Band                   */
  RFM_CDMA_BC11 = 11, /*!< 400 MHz European PAMR Band               */
  RFM_CDMA_BC12 = 12, /*!< 800 MHz PAMR Band                        */
  RFM_CDMA_BC13 = 13, /*!< 2.5 GHz IMT-2000 Extension Band          */
  RFM_CDMA_BC14 = 14, /*!< US PCS 1.9GHz Band                       */
  RFM_CDMA_BC15 = 15, /*!< AWS Band                                 */
  RFM_CDMA_BC16 = 16, /*!< US 2.5GHz Band                           */
  RFM_CDMA_BC17 = 17, /*!< US 2.5GHz Forward Link Only Band         */
  RFM_CDMA_BC18 = 18, /*!< 700 MHz Public Safety Band               */
  RFM_CDMA_BC19 = 19, /*!< Lower 700 MHz Band                       */
  RFM_CDMA_BC20 = 20, /*!< L-Band                                   */

  RFM_CDMA_MAX_BAND   /*!< Terminal value for the enum, not a valid
                           band                                     */
} rfm_cdma_band_class_type;

/*!
  @brief
  CDMA Channel number. uint16 type to be used throughout the driver
*/
typedef uint16 rfm_cdma_chan_type;

#ifdef __cplusplus
}
#endif

#endif /* RFM_CDMA_BAND_TYPES_H */

