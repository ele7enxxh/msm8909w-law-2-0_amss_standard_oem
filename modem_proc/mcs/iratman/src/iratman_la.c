/*!
  @file   iratman_la.c

  @brief  IRATMAN LTE Availability Module

  @details
  This module is used to track LTE Availability Data during power on and to keep
  this data persistent over power cycles. The information is used to dynamically
  calculate a value for Tbackoff.

*/
/*===========================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/iratman/src/iratman_la.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/15   cjb     Added Tbackoff capping algorithm
08/05/14   cjb     Avoid accessing de-allocated memory and code clean-up
03/27/14   pk      Added check for existence of directory, before creating it.
03/12/14   bph     Correct logic in update_white_list_below
01/29/14   bph     Align update_white_list_below w/ v4 requirements. Guard
                   against invalid node data in LA file.
01/24/14   bph     F3 removal/level audit
12/16/13   bph     Improve logging/clean up
12/16/13   bph     Define IRATMAN_LA_PATH for TEST_FRAMEWORK
12/10/13   bph     Remove dependancy on L1 for cell info
12/06/13   bph     FR3839 - dynamic tbackoff

===========================================================================*/
#include "iratman_i.h"
#include "iratman_la.h"
#include "iratman_rbt.h"
#include "iratman_dll.h"

#include "fs_public.h"
#include "md5.h"
#include "modem_mem.h"
#include "msg.h"
#include "ran.h"
#include <stringl/stringl.h> /* for memscpy */
#include "timetick.h"

#ifdef TEST_FRAMEWORK
  #error code not present
#else
  #define IRATMAN_LA_PATH      "/nv/item_files/mcs/iratman"
  #define IRATMAN_LA_FILE_PATH "/nv/item_files/mcs/iratman/la.bin"
  #define IRATMAN_NV_FILE_PATH "/nv/item_files/mcs/iratman/iratman.nv"
#endif

#define IRATMAN_LA_MIN_TBACKOFF  0
#define IRATMAN_LA_MAX_TBACKOFF 15

/*===========================================================================
  INTERNAL
===========================================================================*/
static boolean is_option3_enabled = FALSE;

/* Struct to contain all hash inputs */
typedef struct
{
  uint16                      earfcn;
  iratman_onex_cell_info_type onex_info;
  iratman_hdr_cell_info_type  do_sector_id;
} la_hash_input_type;


/*===========================================================================

  FUNCTION:  IRATAN_LA_PRINT_HASH_INPUT

===========================================================================*/
/*!
  @brief
    Print hash input.

  @details
    Print the hash input which is EARFCN, 1x cell info, and HDR cell info

  @note
    none.

  @return
    TRUE is data is valid, otherwise FALSE.

*/
/*=========================================================================*/
STATIC void iratman_la_print_hash_input
(
  la_hash_input_type *hash_input
)
{
  uint32 *print_do_cell_info_ptr = (uint32*)(hash_input->do_sector_id);

  /*----------------------------------------------------------------------*/

  MSG_8( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
         "EARFCN = %u :"
         " [1x sid=%u, nid=%u, base_id=%u]"
         " [HDR sector ID = 0x%08X %08X %08X %08]",
         hash_input->earfcn,
         hash_input->onex_info.sid,
         hash_input->onex_info.nid,
         hash_input->onex_info.base_id,
         print_do_cell_info_ptr[0],
         print_do_cell_info_ptr[1],
         print_do_cell_info_ptr[2],
         print_do_cell_info_ptr[3] );

} /* iratman_la_print_hash_input */


/*===========================================================================

  FUNCTION:  IRATAN_LA_GET_1X_CELL_INFO

===========================================================================*/
/*!
  @brief
    Get 1x cell info.

  @details
    This function gets the 1x cell info. If valid data exists, it will be copied
    to the input struct's 1x data, otherwise the struct's 1x data will be
    unchanged.

  @note
    none.

  @return
    none.

*/
/*=========================================================================*/
STATIC void iratman_la_get_1x_cell_info
(
  iratman_onex_cell_info_type *onex_cell_info
)
{
  iratman_cell_info_type cell_info;
  get_cell_info_cb_type get_cell_info_cb;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_get_1x_cell_info" );

  get_cell_info_cb = iratman_lte_get_cell_info_cb( RAT_1X );

  if ( get_cell_info_cb != NULL )
  {
    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW, "calling get_cell_info_cb");

    if ( get_cell_info_cb(&cell_info) == TRUE )
    {
      onex_cell_info->sid     = cell_info.onex_cell_info.sid;
      onex_cell_info->nid     = cell_info.onex_cell_info.nid;
      onex_cell_info->base_id = cell_info.onex_cell_info.base_id;
    }
    else
    {
      MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW,
           "valid cell info does not exist, using zeros");
    }
  }
  else
  {
    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW, "get_cell_info_cb is NULL");
  }

} /* iratman_la_get_1x_cell_info */


/*===========================================================================

  FUNCTION:  IRATAN_LA_GET_HDR_CELL_INFO

===========================================================================*/
/*!
  @brief
    Get HDR cell info.

  @details
    This function gets the HDR cell info. If valid data exists, it will be
    copied to the input struct's HDR data, otherwise the struct's HDR data will
    be unchanged.

  @note
    none.

  @return
    none.

*/
/*=========================================================================*/
STATIC void iratman_la_get_hdr_cell_info
(
  iratman_hdr_cell_info_type hdr_sector_ptr
)
{
  iratman_cell_info_type cell_info;
  get_cell_info_cb_type get_cell_info_cb;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_get_hdr_cell_info" );

  get_cell_info_cb = iratman_lte_get_cell_info_cb( RAT_HDR );

  if ( get_cell_info_cb != NULL )
  {
    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW, "calling get_cell_info_cb");

    if ( get_cell_info_cb(&cell_info) == TRUE )
    {
      memscpy(hdr_sector_ptr,
              sizeof(iratman_hdr_cell_info_type),
              cell_info.hdr_cell_info,
              sizeof(cell_info.hdr_cell_info));
    }
    else
    {
      MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW,
           "valid cell info does not exist, using zeros");
    }
  }
  else
  {
    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW, "get_cell_info_cb is NULL");
  }

} /* iratman_la_get_hdr_cell_info */


