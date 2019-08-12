/**
* @file mba_nand.c
* @brief Modem Self Authentication nand implementation
*
* This file implements the API's for EFS2 nand partition protection.
*
*/
/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/src/mba_nand/mba_nand.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $


when       who      what, where, why
--------   ---      ------------------------------------
03/25/2013 dm       Initial version
===========================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * --------------------------------------------------------------------------*/
#include <comdef.h>
#include <string.h>
#include <HALhwio.h>
#include <math.h>
#include "HALxpu2.h"
#include "mba_error_handler.h"
#include "mba_utils.h"
#include "flash_miparti.h"
#include "flash.h"
#include "mba_nand.h"
#include "mba_hwio.h"
#include "msmhwiobase.h"
#include "mba_nand_clocks.h"


/*----------------------------------------------------------------------------
 * Global Variable Declaration & Defintions 
 * --------------------------------------------------------------------------*/
/*
const nand_part_group partition_groups[] = {
  {"0:MIBIB",      PART_MODEM},
  {"0:SBL2",       PART_MODEM},
  {"0:RPM",        PART_MODEM},
  {"0:EFS2",       PART_EFS},
  {"0:DSP1",       PART_OVERLAP},
  {"0:DSP3",       PART_OVERLAP},
  {"0:DSP2",       PART_OVERLAP},
  {"0:APPSBL",     PART_APPS},
  {"0:APPS",       PART_APPS},
  {"0:SYSTEM",     PART_APPS},
  {"0:USERDATA",   PART_APPS},
  {"0:CACHE",      PART_APPS},
  {"0:MISC",       PART_APPS},
  {"0:RECOVERY",   PART_APPS},
  {"0:FOTA",       PART_APPS},
  {"0:RECOVERYFS", PART_APPS},
  {NULL,           PART_NONE}
}; 
*/
extern struct nand_parti_table nand_partition_table;
static nand_part_details_t nand_efs_part;
#define TARGET_FAMILY 0xFFFF0000
/*----------------------------------------------------------------------------
 * Static Function Defintions 
 * -------------------------------------------------------------------------*/

static double mba_log2(double x)
{
   return log(x) / log(2.0);
}

static uint32 mba_getPageSize(void) 
{
  return nand_efs_part.page_size_bytes;
}

static uint32 mba_getSpareBytes(void) 
{
  return 0;
}

static uint32 mba_getPageInBlock(void) 
{
  return nand_efs_part.pages_per_block;
}

static uint64 addrToFlat(mba_nand_addr_t addr)
{
   uint64 _addr;
   const uint32 column_bits = (uint32)ceil(mba_log2(mba_getPageSize()+mba_getSpareBytes()));
   
   _addr = addr.blockNum * mba_getPageInBlock();
   _addr |= addr.pageNum;
   _addr <<= 8 * (uint32)ceil(column_bits/8.0);
   _addr |= addr.columnNum;

   return _addr;
}

mba_xpu_addr_t mba_xpu_addrTrans(mba_nand_addr_t addr)
{
   mba_xpu_addr_t xpu;
   uint64 _addr;

   _addr = addrToFlat(addr);
   xpu.upper_addr = (uint32)((_addr & 0x000000FF00000000ULL) >> 32);
   xpu.lower_addr = (uint32) (_addr & 0x00000000FFFF0000ULL);

   return xpu;
}



/*=============================================================================
FUNCTION  FLASH_FIND_PARTITION_DETAILS

DESCRIPTION
  This function extracts the efs partition details from partition table.

ARGUMENTS
  None

RETURN VALUE
  E_SUCCESS - when all the paritition details are filled
  E_FAILURE - failure case
  updates global nand_efs_part variable with the efs2 partition details.

=============================================================================*/
static int flash_find_partition_details (void)
{
  struct flash_partition_table const *flash_partable = NULL; 
  struct flash_info fs_dev_info = {0};
  flash_handle_t handle_flash = NULL;
  int num_parts;
  uint32 ret_value = E_SUCCESS;

  do
  {
  DALSYS_InitMod(NULL);
  
  /* Get the Flash Device Info */
  if(DAL_SUCCESS != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1, /*Device Id*/
    &handle_flash /*Handle*/)) 
  {
    ret_value =  -E_FAILURE;
	return ret_value;
  }

  if(DAL_SUCCESS!= flash_open_partition(handle_flash, (uint8*)"0:ALL"))
  {
    ret_value =  -E_FAILURE;
	return ret_value;
  }

  if (DAL_SUCCESS != flash_get_info(handle_flash, FLASH_DEVICE_INFO, &fs_dev_info))
  {
    ret_value =  -E_FAILURE;
	break;
  }

  nand_efs_part.pages_per_block = fs_dev_info.partition.pages_per_block;
  nand_efs_part.page_size_bytes = fs_dev_info.partition.page_size_bytes;

  /* get the partition table entry */
  if(!nand_partition_table.parti_table_size_bytes || !nand_partition_table.parti_table)
  {
    ret_value =  -E_FAILURE;
	break;
  }
  
  if(NULL == (flash_partable = (flash_partable_t)nand_partition_table.parti_table))
  {
    ret_value =  -E_FAILURE;
	break;
  }

  nand_efs_part.efs_start = 0;
  nand_efs_part.efs_end = 0;

  /* Search partition table for EFS2 partition */
  for (num_parts = 0; num_parts < flash_partable->numparts; num_parts++)
  {
    uint32 part_start = flash_partable->part_entry[num_parts].offset;
    uint32 part_end = part_start + flash_partable->part_entry[num_parts].length;

    /*check for integer overflow */
    if (part_end < part_start )
    {
      return -E_FAILURE;
    }

    /* Compare Partition name with EFS */
    if(!strncmp(flash_partable->part_entry[num_parts].name,
                 EFS2,FLASH_PART_NAME_LENGTH))
    {
      nand_efs_part.efs_start = part_start;
      nand_efs_part.efs_end = part_end;
      break; /* Break The loop EFS is found. */
    }
  }

  /* make sure EFS partitions found */
  if (nand_efs_part.efs_end == 0)
  {
    ret_value =  -E_FAILURE;
	break;
  }

  }while(0);

 flash_device_close(handle_flash);
 flash_device_detach(handle_flash);

  return ret_value;
}

