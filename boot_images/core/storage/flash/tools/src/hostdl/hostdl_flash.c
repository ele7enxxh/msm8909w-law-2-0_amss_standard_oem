/*==================================================================
 *
 * FILE:        hostdl_flash.c
 *
 * SERVICES:    Flash Programming interface
 *
 * DESCRIPTION:
 *    This module implements the commands to program the flash
 *
 *    This File contains the "public" interface to the flash driver,
 *    which is common to all flash plug-ins.
 *
 * Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_flash.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * --------   ---  ----------------------------------------------
 * 02/15/13     sv      Adding Erase NAND capability in EDL/QPST download
 * 01/29/13     sv      Fix KW issue - Null pointer access
 * 05/30/12     bb      Fix to return correct status when partition table differs
 * 08/16/11     jz      Removed sbuffer layer, support new partitions
 * 03/18/11     jz      Added handling of ERR_PARTI_TOO_BIG
 * 10/14/10     sv      modify flash_device_sectors to fix klocwork issue
 * 05/08/09     jz      Added SB2.0 support in do_open_multi()
 * 10/29/08     mm      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "comdef.h"
#include "hostdl.h"
#include "hostdl_flash.h"
#include "hostdl_config.h"
#include "hostdl_sbuffer.h"
#include "hostdl_log.h"
#include "flash_prog.h"

//--------------------------------------------------------------------------
// External Declarations
//--------------------------------------------------------------------------
extern parti_tbl_type partition_status;
extern byte open_multi_status;

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------
/* This is the number of sectors which will fit in the reply buffer.
 * This really should be done by calculations, but due to the nature of
 * the parameter request reply, it must be done by estimation.  This is
 * because one part of the reply is variable length, and that is the flash
 * name.  So, take all the fixed length fields and add up their length, then
 * take a very long length for the flash name and add that to it.  Subtract
 * this value from the reply length buffer size.  Now you have the number of
 * bytes that are available for storing sector sizes.  Divide this by 4
 * to get the number of sectors we can put in, and then subtract some to be
 * absolutely sure we stay below the limit.
 */

#define MAX_BLOCKS_IN_REPLY 200

//--------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------

/* This needs to be instantiated in every flash device layer that we
 * create.  It is generic to the flash layer */

/* enum to indicate status of probe */
probe_code_type flash_dev_type = FLASH_DEVICE_NOT_PROBED;
flash_param_t *nand_device_params;

//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------

