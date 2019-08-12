#ifndef FTM_MC_H
#define FTM_MC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     M C   P L T   S U P P O R T 

GENERAL DESCRIPTION
  This file contains the MC extensions to support FTM.  It is used only
  for FTM and should not be used for non-FTM builds.
  
Copyright (c) 1999 - 2007 by Qualcomm Technologies Incorporated. All Rights Reserved.
Copyright (c) 2010 - by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/test_mc.h#1 $
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $
  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/11   sar     Merged 1X API cleanup changes.  
04/01/11   sar     Removed #ifdef's to comply with CMI.
11/08/10   Saul    Fixed FTM-Online switching crash due to mdsp enable/disable latency.
10/8/10    anr     1x/DO Factory Features
01/25/07   ak      Merged changes from the tip of this file
08/17/06   ak      Removed the unused function prototype ftm_nv_init()
10/30/05   vm      Added new function ftm_hdrlmac_tc_assign_complete_cb()
09/23/05   jac     Fixed more PLT build conflicts.
09/23/05   jac     Fixed PLT build conflicts.
08/30/05   jac     Finished porting the PLT DO Rev A commands to FTM
08/08/05   ra      added HDR Rev A support
08/01/05   ra      include mc.h for mc_name_type
07/14/03   bmg     Merged in Cougar 3030->4040 changes
02/10/03   wd      Marge files for FTM Task, Also added p4 info.
12/06/02   ra      added ftm_tmc_init to register the diag subcommand dispatch table
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime choice.
10/19/00   bgc     Added ftm_set_srch_parms to fix warnings.
10/26/99   lcc     First cut.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "msg.h"
#include "srch.h"
#include "srch_v.h"
#include "ftm.h"
/* FTM_HAS_HDR_REV_A */ 
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              FTM signals                                 */
/*                              ----------                                  */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           Local Data                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION ftm_tmc_init

DESCRIPTION
  ftm initialization that has to be done in tmc task.

DEPENDENCIES
  TMC task has already started.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tmc_init(void);

/*===========================================================================

FUNCTION TEST_HDRLMAC_TC_ASSIGN_COMPLETE_CB

DESCRIPTION
  Callback function passed to HDR TC assignment command.

DEPENDENCIES

RETURN VALUE
  Will be called when HDR traffic channel is assigned.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_hdrlmac_tc_assign_complete_cb ( void );

#endif /* FTM_MC_H */
