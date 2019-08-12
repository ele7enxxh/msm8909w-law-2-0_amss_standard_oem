/*==============================================================================
  FILE:         uim_logging.c

  OVERVIEW:     File contains the functions for APDU and EFS logging.

  DEPENDENCIES: N/A

                Copyright (c) 2016 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_efs_logging.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       --------------------------------------------------------- 
04/27/16    na         Draft created for EFS logging function
==============================================================================*/
#include "intconv.h"
#include "rex.h"
#include "fs_public.h"
#include "comdef.h"
#include "time_svc.h"
#include "log.h"
#include "uimdrv_msg.h"
#include "fs_lib.h"
#include "fs_stdlib.h"
#include "fs_diag_access.h"
#include "mcfg_fs.h"

#include "uim_p.h"
#include "uimi.h"
#include "uimglobals.h"
#include "uim_logging.h"
#include "uimdrv_gpio.h"


/**
 * DECLARATIONS OF INTERNAL FUNCTIONS
 */
static boolean uim_efslog_purge_check( uim_instance_global_type *uim_ptr );
static boolean uim_efs_write(
  int                       fhandle,
  const char               *databuffer_p,
  int                       datasize,
  uim_instance_global_type *uim_ptr
);

/**
 * DEFINITIONS OF EXTERNAL FUNTIONS
 *
 */

/**
 * This file establishes the file in EFS to which EFSLOG will
 * write. The local EFSLOG buffer cannot be purged until this
 * function has completed successfully. This function will also
 * establish the local EFS buffer
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_efslog_init( uim_instance_global_type *uim_ptr )
{
  byte   apdu_log_file_name[20];
  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_LOG_APDU_TO_EFS,
                               uim_ptr) == FALSE)
  {
    uim_ptr->debug.efslog_logging_in_progress = FALSE;
    return TRUE;
  }
  (void)memset((void *)apdu_log_file_name,
               (int)0x00,
               (size_t)sizeof(apdu_log_file_name));

  if(snprintf((char *)apdu_log_file_name,20,"Uim%dEfsAPDULog.Txt",(uim_ptr->id + 1)) < 0)
  {
    uim_ptr->debug.efslog_logging_in_progress = FALSE;
    return FALSE;
  }
  /* Before creating the EFS File for APDU logging, erase the old one if
  there is one */
  if(MCFG_FS_STATUS_EFS_ERR == mcfg_fs_delete((char *)apdu_log_file_name, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE))
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Error removing file for EFSLog, or no old file to remove");
  }

  // Create file using MCFG file open API
  uim_ptr->debug.efslog_apdu_file_handle = mcfg_fopen((char *)apdu_log_file_name, MCFG_FS_O_WRONLY|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC,
                                                       MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE);

  if (uim_ptr->debug.efslog_apdu_file_handle < 0)
  {
    uim_ptr->debug.efslog_logging_in_progress = FALSE;
    return FALSE;
  }

  /* Allocate space for main buffer */
  uim_ptr->debug.efslog_apdu_buffer.data = uim_malloc(UIM_EFSLOG_MAX_BUFFER_SIZE);

  /* If allocation failed, return without turning on logging */
  if(uim_ptr->debug.efslog_apdu_buffer.data == NULL)
  {
    /* Close the EFS file  */
    (void)mcfg_fclose(uim_ptr->debug.efslog_apdu_file_handle, MCFG_FS_TYPE_EFS);
    uim_ptr->debug.efslog_logging_in_progress = FALSE;
    return FALSE;
  }

  /* Allocate space for secondary outgoing data buffer */
  uim_ptr->debug.efslog_apdu_outgoing_data_buffer.data = uim_malloc(UIM_EFSLOG_MAX_BUFFER_SIZE);

  /* If allocation failed, return without turning on logging */
  if(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.data == NULL)
  {
    /* Close the EFS file and free up the local buffer memory */
    (void)mcfg_fclose(uim_ptr->debug.efslog_apdu_file_handle, MCFG_FS_TYPE_EFS);
    UIM_FREE(uim_ptr->debug.efslog_apdu_buffer.data);
    uim_ptr->debug.efslog_logging_in_progress = FALSE;
    return FALSE;
  }
  uim_ptr->debug.efslog_apdu_buffer.buffer_size = 0;
  uim_ptr->debug.efslog_apdu_outgoing_data_buffer.buffer_size = 0;

  uim_ptr->debug.efslog_logging_in_progress = TRUE;
  return TRUE;
} /* uim_efslog_init */


