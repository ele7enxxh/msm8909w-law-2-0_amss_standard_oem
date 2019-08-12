/*
   Copyright (C) 2012-2014 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/src/mvs_diag.c#1 $
   $Author: mplcsds1 $
*/


/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "msg.h"
#include "err.h"
#include "comdef.h"
#include "apr_errcodes.h"
#include "apr_log.h"
#include "diagbuf.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "vocsvc_nv_api.h"
#include "mvsi.h"
#include "mvs_diag_api.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define MVS_DIAG_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "mvs_diag:%d error[%d]", __LINE__, rc, 0 ); } }

/*list of MVS Subsystem Commands*/
#define MVS_DIAG_TARGET_NV_VER ( 0 )

#define MVS_DIAG_SET_LOOPBACK_COMMAND ( 1 )

#define MVS_DIAG_CREATE_VOICENV_FILE ( 2 )

#ifdef MVS_PROFILING_ENABLED
#define MVS_DIAG_ENABLE_PROFILING_TAP_POINT ( 3 )
#define MVS_DIAG_DISABLE_PROFILING_TAP_POINT ( 4 )
#define MVS_DIAG_CLEAR_PROFILING_BUFFER ( 5 )
#define MVS_DIAG_SAVE_PROFILING_BUFFER ( 6 )
#endif /* MVS_PROFILING_ENABLED */

#define VOID_DIAG_PACKED_PTR PACKED void* PACKED_POST

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

#if defined( T_WINNT ) || defined( FEATURE_WINCE )
   #error code not present
#endif

typedef PACKED struct PACKED_POST mvs_diag_target_ver_rsp_t
{
  diagpkt_subsys_header_type header;
  uint16 major_ver;
  uint16 minor_ver;
  uint16 revision;
}
  mvs_diag_target_ver_rsp_t;

typedef PACKED struct PACKED_POST mvs_diag_target_ver_req_t
{
  diagpkt_subsys_header_type header;
}
  mvs_diag_target_ver_req_t;

typedef PACKED struct PACKED_POST mvs_diag_create_voicenv_req_t
{
  diagpkt_subsys_header_type header;
  uint16 reserve;
}
  mvs_diag_create_voicenv_req_t;

typedef PACKED struct PACKED_POST mvs_diag_create_voicenv_rsp_t
{
  diagpkt_subsys_header_type header;
  mvs_diag_voicenv_rsp_t rsp;
}
  mvs_diag_create_voicenv_rsp_t;

#ifdef MVS_PROFILING_ENABLED
typedef PACKED struct PACKED_POST mvs_diag_enable_profiling_tap_point_t
{
  diagpkt_subsys_header_type header;
  uint32_t tap_point;
}
  mvs_diag_enable_profiling_tap_point_t;

typedef PACKED struct PACKED_POST mvs_diag_enable_profiling_tap_point_rsp_t
{
  diagpkt_subsys_header_type header;
  uint32_t status;
}
  mvs_diag_enable_profiling_tap_point_rsp_t;

typedef PACKED struct PACKED_POST mvs_diag_disable_profiling_tap_point_t
{
  diagpkt_subsys_header_type header;
  uint32_t tap_point;
}
  mvs_diag_disable_profiling_tap_point_t;

typedef PACKED struct PACKED_POST mvs_diag_disable_profiling_tap_point_rsp_t
{
  diagpkt_subsys_header_type header;
  uint32_t status;
}
  mvs_diag_disable_profiling_tap_point_rsp_t;

typedef PACKED struct PACKED_POST mvs_diag_clear_profiling_buffer_t
{
  diagpkt_subsys_header_type header;
}
  mvs_diag_clear_profiling_buffer_t;

typedef PACKED struct PACKED_POST mvs_diag_clear_profiling_buffer_rsp_t
{
  diagpkt_subsys_header_type header;
  uint32_t status;
}
  mvs_diag_clear_profiling_buffer_rsp_t;

