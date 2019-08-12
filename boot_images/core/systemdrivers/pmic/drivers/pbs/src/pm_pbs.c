/*! \file  pm_pbs.c 
 *  
 *  \brief  This file contains the pmic PBS driver implementation
 *  \details  This file contains exposed APIs that can be used to configure PBS.
 *  
 *  &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/pbs/src/pm_pbs.c#1 $ 
$DateTime: 2015/09/01 00:30:35 $  $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
07/16/14     akm     Comm change Updates
03/19/2013   kt      Added pm_pbs_add_ram_sequence API.
10/19/2012   umr     PBS Core Driver.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pbs.h"
#include "pm_pbs_driver.h"
#include "pm_comm.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/* Overall maximum number of sequences per trigger
   independent of trigger type */
#define PM_PBS_MAX_NUM_SEQS                  33

/* Warm reset RAM start address */
#define PM_PBS_WARM_RESET_RAM_START_ADDR     0x066C

/* POFF RAM start address */
#define PM_PBS_POFF_RAM_START_ADDR           (PM_PBS_WARM_RESET_RAM_START_ADDR + (4 * PM_PBS_MAX_NUM_SEQS))

/* BUA RAM start address */
#define PM_PBS_BUA_RAM_START_ADDR            (PM_PBS_POFF_RAM_START_ADDR + (4 * PM_PBS_MAX_NUM_SEQS))

/* Warm reset existing sequence address */
#define PM_PBS_WARM_RESET_EXIST_SEQ_ADDR     0x0008

/* POFF existing sequence address */
#define PM_PBS_POFF_EXIST_SEQ_ADDR           0x000C

/* BUA existing sequence address */
#define PM_PBS_BUA_EXIST_SEQ_ADDR            0x0018

/* Warm reset trigger id */
#define PM_PBS_WARM_RESET_TRIGGER_ID         2

/* POFF trigger id */
#define PM_PBS_POFF_TRIGGER_ID               3

/* BUA trigger id */
#define PM_PBS_BUA_TRIGGER_ID                6

/* Un-initialized PBS sequence value */
#define PM_PBS_SEQ_UNINITIALIZED             0xFFFFFFFF

/* Trigger start address LSB where x is the trigger id */
#define PMIO_PBS_CORE_TRIG_START_ADDR_LSB(x)   (0x7050 + (0x4 * (x)))

/* Trigger start address MSB where x is the trigger id */
#define PMIO_PBS_CORE_TRIG_START_ADDR_MSB(x)   (0x7051 + (0x4 * (x)))

/*===========================================================================

                        FUNCTION DEFINITIONS 

===========================================================================*/
/** 
 * @name pm_pbs_ram_sequences_verify 
 *  
 * @brief This is an internal helper function. This function 
 *        reads the RAM sequences from PBS RAM in READ BURST
 *        mode starting from the PBS RAM address specified by
 *        the caller and compares it with the expected sequences
 *        sent in by the caller. This function internally calls
 *        pm_pbs_config_access to configure READ BURST mode
 *        access to PBS RAM.
 *  
 * @param[in]  slave_id. Primary PMIC chip: 1. Secondary PMIC Chip: 4
 * @param[in]  ram_addr: PBS RAM address starting from which the
 *             sequences are read.
 * @param[in]  seq_ptr: Pointer to expected PBS RAM sequences to
 *             which the read sequences are compared against.
 * @param[in]  num_seqs: Number of sequences to be verified in 
 *             PBS RAM.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__DATA_VERIFY_FAILURE = if the read
 *          sequences from PBS RAM don't match the sequences
 *          sent in by the caller.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          else SPMI ERROR.
 *  
 * @dependencies pm_pbs_enable_access should have been called
 *               prior to reading RAM sequences using this
 *               function.
 */
static pm_err_flag_type 
pm_pbs_ram_sequences_verify(uint32 slave_id, uint16 ram_addr, uint32* seq_ptr, uint8 num_seqs) 
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint32 seq_read = 0;
  uint8 seq_cnt = 0;

  /* Configuring read access in burst mode to PBS RAM */
  err_flag = pm_pbs_config_access(slave_id, ram_addr, PM_PBS_ACCESS_READ_BURST);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Burst Read loaded PBS RAM sequence and verify */
  for (seq_cnt = 0;seq_cnt < num_seqs;seq_cnt++)
  {

    /* Writing the sequence sent by user to PBS RAM */
    err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&seq_read, 1);

    busywait(10);

    if (seq_ptr[seq_cnt] != seq_read)
    {
      return PM_ERR_FLAG__DATA_VERIFY_FAILURE;
    }

    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }

  }

  return err_flag;
}

