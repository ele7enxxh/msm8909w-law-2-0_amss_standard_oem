#ifndef RFWCDMA_CMD_PROC_H
#define RFWCDMA_CMD_PROC_H
/*!
  @file
  rfwcdma_cmd_proc.h

  @brief
  This module is reponsible for WCDMA command processing
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/mc/inc/rfwcdma_cmd_proc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
05/15/14   aro      Registered do_mtpl command
05/15/14   aro      Added cmd proceccsing module initialization and deinitzation funtions
05/01/14   aro     Initial Release

==================================================================================================*/

#include "comdef.h"

/*------------------------------------------------------------------------------------------------*/
boolean
rfwcdma_cmd_proc_init
(
  void
);

/*------------------------------------------------------------------------------------------------*/
boolean
rfwcdma_cmd_proc_deinit
(
  void
);

#endif /* RFWCDMA_CMD_PROC_H */
