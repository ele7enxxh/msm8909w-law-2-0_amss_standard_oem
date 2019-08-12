/*!
  @file msgr_ds.h

  @brief
   Describes the DS modules using the message router

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/msgr_ds.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/10   vrk     Added 3GPP module
04/23/10   vk      Added LTE module
08/09/09   vk      Initial version
===========================================================================*/

#ifndef __MSGR_DS_H__
#define __MSGR_DS_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include <msgr_umid.h>  /* UMID definitions */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


#define MSGR_MODULE_MSGRRECV          0x01
#define MSGR_MODULE_UT                0x02
#define MSGR_MODULE_LTE               0x03
#define MSGR_MODULE_3GPP              0x05
#define MSGR_MODULE_MGR               0x06

#define MSGR_DS_MSGRRECV            MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_MSGRRECV )
#define MSGR_DS_UT                  MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_UT )
#define MSGR_DS_LTE                 MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_LTE )
#define MSGR_DS_3GPP                MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_3GPP )
#define MSGR_DS_MGR                 MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_MGR )

#endif /* __MSGR_DS_H__ */