/** 
 * @name pm_pbs_ram_sequences_load 
 *  
 * @brief This is an internal helper function. This function 
 *        loads RAM sequences to PBS RAM in WRITE BURST mode
 *        starting from the PBS RAM address specified by the
 *        caller. This function internally calls
 *        pm_pbs_ram_sequences_verify functions to check if the
 *        sequences are loaded correctly to PBS RAM. This
 *        function also internally calls pm_pbs_config_access to
 *        configure WRITE BURST mode access to PBS RAM.
 *  
 * @param[in]  slave_id. Primary PMIC chip: 1. Secondary PMIC Chip: 4
 * @param[in]  ram_addr: PBS RAM address at which the sequences 
 *             are started loading.
 * @param[in]  seq_ptr: Pointer to PBS RAM sequences to be 
 *             loaded.
 * @param[in]  num_seqs: Number of sequences to be added to PBS 
 *             RAM.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__DATA_VERIFY_FAILURE = if the RAM
 *          sequences are not loaded correctly.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          else SPMI ERROR.
 *  
 * @dependencies pm_pbs_enable_access should have been called 
 *               prior to loading RAM sequences using this
 *               function.
 */
static pm_err_flag_type 
pm_pbs_ram_sequences_load(uint32 slave_id, uint16 ram_addr, uint32* seq_ptr, uint8 num_seqs) 
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 seq_cnt = 0;

  /* Configuring write access in burst mode to PBS RAM */
  err_flag = pm_pbs_config_access(slave_id, ram_addr, PM_PBS_ACCESS_WRITE_BURST);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  for (seq_cnt = 0;seq_cnt < num_seqs;seq_cnt++)
  {

    /* Burst write the sequence sent by user to PBS RAM */
    err_flag = pm_comm_write_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_WR_DATA0_ADDR, 4, (uint8*)&seq_ptr[seq_cnt], 1);

    busywait(10);

    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }

  }

  /* Verify if the sequences are loaded correctly to PBS RAM */
  err_flag = pm_pbs_ram_sequences_verify(slave_id, ram_addr, seq_ptr, num_seqs);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  return err_flag;
}

/** 
 * @name pm_pbs_add_ram_sequences 
 *  
 * @brief This is a public API. Please refer to pm_pbs.h file 
 *        for further info on this function.
 */