typedef PACKED struct PACKED_POST mvs_diag_save_profiling_buffer_t
{
  diagpkt_subsys_header_type header;
}
  mvs_diag_save_profiling_buffer_t;

typedef PACKED struct PACKED_POST mvs_diag_save_profiling_buffer_rsp_t
{
  diagpkt_subsys_header_type header;
  uint32_t status;
}
  mvs_diag_save_profiling_buffer_rsp_t;
#endif /* MVS_PROFILING_ENABLED */

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

VOID_DIAG_PACKED_PTR mvsdiag_set_loopback (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);

VOID_DIAG_PACKED_PTR mvs_diag_get_target_nv_ver (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);

VOID_DIAG_PACKED_PTR mvs_diag_create_voicenv_file (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);

#ifdef MVS_PROFILING_ENABLED
VOID_DIAG_PACKED_PTR mvs_diag_enable_profiling_tap_point (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);

VOID_DIAG_PACKED_PTR mvs_diag_disable_profiling_tap_point (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);

VOID_DIAG_PACKED_PTR mvs_diag_clear_profiling_buffer (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);

VOID_DIAG_PACKED_PTR mvs_diag_save_profiling_buffer (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);
#endif /* MVS_PROFILING_ENABLED */

/*****************************************************************************
 * GLOBALS                                                              *
 ****************************************************************************/

static const diagpkt_user_table_entry_type mvsdiag_tbl[] =
{
  { MVS_DIAG_TARGET_NV_VER, MVS_DIAG_TARGET_NV_VER, mvs_diag_get_target_nv_ver },
  { MVS_DIAG_SET_LOOPBACK_COMMAND, MVS_DIAG_SET_LOOPBACK_COMMAND, mvsdiag_set_loopback },
  { MVS_DIAG_CREATE_VOICENV_FILE, MVS_DIAG_CREATE_VOICENV_FILE, mvs_diag_create_voicenv_file },
#ifdef MVS_PROFILING_ENABLED
  { MVS_DIAG_ENABLE_PROFILING_TAP_POINT, MVS_DIAG_ENABLE_PROFILING_TAP_POINT, mvs_diag_enable_profiling_tap_point },
  { MVS_DIAG_DISABLE_PROFILING_TAP_POINT, MVS_DIAG_DISABLE_PROFILING_TAP_POINT, mvs_diag_disable_profiling_tap_point },
  { MVS_DIAG_CLEAR_PROFILING_BUFFER, MVS_DIAG_CLEAR_PROFILING_BUFFER, mvs_diag_clear_profiling_buffer },
  { MVS_DIAG_SAVE_PROFILING_BUFFER, MVS_DIAG_SAVE_PROFILING_BUFFER, mvs_diag_save_profiling_buffer },
#endif /* MVS_PROFILING_ENABLED */
};

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

VOID_DIAG_PACKED_PTR mvsdiag_set_loopback (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
)
{
  return NULL;
}

VOID_DIAG_PACKED_PTR mvs_diag_get_target_nv_ver (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
)
{
  mvs_diag_target_ver_rsp_t* rsp_pkt;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_diag_get_target_nv_ver: Recieved " \
      "Packet" );
  rsp_pkt = ( mvs_diag_target_ver_rsp_t * )diagpkt_subsys_alloc(
  	                                           DIAG_SUBSYS_MVS, MVS_DIAG_TARGET_NV_VER,
  	                                           sizeof( mvs_diag_target_ver_rsp_t ) );
  if ( rsp_pkt == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_diag_get_target_nv_ver: Diag " \
          "Packet could not be allocated");
    return NULL;
  }
  rsp_pkt->major_ver = VOCSVC_NV_MAJOR_VERSION;
  rsp_pkt->minor_ver = VOCSVC_NV_MINOR_VERSION;
  rsp_pkt->revision = 0;
  diagpkt_commit( ( void* )rsp_pkt );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_diag_get_target_nv_ver: Rsp " \
        "Pkt: %d %d",
        rsp_pkt->major_ver, rsp_pkt->minor_ver );
  return  rsp_pkt ;
}