/*===========================================================================

  FUNCTION:  IRATAN_LA_PROCESS_NV_FILE

===========================================================================*/
/*!
  @brief
    Read lte availability file into RAM

  @details
    This function checks for the exisistance of the NV file, if it exisits,
    it reads the file and sets the appropriate variables.

  @note
    none.

  @return
    none.

*/
/*=========================================================================*/
STATIC void iratman_la_process_nv_file
(
   void
)
{
  struct fs_stat fstat_buf;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_process_nv_file" );

  if ( efs_stat(IRATMAN_NV_FILE_PATH, &fstat_buf) == 0 )
  {
    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH, "Enable option 3" );
    is_option3_enabled = TRUE;
  }

  // TODO: outstanding NV items.

} /* iratman_la_process_nv_file */


/*===========================================================================

  FUNCTION:  IRATAN_LA_READ_FILE

===========================================================================*/
/*!
  @brief
    Read lte availability file into RAM

  @details
    This function will check for the file defined by IRATMAN_LA_FILE_PATH.
    If it exisits it will attempt to read the contents into the memory pointed
    to by buf.

  @note
    Expectation is that buf points to memory size = max num nodes * num nodes

  @return
    On success: number of bytes read
    On failure: 0

*/
/*=========================================================================*/
STATIC uint32 iratman_la_read_file
(
   byte   *buf,
   size_t  max_buf_size
)
{
  struct fs_stat fstat_buf;
  uint32 bytes_read = 0;
  int result;
  int fd = -2;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_read_file" );

  do
  {
    if ( buf == NULL )
    {
      MSG( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
           "read_file: NULL ptr input" );
      break;
    }

    result = efs_stat(IRATMAN_LA_FILE_PATH, &fstat_buf);
    if( result != 0 )
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Failed IRATMan LTE availability file efs_stat %d", result );

      MSG_SPRINTF_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
                     "EFS file_path = %s", IRATMAN_LA_FILE_PATH );
      break;
    }

    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "IRATMan LTE availability EFS file size = %u bytes",
           fstat_buf.st_size);

    if( fstat_buf.st_size > max_buf_size )
    {
      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "IRATMan LTE availability EFS file size (%u) > max_size (%u)",
             fstat_buf.st_size,
             max_buf_size );
      break;
    }

    fd = efs_open( IRATMAN_LA_FILE_PATH, O_RDONLY );
    if( fd < 0 )
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "IRATMan LTE availability EFS file: fd failure %d", fd );
      break;
    }

    while( bytes_read < fstat_buf.st_size )
    {
      result = efs_read( fd, buf+bytes_read, fstat_buf.st_size-bytes_read );
      if ( result <= 0)
      {
        break;
      }
      bytes_read += result;
    }

    if( bytes_read != fstat_buf.st_size )
    {
      MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
             "Total bytes read (%u) != file_size (%u)",
             bytes_read,
             fstat_buf.st_size );
      break;
    }

  } while ( 0 );

  if ( fd >= 0 )
  {
    efs_close(fd);
  }

  return bytes_read;

} /* iratman_la_read_file */


/*===========================================================================

  FUNCTION:  IRATAN_LA_WRITE_FILE

===========================================================================*/
/*!
  @brief
  Dump contents of buf into a file

  @details
    This function attempts to write the contents of buf into the file defined
    by IRATMAN_LA_FILE_PATH.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_write_file
(
   byte   *buf,
   size_t  buf_len
)
{
  int fd;
  int result;
  struct fs_stat fstat_buf;
  uint32 bytes_written = 0;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_write_file" );

  /* do stat, if directory does not exist, create it. */
  result = efs_stat(IRATMAN_LA_PATH, &fstat_buf);
  if( result != 0 )
  {
    MSG_SPRINTF_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                    "creating directory %s", IRATMAN_LA_PATH );

    result = efs_mkdir ( IRATMAN_LA_PATH, 0755 );

    if ( result == -1 )
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "error creating directory (%d)", efs_errno);
    }
  }

  fd = efs_open(IRATMAN_LA_FILE_PATH, O_CREAT | O_WRONLY);
  if (fd < 0)
  {
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Unable to create/open EFS file for write(%d)", efs_errno);
    MSG_SPRINTF_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
                   "file_path = %s", IRATMAN_LA_FILE_PATH );
    return;
  }

  do
  {
    result = efs_write(fd, buf + bytes_written, buf_len - bytes_written);
  } while (result > 0 && (bytes_written += result) < buf_len);

  result = efs_close(fd);

} /* iratman_la_write_file */


/*===========================================================================

  FUNCTION:  IRATAN_LA_WRITE_NODE_DATA_TO_RAM

===========================================================================*/
/*!
  @brief
  Dump the contetns of the LTE availabiliy data struct sequentialy into RAM.

  @details
    This function writes LTE availability data struct data to sequential RAM.
    The the data is written from oldest to newest (tail of the DLL to the head).

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_write_node_data_to_ram
(
   iratman_la_type *la,
   byte            *buf
)
{
  iratman_la_node_type *node;
  iratman_la_node_data_type *node_data = (iratman_la_node_data_type*)(buf);
  int i;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_write_node_data_to_ram" );

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "node_count = %u, head key = 0x%08X, tail key = 0x%08X",
         la->node_count, la->head->data.key, la->tail->data.key);

  for ( i = 0, node = la->tail; node != NULL; i++, node = node->prev)
  {
    // add node data to buf
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "writting: 0x%08X", node->data.key);
    node_data[i] = node->data;
  }

} /* iratman_la_write_node_data_to_ram */

