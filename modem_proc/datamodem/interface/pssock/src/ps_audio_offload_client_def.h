#ifndef PS_AUDIO_OFFLOAD_CLIENT_DEF_H
#define PS_AUDIO_OFFLOAD_CLIENT_DEF_H
/*===========================================================================

                         PS _ AUDIO _ OFFLOAD _ CLIENT _ DEF. H

DESCRIPTION

  PS Audio Offload Client - Definitions

  
Copyright (c) 2004-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/ps_audio_offload_client_def.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/09/13    msr    First revision
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_crit_sect.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

typedef int32 ps_audio_offload_client_handle_type;

/* Max filters supported by OC client */
#define PS_AUDIO_OFFLOAD_CLIENT_MAX_FLTR                   8
#define PS_AUDIO_OFFLOAD_CLIENT_MAX_PACKET_SIZE            1024
#define PS_AUDIO_OFFLOAD_CLIENT_INVALID_HANDLE            -1
#define PS_AUDIO_OFFLOAD_CLIENT_SOCKET_RCV_BUF_SIZE       (1024 * 5)

#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NONE                             0
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_CREATE_SOCK            1
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_BIND_SOCK              2
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_REG_FLTR               3
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_ALLOC_MEM              4
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NULL_ARG                         5
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_FLTR_ID                  6
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_HANDLE                   7
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_IFACE_ID                 8
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_NUM_FLTR_IN_FLTR_SPEC    9
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_IP_VSN                  10
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_NEXT_HDR_PROT           11
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_UDP_DST_PORT            12
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_OUT_OF_SYSTEM_HEAP_MEM          13
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_NO_FREE_FLTR                    14
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_FLTR_ID_IN_USE                  15
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_READ_SOCK             16
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_HANDLE_MGR                      17
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_FLOW_CONTROLLED            18
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_WRITE_SOCK            19
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_PULL_UP_ADDR          20
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_PUSH_DOWN_ADDR        21
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SOCK_COULD_NOT_REG_EVENT        22
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_PULL_UP_DATA          23
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_SET_SYS_SOCK_OPT      24
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_DEREG_FLTR            25
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_IFACE_PTR               26
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_IP_ADDR_MISMATCHED              27
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_CLOSE_SOCK            28
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_DSM_FLOW_CONTROLLED             29
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_FLTR_MASK               30
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_POOL_ID                 31
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_FLTR_MASK_MISMATCH              32
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_INVALID_FLTR_PTR                33
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_PACKET_SIZE_TOO_BIG             34
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_COULD_NOT_SET_RCV_BUF_SIZE      35

#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_NOT_INITIALIZED             50
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_NO_FREE_PORT                51
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_INVALID_STATE               52
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_SMDL_ERROR                  53
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_PORT_IN_USE                 54
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_OPEN              55
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_CLOSE             56
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_PORT_NOT_OPEN               57
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_OUT_OF_DSM_ITEM             58
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_READ_DATA         59
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_COULD_NOT_WRITE_DATA        60
#define PS_AUDIO_OFFLOAD_CLIENT_ERR_REASON_SIO_FLOW_CONTROLLED             61


extern ps_crit_sect_type ps_audio_offload_client_crit_sect;
#endif /* PS_AUDIO_OFFLOAD_CLIENT_DEF_H */
