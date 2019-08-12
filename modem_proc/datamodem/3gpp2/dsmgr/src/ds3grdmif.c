/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       3 G   D A T A   S E R V I C E S

                R D M   I N T E R F A C E   F U N C T I O N S

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to the Runtime Device
  Mapper (RDM).

EXTERNALIZED FUNCTIONS
  ds3gi_handle_rdm_open_cmd()
    Processes the DS_RDM_OPEN_CMD

  ds3gi_handle_rdm_close_cmd()
    Processes the DS_RDM_CLOSE_CMD

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The open and close callback functions must be registered with RDM before
  Data Services can use RDM's port mapping services.


  Copyright (c) 2006-2012 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3grdmif.c_v   1.1   23 Aug 2002 19:12:30   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3grdmif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/12   sk      Feature cleanup.
01/02/12   msh     Coan: Feature cleanup 
05/26/11   dvk     Global Variables Cleanup
09/09/10   op      Migrated to MSG 2.0 macros
09/22/09   vrk	   Merged LTE changes.
05/05/09   ss      Fixed Compiler Warnings.
02/25/09   sn      Fixed off-target lint errors.
12/15/08   sn      Fixed compilation error.
02/14/08   vk      RDM Open command aggregation.
01/19/06   sl      Serializer related changes.
10/31/01   smp     When a port can't be opened successfully, notify RDM that
                   the open failed (via RDM_APP_BUSY_S).
06/05/01   smp     Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST 
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "rdevmap.h"
#include "dstask.h"
#include "ds3gmgrint.h"
#include "ds3gsiolib.h"
#include "ds3grdmif.h"
#include "data_err_suppr.h"
#include "data_msg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*---------------------------------------------------------------------------
  Structure for DS_CMD_RDM_OPEN
---------------------------------------------------------------------------*/
typedef struct
{
  sio_port_id_type   port_id;          /* Port id of the port to be opened */
  void              *user_data;        /* User data pointer                */
} ds3g_rdm_open_cmd_type;

/*---------------------------------------------------------------------------
  Structure for DS_CMD_RDM_CLOSE
---------------------------------------------------------------------------*/
typedef struct
{
  sio_port_id_type  port_id;           /* Port id of the port to be closed */
} ds3g_rdm_close_cmd_type;

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

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
)
{
  ds_cmd_type             *cmd_ptr = NULL;
  ds3g_rdm_open_cmd_type  *rdm_open_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a command buffer from the DS task.
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_rdm_open_cmd_type));
  if( (cmd_ptr != NULL) && (cmd_ptr->cmd_payload_ptr != NULL) )
  {
    /*-----------------------------------------------------------------------
      Fill in the parameters, and send the command to the DS task.
    -----------------------------------------------------------------------*/
    cmd_ptr->hdr.cmd_id = DS_CMD_RDM_OPEN;
    rdm_open_cmd_ptr = (ds3g_rdm_open_cmd_type*)cmd_ptr->cmd_payload_ptr;
    rdm_open_cmd_ptr->port_id = port_id;

    ds_put_cmd( cmd_ptr );
  }
  else
  {
    /*-----------------------------------------------------------------------
      No free command buffers available. Notify RDM that the open failed.
    -----------------------------------------------------------------------*/
    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
  }
  return;
} /* ds3gi_rdm_open_cb() */

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
)
{
  ds_cmd_type              *cmd_ptr = NULL;
  ds3g_rdm_close_cmd_type  *rdm_close_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);

  /*-------------------------------------------------------------------------
    Get a command buffer from the DS task.
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_rdm_close_cmd_type));
  if( (cmd_ptr != NULL) && (cmd_ptr->cmd_payload_ptr != NULL) )
  {
    /*-----------------------------------------------------------------------
      Fill in the parameters, and send the command to the DS task.
    -----------------------------------------------------------------------*/
    cmd_ptr->hdr.cmd_id = DS_CMD_RDM_CLOSE;
    rdm_close_cmd_ptr = (ds3g_rdm_close_cmd_type*)cmd_ptr->cmd_payload_ptr;
    rdm_close_cmd_ptr->port_id = port_id;
    ds_put_cmd( cmd_ptr );
  }
  else
  {
    /*-----------------------------------------------------------------------
      No free command buffers available. Notify RDM that the close failed.
    -----------------------------------------------------------------------*/
    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
  }
  return;
} /* ds3gi_rdm_close_cb() */

