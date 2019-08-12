
/*!
   @file
   rfcommon_nv.c

   @brief
    leveraged from rfnv.c

   @details
   Functions to read and write NV items. There are two set of functions rfcommon_nv_*
   and rfcommon_rfnv_* the difference is that the rfcommon_nv_* functions are
   used to read old NV items store in NV manager and rfcommon_rfnv_* functions are
   used to read both, old NV items and new RFNV items.
 
   RFNV items are RF rela  ted items with item id greater or equal to 20,000 and are stored
   in a public EFS folder /nv/item_files/rfnv/. See @ref rfnv_items.h for RFNV id numbers.
*/

/*===========================================================================
Copyright (c) 2009 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/nv/src/rfcommon_nv.c#1 $
$DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/16   cv      Compiler warning removal
07/21/16   cv      Adding support for RX Static NV
07/05/16   cv      Adding LTE B66 support
12/08/15   mmd     Enabling RUMI featurization and removing the runtime flag use_rumi_nv_read
10/28/14   ac      function to retreive nv tuner items for DBDC
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
10/14/14   jmf     [selftest] Implement FBRx Noise Floor Cal
10/08/14   cri     Increase uncompression size for Rx V2 NV
10/07/14   weiz    Support PA static compressed NV and fix the compile warning
08/21/14   ndb     KW warning fix
05/27/14   pl      Adding support for Common RX NV
12/16/13   aa      Move fbrx related api's to rfcommon_fbrx_nv.c file  
10/16/13   vs      Added support for tuner LTE CA NV
11/21/13   aa      Moved FBRX Cal NV retrieve api to rfcommon_fbrx_nv.h 
11/19/13   aa      Added api to retrieve fbrx_gain_nv_item per tech per band
11/12/13   aa      Added support to retrieve RFNV_DATA_TYPE_FBRX_GAIN_V2 data
10/24/13   vv      Added a runtime flag(use_rumi_nv_read) to switch between EFS read and rumi NV read(hardcoded values) and removing RUMI featurization
09/30/13   vs      Added changes for tuner nv item code fail case to avoid errors
09/19/13   vs      Adding fix for handling TUNER NV error case of array initialization 
08/27/13   vs      Added api for retrieving tuner NV item code per band per tech
08/26/13   bsh     Use the correct RUMI Flag for NVs 
08/13/13   ndb     Replace memcpy with memscpy
08/05/13   vbh     Update the RUMI feature flag 
08/02/13   jl      NV Buffer Cleared before Reuse
07/30/13   bsh     Added EFS bypass for RUMI 
07/29/13   aca     RL NV version update
07/23/13   dbz     Add FBRX gain switch point NV support
07/18/13   dbz     Add support for common API to intepret FBRX gain v.s. freq NV
07/02/13   ndb     Bug fixes in rfcommon_load_ant_tuner_cl_ctrl_info_from_nv()
06/28/13   ndb     Added rfcommon_load_ant_tuner_cl_ctrl_info_from_nv()
04/11/13   ems     Added support for NV items larger than 4K 
04/10/12   kg      Adding support for rfc.bl creation
10/11/12   aki     Create RFNV BL file and RFNV folder only if missing
11/21/11   sr      Only critical NVs missing will be F3 Error Msgs.
09/01/11   sar     Fixed typo, changing std.h to stdio.h.
08/26/11   sar     Replaced AEE lib with std lib.
07/06/11   aki     And back to: modem heap, fix kw, etc
06/30/11   aki     Reverting back to previous version, for now
06/22/11   aki     Allocate RFNV buffer from modem heap, fix kw and compiler 
                   warnings, misc fixes
03/24/11   pl      Fix warning
07/28/10   aki     Reduced the stack size requirement for rfnv functions, 
                   init update
07/09/10   aki     All RFNV files are now 'regular' files. Init will create 
                   config and folders. 
07/01/10   aki     Merging RFNV FTM changes from dev/rfnv_dev
06/23/10   aki     RFNV modifications  
04/02/10   adk     Merged branch scmm_mdm9k_merge_final
03/19/10   lcl     Pull in fixes from QSC6295
02/10/10   sar     Clocwork fixes.
02/02/10   sar     Clocwork fixes.
01/26/10   sar     Lint fixes.
02/27/09   sr      Initial version
===========================================================================*/
#include "stringl.h" /* for memscpy */
#include "task.h"
#include "rfcommon_nv.h"
#include "msg.h"

#include "modem_mem.h"
#include "rfcommon_msg.h"
#include "stringl.h"

#include "fs_lib.h"
#include "fs_public.h"
#include <stdio.h>  
#include <memory.h> 
#include "zlib.h"

/*Bypass EFS read; will be replaced by RAMDISK Solution*/
#ifdef T_RUMI_EMULATION
   #include "rfrumi.h"
#endif 

#define RFNV_CONFIG_FILE_PATH "/nv/item_store/rfnv"         // Path of RFNV config file for QPST backup
#define RFNV_CONFIG_FILE RFNV_CONFIG_FILE_PATH "/rfnv.bl"   // Path and name of RFNV config file
#define RFC_BL_FILE RFNV_CONFIG_FILE_PATH "/rfc.bl"         // Path and name of RFC bl file
#define RFC_FILE_PATH "/rfc/"


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function set a specific NV item to NV.

  @details

  @param item_code : Item to set
  @param item_ptr  : Pointer to the item
  @param task_ptr  : Calling task's TCB pointer
  @param task_wait_sig : Task signal to wait for when reading NV.
  @param task_wait_handler : Task's wait function to be called when reading NV
 
  @retval
  NV_DONE_S       - request done
  NV_NOTACTIVE_S  - item was not active
  NV_BADPARM_S    - invalid parameter 
