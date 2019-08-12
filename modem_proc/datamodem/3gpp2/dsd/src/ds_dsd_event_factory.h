#ifndef DS_DSD_EVENT_FACTORY_H
#define DS_DSD_EVENT_FACTORY_H
/*===========================================================================

                      D S _ D S D _ E V E N T _ F A C T O R Y . H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_event_factory.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/11    sk    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "dstask_v.h"
#include "ds_dsd_ext_i.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION       DS_DSD_EVENT_FACTORY_CMD_PROCESS

DESCRIPTION   This function processes the DSD input event command.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds_dsd_event_factory_cmd_process
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION       DS_DSD_EVENT_FACTORY_MH_EVENT_CBACK

DESCRIPTION   This is an entry point function to DSD engine. The context is 
               switched to DS dask for every input.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
boolean ds_dsd_event_factory_mh_event_cback
(
  dsd_radio_type            client_radio,
  ds_dsd_event_info_type   *event_info_ptr
);

#endif /* DS_DSD_EVENT_FACTORY_H */