pm_err_flag_type 
pm_pbs_add_ram_sequences(uint8 pmic_chip, pm_pbs_trigger_type trigger, uint32* seq_ptr, uint8 num_seqs) 
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_model_type pmic_model = PMIC_IS_UNKNOWN;
  uint32 un_init_seq_arr[PM_PBS_MAX_NUM_SEQS];
  uint32 slave_id = 0;
  uint32 post_seq = 0;
  uint16 exist_seq_addr = 0;
  uint16 post_seq_addr = 0;
  uint16 post_seq_ram_addr = 0;
  uint16 ram_addr = 0;
  uint8 exist_seq_flag = 0;
  uint8 trig_id = 0;
  uint8 seq_cnt = 0;
  uint8 total_num_seqs = 0;
  uint8 max_num_seqs = 0;
  uint8 data = 0;

  /* Validate arguments and assign required values */
  if (seq_ptr == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  /* Get PMIC chip model type */
  pmic_model = pm_get_pmic_model(pmic_chip);

  /* This feature is not supported on PMI8994 */
  if ((pmic_model == PMIC_IS_INVALID)|| (pmic_model == PMIC_IS_PMI8994))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Get PMIC chip primary slave id */
  err_flag = pm_get_slave_id(pmic_chip, 0, &slave_id);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Read and verify PBS Core periph type */
  err_flag = pm_comm_read_byte(slave_id, PMIO_PBS_CORE_PERPH_TYPE_ADDR, &data, 0);

  if ((err_flag != PM_ERR_FLAG__SUCCESS) || (data != PM_HW_MODULE_PBS))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Determine the RAM and existing sequence addresses and trigger ids based on trigger type */
  switch (trigger)
  {
    case PM_PBS_START_WARM_RESET:
      exist_seq_flag = 1;
      ram_addr = PM_PBS_WARM_RESET_RAM_START_ADDR;
      post_seq_ram_addr = (ram_addr + (4 * num_seqs));
      post_seq_addr = PM_PBS_WARM_RESET_EXIST_SEQ_ADDR;
      trig_id = PM_PBS_WARM_RESET_TRIGGER_ID;
      total_num_seqs = num_seqs + 1;
      max_num_seqs = PM_PBS_MAX_NUM_SEQS;
      break;

    case PM_PBS_START_POFF:
      exist_seq_flag = 1;
      ram_addr = PM_PBS_POFF_RAM_START_ADDR;
      post_seq_ram_addr = (ram_addr + (4 * num_seqs));
      post_seq_addr = PM_PBS_POFF_EXIST_SEQ_ADDR;
      trig_id = PM_PBS_POFF_TRIGGER_ID;
      total_num_seqs = num_seqs + 1;
      max_num_seqs = PM_PBS_MAX_NUM_SEQS;
      break;

    case PM_PBS_BUA_SEQUENCE:
      exist_seq_flag = 1;
      ram_addr = PM_PBS_BUA_RAM_START_ADDR;
      post_seq_ram_addr = (ram_addr + (4 * num_seqs));
      post_seq_addr = PM_PBS_BUA_EXIST_SEQ_ADDR;
      trig_id = PM_PBS_BUA_TRIGGER_ID;
      total_num_seqs = num_seqs + 1;
      max_num_seqs = PM_PBS_MAX_NUM_SEQS;
      break;

    default:
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* If total num of sequences to be added is greater than max allowed, return error */
  if (total_num_seqs > max_num_seqs)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  /* Fill in the array with un-initialized seq value */
  for (seq_cnt = 0;seq_cnt < total_num_seqs;seq_cnt++)
  {
    un_init_seq_arr[seq_cnt] = (uint32)PM_PBS_SEQ_UNINITIALIZED;
  }

  /* Read the existing trigger address for the specific trigger id and save it */
  err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_TRIG_START_ADDR_LSB(trig_id), 2, (uint8*)&exist_seq_addr, 1);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Configuring to enable PBS ROM/RAM access */
  err_flag = pm_pbs_enable_access(slave_id);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Verify if the PBS RAM address space where the sequences will be added is
     uninitialized to make sure we don't over-write any existing valid sequences */
  err_flag = pm_pbs_ram_sequences_verify(slave_id, ram_addr, un_init_seq_arr, total_num_seqs);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
      /* Configuring to disable PBS ROM/RAM access */
      pm_pbs_disable_access(slave_id);

      return err_flag;
  }

  /* Load the sequences to PBS RAM and verify if the sequences are loaded correctly */
  err_flag = pm_pbs_ram_sequences_load(slave_id, ram_addr, seq_ptr, num_seqs);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    /* Configuring to disable PBS ROM/RAM access */
    pm_pbs_disable_access(slave_id);

    return err_flag;
  }

  /* Add a GOTO sequence at the end of PBS RAM address if user entered
     sequence codes have to be pre-pended to the existing sequence */
  if (exist_seq_flag)
  {
    /* Update the post-sequence GOTO address only if the existing
       trigger address is not equal to ram address  */
    if (exist_seq_addr != ram_addr)
    {
      post_seq_addr = exist_seq_addr;
    }

    post_seq = PM_PBS_GOTO(post_seq_addr);
  }
  else
  {
    /* Add an EOS sequence at the end of PBS RAM address if there is no
       existing sequence */
    post_seq = PM_PBS_EOS;
  }

  /* Load the post sequence to PBS RAM and verify if the sequence is loaded correctly */
  err_flag = pm_pbs_ram_sequences_load(slave_id, post_seq_ram_addr, &post_seq, 1);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    /* Configuring to disable PBS ROM/RAM access */
    pm_pbs_disable_access(slave_id);
    return err_flag;
  }

  /* Configuring to disable PBS ROM/RAM access */
  err_flag = pm_pbs_disable_access(slave_id);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  /* Configuring RAM address as the new trigger address */
  err_flag = pm_comm_write_byte_array(slave_id, PMIO_PBS_CORE_TRIG_START_ADDR_LSB(trig_id), 2, (uint8*)&ram_addr, 1);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return err_flag;
  }

  return err_flag;
}
