/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                          
                    F T M  C O M M O N  D I S P A T C H

GENERAL DESCRIPTION
  This is the header file for the embedded FTM Commands that are SV module
  specific

Copyright (c) 2010,2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/ftm/src/ftm_sv_dispatch.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/21/12   sar     Removed unused/obsolete code.
04/05/11   kma     Fixed compilation error 
04/04/11   kma     Fixed compilation error
12/17/10   kma     Fixed compilation error
07/06/10   kma     Fixed compilation error
06/28/10   kma     Created
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "ftmicap.h"

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FTM_HAS_SV_API
#include "ftm_sv_dispatch.h"
#include "rftxplim_power_mm_fusion.h"
#include "rf_test_task_v.h"

/*===========================================================================

FUNCTION FTM_SV_DISPATCH

DESCRIPTION
   This function handles requests to run common tests and other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_sv_dispatch(ftm_sv_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type               rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  return rsp_pkt;
} /* end ftm_sv_dispatch */

#endif /* FTM_HAS_SV_API */
#endif /* FEATURE_FACTORY_TESTMODE */

