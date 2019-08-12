/*=======================================================================
 * FILE:        nand_tools.c
 *
 * SERVICES:    APIs for NAND tools utility.
 *
 * GENERAL DESCRIPTION
 *    APIs for NAND tools utility.
 *
 * Copyright (c) 2008-2010,2011-2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_tools.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 02/15/12     sv      Adding Erase NAND capability in EDL/QPST download
 * 09/17/12     eo      Ehostdl/hostdl support for 9x25
 * 08/02/11     sv      Added DSP3 partition support for 9x15
 * 06/22/11     eo      Added SBLx/RPM partition support for 9x15
 * 08/26/10     sb      Allow flashing without providing partition table for GOBI(OSBL only)
 * 08/12/10     nr      Moved nand_log.h reference to nand_wrapper.h
 * 11/24/09     wt      Fixed the factory image support
 * 04/09/09     bb      Fixed PROG_APPS_BOOT to init prog_param in set_img
 * 02/20/09     jz      Added support for SB2.0
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_prog.h"
#include "nand_utils.h"
#include "nand_core.h"
#include "nand_param.h"
#include "nand_wrapper.h"

#ifdef BUILD_NAND
/* If building NANDPRG, hostdl.c contains the stack */
uint32 svc_stack[0x1000/4];             // main stack
#endif

static boolean parti_tbl_rcvd = FALSE;

static flash_prog_status_t flash_prog_get_flash_param(void **device_params);

static flash_prog_status_t flash_prog_set_sec_mode(uint32 sec_mode);

static flash_prog_status_t flash_prog_init_partition_table(void * parti_data,
  uint32 length, flash_prog_parti_type_t parti_type, uint32 override_flag);

static flash_prog_status_t flash_prog_set_img_type(flash_prog_img_type_t img_type,
  void *hdr_data, uint32 hdr_len);

static flash_prog_status_t flash_prog_program(void * data, uint32 len, uint32 addr);

static flash_prog_status_t flash_prog_finalize(void);

static flash_prog_status_t flash_prog_erase(void);

static flash_prog_status_t flash_prog_get_error_code(fatal_code_type *errno,
  char **mesg);

/* NAND programmer table */
/* Used by clients to program a particular image to the flash */
flash_prog_img_prg_table_t flash_prog_function_tbl =
{
  flash_prog_get_flash_param,
  flash_prog_set_sec_mode,
  flash_prog_init_partition_table,
  flash_prog_set_img_type,
  flash_prog_program,
  flash_prog_finalize,
  flash_prog_get_error_code,
  flash_prog_erase
};

/* Main dispatch function for the NAND writing layer */
extern void nand_dispatch(void);

/* This function sets the type of image to be programmed and
 * calls the init function for that particular image.
 */
static flash_prog_status_t flash_prog_set_img_type(flash_prog_img_type_t img_type , 
  void *hdr_data, uint32 hdr_size)
{
  /* Check if the image type is valid */
  if (img_type >= FLASH_PROG_UNKNOWN_IMG)
  {
    /* Invalid imag type */
    return FLASH_PROG_FAIL;
  }

  /* Detemine if the boot is secured or not */
  prog_param.reserved = FLASH_MI_BOOT_SEC_MODE_TRUSTED;

  /* Ensure that the security mode is set */
  if (FLASH_PROG_SUCCESS != flash_prog_set_security_mode())
  {
    /* Failed to set security mode */
    return FLASH_PROG_FAIL;
  }
  
  if(img_type != FLASH_PROG_FACTORY_IMG)
  {
    /* Ensure that the partition table is received */
    if (FLASH_PROG_SUCCESS != flash_prog_init_partition_table(hdr_data, hdr_size,
    FLASH_PROG_USR_PARTI, FLASH_PROG_PARTI_NO_OVERRRIDE))
    {
      /* Failed to send partition info */
      return FLASH_PROG_FAIL;
    }
  }
  /* Set the correct init function for the image type */
  switch (img_type)
  {
    case FLASH_PROG_QCSBLHD_CONFIGDAT_IMG:
    case FLASH_PROG_QCSBL_IMG:
    case FLASH_PROG_DBL_IMG:
    case FLASH_PROG_FSBL_IMG:
    case FLASH_PROG_OSBL_IMG:
    case FLASH_PROG_SBL1_IMG:
    case FLASH_PROG_SBL2_IMG:
    case FLASH_PROG_RPM_IMG:
    case FLASH_PROG_MODEM_CEFS_IMG:
    case FLASH_PROG_APPS_CEFS_IMG:
    case FLASH_PROG_FOTAUI_IMG:
    case FLASH_PROG_WM_IMG:
    case FLASH_PROG_DSP1_IMG:
    case FLASH_PROG_DSP2_IMG:
    case FLASH_PROG_DSP3_IMG:
    case FLASH_PROG_FACTORY_IMG:
    case FLASH_PROG_ADSP_IMG:
    case FLASH_PROG_OSBL_UPDATE_IMG: 		  
    case FLASH_PROG_AMSS_UPDATE_IMG: 		  
    case FLASH_PROG_DSP_UPDATE_IMG:			  
    {
      break;
    }
    case FLASH_PROG_OEMSBL_IMG:
    case FLASH_PROG_AMSS_IMG:
    case FLASH_PROG_APPS_IMG:
    case FLASH_PROG_CUSTOM_IMG:
    case FLASH_PROG_APPS_BOOT_IMG:
    {
      prog_param.data = (byte *) hdr_data;
      prog_param.size = hdr_size;
      break;
    }

    default:
    {
      return FLASH_PROG_FAIL;
    }
  }

  prog_param.status = FLASH_PROG_INITIALIZE;
  prog_param.reserved = img_type;

  /* Dispatch command to nand tool */
  nand_dispatch();

  return (flash_prog_status_t) prog_param.status;
}


