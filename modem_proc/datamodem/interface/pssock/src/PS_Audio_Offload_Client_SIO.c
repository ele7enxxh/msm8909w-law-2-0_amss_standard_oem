#include "datamodem_variation.h"
#ifdef FEATURE_DATA_AUDIO_OFFLOAD
/*===========================================================================

                         PS _ AUDIO _ OFFLOAD _ CLIENT _ SIO. C

DESCRIPTION

  VOIP Offload Clinet SIO module implementation.

  Implements SIO port routines employed by OC modules

EXTERNALIZED FUNCTIONS

  ps_audio_offload_client_sio_open()
    Initialize the SIO port between VoIP OC and OSA
  ps_audio_offload_client_sio_write()
    Send data on the given SIO port
  ps_audio_offload_client_sio_read()
    Get data from the given SIO port

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/14/13    az     Initial implementation of VoIP Offload Client SIO component 
09/09/13    msr    Second revision 
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include <stringl/stringl.h>
#include "dserrno.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include "ps_svc.h"
#include "PS_Audio_Offload_Client_SIO.h"
#include "ps_audio_offload_client_sioi.h"
#include "ps_audio_offload_client_def.h"
#include "ps_handle_mgr.h"
#include "ps_crit_sect.h"

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/
static ps_audio_offload_client_sio_ctrl_info_type  ps_audio_offload_client_sio_ctrl_info = {0};

static ps_audio_offload_client_sio_info_type  * ps_audio_offload_client_sio_info_ptr[PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR] = {0};

ps_audio_offload_client_sio_event_cback_type  ps_audio_offload_client_sio_event_cback_f_ptr = NULL;


/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIOI_VALIDATE()

  DESCRIPTION:
  Validates if the client handle is ready to be used caller

  INPUT/OUTPUT PARAMETERS:
  audio_offload_client_handle - Client handle to be validated
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any

  RETURN VALUE:
    Success or failure

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_sioi_validate
(
  ps_audio_offload_client_handle_type          audio_offload_client_handle,
  int32                                      * err_reason,
  int16                                      * dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate input
  -------------------------------------------------------------------------*/
  if (NULL == err_reason || NULL == dss_errno)
  {
    LOG_MSG_INVALID_INPUT_0("ps_audio_offload_client_sioi_validate(): NULL arg");
    return -1;
  }

  if (PS_AUDIO_OFFLOAD_CLIENT_INVALID_HANDLE == audio_offload_client_handle)
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_HANDLE;
    *dss_errno  = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check if SIO module is initialized
  -------------------------------------------------------------------------*/
  if (NULL == ps_audio_offload_client_sio_ctrl_info.smdl_handle)
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_NOT_INITIALIZED;
    *dss_errno  = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Make sure that ctrl port is opened from remote side
  -------------------------------------------------------------------------*/
  if (!(PS_AUDIO_OFFLOAD_CLIENT_SIO_STATE_OPEN &
          ps_audio_offload_client_sio_ctrl_info.smdl_state))
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_PORT_NOT_OPEN;
    *dss_errno  = DS_EINVAL;
    return -1;
  }

  return 0;
} /* ps_audio_offload_client_sioi_validate() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIOI_ALLOC_SIO_INFO()

  DESCRIPTION:
    Allocates all the resources required for an SIO port

  INPUT/OUTPUT PARAMETERS:
  audio_offload_client_handle - Client handle associated with the port
  fltr_id - Filter ID associated with the client handle
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any

  RETURN VALUE:
    Pointer to the SIO port structure allocated
 
    On error, the error condition value is stored in *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
ps_audio_offload_client_sio_info_type * ps_audio_offload_client_sioi_alloc_sio_info
(
  ps_audio_offload_client_handle_type          audio_offload_client_handle,
  uint8                                      * fltr_id,
  int32                                      * err_reason,
  int16                                      * dss_errno
)
{
  ps_audio_offload_client_sio_info_type  * sio_info_ptr;
  int16                                    sio_info_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get index corresponding to the handle
  -------------------------------------------------------------------------*/
  sio_info_index =
    ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_AUDIO_OFFLOAD_CLIENT,
                            audio_offload_client_handle);

  if (PS_HANDLE_MGR_INVALID_INDEX == sio_info_index ||
      0 > sio_info_index ||
      PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR <= sio_info_index)
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_HANDLE;
    *dss_errno  = DS_EINVAL;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Check if port info is not already in use
  -------------------------------------------------------------------------*/
  if (NULL != ps_audio_offload_client_sio_info_ptr[sio_info_index])
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_PORT_IN_USE;
    *dss_errno  = DS_EINVAL;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Allocate memory for port info
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(sio_info_ptr, sizeof(ps_audio_offload_client_sio_info_type), 
                           ps_audio_offload_client_sio_info_type* );
  if (NULL == sio_info_ptr)
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_OUT_OF_SYSTEM_HEAP_MEM;
    *dss_errno  = DS_ENOMEM;
    return NULL;
  }

  memset(sio_info_ptr, 0, sizeof(ps_audio_offload_client_sio_info_type));

  sio_info_ptr->audio_offload_client_handle = audio_offload_client_handle;

  ps_audio_offload_client_sio_info_ptr[sio_info_index] = sio_info_ptr;

  *fltr_id = (uint8) sio_info_index;
  return sio_info_ptr;
} /* ps_audio_offload_client_sioi_alloc_sio_info() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIOI_FREE_SIO_INFO()

  DESCRIPTION:
    Deallocates all the resources associated with the SIO port

  INPUT/OUTPUT PARAMETERS:
  sio_info_ptr - Pointer to the SIO port
  fltr_id - Filter ID associated with the SIO port
    
  RETURN VALUE:
    Pointer to the SIO port structure allocated

  SIDE EFFECTS:
    None
===========================================================================*/
void ps_audio_offload_client_sioi_free_sio_info
(
  ps_audio_offload_client_sio_info_type  * sio_info_ptr,
  uint8                                    fltr_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Free the memory
  -------------------------------------------------------------------------*/
  ps_audio_offload_client_sio_info_ptr[fltr_id] = NULL;

  PS_SYSTEM_HEAP_MEM_FREE(sio_info_ptr);

  return;
} /* ps_audio_offload_client_sioi_free_sio_info() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENTI_SIO_CONVERT_ID_TO_FILTER_MASK()

  DESCRIPTION:
    Converts the filter ID to the corresponding mask

  INPUT/OUTPUT PARAMETERS:
  fltr_id - Filter ID whose mask is required
    
  RETURN VALUE:
    Filter mask value

  SIDE EFFECTS:
    None
===========================================================================*/
static uint8 ps_audio_offload_clienti_sio_convert_id_to_filter_mask
(
  uint8 fltr_id
)
{
 return (0x01 << (fltr_id));
}

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIOI_GET_SIO_INFO()

  DESCRIPTION:
    Allocates all the resources required for an SIO port

  INPUT/OUTPUT PARAMETERS:
  audio_offload_client_handle - Client handle associated with the port
  fltr_id_ptr - Filter ID pointer
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any

  RETURN VALUE:
    Pointer to the SIO port structure
 
    On error, the error condition value is stored in *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
