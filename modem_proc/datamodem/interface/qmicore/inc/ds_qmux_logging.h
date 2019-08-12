#ifndef _DS_QMUX_LOGGING_H
#define _DS_QMUX_LOGGING_H
/*===========================================================================

                         D S _ Q M U X _ L O G G I N G . H

DESCRIPTION

  The Data Services Qualcomm MSM Interface QMUX Logging header file.

EXTERNALIZED FUNCTIONS

  qmi_log_packet()
    This function logs the QMUX RX & TX PDU sent over a QMI Link

Copyright (c) 2004-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/13/06    apm    Clean up.
11/06/06    apm    Created file.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "dsm.h"
#include "ds_qmi_defs.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  QMUX PACKET DIRECTION ENUMERATION
---------------------------------------------------------------------------*/
typedef enum
{
  QMUX_TX               = 1,
  QMUX_RX               = 0
} qmux_direction_e_type;



/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_LOG_PACKET

  DESCRIPTION
    Function to log RX & TX QMUX PDUs

  PARAMETERS
    item_ptr     - pointer to the dsm chained to be logged
    qmi_instance - the QMI instance being used
    direction    - TX or RX

  This function constructs the appropriate log code using the 
  qmi_instance and direction parameters.

  LOG CODE RANGE      DIRECTION      FRAMING
  --------------      ---------      -------
  5006-5038             RX/TX          DIAG

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/

extern void qmi_log_packet
(
  dsm_item_type *        item_ptr, 
  qmi_instance_e_type    qmi_instance, 
  qmux_direction_e_type  direction
);
#endif /* _DS_QMUX_LOGGING_H */

