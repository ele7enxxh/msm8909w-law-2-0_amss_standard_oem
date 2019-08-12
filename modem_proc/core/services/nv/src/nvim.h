#ifndef NVIM_H
#define NVIM_H

/*===========================================================================

   N V   I T E M   M A N A G E R   H E A D E R   F I L E

DESCRIPTION
  All the declarations and definitions necessary to support the table  
  lookup of mapping between an NV item and the Flash File System files
  which simulate storage in an EEPROM.
  
Copyright (c) 2008-2013 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/services/nv/src/nvim.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
  
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/13   rks     NV SSM White listing support
09/25/09   pc      Dual SIM/Dual Standby support for LCU
05/27/09   sri     Adding function prototypes, included nvdiag.h
04/20/05   pc      Creation of NV2

===========================================================================*/

/* ======================================================================

                       INCLUDE FILES FOR MODULE

========================================================================*/
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "comdef.h"
#include "nv.h"
#include "nv_v.h"                  /* for nv_item_type */
#include "nvi.h"                 
#include "nv_items.h"
#include "nvdiag.h"

#if !defined(NV_LOCK)
#define NV_LOCK
#include "rex.h"
#define NV_GLOBAL_LOCK_INIT() rex_init_crit_sect (&nv_crit_sect)
#define NV_GLOBAL_LOCK() rex_enter_crit_sect (&nv_crit_sect)
#define NV_GLOBAL_UNLOCK() rex_leave_crit_sect (&nv_crit_sect)
#endif

#ifdef __cplusplus
extern "C" {
#endif
 

/* ======================================================================

                       DEFINITIONS AND DECLARATIONS 

========================================================================*/

#define NVIM_NONARRAY        1

/* #define NVIM_ITEM_INFO_TABLE_SIZE   NV_MAX_I */

#define NVIM_ITEM_INFO_TABLE_SIZE   NV_MAX_I

#define  NVIM_ITEM_NAME_MAX_LENGTH  120
#define  NVIM_FILE_NAME_MAX_LENGTH  128

/* The file containing the ESN and a_keys has a reserved area of FFs */ 
#define PERM_FILE_RESERVED  85     /* Reserved area, all 0xFF */

/* The file containing the factory data has a reserved area of FFs */ 
#define FACTORY_FILE_RESERVED  78     /* Reserved area, all 0xFF */

/* The factory data block has a fixed size */ 
#define NVIM_FACTORY_DATA_SIZE  128

/* Fixed value of dynamic pool area */
#define NVIM_DYNAMIC_POOL_CAPACITY 40960

/* The following defines are simply held over from NVM for convenience */
#define NVIKLTADDR        ((word) 0)

#define NV_FACTORY_RSVD       0x7F       /* give 127 bytes to factory test */

/* Start of factory reserved test area */
#define NVD_MAX_OFFSET       (0x1FFF - NV_FACTORY_RSVD)  /* 8kb */

/* Numbers of bytes represented by a bit in the allocation map */
#define NVIBLOCKSIZE       ((word)8)
/* Number of blocks in a long allocation block */
#define NVIBLOCKSPERLONGALLOC ((word)4)
/* Offset of data in a long block */
#define NVIDATAOFFSET    ((word)1)  
            
#define NVILONGBLOCK      ((word)(NVIBLOCKSIZE*NVIBLOCKSPERLONGALLOC))
#define NVILONGDATASIZE  ((word)(NVILONGBLOCK - NVIDATAOFFSET))

/* Maximum number of files containing SMS data */
#define NVIM_SMS_FILE_COUNT 101 

/* Maximum allowed NAMS, can not modify this value as it affects
   invariant data */
#define NVIM_MAX_NAMS 4

#define MAX_NV_CONTEXT 6
#define MIN_NV_CONTEXT 0

/*===========================================================================
                       TYPE DECLARATIONS
===========================================================================*/

typedef int nvim_item_size_type;


typedef struct {
  int      item_size;    /* Item size */
  uint16   array_size;   /* How many elements of this item */
  boolean  is_present;   /* Item present or not */
}nvim_item_info_type;

/*  NV access permission set based on caller */
typedef enum {
  NV_SSM_READ_WRITE_ALLOWED_S = 0x0,      /* Read and Write both are allowed */
  NV_SSM_WRITE_NOT_ALLOWED_S = 0x1,               /* Only Read is allowed */
  NV_SSM_READ_NOT_ALLOWED_S = 0x2,              /* Only Write is allowed */
  /* NV_SSM_WRITE_NOT_ALLOWED_S | NV_SSM_READ_NOT_ALLOWED_S is the access denied case */
  NV_SSM_WRITE_ONCE_S = 0x4,              /* One time write */
  NV_SSM_READ_SPC_PROTECTED_S = 0x8,      /* Read access with SPC protected */
  NV_SSM_WRITE_SPC_PROTECTED_S = 0x10,     /* Write access with SPC protected */
  NV_SSM_ACCESS_DENIED_S = 0x20,           /* Access denied */
  NV_SSM_DEFAULT_PERM_S = 0x40, /* white list default */
  NV_SSM_ENUM_MAX = 0x7FFFFFFF      /* Pad to 16 bits on ARM */
} nv_ssm_stat_enum_type;


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION NV_INIT

DESCRIPTION
  This function checks the major version number of the the NV task
  as stored at edit time, against the major version number of the
  NV EEPROM as read from NVM.  If the numbers are different then the
  function completely rebuilds the NV EEPROM.  If the major version
  number has not changed then the function checks if the number of
  items defined in NVI.H has increased from the internal NVM stored
  items count.  If so then only those new items are built.  Once the
  NV EEPROM has been initialized the function updates the stored
  major version number to the one contained in NVI.H.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  Rebuilding the NVM takes a significant amount of time, measured in
  seconds on serial eeprom devices, if NVM is rebuilt. Startup time 
  will vary with the type of EEPROM.

===========================================================================*/

LOCAL nv_stat_enum_type  
nv_init (void);


/*===========================================================================

FUNCTION NVIM_OP_GET_PRESENCE

DESCRIPTION
  This function determines whether an entry is valid (each of its field
  has a meaningful value), or empty/invalid entry.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the entry is valid and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean 
nvim_op_get_presence (
  nv_items_enum_type item_code   /* Item ID */
);


