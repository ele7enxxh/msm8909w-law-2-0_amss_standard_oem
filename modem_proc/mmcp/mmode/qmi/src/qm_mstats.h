#ifndef _QM_MSTATS_H
#define _QM_MSTATS_H


/*===========================================================================

                         QM_MSTATS.H

DESCRIPTION

  QMI_MMODE header file for modem stats information module.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/14/13    sm     Created file
===========================================================================*/

#include "comdef.h"
#include "cm.h"
#include "qm_comdef.h"
#include "network_access_service_v01.h"

/*--------------------------------------------------------------------------- 
  data types
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION QM_MSTATS_INIT()

  DESCRIPTION
    This function initializes the modem stats information

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_mstats_init ( void );

/*===========================================================================
  FUNCTION QM_MSTATS_PROCESS_CM_MODEM_STATS_EVENT()

  DESCRIPTION
    This function processes incoming modem stats information.

  PARAMETERS
   p_mstats_info: pointer to modem stats information 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_mstats_process_cm_modem_stats_event ( sys_modem_stats_info_s_type *p_mstats_info );

/*===========================================================================
  FUNCTION QM_MSTATS_REPORT_NEW_INACTIVE_RATS()

  DESCRIPTION
    This function reports to modem stats module whether any RATs have newly gone out of
    service.  RATs listed in this mask have their modem stats information invalidated.

  PARAMETERS
    subs_id: subscription for which the RATs are inactive
    inactive_rat_mask: mask of newly inactive RATs for given subscription
 
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_mstats_report_new_inactive_rats( enum qm_subs_e subs_id, rat_bitmask_type inactive_rat_mask );

/*===========================================================================
  FUNCTION QM_MSTATS_POPULATE_CELL_LOC_INFO()

  DESCRIPTION
    This function populates get_cell_location_info resp based on modem stats module
    information.

  PARAMETERS
    rsp_msg: pointer to GET_CELL_LOCATION_INFO response structure
    subs_id: subscription for which the response should be populated

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_mstats_populate_cell_loc_info ( nas_get_cell_location_info_resp_msg_v01 *rsp_msg, enum qm_subs_e subs_id );

#endif // !_QM_MSTATS_H

