#ifndef __APRV2_IPC_H__
#define __APRV2_IPC_H__

/*
  Copyright (C) 2010-2011 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/apr/custom/inc/aprv2_ipc.h#1 $
  $Author: mplcsds1 $
*/

#include "apr_comdef.h"
#include "aprv2_packet.h"


/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define APR_IPC_PACKET_SIZE (4096)


APR_INTERNAL int32_t aprv2_ipc_init ( void );
APR_INTERNAL int32_t aprv2_ipc_deinit ( void );

APR_INTERNAL int32_t aprv2_ipc_is_domain_local ( uint16_t domain_id );
APR_INTERNAL int32_t aprv2_ipc_send ( aprv2_packet_t* packet );

#ifdef BEAR_FAMILY_CHIPSETS
typedef int32_t ( *aprv2_ipc_send_fn_t ) ( aprv2_packet_t* packet );
APR_INTERNAL int32_t aprv2_ipc_send_dummy ( aprv2_packet_t* packet );
#endif /*BEAR_FAMILY_CHIPSETS*/

#endif /* __APRV2_IPC_I_H__ */

