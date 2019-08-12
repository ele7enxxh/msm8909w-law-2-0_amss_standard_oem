#ifndef FTM_MSGR_H
#define FTM_MSGR_H
/*!
  @file ftm_msgr.h

  @brief
   Describes the modules under Tech - FTM. 

  Details...
  
*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/ftm_msgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/12   ggs     Added MSGR_FTM_GSM module 
05/09/11   sar     Fixed featurizaton to cleanup rfa/api.
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
04/23/09   pl      Adding LTE Non-signaling module
02/25/09   mkv     Initial Check-in
===========================================================================*/
#include <comdef.h>    /* Definition of basic types and macros */
//#include "ftmicap.h"

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include <msgr_umid.h>  /* UMID definitions */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


#define MSGR_MODULE_CORE     0x01
#define MSGR_MODULE_LTE_NS   0x02
#define MSGR_MODULE_GSM      0x03

/* Create additional modules*/

#define MSGR_FTM_CORE    MSGR_TECH_MODULE(MSGR_TECH_FTM, MSGR_MODULE_CORE)
#define MSGR_FTM_LTE_NS  MSGR_TECH_MODULE(MSGR_TECH_FTM, MSGR_MODULE_LTE_NS)
#define MSGR_FTM_GSM     MSGR_TECH_MODULE(MSGR_TECH_FTM, MSGR_MODULE_GSM)

#endif /* FTM_MSGR_H */

