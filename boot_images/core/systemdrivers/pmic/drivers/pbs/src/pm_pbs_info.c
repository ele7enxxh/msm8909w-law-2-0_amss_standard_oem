/*! \file  pm_pbs_info.c
 *
 *  \brief  This file contains the pmic PBS info driver implementation.
 *  \details  This file contains the pm_pbs_info_init & pm_pbs_info_store_glb_ctxt
 *  API implementations.
 *
 *  &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/pbs/src/pm_pbs_info.c#1 $
$DateTime: 2015/09/01 00:30:35 $  $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
09/29/14     mr      Updated/Modified PBS Driver (CR-728234)
07/16/2014   akm     Comm change Updates
11/20/2013   mr      Removed KW errors for Banned Functions API (CR-512648)
11/18/2013   kt      Added test pgm rev to pbs_info struct (increment struct format)
11/01/2013   plc     Update for Arm6 syntax ("attribute")
03/19/2013   kt      Added pm_pbs_info_add_ram_sequence API.
10/19/2012   umr     PBS Core Driver.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_pbs_driver.h"
#include "pm_pbs_info.h"
#include "pm_version.h"
#include "DALGlbCtxt.h"
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* Format of the pm_pbs_info_smem_type structure */
#define PM_PBS_INFO_SMEM_FORMAT              4
/**
  @struct pm_pbs_info_smem_type
  @brief Structure for the shared memory location which is used
         to store PMIC PBS related information such as PBS Lot
         ID, ROM Version, RAM Version, Fab Id, Wafer Id, X
         coord, Y coord and Test PGM Rev. PBS ROM/RAM Revision
         id and Variant (or Branch) id are stored in last 16
         bits(upper and lower 8 bits) of rom_version and
         ram_version.
 */
typedef struct
{
  uint32         format;                           /* Starts from 1 and increments if we add more data */
  uint8          lot_id[PM_PBS_INFO_NUM_LOT_IDS];  /* PBS Lot ID */
  uint32         rom_version;                      /* PBS ROM Version number */
  uint32         ram_version;                      /* PBS RAM Version number */
  uint32         fab_id;                           /* PBS Fab ID */
  uint32         wafer_id;                         /* PBS Wafer ID */
  uint32         x_coord;                          /* PBS X Coord */
  uint32         y_coord;                          /* PBS Y Coord */
  uint32         test_pgm_rev;                     /* PBS Test PGM Rev */
  pm_pbs_info_mfgid_type  mfg_id;                  /* PBS MGF ID */
} pm_pbs_info_smem_type;

/**
  @struct pm_pbs_info_glb_ctxt_type
  @brief Global context data structure for sharing the pbs info
         across processors.
 */
typedef struct
{
  DALGLB_HEADER             dal_glb_header;
  pm_pbs_info_smem_type     pbs_info_glb_arr[PM_MAX_NUM_PMICS];
} pm_pbs_info_glb_ctxt_type;

/* Static global variables to store the pbs info */
static pm_pbs_info_smem_type pm_pbs_info_arr[PM_MAX_NUM_PMICS];

/* Flag to check if PBS Info driver is initialized */
static boolean pm_pbs_info_initialized = FALSE;

/* Pointer to the PBS_INFO_DATA Structure */
static pm_pbs_info_data_type *pm_pbs_data = NULL;


/*==========================================================================

                FUNCTION DEFINITIONS

==========================================================================*/
/**
 * @name pm_pbs_info_rom_read
 *
 * @brief This is an internal helper function for reading
 *        PBS info for PBS Peripheral with 128/256 word ROM
 *        and RAM support. This function reads the PBS
 *        ROM/RAM addresses for PMIC PBS Manufacturing IDs
 *        and foundry information such as PBS Lot ID, ROM
 *        Version, RAM Version, Fab Id, Wafer Id, X coord
 *        and Y coord. This function internally calls
 *        pm_pbs_enable/disable_access to enable/disable
 *        PBS ROM/RAM access and pm_pbs_config_access to
 *        configure READ BURST mode access to PBS ROM/RAM.
 *
 * @param[in]  slave_id. PMIC chip's slave id value.
 * @param[out] pbs_info_ptr: Variable to return to the caller with
 *             PBS info. Please refer to pm_pbs_info_smem_type structure
 *             above for more info on this structure.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          else SPMI ERROR.
 *
 * @dependencies None.
 */
