/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          MEMORY DEBUG INFORMATION 

GENERAL DESCRIPTION
  This module defines the memory debug information to be sent to host

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_dload_debug.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/15   sk      Replaced qmemcpy with SBL_memcpy to add neon instruction support
06/20/14   ck      Modified and renamed boot_dload_dump_qsee to boot_dload_dump_security_regions
06/30/14   aus     Added integer overflow checks.
06/11/14   jz      Fixed boot_is_auth_enabled to check return status in the caller
05/14/14   ck      Restored boot_dload_dump_qsee but modified for Bear family
03/17/14   ck      Added logic to search for QSEE header info from sbl qsee interface
03/17/14   ck      Added dload_mem_debug_zero_peripheral_memory function
03/17/14   ck      Replaced stubs with calls to seccfg driver
01/10/14   ck      Bear family does not need to share ram_dumped_allowed and device_retail_unlocked
12/18/13   ck      Removed boot_dload_dump_tz and boot_dload_is_tz_dump_allowed
12/10/13   ck      boot_is_auth_enabled does not have a return value now
12/06/13   ck      Replaced calls to boot_pbl_is_auth_enabled with boot_is_auth_enabled
11/21/13   ck      Removed boot_set_tz_execute_status
11/21/13   ck      Started changing logic in dload_mem_debug_supported for Bear
11/12/13   kedara  Added changes to support SBL crash dumps
10/11/13   jz      Added remapping functions
09/25/13   aus     Added support for retail unlock dumps
07/01/13   dh      Add 64 bits memory dump support
05/31/13   sy      Replace memcpy to strlcpy
04/24/13   jlaver  Add security check for boot_dload_dump_tz
02/28/13   kedara  Support TZ ramdump check only after TZ is loaded.
02/15/13   dh      Add boot_dload_dump_tz
02/11/13   dh      Add dload_mem_debug_supported
02/03/12   kpa     Fix integer overflow issues with end address calculation. 
09/23/11   dh      move mob_sw_rev define from boot_dload_stubs.c to this file
09/14/11   dh      Rename to boot_dload_debug.c, remove unused include header
08/09/10   kpa     Merge code from 9x15 build for PW1.2. 
10/13/10   plc     Updating to not depend on time api, not depricated clk api  
07/23/10   niting  Added API to support fixed string length required by Sahara.
06/07/10   niting  Added support for Sahara memory dump.
01/10/06   tbg     Modified to compile with boot loader images.
10/20/05   tbg     Ported to msmshared. Moved some functions to target
                   specific file dload_debug_xxxx.c
08/01/05   ih      Created.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_dload.h"
#include "boot_dload_debug.h"
#include "mobile.h"
#include <stdio.h>
#include <string.h>
#include "boot_comdef.h"
#include "IxErrno.h"
#include "boot_util.h"
#include "boot_shared_imem_cookie.h"
#include "boot_authenticator.h"
#include "boot_extern_bus_interface.h"
#include "boot_extern_ddr_interface.h"
#include "boot_extern_seccfg_interface.h"

#ifdef FEATURE_PMIC_RTC
#include "time_svc.h"
#endif

#include "boot_util.h"

#ifdef FEATURE_DLOAD_MEM_DEBUG
#define NUM_REGIONS 26 
#define SCRIPT_BUF_SIZE 2048

#define POS(x) (x > 0 ? x : 0)


/* Mobile Firmware Marketing Software Revision also in mobile.c */
NOTICE_TYPE mob_sw_rev[] = BUILD_ASIC "-" BUILD_TARGET "-" BUILD_VER;

#ifndef BOOT_MEMORY_DEBUG_64BIT

/*debug structure for regular 32 bits ram dump */
typedef struct 
{
  byte save_pref;
  uint32 mem_base;
  uint32 length;
  char desc[DLOAD_DEBUG_STRLEN_BYTES];
  char filename[DLOAD_DEBUG_STRLEN_BYTES];
} dload_debug_type;

#else

