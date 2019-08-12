/*!
  @file
  rflm_tds_msg_proc.h

  @brief
  This module is reponsible for handling handling CMDs sent to RF Tasks from RFLM
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/tdscdma/inc/rflm_tds_msg_proc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
10/06/15   whz     Add support to AutoPin script
03/17/15   whz     Add support for Therm trigger
05/01/14   kb      Initial Release

==================================================================================================*/

#ifndef RFLM_TDS_MSG_PROC_H
#define RFLM_TDS_MSG_PROC_H

#include "rflm.h"
#include "comdef.h"
#include "rflm_api_cmn.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------------------------*/
void
rflm_tds_msgr_send_pdet_trigger
(
  void
);

void
rflm_tds_msgr_send_therm_trigger
(
  void
);

void
rflm_tds_msgr_send_autopin_trigger();


#ifdef __cplusplus
}
#endif


#endif
