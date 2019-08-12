#ifndef RFM_HDR_UMID_H
#define RFM_HDR_UMID_H

/*!
   @file
   rfm_hdr_umid.h

   @brief
   RF Driver's HDR external interface file to export HDR MSGR UMIDs.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/api/rfm_hdr_umid.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/18/12   hdz     Removed RF_RFA_HDR_TEMP_COMP_READ_CMD to rf_apps_task
03/08/12   aro     Updated Payload type for TempComp read message
03/08/12   aro     UMID to peform Temperature Compendation read
03/08/12   aro     Initial version to include HDR MSGR UMIDs.

==============================================================================*/

#include "comdef.h"
#include <msgr.h>
#include <msgr_umid.h>
#include <rfa_msgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/


/*! RF HDR set the coex Tx power limit req type */
/* note: this msg is sent from MCS */
typedef struct
{
  msgr_hdr_struct_type hdr;
	/* The RFM device */
	rfm_device_enum_type device ;
  /* SAR Tx Power limit in dBm x10 */
  int16                sar_tx_pwr_limit;
  /* Coex Tx Power limit in dBm x10 */
  int16                coex_tx_pwr_limit;
} rfa_hdr_set_coex_tx_pwr_limit_req_s;
 
 
/*! @brief ID to set HDR power limit REQ (note: this msg is for MCS)
 */
#define RFA_HDR_MSG_ID_SET_COEX_TX_PWR_LIMIT    0x1

/*! @brief Enumeration of RFA HDR Command UMIDs */
enum
{
  
  RFA_RF_HDR_BASE_CMD = MSGR_UMID_BASE( MSGR_RFA_RF_HDR, MSGR_TYPE_CMD ),
  /*!< First message in the enum */


  MSGR_DEFINE_UMID(RFA, RF_HDR , REQ, SET_COEX_TX_PWR_LIMIT, 
                   RFA_HDR_MSG_ID_SET_COEX_TX_PWR_LIMIT, 
                   rfa_hdr_set_coex_tx_pwr_limit_req_s),
  /*! RF set tx power limit message*/
};




#ifdef __cplusplus
}
#endif

#endif /* RFM_HDR_UMID_H */

