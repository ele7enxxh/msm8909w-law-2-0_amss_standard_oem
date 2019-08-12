/*!
  @file msgr_target_test.h

  @brief  Header file for Message Router Test APIs for on-target

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/msgr/inc/msgr_target_test.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/29/10   gbs     added include and defines
07/06/10   ejv     Initial Revision

===========================================================================*/

#ifndef __MSGR_TARGET_TEST_H__
#define __MSGR_TARGET_TEST_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "mcs_variation.h"
#include <comdef.h>
#include <customer.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define MCS_APP_PROC_Q6SW   0x00
#define MCS_APP_PROC_Q6FW   0x01

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
#ifdef FEATURE_MODEM_TARET_BUILD
#ifdef FEATURE_MSGR_TARGET_TEST
void msgr_test_mcs_app( void );
#endif /* FEATURE_MSGR_TARGET_TEST */
#endif /* FEATURE_MODEM_TARET_BUILD */

#endif /* __MSGR_TARGET_TEST_H__ */