/*===========================================================================
FUNCTION      DS3GI_HANDLE_RDM_OPEN_FOR_PORT

DESCRIPTION   Opens the specified SIO port. RDM is notified whether the open 
               was successful or not.
 
PARAMETERS    sio_port_id: SIO port number of port to open.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Changes the serial state and the ATCoP state, if no serial port 
              was previously open.
===========================================================================*/
void  ds3gi_handle_rdm_open_for_port
(
  sio_port_id_type sio_port_id
)
{
  ds3g_siolib_status_e_type    open_status;
  ds3g_siolib_port_e_type      open_port_id = DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_state_info_type *open_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Command from RDM to open a serial port. If we already have a port open,
    we must close it first and then open the port specified by RDM.
  -------------------------------------------------------------------------*/
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, 
                  "Rx'ed OPEN cmd for sio port id: %d", sio_port_id);

  open_port_id = ds3g_siolib_map_sio_port(sio_port_id);

  if(open_port_id == DS3G_SIOLIB_PORT_NONE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "RDM_OPEN_CMD ignored, Invalid port");
    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
    return;
  }

  open_state = ds3g_siolib_get_ds3g_siolib_state(open_port_id);

  if(NULL == open_state)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "RDM_OPEN_CMD ignored, Invalid Port State");
    return;
  }
  /*-----------------------------------------------------------------------
    Now attempt to open the port specified by RDM.
  -----------------------------------------------------------------------*/
  open_status = ds3g_siolib_open( sio_port_id,
                                  &open_state->serial_info.rx_wm_ptr,
                                  &open_state->serial_info.tx_wm_ptr );

  if( open_status == DS3G_SIOLIB_OK )
  {
    /*---------------------------------------------------------------------
     Notify RDM that the open was successful.
    ---------------------------------------------------------------------*/
    rdm_notify( RDM_DATA_SRVC, RDM_DONE_S );

    /*---------------------------------------------------------------------
    Set the serial state to Autodetect and ATCoP state to Command state.
    ---------------------------------------------------------------------*/
    open_state->serial_info.serial_state = DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;
    open_state->at_state = DS3G_SIOLIB_COMMAND_AT_STATE;
  }
  else
  {
    /*---------------------------------------------------------------------
    Notify RDM that the open was unsuccessful.
    ---------------------------------------------------------------------*/
    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
   }

} /* ds3gi_handle_rdm_open_for_port() */

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
)
{
  sio_port_id_type         port_id;
  ds3g_rdm_open_cmd_type  *rdm_open_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  if(cmd_ptr->hdr.cmd_id != DS_CMD_RDM_OPEN)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid cmd: %d", cmd_ptr->hdr.cmd_id);
    return;
  }

  rdm_open_cmd_ptr = (ds3g_rdm_open_cmd_type*)cmd_ptr->cmd_payload_ptr;
  port_id = rdm_open_cmd_ptr->port_id;

  /*-------------------------------------------------------------------------
    Received command from RDM to open a serial port. 
  -------------------------------------------------------------------------*/
  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Rx'ed RDM_OPEN_CMD, port id: %d", port_id);

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);

  ds3gi_handle_rdm_open_for_port(port_id);

  return;
} /* ds3gi_handle_rdm_open_cmd() */

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
)
{
  ds3g_siolib_port_e_type       port_id = DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_state_info_type  *state = NULL;
  ds3g_rdm_close_cmd_type      *rdm_close_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  if(cmd_ptr->hdr.cmd_id != DS_CMD_RDM_CLOSE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid cmd: %d", cmd_ptr->hdr.cmd_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Command from RDM to close a serial port.
  -------------------------------------------------------------------------*/
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed RDM_CLOSE_CMD");

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);

  rdm_close_cmd_ptr = (ds3g_rdm_close_cmd_type*)cmd_ptr->cmd_payload_ptr;
  port_id = ds3g_siolib_map_sio_port(rdm_close_cmd_ptr->port_id);

  if(port_id == DS3G_SIOLIB_PORT_NONE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "RDM_CLOSE_CMD ignored, Invalid port");
    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
    return;
  }

  state = ds3g_siolib_get_ds3g_siolib_state(port_id);

  if(NULL == state)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "RDM_OPEN_CMD ignored, Invalid Port State");
    return;
  }

  if ( state->call_coming_up || 
       ds3g_siolib_get_ds3g_siolib_pending_close_port() != DS3G_SIOLIB_PORT_NONE )
  {
    /*-----------------------------------------------------------------------
      We are busy, so don't close the port. Notify RDM that the close was
      unsuccessful. *** This logic will need to change when support for
      socket calls is added. ***
    -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "RDM_CLOSE_CMD ignored, DS is busy");
    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
  }
  else
  {
    /*-----------------------------------------------------------------------
      We are idle, ok to close the port.
    -----------------------------------------------------------------------*/
    ds3g_siolib_close(port_id);
  }

  return;
} /* ds3gi_handle_rdm_close_cmd() */

