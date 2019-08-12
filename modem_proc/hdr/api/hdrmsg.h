/*!
  @file hdrmsg.h
  Defines the external message interface for the HDRSW module.
*/

/*===========================================================================

  Copyright (c) 2009-2013 Qualcomm Technologies Incorporated. All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/api/hdrmsg.h#1 $user

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/22/13   arm     Added MSGR_HDR_MULTIRAT.
01/04/12   rmg     Added MSGR_HDR_TX.
07/27/11   cnx     Merged Optimized Handoff changes.
07/12/10   cnx     Renamed MSGR_MODULE_SRCH to MSGR_MODULE_HDRSRCH to fix compiler warnings. 
06/02/10   smd     Moved this file to hdr/api.
05/26/10   arm     Added MSGR_HDR_HIT.
09/30/09   kss     Added MSGR_HDR_AMAC.
09/17/09   etv     Added MSGR_HDR_RMAC.
07/28/09   etv     Included msgr.h
07/16/09   etv     Changed MSGR_MAX_FW_MODULES to MSGR_FW_MAX_MODULES.
07/15/09   etv     Initial rev.
===========================================================================*/
#ifndef HDRMSG_H
#define HDRMSG_H

#include "msgr.h"

#define HDR_FW_MAX_MODULES       0x20

#define MSGR_MODULE_HDRSRCH           0x20 /* (HDR_FW_MAX_MODULES + 0x00) */
#define MSGR_MODULE_DEC               0x21 /* (HDR_FW_MAX_MODULES + 0x01) */
#define MSGR_MODULE_FMAC              0x22 /* (HDR_FW_MAX_MODULES + 0x02) */
#define MSGR_MODULE_RX                0x23 /* (HDR_FW_MAX_MODULES + 0x03) */
#define MSGR_MODULE_CMAC              0x24 /* (HDR_FW_MAX_MODULES + 0x04) */
#define MSGR_MODULE_RMAC              0x25 /* (HDR_FW_MAX_MODULES + 0x05) */
#define MSGR_MODULE_AMAC              0x26 /* (HDR_FW_MAX_MODULES + 0x06) */
#define MSGR_MODULE_HIT               0x27 /* (HDR_FW_MAX_MODULES + 0x07) */
#define MSGR_MODULE_CP                0x28 /* (HDR_FW_MAX_MODULES + 0x08) */
#define MSGR_MODULE_TX                0x29 /* (HDR_FW_MAX_MODULES + 0x09) */
#define MSGR_MODULE_MULTIRAT          0x2a /* (HDR_FW_MAX_MODULES + 0x0a) */
   /* umid generation tool has issue to recognize (HDR_FW_MAX_MODULES + 0x08) */

#define MSGR_HDR_SRCH                 MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_HDRSRCH )
#define MSGR_HDR_DEC                  MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_DEC )
#define MSGR_HDR_FMAC                 MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_FMAC )
#define MSGR_HDR_RX                   MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_RX )
#define MSGR_HDR_CMAC                 MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_CMAC )
#define MSGR_HDR_RMAC                 MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_RMAC )
#define MSGR_HDR_AMAC                 MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_AMAC )
#define MSGR_HDR_HIT                  MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_HIT )
#define MSGR_HDR_CP                   MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_CP )
#define MSGR_HDR_TX                   MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_TX )
#define MSGR_HDR_MULTIRAT             MSGR_TECH_MODULE( MSGR_TECH_HDR, MSGR_MODULE_MULTIRAT )

#endif /* HDRMSG_H */
