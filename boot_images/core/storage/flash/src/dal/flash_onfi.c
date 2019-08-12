/*====================================================================
 *
 * FILE:        flash_onfi.c
 *
 * SERVICES:    Functions specific to ONFI device detection
 *
 * DESCRIPTION: This file implements functions specific to ONFI based
 *              device driver.
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2008-2012, 2015 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_onfi.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/30/15     sb      Read next redundant param page If the first 4 bytes got corrupted
 * 09/28/12     sv      Invalidate cache after onfi read id / read param page
 * 05/14/11     sb      Fix for onfi id read issue when dcache is enabled in OEMSBL
 * 05/18/11     bb      Flash driver Optimization
 * 05/18/11     jz      Remove hard-coded address cycles and clock cycle gap
 * 05/18/11     jz      hal_id cleanup
 * 01/13/11     sb      Adding 4K NAND spare info
 * 08/03/10     bn      Klocwork warning fix, and included comdef.h
 * 07/16/10     bb      Fix compiler warnings for WM7
 * 05/20/10     sv      Update ONFI probe to read more redundant param pages
 * 09/23/09     mh      Add support for multiple hal_ids per client
 * 07/21/09     sv      Modified to use DALSYS api for memcpy
 * 06/19/09     jm      Fix Compilation issue related to packed structures
 * 05/29/09     mh      Change nand_read_custom_ids signature to use hal_id
 * 04/07/09     jz      Use PACKED_POST for GNU C compiler
 * 03/12/09     bb      Set otp_sequence_config to unknown for default case
 * 02/19/09     mh      Clean-up, fix prefast warning
 * 01/30/09     bb      Fixed issues with ONFI probe
 * 11/25/08     mm      Cleanup
 * 04/30/08     eo      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "flash_nand_entry.h"
#include "flash_nand.h"
#include "flash_nand_config.h"
#include "comdef.h"

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/
/* ID command from ONFI */
#define ONFI_READ_ID_CMD                      0x90
#define ONFI_READ_ID_ADDR                     0x20
#define ONFI_READ_ID_NUM_BYTES                0x4

/* ONFI Parameter read page command */
#define ONFI_PARAMETER_PAGE_SIZE              0x100
#define ONFI_READ_PARAMETER_PAGE_CMD          0xEC
#define ONFI_READ_PARAMETER_PAGE_ADDR         0x00
/* Num ONFI parameter data bytes read in a single custom read id call */
#define ONFI_READ_PARAMETER_PAGE_NUM_BYTES    0x200
/* Maximum number of parameter page reads to be done during an ONFI probe.
 * We will read until this max number of parameter pages configured
 * or until a parameter page read has no CRC errors or until the max
 * parameter pages supported by the device - whichever occurs first
 */
#define ONFI_READ_PARAMETER_PAGE_MAX          32

/* NAND entry init data */
extern struct nand_entry_data nand_entry_init_data;

/*-----------------------------------------------------------------------
 *           Type Declarations
 *----------------------------------------------------------------------*/
#ifndef PACKED_POST
  /* empty define */
  #define PACKED_POST
  #define PACKED
#endif

#if defined(FEATURE_WINCE)
  #pragma pack(push,1)
#endif

/* Fields of ONFI parameter page layout as defined by ONFI spec 1.0.
 * Please refer to ONFI spec rev 1.0 for detailed description of
 * these fields */
typedef PACKED struct PACKED_POST
{
  uint8  signature[4];
  uint16 revision;
  PACKED struct PACKED_POST feature_tags
  {
    uint16 bus_x16:1;
    uint16 multiple_LUN_operations:1;
    uint16 nonsequential_programming:1;
    uint16 interleaved_operations:1;
    uint16 odd_to_even_copyback:1;
    uint16 reserved:11;
  } features;
  PACKED struct PACKED_POST optional_cmds_tag
  {
    uint16 page_cache_program:1;
    uint16 read_cache:1;
    uint16 get_set_features:1;
    uint16 read_status_enh:1;
    uint16 copyback:1;
    uint16 read_unique_id:1;
    uint16 reserved:10;
  } optional_cmds_supported;
  uint8  reserved0[22];
  uint8  manufacturer[12];
  uint8  model[20];
  uint8  manufacturer_id;
  uint16 date_code;
  uint8  reserved1[13];
  uint32 num_data_bytes_per_page;
  uint16 num_spare_bytes_per_page;
  uint32 num_data_bytes_per_partial_page;
  uint16 num_spare_bytes_per_partial_page;
  uint32 num_pages_per_block;
  uint32 num_blocks_per_unit;
  uint8  num_units;
  uint8  num_addr_cycles;
  uint8  num_bits_per_cell;
  uint16 max_bad_blocks_per_unit;
  uint16 block_endurance;
  uint8  valid_begin_blocks;
  uint16 valid_begin_blocks_endurance;
  uint8  num_programs_per_page;
  uint8  partial_program_attributes;
  uint8  num_ecc_bits;
  uint8  num_interleaved_addr_bits;
  uint8  interleaved_operation_attributes;
  uint8  reserved2[13];
  uint8  io_pin_capacitance;
  uint16 timing_mode;
  uint16 program_cache_timing_mode;
  uint16 max_tprog;
  uint16 max_tbers;
  uint16 max_tr;
  uint16 max_tccs;
  uint8  reserved3[23];
  uint16 vendor_specific_rev;
  uint8  vendor_specific[88];
  uint16 integrity_crc;
} onfi_parameters;

