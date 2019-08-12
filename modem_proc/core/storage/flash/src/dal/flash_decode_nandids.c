/*====================================================================
 *
 * FILE:        flash_decode_nandids.c
 *
 * SERVICES:    Functions that decode NAND manufacturer device ID bytes
 *
 * DESCRIPTION: This file implements functions specific to certain
 *              NAND manufacturers
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2008-2011, 2013, 2016 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_decode_nandids.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 04/14/16     sb      cleanup
 * 04/28/13     eo      Support variable length device id check
 * 07/27/12     sv      Remove 512 byte page support codes
 * 02/15/11     jz      Remove hard-coded address cycles and clock cycle gap
 * 06/04/10     sb      Correcting the density_mbits for Samsung 1Gbit
 * 10/22/09     jz      Put back the lost density_mbits for Toshiba 512Mbit
 * 10/05/09     bb      Bringing back DALSYS_memcpy
 * 06/26/09     rk      supported 512B spare info, device id decode
 * 07/30/09     sv      Modified to use DALSYS_memcpy for memcpy
 * 07/29/09     jz      Added Toshiba 512Mb support
 * 04/14/09     mh      Add check for 4K page for toshiba parts
 * 04/01/09     mh      Add check for 4K page
 * 02/07/09     bb      Fix warnings.
 * 12/17/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALSys.h"

#include "flash_nand.h"
#include "flash_nand_config.h"

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/
/* Device Names in ASCII string format */

#define FLASH_DEV_NAME_SAMSUNG "SAMSUNG"
#define FLASH_DEV_NAME_TOSHIBA "TOSHIBA"


/*-----------------------------------------------------------------------
 *           Type Declarations
 *----------------------------------------------------------------------*/

/* Spare Bytes info for 2K Default Device */
static struct flash_spare_info flash_spare_info =
{
  { {0, 2048, 2},    /* Bad Block Info - Page 0 */
    {1, 2048, 2}     /* Bad Block Info - Page 1 */
  },
  16, 0, 16,         /* udata max, udata ecc max, udata corrected bytes */
  NULL,              /* Spare udata encode APIs */
  NULL               /* Spare udata decode APIs */
};

/* Toshiba 5 NAND ID bytes decoder */
struct nand_toshiba_ids
{
  uint8 mid;
  uint8 did;

  uint8 icn:2;         /* internal chip number */
  uint8 bpc:2;         /* cell type 2/4/8/16LC */
  uint8 rsvd0:4;       /* reserved */

  uint8 page_size:2;   /* page size(w/o spare) 1/2/4/8KB */
  uint8 spare_size:2;  /* spare size bytes / 512B : 8/16/27 */
  uint8 block_size:2;  /* block size(w/o spare) 64/128/256/512KB */
  uint8 org:1;         /* organization: x8/x16 */
  uint8 rsvd1:1;       /* reserved */

  uint8 rsvd2:8;       /* reserved */
};

/*
 * Decodes the ID bytes from a Toshiba device and determine
 * the device configuration.
 */
