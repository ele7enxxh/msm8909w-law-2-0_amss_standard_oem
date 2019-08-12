#ifndef __QMI_SIM_INTERNAL_SERVICE_H__
#define __QMI_SIM_INTERNAL_SERVICE_H__
/*===========================================================================

                         Q M I _ S I M . H

DESCRIPTION

 The Data Services QMI SIM header file.

EXTERNALIZED FUNCTIONS



                        COPYRIGHT INFORMATION

Copyright (c) 2012 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/inc/qmi_sim_internal.h#1 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/25/15    sp     Updated copyright
07/14/14    tl     Remove always ON feature FEATURE_MODEM_RCINIT
04/18/14    tl     Add to function to initialize globals
10/28/13    kb     Add offtarget support to check mutex is owned or not
10/01/12    vs     Added utility for context check including logic for SGLTE
03/13/12    tkl    Initial check-in of QMI_SIM_INTERNAL
===========================================================================*/

#include "customer.h"
#include "rex.h"

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_INIT_GLOBALS()

  DESCRIPTION
    Initalize QMI SIM internal globals

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_sim_internal_init_globals(void);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_INIT()

  DESCRIPTION
    Called when task gets started.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_sim_internal_init(rex_tcb_type *tcb, rex_sigs_type sig);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_CLOSE()

  DESCRIPTION


  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_sim_internal_close(void);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_HANDLE_EVENT()

  DESCRIPTION
    Called to handle an event after the server thread receives an event
    notification. Callbacks from qmi_csi_register will be invoked in the
    server's context.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_sim_internal_handle_event (rex_tcb_type *tcb, rex_sigs_type sig);

/*===========================================================================
  FUNCTION QMI_SIM_INTERNAL_ACTIVE_MMGSDI_SYNC_CALL()

  DESCRIPTION
    Returns whether there is an active MMGSDI sync call from QMI_SIM_INTERNAL

  PARAMETERS
    None

  RETURN VALUE
    Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean qmi_sim_internal_active_mmgsdi_sync_call(void);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#endif /* __QMI_SIM_INTERNAL_SERVICE_H__ */
