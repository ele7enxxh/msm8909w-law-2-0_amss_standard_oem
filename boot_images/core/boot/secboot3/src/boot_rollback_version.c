/*=============================================================================

               Boot Rollback Version Check

GENERAL DESCRIPTION
  This module provides functionality for version roll back protection

Copyright 2012 - 2015 by Qualcomm Technologies, Inc. All Rights Reserved.
=============================================================================*/


/*=============================================================================

              EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_rollback_version.c#1 $

when       who   what, where, why
--------   ---   ----------------------------------------------------------
01/28/15   jz    Fixed KW error
11/13/14   jz    Updated boot_rollback_blow_fuse_version to support Apps version fuses that
                 span across 3 fuses. Also fixed pointer operation bugs in boot_rollback_calculate_fuse_version.
11/10/14   jz    Fixed the case when MSB is set but LSB is empty in boot_rollback_blow_fuse_version
11/06/14   jz    Fixed the case when MSB is filled but LSB is empty in boot_rollback_calculate_fuse_version
10/23/14   jz    Added back fuse blow support in SBL
06/11/14   jz    Fixed boot_is_auth_enabled to check return status in the caller
04/02/14   ck    Fixed KW issues
12/13/13   ck    Refactored for change in boot_is_auth_enabled parameters
12/09/13   ck    Removed functions for burning fuses as this is done post SBL in Bear family
12/06/13   ck    Fixed spanning bug in boot_rollback_calculate_fuse_version
10/25/12   dh    Initial creation for B family 
 
===========================================================================*/

/*===========================================================================

           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_rollback_version.h"
#include "boot_util.h"
#include "boot_fastcall_tz.h"
#include "qfprom.h"
#include "IxErrno.h"
#include "boot_authenticator.h"

/*=========================================================================== 
 
                     Local Variables
 
===========================================================================*/
/*Internal pointer that points to the rollback image set table */
static struct boot_version_rollback_img_set *boot_rollback_version_img_set_table_ptr = NULL;
static uint32 boot_rollback_version_img_set_size = 0;

/* Internal boolean indicating if hw auth is enabled */
static boot_boolean is_auth_enabled = TRUE;

/*
  Following variables must be declared as static to be allocated
  in uncached DDR ZI because they are shared with TZ qpfrom driver.
  Use zero_init attrubute to force those variables to be allocated in ZI otherwise compiler may 
  optmize it into RW 
*/
__attribute__((section("BOOT_UNCACHED_DDR_ZI_ZONE"), zero_init)) static uint32 qfprom_service_api_status;
__attribute__((section("BOOT_UNCACHED_DDR_ZI_ZONE"), zero_init)) static uint64 qfprom_row_data;

  
/*===========================================================================

                LOCAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

**  Function : boot_rollback_get_set_by_img_type

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns a pointer to the rollback image set given an image type
*
* @param[in] img_sw_type secboot image type
*
*
* @par Dependencies
*    None
* 
* @retval
*    boot_version_rollback_img_set*  Returns NULL if not found, returns pointer
*                                    to the image set if found                                    
* 
* 
*/
static struct boot_version_rollback_img_set *boot_rollback_get_set_by_img_type
(
  secboot_sw_type img_sw_type
)
{
  uint32 i,j;
  
  if(boot_rollback_version_img_set_table_ptr != NULL && 
     boot_rollback_version_img_set_size != 0)
  {
    /* Search through all entries of the img set table */
    for(i = 0; i < boot_rollback_version_img_set_size; i++)
    {
      /* Search through all the images each set contains */
      for(j = 0; j < BOOT_VERSION_ROLLBACK_MAX_IMG_NUM_PER_SET; j++ )
      {
        if (boot_rollback_version_img_set_table_ptr[i].sec_sw_img_list[j] == img_sw_type)
        {
          return &boot_rollback_version_img_set_table_ptr[i];
        }
      }
    }
  }
  
  return NULL;
}


