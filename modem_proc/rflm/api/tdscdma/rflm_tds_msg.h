/*!
  @file
  rflm_tds_msg.h

  @brief
  This module is exports the messaging UMIDs for RFLM TDSCMDA
*/

/*==================================================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

==================================================================================================*/

/*==================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/tdscdma/rflm_tds_msg.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
06/05/15   whz     Add AutoPin support
03/17/15   whz     Add support for Therm trigger
           kb      Initial Release

==================================================================================================*/

#include "rflm_msg.h"
#include "msgr.h"
#include "msgr_umid.h"

/*------------------------------------------------------------------------------------------------*/
/*! Structure to defining the payload for Trigger PDET command*/
typedef struct
{
  int16 rflmTxagcdb10;
  /*!<  TxAGC*/

} rflm_tds_trigger_pdet;

/*------------------------------------------------------------------------------------------------*/
/*! Structure to defining Trigger PDET command*/
typedef struct 
{
  msgr_hdr_s msg_hdr;
  /*!< Message router header */

  rflm_tds_trigger_pdet msg_payload;
  /*!< Payload for trigger_pdet command */

} rflm_tds_trigger_pdet_cmd_t;

/*------------------------------------------------------------------------------------------------*/
/*! Structure to defining Trigger Thermal command*/
typedef struct 
{
  msgr_hdr_s msg_hdr;
  /*!< Message router header */

} rflm_tds_trigger_therm_cmd_t;

/*------------------------------------------------------------------------------------------------*/
/*! Structure to defining Trigger AutoPin command*/
typedef struct 
{
  msgr_hdr_s msg_hdr;
  /*!< Message router header */
} rflm_tds_trigger_autopin_cmd_t;

/*------------------------------------------------------------------------------------------------*/
/*! Enumeration for RFLM TDSCDMA UMID */
enum 
{
  MSGR_DEFINE_UMID( RFLM, TDSCDMA, CMD, TRIGGER_PDET, 0x00, rflm_tds_trigger_pdet_cmd_t )
  /*!< UMID definign the Trigger PDET  command */ 
};

enum 
{
  MSGR_DEFINE_UMID( RFLM, TDSCDMA, CMD, TRIGGER_THERM, 0x01, rflm_tds_trigger_therm_cmd_t )
  /*!< UMID definign the Trigger Therm  command */ 
};

enum 
{
  MSGR_DEFINE_UMID( RFLM, TDSCDMA, CMD, TRIGGER_AUTOPIN, 0x02, rflm_tds_trigger_autopin_cmd_t )
  /*!< UMID definign the Trigger AutoPin command */ 
};

