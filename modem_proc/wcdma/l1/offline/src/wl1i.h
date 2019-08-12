
#ifndef WL1I_H
#define WL1I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            WL 1 I . H                 

GENERAL DESCRIPTION
   Contains declarations for function which are QUALCOMM proprietary 
   and may or may not be shipped as source code.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:$
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wl1i.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

 when       who     what, where, why
 --------   ---     ---------------------------------------------------------
11/24/11    gnk     Removed unused features
02/23/11    asm     Cleanup of unused, mainlined or irrelevant features in offline UL code
12/09/09    uk      Removing fn DEMOD_ADV_RET_FINGERS, fn was added as a 
                    sw workaround for a hw issue   
 03/21/07   bbd     Removed srch timing adjust feature.
 01/30/07   bbd     Removed nudging featurization.
 12/08/06   rmak    Mainlnied FEATURE INTER SYSTEM TIMING MAINT THROUGH SLEEPCTLR
 10/02/06   bbd     Added support for MCAL.
 09/26/06   mg      Extern function wl1_send_nmi_to_mdsp 
 05/16/06   bbd     Added FEATURE_WCDMA_INTERP_PHASE_TRACKING support.
 04/20/06   mg      Extern function wl1_get_mdsp_pc 
 07/18/05   bbd     Temporarily overload searcher HW workaround macro.
 07/01/05   bbd     Added support for 6275 searcher HW workaround under
                    FEATURE_SRCH_TIMING_ADJ.
 03/29/05   bbd     Added support for finger nudging under FEATURE_FINGER_NUDGING.
 02/17/05   bbd     Added mdsp_disable_and_reset
 08/02/04   bbd     De-coupled test mem access workaround for ciphering from
                    the ERAMA workaround.
 02/25/04   bbd     Added workaround for finger front end CGC HW bug
 10/24/03   yiz     Initial version for ERAMA_HW_BUG_SW_WORKAROUND hiding

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "demoddrv.h"

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           Macros                                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Functions                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#if !defined (T_MSM7200)

/*===========================================================================
FUNCTION         WL1_SEND_NMI_TO_MDSP

DESCRIPTION      This function sends a NMI to MDSP

DEPENDENCIES

RETURN VALUE     None.

SIDE EFFECTS
  
===========================================================================*/

extern void wl1_send_nmi_to_mdsp(void);

#endif


/*===========================================================================
FUNCTION         MDSP_DISABLE_AND_RESET

DESCRIPTION      Disables the MDSP firmware from running and puts the HW in
                 reset.

DEPENDENCIES

RETURN VALUE     None.

SIDE EFFECTS
  
===========================================================================*/
void mdsp_disable_and_reset (void);

#endif /* WL1I_H */

