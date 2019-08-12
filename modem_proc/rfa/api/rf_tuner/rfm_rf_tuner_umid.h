#ifndef RFM_RF_TUNER_UMID_H
#define RFM_RF_TUNER_UMID_H

/*!
   @file
   rfm_rf_tuner_umid.h

   @brief
   RF Driver's RF Tuner external interface file to export RF Tuner MSGR UMIDs.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/rf_tuner/rfm_rf_tuner_umid.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/10/12   gh      Initial version to include RF Tuner MSGR UMIDs.

==============================================================================*/

#include "comdef.h"
#include <msgr.h>
#include <msgr_umid.h>
#include <rfa_msgr.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RFM_RF_TUNER_SET_SCENARIO_CMD 0x01
#define RFM_RF_TUNER_GET_SCENARIO_CMD 0x02
#define RFM_RF_TUNER_GET_REVISION_CMD 0x03

/*----------------------------------------------------------------------------*/
/*! @brief Enumeration of RFA RF Tuner Command UMIDs */
enum
{  
  RFA_RF_TUNER_BASE_CMD = MSGR_UMID_BASE(MSGR_RFA_RF_TUNER, MSGR_TYPE_CMD ),
  /*!< First message in the enum */

  MSGR_DEFINE_UMID( RFA, RF_TUNER, CMD, SET_SCENARIO, RFM_RF_TUNER_SET_SCENARIO_CMD, uint32 ),
  /*!< RFA_RF_TUNER_SET_SCENARIO_CMD: Command ID to set scenario */

  MSGR_DEFINE_UMID( RFA, RF_TUNER, CMD, GET_SCENARIO, RFM_RF_TUNER_GET_SCENARIO_CMD, uint32 ),
  /*!< RFA_RF_TUNER_GET_SCENARIO_CMD: Command ID to get scenarios */

  MSGR_DEFINE_UMID( RFA, RF_TUNER, CMD, GET_REVISION, RFM_RF_TUNER_GET_REVISION_CMD, uint32 ),
  /*!< RFA_RF_TUNER_GET_REVISION_CMD: Command ID to get revision and OEM string */
};

#ifdef __cplusplus
}
#endif

#endif /* RFM_RF_TUNER_UMID_H */