/*=============================================================================

FUNCTION  MBA_CONFIGURE_QPIC_NAND

DESCRIPTION
  This function is used by MBA to configure the NAND controller XPUs

ARGUMENTS
  None

RETURN VALUE
  status :  E_SUCCESS
            E_FAILURE

=============================================================================*/

int mba_configure_qpic_nand(void)
{
#ifdef FEATURE_NO_XPU
uint8 xpu_enabled = FALSE;
#else
uint8 xpu_enabled = TRUE;
#endif
  uint32 soc_hw_version = 0x0;
  uint32 soc_hw_family = 0x0;
  uint32 boot_config = 0x0;
  HAL_xpu2_ResourceGroupConfigType *msa_res_list_ptr;
  HAL_xpu2_ResourceGroupConfigType msa_res_list_qpic[2];
  mba_nand_addr_t addr;
  mba_xpu_addr_t xpu_start_addr,xpu_end_addr;
  uint32 num_res_groups = 2;
  uint32 ret_value = E_SUCCESS;

  soc_hw_version = HWIO_TCSR_SOC_HW_VERSION_IN; 
  soc_hw_family = soc_hw_version & TARGET_FAMILY ; 
  boot_config = HWIO_INF(BOOT_CONFIG, FAST_BOOT);

  if( !(soc_hw_family == 0x20030000 && boot_config == 0x4))
  {
  	return E_SUCCESS;
  }

  mba_flash_clocks_init();
  mba_flash_clocks_request();

  do
  {

	  msa_res_list_ptr = &msa_res_list_qpic[0];
	  memset(msa_res_list_qpic, 0, sizeof(msa_res_list_qpic)); 

	  if(E_SUCCESS != flash_find_partition_details())
	  {
		ret_value =  -E_FAILURE;
		break;
	  }

	  /* Get the address range from partition details */    
	  addr.blockNum = nand_efs_part.efs_start;
	  addr.columnNum = 0;
	  addr.pageNum = 0;
	  xpu_start_addr = mba_xpu_addrTrans(addr);
	  
	  addr.blockNum = nand_efs_part.efs_end;
	  addr.columnNum = 0;
	  addr.pageNum = 0;
	  xpu_end_addr = mba_xpu_addrTrans(addr);
	  
	  HWIO_QPIC_QPIC_VMIDMT_MSDR0_OUT(0x38);

          if (xpu_enabled)
          {
	  /* Check for XPU2 resource */
	  if(HAL_XPU2_NO_ERROR != HAL_xpu2_GetMSAResourceGroupConfigInfo(HAL_XPU2_QPIC_NAND, 
		  (HAL_xpu2_ResourceGroupConfigType **)&msa_res_list_ptr, &num_res_groups))
	  {
		ret_value =  -E_FAILURE;
		break;
	  }
          }

	  /* Protect EFS2 Partition by XPU. */  
	  msa_res_list_ptr[0].resAddr.uStartLower = xpu_start_addr.lower_addr;
	  msa_res_list_ptr[0].resAddr.uStartUpper = 0x0;
	  msa_res_list_ptr[0].resAddr.uEndLower = xpu_end_addr.lower_addr;
	  msa_res_list_ptr[0].resAddr.uEndUpper = 0x0;
          if (xpu_enabled)
          {
	  if(HAL_XPU2_NO_ERROR != 
		 HAL_xpu2_ConfigMSAResourceGroup( HAL_XPU2_QPIC_NAND, &msa_res_list_ptr[0] ))
	  {
		ret_value =  -E_FAILURE;
		break;
	  }
          }
  }while(0);

  mba_flash_clocks_request_complete();
  mba_flash_clocks_deinit();

  return ret_value;
}