/*===========================================================================

  FUNCTION:  IRATAN_LA_INSERT

===========================================================================*/
/*!
  @brief
  Insert a node data into the LTE availability data structure.

  @details
    This function uses memory which is allocated in iratman_la_init to insert
    LTE Availability data into the LTE Availability data structure. Once this
    memory is used (data struct at capacity), it will remove the oldest node
    (tail of the DLL) and reuse that nodes memory to insert the new node data.
    The insert operation consists of inserting into the RBT and inserting to
    the head of the DLL (since this node data being inserted is the newest).

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_insert
(
   iratman_la_type *la,
   iratman_la_node_data_type *node_data
)
{
  iratman_la_node_type *new_node;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_insert" );

  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_LOW,
         "la->node_count = %u", la->node_count);

  if (la->node_count >= IRATMAN_LA_MAX_NODES)
  {
    /* at capacity, so remove oldest node to make room for new node */
    iratman_la_node_type *oldest_node = la->tail;

    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Removing key=0x%08X to make room for new", oldest_node->data.key);

    iratman_rbt_remove(&la->root, oldest_node);
    iratman_dll_remove(&la->head, &la->tail, oldest_node);

    /* just reuse old node memory for new node */
    new_node = oldest_node;
  }
  else
  {
    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW, "grab from node_memory" );

    /* not yet at capacity, so grab some data struct memory
       (allocated in iratman_la_init)
     */
    new_node = &(la->node_memory[la->node_count++]);
  }

  new_node->left   = NULL;
  new_node->right  = NULL;
  new_node->next   = NULL;
  new_node->prev   = NULL;
  new_node->data   = *node_data;

  MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW, "add to RBT");
  iratman_rbt_insert(&la->root, new_node);

  MSG( MSG_SSID_IRATMAN, MSG_LEGACY_LOW, "add to DLL");
  iratman_dll_insert_at_head(&la->head, &la->tail, new_node);

} /* iratman_la_insert */


/*===========================================================================

  FUNCTION:  IRATAN_LA_FIND

===========================================================================*/
/*!
  @brief
    Find a LTE Availability node by key.

  @details
    This function finds a LTE availability node by using the key to search the
    RBT.

  @note
    None

  @return
    If node exitst, returns a ptr to the node. Otherwise NULL.

*/
/*=========================================================================*/
STATIC iratman_la_node_type* iratman_la_find
(
   iratman_la_type *la,
   uint32           key
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_find" );
  return iratman_rbt_find(la->root, key);

} /* iratman_la_find */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_MAKE_FRESHEST

===========================================================================*/
/*!
  @brief
    Make a LTE Availability node the freshest (most recently visited)

  @details
    This function is used to make a LTE Availability node the freshest. This
    is called whenever a measurement event is reported.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_make_freshest
(
   iratman_la_type      *la,
   iratman_la_node_type *node
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_make_freshest" );
  iratman_dll_remove(&la->head, &la->tail, node);
  iratman_dll_insert_at_head(&la->head, &la->tail, node);

} /* iratman_la_make_freshest */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_CALC_START_SUCC

===========================================================================*/
/*!
  @brief
    Calculate start_succ

  @details
    Function to calculate the value of start_succ

  @note
    None

  @return
    start_succ

*/
/*=========================================================================*/
STATIC uint8 iratman_la_calc_start_succ
(
   uint8 min_tbackoff,
   uint8 max_tbackoff
)
{
  uint8 start_succ;

  /*----------------------------------------------------------------------*/

  /* sanitize inputs:
      Valid range for min/max tbackoff is 0 to 15 and min <= max (already vetted
      by iratman). The logic for dynamic backoff calculation below subtracts two
      from max and adds one to min.
   */
  if ( max_tbackoff < IRATMAN_LA_MIN_TBACKOFF + 2 )
  {
    max_tbackoff = IRATMAN_LA_MIN_TBACKOFF + 2;
  }
  if ( min_tbackoff > IRATMAN_LA_MAX_TBACKOFF - 1 )
  {
    min_tbackoff = IRATMAN_LA_MAX_TBACKOFF - 1;
  }

  start_succ = MAX( max_tbackoff - 2, min_tbackoff + 1 );

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "min_tbackoff=%u,max_tbackoff=%u,start_succ=%u",
          min_tbackoff, max_tbackoff, start_succ );

  return start_succ;

} /* iratman_la_calc_start_succ */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_CALC_MAX_UK

===========================================================================*/
/*!
  @brief
  Calculate max_uk

  @details
    Calculate the value of max_uk

  @note
    None

  @return
  max_uk

*/
/*=========================================================================*/
STATIC uint8 iratman_la_calc_max_uk
(
   uint8 min_tbackoff,
   uint8 max_tbackoff
)
{
  uint8 max_uk;

  /*----------------------------------------------------------------------*/

  /* sanitize inputs:
      Valid range for min/max tbackoff is 0 to 15 and min <= max (already vetted
      by iratman). The logic for dynamic backoff calculation below subtracts two
      from max and adds two to min.
   */
  if ( max_tbackoff < IRATMAN_LA_MIN_TBACKOFF + 2 )
  {
    max_tbackoff = IRATMAN_LA_MIN_TBACKOFF + 2;
  }
  if ( min_tbackoff > IRATMAN_LA_MAX_TBACKOFF - 2 )
  {
    min_tbackoff = IRATMAN_LA_MAX_TBACKOFF - 2;
  }

  max_uk = MAX(max_tbackoff-2, min_tbackoff+2);

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "min_tbackoff=%u, max_tbackoff=%u, max_uk=%u",
          min_tbackoff, max_tbackoff, max_uk );

  return max_uk;

} /* iratman_la_calc_max_uk */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_CALC_MAX_SUCC

===========================================================================*/
/*!
  @brief
  Calculate max_succ

  @details
    Calculate the value of max_succ

  @note
    None

  @return
  max_succ

*/
/*=========================================================================*/
STATIC uint8 iratman_la_calc_max_succ
(
   uint8 min_tbackoff,
   uint8 max_tbackoff
)
{
  uint8 max_succ;

  /*----------------------------------------------------------------------*/

  /* sanitize inputs:
      Valid range for min/max tbackoff is 0 to 15 and min <= max (already vetted
      by iratman). The logic for dynamic backoff calculation below adds four
      to min.
   */
  if ( min_tbackoff > IRATMAN_LA_MAX_TBACKOFF + 4)
  {
    min_tbackoff = IRATMAN_LA_MAX_TBACKOFF - 4;
  }

  max_succ = MIN(min_tbackoff+4, max_tbackoff);

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "min_tbackoff=%u, max_tbackoff=%u, max_succ=%u",
          min_tbackoff, max_tbackoff, max_succ );

  return max_succ;

} /* iratman_la_calc_max_succ */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_UPDATE_ABOVE_THRES

