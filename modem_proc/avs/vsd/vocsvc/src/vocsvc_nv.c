/*
  Copyright (C) 2012-2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/vocsvc/src/vocsvc_nv.c#1 $
   $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/
 
#include "msg.h"
#include "apr_comdef.h"            
#include "apr_errcodes.h"
#include "fs_errno.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "vocsvc_nv_api.h"
#include "mvs_diag_api.h"

/****************************************************************************
 * DEFINES                                                                *
 ****************************************************************************/

#define VOCSVC_NV_DIR ( "/nv/item_files/voice" )
#define VOCSVC_NV_FILE_PATH ( "/nv/item_files/voice/voicenv.bin" )
#define VOCSVC_NV_MVS_PROFILING_BUFFER ( "/nv/item_files/voice/mvs_profiling.bin" )
#define VOCSVC_NV_PERMISSIONS ( 0777 )
#define VOCSVC_NV_DEFAULT ( 0 ) /* Default value is disable. */

/****************************************************************************
 * DEFINITIONS                                                                *
 ****************************************************************************/

typedef struct vocsvc_nv_version_t
{
  uint16_t major;
  uint16_t minor;
  uint16_t revision;
}
  vocsvc_nv_version_t;

/*
  Always append new elements if needed.
  Avoid adding structure elements.
*/
typedef struct vocsvc_nv_item_t
{
  vocsvc_nv_version_t version; /* Version 3.1. */
    
  uint8_t eamr_enable_3g; /* Only intended for WCDMA, not applicable for TDSCDMA. */
  uint8_t eamr_enable_2g; /* Only intended for GSM. */
  uint8_t eamr_enable_tdscdma; /* Only intended for TDSCDMA. */
  uint8_t evrc_plus_enable; /* Currently unused. To be used to control EVRC+. */
}
  vocsvc_nv_item_t;

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

vocsvc_nv_item_t vocsvc_nv_items = 
{
  { 
    VOCSVC_NV_MAJOR_VERSION, VOCSVC_NV_MINOR_VERSION ,0 
  },
  VOCSVC_NV_DEFAULT, /* eAMR WCDMA. */
  VOCSVC_NV_DEFAULT, /* eAMR GSM. */ 
  VOCSVC_NV_DEFAULT, /* eAMR TDSCDMA. */
  VOCSVC_NV_DEFAULT  /* eEVRC+. */
};

bool_t vocsvc_nv_is_initialized = FALSE;

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/
 
static int32_t vocsvc_nv_init ( void )
{
  int fd;
  int result;
  int32_t rc;
  uint32_t checkpoint = 0;

  fd = efs_open( VOCSVC_NV_FILE_PATH, O_RDWR );
  for( ;; )
  {
    if ( fd < 0 ) break;
    checkpoint = 1; 
    
    result = efs_read( fd, &vocsvc_nv_items, sizeof( vocsvc_nv_items ) );
    efs_close( fd );
    if ( result < 0) break;
    checkpoint = 2;
 
    if ( result < sizeof( vocsvc_nv_items ) )
    {
      if ( vocsvc_nv_items.version.major != VOCSVC_NV_MAJOR_VERSION ) break;
      checkpoint = 3;        
      
      break; 
    }

    checkpoint = 4;
    break;
  }
   
  switch( checkpoint )
    {
  case 0 :
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "vocsvc_nv_init: voice nv file " \
          "missing" ); 
    rc = APR_ENOTEXIST;
    break;

  case 1:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "vocsvc_nv_init: voice nv file " \
           "read failed with error %d",
           efs_errno );
    rc = APR_EFAILED;
    break;

  case 2:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "vocsvc_nv_init: Major Version " \
            "Mismatch, Please load a compatible file of major version %d",
            VOCSVC_NV_MAJOR_VERSION );
    rc = APR_EFAILED;
    break;

  case 3:
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "vocsvc_nv_init: Expected version " \
           "%d.%d, only nv items before minor version %d take effect",
           VOCSVC_NV_MAJOR_VERSION,
           VOCSVC_NV_MINOR_VERSION,
           vocsvc_nv_items.version.minor );
    rc = APR_EVERSION;
    break;

  case 4:
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "vocsvc_nv_init: voice nv file read " \
         "success" );
    vocsvc_nv_is_initialized = TRUE;
    rc = APR_EOK;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "vocsvc_nv_init: Unexpected " \
            "checkPoint %d",
            checkpoint  );
    rc = APR_EUNEXPECTED;
    break;
  }
  return rc;
}

static int32_t vocsvc_nv_get_eamr_config( vocsvc_nv_eamr_config_t* eamr_config )
{
  eamr_config->enable_2g = vocsvc_nv_items.eamr_enable_2g;
  eamr_config->enable_3g = vocsvc_nv_items.eamr_enable_3g;
  eamr_config->enable_tdscdma = vocsvc_nv_items.eamr_enable_tdscdma;

  return APR_EOK;
}

static int32_t vocsvc_nv_get_evrc_plus_config( vocsvc_nv_evrc_plus_config_t* evrc_plus_config )
{
  evrc_plus_config->evrc_plus_is_enabled = vocsvc_nv_items.evrc_plus_enable;
  
  return APR_EOK;
}

