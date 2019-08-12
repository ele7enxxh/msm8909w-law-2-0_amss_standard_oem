/*
   Copyright (C) 2013-2014 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/voicecfg/src/voicecfg.c#1 $
   $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <stdlib.h>
#include "mmstd.h"

#include "msg.h"
#include "fs_errno.h"
#include "fs_public.h"

#include "apr_comdef.h"
#include "apr_errcodes.h"

#include "drv_api.h"

#include "voicecfg_api.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define VOICECFG_FILE_FULLPATH ( "/nv/item_files/voice/voicecfg.bin" )
#define VOICECFG_MAX_FILE_SIZE ( 4096 )

#define VOICECFG_FILE_VERSION_1 ( 0x00012E5C )

#define VOICECFG_ITEM_TYPE_BOOL ( 0x00012E5D )

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

typedef struct voicecfg_file_header_t
{
  uint32_t size; /* Size the entire file. */
  uint32_t version_id; /* Currently supports VOICECFG_FILE_VERSION_1. */
}
  voicecfg_file_header_t;

typedef struct voicecfg_file_item_header_t
{
  uint32_t size; /* Size of the item containing header and data. */
  uint32_t item_id; /* The item identifier. */
  uint32_t type_id; /* The type identifier. */
  uint8_t data[]; /* The data. */
}
  voicecfg_file_item_header_t;

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

static voicecfg_file_header_t voicecfg_header;
static void* voicecfg_items = NULL;

/****************************************************************************
 * INTERNAL API ROUTINES                                                    *
 ****************************************************************************/

static bool_t voicecfg_item_type_id_is_valid (
  uint32_t type_id
)
{
  switch ( type_id )
  {
  case VOICECFG_ITEM_TYPE_BOOL:
    return TRUE;
  
  default:
    return FALSE;
  }
}

static uint32_t voicecfg_get_item (
  uint32_t item_id,
  uint32_t type_id,
  voicecfg_file_item_header_t** ret_item
)
{
  uint32_t rc;
  uint8_t* cur_ptr;
  uint8_t* end_ptr;
  voicecfg_file_item_header_t* cur_item;
  bool_t is_item_found = FALSE;

  for ( ;; )
  {
    /* Validate the input parameters. */
    if ( ret_item == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicecfg_get_item(): " \
            "ret_item(NULL)" );
      rc = APR_EBADPARAM;
      break;
    }

    if ( voicecfg_item_type_id_is_valid( type_id ) == FALSE )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicecfg_get_item(): Invalid " \
              "user-specified type_id(0x%08X)",
              type_id );
      rc = APR_EBADPARAM;
      break;
    }

    /* Check that there are items available to search. */
    if ( voicecfg_items == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicecfg_get_item(): No items " \
            "available. Check if the "
            "voice configuration file exists." );
      rc = APR_ENOTEXIST;
      break;
    }

    /* Find the specific item. */
    cur_ptr = ( ( uint8_t* ) voicecfg_items );
    end_ptr = ( ( ( uint8_t* ) voicecfg_items ) + voicecfg_header.size -
                sizeof( voicecfg_header ) );
    
    while ( cur_ptr < end_ptr )
    {
      cur_item = ( ( voicecfg_file_item_header_t* ) cur_ptr );

      if ( ( cur_ptr + cur_item->size ) > end_ptr )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicecfg_get_item(): " \
                "item_id(0x%08X) size(%d) goes beyond EOF",
                cur_item->item_id, cur_item->size );
        break;
      }

      if ( cur_item->item_id == item_id )
      {
        if ( cur_item->type_id == type_id )
        {
          is_item_found = TRUE;
          *ret_item = cur_item;
        }
        else
        {
          MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicecfg_get_item(): " \
                  "item_id(0x%08X) type_id(0x%08X) "
                  "doesn't match the requested type_id(0x%08X)",
                  cur_item->item_id, cur_item->type_id, type_id );
        }

        /* Stop searching for the item because we found what we were looking
         * for. 
         */
        break;
      }

      cur_ptr += cur_item->size;
    } /* while ( cur_ptr < end_ptr ) */

    if ( is_item_found == FALSE )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "voicecfg_get_item(): Couldn't " \
             "find item_id(0x%08X)",
             item_id );
      rc = APR_ENOTEXIST;
      break;
    }

    return APR_EOK;
  } /* for ( ;; ) */

  return rc;
}

