#ifndef CU_TARGET_H
#define CU_TARGET_H

/** @file cu_target.h
 * @brief
 * This file contains the target specific definitions of the constants, data 
 * structures and and interfaces.
 * 
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/cryptoutil/chipset/inc/cu_target.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/10   qxu      created
Copyright (c) 2008-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
FUNCTION CU_GET_MODEM_KEY

DESCRIPTION     
  generate unqiue key for user application.

PARAMETERS
  key_ptr - Buffer to be filled in with the key data.
  key_len - Length of buffer passed in

DEPENDENCIES    

RETURN VALUE
  E_SUCCESS if success, another error code on failure

SIDE EFFECTS  
  None
===========================================================================*/
secerrno_enum_type cu_get_unique_key
(
  uint8*  key_ptr,
  uint32  key_len
);

#endif /* CU_TARGET_H */