VOID_DIAG_PACKED_PTR mvs_diag_create_voicenv_file (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
)
{
  int32_t rc;
  mvs_diag_create_voicenv_rsp_t* rsp_pkt;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_diag_create_voicenv_file: " \
      "Recieved Packet" );
  rsp_pkt = ( mvs_diag_create_voicenv_rsp_t* )diagpkt_subsys_alloc(
												DIAG_SUBSYS_MVS, MVS_DIAG_CREATE_VOICENV_FILE,
												sizeof( mvs_diag_create_voicenv_rsp_t ) );
  if ( rsp_pkt == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_diag_create_voicenv_file: " \
          "Diag Packet could not be allocated");
    return NULL;
  }

  rc = vocsvc_nv_call( VOCSVC_NV_CALLINDEX_ENUM_CREATE_NV_FILE, &rsp_pkt->rsp, 0 );
  if( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "mvs_diag_create_voicenv_file: NV " \
          "file could not be created" );
  }

  diagpkt_commit( ( void* )rsp_pkt );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,  "mvs_create_voicenv_file: Rsp Pkt: " \
        "%d %d",
        rsp_pkt->rsp.result, rsp_pkt->rsp.bytes_written );
  return  rsp_pkt ;
}

#ifdef MVS_PROFILING_ENABLED
VOID_DIAG_PACKED_PTR mvs_diag_enable_profiling_tap_point (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
)
{
  uint32_t rc;
  mvs_diag_enable_profiling_tap_point_rsp_t* rsp_pkt;
  uint32_t tap_point = ( ( mvs_diag_enable_profiling_tap_point_t* )req_pkt )->tap_point;

  rc = mvs_profiling_enable_tap_point( tap_point );
  if ( rc == APR_EOK )
  {
    MSG_HIGH( "mvs_diag_enable_profiling_tap_point, pkt_len = %d, tap_point = %d", 
               pkt_len, tap_point, 0 );
  }
  else
  {
    MSG_ERROR( "mvs_diag_enable_profiling_tap_point, pkt_len = %d, tap_point = %d out of range", 
                pkt_len, tap_point, 0 );
  }

  /* Send response to DIAG service. */
  rsp_pkt = ( mvs_diag_enable_profiling_tap_point_rsp_t * )diagpkt_subsys_alloc(
  	                                                   DIAG_SUBSYS_MVS, MVS_DIAG_ENABLE_PROFILING_TAP_POINT,
  	                                                   sizeof( mvs_diag_enable_profiling_tap_point_rsp_t ) );
  if ( rsp_pkt == NULL )
  {
    MSG_ERROR( "mvs_diag_enable_profiling_tap_point: Diag Packet could not be allocated", 0, 0, 0 );
    return NULL;
  }

  rsp_pkt->status = rc;
  diagpkt_commit( ( void* )rsp_pkt );

  return rsp_pkt;  
}

VOID_DIAG_PACKED_PTR mvs_diag_disable_profiling_tap_point (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
)
{
  uint32_t rc;
  mvs_diag_disable_profiling_tap_point_rsp_t* rsp_pkt;
  uint32_t tap_point = ( ( mvs_diag_disable_profiling_tap_point_t* )req_pkt )->tap_point;

  rc = mvs_profiling_disable_tap_point( tap_point );
  if ( rc == APR_EOK )
  {
    MSG_HIGH( "mvs_diag_disable_profiling_tap_point, pkt_len = %d, tap_point = %d", 
    pkt_len, tap_point, 0 );
  }
  else
  {
    MSG_ERROR( "mvs_diag_disable_profiling_tap_point, pkt_len = %d, tap_point = %d out of range", 
    pkt_len, tap_point, 0 );
  }

  /* Send response to DIAG service. */
  rsp_pkt = ( mvs_diag_disable_profiling_tap_point_rsp_t * )diagpkt_subsys_alloc(
  	                                                    DIAG_SUBSYS_MVS, MVS_DIAG_DISABLE_PROFILING_TAP_POINT, 
  	                                                    sizeof( mvs_diag_disable_profiling_tap_point_rsp_t ) );
  if ( rsp_pkt == NULL )
  {
    MSG_ERROR( "mvs_diag_disable_profiling_tap_point: Diag Packet could not be allocated", 0, 0, 0 );
    return NULL;
  }

  rsp_pkt->status = rc;
  diagpkt_commit( ( void* )rsp_pkt );

  return rsp_pkt;  
}

