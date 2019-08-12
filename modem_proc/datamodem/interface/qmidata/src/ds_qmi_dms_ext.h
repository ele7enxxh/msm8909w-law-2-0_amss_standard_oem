#ifndef _DS_QMI_DMS_EXT_H
#define _DS_QMI_DMS_EXT_H
/*===========================================================================

                         D S _ Q M I _ D M S _ E X T. H

DESCRIPTION

 The Data Services QMI Device Management Service External header file.
 
Copyright (c) 2005-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_dms_ext.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/20/05    ks     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"


/*===========================================================================
  FUNCTION QMI_DMS_CHARGER_EVENT_OCCURED()

  DESCRIPTION
    Function to be called by QMI charger module when a power state change 
    or battery level is surpassed (as configured by the DMS service via the
    qmi_charger_set_battery_level_limits() function).
    
  PARAMETERS
    event_mask:        Which charger event(s) occured,
                       set of CHARGER_EVENT_*
    power_state_mask:  The state of the charger module,
                       set of PWR_ST_*_MASK
    battery_level:     The current battery strength as a percentage (0-100)

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_dms_charger_event_occured
(
  uint8 event_mask,
  uint8 power_state_mask,
  uint8 battery_level
);

#endif /* _DS_QMI_DMS_EXT_H */