static pm_err_flag_type pm_pbs_info_rom_read (uint32 slave_id, pm_pbs_info_smem_type *pbs_info_ptr)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint32           read_info_temp = 0;
  pm_register_address_type pbs_element_info_temp_addr = 0;  /* Address from where PBS Info to be read */

  uint8            lot_info_cnt = PM_PBS_INFO_NUM_LOT_IDS;
  uint8            lot_info_reg_reads_cnt = (PM_PBS_INFO_NUM_LOT_IDS/3);

  pbs_info_ptr->format = PM_PBS_INFO_SMEM_FORMAT;

  /* Configuring to enable PBS core access for ROM reads */
  err_flag = pm_pbs_enable_access(slave_id);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Configuring the base address for reading PBS ROM info.
   * PM_PBS_INFO_ADDR = PM_PBS_ROM_BASE_ADDR + 4 * [PM_PBS_ROM_SIZE – d’7];
   */
  pbs_element_info_temp_addr = PM_PBS_ROM_BASE_ADDR + 4 * (pm_pbs_data->pbs_mem_size - PM_PBS_ROM_INFO_LINE_FROM_END);
  err_flag = pm_pbs_config_access(slave_id, pbs_element_info_temp_addr, PM_PBS_ACCESS_READ_BURST);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Reading the Fab id and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
     return err_flag;
  }

  /* Masking the opcode and reserved bits from temp variable
     storing the Fab id and TEST_PGM_REV in the pbs info struct */
  pbs_info_ptr->test_pgm_rev = (uint32)(read_info_temp & 0x000000FF);
  pbs_info_ptr->fab_id = (uint32)((read_info_temp>>8) & 0x000000FF);
  read_info_temp = 0;

  /* Reading the Wafer ID, X and Y coords and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
     return err_flag;
  }

  /* Masking the opcode bits from temp variable storing the Wafer ID, X and
     Y coords along with the existing Fab ID in the pbs info struct */
  pbs_info_ptr->wafer_id = (uint32)(read_info_temp & 0x000000FF);
  pbs_info_ptr->x_coord = (uint32)((read_info_temp>>8) & 0x000000FF);
  pbs_info_ptr->y_coord = (uint32)((read_info_temp>>16) & 0x000000FF);
  read_info_temp = 0;

  /* Reading the LOT info and storing it in the pbs info struct */
  while((lot_info_reg_reads_cnt != 0) && (lot_info_cnt > 2))
  {
    /* Reading the LOT info and storing it in temp variable */
    err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }

    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)(read_info_temp & 0x000000FF);
    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)((read_info_temp >> 8) & 0x000000FF);
    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)((read_info_temp >> 16) & 0x000000FF);

    read_info_temp = 0;

    lot_info_reg_reads_cnt--;
  }

  /* Reading the ROM version info and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Store the ROM version info in the PBS Info struct */
  pbs_info_ptr->rom_version = (uint32)(read_info_temp & 0x0000FFFF);
  read_info_temp = 0;


  /* Configuring the base address for reading PBS RAM version info */
  pbs_element_info_temp_addr = PM_PBS_RAM_BASE_ADDR + 4 * (pm_pbs_data->pbs_mem_size - PM_PBS_MEMORY_VER_LINE_FROM_END);
  err_flag = pm_pbs_config_access(slave_id, pbs_element_info_temp_addr, PM_PBS_ACCESS_READ_BURST);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Reading the RAM version info and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp, 1);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Store the RAM version info in the PBS Info struct */
  pbs_info_ptr->ram_version = (uint32)(read_info_temp & 0x0000FFFF);
  read_info_temp = 0;

  /* Configuring to disable PBS core read access */
  err_flag = pm_pbs_disable_access(slave_id);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  return err_flag;
}

