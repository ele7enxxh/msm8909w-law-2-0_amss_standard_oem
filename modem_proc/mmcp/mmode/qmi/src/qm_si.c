/*===========================================================================

                        QM_SI.C 

DESCRIPTION

  QMI_MMODE source file for system information module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
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
01/25/13    sm     Initial version
===========================================================================*/

#include "msg.h"
#include "qm_si.h"
#include "qm_util.h"
#include "qm_meas.h"
#include "qm_mstats.h"
#include "qmi_nas_internal.h"

//===========================================================================
// GLOBALS
//===========================================================================
struct qm_si_global_s
{
  rat_bitmask_type rat_in_svc[QM_SUBS_MAX];
};

static struct qm_si_global_s global;

//===========================================================================
// Function declarations
//===========================================================================

/*===========================================================================
  FUNCTION QM_SI_INIT()

  DESCRIPTION
    This function initializes the system information

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_si_init ( void )
{
  memset( &global, 0, sizeof(struct qm_si_global_s) );
}

/*===========================================================================
  FUNCTION QM_SI_PROCESS_CM_SS_EVENT()

  DESCRIPTION
    This function processes incomming CM SS Event information

  PARAMETERS
    p_ss_info: pointer to incoming CM SS Event
    evt: CM SS Event for the associated info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_si_process_cm_ss_event( cm_mm_msim_ss_info_s_type *p_ss_info, cm_ss_event_e_type evt )
{
  uint32 rat_as_bm;
  rat_bitmask_type current_active = 0;
  rat_bitmask_type new_active = 0;
  rat_bitmask_type new_inactive = 0;
  int i;
  enum qm_subs_e asubs_id = qm_util_map_cm_asubs_to_qm_subs( p_ss_info->asubs_id );

  if( asubs_id == QM_SUBS_MIN ) // No asubs sent for single sim targets
  {
    asubs_id = QM_SUBS_PRI;
  }
  
  switch ( evt )
  {
    case CM_SS_EVENT_SRV_CHANGED:
      for( i = 0; i < p_ss_info->number_of_stacks && i < STACK_INDEX_MAX; i++ )
      {
        rat_as_bm = qm_util_map_cm_sys_mode_to_qm_bitmask( p_ss_info->stack_info[i].sys_mode );

        if ( p_ss_info->stack_info[i].srv_status != SYS_SRV_STATUS_PWR_SAVE &&
             p_ss_info->stack_info[i].srv_status != SYS_SRV_STATUS_NO_SRV )
        {
          current_active |= rat_as_bm;
        }
      }

      new_active   = current_active & (~global.rat_in_svc[asubs_id]);
      new_inactive = global.rat_in_svc[asubs_id] & (~current_active);

      if ( new_inactive != 0 )
      {
        qm_meas_report_new_inactive_rats( asubs_id, new_inactive );
        qm_mstats_report_new_inactive_rats( asubs_id, new_inactive );
      }
      if ( new_active != 0 )
      {
        qm_meas_report_new_active_rats( asubs_id, new_active );
      }

      global.rat_in_svc[asubs_id] = current_active;
      break;

    default:
      break;
  }
}

/*===========================================================================
  FUNCTION QM_SI_IS_RAT_ACTIVE()

  DESCRIPTION
    This function determines if the provided RAT is active, based on latest
    CM SS Event information.  Active RAT is defined as RAT being present in
    sys_mode, and srv_status indicates SRV, Limited or Limited Regional.

  PARAMETERS
    subscription: subscription to check RAT on
    rat: RAT to check

  RETURN VALUE
    boolean: indicating if RAT is active

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_si_is_rat_active( enum qm_subs_e subscription, enum qm_rat_e rat )
{
  boolean ret_val = FALSE;
  uint32  rat_as_bitmask = qm_util_map_rat_enum_to_bitmask( rat );

  if ( global.rat_in_svc[subscription] & rat_as_bitmask )
  {
    ret_val = TRUE;
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION QM_SI_GET_ACTIVE_RATS()

  DESCRIPTION
    This function returns the a mask of the active RATs for the provided
    subscription. Active RAT is defined as RAT being present in sys_mode, and
    srv_status indicates SRV, Limited or Limited Regional.

  PARAMETERS
    subscription: subscription to check RAT on

  RETURN VALUE
    uint32: bitmask of RATs active 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rat_bitmask_type qm_si_get_active_rats( enum qm_subs_e subscription )
{
  rat_bitmask_type ret_val = 0;

  ret_val = global.rat_in_svc[subscription];

  return ret_val;
}