ps_audio_offload_client_sio_info_type * ps_audio_offload_client_sioi_get_sio_info
(
  ps_audio_offload_client_handle_type        audio_offload_client_handle,
  uint8                                    * fltr_id_ptr,
  int32                                    * err_reason,
  int16                                    * dss_errno
)
{
  ps_audio_offload_client_sio_info_type  * sio_info_ptr;
  int16                                    sio_info_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get index corresponding to the handle
  -------------------------------------------------------------------------*/
  sio_info_index =
    ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_AUDIO_OFFLOAD_CLIENT,
                            audio_offload_client_handle);

  if (PS_HANDLE_MGR_INVALID_INDEX == sio_info_index ||
      0 > sio_info_index ||
      PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR <= sio_info_index)
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_HANDLE;
    *dss_errno  = DS_EINVAL;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Make sure that port is in use and with the same handle as the input
  -------------------------------------------------------------------------*/
  sio_info_ptr = ps_audio_offload_client_sio_info_ptr[sio_info_index];

  if (NULL == sio_info_ptr ||
      audio_offload_client_handle != sio_info_ptr->audio_offload_client_handle)
  {
    *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_HANDLE;
    *dss_errno  = DS_EINVAL;
    return NULL;
  }

  *fltr_id_ptr = (uint8) sio_info_index;

  return sio_info_ptr;
} /* ps_audio_offload_client_sioi_get_sio_info() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIOI_SEND_ERROR()

  DESCRIPTION:
    Sends error message to OSA/ADSP over SMDL port

  INPUT/OUTPUT PARAMETERS:
  audio_offload_client_handle - Client handle associated with the port
  sio_error - SIO error enum value
  is_set - Defines if it is SET or RESET request
  err_reason - Error reason passed
  dss_errno - Error code passed

  RETURN VALUE:
    Success or failure
 
    On error, the error condition value is stored in *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_sioi_send_error
(
  ps_audio_offload_client_handle_type            audio_offload_client_handle,
  ps_audio_offload_client_sio_error_enum_type    sio_error,
  boolean                                        is_set,
  int32                                        * err_reason,
  int16                                        * dss_errno
)
{
  ps_audio_offload_client_sio_info_type      * sio_info_ptr = NULL;
  ps_audio_offload_client_sio_ctrl_cmd_type    error_cmd;
  int32                                        ret_val = -1;
  int32                                        bytes_tx;
  uint8                                        fltr_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (-1 == ps_audio_offload_client_sioi_validate(audio_offload_client_handle,
                                                    err_reason,
                                                    dss_errno))
    {
      break;
    }

    sio_info_ptr =
      ps_audio_offload_client_sioi_get_sio_info(audio_offload_client_handle,
                                                &fltr_id,
                                                err_reason,
                                                dss_errno);
    if (NULL == sio_info_ptr)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Inform Offload Server Adapter about the error
    -----------------------------------------------------------------------*/
    if (TRUE == is_set)
    {
      error_cmd.cmd_id = PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_CMD_SET_ERROR;
    }
    else
    {
      error_cmd.cmd_id = PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_CMD_RESET_ERROR;
    }

    error_cmd.fltr_mask = ps_audio_offload_clienti_sio_convert_id_to_filter_mask(fltr_id);
    error_cmd.msg_size  = sizeof(error_cmd.msg);
    error_cmd.msg[0]    = (uint8)sio_error;

    bytes_tx = smdl_write(ps_audio_offload_client_sio_ctrl_info.smdl_handle,
                          sizeof(ps_audio_offload_client_sio_ctrl_cmd_type),
                          (char *) &error_cmd,
                          SMDL_WRITE_FLAGS_NONE);
    if (0 >= bytes_tx)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_WRITE_DATA;
      *dss_errno  = DS_ENETNONET;

      break;
    }

    ret_val = 0;
  } while (0);

  LOG_MSG_INFO1_3("ps_audio_offload_client_sioi_send_error(): "
                  "error reason %d, sio_error %d, is_set %d",
                  *err_reason, sio_error, is_set);

  return ret_val;
} /* ps_audio_offload_client_sioi_send_error() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIOI_EVENT_CBACK()

  DESCRIPTION:
  Callback routine for SIO events. Posts an event to PS task to process them
 
  INPUT/OUTPUT PARAMETERS:
  smdl_handle_type - SMDL handle on which event has occurred
  smdl_event - Event type
  user_data_ptr - Pointer to user provided data, contains client handle

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
void ps_audio_offload_client_sioi_event_cback
(
  smdl_handle_type   smdl_handle,
  smdl_event_type    smdl_event,
  void             * user_data_ptr
)
{
  ps_audio_offload_client_sio_event_info_type  * sio_event_info_ptr;
  int32                                          err_reason;
  int32                                          ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Allocate memory for event info
    -----------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(sio_event_info_ptr,
                             sizeof(ps_audio_offload_client_sio_event_info_type),
                             ps_audio_offload_client_sio_event_info_type* );
    if (NULL == sio_event_info_ptr)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_OUT_OF_SYSTEM_HEAP_MEM;
      break;
    }

    sio_event_info_ptr->smdl_handle = smdl_handle;
    sio_event_info_ptr->smdl_event  = smdl_event;

    sio_event_info_ptr->audio_offload_client_handle =
      (ps_audio_offload_client_handle_type) user_data_ptr;

    /*-----------------------------------------------------------------------
      Post a command to PS task to handle this event
    -----------------------------------------------------------------------*/
    ps_send_cmd(PS_AUDIO_OFFLOAD_CLIENT_SIO_EVENT_CMD, sio_event_info_ptr );

    ret_val = 0;
  } while (0);

  if (-1 == ret_val)
  {
    if (sio_event_info_ptr)
    {
    LOG_MSG_ERROR_3("ps_audio_offload_client_sioi_event_cback(): "
                    "Error reason %d, SMDL handle %d, SMDL event %d",
                    err_reason,
                    sio_event_info_ptr->smdl_handle,
                    sio_event_info_ptr->smdl_event);
    }
    else
    {
      LOG_MSG_ERROR_1("ps_audio_offload_client_sioi_event_cback(): "
                       "Error reason %d, SMDL handle %d, SMDL event %d",
                       err_reason);
    }
    ASSERT(0);
  }

  return;
} /* ps_audio_offload_client_sioi_event_cback() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIOI_PROCESS_EVENT()

  DESCRIPTION:
    Processes SMDL events

  INPUT/OUTPUT PARAMETERS:
  cmd - Command posted to process the event
  user_data_ptr - User data provided, contains client handle

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
void ps_audio_offload_client_sioi_process_event
(
  ps_cmd_enum_type    cmd,
  void              * user_data_ptr
)
{
  ps_audio_offload_client_sio_event_info_type  * sio_event_info_ptr = NULL;
  ps_audio_offload_client_sio_info_type        * sio_info_ptr = NULL;
  int32                                          err_reason;
  int16                                          dss_errno;
  int32                                          ret_val = -1;
  uint8                                          fltr_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_enter_crit_section(&ps_audio_offload_client_crit_sect);

  do
  {
    if (NULL == user_data_ptr ||
        NULL == ps_audio_offload_client_sio_event_cback_f_ptr)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      dss_errno  = DS_EFAULT;
      break;
    }

    sio_event_info_ptr =
      (ps_audio_offload_client_sio_event_info_type *) user_data_ptr;

    if (sio_event_info_ptr->smdl_handle ==
          ps_audio_offload_client_sio_ctrl_info.smdl_handle)
    {
      switch (sio_event_info_ptr->smdl_event)
      {
        case SMDL_EVENT_OPEN:
        {
          ps_audio_offload_client_sio_ctrl_info.smdl_state |=
            PS_AUDIO_OFFLOAD_CLIENT_SIO_STATE_OPEN;

          ret_val = 0;
          break;
        }

        case SMDL_EVENT_CLOSE:
        {
          ps_audio_offload_client_sio_ctrl_info.smdl_state &= 
            ~(PS_AUDIO_OFFLOAD_CLIENT_SIO_STATE_OPEN);

          ret_val = 0;
          break;
        }

        default:
        {
          break;
        }
      }
    }
    else
    {
      sio_info_ptr = ps_audio_offload_client_sioi_get_sio_info
                     (
                       sio_event_info_ptr->audio_offload_client_handle,
                       &fltr_id,
                       &err_reason,
                       &dss_errno
                     );
      if (NULL == sio_info_ptr)
      {
        if ((SMDL_EVENT_CLOSE == sio_event_info_ptr->smdl_event) ||
            (SMDL_EVENT_REMOTE_CLOSE == sio_event_info_ptr->smdl_event))
        {
          ret_val = 0;
        }
        break;
      }

      switch (sio_event_info_ptr->smdl_event)
      {
        case SMDL_EVENT_OPEN:
        {
          sio_info_ptr->smdl_state |= PS_AUDIO_OFFLOAD_CLIENT_SIO_STATE_OPEN;
          ps_audio_offload_client_sio_event_cback_f_ptr
          (
            sio_event_info_ptr->audio_offload_client_handle,
            PS_AUDIO_OFFLOAD_CLIENT_SIO_OPEN_EV
          );

          break;
        }

        case SMDL_EVENT_READ:
        {
          ps_audio_offload_client_sio_event_cback_f_ptr
          (
            sio_event_info_ptr->audio_offload_client_handle,
            PS_AUDIO_OFFLOAD_CLIENT_SIO_READ_EV
          );

          break;
        }

        case SMDL_EVENT_WRITE:
        {
          ps_audio_offload_client_sio_event_cback_f_ptr
          (
            sio_event_info_ptr->audio_offload_client_handle,
            PS_AUDIO_OFFLOAD_CLIENT_SIO_WRITE_EV
          );

          break;
        }

        case SMDL_EVENT_REMOTE_CLOSE:
        case SMDL_EVENT_CLOSE:
        {
          sio_info_ptr->smdl_state &= ~(PS_AUDIO_OFFLOAD_CLIENT_SIO_STATE_OPEN);

          ps_audio_offload_client_sio_event_cback_f_ptr
          (
            sio_event_info_ptr->audio_offload_client_handle,
            PS_AUDIO_OFFLOAD_CLIENT_SIO_CLOSE_EV
          );

          break;
        }
 
        default:
        {
          break;
        }
      }
    }

    ret_val = 0;
  } while (0);

  if (-1 == ret_val)
  {
    LOG_MSG_ERROR_1("ps_audio_offload_client_sioi_process_event(): "
                    "Error reason %d",err_reason);
    
    if(NULL != sio_event_info_ptr)
    {
      LOG_MSG_ERROR_2("ps_audio_offload_client_sioi_process_event(): "
                      "SMDL handle %d, SMDL event %d",
                       sio_event_info_ptr->smdl_handle,
                       sio_event_info_ptr->smdl_event);
    }
  }
  
  PS_SYSTEM_HEAP_MEM_FREE(user_data_ptr);
  ps_leave_crit_section(&ps_audio_offload_client_crit_sect);

  return;
} /* ps_audio_offload_client_sioi_process_event() */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIO_POWERUP()

  DESCRIPTION:
    Initialization routine for SIO module. Called during device boot up time.
    Opens a global SIO control port to talk to OSA/ADSP using control messages

  INPUT/OUTPUT PARAMETERS:
  sio_event_cback_f_ptr - Callback function for SIO events 

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
void ps_audio_offload_client_sio_powerup
(
  ps_audio_offload_client_sio_event_cback_type  sio_event_cback_f_ptr
)
{
  int32  err_reason;
  int32  ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (NULL == sio_event_cback_f_ptr)
    {
      err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      break;
    }

    /*-----------------------------------------------------------------------
      Check if SIO module is already initialized
    -----------------------------------------------------------------------*/
    if (NULL == ps_audio_offload_client_sio_ctrl_info.smdl_handle)
    {
      /*---------------------------------------------------------------------
        Open SMDL channel which is used to exchange control information with
        Offload Server Adapter
      ---------------------------------------------------------------------*/
      ps_audio_offload_client_sio_ctrl_info.smdl_handle =
      smdl_open(PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_PORT_NAME,
                SMD_MODEM_QDSP,
                SMDL_OPEN_FLAGS_MODE_PACKET,
                SMD_MIN_FIFO,
                ps_audio_offload_client_sioi_event_cback,
                NULL);
      if (NULL == ps_audio_offload_client_sio_ctrl_info.smdl_handle)
      {
        err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_OPEN;
        break;
      }

      /*---------------------------------------------------------------------
        Setup cmd handler and save call back
      ---------------------------------------------------------------------*/
      ps_audio_offload_client_sio_event_cback_f_ptr = sio_event_cback_f_ptr;

      ps_set_cmd_handler(PS_AUDIO_OFFLOAD_CLIENT_SIO_EVENT_CMD,
                         ps_audio_offload_client_sioi_process_event);
    }

    ret_val = 0;
  } while (0);

  if (-1 == ret_val)
  {
    LOG_MSG_ERROR_1("ps_audio_offload_client_sio_powerup(): Error reason %d",
                        err_reason);
    ASSERT(0);
  }
  else
  {
    LOG_MSG_INFO1_1("ps_audio_offload_client_sio_powerup(): "
                        "Opened SMDL ctrl channel 0x%x",
                        ps_audio_offload_client_sio_ctrl_info.smdl_handle);
  }

  return;
} /* ps_audio_offload_client_sio_powerup() */


