/*!
  @file
  rflm_c2k_msg.h

  @brief
  This module is exports the messaging UMIDs for RFLM CDMA
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved. By accepting this
  material the recipient agrees that this material and the information contained
  therein are held in confidence and in trust and will not be used, copied,
  reproduced in whole or in part, nor its contents revealed in any manner to
  others without the express written permission of Qualcomm Technologies
  Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/c2k/rflm_c2k_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/20/15   vr      Autopin check-in
08/20/15   cdb     Change autopin msg ID
06/26/15   wwl     Add support for Auto Pin

==============================================================================*/

#include "rflm_msg.h"
#include "msgr.h"
#include "msgr_umid.h"
#include "rflm_api_cmn.h"

/*----------------------------------------------------------------------------*/
/*! Structure to defining the payload for Trigger AutoPin command*/
typedef struct
{
  rflm_handle_tx_t tx_handle;
  uint8 pa_state;
  rflm_db10_t pin;

} rflm_c2k_trigger_autopin;

/*----------------------------------------------------------------------------*/
/*! Structure to defining Trigger AutoPin command*/
typedef struct
{
  msgr_hdr_s msg_hdr;
  /*!< Message router header */

  rflm_c2k_trigger_autopin msg_payload;

} rflm_c2k_trigger_autopin_cmd_t;

/*----------------------------------------------------------------------------*/
/*! Enumeration for RFLM CDMA UMIDs */
enum
{
  MSGR_DEFINE_UMID( RFLM, C2K, CMD, TRIGGER_AUTOPIN, 0x0,
                    rflm_c2k_trigger_autopin_cmd_t )
};

enum
{
  MSGR_DEFINE_UMID( RFLM, C2K, CMD, READ_AUTOPIN, 0x1,
                    rflm_c2k_trigger_autopin_cmd_t )
};

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Send MSGR command to update Pin

  @details
  Sends RF command through msgr to update Pin
*/
void
rflm_cdma_update_pin
(
  rflm_handle_tx_t handle_id,
  uint8 pa_state,
  rflm_db10_t pin
);