/*===========================================================================

FUNCTION NVIM_OP_GET_ARRAY_SIZE

DESCRIPTION
  This function returns the array size of an item, given its ID (item code).

DEPENDENCIES
  None.

RETURN VALUE
  Array size.

SIDE EFFECTS
  Only a meaningful return value if the entry is valid which is determined
  by calling nv_op_is_valid_entry

===========================================================================*/
uint16 
nvim_op_get_array_size (
  nv_items_enum_type   item_code    /* Item ID */
);


/*===========================================================================

FUNCTION NVIM_OP_GET_SIZE

DESCRIPTION
  This function returns the size of an item, given its ID (item code).

DEPENDENCIES
  None.

RETURN VALUE
  Item size.

SIDE EFFECTS
  Only a meaningful return value if the entry is valid which is determined
  by calling nv_op_is_valid_entry

===========================================================================*/
uint32 
nvim_op_get_size (
  nv_items_enum_type   item_code   /* Item ID */
);


/*===========================================================================

FUNCTION NVIM_REMOVE_ITEM

DESCRIPTION
  Removes an NV fixed item from NVDB.

RETURN VALUE
  NV_DONE_S if the write operation is successed.
  NV_FAIL_S if the write operation is fail.

===========================================================================*/
nv_stat_enum_type 
nvim_remove_item (
  nv_cmd_ext_type  *cmd_ext_ptr         /* name of the item */
);


/*===========================================================================

FUNCTION NVIM_REMOVE_ALL

DESCRIPTION
  This removes all files under the NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  Status of read operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvim_remove_all ( void );


/*===========================================================================

FUNCTION NV_FRONT_OP

DESCRIPTION
  This is the function called by both nv_cmd and nv_task. This is required
  in order to use kxmutexes for both synchronous and asynchronous accesses via
  NV.

DEPENDENCIES
  The NV task must have been started up already.  All of the items in
  the nv_cmd_block must be already set.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The nv_cmd_type is placed on the NV command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/

void nv_front_op(
  nv_cmd_ext_type  *cmd_ext_ptr,    /* Pointer to NV command buffer */
  uint32           cmd_req_from    /* cmd requested from local/remote proc */

  );


