#ifndef __BATT_NPA_H__
#define __BATT_NPA_H__
/*!
  @file batt_npa.h

  @brief
   NPA support for Modem BCL
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/batt/inc/batt_npa.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/07/13   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "cxm.h"

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  batt_npa_send_notification

=============================================================================*/
/*!
    @brief
    Stores a BATTERY CURRENT LIMITING record in the global LUT list.
 
    @return
    None
*/
/*===========================================================================*/
void batt_npa_send_notification
(
  cxm_tech_type tech1,
  cxm_tech_type tech2
);

/*=============================================================================

  FUNCTION:  batt_npa_init

=============================================================================*/
/*!
    @brief
    Registers with NPA for the resource available callback
 
    @return
    None
*/
/*===========================================================================*/
void batt_npa_init(void);

#endif /* __BATT_NPA_H__ */
