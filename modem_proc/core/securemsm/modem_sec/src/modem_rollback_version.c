/**
* @file modem_rollback_version.c
* @brief Modem Rollback Protection
*
* This file implements Modem Rollback Protection
*
*/
/*===========================================================================
   Copyright (c) 2012 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/modem_sec/src/modem_rollback_version.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $


when       who      what, where, why
--------   ---      ------------------------------------
01/09/2013 rsahay       Initial version
===========================================================================*/

/* NOTE: This code only works for versions which are in the LSB. Needs update if
 * the version switches to MSB
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "modem_hwio.h"
#include "qfprom.h"
#include "DALSys.h"
#include "modem_rollback_version.h"
#include "modem_hw_mutex.h"

/* Error codes */
#define E_FAILURE                   -1
#define E_BAD_FUSE_ADDRESS          -2
#define E_QFPROM_WRITE_ROW_FAILURE  -3
#define E_NO_LSB_BASE_ADDR          -4
#define E_UPDATE_FUSE_VER_FAIL      -5
#define E_SUCCESS                    0


static DALSYSMemHandle     qfprom_row_data_mem_handle;

/** 
* Count number of 1s which indicate software version
**/
static uint32 modem_count_set_bits(uint32 input_val)
{
  uint32 count = 0;
  for(count = 0; input_val; count++)
  {
    input_val &= input_val - 1 ;
  }
  return count;
}


/*===========================================================================

**  Function : modem_rollback_is_feature_enabled_on_set

** ==========================================================================
*/
/*!
* 
* @brief
*    This function determines if rollback protection feature is enabled 
*     given an image set
*
* @param[in] rollback_img_set pointer to a rollback protection image set
*
*
* @par Dependencies
*    None
* 
* @retval
*    int  Returns True if rollback protection feature is enabled,
*         False otherwise                  
* 
* 
*/

static int modem_rollback_is_feature_enabled_on_set
(
  struct modem_version_rollback_img_set *rollback_img_set
)
{  
  int rollback_enable = FALSE;
  
  /* Apply the bit mask on the fuse content, if result is nonzero then
     this feature is enabled */
  if( (rollback_img_set->feature_enable_base_addr != NULL) &&
      ((*rollback_img_set->feature_enable_base_addr & 
      rollback_img_set->feature_enable_mask) != 0))
  {
      rollback_enable = TRUE;
  }
  return rollback_enable;
}

/*===========================================================================

**  Function : modem_rollback_calculate_fuse_version

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns the fuse version value given an image set
*
* @param[in] rollback_img_set pointer to a rollback protection image set
*
*
* @par Dependencies
*    None
* 
* @retval
*    fuse_version  Returns the fuse version value of this image set                 
* 
* 
*/

static uint32 modem_rollback_calculate_fuse_version
(
  struct modem_version_rollback_img_set *rollback_img_set
)
{
  uint32 fuse_version = 0;
  uint32 fuse_lsb_val = 0;
  
  if(rollback_img_set->version_lsb_base_addr_va != NULL)
  {
    /*Get the value of LSB fuse by applying the bit mask */
    fuse_lsb_val = *(rollback_img_set->version_lsb_base_addr_va) &
                   rollback_img_set->version_mask;
  }

  /* Count the bits that are set in each fuse and add them up,
   *  that will be the version number of current set 
   */
  fuse_version = modem_count_set_bits(fuse_lsb_val);
  
  return fuse_version;
}

/*===========================================================================

**  Function : modem_rollback_blow_raw_fuse

** ==========================================================================
*/
/*!
* 
* @brief
*    Given a single raw fuse address and a raw value that we want 
*    to write to this fuse, this function will blow that fuse.
*
* @param[in] raw_value   the raw value we want to write to this fuse.
*
* @param[in] fuse_address   the raw fuse address
*
* @par Dependencies
*    None
* 
* @retval
*    int Returns error code in case of error
*                  Returns E_SUCCESS in case of no error
* 
*/

