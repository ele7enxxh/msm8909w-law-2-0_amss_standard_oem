/*
===========================================================================

FILE:         ddr_params.c

DESCRIPTION:
  Information of DDR devices supported by target.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/common/params/v1/ddr_params.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/09/14   tw      Added support for common shared library
01/13/14   tw      Added shared header at start of shared memory
07/08/13   wg      Replaced unsafe functions (go/scanban) with safe alternatives
06/07/13   rj      Removed lpddr3 check for ddr training to target specific file.
01/10/13   tw      Removed cookie setting in get param, cookie should be set 
				   only when we finish training
09/21/12   sl      Combined LPDDR device parameters.
08/17/12   sl      Added LPDDR3 parameter size.
02/02/12   sl      Common LPDDR1/LPDDR2/PCDDR3 device parameters.
10/26/11   tw      Merged with ddr_devices.c
10/26/11   tw      Initial port for 8064
07/01/11   dh      Ported imem shared device table feature from 8660, 
                   added ddr_set_params to allow the use of external ddr parameters
06/21/11   tw      Added support for ED8064B2PH LPDDR2 device
05/10/11   tw      Added tDQSCK_min and tDQSCK_max parameters
01/12/11   sl      Updated DDR parameters.
11/24/10   tw      Fixed issue with tWL and tRL difference when auto detecting 
                   DDR Devices.
11/22/10   sl      Updated tREFI of LPDDR2_CS0_CS1_JEDEC.
09/20/10   tw      Updated default JEDEC parameter to be more relaxed.
09/17/10   tw      Added API for Warm boot ddr detection for cases where ddr
                   is already configured
06/23/10   sl      Renamed DDR_Devices.h as ddr_devices.c
04/21/10   tw      Added JEDEC entries for runtime size detection
04/21/10   tw      Added manufacture name to device table to aid in device 
                   detection
03/19/10   tw      Ported to 8660
01/07/09   tw      Added table for PCDDR2 devices
06/27/09   tw      Initial revision.

===========================================================================
                   Copyright 2009-2012 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
===========================================================================
*/

/*==============================================================================
                                    INCLUDES
==============================================================================*/
#include "HAL_DDR.h"
#include "ddr_params.h"
#include "string.h"


/*==============================================================================
                                      DATA
==============================================================================*/
/* Pointer to DDR device table, updated by ddr_set_params() */
static union ddr_device_params *ddr_device_table = NULL;

static struct ddr_shared_params *ddr_shared_header = (struct ddr_shared_params *)SHARED_IMEM_DDR_PARAM_BASE;

/* Pointer to shared imem DDR device table, this table is valid after ddr initialization is done */
static union ddr_device_params *shared_ddr_device_table =
                              (union ddr_device_params *)(SHARED_IMEM_DDR_PARAM_BASE + sizeof(struct ddr_shared_params));

/* Number of entries in DDR device table, updated by ddr_set_params() */
static uint32 ddr_device_table_entry_num = 0;

/* Size of DDR device table entry, updated by ddr_set_params() */
static uint32 ddr_device_table_entry_size = 0;

/* DDR device parameters of CH0 and CH1 */
static union ddr_device_params *ddr_device_params_ch0 = NULL;
static union ddr_device_params *ddr_device_params_ch1 = NULL;

/*Indicates ddr is interleaved or not, TRUE by default*/
static uint32 ddr_interleaved = SDRAM_BOTH;

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
  struct ddr_device_params_header_v1 *ddr_params_header = NULL;
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

    if(*ddr_params_header_version == 1)
    {
      /*The beginning of external DDR parameters is a header which tells the
        number of interfaces and size of ddr device table entry*/
      ddr_params_header = (struct ddr_device_params_header_v1 *)
                          ddr_params_ptr;
      ddr_params_header_size = DDR_PARAMS_HEADER_V1_SIZE;

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

      /* Update interleave indicator */
      ddr_interleaved = ddr_params_header->interleaved;
    }
    else
    {
      /* Currently only Version 1 is supported */
      break;
    }

    status = TRUE;

  } while(0);

  return status;
}

