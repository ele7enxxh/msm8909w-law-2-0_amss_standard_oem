/*===========================================================================

                       D S _ Q M I _ S T U B S . C

DESCRIPTION

  QMI stubs for linking, if QMI source is not compiled in.

EXTERNALIZED FUNCTIONS

  
Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/ds_qmi_stubs.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/22/10    pp     Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common definitions and customization headers */
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_QMI
#include "ds_qmux_ext.h"
#include "ds_rmnet_sm_ext.h"

/*===========================================================================
  FUNCTION QMUX_INIT()

  DESCRIPTION
    Initialize specified QMUX instance.  SIO control IO path will be set up,
    and QMUX instance state will be reset to default.

  PARAMETERS
    None

  RETURN VALUE
    TRUE on successful initialization
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmux_init
(
  void
)
{
  return FALSE;
}

/*===========================================================================
  FUNCTION RMNET_SM_INIT()

  DESCRIPTION
    This function initializes the RMNET state machine.
    Called from serving task's task init function.

  PARAMETERS
    None

  FUTURE:
    iface_ptr: Pointer to the interface which is used by this instance
               of Rm state machine to bring up pkt calls.

  RETURN VALUE
    Signals to be handled by RmNet

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_sm_init
(
  void
)
{
  return FALSE;
}

/*============================================================================

FUNCTION MCS_MODEM_HAS_CAPABILITY

DESCRIPTION
  Returns if modem is capable of supporting queried modem technology.
  
DEPENDENCIES
  None

RETURN VALUE
  mcs_modem_cap_return_enum

SIDE EFFECTS
  None

============================================================================*/
mcs_modem_cap_return_enum mcs_modem_has_capability
(
  mcs_modem_capability_enum capability /* capability to check for */
)
{
  return MCS_MODEM_CAP_AVAILABLE;
}

#endif /* FEATURE_DATA_QMI */

