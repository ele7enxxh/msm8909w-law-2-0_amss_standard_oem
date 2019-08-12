/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM 1x Dispatch

GENERAL DESCRIPTION
  This is the FTM RF file which contains RF specific commands

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/gen8/ftm/src/ftm_gnss_dispatch.c#1 $ 
  $DateTime: 2016/12/13 07:59:49 $ 
  $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/11   sar     Included ftm_rf_cmd.h for rfa/api cleanup.
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
11/16/09   dyc     Removed cust_T_MSM.h (part of mode specific build updates)
10/01/09   hp      Created. Added support for External LNA
===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "comdef.h"
#include "rf_test_task_v.h"
#include "ftm_gnss_dispatch.h"
#include "ftm_gnss_ctl.h"
#include "ftm_rf_cmd.h"


/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/





/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/






/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION FTM_GNSS_DISPATCH

DESCRIPTION
   This function handles requests to run tests and other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_gnss_dispatch(ftm_pkt_type  * ftm)
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  
  switch (ftm->rf_params.ftm_rf_cmd)
  {
  case FTM_GNSS_EXTERNAL_LNA:
    ftm_set_gnss_external_lna(ftm->rf_params.ftm_rf_factory_data.on_off);        
    FTM_MSG_MED("FTM_GNSS_EXTERNAL_LNA: %d", ftm->rf_params.ftm_rf_factory_data.on_off, 0, 0);
    break;

  default:
    FTM_MSG_ERROR("Unknown FTM GNSS RF Command",0,0,0);
    rsp_pkt.cmd = FTM_RSP_BAD_CMD;
  }

  return rsp_pkt;
}   /* end ftm_gnss_dispatch */
#endif /*FEATURE_FACTORY_TESTMODE */