/*===========================================================================

**  Function : boot_rollback_is_feature_enabled_on_set

** ==========================================================================
*/
/*!
* 
* @brief
*    This function determins if rollback protection feature is enabled given an image set
*
* @param[in] rollback_img_set pointer to a rollback protection image set
*
*
* @par Dependencies
*    None
* 
* @retval
*    boot_boolean  Returns True if rollback protection feature is enabled,
*                  False otherwise                  
* 
* 
*/
static boot_boolean boot_rollback_is_feature_enabled_on_set
(
  struct boot_version_rollback_img_set *rollback_img_set
)
{  
  boot_boolean rollback_enable = FALSE;
  
  /* Apply the bit mask on the fuse content, if result is nonzero then
     this feature is enabled */
  if( (rollback_img_set->feature_enable_base_addr != NULL) &&
      ((*rollback_img_set->feature_enable_base_addr &
       rollback_img_set->feature_enable_mask) != 0 ))
  {
      rollback_enable = TRUE;
  }
  
  return rollback_enable;
}


/*===========================================================================

**  Function : boot_rollback_calculate_fuse_version

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
static uint32 boot_rollback_calculate_fuse_version
(
  struct boot_version_rollback_img_set *rollback_img_set
)
{
  uint32 fuse_version = 0;
  uint32 fuse_msb_val = 0;
  uint32 fuse_intermediate_val = 0;
  uint32 fuse_lsb_val = 0;

  uint32 * current_field = NULL;
  uint32 intermediate_bits = 0;

  
  if(rollback_img_set->version_lsb_base_addr != NULL)
  {
    /*Get the raw value of LSB fuse by applying the bit mask */
    fuse_lsb_val = *(rollback_img_set->version_lsb_base_addr) &
                   rollback_img_set->version_lsb_mask;
  }

  
  if(rollback_img_set->version_msb_base_addr != NULL)
  {
    /* There is a possibility with bitmasks 34 bits or larger that
       the fuses can span 3 uint32's.  We must account for this by
       looking at the address span between the lsb base address and
       msb base address.  If span is greater than 4 bytes there is an
       entire 32 bit field or more that must be evaluated. */
    if (rollback_img_set->version_lsb_base_addr != NULL && 
	 ((uint32)rollback_img_set->version_msb_base_addr -
          (uint32)rollback_img_set->version_lsb_base_addr) > sizeof(uint32))
    {
      for (current_field = (uint32 *)(rollback_img_set->version_lsb_base_addr + 1);
           current_field < rollback_img_set->version_msb_base_addr;
           current_field++)
      {
        BL_VERIFY(current_field,
                  BL_ERR_NULL_PTR);

        intermediate_bits = *current_field;
        fuse_intermediate_val += boot_count_set_bits(intermediate_bits); 
      }
    }


    /*Get the raw value of MSB fuse by applying the bit mask */
    fuse_msb_val = *(rollback_img_set->version_msb_base_addr) &
                   rollback_img_set->version_msb_mask;
  }
  

  /* Count the bits that are set in each fuse and add them up,
     that will be the version number of current set */
  fuse_version = boot_count_set_bits(fuse_lsb_val) +
                 fuse_intermediate_val +
                 boot_count_set_bits(fuse_msb_val);

  
  return fuse_version;
}