/**
 * @name pm_pbs_info_misc_periph_read
 *
 * @brief This is an internal helper function for reading
 *        PBS info for PBS Peripheral with 128/256 word ROM
 *        and RAM support. This function reads PMIC PBS
 *        Manufacturing IDs and foundry information such as
 *        PBS Lot ID, ROM Version, RAM Version, Fab Id,
 *        Wafer Id, X/Y coord from MISC Peripheral. This
 *        function internally calls pm_pbs_enable/disable_access
 *        to enable/disable PBS ROM/RAM access and
 *        pm_pbs_config_access to configure READ BURST mode
 *        access to PBS ROM/RAM.
 *
 * @param[in]  slave_id. PMIC chip's slave id value.
 * @param[out] pbs_info_ptr: Variable to return to the caller with
 *             PBS info. Please refer to pm_pbs_info_smem_type structure
 *             above for more info on this structure.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          else SPMI ERROR.
 *
 * @dependencies None.
 */
static pm_err_flag_type pm_pbs_info_misc_periph_read (uint32 slave_id, pm_pbs_info_smem_type *pbs_info_ptr)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8            read_info_temp8 = 0, i = 0;
  uint32           read_info_temp32 = 0;
  uint16           pbs_element_info_temp_addr = 0;  /* Address from where PBS Info to be read */

  uint8            lot_info_cnt = PM_PBS_INFO_NUM_LOT_IDS;
  uint8            lot_info_reg_reads_cnt = (PM_PBS_INFO_NUM_LOT_IDS/4);

  pbs_info_ptr->format = PM_PBS_INFO_SMEM_FORMAT;

  /* Reading the Test_PGM_Rev and storing it in temp variable */
  err_flag = pm_comm_read_byte(slave_id, pm_pbs_data->misc_base_addr + pm_pbs_data->TP_REV, &read_info_temp8, 1);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
     return err_flag;
  }
  pbs_info_ptr->test_pgm_rev = (uint32) read_info_temp8;
  read_info_temp8 = 0;

  /* Reading the Fab id, Wafer id, X/Y Co-ords and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, pm_pbs_data->misc_base_addr + pm_pbs_data->FAB_ID, 4, (uint8*)&read_info_temp32, 1);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Storing the Fab ID, Wafer ID, X and Y coords in the pbs info struct */
  pbs_info_ptr->fab_id = (uint32)(read_info_temp32 & 0x000000FF);
  pbs_info_ptr->wafer_id = (uint32)((read_info_temp32>>8) & 0x000000FF);
  pbs_info_ptr->x_coord = (uint32)((read_info_temp32>>16) & 0x000000FF);
  pbs_info_ptr->y_coord = (uint32)((read_info_temp32>>24) & 0x000000FF);
  read_info_temp32 = 0;

  /* Reading the LOT info and storing it in the PBS Info struct */
  for(i = 0; ((lot_info_reg_reads_cnt != 0) && (lot_info_cnt > 3)); i += 3, lot_info_reg_reads_cnt--)
  {
    /* Reading the LOT info and storing it in temp variable */
    err_flag = pm_comm_read_byte_array(slave_id, (pm_pbs_data->misc_base_addr + pm_pbs_data->LOT_ID) + i, 3, (uint8*)&read_info_temp32, 1);
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }
    read_info_temp32 &= 0x00FFFFFF;

    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)(read_info_temp32 & 0x3F);
    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)((read_info_temp32 >> 6) & 0x3F);
    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)((read_info_temp32 >> 12) & 0x3F);
    pbs_info_ptr->lot_id[--lot_info_cnt] = (uint8)((read_info_temp32 >> 18) & 0x3F);

    read_info_temp32 = 0;
  }

  /* Reading the MFG_ID and storing it in temp variable */
  err_flag = pm_comm_read_byte(slave_id, pm_pbs_data->misc_base_addr + pm_pbs_data->MFG_ID, &read_info_temp8, 1);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
     return err_flag;
  }
  pbs_info_ptr->mfg_id.major =  ((read_info_temp8 & 0xF0) >> 4);
  pbs_info_ptr->mfg_id.minor =  ((read_info_temp8 & 0x0E) >> 1);
  pbs_info_ptr->mfg_id.shrink = (read_info_temp8 & 0x01);
  read_info_temp8 = 0;

  /* Configuring to enable PBS core access for ROM Reads */
  err_flag = pm_pbs_enable_access(slave_id);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Configuring the base address for reading PBS ROM info */
  pbs_element_info_temp_addr = PM_PBS_ROM_BASE_ADDR + 4 * (pm_pbs_data->pbs_mem_size - PM_PBS_MEMORY_VER_LINE_FROM_END);
  err_flag = pm_pbs_config_access(slave_id, pbs_element_info_temp_addr, PM_PBS_ACCESS_READ_BURST);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Reading the RAM version info and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp32, 1);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Store the ROM Version info in the PBS Info struct */
  pbs_info_ptr->rom_version = (uint32)(read_info_temp32 & 0x0000FFFF);
  read_info_temp32 = 0;


  /* Configuring the base address for reading PBS RAM version info */
  pbs_element_info_temp_addr = PM_PBS_RAM_BASE_ADDR + 4 * (pm_pbs_data->pbs_mem_size - PM_PBS_MEMORY_VER_LINE_FROM_END);
  err_flag = pm_pbs_config_access(slave_id, pbs_element_info_temp_addr, PM_PBS_ACCESS_READ_BURST);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Reading the RAM version info and storing it in temp variable */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_info_temp32, 1);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Store the RAM Version info in the PBS Info struct */
  pbs_info_ptr->ram_version = (uint32)(read_info_temp32 & 0x0000FFFF);
  read_info_temp32 = 0;

  /* Configuring to disable PBS core read access */
  err_flag = pm_pbs_disable_access(slave_id);
  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  return err_flag;
}