===========================================================================*/
/*!
  @brief
    Update node above threshold

  @details
    This funtion updates a LTE Availability node after it passes measurement
    criteria.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_update_above_thres
(
   iratman_la_node_data_type    *la_node_data,
   struct iratman_rat_node_type *rat_node
)
{
  uint8 start_succ;
  uint8 max_tbackoff = rat_node->data.lte.mc.max_tbackoff[RAT_INT];
  uint8 min_tbackoff = rat_node->data.lte.mc.min_tbackoff[RAT_INT];
  uint16 tbackoff_dur_s;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_update_above_thres" );

  start_succ = iratman_la_calc_start_succ(min_tbackoff, max_tbackoff);

  tbackoff_dur_s = ran_dist( iratman_sec_ran_next(1 << min_tbackoff),
                             ( word )( 1 << min_tbackoff ),
                             ( word )( 1 << start_succ ) + 1 );

  /* Update the actual Tbackoff duration to be used while setting the timer */
  rat_node->data.lte.mc.tbackoff_dur_ms[RAT_INT] = 1000 * tbackoff_dur_s;

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "min_tbackoff=%u,start_succ=%u,tbackoff_dur_s=%u",
          min_tbackoff, start_succ, tbackoff_dur_s );

  la_node_data->timestamp           = timetick_get();
  la_node_data->is_available        = TRUE;
  la_node_data->failure_count       = 0;
  la_node_data->last_tbackoff_dur_s = tbackoff_dur_s;

} /* iratman_la_update_above_thres */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_ADD_WHITE_LIST_NODE

===========================================================================*/
/*!
  @brief
    Add a new node to the white list.

  @details
    This function is used to add a new node to the white list.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_add_white_list_node
(
   iratman_la_type       *la,
   uint32                 key,
   iratman_rat_node_type *rat_node
)
{
  iratman_la_node_data_type la_data;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_add_white_list_node" );

  la_data.key = key;

  /* The setting the LTE Availability data for a new white list node or updating
     an existing node that is above threshold is the same. */
  iratman_la_update_above_thres(&la_data, rat_node);

  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Adding white list node: 0x%08X",
         la_data.key );

  iratman_la_insert(la, &la_data);

} /* iratman_la_add_white_list_node */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_BLACK_LIST_NODE

===========================================================================*/
/*!
  @brief
    Add a new node to the black list.

  @details
    This function is used to add a new node to the black list.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_add_black_list_node
(
   iratman_la_type       *la,
   uint32                 key,
   iratman_rat_node_type *rat_node
)
{
  iratman_la_node_data_type la_data;
  uint8 max_uk;
  uint8 max_tbackoff = rat_node->data.lte.mc.max_tbackoff[RAT_INT];
  uint8 min_tbackoff = rat_node->data.lte.mc.min_tbackoff[RAT_INT];
  uint16 tbackoff_dur_s;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_add_black_list_node" );

  max_uk = iratman_la_calc_max_uk( min_tbackoff, max_tbackoff );

  tbackoff_dur_s = ran_dist( iratman_sec_ran_next(1 << min_tbackoff),
                             ( word )( 1 << min_tbackoff ),
                             ( word )( 1 << max_uk ) + 1 );

  MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "min_tbackoff=%u, max_uk=%u, tbackoff_dur_s=%u",
         min_tbackoff, max_uk, tbackoff_dur_s );

  /* Update the actual Tbackoff duration to be used while setting the timer */
  rat_node->data.lte.mc.tbackoff_dur_ms[RAT_INT] = 1000 * tbackoff_dur_s;

  la_data.key                 = key;
  la_data.timestamp           = timetick_get();
  la_data.is_available        = FALSE;
  la_data.failure_count       = 1;
  la_data.last_tbackoff_dur_s = tbackoff_dur_s;

  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Adding black list node: 0x%08X",
         la_data.key );

  iratman_la_insert(la, &la_data);

} /* iratman_la_add_black_list_node */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_UPDATE_WHITE_LIST_BELOW_THRES

===========================================================================*/
/*!
  @brief
    Update white list node below threshold

  @details
    This funtion updates a white list node after it fails measurement criteria.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_update_white_list_below_thres
(
   iratman_la_node_data_type    *la_node_data,
   iratman_rat_node_type        *rat_node,
   iratman_lte_meas_result_type  result
)
{
  uint16 tbackoff_dur_s;
  uint8 max_tbackoff = rat_node->data.lte.mc.max_tbackoff[RAT_INT];
  uint8 min_tbackoff = rat_node->data.lte.mc.min_tbackoff[RAT_INT];

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_update_white_list_below_thres" );

  if (result == IRATMAN_LTE_MEAS_RESULT_ABOVE_CLOSE)
  {
    tbackoff_dur_s = la_node_data->last_tbackoff_dur_s;

    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Above close, using last. tbackoff_dur_s=%u",
            tbackoff_dur_s );
  }
  else /* result == IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE */
  {
    uint8 max_succ = iratman_la_calc_max_succ(min_tbackoff, max_tbackoff);

    tbackoff_dur_s = MIN( IRATMAN_LA_SUCC_MULT_FACTOR *
                          la_node_data->last_tbackoff_dur_s,
                          ( uint16 )( 1<<max_succ ) );

    MSG_4( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "Mult_factor_succ=%u, last_tbackoff_dur_s=%u, "
           "max_succ=%u, tbackoff_dur_s=%u",
           IRATMAN_LA_SUCC_MULT_FACTOR,
           la_node_data->last_tbackoff_dur_s,
           max_succ,
           tbackoff_dur_s);
  }

  /* Update the actual Tbackoff duration to be used while setting the timer */
  rat_node->data.lte.mc.tbackoff_dur_ms[RAT_INT] = 1000 * tbackoff_dur_s;

  /* once in white always in white
      -> don't touch is_available
      -> don't touch failure_count ( only relevant for black list nodes )
   */
  la_node_data->timestamp           = timetick_get();
  la_node_data->last_tbackoff_dur_s = tbackoff_dur_s;

} /* iratman_la_update_white_list_below_thres */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_UPDATE_BLACK_LIST_BELOW_THRES