/**
 * This function takes a character to be stored to the buffer
 * and an attribute,only if EFS Logging is currently in
 * progress. It converts the character to ascii, and checks the
 * attribute to see if it differs from the previous character's
 * attribute. If it does not, then it simply places the ASCII
 * value of the character in the next space in the buffer (One
 * character will become two ASCII bytes).  If the attribute is
 * different, it will place a new line character '\n' followed
 * by the attribute tag, a colon, and then the character (in
 * ASCII format).
 *
 * @param attribute Attribute of logging data
 * @param character Data to be logged.
 * @param uim_ptr Pointer to the global data common.
 */
void uim_efslog_apdu_to_buffer(
  uim_instance_global_type *uim_ptr
)
{
  /* for converting from HEX To ASCII */
  char ascii[17] = "0123456789ABCDEF";

  /* Create a local ptr to uim_efslog_apdu_buffer to improve
     code readability */
  uim_efslog_buffer_type *log_buf_ptr = &uim_ptr->debug.efslog_apdu_buffer;

  /* If the buffer is half full, set the EFSLOG signal */
  if(uim_efslog_purge_check(uim_ptr))
  {
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_EFSLOG_PURGE_SIG );
  }

  /* If EFS Logging not currently in progress, return without doing anyting */
  if(!uim_ptr->debug.efslog_logging_in_progress || log_buf_ptr->data == NULL)
  {
    return;
  }
  if (log_buf_ptr->buffer_size >= (UIM_EFSLOG_MAX_BUFFER_SIZE - 8))
  {
    /* There must be at least 8 empty spaces for single slot feature and 17
     * empty spaces for dual slot feature left in the buffer to log another
     * character in case a new attribute tag is encountered
     */
    return;
  }

  /* If the attibute has changed, start a new line */
  if(uim_ptr->debug.log_char.attrib != uim_ptr->debug.efslog_previous_char_attribute)
  {
    /* Insert a carriage return and line feed */
    log_buf_ptr->data[log_buf_ptr->buffer_size++] = UIM_EFSLOG_ASCII_CR;
    log_buf_ptr->data[log_buf_ptr->buffer_size++] = UIM_EFSLOG_ASCII_LF;
    /* Store Upper Nibble of Attribute tag in ASCII */
    switch((uim_log_attrib_type)uim_ptr->debug.log_char.attrib)
    {
      case UIM_LOG_TX_DATA:
        log_buf_ptr->data[log_buf_ptr->buffer_size++] = 'T';
        log_buf_ptr->data[log_buf_ptr->buffer_size++] = 'X';
        break;
      case UIM_LOG_RX_DATA:
        log_buf_ptr->data[log_buf_ptr->buffer_size++] = 'R';
        log_buf_ptr->data[log_buf_ptr->buffer_size++] = 'X';
        break;
      /* Required because of compiler warnings */
      case UIM_LOG_TSTAMP:
      default:
        break;
    }
    
    /* Insert a Colon and a space */
    log_buf_ptr->data[log_buf_ptr->buffer_size++] = ':';
  }

  /* Store Upper Nibble of byte in ASCII */
  log_buf_ptr->data[log_buf_ptr->buffer_size++] = ascii[uim_ptr->debug.log_char.the_char >> 4];

  /* Store Lower Nibble of byte in ASCII */
  log_buf_ptr->data[log_buf_ptr->buffer_size++] = ascii[uim_ptr->debug.log_char.the_char & 0x0F];

  /* Update Previous Char Attribute tag */
  uim_ptr->debug.efslog_previous_char_attribute = uim_ptr->debug.log_char.attrib;

} /* uim_efslog_apdu_to_buffer */


