#ifndef _QM_SI_H
#define _QM_SI_H

/*===========================================================================

                         QM_SI.H

DESCRIPTION

  QMI_MMODE header file for system information module.

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
01/25/13    sm     Created file
===========================================================================*/

#include "cm.h"
#include "qm_comdef.h"

/*--------------------------------------------------------------------------- 
  data types
---------------------------------------------------------------------------*/

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
void qm_si_init ( void );

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
void qm_si_process_cm_ss_event( cm_mm_msim_ss_info_s_type *p_ss_info, cm_ss_event_e_type evt );

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
boolean qm_si_is_rat_active( enum qm_subs_e subscription, enum qm_rat_e rat );

/*===========================================================================
  FUNCTION QM_SI_GET_ACTIVE_RATS()

  DESCRIPTION
    This function returns the a mask of the active RATs for the provided
    subscription. Active RAT is defined as RAT being present in sys_mode, and
    srv_status indicates SRV, Limited or Limited Regional.

  PARAMETERS
    subscription: subscription to check RAT on

  RETURN VALUE
    rat_bitmask_type: bitmask of RATs active 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rat_bitmask_type qm_si_get_active_rats( enum qm_subs_e subscription );

#endif // !_QM_SI_H