static int modem_rollback_blow_raw_fuse
(
  uint32 raw_value,
  const uint32 *fuse_address
)
{
  DALResult dal_result;
  DALSYSMemInfo qfprom_row_data_memInfo;
  uint32 *qfprom_row_data = NULL;
  int status = E_FAILURE;
  uint32 row_base_address = 0;
  size_t len = sizeof(uint64);

  do
  {
    /* Allocate Uncached Memory */
    dal_result = DALSYS_MemRegionAlloc(
      DALSYS_MEM_PROPS_PHYS_CONT | DALSYS_MEM_PROPS_UNCACHED, 
      DALSYS_MEM_ADDR_NOT_SPECIFIED, DALSYS_MEM_ADDR_NOT_SPECIFIED,
      len, &qfprom_row_data_mem_handle, NULL);
    if (dal_result != DAL_SUCCESS || qfprom_row_data_mem_handle == NULL)
    {
      break;
    }
  
    if (DAL_SUCCESS == DALSYS_MemInfo(qfprom_row_data_mem_handle, 
      &qfprom_row_data_memInfo))
    {
      qfprom_row_data = (uint32 *)qfprom_row_data_memInfo.VirtualAddr;
    }
    else
    {
      break;
    }
  
    if( ((uint32)fuse_address % 8) == 0)
    {
      /* Now copy the lsb value we want to blow to the lsb of fuse data buffer */
      qfprom_row_data =  &raw_value;
      row_base_address = (uint32)fuse_address;
    }
    else
    {
      /* The fuse address is not multiples of 32 and 64, should never happen*/
      status = E_BAD_FUSE_ADDRESS;
      break;
    }
    

	fuse_grab_mutex();

    /* Call QFPROM driver and pass over the fuse row base address 
     * and fuse row data 
     */
    if (QFPROM_NO_ERR != qfprom_write_row (row_base_address, 
      qfprom_row_data, 0))
    {
      status = E_QFPROM_WRITE_ROW_FAILURE;
    }   
    else 
    {
      status = E_SUCCESS;
    }   
    fuse_release_mutex();
    
  }while(0);
  
  if (qfprom_row_data)
  {
    DALSYS_DestroyObject(qfprom_row_data_mem_handle);
  }
  return status;
}

/*===========================================================================

**  Function : modem_rollback_blow_fuse_version

** ==========================================================================
*/
/*!
* 
* @brief
*    This function will check if a given image set's current version number is 
*    greater than its fuse version number, if it is, it will blow the version fuse
*    to make fuse version number equals to current version number.
*
* @param[in] rollback_img_set pointer to a rollback protection image set
*
*
* @par Dependencies
*    None
* 
* @retval
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error
* 
*/

static int modem_rollback_blow_fuse_version
(
  struct modem_version_rollback_img_set *rollback_img_set
)
{
  uint32 current_version = 0;
  uint32 fuse_lsb_max = 0;
  uint32 version_to_blow_lsb = 0;
  
  int blow_success = E_FAILURE;
  
  /* current_version is the version number that is obtained from secboot cert */
  current_version = rollback_img_set->current_version_num;
  
  /* Max version number that lsb version fuse can hold */
  fuse_lsb_max = modem_count_set_bits(rollback_img_set->version_mask);

  do
  {
    /* fuse_lsb_max must be non zero and base address must exist */
    if(fuse_lsb_max == 0 || rollback_img_set->version_lsb_base_addr_pa == NULL)
    {
      blow_success = E_NO_LSB_BASE_ADDR;
      break;
    }
    
    /* 
      First step: caculate the lsb and msb raw fuse value 
      fuse raw value is calculated as 2^n - 1 , n is the version number we want to blow
    */    

      /* If current version is smaller than the max version we blow the current version*/
      if(current_version < fuse_lsb_max)
      {
        /* Version is the number of 1's */
        version_to_blow_lsb =  (0x2 << (current_version - 1))- 1;  
      }
      /* If current version is bigger or equals to max then we blow the max */
      else
      {
        version_to_blow_lsb =  (0x2 << (fuse_lsb_max - 1))- 1;
      }

    /* 
      Second step: blow the fuse 
    */
    /* Shift the raw value to appropriate location */
    version_to_blow_lsb = version_to_blow_lsb << 
                          rollback_img_set->version_shift;
    
    /* Blow LSB Fuse */
    blow_success = modem_rollback_blow_raw_fuse(version_to_blow_lsb, 
                   rollback_img_set->version_lsb_base_addr_pa);  

    
  }while(0);
  
  return blow_success;
}