/*debug structure for 64 bits ram dump */
typedef struct 
{
  uint64 save_pref;  /* force 8 bytes alignment */
  uint64 mem_base;
  uint64 length;
  char desc[DLOAD_DEBUG_STRLEN_BYTES];
  char filename[DLOAD_DEBUG_STRLEN_BYTES];
} dload_debug_type;

#endif

struct dload_restricted_region
{
  uint64 mem_base;
  uint64 length;
};


/* Since memory dumps are done in 32-bit chunks, the script has to be aligned
 * as well to get dumped correctly. We also pad it with non-essential chars
 * so nothing gets truncated at the end
 */
static char __align(4) cmm_script[SCRIPT_BUF_SIZE];

static dload_debug_type dload_debug_info[NUM_REGIONS];
static uint32 real_num_regions = 0;
static struct dload_restricted_region dload_restricted_regions[NUM_REGIONS];
static uint32 num_restricted_regions = 0;
static boolean region_table_fixed_length = FALSE;


/*===========================================================================

                     EXTERNAL DEFINITIONS FOR MODULE

===========================================================================*/
/* Shared data between SBL */
extern struct bl_shared_data_type bl_shared_data;


/*===========================================================================

FUNCTION DLOAD_DEBUG_ADD_REGION

DESCRIPTION
  This function adds a region to dload_debug_info[]

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_debug_add_region
(
  dload_save_pref_type save_pref, 
  uint64 mem_base, 
  uint64 length, 
  char *desc, 
  char *filename
)
{
  uint32 i = real_num_regions;
  uint32 desc_length = strlen(desc);
  uint32 filename_length = strlen(filename);

  /* Make sure we don't overrun array and align memory regions */
  if ((desc_length < DLOAD_DEBUG_STRLEN_BYTES) &&
      (filename_length < DLOAD_DEBUG_STRLEN_BYTES) &&
      (i < NUM_REGIONS))
  {  
    dload_debug_info[i].save_pref = (byte)save_pref; 
    dload_debug_info[i].mem_base  = mem_base & ~3; 
    dload_debug_info[i].length    = length  & ~3; 
    strlcpy(dload_debug_info[i].desc, desc, DLOAD_DEBUG_STRLEN_BYTES);
    strlcpy(dload_debug_info[i].filename, filename, DLOAD_DEBUG_STRLEN_BYTES);
  
    real_num_regions++; 
  }
}

/*===========================================================================

FUNCTION DLOAD_DEBUG_ADD_RESTRICTED_REGION

DESCRIPTION
  This function adds a region to dload_restricted_regions[] that cannot
  be directly accessible by H/W DMA.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_debug_add_restricted_region
(
  uint64 mem_base, 
  uint64 length
)
{
  uint32 i = num_restricted_regions;

  /* Make sure we don't overrun array and align memory regions */
  if (i < NUM_REGIONS)
  {
    dload_restricted_regions[i].mem_base  = mem_base & ~3; 
    dload_restricted_regions[i].length    = length  & ~3; 
    num_restricted_regions++; 
  }
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_INIT

