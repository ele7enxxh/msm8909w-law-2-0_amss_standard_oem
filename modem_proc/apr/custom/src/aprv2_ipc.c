/*

  Copyright (C) 2010-2011, 2013-2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/apr/custom/src/aprv2_ipc.c#1 $
  $Author: mplcsds1 $
*/


#include "msg.h"
#include "mmstd.h"
#include "aprv2_ids_domains.h"
#include "aprv2_api_inline.h"
#include "aprv2_diag.h"
#include "aprv2_ipc.h"
#include "apr_smdl.h"
#ifdef BEAR_FAMILY_CHIPSETS
#include "aprv2_api.h"
#include "aprv2_api_i.h"
#endif /*BEAR_FAMILY_CHIPSETS*/
#include "apr_dbg_msg.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/* "apr_audio_svc" used for communicating with APPS */
#define APRV2_IPC_AUDIO_PORT_NAME ( "apr_audio_svc" )
#define APRV2_IPC_APPS2_PORT_NAME ( "apr_apps2" )



/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static apr_smdl_port_handle_t aprv2_ipc_apps_qdsp_port = NULL;
/* aprv2_ipc_apps_qdsp_port2 for ADSP test fwk */
static apr_smdl_port_handle_t aprv2_ipc_apps_qdsp_port2 = NULL;




#ifdef BEAR_FAMILY_CHIPSETS
  aprv2_ipc_send_fn_t aprv2_ipc_send_fn = aprv2_ipc_send_dummy;
#else
/* "apr_voice_svc" used for communicating with ADSP2 */
  #define APRV2_IPC_ADSP_PORT_NAME ( "apr_voice_svc" )
  static apr_smdl_port_handle_t aprv2_ipc_modem_qdsp_port = NULL;
#endif
/*****************************************************************************
 * Core Routine Implementations                                              *
 ****************************************************************************/

/* TODO: Consider moving rx packet handling to apr_smdl implementation. */
#ifndef SIM_DEFINED
static int32_t aprv2_ipc_rx_cb (
  void* rx_cb_data,
  void* rx_read_buf,
  uint32_t size
)
{
  int32_t rc;
  uint32_t packet_size;
  uint32_t alloc_type;
  int32_t handle;
  aprv2_packet_t* new_packet;
  
  for ( ;; )
  {
    new_packet = ( ( aprv2_packet_t* ) rx_read_buf );

    packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE( new_packet->header );

    alloc_type = ( ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, new_packet->header ) ==
                     APRV2_PKT_MSGTYPE_CMDRSP_V ) ?
                     APRV2_ALLOC_TYPE_RESPONSE_RAW :
                     APRV2_ALLOC_TYPE_COMMAND_RAW );

    rc = __aprv2_cmd_alloc(
          new_packet->dst_addr, /* SUPER_HACK: Allocate memory from destination service's packet pool */
          alloc_type, packet_size, &new_packet );
    if ( rc ) break;

    ( void ) mmstd_memcpy( new_packet, packet_size, rx_read_buf, packet_size );

    handle = ( ( uint32_t ) ( ( ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, new_packet->dst_addr ) ) << 8 ) |
                              ( APR_GET_FIELD( APRV2_PKT_SERVICE_ID, new_packet->dst_addr ) ) ) );

    ( void ) __aprv2_cmd_forward( handle, new_packet );

    return APR_EOK;
  }

  APR_DBG_1(APR_DBG_ERROR, "MODEM aprv2_ipc_rx_cb failed, rc = %d." , rc);
  return rc;
}

#endif /* SIM_DEFINED */

APR_INTERNAL int32_t aprv2_ipc_init ( void )
{
  int32_t rc;
  uint32_t checkpoint = 0;

  APR_DBG_0(APR_DBG_HIGH, "MODEM aprv2_ipc_init starting initialization sequence.");

  for ( ;; )
  {
#ifndef SIM_DEFINED
    rc = aprv2_diag_init( );
    if ( rc ) break;
    checkpoint = 1;

    rc = apr_smdl_init( );
    if ( rc ) break;
    checkpoint = 2;

    rc = apr_smdl_open( APRV2_IPC_AUDIO_PORT_NAME, sizeof( APRV2_IPC_AUDIO_PORT_NAME ),
                        APR_SMDL_APPS_MODEM_CHANNEL_ID, aprv2_ipc_rx_cb, NULL,
                        APR_SMDL_STANDARD_FIFO, &aprv2_ipc_apps_qdsp_port );
    if ( rc ) break;
    checkpoint = 3;

    rc = apr_smdl_open( APRV2_IPC_APPS2_PORT_NAME, sizeof( APRV2_IPC_APPS2_PORT_NAME ),
                        APR_SMDL_APPS_MODEM_CHANNEL_ID, aprv2_ipc_rx_cb, NULL,
                        APR_SMDL_STANDARD_FIFO, &aprv2_ipc_apps_qdsp_port2 );
    if ( rc ) break;
    checkpoint = 4;

#ifndef BEAR_FAMILY_CHIPSETS	
    /* "apr_voice_svc" for communicating with ADSP2. */
    rc = apr_smdl_open( APRV2_IPC_ADSP_PORT_NAME, sizeof( APRV2_IPC_ADSP_PORT_NAME ),
                        APR_SMDL_MODEM_QDSP_CHANNEL_ID, aprv2_ipc_rx_cb, NULL, 
                        APR_SMDL_STANDARD_FIFO, &aprv2_ipc_modem_qdsp_port );
    if ( rc ) break;
    checkpoint = 5;
#endif
#endif /* SIM_DEFINED */

#ifdef BEAR_FAMILY_CHIPSETS
    aprv2_ipc_send_fn = aprv2_ipc_send;
#endif /*BEAR_FAMILY_CHIPSETS*/

    APR_DBG_0(APR_DBG_HIGH, "MODEM aprv2_ipc_init succeed.");
    return APR_EOK;
  }

  switch ( checkpoint )
  {
#ifndef BEAR_FAMILY_CHIPSETS
  case 5:
    ( void ) apr_smdl_close( aprv2_ipc_modem_qdsp_port );
   /*-fallthru */
#endif
  case 4:
    ( void ) apr_smdl_close( aprv2_ipc_apps_qdsp_port2 );
    /*-fallthru */
  case 3:
    ( void ) apr_smdl_close( aprv2_ipc_apps_qdsp_port );
    /*-fallthru */
  case 2:
    ( void ) apr_smdl_deinit( );
    /*-fallthru */
#ifndef SIM_DEFINED
  case 1:
    ( void ) aprv2_diag_deinit( );
    /*-fallthru */
#endif /* SIM_DEFINED */
  default:
    break;
  }

  APR_DBG_1(APR_DBG_ERROR, "MODEM aprv2_ipc_init failed after checkpoint=%d.", checkpoint);

  return rc;
}

