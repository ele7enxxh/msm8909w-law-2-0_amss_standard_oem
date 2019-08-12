/*!
  @file
  rflm_msg.h

  @brief
  Describes all the mdoules using message router under Tech RFLM. 

  @details
  This file contains all Tech_Module definitions under Tech RFLM
  that needs to use the message router. RFLM driver of all modes
  (LTE, Wimax, 1x, HDR, WCDMA, GSM, GPS) that needs to use the 
  mesasge router needs to define the Tech_Module in this file in order
  to use the message router. This is a common header for rflm of all modes.
  
*/


/*===========================================================================

  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/cmn/rflm_msg.h#1 $ 
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/21/14   ch      Initial checkin RFLM msgr common definition
===========================================================================*/
 
#ifndef RFLM_MSG_H
#define RFLM_MSG_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/
#include "msgr.h"  

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

#define MSGR_MODULE_RFLM_CMN     0x00
#define MSGR_MODULE_RFLM_GSM     0x01
#define MSGR_MODULE_RFLM_C2K     0x02
#define MSGR_MODULE_RFLM_WCDMA   0x03
#define MSGR_MODULE_RFLM_LTE     0x04
#define MSGR_MODULE_RFLM_TDSCDMA 0x05

#define MSGR_RFLM_CMN         MSGR_TECH_MODULE( MSGR_TECH_RFLM, MSGR_MODULE_RFLM_CMN )
#define MSGR_RFLM_GSM         MSGR_TECH_MODULE( MSGR_TECH_RFLM, MSGR_MODULE_RFLM_GSM )
#define MSGR_RFLM_C2K         MSGR_TECH_MODULE( MSGR_TECH_RFLM, MSGR_MODULE_RFLM_C2K )
#define MSGR_RFLM_WCDMA       MSGR_TECH_MODULE( MSGR_TECH_RFLM, MSGR_MODULE_RFLM_WCDMA )
#define MSGR_RFLM_LTE         MSGR_TECH_MODULE( MSGR_TECH_RFLM, MSGR_MODULE_RFLM_LTE )
#define MSGR_RFLM_TDSCDMA     MSGR_TECH_MODULE( MSGR_TECH_RFLM, MSGR_MODULE_RFLM_TDSCDMA )

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*-----------------------------------------------------------------------*/


#endif /* RFLM_MSG_H */
