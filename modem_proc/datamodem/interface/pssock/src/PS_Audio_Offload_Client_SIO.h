#ifndef PS_AUDIO_OFFLOAD_CLIENT_SIO_H
#define PS_AUDIO_OFFLOAD_CLIENT_SIO_H
/*===========================================================================

                         PS _ OFFLOAD _ CLIENT _ SIO . H

DESCRIPTION

  Audio offload client SIO module APIs

Copyright (c) 2004-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/14/13    az      Initial version
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "dsm.h"
#include "ps_audio_offload_client_def.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/
typedef enum
{
  PS_AUDIO_OFFLOAD_CLIENT_SIO_OPEN_EV  = 0x01,
  PS_AUDIO_OFFLOAD_CLIENT_SIO_READ_EV  = 0x02,
  PS_AUDIO_OFFLOAD_CLIENT_SIO_WRITE_EV = 0x04,
  PS_AUDIO_OFFLOAD_CLIENT_SIO_CLOSE_EV = 0x08,
} ps_audio_offload_client_sio_event_enum_type;

typedef enum
{
  PS_AUDIO_OFFLOAD_CLIENT_SIO_DATA_EWOULDBLOCK  = 0x01,
  PS_AUDIO_OFFLOAD_CLIENT_SIO_DSM_EWOULDBLOCK   = 0x02,
  PS_AUDIO_OFFLOAD_CLIENT_SIO_ENETNONET_FATAL   = 0x04,
  PS_AUDIO_OFFLOAD_CLIENT_SIO_ENOMEM_FATAL      = 0x08,
} ps_audio_offload_client_sio_error_enum_type;


typedef void (*ps_audio_offload_client_sio_event_cback_type)
(
  ps_audio_offload_client_handle_type          audio_offload_client_handle,
  ps_audio_offload_client_sio_event_enum_type  sio_event
);


/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_CLIENT_SIO_POWERUP()

  DESCRIPTION
    Initialization routine for SIO module. Called during device boot up time.
    Opens a global SIO control port to talk to OSA/ADSP using control messages

  PARAMETERS
    sio_event_cback_f_ptr : Callback function for SIO events 
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ps_audio_offload_client_sio_powerup
(
  ps_audio_offload_client_sio_event_cback_type  sio_event_cback_f_ptr
);

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_CLIENT_SIO_OPEN()

  DESCRIPTION
    Opens an SIO data port to setup SMDL channel

  PARAMETERS
    filter_mask                 : Filter mask of the client
    audio_offload_client_handle : Client handle
    ip_family                   : IP family to speficy the IP traffic type
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
int32 ps_audio_offload_client_sio_open
(
  uint8                                        filter_mask,
  ps_audio_offload_client_handle_type          audio_offload_client_handle,
  uint8                                        ip_family,
  int32                                      * err_reason,
  int16                                      * dss_errno
);

int32 ps_audio_offload_client_sio_set_error
(
  ps_audio_offload_client_handle_type            audio_offload_client_handle,
  ps_audio_offload_client_sio_error_enum_type    sio_error,
  int32                                        * err_reason,
  int16                                        * dss_errno
);

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_CLIENT_SIO_SET_ERROR()

  DESCRIPTION
    Function to propagate error code to SIO module, which in turns
    pass it on to OSA/ADSP module over SMDL channel

  PARAMETERS
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
);

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_CLIENT_SIO_READ()

  DESCRIPTION
    Reads data on the SIO port which calls into SMDL channel read API

  PARAMETERS
    audio_offload_client_handle : Client handle
    dsm_item_ptr                : DSM item to store the data
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
int32 ps_audio_offload_client_sio_read
(
  ps_audio_offload_client_handle_type        audio_offload_client_handle,
  dsm_item_type                           ** dsm_item_ptr,
  int32                                    * err_reason,
  int16                                    * dss_errno
);

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_CLIENT_SIO_WRITE()

  DESCRIPTION
    Writes data on the SIO port which calls into SMDL channel write API

  PARAMETERS
    audio_offload_client_handle : Client handle
    dsm_item_ptr                : DSM item holding the data to be written
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
int32 ps_audio_offload_client_sio_write
(
  ps_audio_offload_client_handle_type        audio_offload_client_handle,
  dsm_item_type                           ** dsm_item_ptr,
  int32                                    * err_reason,
  int16                                    * dss_errno
);

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_CLIENT_SIO_CLOSE()

  DESCRIPTION
    Closes SIO port given the filter mask. Closes the associated SMDL channel
    and frees the allocated resources

  PARAMETERS
    fltr_mask                   : Filter mask whose SIO ports needs to be closed
    audio_offload_client_handle : Client handle
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
int32 ps_audio_offload_client_sio_close
(
  uint8                                      fltr_mask,
  ps_audio_offload_client_handle_type        audio_offload_client_handle,
  int32                                    * err_reason,
  int16                                    * dss_errno
);

#endif /* PS_AUDIO_OFFLOAD_CLIENT_SIO_H */
