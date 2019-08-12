#ifndef MCCREG_V_H
#define MCCREG_V_H
/*===========================================================================

           C D M A    R E G I S T R A T I O N    S E R V I C E S
                       H E A D E R    F I L E

DESCRIPTION
  This file data and function declarations necessary for CDMA Registration
  Services.

  Copyright (c) 1993 - 2013 Qualcomm Technologies, Inc. 
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

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccreg.h_v   1.4   01 Aug 2002 19:56:00   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/mccreg_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/10   jtm     Initial SU API split for CP.
09/15/10   azh     Fixed KlocWork warnings.
02/10/10   jj      Added support for ESS feature.
02/23/07   sb      Added support for operational mode feature.
01/15/07   sb      VOIP-1X handoff updates.
06/01/06   an      Added the bypass registration indicator changes
04/25/05   fc      Merged the following from MSMSHARED:
04/20/04   fc       Changes for the update of reg_sid, reg_nid, reg_reg_zone,
                    auth and rand during AHO/APHO.
08/17/04   az      Rel D Negative SCI changes
08/03/04   pg      Added support for Rel D timer based registration.
11/19/03   va      Added support for REL C register_in_idle
07/23/03   bkm     Mainlined FEATURE_ANTIREGISTRATION_BIAS.
08/01/02   fc      Added support for the count of the number of powerup
                   registration performed.
02/22/02   fc      Added support for Release A common channels.
02/14/02   fc      Merged from 1.3 in MSM archive.
 01/25/02  sj      Added prototype for funtion mccreg_uz_update().
 12/14/01  fc      Removed prototype for mccreg_blksys_cb_init.
06/26/01   fc      Merged the following from common archive:
           06/13/01   fc      Added prototype for mccreg_blksys_cb_init.
03/06/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ks      Removed ENHANCED_STANDBY_II feature.
10/05/99   lh      Serving System update for CM is added.
06/22/99   ks      new structure mccreg_bs_reg_info is created for current BS
                   information.
03/01/99   ejv     Merged functions for FEATURE_ANTIREGISTRATION_BIAS.
11/04/98   ych     Added prototype for mccreg_reset_reg().
05/13/96   jca     Updates for J-STD-008 registration procedures.
05/25/95   jca     Updates for IS-95A registration procedures.
03/18/93   jai     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "caii.h"
#include "caii_v.h"
#include "nv.h"
#include "mccreg.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MCCREG_IS_REGISTERED

DESCRIPTION
  This function is called by other subsystems to find out if the
  Mobile has registered.

DEPENDENCIES
  This should be called based on the current Mode of the MS. This function
  returns "FALSE" if the Mobile is not registered,

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS


===========================================================================*/
boolean mccreg_is_registered( void );


void mccreg_reset_reg( void );

#endif /*MCCREG_V_H*/
