#ifndef PS_AUDIO_OFFLOAD_CLIENT_SIOI_H
#define PS_AUDIO_OFFLOAD_CLIENT_SIOI_H
/*===========================================================================

                         PS _ OFFLOAD _ CLIENT . H

DESCRIPTION

  PS Audio Offload CLient.


EXTERNALIZED FUNCTIONS


Copyright (c) 2004-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/14/13    az
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_audio_offload_client_def.h"
#include "smd_lite.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/
#define PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_PORT_NAME  "offload_control"
#define PS_AUDIO_OFFLOAD_CLIENT_SIO_DATA_PORT_NAME  "offload_data"
#define PS_AUDIO_OFFLOAD_CLIENT_SIO_DATA_PORT_NAME_LEN   16

#define PS_AUDIO_OFFLOAD_CLIENT_SIO_STATE_OPEN         0x01
#define PS_AUDIO_OFFLOAD_CLIENT_SIO_STATE_WRITE        0x02

typedef struct
{
  smdl_handle_type                         smdl_handle;
  uint8                                    smdl_state;
} ps_audio_offload_client_sio_ctrl_info_type;


typedef struct
{
  ps_audio_offload_client_handle_type  audio_offload_client_handle;
  smdl_handle_type                     smdl_handle;
  uint8                                smdl_state;
} ps_audio_offload_client_sio_info_type;

typedef struct
{
  ps_audio_offload_client_handle_type       audio_offload_client_handle;
  smdl_handle_type                          smdl_handle;
  smdl_event_type                           smdl_event;
} ps_audio_offload_client_sio_event_info_type;


typedef PACKED struct PACKED_POST
{
  uint8    cmd_id;
  uint8    fltr_mask;
  uint16   msg_size;
  uint8    msg[1];
} ps_audio_offload_client_sio_ctrl_cmd_type;

#define PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_CMD_SETUP        0
#define PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_CMD_TEARDOWN     1
#define PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_CMD_SET_ERROR    2
#define PS_AUDIO_OFFLOAD_CLIENT_SIO_CTRL_CMD_RESET_ERROR  3

#endif /* PS_AUDIO_OFFLOAD_CLIENT_SIOI_H */
