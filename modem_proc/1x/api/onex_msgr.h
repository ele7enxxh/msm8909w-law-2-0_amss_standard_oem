/*!
  @file msgr_1x.h

  @brief
   Describes the modules using the message router

  Details...

*/

/*===========================================================================

  Copyright (c) 2009 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/onex_msgr.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/12/15   jh      Add MSGR_ONEX_MUXMDSP client macro
08/05/10   vks     Move TX, RX MSGR module definitions from muxmsgr.h to
                   onex_msgr.h
07/13/10   vks     Use macros to define module ids for SRCH, CP
11/24/09   vks     Use 0x3 as module id for SRCH and 0x4 for CP
11/24/09   kwo     Initial Checkin
===========================================================================*/

#ifndef __MSGR_1X_H__
#define __MSGR_1X_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include <msgr_umid.h>  /* UMID definitions */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* MSGR MODULE IDs FOR 1X TASKS */
#define MSGR_MODULE_ONEX_TX_SW      0x00

#define MSGR_MODULE_ONEX_RX_SW      0x01

#define MSGR_MODULE_ONEX_SRCH_SW    0x02

#define MSGR_MODULE_ONEX_CP_SW      0x03

#define MSGR_MODULE_ONEX_MUXMDSP_SW 0x04


/* Create TECH MODULE MACRO FOR ONEX TASKS */
#define MSGR_ONEX_TX   MSGR_TECH_MODULE(MSGR_TECH_ONEX, MSGR_MODULE_ONEX_TX_SW)

#define MSGR_ONEX_RX   MSGR_TECH_MODULE(MSGR_TECH_ONEX, MSGR_MODULE_ONEX_RX_SW)

#define MSGR_ONEX_SRCH MSGR_TECH_MODULE(MSGR_TECH_ONEX, MSGR_MODULE_ONEX_SRCH_SW)

#define MSGR_ONEX_CP   MSGR_TECH_MODULE(MSGR_TECH_ONEX, MSGR_MODULE_ONEX_CP_SW)

#define MSGR_ONEX_MUXMDSP MSGR_TECH_MODULE(MSGR_TECH_ONEX, MSGR_MODULE_ONEX_MUXMDSP_SW)


#endif /* __MSGR_1X_H__ */
