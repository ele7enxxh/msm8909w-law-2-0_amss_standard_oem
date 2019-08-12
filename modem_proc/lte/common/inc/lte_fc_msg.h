/*!
  @file
  lte_fc_msg.h

  @brief
  This file contains all External message IDs interfaces exported by LTE FC.


*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/common/inc/lte_fc_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/10   ax      initial version
===========================================================================*/

#ifndef LTE_FC_MSG_H
#define LTE_FC_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr_lte.h>
#include <lte_app.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/

/*! @brief UMID for all indications sent by LTE FC
*/
enum
{
  MSGR_DEFINE_UMID(LTE, FC, IND, SHUTDOWN, 0x0, lte_fc_shutdown_ind_s),
  LTE_FC_MAX_IND,
  LTE_FC_LAST_IND = LTE_FC_MAX_IND - 1
} ;


#endif /* LTE_FC_MSG_H */