VOID_DIAG_PACKED_PTR mvs_diag_clear_profiling_buffer (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
)
{
  mvs_diag_clear_profiling_buffer_rsp_t* rsp_pkt;

  mvs_profiling_clear_buffer( );
  MSG_HIGH( "mvs_diag_clear_profiling_buffer, pkt_len = %d", pkt_len, 0, 0 );

  /* Send response to DIAG service. */
  rsp_pkt = ( mvs_diag_clear_profiling_buffer_rsp_t * )diagpkt_subsys_alloc(
  	                                               DIAG_SUBSYS_MVS, MVS_DIAG_CLEAR_PROFILING_BUFFER, 
  	                                               sizeof( mvs_diag_clear_profiling_buffer_rsp_t ) );
  if ( rsp_pkt == NULL )
  {
    MSG_ERROR( "mvs_diag_clear_profiling_buffer: Diag Packet could not be allocated", 0, 0, 0 );
    return NULL;
  }

  rsp_pkt->status = APR_EOK;
  diagpkt_commit( ( void* )rsp_pkt );

  return rsp_pkt;  
}

VOID_DIAG_PACKED_PTR mvs_diag_save_profiling_buffer (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
)
{
  uint32_t rc;
  mvs_diag_save_profiling_buffer_rsp_t* rsp_pkt;

  rc = mvs_profiling_save_buffer( );
  if ( rc == APR_EOK )
  {
    MSG_HIGH( "mvs_diag_save_profiling_buffer, pkt_len = %d", pkt_len, 0, 0 );
  }
  else
  {
    MSG_ERROR( "mvs_diag_save_profiling_buffer, pkt_len = %d, failed, rc = %d", pkt_len, rc, 0 );
  }

  /* Send response to DIAG service. */
  rsp_pkt = ( mvs_diag_save_profiling_buffer_rsp_t * )diagpkt_subsys_alloc(
  	                                              DIAG_SUBSYS_MVS, MVS_DIAG_SAVE_PROFILING_BUFFER, 
  	                                              sizeof( mvs_diag_save_profiling_buffer_rsp_t ) );
  if ( rsp_pkt == NULL )
  {
    MSG_ERROR( "mvs_diag_save_profiling_buffer: Diag Packet could not be allocated", 0, 0, 0);
    return NULL;
  }

  rsp_pkt->status = rc;
  diagpkt_commit( ( void* )rsp_pkt );

  return rsp_pkt;  
}
#endif /* MVS_PROFILING_ENABLED */

static int32_t mvs_diag_register_with_diag ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "mvs_diag_register_with_diag: " \
       "Registering to DIAG_SUBSYS_MVS" );
  DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_MVS, mvsdiag_tbl );

  return APR_EOK;
}

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

static int32_t mvs_diag_init ( void )
{
  int32_t rc;
  rc = mvs_diag_register_with_diag( );

  return rc;
}

APR_EXTERNAL int32_t mvs_diag_call (
  mvs_diag_callindex_enum_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( index )
  {
    case MVS_DIAG_CALLINDEX_ENUM_INIT:
      rc = mvs_diag_init( );
      break;

    case MVS_DIAG_CALLINDEX_ENUM_POSTINIT:
    case MVS_DIAG_CALLINDEX_ENUM_PREDEINIT:
    case MVS_DIAG_CALLINDEX_ENUM_DEINIT:
      rc = APR_EOK;
      break;

    default:
      rc = APR_EUNSUPPORTED;
      break;
  }
  return rc;
}