/* ============================================================================
**  Function : ddr_get_params
** ============================================================================
*/
/**
*   @brief
*   Get DDR device parameters.
*
*   @param[in]  interface_name  Interface to get DDR device parameters for
*
*   @return
*   DDR device parameters
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
union ddr_device_params *ddr_get_params(SDRAM_INTERFACE interface_name)
{
  union ddr_device_params *device_params, *device_table;
  uint32 device_params_size;

  device_params = (interface_name == SDRAM_INTERFACE_0) ? ddr_device_params_ch0
                                                        : ddr_device_params_ch1;

  if (device_params == NULL)
  {
#ifdef BUILD_BOOT_CHAIN
    /* Use external table for Boot */
    device_table = ddr_device_table;
#else
    /* Use shared IMEM table for RPM */
    device_table = shared_ddr_device_table;
#endif

    if (interface_name == SDRAM_INTERFACE_0)
    {
      device_params = ddr_device_params_ch0 = device_table;
    }
    else
    {
      /* Get size of device table entry */
      switch (device_table[0].common.device_type)
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

      device_params = ddr_device_params_ch1
                    = (union ddr_device_params *)
                      ((uint32)device_table + device_params_size);
    }
  }

  return device_params;

} /* ddr_get_params */

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
       source_params->interface_width == target_params->interface_width)
    {
      if (interface_name == SDRAM_INTERFACE_0)
      {
        ddr_device_params_ch0 = (union ddr_device_params *)
                                ((uint32)ddr_device_table + entry_offset);
      }
      else
      {
        ddr_device_params_ch1 = (union ddr_device_params *)
                                ((uint32)ddr_device_table + entry_offset);
      }
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

  /* Use memmove to ensure the correctness of ram copy if ddr parameters buffer region
     overlaps with shared imem DDR region*/
  memmove((void *)shared_ddr_device_table,
          (void *)ddr_get_params(SDRAM_INTERFACE_0),
          ddr_device_table_entry_size);

  memmove((void *)((uint32)shared_ddr_device_table + ddr_device_table_entry_size),
          (void *)ddr_get_params(SDRAM_INTERFACE_1),
          ddr_device_table_entry_size);

  /* Update DDR device parameters with new locations */
  ddr_device_params_ch0 = shared_ddr_device_table;
  ddr_device_params_ch1 = (union ddr_device_params *)
                          ((uint32)shared_ddr_device_table + ddr_device_table_entry_size);

  if (ddr_params_training_data != NULL)
  {
    /* Move DDR parameter partition behind DDR device parameters */
    memmove((void *)(SHARED_IMEM_DDR_PARAM_BASE + ddr_device_table_entry_size * 2),
            (void *)ddr_params_training_data,
            sizeof(*ddr_params_training_data));

    /* Update DDR parameter partition with new location */
    ddr_params_training_data = (struct ddr_params_partition *)
                           (SHARED_IMEM_DDR_PARAM_BASE + ddr_device_table_entry_size * 2);
  }
} /* ddr_param_set_shared_memory */

/* ============================================================================
**  Function : ddr_param_interleaved
** ============================================================================
*/
/**
*   @brief
*   Return the interleave setting based on ddr parameters
*
*   @return
*   return a bitmask of interleave status
*
*   @dependencies
*   Must be called after ddr_set_params. Calling it before ddr_set_params will
*   return the default setting which is TRUE.
*
*   @sa
*   None
*
*   @sa
*   None
*/
uint32 ddr_param_interleaved(void)
{
  return ddr_interleaved;
}

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

  if (ddr_params_training_data != NULL &&
      ddr_params_training_data->magic_number == DDR_PARAMS_MAGIC_NUM)
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

