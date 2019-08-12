#ifndef _PS_AUDIO_OFFLOAD_CLIENTI_H
#define _PS_AUDIO_OFFLOAD_CLIENTI_H
/*===========================================================================

                         PS _ AUDIO _ OFFLOAD _ CLIENTI . H

DESCRIPTION

  PS Audio Offload Client - Internal definitions 

  Functions for managing VoIP audio offload client

EXTERNALIZED FUNCTIONS

  None

Copyright (c) 2004-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/ps_audio_offload_clienti.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/15/13    az     Initial revision
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/

#include "ps_audio_offload_client_def.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/
typedef struct
{
  ps_audio_offload_client_handle_type      audio_offload_client_handle;
  dsm_item_type                          * read_dsm_item_ptr;
  dsm_item_type                          * write_dsm_item_ptr;
  dss_iface_id_type                        iface_id;
  ps_audio_offload_client_stats_type       stats;
  int16                                    sockfd;
  int16                                    socket_event_mask;
  boolean                                  is_sio_write_enabled;
  uint8                                    ip_family;
} ps_audio_offload_clienti_fltr_info_type;


typedef struct
{
  ps_audio_offload_client_handle_type      audio_offload_client_handle;
  int16                                    sockfd;
  uint32                                   event_mask;
} ps_audio_offload_clienti_socket_event_info_type;

typedef struct
{
  dsm_mempool_id_enum_type                 pool_id;
  dsm_mem_level_enum_type                  event;
} ps_audio_offload_clienti_dsm_event_info_type;

typedef struct
{
  uint32 sock_addr_len;
  uint32 payload_len;
  union {
    struct ps_sockaddr_in  v4_sockaddr;
    struct ps_sockaddr_in6 v6_sockaddr;
  } sock_addr;
} ps_audio_offload_clienti_data_hdr;




/*===========================================================================

                              EXTERNAL FUNCTIONS

===========================================================================*/

#endif