/**
 * This function gets called when a the local logging buffer
 * reaches a certain threashold, and we want to purge this data
 * to the EFS file.  This makes a blocking call to efs_write.
 * First, we copy into the secondary buffer and then write the
 * contents of the secondary buffer to EFS.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_efslog_save_buffer_to_efs( uim_instance_global_type *uim_ptr )
{
  int return_status = 0;

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_LOG_APDU_TO_EFS,
                               uim_ptr) == FALSE)
  {
    /* Increment the EFS Write counter in case we have to stop logging
       before we ever get a GSDI Event */
    uim_ptr->debug.efslog_save_count++;
    return;
  }
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_EFSLOG_PURGE_SIG );

  if( (uim_ptr->debug.efslog_apdu_buffer.buffer_size > 0) &&
      (TRUE == uim_ptr->debug.efslog_logging_in_progress)
    )
  {
    uim_ptr->debug.efslog_apdu_outgoing_data_buffer.buffer_size =
                      uim_ptr->debug.efslog_apdu_buffer.buffer_size;
    uim_memscpy(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.data,
           int32touint32(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.buffer_size),
           uim_ptr->debug.efslog_apdu_buffer.data,
           int32touint32(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.buffer_size));
    /* Clear the buffer and set size to 0 */
    memset(uim_ptr->debug.efslog_apdu_buffer.data,0,UIM_EFSLOG_MAX_BUFFER_SIZE);
    uim_ptr->debug.efslog_apdu_buffer.buffer_size = 0;
    return_status = mcfg_fwrite( uim_ptr->debug.efslog_apdu_file_handle,
                                (void *)(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.data),
                                 int32touint32(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.buffer_size),
                                 MCFG_FS_TYPE_EFS );

    /* If the write failed, we want to clean-up and stop logging.  However,
       if the write failed because the buffer size was zero, we don't count
       this as a failure.  Also, if logging is already off, it means that this
       is the final write called from uim_efslog_clean_up.  Clean-up will
       free the buffers, so don't do it here */

     if((return_status <= 0) &&
        (!(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.buffer_size == 0)) &&
         uim_ptr->debug.efslog_logging_in_progress)
     {
      /* There was an error saving to EFS, or it's full.  Clean up without
      trying to save again */
      uim_ptr->debug.efslog_logging_in_progress = FALSE;
      (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_EFSLOG_PURGE_SIG );
    (void)mcfg_fclose(uim_ptr->debug.efslog_apdu_file_handle, MCFG_FS_TYPE_EFS);
    UIM_FREE(uim_ptr->debug.efslog_apdu_buffer.data);
    UIM_FREE(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.data);
    return;
  }

    /* Increment the EFS Write counter in case we have to stop logging
       before we ever get a GSDI Event */
    uim_ptr->debug.efslog_save_count++;
  }
} /* uim_efslog_save_buffer_to_efs */


/**
 * Cleans up EFSLOG when complete
 *
 * @param uim_ptr
 */
void uim_efslog_clean_up( uim_instance_global_type *uim_ptr )
{
  uim_ptr->debug.efslog_logging_in_progress = FALSE;
  /* In case there's any data left in the buffer, send it to EFS now */
  uim_efslog_save_buffer_to_efs(uim_ptr);

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_LOG_APDU_TO_EFS,
                               uim_ptr) == FALSE)
  {
    return;
  }

  
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_EFSLOG_PURGE_SIG );
  /* Close the EFS file and free up the local buffer memory */
  (void)mcfg_fclose(uim_ptr->debug.efslog_apdu_file_handle, MCFG_FS_TYPE_EFS);
  if(uim_ptr->debug.efslog_apdu_buffer.data != NULL)
  {
    UIM_FREE(uim_ptr->debug.efslog_apdu_buffer.data);
  }
  if(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.data != NULL)
  {
    UIM_FREE(uim_ptr->debug.efslog_apdu_outgoing_data_buffer.data);
  }
  return;
} /* uim_efslog_clean_up */


/**
 * This file checks if a purge is necessary for any EFS Buffers.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean True if EFS logging feature is enabled.
 */
static boolean uim_efslog_purge_check(
  uim_instance_global_type *uim_ptr
)
{
  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_LOG_APDU_TO_EFS,
                               uim_ptr) == TRUE &&
     uim_ptr->debug.efslog_apdu_buffer.buffer_size >= (UIM_EFSLOG_MAX_BUFFER_SIZE/2))
  {
    return TRUE;
  }
  return FALSE;
} /* uim_efslog_purge_check */


/**
 * This file handle the processing of purge signal.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return none.
 */
void uim_efslog_process_purge_sig(uim_instance_global_type *uim_ptr)
{
  
  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_LOG_TO_EFS, uim_ptr) == TRUE)
  {
    /* If we still haven't got a GSDI Event (such as SIM INIT COMPLETED
           telling us to stop logging, and our log is getting massive, clean
           up and stop logging
           Also, if the logging_in_progress flag has been turned off,
	    we should stop logging */
  
	  if((uim_ptr->debug.efslog_save_count >= UIM_EFSLOG_MAX_SAVES_TO_EFS) ||
		 !uim_ptr->debug.efslog_logging_in_progress)
	  {
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "In Purge_sig");
		uim_efslog_clean_up(uim_ptr);
	  }
	  else
	  {
		/* When the purge EFSLOG Signal is sent, send it to EFS. */
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "Writing Buffer to EFS");
		uim_efslog_save_buffer_to_efs(uim_ptr);
	  }
  }
  return;
}/* uim_efslog_process_purge_sig */