/*===========================================================================

**  Function : boot_rollback_blow_raw_fuse

** ==========================================================================
*/
/*!
* 
* @brief
*    Given a single raw fuse address and a raw value that we want to write to this fuse,
*    this function will blow that fuse.
*
* @param[in] raw_value   the raw value we want to write to this fuse.
*
* @param[in] fuse_address   the raw fuse address
*
* @par Dependencies
*    None
* 
* @retval
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error
* 
*/
static bl_error_type boot_rollback_blow_raw_fuse
(
  uint32 raw_value,
  const uint32 *fuse_address
)
{
  uint32 *fuse_msb_ptr = 0;
  uint32 *fuse_lsb_ptr = 0;
  bl_error_type status = BL_ERR_NONE;
  uint32 row_base_address = 0;
  qfprom_row_data = 0;
  qfprom_service_api_status = QFPROM_NO_ERR;

  
  /* First we need to determin if this fuse address is 
     MSB of a fuse row or a LSB of fuse row
     if it is multiple of 8 then it's LSB 
     if it is multiple of 4 then it's MSB*/
     
  do
  {  
    if( ((uint32)fuse_address % 8) == 0)
    {
      /*
        The fuse base address is multiple of 8
        The fuse we want to blow is located in the LSB of a fuse row.
        This is what we are going to pass to tz as fuse row data:
        
        |----MSB----|----LSB----|
        |-org value-|-raw value-|
      */
      /* First we need to copy the orginal value of the msb of the fuse row 
         to our fuse data buffer */
      fuse_msb_ptr = (uint32 *)((uint32)&qfprom_row_data + 4);
      *fuse_msb_ptr = *(fuse_address + 1);
      
      /* Now copy the lsb value we want to blow to the lsb of fuse data buffer */
      qfprom_row_data = qfprom_row_data | raw_value;
      row_base_address = (uint32)fuse_address;
    }
    else if ( ((uint32)fuse_address % 4) == 0)
    {
      /*
        The fuse base address is not multiple of 8 but multiple of 4
        The fuse we want to blow is located in the MSB of a fuse row.
        This is what we are going to pass to tz as fuse row data:
        
        |-----MSB---|----LSB-----|
        |-raw value-|-org value--|
      */
      /* First we need to copy the orginal value of the lsb of the fuse row 
         to our fuse data buffer */
      fuse_lsb_ptr = (uint32 *)&qfprom_row_data;
      *fuse_lsb_ptr = *(fuse_address - 1);
      
      /* Now copy the msb value we want to blow to the msb of fuse data buffer */      
      qfprom_row_data = qfprom_row_data |(((uint64)raw_value) << 32);
      row_base_address = (uint32)fuse_address - 4;
    }
    else
    {
      /* The fuse address is not multiples of 32 and 64, should never happen*/
      status = BL_ERR_CODE_ROLLBACK_VERSION_VERIFY_FAIL;
      break;
    }
    
    /* Call TZ and pass over the fuse row base address and fuse row data 
       TZ will blow fuse and return back */
    if (E_SUCCESS != boot_fastcall_tz_4arg (TZ_WRITE_ROLLBACK_FUSE_CMD, 
                                            row_base_address, 
                                            &qfprom_row_data, 
                                            0,
                                            &qfprom_service_api_status))
    {
      /* Tz returns a negative status value in case of error*/
      status = BL_ERR_CODE_ROLLBACK_VERSION_BLOW_FAIL;
    }

    /* Check if blow operation went ok */
    if(qfprom_service_api_status != QFPROM_NO_ERR)
    {
       status = BL_ERR_CODE_ROLLBACK_VERSION_BLOW_FAIL;
    }    
    
  }while(0);
  
  return status;
}