#if defined(FEATURE_WINCE)
  #pragma pack(pop)
#endif

static byte *onfi_parameter_page = NULL;
static onfi_parameters *static_onfi_parameters = NULL;

/* Spare Bytes info for 2K ONFI Device */
static struct flash_spare_info flash_spare_info_onfi_2048 =
{
  { {0, 2048, 2},     /* Bad Block Info - Page 0 */
    {1, 2048, 2}      /* Bad Block Info - Page 1 */
  },
  16, 0, 16,          /* udata max, udata ecc max, udata corrected bytes */
  NULL,               /* Spare udata encode APIs */
  NULL                /* Spare udata decode APIs */
};

/* Spare Bytes info for 4K x8 Device */
static struct flash_spare_info flash_spare_info_onfi_4096 =
{
  { {0, 4096, 1},     /* Bad Block Info - Page 0 */
    {1, 4096, 1}      /* Bad Block Info - Page 1 */
  },
  32, 0, 32,          /* udata max, udata ecc max, udata corrected bytes */
  NULL,               /* Spare udata encode APIs */
  NULL                /* Spare udata decode APIs */
};

/*
 * Genrate ONFI CRC as per ONFI 1.0 spec. This code has been taken directly
 * from spec's appendix A.  (SAMPLE CODE FOR CRC-16 (INFORMATIVE))
 */
uint16 gen_crc(byte* buffer, uint16 cnt)
{
  /* Bit by bit algorithm without augmented zero bytes */
  const uint16 crcinit = 0x4F4E; /* Initial CRC value in shift register */
  const int order = 16;          /* Order of the CRC-16 */
  const uint16 polynom = 0x8005; /* Polynomial */
  unsigned long j, c, bit, crcmask, crchighbit;
  uint16 crc = crcinit;  /* Initialize the shift register with 0x4F4E */
  unsigned long data_in;

  crcmask = ((((unsigned long) 1 << (order - 1)) - 1) << 1) | 1;
  crchighbit = (unsigned long) 1 << (order - 1);

  while (cnt--)
  {
    data_in=(unsigned long)*(buffer++);
    c = (unsigned long)data_in;
    for( j = 0x80; j; j >>= 1)
    {
      bit = crc & crchighbit;
      crc <<= 1;
      if (c & j) bit ^= crchighbit;
      if (bit) crc ^= polynom;
    }

    crc &= crcmask;
  }

  return crc;
}

/*
 * Issues the id_cmd followed by the id_addr and reads num_bytes of data
 * directly from the device. This can be used to read multiple information
 * from the NAND or NAND like devices that are not read via the NAND
 * controller ID command.
 */