/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIO_OPEN()

  DESCRIPTION:
    Opens an SIO data port to setup SMDL channel

  INPUT/OUTPUT PARAMETERS:
  filter_mask - Filter mask of the client
  audio_offload_client_handle - Client handle
  ip_family - IP family to speficy the IP traffic type
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any
    
  RETURN VALUE:
    Success or failure
 
    On error, the error condition value is stored in *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_sio_open
(
  uint8                                        filter_mask,
  ps_audio_offload_client_handle_type          audio_offload_client_handle,
  uint8                                        ip_family,
  int32                                      * err_reason,
  int16                                      * dss_errno
)
{
  char smdl_channel_name[PS_AUDIO_OFFLOAD_CLIENT_SIO_DATA_PORT_NAME_LEN];

  ps_audio_offload_client_sio_info_type      * sio_info_ptr = NULL;
  ps_audio_offload_client_sio_ctrl_cmd_type    setup_cmd;
  int32                                        ret_val = -1;
  int32                                        bytes_tx;
  uint8                                        fltr_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (-1 == ps_audio_offload_client_sioi_validate(audio_offload_client_handle,
                                                    err_reason,
                                                    dss_errno))
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Allocate memory for SIO info
    -----------------------------------------------------------------------*/
    sio_info_ptr =
      ps_audio_offload_client_sioi_alloc_sio_info(audio_offload_client_handle,
                                                  &fltr_id,
                                                  err_reason,
                                                  dss_errno);
    if (NULL == sio_info_ptr)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Formulate port name
    -----------------------------------------------------------------------*/
    (void) snprintf(smdl_channel_name,
                    sizeof(smdl_channel_name),
                    "%s%d",
                    PS_AUDIO_OFFLOAD_CLIENT_SIO_DATA_PORT_NAME,
                    filter_mask);

    /*-----------------------------------------------------------------------
      Open SMDL channel
    -----------------------------------------------------------------------*/
    sio_info_ptr->smdl_handle =
      smdl_open(smdl_channel_name,
                SMD_MODEM_QDSP,
                SMDL_OPEN_FLAGS_MODE_PACKET,
                SMD_MIN_FIFO,
                ps_audio_offload_client_sioi_event_cback,
                (void *) (sio_info_ptr->audio_offload_client_handle));
    if (NULL == sio_info_ptr->smdl_handle)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_OPEN;
      *dss_errno  = DS_EWOULDBLOCK;
      break;
    }

    /*-----------------------------------------------------------------------
      Inform Offload Server Adapter to open SMDL channel from its side
   ------------------------------------------------------------------------*/
    setup_cmd.cmd_id    = PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_CMD_SETUP;
    setup_cmd.fltr_mask = filter_mask;
    setup_cmd.msg_size  = sizeof(setup_cmd.msg);
    setup_cmd.msg[0]    = ip_family;

    bytes_tx = smdl_write(ps_audio_offload_client_sio_ctrl_info.smdl_handle,
                          sizeof(ps_audio_offload_client_sio_ctrl_cmd_type),
                          (char *) &setup_cmd,
                          SMDL_WRITE_FLAGS_NONE);
    if (0 >= bytes_tx)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_WRITE_DATA;
      *dss_errno  = DS_ENETNONET;
      break;
    }

    LOG_MSG_INFO1_3("ps_audio_offload_client_sio_open(): "
                        "Opened SMDL channel %d with SIO handle %d"
                        "Ip family in setup cmd is %d",
                        sio_info_ptr->smdl_handle,
                        sio_info_ptr->audio_offload_client_handle,
                        setup_cmd.msg);
    ret_val = 0;
  } while (0);

  /*-------------------------------------------------------------------------
    Clean up memory in case of failure
  -------------------------------------------------------------------------*/
  if (-1 == ret_val)
  {
    if (NULL != sio_info_ptr)
    {
      if (NULL != sio_info_ptr->smdl_handle)
      {
        (void) smdl_close(sio_info_ptr->smdl_handle);
      }

      ps_audio_offload_client_sioi_free_sio_info(sio_info_ptr, fltr_id);
    }
  }

  return ret_val;
} /* ps_audio_offload_client_sio_open() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIO_SET_ERROR()

  DESCRIPTION:
    Function to propagate error code to SIO module, which in turns
    pass it on to OSA/ADSP module over SMDL channel

  INPUT/OUTPUT PARAMETERS:
  audio_offload_client_handle - Client handle
  sio_error - SIO error enum type
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any
    
  RETURN VALUE:
    Success or failure
 
    On error, the error condition value is stored in *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_sio_set_error
(
  ps_audio_offload_client_handle_type            audio_offload_client_handle,
  ps_audio_offload_client_sio_error_enum_type    sio_error,
  int32                                        * err_reason,
  int16                                        * dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_audio_offload_client_sioi_send_error(audio_offload_client_handle,
                                                 sio_error,
                                                 TRUE,
                                                 err_reason,
                                                 dss_errno);
} /* ps_audio_offload_client_sio_set_error() */

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_CLIENT_SIO_RESET_ERROR()

  DESCRIPTION
    Function to reset the error code propagated earlier to SIO module. This in
    turns inform  OSA/ADSP module over SMDL channel to reset the error on
    its ends as well

  INPUT/OUTPUT PARAMETERS:
    audio_offload_client_handle : Client handle
    sio_error                   : SIO error enum type
    err_reason                  : Error reason to be returned, if any
    dss_errno                   : Error code to be returned, if any
    
  RETURN VALUE
    Success or failure
 
    On error, the error condition value is stored in *dss_errno.

  DEPENDENCIES
    SIO component must have been initialized before this call

  SIDE EFFECTS
    None
