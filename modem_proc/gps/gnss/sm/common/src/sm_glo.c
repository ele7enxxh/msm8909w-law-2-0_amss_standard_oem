/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Session Manager Utility Module

GENERAL DESCRIPTION
  This module has SM translation function to convert internal nav solution
  structures to gps/gnss structures.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/sm_glo.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/14   jv      Compile time Constellation configurability


============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gps_variation.h"
#include "sm_api.h"
#include "sm_util.h"


sm_ConstellationCfgType sm_set_constellation_config_glo()
{
    return SM_CCFG_CT_GLO;
}