*/
nv_stat_enum_type rfcommon_nv_set_item(nv_items_enum_type item_code, 
                             nv_item_type *item_ptr, rex_tcb_type *task_ptr, 
                             rex_sigs_type task_wait_sig,
                             void (*task_wait_handler)( rex_sigs_type ))
{
  /* Command buffer to NV */
  static nv_cmd_type  nv_cmd_buf;   

  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd = NV_WRITE_F;               /* Write request        */
  nv_cmd_buf.tcb_ptr = task_ptr;             /* Notify back to me    */
  nv_cmd_buf.sigs = task_wait_sig;           /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
  nv_cmd_buf.item = item_code;               /* Item to set          */
  nv_cmd_buf.data_ptr = item_ptr;            /* Where to return it   */

  /* Clear signal, issue the command, and wait for the respone. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );                       

  /* Wait for completion  */
  if(task_wait_handler!=NULL)
  {
    task_wait_handler( task_wait_sig );                
  }

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* return status */
   return (nv_cmd_buf.status);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function performs a Single NV Read operation to retrieve the requested 
  item.

  @details

  @param item_code : Item to get
  @param item_ptr  : Pointer where to put the item 
  @param task_ptr  : Calling task's TCB pointer
  @param task_wait_sig : Task signal to wait for when reading NV.
  @param task_wait_handler : Task's wait function to be called when reading NV 

  @retval
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
    NV_BADPARM_S    - invalid parameter
*/


LOCAL nv_stat_enum_type rfcommon_nv_get_item(nv_items_enum_type item_code,
                                                nv_item_type *item_ptr,
                                                rex_tcb_type *task_ptr,
                                                rex_sigs_type task_wait_sig,
                                                void (*task_wait_handler)( rex_sigs_type ))
{
   /* Command buffer to NV */
   static nv_cmd_type  nv_cmd_buf;

   /*Bypass EFS read; will be replaced by RAMDISK Solution*/
   #ifdef T_RUMI_EMULATION

      boolean rfrumi_nv_get_success = FALSE;
      rfrumi_nv_get_success = rfrumi_nv_get_item((rfnv_item_id_enum_type)item_code, item_ptr);
      if (rfrumi_nv_get_success)
      {
         MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "RF NV item %d found in rumi nv list", item_code);
         return(NV_DONE_S);
      }
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF NV item %d is not found in rumi nv list", item_code);
      return(NV_BADTG_S);
   #endif

   /* Prepare command buffer to NV. */
   nv_cmd_buf.cmd = NV_READ_F;                /* Read request         */
   nv_cmd_buf.tcb_ptr = task_ptr;             /* Notify back to me    */
   nv_cmd_buf.sigs = task_wait_sig;           /* With this signal     */
   nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
   nv_cmd_buf.item = item_code;               /* Item to get          */
   nv_cmd_buf.data_ptr = item_ptr;            /* Where to return it   */

   /* Clear signal, issue the command, and wait for the respone. */
   /* Clear signal for NV  */
   (void) rex_clr_sigs(task_ptr, task_wait_sig);

   /* Issue the request    */
   nv_cmd(&nv_cmd_buf);

   /* Wait for completion  */
   if(task_wait_handler != NULL)
   {
     task_wait_handler(task_wait_sig);
   }
   /* clear the signal again */
   (void) rex_clr_sigs( task_ptr, task_wait_sig );

   /* return status */
   return (nv_cmd_buf.status);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  

  @details

  @param task_ptr  : Calling task's TCB pointer
  @param task_wait_sig : Task signal to wait for when reading NV.
  @param task_wait_handler : Task's wait function to be called when reading NV 

  @retval
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

*/

rfcommon_nv_status_type 
rfcommon_nv_retrieve_items(rfcommon_nv_info_type *nv_info_table, 
                           int16 nv_info_table_size, rex_tcb_type *task_ptr, 
                           rex_sigs_type task_wait_sig,
                           void (*task_wait_handler)( rex_sigs_type ))
{
   nv_item_type nv_item_buf; /* NV item buffer */
   nv_stat_enum_type nv_read_status; /* NV read status */
   int16 nv_item_cnt; /* loop counter for RF NV item fetches */

   /* Pointer to the list of RF NV items */
   const rfcommon_nv_items_list_type *nv_item_list;  
   int16 nv_item_list_siz; /* Number of items in RF NV items list */
   int16 table_index; /* index to each mode */
   rfcommon_nv_status_type status = RFCOMMON_NV_READ_SUCCESS;

   for (table_index = 0; table_index < nv_info_table_size; table_index++) 
   {
     if (nv_info_table[table_index].nv_data_valid_flag == TRUE)
     {
       /* get the pointer to NV items list for each mode */
       nv_item_list =  nv_info_table[table_index].nv_item_list_ptr;

       /* get the list size */
       if( nv_info_table[table_index].get_nv_item_list_size != NULL)
         nv_item_list_siz = (nv_info_table[table_index].get_nv_item_list_size)();
       else
         nv_item_list_siz = 0;

       /* for each NV item in the list, read from NV and copy into local buffer. */
       /* then copy from local buffer into RF NV table for each mode */
       for (nv_item_cnt = 0; nv_item_cnt < nv_item_list_siz; nv_item_cnt++)
       {
         /* read from NV into local buffer */
         nv_read_status = rfcommon_nv_get_item(nv_item_list[nv_item_cnt].nv_item_id, 
                                               &nv_item_buf,
                                               task_ptr,
                                               task_wait_sig,
                                               task_wait_handler );

         /* if read status is good */
         if (nv_read_status == NV_DONE_S)
         {
            /* copy from local buffer into RF NV table */
            nv_info_table[table_index].copy_nv_item( nv_item_list[nv_item_cnt].nv_item_id, &nv_item_buf ); 
         }
         else if (nv_read_status == NV_NOTACTIVE_S)
         {
            MSG_ERROR( "RF NV item %d for mode %d is not available, attr=0x%x",
                       nv_item_list[nv_item_cnt].nv_item_id, table_index, 
                       nv_item_list[nv_item_cnt].nv_item_attrib );

            if (nv_item_list[nv_item_cnt].nv_item_attrib & NV_ITEM_ATTRIB_CRTCL_BMSK)
            {
               nv_info_table[table_index].nv_data_valid_flag = FALSE;
               status = RFCOMMON_NV_INACTIVE_ITEM;
            }
         }
         else
         {
            MSG_ERROR( "NV read failed on RF NV item %d for mode %d, , attr=0x%x", 
                       nv_item_list[nv_item_cnt].nv_item_id, table_index, 
                       nv_item_list[nv_item_cnt].nv_item_attrib );
            
            if (nv_item_list[nv_item_cnt].nv_item_attrib & NV_ITEM_ATTRIB_CRTCL_BMSK)
            {
               nv_info_table[table_index].nv_data_valid_flag = FALSE;
               status = RFCOMMON_NV_READ_FAILED;
            }
         }

       } /*for (nv_item_cnt = 0; nv_item_cnt < nv_item_list_siz; nv_item_cnt++)*/
     } /* rfnv_is_nv_data_valid */
   } /*for (mode_idx = 0; mode_idx < RFNV_SUPPORTED_MODE_NUM; mode_idx++)*/

   return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function creates a file and writes data_size bytes to it from data buffer.
 
  @details
  Support function for rfnv EFS operations
 
  @retval
    Returns 0 if the item is written successfully, or -1 for error. 
*/
static int32 rfcommon_rfnv_create_file(const char *file, const byte *data, int32 data_size)
{
  int fd = -1;

  fd = efs_open (file, O_TRUNC | O_WRONLY | O_CREAT, 0777);
  if (fd < 0)
  {
    return -1;
  }

  if (data_size != efs_write (fd, data, (fs_size_t)data_size))
  {
    (void) efs_close (fd);
    return -1;
  }
  
  return efs_close (fd);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepares EFS for RFNV

  @details
  Function creates config file for RFNV items back from EFS. Config file will
  inidicate the folder from where all RFNV items should be backed up.
 
  - File to be created: "/nv/item_store/rfnv/rfnv.bl"
  - File content: "/nv/item_files/rfnv/"

 
  @retval
    Returns >=0 if the item is written successfully, or <0 for error. 
*/
int32 rfcommon_rfnv_init(void)
{
  int status = 0;
  struct fs_stat temp_buf;

  if(efs_stat(RFNV_CONFIG_FILE, &temp_buf) == -1)
  {
    // Force path creation for RFNV.BL file
    status |= efs_put (RFNV_CONFIG_FILE, RFNV_FILE_PATH, strlen(RFNV_FILE_PATH), O_CREAT | O_AUTODIR, 0777);
    status |= efs_unlink(RFNV_CONFIG_FILE);
    // We want the RFNV.BL file be a normal file
    status |= rfcommon_rfnv_create_file(RFNV_CONFIG_FILE, (byte*)RFNV_FILE_PATH, (int32)strlen(RFNV_FILE_PATH));
  }

  if(efs_stat(RFNV_FILE_PATH, &temp_buf) == -1)
  {
    // Lets do the same for RFNV file path
    char fn[RFNV_MAX_PATH] = "";    
    snprintf(fn, RFNV_MAX_PATH, "%s%s", RFNV_FILE_PATH, "BAADFOOD"); 
    status |= efs_put (fn, RFNV_FILE_PATH, strlen(RFNV_FILE_PATH), O_CREAT | O_AUTODIR, 0777);
    status |= efs_unlink(fn);
  }

  if(efs_stat(RFC_BL_FILE, &temp_buf) == -1)
  {
	// Force path creation for RFC.BL file
    status |= efs_put (RFC_BL_FILE,RFC_FILE_PATH, strlen(RFC_FILE_PATH), O_CREAT | O_AUTODIR, 0777);
    status |= efs_unlink(RFC_BL_FILE);
    // We want the RFC.BL file be a normal file
    status |= rfcommon_rfnv_create_file(RFC_BL_FILE,(byte *)RFC_FILE_PATH, (int32)strlen(RFC_FILE_PATH));
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Writes RFNV item
 
  @details
  This function sets a specific RFNV item to NV or EFS. If item_code is
  greater than RFNV_USE_EFS_NV_CUTOFF then item is written to EFS, otherwise
  the old NV storage is used.
 
  - Check if item id is belowe cutoff and if it is writes NV item to old NV.
  - Creates path to EFS item by appending RFNV_FILE_PATH and item_code.
    File names are right-padded with zeros if needed to make the lenght to be 8.
  - Writes item to EFS and returns write status

 
  @param item_code          : Item to set
  @param item_ptr           : Pointer to the item
  @param item_size          : Size of RFNV item in bytes
  @param task_ptr           : Calling task's TCB pointer
  @param task_wait_sig      : Task signal to wait for when reading NV.
  @param task_wait_handler  : Task's wait function to be called when reading NV
 
  @retval
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
    NV_BADPARM_S    - invalid parameter
*/
nv_stat_enum_type rfcommon_rfnv_set_item
(
  rfnv_item_id_enum_type  item_code,       /* Item to set */
  rfnv_item_type          *item_ptr,       /* Pointer to the item */
  rfnv_item_size_type     item_size,       /* Size of item in bytes */  
  rex_tcb_type            *task_ptr,       /* Calling task's TCB pointer */
  rex_sigs_type           task_wait_sig,   /* Task signal to wait for when reading NV */
  void                    (*task_wait_handler)( rex_sigs_type ) /* Task's wait function to be called when reading NV */
)
{

  if( (int)item_code < RFNV_USE_EFS_NV_CUTOFF )
  {
    return rfcommon_nv_set_item((nv_items_enum_type)item_code, (nv_item_type*)item_ptr, task_ptr, task_wait_sig, task_wait_handler);      
  }
  else
  {
    char path[RFNV_MAX_PATH] = "";
      
    if (RFNV_DATA_SIZE_MAX < item_size)
    {
      return NV_FAIL_S;
    }
  
    (void) snprintf(path, RFNV_MAX_PATH, "%s%08d", RFNV_FILE_PATH, item_code); 
    path[RFNV_MAX_PATH-1] = 0;
      
    if (-1 == efs_put (path, item_ptr, item_size, O_CREAT | O_AUTODIR, 0777))
    {
      return NV_FAIL_S;
    }

    return NV_DONE_S;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reads RFNV item

  @details
  This function performs a Single RFNV Read operation to retrieve the requested 
  item. If item_code is greater than RFNV_USE_EFS_NV_CUTOFF then item is read from EFS,
  otherwise the old NV storage is used.
 
  - Creates path to EFS item by appending RFNV_FILE_PATH and item_code.
    File names are right-padded with zeros if needed to make the lenght to be 8.
  - Reads at most RFNV_DATA_SIZE_MAX bytes to item_ptr
  - Return the status of RFNV read.

  @param item_code
  Item to get

  @param item_ptr
  Pointer where to put the item

  @param item_size
  An in/out parameter specifying the maximum size in bytes of the buffer to
  load the NV item into as the input, and then the actual size of the item
  in bytes after the item has been read.

  @param task_ptr
  Calling task's TCB pointer

  @param task_wait_sig
  Task signal to wait for when reading NV.

  @param task_wait_handler
  Task's wait function to be called when reading NV 

  @retval
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
    NV_FAIL_S       - failed to read existing item from EFS
    NV_BADPARM_S    - invalid parameter

*/
nv_stat_enum_type
rfcommon_rfnv_get_item
(
  rfnv_item_id_enum_type item_code,
  rfnv_item_type         *item_ptr,
  rfnv_item_size_type    *item_size,       
  rex_tcb_type           *task_ptr,
  rex_sigs_type          task_wait_sig,
  void                   (*task_wait_handler)( rex_sigs_type )
)
{
   /*Bypass EFS read; will be replaced by RAMDISK Solution*/
   #ifdef T_RUMI_EMULATION

      boolean rfrumi_nv_get_success = FALSE;
      rfrumi_nv_get_success = rfrumi_nv_get_item(item_code, (nv_item_type*)item_ptr);
      if (rfrumi_nv_get_success)
      {
         MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "RF NV item %d found in rumi nv list", item_code);
         return(NV_DONE_S);
      }
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF NV item %d is not found in rumi nv list", item_code);
      return(NV_BADTG_S);
   #endif

  if( (int)item_code < RFNV_USE_EFS_NV_CUTOFF )
  {
    if ( *item_size >= 128 )
    {
      nv_stat_enum_type status;
      status = rfcommon_nv_get_item((nv_items_enum_type)item_code, (nv_item_type*)item_ptr, task_ptr, task_wait_sig, task_wait_handler);
      *item_size = 128;
      return status;
    }
    else
    {
      *item_size = 0;
      return NV_FAIL_S;
    }
  } /* if( item_code < RFNV_USE_EFS_NV_CUTOFF ) */
  else
  {
    char path[RFNV_MAX_PATH];
    int data_size = -1;
    struct fs_stat temp_buf;

    (void) snprintf(path, RFNV_MAX_PATH, "%s%08d", RFNV_FILE_PATH, item_code);
    path[RFNV_MAX_PATH-1] = 0;

    if(efs_stat(path,&temp_buf) == -1) 
    {
      *item_size = 0;
      return NV_NOTACTIVE_S;
    }

    if (*item_size < temp_buf.st_size)
    {
      *item_size = 0;
      return NV_FAIL_S;
    }

    data_size = efs_get(path, item_ptr, temp_buf.st_size);

    if (-1 == data_size)
    {
      *item_size = 0;
      return NV_FAIL_S;
    }

    *item_size = (rfnv_item_size_type)data_size;
    return NV_DONE_S;
  } /* else if( item_code < RFNV_USE_EFS_NV_CUTOFF ) */
} /* rfcommon_rfnv_get_item() */



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reads all RFNV items for entries listed in rfnv_info_table.
  See rfcommon_rfnv_info_type and rfcommon_rfnv_items_list_type.
 
  - Iterates through the rfnv_info_table array.
  - Checks if current rfnv_info_table array item is valid, only valid items are processed.
  - Gets pointer to the RFNV item table using callback function.
  - Get the size of RFNV item table using callback function.
  - Tries to read RFNV item using rfcommon_rfnv_get_item.
  - If read was success then copy callback function is called.
  - If read failed and RFNV item was flagged with NV_ITEM_ATTRIB_CRTCL_BMSK
    then current rfnv_info_table entry declared as invalid. 

  @details

  @param rfnv_info_table      : Array of rfnv items to retrieve
  @param rfnv_info_table_size  : Size of nv_info_table
  @param task_ptr              : Calling task's TCB pointer
  @param task_wait_sig        : Task signal to wait for when reading NV.
  @param task_wait_handler    : Task's wait function to be called when reading NV 

  @retval
    RFCOMMON_NV_READ_SUCCESS   - request done
    RFCOMMON_NV_INACTIVE_ITEM  - item was not active
    RFCOMMON_NV_READ_FAILED    - read faild

*/
rfcommon_nv_status_type rfcommon_rfnv_retrieve_items(rfcommon_rfnv_info_type *rfnv_info_table, 
                           int16 rfnv_info_table_size, 
                           rex_tcb_type *task_ptr, 
                           rex_sigs_type task_wait_sig,
                           void (*task_wait_handler)( rex_sigs_type ))

{
   rfnv_item_type* rfnv_item_buf = NULL; /* RFNV item buffer */
   nv_stat_enum_type nv_read_status; /* RFNV read status */
   int16 rfnv_item_count; /* loop counter for RFNV item fetches */

   /* Pointer to the list of RFNV items */
   const rfcommon_rfnv_items_list_type *rfnv_item_list;  
   uint16 rfnv_item_list_siz; /* Number of items in RFNV items list */
   uint16 table_index; /* index to each mode */
   rfcommon_nv_status_type status = RFCOMMON_NV_READ_SUCCESS;

   // Allocate memory from modem heap for RFNV items
   rfnv_item_buf = modem_mem_alloc(sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA);
   if(NULL == rfnv_item_buf)
   {
     return RFCOMMON_NV_READ_FAILED;
   }

   for (table_index = 0; table_index < rfnv_info_table_size; table_index++) 
   {
     if (rfnv_info_table[table_index].rfnv_data_valid_flag == TRUE)
     {
       /* get the pointer to NV items list for each mode */
       rfnv_item_list =  rfnv_info_table[table_index].rfnv_item_list_ptr;

       /* get the list size */
       if( rfnv_info_table[table_index].get_rfnv_item_list_size != NULL)
         rfnv_item_list_siz = (rfnv_info_table[table_index].get_rfnv_item_list_size)();
       else
         rfnv_item_list_siz = 0;

       /* for each NV item in the list, read from NV and copy into local buffer. */
       /* then copy from local buffer into RF NV table for each mode */
       for (rfnv_item_count = 0; rfnv_item_count < rfnv_item_list_siz; rfnv_item_count++)
       {
         rfnv_item_size_type item_size = sizeof(rfnv_item_type);
         /* read from NV into local buffer */
         nv_read_status = rfcommon_rfnv_get_item(rfnv_item_list[rfnv_item_count].rfnv_item_id, 
                                               rfnv_item_buf,
                                               &item_size,
                                               task_ptr,
                                               task_wait_sig,
                                               task_wait_handler );

         /* if read status is good */
         if (nv_read_status == NV_DONE_S)
         {
            /* copy from local buffer into RF NV table */        
           rfnv_info_table[table_index].copy_rfnv_item( rfnv_item_list[rfnv_item_count].rfnv_item_id, rfnv_item_buf ); 
         }
         else if (nv_read_status == NV_NOTACTIVE_S)
         {
            MSG_3(MSG_SSID_RF, MSG_LEGACY_LOW, "RF NV item %d for mode %d is not available, attr=0x%x",
                  rfnv_item_list[rfnv_item_count].rfnv_item_id, table_index, 
                  rfnv_item_list[rfnv_item_count].rfnv_item_attrib );

            if (rfnv_item_list[rfnv_item_count].rfnv_item_attrib & NV_ITEM_ATTRIB_CRTCL_BMSK)
            {
               rfnv_info_table[table_index].rfnv_data_valid_flag = FALSE;
               status = RFCOMMON_NV_INACTIVE_ITEM;
               MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF NV item %d for mode %d is not available, attr=0x%x",
                     rfnv_item_list[rfnv_item_count].rfnv_item_id, table_index, 
                     rfnv_item_list[rfnv_item_count].rfnv_item_attrib );

            }
         }
         else
         {
           MSG_3(MSG_SSID_RF, MSG_LEGACY_LOW, "NV read failed on RF NV item %d for mode %d, , attr=0x%x", 
                 rfnv_item_list[rfnv_item_count].rfnv_item_id, table_index, 
                 rfnv_item_list[rfnv_item_count].rfnv_item_attrib );
            
            if (rfnv_item_list[rfnv_item_count].rfnv_item_attrib & NV_ITEM_ATTRIB_CRTCL_BMSK)
            {
               rfnv_info_table[table_index].rfnv_data_valid_flag = FALSE;
               status = RFCOMMON_NV_READ_FAILED;
               MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, "NV read failed on RF NV item %d for mode %d, , attr=0x%x", 
                     rfnv_item_list[rfnv_item_count].rfnv_item_id, table_index, 
                     rfnv_item_list[rfnv_item_count].rfnv_item_attrib );

            }
         }
	    /* Re-Initialize Buffer in case there was any data that was copied */
	    if(item_size > 0 && item_size <= sizeof(rfnv_item_type))
	    {
	        memset((void*)rfnv_item_buf,0,item_size);
	    }
       } /*for (rfnv_item_count = 0; rfnv_item_count < rfnv_item_list_siz; rfnv_item_count++)*/
     } /* rfnv_is_nv_data_valid */
   } /*for (mode_idx = 0; mode_idx < RFNV_SUPPORTED_MODE_NUM; mode_idx++)*/

   // Free RFNV buffer
   modem_mem_free(rfnv_item_buf, MODEM_MEM_CLIENT_RFA);
   rfnv_item_buf = NULL;

   return status;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function is used to load the CL Control info from the CL NV table
  based on the version

  @details

  @param rfnv_item_ptr
  Pointer to the NV buffer where data loaded from NV currently is stored

  @ant_tuner_cl_ctrl_info 
  Pointer to which the CL Control info  will be copied
  
  @return
  boolean TRUE or FALSE 
  TRUE - Memory Allocation & Loading NV table -> success
  FALSE -Failure in Memory Allocation  or loading 
*/

boolean rfcommon_load_ant_tuner_cl_ctrl_info_from_nv( rfnv_item_type *rfnv_item_ptr,
                                                               rfcommon_nv_ant_tuner_cl_ctrl_type *ant_tuner_cl_ctrl_info)
{
  boolean status = TRUE;

  /*Sanity check */
  if(rfnv_item_ptr == NULL || ant_tuner_cl_ctrl_info == NULL )
  {
    return FALSE;
  }
  
  /*Fill the version */
  ant_tuner_cl_ctrl_info->version = ((rfcommon_nv_ant_tuner_cl_ctrl_type *)rfnv_item_ptr)->version;

  /*Allocate the memory based on the version*/
  switch(ant_tuner_cl_ctrl_info->version)
  {
    case RFCOMMON_NV_ATUNER_CL_CTRL_INFO_V_A_ID:
         /* Allocate the memory only once */
        if (ant_tuner_cl_ctrl_info->payload == NULL )
        {
          ant_tuner_cl_ctrl_info->payload = modem_mem_alloc(sizeof(rfcommon_nv_ant_tuner_cl_ctrl_data_type_v_a), 
        	                                             MODEM_MEM_CLIENT_RFA);
        }
        if(ant_tuner_cl_ctrl_info->payload != NULL )
        {
           /*Copy the data  after the version.*/
          memscpy((void*)ant_tuner_cl_ctrl_info->payload,
                  sizeof(rfcommon_nv_ant_tuner_cl_ctrl_data_type_v_a),
           	      (void*)&(((rfcommon_nv_ant_tuner_cl_ctrl_type *)rfnv_item_ptr)->payload),
           	      sizeof(rfcommon_nv_ant_tuner_cl_ctrl_data_type_v_a));    
      	}
        break;
    default:
         MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfcommon_load_ant_tuner_cl_ctrl_info_from_nv() Invalid version %d",
                                             ant_tuner_cl_ctrl_info->version);
	 break;
  }
  return status; 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum list for NVIDs of third party tuner per tech per band 
  To be updated by customer after defining appropriate NV item code
  */

rfnv_item_id_enum_type rfcommon_nv_wcdma_tuner_id_list[RFCOM_NUM_WCDMA_BANDS] =
{
   RFNV_ITEMS_ENUM_MAX,      // RFCOM_BAND_IMT                          
   RFNV_ITEMS_ENUM_MAX,      // RFCOM_BAND_1900, 
   RFNV_ITEMS_ENUM_MAX,      // RFCOM_BAND_BC3,  
   RFNV_ITEMS_ENUM_MAX,      // RFCOM_BAND_BC4,  
   RFNV_ITEMS_ENUM_MAX,      // RFCOM_BAND_800,  
   RFNV_ITEMS_ENUM_MAX,      // RFCOM_BAND_BC8,  
   RFNV_ITEMS_ENUM_MAX,      // RFCOM_BAND_BC9,  
   RFNV_ITEMS_ENUM_MAX,      // RFCOM_BAND_BC11, 
   RFNV_ITEMS_ENUM_MAX       // RFCOM_BAND_BC19, 
};

rfnv_item_id_enum_type rfcommon_nv_cdma_tuner_id_list[RFM_CDMA_MAX_BAND] =
{
   RFNV_ITEMS_ENUM_MAX,    // All band NVID should be updated as needed  
   RFNV_ITEMS_ENUM_MAX,     
   RFNV_ITEMS_ENUM_MAX,     
   RFNV_ITEMS_ENUM_MAX,
   RFNV_ITEMS_ENUM_MAX,
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX     
};


rfnv_item_id_enum_type rfcommon_nv_lte_tuner_id_list[RFCOM_NUM_LTE_BANDS] =
{
   RFNV_ITEMS_ENUM_MAX,   // All band NVID should be updated as needed  
   RFNV_ITEMS_ENUM_MAX,     
   RFNV_ITEMS_ENUM_MAX,     
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX,    
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX,    
   RFNV_ITEMS_ENUM_MAX,
   RFNV_ITEMS_ENUM_MAX,
   RFNV_ITEMS_ENUM_MAX,
   RFNV_ITEMS_ENUM_MAX,
   RFNV_ITEMS_ENUM_MAX,
   RFNV_ITEMS_ENUM_MAX,   
   RFNV_ITEMS_ENUM_MAX,
   RFNV_ITEMS_ENUM_MAX,
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX, 
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX,  
   RFNV_ITEMS_ENUM_MAX   
};

rfnv_item_id_enum_type rfcommon_nv_tdscdma_tuner_id_list[RFCOM_NUM_TDSCDMA_BANDS] =
{
   RFNV_ITEMS_ENUM_MAX,   // All band information should be updated as needed  
   RFNV_ITEMS_ENUM_MAX,     
   RFNV_ITEMS_ENUM_MAX,     
   RFNV_ITEMS_ENUM_MAX,     
};

rfnv_item_id_enum_type rfcommon_nv_gsm_tx_tuner_id_list[RFCOM_NUM_GSM_BANDS] =
{
  RFNV_ITEMS_ENUM_MAX,   // All band NVID should be updated as needed 
  RFNV_ITEMS_ENUM_MAX, 
  RFNV_ITEMS_ENUM_MAX, 
  RFNV_ITEMS_ENUM_MAX, 
};

rfnv_item_id_enum_type rfcommon_nv_gsm_rx_tuner_id_list[RFCOM_NUM_GSM_BANDS] =
{
  RFNV_ITEMS_ENUM_MAX,  // All band NVID should be updated as needed 
  RFNV_ITEMS_ENUM_MAX, 
  RFNV_ITEMS_ENUM_MAX, 
  RFNV_ITEMS_ENUM_MAX, 
};

const rfcommon_nv_lte_ca_info rfcommon_nv_lte_ca_data[] =
{
  /*To be filled as per LTE CA requirement*/
  { RFCOM_NUM_LTE_BANDS, RFCOM_NUM_LTE_BANDS, RFNV_ITEMS_ENUM_MAX },
  { RFCOM_NUM_LTE_BANDS, RFCOM_NUM_LTE_BANDS, RFNV_ITEMS_ENUM_MAX },
  /*Last element of the array is forced to a default and should be as defined here*/
};

const rfcommon_nv_wcdma_dbdc_info rfcommon_nv_wcdma_dbdc_data[] =
{
  /*To be filled as per wcdma DBDC requirement*/
  { RFCOM_NUM_WCDMA_BANDS, RFCOM_NUM_WCDMA_BANDS, RFNV_ITEMS_ENUM_MAX },
  { RFCOM_NUM_WCDMA_BANDS, RFCOM_NUM_WCDMA_BANDS, RFNV_ITEMS_ENUM_MAX },
  /*Last element of the array is forced to a default and should be as defined here*/
};
uint16 ca_nv_struc_size = sizeof(rfcommon_nv_lte_ca_data)
                          /sizeof(rfcommon_nv_lte_ca_info); 
                          /* Find size of the nv structure array this is added to 
                            avoid use of macro for size that can lead to error 
                            The variable is global to avoid recomputation if used 
                            in function.*/

uint16 dbdc_nv_struc_size = sizeof(rfcommon_nv_wcdma_dbdc_data)
                          /sizeof(rfcommon_nv_wcdma_dbdc_info); 
                          /* Find size of the nv structure array this is added to 
                            avoid use of macro for size that can lead to error 
                            The variable is global to avoid recomputation if used 
                            in function.*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for tuner NV item code retrieve based on band, tech and tx/rx
  API returns true or false and updates the item code.
  For now only no band information is updated for wcdma as it
  should be completed after addition of NV items.

  @details
  -The code returns the item code based on tech band and tx rx
  -The customer should take care of populating the NV item code.
  -The API returns fail if item code read is invalid.
 
  @param mode
  tech information
 
  @param band
  band of each tech
 
  @param is_rx
  is this tx or rx
  Needed for GSM case of tune code
 
  @param item_code
  pointer to return item code information
 
  @return
  True or False

*/

boolean rfcommon_nv_get_tuner_item_code
( 
   rfcom_mode_enum_type mode,
   rfcom_band_type_u band,
   boolean is_this_rx_operation,
   rfnv_item_id_enum_type *item_code  
)
{
   boolean status = FALSE;

   if (item_code == NULL)
   {
      return FALSE;
   }

   *item_code = RFNV_ITEMS_ENUM_MAX;

   switch (mode)
   {
   case RFM_IMT_MODE:
     if (band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
     {
        *item_code = rfcommon_nv_wcdma_tuner_id_list[band.wcdma_band];
     }
     break;

   case RFM_EGSM_MODE:
     if (band.gsm_band < RFCOM_NUM_GSM_BANDS)
     {
        if (TRUE == is_this_rx_operation)
        {
           *item_code = rfcommon_nv_gsm_rx_tuner_id_list[band.gsm_band];
        }

        else
        {
           *item_code = rfcommon_nv_gsm_tx_tuner_id_list[band.gsm_band];
        }

     }
     break;

   case RFM_1X_MODE:
   case RFM_1XEVDO_MODE:
   case RFM_SVDO_MODE:
     if (band.cdma_band < RFM_CDMA_MAX_BAND)
     {
        *item_code = rfcommon_nv_cdma_tuner_id_list[band.cdma_band];
     }
     break;

   case RFM_LTE_MODE:
     if (band.lte_band < RFCOM_NUM_LTE_BANDS)
     {
        *item_code = rfcommon_nv_lte_tuner_id_list[band.lte_band];
     }
     break;

   case RFM_TDSCDMA_MODE:
     if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
     {
        *item_code = rfcommon_nv_tdscdma_tuner_id_list[band.tdscdma_band];
     }
     break;

   default:
     break;
   }

   if ( *item_code>0 && *item_code<=RFNV_USE_EFS_NV_CUTOFF )
   {
     *item_code = RFNV_ITEMS_ENUM_MAX;
     status = FALSE;
   }

   /*Condition to check if the item code is not within the NV EFS range
     This should be modified as per range of tuner NV items of custormer*/
   if ( (*item_code > RFNV_USE_EFS_NV_CUTOFF) && (*item_code < RFNV_ITEMS_ENUM_MAX) )
   {
      status = TRUE;
   }
   return (status); 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for tuner NV item code retrieve based on CA bands of LTE
  API returns true or false and updates the item code.

  @details
  -The code returns the item code based on CA combination defined in the CA item
   structure. 
  -The customer should take care of populating the NV item code and its bands in
   rfcommon_nv_lte_ca_data structure.
  -The API returns fail if item code read is invalid.
  -In regards to the structure rfcommon_nv_lte_ca_data[] that stores band combo
   and associated NV, care should be taken to have last element of this array
   as { RFCOM_NUM_LTE_BANDS, RFCOM_NUM_LTE_BANDS, RFNV_ITEMS_ENUM_MAX }
   The while loop in the code looks for this value to terminate the search of
   band combo.
 
  @param band_1
  One of the LTE CA band
 
  @param band_2
  Second LTE CA band
 
  @param item_code
  pointer to return item code information
 
  @return
  True or False

*/
boolean rfcommon_nv_get_tuner_ca_item_code
( 
   rfcom_lte_band_type band_1,
   rfcom_lte_band_type band_2,
   rfnv_item_id_enum_type *item_code  
)
{
   boolean status = FALSE;
   uint8 index = 0;

   if (item_code == NULL)
   {
      return FALSE;
   }

   if (band_1 >= RFCOM_NUM_LTE_BANDS || band_2 >= RFCOM_NUM_LTE_BANDS)/*Validate band information*/
   {
      return FALSE;
   }

   *item_code = RFNV_ITEMS_ENUM_MAX;  /*Re-init the value for case where garbage is read by pointer*/

   for (index = 0; index < ca_nv_struc_size; index++ )
   {
      /*Check for the last element pattern*/
      if (  (rfcommon_nv_lte_ca_data[index].band_x == RFCOM_NUM_LTE_BANDS)
          &&(rfcommon_nv_lte_ca_data[index].band_y == RFCOM_NUM_LTE_BANDS)
          &&(rfcommon_nv_lte_ca_data[index].ca_item_code == RFNV_ITEMS_ENUM_MAX)) 
      {
         break;
      }

      if (rfcommon_nv_lte_ca_data[index].band_x == band_1 
          && rfcommon_nv_lte_ca_data[index].band_y == band_2)
      {
         *item_code = rfcommon_nv_lte_ca_data[index].ca_item_code;
         break;
      }

   }/*End of for*/

   /*To handle cases where the NV entry is wrong*/
   if ( *item_code>0 && *item_code<=RFNV_USE_EFS_NV_CUTOFF ) 
   { 
     *item_code = RFNV_ITEMS_ENUM_MAX;
     status = FALSE;
   }

   /*Condition to check if the item code is not within the NV EFS range 
     This should be modified as per range of tuner NV items of custormer*/
   if ( (*item_code > RFNV_USE_EFS_NV_CUTOFF) && (*item_code < RFNV_ITEMS_ENUM_MAX) )
   {
      status = TRUE;
   }

   return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API for tuner NV item code retrieve based on DBDC bands of WCDMA
  API returns true or false and updates the item code.

  @details
  -The code returns the item code based on DBDC combination defined in the 
   structure. 
  -The customer should take care of populating the NV item code and its bands in
   rfcommon_nv_wcdma_DBDC_data structure.
  -The API returns fail if item code read is invalid.
  -In regards to the structure rfcommon_nv_wcdma_dbdc_data[] that stores band combo
   and associated NV, care should be taken to have last element of this array
   as { RFCOM_NUM_WCDMA_BANDS, RFCOM_NUM_WCDMA_BANDS, RFNV_ITEMS_ENUM_MAX }
   The while loop in the code looks for this value to terminate the search of
   band combo.
 
  @param band_1
  One of the WCDMA DBDC band
 
  @param band_2
  Second WCDMA DBDC band
 
  @param item_code
  pointer to return item code information
 
  @return
  True or False

*/
boolean rfcommon_nv_get_tuner_dbdc_item_code
( 
   rfcom_wcdma_band_type band_1,
   rfcom_wcdma_band_type band_2,
   rfnv_item_id_enum_type *item_code  
)
{
   boolean status = FALSE;
   uint8 index = 0;

   if (item_code == NULL)
   {
      return FALSE;
   }

   if (band_1 >= RFCOM_NUM_WCDMA_BANDS || band_2 >= RFCOM_NUM_WCDMA_BANDS)/*Validate band information*/
   {
      return FALSE;
   }

   *item_code = RFNV_ITEMS_ENUM_MAX;  /*Re-init the value for case where garbage is read by pointer*/

   for (index = 0; index < dbdc_nv_struc_size; index++ )
   {
      /*Check for the last element pattern*/
      if (  (rfcommon_nv_wcdma_dbdc_data[index].band_1 == RFCOM_NUM_WCDMA_BANDS)
          &&(rfcommon_nv_wcdma_dbdc_data[index].band_2 == RFCOM_NUM_WCDMA_BANDS)
          &&(rfcommon_nv_wcdma_dbdc_data[index].dbdc_item_code == RFNV_ITEMS_ENUM_MAX)) 
      {
         break;
      }

      if (rfcommon_nv_wcdma_dbdc_data[index].band_1 == band_1 
          && rfcommon_nv_wcdma_dbdc_data[index].band_2 == band_2)
      {
         *item_code = rfcommon_nv_wcdma_dbdc_data[index].dbdc_item_code;
         break;
      }

   }/*End of for*/

   /*To handle cases where the NV entry is wrong*/
   if ( *item_code>0 && *item_code<=RFNV_USE_EFS_NV_CUTOFF ) 
   { 
     *item_code = RFNV_ITEMS_ENUM_MAX;
     status = FALSE;
   }

   /*Condition to check if the item code is not within the NV EFS range 
     This should be modified as per range of tuner NV items of custormer*/
   if ( (*item_code > RFNV_USE_EFS_NV_CUTOFF) && (*item_code < RFNV_ITEMS_ENUM_MAX) )
   {
      status = TRUE;
   }

   return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reads RFNV PA Static strucutre item

  @details
  API retries giant PA static NV item based on the MODE passed.

  @param rfcom_mode_enum_type mode
  RFCOM_1X_MODE : Returns the entire structure (rfcommon_nv_pa_static_data_type)
  For other 3G and 4G tech 
  ie., RFCOM_WCDMA_MODE, RFCOM_TDSCDMA_MODE, RFCOM_LTE_MODE
  Returns the common structure only (rfcommon_nv_cmn_pa_static_type)
  
  @param void *pa_static_nv_data : Pointer where to put the item 
  @param rfnv_item_id_enum_type pa_static_nv_id : NV Item ID to get
  @param task_ptr  : Calling task's TCB pointer
  @param task_wait_sig : Task signal to wait for when reading NV.
  @param task_wait_handler : Task's wait function to be called when reading NV 

  @retval 
    TRUE - API success
    FALSE - API Failure
*/


boolean 
rfcommon_load_pa_static_nv
(
  rfcom_mode_enum_type mode,
  void *pa_static_nv_data,
  rfnv_item_id_enum_type pa_static_nv_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean status = TRUE;
  nv_stat_enum_type  nv_read_status = NV_DONE_S;

  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;
  /* Pointer to NV decompressed data, Note using the same data type for decompression data. */
  rfnv_item_type *nv_dc_data_ptr = NULL;

  rfnv_item_size_type nv_read_size = sizeof(rfnv_item_type);
  uint32 uncompress_size = sizeof(rfnv_item_type);
  uint32 nv_element_size;
  /* NV decompressino status */
  int8 uncompress_status = Z_ERRNO;

  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_alloc( sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );

  RF_MSG_2(MSG_LEGACY_LOW, "Starting rfcommon_load_pa_static_nv: Mode:%d | NV_ID:%d",mode,pa_static_nv_id);
  
  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG_2(MSG_LEGACY_ERROR, "rfcommon_load_pa_static_nv %d: Failed to allocate memory for %d bytes.", pa_static_nv_id, nv_read_size);
    status = FALSE;
  }
  else
  {
    /* default to all 0's */
    memset( nv_data_ptr, 0, sizeof(rfnv_item_type));

    nv_read_status = rfcommon_rfnv_get_item( pa_static_nv_id,
                                             nv_data_ptr,
                                             &nv_read_size,
                                             caller_tcb_ptr,
                                             task_nv_wait_sig,
                                             task_wait_func_ptr );
    if( nv_read_status == NV_DONE_S )
    {
      RF_MSG_2(MSG_LEGACY_HIGH, "rfcommon_load_pa_static_nv id#: %d NV read successful, size = %d", pa_static_nv_id, nv_read_size);
      status = TRUE;
      // check for NV Id is compressed or not
      if (rfcommon_rfnv_chk_item_compressed(pa_static_nv_id))
      {
        /* if it is compressed data, need to uncompressed data */
        /* Allocate memory for NV data decompression. Allocate memory with nv_item_type for PA static data, and using PA static data type (smaller) causes a problem */
        nv_dc_data_ptr = (rfnv_item_type *)modem_mem_alloc(sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA);

        if (nv_dc_data_ptr != NULL)
        {
          /* Decompress NV data */
          uncompress_status = uncompress((uint8 *)nv_dc_data_ptr,   
                                       &uncompress_size,
                                       (const uint8 *)nv_data_ptr, 
                                       (uint32)nv_read_size);

          if (uncompress_status != Z_OK)
          {
            RF_MSG_2( RF_ERROR, "decompression of PA static NV %d failed!status:%d", pa_static_nv_id, uncompress_status ); 
            /* Free the memory for decompressed NV data */
            modem_mem_free((void*)nv_dc_data_ptr, MODEM_MEM_CLIENT_RFA);
            status = FALSE;  
          }
          else
          {
            RF_MSG_2( MSG_LEGACY_HIGH, "rfcommon_load_pa_static_nv id#:%d NV uncompressed successful, size = %d", pa_static_nv_id, uncompress_size );
            /* Free the memory for raw NV data*/
            modem_mem_free((void*)nv_data_ptr, MODEM_MEM_CLIENT_RFA);

            /* Reuse the nv_data_ptr for the uncompress nv data */
            nv_data_ptr = nv_dc_data_ptr;
            status = TRUE;
          }
      } /* nv_dc_data_ptr */
      else
      {
          RF_MSG_1( RF_ERROR, "Fail to allocate memory for decompress PA static NV %d!", pa_static_nv_id); 
          status = FALSE;
        } /* nv_dc_data_ptr - else */
      } /* compressed NV */
    } /* nv_read_status */
    else
    {
      RF_MSG_1(MSG_LEGACY_ERROR, "rfcommon_load_pa_static_nv id#: %d NV Read Failed", pa_static_nv_id);
      status = FALSE;
    }

    if( status==TRUE )
    {
      switch(mode)
      {
        case RFCOM_1X_MODE:
        case RFCOM_1XEVDO_MODE:
          nv_element_size = sizeof(rfcommon_nv_pa_static_data_type);
          memscpy((void *)pa_static_nv_data, nv_element_size, (void*)nv_data_ptr, nv_element_size );
          status = TRUE;
          break;

        case RFCOM_WCDMA_MODE:
        case RFCOM_TDSCDMA_MODE:
        case RFCOM_LTE_MODE:
        case RFM_LTE_FTM_MODE:
          nv_element_size = sizeof(rfcommon_nv_cmn_pa_static_type);
          memscpy((void *)pa_static_nv_data, nv_element_size, (void*)nv_data_ptr, nv_element_size );        
          break;

        default:
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfcommon_load_pa_static_nv: Unsupported Mode", mode);
          status = FALSE;
          break;
      }
    }
  } /* else nv_data_ptr */

  /* Cleanup */
  if( nv_data_ptr != NULL )
  {
    modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  }
  return status;
}

/*--------------------------------------------------------------------------*/
/* rflte_nv_populate_rx_gain_vs_freq_v2_nv                                  */
/*--------------------------------------------------------------------------*/
/*!
  @brief
  This API populates the RX gain vs freq V2 NV

  @details
  This API populates the RX gain vs freq V2 NV

  @param nv_tbl_type_ptr
  Pointer to the band-specific NV table
 
  @param rfnv_item_ptr
  Pointer to the NV

*/
boolean
rfcommon_populate_rx_gain_vs_freq_v2_nv
(
  rfnv_rx_cal_data_type        *rx_cal_data_ptr,
  void                         *rfnv_item_ptr,
  uint8                        num_gain_states
)
{
  /*---------------------------------------------------------------*/
  /*                        LOCAL VARIABLES                        */
  /*---------------------------------------------------------------*/
  /* NV Data related Variables */
  uint32 bytes_parsed = 0,  total_elements = 0,element_index = 0;
  uint32 uncompress_size = 0, total_nv_size = 0, path_cal_data_size = 0,uncompress_size_allocated =0;
  rfnv_data_variant_marker_type *variant_marker_ptr = NULL;
  rfnv_rx_cal_data_typeid_118_type *rx_cal_data_typeid_118_ptr = NULL;
  uint8 *current_pos_ptr = NULL, *uncompressed_nv_data_ptr = NULL;
  uint16 type_id = 0;
  int8 uncompress_status = Z_ERRNO;
  boolean success = FALSE ;
  uint32 num_elements_typeid118 = 0;
  uint8 *init_pass_to_get_num = NULL;
  uint32 typeid118_total_nv_size = 0;
  uint32 typeid118_path_cal_data_size = 0;
  uint32 typeid118_element_index = 0;

  /*---------------------------------------------------------------*/
  /*                    END OF LOCAL VARIABLES                     */
  /*---------------------------------------------------------------*/
  
  /*---------------------------------------------------------------*/
  /*                         VALIDATION                            */
  /*---------------------------------------------------------------*/
  if ( NULL == rx_cal_data_ptr )
  {
    MSG ( MSG_SSID_RF, 
          MSG_LEGACY_ERROR, 
             "rfcommon_populate_rx_gain_vs_freq_v2_nv: ERROR: "
             "NV Table Pointer is NULL ! "
             "Skiping NV Load "  );
    return success ;	     
  }

  if ( NULL == rfnv_item_ptr )
  {
    MSG ( MSG_SSID_RF, 
          MSG_LEGACY_ERROR, 
          "rfcommon_populate_rx_gain_vs_freq_v2_nv: ERROR: "
          "rx_gain_vs_freq_v2 data Pointer is NULL ! "
          "Skiping NV Load "  );
    return success ;
  }

  if ((num_gain_states > RFNV_MAX_LNA_GAIN_STATES) || 
      (num_gain_states == 0))
  {
    MSG_2 ( MSG_SSID_RF, 
            MSG_LEGACY_ERROR, 
            "rfcommon_populate_rx_gain_vs_freq_v2_nv: ERROR: "
            "num_gain_state %d exceeds MAX %d allowed, or no gain states "
            "Skiping NV Load ",
            num_gain_states, 
            RFNV_MAX_LNA_GAIN_STATES);
    return success ;
  }

  /*---------------------------------------------------------------*/
  /*                      END OF VALIDATION                        */
  /*---------------------------------------------------------------*/

  /*---------------------------------------------------------------*/
  /*                      Decompress the NV                        */
  /*---------------------------------------------------------------*/
  /* Allocate worst-case memory size for NV data decompression. */
  uncompress_size = sizeof(rfnv_data_variant_marker_type);
  uncompress_size += (sizeof(rfnv_rx_cal_data_typeid_118_type) * RFCOMMON_NV_RX_GAIN_V2_NV_MAX_CONTAINERS);
  uncompressed_nv_data_ptr = modem_mem_alloc(uncompress_size, MODEM_MEM_CLIENT_RFA);
  uncompress_size_allocated = uncompress_size;
  
  if (uncompressed_nv_data_ptr == NULL)
  {
     MSG_1 ( MSG_SSID_RF, 
             MSG_LEGACY_ERROR,
             "RX CAL Data ERROR - failed to allocate memory for %d bytes.", 
             uncompress_size );
    return success ;
  }
  /* Decompress NV data */
  uncompress_status = uncompress( (uint8 *)uncompressed_nv_data_ptr,   
                                  &uncompress_size,
                                  (const uint8 *)rfnv_item_ptr, 
                                  sizeof(rfnv_item_type) );
  
  if (uncompress_status != Z_OK)
  {
    MSG ( MSG_SSID_RF, 
          MSG_LEGACY_ERROR,
          "RX CAL Data ERROR - decompression of NV ID failed.");
    modem_mem_free(uncompressed_nv_data_ptr, MODEM_MEM_CLIENT_RFA);
    return success ;
  }

  /*---------------------------------------------------------------*/
  /*                      END OF Decompression                     */
  /*---------------------------------------------------------------*/
  

  /*---------------------------------------------------------------*/
  /*                        NV Population                          */
  /*---------------------------------------------------------------*/
  /* ----- Extract the variant_marker to check how items to parse ----- */
  variant_marker_ptr = ( rfnv_data_variant_marker_type * )uncompressed_nv_data_ptr;
  total_elements = variant_marker_ptr->num_elements;
  bytes_parsed = sizeof(rfnv_data_variant_marker_type);
  current_pos_ptr = ((uint8 *)uncompressed_nv_data_ptr) + bytes_parsed;
  memscpy(&(rx_cal_data_ptr->variant_marker),
          sizeof(rfnv_data_variant_marker_type),
          uncompressed_nv_data_ptr,
          sizeof(rfnv_data_variant_marker_type));

  /* First element must be cal_data_info */
  type_id = *((uint16 *)current_pos_ptr);

   /* copy over the current position to temp location for initial 
     pass to gather the number of each individual type of nv */
  init_pass_to_get_num = current_pos_ptr;

   for ( element_index = 0 ;
        element_index < total_elements 
        ; element_index++)
  {
    type_id = *((uint16 *)init_pass_to_get_num);
    init_pass_to_get_num += sizeof(uint16);
  switch (type_id)
  {
    case RFNV_DATA_TYPE_RX_CAL_OFFSET_V2:
        init_pass_to_get_num += sizeof(rfnv_rx_cal_data_typeid_118_type);
        num_elements_typeid118++;
      break;

      default:
        return success;
      break;
    }
  }  

  rx_cal_data_ptr->num_of_cal_118_elements = num_elements_typeid118;

  typeid118_total_nv_size = ( (sizeof(rfnv_rx_cal_data_typeid_118_type)*num_elements_typeid118) - 
                        (sizeof(int16)*RFNV_NUMBER_OF_PCLS*(RFNV_MAX_LNA_GAIN_STATES-num_gain_states)) );
  
  typeid118_path_cal_data_size = ((sizeof(int16)*RFNV_NUMBER_OF_PCLS*(num_gain_states+1)) + sizeof(uint16)*2);
                         
      if(rx_cal_data_ptr->cal_118_data != NULL)
      {
        modem_mem_free(rx_cal_data_ptr->cal_118_data, 
                       MODEM_MEM_CLIENT_RFA);
      }

  if (num_elements_typeid118 != 0)
  {
	 rx_cal_data_ptr->cal_118_data = modem_mem_alloc(typeid118_total_nv_size,
                                                      MODEM_MEM_CLIENT_RFA);
      if (rx_cal_data_ptr->cal_118_data == NULL)
      {
          MSG ( MSG_SSID_RF, 
                MSG_LEGACY_ERROR, 
                "rfcommon_populate_rx_gain_vs_freq_v2_nv: ERROR: "
                "cannot allocate memory for cal data"
                "Skiping NV Load ");
          modem_mem_free(uncompressed_nv_data_ptr, MODEM_MEM_CLIENT_RFA);
          return success ;
      }
  }

  /* Load the rest of the NV data */
  for ( element_index = 0 , 
        /* Capture success of all loops and if even one element fails return FALSE */
		 success = TRUE,
		 typeid118_element_index = 0 ; 
        element_index < total_elements 
        ; element_index++)
  {
	 type_id = *((uint16 *)current_pos_ptr);
	 bytes_parsed += sizeof(uint16);
        if(bytes_parsed < uncompress_size_allocated)
        {   
           current_pos_ptr = ((uint8 *)uncompressed_nv_data_ptr) + bytes_parsed;
        }
       else
        {
             MSG ( MSG_SSID_RF, 
                MSG_LEGACY_ERROR, 
                "rfcommon_populate_rx_gain_vs_freq_v2_nv: ERROR: "
                "Buffer overflow in uncompressed_nv_data_ptr");
         }

    switch (type_id)
    {
      /* Make sure to update the "success" in all the case
         statements everytime */
      case RFNV_DATA_TYPE_RX_CAL_OFFSET_V2:
        rx_cal_data_typeid_118_ptr = (rfnv_rx_cal_data_typeid_118_type *)current_pos_ptr;
		 memscpy(&(rx_cal_data_ptr->cal_118_data[typeid118_element_index]),
				 typeid118_path_cal_data_size,
                rx_cal_data_typeid_118_ptr,
                sizeof(rfnv_rx_cal_data_typeid_118_type));
        bytes_parsed += sizeof(rfnv_rx_cal_data_typeid_118_type);
	if(bytes_parsed < uncompress_size_allocated)
        {   
            current_pos_ptr = ((uint8 *)uncompressed_nv_data_ptr) + bytes_parsed;
        }
       else
        {
             MSG ( MSG_SSID_RF, 
                MSG_LEGACY_ERROR, 
                "rfcommon_populate_rx_gain_vs_freq_v2_nv: ERROR: "
                "Buffer overflow in uncompressed_nv_data_ptr");
         }
        success &= TRUE ;
		 typeid118_element_index++;
        break;

      default:
        MSG_2 ( MSG_SSID_RF, 
                MSG_LEGACY_ERROR, 
                "rfcommon_populate_rx_gain_vs_freq_v2_nv: ERROR: "
                "unexpected cal info type id %d"
                "Skiping NV Load at index %d", 
                type_id,
                element_index );
        modem_mem_free(rx_cal_data_ptr->cal_118_data, 
                       MODEM_MEM_CLIENT_RFA);

        modem_mem_free(uncompressed_nv_data_ptr, MODEM_MEM_CLIENT_RFA);
        return success ;
    }
  }


  /*---------------------------------------------------------------*/
  /*                    END OF POPULATION                          */
  /*---------------------------------------------------------------*/
  modem_mem_free(uncompressed_nv_data_ptr, MODEM_MEM_CLIENT_RFA);

  return success ;
}/* rfcommon_nv_populate_rx_gain_vs_freq_v2_nv */

/*===========================================================================

FUNCTION RFNV_CHK_NV_ITEM_COMPRESSED

DESCRIPTION
  This function check a specific NV item whether compressed with the following.
  TRUE:    if the input nv_item is compressed
  FALSE:   if the input nv_item is uncompressed

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  Harded checking based on the nv defination enum

===========================================================================*/
boolean rfcommon_rfnv_chk_item_compressed
(
  rfnv_item_id_enum_type  item_code              /* Item to check */
)
{

  boolean is_rfnv_item_compressed = FALSE;

  switch (item_code) 
  {
     case RFNV_LTE_C0_B66_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I:
     case RFNV_LTE_C0_B66_FBRX_DROOP_FIR_FILTER_COEFF_Z_I:
     case RFNV_LTE_C0_B66_PA_STATIC_Z_I:
     case RFNV_LTE_C1_B66_PA_STATIC_Z_I:
	    is_rfnv_item_compressed = TRUE;
            break;

     default:
             is_rfnv_item_compressed = FALSE;
             break;
  }

  if ((((int32) item_code >= (int32) RFNV_CDMA_C0_BC0_PA_STATIC_Z_I) &&
      ((int32) item_code <= (int32) RFNV_WCDMA_B19_FBRX_MODE_VS_GAIN_VS_TEMP_VS_FREQ_Z_I))||
        is_rfnv_item_compressed == TRUE)
  {
     return TRUE;
  }
  else 
  { 
     return FALSE;
  }
}

#define RF_COMMON_FBRX_NOISE_FIXED_SIZE  (sizeof(rfcommon_nv_fbrx_noise_data_type))

boolean
rfcommon_nv_load_fbrx_noise_data_from_nv
(
  rfcommon_nv_fbrx_noise_data_type *fbrx_noise_nv_data,
  rfnv_item_id_enum_type fbrx_noise_data_item,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{
  /* Overall status for this API */
  boolean api_status = TRUE;
  /* NV item size (maximum) */
  uint16 nv_item_size;
  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;
  /* Helper ptr to hold moving ptr location during data copy*/
  uint8     *temp_data_ptr = NULL;
  /* actual size of memory read from the NV*/
  rfnv_item_size_type   fbrx_noise_nv_read_size;
  /* Contains the number of bytes for an element in NV */
  uint32 nv_element_size;

  /* NV read status */
  nv_stat_enum_type  fbrx_noise_nv_read_status = NV_DONE_S;

  nv_item_size = sizeof(rfnv_item_type); 
  fbrx_noise_nv_read_size = sizeof(rfnv_item_type);

  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_alloc( sizeof(rfnv_item_type),
                                                   MODEM_MEM_CLIENT_RFA );
  RF_MSG( RF_HIGH, "Enter rfcommon_nv_load_fbrx_noise_data_from_nv");

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG_1( RF_ERROR, "rfcommon_nv_load_fbrx_noise_data_from_nv - failed to allocate memory for %d bytes.", nv_item_size);
    return FALSE;
  }

  fbrx_noise_nv_read_status = rfcommon_rfnv_get_item( fbrx_noise_data_item,
                                                  nv_data_ptr,
                                                  &fbrx_noise_nv_read_size,
                                                  caller_tcb_ptr,
                                                  task_nv_wait_sig,
                                                  task_wait_func_ptr );

  if ( fbrx_noise_nv_read_status == NV_DONE_S )
  {
    RF_MSG_1( RF_HIGH, "rfcommon_nv_load_fbrx_noise_data_from_nv: NV read successful, size = %d", fbrx_noise_nv_read_size);
  } /* if (fbrx_noise_nv_read_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    RF_MSG_1( RF_ERROR, "rfcommon_nv_load_fbrx_noise_data_from_nv: Initialized the fbrx gain data to default", 0);	
    api_status = FALSE;
    if( nv_data_ptr != NULL )
    {
      modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
    }
    return api_status;
  } /* else: if (fbrx_noise_nv_read_status == NV_DONE_S ) */


  /* If NV data has been successfully retrieved proceed to populating the data structure */
  if (api_status == TRUE) 
  {
    temp_data_ptr = (uint8 *)nv_data_ptr;
    /* copy data for fixed part and variant header of the NV, need to adjust size*/
    nv_element_size = RF_COMMON_FBRX_NOISE_FIXED_SIZE;
    memscpy((void *)fbrx_noise_nv_data, sizeof(*fbrx_noise_nv_data), (void*)temp_data_ptr, nv_element_size );
  }

  /* We are done, do the clean-up  */
  if( nv_data_ptr != NULL )
  {
    modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  }

  return TRUE;
}





/*----------------------------------------------------------------------------*/
/*!
  @brief
  Generic function to load the FBRX gain switch points data for any band.


  @param rfcommon_nv_selftest_fbrx_gain_switch_pt_type
  The rfcommon_nv_selftest_fbrx_gain_switch_pt_type NV data structure where the cal data has to be loaded into

  @param selftest_fbrx_gain_switch_pts_data_item
  The NV item that contains the cal value.

  @param caller_tcb_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_nv_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_func_ptr
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

  @return
  TRUE if the FBRX attn switch points value is loaded
  FALSE if NV did not load, and some more action is required by caller
*/

/* Size of NV */ 
#define RF_COMMON_SELFTEST_FBRX_GAIN_SWITCH_PTS_FIXED_SIZE  (sizeof(rfcommon_nv_selftest_fbrx_gain_switch_pt_type))

boolean
rfcommon_nv_load_selftest_fbrx_gain_switch_pts_from_nv
(
  rfcommon_nv_selftest_fbrx_gain_switch_pt_type *selftest_fbrx_gain_switch_pts_nv_data,
  rfnv_item_id_enum_type selftest_fbrx_gain_switch_pts_data_item,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
)
{

  /* Overall status for this API */
  boolean api_status = TRUE;
  /* NV read status */
  nv_stat_enum_type  selftest_fbrx_gain_switch_pts_nv_read_status = NV_DONE_S;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type); 
  /* Pointer to NV data*/
  rfnv_item_type *nv_data_ptr = NULL;
  /* Helper ptr to hold moving ptr location during data copy*/
  uint8     *temp_data_ptr = NULL;
  /* Contains the number of bytes for an element in NV */
  uint32 nv_element_size;
  /* actual size of memory read from the NV*/
  rfnv_item_size_type   selftest_fbrx_gain_switch_pts_nv_read_size;

  selftest_fbrx_gain_switch_pts_nv_read_size = sizeof(rfnv_item_type);
  /* Allocate memory for raw NV data */
  nv_data_ptr = (rfnv_item_type *)modem_mem_alloc( sizeof(rfnv_item_type),
                                                   MODEM_MEM_CLIENT_RFA );

  /* Null pointer check */
  if (nv_data_ptr == NULL)
  {
    RF_MSG_1( RF_ERROR, "rfcommon_nv_load_selftest_fbrx_gain_switch_pts_from_nv - failed to allocate memory for %d bytes.", nv_item_size);
    return FALSE;
  }
 
  selftest_fbrx_gain_switch_pts_nv_read_status = rfcommon_rfnv_get_item( selftest_fbrx_gain_switch_pts_data_item,
                                                  nv_data_ptr,
                                                  &selftest_fbrx_gain_switch_pts_nv_read_size,
                                                  caller_tcb_ptr,
                                                  task_nv_wait_sig,
                                                  task_wait_func_ptr );

  if ( selftest_fbrx_gain_switch_pts_nv_read_status == NV_DONE_S )
  {
    RF_MSG_1( RF_HIGH, "rfcommon_nv_load_selftest_fbrx_gain_switch_pts_from_nv: NV read successful, size = %d", selftest_fbrx_gain_switch_pts_nv_read_size);
  } /* if (selftest_fbrx_gain_switch_pts_nv_read_status == NV_DONE_S ) */
  else
  {
    /* initialize the structure with default values accordlingly*/
    RF_MSG_1( RF_ERROR, "rfcommon_nv_load_selftest_fbrx_gain_switch_pts_from_nv: Initialized the fbrx gain data to default", 0);	
  	api_status = FALSE;
    if( nv_data_ptr != NULL )
    {
      modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
    }

    return FALSE;
  } /* else: if (fbrx_attn_switch_pts_nv_status == NV_DONE_S ) */

  /* If NV data has been successfully retrieved proceed to populating the data structure */
  if (api_status == TRUE) 
  {
    temp_data_ptr = (uint8 *)nv_data_ptr;
    /* copy data for fixed part and variant header of the NV, need to adjust size*/
    nv_element_size = RF_COMMON_SELFTEST_FBRX_GAIN_SWITCH_PTS_FIXED_SIZE;

    memscpy((void *)selftest_fbrx_gain_switch_pts_nv_data, sizeof(rfcommon_nv_selftest_fbrx_gain_switch_pt_type), (void*)temp_data_ptr, nv_element_size );

    /* print some debug - to be removed*/
    RF_MSG_1( RF_HIGH, "rfcommon_nv_load_selftest_fbrx_gain_switch_pts_from_nv - No. of switch points = %d",selftest_fbrx_gain_switch_pts_nv_data->num_switch_pts);
  }

  /* We are done, do the clean-up  */
  if( nv_data_ptr != NULL )
  {
    modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  }

  return TRUE;

} /* rfcommon_nv_load_fbrx_attn_switch_pts_from_nv() */