===========================================================================*/
int32 ps_audio_offload_client_sio_reset_error
(
  ps_audio_offload_client_handle_type            audio_offload_client_handle,
  ps_audio_offload_client_sio_error_enum_type    sio_error,
  int32                                        * err_reason,
  int16                                        * dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_audio_offload_client_sioi_send_error(audio_offload_client_handle,
                                                 sio_error,
                                                 FALSE,
                                                 err_reason,
                                                 dss_errno);
} /* ps_audio_offload_client_sio_reset_error() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIO_READ()

  DESCRIPTION:
    Reads data on the SIO port which calls into SMDL channel read API

  INPUT/OUTPUT PARAMETERS:
  audio_offload_client_handle - Client handle
  dsm_item_ptr - DSM item to store the data
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any
    
  RETURN VALUE:
    Success or failure
 
    On error, the error condition value is stored in *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_sio_read
(
  ps_audio_offload_client_handle_type        audio_offload_client_handle,
  dsm_item_type                           ** dsm_item_ptr_ptr,
  int32                                    * err_reason,
  int16                                    * dss_errno
)
{
  ps_audio_offload_client_sio_info_type      * sio_info_ptr = NULL;
  int32                                        ret_val = -1;
  int32                                        bytes_read;
  uint8                                        fltr_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (-1 == ps_audio_offload_client_sioi_validate(audio_offload_client_handle,
                                                    err_reason,
                                                    dss_errno))
    {
      break;
    }

    if (NULL == dsm_item_ptr_ptr)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      *dss_errno  = DS_EFAULT;
      break;
    }

    sio_info_ptr =
      ps_audio_offload_client_sioi_get_sio_info(audio_offload_client_handle,
                                                &fltr_id,
                                                err_reason,
                                                dss_errno);
    if (NULL == sio_info_ptr)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Make sure that port is opened from remote side
    -----------------------------------------------------------------------*/
    if (!(PS_AUDIO_OFFLOAD_CLIENT_SIO_STATE_OPEN & sio_info_ptr->smdl_state))
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_PORT_NOT_OPEN;
      *dss_errno  = DS_EWOULDBLOCK;
      break;
    }

    /*-----------------------------------------------------------------------
      Allocate DSM item and read data from SMDL
    -----------------------------------------------------------------------*/
    *dsm_item_ptr_ptr = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL);
    if (NULL == *dsm_item_ptr_ptr)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_OUT_OF_DSM_ITEM;
      *dss_errno  = DS_ENOMEM;
      break;
    }

    bytes_read = smdl_read(sio_info_ptr->smdl_handle,
                           (*dsm_item_ptr_ptr)->size,
                           (*dsm_item_ptr_ptr)->data_ptr,
                           SMDL_READ_FLAGS_NONE);
    if (0 > bytes_read)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_READ_DATA;
      *dss_errno  = DS_ENETNONET;
      dsm_free_packet(dsm_item_ptr_ptr);
      break;
    }
    else if (0 == bytes_read)
    {
      dsm_free_packet(dsm_item_ptr_ptr);
      break;
    }
    else
    {
      (*dsm_item_ptr_ptr)->used = (uint16) bytes_read;
    }

    ret_val = bytes_read;
  } while (0);

  return ret_val;
} /* ps_audio_offload_client_sio_read() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIO_WRITE()

  DESCRIPTION:
    Writes data on the SIO port which calls into SMDL channel write API

  INPUT/OUTPUT PARAMETERS:
  audio_offload_client_handle - Client handle
  dsm_item_ptr - DSM item holding the data to be written
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any
    
  RETURN VALUE:
    Success or failure
 
    On error, the error condition value is stored in *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_sio_write
(
  ps_audio_offload_client_handle_type        audio_offload_client_handle,
  dsm_item_type                           ** dsm_item_ptr,
  int32                                    * err_reason,
  int16                                    * dss_errno
)
{
  ps_audio_offload_client_sio_info_type      * sio_info_ptr = NULL;
  char                                       * buf;
  uint32                                       buf_size;
  int32                                        bytes_tx;
  int32                                        bytes_extracted;
  int32                                        ret_val = -1;
  uint8                                        fltr_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (-1 == ps_audio_offload_client_sioi_validate(audio_offload_client_handle,
                                                    err_reason,
                                                    dss_errno))
    {
      break;
    }

    if (NULL == dsm_item_ptr || NULL == *dsm_item_ptr)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG;
      *dss_errno  = DS_EFAULT;
      break;
    }

    sio_info_ptr =
      ps_audio_offload_client_sioi_get_sio_info(audio_offload_client_handle,
                                                &fltr_id,
                                                err_reason,
                                                dss_errno);
    if (NULL == sio_info_ptr)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Make sure that port is opened from remote side
    -----------------------------------------------------------------------*/
    if (!(PS_AUDIO_OFFLOAD_CLIENT_SIO_STATE_OPEN & sio_info_ptr->smdl_state))
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_PORT_NOT_OPEN;
      *dss_errno  = DS_EWOULDBLOCK;
      break;
    }

    buf_size = dsm_length_packet(*dsm_item_ptr);

    PS_SYSTEM_HEAP_MEM_ALLOC(buf, buf_size, char*);
    if (NULL == buf)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_OUT_OF_SYSTEM_HEAP_MEM;
      *dss_errno  = DS_ENOMEM;
      break;
    }

    bytes_extracted = dsm_extract(*dsm_item_ptr, 0, buf, (uint16)buf_size);
    if (buf_size != bytes_extracted)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_PULL_UP_DATA;
      *dss_errno  = DS_EINVAL;
      ASSERT(0);
      break;
    }

    /*-----------------------------------------------------------------------
      Write data to SMDL
    -----------------------------------------------------------------------*/
    bytes_tx = smdl_write(sio_info_ptr->smdl_handle,
                          buf_size,
                          buf,
                          SMDL_WRITE_FLAGS_NONE);

    PS_SYSTEM_HEAP_MEM_FREE(buf);

    if (0 > bytes_tx)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_WRITE_DATA;
      *dss_errno  = DS_ENETNONET;
      break;
    }
    else if (0 == bytes_tx)
    {
      LOG_MSG_INFO1_0("ps_audio_offload_client_sio_write(): "
                      "SMDL data port is flow controlled");
    }
    else
    {
      dsm_free_packet(dsm_item_ptr);
    }

    ret_val = bytes_tx;

  } while (0);

  return ret_val;
} /* ps_audio_offload_client_sio_write() */

