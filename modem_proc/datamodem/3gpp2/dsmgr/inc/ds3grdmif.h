#ifndef DS3GRDMIF_H
#define DS3GRDMIF_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                R D M   I N T E R F A C E   F U N C T I O N S

	                    H E A D E R   F I L E

DESCRIPTION
  This file contains functions for interfacing to the Runtime Device Mapper,
  including open and close callback functions, and functions for handling
  commands received from RDM. This header file is intended to be used
  internal to the 3G Dsmgr module only.


  Copyright (c) 2001 - 2012 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3grdmif.h_v   1.0   08 Aug 2002 11:19:48   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3grdmif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/12   sk      Feature cleanup.
01/02/12   msh     Coan: Feature cleanup
03/09/09   sa      AU level CMI modifications.
02/14/08   vk      RDM Open command aggregation.
03/02/06   ar      Fix build warnings.
01/19/06   sl      Serializer related changes.
06/06/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"


#include "sio.h"
#include "dstask_v.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GI_RDM_OPEN_CB

DESCRIPTION   This function is called by RDM to tell DS to open the specified 
              port.
              This function simply sends a command to the DS task, the actual
              open (and prior close, if necessary) is done in the DS task
              context.
              This function must be registered with RDM, before DS can use
              the RDM port mapping services.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_rdm_open_cb
(
  sio_port_id_type   port_id,          /* Port id of the port to be opened */
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION      DS3GI_RDM_CLOSE_CB

DESCRIPTION   This function is called by RDM to tell DS to close the 
              specified port.
              This function simply sends a command to the DS task, the actual
              close is done in the DS task context.
              This function must be registered with RDM, before DS can use
              the RDM port mapping services.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_rdm_close_cb
(
  sio_port_id_type   port_id,          /* Port id of the port to be closed */
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION      DS3GI_HANDLE_RDM_OPEN_CMD

DESCRIPTION   This function processes the DS_RDM_OPEN_CMD, which is a command 
              from RDM to open a serial port.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Changes the serial state and the ATCoP state, if no serial port 
              was previously open.
===========================================================================*/
void  ds3gi_handle_rdm_open_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_HANDLE_RDM_CLOSE_CMD

DESCRIPTION   This function processes the DS_RDM_CLOSE_CMD, which is a 
              command from RDM to close a serial port. The port is closed
              only if no data call is active (or in progress). RDM is notified
              whether the close was successful or not.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Changes the serial state and ATCoP state, if the close is 
              actually performed.
===========================================================================*/
void  ds3gi_handle_rdm_close_cmd
( 
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_RDM_INIT

DESCRIPTION   Initializes DS3G RDM module.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_rdm_init
(
  void
);

#endif /* DS3GRDMIF_H */