/*===========================================================================
  This function writes a provided string of bytes into a specified
  block of memory, by calling the appropriate device-specific function.
  Erase blocks as necessary before writing.
===========================================================================*/
static int hostdl_write_buffer_to_flash(uint8 * data, uint32 addr, uint32 len)
{
  flash_prog_status_t result;

  result = (*flash_prog_function_tbl.program_img)((void *)data, len, addr);

  if (FLASH_PROG_SUCCESS != result)
  {
    FLASHLOG(1, ("hwbtf: Buffer commit to flash failed\n"));
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}


/*===========================================================================

  This function does one time initialization of data structures used in
  flash device access

===========================================================================*/

void flash_device_init_data (void)
{
  flash_prog_status_t result;

  flash_dev_type = FLASH_DEVICE_NOT_PROBED;

  result = (*flash_prog_function_tbl.get_flash_param)(
    (void *)&nand_device_params);

  if (FLASH_PROG_SUCCESS == result)
  {
    flash_dev_type = FLASH_DEVICE_FOUND;
    FLASHLOG(1, ("fdid: Flash paramaters retrieved successfully\n"));
  }
  else
  {
    FLASHLOG(1, ("fdid: Failure retrieving flash paramaters\n"));
  }

  return;
}


/*===========================================================================

  This function returns a pointer to a string containing the flash
  device name

===========================================================================*/

char * flash_device_name (void)
{
  char *flash_name;

  flash_name = nand_device_params->flash_name;

  return flash_name;
}

/*===========================================================================

  This function fills the indicated handle_hello response buffer with the
  number of sectors, and a length for each sector.

===========================================================================*/

int flash_device_sectors (uint8 *reply_buffer, uint32 buf_size)
{
  uint32 nbytes;
  int i, p;
  int numblocks, pages, pagesize;

  p = 0;

  numblocks = nand_device_params->num_blocks;
  pages = nand_device_params->pages_in_block;
  pagesize = nand_device_params->page_size;

  /*  Adjust blocks and pages so we don't overflow reply buffer.
   *  The number of blocks and the size of the blocks in reality
   *  only need to add up to the actual space available.  The host tool
   *  only uses these in a progress message, and never uses this
   *  information in reality to manipulate anything else.  So, we keep
   *  shifting down the number blocks while shifting up the size of
   *  the blocks until the number of blocks will fit in the reply buffer.
   */
  while (numblocks > MAX_BLOCKS_IN_REPLY)
  {
    numblocks = numblocks >> 1;
    pages = pages << 1;
  }

  /* Check if reply buffer can hold 2 bytes of block count data 
   * and numblocks * 4 bytes of block size info.
   */
  if ((numblocks * 4 + 2) <= buf_size) 
  {          
    /* Calculate new "sector size from adjusted number of pages */
    nbytes = pages * pagesize;

    FLASHLOG(1,("flash_device_sectors: 0x%x sectors of size 0x%x\n", \
      numblocks, nbytes));

    /* Store the adjusted block count */
    reply_buffer[p++] = (numblocks) & 0xFF;
    reply_buffer[p++] = (numblocks >> 8) & 0xFF;

    /* Finally, store one block size unit for every "adjusted block" */
    for (i = 0; i < numblocks; i++)
    {
      reply_buffer[p++] = (nbytes) & 0xFF;
      reply_buffer[p++] = (nbytes >> 8) & 0xFF;
      reply_buffer[p++] = (nbytes >> 16) & 0xFF;
      reply_buffer[p++] = (nbytes >> 24) & 0xFF;
    }
  }

  return p;
}


/*===========================================================================

  This function Sends the security mode to the layer below

===========================================================================*/
response_code_type do_security_mode (byte mode)
{

  flash_prog_status_t result;

  KICK_WATCHDOG();

  result = (*flash_prog_function_tbl.set_security_mode)(mode);
  if (FLASH_PROG_SUCCESS == result)
  {
    return ACK;
  }
  else
  {
    return NAK_FAILED;
  }

}                               /* do_security_mode() */


/*===========================================================================

  This function Sends the partition table to the layer below

===========================================================================*/
response_code_type do_partition_table (byte *data, byte override, word size)
{

  flash_prog_status_t result;
  fatal_code_type err_code = ERR_OK;

  KICK_WATCHDOG();

  result = (*flash_prog_function_tbl.init_partition_tbl)((void *)data,
    (uint32)size, FLASH_PROG_USR_PARTI, (uint32)override);


  /* If it did not succeed, it may have indicated a partition table
   * mis-match and in this case, we return a very different error
   * that allows the host to retry with override.
   */
  if (FLASH_PROG_SUCCESS != result)
  {
    result = (*flash_prog_function_tbl.get_error_code)(&err_code, (char **)NULL);

    if (FLASH_PROG_SUCCESS == result)
    {
      switch (err_code)
      {
        case ERR_PARTI_NOMATCH:
        {
          partition_status = PARTI_TBL_DIFFERS;
          FLASHLOG(2,("do_parti_tbl: table differs\n"));
          break;
        }
        case ERR_ERASE_FAIL:
        {
          partition_status = PARTI_TBL_ERASE_FAIL;
          FLASHLOG(2,("do_parti_tbl: erase failed\n"));
          break;
        }
        case ERR_PARTI_INVALID:
        {
          partition_status = PARTI_TBL_BAD_FORMAT;
          FLASHLOG(2,("do_parti_tbl: bad table format\n"));
          break;
        }
        case ERR_PARTI_TOO_BIG:
        {
          partition_status = PARTI_TBL_TOO_BIG;
          FLASHLOG(2,("do_parti_tbl: table too big\n"));
          break;
        }
        default:
        {
          partition_status = PARTI_TBL_UNKNOWN_ERROR;
          FLASHLOG(2,("do_parti_tbl: unknown error\n"));
          break;
        }
      }
    }
    else
    {
      FLASHLOG(2,("do_parti_tbl: Cannot get error code\n"));
      partition_status = PARTI_TBL_UNKNOWN_ERROR;
      FLASHLOG(2,("do_parti_tbl: unknown error\n"));
    }

    return NAK_FAILED;
  }

  return ACK;
}



/*===========================================================================

  This function handles the open for factory image mode.

===========================================================================*/

response_code_type do_open (byte mode)
{
  flash_prog_status_t result;
  flash_prog_img_type_t image = FLASH_PROG_UNKNOWN_IMG;

  KICK_WATCHDOG();

  /* Call Stream Buffering layer to init it's data structures.
   * This must be done on every open because we need to reset
   * to initial conditions.
   */
  FLASHLOG(2,("do_open: after sbuffer_init mode is 0x%x\n", mode));

  /* Set status with the command to initialize */
  switch (mode)
  {
    case OPEN_MODE_FACTORY:
    {
      FLASHLOG(2,("do_open: open for Factory Image\n"));
      image = FLASH_PROG_FACTORY_IMG;
      break;
    }

    case OPEN_BOOTLOADER:
    case OPEN_BOOTABLE:
    case OPEN_CEFS:
    case OPEN_MODE_NONE:
    default:
    {
        FLASHLOG(2,("do_open: open for INVALID mode\n"));
        return NAK_FAILED;
    }
  }

  FLASHLOG(2,("do_open: dispatching\n"));
  result = (*flash_prog_function_tbl.set_image_type)(image, (void *)NULL, 0);

  FLASHLOG(2,("do_open: back from dispatch\n"));

  if (FLASH_PROG_SUCCESS != result)
  {
    return NAK_FAILED;
  }

  return ACK;
}


/*===========================================================================

  This function handles the open for all modes.

===========================================================================*/
response_code_type do_open_multi (byte *data, byte mode, uint32 length)
{
  flash_prog_status_t result;
  fatal_code_type err_code = ERR_OK;
  flash_prog_img_type_t image = FLASH_PROG_UNKNOWN_IMG;

  KICK_WATCHDOG();

  FLASHLOG(2,("do_open_multi: entry mode is 0x%x\n", mode));


  /* Call Stream Buffering layer to init it's data structures.
   * This must be done on every open because we need to reset
   * to initial conditions if we open for multiple images
   * in the same session.
    */
  FLASHLOG(2,("do_open_multi: after sbuffer_init mode is 0x%x\n", mode));

  /*
       Set status to correct open mode
  */
  FLASHLOG(2,("do_open_multi: before switch mode is 0x%x\n", mode));
  switch (mode)
  {
    case OPEN_MULTI_MODE_QCSBLHDCFG:
    {
      FLASHLOG(2,("do_open_multi: open for QCSBLHDCFG\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_QCSBLHD_CONFIGDAT_IMG;
      break;
    }
    case OPEN_MULTI_MODE_QCSBL:
    {
      FLASHLOG(2,("do_open_multi: open for QCSBL\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_QCSBL_IMG;
      break;
    }
    case OPEN_MULTI_MODE_OEMSBL:
    {
      FLASHLOG(2,("do_open_multi: open for OEMSBL\n"));
      if (length == 0)
      {
        FLASHLOG(2,("do_open_multi: No header sent\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_REQUIRED;
        return NAK_FAILED;
      }
      if (length > 512)
      {
        FLASHLOG(2,("do_open_multi: Header too large\n"));
        open_multi_status = OPEN_MULTI_LENGTH_EXCEEDED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_OEMSBL_IMG;
      break;
    }
    case OPEN_MULTI_MODE_DBL:
    {
      FLASHLOG(2,("do_open_multi: open for DBL\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_DBL_IMG;
      break;
    }
    case OPEN_MULTI_MODE_SBL1:
    {
      FLASHLOG(2,("do_open_multi: open for SBL1\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_SBL1_IMG;
      break;
    }
    case OPEN_MULTI_MODE_FSBL:
    {
      FLASHLOG(2,("do_open_multi: open for FSBL\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_FSBL_IMG;
      break;
    }
    case OPEN_MULTI_MODE_OSBL:
    {
      FLASHLOG(2,("do_open_multi: open for OSBL\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_OSBL_IMG;
      break;
    }
    case OPEN_MULTI_MODE_SBL2:
    {
      FLASHLOG(2,("do_open_multi: open for SBL2\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_SBL2_IMG;
      break;
    }
    case OPEN_MULTI_MODE_AMSS:
    {
      FLASHLOG(2,("do_open_multi: open for AMSS\n"));
      image = FLASH_PROG_AMSS_IMG;
      break;
    }
    case OPEN_MULTI_MODE_APPSBL:
    {
      FLASHLOG(2,("do_open_multi: open for APPSBL\n"));
      image = FLASH_PROG_APPS_BOOT_IMG;
      break;
    }
    case OPEN_MULTI_MODE_APPS:
    {
      FLASHLOG(2,("do_open_multi: open for APPS\n"));
      image = FLASH_PROG_APPS_IMG;
      break;
    }
    case OPEN_MULTI_MODE_CEFS:
    {
      FLASHLOG(2,("do_open_multi: open for modem CEFS\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_MODEM_CEFS_IMG;
      break;
    }
    case OPEN_MULTI_MODE_APPS_CEFS:
    {
      FLASHLOG(2,("do_open_multi: open for apps CEFS\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_APPS_CEFS_IMG;
      break;
    }
    case OPEN_MULTI_MODE_DSP1:
    {
      FLASHLOG(2,("do_open_multi: open for DSP1\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_DSP1_IMG;
      break;
    }
    case OPEN_MULTI_MODE_CUSTOM:
    {
      FLASHLOG(2,("do_open_multi: open for CUSTOM image\n"));
      if (length == 0)
      {
        FLASHLOG(2,("do_open_multi: No header sent\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_REQUIRED;
        return NAK_FAILED;
      }
      if (length > 512)
      {
        FLASHLOG(2,("do_open_multi: Header too large\n"));
        open_multi_status = OPEN_MULTI_LENGTH_EXCEEDED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_CUSTOM_IMG;
      break;
    }
    case OPEN_MULTI_MODE_FLASH_BIN:
      FLASHLOG(2,("do_open_multi: open for apps flash.bin\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_WM_IMG;
      break;
    case OPEN_MULTI_MODE_DSP2:
    {
      FLASHLOG(2,("do_open_multi: open for DSP2\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_DSP2_IMG;
      break;
    }
    case OPEN_MULTI_MODE_DSP3:
    {
      FLASHLOG(2,("do_open_multi: open for DSP3\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_DSP3_IMG;
      break;
    }
    case OPEN_MULTI_MODE_RPM:
    {
      FLASHLOG(2,("do_open_multi: open for RPM\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_RPM_IMG;
      break;
    }
    case OPEN_MULTI_MODE_RAW:
    {
      FLASHLOG(2,("do_open_multi: open for apps EFS2 Raw\n"));
      if (length > 0)
      {
        FLASHLOG(2,("do_open_multi: not expecting data\n"));
        open_multi_status = OPEN_MULTI_PAYLOAD_NOT_ALLOWED;
        return NAK_FAILED;
      }
      image = FLASH_PROG_RAW_IMG;
      break;
    }
    /* Unknown mode, failure */
    default:
    {
      FLASHLOG(2,("do_open_multi: open for INVALID mode\n"));
      open_multi_status = OPEN_MULTI_UNKNOWN_ERROR;
      return NAK_FAILED;
    }
  }

  /*
       Invoke the NAND flash dispatch function
  */
  FLASHLOG(2,("do_open_multi: dispatching\n"));
  result = (*flash_prog_function_tbl.set_image_type)(image, (void *)data, length);

  FLASHLOG(2,("do_open_multi: back from dispatch\n"));

  /* If it did not succeed, it may have indicated a partition table
   * mis-match and in this case, we return a very different error
   * that allows the host to retry with override.
   */
  if (FLASH_PROG_SUCCESS != result)
  {
    /* Set the correct error code for OPEN_MULTI command that can be
     * propagated back to HOST.
     */
    switch (mode)
    {
      case OPEN_MULTI_MODE_CUSTOM:
      {
        result = (*flash_prog_function_tbl.get_error_code)(&err_code,
          (char **)NULL);
        if (ERR_UNKNWN_PARTI == err_code)
        {
          open_multi_status = OPEN_MULTI_UNKNOWN_PARTITION;
        }
        break;
      }

      /* Unknown mode, failure */
      default:
      {
        open_multi_status = OPEN_MULTI_UNKNOWN_ERROR;
        break;
      }
    }

    return NAK_FAILED;
  }

  return ACK;
}

/*===========================================================================

  This function is a stub for a function needed in the next revision of
  the protocol.

===========================================================================*/

response_code_type do_close ()
{
  flash_prog_status_t result;

  KICK_WATCHDOG();


#if !defined(SKIP_FLASH_WRITE)
  /*
      Stuff the flash_prog variables with values that TRACE32 would have used to
      indicate to FLASH programmer that flash_finalize should be called.
  */
  FLASHLOG(1,("\n----- do_close calling finalize ------\n"));

  /*
       Invoke the NAND flash dispatch function
  */
  result = (*flash_prog_function_tbl.finalize_img)();

  KICK_WATCHDOG();

  if (FLASH_PROG_SUCCESS != result)
  {
    return NAK_FAILED;
  }

#endif  /* SKIP_FLASH_WRITE */

  return ACK;
}

/*===========================================================================

    This function handles the NAND erase.

===========================================================================*/

response_code_type do_erase()
{
  flash_prog_status_t result;

  KICK_WATCHDOG();

  /*
       Invoke the NAND flash dispatch function
  */
  result = (*flash_prog_function_tbl.erase_chip)();

  KICK_WATCHDOG();

  if (FLASH_PROG_SUCCESS != result)
  {
    return NAK_FAILED;
  }

  return ACK;
}

/*===========================================================================

  This function writes a provided string of bytes into a specified
  block of memory, by calling the appropriate device-specific function.
  For NOR flash, this is not really necessary, however, the interface is here
  to support other flash types that need the differentation between write,
  which requires a prior erase and simple_write which does not.

===========================================================================*/

response_code_type do_simple_write (byte *buf, dword addr, word len)
{
  /* We do not do simple_write, but do not want to fail when it is
   * requested.
   */
  return ACK;
}


/*===========================================================================

  This function writes a provided string of bytes into a specified
  block of memory, by calling the appropriate device-specific function.
  Erase blocks as necessary before writing.

===========================================================================*/

response_code_type do_stream_write (byte *buf, dword addr, word len)
{
  int status;

  CHECK_FOR_DATA ();

  /* Call stream buffering layer which in turn will call actual NAND
   * write function appropriately with strictly in order data
   */
  status = hostdl_write_buffer_to_flash (buf, addr, (uint32)len);
  if (status == FALSE)
  {
    FLASHLOG(1,("dsw:  sb failed\n"));
    return NAK_FAILED;
  }
  else
  {
    return ACK;
  }
}


/*===========================================================================

  This function reads string of bytes into a specified
  block of RAM memory, not FLASH.  It is used by QPST to read the mobile
  model number from the running build.

===========================================================================*/
response_code_type do_read (byte * buf, dword addr, word length)
{
  int i, j;

  for (i=0, j=0; j < length; j++)
  {
    buf[i++] = ((byte *) addr)[j];
  }
  return ACK;
}