/*===========================================================================

FUNCTION NV_OTASP_COMMIT

DESCRIPTION
  This function performs the OTASP "commit" operation, which updates
  a number of OTASP-related nv items at once.  

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  Others       Failure for internal call

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nv_otasp_commit (
  nv_cmd_ext_type  *cmd_ext_ptr,    /* Command block */
  uint32           cmd_req_from    /* cmd requested from local/remote proc */

);


/*===========================================================================

FUNCTION NVIM_INIT_PRL_DATA

DESCRIPTION
  This function initializes the "prl_valid_data" and "prl_version_data"
  state data items.  These are maintained as NV state data so the functions
  "nvim_prl_is_valid()" and "nvim_prl_version()" can return an immediate 
  response.  

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void  
nvim_init_prl_data (void);

/*==================================================================
Function: NV_ACCESS_OP_CS_INIT

Description: This functions initialises kxmutex.
==================================================================*/

void 
nv_access_op_cs_init(void);


/*===========================================================================

FUNCTION NVIM_CLEAR_CRCS

DESCRIPTION
  This function is called to clear CRCs in NV when the NV is built. The function 
  clears the NAM checksums, the MIN checksums within each NAM, and, if 
  the ESN and IMEI have not already been written (assigned), the ESN, the IMEI,
  the ESN checksum, the IMEI checksum.  This action lets higher level tasks 
  know that these parameters are not valid and that service programming is 
  required.

DEPENDENCIES
  NVM must have been built.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  NV parameters identified above are zeroed.

===========================================================================*/

LOCAL nv_stat_enum_type  
nvim_clear_crcs (void);


/*===========================================================================

FUNCTION NVIM_INIT_RENTAL_TIMER

DESCRIPTION
  This function reads the rental timer item structure out of NVRAM and
  initializes the "nv_rental_item_image" state data item.  It also
  validates the data structure and recovers from a previous bad powerdown.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type  
nvim_init_rental_timer (void);


/*===========================================================================

FUNCTION NVIM_INIT_RENTAL_CNT

DESCRIPTION
  This function reads the rental count item from NV and initializes 
  the "nv_rental_cnt_image" state data item.  
  
DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type  
nvim_init_rental_cnt (void);


/*===========================================================================

FUNCTION NVIM_MIGRATION_NEEDED

DESCRIPTION
  This function will decide if there is a need to unlink the nvm directory 
  during a transition of older NV services to NV-2

DEPENDENCIES
  None.

RETURN VALUE
  0 if migration is not required
  1 if migratuion is required
  
SIDE EFFECTS
  None.

===========================================================================*/

LOCAL int 
nvim_migration_needed(void);



/*====================================================================
Function: NV_REGISTER_REMOTE_ACCESS_CHECK

Description: Registers callback function nv_check_remote_access()
             with directory listed in nv_access_dirs. On any access to
             nv_access_dirs directories, nv_check_remote_access will 
             be called to determine access permissions.
=====================================================================*/
void
nv_register_remote_access_check (void);


/*===========================================================================

FUNCTION SP_NVIM_READ_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's a Service
  Programming item, which should not be read until the SPC has been
  entered, or security unlocked.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is a Service Programming item,
  FALSE if not

SIDE EFFECTS
  None.

============================================================================*/
boolean sp_nvim_read_item (
  nv_items_enum_type item
);

/*===========================================================================

FUNCTION SP_NVIM_WRITE_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's a Service
  Programming item, which should not be written until the SPC
  has been entered, or security unlocked.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is a Service Programming item,
  FALSE if not

SIDE EFFECTS
  None.

============================================================================*/
boolean sp_nvim_write_item (
  nv_items_enum_type item
);

#ifdef __cplusplus
}
#endif

#endif  /* ifndef NVIM_H */