/*===========================================================================

**  Function : boot_rollback_blow_fuse_version

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
static bl_error_type boot_rollback_blow_fuse_version
(
  struct boot_version_rollback_img_set *rollback_img_set
)
{
  uint32 current_version = 0;
  uint32 fuse_lsb_max = 0;
  uint32 fuse_msb_max = 0;
  uint32 fuse_mid_max = 0;
  uint32 version_to_blow_lsb = 0;
  uint32 version_to_blow_msb = 0;
  uint32 version_to_blow_mid = 0;
  
  bl_error_type blow_success = BL_ERR_NONE;
  
  /* current_version is the version number that is obtained from secboot cert */
  current_version = rollback_img_set->current_set_version_num;
  
  /* Max version number that lsb version fuse can hold */
  fuse_lsb_max = boot_count_set_bits(rollback_img_set->version_lsb_mask);
  
  /* Max version number that msb version fuse can hold */
  fuse_msb_max = boot_count_set_bits(rollback_img_set->version_msb_mask);
  
  do
  {
    /* 
      First step: caculate the lsb and msb raw fuse value 
      fuse raw value is calculated as 2^n - 1 , n is the version number we want to blow
    */
    
    /* If both fuse_msb_max and fuse_lsb_max are filled, it means this image set have both lsb and msb version fuses
       this version number may span between multiple fuses */
    if ((fuse_msb_max != 0) && (fuse_lsb_max != 0))
    {
      /* In this case, both msb and lsb fuse base addresses must exist */
      if ((rollback_img_set->version_msb_base_addr == NULL) &&
           (rollback_img_set->version_lsb_base_addr == NULL))
      {
        return BL_ERR_CODE_ROLLBACK_VERSION_VERIFY_FAIL;
      }
      
      /* There is a possibility with bitmasks 34 bits or larger that
         the fuses can span 3 uint32's.  We must account for this by
         looking at the address span between the lsb base address and
         msb base address.  If span is greater than 4 bytes there is an
         entire 32 bit field or more that must be evaluated. */
      if (((uint32)rollback_img_set->version_msb_base_addr - (uint32)rollback_img_set->version_lsb_base_addr) > sizeof(uint32))
      {
        /* NOTE: here we assume using all the bits in the (middle) fuse address 
          between lsb base and msb base to count for the max version number */
        fuse_mid_max = boot_count_set_bits(0xFFFFFFFF);
      }

      /* If current version is smaller than max version */
      if (current_version < fuse_lsb_max)
      {
        /* blow the current version value */
        version_to_blow_lsb =  (0x2 << (current_version - 1)) - 1;
      }
      else if (current_version < fuse_lsb_max + fuse_mid_max)
      {
        /* we need to blow the max on lsb fuse */
          version_to_blow_lsb = (0x2 << (fuse_lsb_max - 1)) - 1;
		
        /* The version number stored in middle fuse should be (current version - max of lsb fuse) */
        version_to_blow_mid = (0x2 << (current_version - fuse_lsb_max - 1))- 1;          
      }
      else if (current_version < fuse_lsb_max + fuse_mid_max+ fuse_msb_max)
      {
        /* we need to blow the max on lsb fuse and the max on middle fuse */
        version_to_blow_lsb = (0x2 << (fuse_lsb_max - 1)) - 1;
        version_to_blow_mid = (0x2 << (fuse_mid_max - 1)) - 1;
		  
        /* The version number stored in MSB fuse should be (current version - max of lsb fuse - max of mid fuse) */
        version_to_blow_msb = (0x2 << (current_version - fuse_lsb_max - fuse_mid_max - 1)) - 1;          
      }
      /* If current version is bigger or euqals to fuse_lsb_max + fuse_mid_max + fuse_msb_max we blow both all fuses to max*/
      else
      {
        version_to_blow_lsb = (0x2 << (fuse_lsb_max - 1)) - 1;
        version_to_blow_mid = (0x2 << (fuse_mid_max - 1)) - 1;
        version_to_blow_msb = (0x2 << (fuse_msb_max - 1)) - 1;
      }
    }
    /* If either fuse_msb_max or fuse_lsb_max is 0, 
       it means that this image set doesn't have either msb or lsb version fuse */
    else if (fuse_msb_max == 0)
    {
      /* If current version is smaller than the max version we blow the current version*/
      if(current_version < fuse_lsb_max)
      {
        version_to_blow_lsb =  (0x2 << (current_version - 1))- 1;
      }
      /* If current version is bigger or equals to max then we blow the max */
      else
      {
        version_to_blow_lsb =  (0x2 << (fuse_lsb_max - 1))- 1;
      }
    }
    else /* fuse_lsb_max == 0 */
    {
      /* If current version is smaller than the max version we blow the current version*/
      if(current_version < fuse_msb_max)
      {
        version_to_blow_msb =  (0x2 << (current_version - 1))- 1;
      }
      /* If current version is bigger or equals to max then we blow the max */
      else
      {
        version_to_blow_msb =  (0x2 << (fuse_msb_max - 1))- 1;
      }
    }
	
    
    /* 
      Second step: blow the fuse 
    */
    /* Shift the raw value to appropriate location */
    version_to_blow_lsb = version_to_blow_lsb << rollback_img_set->version_lsb_shift;
    version_to_blow_msb = version_to_blow_msb << rollback_img_set->version_msb_shift; 
    
    /* Blow LSB first if LSB has valid value */
    if (blow_success == BL_ERR_NONE && version_to_blow_lsb != 0)
    {
      blow_success = boot_rollback_blow_raw_fuse(version_to_blow_lsb, rollback_img_set->version_lsb_base_addr);
    }
    
    /* If LSB is succesfully blown then blow middle fuse if it has valid value 
	NOTE: here we assume all 32 bits in the middle fuse (between LSB and MSB) are used for the same image version control */
    if (blow_success == BL_ERR_NONE && version_to_blow_mid != 0)
    {
      blow_success = boot_rollback_blow_raw_fuse(version_to_blow_mid, (uint32 *)(rollback_img_set->version_lsb_base_addr + 1));
    }    
    
    /* If LSB is succesfully blown then blow MSB if MSB has valid value */
    if(blow_success == BL_ERR_NONE && version_to_blow_msb != 0)
    {
      blow_success = boot_rollback_blow_raw_fuse(version_to_blow_msb, rollback_img_set->version_msb_base_addr);
    }    
    
  }while(0);
  
  return blow_success;
}

