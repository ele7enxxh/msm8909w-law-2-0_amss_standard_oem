/*!
  @file lte_rrc_pendqi.h

  @brief
  Header defs for RRC pending queue implementation - internal.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_pendqi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/10   vc      Initial Revision. Internal definitions to be accessed by tests

===========================================================================*/

#ifndef LTE_RRC_PENDQI_H
#define LTE_RRC_PENDQI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>
#include <queue.h>
#include "lte_rrc_int_msg.h"
#include "lte_rrc_pendq.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_pendq.c
*/
typedef struct
{
  /*!< Queue items */
  lte_rrc_pendq_queue_item_s queue_items[LTE_RRC_PENDQ_MAX_QUEUE_LEN];

  /*!< Queue of pended messages */
  q_type pend_queue;

  /*!< Queue of initialized links that are free to be used */
  q_type free_queue;

} lte_rrc_pendq_s;


#endif /* LTE_RRC_PENDQI_H */