static int32_t vocsvc_nv_create_nv_file( void* diagpkt_rsp )
{
  int fd;
  mvs_diag_voicenv_rsp_t* rsp = diagpkt_rsp;  
  
  rsp->bytes_written = 0;
  fd = efs_open( VOCSVC_NV_FILE_PATH, O_RDWR|O_CREAT, VOCSVC_NV_PERMISSIONS );
  if ( fd < 0 )
  {
    if ( efs_errno == ENOENT)
    {
      efs_mkdir( VOCSVC_NV_DIR,VOCSVC_NV_PERMISSIONS );
    }
    fd = efs_open( VOCSVC_NV_FILE_PATH, O_RDWR|O_CREAT, VOCSVC_NV_PERMISSIONS );
    if ( fd < 0 )
    {
      rsp->result = APR_EFAILED;
      return APR_EFAILED;
    }
  }
  rsp->bytes_written = ( uint32 )efs_write( fd, &vocsvc_nv_items, sizeof( vocsvc_nv_items ) );
  rsp->result = APR_EOK;
  efs_close( fd );
  return APR_EOK;
}

static int32_t vocsvc_nv_save_mvs_profiling_buffer( 
  vocsvc_nv_save_mvs_profiling_buffer_t* mvs_profiling 
)
{
  int fd;
  int32_t bytes_written;
  uint32_t bytes_left;

  if ( ( mvs_profiling == NULL ) ||
       ( mvs_profiling->buffer == NULL ) )
  {
    MSG_ERROR( "vocsvc_nv_save_mvs_profiling_buffer: bad parameter", 0, 0, 0 );
    return APR_EBADPARAM;
  }

  /* If file does not exist, file will be created. 
     Otherwise old file is overwritten. */
  fd = efs_open( VOCSVC_NV_MVS_PROFILING_BUFFER, 
                 O_CREAT|O_TRUNC|O_WRONLY, 
                 VOCSVC_NV_PERMISSIONS );
  if ( fd < 0 )
  {
    if ( efs_errno == ENOENT )
    {
      efs_mkdir( VOCSVC_NV_DIR, VOCSVC_NV_PERMISSIONS );
    }

    fd = efs_open( VOCSVC_NV_MVS_PROFILING_BUFFER, 
                   O_CREAT|O_TRUNC|O_WRONLY, 
                   VOCSVC_NV_PERMISSIONS );
    if ( fd < 0 )
    {
      MSG_ERROR( "vocsvc_nv_save_mvs_profiling_buffer: not able to open/create file", 0, 0, 0 );
      return APR_EFAILED;
    }
  }

  bytes_left = mvs_profiling->size;
  while ( bytes_left > 0 )
  {
    bytes_written = efs_write( fd, mvs_profiling->buffer, bytes_left );
    if ( bytes_written <= 0 )
    {
       MSG_ERROR( "vocsvc_nv_save_mvs_profiling_buffer: write failed", 0, 0, 0 );
       efs_close( fd );
       return APR_EFAILED;
    }
    else
    {
      bytes_left -= bytes_written;
    }
  }

  efs_close( fd );

  return APR_EOK;
}

APR_EXTERNAL int32_t vocsvc_nv_call (
  vocsvc_nv_callindex_enum_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  if ( vocsvc_nv_is_initialized == FALSE )
  {
    switch( index )
    {
      case VOCSVC_NV_CALLINDEX_ENUM_INIT:
        rc = vocsvc_nv_init( );
        break;
  
      case VOCSVC_NV_CALLINDEX_ENUM_CREATE_NV_FILE:
        rc = vocsvc_nv_create_nv_file( params );
        break;
	  
      default:
        return APR_ENOTREADY;
    }
    return rc;
  }
  switch ( index )
  {
    case VOCSVC_NV_CALLINDEX_ENUM_INIT:
      rc = vocsvc_nv_init( );
      break;
	  
    case VOCSVC_NV_CALLINDEX_ENUM_POSTINIT:
    case VOCSVC_NV_CALLINDEX_ENUM_PREDEINIT:
    case VOCSVC_NV_CALLINDEX_ENUM_DEINIT:
      rc = APR_EOK;
      break;
	  
    case VOCSVC_NV_CALLINDEX_ENUM_GET_EAMR_CONFIG:
      rc = vocsvc_nv_get_eamr_config( ( vocsvc_nv_eamr_config_t* )params );
      break;
	  
    case VOCSVC_NV_CALLINDEX_ENUM_CREATE_NV_FILE:
      rc = vocsvc_nv_create_nv_file( params );
      break;

     case VOCSVC_NV_CALLINDEX_ENUM_SAVE_MVS_PROFILING_BUFFER:
       rc = vocsvc_nv_save_mvs_profiling_buffer( ( vocsvc_nv_save_mvs_profiling_buffer_t* )params );
       break;

    case VOCSVC_NV_CALLINDEX_ENUM_GET_EVRC_PLUS_CONFIG:
      rc = vocsvc_nv_get_evrc_plus_config( ( vocsvc_nv_evrc_plus_config_t* ) params );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "vocsvc_nv_call: voice nv " \
              "unsupported index %d",
              index );
      rc = APR_EUNSUPPORTED;
      break;
  }
  return rc;
}

