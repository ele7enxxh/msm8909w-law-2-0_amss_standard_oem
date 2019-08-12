/*
===========================================================================

FILE:         ddr_params.c

DESCRIPTION:
  Information of DDR devices supported by target.

===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/common/params/v2/ddr_params.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/10/14   sk      'ddr_set_training_checksum' and 'ddr_get_training_checksum' functions added 
05/21/14   aus     Updated to support shared DDR library
09/21/13   sr      Fix for training in boot .
09/06/13   sr      ddr_set_training_cookie added
08/22/13   sr      Initial revision.

===========================================================================
                   Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
===========================================================================
*/

/*==============================================================================
                                    INCLUDES
==============================================================================*/
#include "ddr_params.h"
#include <stringl/stringl.h>
#include "crc.h"

/*==============================================================================
                                      DATA
==============================================================================*/
/* Pointer to DDR device table, updated by ddr_set_params() */
static union ddr_device_params *ddr_device_table = NULL;

static struct ddr_shared_params *ddr_shared_header = (struct ddr_shared_params *)SHARED_IMEM_DDR_PARAM_BASE;

/* Number of entries in DDR device table, updated by ddr_set_params() */
static uint32 ddr_device_table_entry_num = 0;

/* Size of DDR device table entry, updated by ddr_set_params() */
static uint32 ddr_device_table_entry_size = 0;
static uint32 ddr_device_header_size = 0;

/*Indicates ddr is interleaved or not, TRUE by default*/
extern uint32 ddr_interleaved;

/* DDR parameter partition */
static struct ddr_params_partition *ddr_params_training_data = NULL;


/*==============================================================================
                                   FUNCTIONS
==============================================================================*/


/* ============================================================================
**  Function : ddr_get_shared_header
** ============================================================================
*/
/**
*   @brief
*   This function returns a pointer to the shared header that contains useful
*   information from SBL to RPM
*
*   @param
*   None
*                         
*
*   @return
*   pointer to shared header location
*
*   @dependencies
*   None
*
*   @sa
*   None
*
*   @sa
*   None
*/
struct ddr_shared_params * ddr_get_shared_header(void)
{
  return ddr_shared_header;
}

/* ============================================================================
**  Function : ddr_set_params
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR driver's device table to an external device table.
*   It will also update the interleaving mode information according to the parameter
*   header that's passed in.
*   User should append ddr_device_params_header_v1 header in front of their own DDR device 
*   table and pass  a pointer points to the beginning of this header via the this API to 
*   force DDR driver to utilize the user defined parameters.
*   The user defined ddr device table must contain a minimal number of entries to 
*   match the system's ddr devices.
*   For example if a system has two ddr devices attached then the external table must 
*   define at least two entries to match those two devices.
*   However, more entries can be added to the end of table to support other ddr devices
*   with different parameters.
*
*   @param[in]  ddr_params_ptr   Pointer to the external ddr parameters. It should
*                                point to the user defined ddr device table with ddr
*                                parameters header appended in front. This pointer must
*                                points to a 4 bytes aligned address.
*                         
*
*   @return
*   TRUE if DDR device table is updated, FALSE if an error happens and this operation
*   is aborted
*
*   @dependencies
*   Must be called prior to ddr_initialize_device. 
*   This API will take no effect after ddr_initialize_device
*
*   @sa
*   None
*
*   @sa
*   None
*/
boolean ddr_set_params(void *ddr_params_ptr, uint32 ddr_params_size)
{
  struct ddr_device_params_header_v2 *ddr_params_header = NULL;
  boolean status = FALSE;
  uint32 *ddr_params_header_version = NULL;
  uint32 ddr_params_header_size = 0;

  do
  {
    /*First check to see if external ddr parameters exists*/
    if(ddr_params_ptr == NULL || ddr_params_size == 0)
    {
      break;
    }

    /*We know the first byte in ddr parameters data is its version number*/
    ddr_params_header_version = (uint32 *)ddr_params_ptr;

    if(*ddr_params_header_version == 2)
    {
      /*The beginning of external DDR parameters is a header which tells the
        number of interfaces and size of ddr device table entry*/
      ddr_params_header = (struct ddr_device_params_header_v2 *)
                          ddr_params_ptr;
      ddr_params_header_size = DDR_PARAMS_HEADER_V2_SIZE;

      /*Check if the magic number in the header is correct*/
      if(ddr_params_header->magic_number != DDR_PARAMS_MAGIC_NUM)
      {
        break;
      }

      /*Update the device table pointer*/
      /*The actual device table is right after the ddr parameter header*/
      ddr_device_table = (union ddr_device_params *)
                         ((uint32)ddr_params_ptr + ddr_params_header_size);

      /* Update number of device table entries*/
      ddr_device_table_entry_num = ddr_params_header->num_of_device;

      /* Update size of device table entry */
      switch (ddr_device_table[0].common.device_type)
      {
        case DDR_TYPE_LPDDR2:
        case DDR_TYPE_LPDDR3:
          ddr_device_table_entry_size = sizeof(struct ddr_device_params_lpddr);
          break;

        case DDR_TYPE_PCDDR3:
          ddr_device_table_entry_size = sizeof(struct ddr_device_params_pcddr3);
          break;

        default:
          ddr_device_table_entry_size = 0;
          break;
      }

      /* Check if size of device table entry matches size written in header */
      if (ddr_device_table_entry_size == 0 ||
          ddr_device_table_entry_size != ddr_params_header->size_of_param)
      {
        break;
      }
    }
    else
    {
      /* Currently only Version 2 is supported */
      break;
    }

    status = TRUE;

  } while(0);

  return status;
}

