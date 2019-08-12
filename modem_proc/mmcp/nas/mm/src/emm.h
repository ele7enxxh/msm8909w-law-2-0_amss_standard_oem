
/*!
  @file
  lte_rrc_ext_msg.h

  @brief
  LTE RRC external interfaces.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/28/09   vdr     Fixed compiler warnings
06/14/10   RI      Use of global emm_ctrl_data_ptr removed.
                     - must be accessed using function
                        emm_db_get_ctrl_data().
===========================================================================*/

#ifndef _EMM_H_
#define _EMM_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/
#include "emm_database.h"

#endif /* _EMM_H_ */

#endif /*FEATURE_LTE*/