DESCRIPTION
  This function call the function to do target specific initialization,
  and sets up the script.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_mem_debug_init(void)
{
  int j, bufsize = SCRIPT_BUF_SIZE, n;
  char *s;
#ifndef BUILD_BOOT_CHAIN
#ifdef FEATURE_PMIC_RTC
  time_julian_type error_time;
#endif
#endif

  /*
  ** Reset number of regions, to ensure only those added by the function
  ** dload_mem_debug_target_init() will be used.
  */
  real_num_regions = 0;

  /* Clear region table */
  qmemset(dload_debug_info, 0, sizeof(dload_debug_type)*NUM_REGIONS);

  /* Initialize default table length */
  region_table_fixed_length = FALSE;
  
  /* Set up memory regions and clocks for specific target */
  dload_mem_debug_target_init();
  
  /* Initialize the script for the regions we just added
   */
  s = cmm_script;

  /* Add mobile ID */
  n = snprintf(s, POS(bufsize), "; Build ID: %s\n", mob_sw_rev);
  s += n;
  bufsize -= n;

#ifdef BUILD_BOOT_CHAIN
  /* Time stamp currently unavailable from boot loader */
  n = snprintf(s, POS(bufsize), 
               "; Time Stamp unavailable from boot loader.\n");
  s += n;
  bufsize -= n;  
#else
#ifdef FEATURE_PMIC_RTC
  /* Add time stamp */
  time_get_julian(&error_time);
  n = snprintf(s, POS(bufsize), 
        "; Time Stamp: %02d/%02d/%04d %02d:%02d:%02d (M/D/Y H:M:S)\n",
        error_time.month,error_time.day,error_time.year, error_time.hour, 
        error_time.minute,error_time.second
      );
  s += n;
  bufsize -= n;
#endif
#endif

  for(j=0; j < real_num_regions; j++)
  {
    /* Move s to the end of string after printing the string */
    n = snprintf(s, POS(bufsize), 
                 "if OS.FILE(%s)\n"
                 "(\n"
#ifndef BOOT_MEMORY_DEBUG_64BIT
                 "  d.load.binary %s 0x%lx /noclear\n"
#else
                 "  d.load.binary %s 0x%llx /noclear\n"
#endif
                 ")\n",
                 
                 dload_debug_info[j].filename, 
                 dload_debug_info[j].filename,                  
                 dload_debug_info[j].mem_base);
    s += n;
    bufsize -= n;
  }
  /* Pad the end of the script so it does not get truncated when the length of
   * the region gets masked to 32-bit boundaries
   */
  (void)snprintf(s, POS(bufsize), "\n\n\n");

  dload_debug_add_region(MANDATORY, (uint32)cmm_script, (uint32)strlen(cmm_script), 
      "CMM Script", "load.cmm");
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_LEN

DESCRIPTION
  This function returns length of the debug info response

DEPENDENCIES
  None.

RETURN VALUE
  length of the debug info response

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_len(void)
{
  dload_debug_type *p = dload_debug_info;
  uint32 resp = 0, i;

  /* Return fixed length if set */
  if (region_table_fixed_length)
  {
    resp = (real_num_regions*sizeof(dload_debug_type));
  }
  else
  {
    for(i=0; i < real_num_regions; i++)
    {
      resp+=sizeof(p[i].save_pref) + sizeof(p[i].mem_base) + sizeof(p[i].length);
      resp+=strlen(p[i].desc) + 1;      // null-termination char
      resp+=strlen(p[i].filename) + 1;  // null-termination char
    }
  }
  return resp;
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_LEN_INIT

DESCRIPTION
  This function sets the length of the memory region table to be fixed length.
  This is used to add support for Sahara memory dumps that require a fixed
  length memory region table when dload_mem_debug_len called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_mem_debug_len_init(void)
{
  region_table_fixed_length = TRUE;
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_VERIFY_ADDR

DESCRIPTION
  This function checks to see if it belongs to a region of memory that can be
  dumped.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the address verifies, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_verify_addr(uint64 base, uint64 len)
{
  dload_debug_type *p = dload_debug_info;
  uint32 i;
  uint64 input_end_addr =0x0, table_end_addr =0x0;

  input_end_addr = base + len - 1;

  if((len == 0) || (input_end_addr < base))
    return FALSE;
  
  for(i=0; i < real_num_regions; i++)
  {
    if(base >= p[i].mem_base)
    {
      table_end_addr = (uint64)(p[i].mem_base) + (uint64)(p[i].length) - 1;
      if(input_end_addr <= table_end_addr)
        return TRUE;
      }
    }
  return FALSE;
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_IS_RESTRICTED_ADDR

DESCRIPTION
  This function checks to see if it belongs to a restricted region of memory
  that is not directly accessible by H/W DMA.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the address verifies, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_is_restricted_addr(uint64 base, uint64 len)
{
  struct dload_restricted_region *p = dload_restricted_regions;
  uint32 i;
  uint64 input_end_addr =0x0, table_end_addr =0x0;
 
  input_end_addr = base + len - 1;

  if((len == 0) || (input_end_addr < base))
    return FALSE;
	
  for(i=0; i < num_restricted_regions; i++)
  {
    if(base >= p[i].mem_base)
    {
      table_end_addr = (uint64)(p[i].mem_base) + (uint64)(p[i].length) - 1;
      if(input_end_addr <= table_end_addr)
        return TRUE;
      }
    }
  return FALSE;
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_IS_REMAP_NEEDED

DESCRIPTION
  This function checks to see if the address range needs to be remapped in order to be
  dumped.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the address verifies, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_is_remap_needed(uint64 base, uint64 len)
{
  dload_debug_type *region = dload_debug_info;
  uint32 i;
  uint64 input_end_addr =0x0, region_end_addr =0x0;

  input_end_addr = base + len - 1;

  if((len == 0) || (input_end_addr < base))
    return FALSE;  
  
  for (i=0; i < real_num_regions; i++)
  {
    if (base >= region[i].mem_base)
    {
      region_end_addr = (uint64)(region[i].mem_base) + (uint64)(region[i].length) - 1;
      /* Check if the input range falls into this region */
      if (input_end_addr <= region_end_addr)
      {
        /* Check if it's DDR region that needs to be remapped */
        if ((region[i].length > SV_MEMORY_SIZE_LIMIT) && (base > DDR_DUMP_REMAP_BASE))
          return TRUE;
        }
      }
    }
  return FALSE;
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_NUM_ENT

DESCRIPTION
  This function returns number of entries in the mem_debug_info table

DEPENDENCIES
  None.

RETURN VALUE
  number of entries in the mem_debug_info table

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_num_ent(void)
{
  return real_num_regions;
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_SAVE_PREF

DESCRIPTION
  This function returns the save preference of a given region

DEPENDENCIES
  None.

RETURN VALUE
  save preference of a given region
  0 if invalid region number
SIDE EFFECTS
  None.

===========================================================================*/
uint8 dload_mem_debug_save_pref(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].save_pref : 0);
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_MEM_BASE

DESCRIPTION
  This function returns the memory base of a given region

DEPENDENCIES
  None.

RETURN VALUE
  memory base of a given region
  0 if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
uint64 dload_mem_debug_mem_base(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].mem_base : 0);
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_MEM_LENGTH

DESCRIPTION
  This function returns the memory length of a given region

DEPENDENCIES
  None.

RETURN VALUE
  memory length of a given region
  0 if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
uint64 dload_mem_debug_mem_length(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].length : 0);
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_DESC

DESCRIPTION
  This function returns the description of a given region

DEPENDENCIES
  None.

RETURN VALUE
  description of a given region
  NULL if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
char *dload_mem_debug_desc(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].desc : NULL);
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_FILENAME

DESCRIPTION
  This function returns the filename of a given region

DEPENDENCIES
  None.

RETURN VALUE
  filename of a given region
  NULL if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
char *dload_mem_debug_filename(uint8 i)
{
  return (i < real_num_regions ? dload_debug_info[i].filename : NULL);
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_TABLE_ADDR

DESCRIPTION
  This function returns address location of the memory debug region table.
  
DEPENDENCIES
  None.

RETURN VALUE
  Address location of the memory region table.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_table_addr(void)
{
  return (uint32) &dload_debug_info;
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_SUPPORT

DESCRIPTION
  This function returns if memory debug is supported.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_supported()
{
  boolean mem_dump_supported = FALSE;
  boot_boolean is_auth_enabled = FALSE;
  secboot_verified_info_type * sbl_verified_info =
    &(bl_shared_data.sbl_qsee_interface.boot_image_entry[0].image_verified_info);
  bl_error_type status;

  /* Memory debug is supported if one of the following is true:
     1) Auth is not enabled
     2) Qsee memory dump is allowed
     3) Device is retail unlocked
  */

  /* If authentication is enabled the seccfg driver must be called to
     determine if ramdumps are enabled. */
  status = boot_is_auth_enabled(&is_auth_enabled);
  BL_VERIFY((status == BL_ERR_NONE), BL_ERR_IMG_SECURITY_FAIL); 

  if (is_auth_enabled == TRUE)
  {
    /* Debug enabled and Retail unlocked values are in SBL's header. */
    if (boot_qsee_is_memory_dump_allowed(sbl_verified_info) ||
        boot_qsee_is_retail_unlocked(sbl_verified_info))
    {
      mem_dump_supported = TRUE;
    }
  }
  else
  {
    mem_dump_supported = TRUE;
  }
  
  return mem_dump_supported;
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_IS_DEVICE_RETAIL_UNLOCKED

DESCRIPTION
  This function returns if device is retail unlocked
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_is_device_retail_unlocked(void)
{
  secboot_verified_info_type * sbl_verified_info =
    &(bl_shared_data.sbl_qsee_interface.boot_image_entry[0].image_verified_info);
  

  /* Retail unlocked info is in SBL's header. */
  return boot_qsee_is_retail_unlocked(sbl_verified_info);
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_ZERO_PERIPHERAL_MEMORY

DESCRIPTION
  This function zeros peripheral memory for use in retail unlocked scenario
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_mem_debug_zero_peripheral_memory(void)
{
  boot_qsee_zero_peripheral_memory();
}


/*===========================================================================

FUNCTION  BOOT_DLOAD_DUMP_SECURITY_REGIONS

DESCRIPTION
  Some customers like to execute ramdumps before or during LK.  When this
  occurs QSEE and QHEE have already been reloaded and executed.  In this case
  a backup of the QSEE and QHEE memory regions is taken so the delayed ramdump
  has proper data.  A check is made to see if the previous executing QSEE
  and QHEE have populated their shared IMEM areas with values indicating a
  free region to backup to.  
  
DEPENDENCIES
  QSEE and QHEE must have executed once and populated its shared IMEM properly.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void boot_dload_dump_security_regions(bl_shared_data_type *bl_shared_data )
{
  struct security_region_dload_dump_shared_imem_cookie_type *
    security_region_dload_dump_cookie_ptr;
  

  /* Backup QSEE if indicated by magic number. */
  security_region_dload_dump_cookie_ptr =
    (struct security_region_dload_dump_shared_imem_cookie_type *)
    (SHARED_IMEM_BASE + QSEE_DLOAD_DUMP_SHARED_IMEM_OFFSET);
                          
  if(security_region_dload_dump_cookie_ptr->magic_num == 
     QSEE_DLOAD_DUMP_SHARED_IMEM_MAGIC_NUM)
  {
    /* Only copy QSEE memory region. */
    if(security_region_dload_dump_cookie_ptr->src_addr && 
       security_region_dload_dump_cookie_ptr->dst_addr &&
       security_region_dload_dump_cookie_ptr->size <= SCL_QSEE_TOTAL_SIZE)
    {
      sbl_memcpy(security_region_dload_dump_cookie_ptr->dst_addr, 
              security_region_dload_dump_cookie_ptr->src_addr, 
              security_region_dload_dump_cookie_ptr->size);
    }
  }


  /* Backup QHEE if indicated by magic number. */
  security_region_dload_dump_cookie_ptr =
    (struct security_region_dload_dump_shared_imem_cookie_type *)
    (SHARED_IMEM_BASE + QHEE_DLOAD_DUMP_SHARED_IMEM_OFFSET);
                          
  if(security_region_dload_dump_cookie_ptr->magic_num == 
     QHEE_DLOAD_DUMP_SHARED_IMEM_MAGIC_NUM)
  {
    /* Only copy QHEE memory region. */
    if(security_region_dload_dump_cookie_ptr->src_addr && 
       security_region_dload_dump_cookie_ptr->dst_addr &&
       security_region_dload_dump_cookie_ptr->size <= SCL_QHEE_TOTAL_SIZE)
    {
      sbl_memcpy(security_region_dload_dump_cookie_ptr->dst_addr, 
              security_region_dload_dump_cookie_ptr->src_addr, 
              security_region_dload_dump_cookie_ptr->size);
    }
  }
}


#endif /* FEATURE_DLOAD_MEM_DEBUG */