/*===========================================================================

                PUBLIC FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

**  Function : modem_rollback_update_fuse_version

** ==========================================================================
*/
/*!
* 
* @brief
*    Given an image type, this function updates the fuse image version accociated
*    with this image by blowing the fuse if the current version stored in the 
*    image set table is greater than the version stored in the fuse.
*
* @param[in] img_sw_type secure image type
*
*
* @par Dependencies
*    None
* 
*    int  Returns error code in case of error
*         Returns E_SUCCESS in case of no error                              
* 
* 
*/
static int modem_rollback_update_fuse_version
(
  struct modem_version_rollback_img_set* img_sw_table
)
{
  int status = E_SUCCESS;
  uint32 fuse_version = 0;
  
  if (img_sw_table != NULL)
  {
    /* First check if rollback feature fuse is enabled */
    if(modem_rollback_is_feature_enabled_on_set(img_sw_table))
    {
      /* Get the fuse version and compare to set version */
      fuse_version = modem_rollback_calculate_fuse_version(img_sw_table);
      
      if(img_sw_table->current_version_num > fuse_version)
      { 
        /*Blow the fuse if set version is greater than fuse version */
        status = modem_rollback_blow_fuse_version(img_sw_table);
      }
    }
  }
  
  return status;
}

/*===========================================================================

**  Function : modem_rollback_update_img_version

** ==========================================================================
*/
/*!
* 
* @brief
*    This function updates the current image version obtained from cert 
*    in the image set table given an image type
*
* @param[in] img_sw_type secure image type
*
* @param[out] img_version  the latest image versio obtained from cert
*
*
* @par Dependencies
*    None
* 
*    int  Returns error code in case of error
*                  Returns E_SUCCESS in case of no error                              
* 
* 
*/

static int modem_rollback_update_img_version
(
  struct modem_version_rollback_img_set *modem_img_table, 
  struct modem_version_rollback_img_set *mba_img_table
)
{
  int status = E_SUCCESS;

  /* Get the version of the Modem Image from RMB register */
  modem_img_table->current_version_num = (uint32)HWIO_MSS_RELAY_MSG_SHADOW_03_IN;
  
  /* Get the version of the MBA Image from the RMB register */
  mba_img_table->current_version_num = (uint32)HWIO_MSS_RELAY_MSG_SHADOW_01_IN;
  
  return status;
}

/** 
* Main function executed as RCINIT task to check for SW Version
* and if fuse should be blown.
**/
int modem_rollback_version_main (void)
{
  int status = E_FAILURE;

  do
  {
    /* Retrieve the Modem Image Version */
    status = modem_rollback_update_img_version(&modem_sec_modem_img, 
      &modem_sec_mba_img);
    if (status != E_SUCCESS)
    {
      break;
    }

    /* Update the version in fuse of the Modem Image */
    status = modem_rollback_update_fuse_version(&modem_sec_modem_img);
    if (status != E_SUCCESS)
    {
      break;
    }

    /* Update the version in fuse of the Modem Image */
    status = modem_rollback_update_fuse_version(&modem_sec_mba_img);
    if (status != E_SUCCESS)
    {
      break;
    }
  }while(0);
  
  return status;
}