===========================================================================*/
/*!
  @brief
    Update black list node below threshold

  @details
    This funtion updates a black list node after it fails measurement criteria.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_update_black_list_below_thres
(
   iratman_la_node_data_type    *la_node_data,
   iratman_rat_node_type        *rat_node,
   iratman_lte_meas_result_type  result
)
{
  uint16 tbackoff_dur_s;
  uint8 max_tbackoff = rat_node->data.lte.mc.max_tbackoff[RAT_INT];

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_update_black_list_below_thres" );

  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "failure_count=%u", la_node_data->failure_count );

  /*--------------- failcount < MAX ------------------------------------------*/
  if( la_node_data->failure_count < IRATMAN_LA_MAX_ATTEMPTS )
  {
    if( result == IRATMAN_LTE_MEAS_RESULT_ABOVE_CLOSE)
    {
      tbackoff_dur_s = la_node_data->last_tbackoff_dur_s;

      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "using last value,tbackoff_dur_s=%u",
             tbackoff_dur_s );
    }
    else /* result == IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE */
    {
      tbackoff_dur_s = MIN( IRATMAN_LA_UK_MULT_FACTOR *
                            la_node_data->last_tbackoff_dur_s,
                            (uint16)( 1 << max_tbackoff ) );

      MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "last_tbackoff=%u,max_tbackoff=%u,tbackoff_dur_s=%u",
             la_node_data->last_tbackoff_dur_s, max_tbackoff, tbackoff_dur_s );
    }
  }
  /*--------------- failcount == MAX -----------------------------------------*/
  else if( la_node_data->failure_count == IRATMAN_LA_MAX_ATTEMPTS )
  {
    uint16 s_in_one_hour = 60*60;
    uint16 min_fail = MIN( s_in_one_hour, (uint16)( 1 << max_tbackoff ) );
    tbackoff_dur_s = MAX( min_fail, la_node_data->last_tbackoff_dur_s );

    MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "min_fail=%u,last_tbackoff=%u,tbackoff_dur_s=%u",
           min_fail, la_node_data->last_tbackoff_dur_s, tbackoff_dur_s );
  }
  /*--------------- failcount > MAX ------------------------------------------*/
  else
  {
    if( result == IRATMAN_LTE_MEAS_RESULT_ABOVE_CLOSE)
    {
      tbackoff_dur_s = la_node_data->last_tbackoff_dur_s;

      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "using last value,tbackoff_dur_s=%u",
             tbackoff_dur_s );
    }
    else /* result == IRATMAN_LTE_MEAS_RESULT_BELOW_CLOSE */
    {
      tbackoff_dur_s = (uint16) MIN( (IRATMAN_LA_FAIL_MULT_FACTOR_10 *
                                      la_node_data->last_tbackoff_dur_s)/10,
                                     (uint16)( 1 << max_tbackoff ) );

      MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
             "last_tbackoff=%u,max_tbackoff=%u,tbackoff_dur_s=%u",
             la_node_data->last_tbackoff_dur_s, max_tbackoff, tbackoff_dur_s );
    }
  }

  /* Update the actual Tbackoff duration to be used while setting the timer */
  rat_node->data.lte.mc.tbackoff_dur_ms[RAT_INT] = 1000 * tbackoff_dur_s;

  la_node_data->timestamp           = timetick_get();
  la_node_data->last_tbackoff_dur_s = tbackoff_dur_s;

  /* Increment fail count until it hits max uint8 */
  if ( la_node_data->failure_count < 0xFF )
  {
    la_node_data->failure_count++;
  }

} /* iratman_la_update_black_list_below_thres */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_UPDATE_BELOW_THRES

===========================================================================*/
/*!
  @brief
    Update a node below threshold

  @details
    This funtion updates a LTE Availability node after it fails measurement
    criteria.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_update_below_thres
(
   iratman_la_node_data_type    *la_node_data,
   iratman_rat_node_type        *rat_node,
   iratman_lte_meas_result_type  result
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_update_below_thres" );

  if (la_node_data->is_available)
  {
    iratman_la_update_white_list_below_thres(la_node_data, rat_node, result);
  }
  else
  {
    iratman_la_update_black_list_below_thres(la_node_data, rat_node, result);
  }

} /* iratman_la_update_below_thres */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_GENERATE_MD5_HASH_KEY

===========================================================================*/
/*!
  @brief
  Calculate MD5 hash key

  @details
    This function passes the input into the MD5 library to get a 32 bit hash
    key.

  @note
    None

  @return
    32 bit hash key.

*/
/*=========================================================================*/
STATIC uint32 iratman_la_generate_md5_hash_key
(
   la_hash_input_type *input
)
{
  MD5_CTX md5_ctrl;
  uint32 md5_result[16/sizeof(uint32)]; /* for 4 byte alignment */

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_generate_md5_hash_key" );

  MD5Init( &md5_ctrl );
  MD5Update( &md5_ctrl, (unsigned char*)input, sizeof( *input ) );
  MD5Final( (unsigned char*)md5_result, &md5_ctrl );

  /* returning a 32 bit key, arbitrarily picking the first 32 bits */
  return md5_result[0];

} /* iratman_la_generate_md5_hash_key */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_GENERATE_HASH_KEY

===========================================================================*/
/*!
  @brief
    Generate hash key.

  @details
    This function is a wrapper to hide the specific hashing algorithm being used

  @note
    None

  @return
    32 bit hash key.

*/
/*=========================================================================*/
STATIC uint32 iratman_la_generate_hash_key
(
  la_hash_input_type *input
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_generate_hash_key" );
  return iratman_la_generate_md5_hash_key(input);

} /* iratman_la_generate_hash_key */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_GET_KEY_OPTION2

===========================================================================*/
/*!
  @brief
  Get option 2 style hash key.

  @details
    Optoin 2 hash key is the result of:
      H(earfcn, 1x sid, 1x nid, 1x base_id, do_sector_id)

  @note
    None

  @return
    32 bit hash key.

*/
/*=========================================================================*/
STATIC uint32 iratman_la_get_key_option2
(
   uint16 earfcn
)
{
  la_hash_input_type hash_input;
  uint32 hash_out;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_get_key_option2" );

  /* initialize hash input */
  memset( &hash_input, 0x0, sizeof(hash_input) );

  /* Setup hash input */
  hash_input.earfcn = earfcn;
  iratman_la_get_1x_cell_info( &hash_input.onex_info );
  iratman_la_get_hdr_cell_info( hash_input.do_sector_id );

  /* Print hash input */
  iratman_la_print_hash_input( &hash_input );

  /* Generate hash key */
  hash_out = iratman_la_generate_hash_key(&hash_input);

  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
         "Option-2: LA key = 0x%08X",
         hash_out );

  return hash_out;

} /* iratman_la_get_key_option2 */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_GET_KEY_OPTION3

