/*====================================================================
 *
 * FILE: flash_hal_properties.c
 *
 * SERVICES: Set properties for Flash HAL
 *
 * DESCRIPTION: This file contain the interface API to set the
 *              properties for FLASH HAL.
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2009-2010,2012, 2014 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/flash_hal_properties.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/24/14     sb      Fix KW warnings
 * 09/05/12     sv      add cache maintenance APIS support
 * 07/30/12     sv      Remove DM specific codes
 * 02/15/12     sb      Add support for memutils in assembly
 * 10/21/10     sv      Add support to set wait API through hal properties
 * 02/05/10     mcg     Support to get DMOV configs from DAL
 * 07/21/09     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include <flashi_hal.h>
#include <string.h>

/* Function declarations */
void flash_hal_asm_memcpy(void *dest, const void *src, uint32 num_bytes);

/* Flash HAL Property structure which has the desired memory config
 * properties that can be configured from Flash DAL
 */
struct flash_hal_mem_cfg
{
  void *(*memcpy_fptr)(void *, const void *, uint32); /* fptr to memcpy routine */
  void *(*memset_fptr)(void *, int, uint32);          /* fptr to memset routine */
};

/*
 * Flash HAL Property structure which has the desired wait properties 
 * that can be configured from Flash DAL
 */
struct flash_hal_wait_cfg
{
  void (*wait_fptr)(uint32); /* fptr to wait routine */
};

struct flash_hal_cache_cfg
{
  void (*cache_inval_fptr)(uint32 addr, uint32 len);
  void (*cache_clean_fptr)(uint32 addr, uint32 len);
};

/* Default property values used by Flash HAL */
struct flash_hal_properties
{
  struct flash_hal_mem_cfg mem_cfg;
  struct flash_hal_wait_cfg wait_cfg;
  struct flash_hal_cache_cfg cache_cfg;
};

struct flash_hal_properties flash_hal_prop_tbl = 
  {{NULL, NULL}, {NULL}, {NULL, NULL}};

/* memcpy routine which copies data from source to destination
 * in a loop. This is introduced since some processors has issues
 * with standard memcpy routine while accessing uncached memory
 */
void *flash_hal_handle_unc_memcpy(void *dest, const void *src, uint32 num_bytes)
{
  uint8 * c_dest = (uint8 *) dest;
  uint8 * c_src = (uint8 *) src;
  uint32 * l_dest = (uint32 *) dest;
  uint32 * l_src = (uint32 *) src;

  if (((((uint32)dest) & 3) || (((uint32)src) & 3)) && num_bytes)
  {
    while (((((uint32)c_dest) & 3) || (((uint32)c_src) & 3)) && num_bytes)
    {
      *c_dest++ = *c_src++;
      num_bytes--;
    }
  }

  l_dest = (uint32 *) c_dest;
  l_src = (uint32 *) c_src;

  while (num_bytes>=4)
  {
    *l_dest++= *l_src++;
    num_bytes -= 4;
  }

  c_dest = (uint8 *) l_dest;
  c_src = (uint8 *) l_src;

  if (num_bytes)
  {
    while (num_bytes--)
    {
      *c_dest++ = *c_src++;
    }
  }

  return dest;
}

/* Calls std memcpy routine */
void *flash_hal_handle_std_memcpy(void *dest, const void *src, uint32 num_bytes)
{
  return memcpy(dest, src, (size_t)num_bytes);
}

/* Calls std memset routine */
void *flash_hal_handle_std_memset(void *dest, int c, uint32 num_bytes)
{
  return memset(dest, c, (size_t)num_bytes);
}

/* memcpy routine in assembly. This is introduced in boot level flash driver
 * to improve the bootup time 
 */
void *flash_hal_handle_opti_memcpy(void *dest, const void *src, uint32 num_bytes)
{
//  flash_hal_asm_memcpy(dest, src, num_bytes);
  return dest;
}

/* memset routine which sets memory location(s) with
 * a byte data. This is introduced since some processors has issues
 * with standard memset routine while accessing uncached memory
 */
void *flash_hal_handle_unc_memset(void *dest, int c, uint32 num_bytes)
{
  uint32 i;
  uint8 *dest_ptr;

  dest_ptr = (uint8 *)dest;

  /*TBD: Optimizations */
  for(i = 0; i < num_bytes; i++)
  {
    dest_ptr[i] = (uint8)c;
  }
  return dest;
}

/* Configure the properties for Flash HAL */
void flash_hal_set_property(enum flash_hal_property id, void *value)
{
  switch (id)
  {
    case FLASH_HAL_PROPERTY_USE_STD_MEMUTIL:
      flash_hal_prop_tbl.mem_cfg.memcpy_fptr = flash_hal_handle_std_memcpy;
      flash_hal_prop_tbl.mem_cfg.memset_fptr = flash_hal_handle_std_memset;
      break;

    case FLASH_HAL_PROPERTY_USE_INT_MEMUTIL:
      flash_hal_prop_tbl.mem_cfg.memcpy_fptr = flash_hal_handle_unc_memcpy;
      flash_hal_prop_tbl.mem_cfg.memset_fptr = flash_hal_handle_unc_memset;
      break;

    case FLASH_HAL_PROPERTY_USE_INT_ASM_MEMUTIL:
      flash_hal_prop_tbl.mem_cfg.memcpy_fptr = flash_hal_handle_opti_memcpy;
      flash_hal_prop_tbl.mem_cfg.memset_fptr = flash_hal_handle_unc_memset;
      break;

    case FLASH_HAL_PROPERTY_WAIT_CFG:
      flash_hal_prop_tbl.wait_cfg.wait_fptr = (void (*)(uint32))value;
      break;
	
    case FLASH_HAL_PROPERTY_CACHE_INVAL_CFG:
      flash_hal_prop_tbl.cache_cfg.cache_inval_fptr = (void (*)(uint32, uint32))value;
      break;

    case FLASH_HAL_PROPERTY_CACHE_CLEAN_CFG:
      flash_hal_prop_tbl.cache_cfg.cache_clean_fptr = (void (*)(uint32, uint32))value;
      break;

	default:
	  break;
  }
}

/* memory copy wrapper for Flash HAL */
void *flash_hal_memcpy(void *dest, const void* src, uint32 len)
{
  return flash_hal_prop_tbl.mem_cfg.memcpy_fptr(dest, src, len);
}

/* memory set wrapper for Flash HAL */
void *flash_hal_memset(void *dest, int c, uint32 len)
{
  return  flash_hal_prop_tbl.mem_cfg.memset_fptr(dest, c, len);
}


/* wait wrapper for Flash HAL */
void flash_hal_busywait(uint32 wait_time_us)
{
  flash_hal_prop_tbl.wait_cfg.wait_fptr(wait_time_us);
}

/* cache clean wrapper for Flash HAL */
void flash_hal_cache_clean(uint32 addr, uint32 len)
{
  if(flash_hal_prop_tbl.cache_cfg.cache_clean_fptr)
  {
    flash_hal_prop_tbl.cache_cfg.cache_clean_fptr(addr, len);
  }
}

/* cache invalidate wrapper for Flash HAL */
void flash_hal_cache_inval(uint32 addr, uint32 len)
{
  if(flash_hal_prop_tbl.cache_cfg.cache_inval_fptr)
  {
    flash_hal_prop_tbl.cache_cfg.cache_inval_fptr(addr, len);
  }
}
