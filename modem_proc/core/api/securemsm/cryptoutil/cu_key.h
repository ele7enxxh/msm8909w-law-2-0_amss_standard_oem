#ifndef CU_KEY_H
#define CU_KEY_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
 
                           Secure File Sytem

                  I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This header file is public header file providing the unique key service.

EXTERNALIZED FUNCTIONS
  cu_selected_key_type cu_get_key_type(void)
  secerrno_enum_type cu_get_key(uint8* ptr_key)
  void cu_set_key_type(cu_selected_key_type key_type)

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCVPath: $
  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/cryptoutil/cu_key.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/10   dm      Crypto API files rearchitecture
09/13/10   qxu     init version. moved the key feature here from crypto.

===========================================================================*/


#include "comdef.h"
#include "secerrno.h"
#include "UxCommon.h"         //UxMalloc
#include "cu_oem_key.h"

#define CU_KEY_SIZE 16

/** This enum identifies the key type to be used with System
 *  SFS */
typedef enum
{
  E_CU_UNKNOWN_KEY   = 0,   /**< -- unknown key type */
  E_CU_MSM_KEY       = 1,  /**< --  Use MSM Device Key. Only useful (i.e device unique key) if Secure Boot to be enabled */
  E_CU_OEM_KEY       = 2,  /**< --  Use OEM Specified key. Protecting the key is OEM's responsibility */
  E_CU_DUMMY_KEY     = 3,  /**< --  Use dummy key. */
  E_CU_PART_KEY      = 4   /**< --  Use a key from secure partition. Secure partition is a flash partition that is write protected. */
}cu_selected_key_type;

/**
* get the key type, MSM key or OEM key or any others.

  @return 
  cu_selected_key_type type.

  @dependencies 
  None.
*/
cu_selected_key_type cu_get_key_type(void);

/**
* get the unique key.

  @return 
   secerrno_enum_type type: success or failure.

  @dependencies 
  None.
*/
secerrno_enum_type cu_get_key(uint8* ptr_key);

/**
* set the unique key type. OEM can change the key type, if they
* like to use OEM_DEV_KEY for system SFS.It is recommended to
* use OEM_DEV_KEY, if Secure Boot is not enabled on the target.

  @return 
   secerrno_enum_type type: success or failure.

  @dependencies 
  None.
*/
void cu_set_key_type(cu_selected_key_type key_type);

/**
* the function called by RPC to get the SFS key for apps
* processor.

  @return 
   secerrno_enum_type type: success or failure.

  @dependencies 
  None.
*/
secerrno_enum_type cu_get_sfs_key
(
  uint8*  key_ptr,
  uint32  key_len
);

#endif // CU_KEY_H

