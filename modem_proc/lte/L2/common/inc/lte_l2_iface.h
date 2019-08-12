/*!
  @file
  lte_l2_iface.h

  @brief
  This file contains common defines and functions interfaces that L2 layer
  exposes to other modules within LTE.


*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/common/inc/lte_l2_iface.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/15   mg      CR904992: EMBMS DPD tradeoff G and 1x paging
06/30/15   mg      CR863356: L2 not making reservation when unlock is 
                   successful and there is no collision
06/14/15   mg      CR832574: EMBMS DPD two client implementation
05/25/15   mg      Initial version
===========================================================================*/

#ifndef LTE_L2_IFACE_H
#define LTE_L2_IFACE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lte_variation.h"
#include <comdef.h>
#include <msgr.h>
#include <lte_l2_comdef.h>
#include <lte_assert.h>
#include <modem_mem.h>
#include <trm.h>
/*===========================================================================

                               MACROS

===========================================================================*/

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief definition of priorities of the unlock request
*/
typedef enum
{
  LTE_RLCDL_EMBMS_LOWER_THAN_EMBMS_LOW = 0,
  LTE_RLCDL_EMBMS_BETWEEN_EMBMS_LOW_AND_HIGH,
  LTE_RLCDL_EMBMS_EQUALS_EMBMS_HIGH,
  LTE_RLCDL_EMBMS_HIGHER_THAN_EMBMS_HIGH,
  LTE_RLCDL_EMBMS_UNLOCK_REQ_PRIORITY_MAX /*!< MAX */
} lte_rlcdl_embms_unlock_req_pri_e;

/*! @brief RLC - ML1 structure to get unlock information
*/
typedef struct
{
  /*! When is unlock requested - 
      in the unit of subframe from last SF_IND */
  lte_sfn_s                           unlock_by_start_time;

  /*! How long is unlock requested for. In units of msecs */
  uint16                              unlock_by_duration;

  /*! Unlock requested for which client. 0 - 1x 8 - GSM*/
  uint8                               winning_client_id;

  /*! Priority of the winning client*/
  lte_rlcdl_embms_unlock_req_pri_e    unlock_priority;

  /*! Whether LTE is going to unlock now or sometime in the future.
   *  Set to true just before tuneaway and by LTE. Set to false only
   *  for scheduling the unlock by TAM
   */
  boolean                             unlock_now;
  /*! Unlock reason of the winning client
  */
  trm_reason_enum_t                   unlock_reason;
} lte_rlcdl_embms_honor_unlock_req_s;

/*! @brief RLC - ML1 structure to provide honoring unlock information
*/
typedef struct
{
  /*! Whether to unlock or not*/
  boolean                             honor_unlock;
  /*! Time to which the unlock should be defered.
      Valid only if honor_unlock is false*/
  lte_sfn_s                           defer_sfn;
}lte_rlcdl_embms_honor_unlock_rsp_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlcdl_embms_honor_unlock

==============================================================================*/
/*!
    @brief
    This function returns whether the unlock can be honored or not.

    @detail
    If the unlock cannot be honored immediately then this function provides the
	information about the latest sfn at which the unlock can be done. Usually
	the end of the current burst

    @return
    Boolean to honor unlock. If not honored SFN to which the unlock can be deferred to

*/
/*============================================================================*/
extern lte_rlcdl_embms_honor_unlock_rsp_s lte_rlcdl_embms_honor_unlock
(
  lte_rlcdl_embms_honor_unlock_req_s unlock_req
);

#endif /* LTE_L2_IFACE_H */