/* =============================================================================
**  Function : ddr_params_detection
** =============================================================================
*/
/**
*   @brief
*   Detect correct DDR device parameters based on manufacture and dimension
*   information.
*
*   @param[in]  interface_name  Interface to detect for
*
*   @retval  TRUE   Parameters detected
*   @retval  FALSE  Parameters not dectected
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_detection(SDRAM_INTERFACE interface_name)
{
  struct ddr_device_params_common *target_params, *source_params;
  uint32 entry, entry_offset;

  /* Get current device parameters to be matched */
  target_params = &(ddr_get_params(interface_name)->common);

  /* Skip first two entries of device table */
  entry = 2;
  entry_offset = 2 * ddr_device_table_entry_size;

  while (entry < ddr_device_table_entry_num)
  {
    source_params = &(( (union ddr_device_params *)
                        ((uint32)ddr_device_table + entry_offset) )->common);

    if(source_params->manufacture_name == target_params->manufacture_name &&
       source_params->num_rows_cs0 == target_params->num_rows_cs0 &&
       source_params->num_cols_cs0 == target_params->num_cols_cs0 &&
       source_params->num_banks_cs0 == target_params->num_banks_cs0 &&
       source_params->num_rows_cs1== target_params->num_rows_cs1 &&
       source_params->num_cols_cs1 == target_params->num_cols_cs1 &&
       source_params->num_banks_cs1 == target_params->num_banks_cs1 &&
       source_params->interface_width_cs0 == target_params->interface_width_cs0 &&
       source_params->interface_width_cs1 == target_params->interface_width_cs1)
    {
      break;
    }

    entry++;
    entry_offset += ddr_device_table_entry_size;
  }

  return (entry < ddr_device_table_entry_num);

} /* ddr_params_detection */

/* ============================================================================
**  Function : ddr_params_set_shared_memory
** ============================================================================
*/
/**
*   @brief
*   Copies the detected ddr parameter to shared memory region
*
*   @return
*   None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_params_set_shared_memory( void )
{
  /* We need to populate the updated parameter to correct location in shared device table so rpm
   will get the correct entry */

   
#if 0
  /* Use memmove to ensure the correctness of ram copy if ddr parameters buffer region
     overlaps with shared imem DDR region*/
  memmove((void *)shared_ddr_device_table,
          (void *)ddr_get_params(SDRAM_INTERFACE_0),
          ddr_device_table_entry_size);

  memmove((void *)((uint32)shared_ddr_device_table + ddr_device_table_entry_size),
          (void *)ddr_get_params(SDRAM_INTERFACE_1),
          ddr_device_table_entry_size);
#endif

