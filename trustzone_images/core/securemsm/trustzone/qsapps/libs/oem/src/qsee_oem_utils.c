/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      S E C U R I T Y    S E R V I C E S
 
                           SEC OEM Utilities

GENERAL DESCRIPTION
  Contains OEM utility for selecting SFS options that can be configured
  according to OEM requirements

Copyright (c) 2012 - 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/libs/oem/src/qsee_oem_utils.c#1 $ 
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/12   yh      created
*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "qsee_oem_utils.h"
#include "qsee_cipher.h"

#define QSEE_SFS_DEF_SEG_SIZE   1024 // 1K 

/*===========================================================================
*
* FUNCTION: qsee_oem_set_kdf_derive_key ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
void qsee_oem_set_kdf_derive_key(void **key, size_t *key_len)
{
  /* OEM: assign OEM key address to key pointer */
//  static char oem_key[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
//  *key = oem_key;
    *key_len = QSEE_AES128_KEY_SIZE;
  
    *key = NULL; 
}

/*===========================================================================
*
* FUNCTION: qsee_sfs_set_segment_size ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
uint32 qsee_sfs_set_segment_size(void)
{
  /* OEM can change the segment size,
     1 must be >0 && < 0x1000000
     2 must be multiple of QSEE_AES256_IV_SIZE */
  return QSEE_SFS_DEF_SEG_SIZE;
}

/*===========================================================================
*
* FUNCTION: qsee_sfs_set_safe_mode ()
*
* DESCRIPTION:
*
*   See header file
*
===========================================================================*/
boolean qsee_sfs_set_safe_mode(void)
{
  return TRUE;
}

/*===========================================================================
*
* FUNCTION: qsee_sfs_set_root_path ()
*
* DESCRIPTION:
* sfs_root: absolute path to HLOS root folder, start with '/', end with '/'
*           strlen of sfs_root < SFS_MAX_FILENAME_SIZE
*
*   See header file
*
===========================================================================*/
const char sfs_root[]="/persist/data/";
const char * qsee_sfs_set_root_path(void)
{
  return sfs_root;
//  return NULL;
}