===========================================================================*/
/*!
  @brief
  Get option 3 style hash key.

  @details
    Optoin 3 hash key is the result of:
      H(earfcn, <home_rat info>)

    When home_rat = RAT_1X,
    home_rat_info = 1x sid, 1x nid, 1x base_id + do_sector_id zero'ed out

    When home_rat = RAT_HDR
    home_rat_info = do_sector_id + 1x sid, nid, base_id zero'ed out

  @note
    None

  @return
    32 bit hash key.

*/
/*=========================================================================*/
STATIC uint32 iratman_la_get_key_option3
(
   uint16 earfcn,
   iratman_rat_e_type home_rat
)
{
  la_hash_input_type hash_input;
  uint32 hash_out;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_get_key_option3" );

  /* initialize hash input */
  memset( &hash_input, 0x0, sizeof(hash_input) );

  /* setup hash input EARFCN */
  hash_input.earfcn = earfcn;

  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_MED, "RAT = %u", home_rat );

  if (home_rat == RAT_1X)
  {
    /* setup hash input 1x cell info */
    iratman_la_get_1x_cell_info( &hash_input.onex_info );
  }
  else if (home_rat == RAT_HDR)
  {
    /* setup hash input HDR cell info */
    iratman_la_get_hdr_cell_info( hash_input.do_sector_id );
  }
  else
  {
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR, "Invalid rat: %u", home_rat );
  }

  /* print hash input */
  iratman_la_print_hash_input( &hash_input );

  /* Generate hash key */
  hash_out = iratman_la_generate_hash_key(&hash_input);

  MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
         "Option-3: LA key = 0x%08X",
         hash_out );

  return hash_out;

} /* iratman_la_get_key_option3 */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_UPDATE_DATA

===========================================================================*/
/*!
  @brief
  Update LTE Availability data.

  @details
    This function uses the key and result to update LTE Availability data
    and to set a new value of tbackoff for the rat_node.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_update_data
(
   iratman_la_type              *la,
   iratman_lte_meas_result_type  result,
   iratman_rat_node_type        *rat_node,
   uint32                        key
)
{
  iratman_la_node_type *la_node = iratman_la_find(la, key);

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_update_data" );

  if (la_node == NULL)
  {
    if (result == IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X)
    {
      iratman_la_add_white_list_node(la, key, rat_node);
    }
    else
    {
      iratman_la_add_black_list_node(la, key, rat_node);
    }

    /* No need to call make_freshest, a new node will always
       be added to the head of the DLL */
  }
  else
  {
    if (result == IRATMAN_LTE_MEAS_RESULT_ABOVE_THRES_X)
    {
      iratman_la_update_above_thres(&(la_node->data), rat_node);
    }
    else
    {
      iratman_la_update_below_thres(&(la_node->data), rat_node, result);
    }

    /* The most recently used node moves to the head of the DLL */
    iratman_la_make_freshest(la, la_node);
  }

} /* iratman_la_update_data */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_REPORT_EVENT_OPTION2

===========================================================================*/
/*!
  @brief
  Report event option 2 style.

  @details
    This function implements option 2.

    Option 2 is:
    - key = H(1x BASE_ID+SID/NID, DO SECTOR_ID, and EARFCN)
    - key is used to add/find/update LA data.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_report_event_option2
(
   iratman_la_type              *la,
   iratman_lte_meas_result_type  result,
   iratman_rat_node_type        *rat_node
)
{
  uint32 key;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_report_event_option2" );

  key = iratman_la_get_key_option2(rat_node->data.lte.mc.earfcn);

  iratman_la_update_data(la, result, rat_node, key);

} /* iratman_la_report_event_option2 */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_GET_OTHER_RAT_TYPE

===========================================================================*/
/*!
  @brief
    Get other RAT type.

  @details
    Funtion to get other rat type from home_rat. Used for option3.

  @note
    None

  @return
    RAT_HDR if home_rat = RAT_1X, RAT_HDR Otherwise.

*/
/*=========================================================================*/
STATIC iratman_rat_e_type iratman_la_get_other_rat_type
(
   iratman_rat_e_type home_rat
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_get_other_rat_type" );
  return( ( home_rat == RAT_1X ) ? RAT_HDR : RAT_1X );

} /* iratman_la_get_other_rat_type */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_SET_TBACKOFF_OPTION3

===========================================================================*/
/*!
  @brief
    Set rat_node's tbackoff according to option 3.

  @details
    Option 3 is summarized below:

    Tech A has node in: | Tech B has node in: | Value Used
    ----------------------------------------------------------------
    Black List          | Black List          | MAX(Tech A, Tech B)
    White List          | White List          | MIN(Tech A, Tech B)
    Black List          | White List          | Tech B's value
    White List          | Black List          | Tech A's value

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_set_tbackoff_option3
(
  iratman_la_node_type *techA_la_node,
  iratman_la_node_type *techB_la_node,
  iratman_rat_node_type *rat_node
)
{
  uint32 tbackoff_dur_s = 0;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_set_tbackoff_option3" );

  /* techA AND techB have the node in black list */
  if ( !techA_la_node->data.is_available &&
       !techB_la_node->data.is_available )
  {
    tbackoff_dur_s = MAX( techA_la_node->data.last_tbackoff_dur_s,
                          techB_la_node->data.last_tbackoff_dur_s );
  }
  /* techA AND techB have the node in white list */
  else if ( techA_la_node->data.is_available &&
            techB_la_node->data.is_available )
  {
    tbackoff_dur_s = MIN( techA_la_node->data.last_tbackoff_dur_s,
                          techB_la_node->data.last_tbackoff_dur_s );
  }
  /* techA has node in black list, techB has the node in white list */
  else if ( !techA_la_node->data.is_available &&
             techB_la_node->data.is_available)
  {
    tbackoff_dur_s = techB_la_node->data.last_tbackoff_dur_s;
  }
  /* techA has node in white list, techB has the node in black list */
  else if (  techA_la_node->data.is_available &&
             !techB_la_node->data.is_available)
  {
    tbackoff_dur_s = techA_la_node->data.last_tbackoff_dur_s;
  }

  MSG_2(MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
        "techA.avail=%u. techA.last_backoff=%u",
        techA_la_node->data.is_available,
        techA_la_node->data.last_tbackoff_dur_s);

  MSG_2(MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
        "techB.avail=%u. techB.last_backoff=%u",
        techB_la_node->data.is_available,
        techB_la_node->data.last_tbackoff_dur_s);

  MSG_1(MSG_SSID_IRATMAN, MSG_LEGACY_HIGH, "value used=%u", tbackoff_dur_s);

  /* Update the actual Tbackoff duration to be used while setting the timer */
  rat_node->data.lte.mc.tbackoff_dur_ms[RAT_INT] = 1000 * tbackoff_dur_s;

} /* iratman_la_set_tbackoff_option3 */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_REPORT_EVENT_OPTION3