/**
 * @name pm_pbs_info_init
 *
 * @brief Please refer to pm_pbs_info.h file for info regarding
 *        this function.
 */
pm_err_flag_type pm_pbs_info_init (void)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG__SUCCESS;
  uint32             slave_id = 0;
  uint8              pmic_index = 0;
  uint8              data = 0;
  pm_pbs_info_data_type **pm_pbs_data_array = NULL;

  DALSYS_memset(pm_pbs_info_arr, 0, sizeof(pm_pbs_info_arr));

  /* Retrieve place where MFG info stored */
  pm_pbs_data_array = (pm_pbs_info_data_type **)pm_target_information_get_specific_info(PM_PROP_PBS_INFO);
  if(NULL == pm_pbs_data_array)
  {
     return PM_ERR_FLAG__INVALID_POINTER;
  }

  for (pmic_index = 0; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
  {
    /* Get PMIC device primary slave id */
    err_flag = pm_get_slave_id(pmic_index, 0, &slave_id);
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      /* Second or third PMIC doesn't exist */
      err_flag = PM_ERR_FLAG__SUCCESS;
      break;
    }

    /* Read PBS Peripheral info and check if PBS peripheral exists */
    err_flag = pm_comm_read_byte(slave_id, PMIO_PBS_CORE_PERPH_TYPE_ADDR, &data, 0);
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
       return err_flag;
    }

    if (data != PM_HW_MODULE_PBS)
    {
       return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    /* Retrieve PBS info from PBS info array config data 
       CAUTION: It's expected that target has config data for all PMIC index*/
    pm_pbs_data = pm_pbs_data_array[pmic_index];
    if(NULL == pm_pbs_data)
    {
       return PM_ERR_FLAG__INVALID_POINTER;
    }

    if(PM_PBS_INFO_IN_MISC == pm_pbs_data->pbs_info_place_holder)
    {
       /* Read all the PMIC's PBS Manufacturing IDs from MISC Peripheral */
       err_flag = pm_pbs_info_misc_periph_read(slave_id, &(pm_pbs_info_arr[pmic_index]));
    }
    else
    {
       /* Read all the PMIC's PBS Manufacturing IDs from PBS ROM */
       err_flag = pm_pbs_info_rom_read(slave_id, &(pm_pbs_info_arr[pmic_index]));
    }

    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      /* Configuring to disable PBS core read access */
      pm_pbs_disable_access(slave_id);

      return err_flag;
    }
  }

  pm_pbs_info_initialized = TRUE;
  return err_flag;
}

