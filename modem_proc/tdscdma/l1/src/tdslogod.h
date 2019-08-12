#ifndef LOGOD_H
#define LOGOD_H
/*===========================================================================

                    L O G O D . H
DESCRIPTION
  This file contains data types and prototypes for log on demand.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  

  Copyright (c) 2000, 2001, 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/logod.h_v   1.3   26 Jun 2002 16:07:48   msatyana  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdslogod.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/29/2002  m       Initial release to support LOD

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "tdsl1def.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_L1_LOG_ON_DEMAND
typedef struct
{
  /* generic command header */
  tdsl1def_l1_local_cmd_hdr_type hdr;
  log_code_type         log_on_demand_code;
} l1_log_on_demand_type;
#endif

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION  dl_cctrch_log_on_demand

DESCRIPTION
  This function receives log on demand request for CCTrCH information
  from QXDM and triggers log generation to QXDM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
log_on_demand_status_enum_type handle_log_on_demand(log_code_type log_code);
#endif

#endif

