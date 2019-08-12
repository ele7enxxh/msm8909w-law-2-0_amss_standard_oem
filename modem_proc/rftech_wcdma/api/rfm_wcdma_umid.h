#ifndef RFM_WCDMA_UMID_H
#define RFM_WCDMA_UMID_H

/*!
   @file
   rfm_wcdma_umid.h

   @brief
   RF Driver's WCDMA external interface file to export WCDMA MSGR UMIDs.
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfm_wcdma_umid.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/06/14   ak     Initial version to include WCDMA MSGR UMIDs and WCDMA dispatcher function for RF Apps Task .

==============================================================================*/

#include "comdef.h"
#include <msgr.h>
#include <msgr_umid.h>
#include <rfa_msgr.h>

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
/*! List of WCDMA commands IDs */
enum
{
  RFA_WCDMA_MSG_ID_TEMP_COMP_UPDATE = 0x1
};

/*----------------------------------------------------------------------------*/
/*! Structure holding the Payload data for Temp Comp Update data structure */
typedef struct
{

  uint32 magic_number;
  /*!< Dummy Variable */

} rfa_wcdma_temp_comp_update_req_s;

/*----------------------------------------------------------------------------*/
/*! Enumeration of RFA WCDMA Command UMIDs */
enum
{
  
  RFA_RF_WCDMA_BASE_CMD = MSGR_UMID_BASE( MSGR_RFA_RF_WCDMA, MSGR_TYPE_CMD ),
  /*!< First message in the enum */


  MSGR_DEFINE_UMID(RFA, RF_WCDMA , CMD, TEMP_COMP_UPDATE, 
                   RFA_WCDMA_MSG_ID_TEMP_COMP_UPDATE, 
                   rfa_1x_set_coex_tx_pwr_limit_req_s),
  /*! RF set tx power limit message*/
};

#ifdef __cplusplus
}
#endif

#endif /* RFM_WCDMA_UMID_H */