===========================================================================*/
/*!
  @brief
    Report event option 3 style.

  @details
    This function implements option 3.

    Option 3 is:
    - Seperate LA data is maintained for 1x and HDR.
    - When a tech reports a result, that tech's LA data is updated, the other
      tech's LA data is not affected.
    - If LA data exists for both tech they will be combine to set rat_node's
      tbackoff (see function header for iratman_la_set_tbackoff_option3() for
      more details).

  @note
    None

  @return
    void

*/
/*=========================================================================*/
STATIC void iratman_la_report_event_option3
(
  iratman_la_type              *la,
  iratman_lte_meas_result_type  result,
  iratman_rat_node_type        *rat_node,
  iratman_rat_e_type            home_rat
)
{
  uint32                home_rat_key;
  uint32                other_rat_key;
  iratman_la_node_type *home_rat_la_node;
  iratman_la_node_type *other_rat_la_node;
  iratman_rat_e_type    other_rat;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_report_event_option3" );

  /* Update LA info for home_rat */
  MSG(MSG_SSID_IRATMAN, MSG_LEGACY_HIGH, "Get LA key for home RAT");
  home_rat_key = iratman_la_get_key_option3(rat_node->data.lte.mc.earfcn,
                                            home_rat);

  iratman_la_update_data(la, result, rat_node, home_rat_key);

  /* Check if other RAT has a cooresponding LA entry */
  other_rat = iratman_la_get_other_rat_type(home_rat);

  MSG(MSG_SSID_IRATMAN, MSG_LEGACY_HIGH, "Get LA key for other RAT");

  other_rat_key = iratman_la_get_key_option3(rat_node->data.lte.mc.earfcn,
                                             other_rat);
  other_rat_la_node = iratman_la_find(la, other_rat_key);

  /* If other RAT LA entry exists, use it to set tbackback for rat_node.
     otherwise done.
   */
  if ( other_rat_la_node != NULL )
  {
    MSG(MSG_SSID_IRATMAN, MSG_LEGACY_HIGH, "Other RAT LA entry exists");

    home_rat_la_node = iratman_la_find(la, home_rat_key);

    if (home_rat_la_node != NULL)
    {
      iratman_la_set_tbackoff_option3(home_rat_la_node,
                                      other_rat_la_node,
                                      rat_node);
    }
  }
  else
  {
    MSG(MSG_SSID_IRATMAN, MSG_LEGACY_HIGH, "Other RAT LA entry does NOT exist");
  }

} /* iratman_la_report_event_option3 */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_IS_NODE_VALID

===========================================================================*/
/*!
  @brief
    Validate node data

  @details
    Checks that data is in valid range.

  @note
    None

  @return
    TRUE if node is valid, otherwise FALSE.

*/
/*=========================================================================*/
STATIC boolean iratman_la_is_node_valid
(
   iratman_la_node_data_type *node_data
)
{
  boolean return_code = TRUE;

  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_is_node_valid" );

  /* Valid range for tbackoff dur s is: 1 - 2^15 */
  if ( node_data->last_tbackoff_dur_s == 0 ||
       node_data->last_tbackoff_dur_s > (1<<IRATMAN_LA_MAX_TBACKOFF) )
  {
    return_code = FALSE;
  }

  return return_code;

} /* iratman_la_is_node_valid */


