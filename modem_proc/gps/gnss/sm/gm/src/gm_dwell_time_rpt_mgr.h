/*===========================================================================

                  GM_DWELL_TIME_RPT_MGR Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM Dwell Time Report Manager.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/gm/src/gm_dwell_time_rpt_mgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/18/15   ss    Initial creation of file.
===========================================================================*/
#ifndef GM_DWELL_TIME_RPT_MGR_H
#define GM_DWELL_TIME_RPT_MGR_H
#include "gm_core.h"
#include "gm_list.h"


/*===========================================================================

FUNCTION TYPE gm_dwell_time_rpt_mgr_init

DESCRIPTION
  Initialization function
DEPENDENCIES

RETURN VALUE
Void
SIDE EFFECTS
  None.

===========================================================================*/
void gm_dwell_time_rpt_mgr_init(void);

/*===========================================================================

FUNCTION TYPE gm_dwell_time_rpt_mgr_dwell_start

DESCRIPTION
  Starts Dwell timer for a geofence
DEPENDENCIES

RETURN VALUE
Boolean: pass\fail
SIDE EFFECTS
  None.

===========================================================================*/
boolean gm_dwell_time_rpt_mgr_dwell_start(sm_gm_geofence_node_type* geofence_node);

/*===========================================================================

FUNCTION TYPE gm_dwell_time_rpt_mgr_dwell_stop

DESCRIPTION
  Stops Dwell timer for a geofence
DEPENDENCIES

RETURN VALUE
Boolean: pass\fail
SIDE EFFECTS
  None.

===========================================================================*/
boolean gm_dwell_time_rpt_mgr_dwell_stop(sm_gm_geofence_node_type* geofence_node);

/*===========================================================================

FUNCTION TYPE gm_dwell_time_rpt_mgr_dwell_timer_expiry

DESCRIPTION
  Handles Dwell timer expiry
  
DEPENDENCIES

RETURN VALUE
Boolean: pass\fail
SIDE EFFECTS
  None.

===========================================================================*/
void gm_dwell_time_rpt_mgr_dwell_timer_expiry(void);


#endif /*GM_DWELL_TIME_RPT_MGR_H*/