#if 0 
  /* Update DDR device parameters with new locations */
  ddr_device_params_ch0 = shared_ddr_device_table;
  ddr_device_params_ch1 = (union ddr_device_params *)
                          ((uint32)shared_ddr_device_table + ddr_device_table_entry_size);
#endif						  
ddr_device_header_size = sizeof(struct ddr_shared_params);
  if (ddr_params_training_data != NULL)
  {
    /* Move DDR parameter partition behind DDR device parameters */
    memmove((void *)(SHARED_IMEM_DDR_PARAM_BASE + (ddr_device_table_entry_size * 2) + ddr_device_header_size),
            (void *)ddr_params_training_data,
            sizeof(*ddr_params_training_data));

    /* Update DDR parameter partition with new location */
    ddr_params_training_data = (struct ddr_params_partition *)
                           (SHARED_IMEM_DDR_PARAM_BASE + ddr_device_table_entry_size * 2 + ddr_device_header_size);
  }
} /* ddr_param_set_shared_memory */

/* =============================================================================
**  Function : ddr_params_get_training_data
** =============================================================================
*/
/**
*   @brief
*   Get DDR parameter partition: partition base is returned, and partition size
*   is updated through pointer.
*
*   @param[out]  size  DDR parameter partition size
*
*   @return
*   DDR parameter partition base
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void *ddr_params_get_training_data(uint32 *size)
{
  *size = sizeof(struct ddr_params_partition);

  return (void *)ddr_params_training_data;

} /* ddr_params_get_partition */

/* =============================================================================
**  Function : ddr_params_set_training_data
** =============================================================================
*/
/**
*   @brief
*   Set DDR parameter partition.
*
*   @param[in]  base  DDR parameter partition base
*   @param[in]  size  DDR parameter partition size
*
*   @retval  TRUE   Partition set successfully
*   @retval  FALSE  Partition set unsuccessfully
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_set_training_data(void *base, uint32 size)
{
  boolean success;

  if ( base != NULL && size == sizeof(struct ddr_params_partition) )
  {
    ddr_params_training_data = (struct ddr_params_partition *)base;

    /* Check magic number to see if partition is valid */
    if (ddr_params_training_data->magic_number != DDR_PARAMS_MAGIC_NUM)
    {
      /* If partition is invalid, zero out partition and set magic number */
      memset(ddr_params_training_data, 0, sizeof(*ddr_params_training_data));
    }

    success = TRUE;
  }
  else
  {
    success = FALSE;
  }

  return success;

} /* ddr_params_set_partition */


/* =============================================================================
**  Function : ddr_get_trained_params
** =============================================================================
*/
/**
*   @brief
*   Get DDR trained parameters.
*
*   @param[in]  interface_name  Interface to get for
*
*   @return
*   DDR trained parameters
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
union ddr_trained_params *ddr_get_trained_params(SDRAM_INTERFACE interface_name)
{
  union ddr_trained_params *trained_params;

#ifndef BUILD_BOOT_CHAIN
  if (ddr_params_training_data == NULL)
  {
    uint32 device_params_size;

    /* Get size of device table entry */
    switch (ddr_get_params(SDRAM_INTERFACE_0)->common.device_type)
    {
      case DDR_TYPE_LPDDR2:
      case DDR_TYPE_LPDDR3:
        device_params_size = sizeof(struct ddr_device_params_lpddr);
        break;

      case DDR_TYPE_PCDDR3:
        device_params_size = sizeof(struct ddr_device_params_pcddr3);
        break;

      default:
        device_params_size = 0;
        break;
    }

    /* DDR parameter partition is after two device table entries */
    ddr_params_training_data = (struct ddr_params_partition *)
                           (SHARED_IMEM_DDR_PARAM_BASE + device_params_size * 2);
  }
#endif

  if (ddr_params_training_data != NULL)
  {
    trained_params  = (interface_name == SDRAM_INTERFACE_0)
                      ? &(ddr_params_training_data->trained_params[0])
                      : &(ddr_params_training_data->trained_params[1]);
  }
  else
  {
    trained_params = NULL;
  }

  return trained_params;

} /* ddr_get_trained_params */