/*===========================================================================
                      EXPORTED FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

  FUNCTION:  IRATMAN_LA_INIT

===========================================================================*/
/*!
  @brief
  Initialize the IRATMAN LTE Availability Module

  @details
    This function is called from iratman_init to setup the resources for the
    LTE Availability Module. Initialization includes checking NV items,
    allocating memory for the LTE Availability data struct, and reading the
    contents of IRATMAN_LA_FILE_PATH into the data struct.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_la_init
(
   iratman_la_type* la
)
{
  uint32 i;
  uint32 bytes_read;
  byte *tmp_buf;
  size_t max_file_size = sizeof(iratman_la_node_data_type) *
                         IRATMAN_LA_MAX_NODES;

  size_t max_data_struct_size  = sizeof(iratman_la_node_type) *
                                 IRATMAN_LA_MAX_NODES;

  /*----------------------------------------------------------------------*/

  iratman_la_node_data_type *node_data;

  IRATMAN_DEBUG_MSG( "iratman_la_init" );

  IRATMAN_ERR_FATAL_IF_NULL( la );

  if (la->initialized == TRUE)
  {
    return;
  }

  IRATMAN_INFO_MSG( "Initialize IRATMan LTE availability module" );

  /* Process NV file */
  iratman_la_process_nv_file();

  /* Allocate tmp buffer to read LTE Availability file into RAM */
  tmp_buf = modem_mem_alloc(max_file_size, MODEM_MEM_CLIENT_MCS_CRIT);
  if (tmp_buf == NULL)
  {
    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH, "modem_mem_alloc failed" );
    return;
  }

  /*  Allocate memory for fully populated lte availability data struct.
      This memory will be freed by iratman_la_deinit
   */
  la->node_memory = modem_mem_alloc(max_data_struct_size,
                                    MODEM_MEM_CLIENT_MCS_CRIT);
  if (la->node_memory == NULL)
  {
    MSG( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH, "modem_mem_alloc failed" );
    modem_mem_free(tmp_buf, MODEM_MEM_CLIENT_MCS_CRIT);
    return;
  }

  /* Read file into RAM */
  bytes_read = iratman_la_read_file(tmp_buf, max_file_size);

  if ( bytes_read % sizeof(iratman_la_node_data_type) != 0 )
  {
    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_ERROR,
           "Unexpected File Length(%u bytes)", bytes_read );
  }

  /* Initialize data struct pointers */
  la->head        = NULL;
  la->tail        = NULL;
  la->root        = NULL;
  la->node_count  = 0;

  /* Initialize pointer for reading RAM */
  node_data = (iratman_la_node_data_type*)(tmp_buf);

  /* Insert LA node data from RAM into the LTE Availability data struct */
  for( i = 0; i < bytes_read/sizeof(iratman_la_node_data_type); i++ )
  {
    MSG_5( MSG_SSID_IRATMAN, MSG_LEGACY_MED,
           "Adding LA node: key=0x%08X, timestamp=0x%08X, "
           "last_tbackoff_dur_s=%u, failure_count=%u, is_available=%u",
           node_data[i].key,
           node_data[i].timestamp,
           node_data[i].last_tbackoff_dur_s,
           node_data[i].failure_count,
           node_data[i].is_available );

    if ( iratman_la_is_node_valid(&node_data[i]) == TRUE )
    {
      iratman_la_insert(la, &node_data[i]);
    }
  }

  /* free tmp buffer */
  modem_mem_free(tmp_buf, MODEM_MEM_CLIENT_MCS_CRIT);

  MSG_2( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
         "Added %u/%u LA nodes into LTE availability data struct",
         la->node_count,
         bytes_read/sizeof(iratman_la_node_data_type) );

  la->initialized = TRUE;

} /* iratman_la_init */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_DEINIT

===========================================================================*/
/*!
  @brief
  De-initialize the IRATMAN LTE Availability Module

  @details
    This function is called from iratman_flush_persistent_data to flush the LTE
    Availability data struct to a file and to free the resources from the LTE
    Availability Module.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_la_deinit
(
   iratman_la_type *la
)
{
  /*----------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_deinit" );

  IRATMAN_ERR_FATAL_IF_NULL( la );

  if( la->initialized == FALSE )
  {
    return;
  }

  IRATMAN_INFO_MSG( "De-initialize IRATMan LTE availability module" );

  if( la->node_count > 0 )
  {
    size_t la_file_size;
    byte *tmp_buf;

    /*  Allocate tmp buffer to dump data struct sequentitaly into RAM
        (prep for a single call to efs_write).
     */
    la_file_size = sizeof(iratman_la_node_data_type) * la->node_count;

    MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_HIGH,
           "allocating tmp memory: %d", la_file_size );

    tmp_buf = modem_mem_alloc(la_file_size, MODEM_MEM_CLIENT_MCS_CRIT);

    if( tmp_buf == NULL )
    {
      MSG_1( MSG_SSID_IRATMAN, MSG_LEGACY_FATAL,
             "modem_mem_alloc failed for size %d",
             la_file_size );
    }
    else
    {
      iratman_la_write_node_data_to_ram(la, tmp_buf);

      iratman_la_write_file(tmp_buf, la_file_size);

      /* free tmp buffer memory */
      modem_mem_free(tmp_buf, MODEM_MEM_CLIENT_MCS_CRIT);
    }
  }

  /* free data structure memory (allocatd in iratman_la_init) */
  modem_mem_free( la->node_memory, MODEM_MEM_CLIENT_MCS_CRIT );

  /* Reset data struct pointers */
  la->head        = NULL;
  la->tail        = NULL;
  la->root        = NULL;
  la->node_count  = 0;
  la->initialized = FALSE;

} /* iratman_la_deinit */


/*===========================================================================

  FUNCTION:  IRATMAN_LA_REPORT_EVENT

===========================================================================*/
/*!
  @brief
  Used to report a measurement event to the LTE Availability Module.

  @details
    This function is called by iratman when a measure fails to meet reselection
    criteria, or when an iratman node has passes reselection.

  @note
    None

  @return
    void

*/
/*=========================================================================*/
void iratman_la_report_event
(
   iratman_la_type              *la,
   iratman_lte_meas_result_type  result,
   iratman_rat_node_type        *rat_node,
   iratman_rat_e_type            home_rat    /* only needed for option3 */
)
{
  /*------------------------------------------------------------------*/

  IRATMAN_DEBUG_MSG( "iratman_la_report_event" );

  IRATMAN_ERR_FATAL_IF_NULL( la );
  IRATMAN_ERR_FATAL_IF_NULL( rat_node );

  /* Check if LA is initialized */
  if( la->initialized == FALSE )
  {
    MSG_3( MSG_SSID_IRATMAN, MSG_LEGACY_FATAL,
           "LTE availability module not initialized. "
           "Ignoring report event: home_rat=%u, EARFCN=%u, result=%u",
           home_rat,
           rat_node->data.lte.mc.earfcn,
           result );

    return;
  }

  if( is_option3_enabled )
  {
    iratman_la_report_event_option3(la, result, rat_node, home_rat);
  }
  else
  {
    iratman_la_report_event_option2(la, result, rat_node);
  }

  /* Tbackoff capping / override */
  if( rat_node->data.lte.mc.tbackoff_dur_ms[RAT_INT] >
                        ( 1000 * ( (uint32) 1 << IRATMAN_LTE_TBACKOFF_CAP ) ) )
  {
    MSG_2( MSG_SSID_1X_SRCH, MSG_LEGACY_HIGH,
           "Tbackoff capped: Orig=%u ms --> Capped=%u ms",
           rat_node->data.lte.mc.tbackoff_dur_ms[RAT_INT],
           ( 1000 * ( (uint32) 1 << IRATMAN_LTE_TBACKOFF_CAP ) ) );

    rat_node->data.lte.mc.tbackoff_dur_ms[RAT_INT] =
                        ( 1000 * ( (uint32) 1 << IRATMAN_LTE_TBACKOFF_CAP ) );
  }

} /* iratman_la_report_event */