/**
 * @name pm_pbs_info_store_glb_ctxt
 *
 * @brief Please refer to pm_pbs_info.h file for info regarding
 *        this function.
 */
pm_err_flag_type pm_pbs_info_store_glb_ctxt (void)
{
  pm_pbs_info_glb_ctxt_type*  pbs_glb_ctxt_ptr = NULL;
  uint8                       pmic_index = 0;

  /* Return error if PBS driver is not already initialized */
  if (!pm_pbs_info_initialized)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Ensure global context has been initialized */
  if(DAL_SUCCESS != DALGLBCTXT_Init())
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Allocate the context */
  if(DAL_SUCCESS != DALGLBCTXT_AllocCtxt("PM_PBS_INFO", sizeof(pm_pbs_info_glb_ctxt_type),
                                         DALGLBCTXT_LOCK_TYPE_SPINLOCK, (void **)&pbs_glb_ctxt_ptr))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  if (pbs_glb_ctxt_ptr == NULL)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  for (pmic_index = 0; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
  {
    /* Fill in the global context with PMIC's PBS info */
    DALSYS_memscpy(&(pbs_glb_ctxt_ptr->pbs_info_glb_arr[pmic_index]), sizeof(pm_pbs_info_smem_type), &(pm_pbs_info_arr[pmic_index]), sizeof(pm_pbs_info_smem_type));
  }

  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_get_pbs_info(uint8 pmic_device_index, pm_pbs_info_type* pbs_info_ptr)
{
   /* Return error if PBS driver is not already initialized or invalid arguments are passed */
  if((NULL == pbs_info_ptr) || (pmic_device_index >= PM_MAX_NUM_PMICS) || (!pm_pbs_info_initialized))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  if (pm_pbs_info_arr[pmic_device_index].format != PM_PBS_INFO_SMEM_FORMAT)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  //get the PBS info from static global variable and return it
  DALSYS_memscpy(pbs_info_ptr->lot_id, sizeof(pbs_info_ptr->lot_id), pm_pbs_info_arr[pmic_device_index].lot_id, sizeof(pbs_info_ptr->lot_id));
  pbs_info_ptr->rom_version = pm_pbs_info_arr[pmic_device_index].rom_version;
  pbs_info_ptr->ram_version = pm_pbs_info_arr[pmic_device_index].ram_version;
  pbs_info_ptr->fab_id = pm_pbs_info_arr[pmic_device_index].fab_id;
  pbs_info_ptr->wafer_id = pm_pbs_info_arr[pmic_device_index].wafer_id;
  pbs_info_ptr->x_coord = pm_pbs_info_arr[pmic_device_index].x_coord;
  pbs_info_ptr->y_coord = pm_pbs_info_arr[pmic_device_index].y_coord;
  pbs_info_ptr->test_pgm_rev = pm_pbs_info_arr[pmic_device_index].test_pgm_rev;

  pbs_info_ptr->mfg_id.major = pm_pbs_info_arr[pmic_device_index].mfg_id.major;
  pbs_info_ptr->mfg_id.minor = pm_pbs_info_arr[pmic_device_index].mfg_id.minor;
  pbs_info_ptr->mfg_id.shrink = pm_pbs_info_arr[pmic_device_index].mfg_id.shrink;

  return PM_ERR_FLAG__SUCCESS;
}