/*
 * This function sets prog_param and calls nand_dispatch
 */
flash_prog_status_t flash_prog_init_partition_table(void * parti_data,
  uint32 length, flash_prog_parti_type_t parti_tbl_type, uint32 override_flag)
{
  flash_prog_status_t status;

  if (parti_tbl_rcvd == TRUE)
  {
    return FLASH_PROG_SUCCESS;
  }

  if (FLASH_PROG_USR_PARTI == parti_tbl_type)
  {
    /* Set the correct mode */
    prog_param.status = FLASH_PROG_USR_PARTI_TBL;

    /* Populate the partition table data */
    prog_param.data = (byte *)parti_data;
    prog_param.size   = length;
    prog_param.reserved = override_flag;

    /* Dispatch command to nand */
    nand_dispatch();

    status = (flash_prog_status_t) prog_param.status;
  }
#if defined(FEATURE_GOBI_USB_BOOT_LOADS_TO_FLASH) && defined(BUILD_BOOT_CHAIN) && !defined(BUILD_HOSTDL)
  else if (FLASH_PROG_NO_PARTI == parti_tbl_type)
  {
    /* If we are just using the partition table already stored, then just 
     * set the status to success so we will set parti_tbl_rcvd = TRUE below. */
    status = FLASH_PROG_SUCCESS;
    /* Also set the nand_core global */
    partition_rcvd = TRUE;
  }
#endif /* FEATURE_GOBI_USB_BOOT_LOADS_TO_FLASH && BUILD_BOOT_CHAIN && !BUILD_HOSTDL */
  else
  {
    /* Only user partition table can be programmed. For other types of
     * partition table indicate failure */
    status = FLASH_PROG_FAIL;
  }

  if (status == FLASH_PROG_SUCCESS)
  {
    parti_tbl_rcvd = TRUE;
  }

  return status;
}


/*
 * This function sets prog_param and calls nand_dispatch
 */
static flash_prog_status_t flash_prog_program(void * data, uint32 len, uint32 addr)
{
  /* Set the correct mode */
  prog_param.status = FLASH_PROG_PROGRAM;

  prog_param.offset     = 0x00;
  prog_param.addr       = addr;
  prog_param.size       = len;
  prog_param.data       = data;

  prog_param.reserved = ERR_OK;

  /* Dispatch command to nand */
  nand_dispatch();

  return (flash_prog_status_t) prog_param.status;
}


/*
 * This function sets prog_param and calls nand_dispatch
 */
static flash_prog_status_t flash_prog_finalize(void)
{
  /* Set the correct mode */
  prog_param.status = FLASH_PROG_FINALIZE;

  prog_param.reserved = ERR_OK;

  /* Dispatch command to nand */
  nand_dispatch();

  return (flash_prog_status_t) prog_param.status;
}

/*
 * This function sets prog_param with erase command code and
 * calls nand_dispatch
 */
static flash_prog_status_t flash_prog_erase(void)
{
  /* Set the correct mode */
  prog_param.status = FLASH_PROG_ERASE_CHIP;

  prog_param.reserved = ERR_OK;

  /* Dispatch command to nand */
  nand_dispatch();

  return (flash_prog_status_t) prog_param.status;
}

/*
 * This function sets prog_param and calls nand_dispatch
 */
static flash_prog_status_t flash_prog_set_sec_mode(uint32 sec_mode)
{
  /* Detemine if the boot is secured or not */
  prog_param.reserved = FLASH_MI_BOOT_SEC_MODE_TRUSTED;

  /* Set the correct mode */
  prog_param.status = FLASH_PROG_SECURITY_MODE;

  /* Dispatch command to nand */
  nand_dispatch();

  return (flash_prog_status_t) prog_param.status;
}


/*
 *  This function sets prog_param and calls nand_dispatch to retrieves name
 * string for the flash device.
 */
static flash_prog_status_t flash_prog_get_flash_param(void **device_params)
{
  /* Set the correct mode */
  prog_param.status = FLASH_PROG_GET_FLASH_PARAM;

  prog_param.reserved = ERR_OK;

  /* Dispatch command to nand */
  nand_dispatch();

  if (prog_param.status == FLASH_PROG_FAIL)
  {
    *device_params = (flash_param_t *)NULL;
  }
  else
  {
    *device_params = (flash_param_t *)prog_param.reserved;
  }

  return (flash_prog_status_t) prog_param.status;
}


/*
 *  This function sets prog_param and calls nand_dispatch to retrieves name
 * string for the flash device.
 */
static flash_prog_status_t flash_prog_get_error_code(fatal_code_type *errno,
  char **mesg)
{
  /* Set the correct mode */
  *errno = (fatal_code_type) prog_param.reserved;

  if ((NULL != mesg) && (*errno != ERR_OK))
  {
    /* change this when error message string is available */
    *mesg = (char *) prog_param.status;
  }

  return FLASH_PROG_SUCCESS;
}