static int nand_read_custom_ids(flash_client_ctxt *client_ctxt, uint32 id_cmd,
   uint32 id_addr, void *buffer, uint32 num_bytes)
{
  uint32 retry_count = 8, hal_id = 0;
  uint32 col_addr = 0, row_addr = 0;
  static uint32 num_addr = 1;
  int status = FLASH_DEVICE_DONE;
  struct flash_hal_buffer_desc *buff_desc;
  struct flash_hal_predef_op_list *op_list;
  struct flash_hal_predef_onfi_data onfi_data;
  enum flash_hal_status hal_status = FLASH_HAL_SUCCESS;
  struct nand_dev_pdata *nand_pdata = GET_PDATA(client_ctxt);

  hal_id = client_ctxt->client_data.hal_id;

  /* Disable ECC */
  flash_hal_set_ecc_state(hal_id, FLASH_HAL_HW_ECC_DISABLE);

  col_addr = id_addr;
  row_addr = 0;

  buff_desc = &onfi_data.buff_desc;

  buff_desc->buff_vaddr = (uint32) nand_pdata->raw_page_buffer;
  buff_desc->buff_paddr = flash_vtop_get_physical(buff_desc->buff_vaddr);
  buff_desc->buff_size = num_bytes;
  buff_desc->num_pages = 0;
  onfi_data.col_offset = col_addr;

  flash_dcache_inval(buff_desc->buff_vaddr, buff_desc->buff_size);
	
retry:

  onfi_data.page = row_addr;
  onfi_data.addr_cycles = num_addr;

  if (id_cmd == ONFI_READ_ID_CMD)
  {
    flash_hal_read_onfi_id(hal_id, &onfi_data, &op_list, &hal_status);
  }
  else
  {
    flash_hal_read_param_page(hal_id, &onfi_data, &op_list, &hal_status);
  }

  flash_dcache_inval(buff_desc->buff_vaddr, buff_desc->buff_size);
    
  if (FLASH_HAL_SUCCESS != hal_status)
  {
    if (FLASH_HAL_FAIL_MPU_ERR == hal_status)
    {
      /* Flip bits of address bytes 4 and 5 to see if any MPU
       * errors can be avoided and the device can be detected */
      num_addr = 5;
      if (retry_count)
      {
        switch (retry_count)
        {
          case 8: row_addr = 0x1000; break;
          case 4: row_addr = 0x10000; break;
          default: row_addr <<= 1; break;
        }
        --retry_count;
        goto retry;
      }
      else
      {
        FLASHLOG (3, ("Reading Custom IDs Failed after max retries\n"));
        status = FLASH_DEVICE_FAIL;
      }
    }
    else
    {
      status = FLASH_DEVICE_FAIL;
    }
  }
  else
  {
    DALSYS_memcpy(buffer, nand_pdata->raw_page_buffer, num_bytes);
  }

  return status;
}

/*
 * For ONFI compliant devices, read the parameter page and perform a CRC
 * check on it.  And then configure the device specific table entries
 * dynamically.
 */
int nand_onfi_probe (flash_client_ctxt *client_ctxt,
  struct flash_nand_params **device_params, DALBOOL *is_onfi)
{
  int    idx, status = FLASH_DEVICE_FAIL;
  uint32 onfi_id;
  struct flash_nand_params *param;
  uint32 param_pages_read = 0;
  uint32 param_pages_per_read = 0;
  uint32 parameter_count = 0;
  uint8  onfi_probe_done = FALSE;

  *is_onfi = 0;

  status = nand_read_custom_ids(client_ctxt,
    ONFI_READ_ID_CMD, ONFI_READ_ID_ADDR, (void *)&onfi_id,
    ONFI_READ_ID_NUM_BYTES);