/* =============================================================================
**  Function : ddr_get_training_cookie
** =============================================================================
*/
/**
*   @brief
*   Get DDR training magic cookie.
*
*   @param
*   None
*
*   @return
*   DDR training cookie, 0 if no valid training partition is found
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
uint32 ddr_get_training_cookie(void)
{
#ifndef BUILD_BOOT_CHAIN
  if (ddr_params_training_data == NULL)
  {
    uint32 device_params_size;

    /* Get size of device table entry */
    switch (ddr_get_params(SDRAM_INTERFACE_0)->common.device_type)
    {
      case DDR_TYPE_LPDDR2:
      case DDR_TYPE_LPDDR3:
        device_params_size = sizeof(struct ddr_device_params_lpddr);
        break;

      case DDR_TYPE_PCDDR3:
        device_params_size = sizeof(struct ddr_device_params_pcddr3);
        break;

      default:
        device_params_size = 0;
        break;
    }

    /* DDR parameter partition is after two device table entries */
    ddr_params_training_data = (struct ddr_params_partition *)
                           (SHARED_IMEM_DDR_PARAM_BASE + device_params_size * 2);

  }
#endif

  if (ddr_params_training_data != NULL)
  {
    return ddr_params_training_data->magic_number;
  }
  else
  {
    return 0;
  }
}
/* =============================================================================
**  Function : ddr_set_training_cookie
** =============================================================================
*/
/**
*   @brief
*   Set DDR training magic cookie.
*
*   @param
*   None
*
*   @return
*   boolean - operation success or fail
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_set_training_cookie(uint32 cookie)
{
#ifndef BUILD_BOOT_CHAIN
  if (ddr_params_training_data == NULL)
  {
    uint32 device_params_size;

    /* Get size of device table entry */
    switch (ddr_get_params(SDRAM_INTERFACE_0)->common.device_type)
    {
      case DDR_TYPE_LPDDR2:
      case DDR_TYPE_LPDDR3:
        device_params_size = sizeof(struct ddr_device_params_lpddr);
        break;

      case DDR_TYPE_PCDDR3:
        device_params_size = sizeof(struct ddr_device_params_pcddr3);
        break;
        
      default:
        device_params_size = 0;
        break;
    }

    /* DDR parameter partition is after two device table entries */
    ddr_params_training_data = (struct ddr_params_partition *)
                           (SHARED_IMEM_DDR_PARAM_BASE + device_params_size * 2);

  }
#endif

  if (ddr_params_training_data != NULL)
  {
    ddr_params_training_data->magic_number = cookie;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/* =============================================================================
**  Function : ddr_get_training_checksum
** =============================================================================
*/
/**
*   @brief
*   Get DDR training checksum.
*
*   @param
*   None
*
*   @return
*   DDR training data checksum, 0 if no valid training partition is found
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
uint32 ddr_get_training_checksum(uint32 serial_number)
{
  uint32 tr_data_size_in_bits;
  uint32 crc32 = 0x0UL;
  
  if (ddr_params_training_data != NULL)
  {
    /* Calculate the training data size in bits */
    tr_data_size_in_bits = sizeof (union ddr_trained_params) * 8;
    crc32 = crc_32_calc((uint8 *)(ddr_params_training_data->trained_params), (uint16)tr_data_size_in_bits, serial_number);
    return crc32;
  }
  else
  {
    return 0;
  }
}
/* =============================================================================
**  Function : ddr_set_training_checksum
** =============================================================================
*/
/**
*   @brief
*   Compute the checksum over entire training data
*
*   @param
*   Serial number
*
*   @return
*   CRC checksum
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_set_training_checksum(uint32 serial_number)
{
  uint32 tr_data_size_in_bits;
  uint32 crc32 = 0x0UL;
	
  if (ddr_params_training_data != NULL)
  {
    /* Calculate the training data size in bits */
    tr_data_size_in_bits = sizeof (union ddr_trained_params) * 8;
    crc32 = crc_32_calc((uint8 *)(ddr_params_training_data->trained_params), (uint16)tr_data_size_in_bits, serial_number);
    ddr_params_training_data->training_checksum = crc32;
	return TRUE;
  }
  else
  {
    return FALSE;
  }
}