static int nand_decode_toshiba(flash_client_ctxt *client_ctxt, struct
  flash_nand_params **device_params, uint8 *ids)
{
  int status = FLASH_DEVICE_FAIL;
  int idx;
  uint32 block_size_kbytes, page_size_kbytes, density_mbits = 0;
  struct flash_nand_params *param;
  struct nand_toshiba_ids *toshiba_ids = (struct nand_toshiba_ids *) ids;
  unsigned int slc_small_device = FALSE;
  enum flash_hal_dev_width slc_small_device_width = FLASH_HAL_DEV_WIDTH_UNKNOWN;
  status = DALSYS_Malloc(sizeof(struct flash_nand_params),
    (void **)device_params);

  if (DAL_SUCCESS == status)
  {
    switch(toshiba_ids->did)
    {
      case 0x36: /*SLC Small device. */
        slc_small_device = TRUE;
        slc_small_device_width = FLASH_HAL_8_BIT_WIDTH;
        density_mbits = 512;
        break;
      case 0x46: /*SLC Small device. */
        slc_small_device = TRUE;
        slc_small_device_width = FLASH_HAL_16_BIT_WIDTH;
        density_mbits = 512;
        break;
      case 0x79: /*SLC Small device. */
        slc_small_device = TRUE;
        slc_small_device_width = FLASH_HAL_8_BIT_WIDTH;
        density_mbits = 1024; break;
      case 0xA0: case 0xB0: case 0xD0: case 0xC0: /* 512 Mbit */
        density_mbits = 512;
        break;
      case 0xA1: case 0xB1: case 0xD1: case 0xC1: /* 1 Gbit */
        density_mbits = 1024;
        break;
      case 0xAA: case 0xBA: case 0xDA: case 0xCA: /* 2 Gbit */
        density_mbits = 2048;
        break;
      case 0xAC: case 0xBC: case 0xDC: case 0xCC: /* 4 Gbit */
        density_mbits = 4096;
        break;
      case 0xA3: case 0xB3: case 0xD3: case 0xC3: /* 8 Gbit */
        density_mbits = 8192;
        break;
      case 0xA5: case 0xB5: case 0xD5: case 0xC5: /* 16 Gbit */
        density_mbits = 16384;
        break;
      default: break;
    }

    if (density_mbits == 0)
    {
      FLASHLOG(3, ("\nUnable to decode Device ID density\n"));
      status = FLASH_DEVICE_FAIL;
      DALSYS_Free(*device_params);
    }
    else
    {
      param = *device_params;

      idx = 4;
      while (idx--)
      {
        param->dev_ids[idx] = ids[idx];
      }
      param->dev_ids_num_bytes = 2;

      DALSYS_memcpy(param->device_name, FLASH_DEV_NAME_TOSHIBA, sizeof(FLASH_DEV_NAME_TOSHIBA));
      param->device_name[sizeof(FLASH_DEV_NAME_TOSHIBA)] = 0x0;
      param->max_wb_ns = 200;   /* max tWB/tRB from datasheet */
      param->op_status_mask = 0xFF;
      param->op_status_value = 0x1;

      if (TRUE == slc_small_device)
      {
        param->page_size = 512;
        param->feature_flags1.ecc = NAND_2BIT_HW_ECC;
        block_size_kbytes = 16;
        param->pages_per_block = 32;
        param->spare_size = 16;
        param->otp_sequence_cfg = FLASH_NAND_OTP_SEQUENCE_CFG6;
        param->dev_width = slc_small_device_width;
      }
      else
      {
        param->dev_width = (toshiba_ids->org == 0) ? FLASH_HAL_8_BIT_WIDTH :
          FLASH_HAL_16_BIT_WIDTH;

        switch (toshiba_ids->spare_size)
        {
          case 0:
          case 1:
            param->feature_flags1.ecc = NAND_1BIT_HW_ECC;
            break;
          case 2:
            param->feature_flags1.ecc = NAND_8BIT_HW_ECC;
            break;
          default:
            param->feature_flags1.ecc = NAND_NO_HW_ECC;
            break;
        }

        param->page_size = 1024 << toshiba_ids->page_size;

        page_size_kbytes = param->page_size >> 10;

        /* uint8 block_size:2 for 64/128/256/512KB */
        block_size_kbytes = 64 << toshiba_ids->block_size;

        param->pages_per_block = block_size_kbytes / page_size_kbytes;

        param->spare_size = (8 << toshiba_ids->spare_size) *
          (param->page_size / 512);

        /* Set the OTP sequence */
        param->otp_sequence_cfg =
          ((param->page_size == 2048) ||  (param->page_size == 4096))?
          FLASH_NAND_OTP_SEQUENCE_CFG2 : FLASH_NAND_OTP_SEQUENCE_UNKNOWN;

      }

      /* Divide density by block size to calculate block count,
       * (1024/8 * density_mbits) to convert to kbytes */
      param->block_count = (1024 / 8 * density_mbits) / block_size_kbytes;
      
      status = FLASH_DEVICE_DONE;

      if ((param->page_size == 2048) && (param->feature_flags1.ecc))
      {
        flash_spare_info.bad_block_info[0].byte_offset = 2048;
        flash_spare_info.udata_max = 16;
        flash_spare_info.max_corrected_udata_bytes = 16;
        param->dev_spare_info = &flash_spare_info;
        param->dev_spare_info->bad_block_info[0].byte_length =
        (param->dev_width == FLASH_HAL_8_BIT_WIDTH) ? 1 : 2;
      }
      else if ((param->page_size == 4096) && (param->feature_flags1.ecc))
      {
        flash_spare_info.bad_block_info[0].byte_offset = 4096;
        flash_spare_info.udata_max = 32;
        flash_spare_info.max_corrected_udata_bytes = 32;
        param->dev_spare_info = &flash_spare_info;

        param->dev_spare_info->bad_block_info[0].byte_length =
          (param->dev_width == FLASH_HAL_8_BIT_WIDTH) ? 1 : 2;
      }
      else
      {
        status = FLASH_DEVICE_FAIL;
        DALSYS_Free(*device_params);
      }
    }
  }

  return status;
}

/*
 * Decodes the ID bytes from a device
 */
int nand_decode_ids(flash_client_ctxt *client_ctxt, struct
  flash_nand_params **device_params, uint8 *ids)
{
  int status = FLASH_DEVICE_FAIL;

  switch (ids[0])
  {
    case 0x98:
      status = nand_decode_toshiba(client_ctxt, device_params, ids);
      break;
    default:
      break;
  }

  return status;
}
