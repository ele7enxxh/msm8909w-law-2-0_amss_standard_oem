#ifndef RFM_1X_UMID_H
#define RFM_1X_UMID_H

/*!
   @file
   rfm_1x_umid.h

   @brief
   RF Driver's 1x external interface file to export 1x MSGR UMIDs.
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/api/rfm_1x_umid.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/03/14   cc      Fix rf_cdma_update_agc crash when it's called
04/18/12   hdz     Removed RFA_RF_1X_TEMP_COMP_READ
03/26/12   hdz     Added msg for Reloading tx/rx agc 
03/08/12   aro     Updated Payload type for TempComp read message
03/08/12   aro     UMID to peform Temperature Compendation read
03/08/12   aro     Initial version to include 1x MSGR UMIDs.

==============================================================================*/

#include "comdef.h"
#include <msgr.h>
#include <msgr_umid.h>
#include <rfa_msgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/

#define RFA_RF_1X_TXRX_AGC_RELOAD              0x01
/*! @brief ID to set 1X power limit REQ (note: this msg is for MCS)
 */
#define RFA_1X_MSG_ID_SET_COEX_TX_PWR_LIMIT    0x02


typedef struct
{
  msgr_hdr_struct_type hdr;
} rfa_1x_txrx_agc_reload_cmd_t;


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
} rfa_1x_set_coex_tx_pwr_limit_req_s;
 
 

/*! @brief Enumeration of RFA 1x Command UMIDs */
enum
{
  
  RFA_RF_1X_BASE_CMD = MSGR_UMID_BASE( MSGR_RFA_RF_1X, MSGR_TYPE_CMD ),
  /*!< First message in the enum */

  MSGR_DEFINE_UMID( RFA, RF_1X, CMD, TXRX_AGC_RELOAD, 
                    RFA_RF_1X_TXRX_AGC_RELOAD,
                    rfa_1x_txrx_agc_reload_cmd_t ),
  /*!< Command to perform 1x Temperature compensation read */

  MSGR_DEFINE_UMID(RFA, RF_1X , REQ, SET_COEX_TX_PWR_LIMIT, 
                   RFA_1X_MSG_ID_SET_COEX_TX_PWR_LIMIT, 
                   rfa_1x_set_coex_tx_pwr_limit_req_s),
  /*! RF set tx power limit message*/
};

#ifdef __cplusplus
}
#endif

#endif /* RFM_1X_UMID_H */