/*===========================================================================
  FUNCTION:
  PS_AUDIO_OFFLOAD_CLIENT_SIO_CLOSE()

  DESCRIPTION:
    Closes SIO port given the filter mask. Closes the associated SMDL channel
    and frees the allocated resources

  INPUT/OUTPUT PARAMETERS:
  fltr_mask - Filter mask whose SIO ports needs to be closed
  audio_offload_client_handle - Client handle
  err_reason - Error reason to be returned, if any
  dss_errno - Error code to be returned, if any
    
  RETURN VALUE:
    Success or failure
 
    On error, the error condition value is stored in *dss_errno.

  SIDE EFFECTS:
    None
===========================================================================*/
int32 ps_audio_offload_client_sio_close
(
  uint8                                      fltr_mask,
  ps_audio_offload_client_handle_type        audio_offload_client_handle,
  int32                                    * err_reason,
  int16                                    * dss_errno
)
{
  ps_audio_offload_client_sio_info_type      * sio_info_ptr = NULL;
  ps_audio_offload_client_sio_ctrl_cmd_type    teardown_cmd;
  int32                                        ret_val = -1;
  int32                                        bytes_tx;
  uint8                                        fltr_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (-1 == ps_audio_offload_client_sioi_validate(audio_offload_client_handle,
                                                    err_reason,
                                                    dss_errno))
    {
      break;
    }

    sio_info_ptr =
      ps_audio_offload_client_sioi_get_sio_info(audio_offload_client_handle,
                                                &fltr_id,
                                                err_reason,
                                                dss_errno);
    if (NULL == sio_info_ptr)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Inform Offload Server Adapter to close SMDL channel from its side
   ------------------------------------------------------------------------*/
    teardown_cmd.cmd_id    = PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_CMD_TEARDOWN;
    teardown_cmd.fltr_mask = fltr_mask;
    teardown_cmd.msg_size  = 0;
    teardown_cmd.msg[0]    = 0;

    bytes_tx = smdl_write(ps_audio_offload_client_sio_ctrl_info.smdl_handle,
                          sizeof(ps_audio_offload_client_sio_ctrl_cmd_type),
                          (char *) &teardown_cmd,
                          SMDL_WRITE_FLAGS_NONE);
    if (0 > bytes_tx)
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_WRITE_DATA;
      *dss_errno  = DS_ENETNONET;
      break;
    }

    /*-----------------------------------------------------------------------
      Close SMDL channel
    -----------------------------------------------------------------------*/
    if (0 > smdl_close(sio_info_ptr->smdl_handle))
    {
      *err_reason = PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_CLOSE;
      *dss_errno  = DS_EWOULDBLOCK;
      break;
    }

    LOG_MSG_INFO1_1("ps_audio_offload_client_sio_close(): "
                  "Closed sio port for mask %d",
                  fltr_mask);

    ps_audio_offload_client_sioi_free_sio_info(sio_info_ptr, fltr_id);

    ret_val = 0;
  } while (0);

  return ret_val;
} /* ps_audio_offload_client_sio_close() */

#endif