APR_INTERNAL int32_t aprv2_ipc_deinit ( void )
{
  APR_DBG_0(APR_DBG_HIGH, "MODEM aprv2_ipc_deinit start.");

#ifndef SIM_DEFINED
#ifndef BEAR_FAMILY_CHIPSETS
  ( void ) apr_smdl_close( aprv2_ipc_modem_qdsp_port );
#endif
  ( void ) apr_smdl_close( aprv2_ipc_apps_qdsp_port2 );
  ( void ) apr_smdl_close( aprv2_ipc_apps_qdsp_port );  
  ( void ) apr_smdl_deinit( );
  ( void ) aprv2_diag_deinit( );
#endif /* SIM_DEFINED */
  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_ipc_is_domain_local (
  uint16_t domain_id
)
{
  switch ( domain_id )
  {
  case APRV2_IDS_DOMAIN_ID_MODEM_V:

#ifndef BEAR_FAMILY_CHIPSETS
  case APRV2_IDS_DOMAIN_ID_ADSP_V:
#endif /* BEAR_FAMILY_CHIPSETS */

    return APR_EOK;

  default:
    return APR_EFAILED;
  }
}

APR_INTERNAL int32_t aprv2_ipc_send (
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EOK;
  uint16_t dst_domain_id;
  uint32_t packet_len;

  if ( packet == NULL )
  {
    return APR_EBADPARAM;
  }

#ifndef SIM_DEFINED
  if ( 
#ifndef BEAR_FAMILY_CHIPSETS
       ( aprv2_ipc_modem_qdsp_port == NULL ) ||
#endif
       ( aprv2_ipc_apps_qdsp_port2 == NULL ) ||
       ( aprv2_ipc_apps_qdsp_port == NULL )
     )
  {
    return APR_ENOTREADY;
  }
#endif /* SIM_DEFINED */

  packet_len =  APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );

  dst_domain_id  = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, packet->dst_addr );

  
  switch ( dst_domain_id )
  {
#ifndef SIM_DEFINED
  case APRV2_IDS_DOMAIN_ID_PC_V:
    rc = aprv2_diag_send( packet );
    break;
#endif /* SIM_DEFINED */

  case APRV2_IDS_DOMAIN_ID_APPS_V:
   
    rc = apr_smdl_write( aprv2_ipc_apps_qdsp_port, packet, packet_len );

    break;

  case APRV2_IDS_DOMAIN_ID_ADSP_V:
#ifdef BEAR_FAMILY_CHIPSETS
    rc = APR_ELPC;
    break;
#else //BEAR_FAMILY_CHIPSETS

    rc = apr_smdl_write( aprv2_ipc_modem_qdsp_port, packet, packet_len );

    break;
#endif

  case APRV2_IDS_DOMAIN_ID_SIM_V:
  case APRV2_IDS_DOMAIN_ID_MODEM_V:
    rc = APR_ELPC;
    break;

  case APRV2_IDS_DOMAIN_ID_APPS2_V:
    rc = apr_smdl_write( aprv2_ipc_apps_qdsp_port2, packet, packet_len );

    break;

  default:
    APR_DBG_1(APR_DBG_ERROR, "MODEM apr_ipc_send(0x%08X) sending to invaild domain", packet);
    rc = APR_EUNSUPPORTED;
    break;
  }

  if ( ( dst_domain_id != APRV2_IDS_DOMAIN_ID_PC_V ) && ( rc == APR_EOK ) )
  {
    ( void ) __aprv2_cmd_free( packet->dst_addr, packet );
  }
  return rc;
}

#ifdef BEAR_FAMILY_CHIPSETS
APR_INTERNAL int32_t aprv2_ipc_send_dummy (
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EOK;
  uint16_t dst_domain_id;
  aprv2_cmd_free_t free_args;

  if ( NULL == packet )
  {
    return APR_EBADPARAM;
  }
  

  dst_domain_id  = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, packet->dst_addr );

  switch ( dst_domain_id )
  {
    case APRV2_IDS_DOMAIN_ID_MODEM_V:
    case APRV2_IDS_DOMAIN_ID_ADSP_V:
      rc = APR_ELPC;
      break;

    default:
      APR_DBG_1(APR_DBG_ERROR, "aprv2_ipc_send_dummy(0x%08X) IPC not ready or send to invalid domains", packet);
      free_args.handle = 0;
      free_args.packet = packet;
      ( void ) aprv2_cmd_free( &free_args );
       break;
  }

  return rc;
}
#endif /*BEAR_FAMILY_CHIPSETS*/
