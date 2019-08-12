#ifndef DLOAD_DEBUG_H
#define DLOAD_DEBUG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              M E M O R Y   D E B U G   I N F O R M A T I O N 

GENERAL DESCRIPTION
  This module defines the memory debug information to be sent to host

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_dload_debug.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/14   ck      Renamed boot_dload_dump_qsee to boot_dload_dump_security_regions
05/14/14   ck      Restored boot_dload_dump_qsee
03/17/14   ck      Added dload_mem_debug_zero_peripheral_memory
12/18/13   ck      Removed boot_dload_dump_tz
11/21/13   ck      Removed boot_set_tz_execute_status
11/12/13   kedara  Added changes to support SBL crash dumps
10/11/13   jz      Added remapping functions
09/25/13   aus     Added support for retail unlock dumps
07/01/13   dh      Add 64 bits memory dump support
02/15/13   kedara  Added boot_set_tz_execute_status.
02/15/13   dh      Add boot_dload_dump_tz
09/14/11   dh      rename to boot_dload_debug.h
07/23/10   niting  Added API to support fixed string length required by Sahara.
06/07/10   niting  Added DLOAD_DEBUG_STRLEN_BYTES constant for strings
05/31/06   tbg     Moved target specific #defines to doad_debug_xxxx.c 
10/20/05   tbg     Ported to msmshared. Moved some functions to target
                   specific file dload_debug_xxxx.c
08/01/05   ih      Created.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "boot_sbl_if.h"

/*===========================================================================

                     EXTERNAL DEFINITIONS FOR MODULE

===========================================================================*/

typedef enum
{
  OPTIONAL_DEF_NO_SAVE = 0,     /* optional, but do not save by default */
  OPTIONAL_DEF_SAVE,            /* optional, but save by default */
  MANDATORY                     /* mandatory, no user modification */
} dload_save_pref_type;

#define DLOAD_DEBUG_STRLEN_BYTES 20

/* The following are defined for checking if address remapping is needed */
#define DDR_DUMP_SIZE_MB 1024  // 1GB
#define DDR_DUMP_REMAP_BASE (DDR_MEM_BASE + DDR_DUMP_SIZE_MB << CONVERT_TO_MB_SHIFT)

/* Max Size limit for Silicon Vendor internal memory, such as ocimem: 128MB */
#define SV_MEMORY_SIZE_LIMIT  (0x8000000)

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_INIT

DESCRIPTION
  This function initializes dload_debug_info[]

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_mem_debug_init(void);


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
void dload_debug_add_region(dload_save_pref_type save_pref, uint64 mem_base, uint64 length, 
                            char *desc, char *filename);

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
void dload_debug_add_restricted_region(uint64 mem_base, uint64 length);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_TARGET_INIT

DESCRIPTION
  This function initializes dload_debug_info[] for the specific target.
  Must be implemented in dload_debug_xxxx.c file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_mem_debug_target_init(void);

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
uint32 dload_mem_debug_len(void);

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
void dload_mem_debug_len_init(void);

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
boolean dload_mem_debug_verify_addr(uint64 base, uint64 len);


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
boolean dload_mem_debug_is_restricted_addr(uint64 base, uint64 len);


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
uint32 dload_mem_debug_num_ent(void);


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
uint8 dload_mem_debug_save_pref(uint8 i);

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
uint64 dload_mem_debug_mem_base(uint8 i);

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
uint64 dload_mem_debug_mem_length(uint8 i);

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
char *dload_mem_debug_desc(uint8 i);

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
char *dload_mem_debug_filename(uint8 i);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_SUPPORTED

DESCRIPTION
  This function returns if memory debug is supported.
  Must be implemented in dload_debug_xxxx.c file.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_supported(void);

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
boolean dload_mem_debug_is_device_retail_unlocked(void);

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
uint32 dload_mem_debug_table_addr(void);

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
void dload_mem_debug_zero_peripheral_memory(void);


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
void boot_dload_dump_security_regions(bl_shared_data_type *bl_shared_data );


#endif  /* DLOAD_DEBUG_H */
