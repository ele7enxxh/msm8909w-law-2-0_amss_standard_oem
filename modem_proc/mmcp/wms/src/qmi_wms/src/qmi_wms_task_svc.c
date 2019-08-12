/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Q M I _ WMS _ T A S K _ S V C . C

DESCRIPTION
  QMI WMS Task services provided to its modules. Contained herein are the
  functions needed for signal notification to QMI WMS task, accessing NV
  items and sending commands to be processed in QMI WMS task context


EXTERNALIZED FUNCTIONS
  qmi_wms_enable_sig()
    Enable a specific signal for QMI task to process.

  qmi_wms_disable_sig()
    Disable a specific signal from QMI task processing.

  qmi_wms_set_signal()
    Sets a signal for the QMI_WMS task

  qmi_wms_clear_signal()
    Clear a signal for the QMI_WMS task.

  qmi_wms_set_sig_handler()
    Sets signal handler for specific signal processed in QMI WMS task.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless MC has created the QMI task
  and qmi_wms_task() has been invoked.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/src/qmi_wms_task_svc.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "rex.h"
#include "qmi_wms_task_svc.h"
#include "qmi_wms_taski.h"
#include "modem_mem.h"
#include "amssassert.h"
#include "qmi_wms_crit_sect.h"
#include "msg.h"


/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

                  EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================
FUNCTION QMI_WMS_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the QMI WMS task uses for rex_wait().

PARAMETERS
  sig   : Signal to be enabled.

DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is
  defaulted (printing an err msg.)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_enable_sig
(
  qmi_wms_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



#ifdef FEATURE_REX_SIGS_EXT
  QMI_WMS_SET_SIG(qmi_wmsi_enabled_sig_mask, sig);
#else
  qmi_wmsi_enabled_sig_mask |= ( (rex_sigs_type) 1 << (rex_sigs_type) sig );
#endif /* FEATURE_REX_SIGS_EXT */

  QMI_WMS_SET_SIGNAL( QMI_WMS_SIG_MASK_CHANGE_SIGNAL );
} /* qmi_wms_enable_sig() */

/*===========================================================================
FUNCTION QMI_WMS_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

PARAMETERS
  sig   : Signal to be disabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_disable_sig
(
  qmi_wms_sig_enum_type sig                       /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_REX_SIGS_EXT
  QMI_WMS_CLR_SIG(qmi_wmsi_enabled_sig_mask, sig);
#else
  qmi_wmsi_enabled_sig_mask &= ~( (rex_sigs_type) 1 << (rex_sigs_type) sig );
#endif /*FEATURE_REX_SIGS_EXT */
} /* qmi_wms_disable_sig() */

/*===========================================================================

FUNCTION QMI_WMS_SET_SIGNAL()

DESCRIPTION
  Sets a signal for the QMI_WMS task and This function notifies the scheduler
  that there is processing to be performed in QMI_WMS task.

PARAMETERS
  sig   : Signal to be set.

DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is
  defaulted (printing an err msg.)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_wms_set_signal
(
  qmi_wms_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  QMI_WMS_SET_SIGNAL( sig );
} /* qmi_wms_set_signal() */

/*===========================================================================
FUNCTION QMI_WMS_CLEAR_SIGNAL()

DESCRIPTION
  Clear a signal for the QMI_WMS task.

PARAMETERS
  sig   : Signal to be cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_clear_signal
(
  qmi_wms_sig_enum_type sig                       /* Signal to be disabled     */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_WMS_CLR_SIGNAL( sig );
} /* qmi_wms_clear_signal() */