  /* Check if it is an ONFI compatibile device. */
  if ((FLASH_DEVICE_DONE == status) && (0x49464E4F == onfi_id))
  {
    status = DALSYS_Malloc(sizeof(struct flash_nand_params),
      (void **)device_params);

    if (DAL_SUCCESS == status)
    {
      status = DALSYS_Malloc(ONFI_READ_PARAMETER_PAGE_NUM_BYTES, (void **)
        &onfi_parameter_page);

      if (DAL_SUCCESS != status)
      {
        DALSYS_Free((void *)(*device_params));
      }
    }

    if (DAL_SUCCESS == status)
    {
      /* We have detected an ONFI compatible device. Read its Parameter
         Page(s). We will read until the max number of parameter pages
         configured or until a parameter page has no CRC errors or until
         the max parameter pages supported by the device - whichever
         occurs first */
      param = *device_params;
      *is_onfi = 1;
      param_pages_per_read = (ONFI_READ_PARAMETER_PAGE_NUM_BYTES /
                                    ONFI_PARAMETER_PAGE_SIZE);

      while((param_pages_read < ONFI_READ_PARAMETER_PAGE_MAX) &&
        ((onfi_probe_done == FALSE)))
      {
        status = nand_read_custom_ids(client_ctxt,
          ONFI_READ_PARAMETER_PAGE_CMD,
          ONFI_READ_PARAMETER_PAGE_ADDR + param_pages_read,
          (void *)onfi_parameter_page,
          ONFI_READ_PARAMETER_PAGE_NUM_BYTES);

        if (FLASH_DEVICE_DONE == status)
        {
          uint16 computed_crc;
          byte   *parameter_page = onfi_parameter_page;

          status = FLASH_DEVICE_FAIL;
          parameter_count = param_pages_per_read;

          do
          {
            static_onfi_parameters = (onfi_parameters *)parameter_page;

            computed_crc = gen_crc(parameter_page,
              (sizeof(onfi_parameters) - 2));

            if (computed_crc == static_onfi_parameters->integrity_crc)
            {
              for (idx = 0; (idx < sizeof(static_onfi_parameters->model) &&
                (idx < FLASH_MAX_DEVICE_NAME)); idx++)
              {
                param->device_name[idx] = *(static_onfi_parameters->model + idx);
              }

              /* Null terminate the the character array. Last couple of
               * charcters are ascii blanks (0x20) which can be excluded */
              param->device_name[sizeof(static_onfi_parameters->model) - 1] =
                0x0;

              param->block_count = static_onfi_parameters->num_blocks_per_unit *
                static_onfi_parameters->num_units;

              param->dev_ids[0] = static_onfi_parameters->manufacturer_id;
              param->dev_ids[1] = 0;

              param->op_status_mask = 0xFF;
              param->op_status_value = 0x1;

              param->dev_width = ((static_onfi_parameters->features.bus_x16 ==
                0) ? FLASH_HAL_8_BIT_WIDTH : FLASH_HAL_16_BIT_WIDTH);

              param->feature_flags1.mcopy = (
                (static_onfi_parameters->num_data_bytes_per_page == 512) ?
                FLASH_DEFAULT_READ_WRITE_CP :
                ((static_onfi_parameters->optional_cmds_supported.copyback &&
                static_onfi_parameters->features.odd_to_even_copyback) ?
                NAND_0x00_0x35_0x85_0x10_ODD_EVEN_CP :
                NAND_0x00_0x35_0x85_0x10_ODD_ODD_CP));

              param->feature_flags1.mreads = (
                static_onfi_parameters->optional_cmds_supported.read_cache) ?
              NAND_0x00_0x30_0x31n_0x3F_READ_MPAGES :
              FLASH_DEFAULT_READ_MPAGES;

              param->feature_flags1.mwrites = (
                static_onfi_parameters->optional_cmds_supported.page_cache_program) ?
                NAND_0x80_0x15_0x80_0x10_WRITE_MPAGES :
                FLASH_DEFAULT_WRITE_MPAGES;

              param->feature_flags1.ecc = (enum nand_ecc_requirement)
                static_onfi_parameters->num_ecc_bits;

              param->pages_per_block =
                static_onfi_parameters->num_pages_per_block;

              param->page_size = static_onfi_parameters->num_data_bytes_per_page;

              param->spare_size =
                static_onfi_parameters->num_spare_bytes_per_page;

              param->dev_spare_info = (param->page_size == 2048)?
                &flash_spare_info_onfi_2048 : &flash_spare_info_onfi_4096;

              param->dev_spare_info->bad_block_info[0].byte_length =
                (param->dev_width == FLASH_HAL_8_BIT_WIDTH) ? 1 : 2;
              param->dev_spare_info->bad_block_info[1].byte_length =
                param->dev_spare_info->bad_block_info[0].byte_length;

              if (static_onfi_parameters->optional_cmds_supported.read_unique_id)
              {
                param->otp_sequence_cfg = FLASH_NAND_OTP_SEQUENCE_CFG5;
              }
              else if (static_onfi_parameters->manufacturer_id == 0x2C)
              {
                param->otp_sequence_cfg = FLASH_NAND_OTP_SEQUENCE_CFG3;
              }
              else
              {
                param->otp_sequence_cfg = FLASH_NAND_OTP_SEQUENCE_UNKNOWN;
              }

              param->max_wb_ns = 100;  /* max tWB from datasheet */

              status = FLASH_DEVICE_DONE;
              onfi_probe_done = TRUE;
            }
            else
            {
              FLASHLOG(3,("CRC Failed on parameter data set %d\n",
                (param_pages_read + param_pages_per_read - parameter_count) + 1));
              --parameter_count;
              parameter_page += ONFI_PARAMETER_PAGE_SIZE;
            } /* if (computed_crc == ..._parameters->integrity_crc) */
          } while ((onfi_probe_done == FALSE) && (parameter_count != 0));
        } /* if (FLASH_DEVICE_DONE == status) */

        param_pages_read += param_pages_per_read;
      } /*  while((param_pages_read < ONFI_READ_PARAMETER_PAGE_MAX) */
      if ((status == FLASH_DEVICE_FAIL))
      {
        FLASHLOG(3, ("ONFI CRCs Mismatched in all parameter pages!\n"));
        DALSYS_Free(*device_params);
      }
      DALSYS_Free(onfi_parameter_page);
    } /* end of if (DAL_SUCCESS == status) */
  } /* if ((FLASH_DEVICE_DONE == status) && (0x49464E4F == *onfi_id)) */

  return status;
}