static uint32_t voicecfg_get_bool_item (
  voicecfg_cmd_get_bool_item_t* params
)
{
  uint32_t rc;
  voicecfg_file_item_header_t* item = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      rc = APR_EBADPARAM;
      break;
    }

    rc = voicecfg_get_item( params->id, VOICECFG_ITEM_TYPE_BOOL, &item );
    if ( rc ) break;

    params->ret_value = ( ( bool_t ) item->data[ 0 ] );

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,  "voicecfg_get_bool_item(): Found " \
          "item_id(0x%08X) value(0x%02X)",
          item->item_id, params->ret_value );

    return APR_EOK;
  } /* for ( ;; ) */

  return rc;
}

static uint32_t voicecfg_init ( void )
{
  uint32_t checkpoint = 0;
  int32_t err;
  int fd;
  struct fs_stat f_stat;
  int32_t read_size;
  uint32_t items_size;

  for ( ;; )
  {
    fd = efs_open( VOICECFG_FILE_FULLPATH, O_RDONLY );
    if ( fd < 0 ) break;
    checkpoint = 1;

    read_size = efs_read( fd, &voicecfg_header, sizeof( voicecfg_header ) );
    if ( read_size != sizeof( voicecfg_header ) ) break;
    checkpoint = 2;

    err = efs_fstat( fd, &f_stat );
    if ( err < 0 ) break;
    checkpoint = 3;

    if ( f_stat.st_size != voicecfg_header.size ) break;
    checkpoint = 4;

    if ( voicecfg_header.size > VOICECFG_MAX_FILE_SIZE ) break;
    checkpoint = 5;

    if ( voicecfg_header.version_id != VOICECFG_FILE_VERSION_1 ) break;
    checkpoint = 6;

    voicecfg_items = malloc( voicecfg_header.size );
    if ( voicecfg_items == NULL ) break;
    checkpoint = 7;

    items_size = ( voicecfg_header.size - sizeof( voicecfg_header ) );

    read_size = efs_read( fd, voicecfg_items, items_size );
    if ( read_size < 0 ) break;
    checkpoint = 8;

    if ( ( ( uint32_t ) read_size ) != items_size ) break;
    checkpoint = 9;

    ( void ) efs_close( fd );

    return APR_EOK;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "Failed at checkpoint (%d)",
          checkpoint );

  switch ( checkpoint )
  {
  case 9:
  case 8:
  case 7:
    free( voicecfg_items );
    voicecfg_items = NULL;
    /*-fallthru */

  case 6:
  case 5:
  case 4:
  case 3:
  case 2:
  case 1:
    ( void ) efs_close( fd );
    /*-fallthru */

  default:
    break;
  }

  return APR_EFAILED;
}

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

APR_EXTERNAL uint32_t voicecfg_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc;

  switch ( cmd_id )
  {
    case DRV_CMDID_INIT:
      rc = voicecfg_init( );
      break;

    case DRV_CMDID_POSTINIT:
    case DRV_CMDID_PREDEINIT:
    case DRV_CMDID_DEINIT:
      rc = APR_EOK;
      break;

    case VOICECFG_CMD_GET_BOOL_ITEM:
      rc = voicecfg_get_bool_item( ( voicecfg_cmd_get_bool_item_t* ) params );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "Unsupported cmd_id (0x%08X)",
              cmd_id );
      rc = APR_EUNSUPPORTED;
      break;
  }

  return rc;
}