/*===========================================================================

                PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

**  Function : boot_rollback_init_img_set_table

** ==========================================================================
*/
/*!
* 
* @brief
*    This function initializes the rollback version image set table.
*
* @param[in] img_set_table pointer to the table
*
* @param[in] img_set_num number of images sets the table contains
*
*
* @par Dependencies
*    None
* 
* @retval None                                 
* 
* 
*/
void boot_rollback_init_img_set_table
(
  bl_shared_data_type *bl_shared_data,
  struct boot_version_rollback_img_set *img_set_table,
  uint32 img_set_num
)
{
  boot_rollback_version_img_set_table_ptr = img_set_table;
  boot_rollback_version_img_set_size = img_set_num;
  
  /* Determin if hw auth is enabled or not */  
  BL_VERIFY((boot_is_auth_enabled(&is_auth_enabled) == BL_ERR_NONE), 
             BL_ERR_IMG_SECURITY_FAIL); 
}


/*===========================================================================

**  Function : boot_rollback_get_fuse_version

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns the image version that's stored on the fuse given a
*    image type
*
* @param[in] img_sw_type secure image type
*
* @param[out] fuse_version pointer to the image version to be populated
*
*
* @par Dependencies
*    None
* 
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error                              
* 
* 
*/
bl_error_type boot_rollback_get_fuse_version
(
  secboot_sw_type img_sw_type,
  uint32 *fuse_version
)
{
  bl_error_type status = BL_ERR_NONE;
  
  /* First get the set the target image belongs to */
  struct boot_version_rollback_img_set *rollback_img_set = 
                boot_rollback_get_set_by_img_type(img_sw_type);
                      
  if (rollback_img_set != NULL)
  {
    /* First check if rollback feature fuse and auth are both enabled */
    if(is_auth_enabled == TRUE &&
       boot_rollback_is_feature_enabled_on_set(rollback_img_set))
    {
      *fuse_version = boot_rollback_calculate_fuse_version(rollback_img_set);
    }
    else
    {
      /* If rollback feature fuse is not enabled on this set then default to 0*/
      *fuse_version = 0;
    }
  }
  else
  {
    /* Error if can't find this image */
    status = BL_ERR_CODE_ROLLBACK_VERSION_VERIFY_FAIL;
  }
  
  return status;
}


/*===========================================================================

**  Function : boot_rollback_update_img_version

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
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error                              
* 
* 
*/
bl_error_type boot_rollback_update_img_version
(
  secboot_sw_type img_sw_type,
  uint32 img_version
)
{
  bl_error_type status = BL_ERR_NONE;
  
  /* First get the set the target image belongs to */
  struct boot_version_rollback_img_set *rollback_img_set = 
                boot_rollback_get_set_by_img_type(img_sw_type);

  if (rollback_img_set != NULL)
  {
    /* First check if rollback feature fuse is enabled */
    /* First check if rollback feature fuse and auth are both enabled */
    if(is_auth_enabled == TRUE &&
       boot_rollback_is_feature_enabled_on_set(rollback_img_set))
    {
      /* Only update if current image version is greater than set version */
      if(img_version > rollback_img_set->current_set_version_num)
      {
        rollback_img_set->current_set_version_num = img_version;
      }
    }
  }
  else
  {
    /* Error if can't find this image */
    status = BL_ERR_CODE_ROLLBACK_VERSION_VERIFY_FAIL;
  }
  
  return status;
}

/*===========================================================================

**  Function : boot_rollback_update_fuse_version

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
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error                              
* 
* 
*/
bl_error_type boot_rollback_update_fuse_version
(
  secboot_sw_type img_sw_type
)
{
  struct boot_version_rollback_img_set *rollback_img_set = 
                boot_rollback_get_set_by_img_type(img_sw_type);
  bl_error_type status = BL_ERR_NONE;
  uint32 fuse_version = 0;
  
  if (rollback_img_set != NULL)
  {
    /* First check if rollback feature fuse and auth are both enabled */
    if(is_auth_enabled == TRUE &&
       boot_rollback_is_feature_enabled_on_set(rollback_img_set))
    {
      /* Get the fuse version and compare to set version */
      fuse_version = boot_rollback_calculate_fuse_version(rollback_img_set);
      
      if(rollback_img_set->current_set_version_num > fuse_version)
      { 
        /*Blow the fuse if set version is greater than fuse version */
        status = boot_rollback_blow_fuse_version(rollback_img_set);
      }
    }
  }
  else
  {
    /* Error if can't find this image */
    status = BL_ERR_CODE_ROLLBACK_VERSION_VERIFY_FAIL;    
  }
  
  return status;
}
